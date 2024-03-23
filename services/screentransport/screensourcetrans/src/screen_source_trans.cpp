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

#include "screen_source_trans.h"

#include <chrono>
#include <pthread.h>

#include "distributed_hardware_errno.h"
#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_fwkkit.h"
#include "dscreen_hisysevent.h"
#include "dscreen_hitrace.h"
#include "dscreen_log.h"
#include "image_source_processor.h"
#include "screen_data_channel_impl.h"
namespace OHOS {
namespace DistributedHardware {
constexpr const char* FDATA_THREAD = "FeedDataThread";
int32_t ScreenSourceTrans::SetUp(const VideoParam &localParam, const VideoParam &remoteParam,
    const std::string &peerDevId)
{
    DHLOGI("%{public}s: SetUp.", DSCREEN_LOG_TAG);
    int32_t ret = CheckTransParam(localParam, remoteParam, peerDevId);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: SetUp failed param error ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }
    ret = InitScreenTrans(localParam, remoteParam, peerDevId);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: SetUp failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }
    ret = SetConsumerSurface();
    if (ret != DH_SUCCESS) {
        DHLOGE("sourcetrans set image surface failed.");
        return ret;
    }
    ret = screenDecisionCenter_->SetJpegSurface(consumerSurface_);
    if (ret != DH_SUCCESS) {
        DHLOGE("screenDecisionCenter set jpeg surface failed.");
        return ret;
    }
    DHLOGI("%{public}s: SetUp success.", DSCREEN_LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::SetConsumerSurface()
{
    DHLOGI("%{public}s: SetConsumerSurface.", DSCREEN_LOG_TAG);
    consumerSurface_ = imageProcessor_->GetConsumerSurface();
    if (consumerSurface_ == nullptr) {
        DHLOGE("%{public}s: consumerSurface is nullptr", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_SURFACE_INVALIED;
    }
    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::Release()
{
    DHLOGI("%{public}s: Release.", DSCREEN_LOG_TAG);
    if (imageProcessor_ == nullptr || screenChannel_ == nullptr) {
        DHLOGE("%{public}s: Processor or channel is null, Setup first.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = imageProcessor_->ReleaseImageProcessor();
    if (ret != DH_SUCCESS) {
        DHLOGD("%{public}s: Release image processor failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
    }
    imageProcessor_ = nullptr;

    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SOURCE_RELEASE_SESSION_START);
    ret = screenChannel_->ReleaseSession();
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_SUCCESS) {
        DHLOGD("%{public}s: Release channel session failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
    }
    screenChannel_ = nullptr;

    std::lock_guard<std::mutex> lck(dataQueueMtx_);
    while (!dataQueue_.empty()) {
        dataQueue_.pop();
    }

    DHLOGI("%{public}s: Release success.", DSCREEN_LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::Start()
{
    DHLOGI("%{public}s: Start.", DSCREEN_LOG_TAG);
    if (screenChannel_ == nullptr) {
        DHLOGE("%{public}s: channel is null, Setup first.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SOURCE_OPEN_SESSION_START);
    std::shared_ptr<IScreenChannelListener> listener = shared_from_this();
    if (listener == nullptr) {
        DHLOGE("%{public}s: Channel listener is null", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    if (screenChannel_ == nullptr) {
        DHLOGE("%{public}s: Channel is null", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    int32_t ret = screenChannel_->OpenSession(listener);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Open channel session failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }

    DHLOGI("%{public}s: Wait for channel session opened.", DSCREEN_LOG_TAG);
    std::unique_lock<std::mutex> lck(sessionMtx_);
    auto status =
        sessionCond_.wait_for(lck, std::chrono::seconds(SESSION_WAIT_SECONDS), [this]() { return isChannelReady_; });
    if (!status) {
        DHLOGE("%{public}s: Open channel session timeout(%{public}" PRId32 "s).", DSCREEN_LOG_TAG,
            SESSION_WAIT_SECONDS);
        return ERR_DH_SCREEN_TRANS_TIMEOUT;
    }

    DHLOGI("%{public}s: Source start enable low latency.", DSCREEN_LOG_TAG);
    std::shared_ptr<DistributedHardwareFwkKit> dhFwkKit = DScreenFwkKit::GetInstance().GetDHFwkKit();
    if (dhFwkKit != nullptr) {
        ret = dhFwkKit->PublishMessage(DHTopic::TOPIC_LOW_LATENCY, ENABLE_LOW_LATENCY.dump());
        if (ret != DH_FWK_SUCCESS) {
            DHLOGE("%{public}s: Source start enable low latency failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        }
    }

    DHLOGI("%{public}s: Start success.", DSCREEN_LOG_TAG);
    FinishTrace(DSCREEN_HITRACE_LABEL);
    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::Stop()
{
    DHLOGI("%{public}s: Stop.", DSCREEN_LOG_TAG);
    if (imageProcessor_ == nullptr || screenChannel_ == nullptr) {
        DHLOGE("%{public}s: Processor or channel is null, Setup first.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    bool stopStatus = true;
    int32_t ret = imageProcessor_->StopImageProcessor();
    if (ret != DH_SUCCESS) {
        DHLOGD("%{public}s: Stop image processor failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        stopStatus = false;
    }

    DHLOGI("%{public}s: Source stop enable low latency.", DSCREEN_LOG_TAG);
    std::shared_ptr<DistributedHardwareFwkKit> dhFwkKit = DScreenFwkKit::GetInstance().GetDHFwkKit();
    if (dhFwkKit != nullptr) {
        ret = dhFwkKit->PublishMessage(DHTopic::TOPIC_LOW_LATENCY, DISABLE_LOW_LATENCY.dump());
        if (ret != DH_FWK_SUCCESS) {
            DHLOGE("%{public}s: Source stop enable low latency failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        }
    }

    isChannelReady_ = false;
    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SOURCE_CLOSE_SESSION_START);
    ret = screenChannel_->CloseSession();
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_SUCCESS) {
        DHLOGD("%{public}s: Close Session failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        stopStatus = false;
    }

    if (!stopStatus) {
        DHLOGE("%{public}s: Stop source trans failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ERROR;
    }
    DHLOGI("%{public}s: Stop success.", DSCREEN_LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::RegisterStateCallback(const std::shared_ptr<IScreenSourceTransCallback> &callback)
{
    DHLOGI("%{public}s:RegisterStateCallback.", DSCREEN_LOG_TAG);
    if (callback == nullptr) {
        DHLOGE("%{public}s: Trans callback is null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    transCallback_ = callback;

    return DH_SUCCESS;
}

sptr<Surface> ScreenSourceTrans::GetImageSurface()
{
    DHLOGI("%{public}s:GetImageSurface.", DSCREEN_LOG_TAG);
    return imageProcessor_->GetImageSurface();
}

void ScreenSourceTrans::SetScreenVersion(const std::string &version)
{
    version_ = version;
}

int32_t ScreenSourceTrans::CheckVideoParam(const VideoParam &param)
{
    if ((param.GetCodecType() != VIDEO_CODEC_TYPE_VIDEO_H264) &&
        (param.GetCodecType() != VIDEO_CODEC_TYPE_VIDEO_H265) &&
        (param.GetCodecType() != VIDEO_CODEC_TYPE_VIDEO_MPEG4)) {
        DHLOGE("%{public}s: Invalid codec type.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM;
    }

    if ((param.GetVideoFormat() != VIDEO_DATA_FORMAT_YUVI420) &&
        (param.GetVideoFormat() != VIDEO_DATA_FORMAT_NV12) &&
        (param.GetVideoFormat() != VIDEO_DATA_FORMAT_NV21) &&
        (param.GetVideoFormat() != VIDEO_DATA_FORMAT_RGBA8888)) {
        DHLOGE("%{public}s: Invalid video data format.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM;
    }

    if ((param.GetVideoWidth() > DSCREEN_MAX_VIDEO_DATA_WIDTH) ||
        (param.GetVideoHeight() > DSCREEN_MAX_VIDEO_DATA_HEIGHT)) {
        DHLOGE("%{public}s: Invalid video data size.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM;
    }

    if ((param.GetScreenWidth() > DSCREEN_MAX_SCREEN_DATA_WIDTH) ||
        (param.GetScreenHeight() > DSCREEN_MAX_SCREEN_DATA_HEIGHT)) {
        DHLOGE("%{public}s: Invalid screen data size.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM;
    }

    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::CheckTransParam(const VideoParam &localParam, const VideoParam &remoteParam,
    const std::string &peerDevId)
{
    DHLOGI("%{public}s:CheckTransParam.", DSCREEN_LOG_TAG);
    if (peerDevId.empty()) {
        DHLOGE("%{public}s: Remote device id is null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = CheckVideoParam(localParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: check localParam param failed.", DSCREEN_LOG_TAG);
        return ret;
    }

    ret = CheckVideoParam(remoteParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: check remoteParam param failed.", DSCREEN_LOG_TAG);
        return ret;
    }
    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::InitScreenTrans(const VideoParam &localParam, const VideoParam &remoteParam,
    const std::string &peerDevId)
{
    DHLOGI("%{public}s:InitScreenTrans.", DSCREEN_LOG_TAG);
    screenChannel_ = std::make_shared<ScreenDataChannelImpl>(peerDevId);
    if (std::atoi(version_.c_str()) > DSCREEN_MIN_VERSION) {
        screenChannel_->SetJpegSessionFlag(true);
    }
    int32_t ret = RegisterChannelListener();
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Register channel listener failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        screenChannel_ = nullptr;
        return ret;
    }
    screenDecisionCenter_ = std::make_shared<ScreenDecisionCenter>(localParam);
    imageProcessor_ = std::make_shared<ImageSourceProcessor>();
    ret = RegisterProcessorListener(localParam, remoteParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Register data processor listener failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        screenChannel_ = nullptr;
        imageProcessor_ = nullptr;
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::RegisterChannelListener()
{
    DHLOGI("%{public}s: RegisterChannelListener.", DSCREEN_LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenSourceTrans::RegisterProcessorListener(const VideoParam &localParam, const VideoParam &remoteParam)
{
    DHLOGI("%{public}s: RegisterProcessorListener.", DSCREEN_LOG_TAG);
    std::shared_ptr<IImageSourceProcessorListener> listener = shared_from_this();
    if (listener == nullptr) {
        DHLOGE("%{public}s: Processor listener is null", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ERROR;
    }

    if (imageProcessor_ == nullptr) {
        DHLOGE("%{public}s: imageProcessor is null", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    int32_t ret = imageProcessor_->ConfigureImageProcessor(localParam, remoteParam, listener);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Config image processor failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "Config image processor failed.");
        return ret;
    }
    ret = screenDecisionCenter_->ConfigureDecisionCenter(listener, imageProcessor_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Config decision center failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }
    return DH_SUCCESS;
}

void ScreenSourceTrans::OnSessionOpened()
{
    DHLOGI("%{public}s: OnChannelSessionOpened.", DSCREEN_LOG_TAG);
    if (imageProcessor_ == nullptr) {
        DHLOGE("%{public}s: imageProcessor is null", DSCREEN_LOG_TAG);
        return;
    }
    int32_t ret = imageProcessor_->StartImageProcessor();
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Start image processor failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return;
    }

    isChannelReady_ = true;
    DHLOGI("%{public}s: Start thread.", DSCREEN_LOG_TAG);
    std::thread(&ScreenSourceTrans::FeedChannelData, this).detach();
    std::unique_lock<std::mutex> lck(sessionMtx_);
    sessionCond_.notify_all();
}

void ScreenSourceTrans::OnSessionClosed()
{
    DHLOGI("%{public}s: OnChannelSessionClosed.", DSCREEN_LOG_TAG);
    isChannelReady_ = false;

    std::shared_ptr<IScreenSourceTransCallback> callback = transCallback_.lock();
    if (callback == nullptr) {
        DHLOGE("%{public}s: Trans callback is null.", DSCREEN_LOG_TAG);
        return;
    }
    callback->OnError(ERR_DH_SCREEN_TRANS_SESSION_CLOSED, "OnChannelSessionClosed");
}

void ScreenSourceTrans::OnDataReceived(const std::shared_ptr<DataBuffer> &data)
{
    (void) data;
    DHLOGI("%{public}s: OnChannelDataReceived source trans not support.", DSCREEN_LOG_TAG);
}

void ScreenSourceTrans::OnDamageProcessDone(sptr<SurfaceBuffer> &surfaceBuffer, const std::vector<OHOS::Rect> &damages)
{
    DHLOGI("%{public}s: OnDamageProcessDone.", DSCREEN_LOG_TAG);
    if (surfaceBuffer == nullptr) {
        DHLOGE("%{public}s: Trans surfaceBuffer is null.", DSCREEN_LOG_TAG);
        return;
    }
    if (std::atoi(version_.c_str()) == DSCREEN_MIN_VERSION) {
        DHLOGI("%{public}s: not support partial refresh, run full full image process.", DSCREEN_LOG_TAG);
        imageProcessor_->ProcessFullImage(surfaceBuffer);
    } else {
        DHLOGI("%{public}s: run partial refresh image process.", DSCREEN_LOG_TAG);
        screenDecisionCenter_->InputBufferImage(surfaceBuffer, damages);
    }
}

void ScreenSourceTrans::OnImageProcessDone(const std::shared_ptr<DataBuffer> &data)
{
    DHLOGD("%{public}s: OnImageProcessDone.", DSCREEN_LOG_TAG);
    std::lock_guard<std::mutex> lck(dataQueueMtx_);
    while (dataQueue_.size() >= DATA_QUEUE_MAX_SIZE) {
        DHLOGE("%{public}s: Data queue overflow.", DSCREEN_LOG_TAG);
        dataQueue_.pop();
    }
    dataQueue_.push(data);
    dataCond_.notify_all();
}

void ScreenSourceTrans::OnProcessorStateNotify(int32_t state)
{
    DHLOGI("%{public}s:OnProcessorStateNotify.", DSCREEN_LOG_TAG);
    std::shared_ptr<IScreenSourceTransCallback> callback = transCallback_.lock();
    if (callback == nullptr) {
        DHLOGE("%{public}s: Trans callback is null.", DSCREEN_LOG_TAG);
        return;
    }
    callback->OnError(state, "OnProcessorStateNotify");
}

void ScreenSourceTrans::FeedChannelData()
{
    int32_t ret = pthread_setname_np(pthread_self(), FDATA_THREAD);
    if (ret != DH_SUCCESS) {
        DHLOGE("ScreenSourceTrans set thread name failed, ret %{public}" PRId32, ret);
    }
    while (isChannelReady_) {
        std::shared_ptr<DataBuffer> screenData;
        {
            std::unique_lock<std::mutex> lock(dataQueueMtx_);
            dataCond_.wait_for(lock, std::chrono::seconds(DATA_WAIT_SECONDS), [this]() { return !dataQueue_.empty(); });
            if (dataQueue_.empty()) {
                DHLOGD("%{public}s:Data queue is empty.", DSCREEN_LOG_TAG);
                continue;
            }
            screenData = dataQueue_.front();
            dataQueue_.pop();
        }

        if (screenChannel_ == nullptr) {
            DHLOGE("%{public}s: Channel is null", DSCREEN_LOG_TAG);
            return;
        }
        if (screenData == nullptr) {
            DHLOGE("%{public}s: Screen data is null", DSCREEN_LOG_TAG);
            continue;
        }

        DHLOGD("%{public}s: FeedChannelData.", DSCREEN_LOG_TAG);
        ret = screenChannel_->SendData(screenData);
        if (ret != DH_SUCCESS) {
            DHLOGD("%{public}s:Send data failed.", DSCREEN_LOG_TAG);
        }
    }
}
} // namespace DistributedHardware
} // namespace OHOS