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

#ifndef OHOS_IMAGE_SINK_DECODER_H
#define OHOS_IMAGE_SINK_DECODER_H

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <stddef.h>

#include "avsharedmemory.h"
#include "avcodec_common.h"
#include "avcodec_video_decoder.h"
#include "media_errors.h"
#include "format.h"
#include "surface.h"

#include "data_buffer.h"
#include "iimage_sink_processor_listener.h"
#include "image_decoder_callback.h"
#include "video_param.h"

namespace OHOS {
namespace DistributedHardware {
class ImageSinkDecoder : public std::enable_shared_from_this<ImageSinkDecoder> {
public:
    ImageSinkDecoder(const std::shared_ptr<IImageSinkProcessorListener> &imageListener)
        : imageProcessorListener_(imageListener) {};
    ~ImageSinkDecoder() = default;

    int32_t ConfigureDecoder(const VideoParam &configParam);
    int32_t ReleaseDecoder();
    int32_t StartDecoder();
    int32_t StopDecoder();
    int32_t SetOutputSurface(sptr<Surface> &surface);
    int32_t InputScreenData(const std::shared_ptr<DataBuffer> &data);

    void OnError(Media::AVCodecErrorType errorType, int32_t errorCode);
    void OnInputBufferAvailable(uint32_t index);
    void OnOutputBufferAvailable(uint32_t index, Media::AVCodecBufferInfo info, Media::AVCodecBufferFlag flag);
    void OnOutputFormatChanged(const Media::Format &format);

private:
    int32_t SetDecoderFormat(const VideoParam &configParam);
    int32_t InitVideoDecoder(const VideoParam &configParam);
    int32_t StartInputThread();
    int32_t StopInputThread();
    void DecodeScreenData();
    int32_t ProcessData(const std::shared_ptr<DataBuffer> &screenData, const int32_t bufferIndex);

private:
    static const constexpr char *LOG_TAG = "ImageSinkDecoder";
    static constexpr uint32_t DECODE_WAIT_MILLISECONDS = 5000;
    static constexpr size_t DATA_QUEUE_MAX_SIZE = 1000;

    std::mutex dataMutex_;
    std::mutex decodeMutex_;
    std::thread decodeThread_;
    std::condition_variable decodeCond_;

    Media::Format imageFormat_;
    Media::AVCodecBufferInfo decoderBufferInfo_;

    bool isDecoderReady_ = false;
    std::queue<std::shared_ptr<DataBuffer>> videoDataQueue_;
    std::queue<int32_t> bufferIndexQueue_;
    std::shared_ptr<Media::AVCodecVideoDecoder> videoDecoder_;
    std::shared_ptr<Media::AVCodecCallback> decodeVideoCallback_;
    std::weak_ptr<IImageSinkProcessorListener> imageProcessorListener_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif