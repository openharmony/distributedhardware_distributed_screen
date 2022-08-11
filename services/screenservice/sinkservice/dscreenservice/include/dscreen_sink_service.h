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

#ifndef OHOS_DSCREEN_SINK_SERVICE_H
#define OHOS_DSCREEN_SINK_SERVICE_H

#include "ipc_object_stub.h"
#include "system_ability.h"

#include "dscreen_sink_stub.h"
#include "screenregionmgr.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenSinkService : public SystemAbility, public DScreenSinkStub,
    public std::enable_shared_from_this<DScreenSinkService> {
DECLARE_SYSTEM_ABILITY(DScreenSinkService);
public:
    DScreenSinkService(int32_t saId, bool runOnCreate);
    ~DScreenSinkService() = default;
    int32_t InitSink(const std::string &params) override;
    int32_t ReleaseSink() override;
    int32_t SubscribeLocalHardware(const std::string &dhId, const std::string &param) override;
    int32_t UnsubscribeLocalHardware(const std::string &dhId) override;
    void DScreenNotify(const std::string &devId, int32_t eventCode, const std::string &eventContent) override;
    int32_t Dump(int32_t fd, const std::vector<std::u16string>& args) override;

protected:
    void OnStart() override;
    void OnStop() override;
    DISALLOW_COPY_AND_MOVE(DScreenSinkService);

private:
    std::shared_ptr<ScreenRegionManager> screenRegionMgr_ = nullptr;
    bool registerToService_ = false;

    bool Init();
};
} // namespace DistributedHardware
} // namespace OHOS
#endif