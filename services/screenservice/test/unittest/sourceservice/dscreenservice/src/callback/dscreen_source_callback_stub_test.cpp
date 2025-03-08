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
#include "dscreen_source_callback_stub_test.h"
#include "dscreen_constants.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void DScreenSourceCallbackStubTest::SetUpTestCase(void)
{
}

void DScreenSourceCallbackStubTest::TearDownTestCase(void)
{
}

void DScreenSourceCallbackStubTest::SetUp(void)
{
}

void DScreenSourceCallbackStubTest::TearDown(void)
{
}

int32_t DScreenSourceCallbackStubTest::TestDScreenSourceCallbackStub::OnNotifyRegResult(const std::string &devId,
    const std::string &dhId, const std::string &reqId, int32_t status, const std::string &data)
{
    (void)devId;
    (void)dhId;
    (void)reqId;
    (void)status;
    (void)data;
    return DH_SUCCESS;
}

int32_t DScreenSourceCallbackStubTest::TestDScreenSourceCallbackStub::OnNotifyUnregResult(const std::string &devId,
    const std::string &dhId, const std::string &reqId, int32_t status, const std::string &data)
{
    (void)devId;
    (void)dhId;
    (void)reqId;
    (void)status;
    (void)data;
    return DH_SUCCESS;
}

/**
 * @tc.name: OnNotifyRegResult_001
 * @tc.desc: Invoke the OnNotifyRegResult ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceCallbackStubTest, OnNotifyRegResult_001, TestSize.Level1)
{
    sptr<IRemoteObject> sourceCbkStubPtr(new TestDScreenSourceCallbackStub());
    DScreenSourceCallbackProxy sourceCbkProxy(sourceCbkStubPtr);

    int32_t ret = sourceCbkProxy.OnNotifyRegResult("", "dhId000", "reqId000", 0, "data000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    std::string devId = R"(dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d
        Id000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d
        hId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000)";
    ret = sourceCbkProxy.OnNotifyRegResult(devId, "dhId000", "reqId000", 0, "data000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceCbkProxy.OnNotifyRegResult("devId000", "", "reqId000", 0, "data000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceCbkProxy.OnNotifyRegResult("devId000", devId, "reqId000", 0, "data000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceCbkProxy.OnNotifyRegResult("devId000", "dhId000", "", 0, "data000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceCbkProxy.OnNotifyRegResult("devId000", "dhId000", devId, 0, "data000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceCbkProxy.OnNotifyRegResult("devId000", "dhId000", "reqId000", 0, "");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceCbkProxy.OnNotifyRegResult("devId000", "dhId000", "reqId000", 0, "data000");
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: OnNotifyUnregResult_001
 * @tc.desc: Invoke the OnNotifyUnregResult ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceCallbackStubTest, OnNotifyUnregResult_001, TestSize.Level1)
{
    sptr<IRemoteObject> sourceCbkStubPtr(new TestDScreenSourceCallbackStub());
    DScreenSourceCallbackProxy sourceCbkProxy(sourceCbkStubPtr);

    int32_t ret = sourceCbkProxy.OnNotifyUnregResult("", "dhId000", "reqId000", 0, "data000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    std::string devId = R"(dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d
        Id000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000d
        hId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000dhId000)";
    ret = sourceCbkProxy.OnNotifyUnregResult(devId, "dhId000", "reqId000", 0, "data000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceCbkProxy.OnNotifyUnregResult("devId000", "", "reqId000", 0, "data000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceCbkProxy.OnNotifyUnregResult("devId000", devId, "reqId000", 0, "data000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceCbkProxy.OnNotifyUnregResult("devId000", "dhId000", "", 0, "data000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceCbkProxy.OnNotifyUnregResult("devId000", "dhId000", devId, 0, "data000");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceCbkProxy.OnNotifyUnregResult("devId000", "dhId000", "reqId000", 0, "");
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);

    ret = sourceCbkProxy.OnNotifyUnregResult("devId000", "dhId000", "reqId000", 0, "data000");
    EXPECT_EQ(DH_SUCCESS, ret);

    std::string exceedParamMaxSizeStr(PARAM_MAX_SIZE + 1, 'a');
    ret = sourceCbkProxy.OnNotifyUnregResult("devId000", "dhId000", "reqId000", 0, exceedParamMaxSizeStr);
    EXPECT_NE(DH_SUCCESS, ret);
}

/**
 * @tc.name: OnRemoteRequest_001
 * @tc.desc: Invoke the OnRemoteRequest ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceCallbackStubTest, OnRemoteRequest_001, TestSize.Level1)
{
    std::shared_ptr<DScreenSourceCallbackStub> callback = std::make_shared<TestDScreenSourceCallbackStub>();
    std::u16string descriptor = DScreenSourceCallbackStub::GetDescriptor();
    std::u16string requestDescriptor = descriptor;
    uint32_t requestCode = 999;
    MessageParcel data;
    data.WriteInterfaceToken(requestDescriptor);
    data.WriteUint32(requestCode);
    MessageParcel reply;
    MessageOption option;
    int32_t result = callback->OnRemoteRequest(requestCode, data, reply, option);
    EXPECT_NE(0, result);
}

/**
 * @tc.name: CheckParams_001
 * @tc.desc: Invoke the CheckParams ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceCallbackStubTest, CheckParams_001, TestSize.Level1)
{
    std::string devId = "devId";
    std::string dhId = "dhId";
    std::string reqId = "reqId";
    std::string resultData = "resultData";
    std::shared_ptr<DScreenSourceCallbackStub> callback = std::make_shared<TestDScreenSourceCallbackStub>();
    EXPECT_TRUE(callback->CheckParams(devId, dhId, reqId, resultData));
}

/**
 * @tc.name: CheckParams_002
 * @tc.desc: Invoke the CheckParams ipc interface.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceCallbackStubTest, CheckParams_002, TestSize.Level1)
{
    std::string exceedDidMaxSizeStr(DID_MAX_SIZE + 1, 'a');
    std::string exceedParamMaxSizeStr(PARAM_MAX_SIZE + 1, 'a');
    std::string devId = "devId";
    std::string dhId = "dhId";
    std::string reqId = "reqId";
    std::string resultData = exceedParamMaxSizeStr;
    std::shared_ptr<DScreenSourceCallbackStub> callback = std::make_shared<TestDScreenSourceCallbackStub>();
    EXPECT_FALSE(callback->CheckParams(devId, dhId, reqId, resultData));

    resultData.clear();
    EXPECT_FALSE(callback->CheckParams(devId, dhId, reqId, resultData));

    reqId = exceedDidMaxSizeStr;
    EXPECT_FALSE(callback->CheckParams(devId, dhId, reqId, resultData));

    reqId.clear();
    EXPECT_FALSE(callback->CheckParams(devId, dhId, reqId, resultData));

    dhId = exceedDidMaxSizeStr;
    EXPECT_FALSE(callback->CheckParams(devId, dhId, reqId, resultData));

    dhId.clear();
    EXPECT_FALSE(callback->CheckParams(devId, dhId, reqId, resultData));

    devId = exceedDidMaxSizeStr;
    EXPECT_FALSE(callback->CheckParams(devId, dhId, reqId, resultData));

    devId.clear();
    EXPECT_FALSE(callback->CheckParams(devId, dhId, reqId, resultData));
}
}  // namespace DistributedHardware
}  // namespace OHOS
