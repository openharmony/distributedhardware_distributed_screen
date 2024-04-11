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

#include "softbus_adapter_test.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
    namespace {
    const std::string PEER_SESSION_NAME = "ohos.dhardware.dscreen.session8647073e02e7a78f09473aa125";
    const std::string REMOTE_DEV_ID = "f6d4c0864707aefte7a78f09473aa122ff57fc81c00981fcf5be989e7d112125";
    const std::string DSCREEN_PKG_NAME_TEST = "ohos.dhardware.dscreen";
}
void SoftbusAdapterTest::SetUpTestCase(void) {}

void SoftbusAdapterTest::TearDownTestCase(void) {}

void SoftbusAdapterTest::SetUp(void) {}

void SoftbusAdapterTest::TearDown(void) {}

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

    infoInstance.processName = "SoftBusAdapterTest";
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

void DisablePermissionAccess(const uint64_t &tokenId)
{
    OHOS::Security::AccessToken::AccessTokenKit::DeleteToken(tokenId);
}

static void ScreenOnSoftbusSessionOpened(int32_t sessionId, PeerSocketInfo info) {}

static void ScreenOnSoftbusSessionClosed(int32_t sessionId, ShutdownReason reason) {}

static void ScreenOnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen) {}

static void ScreenOnStreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
    const StreamFrameInfo *frameInfo) {}

static void ScreenOnMessageReceived(int sessionId, const void *data, unsigned int dataLen) {}

