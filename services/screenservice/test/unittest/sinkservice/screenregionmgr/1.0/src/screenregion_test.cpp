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

#include "1.0/include/screenregion_test.h"
#include "accesstoken_kit.h"
#include "dscreen_constants.h"
#include "display_manager.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
namespace V1_0 {
constexpr static uint32_t VIDEO_DATA_NUM = 480;

void ScreenRegionTestV1::SetUpTestCase(void) {}

void ScreenRegionTestV1::TearDownTestCase(void) {}

void ScreenRegionTestV1::SetUp(void)
{
    const char **perms = new const char *[1];
    perms[0] = "ohos.permission.SYSTEM_FLOAT_WINDOW";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "screen_client_unittest",
        .aplStr = "system_core",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    delete[] perms;

    const std::string remoteDevId = "sourceDevId";
    uint64_t screenId = 0;
    uint64_t displayId = 0;
    screenRegion_ = std::make_shared<ScreenRegion>(remoteDevId, screenId, displayId);
}

void ScreenRegionTestV1::TearDown(void) {}

/**
 * @tc.name: OnTransError_001
 * @tc.desc: Verify the OnTransError function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV1, OnTransError_001, TestSize.Level1)
{
    int32_t err = 0;
    const std::string content = "ScreenRegionTestV1";
    screenRegion_->OnTransError(err, content);
    EXPECT_EQ(false, screenRegion_->isRunning);
}

/**
 * @tc.name: SetMapRelation_001
 * @tc.desc: Verify the SetMapRelation function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV1, SetMapRelation_001, TestSize.Level1)
{
    std::shared_ptr<DScreenMapRelation> mapRelation = std::make_shared<DScreenMapRelation>();
    screenRegion_->SetMapRelation(mapRelation);
    EXPECT_EQ(mapRelation, screenRegion_->mapRelation_);
}

/**
 * @tc.name: GetScreenId_001
 * @tc.desc: Verify the GetScreenId function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV1, GetScreenId_001, TestSize.Level1)
{
    screenRegion_->screenId_ = 5;
    uint64_t ret = screenRegion_->GetScreenId();
    EXPECT_EQ(screenRegion_->screenId_, ret);
}

/**
 * @tc.name: GetDisplayId_001
 * @tc.desc: Verify the GetDisplayId function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV1, GetDisplayId_001, TestSize.Level1)
{
    screenRegion_->displayId_ = 5;
    uint64_t ret = screenRegion_->GetDisplayId();
    EXPECT_EQ(screenRegion_->displayId_, ret);
}

/**
 * @tc.name: GetRemoteDevId_001
 * @tc.desc: Verify the GetRemoteDevId function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV1, GetRemoteDevId_001, TestSize.Level1)
{
    screenRegion_->remoteDevId_ = "remoteDevId";
    std::string ret = screenRegion_->GetRemoteDevId();
    EXPECT_EQ(screenRegion_->remoteDevId_, ret);
}

/**
 * @tc.name: GetWindowId_001
 * @tc.desc: Verify the GetWindowId function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV1, GetWindowId_001, TestSize.Level1)
{
    screenRegion_->windowId_ = 5;
    int32_t ret = screenRegion_->GetWindowId();
    EXPECT_EQ(screenRegion_->windowId_, ret);
}

/**
 * @tc.name: GetVideoParam_001
 * @tc.desc: Verify the GetVideoParam function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV1, GetVideoParam_001, TestSize.Level1)
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
HWTEST_F(ScreenRegionTestV1, GetWindowProperty_001, TestSize.Level1)
{
    screenRegion_->windowProperty_ = std::make_shared<WindowProperty>();
    screenRegion_->windowProperty_->width = VIDEO_DATA_NUM;
    screenRegion_->windowProperty_->height = VIDEO_DATA_NUM;
    screenRegion_->windowProperty_->displayId = Rosen::DisplayManager::GetInstance().GetDefaultDisplay()->GetId();
    std::shared_ptr<WindowProperty> ret = screenRegion_->GetWindowProperty();
    EXPECT_EQ(screenRegion_->windowProperty_, ret);
}

/**
 * @tc.name: SetUpWindow_001
 * @tc.desc: Verify the SetUpWindow function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV1, SetUpWindow_001, TestSize.Level1)
{
    screenRegion_->mapRelation_ = nullptr;
    int32_t ret = screenRegion_->SetUpWindow();
    EXPECT_EQ(ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED, ret);
}

/**
 * @tc.name: SetUp_001
 * @tc.desc: Verify the SetUp function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV1, SetUp_001, TestSize.Level1)
{
    screenRegion_->mapRelation_ = std::make_shared<DScreenMapRelation>();
    ScreenRect screenRect;
    screenRect.startX = 0;
    screenRect.startY = 0;
    screenRect.width = 256;
    screenRect.height = 256;
    screenRegion_->mapRelation_->SetScreenRect(screenRect);
    std::shared_ptr<VideoParam> videoParam = std::make_shared<VideoParam>();
    videoParam->screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    videoParam->screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    videoParam->videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    videoParam->videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    videoParam->codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    videoParam->videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    videoParam->fps_ = 30.0;
    screenRegion_->SetVideoParam(videoParam);
    std::string version = "1.0";
    int32_t ret = screenRegion_->SetUp(version);

    EXPECT_EQ(ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED, ret);
}

/**
 * @tc.name: SetUp_002
 * @tc.desc: Verify the SetUp function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV1, SetUp_002, TestSize.Level1)
{
    screenRegion_->mapRelation_ = nullptr;
    std::string version = "1.0";
    int32_t ret = screenRegion_->SetUp(version);

    EXPECT_EQ(ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED, ret);
}

/**
 * @tc.name: Start_001
 * @tc.desc: Verify the Start function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV1, Start_001, TestSize.Level1)
{
    int32_t ret = screenRegion_->Start();

    EXPECT_EQ(ERR_DH_SCREEN_SA_SINKTRANS_NOT_INIT, ret);
}

/**
 * @tc.name: Start_002
 * @tc.desc: Verify the Start function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV1, Start_002, TestSize.Level1)
{
    screenRegion_->sinkTrans_ = std::make_shared<ScreenSinkTrans>();
    int32_t ret = screenRegion_->Start();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, ret);
}

/**
 * @tc.name: Stop_001
 * @tc.desc: Verify the Stop function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV1, Stop_001, TestSize.Level1)
{
    screenRegion_->isRunning = false;
    int32_t ret = screenRegion_->Stop();

    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: Stop_002
 * @tc.desc: Verify the Stop function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV1, Stop_002, TestSize.Level1)
{
    screenRegion_->isRunning = true;
    int32_t ret = screenRegion_->Stop();

    EXPECT_EQ(ERR_DH_SCREEN_SA_SINKTRANS_NOT_INIT, ret);
}

/**
 * @tc.name: Stop_003
 * @tc.desc: Verify the Stop function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTestV1, Stop_003, TestSize.Level1)
{
    screenRegion_->isRunning = true;
    screenRegion_->sinkTrans_ = std::make_shared<ScreenSinkTrans>();
    int32_t ret = screenRegion_->Stop();

    EXPECT_NE(ERR_DH_SCREEN_SA_SINKTRANS_NOT_INIT, ret);
}
}  // namespace V1_0
}  // namespace DistributedHardware
}  // namespace OHOS
