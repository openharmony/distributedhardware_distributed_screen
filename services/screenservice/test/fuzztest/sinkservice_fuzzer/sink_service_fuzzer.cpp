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

#include "sink_service_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string>

#include "dscreen_sink_service.h"

namespace OHOS {
namespace DistributedHardware {
void SinkServiceFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= sizeof(int32_t))) {
        return;
    }

    int32_t saId = *(reinterpret_cast<const int32_t*>(data));
    bool runOnCreate = true;
    std::shared_ptr<DScreenSinkService> sinkService = std::make_shared<DScreenSinkService>(saId, runOnCreate);
    std::string params(reinterpret_cast<const char*>(data), size);
    std::string dhId(reinterpret_cast<const char*>(data), size);
    std::string param(reinterpret_cast<const char*>(data), size);
    std::string devId(reinterpret_cast<const char*>(data), size);
    int32_t eventCode = *(reinterpret_cast<const int32_t*>(data));
    std::string eventContent(reinterpret_cast<const char*>(data), size);

    sinkService->DScreenNotify(devId, eventCode, eventContent);
    sinkService->InitSink(params);
    sinkService->SubscribeLocalHardware(dhId, param);
    sinkService->UnsubscribeLocalHardware(dhId);
    sinkService->ReleaseSink();
    
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::SinkServiceFuzzTest(data, size);
    return 0;
}