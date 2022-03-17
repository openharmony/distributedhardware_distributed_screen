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

#include "screen_sink_trans_test.h"

#include <chrono>

#include "dscreen_errcode.h"
#include "image_sink_processor.h"
#include "screen_data_channel_impl.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void ScreenSinkTransTest::SetUpTestCase(void) {}

void ScreenSinkTransTest::TearDownTestCase(void) {}

void ScreenSinkTransTest::SetUp(void)
{
    peerDevId_ = "test";
    trans_ = std::make_shared<ScreenSinkTrans>();
    trans_->imageProcessor_ = std::make_shared<ImageSinkProcessor>();
    trans_->screenChannel_ = std::make_shared<ScreenDataChannelImpl>(peerDevId_);
    trans_->decoderSurface_ = Surface::CreateSurfaceAsConsumer("test");
    trans_->transCallback_ = std::make_shared<MockIScreenSinkTransCallback>();

    param_.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    param_.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    param_.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    param_.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    param_.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    param_.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    param_.fps_ = FPS;
}

void ScreenSinkTransTest::TearDown(void) {}

/**
 * @tc.name: setup_test_001
 * @tc.desc: Verify the SetUp function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, setup_test_001, TestSize.Level1)
{
    param_.screenWidth_ = WIDTH_INVALID;
    param_.screenHeight_ = HEIGHT_INVALID;
    param_.videoWidth_ = WIDTH_INVALID;
    param_.videoHeight_ = HEIGHT_INVALID;

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM, trans_->SetUp(param_, param_, peerDevId_));
}

/**
 * @tc.name: register_processor_listener_001
 * @tc.desc: Verify the RegisterChannelListener function success.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, register_processor_listener_001, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, trans_->RegisterProcessorListener(param_, param_, peerDevId_));
}

/**
 * @tc.name: release_test_001
 * @tc.desc: Verify the Release function success.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, release_test_001, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, trans_->Release());
}

/**
 * @tc.name: release_test_002
 * @tc.desc: Verify the Release function fail.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, release_test_002, TestSize.Level1)
{
    trans_->imageProcessor_ = nullptr;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, trans_->Release());
}

/**
 * @tc.name: start_test_002
 * @tc.desc: Verify the Start function fail.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, start_test_002, TestSize.Level1)
{
    trans_->imageProcessor_ = nullptr;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, trans_->Start());
}

/**
 * @tc.name: stop_test_002
 * @tc.desc: Verify the Stop function success.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, stop_test_002, TestSize.Level1)
{
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, trans_->Stop());
}

/**
 * @tc.name: register_state_callback_test_001
 * @tc.desc: Verify the RegisterStateCallback function success.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, register_state_callback_test_001, TestSize.Level1)
{
    std::shared_ptr<IScreenSinkTransCallback> callback =
        std::make_shared<MockIScreenSinkTransCallback>();
    EXPECT_EQ(DH_SUCCESS, trans_->RegisterStateCallback(callback));
}

/**
 * @tc.name: register_state_callback_test_002
 * @tc.desc: Verify the RegisterStateCallback function fail.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, register_state_callback_test_002, TestSize.Level1)
{
    std::shared_ptr<IScreenSinkTransCallback> callback = nullptr;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, trans_->RegisterStateCallback(callback));
}

/**
 * @tc.name: set_image_surface_test_001
 * @tc.desc: Verify the SetImageSurface function success.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, set_image_surface_test_001, TestSize.Level1)
{
    sptr<Surface> surface = Surface::CreateSurfaceAsConsumer("test");
    EXPECT_EQ(DH_SUCCESS, trans_->SetImageSurface(surface));
}

/**
 * @tc.name: set_image_surface_test_002
 * @tc.desc: Verify the SetImageSurface function fail.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, set_image_surface_test_002, TestSize.Level1)
{
    sptr<Surface> surface = nullptr;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, trans_->SetImageSurface(surface));
}

/**
 * @tc.name: check_trans_param_test_001
 * @tc.desc: Verify the CheckTransParam function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, check_trans_param_test_001, TestSize.Level1)
{
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, trans_->CheckTransParam(param_, param_, ""));

    param_.codecType_ = VIDEO_CODEC_TYPE_INVALID;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM, trans_->CheckTransParam(param_, param_, peerDevId_));

    param_.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    param_.videoFormat_ = VIDEO_DATA_FORMAT_INVALID;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM, trans_->CheckTransParam(param_, param_, peerDevId_));

    param_.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    param_.videoWidth_ = WIDTH_INVALID;
    param_.videoHeight_ = HEIGHT_INVALID;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM, trans_->CheckTransParam(param_, param_, peerDevId_));

    param_.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    param_.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    param_.screenWidth_ = WIDTH_INVALID;
    param_.screenHeight_ = HEIGHT_INVALID;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM, trans_->CheckTransParam(param_, param_, peerDevId_));

    param_.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    param_.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    EXPECT_EQ(DH_SUCCESS, trans_->CheckTransParam(param_, param_, peerDevId_));
}

/**
 * @tc.name: on_session_closed_test_001
 * @tc.desc: Verify the OnSessionClosed function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, on_session_closed_test_001, TestSize.Level1)
{
    trans_->transCallback_ = std::make_shared<MockIScreenSinkTransCallback>();
    trans_->OnSessionClosed();
}

/**
 * @tc.name: on_data_received_test_001
 * @tc.desc: Verify the OnDataReceived function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, on_data_received_test_001, TestSize.Level1)
{
    std::shared_ptr<DataBuffer> dataBuffer = nullptr;
    trans_->OnDataReceived(dataBuffer);
}

/**
 * @tc.name: on_processor_state_notify_test_001
 * @tc.desc: Verify the RegisterChannelListener function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, on_processor_state_notify_test_001, TestSize.Level1)
{
    int32_t state = DH_SUCCESS;
    trans_->transCallback_ = std::make_shared<MockIScreenSinkTransCallback>();
    trans_->OnProcessorStateNotify(state);
}

/**
 * @tc.name: on_processor_state_notify_test_002
 * @tc.desc: Verify the RegisterChannelListener function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, on_processor_state_notify_test_002, TestSize.Level1)
{
    int32_t state = DH_SUCCESS;
    std::shared_ptr<IScreenSinkTransCallback> callback = nullptr;
    trans_->transCallback_ = callback;
    trans_->OnProcessorStateNotify(state);
}
}
}