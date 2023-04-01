/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#define private public
#include "screen_data_channel_impl_test.h"
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void ScreenDataChannelImplTest::SetUpTestCase(void) {}

void ScreenDataChannelImplTest::TearDownTestCase(void) {}

void ScreenDataChannelImplTest::SetUp(void)
{
    std::string peerDevId = "test";
    dataChannelImpl_ = std::make_shared<ScreenDataChannelImpl>(peerDevId);
}

void ScreenDataChannelImplTest::TearDown(void) {}

/**
 * @tc.name: CreateSession_001
 * @tc.desc: Verify the CreateSession function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, CreateSession_001, TestSize.Level1)
{
    std::shared_ptr<IScreenChannelListener> listener = nullptr;
    int32_t ret = dataChannelImpl_->CreateSession(listener);

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, ret);
}

/**
 * @tc.name: release_session_test_002
 * @tc.desc: Verify the ReleaseSession function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, release_session_test_002, TestSize.Level1)
{
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ILLEGAL_OPERATION, dataChannelImpl_->ReleaseSession());
}

/**
 * @tc.name: close_session_test_001
 * @tc.desc: Verify the CloseSession function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, close_session_test_001, TestSize.Level1)
{
    dataChannelImpl_->sessionId_ = 1;
    EXPECT_EQ(DH_SUCCESS, dataChannelImpl_->CloseSession());
}

/**
 * @tc.name: close_session_test_002
 * @tc.desc: Verify the CloseSession function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, close_session_test_002, TestSize.Level1)
{
    dataChannelImpl_->channelListener_ = std::make_shared<MockIScreenChannelListener>();

    StreamData ext = {0};
    StreamFrameInfo *param = nullptr;

    int32_t sessionId = 0;
    StreamData data;
    data.buf = new char[DATA_LEN];
    data.bufLen = DATA_LEN;

    dataChannelImpl_->OnStreamReceived(sessionId, &data, &ext, param);
    delete[] data.buf;
    dataChannelImpl_->sessionId_ = 0;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_SESSION_NOT_OPEN, dataChannelImpl_->CloseSession());
}

/**
 * @tc.name: send_data_test_002
 * @tc.desc: Verify the SendData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, send_data_test_002, TestSize.Level1)
{
    std::shared_ptr<IScreenChannelListener> listener = nullptr;
    dataChannelImpl_->channelListener_ = listener;
    int32_t sessionId = 0;
    dataChannelImpl_->OnSessionOpened(sessionId, 0);
    dataChannelImpl_->OnSessionClosed(sessionId);
    std::shared_ptr<DataBuffer> data = nullptr;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, dataChannelImpl_->SendData(data));
}

/**
 * @tc.name: on_session_opened_test_001
 * @tc.desc: Verify the OnSessionOpened function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, on_session_opened_test_001, TestSize.Level1)
{
    std::shared_ptr<IScreenChannelListener> listener = std::make_shared<MockIScreenChannelListener>();
    dataChannelImpl_->channelListener_ = listener;
    int32_t sessionId = 1;
    dataChannelImpl_->OnSessionOpened(sessionId, 0);
    EXPECT_EQ(sessionId, dataChannelImpl_->sessionId_);
}

/**
 * @tc.name: on_session_opened_test_002
 * @tc.desc: Verify the OnSessionOpened function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, on_session_opened_test_002, TestSize.Level1)
{
    dataChannelImpl_->channelListener_ = std::make_shared<MockIScreenChannelListener>();
    dataChannelImpl_->OnSessionOpened(-1, -1);
    EXPECT_EQ(0, dataChannelImpl_->sessionId_);
}
} // DistributedHardware
} // OHOS