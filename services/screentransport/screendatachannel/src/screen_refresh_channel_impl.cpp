/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "screen_refresh_channel_impl.h"

#include <securec.h>

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_log.h"
#include "dscreen_util.h"
#include "nlohmann/json.hpp"
#include "dscreen_json_util.h"

namespace OHOS {
namespace DistributedHardware {
int32_t ScreenRefreshChannelImpl::CreateSession(const std::shared_ptr<IScreenChannelListener> &listener)
{
    DHLOGI("%s: CreateSession, peerDevId(%s)", LOG_TAG, GetAnonyString(peerDevId_).c_str());
    if (listener == nullptr) {
        DHLOGE("%s: Channel listener is null", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret =
        SoftbusAdapter::GetInstance().CreateSoftbusSessionServer(PKG_NAME, DATA_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Create softbus session failed ret: %" PRId32, LOG_TAG, ret);
        return ret;
    }

    std::shared_ptr<ISoftbusListener> softbusListener = shared_from_this();
    ret = SoftbusAdapter::GetInstance().RegisterSoftbusListener(softbusListener, DATA_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Register softbus adapter listener failed ret: %" PRId32, LOG_TAG, ret);
        return ret;
    }

    ret = SoftbusAdapter::GetInstance().CreateSoftbusSessionServer(PKG_NAME, JPEG_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Create softbus session failed ret: %" PRId32, LOG_TAG, ret);
        return ret;
    }

    ret = SoftbusAdapter::GetInstance().RegisterSoftbusListener(softbusListener, JPEG_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Register softbus adapter listener failed ret: %" PRId32, LOG_TAG, ret);
        return ret;
    }

    channelListener_ = listener;
    DHLOGI("%s: Create softbus session success", LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenRefreshChannelImpl::ReleaseSession()
{
    DHLOGI("%s: ReleaseSession, peerDevId(%s)", LOG_TAG, GetAnonyString(peerDevId_).c_str());
    int32_t ret = SoftbusAdapter::GetInstance().RemoveSoftbusSessionServer(PKG_NAME, DATA_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Release softbus session failed ret: %" PRId32, LOG_TAG, ret);
        return ret;
    }

    ret = SoftbusAdapter::GetInstance().UnRegisterSoftbusListener(DATA_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: UnRegister softbus adapter listener failed ret: %" PRId32, LOG_TAG, ret);
        return ret;
    }

    ret = SoftbusAdapter::GetInstance().RemoveSoftbusSessionServer(PKG_NAME, JPEG_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Release softbus session failed ret: %" PRId32, LOG_TAG, ret);
        return ret;
    }

    ret = SoftbusAdapter::GetInstance().UnRegisterSoftbusListener(JPEG_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: UnRegister softbus adapter listener failed ret: %" PRId32, LOG_TAG, ret);
        return ret;
    }

    DHLOGI("%s: Release softbus session success", LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenRefreshChannelImpl::OpenSession()
{
    DHLOGI("%s: OpenSession, peerDevId(%s)", LOG_TAG, GetAnonyString(peerDevId_).c_str());
    int32_t ret = OpenDataSession();
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Open screen session failed, ret: %" PRId32, LOG_TAG, sessionId_);
        return ERR_DH_SCREEN_TRANS_ERROR;
    }
    ret = OpenJpegSession();
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Open screen session failed, ret: %" PRId32, LOG_TAG, jpegSessionId_);
        return ERR_DH_SCREEN_TRANS_ERROR;
    }
    return DH_SUCCESS;
}

int32_t ScreenRefreshChannelImpl::OpenDataSession()
{
    DHLOGI("%s: OpenDataSession, peerDevId(%s)", LOG_TAG, GetAnonyString(peerDevId_).c_str());
    int32_t sessionId =
        SoftbusAdapter::GetInstance().OpenSoftbusSession(DATA_SESSION_NAME, DATA_SESSION_NAME, peerDevId_);
    if (sessionId < 0) {
        DHLOGE("%s: Open screen session failed, ret: %" PRId32, LOG_TAG, sessionId);
        ReportOptFail(DSCREEN_OPT_FAIL, sessionId, "Open screen session failed");
        return ERR_DH_SCREEN_TRANS_ERROR;
    }
    sessionId_ = sessionId;

    DHLOGI("%s: Open screen session success, sessionId %" PRId32, LOG_TAG, sessionId_);
    return DH_SUCCESS;
}

int32_t ScreenRefreshChannelImpl::OpenJpegSession()
{
    DHLOGI("%s: OpenJpegSession, peerDevId(%s)", LOG_TAG, GetAnonyString(peerDevId_).c_str());
    int32_t sessionId =
        SoftbusAdapter::GetInstance().OpenSoftbusSession(JPEG_SESSION_NAME, JPEG_SESSION_NAME, peerDevId_);
    if (sessionId < 0) {
        DHLOGE("%s: Open screen session failed, ret: %" PRId32, LOG_TAG, sessionId);
        ReportOptFail(DSCREEN_OPT_FAIL, sessionId, "Open screen session failed");
        return ERR_DH_SCREEN_TRANS_ERROR;
    }
    jpegSessionId_ = sessionId;

    DHLOGI("%s: Open screen session success, sessionId %" PRId32, LOG_TAG, sessionId_);
    return DH_SUCCESS;

}

int32_t ScreenRefreshChannelImpl::CloseSession()
{
    DHLOGI("%s: CloseSession, sessionId %" PRId32, LOG_TAG, sessionId_);
    if (sessionId_ == 0) {
        DHLOGD("%s: Session is not opened.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_SESSION_NOT_OPEN;
    }

    int32_t ret = SoftbusAdapter::GetInstance().CloseSoftbusSession(sessionId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Close screen session failed ret: %" PRId32, LOG_TAG, ret);
        return ret;
    }
    sessionId_ = 0;

    if (jpegSessionId_ == 0) {
        DHLOGD("%s: jpegSessionId is not opened.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_SESSION_NOT_OPEN;
    }

    ret = SoftbusAdapter::GetInstance().CloseSoftbusSession(jpegSessionId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Close jpeg session failed ret: %" PRId32, LOG_TAG, ret);
        return ret;
    }
    jpegSessionId_ = 0;
    DHLOGI("%s: Close screen session success", LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenRefreshChannelImpl::SendData(const std::shared_ptr<DataBuffer> &screenData)
{
    DHLOGI("%s: SendData.", LOG_TAG);
    if (screenData == nullptr || screenData->Data() == nullptr) {
        DHLOGE("%s: Screen data is null", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    uint8_t dataType = screenData->DataType();
    if (dataType == VIDEO_PART_SCREEN_DATA) {
        int32_t ret = SendDirtyData(screenData);
        if (ret != DH_SUCCESS) {
            DHLOGE("%s: send dirty data failed, ret: %.", PRId32, LOG_TAG, ret);
            return ret;
        }
    } else if (dataType == VIDEO_FULL_SCREEN_DATA) {
        int32_t ret = SendFullData(screenData);
        if (ret != DH_SUCCESS) {
            DHLOGE("%s: send full data failed, ret: %.", PRId32, LOG_TAG, ret);
            return ret;
        }
    }
    return DH_SUCCESS;
}

int32_t ScreenRefreshChannelImpl::SendFullData(const std::shared_ptr<DataBuffer> &screenData)
{
    DHLOGI("%s: SendFullData sessionId: %.", PRId32, LOG_TAG, sessionId_);
    if (screenData == nullptr) {
        DHLOGE("%s: Screen data is null", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    StreamData data = {reinterpret_cast<char *>(screenData->Data()), screenData->Capacity()};
    StreamData ext = {0};
    StreamFrameInfo frameInfo = {0};
    int32_t ret = SoftbusAdapter::GetInstance().SendSoftbusStream(sessionId_, &data, &ext, &frameInfo);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Send full data failed ret: %" PRId32, LOG_TAG, ret);
        return ret;
    }
    DHLOGI("%s: SendFullData success.", LOG_TAG);
    return DH_SUCCESS;
}
int32_t ScreenRefreshChannelImpl::SendDirtyData(const std::shared_ptr<DataBuffer> &screenData)
{
    DHLOGI("%s: SendDirtyData sessionId: %.", PRId32, LOG_TAG, jpegSessionId_);
    if (screenData == nullptr) {
        DHLOGE("%s: Screen data is null", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    nlohmann::json rectJson;
    std::vector<DirtyRect> dirtyRectVec = screenData->DirtyRectVec();
    rectJson["dataType"] = screenData->DataType();
    rectJson["dirtySize"] = static_cast<uint8_t>(dirtyRectVec.size());
    int32_t rectIndex = 0;
    for (auto item : dirtyRectVec) {
        std::string tempRectIndex = std::to_string(rectIndex);
        rectJson[tempRectIndex] = {item.xPos, item.yPos, item.width, item.height, item.dirtySize};
        rectIndex++;
    }
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
        DHLOGE("%s: Send dirty data failed ret: %" PRId32, LOG_TAG, ret);
        delete [] dirtyInfo;
        return ret;
    }
    delete [] dirtyInfo;
    DHLOGI("%s: SendDirtyData success.", LOG_TAG);
    return DH_SUCCESS;
}

void ScreenRefreshChannelImpl::OnSessionOpened(int32_t sessionId, int32_t result)
{
    DHLOGI("%s: OnScreenSessionOpened, sessionId: %" PRId32", result: %" PRId32, LOG_TAG, sessionId, result);
    if (result != 0) {
        DHLOGE("Session open failed", LOG_TAG);
        return;
    }

    if (sessionId == sessionId_) {
        dataSessionOpened = true;
        sessionId_ = sessionId;
    } else if (sessionId == jpegSessionId_) {
        jpegSessionOpened = true;
        jpegSessionId_ = sessionId;
    }
    if (!dataSessionOpened || !jpegSessionOpened) {
        return;
    }

    std::shared_ptr<IScreenChannelListener> listener = channelListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%s: Channel listener is null", LOG_TAG);
        return;
    }
    listener->OnSessionOpened();
}

void ScreenRefreshChannelImpl::OnSessionClosed(int32_t sessionId)
{
    DHLOGI("%s: OnScreenSessionClosed, sessionId %" PRId32, LOG_TAG, sessionId);
    std::shared_ptr<IScreenChannelListener> listener = channelListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%s: Channel listener is null", LOG_TAG);
        return;
    }
    listener->OnSessionClosed();
}

void ScreenRefreshChannelImpl::OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    (void) sessionId;
    (void) data;
    (void) dataLen;

    DHLOGD("%s: OnScreenBytesReceived data channel not support yet", LOG_TAG);
}

void ScreenRefreshChannelImpl::OnStreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
    const StreamFrameInfo *param)
{
    DHLOGI("%s: OnStreamReceived.", LOG_TAG);
    if (data == nullptr) {
        DHLOGE("%s: Stream data is null", LOG_TAG);
        return;
    }

    std::shared_ptr<IScreenChannelListener> listener = channelListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%s: Channel listener is null.", LOG_TAG);
        return;
    }
    if (ext->bufLen == 0) {
        DHLOGI("sink received full data.");
        auto dataBuffer = std::make_shared<DataBuffer>(data->bufLen);
        int32_t ret = memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), reinterpret_cast<uint8_t*>(data->buf), data->bufLen);
        if (ret != EOK) {
            DHLOGE("%s: Full data memcpy failed.", LOG_TAG);
            return;
        }
        dataBuffer->SetDataType(VIDEO_FULL_SCREEN_DATA);
        listener->OnDataReceived(dataBuffer);
        return;
    } else {
        DHLOGI("sink received dirty data.");
        auto dataBuffer = std::make_shared<DataBuffer>(data->bufLen);
        int32_t ret = memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), reinterpret_cast<uint8_t*>(data->buf), data->bufLen);
        if (ret != EOK) {
            DHLOGE("%s: Dirty data memcpy_s failed.", LOG_TAG);
            return;
        }
        nlohmann::json rectJson = nlohmann::json::parse(ext->buf, nullptr, false);
        if (rectJson.is_discarded()) {
            DHLOGE("%s: OnStreamReceived rectJson invalied", LOG_TAG);
            return;
        }
        if (!IsInt32(rectJson, "dirtySize") || !IsInt32(rectJson, "dataType")) {
            return;
        }
        int32_t dirtySize = rectJson["dirtySize"].get<int32_t>();
        int32_t dataType = rectJson["dataType"].get<int32_t>();
        uint8_t num = 0;
        while (num < dirtySize) {
            auto item = std::to_string(num);
            if (!rectJson.contains(item)) {
                return;
            }
            int32_t X = rectJson[item][0].get<int32_t>();
            int32_t Y = rectJson[item][1].get<int32_t>();
            int32_t W = rectJson[item][2].get<int32_t>();
            int32_t H = rectJson[item][3].get<int32_t>();
            int32_t Size = rectJson[item][4].get<int32_t>();
            DirtyRect rect = {X, Y, W, H, Size};
            dataBuffer->AddDirtyRect(rect);
            num++;
        }
        dataBuffer->SetDataType(dataType);
        listener->OnDataReceived(dataBuffer);
    }
}
} // namespace DistributedHardware
} // namespace OHOS