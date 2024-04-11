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

#include "1.0/include/dscreen_test.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "1.0/include/dscreen_manager.h"
#include "common/include/screen_manager_adapter.h"
#include "dscreen_util.h"
#include "video_param.h"
#include "screen_source_trans.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
namespace V1_0 {
constexpr static uint32_t VIDEO_DATA_NUM = 480;
void DScreenTestV1::SetUpTestCase(void) {}

void DScreenTestV1::TearDownTestCase(void) {}

void DScreenTestV1::SetUp(void)
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

void DScreenTestV1::TearDown(void) {}

void EnablePermissionAccess(const char* perms[], size_t permsNum, uint64_t &tokenId)
{
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = permsNum,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .aplStr = "system_basic",
    };

    infoInstance.processName = "DscreenMgrTest";
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

void DisablePermissionAccess(const uint64_t &tokenId)
{
    OHOS::Security::AccessToken::AccessTokenKit::DeleteToken(tokenId);
}

/**
 * @tc.name: AddTask_001
 * @tc.desc: Verify the AddTask function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, AddTask_001, TestSize.Level1)
{
    int32_t err = 0;
    std::string content = "content";
    dScreen_->OnTransError(err, content);

    std::shared_ptr<VideoParam> videoParam = nullptr;
    dScreen_->SetVideoParam(videoParam);

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
 * @tc.name: AddTask_001
 * @tc.desc: Verify the AddTask function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, AddTask_002, TestSize.Level1)
{
    std::string reqId = "reqId";
    std::string attrs = "attrs";
    std::shared_ptr<Task> task = std::make_shared<Task>(TaskType::TASK_ENABLE, reqId, attrs);
    int32_t ret = dScreen_->AddTask(task);
    EXPECT_EQ(DH_SUCCESS, ret);
    task = std::make_shared<Task>(TaskType::TASK_DISABLE, reqId, attrs);
    dScreen_->HandleTask(task);
    task = std::make_shared<Task>(TaskType::TASK_CONNECT, reqId, attrs);
    dScreen_->HandleTask(task);
    task = std::make_shared<Task>(TaskType::TASK_DISCONNECT, reqId, attrs);
    dScreen_->HandleTask(task);
}

/**
 * @tc.name: HandleEnable_001
 * @tc.desc: Verify the HandleEnable function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, HandleEnable_001, TestSize.Level1)
{
    std::string reqId = "reqId";
    std::string attrs = "attrs";
    std::shared_ptr<Task> task = std::make_shared<Task>(TaskType::TASK_ENABLE, reqId, attrs);
    dScreen_->dscreenCallback_ = nullptr;
    std::string param = "param";
    std::string taskId = "taskId";
    dScreen_->HandleEnable(param, taskId);
    int32_t ret = dScreen_->AddTask(task);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: HandleEnable_002
 * @tc.desc: Verify the HandleEnable function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, HandleEnable_002, TestSize.Level1)
{
    std::string reqId = "reqId";
    std::string attrs = "attrs";
    std::shared_ptr<Task> task = std::make_shared<Task>(TaskType::TASK_DISCONNECT, reqId, attrs);
    dScreen_->curState_ = ENABLED;
    dScreen_->videoParam_ = nullptr;
    std::string param = "{\"codecType\":\"OMX_rk_video_encoder_avc\", \
        \"screenHeight\":1280, \"screenVersion\":\"1.0\", \"screenWidth\":720}";
    std::string taskId = "taskId";
    dScreen_->HandleEnable(param, taskId);
    int32_t ret = dScreen_->AddTask(task);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: HandleEnable_003
 * @tc.desc: Verify the HandleEnable function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, HandleEnable_003, TestSize.Level1)
{
    std::string reqId = "reqId";
    std::string attrs = "attrs";
    std::shared_ptr<Task> task = std::make_shared<Task>(TaskType::TASK_DISCONNECT, reqId, attrs);
    dScreen_->videoParam_ = std::make_shared<VideoParam>();
    std::string param = "{\"codecType\":\"OMX_rk_video_encoder_avc\", \
        \"screenHeight\":1280, \"screenVersion\":\"1.0\", \"screenWidth\":720}";
    std::string taskId = "taskId";
    dScreen_->HandleEnable(param, taskId);
    int32_t ret = dScreen_->AddTask(task);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: HandleEnable_004
 * @tc.desc: Verify the HandleEnable function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, HandleEnable_004, TestSize.Level1)
{
    std::string reqId = "reqId";
    std::string attrs = "attrs";
    std::shared_ptr<Task> task = std::make_shared<Task>(TaskType::TASK_DISCONNECT, reqId, attrs);
    dScreen_->videoParam_ = std::make_shared<VideoParam>();
    std::string param = "{\"codecType\":\"[\"OMX_rk_video_encoder_avc\", \"OMX_rk_video_encoder_hevc\", \
        \"avenc_mpeg4\"]\", \"screenHeight\":1280, \"screenVersion\":\"1.0\", \"screenWidth\":720}";
    std::string taskId = "taskId";
    dScreen_->HandleEnable(param, taskId);
    int32_t ret = dScreen_->AddTask(task);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: HandleEnable_005
 * @tc.desc: Verify the HandleEnable function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, HandleEnable_005, TestSize.Level1)
{
    std::string reqId = "reqId";
    std::string attrs = "attrs";
    std::shared_ptr<Task> task = std::make_shared<Task>(TaskType::TASK_DISCONNECT, reqId, attrs);
    dScreen_->videoParam_ = std::make_shared<VideoParam>();
    std::string param = "{\"codecType\":\"[\"OMX_rk_video_encoder_avc\", \"OMX_rk_video_encoder_hevc\", \
        \"avenc_mpeg4\"]\", \"screenHeight\":1280, \"screenVersion\":\"1.0\", \"screenWidth\":720}";

    std::string screenName = DSCREEN_PREFIX + SEPERATOR + GetInterruptString(dScreen_->devId_) +
                             SEPERATOR + GetInterruptString(dScreen_->dhId_);
    ScreenMgrAdapter::GetInstance().screenIdMap_.emplace(screenName, 100);
    std::string taskId = "taskId";
    dScreen_->HandleEnable(param, taskId);
    int32_t ret = dScreen_->AddTask(task);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: CheckJsonData_001
 * @tc.desc: Verify the CheckJsonData function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, CheckJsonData_001, TestSize.Level1)
{
    json attrJson;

    std::string taskId = "taskId";
    dScreen_->dscreenCallback_ = nullptr;
    dScreen_->HandleDisable(taskId);
    dScreen_->HandleConnect();
    dScreen_->curState_ = CONNECTED;
    dScreen_->HandleDisconnect();

    int32_t ret = dScreen_->CheckJsonData(attrJson);
    EXPECT_EQ(ERR_DH_SCREEN_SA_ENABLE_JSON_ERROR, ret);
}

/**
 * @tc.name: CheckJsonData_002
 * @tc.desc: Verify the CheckJsonData function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, CheckJsonData_002, TestSize.Level1)
{
    json attrJson;
    attrJson["dhid"] = "dhid";
    std::string taskId = "taskId";
    dScreen_->screenId_ = 100;
    dScreen_->HandleDisable(taskId);
    dScreen_->curState_ = DISABLED;
    dScreen_->HandleDisconnect();

    int32_t ret = dScreen_->CheckJsonData(attrJson);
    EXPECT_EQ(ERR_DH_SCREEN_SA_ENABLE_JSON_ERROR, ret);
}

/**
 * @tc.name: CheckJsonData_003
 * @tc.desc: Verify the CheckJsonData function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, CheckJsonData_003, TestSize.Level1)
{
    json attrJson;
    uint32_t width = 100;
    uint32_t heigth = 100;
    attrJson["screenWidth"] = width;
    attrJson["screenHeight"] = heigth;
    attrJson["codecType"] = 0;

    std::string devId = "devId";
    std::string dhId = "dhId";
    ScreenMgrAdapter::GetInstance().screenIdMap_.clear();
    std::shared_ptr<VideoParam> videoParam = std::make_shared<VideoParam>();
    uint64_t screenId = ScreenMgrAdapter::GetInstance().CreateVirtualScreen(devId, dhId, videoParam);
    std::string taskId = "taskId";
    dScreen_->screenId_ = screenId;
    dScreen_->HandleDisable(taskId);

    int32_t ret = dScreen_->CheckJsonData(attrJson);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: NegotiateCodecType_001
 * @tc.desc: Verify the NegotiateCodecType function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, NegotiateCodecType_001, TestSize.Level1)
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
HWTEST_F(DScreenTestV1, NegotiateCodecType_002, TestSize.Level1)
{
    std::string remoteCodecInfoStr = "{\"codecType\":\"[\"OMX_rk_video_encoder_avc\", \"OMX_rk_video_encoder_hevc\", \
        \"avenc_mpeg4\"]\"}";
    int32_t ret = dScreen_->NegotiateCodecType(remoteCodecInfoStr);
    EXPECT_EQ(ERR_DH_SCREEN_SA_DSCREEN_NEGOTIATE_CODEC_FAIL, ret);
}

/**
 * @tc.name: SetUp_001
 * @tc.desc: Verify the SetUp function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, SetUp_001, TestSize.Level1)
{
    const char* perms[] = {
        "ohos.permission.DISTRIBUTED_DATASYNC",
        "ohos.permission.CAPTURE_SCREEN",
    };
    EnablePermissionAccess(perms, sizeof(perms) / sizeof(perms[0]), tokenId_);
    dScreen_->sourceTrans_ = nullptr;
    dScreen_->version_ = "1.0";
    int32_t ret = dScreen_->SetUp();
    EXPECT_EQ(DH_SUCCESS, ret);
    DisablePermissionAccess(tokenId_);
}

/**
 * @tc.name: SetUp_002
 * @tc.desc: Verify the SetUp function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, SetUp_002, TestSize.Level1)
{
    const char* perms[] = {
        "ohos.permission.DISTRIBUTED_DATASYNC",
        "ohos.permission.CAPTURE_SCREEN",
    };
    EnablePermissionAccess(perms, sizeof(perms) / sizeof(perms[0]), tokenId_);
    dScreen_->sourceTrans_ = std::make_shared<ScreenSourceTrans>();
    dScreen_->version_ = "1.0";
    int32_t ret = dScreen_->SetUp();
    EXPECT_EQ(DH_SUCCESS, ret);
    DisablePermissionAccess(tokenId_);
}

/**
 * @tc.name: Start_001
 * @tc.desc: Verify the Start function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, Start_001, TestSize.Level1)
{
    dScreen_->sourceTrans_ = nullptr;
    int32_t ret = dScreen_->Start();
    EXPECT_EQ(ERR_DH_SCREEN_SA_SOURCETRANS_NOT_INIT, ret);
}

/**
 * @tc.name: Start_002
 * @tc.desc: Verify the Start function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, Start_002, TestSize.Level1)
{
    dScreen_->sourceTrans_ = std::make_shared<ScreenSourceTrans>();
    int32_t ret = dScreen_->Start();
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, ret);
}

/**
 * @tc.name: Stop_001
 * @tc.desc: Verify the Stop function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, Stop_001, TestSize.Level1)
{
    dScreen_->sourceTrans_ = nullptr;
    int32_t ret = dScreen_->Stop();
    EXPECT_EQ(ERR_DH_SCREEN_SA_SOURCETRANS_NOT_INIT, ret);
}

/**
 * @tc.name: Stop_002
 * @tc.desc: Verify the Stop function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenTestV1, Stop_002, TestSize.Level1)
{
    dScreen_->sourceTrans_ = std::make_shared<ScreenSourceTrans>();
    int32_t ret = dScreen_->Stop();
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, ret);
}
}
}
}