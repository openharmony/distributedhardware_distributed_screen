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

#include "screen_source_trans_test.h"
#include "screentrans_test_utils.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void ScreenSourceTransTest::SetUpTestCase(void) {}

void ScreenSourceTransTest::TearDownTestCase(void) {}

void ScreenSourceTransTest::SetUp(void)
{
    trans = std::make_shared<ScreenSourceTrans>();
}

void ScreenSourceTransTest::TearDown(void) {}

/**
 * @tc.name: SetUp_001
 * @tc.desc: Verify the SetUp function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, SetUp_001, TestSize.Level1)
{
    VideoParam localParam;
    VideoParam remoteParam;
    std::string peerDevId;

    int32_t actual = trans->SetUp(localParam, remoteParam, peerDevId);

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: Release_001
 * @tc.desc: Verify the Release function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, Release_001, TestSize.Level1)
{
    int32_t actual = trans->Release();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: Release_002
 * @tc.desc: Verify the Release function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, Release_002, TestSize.Level1)
{
    trans->imageProcessor_ = std::make_shared<ImageSourceProcessor>();
    std::string peerDevId = "hello";
    trans->screenChannel_ = std::make_shared<ScreenDataChannelImpl>(peerDevId);
    int32_t actual = trans->Release();

    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: Start_001
 * @tc.desc: Verify the Start function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, Start_001, TestSize.Level1)
{
    int32_t actual = trans->Start();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: Start_002
 * @tc.desc: Verify the Start function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, Start_002, TestSize.Level1)
{
    trans->imageProcessor_ = std::make_shared<ImageSourceProcessor>();
    std::string peerDevId = "hello";
    trans->screenChannel_ = std::make_shared<ScreenDataChannelImpl>(peerDevId);
    int32_t actual = trans->Start();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, actual);
}

/**
 * @tc.name: RegisterStateCallback_001
 * @tc.desc: Verify the RegisterStateCallback function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, RegisterStateCallback_001, TestSize.Level1)
{
    std::shared_ptr<IScreenSourceTransCallback> callback = nullptr;
    int32_t actual = trans->RegisterStateCallback(callback);

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: RegisterStateCallback_002
 * @tc.desc: Verify the RegisterStateCallback function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, RegisterStateCallback_002, TestSize.Level1)
{
    std::shared_ptr<IScreenSourceTransCallback> callback =
        std::make_shared<MockIScreenSourceTransCallback>();
    int32_t actual = trans->RegisterStateCallback(callback);

    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: CheckTransParam_001
 * @tc.desc: Verify the CheckTransParam function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, CheckTransParam_001, TestSize.Level1)
{
    VideoParam localParam;
    VideoParam remoteParam;
    std::string peerDevId;
    int32_t actual = trans->CheckTransParam(localParam, remoteParam, peerDevId);

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: CheckTransParam_002
 * @tc.desc: Verify the CheckTransParam function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, CheckTransParam_002, TestSize.Level1)
{
    VideoParam localParam;
    localParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    localParam.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    localParam.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    localParam.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    localParam.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    localParam.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    VideoParam remoteParam;
    remoteParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    remoteParam.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    remoteParam.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    remoteParam.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    remoteParam.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    remoteParam.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    std::string peerDevId = "hello";
    int32_t actual = trans->CheckTransParam(localParam, remoteParam, peerDevId);

    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: OnSessionOpened_001
 * @tc.desc: Verify the OnSessionOpened function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, OnSessionOpened_001, TestSize.Level1)
{
    trans->imageProcessor_ = std::make_shared<ImageSourceProcessor>();

    trans->OnSessionOpened();

    EXPECT_EQ(false, trans->isChannelReady_);
}
}
}
