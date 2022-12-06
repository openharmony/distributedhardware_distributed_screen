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

#ifndef OHOS_DSCREEN_SINK_STUB_TEST_H
#define OHOS_DSCREEN_SINK_STUB_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "dscreen_errcode.h"
#include "dscreen_sink_proxy.h"
#include "dscreen_sink_stub.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenSinkStubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    class TestDScreenSinkStub : public OHOS::DistributedHardware::DScreenSinkStub {
    public:
        TestDScreenSinkStub() = default;
        virtual ~TestDScreenSinkStub() = default;
        int32_t InitSink(const std::string &params);
        int32_t ReleaseSink();
        int32_t SubscribeLocalHardware(const std::string &dhId, const std::string &param);
        int32_t UnsubscribeLocalHardware(const std::string &dhId);
        void DScreenNotify(const std::string &devId, int32_t eventCode, const std::string &eventContent);

        std::string devId_;
        int32_t eventCode_;
        std::string eventContent_;
    };
};
} // namespace DistributedHardware
} // namespace OHOS
#endif