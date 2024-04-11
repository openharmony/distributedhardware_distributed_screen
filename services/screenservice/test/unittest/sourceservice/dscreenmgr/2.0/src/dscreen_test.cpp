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

#include "2.0/include/dscreen_test.h"
#include "xcollie/watchdog.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "2.0/include/dscreen_manager.h"
#include "common/include/screen_manager_adapter.h"
#include "dscreen_util.h"
#include "video_param.h"
#include "screen_source_trans.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
namespace V2_0 {
constexpr static uint32_t VIDEO_DATA_NUM = 480;
void DScreenTestV2::SetUpTestCase(void) {}

void DScreenTestV2::TearDownTestCase(void) {}

void DScreenTestV2::SetUp(void)
{
    std::string devId = "devId";
    std::string dhId = "dhId";
    std::shared_ptr<DScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    dScreen_ = std::make_shared<DScreen>(devId, dhId, dScreenCallback);
    dScreen_->videoParam_ = std::make_shared<VideoParam>();
    dScreen_->videoParam_->SetCodecType(VIDEO_CODEC_TYPE_VIDEO_H264);
    dScreen_->videoParam_->SetVideoFormat(VIDEO_DATA_FORMAT_YUVI420);
    dScreen_->videoParam_->SetVideoHeight(VIDEO_DATA_NUM);
    dScreen_->videoParam_->SetVideoWidth(VIDEO_DATA_NUM);
    dScreen_->videoParam_->SetScreenHeight(VIDEO_DATA_NUM);
    dScreen_->videoParam_->SetScreenWidth(VIDEO_DATA_NUM);
    dScreen_->videoParam_->SetFps(DEFAULT_FPS);
    dScreen_->devId_ = "peerDevId";
}

void DScreenTestV2::TearDown(void)
{
    OHOS::HiviewDFX::Watchdog::GetInstance().StopWatchdog();
}

/**
 * @tc.name: AddTask_001
 * @tc.desc: Verify the AddTask function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, AddTask_001, TestSize.Level1)
{
    dScreen_->GetVideoParam();
    DScreenState state = ENABLING;
    dScreen_->SetState(state);
    dScreen_->GetState();
    dScreen_->GetScreenId();
    dScreen_->GetDHId();
    dScreen_->GetDevId();

    std::shared_ptr<Task> task = nullptr;
    int32_t ret = dScreen_->AddTask(task);
    EXPECT_EQ(ERR_DH_SCREEN_SA_DSCREEN_TASK_NOT_VALID, ret);
}

/**
 * @tc.name: HandleTask_001
 * @tc.desc: Verify the HandleTask function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, HandleTask_001, TestSize.Level1)
{
    std::string reqId = "reqId";
    std::string attrs = "attrs";
    std::shared_ptr<Task> task = std::make_shared<Task>(TaskType::TASK_DISCONNECT, reqId, attrs);
    int32_t ret = dScreen_->AddTask(task);
    EXPECT_EQ(DH_SUCCESS, ret);
    task = std::make_shared<Task>(TaskType::TASK_DISABLE, reqId, attrs);
    dScreen_->HandleTask(task);
    task = std::make_shared<Task>(TaskType::TASK_CONNECT, reqId, attrs);
    dScreen_->HandleTask(task);
    task = std::make_shared<Task>(TaskType::TASK_DISCONNECT, reqId, attrs);
    dScreen_->HandleTask(task);
    EXPECT_EQ(SCREEN_ID_INVALID, dScreen_->screenId_);
}

/**
 * @tc.name: HandleEnable_001
 * @tc.desc: Verify the HandleEnable function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, HandleEnable_001, TestSize.Level1)
{
    dScreen_->dscreenCallback_ = nullptr;
    std::string param = "param";
    std::string taskId = "taskId";
    dScreen_->HandleEnable(param, taskId);
    dScreen_-> dscreenCallback_ = std::make_shared<DScreenCallback>();
    DScreenState state = ENABLING;
    dScreen_->SetState(state);
    dScreen_->HandleEnable(param, taskId);
    EXPECT_EQ(SCREEN_ID_INVALID, dScreen_->screenId_);
}

/**
 * @tc.name: HandleDisable_001
 * @tc.desc: Verify the HandleDisable function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, HandleDisable_001, TestSize.Level1)
{
    std::string taskId = "taskId";
    dScreen_->HandleDisable(taskId);
    dScreen_->dscreenCallback_ = std::make_shared<DScreenCallback>();
    dScreen_->HandleDisable(taskId);
    EXPECT_EQ(SCREEN_ID_INVALID, dScreen_->screenId_);
}

/**
 * @tc.name: HandleConnect_001
 * @tc.desc: Verify the HandleConnect function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, HandleConnect_001, TestSize.Level1)
{
    dScreen_->SetState(DISABLED);
    dScreen_->HandleConnect();

    dScreen_->SetState(ENABLED);
    dScreen_->HandleConnect();
    EXPECT_EQ(SCREEN_ID_INVALID, dScreen_->screenId_);
}

/**
 * @tc.name: HandleDisconnect_001
 * @tc.desc: Verify the HandleDisconnect function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, HandleDisconnect_001, TestSize.Level1)
{
    dScreen_->SetState(DISABLED);
    dScreen_->HandleDisconnect();

    dScreen_->SetState(CONNECTED);
    dScreen_->HandleDisconnect();
    EXPECT_EQ(SCREEN_ID_INVALID, dScreen_->screenId_);
}

/**
 * @tc.name: ConfigSurface_001
 * @tc.desc: Verify the ConfigSurface function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, ConfigSurface_001, TestSize.Level1)
{
    int32_t ret = dScreen_->ConfigSurface();
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: RemoveSurface_001
 * @tc.desc: Verify the RemoveSurface function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, RemoveSurface_001, TestSize.Level1)
{
    int32_t ret = dScreen_->RemoveSurface();
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, ret);
}

/**
 * @tc.name: ConsumeSurface_001
 * @tc.desc: Verify the ConsumeSurface function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, ConsumeSurface_001, TestSize.Level1)
{
    dScreen_->ConsumeSurface();
    dScreen_->senderAdapter_ = std::make_shared<AVTransSenderAdapter>();
    dScreen_->ConsumeSurface();
    EXPECT_EQ(SCREEN_ID_INVALID, dScreen_->screenId_);
}

/**
 * @tc.name: InitSenderEngine_001
 * @tc.desc: Verify the InitSenderEngine function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, InitSenderEngine_001, TestSize.Level1)
{
    IAVEngineProvider *providerPtr = nullptr;
    std::string peerDevId = "peer";
    int32_t ret = dScreen_->InitSenderEngine(providerPtr, peerDevId);
    EXPECT_EQ(ERR_DH_AV_TRANS_INIT_FAILED, ret);
}

/**
 * @tc.name: StartSenderEngine_001
 * @tc.desc: Verify the StartSenderEngine function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, StartSenderEngine_001, TestSize.Level1)
{
    int32_t ret = dScreen_->StartSenderEngine();
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, ret);
    dScreen_->senderAdapter_ = std::make_shared<AVTransSenderAdapter>();
    ret = dScreen_->StartSenderEngine();
    EXPECT_EQ(ERR_DH_AV_TRANS_CREATE_CHANNEL_FAILED, ret);
}

/**
 * @tc.name: StartSenderEngine_002
 * @tc.desc: Verify the StartSenderEngine function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, StartSenderEngine_002, TestSize.Level1)
{
    int32_t ret = dScreen_->StartSenderEngine();
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, ret);
    dScreen_->senderAdapter_ = std::make_shared<AVTransSenderAdapter>();
    dScreen_->senderAdapter_->chnCreateSuccess_ = true;
    dScreen_->screenId_ = 1;
    ret = dScreen_->StartSenderEngine();
    EXPECT_EQ(ERR_DH_AV_TRANS_SETUP_FAILED, ret);
}

/**
 * @tc.name: StopSenderEngine_001
 * @tc.desc: Verify the StopSenderEngine function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, StopSenderEngine_001, TestSize.Level1)
{
    dScreen_->senderAdapter_ = nullptr;
    int32_t ret = dScreen_->StopSenderEngine();
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, ret);
    std::string codecType = "codecType";
    std::string pixelFormat = "pixelFormat";
    dScreen_->senderAdapter_ = std::make_shared<AVTransSenderAdapter>();
    dScreen_->ChooseParameter(codecType, pixelFormat);
    dScreen_->dhId_ = "dhId";
    ret = dScreen_->StopSenderEngine();
    EXPECT_EQ(ERR_DH_AV_TRANS_STOP_FAILED, ret);
}

/**
 * @tc.name: SetUp_001
 * @tc.desc: Verify the SetUp function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, SetUp_001, TestSize.Level1)
{
    int32_t ret = dScreen_->SetUp();
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, ret);
    dScreen_->senderAdapter_ = std::make_shared<AVTransSenderAdapter>();
    ret = dScreen_->SetUp();
    EXPECT_EQ(ERR_DH_AV_TRANS_SETUP_FAILED, ret);
}

/**
 * @tc.name: SetUp_002
 * @tc.desc: Verify the SetUp function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, SetUp_002, TestSize.Level1)
{
    dScreen_->senderAdapter_ = std::make_shared<AVTransSenderAdapter>();
    dScreen_->screenId_ = -1;
    int32_t ret = dScreen_->SetUp();
    EXPECT_EQ(ERR_DH_AV_TRANS_SETUP_FAILED, ret);
}

/**
 * @tc.name: SetUp_003
 * @tc.desc: Verify the SetUp function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, SetUp_003, TestSize.Level1)
{
    dScreen_->senderAdapter_ = std::make_shared<AVTransSenderAdapter>();
    dScreen_->screenId_ = 200000;
    int32_t ret = dScreen_->SetUp();
    EXPECT_EQ(ERR_DH_AV_TRANS_SETUP_FAILED, ret);
}

/**
 * @tc.name: SetUp_004
 * @tc.desc: Verify the SetUp function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, SetUp_004, TestSize.Level1)
{
    dScreen_->senderAdapter_ = nullptr;
    int32_t ret = dScreen_->SetUp();
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, ret);
}

/**
 * @tc.name: SetUp_005
 * @tc.desc: Verify the SetUp function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, SetUp_005, TestSize.Level1)
{
    dScreen_->senderAdapter_ = std::make_shared<AVTransSenderAdapter>();
    dScreen_->videoParam_ = nullptr;
    int32_t ret = dScreen_->SetUp();
    EXPECT_EQ(ERR_DH_SCREEN_SA_VALUE_NOT_INIT, ret);
}

/**
 * @tc.name: WaitForSinkStarted_001
 * @tc.desc: Verify the WaitForSinkStarted function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, WaitForSinkStarted_001, TestSize.Level1)
{
    int32_t ret = dScreen_->WaitForSinkStarted();
    EXPECT_EQ(ERR_DH_AV_TRANS_TIMEOUT, ret);
}

/**
 * @tc.name: NegotiateCodecType_001
 * @tc.desc: Verify the NegotiateCodecType function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, NegotiateCodecType_001, TestSize.Level1)
{
    std::string remoteCodecInfoStr = "remoteCodecInfoStr";
    int32_t ret = dScreen_->NegotiateCodecType(remoteCodecInfoStr);
    EXPECT_EQ(ERR_DH_SCREEN_SA_DSCREEN_NEGOTIATE_CODEC_FAIL, ret);
}

/**
 * @tc.name: NegotiateCodecType_002
 * @tc.desc: Verify the NegotiateCodecType function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, NegotiateCodecType_002, TestSize.Level1)
{
    std::string remoteCodecInfoStr = "{\"codecType\":\"[\"OMX_rk_video_encoder_avc\", \"OMX_rk_video_encoder_hevc\", \
        \"avenc_mpeg4\"]\"}";
    int32_t ret = dScreen_->NegotiateCodecType(remoteCodecInfoStr);
    EXPECT_EQ(ERR_DH_SCREEN_SA_DSCREEN_NEGOTIATE_CODEC_FAIL, ret);
}

/**
 * @tc.name: CheckJsonData_001
 * @tc.desc: Verify the CheckJsonData function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, CheckJsonData_001, TestSize.Level1)
{
    json attrJson;
    uint32_t width = 100;
    uint32_t heigth = 100;
    attrJson["screenWidth"] = width;
    attrJson["screenHeight"] = heigth;
    attrJson["codecType"] = 0;
    EXPECT_EQ(true, dScreen_->CheckJsonData(attrJson));
}

/**
 * @tc.name: CheckJsonData_002
 * @tc.desc: Verify the CheckJsonData function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, CheckJsonData_002, TestSize.Level1)
{
    json attrJson;
    std::string taskId = "taskId";
    EXPECT_EQ(false, dScreen_->CheckJsonData(attrJson));
}

/**
 * @tc.name: CheckJsonData_003
 * @tc.desc: Verify the CheckJsonData function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, CheckJsonData_003, TestSize.Level1)
{
    json attrJson;
    attrJson["dhid"] = "dhid";
    std::string taskId = "taskId";
    EXPECT_EQ(false, dScreen_->CheckJsonData(attrJson));
}

/**
 * @tc.name: OnEngineEvent_001
 * @tc.desc: Verify the OnEngineEvent function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV2, OnEngineEvent_001, TestSize.Level1)
{
    DScreenEventType event;
    event = ENGINE_ERROR;
    std::string content = "content";
    dScreen_->OnEngineEvent(event, content);
    event = TRANS_CHANNEL_CLOSED;
    dScreen_->OnEngineEvent(event, content);
    EXPECT_EQ(SCREEN_ID_INVALID, dScreen_->screenId_);
}
}
}
}