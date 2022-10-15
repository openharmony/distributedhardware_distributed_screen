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

#include "screen_data_channel_impl.h"

#include <securec.h>

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_log.h"
#include "dscreen_util.h"

namespace OHOS {
namespace DistributedHardware {
int32_t ScreenDataChannelImpl::CreateSession(const std::shared_ptr<IScreenChannelListener> &listener)
{
    DHLOGI("%s: CreateSession, peerDevId(%s)", LOG_TAG, GetAnonyString(peerDevId_).c_str());
    if (listener == nullptr) {
        DHLOGE("%s: Channel listener is null", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret =
        SoftbusAdapter::GetInstance().CreateSoftbusSessionServer(PKG_NAME, DATA_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Create softbus session failed ret: %d.", LOG_TAG, ret);
        return ret;
    }

    std::shared_ptr<ISoftbusListener> softbusListener = shared_from_this();
    ret = SoftbusAdapter::GetInstance().RegisterSoftbusListener(softbusListener, DATA_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Register softbus adapter listener failed ret: %d.", LOG_TAG, ret);
        return ret;
    }

    channelListener_ = listener;
    DHLOGI("%s: Create softbus session success.", LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenDataChannelImpl::ReleaseSession()
{
    DHLOGI("%s: ReleaseSession, peerDevId(%s)", LOG_TAG, GetAnonyString(peerDevId_).c_str());
    int32_t ret = SoftbusAdapter::GetInstance().RemoveSoftbusSessionServer(PKG_NAME, DATA_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Release softbus session failed ret: %d.", LOG_TAG, ret);
        return ret;
    }

    ret = SoftbusAdapter::GetInstance().UnRegisterSoftbusListener(DATA_SESSION_NAME, peerDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: UnRegister softbus adapter listener failed ret: %d.", LOG_TAG, ret);
        return ret;
    }

    DHLOGI("%s: Release softbus session success.", LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenDataChannelImpl::OpenSession()
{
    DHLOGI("%s: OpenSession, peerDevId(%s)", LOG_TAG, GetAnonyString(peerDevId_).c_str());
    int32_t sessionId =
        SoftbusAdapter::GetInstance().OpenSoftbusSession(DATA_SESSION_NAME, DATA_SESSION_NAME, peerDevId_);
    if (sessionId < 0) {
        DHLOGE("%s: Open screen session failed, ret: %d", LOG_TAG, sessionId);
        ReportOptFail(DSCREEN_OPT_FAIL, sessionId, "Open screen session failed");
        return ERR_DH_SCREEN_TRANS_ERROR;
    }
    sessionId_ = sessionId;

    DHLOGI("%s: Open screen session success, sessionId(%d)", LOG_TAG, sessionId_);
    return DH_SUCCESS;
}

int32_t ScreenDataChannelImpl::CloseSession()
{
    DHLOGI("%s: CloseSession, sessionId(%d)", LOG_TAG, sessionId_);
    if (sessionId_ == 0) {
        DHLOGD("%s: Session is not opened.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_SESSION_NOT_OPEN;
    }

    int32_t ret = SoftbusAdapter::GetInstance().CloseSoftbusSession(sessionId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Close screen session failed ret: %d.", LOG_TAG, ret);
        return ret;
    }
    sessionId_ = 0;

    DHLOGI("%s: Close screen session success.", LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenDataChannelImpl::SendData(const std::shared_ptr<DataBuffer> &screenData)
{
    DHLOGD("%s: SendData, sessionId(%d)", LOG_TAG, sessionId_);
    if (screenData == nullptr) {
        DHLOGE("%s: Screen data is null", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    StreamData data = {reinterpret_cast<char *>(screenData->Data()), screenData->Capacity()};
    StreamData ext = {0};
    StreamFrameInfo frameInfo = {0};

    int32_t ret = SoftbusAdapter::GetInstance().SendSoftbusStream(sessionId_, &data, &ext, &frameInfo);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Send screen data failed ret: %d.", LOG_TAG, ret);
        return ret;
    }

    return DH_SUCCESS;
}

void ScreenDataChannelImpl::OnSessionOpened(int32_t sessionId, int32_t result)
{
    DHLOGI("%s: OnScreenSessionOpened, sessionId: %d, result: %d", LOG_TAG, sessionId, result);
    if (result != 0) {
        DHLOGE("Session open failed.", LOG_TAG);
        return;
    }

    std::shared_ptr<IScreenChannelListener> listener = channelListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%s: Channel listener is null", LOG_TAG);
        return;
    }
    listener->OnSessionOpened();
    sessionId_ = sessionId;
}

void ScreenDataChannelImpl::OnSessionClosed(int32_t sessionId)
{
    DHLOGI("%s: OnScreenSessionClosed, sessionId(%d).", LOG_TAG, sessionId);
    std::shared_ptr<IScreenChannelListener> listener = channelListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%s: Channel listener is null", LOG_TAG);
        return;
    }
    listener->OnSessionClosed();
}

void ScreenDataChannelImpl::OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen)
{
    (void) sessionId;
    (void) data;
    (void) dataLen;

    DHLOGD("%s: OnScreenBytesReceived data channel not support yet.", LOG_TAG);
}

void ScreenDataChannelImpl::OnStreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
    const StreamFrameInfo *param)
{
    (void)ext;
    (void)param;

    if (data == nullptr) {
        DHLOGE("%s: Stream data is null.", LOG_TAG);
        return;
    }

    std::shared_ptr<IScreenChannelListener> listener = channelListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%s: Channel listener is null.", LOG_TAG);
        return;
    }

    DHLOGI("%s: OnScreenStreamReceived, sessionId(%d) dataSize(%zu).", LOG_TAG, sessionId, data->bufLen);
    auto dataBuffer = std::make_shared<DataBuffer>(data->bufLen);

    int32_t ret = memcpy_s(dataBuffer->Data(), dataBuffer->Capacity(), reinterpret_cast<uint8_t *>(data->buf),
        data->bufLen);
    if (ret != EOK) {
        DHLOGE("%s: Data memcpy_s failed.", LOG_TAG);
        return;
    }
    listener->OnDataReceived(dataBuffer);
}
} // namespace DistributedHardware
} // namespace OHOS