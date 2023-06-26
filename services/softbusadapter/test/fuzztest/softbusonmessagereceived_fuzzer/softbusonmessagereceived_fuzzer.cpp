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

#include "softbusonmessagereceived_fuzzer.h"

#include <iostream>

#include "softbus_adapter.h"

namespace OHOS {
namespace DistributedHardware {
void SoftbusOnMessageReceivedFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < (sizeof(int32_t)))) {
        return;
    }
    int32_t sessionId = *(reinterpret_cast<const int32_t*>(data));
    void *data = nullptr;
    unsigned int  dataLen = *(reinterpret_cast<const  unsigned int *>(data));
    std::shared_ptr<SoftbusAdapter> adapter = std::make_shared<SoftbusAdapter>();
    adapter->OnMessageReceived(sessionId, data, dataLen);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::SoftbusOnMessageReceivedFuzzTest(data, size);
    return 0;
}

