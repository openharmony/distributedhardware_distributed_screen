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

#include "image_sink_decoder.h"

#include <chrono>
#include <securec.h>

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_log.h"

namespace OHOS {
namespace DistributedHardware {
int32_t ImageSinkDecoder::ConfigureDecoder(const VideoParam &configParam)
{
    DHLOGI("%s: ConfigureDecoder.", LOG_TAG);
    int32_t ret = InitVideoDecoder(configParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: InitVideoDecoder failed.", LOG_TAG);
        return ret;
    }

    ret = SetDecoderFormat(configParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: SetDecoderFormat failed.", LOG_TAG);
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::ReleaseDecoder()
{
    DHLOGI("%s: ReleaseDecoder.", LOG_TAG);
    if (videoDecoder_ == nullptr) {
        DHLOGE("%s: Decoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = videoDecoder_->Release();
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: ReleaseDecoder failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_RELEASE_FAILED;
    }
    decodeVideoCallback_ = nullptr;
    videoDecoder_ = nullptr;

    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::StartDecoder()
{
    DHLOGI("%s: StartDecoder.", LOG_TAG);
    if (videoDecoder_ == nullptr) {
        DHLOGE("%s: Decoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = videoDecoder_->Prepare();
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Prepare decoder failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_PREPARE_FAILED;
    }

    ret = videoDecoder_->Start();
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Start decoder failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_START_FAILED;
    }
    StartInputThread();

    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::StopDecoder()
{
    DHLOGI("%s: StopDecoder.", LOG_TAG);
    if (videoDecoder_ == nullptr) {
        DHLOGE("%s: Decoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = videoDecoder_->Flush();
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Flush decoder failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_FLUSH_FAILED;
    }

    ret = videoDecoder_->Stop();
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Stop decoder failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_STOP_FAILED;
    }
    StopInputThread();

    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::InitVideoDecoder(const VideoParam &configParam)
{
    DHLOGI("%s: InitVideoDecoder.", LOG_TAG);
    switch (configParam.GetCodecType()) {
        case VIDEO_CODEC_TYPE_VIDEO_H264:
            videoDecoder_ = Media::VideoDecoderFactory::CreateByMime("video/avc");
            break;
        case VIDEO_CODEC_TYPE_VIDEO_H265:
            videoDecoder_ = Media::VideoDecoderFactory::CreateByMime("video/hevc");
            break;
        case VIDEO_CODEC_TYPE_VIDEO_MPEG4:
            videoDecoder_ = Media::VideoDecoderFactory::CreateByMime("video/mp4v-es");
            break;
        default:
            DHLOGE("%s: codecType is invalid!", LOG_TAG);
            videoDecoder_ = nullptr;
    }

    if (videoDecoder_ == nullptr) {
        DHLOGE("%s: Create videoEncode failed.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    decodeVideoCallback_ = std::make_shared<ImageDecoderCallback>(shared_from_this());
    int32_t ret = videoDecoder_->SetCallback(decodeVideoCallback_);
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Set decoder callback failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SET_CALLBACK_FAILED;
    }

    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::SetDecoderFormat(const VideoParam &configParam)
{
    DHLOGI("%s: SetDecoderFormat.", LOG_TAG);
    if (videoDecoder_ == nullptr) {
        DHLOGE("%s: Decoder is null.", LOG_TAG);
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
            DHLOGE("The current codec type does not support decoding.");
            return ERR_DH_SCREEN_TRANS_ILLEGAL_OPERATION;
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
            DHLOGE("The current pixel format does not support decoding.");
            return ERR_DH_SCREEN_TRANS_ILLEGAL_OPERATION;
    }

    imageFormat_.PutLongValue("max_input_size", MAX_YUV420_BUFFER_SIZE);
    imageFormat_.PutIntValue("width", configParam.GetVideoWidth());
    imageFormat_.PutIntValue("height", configParam.GetVideoHeight());
    imageFormat_.PutIntValue("frame_rate", configParam.GetFps());

    int32_t ret = videoDecoder_->Configure(imageFormat_);
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: configure decoder format param failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_CONFIGURE_FAILED;
    }

    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::SetOutputSurface(sptr<Surface> &surface)
{
    DHLOGI("%s: SetOutputSurface.", LOG_TAG);
    if (videoDecoder_ == nullptr) {
        DHLOGE("%s: Decoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = videoDecoder_->SetOutputSurface(surface);
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: SetOutputSurface failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }

    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::InputScreenData(const std::shared_ptr<DataBuffer> &data)
{
    DHLOGD("%s: InputScreenData.", LOG_TAG);
    std::lock_guard<std::mutex> dataLock(dataMutex_);
    while (videoDataQueue_.size() >= DATA_QUEUE_MAX_SIZE) {
        DHLOGE("%s: videoData queue overflow.", LOG_TAG);
        videoDataQueue_.pop();
    }
    videoDataQueue_.push(data);
    decodeCond_.notify_all();

    return DH_SUCCESS;
}

void ImageSinkDecoder::OnError(Media::AVCodecErrorType errorType, int32_t errorCode)
{
    DHLOGI("%s: OnImageDecodeError, errorType(%d), errorCode(%d)", LOG_TAG, errorType, errorCode);
    std::shared_ptr<IImageSinkProcessorListener> listener = imageProcessorListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%s: Listener is null.", LOG_TAG);
        return;
    }
    listener->OnProcessorStateNotify(errorCode);
}

void ImageSinkDecoder::OnInputBufferAvailable(uint32_t index)
{
    DHLOGI("%s: OnDecodeInputBufferAvailable: %u.", LOG_TAG, index);
    std::lock_guard<std::mutex> dataLock(dataMutex_);
    bufferIndexQueue_.push(index);
}

void ImageSinkDecoder::OnOutputBufferAvailable(uint32_t index, Media::AVCodecBufferInfo info,
    Media::AVCodecBufferFlag flag)
{
    DHLOGI("%s: OnDecodeOutputBufferAvailable.", LOG_TAG);
    if (videoDecoder_ == nullptr) {
        DHLOGE("%s: Decoder is null.", LOG_TAG);
        return;
    }

    decoderBufferInfo_ = info;
    int32_t ret = videoDecoder_->ReleaseOutputBuffer(index, true);
    if (ret != Media::MSERR_OK) {
        DHLOGD("%s: ReleaseOutputBuffer failed.", LOG_TAG);
    }
}

void ImageSinkDecoder::OnOutputFormatChanged(const Media::Format &format)
{
    (void) format;
}

int32_t ImageSinkDecoder::StartInputThread()
{
    DHLOGI("%s: StartInputThread.", LOG_TAG);
    isDecoderReady_ = true;
    decodeThread_ = std::thread(&ImageSinkDecoder::DecodeScreenData, this);

    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::StopInputThread()
{
    DHLOGI("%s: StopInputThread.", LOG_TAG);
    isDecoderReady_ = false;
    if (decodeThread_.joinable()) {
        decodeThread_.join();
    }
    std::lock_guard<std::mutex> dataLock(dataMutex_);
    while (!bufferIndexQueue_.empty()) {
        bufferIndexQueue_.pop();
    }
    while (!videoDataQueue_.empty()) {
        videoDataQueue_.pop();
    }

    return DH_SUCCESS;
}

void ImageSinkDecoder::DecodeScreenData()
{
    DHLOGI("%s: DecodeScreenData.", LOG_TAG);
    while (isDecoderReady_) {
        std::shared_ptr<DataBuffer> screenData;
        int32_t bufferIndex = 0;
        {
            std::unique_lock<std::mutex> lock(dataMutex_);
            decodeCond_.wait_for(lock, std::chrono::milliseconds(DECODE_WAIT_MILLISECONDS),
                [this]() { return (!videoDataQueue_.empty() && !bufferIndexQueue_.empty()); });

            if (videoDataQueue_.empty() || bufferIndexQueue_.empty()) {
                DHLOGD("%s: Index queue or data queue is empty.", LOG_TAG);
                continue;
            }
            bufferIndex = bufferIndexQueue_.front();
            bufferIndexQueue_.pop();
            screenData = videoDataQueue_.front();
            videoDataQueue_.pop();
        }

        int32_t ret = ProcessData(screenData, bufferIndex);
        if (ret == ERR_DH_SCREEN_TRANS_NULL_VALUE) {
            return;
        } else if (ret != DH_SUCCESS) {
            continue;
        }
    }
}

int32_t ImageSinkDecoder::ProcessData(const std::shared_ptr<DataBuffer> &screenData, const int32_t bufferIndex)
{
    DHLOGI("%s: ProcessData.", LOG_TAG);
    if (videoDecoder_ == nullptr) {
        DHLOGE("%s: Decoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    auto inputBuffer = videoDecoder_->GetInputBuffer(bufferIndex);
    if (inputBuffer == nullptr) {
        DHLOGE("%s: GetInputBuffer failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }

    int32_t ret = memcpy_s(inputBuffer->GetBase(), inputBuffer->GetSize(), screenData->Data(), screenData->Capacity());
    if (ret != EOK) {
        DHLOGE("%s: Copy data failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }

    DHLOGD("%s: Decode screen data.", LOG_TAG);
    Media::AVCodecBufferInfo bufferInfo;
    bufferInfo.presentationTimeUs = 0;
    bufferInfo.size = static_cast<int32_t>(screenData->Capacity());
    bufferInfo.offset = 0;
    ret = videoDecoder_->QueueInputBuffer(bufferIndex, bufferInfo, Media::AVCODEC_BUFFER_FLAG_NONE);
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: QueueInputBuffer failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }
    return DH_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS
