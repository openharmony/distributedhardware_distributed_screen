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

#include "screen_data_channel_impl_test.h"
#include "dscreen_json_util.h"
#include "dscreen_util.h"
#include "screen_source_trans.h"
#include "softbus_adapter.h"
#include "token_setproc.h"

static int32_t g_mockCreateSoftbusSessionServerReturnInt32Value = -1;
static int32_t g_mockOpenSoftbusSessionReturnInt32Value = -1;
static int32_t g_mockRegisterSoftbusListenerReturnInt32Value = -1;
static int32_t g_mockRemoveSoftbusSessionServerReturnInt32Value = -1;
static int32_t g_mockUnRegisterSoftbusListenerReturnInt32Value = -1;
static int32_t g_mockCloseSoftbusSessionReturnInt32Value = -1;

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
namespace {
    const std::string PEER_SESSION_NAME = "ohos.dhardware.dscreen.session8647073e02e7a78f09473aa123";
    const std::string REMOTE_DEV_ID = "f6d4c0864707aefte7a78f09473aa122ff57fc81c00981fcf5be989e7d112123";
    const std::string DSCREEN_PKG_NAME_TEST = "ohos.dhardware.dscreen";
}
int32_t SoftbusAdapter::CreateSoftbusSessionServer(const std::string &pkgname, const std::string &sessionName,
    const std::string &peerDevId)
{
    return g_mockCreateSoftbusSessionServerReturnInt32Value;
}

int32_t SoftbusAdapter::RemoveSoftbusSessionServer(const std::string &pkgname, const std::string &sessionName,
    const std::string &peerDevId)
{
    return g_mockRemoveSoftbusSessionServerReturnInt32Value;
}

int32_t SoftbusAdapter::RegisterSoftbusListener(const std::shared_ptr<ISoftbusListener> &listener,
    const std::string &sessionName, const std::string &peerDevId)
{
    return g_mockRegisterSoftbusListenerReturnInt32Value;
}

int32_t SoftbusAdapter::UnRegisterSoftbusListener(const std::string &sessionName, const std::string &peerDevId)
{
    return g_mockUnRegisterSoftbusListenerReturnInt32Value;
}

int32_t SoftbusAdapter::OpenSoftbusSession(const std::string &mySessionName, const std::string &peerSessionName,
    const std::string &peerDevId)
{
    return g_mockOpenSoftbusSessionReturnInt32Value;
}

int32_t SoftbusAdapter::CloseSoftbusSession(const int32_t sessionId)
{
    return g_mockCloseSoftbusSessionReturnInt32Value;
}

void ScreenDataChannelImplTest::SetUpTestCase(void) {}

void ScreenDataChannelImplTest::TearDownTestCase(void) {}

void ScreenDataChannelImplTest::SetUp(void)
{
    g_mockRegisterSoftbusListenerReturnInt32Value = -1;
    g_mockCreateSoftbusSessionServerReturnInt32Value = -1;
    g_mockRemoveSoftbusSessionServerReturnInt32Value = -1;
    g_mockUnRegisterSoftbusListenerReturnInt32Value = -1;
    g_mockOpenSoftbusSessionReturnInt32Value = -1;
    g_mockCloseSoftbusSessionReturnInt32Value = -1;
    std::string peerDevId = "test";
    dataChannelImpl_ = std::make_shared<ScreenDataChannelImpl>(peerDevId);
}

void ScreenDataChannelImplTest::TearDown(void)
{
    if (dataChannelImpl_ != nullptr) {
        dataChannelImpl_->CloseSession();
        dataChannelImpl_->ReleaseSession();
        dataChannelImpl_ = nullptr;
    }
}

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
 * @tc.name: CreateSession_002
 * @tc.desc: Verify the CreateSession function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, CreateSession_002, TestSize.Level1)
{
    g_mockCreateSoftbusSessionServerReturnInt32Value = DH_SUCCESS;
    g_mockRegisterSoftbusListenerReturnInt32Value = DH_SUCCESS;
    std::shared_ptr<IScreenChannelListener> listener = std::make_shared<MockIScreenChannelListener>();
    dataChannelImpl_->jpegSessionFlag_ = false;
    int32_t ret = dataChannelImpl_->CreateSession(listener);
    EXPECT_EQ(DH_SUCCESS, ret);

    dataChannelImpl_->jpegSessionFlag_ = true;
    ret = dataChannelImpl_->CreateSession(listener);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: CreateSession_003
 * @tc.desc: Verify the CreateSession function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, CreateSession_003, TestSize.Level1)
{
    std::shared_ptr<IScreenChannelListener> listener = std::make_shared<MockIScreenChannelListener>();
    int32_t ret = dataChannelImpl_->CreateSession(listener);
    EXPECT_NE(DH_SUCCESS, ret);

    g_mockCreateSoftbusSessionServerReturnInt32Value = DH_SUCCESS;
    ret = dataChannelImpl_->CreateSession(listener);
    EXPECT_NE(DH_SUCCESS, ret);
}

