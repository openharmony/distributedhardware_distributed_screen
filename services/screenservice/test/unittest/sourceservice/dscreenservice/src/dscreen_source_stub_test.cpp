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

#include "dscreen_source_stub_test.h"

#include <memory>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void DScreenSourceStubTest::SetUpTestCase(void)
{
}

void DScreenSourceStubTest::TearDownTestCase(void)
{
}

void DScreenSourceStubTest::SetUp(void)
{
}

void DScreenSourceStubTest::TearDown(void)
{
}

int32_t DScreenSourceStubTest::TestDScreenSourceStub::InitSource(const std::string &params,
    const sptr<IDScreenSourceCallback> &callback)
{
    (void)params;
    (void)callback;
    return DH_SUCCESS;
}

int32_t DScreenSourceStubTest::TestDScreenSourceStub::ReleaseSource()
{
    return DH_SUCCESS;
}

int32_t DScreenSourceStubTest::TestDScreenSourceStub::RegisterDistributedHardware(const std::string &devId,
    const std::string &dhId, const EnableParam &param, const std::string &reqId)
{
    (void)devId;
    (void)dhId;
    (void)param;
    (void)reqId;
    return DH_SUCCESS;
}

int32_t DScreenSourceStubTest::TestDScreenSourceStub::UnregisterDistributedHardware(const std::string &devId,
    const std::string &dhId, const std::string &reqId)
{
    (void)devId;
    (void)dhId;
    (void)reqId;
    return DH_SUCCESS;
}

int32_t DScreenSourceStubTest::TestDScreenSourceStub::ConfigDistributedHardware(const std::string &devId,
    const std::string &dhId, const std::string &key, const std::string &value)
{
    (void)devId;
    (void)dhId;
    (void)key;
    (void)value;
    return DH_SUCCESS;
}

void DScreenSourceStubTest::TestDScreenSourceStub::DScreenNotify(const std::string &devId, int32_t eventCode,
    const std::string &eventContent)
{
    devId_ = devId;
    eventCode_ = eventCode;
    eventContent_ = eventContent;
}

/**
 * @tc.name: InitSource_001
 * @tc.desc: Invoke the InitSource ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, InitSource_001, TestSize.Level1)
{
    sptr<IRemoteObject> sourceStubPtr = new TestDScreenSourceStub();
    DScreenSourceProxy sourceProxy(sourceStubPtr);

    sptr<DScreenSourceCallback> callback;
    int32_t ret = sourceProxy.InitSource("", callback);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.InitSource("params000", callback);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    callback = new DScreenSourceCallback();
    ret = sourceProxy.InitSource("params000", callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: ReleaseSource_001
 * @tc.desc: Invoke the ReleaseSource ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, ReleaseSource_001, TestSize.Level1)
{
    sptr<IRemoteObject> sourceStubPtr = new TestDScreenSourceStub();
    DScreenSourceProxy sourceProxy(sourceStubPtr);
    int32_t ret = sourceProxy.ReleaseSource();
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: RegisterDistributedHardware_001
 * @tc.desc: Invoke the RegisterDistributedHardware ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, RegisterDistributedHardware_001, TestSize.Level1)
{
    sptr<IRemoteObject> sourceStubPtr = new TestDScreenSourceStub();
    DScreenSourceProxy sourceProxy(sourceStubPtr);

    EnableParam param;
    param.version = "";
    param.attrs = "";
    int32_t ret = sourceProxy.RegisterDistributedHardware("", "dhId000", param, "reqId000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    std::string devId = R"(dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d
        Id000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d
        hId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000)";
    ret = sourceProxy.RegisterDistributedHardware(devId, "dhId000", param, "reqId000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.RegisterDistributedHardware("devId000", "", param, "reqId000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.RegisterDistributedHardware("devId000", devId, param, "reqId000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.RegisterDistributedHardware("devId000", "dhId000", param, "");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.RegisterDistributedHardware("devId000", "dhId000", param, devId);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.RegisterDistributedHardware("devId000", "dhId000", param, "reqId000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    param.version = "1";
    ret = sourceProxy.RegisterDistributedHardware("devId000", "dhId000", param, "reqId000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    param.attrs = "attrs";
    ret = sourceProxy.RegisterDistributedHardware("devId000", "dhId000", param, "reqId000");
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: UnregisterDistributedHardware_001
 * @tc.desc: Invoke the UnregisterDistributedHardware ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, UnregisterDistributedHardware_001, TestSize.Level1)
{
    sptr<IRemoteObject> sourceStubPtr = new TestDScreenSourceStub();
    DScreenSourceProxy sourceProxy(sourceStubPtr);

    int32_t ret = sourceProxy.UnregisterDistributedHardware("", "dhId000", "reqId000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    std::string devId = R"(dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d
        Id000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d
        hId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000)";
    ret = sourceProxy.UnregisterDistributedHardware(devId, "dhId000", "reqId000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.UnregisterDistributedHardware("devId000", "", "reqId000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.UnregisterDistributedHardware("devId000", devId, "reqId000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.UnregisterDistributedHardware("devId000", "dhId000", "");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.UnregisterDistributedHardware("devId000", "dhId000", devId);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.UnregisterDistributedHardware("devId000", "dhId000", "reqId000");
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: ConfigDistributedHardware_001
 * @tc.desc: Invoke the ConfigDistributedHardware ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, ConfigDistributedHardware_001, TestSize.Level1)
{
    sptr<IRemoteObject> sourceStubPtr = new TestDScreenSourceStub();
    DScreenSourceProxy sourceProxy(sourceStubPtr);

    int32_t ret = sourceProxy.ConfigDistributedHardware("", "dhId000", "key000", "value000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    std::string devId = R"(dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d
        Id000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d
        hId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000)";
    ret = sourceProxy.ConfigDistributedHardware(devId, "dhId000", "key000", "value000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.ConfigDistributedHardware("devId000", "", "key000", "value000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.ConfigDistributedHardware("devId000", devId, "key000", "value000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.ConfigDistributedHardware("devId000", "dhId000", "", "value000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.ConfigDistributedHardware("devId000", "dhId000", "key000", "");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceProxy.ConfigDistributedHardware("devId000", "dhId000", "key000", "value000");
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: DScreenNotify_001
 * @tc.desc: Invoke the DScreenNotify ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, DScreenNotify_001, TestSize.Level1)
{
    sptr<IRemoteObject> sourceStubPtr = new TestDScreenSourceStub();
    DScreenSourceProxy sourceProxy(sourceStubPtr);

    sourceProxy.DScreenNotify("", 0, "eventContent000");

    std::string longString = R"(dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d
        Id000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d
        hId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000)";
    sourceProxy.DScreenNotify(longString, 0, "eventContent000");

    sourceProxy.DScreenNotify("devId000", 0, "");

    std::string devId = "devId000";
    int32_t eventCode = 0;
    std::string eventContent = "eventContent000";
    sourceProxy.DScreenNotify(devId, eventCode, eventContent);
    EXPECT_STREQ(devId.c_str(), ((sptr<TestDScreenSourceStub> &)sourceStubPtr)->devId_.c_str());
    EXPECT_EQ(eventCode, ((sptr<TestDScreenSourceStub> &)sourceStubPtr)->eventCode_);
    EXPECT_STREQ(eventContent.c_str(), ((sptr<TestDScreenSourceStub> &)sourceStubPtr)->eventContent_.c_str());
}
}
}