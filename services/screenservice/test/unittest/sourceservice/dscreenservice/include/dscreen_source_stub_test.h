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

#ifndef OHOS_DSCREEN_SOURCE_STUB_TEST_H
#define OHOS_DSCREEN_SOURCE_STUB_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "dscreen_errcode.h"
#include "dscreen_source_callback_proxy.h"
#include "dscreen_source_callback.h"
#include "dscreen_source_proxy.h"
#include "dscreen_source_stub.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenSourceStubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    class TestDScreenSourceStub : public OHOS::DistributedHardware::DScreenSourceStub {
    public:
        TestDScreenSourceStub() = default;
        virtual ~TestDScreenSourceStub() = default;
        int32_t InitSource(const std::string &params, const sptr<IDScreenSourceCallback> &callback);
        int32_t ReleaseSource();
        int32_t RegisterDistributedHardware(const std::string &devId, const std::string &dhId,
            const EnableParam &param, const std::string &reqId);
        int32_t UnregisterDistributedHardware(const std::string &devId, const std::string &dhId,
            const std::string &reqId);
        int32_t ConfigDistributedHardware(const std::string &devId, const std::string &dhId,
            const std::string &key, const std::string &value);
        void DScreenNotify(const std::string &devId, int32_t eventCode,
            const std::string &eventContent);

        std::string devId_;
        int32_t eventCode_;
        std::string eventContent_;
    };
};
} // namespace DistributedHardware
} // namespace OHOS
#endif