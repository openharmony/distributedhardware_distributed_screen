/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "dscreensourceservice_fuzzer.h"

#include "string_ex.h"
#include "idscreen_source.h"
#include "dscreen_source_service.h"
#include "dscreen_source_callback.h"

namespace OHOS {
namespace DistributedHardware {
const std::u16string ABILITYMGR_INTERFACE_TOKEN = u"ohos.aafwk.AbilityManager";
void DscreenSourceServiceFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }

    std::string version(reinterpret_cast<const char*>(data), size);
    EnableParam param;
    param.sinkVersion = version;
    param.sinkAttrs = "";
    uint32_t eventCode = 1;
    std::string dhId(reinterpret_cast<const char*>(data), size);
    std::string devId(reinterpret_cast<const char*>(data), size);
    std::string reqId(reinterpret_cast<const char*>(data), size);
    std::string eventContent(reinterpret_cast<const char*>(data), size);
    std::string params(reinterpret_cast<const char*>(data), size);
    std::shared_ptr<DScreenSourceService> sourceServicePtr = std::make_shared<DScreenSourceService>(0, false);
    sptr<IDScreenSourceCallback> callback(new DScreenSourceCallback());
    sourceServicePtr->registerToService_ = true;
    sourceServicePtr->Init();
    sourceServicePtr->InitSource(params, callback);
    sourceServicePtr->RegisterDistributedHardware(devId, dhId, param, reqId);
    sourceServicePtr->DScreenNotify(devId, eventCode, eventContent);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::DscreenSourceServiceFuzzTest(data, size);
    return 0;
}
