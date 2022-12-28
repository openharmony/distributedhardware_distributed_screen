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
#include <memory>

#include "media_errors.h"
#include "avcodec_common.h"
#include "format.h"

#define private public
#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "screen_callback_test.h"
#include "screentrans_test_utils.h"
#include "iimage_source_processor_listener.h"
#include "image_source_encoder.h"
#include "iscreen_channel_listener.h"
#include "screen_data_channel_impl.h"
#include "image_sink_decoder.h"
#include "image_decoder_callback.h"
#undef private
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void ScreenCallbackTest::SetUpTestCase(void) {}

void ScreenCallbackTest::TearDownTestCase(void) {}

void ScreenCallbackTest::SetUp(void)
{
    sourceTrans = std::make_shared<ScreenSourceTrans>();
    sinkTrans = std::make_shared<ScreenSinkTrans>();
}

void ScreenCallbackTest::TearDown(void) {}

/**
 * @tc.name: RegisterStateCallback_003
 * @tc.desc: Verify the RegisterStateCallback function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenCallbackTest, RegisterStateCallback_003, TestSize.Level1)
{
    std::shared_ptr<IScreenSourceTransCallback> callback = std::make_shared<MockIScreenSourceTransCallback>();
    int32_t actual = sourceTrans->RegisterStateCallback(callback);

    std::string peerDevId = "peerDevId";
    std::shared_ptr<ScreenDataChannelImpl> screenChannel = std::make_shared<ScreenDataChannelImpl>(peerDevId);
    std::shared_ptr<IScreenChannelListener> listener = std::make_shared<ScreenSourceTrans>();
    screenChannel->channelListener_ = listener;
    int32_t sessionId = 0;
    int32_t result = 0;
    screenChannel->OnSessionOpened(sessionId, result);
    screenChannel->OnSessionClosed(sessionId);
    const void *data = nullptr;
    uint32_t dataLen = 0;
    screenChannel->OnBytesReceived(sessionId, data, dataLen);
    const StreamData *streamData = nullptr;
    const StreamData *ext = nullptr;
    const StreamFrameInfo *param = nullptr;
    screenChannel->OnStreamReceived(sessionId, streamData, ext, param);

    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: RegisterStateCallback_004
 * @tc.desc: Verify the RegisterStateCallback function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenCallbackTest, RegisterStateCallback_004, TestSize.Level1)
{
    std::shared_ptr<IScreenSourceTransCallback> callback = std::make_shared<MockIScreenSourceTransCallback>();
    int32_t actual = sourceTrans->RegisterStateCallback(callback);

    std::shared_ptr<IImageSourceProcessorListener> listener = std::make_shared<ScreenSourceTrans>();
    std::shared_ptr<ImageSourceEncoder> encoder = std::make_shared<ImageSourceEncoder>(listener);
    std::shared_ptr<ImageEncoderCallback> encoderCallback = std::make_shared<ImageEncoderCallback>(encoder);
    Media::AVCodecErrorType errorType = Media::AVCODEC_ERROR_INTERNAL;
    int32_t errorCode = 0;
    encoderCallback->OnError(errorType, errorCode);
    uint32_t index = 0;
    Media::AVCodecBufferInfo info;
    info.presentationTimeUs = 0;
    info.size = 0;
    info.offset = 0;
    Media::AVCodecBufferFlag flag = Media::AVCODEC_BUFFER_FLAG_NONE;
    encoderCallback->OnOutputBufferAvailable(index, info, flag);
    encoderCallback->OnInputBufferAvailable(index);
    Media::Format format;
    encoderCallback->OnOutputFormatChanged(format);

    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: RegisterStateCallback_005
 * @tc.desc: Verify the RegisterStateCallback function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenCallbackTest, RegisterStateCallback_005, TestSize.Level1)
{
    std::shared_ptr<IScreenSinkTransCallback> callback = std::make_shared<MockIScreenSinkTransCallback>();
    int32_t actual = sinkTrans->RegisterStateCallback(callback);

    std::shared_ptr<IImageSinkProcessorListener> listener = std::make_shared<ScreenSinkTrans>();
    std::shared_ptr<ImageSinkDecoder> decoder = std::make_shared<ImageSinkDecoder>(listener);
    std::shared_ptr<ImageDecoderCallback> decoderCallback = std::make_shared<ImageDecoderCallback>(decoder);

    Media::AVCodecErrorType errorType = Media::AVCODEC_ERROR_INTERNAL;
    int32_t errorCode = 0;
    decoderCallback->OnError(errorType, errorCode);
    uint32_t index = 0;
    Media::AVCodecBufferInfo info;
    info.presentationTimeUs = 0;
    info.size = 0;
    info.offset = 0;
    Media::AVCodecBufferFlag flag = Media::AVCODEC_BUFFER_FLAG_NONE;
    decoderCallback->OnOutputBufferAvailable(index, info, flag);
    decoderCallback->OnInputBufferAvailable(index);
    Media::Format format;
    decoderCallback->OnOutputFormatChanged(format);

    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: RegisterStateCallback_006
 * @tc.desc: Verify the RegisterStateCallback function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenCallbackTest, RegisterStateCallback_006, TestSize.Level1)
{
    std::shared_ptr<IScreenSinkTransCallback> callback = std::make_shared<MockIScreenSinkTransCallback>();
    int32_t actual = sinkTrans->RegisterStateCallback(callback);
    
    std::string peerDevId = "peerDevId";
    std::shared_ptr<ScreenDataChannelImpl> screenChannel = std::make_shared<ScreenDataChannelImpl>(peerDevId);
    std::shared_ptr<IScreenChannelListener> listener = std::make_shared<ScreenSinkTrans>();
    screenChannel->channelListener_ = listener;
    int32_t sessionId = 0;
    int32_t result = 0;
    screenChannel->OnSessionOpened(sessionId, result);
    screenChannel->OnSessionClosed(sessionId);
    const void *data = nullptr;
    uint32_t dataLen = 0;
    screenChannel->OnBytesReceived(sessionId, data, dataLen);
    const StreamData *streamData = nullptr;
    const StreamData *ext = nullptr;
    const StreamFrameInfo *param = nullptr;
    screenChannel->OnStreamReceived(sessionId, streamData, ext, param);

    EXPECT_EQ(DH_SUCCESS, actual);
}
} // namespace DistributedHardware
} // namespace OHOS