/**
 * @tc.name: OpenSession_001
 * @tc.desc: Verify the OpenSession function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, OpenSession_001, TestSize.Level1)
{
    std::shared_ptr<IScreenChannelListener> listener = std::make_shared<MockIScreenChannelListener>();
    int32_t ret = dataChannelImpl_->OpenSession(listener);
    EXPECT_NE(DH_SUCCESS, ret);

    g_mockRegisterSoftbusListenerReturnInt32Value = DH_SUCCESS;
    ret = dataChannelImpl_->OpenSession(listener);
    EXPECT_NE(DH_SUCCESS, ret);

    g_mockOpenSoftbusSessionReturnInt32Value = DH_SUCCESS;
    dataChannelImpl_->jpegSessionFlag_ = true;
    ret = dataChannelImpl_->OpenSession(listener);
    EXPECT_EQ(DH_SUCCESS, ret);

    dataChannelImpl_->jpegSessionFlag_ = false;
    ret = dataChannelImpl_->OpenSession(listener);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: OpenSession_002
 * @tc.desc: Verify the OpenSession function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, OpenSession_002, TestSize.Level1)
{
    int32_t ret = dataChannelImpl_->OpenSession(nullptr);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, ret);
}

/**
 * @tc.name: SendFullData_001
 * @tc.desc: Verify the SendFullData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, SendFullData_001, TestSize.Level1)
{
    std::shared_ptr<DataBuffer> screenData = nullptr;
    int32_t ret = dataChannelImpl_->SendFullData(screenData);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, ret);
}

/**
 * @tc.name: SendFullData_002
 * @tc.desc: Verify the SendFullData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, SendFullData_002, TestSize.Level1)
{
    std::shared_ptr<DataBuffer> data = std::make_shared<DataBuffer>(10);
    dataChannelImpl_->sessionId_ = 1;
    int32_t ret = dataChannelImpl_->SendFullData(data);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, ret);
}

/**
 * @tc.name: SendDirtyData_001
 * @tc.desc: Verify the SendDirtyData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, SendDirtyData_001, TestSize.Level1)
{
    std::shared_ptr<DataBuffer> screenData = nullptr;
    int32_t ret = dataChannelImpl_->SendDirtyData(screenData);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, ret);
}

/**
 * @tc.name: SendDirtyData_002
 * @tc.desc: Verify the SendDirtyData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, SendDirtyData_002, TestSize.Level1)
{
    std::shared_ptr<DataBuffer> data = std::make_shared<DataBuffer>(10);
    dataChannelImpl_->jpegSessionId_ = 1;
    int32_t ret = dataChannelImpl_->SendFullData(data);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, ret);
}

/**
 * @tc.name: release_session_test_001
 * @tc.desc: Verify the ReleaseSession function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, release_session_test_001, TestSize.Level1)
{
    g_mockRemoveSoftbusSessionServerReturnInt32Value = DH_SUCCESS;
    g_mockUnRegisterSoftbusListenerReturnInt32Value = DH_SUCCESS;
    dataChannelImpl_->jpegSessionFlag_ = false;
    EXPECT_EQ(DH_SUCCESS, dataChannelImpl_->ReleaseSession());

    dataChannelImpl_->jpegSessionFlag_ = true;
    EXPECT_EQ(DH_SUCCESS, dataChannelImpl_->ReleaseSession());

    g_mockUnRegisterSoftbusListenerReturnInt32Value = -1;
    EXPECT_NE(DH_SUCCESS, dataChannelImpl_->ReleaseSession());
}

/**
 * @tc.name: close_session_test_001
 * @tc.desc: Verify the CloseSession function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, close_session_test_001, TestSize.Level1)
{
    dataChannelImpl_->sessionId_ = 0;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_SESSION_NOT_OPEN, dataChannelImpl_->CloseSession());

    dataChannelImpl_->sessionId_ = 1;
    dataChannelImpl_->jpegSessionFlag_ = true;
    dataChannelImpl_->jpegSessionId_ = 1;
    g_mockCloseSoftbusSessionReturnInt32Value = DH_SUCCESS;
    EXPECT_EQ(DH_SUCCESS, dataChannelImpl_->CloseSession());

    dataChannelImpl_->sessionId_ = 1;
    dataChannelImpl_->jpegSessionId_ = 0;
    EXPECT_EQ(DH_SUCCESS, dataChannelImpl_->CloseSession());

    dataChannelImpl_->sessionId_ = 1;
    dataChannelImpl_->jpegSessionFlag_ = false;
    EXPECT_EQ(DH_SUCCESS, dataChannelImpl_->CloseSession());

    dataChannelImpl_->sessionId_ = 1;
    g_mockCloseSoftbusSessionReturnInt32Value = -1;
    EXPECT_NE(DH_SUCCESS, dataChannelImpl_->CloseSession());
}

/**
 * @tc.name: close_session_test_002
 * @tc.desc: Verify the CloseSession function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, close_session_test_002, TestSize.Level1)
{
    std::shared_ptr<IScreenChannelListener> listener = std::make_shared<ScreenSourceTrans>();
    dataChannelImpl_->channelListener_ = listener;
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
 * @tc.name: send_data_test_001
 * @tc.desc: Verify the SendData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, send_data_test_001, TestSize.Level1)
{
    std::shared_ptr<IScreenChannelListener> listener = nullptr;
    dataChannelImpl_->channelListener_ = listener;
    int32_t sessionId = 0;
    PeerSocketInfo peerSocketInfo = {
        .name = const_cast<char*>(PEER_SESSION_NAME.c_str()),
        .networkId = const_cast<char*>(REMOTE_DEV_ID.c_str()),
        .pkgName = const_cast<char*>(DSCREEN_PKG_NAME_TEST.c_str()),
        .dataType = DATA_TYPE_BYTES
    };
    dataChannelImpl_->OnSessionOpened(sessionId, peerSocketInfo);
    ShutdownReason reason = SHUTDOWN_REASON_UNKNOWN;
    dataChannelImpl_->OnSessionClosed(sessionId, reason);
    std::shared_ptr<DataBuffer> data = std::make_shared<DataBuffer>(10);
    data->SetDataType(VIDEO_PART_SCREEN_DATA);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, dataChannelImpl_->SendData(data));
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
    std::shared_ptr<DataBuffer> data = nullptr;
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, dataChannelImpl_->SendData(data));
}

/**
 * @tc.name: send_data_test_003
 * @tc.desc: Verify the SendData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, send_data_test_003, TestSize.Level1)
{
    std::shared_ptr<IScreenChannelListener> listener = nullptr;
    dataChannelImpl_->channelListener_ = listener;
    std::shared_ptr<DataBuffer> data = std::make_shared<DataBuffer>(10);
    data->SetDataType(VIDEO_FULL_SCREEN_DATA);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, dataChannelImpl_->SendData(data));
}

/**
 * @tc.name: on_session_opened_test_001
 * @tc.desc: Verify the OnSessionOpened function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, on_session_opened_test_001, TestSize.Level1)
{
    std::shared_ptr<IScreenChannelListener> listener = std::make_shared<ScreenSourceTrans>();
    dataChannelImpl_->channelListener_ = listener;
    int32_t sessionId = 1;
    PeerSocketInfo peerSocketInfo = {
        .name = const_cast<char*>(PEER_SESSION_NAME.c_str()),
        .networkId = const_cast<char*>(REMOTE_DEV_ID.c_str()),
        .pkgName = const_cast<char*>(DSCREEN_PKG_NAME_TEST.c_str()),
        .dataType = DATA_TYPE_BYTES
    };
    dataChannelImpl_->OnSessionOpened(sessionId, peerSocketInfo);
    EXPECT_EQ(sessionId, dataChannelImpl_->sessionId_);
}

/**
 * @tc.name: on_session_opened_test_003
 * @tc.desc: Verify the OnSessionOpened function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, on_session_opened_test_003, TestSize.Level1)
{
    std::shared_ptr<IScreenChannelListener> listener = std::make_shared<ScreenSourceTrans>();
    dataChannelImpl_->channelListener_ = listener;
    int32_t sessionId = 1;
    dataChannelImpl_->jpegSessionFlag_ = true;
    dataChannelImpl_->sessionId_ = 1;
    dataChannelImpl_->jpegSessionId_ = 1;
    dataChannelImpl_->jpegSessionOpened = false;
    PeerSocketInfo peerSocketInfo = {
        .name = const_cast<char*>(PEER_SESSION_NAME.c_str()),
        .networkId = const_cast<char*>(REMOTE_DEV_ID.c_str()),
        .pkgName = const_cast<char*>(DSCREEN_PKG_NAME_TEST.c_str()),
        .dataType = DATA_TYPE_BYTES
    };
    dataChannelImpl_->OnSessionOpened(sessionId, peerSocketInfo);

    dataChannelImpl_->sessionId_ = 2;
    dataChannelImpl_->dataSessionOpened = false;
    dataChannelImpl_->OnSessionOpened(sessionId, peerSocketInfo);
    EXPECT_NE(nullptr, dataChannelImpl_->channelListener_.lock());
}

/**
 * @tc.name: JsonToDirtyJson_001
 * @tc.desc: Verify the JsonToDirtyJson function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, JsonToDirtyJson_001, TestSize.Level1)
{
    std::shared_ptr<DataBuffer> screenData = std::make_shared<DataBuffer>(10);
    nlohmann::json rectJson;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);
    EXPECT_EQ(0, screenData->DataType());
    rectJson["dirtySize"] = DIRTY_MAX_SIZE - 1;
    rectJson["dataType"] = 2;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);
    EXPECT_EQ(0, screenData->DataType());
    rectJson["3"] = 2;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);
    EXPECT_EQ(0, screenData->DataType());
    nlohmann::json testJson;
    rectJson["0"] = testJson;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);
    EXPECT_EQ(0, screenData->DataType());
    rectJson["0"][KEY_POINT_DIRTY_X] = 100;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);
    EXPECT_EQ(0, screenData->DataType());
    rectJson["0"][KEY_POINT_DIRTY_Y] = 100;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);
    EXPECT_EQ(0, screenData->DataType());
    rectJson["0"][KEY_POINT_DIRTY_W] = 100;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);
    EXPECT_EQ(0, screenData->DataType());
    rectJson["0"][KEY_POINT_DIRTY_H] = 100;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);
    EXPECT_EQ(0, screenData->DataType());
    rectJson["0"][KEY_POINT_DIRTY_SIZE] = 100;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);
    EXPECT_EQ(0, screenData->DataType());
    rectJson["dirtySize"] = DIRTY_MAX_SIZE;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);
    EXPECT_EQ(0, screenData->DataType());
}

/**
 * @tc.name: ProcessDirtyData_001
 * @tc.desc: Verify the ProcessDirtyData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, ProcessDirtyData_001, TestSize.Level1)
{
    StreamData data = {0};
    StreamData ext = {0};
    std::shared_ptr<DataBuffer> dataBuffer = std::make_shared<DataBuffer>(10);
    dataChannelImpl_->ProcessDirtyData(&data, dataBuffer, &ext);
    EXPECT_EQ(0, dataBuffer->DataType());

    std::shared_ptr<IScreenChannelListener> listener = std::make_shared<ScreenSourceTrans>();
    dataChannelImpl_->channelListener_ = listener;
    data.bufLen = dataBuffer->Capacity() + 1;
    dataChannelImpl_->ProcessDirtyData(&data, dataBuffer, &ext);
    EXPECT_EQ(0, dataBuffer->DataType());
}

/**
 * @tc.name: SetJpegSessionFlag_001
 * @tc.desc: Verify the SetJpegSessionFlag function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenDataChannelImplTest, SetJpegSessionFlag_001, TestSize.Level1)
{
    bool flag = true;
    dataChannelImpl_->SetJpegSessionFlag(flag);
    EXPECT_EQ(dataChannelImpl_->jpegSessionFlag_, flag);

    flag = false;
    dataChannelImpl_->SetJpegSessionFlag(flag);
    EXPECT_EQ(dataChannelImpl_->jpegSessionFlag_, flag);
}
} // DistributedHardware
} // OHOS