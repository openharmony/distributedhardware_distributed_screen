/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "2.0/include/dscreen_manager_test.h"
#include "common/include/screen_manager_adapter.h"
#include "xcollie/watchdog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
namespace V2_0 {
void DScreenManagerTestV2::SetUpTestCase(void) {}

void DScreenManagerTestV2::TearDownTestCase(void) {}

void DScreenManagerTestV2::SetUp() {}

void DScreenManagerTestV2::TearDown()
{
    OHOS::HiviewDFX::Watchdog::GetInstance().StopWatchdog();
}

/**
 * @tc.name: Initialize_001
 * @tc.desc: Verify the Init function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, Initialize_001, TestSize.Level1)
{
    sptr<DScreenGroupListener> dScreenGroupListener_(new (std::nothrow) DScreenGroupListener());
    int32_t ret = DScreenManager::GetInstance().Initialize();
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: Release_001
 * @tc.desc: Verify the Release function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, Release_001, TestSize.Level1)
{
    int32_t ret = DScreenManager::GetInstance().Release();
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: HandleScreenChange_001
 * @tc.desc: Verify the HandleScreenChange function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, HandleScreenChange_001, TestSize.Level1)
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

    int32_t ret = DScreenManager::GetInstance().Initialize();
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: StartDScreenMirror_001
 * @tc.desc: Verify the StartDScreenMirror function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, StartDScreenMirror_001, TestSize.Level1)
{
    std::shared_ptr<DScreen> changedScreen = nullptr;
    int32_t ret = DScreenManager::GetInstance().StartDScreenMirror(changedScreen);
    EXPECT_EQ(ERR_DH_SCREEN_SA_VALUE_NOT_INIT, ret);

    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    changedScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    ret = DScreenManager::GetInstance().StartDScreenMirror(changedScreen);

    changedScreen->SetState(CONNECTING);
    ret = DScreenManager::GetInstance().StartDScreenMirror(changedScreen);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: StopDScreenMirror_001
 * @tc.desc: Verify the StopDScreenMirror function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, StopDScreenMirror_001, TestSize.Level1)
{
    std::shared_ptr<DScreen> changedScreen = nullptr;
    int32_t ret = DScreenManager::GetInstance().StopDScreenMirror(changedScreen);
    EXPECT_EQ(ERR_DH_SCREEN_SA_VALUE_NOT_INIT, ret);

    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    changedScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    ret = DScreenManager::GetInstance().StopDScreenMirror(changedScreen);
    EXPECT_EQ(DH_SUCCESS, ret);

    changedScreen->SetState(CONNECTING);
    ret = DScreenManager::GetInstance().StopDScreenMirror(changedScreen);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: OnRegResult_001
 * @tc.desc: Verify the OnRegResult function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, OnRegResult_001, TestSize.Level1)
{
    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<DScreen> changedScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    DScreenManager::GetInstance().OnRegResult(changedScreen, "taskId000", DH_SUCCESS, "dscreen enable success.");

    int32_t ret = DScreenManager::GetInstance().Release();
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: OnUnregResult_001
 * @tc.desc: Verify the OnUnregResult function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, OnUnregResult_001, TestSize.Level1)
{
    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<DScreen> changedScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    DScreenManager::GetInstance().OnUnregResult(changedScreen, "taskId000", DH_SUCCESS, "dscreen disable success.");

    int32_t ret = DScreenManager::GetInstance().Release();
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: EnableDistributedScreen_001
 * @tc.desc: Verify the EnableDistributedScreen function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, EnableDistributedScreen_001, TestSize.Level1)
{
    int32_t ret = DScreenManager::GetInstance().EnableDistributedScreen("", "", EnableParam{"", "", "", ""}, "");
    EXPECT_EQ(ERR_DH_SCREEN_SA_ENABLE_FAILED, ret);

    std::string devId = "devId000";
    std::string dhId = "dhId000";
    EnableParam param = {"", "", "2.0", "attrs000"};
    std::string reqId = "reqId000";

    DScreenManager::GetInstance().dScreenCallback_ = std::make_shared<DScreenCallback>();
    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>(devId, dhId, dScreenCallback);
    dScreen->SetState(DScreenState::ENABLED);
    ret = DScreenManager::GetInstance().EnableDistributedScreen(devId, dhId, param, reqId);
    EXPECT_EQ(DH_SUCCESS, ret);

    dScreen->SetState(DScreenState::DISCONNECTING);
    ret = DScreenManager::GetInstance().EnableDistributedScreen(devId, dhId, param, reqId);

    dScreen->SetState(DScreenState::ENABLING);
    ret = DScreenManager::GetInstance().EnableDistributedScreen(devId, dhId, param, reqId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: DisableDistributedScreen_001
 * @tc.desc: Verify the DisableDistributedScreen function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, DisableDistributedScreen_001, TestSize.Level1)
{
    int32_t ret = DScreenManager::GetInstance().DisableDistributedScreen("devId111", "dhId111", "reqId111");
    EXPECT_EQ(DH_SUCCESS, ret);

    ret = DScreenManager::GetInstance().Release();
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: DisableDistributedScreen_002
 * @tc.desc: erify the DisableDistributedScreen function failed
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, DisableDistributedScreen_002, TestSize.Level1)
{
    std::shared_ptr<IDScreenCallback> dScreenCallback = nullptr;
    std::string devId = "devId000";
    std::string dhId = "dhId000";
    std::string reqId = "reqId000";
    std::string dScreenIdx = devId + SEPERATOR + dhId;
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>(devId, dhId, dScreenCallback);
    DScreenManager::GetInstance().dScreens_[dScreenIdx] = dScreen;

    DScreenManager::GetInstance().dScreens_[dScreenIdx]->SetState(DScreenState::DISABLED);
    int32_t ret = DScreenManager::GetInstance().DisableDistributedScreen(devId, dhId, reqId);
    EXPECT_EQ(ERR_DH_SCREEN_SA_DISABLE_FAILED, ret);

    DScreenManager::GetInstance().dScreens_[dScreenIdx]->SetState(DScreenState::DISABLING);
    ret = DScreenManager::GetInstance().DisableDistributedScreen(devId, dhId, reqId);
    EXPECT_EQ(ERR_DH_SCREEN_SA_DISABLE_FAILED, ret);

    DScreenManager::GetInstance().dScreens_[dScreenIdx]->SetState(DScreenState::ENABLED);
    ret = DScreenManager::GetInstance().DisableDistributedScreen(devId, dhId, reqId);
    EXPECT_EQ(DH_SUCCESS, ret);

    DScreenManager::GetInstance().dScreens_[dScreenIdx]->SetState(DScreenState::ENABLING);
    ret = DScreenManager::GetInstance().DisableDistributedScreen(devId, dhId, reqId);
    EXPECT_EQ(DH_SUCCESS, ret);

    DScreenManager::GetInstance().dScreens_[dScreenIdx]->SetState(DScreenState::CONNECTING);
    ret = DScreenManager::GetInstance().DisableDistributedScreen(devId, dhId, reqId);
    EXPECT_EQ(DH_SUCCESS, ret);

    DScreenManager::GetInstance().dScreens_[dScreenIdx]->SetState(DScreenState::CONNECTED);
    ret = DScreenManager::GetInstance().DisableDistributedScreen(devId, dhId, reqId);
    EXPECT_EQ(DH_SUCCESS, ret);

    DScreenManager::GetInstance().dScreens_[dScreenIdx]->SetState(DScreenState::DISCONNECTING);
    ret = DScreenManager::GetInstance().DisableDistributedScreen(devId, dhId, reqId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: FindDScreenById_001
 * @tc.desc: Verify the FindDScreenById function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, FindDScreenById_001, TestSize.Level1)
{
    std::shared_ptr<DScreen> dScreen = DScreenManager::GetInstance().FindDScreenById(99);
    EXPECT_EQ(nullptr, dScreen);
}

/**
 * @tc.name: FindDScreenById_002
 * @tc.desc: Verify the FindDScreenById function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, FindDScreenById_002, TestSize.Level1)
{
    std::string devId = "devId000";
    std::string dhId = "dhId000";
    std::string dScreenIdx = devId + SEPERATOR + dhId;
    std::shared_ptr<DScreen> dScreen = nullptr;
    DScreenManager::GetInstance().dScreens_[dScreenIdx] = dScreen;
    EXPECT_EQ(nullptr, DScreenManager::GetInstance().FindDScreenById(99));
}

/**
 * @tc.name: FindDScreenById_003
 * @tc.desc: Verify the FindDScreenById function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, FindDScreenById_003, TestSize.Level1)
{
    std::shared_ptr<IDScreenCallback> dScreenCallback = nullptr;
    std::string devId = "devId000";
    std::string dhId = "dhId000";
    std::string dScreenIdx = devId + SEPERATOR + dhId;
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>(devId, dhId, dScreenCallback);
    dScreen->screenId_ = 99;
    DScreenManager::GetInstance().dScreens_[dScreenIdx] = dScreen;
    EXPECT_EQ(dScreen, DScreenManager::GetInstance().FindDScreenById(99));
}

/**
 * @tc.name: FindDScreenById_004
 * @tc.desc: Verify the FindDScreenById function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, FindDScreenById_004, TestSize.Level1)
{
    std::shared_ptr<IDScreenCallback> dScreenCallback = nullptr;
    std::string devId = "devId000";
    std::string dhId = "dhId000";
    std::string dScreenIdx = devId + SEPERATOR + dhId;
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>(devId, dhId, dScreenCallback);
    dScreen->screenId_ = 90;
    DScreenManager::GetInstance().dScreens_[dScreenIdx] = dScreen;
    EXPECT_EQ(nullptr, DScreenManager::GetInstance().FindDScreenById(99));
}

/**
 * @tc.name: GetScreenDumpInfo_001
 * @tc.desc: Verify the GetScreenDumpInfo function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, GetScreenDumpInfo_001, TestSize.Level1)
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
HWTEST_F(DScreenManagerTestV2, GetScreenDumpInfo_002, TestSize.Level1)
{
    std::string result;
    DScreenManager::GetInstance().GetScreenDumpInfo(result);
    EXPECT_EQ(false, result.empty());
}

/**
 * @tc.name: PublishMessage_001
 * @tc.desc: Verify the PublishMessage function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerTestV2, PublishMessage_001, TestSize.Level1)
{
    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<DScreen> dScreen = std::make_shared<DScreen>("devId000", "dhId000", dScreenCallback);
    DScreenManager::GetInstance().PublishMessage(DHTopic::TOPIC_START_DSCREEN, dScreen);

    DScreenManager::GetInstance().PublishMessage(DHTopic::TOPIC_STOP_DSCREEN, dScreen);

    DScreenManager::GetInstance().PublishMessage(DHTopic::TOPIC_MIN, dScreen);

    int32_t ret = DScreenManager::GetInstance().Release();
    EXPECT_EQ(DH_SUCCESS, ret);
}
}
}
}