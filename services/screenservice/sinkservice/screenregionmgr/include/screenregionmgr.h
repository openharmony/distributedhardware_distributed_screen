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

#ifndef OHOS_DSCREEN_SCREEN_REGION_MGR_H
#define OHOS_DSCREEN_SCREEN_REGION_MGR_H

#include <map>
#include <mutex>

#include "distributed_hardware_fwk_kit.h"
#include "single_instance.h"

#include "screenregion.h"
#include "idscreen_source.h"

namespace OHOS {
namespace DistributedHardware {
class ScreenRegionManager {
DECLARE_SINGLE_INSTANCE_BASE(ScreenRegionManager);
public:
    int32_t ReleaseAllRegions();
    void HandleDScreenNotify(const std::string &devId, int32_t eventCode, const std::string &eventContent);
    void GetScreenDumpInfo(std::string &result);
    void PublishMessage(const DHTopic topic, const uint64_t &screenId, const std::string &remoteDevId,
        const int32_t &windowId, std::shared_ptr<WindowProperty> windowProperty);

private:
    ScreenRegionManager();
    ~ScreenRegionManager();
    std::map<std::string, std::shared_ptr<ScreenRegion>> screenRegions_;
    std::mutex screenRegionsMtx_;
    std::string localDevId_;

    sptr<IDScreenSource> GetDScreenSourceSA(const std::string &devId);
    int32_t NotifyRemoteScreenService(const std::string &remoteDevId, const std::string &dhId,
        int32_t eventCode, const std::string &eventContent);
    bool CheckContentJson(json &eventContentJson);
    void HandleNotifySetUp(const std::string &remoteDevId, const std::string &eventContent);
    void NotifyRemoteSourceSetUpResult(const std::string &remoteDevId, const std::string &dhId,
        int32_t errCode, const std::string &errContent);
};
} // namespace DistributedHardware
} // namespace OHOS
#endif