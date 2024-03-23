/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "screen_data_channel_impl.h"

#include <securec.h>

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_log.h"
#include "dscreen_util.h"
#include "dscreen_json_util.h"

namespace OHOS {
namespace DistributedHardware {
int32_t ScreenDataChannelImpl::CreateSession(const std::shared_ptr<IScreenChannelListener> &listener)
{
    DHLOGI("%{public}s: CreateSession, peerDevId(%{public}s)", DSCREEN_LOG_TAG, GetAnonyString(peerDevId_).c_str());
    if (listener == nullptr) {
        DHLOGE("%{public}s: Channel listener is null", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    int32_t ret =
        SoftbusAdapter::GetInstance().CreateSoftbusSessionServer(PKG_NAME, DATA_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Create data session failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }
    std::shared_ptr<ISoftbusListener> softbusListener = shared_from_this();
    ret = SoftbusAdapter::GetInstance().RegisterSoftbusListener(softbusListener, DATA_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Register data adapter listener failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }
    if (jpegSessionFlag_ == true) {
        ret =
            SoftbusAdapter::GetInstance().CreateSoftbusSessionServer(PKG_NAME, JPEG_SESSION_NAME, peerDevId_);
        if (ret != DH_SUCCESS) {
            DHLOGE("%{public}s: Create jpeg session failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
            return ret;
        }
        ret =
            SoftbusAdapter::GetInstance().RegisterSoftbusListener(softbusListener, JPEG_SESSION_NAME, peerDevId_);
        if (ret != DH_SUCCESS) {
            DHLOGE("%{public}s: Register jpeg adapter listener failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
            return ret;
        }
    }
    channelListener_ = listener;
    DHLOGI("%{public}s: Create session success", DSCREEN_LOG_TAG);
    return DH_SUCCESS;
}

void ScreenDataChannelImpl::SetJpegSessionFlag(bool flag)
{
    jpegSessionFlag_ = flag;
}

int32_t ScreenDataChannelImpl::ReleaseSession()
{
    DHLOGI("%{public}s: ReleaseSession, peerDevId(%{public}s)", DSCREEN_LOG_TAG, GetAnonyString(peerDevId_).c_str());
    int32_t ret = SoftbusAdapter::GetInstance().RemoveSoftbusSessionServer(PKG_NAME, DATA_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Release data session failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }
    ret = SoftbusAdapter::GetInstance().UnRegisterSoftbusListener(DATA_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: UnRegister data adapter listener failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }

    if (jpegSessionFlag_ == true) {
        ret = SoftbusAdapter::GetInstance().RemoveSoftbusSessionServer(PKG_NAME, JPEG_SESSION_NAME, peerDevId_);
        if (ret != DH_SUCCESS) {
            DHLOGE("%{public}s: Release jpeg session failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
            return ret;
        }
        ret = SoftbusAdapter::GetInstance().UnRegisterSoftbusListener(JPEG_SESSION_NAME, peerDevId_);
        if (ret != DH_SUCCESS) {
            DHLOGE("%{public}s: UnRegister jpeg adapter listener failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
            return ret;
        }
        jpegSessionFlag_ = false;
    }
    DHLOGI("%{public}s: Release session success", DSCREEN_LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenDataChannelImpl::OpenSession(const std::shared_ptr<IScreenChannelListener> &listener)
{
    DHLOGI("%{public}s: OpenSession, peerDevId(%{public}s)", DSCREEN_LOG_TAG, GetAnonyString(peerDevId_).c_str());
    if (listener == nullptr) {
        DHLOGE("%{public}s: Channel listener is null", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    channelListener_ = listener;
    std::shared_ptr<ISoftbusListener> softbusListener = shared_from_this();
    int32_t ret = SoftbusAdapter::GetInstance().RegisterSoftbusListener(softbusListener, DATA_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Register data adapter listener failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }
    int32_t sessionId =
        SoftbusAdapter::GetInstance().OpenSoftbusSession(DATA_SESSION_NAME, DATA_SESSION_NAME, peerDevId_);
    if (sessionId < 0) {
        DHLOGE("%{public}s: Open data session failed, ret: %{public}" PRId32, DSCREEN_LOG_TAG, sessionId);
        ReportOptFail(DSCREEN_OPT_FAIL, sessionId, "Open data session failed");
        return ERR_DH_SCREEN_TRANS_ERROR;
    }
    sessionId_ = sessionId;
    if (jpegSessionFlag_ == true) {
        ret =
            SoftbusAdapter::GetInstance().RegisterSoftbusListener(softbusListener, JPEG_SESSION_NAME, peerDevId_);
        if (ret != DH_SUCCESS) {
            DHLOGE("%{public}s: Register jpeg adapter listener failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
            return ret;
        }
        sessionId =
            SoftbusAdapter::GetInstance().OpenSoftbusSession(JPEG_SESSION_NAME, JPEG_SESSION_NAME, peerDevId_);
        if (sessionId < 0) {
            DHLOGE("%{public}s: Open jpeg session failed, ret: %{public}" PRId32, DSCREEN_LOG_TAG, sessionId);
            ReportOptFail(DSCREEN_OPT_FAIL, sessionId, "Open jpeg session failed");
            return ERR_DH_SCREEN_TRANS_ERROR;
        }
        jpegSessionId_ = sessionId;
    }
    DHLOGI("%{public}s: Open screen session success, sessionId %{public}" PRId32, DSCREEN_LOG_TAG, sessionId_);
    return DH_SUCCESS;
}

int32_t ScreenDataChannelImpl::CloseSession()
{
    DHLOGI("%{public}s: CloseSession, sessionId %{public}" PRId32, DSCREEN_LOG_TAG, sessionId_);
    if (sessionId_ == 0) {
        DHLOGD("%{public}s: Session is not opened.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_SESSION_NOT_OPEN;
    }
    int32_t ret = SoftbusAdapter::GetInstance().CloseSoftbusSession(sessionId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Close screen session failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }
    sessionId_ = 0;
    if (jpegSessionFlag_ == true && jpegSessionId_ != 0) {
        ret = SoftbusAdapter::GetInstance().CloseSoftbusSession(jpegSessionId_);
        if (ret != DH_SUCCESS) {
            DHLOGE("%{public}s: Close jpeg session failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
            return ret;
        }
    jpegSessionId_ = 0;
    }
    DHLOGI("%{public}s: Close session success", DSCREEN_LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenDataChannelImpl::SendData(const std::shared_ptr<DataBuffer> &screenData)
{
    DHLOGI("%{public}s: SendData.", DSCREEN_LOG_TAG);
    if (screenData == nullptr || screenData->Data() == nullptr) {
        DHLOGE("%{public}s: Screen data is null", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    uint8_t dataType = screenData->DataType();
    if (dataType == VIDEO_PART_SCREEN_DATA) {
        int32_t ret = SendDirtyData(screenData);
        if (ret != DH_SUCCESS) {
            DHLOGE("%{public}s: send dirty data failed, ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
            return ret;
        }
    } else if (dataType == VIDEO_FULL_SCREEN_DATA) {
        int32_t ret = SendFullData(screenData);
        if (ret != DH_SUCCESS) {
            DHLOGE("%{public}s: send full data failed, ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
            return ret;
        }
    }
    return DH_SUCCESS;
}

int32_t ScreenDataChannelImpl::SendFullData(const std::shared_ptr<DataBuffer> &screenData)
{
    DHLOGI("%{public}s: SendFullData sessionId: %{public}" PRId32, DSCREEN_LOG_TAG, sessionId_);
    if (screenData == nullptr) {
        DHLOGE("%{public}s: Screen data is null", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    StreamData data = {reinterpret_cast<char *>(screenData->Data()), screenData->Capacity()};
    StreamData ext = {0};
    StreamFrameInfo frameInfo = {0};
    int32_t ret = SoftbusAdapter::GetInstance().SendSoftbusStream(sessionId_, &data, &ext, &frameInfo);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Send full data failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }
    DHLOGI("%{public}s: SendFullData success.", DSCREEN_LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenDataChannelImpl::SendDirtyData(const std::shared_ptr<DataBuffer> &screenData)
{
    DHLOGI("%{public}s: SendDirtyData sessionId: %{public}" PRId32, DSCREEN_LOG_TAG, jpegSessionId_);
    if (screenData == nullptr) {
        DHLOGE("%{public}s: Screen data is null", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    nlohmann::json rectJson;
    DirtyVecToJson(rectJson, screenData);
    StreamData data = {reinterpret_cast<char *>(screenData->Data()), screenData->Capacity()};
    std::string rectInfo = rectJson.dump();
    char *dirtyInfo = new char[rectInfo.length() + 1] {0};
    int32_t ret = memcpy_s(dirtyInfo, rectInfo.length(), rectInfo.c_str(), rectInfo.length());
    if (ret != EOK) {
        DHLOGE("SendDirtyData memcpy_s failed.");
        delete [] dirtyInfo;
        return ret;
    }
    StreamData ext = {dirtyInfo, rectInfo.length() + 1};
    StreamFrameInfo frameInfo = {0};
    ret = SoftbusAdapter::GetInstance().SendSoftbusStream(jpegSessionId_, &data, &ext, &frameInfo);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Send dirty data failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        delete [] dirtyInfo;
        return ret;
    }
    delete [] dirtyInfo;
    DHLOGI("%{public}s: SendDirtyData success.", DSCREEN_LOG_TAG);
    return DH_SUCCESS;
}


void ScreenDataChannelImpl::OnSessionOpened(int32_t sessionId, PeerSocketInfo info)
{
    (void)info;
    DHLOGI("%{public}s: OnScreenSessionOpened, sessionId: %{public}" PRId32, DSCREEN_LOG_TAG, sessionId);
    if (jpegSessionFlag_ == false) {
        dataSessionOpened = true;
        sessionId_ = sessionId;
    } else {
        if (sessionId == sessionId_) {
        dataSessionOpened = true;
        sessionId_ = sessionId;
        } else if (sessionId == jpegSessionId_) {
            jpegSessionOpened = true;
            jpegSessionId_ = sessionId;
        }
    }
    std::shared_ptr<IScreenChannelListener> listener = channelListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%{public}s: Channel listener is null", DSCREEN_LOG_TAG);
        return;
    }
    listener->OnSessionOpened();
}

void ScreenDataChannelImpl::OnSessionClosed(int32_t sessionId, ShutdownReason reason)
{
    (void)reason;
    DHLOGI("%{public}s: OnScreenSessionClosed, sessionId %{public}" PRId32, DSCREEN_LOG_TAG, sessionId);
    std::shared_ptr<IScreenChannelListener> listener = channelListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%{public}s: Channel listener is null", DSCREEN_LOG_TAG);
        return;
    }
    listener->OnSessionClosed();
}

void ScreenDataChannelImpl::OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    (void) sessionId;
    (void) data;
    (void) dataLen;

    DHLOGD("%{public}s: OnScreenBytesReceived data channel not support yet", DSCREEN_LOG_TAG);
}

void ScreenDataChannelImpl::OnStreamReceived(int32_t sessionId, const StreamData *data,
    const StreamData *ext, const StreamFrameInfo *param)
{
    DHLOGI("%{public}s: OnStreamReceived, receiv data from softbus.", DSCREEN_LOG_TAG);
    if (data == nullptr) {
        DHLOGE("%{public}s: Stream data is null", DSCREEN_LOG_TAG);
        return;
    }
    auto dataBuffer = std::make_shared<DataBuffer>(data->bufLen);
    if (ext->bufLen == 0) {
        DHLOGI("sink received full data.");
        ProcessDullData(data, dataBuffer);
        return;
    } else {
        DHLOGI("sink received dirty data.");
        ProcessDirtyData(data, dataBuffer, ext);
    }
}

void ScreenDataChannelImpl::ProcessDullData(const StreamData *data, std::shared_ptr<DataBuffer> dataBuffer)
{
    DHLOGI("%{public}s: ProcessDullData.", DSCREEN_LOG_TAG);
    std::shared_ptr<IScreenChannelListener> listener = channelListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%{public}s: Channel listener is null.", DSCREEN_LOG_TAG);
        return;
    }
    int32_t ret = memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(),
        reinterpret_cast<uint8_t*>(data->buf), data->bufLen);
    if (ret != EOK) {
        DHLOGE("%{public}s: Full data memcpy failed.", DSCREEN_LOG_TAG);
        return;
    }
    dataBuffer->SetDataType(VIDEO_FULL_SCREEN_DATA);
    listener->OnDataReceived(dataBuffer);
}

void ScreenDataChannelImpl::ProcessDirtyData(const StreamData *data,
    std::shared_ptr<DataBuffer> dataBuffer, const StreamData *ext)
{
    DHLOGI("%{public}s: ProcessDirtyData.", DSCREEN_LOG_TAG);
    std::shared_ptr<IScreenChannelListener> listener = channelListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%{public}s: Channel listener is null.", DSCREEN_LOG_TAG);
        return;
    }
    int32_t ret = memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(),
        reinterpret_cast<uint8_t*>(data->buf), data->bufLen);
    if (ret != EOK) {
        DHLOGE("%{public}s: Dirty data memcpy_s failed.", DSCREEN_LOG_TAG);
        return;
    }
    nlohmann::json rectJson = nlohmann::json::parse(ext->buf, nullptr, false);
    if (rectJson.is_discarded()) {
        DHLOGE("%{public}s: OnStreamReceived rectJson invalid", DSCREEN_LOG_TAG);
        return;
    }
    JsonToDirtyJson(rectJson, dataBuffer);
    listener->OnDataReceived(dataBuffer);
}

void ScreenDataChannelImpl::DirtyVecToJson(nlohmann::json &rectJson, const std::shared_ptr<DataBuffer> &screenData)
{
    std::vector<DirtyRect> dirtyRectVec = screenData->GetDirtyRectVec();
    rectJson["dataType"] = screenData->DataType();
    rectJson["dirtySize"] = dirtyRectVec.size();
    int32_t rectIndex = 0;
    for (auto item : dirtyRectVec) {
        std::string tempRectIndex = std::to_string(rectIndex);
        rectJson[tempRectIndex] = nlohmann::json {
            {KEY_POINT_DIRTY_X, item.xPos},
            {KEY_POINT_DIRTY_Y, item.yPos},
            {KEY_POINT_DIRTY_W, item.width},
            {KEY_POINT_DIRTY_H, item.height},
            {KEY_POINT_DIRTY_SIZE, item.dirtySize}
        };
        rectIndex++;
    }
}

void ScreenDataChannelImpl::JsonToDirtyJson(nlohmann::json &rectJson, std::shared_ptr<DataBuffer> &screenData)
{
    if (!IsInt32(rectJson, "dirtySize") || !IsInt32(rectJson, "dataType")) {
        return;
    }
    int32_t dirtySize = rectJson["dirtySize"].get<int32_t>();
    int32_t dataType = rectJson["dataType"].get<int32_t>();
    int32_t num = 0;
    if (dirtySize >= DIRTY_MAX_SIZE) {
        return;
    }
    while (num < dirtySize) {
        auto item = std::to_string(num);
        if (!rectJson.contains(item)) {
            return;
        }
        if (!IsInt32(rectJson[item], KEY_POINT_DIRTY_X) || !IsInt32(rectJson[item], KEY_POINT_DIRTY_Y) ||
            !IsInt32(rectJson[item], KEY_POINT_DIRTY_W) || !IsInt32(rectJson[item], KEY_POINT_DIRTY_H) ||
            !IsInt32(rectJson[item], KEY_POINT_DIRTY_SIZE)) {
            DHLOGE("%{public}s: JsonToDirtyJson rectJson not contain int32", DSCREEN_LOG_TAG);
            return;
        }
        int32_t X = rectJson[item][KEY_POINT_DIRTY_X].get<int32_t>();
        int32_t Y = rectJson[item][KEY_POINT_DIRTY_Y].get<int32_t>();
        int32_t W = rectJson[item][KEY_POINT_DIRTY_W].get<int32_t>();
        int32_t H = rectJson[item][KEY_POINT_DIRTY_H].get<int32_t>();
        int32_t Size = rectJson[item][KEY_POINT_DIRTY_SIZE].get<int32_t>();
        DirtyRect rect = {X, Y, W, H, Size};
        screenData->AddDirtyRect(rect);
        num++;
    }
    screenData->SetDataType(dataType);
}
} // namespace DistributedHardware
} // namespace OHOS