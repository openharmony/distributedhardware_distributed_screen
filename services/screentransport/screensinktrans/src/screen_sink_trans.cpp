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

#include "screen_sink_trans.h"

#include "distributed_hardware_errno.h"
#include "dscreen_errcode.h"
#include "dscreen_fwkkit.h"
#include "dscreen_hisysevent.h"
#include "dscreen_hitrace.h"
#include "dscreen_log.h"
#include "image_sink_processor.h"
#include "screen_data_channel_impl.h"
namespace OHOS {
namespace DistributedHardware {
int32_t ScreenSinkTrans::SetUp(const VideoParam &localParam, const VideoParam &remoteParam,
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

    DHLOGI("%{public}s: SetUp success.", DSCREEN_LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenSinkTrans::Release()
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

    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SINK_RELEASE_SESSION_START);
    ret = screenChannel_->ReleaseSession();
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_SUCCESS) {
        DHLOGD("%{public}s: Release channel session failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
    }
    screenChannel_ = nullptr;

    DHLOGI("%{public}s: Release success.", DSCREEN_LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenSinkTrans::Start()
{
    DHLOGI("%{public}s: Start.", DSCREEN_LOG_TAG);
    if (imageProcessor_ == nullptr) {
        DHLOGE("%{public}s: Processor is null, Setup first.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = imageProcessor_->StartImageProcessor();
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Start image processor failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }

    DHLOGI("%{public}s: Start success.", DSCREEN_LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenSinkTrans::Stop()
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

    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SINK_CLOSE_SESSION_START);
    ret = screenChannel_->CloseSession();
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_SUCCESS && ret != ERR_DH_SCREEN_TRANS_SESSION_NOT_OPEN) {
        DHLOGD("%{public}s: Close Session failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        stopStatus = false;
    }

    if (!stopStatus) {
        DHLOGE("%{public}s: Stop sink trans failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ERROR;
    }

    DHLOGI("%{public}s: Stop success.", DSCREEN_LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenSinkTrans::RegisterStateCallback(const std::shared_ptr<IScreenSinkTransCallback> &callback)
{
    DHLOGI("%{public}s:RegisterStateCallback.", DSCREEN_LOG_TAG);
    if (callback == nullptr) {
        DHLOGE("%{public}s: Trans callback is null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    transCallback_ = callback;

    return DH_SUCCESS;
}

int32_t ScreenSinkTrans::SetImageSurface(const sptr<Surface> &surface)
{
    if (surface == nullptr) {
        DHLOGE("%{public}s: Image surface is null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    decoderSurface_ = surface;

    return DH_SUCCESS;
}

int32_t ScreenSinkTrans::CheckVideoParam(const VideoParam &param)
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

int32_t ScreenSinkTrans::CheckTransParam(const VideoParam &localParam, const VideoParam &remoteParam,
    const std::string &peerDevId)
{
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

int32_t ScreenSinkTrans::InitScreenTrans(const VideoParam &localParam, const VideoParam &remoteParam,
    const std::string &peerDevId)
{
    screenChannel_ = std::make_shared<ScreenDataChannelImpl>(peerDevId);
    if (std::atoi(version_.c_str()) > DSCREEN_MIN_VERSION) {
        screenChannel_->SetJpegSessionFlag(true);
    }
    int32_t ret = RegisterChannelListener();
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Register channel listener failed.", DSCREEN_LOG_TAG);
        screenChannel_ = nullptr;
        return ret;
    }

    imageProcessor_ = std::make_shared<ImageSinkProcessor>();

    ret = RegisterProcessorListener(localParam, remoteParam, peerDevId);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Register processor listener failed.", DSCREEN_LOG_TAG);
        screenChannel_ = nullptr;
        imageProcessor_ = nullptr;
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ScreenSinkTrans::RegisterChannelListener()
{
    DHLOGI("%{public}s: RegisterChannelListener.", DSCREEN_LOG_TAG);
    std::shared_ptr<IScreenChannelListener> listener = shared_from_this();
    if (listener == nullptr) {
        DHLOGE("%{public}s: Channel Listener is null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = screenChannel_->CreateSession(listener);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Register channel listenner failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "dscreen sink Create session failed.");
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ScreenSinkTrans::RegisterProcessorListener(const VideoParam &localParam, const VideoParam &remoteParam,
    const std::string &peerDevId)
{
    if (imageProcessor_ == nullptr || decoderSurface_ == nullptr) {
        DHLOGE("%{public}s: imageProcessor_ or decoderSurface_ is nullptr.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    DHLOGI("%{public}s: RegisterProcessorListener.", DSCREEN_LOG_TAG);
    std::shared_ptr<IImageSinkProcessorListener> listener = shared_from_this();
    if (listener == nullptr) {
        DHLOGE("%{public}s: Channel listener to null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = imageProcessor_->ConfigureImageProcessor(localParam, remoteParam, listener);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Config image processor failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "Config image processor failed.");
        return ret;
    }

    ret = imageProcessor_->SetImageSurface(decoderSurface_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Set image surface failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }
    return DH_SUCCESS;
}

void ScreenSinkTrans::OnSessionOpened()
{
    DHLOGI("%{public}s: OnChannelSessionOpened and start enable low latency", DSCREEN_LOG_TAG);
    std::shared_ptr<DistributedHardwareFwkKit> dhFwkKit = DScreenFwkKit::GetInstance().GetDHFwkKit();
    if (dhFwkKit != nullptr) {
        int32_t ret = dhFwkKit->PublishMessage(DHTopic::TOPIC_LOW_LATENCY, ENABLE_LOW_LATENCY.dump());
        if (ret != DH_FWK_SUCCESS) {
            DHLOGE("%{public}s: Sink start enable low latency failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        }
    }
}

void ScreenSinkTrans::OnSessionClosed()
{
    DHLOGI("%{public}s: OnChannelSessionClosed and stop enable low latency", DSCREEN_LOG_TAG);
    std::shared_ptr<DistributedHardwareFwkKit> dhFwkKit = DScreenFwkKit::GetInstance().GetDHFwkKit();
    if (dhFwkKit != nullptr) {
        int32_t ret = dhFwkKit->PublishMessage(DHTopic::TOPIC_LOW_LATENCY, DISABLE_LOW_LATENCY.dump());
        if (ret != DH_FWK_SUCCESS) {
            DHLOGE("%{public}s: Sink stop enable low latency failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        }
    }

    std::shared_ptr<IScreenSinkTransCallback> callback = transCallback_.lock();
    if (callback == nullptr) {
        DHLOGE("%{public}s: Trans callback is null.", DSCREEN_LOG_TAG);
        return;
    }
    callback->OnError(ERR_DH_SCREEN_TRANS_SESSION_CLOSED, "OnChannelSessionClosed");
}

void ScreenSinkTrans::OnDataReceived(const std::shared_ptr<DataBuffer> &data)
{
    if (imageProcessor_ == nullptr || data == nullptr) {
        DHLOGE("%{public}s: imageProcessor_ or data is nullptr.", DSCREEN_LOG_TAG);
        return;
    }
    DHLOGD("%{public}s: OnChannelDataReceived.", DSCREEN_LOG_TAG);
    int32_t ret = imageProcessor_->ProcessImage(data);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: send data to image processor failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
    }
}

void ScreenSinkTrans::SetScreenVersion(const std::string &version)
{
    version_ = version;
}

void ScreenSinkTrans::OnProcessorStateNotify(int32_t state)
{
    DHLOGI("%{public}s: OnProcessorStateNotify.", DSCREEN_LOG_TAG);
    std::shared_ptr<IScreenSinkTransCallback> callback = transCallback_.lock();
    if (callback == nullptr) {
        DHLOGE("%{public}s: Trans callback is null.", DSCREEN_LOG_TAG);
        return;
    }
    callback->OnError(state, "OnProcessorStateNotify");
}
} // namespace DistributedHardware
} // namespace OHOS