/**
 * @tc.name: CreateSoftbusSessionServer_001
 * @tc.desc: Verify the CreateSoftbusSessionServer function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, CreateSoftbusSessionServer_001, TestSize.Level1)
{
    const char* perms[] = {
        "ohos.permission.DISTRIBUTED_DATASYNC",
        "ohos.permission.CAPTURE_SCREEN",
    };
    EnablePermissionAccess(perms, sizeof(perms) / sizeof(perms[0]), tokenId_);
    softbusAdapter.sessListener_.OnBind = ScreenOnSoftbusSessionOpened;
    softbusAdapter.sessListener_.OnShutdown = ScreenOnSoftbusSessionClosed;
    softbusAdapter.sessListener_.OnBytes = ScreenOnBytesReceived;
    softbusAdapter.sessListener_.OnStream = ScreenOnStreamReceived;
    softbusAdapter.sessListener_.OnMessage = ScreenOnMessageReceived;

    std::string pkgname = PKG_NAME;
    std::string sessionName = DATA_SESSION_NAME;
    std::string peerDevId = "peerDevId";
    int32_t sessionId = 0;
    void *data = nullptr;
    uint32_t dataLen = 0;
    StreamData sData;
    sData.bufLen = 0;
    sData.buf = reinterpret_cast<char*>(&dataLen);
    StreamData *streamData = nullptr;
    StreamData *ext = nullptr;
    StreamFrameInfo *frameInfo = nullptr;
    softbusAdapter.OnBytesReceived(sessionId, data, dataLen);
    softbusAdapter.OnStreamReceived(sessionId, streamData, ext, frameInfo);
    data = reinterpret_cast<void*>(&dataLen);
    streamData = &sData;
    softbusAdapter.OnBytesReceived(sessionId, data, dataLen);
    softbusAdapter.OnStreamReceived(sessionId, streamData, ext, frameInfo);
    dataLen = 100;
    sData.bufLen = 100;
    softbusAdapter.OnBytesReceived(sessionId, data, dataLen);
    softbusAdapter.OnStreamReceived(sessionId, streamData, ext, frameInfo);
    int32_t actual = softbusAdapter.CreateSoftbusSessionServer(pkgname, sessionName, peerDevId);
    EXPECT_EQ(DH_SUCCESS, actual);
    softbusAdapter.RemoveSoftbusSessionServer(pkgname, sessionName, peerDevId);
    DisablePermissionAccess(tokenId_);
}

/**
 * @tc.name: CreateSoftbusSessionServer_002
 * @tc.desc: Verify the CreateSoftbusSessionServer function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, CreateSoftbusSessionServer_002, TestSize.Level1)
{
    softbusAdapter.serverIdMap_.clear();
    std::string pkgname = PKG_NAME;
    std::string sessionName = DATA_SESSION_NAME;
    std::string peerDevId = "peerDevId";

    int32_t actual = softbusAdapter.CreateSoftbusSessionServer(pkgname, sessionName, peerDevId);
    EXPECT_EQ(DH_SUCCESS, actual);
    softbusAdapter.RemoveSoftbusSessionServer(pkgname, sessionName, peerDevId);
}

/**
 * @tc.name: CreateSoftbusSessionServer_003
 * @tc.desc: Verify the CreateSoftbusSessionServer function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, CreateSoftbusSessionServer_003, TestSize.Level1)
{
    std::string pkgName = "ohos.dhardware.dscreentest";
    std::string sessionName = DATA_SESSION_NAME;
    std::string peerDevId = "peerDevId";
    softbusAdapter.serverIdMap_.clear();
    softbusAdapter.serverIdMap_.insert(std::make_pair(100000, sessionName + "_" + peerDevId));
    int32_t actual = softbusAdapter.CreateSoftbusSessionServer(pkgName, sessionName, peerDevId);
    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: CreateSoftbusSessionServer_004
 * @tc.desc: Verify the CreateSoftbusSessionServer function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, CreateSoftbusSessionServer_004, TestSize.Level1)
{
    std::string pkgName = "ohos.dhardware.dscreentest";
    std::string sessionName = DATA_SESSION_NAME;
    std::string peerDevId = "peerDevId";
    softbusAdapter.serverIdMap_.clear();
    softbusAdapter.serverIdMap_.insert(std::make_pair(100000, sessionName + "__" + peerDevId));
    int32_t actual = softbusAdapter.CreateSoftbusSessionServer(pkgName, sessionName, peerDevId);
    EXPECT_EQ(ERR_DH_SCREEN_ADAPTER_BAD_VALUE, actual);
}

/**
 * @tc.name: RegisterSoftbusListener_001
 * @tc.desc: Verify the RegisterSoftbusListener function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, RegisterSoftbusListener_001, TestSize.Level1)
{
    std::shared_ptr<ISoftbusListener> listener = std::make_shared<MockSoftbusListener>();
    std::string sessionName = DATA_SESSION_NAME;
    std::string peerDevId = "testDevId";

    int32_t actual = softbusAdapter.RegisterSoftbusListener(listener, sessionName, peerDevId);
    EXPECT_EQ(DH_SUCCESS, actual);

    actual = softbusAdapter.UnRegisterSoftbusListener(sessionName, peerDevId);
    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: RegisterSoftbusListener_002
 * @tc.desc: Verify the RegisterSoftbusListener function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, RegisterSoftbusListener_002, TestSize.Level1)
{
    std::shared_ptr<ISoftbusListener> listener = std::make_shared<MockSoftbusListener>();
    std::string sessionName = DATA_SESSION_NAME;
    std::string peerDevId = "testDevId";

    int32_t actual = softbusAdapter.RegisterSoftbusListener(listener, sessionName, peerDevId);
    EXPECT_EQ(DH_SUCCESS, actual);

    actual = softbusAdapter.RegisterSoftbusListener(listener, sessionName, peerDevId);
    EXPECT_EQ(ERR_DH_SCREEN_ADAPTER_REGISTER_SOFTBUS_LISTENER_FAIL, actual);
}

/**
 * @tc.name: RegisterSoftbusListener_003
 * @tc.desc: Verify the RegisterSoftbusListener function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, RegisterSoftbusListener_003, TestSize.Level1)
{
    std::shared_ptr<ISoftbusListener> listener = nullptr;
    std::string sessionName = DATA_SESSION_NAME;
    std::string peerDevId = "testDevId";

    int32_t actual = softbusAdapter.RegisterSoftbusListener(listener, sessionName, peerDevId);
    EXPECT_EQ(ERR_DH_SCREEN_ADAPTER_REGISTER_SOFTBUS_LISTENER_FAIL, actual);
}

/**
 * @tc.name: UnRegisterSoftbusListener_001
 * @tc.desc: Verify the UnRegisterSoftbusListener function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, UnRegisterSoftbusListener_001, TestSize.Level1)
{
    std::string sessionName = DATA_SESSION_NAME;
    std::string peerDevId = "testDevId";

    int32_t actual = softbusAdapter.UnRegisterSoftbusListener(sessionName, peerDevId);

    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: RemoveSoftbusSessionServer_001
 * @tc.desc: Verify the RemoveSoftbusSessionServer function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, RemoveSoftbusSessionServer_001, TestSize.Level1)
{
    std::string pkgname = "";
    std::string sessionName = "";
    std::string peerDevId = "";

    int32_t actual = softbusAdapter.RemoveSoftbusSessionServer(pkgname, sessionName, peerDevId);

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: RemoveSoftbusSessionServer_002
 * @tc.desc: Verify the RemoveSoftbusSessionServer function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, RemoveSoftbusSessionServer_002, TestSize.Level1)
{
    std::string pkgname = PKG_NAME;
    std::string sessionName = DATA_SESSION_NAME;
    std::string peerDevId = "peerDevId";

    int32_t actual = softbusAdapter.RemoveSoftbusSessionServer(pkgname, sessionName, peerDevId);
    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: OpenSoftbusSession_001
 * @tc.desc: Verify the OpenSoftbusSession function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, OpenSoftbusSession_001, TestSize.Level1)
{
    std::string mySessionName = DATA_SESSION_NAME;
    std::string peerSessionName = DATA_SESSION_NAME;
    std::string peerDevId = "testDevId";
    int32_t actual = softbusAdapter.OpenSoftbusSession(mySessionName, peerSessionName, peerDevId);
    EXPECT_EQ(ERR_DH_SCREEN_ADAPTER_PARA_ERROR, actual);
}

/**
 * @tc.name: CloseSoftbusSession_001
 * @tc.desc: Verify the CloseSoftbusSession function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, CloseSoftbusSession_001, TestSize.Level1)
{
    int32_t actual = softbusAdapter.CloseSoftbusSession(0);
    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: SendSoftbusBytes_001
 * @tc.desc: Verify the SendSoftbusBytes function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, SendSoftbusBytes_001, TestSize.Level1)
{
    int32_t sessionId = 0;
    void *data = nullptr;
    int32_t dataLen = 0;
    int32_t actual = softbusAdapter.SendSoftbusBytes(sessionId, data, dataLen);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, actual);
}

/**
 * @tc.name: SendSoftbusStream_001
 * @tc.desc: Verify the SendSoftbusStream function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, SendSoftbusStream_001, TestSize.Level1)
{
    int32_t sessionId = 0;
    StreamData *data = nullptr;
    StreamData *ext = nullptr;
    StreamFrameInfo *param = nullptr;
    int32_t actual = softbusAdapter.SendSoftbusStream(sessionId, data, ext, param);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, actual);
}

/**
 * @tc.name: GetSoftbusListener_001
 * @tc.desc: Verify the GetSoftbusListener function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, GetSoftbusListener_001, TestSize.Level1)
{
    int32_t sessionId = 0;
    SessionInfo sessionInfo;
    sessionInfo.sessionName = "hello";
    sessionInfo.peerDevId = "world";
    std::shared_ptr<ISoftbusListener> listener = nullptr;
    softbusAdapter.mapListeners_["hello_world"] = listener;
    std::shared_ptr<ISoftbusListener> actual = softbusAdapter.GetSoftbusListenerById(sessionId);
    EXPECT_EQ(nullptr, actual);
}

/**
 * @tc.name: OnSoftbusSessionOpened_001
 * @tc.desc: Verify the OnSoftbusSessionOpened function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, OnSoftbusSessionOpened_001, TestSize.Level1)
{
    PeerSocketInfo peerSocketInfo = {
        .name = const_cast<char*>(PEER_SESSION_NAME.c_str()),
        .networkId = const_cast<char*>(REMOTE_DEV_ID.c_str()),
        .pkgName = const_cast<char*>(DSCREEN_PKG_NAME_TEST.c_str()),
        .dataType = DATA_TYPE_BYTES
    };
    int32_t actual = softbusAdapter.OnSoftbusSessionOpened(0, peerSocketInfo);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, actual);
}

/**
 * @tc.name: GetSoftbusListenerById_001
 * @tc.desc: Verify the GetSoftbusListenerById function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, GetSoftbusListenerById_001, TestSize.Level1)
{
    int32_t sessionId = 0;
    softbusAdapter.mapSessListeners_[sessionId] = nullptr;
    std::shared_ptr<ISoftbusListener> listener = softbusAdapter.GetSoftbusListenerById(sessionId);
    EXPECT_EQ(nullptr, listener);
}
} // DistributedHardware
} // OHOS