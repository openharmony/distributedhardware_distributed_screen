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

#ifndef OHOS_IMAGE_SINK_DECODER_H
#define OHOS_IMAGE_SINK_DECODER_H

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <cstddef>

#include "buffer/avsharedmemory.h"
#include "avcodec_common.h"
#include "avcodec_video_decoder.h"
#include "avcodec_errors.h"
#include "meta/format.h"
#include "surface.h"
#include "iconsumer_surface.h"

#include "data_buffer.h"
#include "iimage_sink_processor_listener.h"
#include "image_decoder_callback.h"
#include "video_param.h"

namespace OHOS {
namespace DistributedHardware {
class ConsumBufferListener : public IBufferConsumerListener {
public:
    ConsumBufferListener(const std::shared_ptr<ImageSinkDecoder> decoder) : decoder_(decoder) {};
    ~ConsumBufferListener() = default;
    void OnBufferAvailable() override;
private:
    static const constexpr char *DSCREEN_LOG_TAG = "ConsumBufferListener";
    std::shared_ptr<ImageSinkDecoder> decoder_;
};
class ImageSinkDecoder : public std::enable_shared_from_this<ImageSinkDecoder> {
public:
    explicit ImageSinkDecoder(const std::shared_ptr<IImageSinkProcessorListener> &imageListener)
        : imageProcessorListener_(imageListener) {};
    ~ImageSinkDecoder() = default;

    int32_t ConfigureDecoder(const VideoParam &configParam);
    int32_t ReleaseDecoder();
    int32_t StartDecoder();
    int32_t StopDecoder();
    int32_t SetOutputSurface(sptr<Surface> &surface);
    int32_t InputScreenData(const std::shared_ptr<DataBuffer> &data);
    int32_t AddSurface();
    void ConsumeSurface();
    uint8_t *GetLastFrame();
    sptr<SurfaceBuffer> GetWinSurfaceBuffer();
    void NormalProcess(sptr<SurfaceBuffer> surfaceBuffer, sptr<SurfaceBuffer> windowSurfaceBuffer);
    void OffsetProcess(sptr<SurfaceBuffer> surfaceBuffer, sptr<SurfaceBuffer> windowSurfaceBuffer);
    void OnError(MediaAVCodec::AVCodecErrorType errorType, int32_t errorCode);
    void OnInputBufferAvailable(uint32_t index, std::shared_ptr<Media::AVSharedMemory> buffer);
    void OnOutputBufferAvailable(uint32_t index, MediaAVCodec::AVCodecBufferInfo info,
                                 MediaAVCodec::AVCodecBufferFlag flag, std::shared_ptr<Media::AVSharedMemory> buffer);
    void OnOutputFormatChanged(const Media::Format &format);

private:
    int32_t SetDecoderFormat(const VideoParam &configParam);
    int32_t InitVideoDecoder(const VideoParam &configParam);
    int32_t StartInputThread();
    int32_t StopInputThread();
    void DecodeScreenData();
    int32_t ProcessData(const std::shared_ptr<DataBuffer> &screenData, const int32_t bufferIndex);

private:
    static const constexpr char *DSCREEN_LOG_TAG = "ImageSinkDecoder";
    VideoParam configParam_;
    std::mutex dataMutex_;
    std::mutex decodeMutex_;
    std::thread decodeThread_;
    std::condition_variable decodeCond_;

    uint8_t *lastFrame_ = nullptr;
    int32_t lastFrameSize_ = 0;
    Media::Format imageFormat_;
    Media::Format decodeOutputFormat_;
    MediaAVCodec::AVCodecBufferInfo decoderBufferInfo_;

    bool isDecoderReady_ = false;
    uint32_t alignedHeight_ = 0;
    sptr<IConsumerSurface> consumerSurface_;
    sptr<Surface> producerSurface_;
    sptr<Surface> windowSurface_;
    sptr<IBufferConsumerListener> consumerBufferListener_;
    std::queue<std::shared_ptr<DataBuffer>> videoDataQueue_;
    std::queue<int32_t> availableInputIndexsQueue_;
    std::queue<std::shared_ptr<Media::AVSharedMemory>> availableInputBufferQueue_;
    std::shared_ptr<MediaAVCodec::AVCodecVideoDecoder> videoDecoder_;
    std::shared_ptr<MediaAVCodec::AVCodecCallback> decodeVideoCallback_;
    std::weak_ptr<IImageSinkProcessorListener> imageProcessorListener_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif