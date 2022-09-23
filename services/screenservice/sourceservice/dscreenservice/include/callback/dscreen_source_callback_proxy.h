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

#ifndef OHOS_DSCREEN_SOURCE_CALLBACK_PROXY_H
#define OHOS_DSCREEN_SOURCE_CALLBACK_PROXY_H

#include "iremote_proxy.h"

#include "idscreen_source_callback.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenSourceCallbackProxy : public IRemoteProxy<IDScreenSourceCallback> {
public:
    explicit DScreenSourceCallbackProxy(const sptr<IRemoteObject> impl)
        : IRemoteProxy<IDScreenSourceCallback>(impl)
    {
    }

    ~DScreenSourceCallbackProxy() {}
    int32_t OnNotifyRegResult(const std::string &devId, const std::string &dhId, const std::string &reqId,
        int32_t status, const std::string &resultData) override;
    int32_t OnNotifyUnregResult(const std::string &devId, const std::string &dhId, const std::string &reqId,
        int32_t status, const std::string &resultData) override;

private:
    static inline BrokerDelegator<DScreenSourceCallbackProxy> delegator_;
    bool CheckParams(const std::string &devId, const std::string &dhId, const std::string &reqId,
        const std::string &resultData);
    const size_t DID_MAX_SIZE = 256;
    const size_t PARAM_MAX_SIZE = 50 * 1024 * 1024;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif
