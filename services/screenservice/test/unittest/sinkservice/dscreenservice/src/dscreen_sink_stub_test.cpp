/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include <memory>
#include "dscreen_sink_stub_test.h"
#include "accesstoken_kit.h"
#include "dscreen_constants.h"

using namespace testing;
using namespace testing::ext;

static int g_mockVerifyAccessTokenReturnIntValue = -1;

namespace OHOS {
namespace Security::AccessToken {
int AccessTokenKit::VerifyAccessToken(AccessTokenID tokenID, const std::string& permissionName)
{
    return g_mockVerifyAccessTokenReturnIntValue;
}
}

namespace DistributedHardware {
void DScreenSinkStubTest::SetUpTestCase(void)
{
}

void DScreenSinkStubTest::TearDownTestCase(void)
{
}

void DScreenSinkStubTest::SetUp(void)
{
    g_mockVerifyAccessTokenReturnIntValue = -1;
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
    sptr<IRemoteObject> sinkStubPtr(new TestDScreenSinkStub());
    DScreenSinkProxy sinkProxy(sinkStubPtr);

    int32_t ret = sinkProxy.InitSink("params000");
    EXPECT_EQ(DH_SUCCESS, ret);

    ret = sinkProxy.InitSink("");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    std::string param(PARAM_MAX_SIZE + 1, 'a');
    ret = sinkProxy.InitSink(param);
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
    sptr<IRemoteObject> sinkStubPtr(new TestDScreenSinkStub());
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
    sptr<IRemoteObject> sinkStubPtr(new TestDScreenSinkStub());
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
 * @tc.name: SubscribeLocalHardware_002
 * @tc.desc: Test the SubscribeLocalHardware method when param exceeds the maximum size.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkStubTest, SubscribeLocalHardware_002, TestSize.Level1)
{
    sptr<IRemoteObject> sinkStubPtr(new TestDScreenSinkStub());
    DScreenSinkProxy sinkProxy(sinkStubPtr);
    std::string param(PARAM_MAX_SIZE + 1, 'a');
    int32_t ret = sinkProxy.SubscribeLocalHardware("dhId000", param);
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
    sptr<IRemoteObject> sinkStubPtr(new TestDScreenSinkStub());
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
 * @tc.name: UnsubscribeLocalHardware_002
 * @tc.desc: Invoke the UnsubscribeLocalHardware ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkStubTest, UnsubscribeLocalHardware_002, TestSize.Level1)
{
    sptr<IRemoteObject> sinkStubPtr(new TestDScreenSinkStub());
    DScreenSinkProxy sinkProxy(sinkStubPtr);
    std::string dhId(DID_MAX_SIZE + 1, 'a');
    int32_t ret = sinkProxy.UnsubscribeLocalHardware(dhId);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    dhId.clear();
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
    sptr<IRemoteObject> sinkStubPtr(new TestDScreenSinkStub());
    DScreenSinkProxy sinkProxy(sinkStubPtr);

    sinkProxy.DScreenNotify("", 0, "eventContent000");

    sinkProxy.DScreenNotify("devId000", 0, "");

    std::string longString = R"(dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId00
        Id000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d0dh
        hId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId0000dh)";
    sinkProxy.DScreenNotify(longString, 0, "eventContent000");

    std::string devId = "devId000";
    int32_t eventCode = 0;
    std::string exceedParamMaxSizeStr(PARAM_MAX_SIZE + 1, 'a');
    sinkProxy.DScreenNotify(devId, eventCode, exceedParamMaxSizeStr);

