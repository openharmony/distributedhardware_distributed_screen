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

#include "image_sink_decoder_test.h"

#include <chrono>
#include <securec.h>

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "iconsumer_surface.h"
#include "buffer/avsharedmemorybase.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
constexpr static uint32_t VIDEO_DATA_NUM = 480;
void ImageSinkDecoderTest::SetUpTestCase(void) {}

void ImageSinkDecoderTest::TearDownTestCase(void) {}

void ImageSinkDecoderTest::SetUp(void)
{
    param_.screenWidth_ = VIDEO_DATA_NUM;
    param_.screenHeight_ = VIDEO_DATA_NUM;
    param_.videoWidth_ = VIDEO_DATA_NUM;
    param_.videoHeight_ = VIDEO_DATA_NUM;
    param_.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    param_.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    param_.fps_ = FPS;

    imageListener_ = std::make_shared<MockIImageSinkProcessorListener>();
    imageDecoder_ = std::make_shared<ImageSinkDecoder>(imageListener_);
    videoDecoder_ = MediaAVCodec::VideoDecoderFactory::CreateByMime(
        std::string(MediaAVCodec::CodecMimeType::VIDEO_AVC));
    imageDecoder_->videoDecoder_ = videoDecoder_;
}

void ImageSinkDecoderTest::TearDown(void)
{
    if (videoDecoder_ != nullptr) {
        videoDecoder_->Stop();
        videoDecoder_->Release();
        videoDecoder_ = nullptr;
    }
}

