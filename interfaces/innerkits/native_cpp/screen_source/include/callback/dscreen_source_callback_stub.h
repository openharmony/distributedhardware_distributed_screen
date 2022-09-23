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

#ifndef OHOS_DSCREEN_SOURCE_CALLBACK_STUB_H
#define OHOS_DSCREEN_SOURCE_CALLBACK_STUB_H

#include <cstdint>
#include <map>

#include "iremote_stub.h"
#include "refbase.h"

#include "idscreen_source_callback.h"

namespace OHOS { class MessageOption; }
namespace OHOS { class MessageParcel; }
namespace OHOS {
namespace DistributedHardware {
class DScreenSourceCallbackStub : public IRemoteStub<IDScreenSourceCallback> {
public:
    DScreenSourceCallbackStub();
    virtual ~DScreenSourceCallbackStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;
private:
    using DScreenFunc = int32_t (DScreenSourceCallbackStub::*)(MessageParcel &data, MessageParcel &reply,
        MessageOption &option);
    std::map<int32_t, DScreenFunc> memberFuncMap_;
    const size_t DID_MAX_SIZE = 256;
    const size_t PARAM_MAX_SIZE = 50 * 1024 * 1024;

    int32_t OnNotifyRegResultInner(MessageParcel &data, MessageParcel &reply,
        MessageOption &option);
    int32_t OnNotifyUnregResultInner(MessageParcel &data, MessageParcel &reply,
        MessageOption &option);
    bool CheckParams(const std::string &devId, const std::string &dhId, const std::string &reqId,
        const std::string &resultData);
};
}
}
#endif