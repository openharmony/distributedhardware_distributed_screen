/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_SCREEN_DECISION_CENTER_H
#define OHOS_SCREEN_DECISION_CENTER_H

#include <time.h>

#include "image_source_processor.h"
#include "jpeg_image_processor.h"
#include "surface_type.h"
#include "video_param.h"

namespace OHOS {
namespace DistributedHardware {
class ScreenDecisionCenter {
public:
    explicit ScreenDecisionCenter(const VideoParam &configParam) : configParam_(configParam){};
    ~ScreenDecisionCenter() = default;
    bool IsDirtyRectValid(const std::vector<OHOS::Rect> &damages);
    bool JudgeDirtyThreshold(const std::vector<OHOS::Rect> &damages);
    bool LimitTime(uint32_t timethreshold);
    int32_t InputBufferImage(sptr<SurfaceBuffer> &surfaceBuffer, const std::vector<OHOS::Rect> &damages);
    int32_t ConfigureDecisionCenter(std::shared_ptr<IImageSourceProcessorListener> &listener,
            std::shared_ptr<IImageSourceProcessor> &imageProcessor);
    int32_t SetJpegSurface(sptr<Surface> &surface);

private:
    static const constexpr int32_t DIRTY_REGION_ARE_THRESHOLD = 260000;
    static const constexpr int32_t FORCE_FULL_IMAGE_TIME_INTERAL = 10;  //10 seconds
    static const constexpr int32_t MIN_SURPPORT_FRAME_COUNT = 10;  //10 frames
    static const constexpr char *DSCREEN_LOG_TAG = "ScreenDecisionCenter";
    std::shared_ptr<JpegImageProcessor> imageJpeg_ = nullptr;
    std::shared_ptr<IImageSourceProcessor> imageProcessor_ = nullptr;
    VideoParam configParam_;
    int32_t frameCount_ = 0;
    time_t sendFullTime_ = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif