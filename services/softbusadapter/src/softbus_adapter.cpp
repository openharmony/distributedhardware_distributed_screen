/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "softbus_adapter.h"

#include <securec.h>
#include <unistd.h>

#include "softbus_bus_center.h"
#include "softbus_common.h"

#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_util.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(SoftbusAdapter);
static void ScreenOnSoftbusSessionOpened(int32_t sessionId, PeerSocketInfo info)
{
    SoftbusAdapter::GetInstance().OnSoftbusSessionOpened(sessionId, info);
}

static void ScreenOnSoftbusSessionClosed(int32_t sessionId, ShutdownReason reason)
{
    SoftbusAdapter::GetInstance().OnSoftbusSessionClosed(sessionId, reason);
}

static void ScreenOnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    SoftbusAdapter::GetInstance().OnBytesReceived(sessionId, data, dataLen);
}

static void ScreenOnStreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
    const StreamFrameInfo *frameInfo)
{
    SoftbusAdapter::GetInstance().OnStreamReceived(sessionId, data, ext, frameInfo);
}

static void ScreenOnMessageReceived(int sessionId, const void *data, unsigned int dataLen)
{
    SoftbusAdapter::GetInstance().OnMessageReceived(sessionId, data, dataLen);
}

SoftbusAdapter::SoftbusAdapter()
{
    DHLOGI("SoftbusAdapter");
    sessListener_.OnBind = ScreenOnSoftbusSessionOpened;
    sessListener_.OnShutdown = ScreenOnSoftbusSessionClosed;
    sessListener_.OnBytes = ScreenOnBytesReceived;
    sessListener_.OnStream = ScreenOnStreamReceived;
    sessListener_.OnMessage = ScreenOnMessageReceived;
}

SoftbusAdapter::~SoftbusAdapter()
{
    DHLOGI("~SoftbusAdapter");
}

