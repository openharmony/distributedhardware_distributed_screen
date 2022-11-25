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

#include "image_source_encoder.h"

#include <securec.h>
#include <stddef.h>

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_log.h"

namespace OHOS {
namespace DistributedHardware {
int32_t ImageSourceEncoder::ConfigureEncoder(const VideoParam &configParam)
{
    DHLOGI("%s: ConfigureEncoder.", LOG_TAG);
    int32_t ret = InitVideoEncoder(configParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Init encoder failed ret:%d.", LOG_TAG, ret);
        return ret;
    }

    ret = SetEncoderFormat(configParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Set encoder format failed ret:%d.", LOG_TAG, ret);
        return ret;
    }

    videoSurface_ = videoEncoder_->CreateInputSurface();
    if (videoSurface_ == nullptr) {
        DHLOGE("%s: Create encoder surface failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }

    return DH_SUCCESS;
}

int32_t ImageSourceEncoder::ReleaseEncoder()
{
    DHLOGI("%s: ReleaseEncoder.", LOG_TAG);
    if (videoEncoder_ == nullptr) {
        DHLOGE("%s: Encoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = videoEncoder_->Release();
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Release encoder failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_RELEASE_FAILED;
    }
    encodeVideoCallback_ = nullptr;
    videoEncoder_ = nullptr;

    return DH_SUCCESS;
}

int32_t ImageSourceEncoder::StartEncoder()
{
    DHLOGI("%s: StartEncoder.", LOG_TAG);
    if (videoEncoder_ == nullptr) {
        DHLOGE("%s: Encoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = videoEncoder_->Prepare();
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Prepare encoder failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_PREPARE_FAILED;
    }

    ret = videoEncoder_->Start();
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Start encoder failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_START_FAILED;
    }

    return DH_SUCCESS;
}

int32_t ImageSourceEncoder::StopEncoder()
{
    DHLOGI("%s: StopEncoder.", LOG_TAG);
    if (videoEncoder_ == nullptr) {
        DHLOGE("%s: Encoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = videoEncoder_->Flush();
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Flush encoder failed.", LOG_TAG);
    }

    ret = videoEncoder_->Stop();
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Stop encoder failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_STOP_FAILED;
    }

    return DH_SUCCESS;
}

int32_t ImageSourceEncoder::InitVideoEncoder(const VideoParam &configParam)
{
    DHLOGI("%s: InitVideoEncoder.", LOG_TAG);
    switch (configParam.GetCodecType()) {
        case VIDEO_CODEC_TYPE_VIDEO_H264:
            videoEncoder_ = Media::VideoEncoderFactory::CreateByMime("video/avc");
            break;
        case VIDEO_CODEC_TYPE_VIDEO_H265:
            videoEncoder_ = Media::VideoEncoderFactory::CreateByMime("video/hevc");
            break;
        case VIDEO_CODEC_TYPE_VIDEO_MPEG4:
            videoEncoder_ = Media::VideoEncoderFactory::CreateByMime("video/mp4v-es");
            break;
        default:
            DHLOGE("%s: codecType is invalid!", LOG_TAG);
            videoEncoder_ = nullptr;
    }

    if (videoEncoder_ == nullptr) {
        DHLOGE("%s: Create videoEncoder failed.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_CREATE_CODEC_FAILED;
    }

    encodeVideoCallback_ = std::make_shared<ImageEncoderCallback>(shared_from_this());
    int32_t ret = videoEncoder_->SetCallback(encodeVideoCallback_);
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Set codec callback failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SET_CALLBACK_FAILED;
    }

    return DH_SUCCESS;
}

int32_t ImageSourceEncoder::SetEncoderFormat(const VideoParam &configParam)
{
    DHLOGI("%s: SetEncoderFormat.", LOG_TAG);
    if (videoEncoder_ == nullptr) {
        DHLOGE("%s: Encoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    switch (configParam.GetCodecType()) {
        case VIDEO_CODEC_TYPE_VIDEO_H264:
            imageFormat_.PutStringValue("codec_mime", "video/avc");
            break;
        case VIDEO_CODEC_TYPE_VIDEO_H265:
            imageFormat_.PutStringValue("codec_mime", "video/hevc");
            break;
        case VIDEO_CODEC_TYPE_VIDEO_MPEG4:
            imageFormat_.PutStringValue("codec_mime", "video/mp4v-es");
            break;
        default:
            DHLOGE("%s: Codec type is invalid.", LOG_TAG);
            return ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM;
    }
    switch (configParam.GetVideoFormat()) {
        case VIDEO_DATA_FORMAT_YUVI420:
            imageFormat_.PutIntValue("pixel_format", Media::VideoPixelFormat::YUVI420);
            break;
        case VIDEO_DATA_FORMAT_NV12:
            imageFormat_.PutIntValue("pixel_format", Media::VideoPixelFormat::NV12);
            break;
        case VIDEO_DATA_FORMAT_NV21:
            imageFormat_.PutIntValue("pixel_format", Media::VideoPixelFormat::NV21);
            break;
        case VIDEO_DATA_FORMAT_RGBA8888:
            imageFormat_.PutIntValue("pixel_format", Media::VideoPixelFormat::RGBA);
            break;
        default:
            DHLOGE("%s: Video format is invalid.", LOG_TAG);
            return ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM;
    }
    imageFormat_.PutLongValue("max_input_size", MAX_YUV420_BUFFER_SIZE);
    imageFormat_.PutIntValue("width", configParam.GetVideoWidth());
    imageFormat_.PutIntValue("height", configParam.GetVideoHeight());
    imageFormat_.PutIntValue("frame_rate", configParam.GetFps());

    int32_t ret = videoEncoder_->Configure(imageFormat_);
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Configure encoder failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_CONFIGURE_FAILED;
    }
    return DH_SUCCESS;
}

void ImageSourceEncoder::OnError(Media::AVCodecErrorType errorType, int32_t errorCode)
{
    DHLOGI("%s: Encoder error, errorType(%d), errorCode(%d)", LOG_TAG, errorType, errorCode);
    std::shared_ptr<IImageSourceProcessorListener> listener = imageProcessorListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%s: Processor listener is null", LOG_TAG);
        return;
    }
    listener->OnProcessorStateNotify(errorCode);
}

void ImageSourceEncoder::OnOutputBufferAvailable(uint32_t index, Media::AVCodecBufferInfo info,
    Media::AVCodecBufferFlag flag)
{
    DHLOGD("%s: OnOutputBufferAvailable.", LOG_TAG);
    std::shared_ptr<IImageSourceProcessorListener> listener = imageProcessorListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%s: Processor listener is null", LOG_TAG);
        return;
    }
    if (videoEncoder_ == nullptr) {
        DHLOGE("%s: Encoder is null.", LOG_TAG);
        return;
    }

    encoderBufferInfo_ = info;
    videoSharedMemory_ = videoEncoder_->GetOutputBuffer(index);
    if (videoSharedMemory_ == nullptr) {
        DHLOGE("%s: GetOutputBuffer failed.", LOG_TAG);
        return;
    }

    size_t dataSize = static_cast<size_t>(info.size);
    if (dataSize == 0 || dataSize > DATA_BUFFER_MAX_SIZE) {
        DHLOGE("%s:OnOutputBufferAvailable params invalid, size: %d.", LOG_TAG, dataSize);
        return;
    }
    auto dataBuf = std::make_shared<DataBuffer>(dataSize);
    int32_t ret = memcpy_s(dataBuf->Data(), dataBuf->Capacity(), videoSharedMemory_->GetBase(), dataSize);
    if (ret != EOK) {
        DHLOGE("%s: Copy data failed.", LOG_TAG);
        return;
    }
    listener->OnImageProcessDone(dataBuf);

    ret = videoEncoder_->ReleaseOutputBuffer(index);
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: videoEncoder ReleaseOutputBuffer failed.", LOG_TAG);
    }
}

void ImageSourceEncoder::OnInputBufferAvailable(uint32_t index)
{
    (void) index;
}

void ImageSourceEncoder::OnOutputFormatChanged(const Media::Format &format)
{
    (void) format;
}
} // namespace DistributedHardware
} // namespace OHOS