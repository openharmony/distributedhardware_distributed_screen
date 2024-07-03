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

#include "unregisterdistributedhardware_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "dscreen_constants.h"
#include "dscreen_source_callback.h"
#include "dscreen_source_proxy.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"

namespace OHOS {
namespace DistributedHardware {
void UnregisterDistributedHardwareFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }

    std::string dhId(reinterpret_cast<const char*>(data), size);
    std::string devId(reinterpret_cast<const char*>(data), size);
    std::string reqId(reinterpret_cast<const char*>(data), size);

    sptr<ISystemAbilityManager> samgr =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        return;
    }
    int32_t saId = *(reinterpret_cast<const int32_t*>(data));
    sptr<IRemoteObject> remoteObject = samgr->GetSystemAbility(saId);
    if (remoteObject == nullptr) {
        return;
    }

    std::shared_ptr<IDScreenSource> dscreenSourceProxy = std::make_shared<DScreenSourceProxy>(remoteObject);

    dscreenSourceProxy->UnregisterDistributedHardware(devId, dhId, reqId);
}
}  // namespace DistributedHardware
}  // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::UnregisterDistributedHardwareFuzzTest(data, size);
    return 0;
}