int32_t SoftbusAdapter::RegisterSoftbusListener(const std::shared_ptr<ISoftbusListener> &listener,
    const std::string &sessionName, const std::string &peerDevId)
{
    if (listener == nullptr) {
        DHLOGE("%{public}s: listener is nullptr.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_ADAPTER_REGISTER_SOFTBUS_LISTENER_FAIL;
    }
    DHLOGI("%{public}s: RegisterListener sess:%{public}s id:%{public}s.", DSCREEN_LOG_TAG, sessionName.c_str(),
        GetAnonyString(peerDevId).c_str());
    std::string strListenerKey = sessionName + "_" + peerDevId;
    std::lock_guard<std::mutex> lisLock(listenerMtx_);
    if (mapListeners_.find(strListenerKey) != mapListeners_.end()) {
        DHLOGE("%{public}s: Session listener already register.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_ADAPTER_REGISTER_SOFTBUS_LISTENER_FAIL;
    }
    mapListeners_.insert(std::make_pair(strListenerKey, listener));

    return DH_SUCCESS;
}

int32_t SoftbusAdapter::UnRegisterSoftbusListener(const std::string &sessionName, const std::string &peerDevId)
{
    DHLOGI("%{public}s: UnRegisterListener sess:%{public}s id:%{public}s.", DSCREEN_LOG_TAG, sessionName.c_str(),
        GetAnonyString(peerDevId).c_str());
    std::string strListenerKey = sessionName + "_" + peerDevId;

    std::lock_guard<std::mutex> lisLock(listenerMtx_);
    mapListeners_.erase(strListenerKey);

    return DH_SUCCESS;
}

int32_t SoftbusAdapter::CreateSoftbusSessionServer(const std::string &pkgname, const std::string &sessionName,
    const std::string &peerDevId)
{
    DHLOGI("%{public}s: CreateSessionServer sess:%{public}s id:%{public}s.", DSCREEN_LOG_TAG, sessionName.c_str(),
        GetAnonyString(peerDevId).c_str());
    {
        std::lock_guard<std::mutex> lock(serverIdMapMutex_);
        std::string idMapValue = sessionName + "_" + peerDevId;
        for (auto it = serverIdMap_.begin(); it != serverIdMap_.end(); it++) {
            if (((it->second).find(idMapValue) != std::string::npos)) {
                DHLOGI("%{public}s: Session already create.", sessionName.c_str());
                return DH_SUCCESS;
            }
        }
    }

    SocketInfo serverInfo = {
        .name = const_cast<char*>(sessionName.c_str()),
        .pkgName = const_cast<char*>(pkgname.c_str()),
        .dataType = DATA_TYPE_VIDEO_STREAM,
    };
    int32_t socketId = Socket(serverInfo);
    if (socketId < 0) {
        DHLOGE("Create Socket fail socketId:%{public}" PRId32, socketId);
        return ERR_DH_SCREEN_ADAPTER_BAD_VALUE;
    }
    QosTV qos[] = {
        {.qos = QOS_TYPE_MIN_BW,        .value = 40 * 1024 * 1024},
        {.qos = QOS_TYPE_MAX_LATENCY,        .value = 8000},
        {.qos = QOS_TYPE_MIN_LATENCY,        .value = 2000},
    };

    int32_t ret = Listen(socketId, qos, sizeof(qos) / sizeof(qos[0]), &sessListener_);
    if (ret != DH_SUCCESS) {
        DHLOGE("Listen socket error for sessionName:%{public}s", sessionName.c_str());
        return ERR_DH_SCREEN_ADAPTER_BAD_VALUE;
    }
    {
        std::lock_guard<std::mutex> lock(serverIdMapMutex_);
        serverIdMap_.insert(std::make_pair(socketId, sessionName + "_" + peerDevId));
    }
    DHLOGI("%{public}s: CreateSessionServer success sessionId. %{public}" PRId32, DSCREEN_LOG_TAG, socketId);
    return DH_SUCCESS;
}

int32_t SoftbusAdapter::RemoveSoftbusSessionServer(const std::string &pkgname, const std::string &sessionName,
    const std::string &peerDevId)
{
    (void)pkgname;
    if (sessionName.empty() || peerDevId.empty()) {
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    DHLOGI("%{public}s: RemoveSessionServer sess:%{public}s id:%{public}s", DSCREEN_LOG_TAG, sessionName.c_str(),
        GetAnonyString(peerDevId).c_str());
    int32_t serverSocketId = INVALID_SESSION_ID;
    {
        std::lock_guard<std::mutex> lock(serverIdMapMutex_);
        std::string idMapValue = sessionName + "_" + peerDevId;
        for (auto it = serverIdMap_.begin(); it != serverIdMap_.end();) {
            if (((it->second).find(idMapValue) != std::string::npos)) {
                serverSocketId = it->first;
                it = serverIdMap_.erase(it);
            } else {
                ++it;
            }
        }
    }
    Shutdown(serverSocketId);
    DHLOGI("%{public}s: RemoveSessionServer success.", DSCREEN_LOG_TAG);
    return DH_SUCCESS;
}

int32_t SoftbusAdapter::OpenSoftbusSession(const std::string &mySessionName, const std::string &peerSessionName,
    const std::string &peerDevId)
{
    DHLOGI("%{public}s: OpenSoftbusSession mysess:%{public}s peersess:%{public}s id:%{public}s.", DSCREEN_LOG_TAG,
        mySessionName.c_str(), peerSessionName.c_str(), GetAnonyString(peerDevId).c_str());

    QosTV qos[] = {
        {.qos = QOS_TYPE_MIN_BW,        .value = 40 * 1024 * 1024},
        {.qos = QOS_TYPE_MAX_LATENCY,        .value = 8000},
        {.qos = QOS_TYPE_MIN_LATENCY,        .value = 2000},
    };
    std::string localSesionName = mySessionName + "_" + std::to_string(GetCurrentTimeUs());
    SocketInfo clientInfo = {
        .name = const_cast<char*>((localSesionName.c_str())),
        .peerName = const_cast<char*>(peerSessionName.c_str()),
        .peerNetworkId = const_cast<char*>(peerDevId.c_str()),
        .pkgName = const_cast<char*>(PKG_NAME.c_str()),
        .dataType = DATA_TYPE_VIDEO_STREAM,
    };
    int32_t socketId = Socket(clientInfo);
    if (socketId < 0) {
        DHLOGE("Create OpenSoftbusChannel Socket error");
        return ERR_DH_SCREEN_ADAPTER_PARA_ERROR;
    }
    int32_t ret = Bind(socketId, qos, sizeof(qos) / sizeof(qos[0]), &sessListener_);
    if (ret != DH_SUCCESS) {
        DHLOGE("Bind SocketClient error");
        return ERR_DH_SCREEN_ADAPTER_PARA_ERROR;
    }
    {
        std::lock_guard<std::mutex> lock(idMapMutex_);
        devId2SessIdMap_.insert(std::make_pair(socketId, mySessionName + "_" + peerDevId));
    }
    std::shared_ptr<ISoftbusListener> &listener = GetSoftbusListenerByName(socketId);
    if (listener == nullptr) {
        DHLOGE("Get softbus listener failed.");
        return ERR_DH_SCREEN_TRANS_ERROR;
    }
    PeerSocketInfo info;
    ret = OnSoftbusSessionOpened(socketId, info);
    if (ret != DH_SUCCESS) {
        return ret;
    }
    DHLOGI("%{public}s: OpenSoftbusSession success sessionId: %{public}" PRId32, DSCREEN_LOG_TAG, socketId);
    return socketId;
}

int32_t SoftbusAdapter::CloseSoftbusSession(const int32_t sessionId)
{
    DHLOGI("%{public}s: CloseSoftbusSession, sessid:%{public}" PRId32, DSCREEN_LOG_TAG, sessionId);
    Shutdown(sessionId);
    {
        std::lock_guard<std::mutex> lock(idMapMutex_);
        devId2SessIdMap_.erase(sessionId);
    }
    std::lock_guard<std::mutex> lisLock(listenerMtx_);
    mapSessListeners_.erase(sessionId);

    DHLOGI("%{public}s: CloseSoftbusSession success.", DSCREEN_LOG_TAG);
    return DH_SUCCESS;
}

int32_t SoftbusAdapter::SendSoftbusBytes(int32_t sessionId, const void *data, int32_t dataLen) const
{
    DHLOGD("%{public}s: SendSoftbusBytes, sessid:%{public}" PRId32, DSCREEN_LOG_TAG, sessionId);
    int32_t ret = SendBytes(sessionId, data, dataLen);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: SendBytes failed ret:%{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ERR_DH_SCREEN_TRANS_ERROR;
    }

    return DH_SUCCESS;
}

int32_t SoftbusAdapter::SendSoftbusStream(int32_t sessionId, const StreamData *data, const StreamData *ext,
    const StreamFrameInfo *param) const
{
    DHLOGD("%{public}s: SendSoftbusStream, sessid:%{public}" PRId32, DSCREEN_LOG_TAG, sessionId);
    int32_t ret = SendStream(sessionId, data, ext, param);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: SendStream failed ret:%{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ERR_DH_SCREEN_TRANS_ERROR;
    }

    return DH_SUCCESS;
}

std::shared_ptr<ISoftbusListener> &SoftbusAdapter::GetSoftbusListenerByName(int32_t sessionId)
{
    DHLOGD("%{public}s: GetSoftbusListenerByName, sessionId:%{public}" PRId32, DSCREEN_LOG_TAG, sessionId);
    std::string strListenerKey = "";
    {
        std::lock_guard<std::mutex> lock(idMapMutex_);
        for (auto it = devId2SessIdMap_.begin(); it != devId2SessIdMap_.end(); it++) {
            if (it->first == sessionId) {
                strListenerKey = it->second;
                break;
            }
        }
    }
    std::lock_guard<std::mutex> lisLock(listenerMtx_);
    if (mapListeners_.find(strListenerKey) == mapListeners_.end()) {
        DHLOGE("%{public}s: Find listener failed.", DSCREEN_LOG_TAG);
        return nullListener_;
    }
    return mapListeners_[strListenerKey];
}

std::shared_ptr<ISoftbusListener> &SoftbusAdapter::GetSoftbusListenerById(int32_t sessionId)
{
    std::lock_guard<std::mutex> lisLock(listenerMtx_);
    if (mapSessListeners_.find(sessionId) == mapSessListeners_.end()) {
        DHLOGE("%{public}s: Find listener failed.", DSCREEN_LOG_TAG);
        return nullListener_;
    }

    return mapSessListeners_[sessionId];
}

int32_t SoftbusAdapter::OnSoftbusSessionOpened(int32_t sessionId, PeerSocketInfo info)
{
    DHLOGI("%{public}s: OnSoftbusSessionOpened session:%{public}" PRId32, DSCREEN_LOG_TAG, sessionId);
    {
        std::lock_guard<std::mutex> lock(serverIdMapMutex_);
        for (auto it = serverIdMap_.begin(); it != serverIdMap_.end(); it++) {
            if (info.networkId == nullptr) {
                break;
            }
            std::string peerDevId(info.networkId);
            if ((it->second).find(peerDevId) != std::string::npos) {
                std::lock_guard<std::mutex> sessionLock(idMapMutex_);
                devId2SessIdMap_.insert(std::make_pair(sessionId, it->second));
                break;
            }
        }
    }

    std::shared_ptr<ISoftbusListener> &listener = GetSoftbusListenerByName(sessionId);
    if (listener == nullptr) {
        DHLOGE("Get softbus listener failed.");
        return ERR_DH_SCREEN_TRANS_ERROR;
    }
    listener->OnSessionOpened(sessionId, info);

    std::lock_guard<std::mutex> lisLock(listenerMtx_);
    mapSessListeners_.insert(std::make_pair(sessionId, listener));

    return DH_SUCCESS;
}

void SoftbusAdapter::OnSoftbusSessionClosed(int32_t sessionId, ShutdownReason reason)
{
    DHLOGI("%{public}s: OnSessionClosed sessionId:%{public}" PRId32, DSCREEN_LOG_TAG, sessionId);
    std::shared_ptr<ISoftbusListener> &listener = GetSoftbusListenerById(sessionId);
    if (listener == nullptr) {
        DHLOGE("Get softbus listener failed.");
        return;
    }
    listener->OnSessionClosed(sessionId, reason);
    {
        std::lock_guard<std::mutex> lock(idMapMutex_);
        devId2SessIdMap_.erase(sessionId);
    }
    std::lock_guard<std::mutex> lisLock(listenerMtx_);
    mapSessListeners_.erase(sessionId);
}

void SoftbusAdapter::OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    DHLOGD("%{public}s: OnBytesReceived, sessionId:%{public}" PRId32, DSCREEN_LOG_TAG, sessionId);
    if (data == nullptr) {
        DHLOGE("BytesData is null.");
        return;
    }
    if (dataLen == 0 || dataLen > DSCREEN_MAX_RECV_DATA_LEN) {
        DHLOGE("BytesData length is too large, dataLen:%{public}" PRIu32, dataLen);
        return;
    }

    std::shared_ptr<ISoftbusListener> &listener = GetSoftbusListenerByName(sessionId);
    if (listener == nullptr) {
        DHLOGE("Get softbus listener failed.");
        return;
    }
    listener->OnBytesReceived(sessionId, data, dataLen);
}

void SoftbusAdapter::OnStreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
    const StreamFrameInfo *frameInfo)
{
    DHLOGD("%{public}s OnStreamReceived, sessionId:%{public}" PRId32, DSCREEN_LOG_TAG, sessionId);
    if (data == nullptr) {
        DHLOGE("StreamData is null.");
        return;
    }
    if (data->bufLen <= 0 || data->bufLen > static_cast<int32_t>(DSCREEN_MAX_RECV_DATA_LEN)) {
        DHLOGE("StreamData length is too large, dataLen:%{public}" PRId32, data->bufLen);
        return;
    }

    std::shared_ptr<ISoftbusListener> &listener = GetSoftbusListenerByName(sessionId);
    if (listener == nullptr) {
        DHLOGE("Get softbus listener failed.");
        return;
    }
    listener->OnStreamReceived(sessionId, data, ext, frameInfo);
}

void SoftbusAdapter::OnMessageReceived(int sessionId, const void *data, unsigned int dataLen) const
{
    (void)data;
    (void)dataLen;
    DHLOGD("%{public}s OnMessageReceived, sessionId:%{public}" PRId32, DSCREEN_LOG_TAG, sessionId);
}
} // namespace DistributedHardware
} // namespace OHOS