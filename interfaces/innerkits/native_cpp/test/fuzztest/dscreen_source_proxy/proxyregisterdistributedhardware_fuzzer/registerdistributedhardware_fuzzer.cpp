/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include <cstddef>
#include <cstdint>

#include "registerdistributedhardware_fuzzer.h"
#include "dscreen_constants.h"
#include "dscreen_source_callback.h"
#include "dscreen_source_proxy.h"
#include "dscreen_source_stub.h"
#include "fuzzer/FuzzedDataProvider.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"

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
    void DScreenNotify(const std::string &devId, int32_t eventCode, const std::string &eventContent) override{};
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
};

void RegisterDistributedHardwareFuzzTest(const uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return;
    }

    FuzzedDataProvider dataProvider(data, size);
    std::string devId(dataProvider.ConsumeRandomLengthString());
    std::string dhId(dataProvider.ConsumeRandomLengthString());
    std::string reqId(dataProvider.ConsumeRandomLengthString());
    std::string version(dataProvider.ConsumeRandomLengthString());
    std::string attrs(dataProvider.ConsumeRandomLengthString());
    EnableParam param;
    param.sinkVersion = version;
    param.sinkAttrs = attrs;

    sptr<IRemoteObject> remoteObject(new DScreenSourceStubFuzzTest());
    std::shared_ptr<IDScreenSource> dscreenSourceProxy = std::make_shared<DScreenSourceProxy>(remoteObject);
    dscreenSourceProxy->RegisterDistributedHardware(devId, dhId, param, reqId);
}
} // namespace DistributedHardware
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::RegisterDistributedHardwareFuzzTest(data, size);
    return 0;
}