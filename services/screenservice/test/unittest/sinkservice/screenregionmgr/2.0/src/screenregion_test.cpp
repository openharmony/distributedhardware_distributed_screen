/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "2.0/include/screenregion_test.h"
#include "dscreen_constants.h"
#include "engine_test_utils.h"
#include "screen_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
namespace V2_0 {
void ScreenRegionTestV2::SetUpTestCase(void) {}

void ScreenRegionTestV2::TearDownTestCase(void) {}

void ScreenRegionTestV2::SetUp(void)
{
    const std::string remoteDevId = "sourceDevId";
    screenRegion_ = std::make_shared<ScreenRegion>(remoteDevId);
}

void ScreenRegionTestV2::TearDown(void) {}

/**
 * @tc.name: OnEngineEvent_001
 * @tc.desc: Verify the OnEngineEvent function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, OnEngineEvent_001, TestSize.Level1)
{
    DScreenEventType event = DScreenEventType::ENGINE_ERROR;
    const std::string content = "OnEngineEvent_001";
    screenRegion_->receiverAdapter_ = nullptr;
    screenRegion_->OnEngineEvent(event, content);
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, screenRegion_->StopReceiverEngine());
}

/**
 * @tc.name: OnEngineMessage_001
 * @tc.desc: Verify the OnEngineMessage function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, OnEngineMessage_001, TestSize.Level1)
{
    std::shared_ptr<AVTransMessage> message  = std::make_shared<AVTransMessage>();
    message->type_ = DScreenMsgType::START_MIRROR;
    message->content_ = "invaild_json_string";
    screenRegion_->receiverAdapter_ = std::make_shared<AVTransReceiverAdapter>();
    screenRegion_->OnEngineMessage(message);
    EXPECT_EQ(ERR_DH_AV_TRANS_SETUP_FAILED, screenRegion_->StartReceiverEngine(message->content_));
}

/**
 * @tc.name: OnEngineMessage_002
 * @tc.desc: Verify the OnEngineMessage function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, OnEngineMessage_002, TestSize.Level1)
{
    std::shared_ptr<AVTransMessage> message  = std::make_shared<AVTransMessage>();
    message->type_ = DScreenMsgType::STOP_MIRROR;
    screenRegion_->receiverAdapter_ = nullptr;
    screenRegion_->OnEngineMessage(message);
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, screenRegion_->StopReceiverEngine());
}

/**
 * @tc.name: Release_001
 * @tc.desc: Verify the Release function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, Release_001, TestSize.Level1)
{
    screenRegion_->isRunning = false;
    EXPECT_EQ(DH_SUCCESS, screenRegion_->Release());
    screenRegion_->isRunning = true;
    screenRegion_->receiverAdapter_ = nullptr;
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, screenRegion_->Release());
}

/**
 * @tc.name: Release_002
 * @tc.desc: Verify the Release function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, Release_002, TestSize.Level1)
{
    screenRegion_->isRunning = true;
    screenRegion_->receiverAdapter_ = std::make_shared<AVTransReceiverAdapter>();
    screenRegion_->receiverAdapter_->receiverEngine_ = std::make_shared<MockIAVReceiverEngine>();
    EXPECT_EQ(DH_SUCCESS, screenRegion_->Release());
    EXPECT_EQ(false, screenRegion_->isRunning);
}

/**
 * @tc.name: InitReceiverEngine_001
 * @tc.desc: Verify the InitReceiverEngine function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, InitReceiverEngine_001, TestSize.Level1)
{
    screenRegion_->receiverAdapter_ = std::make_shared<AVTransReceiverAdapter>();
    screenRegion_->receiverAdapter_->initialized_ =false;
    std::shared_ptr<IAVEngineProvider> providerPtr = nullptr;
    EXPECT_EQ(ERR_DH_AV_TRANS_INIT_FAILED, screenRegion_->InitReceiverEngine(providerPtr.get()));
}

/**
 * @tc.name: InitReceiverEngine_002
 * @tc.desc: Verify the InitReceiverEngine function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, InitReceiverEngine_002, TestSize.Level1)
{
    screenRegion_->receiverAdapter_ = std::make_shared<AVTransReceiverAdapter>();
    screenRegion_->receiverAdapter_->initialized_ =true;
    std::shared_ptr<IAVEngineProvider> providerPtr = nullptr;
    screenRegion_->receiverAdapter_->adapterCallback_ =  std::make_shared<MockAVReceiverAdapterCallback>();
    EXPECT_EQ(DH_SUCCESS, screenRegion_->InitReceiverEngine(providerPtr.get()));
}

/**
 * @tc.name: GetWindowId_001
 * @tc.desc: Verify the GetWindowId function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, GetWindowId_001, TestSize.Level1)
{
    screenRegion_->windowId_ = 5;
    int32_t ret = screenRegion_->GetWindowId();
    EXPECT_EQ(screenRegion_->windowId_, ret);
}

/**
 * @tc.name: GetScreenId_001
 * @tc.desc: Verify the GetScreenId function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, GetScreenId_001, TestSize.Level1)
{
    screenRegion_->screenId_ = 5;
    uint64_t ret = screenRegion_->GetScreenId();
    EXPECT_EQ(screenRegion_->screenId_, ret);
}

/**
 * @tc.name: GetRemoteDevId_001
 * @tc.desc: Verify the GetRemoteDevId function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, GetRemoteDevId_001, TestSize.Level1)
{
    screenRegion_->remoteDevId_ = "remoteDevId";
    std::string ret = screenRegion_->GetRemoteDevId();
    EXPECT_EQ(screenRegion_->remoteDevId_, ret);
}

/**
 * @tc.name: GetVideoParam_001
 * @tc.desc: Verify the GetVideoParam function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, GetVideoParam_001, TestSize.Level1)
{
    screenRegion_->videoParam_ = std::make_shared<VideoParam>();
    std::shared_ptr<VideoParam> ret = screenRegion_->GetVideoParam();
    EXPECT_EQ(screenRegion_->videoParam_, ret);
}

/**
 * @tc.name: GetWindowProperty_001
 * @tc.desc: Verify the GetWindowProperty function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, GetWindowProperty_001, TestSize.Level1)
{
    screenRegion_->windowProperty_ = std::make_shared<WindowProperty>();
    std::shared_ptr<WindowProperty> ret = screenRegion_->GetWindowProperty();
    EXPECT_EQ(screenRegion_->windowProperty_, ret);
}

/**
 * @tc.name: StartReceiverEngine_001
 * @tc.desc: Verify the StartReceiverEngine function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, StartReceiverEngine_001, TestSize.Level1)
{
    screenRegion_->receiverAdapter_ = nullptr;
    const std::string content = "StartReceiverEngine_001";
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, screenRegion_->StartReceiverEngine(content));
}

/**
 * @tc.name: StartReceiverEngine_002
 * @tc.desc: Verify the StartReceiverEngine function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, StartReceiverEngine_002, TestSize.Level1)
{
    screenRegion_->receiverAdapter_ = std::make_shared<AVTransReceiverAdapter>();
    const std::string content = "StartReceiverEngine_002";
    EXPECT_EQ(ERR_DH_AV_TRANS_SETUP_FAILED, screenRegion_->StartReceiverEngine(content));
}

/**
 * @tc.name: StartReceiverEngine_003
 * @tc.desc: Verify the StartReceiverEngine function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, StartReceiverEngine_003, TestSize.Level1)
{
    screenRegion_->receiverAdapter_ = std::make_shared<AVTransReceiverAdapter>();
    const std::string content = "{\"dhId\":\"SCREEN#0\", \"mapRelation\":{\"displayId\":0, \
        \"displayRect\":{\"height\":1280, \"startX\":0, \"startY\":0, \"width\":720}, \"screenId\":2, \
        \"screenRect\":{\"height\":1280, \"startX\":0, \"startY\":0, \"width\":720}}, \"screenId\":2, \
        \"videoParam\":{\"codecType\":2, \"colorFormat\":3, \"fps\":30, \"screenHeight\":1280, \
        \"screenWidth\":720, \"videoHeight\":1280, \"videoWidth\":720}}";
    screenRegion_->receiverAdapter_->receiverEngine_ = nullptr;
    EXPECT_EQ(ERR_DH_AV_TRANS_START_FAILED, screenRegion_->StartReceiverEngine(content));
}

/**
 * @tc.name: StartReceiverEngine_004
 * @tc.desc: Verify the StartReceiverEngine function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, StartReceiverEngine_004, TestSize.Level1)
{
    screenRegion_->receiverAdapter_ = std::make_shared<AVTransReceiverAdapter>();
    const std::string content = "{\"dhId\":\"SCREEN#0\", \"mapRelation\":{\"displayId\":0, \
        \"displayRect\":{\"height\":1280, \"startX\":0, \"startY\":0, \"width\":720}, \"screenId\":2, \
        \"screenRect\":{\"height\":1280, \"startX\":0, \"startY\":0, \"width\":720}}, \"screenId\":2, \
        \"videoParam\":{\"codecType\":2, \"colorFormat\":3, \"fps\":30, \"screenHeight\":1280, \
        \"screenWidth\":720, \"videoHeight\":1280, \"videoWidth\":720}}";
    screenRegion_->receiverAdapter_->receiverEngine_ = std::make_shared<MockIAVReceiverEngine>();
    EXPECT_EQ(DH_SUCCESS, screenRegion_->StartReceiverEngine(content));
    EXPECT_EQ(true, screenRegion_->isRunning);
}

/**
 * @tc.name: StopReceiverEngine_001
 * @tc.desc: Verify the StopReceiverEngine function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, StopReceiverEngine_001, TestSize.Level1)
{
    screenRegion_->receiverAdapter_ = nullptr;
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, screenRegion_->StopReceiverEngine());
}

/**
 * @tc.name: StopReceiverEngine_002
 * @tc.desc: Verify the StopReceiverEngine function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, StopReceiverEngine_002, TestSize.Level1)
{
    screenRegion_->receiverAdapter_ = std::make_shared<AVTransReceiverAdapter>();
    screenRegion_->receiverAdapter_->receiverEngine_ = nullptr;
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, screenRegion_->StopReceiverEngine());
    screenRegion_->receiverAdapter_->receiverEngine_ = std::make_shared<MockIAVReceiverEngine>();
    EXPECT_EQ(DH_SUCCESS, screenRegion_->StopReceiverEngine());
}

/**
 * @tc.name: SetUp_001
 * @tc.desc: Verify the SetUp function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, SetUp_001, TestSize.Level1)
{
    const std::string content = "SetUp_001";
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, screenRegion_->SetUp(content));
}

/**
 * @tc.name: ConfigWindow_001
 * @tc.desc: Verify the ConfigWindow function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, ConfigWindow_001, TestSize.Level1)
{
    std::shared_ptr<DScreenMapRelation> mapRelation_ = nullptr;
    EXPECT_EQ(ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED, screenRegion_->ConfigWindow());
}

/**
 * @tc.name: ConfigWindow_002
 * @tc.desc: Verify the ConfigWindow function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV2, ConfigWindow_002, TestSize.Level1)
{
    screenRegion_->mapRelation_ = std::make_shared<DScreenMapRelation>();
    std::shared_ptr<WindowProperty> windowProperty = std::make_shared<WindowProperty>();
    ScreenClient::GetInstance().AddWindow(windowProperty);
    EXPECT_EQ(DH_SUCCESS, screenRegion_->ConfigWindow());
}
} // namespace V2_0
} // namespace DistributedHardware
} // namespace OHOS