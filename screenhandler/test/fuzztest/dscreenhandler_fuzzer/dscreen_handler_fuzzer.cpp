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

#include "dscreen_handler_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <iostream>

#include "dscreen_handler.h"
#include "device_type.h"
#include "ihardware_handler.h"
#include "local_hardware_manager.h"
#include "plugin_listener_impl.h"

namespace OHOS {
namespace DistributedHardware {
void DscreenHandlerFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }

    DHType dhType = DHType::DISPLAY;
    std::shared_ptr<PluginListener> listener = std::make_shared<PluginListenerImpl>(dhType);
    std::string dhId(reinterpret_cast<const char*>(data), size);
    std::string attr(reinterpret_cast<const char*>(data), size);

    DScreenHandler::GetInstance().Initialize();
    DScreenHandler::GetInstance().Query();
    DScreenHandler::GetInstance().QueryExtraInfo();
    DScreenHandler::GetInstance().IsSupportPlugin();
    DScreenHandler::GetInstance().RegisterPluginListener(listener);
    DScreenHandler::GetInstance().PluginHardware(dhId, attr);
    DScreenHandler::GetInstance().UnPluginHardware(dhId);
    DScreenHandler::GetInstance().UnRegisterPluginListener();
    DScreenHandler::GetInstance().QueryCodecInfo();
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::DscreenHandlerFuzzTest(data, size);
    return 0;
}

