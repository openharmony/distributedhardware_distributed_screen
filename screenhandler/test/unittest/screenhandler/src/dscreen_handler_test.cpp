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

#include "dscreen_handler_test.h"
#include "dscreen_constants.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void DScreenHandlerTest::SetUpTestCase(void) {}

void DScreenHandlerTest::TearDownTestCase(void) {}

void DScreenHandlerTest::SetUp(void) {}

void DScreenHandlerTest::TearDown(void) {}

/**
 * @tc.name: Initialize_001
 * @tc.desc: Verify the Initialize function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenHandlerTest, Initialize_001, TestSize.Level1)
{
    int32_t ret = DScreenHandler::GetInstance().Initialize();
    uint64_t screenId = SCREEN_ID_DEFAULT;
    DScreenHandler::GetInstance().screenListener_->OnConnect(screenId);
    screenId = 9999999;
    DScreenHandler::GetInstance().screenListener_->OnConnect(screenId);
    DScreenHandler::GetInstance().screenListener_->OnDisconnect(screenId);
    screenId = 1;
    DScreenHandler::GetInstance().screenListener_->OnConnect(screenId);
    const std::string dhId = "dhId";
    const std::string attr = "attr";
    const std::string subtype = "screen";
    DScreenHandler::GetInstance().listener_ = nullptr;
    DScreenHandler::GetInstance().PluginHardware(dhId, attr, subtype);
    DScreenHandler::GetInstance().UnPluginHardware(dhId);
    DScreenHandler::GetInstance().listener_ = std::make_shared<MockPluginListener>();
    DScreenHandler::GetInstance().PluginHardware(dhId, attr, subtype);
    DScreenHandler::GetInstance().UnPluginHardware(dhId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: ByteCalculate_001
 * @tc.desc: Verify the ByteCalculate function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenHandlerTest, ByteCalculate_001, TestSize.Level1)
{
    uint32_t ret = DScreenHandler::GetInstance().screenListener_->ByteCalculate(320);
    EXPECT_EQ(320, ret);
    ret = DScreenHandler::GetInstance().screenListener_->ByteCalculate(312);
    EXPECT_EQ(320, ret);
}

/**
 * @tc.name: Query_001
 * @tc.desc: Verify the Query function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenHandlerTest, Query_001, TestSize.Level1)
{
    EXPECT_EQ(DScreenHandler::GetInstance().Query().empty(), false);
}

/**
 * @tc.name: RegisterPluginListener_001
 * @tc.desc: Verify the RegisterPluginListener function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenHandlerTest, RegisterPluginListener_001, TestSize.Level1)
{
    std::shared_ptr<PluginListener> listener = std::make_shared<MockPluginListener>();
    DScreenHandler::GetInstance().RegisterPluginListener(listener);
    EXPECT_EQ(listener, DScreenHandler::GetInstance().listener_);
    DScreenHandler::GetInstance().UnRegisterPluginListener();
    EXPECT_EQ(nullptr, DScreenHandler::GetInstance().listener_);
}

/**
 * @tc.name: RegisterPluginListener_002
 * @tc.desc: Verify the RegisterPluginListener function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenHandlerTest, RegisterPluginListener_002, TestSize.Level1)
{
    std::shared_ptr<PluginListener> listener = nullptr;
    DScreenHandler::GetInstance().listener_ = nullptr;
    DScreenHandler::GetInstance().RegisterPluginListener(listener);

    EXPECT_EQ(listener, DScreenHandler::GetInstance().listener_);
}

/**
 * @tc.name: QueryCodecInfo_001
 * @tc.desc: Verify the QueryCodecInfo function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenHandlerTest, QueryCodecInfo_001, TestSize.Level1)
{
    DScreenHandler::GetInstance().codecInfoStr_ = "codecInfoStr_";
    std::string ret = DScreenHandler::GetInstance().QueryCodecInfo();
    EXPECT_EQ(DScreenHandler::GetInstance().codecInfoStr_, ret);
}
}  // namespace DistributedHardware
}  // namespace OHOS
