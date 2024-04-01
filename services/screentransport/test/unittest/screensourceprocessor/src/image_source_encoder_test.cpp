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

#include "image_source_encoder_test.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void ImageSourceEncoderTest::SetUpTestCase(void) {}

void ImageSourceEncoderTest::TearDownTestCase(void) {}

void ImageSourceEncoderTest::SetUp(void)
{
    std::shared_ptr<IImageSourceProcessorListener> imageListener = nullptr;
    encoder = std::make_shared<ImageSourceEncoder>(imageListener);
}

void ImageSourceEncoderTest::TearDown(void)
{
    if (encoder != nullptr) {
        encoder->StopEncoder();
        encoder->ReleaseEncoder();
    }
}

/**
 * @tc.name: ConfigureEncoder_001
 * @tc.desc: Verify the ConfigureEncoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, ConfigureEncoder_001, TestSize.Level1)
{
    VideoParam configParam;
    configParam.SetCodecType(2);
    int32_t actual = encoder->ConfigureEncoder(configParam);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_CREATE_CODEC_FAILED, actual);
}

/**
 * @tc.name: ConfigureEncoder_002
 * @tc.desc: Verify the ConfigureEncoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, ConfigureEncoder_002, TestSize.Level1)
{
    VideoParam configParam;
    configParam.SetCodecType(VIDEO_CODEC_TYPE_VIDEO_H264);
    int32_t actual = encoder->ConfigureEncoder(configParam);
    EXPECT_EQ(ERR_DH_SCREEN_CODEC_CONFIGURE_FAILED, actual);
}

/**
 * @tc.name: ConfigureEncoder_003
 * @tc.desc: Verify the ConfigureEncoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */

HWTEST_F(ImageSourceEncoderTest, ConfigureEncoder_004, TestSize.Level1)
{
    VideoParam configParam;
    configParam.SetCodecType(VIDEO_CODEC_TYPE_VIDEO_H264);
    configParam.SetVideoWidth(DSCREEN_MAX_VIDEO_DATA_WIDTH);
    configParam.SetVideoHeight(DSCREEN_MAX_VIDEO_DATA_HEIGHT);

    int32_t actual = encoder->ConfigureEncoder(configParam);
    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: ReleaseEncoder_001
 * @tc.desc: Verify the ReleaseEncoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, ReleaseEncoder_001, TestSize.Level1)
{
    int32_t actual = encoder->ReleaseEncoder();
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: ReleaseEncoder_002
 * @tc.desc: Verify the ReleaseEncoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, ReleaseEncoder_002, TestSize.Level1)
{
    encoder->videoEncoder_ = MediaAVCodec::VideoEncoderFactory::CreateByMime(
        std::string(MediaAVCodec::CodecMimeType::VIDEO_AVC));

    int32_t actual = encoder->ReleaseEncoder();
    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: StartEncoder_001
 * @tc.desc: Verify the StartEncoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, StartEncoder_001, TestSize.Level1)
{
    int32_t actual = encoder->StartEncoder();
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: StartEncoder_002
 * @tc.desc: Verify the StartEncoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, StartEncoder_002, TestSize.Level1)
{
    encoder->videoEncoder_ = MediaAVCodec::VideoEncoderFactory::CreateByMime(
        std::string(MediaAVCodec::CodecMimeType::VIDEO_AVC));
    int32_t actual = encoder->StartEncoder();
    EXPECT_EQ(ERR_DH_SCREEN_CODEC_START_FAILED, actual);
}

