/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "dscreen_source_stub_test.h"
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
void DScreenSourceStubTest::SetUpTestCase(void)
{
}

void DScreenSourceStubTest::TearDownTestCase(void)
{
}

void DScreenSourceStubTest::SetUp(void)
{
    g_mockVerifyAccessTokenReturnIntValue = -1;
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
    sptr<IRemoteObject> sourceStubPtr(new TestDScreenSourceStub());
    DScreenSourceProxy sourceProxy(sourceStubPtr);

    sptr<DScreenSourceCallback> callback;
    int32_t ret = sourceProxy.InitSource("", callback);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    std::string params(PARAM_MAX_SIZE + 1, 'a');
    ret = sourceProxy.InitSource(params, callback);
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
    sptr<IRemoteObject> sourceStubPtr(new TestDScreenSourceStub());
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
    sptr<IRemoteObject> sourceStubPtr(new TestDScreenSourceStub());
    DScreenSourceProxy sourceProxy(sourceStubPtr);

    EnableParam param;
    param.sinkVersion = "";
    param.sinkAttrs = "";
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

    param.sinkVersion = "1";
    ret = sourceProxy.RegisterDistributedHardware("devId000", "dhId000", param, "reqId000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    param.sinkAttrs = "attrs";
    ret = sourceProxy.RegisterDistributedHardware("devId000", "dhId000", param, "reqId000");
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: CheckRegParams_001
 * @tc.desc: Test CheckRegParams with valid parameters.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, DScreenSourceProxy_CheckRegParams_001, TestSize.Level1)
{
    std::string exceedParamMaxSizeStr(PARAM_MAX_SIZE + 1, 'a');
    EnableParam validParam;
    validParam.sinkVersion = exceedParamMaxSizeStr;
    validParam.sinkAttrs = exceedParamMaxSizeStr;
    std::string devId = "devId";
    std::string dhId = "dhId";
    std::string reqId = "reqId";

    sptr<IRemoteObject> sourceStubPtr(new TestDScreenSourceStub());
    DScreenSourceProxy sourceProxy(sourceStubPtr);
    EXPECT_FALSE(sourceProxy.CheckRegParams(devId, dhId, validParam, reqId));

    validParam.sinkVersion = "sinkVersion";
    EXPECT_FALSE(sourceProxy.CheckRegParams(devId, dhId, validParam, reqId));

    validParam.sinkAttrs = "sinkAttrs";
    EXPECT_TRUE(sourceProxy.CheckRegParams(devId, dhId, validParam, reqId));
}

/**
 * @tc.name: CheckConfigParams_001
 * @tc.desc: Test CheckConfigParams with valid parameters.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, DScreenSourceProxy_CheckConfigParams_001, TestSize.Level1)
{
    std::string exceedParamMaxSizeStr(PARAM_MAX_SIZE + 1, 'a');
    std::string devId = "devId";
    std::string dhId = "dhId";
    std::string key = exceedParamMaxSizeStr;
    std::string value = exceedParamMaxSizeStr;

    sptr<IRemoteObject> sourceStubPtr(new TestDScreenSourceStub());
    DScreenSourceProxy sourceProxy(sourceStubPtr);
    EXPECT_FALSE(sourceProxy.CheckConfigParams(devId, dhId, key, value));

    key = "key";
    EXPECT_FALSE(sourceProxy.CheckConfigParams(devId, dhId, key, value));

    value = "value";
    EXPECT_TRUE(sourceProxy.CheckConfigParams(devId, dhId, key, value));
}

/**
 * @tc.name: UnregisterDistributedHardware_001
 * @tc.desc: Invoke the UnregisterDistributedHardware ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, UnregisterDistributedHardware_001, TestSize.Level1)
{
    sptr<IRemoteObject> sourceStubPtr(new TestDScreenSourceStub());
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
    sptr<IRemoteObject> sourceStubPtr(new TestDScreenSourceStub());
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
    std::string exceedDidMaxSizeStr(DID_MAX_SIZE + 1, 'a');
    std::string exceedParamMaxSizeStr(PARAM_MAX_SIZE + 1, 'a');
    int32_t eventCode = 0;
    std::string eventContent = "";
    std::string devId = "";

    sptr<IRemoteObject> sourceStubPtr(new TestDScreenSourceStub());
    DScreenSourceProxy sourceProxy(sourceStubPtr);
    sourceProxy.DScreenNotify(devId, eventCode, eventContent);

    devId = exceedDidMaxSizeStr;
    sourceProxy.DScreenNotify(devId, eventCode, eventContent);

    devId = "devId000";
    sourceProxy.DScreenNotify(devId, eventCode, eventContent);

    eventContent = exceedParamMaxSizeStr;
    sourceProxy.DScreenNotify(devId, eventCode, eventContent);

    eventContent = "eventContent000";
    sourceProxy.DScreenNotify(devId, eventCode, eventContent);
    EXPECT_STREQ(devId.c_str(), ((sptr<TestDScreenSourceStub> &)sourceStubPtr)->devId_.c_str());
    EXPECT_EQ(eventCode, ((sptr<TestDScreenSourceStub> &)sourceStubPtr)->eventCode_);
    EXPECT_STREQ(eventContent.c_str(), ((sptr<TestDScreenSourceStub> &)sourceStubPtr)->eventContent_.c_str());
}

/**
 * @tc.name: OnRemoteRequest_001
 * @tc.desc: Test with valid interface descriptor and request code.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, OnRemoteRequest_001, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    uint32_t requestCode = 1;
    MessageParcel data;
    data.WriteInterfaceToken(stubPtr->GetDescriptor());
    data.WriteUint32(requestCode);
    MessageParcel reply;
    MessageOption option;
    int32_t result = stubPtr->OnRemoteRequest(requestCode, data, reply, option);
    EXPECT_EQ(SESSION_UNOPEN_ERR, result);
}

/**
 * @tc.name: OnRemoteRequest_002
 * @tc.desc: Test with invalid interface descriptor.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, OnRemoteRequest_002, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    std::u16string requestDescriptor = u"invalid_descriptor";
    uint32_t requestCode = 1;
    MessageParcel data;
    data.WriteInterfaceToken(requestDescriptor);
    data.WriteUint32(requestCode);
    MessageParcel reply;
    MessageOption option;
    int32_t result = stubPtr->OnRemoteRequest(requestCode, data, reply, option);
    EXPECT_EQ(ERR_INVALID_DATA, result);
}

/**
 * @tc.name: OnRemoteRequest_003
 * @tc.desc: Test with invalid request code.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, OnRemoteRequest_003, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    std::u16string descriptor = DScreenSourceStub::GetDescriptor();
    std::u16string requestDescriptor = descriptor;
    uint32_t requestCode = 999;
    MessageParcel data;
    data.WriteInterfaceToken(requestDescriptor);
    data.WriteUint32(requestCode);
    MessageParcel reply;
    MessageOption option;
    int32_t result = stubPtr->OnRemoteRequest(requestCode, data, reply, option);
    EXPECT_EQ(IPC_STUB_UNKNOW_TRANS_ERR, result);
}

/**
 * @tc.name: InitSourceInner_001
 * @tc.desc: When there is no permission
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, InitSourceInner_001, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    std::u16string descriptor = stubPtr->GetDescriptor();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t result = stubPtr->InitSourceInner(data, reply, option);
    EXPECT_EQ(DSCREEN_INIT_ERR, result);
}

/**
 * @tc.name: InitSourceInner_002
 * @tc.desc: When HasEnableDHPermission returns true but param is empty or exceeds max size
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, InitSourceInner_002, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    g_mockVerifyAccessTokenReturnIntValue = 0;
    data.WriteString("");
    int32_t result = stubPtr->InitSourceInner(data, reply, option);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, result);
    std::string longParam(PARAM_MAX_SIZE + 1, 'a');
    data.WriteString(longParam);
    result = stubPtr->InitSourceInner(data, reply, option);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, result);
}

/**
 * @tc.name: InitSourceInner_003
 * @tc.desc: When HasEnableDHPermission returns true but remoteObject is nullptr
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, InitSourceInner_003, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    g_mockVerifyAccessTokenReturnIntValue = 0;
    data.WriteString("validParam");
    int32_t result = stubPtr->InitSourceInner(data, reply, option);
    EXPECT_EQ(ERR_DH_SCREEN_SA_READPARAM_FAILED, result);
}

/**
 * @tc.name: InitSourceInner_005
 * @tc.desc: When HasEnableDHPermission returns true and all parameters are valid
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, InitSourceInner_005, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    g_mockVerifyAccessTokenReturnIntValue = 0;
    data.WriteString("validParam");
    sptr<IRemoteObject> remoteObject = new TestDScreenSourceStub();
    data.WriteRemoteObject(remoteObject);
    int32_t result = stubPtr->InitSourceInner(data, reply, option);
    EXPECT_EQ(DH_SUCCESS, result);
}

/**
 * @tc.name: ReleaseSourceInner_001
 * @tc.desc: When there is no permission
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, ReleaseSourceInner_001, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    std::u16string descriptor = stubPtr->GetDescriptor();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t result = stubPtr->ReleaseSourceInner(data, reply, option);
    EXPECT_EQ(DSCREEN_INIT_ERR, result);
}

/**
 * @tc.name: ReleaseSourceInner_002
 * @tc.desc: When HasEnableDHPermission returns true
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, ReleaseSourceInner_002, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    g_mockVerifyAccessTokenReturnIntValue = 0;
    int32_t result = stubPtr->ReleaseSourceInner(data, reply, option);
    EXPECT_EQ(DH_SUCCESS, result);
}

/**
 * @tc.name: RegisterDistributedHardwareInner_001
 * @tc.desc: When there is no permission
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, RegisterDistributedHardwareInner_001, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    std::u16string descriptor = stubPtr->GetDescriptor();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int32_t result = stubPtr->RegisterDistributedHardwareInner(data, reply, option);
    EXPECT_EQ(DSCREEN_INIT_ERR, result);
}

/**
 * @tc.name: RegisterDistributedHardwareInner_002
 * @tc.desc: When HasEnableDHPermission returns true but input parameters are invalid
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, RegisterDistributedHardwareInner_002, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    g_mockVerifyAccessTokenReturnIntValue = 0;
    data.WriteString("");
    data.WriteString("dhId");
    data.WriteString("version");
    data.WriteString("attrs");
    data.WriteString("reqId");
    int32_t result = stubPtr->RegisterDistributedHardwareInner(data, reply, option);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, result);

    data.WriteString("devId");
    data.WriteString("");
    data.WriteString("version");
    data.WriteString("attrs");
    data.WriteString("reqId");
    result = stubPtr->RegisterDistributedHardwareInner(data, reply, option);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, result);

    data.WriteString("devId");
    data.WriteString("dhId");
    data.WriteString("");
    data.WriteString("attrs");
    data.WriteString("reqId");
    result = stubPtr->RegisterDistributedHardwareInner(data, reply, option);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, result);

    data.WriteString("devId");
    data.WriteString("dhId");
    data.WriteString("version");
    data.WriteString("");
    data.WriteString("reqId");
    result = stubPtr->RegisterDistributedHardwareInner(data, reply, option);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, result);

    data.WriteString("devId");
    data.WriteString("dhId");
    data.WriteString("version");
    data.WriteString("attrs");
    data.WriteString("");
    result = stubPtr->RegisterDistributedHardwareInner(data, reply, option);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, result);
}

/**
 * @tc.name: RegisterDistributedHardwareInner_003
 * @tc.desc: When HasEnableDHPermission returns true and all parameters are valid
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, RegisterDistributedHardwareInner_003, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    g_mockVerifyAccessTokenReturnIntValue = 0;
    data.WriteString("devId");
    data.WriteString("dhId");
    data.WriteString("version");
    data.WriteString("attrs");
    data.WriteString("reqId");
    int32_t result = stubPtr->RegisterDistributedHardwareInner(data, reply, option);
    EXPECT_EQ(DH_SUCCESS, result);
}

/**
 * @tc.name: UnregisterDistributedHardwareInner_001
 * @tc.desc: When HasEnableDHPermission returns true but input parameters are invalid
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, UnregisterDistributedHardwareInner_001, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    g_mockVerifyAccessTokenReturnIntValue = 0;
    data.WriteString("");
    data.WriteString("dhId");
    data.WriteString("reqId");
    int32_t result = stubPtr->UnregisterDistributedHardwareInner(data, reply, option);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, result);

    data.WriteString("devId");
    data.WriteString("");
    data.WriteString("reqId");
    result = stubPtr->UnregisterDistributedHardwareInner(data, reply, option);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, result);

    data.WriteString("devId");
    data.WriteString("dhId");
    data.WriteString("");
    result = stubPtr->UnregisterDistributedHardwareInner(data, reply, option);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, result);
}

/**
 * @tc.name: UnregisterDistributedHardwareInner_002
 * @tc.desc: When HasEnableDHPermission returns true and all parameters are valid
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, UnregisterDistributedHardwareInner_002, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    g_mockVerifyAccessTokenReturnIntValue = 0;
    data.WriteString("devId");
    data.WriteString("dhId");
    data.WriteString("reqId");
    int32_t result = stubPtr->UnregisterDistributedHardwareInner(data, reply, option);
    EXPECT_EQ(DH_SUCCESS, result);
}

/**
 * @tc.name: CheckRegParams_001
 * @tc.desc: ValidParams
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, CheckRegParams_001, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    bool result = stubPtr->CheckRegParams("devId", "dhId", "version", "attrs", "reqId");
    EXPECT_TRUE(result);
}

/**
 * @tc.name: CheckRegParams_002
 * @tc.desc: Parameter is empty
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

HWTEST_F(DScreenSourceStubTest, CheckRegParams_002, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    bool result;
    result = stubPtr->CheckRegParams("", "dhId", "version", "attrs", "reqId");
    EXPECT_FALSE(result);
    result = stubPtr->CheckRegParams("devId", "", "version", "attrs", "reqId");
    EXPECT_FALSE(result);
    result = stubPtr->CheckRegParams("devId", "dhId", "", "attrs", "reqId");
    EXPECT_FALSE(result);
    result = stubPtr->CheckRegParams("devId", "dhId", "version", "", "reqId");
    EXPECT_FALSE(result);
    result = stubPtr->CheckRegParams("devId", "dhId", "version", "attrs", "");
    EXPECT_FALSE(result);
}

/**
 * @tc.name: CheckRegParams_003
 * @tc.desc: parameter exceeds maximum length
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

HWTEST_F(DScreenSourceStubTest, CheckRegParams_003, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    std::string exceedDidMaxSizeStr(DID_MAX_SIZE + 1, 'a');
    std::string exceedParamMaxSizeStr(PARAM_MAX_SIZE + 1, 'a');
    bool result;
    result = stubPtr->CheckRegParams(exceedDidMaxSizeStr, "dhId", "version", "attrs", "reqId");
    EXPECT_FALSE(result);
    result = stubPtr->CheckRegParams("devId", exceedDidMaxSizeStr, "version", "attrs", "reqId");
    EXPECT_FALSE(result);
    result = stubPtr->CheckRegParams("devId", "dhId", exceedParamMaxSizeStr, "attrs", "reqId");
    EXPECT_FALSE(result);
    result = stubPtr->CheckRegParams("devId", "dhId", "version", exceedParamMaxSizeStr, "reqId");
    EXPECT_FALSE(result);
    result = stubPtr->CheckRegParams("devId", "dhId", "version", "attrs", exceedDidMaxSizeStr);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: CheckUnregParams_001
 * @tc.desc: ValidParams
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, CheckUnregParams_001, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    bool result = stubPtr->CheckUnregParams("devId", "dhId", "reqId");
    EXPECT_TRUE(result);
}

/**
 * @tc.name: CheckUnregParams_002
 * @tc.desc: Parameter is empty
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

HWTEST_F(DScreenSourceStubTest, CheckUnregParams_002, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    bool result;
    result = stubPtr->CheckUnregParams("", "dhId", "reqId");
    EXPECT_FALSE(result);
    result = stubPtr->CheckUnregParams("devId", "", "reqId");
    EXPECT_FALSE(result);
    result = stubPtr->CheckUnregParams("devId", "dhId", "");
    EXPECT_FALSE(result);
}

/**
 * @tc.name: CheckUnregParams_003
 * @tc.desc: parameter exceeds maximum length
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

HWTEST_F(DScreenSourceStubTest, CheckUnregParams_003, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    std::string exceedDidMaxSizeStr(DID_MAX_SIZE + 1, 'a');
    bool result;
    result = stubPtr->CheckUnregParams(exceedDidMaxSizeStr, "dhId", "reqId");
    EXPECT_FALSE(result);
    result = stubPtr->CheckUnregParams("devId", exceedDidMaxSizeStr, "reqId");
    EXPECT_FALSE(result);
    result = stubPtr->CheckUnregParams("devId", "dhId", exceedDidMaxSizeStr);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: CheckConfigParams_001
 * @tc.desc: ValidParams
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceStubTest, CheckConfigParams_001, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    bool result = stubPtr->CheckConfigParams("devId", "dhId", "key", "value");
    EXPECT_TRUE(result);
}

/**
 * @tc.name: CheckConfigParams_002
 * @tc.desc: Parameter is empty
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

HWTEST_F(DScreenSourceStubTest, CheckConfigParams_002, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    bool result;
    result = stubPtr->CheckConfigParams("", "dhId", "key", "value");
    EXPECT_FALSE(result);
    result = stubPtr->CheckConfigParams("devId", "", "key", "value");
    EXPECT_FALSE(result);
    result = stubPtr->CheckConfigParams("devId", "dhId", "", "value");
    EXPECT_FALSE(result);
    result = stubPtr->CheckConfigParams("devId", "dhId", "key", "");
    EXPECT_FALSE(result);
}

/**
 * @tc.name: CheckConfigParams_003
 * @tc.desc: parameter exceeds maximum length
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

HWTEST_F(DScreenSourceStubTest, CheckConfigParams_003, TestSize.Level1)
{
    std::shared_ptr<TestDScreenSourceStub> stubPtr = std::make_shared<TestDScreenSourceStub>();
    std::string exceedDidMaxSizeStr(DID_MAX_SIZE + 1, 'a');
    std::string exceedParamMaxSizeStr(PARAM_MAX_SIZE + 1, 'a');
    bool result;
    result = stubPtr->CheckConfigParams(exceedDidMaxSizeStr, "dhId", "key", "value");
    EXPECT_FALSE(result);
    result = stubPtr->CheckConfigParams("devId", exceedDidMaxSizeStr, "key", "value");
    EXPECT_FALSE(result);
    result = stubPtr->CheckConfigParams("devId", "dhId", exceedParamMaxSizeStr, "value");
    EXPECT_FALSE(result);
    result = stubPtr->CheckConfigParams("devId", "dhId", "key", exceedParamMaxSizeStr);
    EXPECT_FALSE(result);
}
}  // namespace DistributedHardware
}  // namespace OHOS
