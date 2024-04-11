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
#include "jpeg_image_processor_test.h"
#include "data_buffer.h"
#include "dscreen_errcode.h"
#include "iconsumer_surface.h"
#include "surface_type.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void ScreenImageJpegTest::SetUpTestCase(void) {}

void ScreenImageJpegTest::TearDownTestCase(void) {}

void ScreenImageJpegTest::SetUp(void)
{
    param_.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    param_.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    param_.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    param_.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    param_.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    param_.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    jpeg_ = std::make_shared<JpegImageProcessor>(param_);
}

void ScreenImageJpegTest::TearDown(void) {}

/**
 * @tc.name: SetOutputSurface_001
 * @tc.desc: Verify the SetOutputSurface function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenImageJpegTest, SetOutputSurface_001, TestSize.Level1)
{
    sptr<Surface> jpegSurface = IConsumerSurface::Create();
    int32_t ret = jpeg_->SetOutputSurface(jpegSurface);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: SetOutputSurface_002
 * @tc.desc: Verify the SetOutputSurface function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenImageJpegTest, SetOutputSurface_002, TestSize.Level1)
{
    int32_t ret = jpeg_->SetOutputSurface(nullptr);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, ret);
}

/**
 * @tc.name: FillDirtyImages2Surface_001
 * @tc.desc: Verify the FillDirtyImages2Surface function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenImageJpegTest, FillDirtyImages2Surface_001, TestSize.Level1)
{
    jpeg_->imageSurface_ = nullptr;
    std::shared_ptr<DataBuffer> dataBuffer = std::make_shared<DataBuffer>(10);
    DirtyRect rect = {20, 20, 20, 20, 20};
    dataBuffer->AddDirtyRect(rect);
    int32_t ret = jpeg_->FillDirtyImages2Surface(dataBuffer, nullptr);
    EXPECT_EQ(ERR_DH_SCREEN_SURFACE_INVALIED, ret);
}

/**
 * @tc.name: FillDirtyImages2Surface_002
 * @tc.desc: Verify the FillDirtyImages2Surface function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenImageJpegTest, FillDirtyImages2Surface_002, TestSize.Level1)
{
    sptr<Surface> consumerSurface = IConsumerSurface::Create();
    sptr<IBufferProducer> prodecer = consumerSurface->GetProducer();
    jpeg_->imageSurface_ = Surface::CreateSurfaceAsProducer(prodecer);
    std::shared_ptr<DataBuffer> dataBuffer = std::make_shared<DataBuffer>(20);
    DirtyRect rect = {2600, 2800, 20, 20, 20};
    dataBuffer->AddDirtyRect(rect);
    uint8_t *lastFrame = new uint8_t[20] {0};
    int32_t ret = jpeg_->FillDirtyImages2Surface(dataBuffer, lastFrame);
    delete [] lastFrame;
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);
}

/**
 * @tc.name: ReplaceDamage2LastFrame_001
 * @tc.desc: Verify the ReplaceDamage2LastFrame function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenImageJpegTest, ReplaceDamage2LastFrame_001, TestSize.Level1)
{
    uint8_t *lastframe = new uint8_t[28385280] {125};
    uint8_t *dirtyImageData = new uint8_t[1600] {125};
    DirtyRect rect = {20, 20, 20, 24, 20};
    int32_t ret = jpeg_->ReplaceDamage2LastFrame(lastframe, dirtyImageData, rect);
    EXPECT_EQ(DH_SUCCESS, ret);
}
} // namespace DistributedHardware
} // namespace OHOS