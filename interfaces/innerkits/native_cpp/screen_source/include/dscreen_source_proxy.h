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

#ifndef OHOS_DSCREEN_SOURCE_PROXY_H
#define OHOS_DSCREEN_SOURCE_PROXY_H

#include <cstdint>

#include "idistributed_hardware_source.h"
#include "idscreen_source.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "refbase.h"

namespace OHOS { class IRemoteObject; }
namespace OHOS {
namespace DistributedHardware {
class DScreenSourceProxy : public IRemoteProxy<IDScreenSource> {
public:
    explicit DScreenSourceProxy(const sptr<IRemoteObject> impl)
        : IRemoteProxy<IDScreenSource>(impl)
    {
    }

    ~DScreenSourceProxy() {}
    int32_t InitSource(const std::string &params, const sptr<IDScreenSourceCallback> &callback) override;
    int32_t ReleaseSource() override;
    int32_t RegisterDistributedHardware(const std::string &devId, const std::string &dhId,
        const EnableParam &param, const std::string &reqId) override;
    int32_t UnregisterDistributedHardware(const std::string &devId, const std::string &dhId,
        const std::string &reqId) override;
    int32_t ConfigDistributedHardware(const std::string &devId, const std::string &dhId,
        const std::string &key, const std::string &value) override;
    void DScreenNotify(const std::string &devId, int32_t eventCode, const std::string &eventContent) override;

private:
    static inline BrokerDelegator<DScreenSourceProxy> delegator_;
    bool CheckRegParams(const std::string &devId, const std::string &dhId,
        const EnableParam &param, const std::string &reqId);
    bool CheckUnregParams(const std::string &devId, const std::string &dhId, const std::string &reqId);
    bool CheckConfigParams(const std::string &devId, const std::string &dhId,
        const std::string &key, const std::string &value);
    const size_t DID_MAX_SIZE = 256;
    const size_t PARAM_MAX_SIZE = 50 * 1024 * 1024;
};
}
}
#endif