    std::string eventContent = "eventContent000";
    sinkProxy.DScreenNotify(devId, eventCode, eventContent);
    EXPECT_STREQ(devId.c_str(), ((sptr<TestDScreenSinkStub> &)sinkStubPtr)->devId_.c_str());
    EXPECT_EQ(eventCode, ((sptr<TestDScreenSinkStub> &)sinkStubPtr)->eventCode_);
    EXPECT_STREQ(eventContent.c_str(), ((sptr<TestDScreenSinkStub> &)sinkStubPtr)->eventContent_.c_str());
}

/**
 * @tc.name: DScreenNotifyInner_001
 * @tc.desc: Verify the DScreenNotifyInner function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkStubTest, DScreenNotifyInner_001, TestSize.Level1)
{
    sptr<DScreenSinkStub> sinkStub(new TestDScreenSinkStub());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::string devId = "";
    int32_t eventCode = -1;
    std::string eventContent = "";
    data.WriteString(devId);
    data.WriteInt32(eventCode);
    data.WriteString(eventContent);
    int32_t ret = sinkStub->DScreenNotifyInner(data, reply, option);
    EXPECT_EQ(ret, ERR_DH_SCREEN_INPUT_PARAM_INVALID);
}

/**
 * @tc.name: DScreenNotifyInner_002
 * @tc.desc: Verify the DScreenNotifyInner function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkStubTest, DScreenNotifyInner_002, TestSize.Level1)
{
    sptr<DScreenSinkStub> sinkStub(new TestDScreenSinkStub());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::string exceedDidMaxSizeStr(DID_MAX_SIZE + 1, 'a');
    int32_t eventCode = -1;
    std::string eventContent = "";
    data.WriteString(exceedDidMaxSizeStr);
    data.WriteInt32(eventCode);
    data.WriteString(eventContent);
    int32_t ret = sinkStub->DScreenNotifyInner(data, reply, option);
    EXPECT_EQ(ret, ERR_DH_SCREEN_INPUT_PARAM_INVALID);
}

/**
 * @tc.name: DScreenNotifyInner_003
 * @tc.desc: Verify the DScreenNotifyInner function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkStubTest, DScreenNotifyInner_003, TestSize.Level1)
{
    sptr<DScreenSinkStub> sinkStub(new TestDScreenSinkStub());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::string devId = "test";
    int32_t eventCode = -1;
    std::string eventContent = "";
    data.WriteString(devId);
    data.WriteInt32(eventCode);
    data.WriteString(eventContent);
    int32_t ret = sinkStub->DScreenNotifyInner(data, reply, option);
    EXPECT_EQ(ret, ERR_DH_SCREEN_INPUT_PARAM_INVALID);
}

/**
 * @tc.name: DScreenNotifyInner_004
 * @tc.desc: Verify the DScreenNotifyInner function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkStubTest, DScreenNotifyInner_004, TestSize.Level1)
{
    sptr<DScreenSinkStub> sinkStub(new TestDScreenSinkStub());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::string devId = "test";
    int32_t eventCode = -1;
    std::string exceedParamMaxSizeStr(PARAM_MAX_SIZE + 1, 'a');
    data.WriteString(devId);
    data.WriteInt32(eventCode);
    data.WriteString(exceedParamMaxSizeStr);
    int32_t ret = sinkStub->DScreenNotifyInner(data, reply, option);
    EXPECT_EQ(ret, ERR_DH_SCREEN_INPUT_PARAM_INVALID);
}

/**
 * @tc.name: DScreenNotifyInner_005
 * @tc.desc: Verify the DScreenNotifyInner function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkStubTest, DScreenNotifyInner_005, TestSize.Level1)
{
    sptr<DScreenSinkStub> sinkStub(new TestDScreenSinkStub());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::string devId = "test";
    int32_t eventCode = -1;
    std::string eventContent = "test";
    data.WriteString(devId);
    data.WriteInt32(eventCode);
    data.WriteString(eventContent);
    int32_t ret = sinkStub->DScreenNotifyInner(data, reply, option);
    EXPECT_EQ(ret, DH_SUCCESS);
}

/**
 * @tc.name: InitSinkInner_001
 * @tc.desc: Verify the InitSinkInner function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkStubTest, InitSinkInner_001, TestSize.Level1)
{
    sptr<DScreenSinkStub> sinkStub(new TestDScreenSinkStub());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::string param = "";
    data.WriteString(param);
    g_mockVerifyAccessTokenReturnIntValue = 0;
    int32_t ret = sinkStub->InitSinkInner(data, reply, option);
    EXPECT_EQ(ret, ERR_DH_SCREEN_INPUT_PARAM_INVALID);

    param = "param";
    data.WriteString(param);
    ret = sinkStub->InitSinkInner(data, reply, option);
    EXPECT_EQ(ret, DH_SUCCESS);
}

/**
 * @tc.name: ReleaseSinkInner_001
 * @tc.desc: Verify the ReleaseSinkInner function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkStubTest, ReleaseSinkInner_001, TestSize.Level1)
{
    sptr<DScreenSinkStub> sinkStub(new TestDScreenSinkStub());
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    g_mockVerifyAccessTokenReturnIntValue = 0;
    int32_t ret = sinkStub->ReleaseSinkInner(data, reply, option);
    EXPECT_EQ(ret, DH_SUCCESS);
}
}  // namespace DistributedHardware
}  // namespace OHOS
