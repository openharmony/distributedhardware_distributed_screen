/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_DSCREEN_SCREEN_REGION_MGR_V2_0_H
#define OHOS_DSCREEN_SCREEN_REGION_MGR_V2_0_H

#include <map>
#include <mutex>

#include "single_instance.h"
#include "screenregion.h"
#include "idscreen_source.h"

namespace OHOS {
namespace DistributedHardware {
namespace V2_0 {
class EngineProviderListener : public IAVEngineProviderCallback {
public:
    EngineProviderListener() {};
    ~EngineProviderListener() = default;

    // interfaces from IAVEngineProviderCallback
    int32_t OnProviderEvent(const AVTransEvent& event) override;
};

class ScreenRegionManager {
DECLARE_SINGLE_INSTANCE_BASE(ScreenRegionManager);
public:
    ScreenRegionManager();
    ~ScreenRegionManager();

    int32_t Initialize();
    int32_t Release();
    int32_t CreateDScreenRegion(const std::string &peerDevId);
    int32_t DestoryDScreenRegion(const std::string &peerDevId);
    void GetScreenDumpInfo(std::string &result);

private:
    int32_t LoadAVReceiverEngineProvider();
    int32_t UnloadAVReceiverEngineProvider();

private:
    std::vector<std::shared_ptr<ScreenRegion>> screenRegions_;
    std::mutex screenRegionsMtx_;
    std::string localDevId_;

    IAVEngineProvider *providerPtr_ = nullptr;
};
} // namespace V2_0
} // namespace DistributedHardware
} // namespace OHOS
#endif