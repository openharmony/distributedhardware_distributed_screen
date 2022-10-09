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

#include "screenregion_test.h"
#include "dscreen_constants.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void ScreenRegionTest::SetUpTestCase(void) {}

void ScreenRegionTest::TearDownTestCase(void) {}

void ScreenRegionTest::SetUp(void)
{
    const std::string remoteDevId = "sourceDevId";
    uint64_t screenId = 0;
    uint64_t displayId = 0;
    screenRegion_ = std::make_shared<ScreenRegion>(remoteDevId, screenId, displayId);
}

void ScreenRegionTest::TearDown(void)
{
    sleep(1);
}

/**
 * @tc.name: SetUp_001
 * @tc.desc: Verify the SetUp function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTest, SetUp_001, TestSize.Level1)
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
    videoParam->fps_ = 30;
    screenRegion_->SetVideoParam(videoParam);
    int32_t ret = screenRegion_->SetUp();

    EXPECT_EQ(ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED, ret);
}

/**
 * @tc.name: Start_001
 * @tc.desc: Verify the Start function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionTest, Start_001, TestSize.Level1)
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
HWTEST_F(ScreenRegionTest, Start_002, TestSize.Level1)
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
HWTEST_F(ScreenRegionTest, Stop_001, TestSize.Level1)
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
HWTEST_F(ScreenRegionTest, Stop_002, TestSize.Level1)
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
HWTEST_F(ScreenRegionTest, Stop_003, TestSize.Level1)
{
    screenRegion_->isRunning = true;
    screenRegion_->sinkTrans_ = std::make_shared<ScreenSinkTrans>();
    int32_t ret = screenRegion_->Stop();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, ret);
}
}
}