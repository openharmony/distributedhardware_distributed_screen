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

#ifndef OHOS_DSCREEN_COMMON_H
#define OHOS_DSCREEN_COMMON_H

#include <string>

#include "dscreen_source_callback.h"
#include "dscreen_source_callback_stub.h"
#include "dscreen_source_handler.h"
#include "dscreen_source_proxy.h"
#include "idscreen_source_callback.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenSourceCallbackStubVTest : public DScreenSourceCallbackStub {
public:
    ~DScreenSourceCallbackStubVTest() {}
    int32_t OnNotifyRegResultInner(MessageParcel &data, MessageParcel &reply,
        MessageOption &option)
    {
        return 0;
    }
    int32_t OnNotifyUnregResultInner(MessageParcel &data, MessageParcel &reply,
        MessageOption &option)
    {
        return 0;
    }
};

class IDScreenSourceCallbackTest : public IDScreenSourceCallback {
public:
    ~IDScreenSourceCallbackTest() {}
    sptr<IRemoteObject> AsObject()
    {
        return nullptr;
    }

    int32_t OnNotifyRegResult(const std::string &devId, const std::string &dhId,
        const std::string &reqId, int32_t status, const std::string &data)
    {
        return 0;
    }

    int32_t OnNotifyUnregResult(const std::string &devId, const std::string &dhId,
        const std::string &reqId, int32_t status, const std::string &data)
    {
        return 0;
    }
};

class RegisterCallbackTest : public RegisterCallback {
public:
    RegisterCallbackTest() = default;
    virtual ~RegisterCallbackTest() = default;

    int32_t OnRegisterResult(const std::string &uuid, const std::string &dhId, int32_t status,
        const std::string &data)
    {
        return 0;
    }
};


class UnregisterCallbackTest : public UnregisterCallback {
public:
    UnregisterCallbackTest() = default;
    virtual ~UnregisterCallbackTest() = default;

    int32_t OnUnregisterResult(const std::string &uuid, const std::string &dhId, int32_t status,
        const std::string &data)
    {
        return 0;
    }
};
}
}


#endif