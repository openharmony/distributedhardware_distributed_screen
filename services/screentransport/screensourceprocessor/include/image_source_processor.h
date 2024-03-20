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

#ifndef OHOS_IMAGE_SOURCE_PROCESSOR_H
#define OHOS_IMAGE_SOURCE_PROCESSOR_H

#include "iimage_source_processor.h"
#include "iimage_source_processor_listener.h"
#include "image_source_encoder.h"
#include "video_param.h"

namespace OHOS {
namespace DistributedHardware {
class ImageSourceProcessor : public IImageSourceProcessor {
public:
    ImageSourceProcessor() = default;
    ~ImageSourceProcessor() override = default;

    int32_t ConfigureImageProcessor(const VideoParam &localParam, const VideoParam &remoteParam,
        const std::shared_ptr<IImageSourceProcessorListener> &listener) override;
    int32_t ReleaseImageProcessor() override;
    int32_t StartImageProcessor() override;
    int32_t StopImageProcessor() override;
    sptr<Surface> GetImageSurface() override;
    sptr<Surface> GetConsumerSurface() override;
    int32_t ProcessFullImage(sptr<SurfaceBuffer> &surfaceBuffer) override;

private:
    static const constexpr char *DSCREEN_LOG_TAG = "ImageSourceProcessor";
    VideoParam localParam_;
    VideoParam remoteParam_;
    std::shared_ptr<ImageSourceEncoder> imageEncoder_ = nullptr;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif