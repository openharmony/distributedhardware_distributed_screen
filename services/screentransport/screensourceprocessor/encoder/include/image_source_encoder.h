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

#ifndef OHOS_IMAGE_SOURCE_ENCODER_H
#define OHOS_IMAGE_SOURCE_ENCODER_H

#include <memory>

#include "avsharedmemory.h"
#include "avcodec_video_encoder.h"
#include "avcodec_common.h"
#include "media_errors.h"
#include "format.h"
#include "surface.h"

#include "iimage_source_processor_listener.h"
#include "image_encoder_callback.h"
#include "video_param.h"

namespace OHOS {
namespace DistributedHardware {
class ImageSourceEncoder : public std::enable_shared_from_this<ImageSourceEncoder> {
public:
    explicit ImageSourceEncoder(const std::shared_ptr<IImageSourceProcessorListener> &imageListener)
        : imageProcessorListener_(imageListener) {};
    ~ImageSourceEncoder() = default;

    int32_t ConfigureEncoder(const VideoParam &cofigParam);
    int32_t ReleaseEncoder();
    int32_t StartEncoder();
    int32_t StopEncoder();
    sptr<Surface> &GetInputSurface()
    {
        return videoSurface_;
    }

    void OnError(Media::AVCodecErrorType errorType, int32_t errorCode);
    void OnOutputBufferAvailable(uint32_t index, Media::AVCodecBufferInfo info, Media::AVCodecBufferFlag flag);
    void OnInputBufferAvailable(uint32_t index);
    void OnOutputFormatChanged(const Media::Format &format);

private:
    int32_t InitVideoEncoder(const VideoParam &configParam);
    int32_t SetEncoderFormat(const VideoParam &cofigParam);

private:
    static const constexpr char *LOG_TAG = "ImageSourceEncoder";

    Media::Format imageFormat_;
    Media::AVCodecBufferInfo encoderBufferInfo_;

    sptr<Surface> videoSurface_;
    std::shared_ptr<Media::AVCodecVideoEncoder> videoEncoder_;
    std::shared_ptr<Media::AVSharedMemory> videoSharedMemory_;
    std::shared_ptr<Media::AVCodecCallback> encodeVideoCallback_;
    std::weak_ptr<IImageSourceProcessorListener> imageProcessorListener_;
    const size_t DATA_BUFFER_MAX_SIZE = 10 * 1024 * 1024;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif