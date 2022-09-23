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

#ifndef OHOS_DSCREEN_SINK_PROXY_H
#define OHOS_DSCREEN_SINK_PROXY_H

#include <cstdint>

#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "refbase.h"

#include "idscreen_sink.h"

namespace OHOS { class IRemoteObject; }
namespace OHOS {
namespace DistributedHardware {
class DScreenSinkProxy : public IRemoteProxy<IDScreenSink> {
public:
    explicit DScreenSinkProxy(const sptr<IRemoteObject> impl)
        : IRemoteProxy<IDScreenSink>(impl)
    {
    }

    ~DScreenSinkProxy() {}
    int32_t InitSink(const std::string &params) override;
    int32_t ReleaseSink() override;
    int32_t SubscribeLocalHardware(const std::string &dhId, const std::string &param) override;
    int32_t UnsubscribeLocalHardware(const std::string &dhId) override;
    void DScreenNotify(const std::string &devId, int32_t eventCode, const std::string &eventContent) override;

private:
    static inline BrokerDelegator<DScreenSinkProxy> delegator_;

    const size_t DID_MAX_SIZE = 256;
    const size_t PARAM_MAX_SIZE = 50 * 1024 * 1024;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif
