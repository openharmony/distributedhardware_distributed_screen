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

#include "dscreensinkstub_fuzzer.h"

#include "dscreen_sink_stub.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenSinkStubFuzzTest : public OHOS::DistributedHardware::DScreenSinkStub {
public:
    DScreenSinkStubFuzzTest() = default;
    ~DScreenSinkStubFuzzTest() = default;
    int32_t InitSink(const std::string &params) override
    {
        return 0;
    };
    int32_t ReleaseSink() override
    {
        return 0;
    };
    int32_t SubscribeLocalHardware(const std::string &dhId, const std::string &param) override
    {
        return 0;
    };
    int32_t UnsubscribeLocalHardware(const std::string &dhId) override
    {
        return 0;
    };
    void DScreenNotify(const std::string &devId, int32_t eventCode, const std::string &eventContent) override {};
};


void DscreenSinkStubFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }

    MessageParcel pdata;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data)) % 2;
    uint32_t status = *(reinterpret_cast<const int32_t*>(data));
    std::string dhId(reinterpret_cast<const char*>(data), size);
    std::string devId(reinterpret_cast<const char*>(data), size);
    std::string reqId(reinterpret_cast<const char*>(data), size);
    std::string dataStr(reinterpret_cast<const char*>(data), size);
    pdata.WriteInt32(status);
    pdata.WriteString(devId);
    pdata.WriteString(dhId);
    pdata.WriteString(reqId);
    pdata.WriteString(dataStr);

    sptr<DScreenSinkStubFuzzTest> sourceStubPtr(new DScreenSinkStubFuzzTest());
    sourceStubPtr->OnRemoteRequest(code, pdata, reply, option);
    sourceStubPtr->InitSinkInner(pdata, reply, option);
    sourceStubPtr->ReleaseSinkInner(pdata, reply, option);
    sourceStubPtr->SubscribeDistributedHardwareInner(pdata, reply, option);
    sourceStubPtr->UnsubscribeDistributedHardwareInner(pdata, reply, option);
    sourceStubPtr->DScreenNotifyInner(pdata, reply, option);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::DscreenSinkStubFuzzTest(data, size);
    return 0;
}