/**
 * @tc.name: configure_decoder_test_001
 * @tc.desc: Verify the ConfigureDecoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, configure_decoder_test_001, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, imageDecoder_->ConfigureDecoder(param_));
}

/**
 * @tc.name: configure_decoder_test_002
 * @tc.desc: Verify the ConfigureDecoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, configure_decoder_test_002, TestSize.Level1)
{
    param_.codecType_ = VIDEO_CODEC_TYPE_INVALID;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, imageDecoder_->ConfigureDecoder(param_));
}

/**
 * @tc.name: configure_decoder_test_003
 * @tc.desc: Verify the ConfigureDecoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, configure_decoder_test_003, TestSize.Level1)
{
    param_.SetVideoWidth(0);
    EXPECT_EQ(ERR_DH_SCREEN_CODEC_CONFIGURE_FAILED, imageDecoder_->ConfigureDecoder(param_));
}

/**
 * @tc.name: release_decoder_test_001
 * @tc.desc: Verify the ReleaseDecoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, release_decoder_test_001, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, imageDecoder_->ReleaseDecoder());
}

/**
 * @tc.name: release_decoder_test_002
 * @tc.desc: Verify the ReleaseDecoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, release_decoder_test_002, TestSize.Level1)
{
    imageDecoder_->videoDecoder_ = nullptr;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, imageDecoder_->ReleaseDecoder());
}

/**
 * @tc.name: start_decoder_test_001
 * @tc.desc: Verify the StartDecoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, start_decoder_test_001, TestSize.Level1)
{
    EXPECT_EQ(ERR_DH_SCREEN_CODEC_START_FAILED, imageDecoder_->StartDecoder());
}

/**
 * @tc.name: start_decoder_test_002
 * @tc.desc: Verify the StartDecoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, start_decoder_test_002, TestSize.Level1)
{
    imageDecoder_->videoDecoder_ = nullptr;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, imageDecoder_->StartDecoder());
}

/**
 * @tc.name: stop_decoder_test_001
 * @tc.desc: Verify the StopDecoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, stop_decoder_test_001, TestSize.Level1)
{
    imageDecoder_->videoDecoder_ = nullptr;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, imageDecoder_->StopDecoder());
}

/**
 * @tc.name: stop_decoder_test_002
 * @tc.desc: Verify the StopDecoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, stop_decoder_test_002, TestSize.Level1)
{
    EXPECT_EQ(ERR_DH_SCREEN_CODEC_FLUSH_FAILED, imageDecoder_->StopDecoder());
}

/**
 * @tc.name: init_video_decoder_test_001
 * @tc.desc: Verify the InitVideoDecoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, init_video_decoder_test_001, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, imageDecoder_->InitVideoDecoder(param_));
}

/**
 * @tc.name: init_video_decoder_test_002
 * @tc.desc: Verify the InitVideoDecoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, init_video_decoder_test_002, TestSize.Level1)
{
    param_.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H265;
    EXPECT_EQ(DH_SUCCESS, imageDecoder_->InitVideoDecoder(param_));
}

/**
 * @tc.name: init_video_decoder_test_003
 * @tc.desc: Verify the InitVideoDecoder function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, init_video_decoder_test_003, TestSize.Level1)
{
    param_.codecType_ = VIDEO_CODEC_TYPE_INVALID;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, imageDecoder_->InitVideoDecoder(param_));
}

/**
 * @tc.name: set_decoder_format_test_001
 * @tc.desc: Verify the SetDecoderFormat function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, set_decoder_format_test_001, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, imageDecoder_->SetDecoderFormat(param_));
}

/**
 * @tc.name: set_decoder_format_test_002
 * @tc.desc: Verify the SetDecoderFormat function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, set_decoder_format_test_002, TestSize.Level1)
{
    param_.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H265;
    EXPECT_EQ(DH_SUCCESS, imageDecoder_->SetDecoderFormat(param_));
}

/**
 * @tc.name: set_decoder_format_test_003
 * @tc.desc: Verify the SetDecoderFormat function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, set_decoder_format_test_003, TestSize.Level1)
{
    imageDecoder_->videoDecoder_ = nullptr;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, imageDecoder_->SetDecoderFormat(param_));
}

/**
 * @tc.name: set_decoder_format_test_004
 * @tc.desc: Verify the SetDecoderFormat function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, set_decoder_format_test_004, TestSize.Level1)
{
    param_.codecType_ = VIDEO_CODEC_TYPE_INVALID;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ILLEGAL_OPERATION, imageDecoder_->SetDecoderFormat(param_));
}

/**
 * @tc.name: set_decoder_format_test_005
 * @tc.desc: Verify the SetDecoderFormat function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, set_decoder_format_test_005, TestSize.Level1)
{
    param_.SetCodecType(VIDEO_CODEC_TYPE_VIDEO_H264);
    EXPECT_EQ(DH_SUCCESS, imageDecoder_->SetDecoderFormat(param_));
}

/**
 * @tc.name: set_decoder_format_test_006
 * @tc.desc: Verify the SetDecoderFormat function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, set_decoder_format_test_006, TestSize.Level1)
{
    param_.SetVideoWidth(0);
    EXPECT_EQ(ERR_DH_SCREEN_CODEC_CONFIGURE_FAILED, imageDecoder_->SetDecoderFormat(param_));
}

/**
 * @tc.name: set_output_surface_test_001
 * @tc.desc: Verify the SetOutputSurface function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, set_output_surface_test_001, TestSize.Level1)
{
    MediaAVCodec::AVCodecErrorType errorType = MediaAVCodec::AVCODEC_ERROR_EXTEND_START;
    imageDecoder_->OnError(errorType, DH_SUCCESS);
    sptr<IConsumerSurface> surface = IConsumerSurface::Create("test");
    sptr<IBufferProducer> bp = surface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    imageDecoder_->videoDecoder_ = nullptr;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, imageDecoder_->SetOutputSurface(pSurface));
}

/**
 * @tc.name: InputScreenData_test_001
 * @tc.desc: Verify the InputScreenData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, InputScreenData_test_001, TestSize.Level1)
{
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, imageDecoder_->InputScreenData(nullptr));
}

/**
 * @tc.name: InputScreenData_test_002
 * @tc.desc: Verify the InputScreenData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, InputScreenData_test_002, TestSize.Level1)
{
    std::shared_ptr<DataBuffer> data = std::make_shared<DataBuffer>(10);
    for (uint32_t i = 0; i < DATA_QUEUE_MAX_SIZE + 1; i++) {
        imageDecoder_->videoDataQueue_.push(data);
    }
    EXPECT_EQ(DH_SUCCESS, imageDecoder_->InputScreenData(data));
    std::queue<std::shared_ptr<DataBuffer>>().swap(imageDecoder_->videoDataQueue_);
}

/**
 * @tc.name: on_input_buffer_available_test_001
 * @tc.desc: Verify the OnInputBufferAvailable function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, on_input_buffer_available_test_001, TestSize.Level1)
{
    MediaAVCodec::AVCodecErrorType errorType = MediaAVCodec::AVCODEC_ERROR_EXTEND_START;
    int32_t errorCode = DH_SUCCESS;
    std::shared_ptr<IImageSinkProcessorListener> listener= nullptr;
    imageDecoder_->imageProcessorListener_ = listener;
    imageDecoder_->OnError(errorType, errorCode);
    unsigned int len = 1;
    std::shared_ptr<Media::AVSharedMemory> buffer = nullptr;
    imageDecoder_->OnInputBufferAvailable(0, buffer);
    EXPECT_EQ(len, imageDecoder_->availableInputIndexsQueue_.size());
}

/**
 * @tc.name: on_output_buffer_available_test_001
 * @tc.desc: Verify the OnInputBufferAvailable function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, on_output_buffer_available_test_001, TestSize.Level1)
{
    uint32_t index = 0;
    MediaAVCodec::AVCodecBufferFlag flag = MediaAVCodec::AVCODEC_BUFFER_FLAG_CODEC_DATA;
    MediaAVCodec::AVCodecBufferInfo info;
    info.presentationTimeUs = 1;
    std::shared_ptr<Media::AVSharedMemory> buffer = nullptr;
    imageDecoder_->OnOutputBufferAvailable(index, info, flag, buffer);
    EXPECT_EQ(info.presentationTimeUs, imageDecoder_->decoderBufferInfo_.presentationTimeUs);
}

/**
 * @tc.name: on_output_buffer_available_test_002
 * @tc.desc: Verify the OnInputBufferAvailable function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, on_output_buffer_available_test_002, TestSize.Level1)
{
    uint32_t index = 0;
    MediaAVCodec::AVCodecBufferFlag flag = MediaAVCodec::AVCODEC_BUFFER_FLAG_CODEC_DATA;
    MediaAVCodec::AVCodecBufferInfo info;
    info.presentationTimeUs = 1;
    imageDecoder_->decoderBufferInfo_.presentationTimeUs = 0;
    imageDecoder_->videoDecoder_ = nullptr;
    std::shared_ptr<Media::AVSharedMemory> buffer = nullptr;
    imageDecoder_->OnOutputBufferAvailable(index, info, flag, buffer);
    EXPECT_NE(info.presentationTimeUs, imageDecoder_->decoderBufferInfo_.presentationTimeUs);
}

/**
 * @tc.name: on_output_format_changed_test_001
 * @tc.desc: Verify the OnOutputFormatChanged function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, on_output_format_changed_test_001, TestSize.Level1)
{
    Media::Format format;
    imageDecoder_->OnOutputFormatChanged(format);
    EXPECT_EQ(false, imageDecoder_->isDecoderReady_);
}

/**
 * @tc.name: ProcessData_001
 * @tc.desc: Verify the DecodeScreenData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, ProcessData_001, TestSize.Level1)
{
    std::shared_ptr<DataBuffer> screenData = std::make_shared<DataBuffer>(100);
    int32_t ret = imageDecoder_->ProcessData(screenData, 0);
    EXPECT_EQ(ERR_DH_SCREEN_CODEC_SURFACE_ERROR, ret);
}

/**
 * @tc.name: ProcessData_002
 * @tc.desc: Verify the DecodeScreenData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, ProcessData_002, TestSize.Level1)
{
    imageDecoder_->videoDecoder_ = nullptr;
    std::shared_ptr<DataBuffer> screenData = std::make_shared<DataBuffer>(100);
    int32_t ret = imageDecoder_->ProcessData(screenData, 0);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, ret);
}

/**
 * @tc.name: ProcessData_003
 * @tc.desc: Verify the DecodeScreenData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, ProcessData_003, TestSize.Level1)
{
    std::shared_ptr<DataBuffer> screenData = nullptr;
    int32_t ret = imageDecoder_->ProcessData(screenData, 0);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, ret);
}

/**
 * @tc.name: ProcessData_004
 * @tc.desc: Verify the DecodeScreenData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, ProcessData_004, TestSize.Level1)
{
    std::shared_ptr<Media::AVSharedMemory> buffer = nullptr;
    imageDecoder_->availableInputBufferQueue_.push(buffer);
    std::shared_ptr<DataBuffer> screenData = std::make_shared<DataBuffer>(100);
    int32_t ret = imageDecoder_->ProcessData(screenData, 0);
    EXPECT_EQ(ERR_DH_SCREEN_CODEC_SURFACE_ERROR, ret);
}

/**
 * @tc.name: ProcessData_005
 * @tc.desc: Verify the DecodeScreenData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, ProcessData_005, TestSize.Level1)
{
    std::shared_ptr<Media::AVSharedMemory> buffer = Media::AVSharedMemoryBase::CreateFromLocal(100,
        Media::AVSharedMemory::FLAGS_READ_WRITE, "userBuffer");;
    imageDecoder_->availableInputBufferQueue_.push(buffer);
    std::shared_ptr<DataBuffer> screenData = std::make_shared<DataBuffer>(100);
    int32_t ret = imageDecoder_->ProcessData(screenData, 0);
    EXPECT_EQ(ERR_DH_SCREEN_CODEC_SURFACE_ERROR, ret);
}

/**
 * @tc.name: ProcessData_006
 * @tc.desc: Verify the DecodeScreenData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, ProcessData_006, TestSize.Level1)
{
    std::shared_ptr<Media::AVSharedMemory> buffer = Media::AVSharedMemoryBase::CreateFromLocal(100,
        Media::AVSharedMemory::FLAGS_READ_WRITE, "userBuffer");;
    imageDecoder_->availableInputBufferQueue_.push(buffer);
    std::shared_ptr<DataBuffer> screenData = std::make_shared<DataBuffer>(110);
    int32_t ret = imageDecoder_->ProcessData(screenData, 0);
    EXPECT_EQ(ERR_DH_SCREEN_CODEC_SURFACE_ERROR, ret);
}

/**
 * @tc.name: start_input_thread_test_001
 * @tc.desc: Verify the StartInputThread  StopInputThread function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, start_input_thread_test_001, TestSize.Level1)
{
    imageDecoder_->videoDecoder_ = nullptr;
    imageDecoder_->StartInputThread();
    EXPECT_EQ(true, imageDecoder_->isDecoderReady_);
    imageDecoder_->StopInputThread();
    EXPECT_EQ(false, imageDecoder_->isDecoderReady_);
}

/**
 * @tc.name: process_data_test_001
 * @tc.desc: Verify the StartInputThread  StopInputThread function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ImageSinkDecoderTest, process_data_test_001, TestSize.Level1)
{
    std::shared_ptr<DataBuffer> data = std::make_shared<DataBuffer>(DATA_LEN);
    imageDecoder_->videoDataQueue_.push(data);

    uint32_t index = 0;
    imageDecoder_->availableInputIndexsQueue_.push(index);

    imageDecoder_->StartInputThread();
    EXPECT_EQ(true, imageDecoder_->isDecoderReady_);

    imageDecoder_->StopInputThread();
    EXPECT_EQ(false, imageDecoder_->isDecoderReady_);
}
}
}