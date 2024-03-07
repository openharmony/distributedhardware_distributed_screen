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

#include "onoutputformatchanged_fuzzer.h"

#include <iostream>

#include "avcodec_common.h"
#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "meta/format.h"
#include "iimage_source_processor_listener.h"
#include "image_source_encoder.h"
#include "image_encoder_callback.h"
#include "iscreen_channel_listener.h"
#include "avcodec_errors.h"
#include "screen_source_trans.h"

namespace OHOS {
namespace DistributedHardware {
void OnOutputFormatChangedFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }
    std::shared_ptr<IImageSourceProcessorListener> listener = std::make_shared<ScreenSourceTrans>();
    std::shared_ptr<ImageSourceEncoder> encoder = std::make_shared<ImageSourceEncoder>(listener);
    std::shared_ptr<ImageEncoderCallback> encoderCallback = std::make_shared<ImageEncoderCallback>(encoder);

    Media::Format format;
    encoderCallback->OnOutputFormatChanged(format);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::OnOutputFormatChangedFuzzTest(data, size);
    return 0;
}

