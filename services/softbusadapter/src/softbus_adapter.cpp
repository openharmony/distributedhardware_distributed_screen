/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
static int32_t ScreenOnSoftbusSessionOpened(int32_t sessionId, int32_t result)
{
    return SoftbusAdapter::GetInstance().OnSoftbusSessionOpened(sessionId, result);
}

static void ScreenOnSoftbusSessionClosed(int32_t sessionId)
{
    SoftbusAdapter::GetInstance().OnSoftbusSessionClosed(sessionId);
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

static void ScreenOnQosEvent(int sessionId, int eventId, int tvCount, const QosTv *tvList)
{
    SoftbusAdapter::GetInstance().OnQosEvent(sessionId, eventId, tvCount, tvList);
}

SoftbusAdapter::SoftbusAdapter()
{
    DHLOGI("SoftbusAdapter");
    sessListener_.OnSessionOpened = ScreenOnSoftbusSessionOpened;
    sessListener_.OnSessionClosed = ScreenOnSoftbusSessionClosed;
    sessListener_.OnBytesReceived = ScreenOnBytesReceived;
    sessListener_.OnStreamReceived = ScreenOnStreamReceived;
    sessListener_.OnMessageReceived = ScreenOnMessageReceived;
    sessListener_.OnQosEvent = ScreenOnQosEvent;
}

SoftbusAdapter::~SoftbusAdapter()
{
    DHLOGI("~SoftbusAdapter");
}

int32_t SoftbusAdapter::RegisterSoftbusListener(const std::shared_ptr<ISoftbusListener> &listener,
    const std::string &sessionName, const std::string &peerDevId)
{
    if (listener == nullptr) {
        DHLOGE("%s: listener is nullptr.", LOG_TAG);
        return ERR_DH_SCREEN_ADAPTER_REGISTER_SOFTBUS_LISTENER_FAIL;
    }
    DHLOGI("%s: RegisterListener sess:%s id:%s.", LOG_TAG, sessionName.c_str(), GetAnonyString(peerDevId).c_str());
    std::string strListenerKey = sessionName + "_" + peerDevId;

    std::lock_guard<std::mutex> lisLock(listenerMtx_);
    if (mapListeners_.find(strListenerKey) != mapListeners_.end()) {
        DHLOGE("%s: Session listener already register.", LOG_TAG);
        return ERR_DH_SCREEN_ADAPTER_REGISTER_SOFTBUS_LISTENER_FAIL;
    }
    mapListeners_.insert(std::make_pair(strListenerKey, listener));

    return DH_SUCCESS;
}

int32_t SoftbusAdapter::UnRegisterSoftbusListener(const std::string &sessionName, const std::string &peerDevId)
{
    DHLOGI("%s: UnRegisterListener sess:%s id:%s.", LOG_TAG, sessionName.c_str(), GetAnonyString(peerDevId).c_str());
    std::string strListenerKey = sessionName + "_" + peerDevId;

    std::lock_guard<std::mutex> lisLock(listenerMtx_);
    mapListeners_.erase(strListenerKey);

    return DH_SUCCESS;
}

int32_t SoftbusAdapter::CreateSoftbusSessionServer(const std::string &pkgname, const std::string &sessionName,
    const std::string &peerDevId)
{
    DHLOGI("%s: CreateSessionServer sess:%s id:%s.", LOG_TAG, sessionName.c_str(), GetAnonyString(peerDevId).c_str());
    std::lock_guard<std::mutex> setLock(sessSetMtx_);
    if (mapSessionSet_.find(sessionName) == mapSessionSet_.end()) {
        int32_t ret = CreateSessionServer(pkgname.c_str(), sessionName.c_str(), &sessListener_);
        if (ret != DH_SUCCESS) {
            DHLOGE("%s: CreateSessionServer failed.", LOG_TAG);
            return ret;
        }
    } else {
        DHLOGD("%s: Session already create.", sessionName.c_str());
    }

    mapSessionSet_[sessionName].insert(peerDevId);
    DHLOGI("%s: CreateSessionServer success.", LOG_TAG);
    return DH_SUCCESS;
}

int32_t SoftbusAdapter::RemoveSoftbusSessionServer(const std::string &pkgname, const std::string &sessionName,
    const std::string &peerDevId)
{
    DHLOGI("%s: RemoveSessionServer sess:%s id:%s", LOG_TAG, sessionName.c_str(), GetAnonyString(peerDevId).c_str());
    std::lock_guard<std::mutex> setLock(sessSetMtx_);
    if (mapSessionSet_.find(sessionName) == mapSessionSet_.end()) {
        DHLOGE("%s: Session name can not find.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ILLEGAL_OPERATION;
    } else if (mapSessionSet_[sessionName].find(peerDevId) == mapSessionSet_[sessionName].end()) {
        DHLOGE("%s: PeerDevId can not find.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ILLEGAL_OPERATION;
    }

    int32_t ret = RemoveSessionServer(pkgname.c_str(), sessionName.c_str());
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: RemoveSessionServer failed.", LOG_TAG);
        return ret;
    }

    mapSessionSet_[sessionName].erase(peerDevId);
    if (mapSessionSet_[sessionName].empty()) {
        mapSessionSet_.erase(sessionName);
    }
    DHLOGI("%s: RemoveSessionServer success.", LOG_TAG);
    return DH_SUCCESS;
}

int32_t SoftbusAdapter::OpenSoftbusSession(const std::string &mySessionName, const std::string &peerSessionName,
    const std::string &peerDevId)
{
    DHLOGI("%s: OpenSoftbusSession mysess:%s peersess:%s id:%s.", LOG_TAG, mySessionName.c_str(),
        peerSessionName.c_str(), GetAnonyString(peerDevId).c_str());
    int dataType = TYPE_BYTES;
    int streamType = -1;
    if (mySessionName == DATA_SESSION_NAME) {
        dataType = TYPE_STREAM;
        streamType = RAW_STREAM;
    }

    SessionAttribute attr = { 0 };
    attr.dataType = dataType;
    attr.linkTypeNum = LINK_TYPE_MAX;
    LinkType linkTypeList[LINK_TYPE_MAX] = {
        LINK_TYPE_WIFI_P2P,
        LINK_TYPE_WIFI_WLAN_5G,
        LINK_TYPE_WIFI_WLAN_2G,
        LINK_TYPE_BR,
    };
    int32_t ret = memcpy_s(attr.linkType, sizeof(attr.linkType), linkTypeList, sizeof(linkTypeList));
    if (ret != EOK) {
        DHLOGE("%s: Data copy failed.", LOG_TAG);
        return ERR_DH_SCREEN_ADAPTER_PARA_ERROR;
    }
    attr.attr.streamAttr.streamType = streamType;
    int32_t sessionId = OpenSession(mySessionName.c_str(), peerSessionName.c_str(), peerDevId.c_str(), "0", &attr);
    if (sessionId < 0) {
        DHLOGE("%s: OpenSession failed sessionId:%d.", LOG_TAG, sessionId);
        return ERR_DH_SCREEN_ADAPTER_OPEN_SESSION_FAIL;
    }

    DHLOGI("%s: OpenSoftbusSession success sessionId:%d.", LOG_TAG, sessionId);
    return sessionId;
}

int32_t SoftbusAdapter::CloseSoftbusSession(const int32_t sessionId)
{
    DHLOGI("%s: CloseSoftbusSession, sessid:%d.", LOG_TAG, sessionId);
    CloseSession(sessionId);

    std::lock_guard<std::mutex> lisLock(listenerMtx_);
    mapSessListeners_.erase(sessionId);

    DHLOGI("%s: CloseSoftbusSession success.", LOG_TAG);
    return DH_SUCCESS;
}

int32_t SoftbusAdapter::SendSoftbusBytes(int32_t sessionId, const void *data, int32_t dataLen)
{
    DHLOGD("%s: SendSoftbusBytes, sessid:%d.", LOG_TAG, sessionId);
    int32_t ret = SendBytes(sessionId, data, dataLen);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: SendBytes failed ret:%d.", LOG_TAG, ret);
        return ERR_DH_SCREEN_TRANS_ERROR;
    }

    return DH_SUCCESS;
}

int32_t SoftbusAdapter::SendSoftbusStream(int32_t sessionId, const StreamData *data, const StreamData *ext,
    const StreamFrameInfo *param)
{
    DHLOGD("%s: SendSoftbusStream, sessid:%d.", LOG_TAG, sessionId);
    int32_t ret = SendStream(sessionId, data, ext, param);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: SendStream failed ret:%d.", LOG_TAG, ret);
        return ERR_DH_SCREEN_TRANS_ERROR;
    }

    return DH_SUCCESS;
}

