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

#include "dscreen_source_callback_stub_test.h"

#include <memory>

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
    sptr<IRemoteObject> sourceCbkStubPtr = new TestDScreenSourceCallbackStub();
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
    sptr<IRemoteObject> sourceCbkStubPtr = new TestDScreenSourceCallbackStub();
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
}
}
}