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

#ifndef OHOS_DSCREEN_SOURCE_CALLBACK_STUB_TEST_H
#define OHOS_DSCREEN_SOURCE_CALLBACK_STUB_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "dscreen_errcode.h"
#include "dscreen_source_callback_proxy.h"
#include "dscreen_source_callback_stub.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenSourceCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    class TestDScreenSourceCallbackStub : public OHOS::DistributedHardware::DScreenSourceCallbackStub {
    public:
        TestDScreenSourceCallbackStub() = default;
        virtual ~TestDScreenSourceCallbackStub() = default;
        int32_t OnNotifyRegResult(const std::string &devId, const std::string &dhId,
            const std::string &reqId, int32_t status, const std::string &data);
        int32_t OnNotifyUnregResult(const std::string &devId, const std::string &dhId,
            const std::string &reqId, int32_t status, const std::string &data);
    };
};
} // namespace DistributedHardware
} // namespace OHOS
#endif