std::shared_ptr<ISoftbusListener> &SoftbusAdapter::GetSoftbusListenerByName(int32_t sessionId)
{
    char sessionName[DSCREEN_MAX_SESSION_NAME_LEN] = "";
    char peerDevId[DSCREEN_MAX_DEVICE_ID_LEN] = "";
    int32_t ret = GetPeerSessionName(sessionId, sessionName, sizeof(sessionName));
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: GetPeerSessionName failed ret:%d.", LOG_TAG, ret);
        return nullListener_;
    }
    ret = GetPeerDeviceId(sessionId, peerDevId, sizeof(peerDevId));
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: GetPeerDeviceId failed ret:%d.", LOG_TAG, ret);
        return nullListener_;
    }

    std::string sessionNameStr(sessionName);
    std::string peerDevIdStr(peerDevId);
    DHLOGD("%s: GetSoftbusListenerByName sessionName:%s, peerDevId:%s.", LOG_TAG, sessionNameStr.c_str(),
        GetAnonyString(peerDevIdStr).c_str());
    std::string strListenerKey = sessionNameStr + "_" + peerDevIdStr;

    std::lock_guard<std::mutex> lisLock(listenerMtx_);
    if (mapListeners_.find(strListenerKey) == mapListeners_.end()) {
        DHLOGE("%s: Find listener failed.", LOG_TAG);
        return nullListener_;
    }
    return mapListeners_[strListenerKey];
}

