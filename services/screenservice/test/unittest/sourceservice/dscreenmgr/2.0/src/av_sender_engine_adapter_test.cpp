/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "2.0/include/av_sender_engine_adapter_test.h"

#include "dscreen_errcode.h"
#include "histreamer_ability_parser.h"
#include "2.0/include/dscreen.h"
#include "dscreen_log.h"
#include "dscreen_util.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void AVSenderEngineAdapterTest::SetUpTestCase(void) {}

void AVSenderEngineAdapterTest::TearDownTestCase(void) {}

void AVSenderEngineAdapterTest::SetUp(void)
{
    senderAdapter_ = std::make_shared<AVTransSenderAdapter>();
}

void AVSenderEngineAdapterTest::TearDown(void)
{
    senderAdapter_ = nullptr;
}

/**
 * @tc.name: Initialize_001
 * @tc.desc: Verify the Initialize function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, Initialize_001, TestSize.Level1)
{
    IAVEngineProvider *providerPtr = nullptr;
    std::string peerDevId = "peerDevId";
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, senderAdapter_->Initialize(providerPtr, peerDevId));
    senderAdapter_->initialized_ = true;
    EXPECT_EQ(DH_SUCCESS, senderAdapter_->Initialize(providerPtr, peerDevId));
}

/**
 * @tc.name: Release_001
 * @tc.desc: Verify the Release function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, Release_001, TestSize.Level1)
{
    std::shared_ptr<MockAVSenderEngine> mockInstance = std::make_shared<MockAVSenderEngine>();
    senderAdapter_->senderEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, Release()).Times(1).WillOnce(testing::Return(0));
    senderAdapter_->Release();
    EXPECT_EQ(nullptr, senderAdapter_->senderEngine_);
}

/**
 * @tc.name: Release_002
 * @tc.desc: Verify the Release function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, Release_002, TestSize.Level1)
{
    std::shared_ptr<MockAVSenderEngine> mockInstance = std::make_shared<MockAVSenderEngine>();
    senderAdapter_->senderEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, Release()).Times(1).WillOnce(testing::Return(-1));
    senderAdapter_->Release();
    EXPECT_EQ(nullptr, senderAdapter_->senderEngine_);
}

/**
 * @tc.name: Start_001
 * @tc.desc: Verify the Start and Stop function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, Start_001, TestSize.Level1)
{
    senderAdapter_->transStartSuccess_ = false;
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, senderAdapter_->Start());
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, senderAdapter_->Stop());
}

/**
 * @tc.name: Start_002
 * @tc.desc: Verify the Start and Stop function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, Start_002, TestSize.Level1)
{
    senderAdapter_->transStartSuccess_ = true;
    std::shared_ptr<MockAVSenderEngine> mockInstance = std::make_shared<MockAVSenderEngine>();
    senderAdapter_->senderEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, Stop()).Times(1).WillOnce(testing::Return(0));
    EXPECT_EQ(DH_SUCCESS, senderAdapter_->Start());
    EXPECT_EQ(DH_SUCCESS, senderAdapter_->Stop());
}

/**
 * @tc.name: Start_003
 * @tc.desc: Verify the Start and Stop function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, Start_003, TestSize.Level1)
{
    senderAdapter_->transStartSuccess_ = false;
    std::shared_ptr<MockAVSenderEngine> mockInstance = std::make_shared<MockAVSenderEngine>();
    senderAdapter_->senderEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, Start()).Times(1).WillOnce(testing::Return(0));
    EXPECT_CALL(*mockInstance, Stop()).Times(1).WillOnce(testing::Return(-1));
    EXPECT_EQ(DH_SUCCESS, senderAdapter_->Start());
    EXPECT_EQ(ERR_DH_AV_TRANS_STOP_FAILED, senderAdapter_->Stop());
}

/**
 * @tc.name: Start_004
 * @tc.desc: Verify the Start and Stop function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, Start_004, TestSize.Level1)
{
    senderAdapter_->transStartSuccess_ = false;
    std::shared_ptr<MockAVSenderEngine> mockInstance = std::make_shared<MockAVSenderEngine>();
    senderAdapter_->senderEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, Start()).Times(1).WillOnce(testing::Return(-1));
    EXPECT_EQ(ERR_DH_AV_TRANS_START_FAILED, senderAdapter_->Start());
}

/**
 * @tc.name: SetParameter_001
 * @tc.desc: Verify the SetParameter function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, SetParameter_001, TestSize.Level1)
{
    std::string param = "param";
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, senderAdapter_->SetParameter(AVTransTag::VIDEO_FRAME_RATE, param));
}

/**
 * @tc.name: SetParameter_002
 * @tc.desc: Verify the SetParameter function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, SetParameter_002, TestSize.Level1)
{
    std::string param = "param";
    std::shared_ptr<MockAVSenderEngine> mockInstance = std::make_shared<MockAVSenderEngine>();
    senderAdapter_->senderEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, SetParameter(testing::_, testing::_)).Times(1).WillOnce(testing::Return(0));
    EXPECT_EQ(DH_SUCCESS, senderAdapter_->SetParameter(AVTransTag::VIDEO_FRAME_RATE, param));
}

/**
 * @tc.name: SetParameter_003
 * @tc.desc: Verify the SetParameter function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, SetParameter_003, TestSize.Level1)
{
    std::string param = "param";
    std::shared_ptr<MockAVSenderEngine> mockInstance = std::make_shared<MockAVSenderEngine>();
    senderAdapter_->senderEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, SetParameter(testing::_, testing::_)).Times(1).WillOnce(testing::Return(-1));
    EXPECT_EQ(ERR_DH_AV_TRANS_SETUP_FAILED, senderAdapter_->SetParameter(AVTransTag::VIDEO_FRAME_RATE, param));
}

/**
 * @tc.name: PushData_001
 * @tc.desc: Verify the PushData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, PushData_001, TestSize.Level1)
{
    struct VideoData videoData;
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, senderAdapter_->PushData(videoData));
    std::shared_ptr<MockAVSenderEngine> mockInstance = std::make_shared<MockAVSenderEngine>();
    senderAdapter_->senderEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, PushData(testing::_)).Times(1).WillOnce(testing::Return(0));
    EXPECT_EQ(DH_SUCCESS, senderAdapter_->PushData(videoData));
}

/**
 * @tc.name: PushData_002
 * @tc.desc: Verify the PushData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, PushData_002, TestSize.Level1)
{
    struct VideoData videoData;
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, senderAdapter_->PushData(videoData));
    std::shared_ptr<MockAVSenderEngine> mockInstance = std::make_shared<MockAVSenderEngine>();
    senderAdapter_->senderEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, PushData(testing::_)).Times(1).WillOnce(testing::Return(-1));
    EXPECT_EQ(ERR_DH_AV_TRANS_FEED_DATA_FAILED, senderAdapter_->PushData(videoData));
}

/**
 * @tc.name: CreateControlChannel_001
 * @tc.desc: Verify the CreateControlChannel function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, CreateControlChannel_001, TestSize.Level1)
{
    std::string peerDevId = "peerDevId";
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, senderAdapter_->CreateControlChannel(peerDevId));
    senderAdapter_->chnCreateSuccess_ = true;
    EXPECT_EQ(DH_SUCCESS, senderAdapter_->CreateControlChannel(peerDevId));
}

/**
 * @tc.name: CreateControlChannel_002
 * @tc.desc: Verify the CreateControlChannel function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, CreateControlChannel_002, TestSize.Level1)
{
    std::string peerDevId = "peerDevId";
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, senderAdapter_->CreateControlChannel(peerDevId));
    std::shared_ptr<MockAVSenderEngine> mockInstance = std::make_shared<MockAVSenderEngine>();
    senderAdapter_->senderEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, CreateControlChannel(testing::_, testing::_)).Times(1).WillOnce(testing::Return(0));
    EXPECT_EQ(ERR_DH_AV_TRANS_CREATE_CHANNEL_FAILED, senderAdapter_->CreateControlChannel(peerDevId));
}

/**
 * @tc.name: CreateControlChannel_003
 * @tc.desc: Verify the CreateControlChannel function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, CreateControlChannel_003, TestSize.Level1)
{
    std::string peerDevId = "peerDevId";
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, senderAdapter_->CreateControlChannel(peerDevId));
    std::shared_ptr<MockAVSenderEngine> mockInstance = std::make_shared<MockAVSenderEngine>();
    senderAdapter_->senderEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, CreateControlChannel(testing::_, testing::_)).Times(1).WillOnce(testing::Return(-1));
    EXPECT_EQ(ERR_DH_AV_TRANS_CREATE_CHANNEL_FAILED, senderAdapter_->CreateControlChannel(peerDevId));
}

/**
 * @tc.name: SendMessageToRemote_001
 * @tc.desc: Verify the SendMessageToRemote function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, SendMessageToRemote_001, TestSize.Level1)
{
    uint32_t type = 0;
    std::string content = "content";
    std::string dstDevId = "dstDevId";
    auto message = std::make_shared<AVTransMessage>(type, content, dstDevId);
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, senderAdapter_->SendMessageToRemote(message));
    std::shared_ptr<MockAVSenderEngine> mockInstance = std::make_shared<MockAVSenderEngine>();
    senderAdapter_->senderEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, SendMessage(testing::_)).Times(1).WillOnce(testing::Return(0));
    EXPECT_EQ(DH_SUCCESS, senderAdapter_->SendMessageToRemote(message));
}

/**
 * @tc.name: SendMessageToRemote_002
 * @tc.desc: Verify the SendMessageToRemote function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, SendMessageToRemote_002, TestSize.Level1)
{
    uint32_t type = 0;
    std::string content = "content";
    std::string dstDevId = "dstDevId";
    auto message = std::make_shared<AVTransMessage>(type, content, dstDevId);
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, senderAdapter_->SendMessageToRemote(message));
    std::shared_ptr<MockAVSenderEngine> mockInstance = std::make_shared<MockAVSenderEngine>();
    senderAdapter_->senderEngine_ = mockInstance;
    EXPECT_CALL(*mockInstance, SendMessage(testing::_)).Times(1).WillOnce(testing::Return(-1));
    EXPECT_EQ(ERR_DH_AV_TRANS_SEND_MSG_FAILED, senderAdapter_->SendMessageToRemote(message));
}

/**
 * @tc.name: RegisterAdapterCallback_001
 * @tc.desc: Verify the RegisterAdapterCallback function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, RegisterAdapterCallback_001, TestSize.Level1)
{
    std::shared_ptr<AVSenderAdapterCallback> callback = nullptr;
    EXPECT_EQ(ERR_DH_AV_TRANS_NULL_VALUE, senderAdapter_->RegisterAdapterCallback(callback));
    callback = std::make_shared<MockAVSenderAdapterCallback>();
    EXPECT_EQ(DH_SUCCESS, senderAdapter_->RegisterAdapterCallback(callback));
}

/**
 * @tc.name: OnSenderEvent_001
 * @tc.desc: Verify the OnSenderEvent function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, OnSenderEvent_001, TestSize.Level1)
{
    AVTransEvent event;
    EXPECT_EQ(DH_AVT_SUCCESS, senderAdapter_->OnSenderEvent(event));
    std::shared_ptr<AVTransMessage> message = nullptr;
    EXPECT_EQ(DH_AVT_SUCCESS, senderAdapter_->OnMessageReceived(message));
    senderAdapter_->adapterCallback_ = std::make_shared<MockAVSenderAdapterCallback>();
    EXPECT_EQ(DH_AVT_SUCCESS, senderAdapter_->OnSenderEvent(event));
    EXPECT_EQ(DH_AVT_SUCCESS, senderAdapter_->OnMessageReceived(message));
}

/**
 * @tc.name: OnSenderEvent_002
 * @tc.desc: Verify the OnSenderEvent function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(AVSenderEngineAdapterTest, OnSenderEvent_002, TestSize.Level1)
{
    AVTransEvent event;
    event.type = EventType::EVENT_CHANNEL_CLOSED;
    senderAdapter_->adapterCallback_ = std::make_shared<MockAVSenderAdapterCallback>();
    EXPECT_EQ(DH_AVT_SUCCESS, senderAdapter_->OnSenderEvent(event));

    event.type = EventType::EVENT_START_SUCCESS;
    EXPECT_EQ(DH_AVT_SUCCESS, senderAdapter_->OnSenderEvent(event));

    event.type = EventType::EVENT_REMOTE_ERROR;
    EXPECT_EQ(DH_AVT_SUCCESS, senderAdapter_->OnSenderEvent(event));

    event.type = EventType::EVENT_REMOVE_STREAM;
    EXPECT_EQ(DH_AVT_SUCCESS, senderAdapter_->OnSenderEvent(event));

    senderAdapter_->adapterCallback_ = nullptr;
    event.type = EventType::EVENT_CHANNEL_CLOSED;
    EXPECT_EQ(DH_AVT_SUCCESS, senderAdapter_->OnSenderEvent(event));

    event.type = EventType::EVENT_START_SUCCESS;
    EXPECT_EQ(DH_AVT_SUCCESS, senderAdapter_->OnSenderEvent(event));

    event.type = EventType::EVENT_REMOTE_ERROR;
    EXPECT_EQ(DH_AVT_SUCCESS, senderAdapter_->OnSenderEvent(event));
}
} // namespace DistributedHardware
} // namespace OHOS
