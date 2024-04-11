/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include <pthread.h>
#include <securec.h>

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_log.h"

namespace OHOS {
namespace DistributedHardware {
constexpr const char* DECODE_THREAD = "DecodeThread";
int32_t ImageSinkDecoder::ConfigureDecoder(const VideoParam &configParam)
{
    DHLOGI("%{public}s: ConfigureDecoder.", DSCREEN_LOG_TAG);
    int32_t ret = InitVideoDecoder(configParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: InitVideoDecoder failed.", DSCREEN_LOG_TAG);
        return ret;
    }

    ret = SetDecoderFormat(configParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: SetDecoderFormat failed.", DSCREEN_LOG_TAG);
        return ret;
    }
    configParam_ = configParam;
    ret = AddSurface();
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Add surface failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        consumerSurface_ = nullptr;
        producerSurface_ = nullptr;
        return ret;
    }
    alignedHeight_ = configParam_.GetVideoHeight();
    if (alignedHeight_ % ALIGNEDBITS != 0) {
        alignedHeight_ = ((alignedHeight_ / ALIGNEDBITS) + 1) * ALIGNEDBITS;
    }
    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::AddSurface()
{
    DHLOGI("%{public}s: AddSurface.", DSCREEN_LOG_TAG);
    consumerSurface_ = IConsumerSurface::Create();
    if (consumerSurface_ == nullptr) {
        DHLOGE("%{public}s: Create consumer surface failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }

    consumerSurface_->SetDefaultUsage(BUFFER_USAGE_CPU_READ);

    sptr<IBufferProducer> producer = consumerSurface_->GetProducer();
    if (producer == nullptr) {
        DHLOGE("%{public}s: Get preducer surface failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }

    producerSurface_ = Surface::CreateSurfaceAsProducer(producer);
    if (producerSurface_ == nullptr) {
        DHLOGE("%{public}s: Create preducer surface failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }

    if (consumerBufferListener_ == nullptr) {
        consumerBufferListener_ = new ConsumBufferListener(shared_from_this());
    }
    consumerSurface_->RegisterConsumerListener(consumerBufferListener_);
    lastFrameSize_ = static_cast<int32_t>(configParam_.GetVideoWidth() *
        configParam_.GetVideoHeight() * RGB_CHROMA / TWO);
    lastFrame_ = new uint8_t[lastFrameSize_];
    return DH_SUCCESS;
}

uint8_t *ImageSinkDecoder::GetLastFrame()
{
    return lastFrame_;
}

sptr<SurfaceBuffer> ImageSinkDecoder::GetWinSurfaceBuffer()
{
    DHLOGI("%{public}s: GetWinSurfaceBuffer.", DSCREEN_LOG_TAG);
    sptr<SurfaceBuffer> windowSurfaceBuffer = nullptr;
    int32_t releaseFence = -1;
    OHOS::BufferRequestConfig requestConfig = {
        .width = static_cast<int32_t>(configParam_.GetVideoWidth()),
        .height = static_cast<int32_t>(configParam_.GetVideoHeight()),
        .strideAlignment = STRIDE_ALIGNMENT,
        .format = GRAPHIC_PIXEL_FMT_YCBCR_420_SP,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
    };
    SurfaceError surfaceErr = windowSurface_->RequestBuffer(windowSurfaceBuffer, releaseFence, requestConfig);
    if (surfaceErr != SURFACE_ERROR_OK || windowSurfaceBuffer == nullptr) {
        DHLOGE("%{public}s: windowSurface_ request buffer failed, buffer is nullptr.", DSCREEN_LOG_TAG);
        windowSurface_->CancelBuffer(windowSurfaceBuffer);
    }
    return windowSurfaceBuffer;
}

void ImageSinkDecoder::NormalProcess(sptr<SurfaceBuffer> surfaceBuffer, sptr<SurfaceBuffer> windowSurfaceBuffer)
{
    DHLOGI("%{public}s: NormalProcess.", DSCREEN_LOG_TAG);
    auto surfaceAddr = static_cast<uint8_t*>(surfaceBuffer->GetVirAddr());
    auto windowSurfaceAddr = static_cast<uint8_t*>(windowSurfaceBuffer->GetVirAddr());
    int32_t sizeData = lastFrameSize_;
    uint32_t size = windowSurfaceBuffer->GetSize();
    int32_t ret = memcpy_s(windowSurfaceAddr, size, surfaceAddr, sizeData);
    if (ret != EOK) {
        DHLOGE("%{public}s: surfaceBuffer memcpy run failed.", DSCREEN_LOG_TAG);
        windowSurface_->CancelBuffer(windowSurfaceBuffer);
    }
}

void ImageSinkDecoder::OffsetProcess(sptr<SurfaceBuffer> surfaceBuffer, sptr<SurfaceBuffer> windowSurfaceBuffer)
{
    DHLOGI("%{public}s: OffsetProcess.", DSCREEN_LOG_TAG);
    auto surfaceAddr = static_cast<uint8_t*>(surfaceBuffer->GetVirAddr());
    auto windowSurfaceAddr = static_cast<uint8_t*>(windowSurfaceBuffer->GetVirAddr());
    uint32_t size = windowSurfaceBuffer->GetSize();
    uint32_t srcDataOffset = 0;
    uint32_t dstDataOffset = 0;
    uint32_t alignedWidth = static_cast<uint32_t>(surfaceBuffer->GetStride());
    uint32_t chromaOffset = alignedWidth * configParam_.GetVideoHeight();
    for (unsigned int yh = 0 ; yh < configParam_.GetVideoHeight() ; yh++) {
        int32_t ret = memcpy_s(windowSurfaceAddr + dstDataOffset, chromaOffset - dstDataOffset,
            surfaceAddr + srcDataOffset, alignedWidth);
        if (ret != EOK) {
            DHLOGE("%{public}s: surfaceBuffer memcpy_s run failed.", DSCREEN_LOG_TAG);
            windowSurface_->CancelBuffer(windowSurfaceBuffer);
            return;
        }
        dstDataOffset += alignedWidth;
        srcDataOffset += alignedWidth;
    }
    dstDataOffset = chromaOffset;
    srcDataOffset = alignedWidth * alignedHeight_;
    for (unsigned int uvh = 0 ; uvh < configParam_.GetVideoHeight() / TWO; uvh++) {
        int32_t ret = memcpy_s(windowSurfaceAddr + dstDataOffset, size - dstDataOffset,
            surfaceAddr + srcDataOffset, alignedWidth);
        if (ret != EOK) {
            DHLOGE("%{public}s: surfaceBuffer memcpy_s run failed.", DSCREEN_LOG_TAG);
            windowSurface_->CancelBuffer(windowSurfaceBuffer);
            return;
        }
        dstDataOffset += alignedWidth;
        srcDataOffset += alignedWidth;
    }
}

void ImageSinkDecoder::ConsumeSurface()
{
    DHLOGI("%{public}s: ConsumeSurface.", DSCREEN_LOG_TAG);
    std::unique_lock<std::mutex> bufLock(decodeMutex_);
    if (consumerSurface_ == nullptr) {
        DHLOGE("%{public}s: consumerSurface_ is nullptr.", DSCREEN_LOG_TAG);
        return;
    }
    sptr<SurfaceBuffer> surfaceBuffer = nullptr;
    int32_t fence = -1;
    int64_t timestamp = 0;
    OHOS::Rect damage = {0, 0, 0, 0};
    SurfaceError surfaceErr = consumerSurface_->AcquireBuffer(surfaceBuffer, fence, timestamp, damage);
    if (surfaceErr != SURFACE_ERROR_OK) {
        return;
    }
    sptr<OHOS::SurfaceBuffer> windowSurfaceBuffer = GetWinSurfaceBuffer();
    if (windowSurfaceBuffer == nullptr) {
        consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
        return;
    }

    if (lastFrameSize_ != surfaceBuffer->GetStride() * surfaceBuffer->GetHeight() * THREE / TWO) {
        if (lastFrame_ != nullptr) {
            delete [] lastFrame_;
        }
        lastFrameSize_ = surfaceBuffer->GetStride() * surfaceBuffer->GetHeight() * THREE / TWO;
        lastFrame_ = new uint8_t[lastFrameSize_];
    }

    if (static_cast<uint32_t>(alignedHeight_) == configParam_.GetVideoHeight()) {
        NormalProcess(surfaceBuffer, windowSurfaceBuffer);
    } else {
        OffsetProcess(surfaceBuffer, windowSurfaceBuffer);
    }
    auto windowSurfaceAddr = static_cast<uint8_t*>(windowSurfaceBuffer->GetVirAddr());
    BufferFlushConfig flushConfig = { {0, 0, windowSurfaceBuffer->GetWidth(), windowSurfaceBuffer->GetHeight()}, 0};
    int32_t ret = memcpy_s(lastFrame_, lastFrameSize_, windowSurfaceAddr, lastFrameSize_);
    if (ret != EOK) {
        DHLOGE("%{public}s: surfaceBuffer memcpy_s run failed.", DSCREEN_LOG_TAG);
        consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
        windowSurface_->CancelBuffer(windowSurfaceBuffer);
        return;
    }
    DHLOGI("%{public}s: ConsumeSurface success, send NV12 to window.", DSCREEN_LOG_TAG);
    surfaceErr = windowSurface_->FlushBuffer(windowSurfaceBuffer, -1, flushConfig);
    if (surfaceErr != SURFACE_ERROR_OK) {
        DHLOGE("%{public}s: windowSurface_ flush buffer failed.", DSCREEN_LOG_TAG);
        consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
        windowSurface_->CancelBuffer(windowSurfaceBuffer);
        return;
    }
    consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
}

void ConsumBufferListener::OnBufferAvailable()
{
    DHLOGI("%{public}s: OnBufferAvailable, receive NV12 data from decoder.", DSCREEN_LOG_TAG);
    decoder_->ConsumeSurface();
}

int32_t ImageSinkDecoder::ReleaseDecoder()
{
    DHLOGI("%{public}s: ReleaseDecoder.", DSCREEN_LOG_TAG);
    if (videoDecoder_ == nullptr) {
        DHLOGE("%{public}s: Decoder is null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    if (lastFrame_ != nullptr) {
        delete [] lastFrame_;
    }
    int32_t ret = videoDecoder_->Release();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: ReleaseDecoder failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_RELEASE_FAILED;
    }
    decodeVideoCallback_ = nullptr;
    videoDecoder_ = nullptr;

    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::StartDecoder()
{
    DHLOGI("%{public}s: StartDecoder.", DSCREEN_LOG_TAG);
    if (videoDecoder_ == nullptr) {
        DHLOGE("%{public}s: Decoder is null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = videoDecoder_->Prepare();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: Prepare decoder failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_PREPARE_FAILED;
    }

    ret = videoDecoder_->Start();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: Start decoder failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_START_FAILED;
    }
    StartInputThread();

    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::StopDecoder()
{
    DHLOGI("%{public}s: StopDecoder.", DSCREEN_LOG_TAG);
    if (videoDecoder_ == nullptr) {
        DHLOGE("%{public}s: Decoder is null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = videoDecoder_->Flush();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: Flush decoder failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_FLUSH_FAILED;
    }

    ret = videoDecoder_->Stop();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: Stop decoder failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_STOP_FAILED;
    }
    StopInputThread();

    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::InitVideoDecoder(const VideoParam &configParam)
{
    DHLOGI("%{public}s: InitVideoDecoder.", DSCREEN_LOG_TAG);
    switch (configParam.GetCodecType()) {
        case VIDEO_CODEC_TYPE_VIDEO_H264:
            videoDecoder_ = MediaAVCodec::VideoDecoderFactory::CreateByMime(
                std::string(MediaAVCodec::CodecMimeType::VIDEO_AVC));
            break;
        case VIDEO_CODEC_TYPE_VIDEO_H265:
            videoDecoder_ = MediaAVCodec::VideoDecoderFactory::CreateByMime(
                std::string(MediaAVCodec::CodecMimeType::VIDEO_HEVC));
            break;
        default:
            DHLOGE("%{public}s: codecType is invalid!", DSCREEN_LOG_TAG);
            videoDecoder_ = nullptr;
    }

    if (videoDecoder_ == nullptr) {
        DHLOGE("%{public}s: Create videoEncoder failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    decodeVideoCallback_ = std::make_shared<ImageDecoderCallback>(shared_from_this());
    int32_t ret = videoDecoder_->SetCallback(decodeVideoCallback_);
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: Set decoder callback failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SET_CALLBACK_FAILED;
    }

    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::SetDecoderFormat(const VideoParam &configParam)
{
    DHLOGI("%{public}s: SetDecoderFormat.", DSCREEN_LOG_TAG);
    if (videoDecoder_ == nullptr) {
        DHLOGE("%{public}s: Decoder is null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    switch (configParam.GetCodecType()) {
        case VIDEO_CODEC_TYPE_VIDEO_H264:
            imageFormat_.PutStringValue("codec_mime", MediaAVCodec::CodecMimeType::VIDEO_AVC);
            break;
        case VIDEO_CODEC_TYPE_VIDEO_H265:
            imageFormat_.PutStringValue("codec_mime", MediaAVCodec::CodecMimeType::VIDEO_HEVC);
            break;
        default:
            DHLOGE("The current codec type does not support decoding.");
            return ERR_DH_SCREEN_TRANS_ILLEGAL_OPERATION;
    }

    imageFormat_.PutIntValue("pixel_format", static_cast<int32_t>(MediaAVCodec::VideoPixelFormat::NV12));
    imageFormat_.PutLongValue("max_input_size", MAX_YUV420_BUFFER_SIZE);
    imageFormat_.PutIntValue("width", configParam.GetVideoWidth());
    imageFormat_.PutIntValue("height", configParam.GetVideoHeight());
    imageFormat_.PutDoubleValue("frame_rate", configParam.GetFps());

    int32_t ret = videoDecoder_->Configure(imageFormat_);
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: configure decoder format param failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_CONFIGURE_FAILED;
    }

    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::SetOutputSurface(sptr<Surface> &surface)
{
    DHLOGI("%{public}s: SetOutputSurface.", DSCREEN_LOG_TAG);
    if (videoDecoder_ == nullptr || surface == nullptr) {
        DHLOGE("%{public}s: Decoder or surface is null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    windowSurface_ = surface;
    if (consumerSurface_ == nullptr || producerSurface_ == nullptr || !configParam_.GetPartialRefreshFlag()) {
        int32_t ret = videoDecoder_->SetOutputSurface(surface);
        if (ret != MediaAVCodec::AVCS_ERR_OK) {
            DHLOGE("%{public}s: SetOutputSurface failed.", DSCREEN_LOG_TAG);
            return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
        }
    } else {
        int32_t ret = videoDecoder_->SetOutputSurface(producerSurface_);
        if (ret != MediaAVCodec::AVCS_ERR_OK) {
            DHLOGE("%{public}s: SetOutputSurface failed.", DSCREEN_LOG_TAG);
            return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
        }
    }
    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::InputScreenData(const std::shared_ptr<DataBuffer> &data)
{
    if (data == nullptr) {
        DHLOGE("%{public}s: InputScreenData failed, data is nullptr.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }
    DHLOGD("%{public}s: InputScreenData.", DSCREEN_LOG_TAG);
    std::lock_guard<std::mutex> dataLock(dataMutex_);
    while (videoDataQueue_.size() >= DATA_QUEUE_MAX_SIZE) {
        DHLOGE("%{public}s: videoData queue overflow.", DSCREEN_LOG_TAG);
        videoDataQueue_.pop();
    }
    videoDataQueue_.push(data);
    decodeCond_.notify_all();

    return DH_SUCCESS;
}

void ImageSinkDecoder::OnError(MediaAVCodec::AVCodecErrorType errorType, int32_t errorCode)
{
    DHLOGI("%{public}s: OnImageDecodeError, errorType:%{public}" PRId32 ", errorCode:%{public}" PRId32,
        DSCREEN_LOG_TAG, errorType, errorCode);
    std::shared_ptr<IImageSinkProcessorListener> listener = imageProcessorListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%{public}s: Listener is null.", DSCREEN_LOG_TAG);
        return;
    }
    listener->OnProcessorStateNotify(errorCode);
}

void ImageSinkDecoder::OnInputBufferAvailable(uint32_t index, std::shared_ptr<Media::AVSharedMemory> buffer)
{
    DHLOGI("%{public}s: OnDecodeInputBufferAvailable: %{public}" PRIu32, DSCREEN_LOG_TAG, index);
    std::lock_guard<std::mutex> dataLock(dataMutex_);
    availableInputIndexsQueue_.push(index);
    availableInputBufferQueue_.push(buffer);
}

void ImageSinkDecoder::OnOutputBufferAvailable(uint32_t index, MediaAVCodec::AVCodecBufferInfo info,
    MediaAVCodec::AVCodecBufferFlag flag, std::shared_ptr<Media::AVSharedMemory> buffer)
{
    DHLOGI("%{public}s: OnDecodeOutputBufferAvailable.", DSCREEN_LOG_TAG);
    if (videoDecoder_ == nullptr) {
        DHLOGE("%{public}s: Decoder is null.", DSCREEN_LOG_TAG);
        return;
    }

    decoderBufferInfo_ = info;
    int32_t ret = videoDecoder_->ReleaseOutputBuffer(index, true);
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGD("%{public}s: ReleaseOutputBuffer failed.", DSCREEN_LOG_TAG);
    }
}

void ImageSinkDecoder::OnOutputFormatChanged(const Media::Format &format)
{
    (void) format;
}

int32_t ImageSinkDecoder::StartInputThread()
{
    DHLOGI("%{public}s: StartInputThread.", DSCREEN_LOG_TAG);
    isDecoderReady_ = true;
    decodeThread_ = std::thread(&ImageSinkDecoder::DecodeScreenData, this);
    return DH_SUCCESS;
}

int32_t ImageSinkDecoder::StopInputThread()
{
    DHLOGI("%{public}s: StopInputThread.", DSCREEN_LOG_TAG);
    isDecoderReady_ = false;
    if (decodeThread_.joinable()) {
        decodeThread_.join();
    }
    std::lock_guard<std::mutex> dataLock(dataMutex_);
    std::queue<std::shared_ptr<DataBuffer>>().swap(videoDataQueue_);
    std::queue<int32_t>().swap(availableInputIndexsQueue_);
    std::queue<std::shared_ptr<Media::AVSharedMemory>>().swap(availableInputBufferQueue_);

    return DH_SUCCESS;
}

void ImageSinkDecoder::DecodeScreenData()
{
    DHLOGI("%{public}s: DecodeScreenData.", DSCREEN_LOG_TAG);
    int32_t ret = pthread_setname_np(pthread_self(), DECODE_THREAD);
    if (ret != DH_SUCCESS) {
        DHLOGE("ImageSinkDecoder set thread name failed, ret %{public}" PRId32, ret);
    }
    while (isDecoderReady_) {
        std::shared_ptr<DataBuffer> screenData;
        int32_t bufferIndex = 0;
        {
            std::unique_lock<std::mutex> lock(dataMutex_);
            decodeCond_.wait_for(lock, std::chrono::milliseconds(DECODE_WAIT_MILLISECONDS), [this]() {
                return (!videoDataQueue_.empty() && !availableInputIndexsQueue_.empty() &&
                        !availableInputBufferQueue_.empty());
            });

            if (videoDataQueue_.empty() || availableInputIndexsQueue_.empty() || availableInputBufferQueue_.empty()) {
                DHLOGD("%{public}s: Index queue or data queue or buffer queue is empty.", DSCREEN_LOG_TAG);
                continue;
            }
            bufferIndex = availableInputIndexsQueue_.front();
            availableInputIndexsQueue_.pop();
            screenData = videoDataQueue_.front();
            videoDataQueue_.pop();
        }

        ret = ProcessData(screenData, bufferIndex);
        if (ret == ERR_DH_SCREEN_TRANS_NULL_VALUE) {
            return;
        } else if (ret != DH_SUCCESS) {
            continue;
        }
    }
}

int32_t ImageSinkDecoder::ProcessData(const std::shared_ptr<DataBuffer> &screenData, const int32_t bufferIndex)
{
    DHLOGI("%{public}s: ProcessData.", DSCREEN_LOG_TAG);
    if (videoDecoder_ == nullptr || screenData == nullptr) {
        DHLOGE("%{public}s: Decoder or screenData is null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    if (availableInputBufferQueue_.empty()) {
        DHLOGD("%{public}s: input buffer queue is empty.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }

    std::shared_ptr<Media::AVSharedMemory> inputBuffer = availableInputBufferQueue_.front();
    if (inputBuffer == nullptr) {
        DHLOGE("%{public}s: Failed to obtain the input shared memory corresponding to the [%{public}d] index.",
            DSCREEN_LOG_TAG, bufferIndex);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }

    int32_t ret = memcpy_s(inputBuffer->GetBase(), inputBuffer->GetSize(), screenData->Data(), screenData->Capacity());
    if (ret != EOK) {
        DHLOGE("%{public}s: Copy data failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }

    DHLOGD("%{public}s: Decode screen data. send data to H264 decoder", DSCREEN_LOG_TAG);
    MediaAVCodec::AVCodecBufferInfo bufferInfo;
    bufferInfo.presentationTimeUs = 0;
    bufferInfo.size = static_cast<int32_t>(screenData->Capacity());
    bufferInfo.offset = 0;
    ret = videoDecoder_->QueueInputBuffer(bufferIndex, bufferInfo, MediaAVCodec::AVCODEC_BUFFER_FLAG_NONE);
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: QueueInputBuffer failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }
    availableInputBufferQueue_.pop();
    return DH_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS
