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

#ifndef OHOS_DSCREEN_SOURCE_STUB_H
#define OHOS_DSCREEN_SOURCE_STUB_H

#include <map>
#include "iremote_stub.h"
#include "idscreen_source.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenSourceStub : public IRemoteStub<IDScreenSource> {
public:
    DScreenSourceStub();
    virtual ~DScreenSourceStub() = default;
    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;

private:
    int32_t InitSourceInner(MessageParcel &data, MessageParcel &reply,
        MessageOption &option);
    int32_t ReleaseSourceInner(MessageParcel &data, MessageParcel &reply,
        MessageOption &option);
    int32_t RegisterDistributedHardwareInner(MessageParcel &data, MessageParcel &reply,
        MessageOption &option);
    int32_t UnregisterDistributedHardwareInner(MessageParcel &data, MessageParcel &reply,
        MessageOption &option);
    int32_t ConfigDistributedHardwareInner(MessageParcel &data, MessageParcel &reply,
        MessageOption &option);
    int32_t DScreenNotifyInner(MessageParcel &data, MessageParcel &reply,
        MessageOption &option);
    bool CheckRegParams(const std::string &devId, const std::string &dhId,
        const std::string &version, const std::string &attrs, const std::string &reqId);
    bool CheckUnregParams(const std::string &devId, const std::string &dhId, const std::string &reqId);
    bool CheckConfigParams(const std::string &devId, const std::string &dhId,
        const std::string &key, const std::string &value);
    using DScreenSourceFunc = int32_t (DScreenSourceStub::*)(MessageParcel &data, MessageParcel &reply,
        MessageOption &option);
    std::map<int32_t, DScreenSourceFunc> memberFuncMap_;

    const size_t DID_MAX_SIZE = 256;
    const size_t PARAM_MAX_SIZE = 50 * 1024 * 1024;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif