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

#ifndef OHOS_SCREEN_MGR_ADAPTER_H
#define OHOS_SCREEN_MGR_ADAPTER_H

#include <map>
#include <surface.h>

#include "single_instance.h"
#include "screen_manager.h"

#include "dscreen_maprelation.h"
#include "video_param.h"

namespace OHOS {
namespace DistributedHardware {
class ScreenMgrAdapter {
DECLARE_SINGLE_INSTANCE_BASE(ScreenMgrAdapter);

public:
    uint64_t CreateVirtualScreen(const std::string &devId, const std::string &dhId,
        const std::shared_ptr<VideoParam> &videoParam);
    int32_t RegisterScreenGroupListener(sptr<Rosen::ScreenManager::IScreenGroupListener> listener);
    int32_t UnregisterScreenGroupListener(sptr<Rosen::ScreenManager::IScreenGroupListener> listener);
    int32_t RemoveVirtualScreen(uint64_t screenId);
    int32_t SetImageSurface(uint64_t screenId, sptr<OHOS::Surface> surface);
    std::shared_ptr<DScreenMapRelation> GetMapRelation(uint64_t screenId);
    void RemoveScreenFromGroup(uint64_t screenId);

private:
    ScreenMgrAdapter() = default;
    ~ScreenMgrAdapter();

    bool listenerRegistered_ = false;
    std::map<std::string, uint64_t> screenIdMap_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif