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

#include "screen_sink_trans_test.h"

#include <chrono>

#include "dscreen_errcode.h"
#include "iconsumer_surface.h"
#include "image_sink_processor.h"
#include "screen_data_channel_impl.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
constexpr static uint32_t VIDEO_DATA_NUM = 480;
void ScreenSinkTransTest::SetUpTestCase(void) {}

void ScreenSinkTransTest::TearDownTestCase(void) {}

void ScreenSinkTransTest::SetUp(void)
{
    peerDevId_ = "test";
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("test");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    trans_ = std::make_shared<ScreenSinkTrans>();
    trans_->imageProcessor_ = std::make_shared<ImageSinkProcessor>();
    trans_->screenChannel_ = std::make_shared<ScreenDataChannelImpl>(peerDevId_);
    trans_->decoderSurface_ = pSurface;
    trans_->transCallback_ = std::make_shared<MockIScreenSinkTransCallback>();

    param_.screenWidth_ = VIDEO_DATA_NUM;
    param_.screenHeight_ = VIDEO_DATA_NUM;
    param_.videoWidth_ = VIDEO_DATA_NUM;
    param_.videoHeight_ = VIDEO_DATA_NUM;
    param_.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    param_.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    param_.fps_ = FPS;
}

void ScreenSinkTransTest::TearDown(void)
{
    if (trans_ != nullptr) {
        trans_->Stop();
        trans_->Release();
    }
}

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
 * @tc.name: register_processor_listener_002
 * @tc.desc: Verify the RegisterChannelListener function success.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, register_processor_listener_002, TestSize.Level1)
{
    trans_->imageProcessor_ = nullptr;
    int32_t ret = trans_->RegisterProcessorListener(param_, param_, peerDevId_);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, ret);
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
    trans_->transCallback_ = callback;
    trans_->OnProcessorStateNotify(DH_SUCCESS);
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
    sptr<IConsumerSurface> surface = IConsumerSurface::Create("test");
    sptr<IBufferProducer> bufferProducer = surface->GetProducer();
    sptr<Surface> producerSurface = Surface::CreateSurfaceAsProducer(bufferProducer);
    EXPECT_EQ(DH_SUCCESS, trans_->SetImageSurface(producerSurface));
}

/**
 * @tc.name: set_image_surface_test_002
 * @tc.desc: Verify the SetImageSurface function fail.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSinkTransTest, set_image_surface_test_002, TestSize.Level1)
{
    trans_->transCallback_ = std::make_shared<MockIScreenSinkTransCallback>();
    trans_->OnSessionClosed();
    trans_->OnProcessorStateNotify(DH_SUCCESS);
    trans_->OnDataReceived(nullptr);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, trans_->SetImageSurface(nullptr));
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
}
}