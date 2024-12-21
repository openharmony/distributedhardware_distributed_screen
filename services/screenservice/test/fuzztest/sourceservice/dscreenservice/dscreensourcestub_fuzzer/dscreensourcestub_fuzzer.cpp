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

#include "dscreensourcestub_fuzzer.h"
#include "dscreen_source_stub.h"
#include "fuzzer/FuzzedDataProvider.h"
#include "idscreen_source.h"

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
    int32_t RegisterDistributedHardware(const std::string &devId, const std::string &dhId, const EnableParam &param,
        const std::string &reqId) override
    {
        return 0;
    };
    int32_t UnregisterDistributedHardware(const std::string &devId, const std::string &dhId,
        const std::string &reqId) override
    {
        return 0;
    };
    int32_t ConfigDistributedHardware(const std::string &devId, const std::string &dhId, const std::string &key,
        const std::string &value) override
    {
        return 0;
    };
    void DScreenNotify(const std::string &devId, int32_t eventCode, const std::string &eventContent) override{};
};

void DscreenSourceStubFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider dataProvider(data, size);
    uint32_t code = dataProvider.ConsumeIntegral<uint32_t>();
    uint32_t status = dataProvider.ConsumeIntegral<int32_t>();
    std::string dhId(dataProvider.ConsumeRandomLengthString());
    std::string devId(dataProvider.ConsumeRandomLengthString());
    std::string reqId(dataProvider.ConsumeRandomLengthString());
    std::string dataStr(dataProvider.ConsumeRandomLengthString());
    std::string version(dataProvider.ConsumeRandomLengthString());
    std::string attrs(dataProvider.ConsumeRandomLengthString());

    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    pdata.WriteInt32(status);
    pdata.WriteString(devId);
    pdata.WriteString(dhId);
    pdata.WriteString(reqId);
    pdata.WriteString(dataStr);

    std::shared_ptr<DScreenSourceStub> sourceStubPtr = std::make_shared<DScreenSourceStubFuzzTest>();
    sourceStubPtr->OnRemoteRequest(code, pdata, reply, option);
    sourceStubPtr->InitSourceInner(pdata, reply, option);
    sourceStubPtr->ReleaseSourceInner(pdata, reply, option);
    sourceStubPtr->RegisterDistributedHardwareInner(pdata, reply, option);
    sourceStubPtr->UnregisterDistributedHardwareInner(pdata, reply, option);
    sourceStubPtr->ConfigDistributedHardwareInner(pdata, reply, option);
    sourceStubPtr->DScreenNotifyInner(pdata, reply, option);
    sourceStubPtr->CheckRegParams(devId, dhId, version, attrs, reqId);
    sourceStubPtr->CheckUnregParams(devId, dhId, reqId);
}
} // namespace DistributedHardware
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::DscreenSourceStubFuzzTest(data, size);
    return 0;
}
