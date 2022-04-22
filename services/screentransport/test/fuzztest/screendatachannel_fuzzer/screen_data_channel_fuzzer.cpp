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

#include "screen_data_channel_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "screen_data_channel_impl.h"
#include "screen_sink_trans.h"
#include "screen_source_trans.h"

namespace OHOS {
namespace DistributedHardware {
void ScreenDataChannelFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }

    std::string peerDevId(reinterpret_cast<const char*>(data), size);
    std::shared_ptr<ScreenDataChannelImpl> screenDataChannelImpl = std::make_shared<ScreenDataChannelImpl>(peerDevId);
    std::shared_ptr<IScreenChannelListener> listener = std::make_shared<ScreenSourceTrans>();
    size_t capacity = size;
    std::shared_ptr<DataBuffer> screenData = std::make_shared<DataBuffer>(capacity);

    screenDataChannelImpl->CreateSession(listener);
    screenDataChannelImpl->OpenSession();
    screenDataChannelImpl->SendData(screenData);
    screenDataChannelImpl->CloseSession();
    screenDataChannelImpl->ReleaseSession();
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::ScreenDataChannelFuzzTest(data, size);
    
    return 0;
}