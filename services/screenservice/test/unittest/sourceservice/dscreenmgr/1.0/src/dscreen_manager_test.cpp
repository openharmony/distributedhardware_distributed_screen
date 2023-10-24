/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

void DScreenManagerTestV1::SetUp() {}

void DScreenManagerTestV1::TearDown() {}

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
 * @tc.desc: Verify the OnRegResult function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, OnRegResult_001, TestSize.Level1)
{
    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<DScreen> changedScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    DScreenManager::GetInstance().OnRegResult(changedScreen, "taskId000", DH_SUCCESS, "dscreen enable success.");

    int32_t ret = DScreenManager::GetInstance().Init();
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: OnUnregResult_001
 * @tc.desc: Verify the OnUnregResult function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, OnUnregResult_001, TestSize.Level1)
{
    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<DScreen> changedScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    DScreenManager::GetInstance().OnUnregResult(changedScreen, "taskId000", DH_SUCCESS, "dscreen disable success.");

    int32_t ret = DScreenManager::GetInstance().Init();
    EXPECT_EQ(DH_SUCCESS, ret);
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
 * @tc.name: GetScreenDumpInfo_002
 * @tc.desc: Verify the GetScreenDumpInfo function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, GetScreenDumpInfo_002, TestSize.Level1)
{
    std::string result;
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
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
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

    int32_t ret = DScreenManager::GetInstance().Init();
    EXPECT_EQ(DH_SUCCESS, ret);
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
 * @tc.desc: Verify the DisableDistributedScreen function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV1, DisableDistributedScreen_001, TestSize.Level1)
{
    int32_t ret = DScreenManager::GetInstance().DisableDistributedScreen("devId111", "dhId111", "reqId111");
    EXPECT_EQ(DH_SUCCESS, ret);

    ret = DScreenManager::GetInstance().UnInit();
    EXPECT_EQ(DH_SUCCESS, ret);
}
}
}
}