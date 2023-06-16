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

#include "onchange_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "1.0/include/dscreen_manager.h"

namespace OHOS {
namespace DistributedHardware {
namespace V1_0 {
void OnChangeFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(uint64_t))) {
        return;
    }
    std::vector<uint64_t> screenIds;
    uint64_t screenIdOne = *(reinterpret_cast<const uint64_t*>(data));
    screenIds.emplace_back(screenIdOne);
    Rosen::ScreenGroupChangeEvent event = Rosen::ScreenGroupChangeEvent::ADD_TO_GROUP;
    DScreenGroupListener dScreenGroupListener;
    dScreenGroupListener.OnChange(screenIds, event);
}
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::V1_0::OnChangeFuzzTest(data, size);
    return 0;
}