std::shared_ptr<ISoftbusListener> &SoftbusAdapter::GetSoftbusListenerById(int32_t sessionId)
{
    std::lock_guard<std::mutex> lisLock(listenerMtx_);
    if (mapSessListeners_.find(sessionId) == mapSessListeners_.end()) {
        DHLOGE("%s: Find listener failed.", LOG_TAG);
        return nullListener_;
    }

    return mapSessListeners_[sessionId];
}

int32_t SoftbusAdapter::OnSoftbusSessionOpened(int32_t sessionId, int32_t result)
{
    DHLOGI("%s: OnSessionOpened session:%d, result:%d.", LOG_TAG, sessionId, result);
    if (result != DH_SUCCESS) {
        DHLOGE("%s: OnSessionOpened failed.", LOG_TAG);
        return ERR_DH_SCREEN_ADAPTER_OPEN_SESSION_FAIL;
    }

    std::shared_ptr<ISoftbusListener> &listener = GetSoftbusListenerByName(sessionId);
    if (listener == nullptr) {
        DHLOGE("Get softbus listener failed.");
        return ERR_DH_SCREEN_TRANS_ERROR;
    }
    listener->OnSessionOpened(sessionId, result);

    std::lock_guard<std::mutex> lisLock(listenerMtx_);
    mapSessListeners_.insert(std::make_pair(sessionId, listener));

    return DH_SUCCESS;
}

void SoftbusAdapter::OnSoftbusSessionClosed(int32_t sessionId)
{
    DHLOGI("%s: OnSessionClosed sessionId:%d.", LOG_TAG, sessionId);
    std::shared_ptr<ISoftbusListener> &listener = GetSoftbusListenerById(sessionId);
    if (listener == nullptr) {
        DHLOGE("Get softbus listener failed.");
        return;
    }
    listener->OnSessionClosed(sessionId);

    std::lock_guard<std::mutex> lisLock(listenerMtx_);
    mapSessListeners_.erase(sessionId);
}

void SoftbusAdapter::OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    DHLOGD("%s: OnBytesReceived, sessionId:%d.", LOG_TAG, sessionId);
    if (data == nullptr) {
        DHLOGE("BytesData is null.");
        return;
    }
    if (dataLen == 0 || dataLen > DSCREEN_MAX_RECV_DATA_LEN) {
        DHLOGE("BytesData length is too large, dataLen:%u.", dataLen);
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
    DHLOGD("%s OnStreamReceived, sessionId:%d.", LOG_TAG, sessionId);
    if (data == nullptr) {
        DHLOGE("StreamData is null.");
        return;
    }
    if (data->bufLen <= 0 || data->bufLen > (int32_t)DSCREEN_MAX_RECV_DATA_LEN) {
        DHLOGE("StreamData length is too large, dataLen:%d.", data->bufLen);
        return;
    }

    std::shared_ptr<ISoftbusListener> &listener = GetSoftbusListenerByName(sessionId);
    if (listener == nullptr) {
        DHLOGE("Get softbus listener failed.");
        return;
    }
    listener->OnStreamReceived(sessionId, data, ext, frameInfo);
}

void SoftbusAdapter::OnMessageReceived(int sessionId, const void *data, unsigned int dataLen)
{
    DHLOGD("%s OnMessageReceived, sessionId:%d.", LOG_TAG, sessionId);
}

void SoftbusAdapter::OnQosEvent(int sessionId, int eventId, int tvCount, const QosTv *tvList)
{
    DHLOGD("%s OnQosEvent, sessionId:%d.", LOG_TAG, sessionId);
}
} // namespace DistributedHardware
} // namespace OHOS