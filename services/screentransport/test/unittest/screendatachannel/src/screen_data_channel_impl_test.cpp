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
#include "dscreen_util.h"
#include "dscreen_json_util.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
namespace {
    const std::string PEER_SESSION_NAME = "ohos.dhardware.dscreen.session8647073e02e7a78f09473aa123";
    const std::string REMOTE_DEV_ID = "f6d4c0864707aefte7a78f09473aa122ff57fc81c00981fcf5be989e7d112123";
    const std::string DSCREEN_PKG_NAME_TEST = "ohos.dhardware.dscreen";
}
void ScreenDataChannelImplTest::SetUpTestCase(void) {}

void ScreenDataChannelImplTest::TearDownTestCase(void) {}

void ScreenDataChannelImplTest::SetUp(void)
{
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

void EnablePermissionAccess(const char* perms[], size_t permsNum, uint64_t &tokenId)
{
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = permsNum,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .aplStr = "system_basic",
    };

    infoInstance.processName = "DataChannelTest";
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

void DisablePermissionAccess(const uint64_t &tokenId)
{
    OHOS::Security::AccessToken::AccessTokenKit::DeleteToken(tokenId);
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
    const char* perms[] = {
        "ohos.permission.DISTRIBUTED_DATASYNC",
        "ohos.permission.CAPTURE_SCREEN",
    };
    EnablePermissionAccess(perms, sizeof(perms) / sizeof(perms[0]), tokenId_);
    std::shared_ptr<IScreenChannelListener> listener = std::make_shared<MockIScreenChannelListener>();
    dataChannelImpl_->jpegSessionFlag_ = false;
    int32_t ret = dataChannelImpl_->CreateSession(listener);
    DisablePermissionAccess(tokenId_);
    EXPECT_EQ(DH_SUCCESS, ret);
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
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, ret);
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
    const char* perms[] = {
        "ohos.permission.DISTRIBUTED_DATASYNC",
        "ohos.permission.CAPTURE_SCREEN",
    };
    EnablePermissionAccess(perms, sizeof(perms) / sizeof(perms[0]), tokenId_);
    EXPECT_EQ(DH_SUCCESS, dataChannelImpl_->ReleaseSession());
    DisablePermissionAccess(tokenId_);
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
    std::shared_ptr<IScreenChannelListener> listener = std::make_shared<MockIScreenChannelListener>();
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
    std::shared_ptr<IScreenChannelListener>  mockListener = std::make_shared<MockIScreenChannelListener>();
    dataChannelImpl_->channelListener_ = mockListener;
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

    rectJson["dirtySize"] = 2;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);

    rectJson["dirtySize"] = INT32_MAX + 1;
    rectJson["dataType"] = 2;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);

    rectJson["dirtySize"] = DIRTY_MAX_SIZE;
    rectJson["dataType"] = 2;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);

    rectJson["dirtySize"] = 2;
    rectJson["dataType"] = 2;
    rectJson["3"] = 2;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);

    nlohmann::json testJson;
    rectJson["0"] = testJson;
    rectJson["0"][KEY_POINT_DIRTY_X] = INT32_MAX + 1;
    rectJson["0"][KEY_POINT_DIRTY_Y] = INT32_MAX + 1;
    rectJson["0"][KEY_POINT_DIRTY_W] = INT32_MAX + 1;
    rectJson["0"][KEY_POINT_DIRTY_H] = INT32_MAX + 1;
    rectJson["0"][KEY_POINT_DIRTY_SIZE] = INT32_MAX + 1;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);

    rectJson["0"][KEY_POINT_DIRTY_SIZE] = 2;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);

    rectJson["0"][KEY_POINT_DIRTY_H] = 2;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);

    rectJson["0"][KEY_POINT_DIRTY_W] = 2;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);

    rectJson["0"][KEY_POINT_DIRTY_Y] = 2;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);

    rectJson["0"][KEY_POINT_DIRTY_X] = 2;
    dataChannelImpl_->JsonToDirtyJson(rectJson, screenData);
    EXPECT_EQ(nullptr, dataChannelImpl_->channelListener_.lock());
}

} // DistributedHardware
} // OHOS