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

#ifndef IMAGE_SINK_PROCESSOR_H
#define IMAGE_SINK_PROCESSOR_H

#include "iimage_sink_processor.h"
#include "image_sink_decoder.h"
#include "video_param.h"
#include "data_buffer.h"
#include "jpeg_image_processor.h"

namespace OHOS {
namespace DistributedHardware {
class ImageSinkProcessor : public IImageSinkProcessor {
public:
    ImageSinkProcessor() = default;
    ~ImageSinkProcessor() override = default;

    int32_t ConfigureImageProcessor(const VideoParam &localParam, const VideoParam &remoteParam,
        const std::shared_ptr<IImageSinkProcessorListener> &imageListener) override;
    int32_t ReleaseImageProcessor() override;
    int32_t StartImageProcessor() override;
    int32_t StopImageProcessor() override;
    int32_t SetImageSurface(sptr<Surface> &surface) override;
    int32_t ProcessImage(const std::shared_ptr<DataBuffer> &data) override;

private:
    static const constexpr char *DSCREEN_LOG_TAG = "ImageSinkProcessor";
    VideoParam localParam_;
    VideoParam remoteParam_;
    std::shared_ptr<ImageSinkDecoder> imageDecoder_ = nullptr;
    std::shared_ptr<JpegImageProcessor> imageJpeg_ = nullptr;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif