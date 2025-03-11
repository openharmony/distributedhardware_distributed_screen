/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "2.0/include/av_receiver_engine_adapter_test.h"

#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "dscreen_util.h"
#include "engine_test_utils.h"
#include "2.0/include/dscreen.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void AVReceiverEngineAdapterTest::SetUpTestCase(void) {}

void AVReceiverEngineAdapterTest::TearDownTestCase(void) {}

void AVReceiverEngineAdapterTest::SetUp(void)
{
    receiverAdapter_ = std::make_shared<AVTransReceiverAdapter>();
}

void AVReceiverEngineAdapterTest::TearDown(void)
{
    receiverAdapter_ = nullptr;
}

/**
 * @tc.name: Initialize_001
 * @tc.desc: Verify the Initialize function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVReceiverEngineAdapterTest, Initialize_001, TestSize.Level1)
{
    IAVEngineProvider *providerPtr = nullptr;
    std::string peerDevId = "peerDevId";
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, receiverAdapter_->Initialize(providerPtr, peerDevId));
    receiverAdapter_->initialized_ = true;
    EXPECT_EQ(DH_SUCCESS, receiverAdapter_->Initialize(providerPtr, peerDevId));
}

/**
 * @tc.name: Initialize_002
 * @tc.desc: Verify the Initialize function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVReceiverEngineAdapterTest, Initialize_002, TestSize.Level1)
{
    std::string peerDevId = "peerDevId";
    std::shared_ptr<MockAVReceiverEngine> mockInstance = std::make_shared<MockAVReceiverEngine>();
    receiverAdapter_->receiverEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, Start()).Times(1).WillOnce(testing::Return(0));
    EXPECT_CALL(*mockInstance, Stop()).Times(1).WillOnce(testing::Return(0));
    EXPECT_CALL(*mockInstance, Release()).Times(1).WillOnce(testing::Return(0));
    EXPECT_EQ(DH_SUCCESS, receiverAdapter_->Start());
    EXPECT_EQ(DH_SUCCESS, receiverAdapter_->Stop());
    EXPECT_EQ(DH_SUCCESS, receiverAdapter_->Release());
}

/**
 * @tc.name: Release_001
 * @tc.desc: Verify the Release function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVReceiverEngineAdapterTest, Release_001, TestSize.Level1)
{
    std::string peerDevId = "peerDevId";
    std::shared_ptr<MockAVReceiverEngine> mockInstance = std::make_shared<MockAVReceiverEngine>();
    receiverAdapter_->receiverEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, Release()).Times(1).WillOnce(testing::Return(0));
    int32_t ret = receiverAdapter_->Release();
    EXPECT_EQ(receiverAdapter_->receiverEngine_, nullptr);
    EXPECT_EQ(ret, DH_SUCCESS);
}

/**
 * @tc.name: Start_001
 * @tc.desc: Verify the Start and Stop function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVReceiverEngineAdapterTest, Start_001, TestSize.Level1)
{
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, receiverAdapter_->Start());
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, receiverAdapter_->Stop());
}

/**
 * @tc.name: Start_002
 * @tc.desc: Verify the Start and Stop function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVReceiverEngineAdapterTest, Start_002, TestSize.Level1)
{
    std::shared_ptr<MockAVReceiverEngine> mockInstance = std::make_shared<MockAVReceiverEngine>();
    receiverAdapter_->receiverEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, Start()).Times(1).WillOnce(testing::Return(0));
    EXPECT_CALL(*mockInstance, Stop()).Times(1).WillOnce(testing::Return(0));
    EXPECT_EQ(DH_SUCCESS, receiverAdapter_->Start());
    EXPECT_EQ(DH_SUCCESS, receiverAdapter_->Stop());
}

/**
 * @tc.name: Start_003
 * @tc.desc: Verify the Start and Stop function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVReceiverEngineAdapterTest, Start_003, TestSize.Level1)
{
    std::shared_ptr<MockAVReceiverEngine> mockInstance = std::make_shared<MockAVReceiverEngine>();
    receiverAdapter_->receiverEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, Start()).Times(1).WillOnce(testing::Return(0));
    EXPECT_CALL(*mockInstance, Stop()).Times(1).WillOnce(testing::Return(-1));
    EXPECT_EQ(DH_SUCCESS, receiverAdapter_->Start());
    EXPECT_EQ(ERR_DH_AV_TRANS_STOP_FAILED, receiverAdapter_->Stop());
}

/**
 * @tc.name: SetParameter_001
 * @tc.desc: Verify the Start and SetParameter function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVReceiverEngineAdapterTest, SetParameter_001, TestSize.Level1)
{
    std::string param = "param";
    receiverAdapter_->receiverEngine_ = nullptr;
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, receiverAdapter_->SetParameter(AVTransTag::VIDEO_FRAME_RATE, param));
}

/**
 * @tc.name: SetParameter_002
 * @tc.desc: Verify the Start and SetParameter function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVReceiverEngineAdapterTest, SetParameter_002, TestSize.Level1)
{
    std::string param = "param";
    std::shared_ptr<MockAVReceiverEngine> mockInstance = std::make_shared<MockAVReceiverEngine>();
    receiverAdapter_->receiverEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, SetParameter(_, _)).Times(1).WillOnce(testing::Return(0));
    EXPECT_EQ(DH_SUCCESS, receiverAdapter_->SetParameter(AVTransTag::VIDEO_FRAME_RATE, param));
}

/**
 * @tc.name: SendMessageToRemote_001
 * @tc.desc: Verify the Start and SendMessageToRemote function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVReceiverEngineAdapterTest, SendMessageToRemote_001, TestSize.Level1)
{
    uint32_t type = 0;
    std::string content = "content";
    std::string dstDevId = "dstDevId";
    auto message = std::make_shared<AVTransMessage>(type, content, dstDevId);
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, receiverAdapter_->SendMessageToRemote(message));
    std::shared_ptr<MockAVReceiverEngine> mockInstance = std::make_shared<MockAVReceiverEngine>();
    receiverAdapter_->receiverEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, SendMessage(_)).Times(1).WillOnce(testing::Return(0));
    EXPECT_EQ(DH_SUCCESS, receiverAdapter_->SendMessageToRemote(message));
}

/**
 * @tc.name: RegisterAdapterCallback_001
 * @tc.desc: Verify the Start and RegisterAdapterCallback function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVReceiverEngineAdapterTest, RegisterAdapterCallback_001, TestSize.Level1)
{
    std::shared_ptr<AVReceiverAdapterCallback> callback = nullptr;
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, receiverAdapter_->RegisterAdapterCallback(callback));
    callback = std::make_shared<MockAVReceiverAdapterCallback>();
    EXPECT_EQ(DH_SUCCESS, receiverAdapter_->RegisterAdapterCallback(callback));
}

/**
 * @tc.name: OnReceiverEvent_001
 * @tc.desc: Verify the Start and OnReceiverEvent function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVReceiverEngineAdapterTest, OnReceiverEvent_001, TestSize.Level1)
{
    AVTransEvent event;
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, receiverAdapter_->OnReceiverEvent(event));
    std::shared_ptr<AVTransMessage> message = nullptr;
    EXPECT_EQ(DH_SUCCESS, receiverAdapter_->OnMessageReceived(message));
    std::shared_ptr<AVTransBuffer> buffer = nullptr;
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, receiverAdapter_->OnDataAvailable(buffer));
    receiverAdapter_->adapterCallback_ = std::make_shared<MockAVReceiverAdapterCallback>();
    std::shared_ptr<MockAVReceiverEngine> mockInstance = std::make_shared<MockAVReceiverEngine>();
    receiverAdapter_->receiverEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, Release()).Times(1).WillOnce(testing::Return(0));
    EXPECT_EQ(DH_SUCCESS, receiverAdapter_->OnReceiverEvent(event));
    EXPECT_EQ(DH_SUCCESS, receiverAdapter_->OnMessageReceived(message));
    EXPECT_EQ(DH_SUCCESS, receiverAdapter_->OnDataAvailable(buffer));
    EXPECT_EQ(DH_SUCCESS, receiverAdapter_->Release());
}

/**
 * @tc.name: OnReceiverEvent_002
 * @tc.desc: Verify the Start and OnReceiverEvent function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVReceiverEngineAdapterTest, OnReceiverEvent_002, TestSize.Level1)
{
    AVTransEvent event;
    event.type = EventType::EVENT_START_SUCCESS;
    receiverAdapter_->adapterCallback_ = std::make_shared<MockAVReceiverAdapterCallback>();
    EXPECT_EQ(DH_AVT_SUCCESS, receiverAdapter_->OnReceiverEvent(event));

    event.type = EventType::EVENT_CHANNEL_CLOSED;
    EXPECT_EQ(DH_AVT_SUCCESS, receiverAdapter_->OnReceiverEvent(event));

    event.type = EventType::EVENT_ENGINE_ERROR;
    EXPECT_EQ(DH_AVT_SUCCESS, receiverAdapter_->OnReceiverEvent(event));

    event.type = EventType::EVENT_REMOTE_ERROR;
    EXPECT_EQ(DH_AVT_SUCCESS, receiverAdapter_->OnReceiverEvent(event));

    event.type = EventType::EVENT_CHANNEL_OPENED;
    EXPECT_EQ(DH_AVT_SUCCESS, receiverAdapter_->OnReceiverEvent(event));
}
} // namespace DistributedHardware
} // namespace OHOS
