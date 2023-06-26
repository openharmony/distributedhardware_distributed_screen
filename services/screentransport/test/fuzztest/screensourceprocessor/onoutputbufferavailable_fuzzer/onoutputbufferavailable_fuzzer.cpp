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

#include "onoutputbufferavailable_fuzzer.h"

#include <iostream>

#include "avcodec_common.h"
#include "format.h"
#include "image_encoder_callback.h"

namespace OHOS {
namespace DistributedHardware {
void OnOutputBufferAvailableFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < (sizeof(uint32_t)))) {
        return;
    }
    std::shared_ptr<IImageSourceProcessorListener> listener = std::make_shared<ScreenSourceTrans>();
    std::shared_ptr<ImageSourceEncoder> encoder = std::make_shared<ImageSourceEncoder>(listener);
    std::shared_ptr<ImageEncoderCallback> encoderCallback = std::make_shared<ImageEncoderCallback>(encoder);
    uint32_t index = *(reinterpret_cast<const uint32_t*>(data));
    Media::AVCodecBufferInfo info = *(reinterpret_cast<const Media::AVCodecBufferInfo*>(data));
    Media::AVCodecBufferFlag flag = Media::AVCODEC_BUFFER_FLAG_NONE;
    encoderCallback->OnOutputBufferAvailable(index, info, flag);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::OnOutputBufferAvailableFuzzTest(data, size);
    return 0;
}

