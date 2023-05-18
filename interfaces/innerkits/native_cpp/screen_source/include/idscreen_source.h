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

#ifndef OHOS_IDSCREEN_SOURCE_H
#define OHOS_IDSCREEN_SOURCE_H

#include "iremote_broker.h"
#include "idscreen_source_callback.h"
#include "idistributed_hardware_source.h"

namespace OHOS {
namespace DistributedHardware {
class IDScreenSource : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.distributedhardware.distributedscreensource");

    ~IDScreenSource() override {}
    virtual int32_t InitSource(const std::string &params, const sptr<IDScreenSourceCallback> &callback) = 0;
    virtual int32_t ReleaseSource() = 0;
    virtual int32_t RegisterDistributedHardware(const std::string &devId, const std::string &dhId,
        const EnableParam &param, const std::string &reqId) = 0;
    virtual int32_t UnregisterDistributedHardware(const std::string &devId, const std::string &dhId,
        const std::string &reqId) = 0;
    virtual int32_t ConfigDistributedHardware(const std::string &devId, const std::string &dhId,
        const std::string &key, const std::string &value) = 0;
    virtual void DScreenNotify(const std::string &devId, int32_t eventCode,
        const std::string &eventContent) = 0;
};
}
}
#endif