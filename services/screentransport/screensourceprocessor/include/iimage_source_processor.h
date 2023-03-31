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

#ifndef OHOS_IIMAGE_SOURCE_PROCESSOR_H
#define OHOS_IIMAGE_SOURCE_PROCESSOR_H

#include "surface.h"
#include "surface_buffer.h"
#include <memory>

#include "data_buffer.h"
#include "iimage_source_processor_listener.h"
#include "video_param.h"

namespace OHOS {
namespace DistributedHardware {
class IImageSourceProcessor {
public:
    virtual ~IImageSourceProcessor() = default;

    virtual int32_t ConfigureImageProcessor(const VideoParam &localParam, const VideoParam &remoteParam,
        const std::shared_ptr<IImageSourceProcessorListener> &listener) = 0;
    virtual int32_t ReleaseImageProcessor() = 0;
    virtual int32_t StartImageProcessor() = 0;
    virtual int32_t StopImageProcessor() = 0;
    virtual sptr<Surface> GetImageSurface() = 0;
    virtual sptr<Surface> GetConsumerSurface() = 0;
    virtual int32_t ProcessFullImage(sptr<SurfaceBuffer> &surfaceBuffer) = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif