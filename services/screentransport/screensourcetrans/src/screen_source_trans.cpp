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

#include "screen_source_trans.h"

#include <chrono>

#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_hitrace.h"
#include "dscreen_log.h"
#include "image_source_processor.h"
#include "screen_data_channel_impl.h"

namespace OHOS {
namespace DistributedHardware {
int32_t ScreenSourceTrans::SetUp(const VideoParam &localParam, const VideoParam &remoteParam,
    const std::string &peerDevId)
{
    DHLOGI("%s: SetUp.", LOG_TAG);
    int32_t ret = CheckTransParam(localParam, remoteParam, peerDevId);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: SetUp failed param error ret: %d.", LOG_TAG, ret);
        return ret;
    }

    ret = InitScreenTrans(localParam, remoteParam, peerDevId);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: SetUp failed ret: %d.", LOG_TAG, ret);
        return ret;
    }

    DHLOGI("%s: SetUp success.", LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::Release()
{
    DHLOGI("%s: Release.", LOG_TAG);
    if (imageProcessor_ == nullptr || screenChannel_ == nullptr) {
        DHLOGE("%s: Processor or channel is null, Setup first.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = imageProcessor_->ReleaseImageProcessor();
    if (ret != DH_SUCCESS) {
        DHLOGD("%s: Release image processor failed ret: %d.", LOG_TAG, ret);
    }
    imageProcessor_ = nullptr;

    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SOURCE_RELEASE_SESSION_START);
    ret = screenChannel_->ReleaseSession();
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_SUCCESS) {
        DHLOGD("%s: Release channel session failed ret: %d.", LOG_TAG, ret);
    }
    screenChannel_ = nullptr;

    std::lock_guard<std::mutex> lck(dataQueueMtx_);
    while (!dataQueue_.empty()) {
        dataQueue_.pop();
    }

    DHLOGI("%s: Release success.", LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::Start()
{
    DHLOGI("%s: Start.", LOG_TAG);
    if (imageProcessor_ == nullptr || screenChannel_ == nullptr) {
        DHLOGE("%s: Processor or channel is null, Setup first.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SOURCE_OPEN_SESSION_START);
    int32_t ret = screenChannel_->OpenSession();
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Open channel session failed ret: %d.", LOG_TAG, ret);
        return ret;
    }

    DHLOGI("%s: Wait for channel session opened.", LOG_TAG);
    std::unique_lock<std::mutex> lck(sessionMtx_);
    auto status =
        sessionCond_.wait_for(lck, std::chrono::seconds(SESSION_WAIT_SECONDS), [this]() { return isChannelReady_; });
    if (!status) {
        DHLOGE("%s: Open channel session timeout(%ds).", LOG_TAG, SESSION_WAIT_SECONDS);
        return ERR_DH_SCREEN_TRANS_TIMEOUT;
    }

    DHLOGI("%s: Start success.", LOG_TAG);
    FinishTrace(DSCREEN_HITRACE_LABEL);
    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::Stop()
{
    DHLOGI("%s: Stop.", LOG_TAG);
    if (imageProcessor_ == nullptr || screenChannel_ == nullptr) {
        DHLOGE("%s: Processor or channel is null, Setup first.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    bool stopStatus = true;
    int32_t ret = imageProcessor_->StopImageProcessor();
    if (ret != DH_SUCCESS) {
        DHLOGD("%s: Stop image processor failed ret: %d.", LOG_TAG, ret);
        stopStatus = false;
    }

    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SOURCE_CLOSE_SESSION_START);
    ret = screenChannel_->CloseSession();
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_SUCCESS) {
        DHLOGD("%s: Close Session failed ret: %d.", LOG_TAG, ret);
        stopStatus = false;
    }
    isChannelReady_ = false;
    if (sendDataThread_.joinable()) {
        sendDataThread_.join();
    }

    if (!stopStatus) {
        DHLOGE("%s: Stop source trans failed.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ERROR;
    }
    DHLOGI("%s: Stop success.", LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::RegisterStateCallback(const std::shared_ptr<IScreenSourceTransCallback> &callback)
{
    DHLOGI("%s:RegisterStateCallback.", LOG_TAG);
    if (callback == nullptr) {
        DHLOGE("%s: Trans callback is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    transCallback_ = callback;

    return DH_SUCCESS;
}

sptr<Surface> &ScreenSourceTrans::GetImageSurface()
{
    DHLOGI("%s:GetImageSurface.", LOG_TAG);
    return encoderSurface_;
}

int32_t ScreenSourceTrans::CheckVideoParam(const VideoParam &param)
{
    if ((param.GetCodecType() != VIDEO_CODEC_TYPE_VIDEO_H264) &&
        (param.GetCodecType() != VIDEO_CODEC_TYPE_VIDEO_H265) &&
        (param.GetCodecType() != VIDEO_CODEC_TYPE_VIDEO_MPEG4)) {
        DHLOGE("%s: Invalid codec type.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM;
    }

    if ((param.GetVideoFormat() != VIDEO_DATA_FORMAT_YUVI420) &&
        (param.GetVideoFormat() != VIDEO_DATA_FORMAT_NV12) &&
        (param.GetVideoFormat() != VIDEO_DATA_FORMAT_NV21) &&
        (param.GetVideoFormat() != VIDEO_DATA_FORMAT_RGBA8888)) {
        DHLOGE("%s: Invalid video data format.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM;
    }

    if ((param.GetVideoWidth() > DSCREEN_MAX_VIDEO_DATA_WIDTH) ||
        (param.GetVideoHeight() > DSCREEN_MAX_VIDEO_DATA_HEIGHT)) {
        DHLOGE("%s: Invalid video data size.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM;
    }

    if ((param.GetScreenWidth() > DSCREEN_MAX_SCREEN_DATA_WIDTH) ||
        (param.GetScreenHeight() > DSCREEN_MAX_SCREEN_DATA_HEIGHT)) {
        DHLOGE("%s: Invalid screen data size.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM;
    }

    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::CheckTransParam(const VideoParam &localParam, const VideoParam &remoteParam,
    const std::string &peerDevId)
{
    DHLOGI("%s:CheckTransParam.", LOG_TAG);
    if (peerDevId.empty()) {
        DHLOGE("%s: Remote device id is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = CheckVideoParam(localParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: check localParam param failed.", LOG_TAG);
        return ret;
    }

    ret = CheckVideoParam(remoteParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: check remoteParam param failed.", LOG_TAG);
        return ret;
    }

    DHLOGI("%s: Local: codecType(%u), videoFormat(%u), videoSize(%ux%u), screenSize(%ux%u).", LOG_TAG,
        localParam.GetCodecType(), localParam.GetVideoFormat(), localParam.GetVideoWidth(),
        localParam.GetVideoHeight(), localParam.GetScreenWidth(), localParam.GetScreenHeight());
    DHLOGI("%s: Remote: codecType(%u), videoFormat(%u), videoSize(%ux%u), screenSize(%ux%u).", LOG_TAG,
        remoteParam.GetCodecType(), remoteParam.GetVideoFormat(), remoteParam.GetVideoWidth(),
        remoteParam.GetVideoHeight(), remoteParam.GetScreenWidth(), remoteParam.GetScreenHeight());
    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::InitScreenTrans(const VideoParam &localParam, const VideoParam &remoteParam,
    const std::string &peerDevId)
{
    DHLOGI("%s:InitScreenTrans.", LOG_TAG);
    screenChannel_ = std::make_shared<ScreenDataChannelImpl>(peerDevId);

    int32_t ret = RegisterChannelListener();
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Register channel listener failed ret: %d.", LOG_TAG, ret);
        screenChannel_ = nullptr;
        return ret;
    }

    imageProcessor_ = std::make_shared<ImageSourceProcessor>();

    ret = RegisterProcessorListener(localParam, remoteParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Register data processor listener failed ret: %d.", LOG_TAG, ret);
        screenChannel_ = nullptr;
        imageProcessor_ = nullptr;
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::RegisterChannelListener()
{
    DHLOGI("%s: RegisterChannelListener.", LOG_TAG);
    std::shared_ptr<IScreenChannelListener> listener = shared_from_this();
    if (listener == nullptr) {
        DHLOGE("%s: Channel listener is null", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    if (screenChannel_ == nullptr) {
        DHLOGE("%s: Channel is null", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    int32_t ret = screenChannel_->CreateSession(listener);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Create session failed ret: %d.", LOG_TAG);
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "dscreen source Create session failed.");
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::RegisterProcessorListener(const VideoParam &localParam, const VideoParam &remoteParam)
{
    DHLOGI("%s: RegisterProcessorListener.", LOG_TAG);
    std::shared_ptr<IImageSourceProcessorListener> listener = shared_from_this();
    if (listener == nullptr) {
        DHLOGE("%s: Processor listener is null", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ERROR;
    }

    if (imageProcessor_ == nullptr) {
        DHLOGE("%s: imageProcessor is null", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    int32_t ret = imageProcessor_->ConfigureImageProcessor(localParam, remoteParam, listener);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Config image processor failed ret: %d.", LOG_TAG, ret);
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "Config image processor failed.");
        return ret;
    }

    encoderSurface_ = imageProcessor_->GetImageSurface();
    if (encoderSurface_ == nullptr) {
        DHLOGE("%s: Surface is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    return DH_SUCCESS;
}

void ScreenSourceTrans::OnSessionOpened()
{
    DHLOGI("%s: OnChannelSessionOpened.", LOG_TAG);
    if (imageProcessor_ == nullptr) {
        DHLOGE("%s: imageProcessor is null", LOG_TAG);
        return;
    }
    int32_t ret = imageProcessor_->StartImageProcessor();
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Start image processor failed ret: %d.", LOG_TAG, ret);
        return;
    }

    isChannelReady_ = true;
    DHLOGI("%s: Start thread.", LOG_TAG);
    sendDataThread_ = std::thread(&ScreenSourceTrans::FeedChannelData, this);

    std::unique_lock<std::mutex> lck(sessionMtx_);
    sessionCond_.notify_all();
}

void ScreenSourceTrans::OnSessionClosed()
{
    DHLOGI("%s: OnChannelSessionClosed.", LOG_TAG);
    isChannelReady_ = false;
    if (sendDataThread_.joinable()) {
        sendDataThread_.join();
    }

    std::shared_ptr<IScreenSourceTransCallback> callback = transCallback_.lock();
    if (callback == nullptr) {
        DHLOGE("%s: Trans callback is null.", LOG_TAG);
        return;
    }
    callback->OnError(ERR_DH_SCREEN_TRANS_SESSION_CLOSED, "OnChannelSessionClosed");
}

void ScreenSourceTrans::OnDataReceived(const std::shared_ptr<DataBuffer> &data)
{
    (void) data;
    DHLOGI("%s: OnChannelDataReceived source trans not support.", LOG_TAG);
}

void ScreenSourceTrans::OnImageProcessDone(const std::shared_ptr<DataBuffer> &data)
{
    DHLOGD("%s: OnProcessorDataReceived received data from data processor.", LOG_TAG);
    std::lock_guard<std::mutex> lck(dataQueueMtx_);
    while (dataQueue_.size() >= DATA_QUEUE_MAX_SIZE) {
        DHLOGE("%s: Data queue overflow.", LOG_TAG);
        dataQueue_.pop();
    }
    dataQueue_.push(data);
    dataCond_.notify_all();
}

void ScreenSourceTrans::OnProcessorStateNotify(int32_t state)
{
    DHLOGI("%s:OnProcessorStateNotify.", LOG_TAG);
    std::shared_ptr<IScreenSourceTransCallback> callback = transCallback_.lock();
    if (callback == nullptr) {
        DHLOGE("%s: Trans callback is null.", LOG_TAG);
        return;
    }
    callback->OnError(state, "OnProcessorStateNotify");
}

void ScreenSourceTrans::FeedChannelData()
{
    while (isChannelReady_) {
        std::shared_ptr<DataBuffer> screenData;
        {
            std::unique_lock<std::mutex> lock(dataQueueMtx_);
            dataCond_.wait_for(lock, std::chrono::seconds(DATA_WAIT_SECONDS), [this]() { return !dataQueue_.empty(); });
            if (dataQueue_.empty()) {
                DHLOGD("%s:Data queue is empty.", LOG_TAG);
                continue;
            }
            screenData = dataQueue_.front();
            dataQueue_.pop();
        }

        if (screenChannel_ == nullptr) {
            DHLOGE("%s: Channel is null", LOG_TAG);
            return;
        }
        if (screenData == nullptr) {
            DHLOGE("%s: Screen data is null", LOG_TAG);
            continue;
        }

        DHLOGD("%s: FeedChannelData.", LOG_TAG);
        int32_t ret = screenChannel_->SendData(screenData);
        if (ret != DH_SUCCESS) {
            DHLOGD("%s:Send data failed.", LOG_TAG);
        }
        DHLOGD("%s: FeedChannelData success.", LOG_TAG);
    }
}
} // namespace DistributedHardware
} // namespace OHOS