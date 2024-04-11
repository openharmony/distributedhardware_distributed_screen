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
#include "screen_decision_center_test.h"
#include "dscreen_errcode.h"
#include "surface_type.h"
#include "iconsumer_surface.h"
#include "iimage_source_processor.h"
#include "iimage_source_processor_listener.h"
#include "jpeg_image_processor.h"
#include "screen_source_trans.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void ScreenDecisionCenterTest::SetUpTestCase(void) {}

void ScreenDecisionCenterTest::TearDownTestCase(void) {}

void ScreenDecisionCenterTest::SetUp(void)
{
    param_.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    param_.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    param_.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    param_.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    param_.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    param_.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    decision = std::make_shared<ScreenDecisionCenter>(param_);
}

void ScreenDecisionCenterTest::TearDown(void)
{
    if (decision != nullptr) {
        decision = nullptr;
    }
}

/**
 * @tc.name: IsDirtyRectValid_001
 * @tc.desc: Verify the IsDirtyRectValid function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDecisionCenterTest, IsDirtyRectValid_001, TestSize.Level1)
{
    std::vector<OHOS::Rect> damages;
    int32_t ret = decision->IsDirtyRectValid(damages);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.name: IsDirtyRectValid_002
 * @tc.desc: Verify the IsDirtyRectValid function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDecisionCenterTest, IsDirtyRectValid_002, TestSize.Level1)
{
    std::vector<OHOS::Rect> damages;
    OHOS::Rect damage = {-1, 0, 0, 0};
    damages.push_back(damage);
    int32_t ret = decision->IsDirtyRectValid(damages);
    EXPECT_EQ(false, ret);

    damages.clear();
    decision->configParam_.SetScreenWidth(1);
    damage = {2, 0, 0, 0};
    damages.push_back(damage);
    ret = decision->IsDirtyRectValid(damages);
    EXPECT_EQ(false, ret);

    damages.clear();
    decision->configParam_.SetScreenHeight(1);
    damage = {1, -1, 0, 0};
    damages.push_back(damage);
    ret = decision->IsDirtyRectValid(damages);
    EXPECT_EQ(false, ret);

    damages.clear();
    damage = {1, 2, 0, 0};
    damages.push_back(damage);
    ret = decision->IsDirtyRectValid(damages);
    EXPECT_EQ(false, ret);

    damages.clear();
    damage = {1, 0, 0, 0};
    damages.push_back(damage);
    ret = decision->IsDirtyRectValid(damages);
    EXPECT_EQ(false, ret);

    damages.clear();
    damage = {0, 1, 0, 0};
    damages.push_back(damage);
    ret = decision->IsDirtyRectValid(damages);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: IsDirtyRectValid_003
 * @tc.desc: Verify the IsDirtyRectValid function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDecisionCenterTest, IsDirtyRectValid_003, TestSize.Level1)
{
    std::vector<OHOS::Rect> damages;
    decision->configParam_.SetScreenWidth(1);
    decision->configParam_.SetScreenHeight(1);
    OHOS::Rect damage = {0, 0, -1, 0};
    damages.push_back(damage);
    int32_t ret = decision->IsDirtyRectValid(damages);
    EXPECT_EQ(false, ret);

    damages.clear();
    damage = {0, 0, 10, 0};
    damages.push_back(damage);
    ret = decision->IsDirtyRectValid(damages);
    EXPECT_EQ(false, ret);

    damages.clear();
    damage = {0, 0, 0, -1};
    damages.push_back(damage);
    ret = decision->IsDirtyRectValid(damages);
    EXPECT_EQ(false, ret);

    damages.clear();
    damage = {0, 0, 0, 10};
    damages.push_back(damage);
    ret = decision->IsDirtyRectValid(damages);
    EXPECT_EQ(false, ret);

    damages.clear();
    damage = {0, 0, 0, 0};
    damages.push_back(damage);
    ret = decision->IsDirtyRectValid(damages);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: IsDirtyRectValid_005
 * @tc.desc: Verify the IsDirtyRectValid function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDecisionCenterTest, IsDirtyRectValid_004, TestSize.Level1)
{
    std::vector<OHOS::Rect> damages;
    OHOS::Rect damage = {2, 2, 2, 2};
    damages.push_back(damage);
    damages.push_back(damage);
    int32_t ret = decision->IsDirtyRectValid(damages);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: JudgeDirtyThreshold_001
 * @tc.desc: Verify the JudgeDirtyThreshold function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDecisionCenterTest, JudgeDirtyThreshold_001, TestSize.Level1)
{
    std::vector<OHOS::Rect> damages;
    int32_t ret = decision->JudgeDirtyThreshold(damages);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: JudgeDirtyThreshold_002
 * @tc.desc: Verify the JudgeDirtyThreshold function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDecisionCenterTest, JudgeDirtyThreshold_002, TestSize.Level1)
{
    std::vector<OHOS::Rect> damages;
    OHOS::Rect damage = {2, 2, 2, 2};
    damages.push_back(damage);
    int32_t ret = decision->JudgeDirtyThreshold(damages);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: JudgeDirtyThreshold_003
 * @tc.desc: Verify the JudgeDirtyThreshold function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDecisionCenterTest, JudgeDirtyThreshold_003, TestSize.Level1)
{
    std::vector<OHOS::Rect> damages;
    OHOS::Rect damage = {0, 0, 2600, 2780};
    damages.push_back(damage);
    int32_t ret = decision->JudgeDirtyThreshold(damages);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.name: LimitTime_001
 * @tc.desc: Verify the LimitTime function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDecisionCenterTest, LimitTime_001, TestSize.Level1)
{
    decision->sendFullTime_ = time(nullptr);
    int32_t ret = decision->LimitTime(0);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: InputBufferImage_001
 * @tc.desc: Verify the InputBufferImage function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDecisionCenterTest, InputBufferImage_001, TestSize.Level1)
{
    std::vector<OHOS::Rect> damages;
    sptr<SurfaceBuffer> surfaceBuffer = nullptr;
    int32_t ret = decision->InputBufferImage(surfaceBuffer, damages);
    EXPECT_EQ(ERR_DH_SCREEN_SURFACE_BUFFER_INVALIED, ret);
}

/**
 * @tc.name: InputBufferImage_002
 * @tc.desc: Verify the InputBufferImage function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDecisionCenterTest, InputBufferImage_002, TestSize.Level1)
{
    std::vector<OHOS::Rect> damages;
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    decision->imageProcessor_ = std::make_shared<ImageSourceProcessorStub>();
    decision->imageJpeg_ = std::make_shared<JpegImageProcessorStub>(param_);
    int32_t ret = decision->InputBufferImage(buffer, damages);
    EXPECT_EQ(DH_SUCCESS, ret);

    OHOS::Rect damage = {0, 0, 0, 0};
    damages.push_back(damage);
    decision->frameCount_ = ScreenDecisionCenter::MIN_SURPPORT_FRAME_COUNT - 1;
    ret = decision->InputBufferImage(buffer, damages);
    EXPECT_EQ(DH_SUCCESS, ret);

    decision->frameCount_ = ScreenDecisionCenter::MIN_SURPPORT_FRAME_COUNT;
    decision->sendFullTime_ = time(nullptr) - ScreenDecisionCenter::FORCE_FULL_IMAGE_TIME_INTERAL - 1;
    ret = decision->InputBufferImage(buffer, damages);
    EXPECT_EQ(DH_SUCCESS, ret);

    damages.clear();
    damage = {-1, 0, 0, 0};
    damages.push_back(damage);
    decision->sendFullTime_ = time(nullptr);
    ret = decision->InputBufferImage(buffer, damages);
    EXPECT_EQ(DH_SUCCESS, ret);

    damages.clear();
    damage = {0, 0, 2600, 2780};
    damages.push_back(damage);
    decision->sendFullTime_ = time(nullptr);
    ret = decision->InputBufferImage(buffer, damages);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: ConfigureDecisionCenter_001
 * @tc.desc: Verify the ConfigureDecisionCenter function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDecisionCenterTest, ConfigureDecisionCenter_001, TestSize.Level1)
{
    std::shared_ptr<IImageSourceProcessorListener> listener = nullptr;
    std::shared_ptr<IImageSourceProcessor> imageProcessor = nullptr;
    int32_t ret = decision->ConfigureDecisionCenter(listener, imageProcessor);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, ret);
}

/**
 * @tc.name: ConfigureDecisionCenter_002
 * @tc.desc: Verify the ConfigureDecisionCenter function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDecisionCenterTest, ConfigureDecisionCenter_002, TestSize.Level1)
{
    std::shared_ptr<IImageSourceProcessorListener> listener = std::make_shared<ScreenSourceTrans>();
    std::shared_ptr<IImageSourceProcessor> imageProcessor = nullptr;
    int32_t ret = decision->ConfigureDecisionCenter(listener, imageProcessor);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, ret);
}

/**
 * @tc.name: ConfigureDecisionCenter_003
 * @tc.desc: Verify the ConfigureDecisionCenter function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDecisionCenterTest, ConfigureDecisionCenter_003, TestSize.Level1)
{
    std::shared_ptr<IImageSourceProcessorListener> listener = std::make_shared<ScreenSourceTrans>();
    std::shared_ptr<IImageSourceProcessor> imageProcessor = std::make_shared<ImageSourceProcessor>();
    int32_t ret = decision->ConfigureDecisionCenter(listener, imageProcessor);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: SetJpegSurface_001
 * @tc.desc: Verify the SetJpegSurface function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDecisionCenterTest, SetJpegSurface_001, TestSize.Level1)
{
    decision->imageJpeg_ = std::make_shared<JpegImageProcessor>(param_);
    sptr<Surface> jpegSurface;
    int32_t ret = decision->SetJpegSurface(jpegSurface);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, ret);
    jpegSurface = IConsumerSurface::Create();
    ret = decision->SetJpegSurface(jpegSurface);
    EXPECT_EQ(DH_SUCCESS, ret);
}
} // namespace DistributedHardware
} // namespace OHOS