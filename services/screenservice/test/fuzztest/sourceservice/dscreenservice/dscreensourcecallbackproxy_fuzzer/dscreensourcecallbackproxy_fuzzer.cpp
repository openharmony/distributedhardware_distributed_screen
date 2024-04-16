/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "dscreensourcecallbackproxy_fuzzer.h"

#include "idscreen_source.h"
#include "dscreen_source_callback_proxy.h"
#include "idscreen_source.h"
#include "dscreen_source_stub.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenSourceStubFuzzTest : public OHOS::DistributedHardware::DScreenSourceStub {
public:
    DScreenSourceStubFuzzTest() = default;
    ~DScreenSourceStubFuzzTest() = default;
    int32_t InitSource(const std::string &params, const sptr<IDScreenSourceCallback> &callback) override
    {
        return 0;
    };
    int32_t ReleaseSource() override
    {
        return 0;
    };
    int32_t RegisterDistributedHardware(const std::string &devId, const std::string &dhId,
        const EnableParam &param, const std::string &reqId) override
    {
        return 0;
    };
    void DScreenNotify(const std::string &devId, int32_t eventCode,
        const std::string &eventContent) override {};
    int32_t UnregisterDistributedHardware(const std::string &devId, const std::string &dhId,
        const std::string &reqId) override
    {
        return 0;
    };
    int32_t ConfigDistributedHardware(const std::string &devId, const std::string &dhId,
        const std::string &key, const std::string &value) override
    {
        return 0;
    };
};


void DscreenSourceCallbackProxyFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    int32_t status = *(reinterpret_cast<const int32_t*>(data));
    std::string dhId(reinterpret_cast<const char*>(data), size);
    std::string devId(reinterpret_cast<const char*>(data), size);
    std::string reqId(reinterpret_cast<const char*>(data), size);
    std::string resultData(reinterpret_cast<const char*>(data), size);

    sptr<IRemoteObject> dscreenSourceStubPtr(new DScreenSourceStubFuzzTest());
    sptr<DScreenSourceCallbackProxy> sourceCbkProxy(new DScreenSourceCallbackProxy(dscreenSourceStubPtr));
    sourceCbkProxy->OnNotifyRegResult(devId, dhId, reqId, status, resultData);
    sourceCbkProxy->OnNotifyUnregResult(devId, dhId, reqId, status, resultData);
    sourceCbkProxy->CheckParams(devId, dhId, reqId, resultData);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::DscreenSourceCallbackProxyFuzzTest(data, size);
    return 0;
}
