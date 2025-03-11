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

#include "1.0/include/dscreen_manager_test.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
namespace V1_0 {
void DScreenManagerTestV1::SetUpTestCase(void) {}

void DScreenManagerTestV1::TearDownTestCase(void) {}

void DScreenManagerTestV1::SetUp()
{
    DScreenManager::GetInstance().Init();
}

void DScreenManagerTestV1::TearDown()
{
    DScreenManager::GetInstance().UnInit();
}

/**
 * @tc.name: Init_001
 * @tc.desc: Verify the Init function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, Init_001, TestSize.Level1)
{
    int32_t ret = DScreenManager::GetInstance().Init();
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: HandleScreenChange_001
 * @tc.desc: Verify the HandleScreenChange function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, HandleScreenChange_001, TestSize.Level1)
{
    std::shared_ptr<DScreen> changedScreen = nullptr;
    Rosen::ScreenGroupChangeEvent event = Rosen::ScreenGroupChangeEvent::ADD_TO_GROUP;
    DScreenManager::GetInstance().HandleScreenChange(changedScreen, event);

    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    changedScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    DScreenManager::GetInstance().HandleScreenChange(changedScreen, event);

    event = Rosen::ScreenGroupChangeEvent::REMOVE_FROM_GROUP;
    DScreenManager::GetInstance().HandleScreenChange(changedScreen, event);

    event = Rosen::ScreenGroupChangeEvent::CHANGE_GROUP;
    DScreenManager::GetInstance().HandleScreenChange(changedScreen, event);

    event = (Rosen::ScreenGroupChangeEvent)99;
    DScreenManager::GetInstance().HandleScreenChange(changedScreen, event);

    int32_t ret = DScreenManager::GetInstance().Init();
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: AddToGroup_001
 * @tc.desc: Verify the AddToGroup function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, AddToGroup_001, TestSize.Level1)
{
    std::shared_ptr<DScreen> changedScreen = nullptr;
    int32_t ret = DScreenManager::GetInstance().AddToGroup(changedScreen, 0);
    EXPECT_EQ(ERR_DH_SCREEN_SA_VALUE_NOT_INIT, ret);

    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    changedScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    ret = DScreenManager::GetInstance().AddToGroup(changedScreen, changedScreen->GetScreenId());
    EXPECT_EQ(ERR_DH_SCREEN_SA_VALUE_NOT_INIT, ret);

    changedScreen->SetState(DScreenState::CONNECTING);
    ret = DScreenManager::GetInstance().AddToGroup(changedScreen, changedScreen->GetScreenId());
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: RemoveFromGroup_001
 * @tc.desc: Verify the RemoveFromGroup function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, RemoveFromGroup_001, TestSize.Level1)
{
    std::shared_ptr<DScreen> changedScreen = nullptr;
    int32_t ret = DScreenManager::GetInstance().RemoveFromGroup(changedScreen, 0);
    EXPECT_EQ(ERR_DH_SCREEN_SA_VALUE_NOT_INIT, ret);

    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    changedScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    ret = DScreenManager::GetInstance().RemoveFromGroup(changedScreen, changedScreen->GetScreenId());
    EXPECT_EQ(DH_SUCCESS, ret);

    changedScreen->SetState(DScreenState::DISCONNECTING);
    ret = DScreenManager::GetInstance().RemoveFromGroup(changedScreen, changedScreen->GetScreenId());
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: OnRegResult_001
 * @tc.desc: Verify the OnRegResult function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, OnRegResult_001, TestSize.Level1)
{
    std::string devId = "devId";
    std::string dhId = "dhId";
    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>(devId, dhId, dScreenCallback);
    std::string reqId;
    int32_t status = 0;
    std::string data;
    DScreenManager::GetInstance().dScreenSourceCallbackProxy_ = nullptr;
    DScreenManager::GetInstance().OnRegResult(dScreen, reqId, status, data);

    sptr<MockDScreenSourceCallback> mockInstance(new MockDScreenSourceCallback());
    DScreenManager::GetInstance().dScreenSourceCallbackProxy_ = mockInstance;
    EXPECT_CALL(*mockInstance, OnNotifyRegResult(_, _, _, _, _)).Times(1);
    DScreenManager::GetInstance().OnRegResult(dScreen, reqId, status, data);
    DScreenManager::GetInstance().dScreenSourceCallbackProxy_ = nullptr;
}

/**
 * @tc.name: OnUnregResult_001
 * @tc.desc: Verify the OnUnregResult function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, OnUnregResult_001, TestSize.Level1)
{
    std::string devId = "devId";
    std::string dhId = "dhId";
    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>(devId, dhId, dScreenCallback);
    std::string reqId;
    int32_t status = 0;
    std::string data;
    DScreenManager::GetInstance().dScreenSourceCallbackProxy_ = nullptr;
    DScreenManager::GetInstance().OnUnregResult(dScreen, reqId, status, data);

    sptr<MockDScreenSourceCallback> mockInstance(new MockDScreenSourceCallback());
    DScreenManager::GetInstance().dScreenSourceCallbackProxy_ = mockInstance;
    EXPECT_CALL(*mockInstance, OnNotifyUnregResult(_, _, _, _, _)).Times(1);
    DScreenManager::GetInstance().OnUnregResult(dScreen, reqId, status, data);
    DScreenManager::GetInstance().dScreenSourceCallbackProxy_ = nullptr;
}

/**
 * @tc.name: EnableDistributedScreen_001
 * @tc.desc: Verify the EnableDistributedScreen function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, EnableDistributedScreen_001, TestSize.Level1)
{
    int32_t ret = DScreenManager::GetInstance().UnInit();
    EXPECT_EQ(DH_SUCCESS, ret);

    ret = DScreenManager::GetInstance().EnableDistributedScreen("", "", EnableParam{"", "", "", ""}, "");
    EXPECT_EQ(ERR_DH_SCREEN_SA_ENABLE_FAILED, ret);

    ret = DScreenManager::GetInstance().Init();
    EXPECT_EQ(DH_SUCCESS, ret);

    std::string devId = "devId000";
    std::string dhId = "dhId000";
    EnableParam param = {"", "", "2.0", "attrs000"};
    std::string reqId = "reqId000";
    ret = DScreenManager::GetInstance().EnableDistributedScreen(devId, dhId, param, reqId);
    EXPECT_EQ(DH_SUCCESS, ret);

    std::shared_ptr<DScreen> dScreen = DScreenManager::GetInstance().FindDScreenByScreenId(SCREEN_ID_INVALID);
    dScreen->SetState(DScreenState::ENABLED);
    ret = DScreenManager::GetInstance().EnableDistributedScreen(devId, dhId, param, reqId);
    EXPECT_EQ(DH_SUCCESS, ret);

    dScreen->SetState(DScreenState::ENABLING);
    ret = DScreenManager::GetInstance().EnableDistributedScreen(devId, dhId, param, reqId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: EnableDistributedScreen_002
 * @tc.desc: Verify the EnableDistributedScreen function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, EnableDistributedScreen_002, TestSize.Level1)
{
    std::string devId = "test";
    std::string dhId = "";
    EnableParam param;
    param.sinkVersion = "";
    param.sinkAttrs = "";
    std::string reqId = "";
    int32_t ret = DScreenManager::GetInstance().EnableDistributedScreen(devId, dhId, param, reqId);
    EXPECT_EQ(ERR_DH_SCREEN_SA_ENABLE_FAILED, ret);

    dhId = "test";
    ret = DScreenManager::GetInstance().EnableDistributedScreen(devId, dhId, param, reqId);
    EXPECT_EQ(ERR_DH_SCREEN_SA_ENABLE_FAILED, ret);

    param.sinkVersion = "test";
    ret = DScreenManager::GetInstance().EnableDistributedScreen(devId, dhId, param, reqId);
    EXPECT_EQ(ERR_DH_SCREEN_SA_ENABLE_FAILED, ret);

    param.sinkAttrs = "test";
    ret = DScreenManager::GetInstance().EnableDistributedScreen(devId, dhId, param, reqId);
    EXPECT_EQ(ERR_DH_SCREEN_SA_ENABLE_FAILED, ret);
}

/**
 * @tc.name: FindDScreenByScreenId_001
 * @tc.desc: Verify the FindDScreenByScreenId function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, FindDScreenByScreenId_001, TestSize.Level1)
{
    std::shared_ptr<DScreen> dScreen = DScreenManager::GetInstance().FindDScreenByScreenId(99);
    EXPECT_EQ(nullptr, dScreen);
}

/**
 * @tc.name: FindDScreenByScreenId_002
 * @tc.desc: Verify the FindDScreenByScreenId function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, FindDScreenByScreenId_002, TestSize.Level1)
{
    uint64_t dScreenId = 999;
    std::string devId = "devId000";
    std::string dhId = "dhId000";
    std::string reqId = "reqId000";
    std::string dScreenIdx = devId + SEPERATOR + dhId;
    std::shared_ptr<DScreen> dScreen = nullptr;
    DScreenManager::GetInstance().dScreens_.clear();
    DScreenManager::GetInstance().dScreens_[dScreenIdx] = dScreen;
    std::shared_ptr<DScreen> screen = DScreenManager::GetInstance().FindDScreenByScreenId(dScreenId);
    EXPECT_EQ(nullptr, screen);
    DScreenManager::GetInstance().dScreens_.clear();
}

/**
 * @tc.name: FindDScreenByScreenId_003
 * @tc.desc: Verify the FindDScreenByScreenId function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, FindDScreenByScreenId_003, TestSize.Level1)
{
    uint64_t dScreenId = 999;
    std::string devId = "devId000";
    std::string dhId = "dhId000";
    std::string reqId = "reqId000";
    std::string dScreenIdx = devId + SEPERATOR + dhId;
    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    dScreen->screenId_ = dScreenId;
    DScreenManager::GetInstance().dScreens_.clear();
    DScreenManager::GetInstance().dScreens_[dScreenIdx] = dScreen;
    std::shared_ptr<DScreen> screen = DScreenManager::GetInstance().FindDScreenByScreenId(dScreenId);
    EXPECT_NE(nullptr, screen);
    DScreenManager::GetInstance().dScreens_.clear();
}

/**
 * @tc.name: GetScreenDumpInfo_001
 * @tc.desc: Verify the GetScreenDumpInfo function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, GetScreenDumpInfo_001, TestSize.Level1)
{
    std::string result;
    DScreenManager::GetInstance().GetScreenDumpInfo(result);
    EXPECT_EQ(false, result.empty());
}

/**
 * @tc.name: GetScreenDumpInfo_002
 * @tc.desc: Verify the GetScreenDumpInfo function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, GetScreenDumpInfo_002, TestSize.Level1)
{
    std::string devId = "devId000";
    std::string dhId = "dhId000";
    std::string reqId = "reqId000";
    std::string dScreenIdx = devId + SEPERATOR + dhId;
    std::shared_ptr<DScreen> dScreen = nullptr;
    DScreenManager::GetInstance().dScreens_[dScreenIdx] = dScreen;
    std::string result;
    DScreenManager::GetInstance().GetScreenDumpInfo(result);
    EXPECT_EQ(false, result.empty());

    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    dScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    DScreenManager::GetInstance().dScreens_[dScreenIdx] = dScreen;
    DScreenManager::GetInstance().GetScreenDumpInfo(result);
    EXPECT_EQ(false, result.empty());

    dScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    std::shared_ptr<VideoParam> videoParam = std::make_shared<VideoParam>();
    videoParam->screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    videoParam->screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    videoParam->videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    videoParam->videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    videoParam->codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    videoParam->videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    videoParam->fps_ = 30.0;
    dScreen->SetVideoParam(videoParam);
    DScreenManager::GetInstance().dScreens_[dScreenIdx] = dScreen;
    DScreenManager::GetInstance().GetScreenDumpInfo(result);
    EXPECT_EQ(false, result.empty());
}

/**
 * @tc.name: HandleDScreenNotify_001
 * @tc.desc: Verify the HandleDScreenNotify function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, HandleDScreenNotify_001, TestSize.Level1)
{
    std::string devId = "devId111";
    int32_t eventCode = -1;
    std::string eventContent = "eventContent111";
    DScreenManager::GetInstance().HandleDScreenNotify(devId, eventCode, eventContent);

    eventCode = NOTIFY_SOURCE_SETUP_RESULT;
    DScreenManager::GetInstance().HandleDScreenNotify(devId, eventCode, eventContent);

    int32_t ret = DScreenManager::GetInstance().Init();
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: NotifyRemoteScreenService_001
 * @tc.desc: Verify the NotifyRemoteScreenService function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, NotifyRemoteScreenService_001, TestSize.Level1)
{
    std::string devId = "devId111";
    int32_t eventCode = NOTIFY_SOURCE_SETUP_RESULT;
    std::string eventContent = "eventContent111";
    int32_t ret = DScreenManager::GetInstance().NotifyRemoteScreenService(devId, eventCode, eventContent);
    EXPECT_EQ(ERR_DH_SCREEN_SA_GET_REMOTE_SINK_SERVICE_FAIL, ret);
}

/**
 * @tc.name: PublishMessage_001
 * @tc.desc: Verify the PublishMessage function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, PublishMessage_001, TestSize.Level1)
{
    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    dScreenCallback->OnRegResult(nullptr, "", 0, "");
    dScreenCallback->OnUnregResult(nullptr, "", 0, "");
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    DScreenManager::GetInstance().PublishMessage(DHTopic::TOPIC_START_DSCREEN, dScreen);

    std::shared_ptr<VideoParam> videoParam = std::make_shared<VideoParam>();
    dScreen->SetVideoParam(videoParam);
    DScreenManager::GetInstance().PublishMessage(DHTopic::TOPIC_START_DSCREEN, dScreen);

    DScreenManager::GetInstance().PublishMessage(DHTopic::TOPIC_STOP_DSCREEN, dScreen);

    DScreenManager::GetInstance().PublishMessage(DHTopic::TOPIC_MIN, dScreen);

    int32_t ret = DScreenManager::GetInstance().Init();
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: NotifyRemoteSinkSetUp_001
 * @tc.desc: Verify the NotifyRemoteSinkSetUp function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, NotifyRemoteSinkSetUp_001, TestSize.Level1)
{
    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    DScreenManager::GetInstance().NotifyRemoteSinkSetUp(dScreen);

    std::shared_ptr<VideoParam> videoParam = std::make_shared<VideoParam>();
    videoParam->screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    videoParam->screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    videoParam->videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    videoParam->videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    videoParam->codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    videoParam->videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    videoParam->fps_ = 30.0;
    dScreen->SetVideoParam(videoParam);
    DScreenManager::GetInstance().NotifyRemoteSinkSetUp(dScreen);

    std::shared_ptr<DScreenMapRelation> mapRelation = std::make_shared<DScreenMapRelation>();
    uint64_t dScreenId = 999;
    dScreen->screenId_ = dScreenId;
    DScreenManager::GetInstance().mapRelations_[dScreenId] = mapRelation;
    DScreenManager::GetInstance().NotifyRemoteSinkSetUp(dScreen);
    EXPECT_TRUE(DScreenManager::GetInstance().localDevId_.empty());
}

/**
 * @tc.name: HandleNotifySetUpResult_001
 * @tc.desc: Verify the HandleNotifySetUpResult function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, HandleNotifySetUpResult_001, TestSize.Level1)
{
    DScreenManager::GetInstance().HandleNotifySetUpResult("remoteDevId", "eventContent");

    std::string eventContent = "{\"width\":\"width000\",\"errCode\":0}";
    DScreenManager::GetInstance().HandleNotifySetUpResult("remoteDevId", eventContent);

    eventContent = "{\"dhId\":\"dhId999\",\"width\":\"width000\",\"errContent\":\"errContent000\"}";
    DScreenManager::GetInstance().HandleNotifySetUpResult("remoteDevId", eventContent);

    eventContent = "{\"dhId\":\"dhId999\",\"errCode\":0,\"width\":\"width000\"}";
    DScreenManager::GetInstance().HandleNotifySetUpResult("remoteDevId", eventContent);

    eventContent = "{\"dhId\":\"dhId999\",\"errCode\":0,\"errContent\":\"errContent000\"}";
    DScreenManager::GetInstance().HandleNotifySetUpResult("remoteDevId", eventContent);

    eventContent = "{\"dhId\":\"dhId000\",\"errCode\":1,\"errContent\":\"errContent000\"}";
    DScreenManager::GetInstance().HandleNotifySetUpResult("devId000", eventContent);

    int32_t ret = DScreenManager::GetInstance().Init();
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: DisableDistributedScreen_001
 * @tc.desc: Verify the DisableDistributedScreen function when dScreens_ is empty.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, DisableDistributedScreen_001, TestSize.Level1)
{
    std::string devId = "devId000";
    std::string dhId = "dhId000";
    std::string reqId = "reqId000";
    int32_t ret = DScreenManager::GetInstance().DisableDistributedScreen(devId, dhId, reqId);
    EXPECT_EQ(DH_SUCCESS, ret);

    std::string dScreenIdx = devId + SEPERATOR + dhId;
    std::shared_ptr<DScreen> dScreen = nullptr;
    DScreenManager::GetInstance().dScreens_.clear();
    DScreenManager::GetInstance().dScreens_[dScreenIdx] = dScreen;
    ret = DScreenManager::GetInstance().DisableDistributedScreen(devId, dhId, reqId);
    EXPECT_EQ(ERR_DH_SCREEN_SA_DISABLE_FAILED, ret);
    DScreenManager::GetInstance().dScreens_.clear();
}

/**
 * @tc.name: DisableDistributedScreen_002
 * @tc.desc: Verify the DisableDistributedScreen function when DScreen state is DISABLED.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, DisableDistributedScreen_002, TestSize.Level1)
{
    std::string devId = "devId000";
    std::string dhId = "dhId000";
    std::string reqId = "reqId000";
    uint64_t dScreenId = 1000;
    std::string dScreenIdx = devId + SEPERATOR + dhId;
    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>(devId, dhId, dScreenCallback);
    dScreen->screenId_ = dScreenId;
    dScreen->SetState(DScreenState::DISABLED);
    DScreenManager::GetInstance().dScreens_.clear();
    DScreenManager::GetInstance().dScreens_[dScreenIdx] = dScreen;
    int32_t ret = DScreenManager::GetInstance().DisableDistributedScreen(devId, dhId, reqId);
    EXPECT_EQ(ERR_DH_SCREEN_SA_DISABLE_FAILED, ret);
    DScreenManager::GetInstance().dScreens_.clear();
}

/**
 * @tc.name: DisableDistributedScreen_003
 * @tc.desc: Verify the DisableDistributedScreen function when DScreen state is ENABLED.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, DisableDistributedScreen_003, TestSize.Level1)
{
    std::string devId = "devId000";
    std::string dhId = "dhId000";
    std::string reqId = "reqId000";
    uint64_t dScreenId = 1000;
    std::string dScreenIdx = devId + SEPERATOR + dhId;
    std::shared_ptr<IDScreenCallback> dScreenCallback = nullptr;
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>(devId, dhId, dScreenCallback);
    dScreen->screenId_ = dScreenId;
    dScreen->SetState(DScreenState::ENABLED);
    DScreenManager::GetInstance().dScreens_.clear();
    DScreenManager::GetInstance().dScreens_[dScreenIdx] = dScreen;
    int32_t ret = DScreenManager::GetInstance().DisableDistributedScreen(devId, dhId, reqId);
    EXPECT_EQ(DH_SUCCESS, ret);
    DScreenManager::GetInstance().dScreens_.clear();
}

/**
 * @tc.name: DisableDistributedScreen_004
 * @tc.desc: Verify the DisableDistributedScreen function when DScreen state is CONNECTING.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, DisableDistributedScreen_004, TestSize.Level1)
{
    std::string devId = "devId000";
    std::string dhId = "dhId000";
    std::string reqId = "reqId000";
    uint64_t dScreenId = 1000;
    std::string dScreenIdx = devId + SEPERATOR + dhId;
    std::shared_ptr<IDScreenCallback> dScreenCallback = nullptr;
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>(devId, dhId, dScreenCallback);
    dScreen->screenId_ = dScreenId;
    dScreen->SetState(DScreenState::CONNECTING);
    DScreenManager::GetInstance().dScreens_.clear();
    DScreenManager::GetInstance().dScreens_[dScreenIdx] = dScreen;
    int32_t ret = DScreenManager::GetInstance().DisableDistributedScreen(devId, dhId, reqId);
    EXPECT_EQ(DH_SUCCESS, ret);
    DScreenManager::GetInstance().dScreens_.clear();
}
}  // namespace V1_0
}  // namespace DistributedHardware
}  // namespace OHOS
