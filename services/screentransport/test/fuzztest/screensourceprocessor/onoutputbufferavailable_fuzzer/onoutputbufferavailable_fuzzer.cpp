/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include <iostream>

#include "onoutputbufferavailable_fuzzer.h"
#include "avcodec_common.h"
#include "fuzzer/FuzzedDataProvider.h"
#include "image_encoder_callback.h"
#include "meta/format.h"
#include "screen_source_trans.h"

namespace OHOS {
namespace DistributedHardware {
void OnOutputBufferAvailableFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider dataProvider(data, size);
    uint32_t index = dataProvider.ConsumeIntegral<uint32_t>();

    std::shared_ptr<IImageSourceProcessorListener> listener = std::make_shared<ScreenSourceTrans>();
    std::shared_ptr<ImageSourceEncoder> encoder = std::make_shared<ImageSourceEncoder>(listener);
    std::shared_ptr<ImageEncoderCallback> encoderCallback = std::make_shared<ImageEncoderCallback>(encoder);
    MediaAVCodec::AVCodecBufferInfo info;
    MediaAVCodec::AVCodecBufferFlag flag = MediaAVCodec::AVCODEC_BUFFER_FLAG_NONE;
    std::shared_ptr<Media::AVSharedMemory> buffer = nullptr;
    encoderCallback->OnOutputBufferAvailable(index, info, flag, buffer);
}
} // namespace DistributedHardware
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::OnOutputBufferAvailableFuzzTest(data, size);
    return 0;
}
