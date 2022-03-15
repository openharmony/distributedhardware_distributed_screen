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

#include "image_sink_processor_test.h"

#include <chrono>
#include <securec.h>

#include "dscreen_constants.h"
#include "dscreen_errcode.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void ImageSinkProcessorTest::SetUpTestCase(void) {}

void ImageSinkProcessorTest::TearDownTestCase(void) {}

void ImageSinkProcessorTest::SetUp(void)
{
    param_.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    param_.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    param_.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    param_.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    param_.fps_ = FPS;
    param_.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    param_.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;

    processor_ = std::make_shared<ImageSinkProcessor>();
    imageListener_ = std::make_shared<MockIImageSinkProcessorListener>();
    processor_->imageDecoder_ = std::make_shared<ImageSinkDecoder>(imageListener_);
    processor_->imageDecoder_->videoDecoder_ = Media::VideoDecoderFactory::CreateByMime("video/avc");
}

void ImageSinkProcessorTest::TearDown(void) {}

/**
 * @tc.name: configure_image_processor_test_001
 * @tc.desc: Verify the ConfigureImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkProcessorTest, configure_image_processor_test_001, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, processor_->ConfigureImageProcessor(param_, param_, imageListener_));
}

/**
 * @tc.name: configure_image_processor_test_002
 * @tc.desc: Verify the ConfigureImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkProcessorTest, configure_image_processor_test_002, TestSize.Level1)
{
    param_.codecType_ = VIDEO_CODEC_TYPE_INVALID;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE,
        processor_->ConfigureImageProcessor(param_, param_, imageListener_));
}

/**
 * @tc.name: release_image_processor_test_001
 * @tc.desc: Verify the ReleaseImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkProcessorTest, release_image_processor_test_001, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, processor_->ReleaseImageProcessor());
}

/**
 * @tc.name: release_image_processor_test_002
 * @tc.desc: Verify the ReleaseImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkProcessorTest, release_image_processor_test_002, TestSize.Level1)
{
    processor_->imageDecoder_ = nullptr;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, processor_->ReleaseImageProcessor());
}

/**
 * @tc.name: start_image_processor_test_001
 * @tc.desc: Verify the StartImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkProcessorTest, start_image_processor_test_001, TestSize.Level1)
{
    EXPECT_EQ(ERR_DH_SCREEN_CODEC_PREPARE_FAILED, processor_->StartImageProcessor());
}

/**
 * @tc.name: start_image_processor_test_002
 * @tc.desc: Verify the StartImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkProcessorTest, start_image_processor_test_002, TestSize.Level1)
{
    processor_->imageDecoder_ = nullptr;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, processor_->StartImageProcessor());
}

/**
 * @tc.name: stop_image_processor_test_001
 * @tc.desc: Verify the StopImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkProcessorTest, stop_image_processor_test_001, TestSize.Level1)
{
    EXPECT_EQ(ERR_DH_SCREEN_CODEC_FLUSH_FAILED, processor_->StopImageProcessor());
}

/**
 * @tc.name: stop_image_processor_test_002
 * @tc.desc: Verify the StopImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkProcessorTest, stop_image_processor_test_002, TestSize.Level1)
{
    processor_->imageDecoder_ = nullptr;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, processor_->StopImageProcessor());
}

/**
 * @tc.name: set_image_surface_test_001
 * @tc.desc: Verify the StopImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkProcessorTest, set_image_surface_test_001, TestSize.Level1)
{
    sptr<Surface> surface = Surface::CreateSurfaceAsConsumer("test");
    EXPECT_EQ(ERR_DH_SCREEN_CODEC_SURFACE_ERROR, processor_->SetImageSurface(surface));
}

/**
 * @tc.name: set_image_surface_test_002
 * @tc.desc: Verify the StopImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkProcessorTest, set_image_surface_test_002, TestSize.Level1)
{
    processor_->imageDecoder_ = nullptr;
    sptr<Surface> surface = Surface::CreateSurfaceAsConsumer("test");
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, processor_->SetImageSurface(surface));
}

/**
 * @tc.name: processimage_test_001
 * @tc.desc: Verify the StopImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkProcessorTest, processimage_test_001, TestSize.Level1)
{
    std::shared_ptr<DataBuffer> data = std::make_shared<DataBuffer>(DATA_LEN);
    EXPECT_EQ(DH_SUCCESS, processor_->ProcessImage(data));
}

/**
 * @tc.name: processimage_test_002
 * @tc.desc: Verify the StopImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkProcessorTest, processimage_test_002, TestSize.Level1)
{
    processor_->imageDecoder_ = nullptr;
    std::shared_ptr<DataBuffer> data = std::make_shared<DataBuffer>(DATA_LEN);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, processor_->ProcessImage(data));
}
}
}