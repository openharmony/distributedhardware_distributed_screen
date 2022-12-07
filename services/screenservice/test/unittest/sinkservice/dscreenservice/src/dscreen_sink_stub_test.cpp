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

#include "dscreen_sink_stub_test.h"

#include <memory>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void DScreenSinkStubTest::SetUpTestCase(void)
{
}

void DScreenSinkStubTest::TearDownTestCase(void)
{
}

void DScreenSinkStubTest::SetUp(void)
{
}

void DScreenSinkStubTest::TearDown(void)
{
}

int32_t DScreenSinkStubTest::TestDScreenSinkStub::InitSink(const std::string &params)
{
    (void)params;
    return DH_SUCCESS;
}

int32_t DScreenSinkStubTest::TestDScreenSinkStub::ReleaseSink()
{
    return DH_SUCCESS;
}

int32_t DScreenSinkStubTest::TestDScreenSinkStub::SubscribeLocalHardware(const std::string &dhId,
    const std::string &param)
{
    (void)dhId;
    (void)param;
    return DH_SUCCESS;
}

int32_t DScreenSinkStubTest::TestDScreenSinkStub::UnsubscribeLocalHardware(const std::string &dhId)
{
    (void)dhId;
    return DH_SUCCESS;
}

void DScreenSinkStubTest::TestDScreenSinkStub::DScreenNotify(const std::string &devId, int32_t eventCode,
    const std::string &eventContent)
{
    devId_ = devId;
    eventCode_ = eventCode;
    eventContent_ = eventContent;
}

/**
 * @tc.name: InitSink_001
 * @tc.desc: Invoke the InitSink ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkStubTest, InitSink_001, TestSize.Level1)
{
    sptr<IRemoteObject> sinkStubPtr = new TestDScreenSinkStub();
    DScreenSinkProxy sinkProxy(sinkStubPtr);

    int32_t ret = sinkProxy.InitSink("params000");
    EXPECT_EQ(DH_SUCCESS, ret);
    
    ret = sinkProxy.InitSink("");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);
}

/**
 * @tc.name: ReleaseSink_001
 * @tc.desc: Invoke the ReleaseSink ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkStubTest, ReleaseSink_001, TestSize.Level1)
{
    sptr<IRemoteObject> sinkStubPtr = new TestDScreenSinkStub();
    DScreenSinkProxy sinkProxy(sinkStubPtr);

    int32_t ret = sinkProxy.ReleaseSink();
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: SubscribeLocalHardware_001
 * @tc.desc: Invoke the SubscribeLocalHardware ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkStubTest, SubscribeLocalHardware_001, TestSize.Level1)
{
    sptr<IRemoteObject> sinkStubPtr = new TestDScreenSinkStub();
    DScreenSinkProxy sinkProxy(sinkStubPtr);

    int32_t ret = sinkProxy.SubscribeLocalHardware("dhId000", "param000");
    EXPECT_EQ(DH_SUCCESS, ret);

    ret = sinkProxy.SubscribeLocalHardware("", "param000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sinkProxy.SubscribeLocalHardware("dhId000", "");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    std::string dhId = R"(dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dh
        Id000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d
        hId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000)";
    ret = sinkProxy.SubscribeLocalHardware(dhId, "param000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);
}

/**
 * @tc.name: UnsubscribeLocalHardware_001
 * @tc.desc: Invoke the UnsubscribeLocalHardware ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkStubTest, UnsubscribeLocalHardware_001, TestSize.Level1)
{
    sptr<IRemoteObject> sinkStubPtr = new TestDScreenSinkStub();
    DScreenSinkProxy sinkProxy(sinkStubPtr);

    int32_t ret = sinkProxy.UnsubscribeLocalHardware("dhId000");
    EXPECT_EQ(DH_SUCCESS, ret);

    ret = sinkProxy.UnsubscribeLocalHardware("");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    std::string dhId = R"(dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dh
        Id000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d
        hId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000)";
    ret = sinkProxy.UnsubscribeLocalHardware(dhId);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);
}

/**
 * @tc.name: DScreenNotify_001
 * @tc.desc: Invoke the DScreenNotify ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkStubTest, DScreenNotify_001, TestSize.Level1)
{
    sptr<IRemoteObject> sinkStubPtr = new TestDScreenSinkStub();
    DScreenSinkProxy sinkProxy(sinkStubPtr);

    sinkProxy.DScreenNotify("", 0, "eventContent000");

    sinkProxy.DScreenNotify("devId000", 0, "");

    std::string longString = R"(dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId00
        Id000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d0dh
        hId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId0000dh)";
    sinkProxy.DScreenNotify(longString, 0, "eventContent000");

    std::string devId = "devId000";
    int32_t eventCode = 0;
    std::string eventContent = "eventContent000";
    sinkProxy.DScreenNotify(devId, eventCode, eventContent);
    EXPECT_STREQ(devId.c_str(), ((sptr<TestDScreenSinkStub> &)sinkStubPtr)->devId_.c_str());
    EXPECT_EQ(eventCode, ((sptr<TestDScreenSinkStub> &)sinkStubPtr)->eventCode_);
    EXPECT_STREQ(eventContent.c_str(), ((sptr<TestDScreenSinkStub> &)sinkStubPtr)->eventContent_.c_str());
}
}
}