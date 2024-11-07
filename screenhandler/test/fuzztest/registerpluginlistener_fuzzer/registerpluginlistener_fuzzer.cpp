/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "registerpluginlistener_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "dscreen_handler.h"
#include "device_type.h"
#include "ihardware_handler.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenHandlerFuzzTestPluginListener : public PluginListener {
public:
    explicit DScreenHandlerFuzzTestPluginListener(const DHType type) {}
    virtual ~DScreenHandlerFuzzTestPluginListener() = default;

    void PluginHardware(const std::string &dhId, const std::string &attrs, const std::string &subtype) override {}

    void UnPluginHardware(const std::string &dhId) override {}
};

void RegisterPluginListenerFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }

    int32_t i = 0;
    std::unordered_map<int32_t, DHType> dhTypeMap;
    dhTypeMap[i++] = DHType::UNKNOWN;
    dhTypeMap[i++] = DHType::CAMERA;
    dhTypeMap[i++] = DHType::AUDIO;
    dhTypeMap[i++] = DHType::SCREEN;
    dhTypeMap[i++] = DHType::GPS;
    dhTypeMap[i++] = DHType::INPUT;
    dhTypeMap[i++] = DHType::HFP;
    dhTypeMap[i++] = DHType::A2D;
    dhTypeMap[i++] = DHType::VIRMODEM_AUDIO;
    dhTypeMap[i++] = DHType::MODEM;
    dhTypeMap[i++] = DHType::MAX_DH;

    int32_t key = *(reinterpret_cast<const int32_t*>(data)) % static_cast<int32_t>(dhTypeMap.size());
    if (dhTypeMap.count(key) == 0) {
        return;
    }

    DHType dhType = dhTypeMap[key];
    std::shared_ptr<PluginListener> listener = std::make_shared<DScreenHandlerFuzzTestPluginListener>(dhType);
    DScreenHandler::GetInstance().RegisterPluginListener(listener);
}
}  // namespace DistributedHardware
}  // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::RegisterPluginListenerFuzzTest(data, size);
    return 0;
}

