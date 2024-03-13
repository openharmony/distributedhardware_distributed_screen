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

#include "image_source_processor_test.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void ImageSourceProcessorTest::SetUpTestCase(void) {}

void ImageSourceProcessorTest::TearDownTestCase(void) {}

void ImageSourceProcessorTest::SetUp(void) {}

void ImageSourceProcessorTest::TearDown(void) {}

/**
 * @tc.name: ConfigureImageProcessor_001
 * @tc.desc: Verify the ConfigureImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceProcessorTest, ConfigureImageProcessor_001, TestSize.Level1)
{
    VideoParam localParam;
    VideoParam remoteParam;
    std::shared_ptr<IImageSourceProcessorListener> listener = nullptr;

    int32_t actual = processor.ConfigureImageProcessor(localParam, remoteParam, listener);

    EXPECT_EQ(ERR_DH_SCREEN_CODEC_CONFIGURE_FAILED, actual);
}

/**
 * @tc.name: ConfigureImageProcessor_002
 * @tc.desc: Verify the ConfigureImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceProcessorTest, ConfigureImageProcessor_002, TestSize.Level1)
{
    VideoParam localParam;
    VideoParam remoteParam;
    localParam.SetCodecType(VIDEO_CODEC_TYPE_VIDEO_H264);
    localParam.SetVideoWidth(DSCREEN_MAX_VIDEO_DATA_WIDTH);
    localParam.SetVideoHeight(DSCREEN_MAX_VIDEO_DATA_HEIGHT);

    std::shared_ptr<IImageSourceProcessorListener> listener = nullptr;
    int32_t actual = processor.ConfigureImageProcessor(localParam, remoteParam, listener);

    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: ReleaseImageProcessor_001
 * @tc.desc: Verify the ReleaseImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceProcessorTest, ReleaseImageProcessor_001, TestSize.Level1)
{
    int32_t actual = processor.ReleaseImageProcessor();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: ReleaseImageProcessor_002
 * @tc.desc: Verify the ReleaseImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceProcessorTest, ReleaseImageProcessor_002, TestSize.Level1)
{
    std::shared_ptr<IImageSourceProcessorListener> listener = nullptr;
    processor.imageEncoder_ = std::make_shared<ImageSourceEncoder>(listener);
    int32_t actual = processor.ReleaseImageProcessor();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: StartImageProcessor_001
 * @tc.desc: Verify the StartImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceProcessorTest, StartImageProcessor_001, TestSize.Level1)
{
    int32_t actual = processor.StartImageProcessor();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: StartImageProcessor_002
 * @tc.desc: Verify the StartImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceProcessorTest, StartImageProcessor_002, TestSize.Level1)
{
    std::shared_ptr<IImageSourceProcessorListener> listener = nullptr;
    processor.imageEncoder_ = std::make_shared<ImageSourceEncoder>(listener);
    int32_t actual = processor.StartImageProcessor();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: StopImageProcessor_001
 * @tc.desc: Verify the StopImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceProcessorTest, StopImageProcessor_001, TestSize.Level1)
{
    int32_t actual = processor.StopImageProcessor();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: StopImageProcessor_002
 * @tc.desc: Verify the StopImageProcessor function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceProcessorTest, StopImageProcessor_002, TestSize.Level1)
{
    std::shared_ptr<IImageSourceProcessorListener> listener = nullptr;
    processor.imageEncoder_ = std::make_shared<ImageSourceEncoder>(listener);
    int32_t actual = processor.StopImageProcessor();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: GetImageSurface_001
 * @tc.desc: Verify the GetImageSurface function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceProcessorTest, GetImageSurface_001, TestSize.Level1)
{
    std::shared_ptr<IImageSourceProcessorListener> listener = nullptr;
    processor.imageEncoder_ = std::make_shared<ImageSourceEncoder>(listener);
    sptr<Surface> actual = processor.GetImageSurface();

    EXPECT_EQ(nullptr, actual);
}
} // DistributedHardware
} // OHOS