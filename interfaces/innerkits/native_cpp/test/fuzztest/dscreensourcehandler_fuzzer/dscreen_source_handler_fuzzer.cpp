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

#include "dscreen_source_handler_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "component_disable.h"
#include "component_enable.h"
#include "dscreen_constants.h"
#include "dscreen_source_handler.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"

namespace OHOS {
namespace DistributedHardware {
void DscreenSourceHandlerFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }

    std::string params(reinterpret_cast<const char*>(data), size);
    std::string devId(reinterpret_cast<const char*>(data), size);
    std::string dhId(reinterpret_cast<const char*>(data), size);
    std::string key(reinterpret_cast<const char*>(data), size);
    std::string value(reinterpret_cast<const char*>(data), size);
    std::string str(reinterpret_cast<const char*>(data), size);
    EnableParam param;
    param.version = str;
    param.attrs = str;
    std::shared_ptr<RegisterCallback> callback = std::make_shared<ComponentEnable>();
    std::shared_ptr<UnregisterCallback> uncallback = std::make_shared<ComponentDisable>();

    sptr<ISystemAbilityManager> samgr =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = samgr->GetSystemAbility(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID);
    wptr<IRemoteObject> remote (remoteObject);

    DScreenSourceHandler::GetInstance().InitSource(params);
    DScreenSourceHandler::GetInstance().RegisterDistributedHardware(devId, dhId, param, callback);
    DScreenSourceHandler::GetInstance().ConfigDistributedHardware(devId, dhId, key, value);
    DScreenSourceHandler::GetInstance().UnregisterDistributedHardware(devId, dhId, uncallback);
    DScreenSourceHandler::GetInstance().ReleaseSource();
    DScreenSourceHandler::GetInstance().OnRemoteSourceSvrDied(remote);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::DscreenSourceHandlerFuzzTest(data, size);
    return 0;
}