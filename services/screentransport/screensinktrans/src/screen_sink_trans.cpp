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

#include "screen_sink_trans.h"

#include "dscreen_errcode.h"
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

int32_t ScreenSinkTrans::Release()
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

    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SINK_RELEASE_SESSION_START);
    ret = screenChannel_->ReleaseSession();
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_SUCCESS) {
        DHLOGD("%s: Release channel session failed ret: %d.", LOG_TAG, ret);
    }
    screenChannel_ = nullptr;

    DHLOGI("%s: Release success.", LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenSinkTrans::Start()
{
    DHLOGI("%s: Start.", LOG_TAG);
    if (imageProcessor_ == nullptr || screenChannel_ == nullptr) {
        DHLOGE("%s: Processor or channel is null, Setup first.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = imageProcessor_->StartImageProcessor();
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Start image processor failed ret: %d.", LOG_TAG, ret);
        return ret;
    }

    DHLOGI("%s: Start success.", LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenSinkTrans::Stop()
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

    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SINK_CLOSE_SESSION_START);
    ret = screenChannel_->CloseSession();
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_SUCCESS && ret != ERR_DH_SCREEN_TRANS_SESSION_NOT_OPEN) {
        DHLOGD("%s: Close Session failed ret: %d.", LOG_TAG, ret);
        stopStatus = false;
    }

    if (!stopStatus) {
        DHLOGE("%s: Stop sink trans failed.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_ERROR;
    }

    DHLOGI("%s: Stop success.", LOG_TAG);
    return DH_SUCCESS;
}

int32_t ScreenSinkTrans::RegisterStateCallback(const std::shared_ptr<IScreenSinkTransCallback> &callback)
{
    DHLOGI("%s:RegisterStateCallback.", LOG_TAG);
    if (callback == nullptr) {
        DHLOGE("%s: Trans callback is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    transCallback_ = callback;

    return DH_SUCCESS;
}

int32_t ScreenSinkTrans::SetImageSurface(const sptr<Surface> &surface)
{
    if (surface == nullptr) {
        DHLOGE("%s: Image surface is null.", LOG_TAG);
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

int32_t ScreenSinkTrans::CheckTransParam(const VideoParam &localParam, const VideoParam &remoteParam,
    const std::string &peerDevId)
{
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

int32_t ScreenSinkTrans::InitScreenTrans(const VideoParam &localParam, const VideoParam &remoteParam,
    const std::string &peerDevId)
{
    screenChannel_ = std::make_shared<ScreenDataChannelImpl>(peerDevId);

    int32_t ret = RegisterChannelListener();
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Register channel listener failed.", LOG_TAG);
        screenChannel_ = nullptr;
        return ret;
    }

    imageProcessor_ = std::make_shared<ImageSinkProcessor>();

    ret = RegisterProcessorListener(localParam, remoteParam, peerDevId);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Register processor listener failed.", LOG_TAG);
        screenChannel_ = nullptr;
        imageProcessor_ = nullptr;
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ScreenSinkTrans::RegisterChannelListener()
{
    DHLOGI("%s: RegisterChannelListener.", LOG_TAG);
    std::shared_ptr<IScreenChannelListener> listener = shared_from_this();
    if (listener == nullptr) {
        DHLOGE("%s: Channel Listener is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = screenChannel_->CreateSession(listener);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Register channel listenner failed ret: %d.", LOG_TAG, ret);
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "dscreen sink Create session failed.");
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ScreenSinkTrans::RegisterProcessorListener(const VideoParam &localParam, const VideoParam &remoteParam,
    const std::string &peerDevId)
{
    DHLOGI("%s: RegisterProcessorListener.", LOG_TAG);
    std::shared_ptr<IImageSinkProcessorListener> listener = shared_from_this();
    if (listener == nullptr) {
        DHLOGE("%s: Channel listener to null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = imageProcessor_->ConfigureImageProcessor(localParam, remoteParam, listener);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Config image processor failed ret: %d.", LOG_TAG, ret);
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "Config image processor failed.");
        return ret;
    }

    ret = imageProcessor_->SetImageSurface(decoderSurface_);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Set image surface failed ret: %d.", LOG_TAG, ret);
        return ret;
    }
    return DH_SUCCESS;
}

void ScreenSinkTrans::OnSessionOpened()
{
    DHLOGI("%s: OnChannelSessionOpened.", LOG_TAG);
}

void ScreenSinkTrans::OnSessionClosed()
{
    DHLOGI("%s:OnChannelSessionClosed.", LOG_TAG);
    std::shared_ptr<IScreenSinkTransCallback> callback = transCallback_.lock();
    if (callback == nullptr) {
        DHLOGE("%s: Trans callback is null.", LOG_TAG);
        return;
    }
    callback->OnError(ERR_DH_SCREEN_TRANS_SESSION_CLOSED, "OnChannelSessionClosed");
}

void ScreenSinkTrans::OnDataReceived(const std::shared_ptr<DataBuffer> &data)
{
    DHLOGD("%s: OnChannelDataReceived.", LOG_TAG);
    int32_t ret = imageProcessor_->ProcessImage(data);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: send data to image processor failed ret: %d.", LOG_TAG, ret);
    }
}

void ScreenSinkTrans::OnProcessorStateNotify(int32_t state)
{
    DHLOGI("%s: OnProcessorStateNotify.", LOG_TAG);
    std::shared_ptr<IScreenSinkTransCallback> callback = transCallback_.lock();
    if (callback == nullptr) {
        DHLOGE("%s: Trans callback is null.", LOG_TAG);
        return;
    }
    callback->OnError(state, "OnProcessorStateNotify");
}
} // namespace DistributedHardware
} // namespace OHOS