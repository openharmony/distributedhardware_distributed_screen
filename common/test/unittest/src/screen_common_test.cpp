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
#include "screen_common_test.h"

#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_json_util.h"
#include "dscreen_log.h"
#include "dscreen_util.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void ScreenCommonTest::SetUpTestCase(void) {}

void ScreenCommonTest::TearDownTestCase(void) {}

void ScreenCommonTest::SetUp() {}

void ScreenCommonTest::TearDown() {}

/**
 * @tc.name: common_001
 * @tc.desc: Verify the common function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenCommonTest, common_001, TestSize.Level1)
{
    DHLOGW("common_001.");
    std::string networkId = "networkId";
    int32_t ret = GetLocalDeviceNetworkId(networkId);
    EXPECT_NE(DH_SUCCESS, ret);
}

/**
 * @tc.name: common_002
 * @tc.desc: Verify the common function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenCommonTest, common_002, TestSize.Level1)
{
    DHLOGW("common_002.");
    ReportSaFail(eventName, errCode, saId, errMsg);
    ReportSaFail(eventName, errCode, saId, errMsg);
    ReportRegisterFail(eventName, errCode, devId, dhId, errMsg);
    ReportUnRegisterFail(eventName, errCode, devId, dhId, errMsg);
    ReportOptFail(eventName, errCode, errMsg);
    ReportSaEvent(eventName, saId, errMsg);
    ReportRegisterScreenEvent(eventName, devId, dhId, errMsg);
    ReportUnRegisterScreenEvent(eventName, devId, dhId, errMsg);
    ReportScreenMirrorEvent(eventName, devId, dhId, errMsg);
    ReportSaFail(longEventName, errCode, saId, errMsg);
    ReportSaFail(longEventName, errCode, saId, errMsg);
    ReportRegisterFail(longEventName, errCode, devId, dhId, errMsg);
    ReportUnRegisterFail(longEventName, errCode, devId, dhId, errMsg);
    ReportOptFail(longEventName, errCode, errMsg);
    ReportSaEvent(longEventName, saId, errMsg);
    ReportRegisterScreenEvent(longEventName, devId, dhId, errMsg);
    ReportUnRegisterScreenEvent(longEventName, devId, dhId, errMsg);
    ReportScreenMirrorEvent(longEventName, devId, dhId, errMsg);
    std::string value = "Id";
    std::string ret = GetAnonyString(value);
    EXPECT_EQ("******", ret);
}

/**
 * @tc.name: common_003
 * @tc.desc: Verify the common function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenCommonTest, common_003, TestSize.Level1)
{
    DHLOGW("common_003.");
    std::string value = "Id";
    std::string ret = GetInterruptString(value);
    EXPECT_EQ(value, ret);
}

/**
 * @tc.name: common_004
 * @tc.desc: Verify the common function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenCommonTest, common_004, TestSize.Level1)
{
    DHLOGW("common_004.");
    std::string value = "Idvalues";
    std::string ret = GetInterruptString(value);
    EXPECT_EQ("Idva", ret);
}

/**
 * @tc.name: IsInt64_001
 * @tc.desc: Verify the IsInt64 function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenCommonTest, IsInt64_001, TestSize.Level1)
{
    nlohmann::json testJson;
    std::string key = "test";
    EXPECT_FALSE(IsInt64(testJson, key));

    testJson[key] = "test";
    EXPECT_FALSE(IsInt64(testJson, key));

    testJson[key] = 1;
    EXPECT_TRUE(IsInt64(testJson, key));
}

/**
 * @tc.name: IsArray_001
 * @tc.desc: Verify the IsArray function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenCommonTest, IsArray_001, TestSize.Level1)
{
    nlohmann::json testJson;
    std::string key = "test";
    EXPECT_FALSE(IsArray(testJson, key));

    testJson[key] = "test";
    EXPECT_FALSE(IsArray(testJson, key));

    testJson[key] = {1, 2, 3};
    EXPECT_TRUE(IsArray(testJson, key));
}
} // namespace DistributedHardware
} // namespace OHOS