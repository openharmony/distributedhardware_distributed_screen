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

#include "screen_sink_processor_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string>

#include "image_sink_processor.h"
#include "screen_sink_trans.h"

namespace OHOS {
namespace DistributedHardware {
void ScreenSinkProcessorFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }

    std::shared_ptr<ImageSinkProcessor> imageSinkProcessor = std::make_shared<ImageSinkProcessor>();
    std::shared_ptr<VideoParam> param = std::make_shared<VideoParam>();
    uint32_t screenWidth = *(reinterpret_cast<const uint32_t*>(data));
    uint32_t screenHeight = *(reinterpret_cast<const uint32_t*>(data));
    uint32_t videoWidth = *(reinterpret_cast<const uint32_t*>(data));
    uint32_t videoHeight = *(reinterpret_cast<const uint32_t*>(data));
    param->SetScreenWidth(screenWidth);
    param->SetScreenHeight(screenHeight);
    param->SetVideoWidth(videoWidth);
    param->SetScreenHeight(videoHeight);
    std::shared_ptr<IImageSinkProcessorListener> imageListener = std::make_shared<ScreenSinkTrans>();
    std::string name(reinterpret_cast<const char*>(data), size);
    sptr<Surface> surface = Surface::CreateSurfaceAsConsumer(name);
    size_t capacity = size;
    std::shared_ptr<DataBuffer> screenData = std::make_shared<DataBuffer>(capacity);

    imageSinkProcessor->SetImageSurface(surface);
    imageSinkProcessor->ConfigureImageProcessor(*param, *param, imageListener);
    imageSinkProcessor->ProcessImage(screenData);
    imageSinkProcessor->StartImageProcessor();
    imageSinkProcessor->StopImageProcessor();
    imageSinkProcessor->ReleaseImageProcessor();
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::ScreenSinkProcessorFuzzTest(data, size);
    return 0;
}