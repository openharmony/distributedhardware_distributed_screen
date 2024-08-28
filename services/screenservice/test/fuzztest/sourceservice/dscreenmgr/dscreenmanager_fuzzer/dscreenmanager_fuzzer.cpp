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

#include "dscreenmanager_fuzzer.h"

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

void HandleScreenChangeFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(uint32_t))) {
        return;
    }

    uint32_t eventValue = *(reinterpret_cast<const uint32_t*>(data)) % 3;
    if ((eventValue != static_cast<uint32_t>(Rosen::ScreenGroupChangeEvent::ADD_TO_GROUP)) ||
        (eventValue != static_cast<uint32_t>(Rosen::ScreenGroupChangeEvent::REMOVE_FROM_GROUP)) ||
        (eventValue != static_cast<uint32_t>(Rosen::ScreenGroupChangeEvent::CHANGE_GROUP))) {
        return;
    }

    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    Rosen::ScreenGroupChangeEvent event = static_cast<Rosen::ScreenGroupChangeEvent>(eventValue);

    std::shared_ptr<DScreenManager> dscreenManager = std::make_shared<DScreenManager>();
    dscreenManager->HandleScreenChange(dScreen, event);
}

void AddToGroupFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(uint64_t))) {
        return;
    }

    uint64_t screenId = *(reinterpret_cast<const uint64_t*>(data));
    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);

    std::shared_ptr<DScreenManager> dscreenManager = std::make_shared<DScreenManager>();
    dscreenManager->AddToGroup(dScreen, screenId);
    dscreenManager->RemoveFromGroup(dScreen, screenId);
}

void DScreenCallbackOnRegResultFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }

    std::string screenId(reinterpret_cast<const char*>(data), size);
    int32_t status = *(reinterpret_cast<const int32_t*>(data));
    std::string dataStr(reinterpret_cast<const char*>(data), size);

    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    dScreenCallback->OnRegResult(dScreen, screenId, status, dataStr);
    dScreenCallback->OnUnregResult(dScreen, screenId, status, dataStr);
}

void DScreenManagerOnUnregResultFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }

    std::string reqId(reinterpret_cast<const char*>(data), size);
    int32_t status = *(reinterpret_cast<const int32_t*>(data));
    std::string dataStr(reinterpret_cast<const char*>(data), size);

    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    std::shared_ptr<DScreenManager> dscreenManager = std::make_shared<DScreenManager>();
    dscreenManager->OnRegResult(dScreen, reqId, status, dataStr);
    dscreenManager->OnUnregResult(dScreen, reqId, status, dataStr);
}

void NotifyRemoteScreenServiceFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }

    std::string devId(reinterpret_cast<const char*>(data), size);
    int32_t eventCode = *(reinterpret_cast<const int32_t*>(data));
    std::string eventContent(reinterpret_cast<const char*>(data), size);

    std::shared_ptr<DScreenManager> dscreenManager = std::make_shared<DScreenManager>();
    dscreenManager->NotifyRemoteScreenService(devId, eventCode, eventContent);
}

void HandleNotifySetUpResultFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    std::string remoteDevId(reinterpret_cast<const char*>(data), size);
    std::string eventContent(reinterpret_cast<const char*>(data), size);

    std::shared_ptr<DScreenManager> dscreenManager = std::make_shared<DScreenManager>();
    dscreenManager->HandleNotifySetUpResult(remoteDevId, eventContent);
}

}  // namespace V1_0
}  // namespace DistributedHardware
}  // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::V1_0::OnChangeFuzzTest(data, size);
    OHOS::DistributedHardware::V1_0::HandleScreenChangeFuzzTest(data, size);
    OHOS::DistributedHardware::V1_0::AddToGroupFuzzTest(data, size);
    OHOS::DistributedHardware::V1_0::DScreenCallbackOnRegResultFuzzTest(data, size);
    OHOS::DistributedHardware::V1_0::DScreenManagerOnUnregResultFuzzTest(data, size);
    OHOS::DistributedHardware::V1_0::NotifyRemoteScreenServiceFuzzTest(data, size);
    OHOS::DistributedHardware::V1_0::HandleNotifySetUpResultFuzzTest(data, size);
    return 0;
}
