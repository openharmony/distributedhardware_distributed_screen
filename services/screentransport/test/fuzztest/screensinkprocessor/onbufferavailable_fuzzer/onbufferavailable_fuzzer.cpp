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

#include "avcodec_common.h"
#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "meta/format.h"
#include "iimage_source_processor_listener.h"
#include "screen_sink_trans.h"
#include "image_sink_decoder.h"
#include "image_source_encoder.h"
#include "image_encoder_callback.h"
#include "iscreen_channel_listener.h"

namespace OHOS {
namespace DistributedHardware {
void OnBufferAvailableFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    std::shared_ptr<IImageSinkProcessorListener> listener = std::make_shared<ScreenSinkTrans>();
    std::shared_ptr<ImageSinkDecoder> decoder = std::make_shared<ImageSinkDecoder>(listener);
    std::shared_ptr<ConsumBufferListener> bufferListen = std::make_shared<ConsumBufferListener>(decoder);
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

