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

#ifndef OHOS_JPEG_IMAGE_PROCESS_H
#define OHOS_JPEG_IMAGE_PROCESS_H

#include <thread>
#include <mutex>
#include <condition_variable>

#include "data_buffer.h"
#include "dscreen_constants.h"
#include "iimage_source_processor_listener.h"

#include "surface.h"
#include "video_param.h"
namespace OHOS {
namespace DistributedHardware {
class JpegImageProcessor : public std::enable_shared_from_this<JpegImageProcessor> {
public:
    explicit JpegImageProcessor(const VideoParam &configParam) : configParam_(configParam){};
    ~JpegImageProcessor() = default;
    int32_t SetOutputSurface(sptr<Surface> surface);
    int32_t FillDirtyImages2Surface(const std::shared_ptr<DataBuffer> &data, uint8_t *lastFrame);
    int32_t ProcessDamageSurface(sptr<SurfaceBuffer> &surfaceBuffer, const std::vector<OHOS::Rect> &damages);
    int32_t SetImageProcessListener(std::shared_ptr<IImageSourceProcessorListener> &listener);
    void EncodeDamageData(sptr<SurfaceBuffer> &surfaceBuffer, const OHOS::Rect &damage,
        std::shared_ptr<DataBuffer> &data);
    int32_t DecodeDamageData(const std::shared_ptr<DataBuffer> &data, uint8_t *lastFrame);
    int32_t ReplaceDamage2LastFrame(uint8_t *lastFrame, uint8_t *dirtyImageData, const DirtyRect rect);
private:
    uint32_t CompressRgbaToJpeg(const OHOS::Rect &damage, uint8_t *inputData,
        uint32_t inputDataSize, std::shared_ptr<DataBuffer> &data);
    void DecompressJpegToNV12(size_t jpegSize, uint8_t *inputData, uint8_t *outputData);

    static const constexpr char *DSCREEN_LOG_TAG = "JpegImageProcessor";
    sptr<Surface> imageSurface_;
    VideoParam configParam_;
    std::weak_ptr<IImageSourceProcessorListener> imageProcessorListener_;
    std::mutex dataMutex_;
    std::condition_variable dataCond_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif