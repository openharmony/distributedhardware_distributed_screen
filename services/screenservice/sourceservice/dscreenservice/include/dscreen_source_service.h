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

#ifndef OHOS_DSCREEN_SOURCE_SERVICE_H
#define OHOS_DSCREEN_SOURCE_SERVICE_H

#include "system_ability.h"
#include "ipc_object_stub.h"

#include "dscreen_manager.h"
#include "dscreen_source_stub.h"
#include "idscreen_source_callback.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenSourceService : public SystemAbility, public DScreenSourceStub,
    public std::enable_shared_from_this<DScreenSourceService> {
DECLARE_SYSTEM_ABILITY(DScreenSourceService);
public:
    DScreenSourceService(int32_t saId, bool runOnCreate);
    ~DScreenSourceService();
    int32_t InitSource(const std::string &params, const sptr<IDScreenSourceCallback> &callback) override;
    int32_t ReleaseSource() override;
    int32_t RegisterDistributedHardware(const std::string &devId, const std::string &dhId,
        const EnableParam &param, const std::string &reqId) override;
    int32_t UnregisterDistributedHardware(const std::string &devId, const std::string &dhId,
        const std::string &reqId) override;
    int32_t ConfigDistributedHardware(const std::string &devId, const std::string &dhId, const std::string &key,
        const std::string &value) override;
    void DScreenNotify(const std::string &devId, const int32_t eventCode, const std::string &eventContent) override;
    int32_t Dump(int32_t fd, const std::vector<std::u16string>& args) override;

protected:
    void OnStart() override;
    void OnStop() override;
    DISALLOW_COPY_AND_MOVE(DScreenSourceService);

private:
    bool Init();

    bool registerToService_ = false;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif