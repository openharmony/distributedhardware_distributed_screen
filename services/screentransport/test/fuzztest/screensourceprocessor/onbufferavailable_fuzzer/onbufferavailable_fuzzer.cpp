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

#include "onbufferavailable_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "image_source_encoder.h"
#include "screen_source_trans.h"

namespace OHOS {
namespace DistributedHardware {
void OnBufferAvailableFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    std::shared_ptr<IImageSourceProcessorListener> listener = std::make_shared<ScreenSourceTrans>();
    std::shared_ptr<ImageSourceEncoder> encoder = std::make_shared<ImageSourceEncoder>(listener);
    std::shared_ptr<ConsumerBufferListener> bufferListen = std::make_shared<ConsumerBufferListener>(encoder);
    bufferListen->OnBufferAvailable();
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::OnBufferAvailableFuzzTest(data, size);
    return 0;
}