/**
 * @tc.name: StopEncoder_001
 * @tc.desc: Verify the StopEncoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, StopEncoder_001, TestSize.Level1)
{
    int32_t actual = encoder->StopEncoder();
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: StopEncoder_002
 * @tc.desc: Verify the StartEncoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, StopEncoder_002, TestSize.Level1)
{
    encoder->videoEncoder_ = MediaAVCodec::VideoEncoderFactory::CreateByMime(
        std::string(MediaAVCodec::CodecMimeType::VIDEO_AVC));

    int32_t actual = encoder->StopEncoder();
    EXPECT_EQ(ERR_DH_SCREEN_CODEC_STOP_FAILED, actual);
}

/**
 * @tc.name: InitVideoEncoder_001
 * @tc.desc: Verify the InitVideoEncoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, InitVideoEncoder_001, TestSize.Level1)
{
    VideoParam configParam;
    configParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;

    int32_t actual = encoder->InitVideoEncoder(configParam);
    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: InitVideoEncoder_002
 * @tc.desc: Verify the InitVideoEncoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, InitVideoEncoder_002, TestSize.Level1)
{
    VideoParam configParam;
    configParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_MPEG4;

    int32_t actual = encoder->InitVideoEncoder(configParam);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_CREATE_CODEC_FAILED, actual);
}

/**
 * @tc.name: InitVideoEncoder_003
 * @tc.desc: Verify the InitVideoEncoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, InitVideoEncoder_003, TestSize.Level1)
{
    VideoParam configParam;
    configParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H265;

    int32_t actual = encoder->InitVideoEncoder(configParam);
    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: InitVideoEncoder_004
 * @tc.desc: Verify the InitVideoEncoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, InitVideoEncoder_004, TestSize.Level1)
{
    VideoParam configParam;
    configParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;

    int32_t actual = encoder->InitVideoEncoder(configParam);
    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: SetEncoderFormat_001
 * @tc.desc: Verify the SetEncoderFormat function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, SetEncoderFormat_001, TestSize.Level1)
{
    VideoParam configParam;
    int32_t actual = encoder->SetEncoderFormat(configParam);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: SetEncoderFormat_002
 * @tc.desc: Verify the SetEncoderFormat function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, SetEncoderFormat_002, TestSize.Level1)
{
    VideoParam configParam;
    configParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    int32_t actual = encoder->InitVideoEncoder(configParam);
    configParam.codecType_ = -1;
    actual = encoder->SetEncoderFormat(configParam);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM, actual);
}

/**
 * @tc.name: SetEncoderFormat_003
 * @tc.desc: Verify the SetEncoderFormat function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, SetEncoderFormat_003, TestSize.Level1)
{
    VideoParam configParam;
    configParam.SetCodecType(VIDEO_CODEC_TYPE_VIDEO_H264);
    configParam.SetVideoWidth(DSCREEN_MAX_VIDEO_DATA_WIDTH);
    configParam.SetVideoHeight(DSCREEN_MAX_VIDEO_DATA_HEIGHT);
    int32_t actual = encoder->InitVideoEncoder(configParam);
    actual = encoder->SetEncoderFormat(configParam);

    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: SetEncoderFormat_004
 * @tc.desc: Verify the SetEncoderFormat function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, SetEncoderFormat_004, TestSize.Level1)
{
    VideoParam configParam;
    configParam.SetCodecType(VIDEO_CODEC_TYPE_VIDEO_H265);
    configParam.SetVideoWidth(DSCREEN_MAX_VIDEO_DATA_WIDTH);
    configParam.SetVideoHeight(DSCREEN_MAX_VIDEO_DATA_HEIGHT);
    int32_t actual = encoder->InitVideoEncoder(configParam);
    actual = encoder->SetEncoderFormat(configParam);

    EXPECT_EQ(DH_SUCCESS, actual);
}


/**
 * @tc.name: SetEncoderFormat_007
 * @tc.desc: Verify the SetEncoderFormat function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSourceEncoderTest, SetEncoderFormat_007, TestSize.Level1)
{
    VideoParam configParam;
    configParam.SetCodecType(VIDEO_CODEC_TYPE_VIDEO_H265);
    configParam.SetVideoWidth(DSCREEN_MAX_VIDEO_DATA_WIDTH);
    configParam.SetVideoHeight(DSCREEN_MAX_VIDEO_DATA_HEIGHT);
    int32_t actual = encoder->InitVideoEncoder(configParam);
    configParam.SetCodecType(VIDEO_CODEC_TYPE_VIDEO_MPEG4);
    actual = encoder->SetEncoderFormat(configParam);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM, actual);
}

} // DistributedHardware
} // OHOS