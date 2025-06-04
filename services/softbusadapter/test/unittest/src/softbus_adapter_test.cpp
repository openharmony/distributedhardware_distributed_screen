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
#include "device_manager_impl_mock.h"
#include "dscreen_errcode.h"
#include "dscreen_util.h"
#include "mock_other_method.h"
#include "socket.h"
#include "softbus_adapter.h"
#include "softbus_permission_check.h"
#include "token_setproc.h"

using namespace testing;
using namespace testing::ext;

static int g_mockBindReturnIntValue = 0;
static int g_mockListenReturnIntValue = 0;
static int g_mockSendBytesReturnIntValue = 0;
static int g_mockSendStreamReturnIntValue = 0;
static int g_mockSocketReturnIntValue = 0;

namespace OHOS {
namespace DistributedHardware {
namespace {
    const std::string PEER_SESSION_NAME = "ohos.dhardware.dscreen.session8647073e02e7a78f09473aa125";
    const std::string REMOTE_DEV_ID = "f6d4c0864707aefte7a78f09473aa122ff57fc81c00981fcf5be989e7d112125";
    const std::string DSCREEN_PKG_NAME_TEST = "ohos.dhardware.dscreen";
}

static bool g_checkSrc = true;
static bool g_checkSink = true;
static bool g_setAccess = true;
static bool g_transCaller = true;
static bool g_fillLocal = true;

bool SoftBusPermissionCheck::CheckSrcPermission(const std::string &sinkNetworkId)
{
    return g_checkSrc;
}

bool SoftBusPermissionCheck::CheckSinkPermission(const AccountInfo &callerAccountInfo)
{
    return g_checkSink;
}

bool SoftBusPermissionCheck::SetAccessInfoToSocket(const int32_t sessionId)
{
    return g_setAccess;
}
bool SoftBusPermissionCheck::TransCallerInfo(SocketAccessInfo *callerInfo,
    AccountInfo &callerAccountInfo, const std::string &networkId)
{
    return g_transCaller;
}

bool SoftBusPermissionCheck::FillLocalInfo(SocketAccessInfo *localInfo)
{
    return g_fillLocal;
}

void SoftbusAdapterTest::SetUpTestCase(void)
{
}

void SoftbusAdapterTest::TearDownTestCase(void)
{
}

void SoftbusAdapterTest::SetUp(void)
{
    g_mockBindReturnIntValue = 0;
    g_mockListenReturnIntValue = 0;
    g_mockSendBytesReturnIntValue = 0;
    g_mockSendStreamReturnIntValue = 0;
    g_mockSocketReturnIntValue = 0;
    g_checkSrc = true;
    g_checkSink = true;
    g_setAccess = true;
    g_transCaller = true;
    g_fillLocal = true;
    softbusAdapter_ = std::make_shared<SoftbusAdapter>();
}

void SoftbusAdapterTest::TearDown(void)
{
    softbusAdapter_ = nullptr;
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
    softbusAdapter_->sessListener_.OnBind = ScreenOnSoftbusSessionOpened;
    softbusAdapter_->sessListener_.OnShutdown = ScreenOnSoftbusSessionClosed;
    softbusAdapter_->sessListener_.OnBytes = ScreenOnBytesReceived;
    softbusAdapter_->sessListener_.OnStream = ScreenOnStreamReceived;
    softbusAdapter_->sessListener_.OnMessage = ScreenOnMessageReceived;

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
    softbusAdapter_->OnBytesReceived(sessionId, data, dataLen);
    softbusAdapter_->OnStreamReceived(sessionId, streamData, ext, frameInfo);
    data = reinterpret_cast<void*>(&dataLen);
    streamData = &sData;
    softbusAdapter_->OnBytesReceived(sessionId, data, dataLen);
    softbusAdapter_->OnStreamReceived(sessionId, streamData, ext, frameInfo);
    dataLen = DSCREEN_MAX_RECV_DATA_LEN + 1;
    sData.bufLen = DSCREEN_MAX_RECV_DATA_LEN + 1;
    softbusAdapter_->OnBytesReceived(sessionId, data, dataLen);
    softbusAdapter_->OnStreamReceived(sessionId, streamData, ext, frameInfo);
    dataLen = 100;
    sData.bufLen = 100;
    softbusAdapter_->OnBytesReceived(sessionId, data, dataLen);
    softbusAdapter_->OnStreamReceived(sessionId, streamData, ext, frameInfo);

    std::shared_ptr<ISoftbusListener> listener = std::make_shared<MockSoftbusListener>();
    softbusAdapter_->devId2SessIdMap_[sessionId] = "strListenerKey";
    softbusAdapter_->mapListeners_["strListenerKey"] = listener;
    softbusAdapter_->OnBytesReceived(sessionId, data, dataLen);
    softbusAdapter_->OnStreamReceived(sessionId, streamData, ext, frameInfo);

    int32_t actual = softbusAdapter_->CreateSoftbusSessionServer(pkgname, sessionName, peerDevId);
    EXPECT_EQ(DH_SUCCESS, actual);
    softbusAdapter_->RemoveSoftbusSessionServer(pkgname, sessionName, peerDevId);
}

/**
 * @tc.name: CreateSoftbusSessionServer_002
 * @tc.desc: Verify the CreateSoftbusSessionServer function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, CreateSoftbusSessionServer_002, TestSize.Level1)
{
    std::string pkgname = PKG_NAME;
    std::string sessionName = DATA_SESSION_NAME;
    std::string peerDevId = "peerDevId";
    int32_t actual = softbusAdapter_->CreateSoftbusSessionServer(pkgname, sessionName, peerDevId);
    EXPECT_EQ(DH_SUCCESS, actual);
    softbusAdapter_->RemoveSoftbusSessionServer(pkgname, sessionName, peerDevId);
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
    softbusAdapter_->serverIdMap_.insert(std::make_pair(100000, sessionName + "_" + peerDevId));
    int32_t actual = softbusAdapter_->CreateSoftbusSessionServer(pkgName, sessionName, peerDevId);
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
    std::string pkgName = PKG_NAME;
    std::string sessionName = DATA_SESSION_NAME;
    std::string peerDevId = "peerDevId";
    g_mockListenReturnIntValue = -1;
    softbusAdapter_->serverIdMap_.insert(std::make_pair(100000, "test"));
    int32_t actual = softbusAdapter_->CreateSoftbusSessionServer(pkgName, sessionName, peerDevId);
    EXPECT_EQ(ERR_DH_SCREEN_ADAPTER_BAD_VALUE, actual);

    softbusAdapter_->serverIdMap_.clear();
    g_mockSocketReturnIntValue = -1;
    actual = softbusAdapter_->CreateSoftbusSessionServer(pkgName, sessionName, peerDevId);
    EXPECT_EQ(ERR_DH_SCREEN_ADAPTER_BAD_VALUE, actual);
    softbusAdapter_->RemoveSoftbusSessionServer(pkgName, sessionName, peerDevId);
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

    int32_t actual = softbusAdapter_->RegisterSoftbusListener(listener, sessionName, peerDevId);
    EXPECT_EQ(DH_SUCCESS, actual);

    actual = softbusAdapter_->UnRegisterSoftbusListener(sessionName, peerDevId);
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

    int32_t actual = softbusAdapter_->RegisterSoftbusListener(listener, sessionName, peerDevId);
    EXPECT_EQ(DH_SUCCESS, actual);

    actual = softbusAdapter_->RegisterSoftbusListener(listener, sessionName, peerDevId);
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

    int32_t actual = softbusAdapter_->RegisterSoftbusListener(listener, sessionName, peerDevId);
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

    int32_t actual = softbusAdapter_->UnRegisterSoftbusListener(sessionName, peerDevId);

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

    int32_t actual = softbusAdapter_->RemoveSoftbusSessionServer(pkgname, sessionName, peerDevId);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);

    sessionName = "sessionName";
    actual = softbusAdapter_->RemoveSoftbusSessionServer(pkgname, sessionName, peerDevId);
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

    int32_t actual = softbusAdapter_->RemoveSoftbusSessionServer(pkgname, sessionName, peerDevId);
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

    int32_t actual = softbusAdapter_->OpenSoftbusSession(mySessionName, peerSessionName, peerDevId);
    EXPECT_NE(DH_SUCCESS, actual);

    g_setAccess = false;
    actual = softbusAdapter_->OpenSoftbusSession(mySessionName, peerSessionName, peerDevId);
    EXPECT_EQ(ERR_DH_SCREEN_ADAPTER_CONTEXT, actual);

    g_checkSrc = false;
    actual = softbusAdapter_->OpenSoftbusSession(mySessionName, peerSessionName, peerDevId);
    EXPECT_EQ(ERR_DH_SCREEN_ADAPTER_PERMISSION_DENIED, actual);

    g_checkSrc = true;
    g_setAccess = true;
    g_mockBindReturnIntValue = -1;
    actual = softbusAdapter_->OpenSoftbusSession(mySessionName, peerSessionName, peerDevId);
    EXPECT_EQ(ERR_DH_SCREEN_ADAPTER_PARA_ERROR, actual);

    g_mockSocketReturnIntValue = -1;
    actual = softbusAdapter_->OpenSoftbusSession(mySessionName, peerSessionName, peerDevId);
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
    int32_t actual = softbusAdapter_->CloseSoftbusSession(0);
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
    int32_t actual = softbusAdapter_->SendSoftbusBytes(sessionId, data, dataLen);
    EXPECT_EQ(DH_SUCCESS, actual);

    g_mockSendBytesReturnIntValue = -1;
    actual = softbusAdapter_->SendSoftbusBytes(sessionId, data, dataLen);
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
    int32_t actual = softbusAdapter_->SendSoftbusStream(sessionId, data, ext, param);
    EXPECT_EQ(DH_SUCCESS, actual);
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
    int32_t actual = softbusAdapter_->OnSoftbusSessionOpened(0, peerSocketInfo);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, actual);
}

/**
 * @tc.name: OnSoftbusSessionOpened_002
 * @tc.desc: Verify the OnSoftbusSessionOpened function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, OnSoftbusSessionOpened_002, TestSize.Level1)
{
    PeerSocketInfo peerSocketInfo = {
        .name = const_cast<char*>(PEER_SESSION_NAME.c_str()),
        .networkId = nullptr,
        .pkgName = const_cast<char*>(DSCREEN_PKG_NAME_TEST.c_str()),
        .dataType = DATA_TYPE_BYTES
    };
    std::string pkgName = "ohos.dhardware.dscreentest";
    std::string sessionName = DATA_SESSION_NAME;
    std::string peerDevId = "peerDevId";
    softbusAdapter_->serverIdMap_.insert(std::make_pair(100000, sessionName + "_" + peerDevId));

    int32_t sessionId = 0;
    std::shared_ptr<ISoftbusListener> listener = std::make_shared<MockSoftbusListener>();
    softbusAdapter_->devId2SessIdMap_[sessionId] = "strListenerKey";
    softbusAdapter_->mapListeners_["strListenerKey"] = listener;

    int32_t actual = softbusAdapter_->OnSoftbusSessionOpened(sessionId, peerSocketInfo);
    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: OnSoftbusSessionOpened_003
 * @tc.desc: Verify the OnSoftbusSessionOpened function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, OnSoftbusSessionOpened_003, TestSize.Level1)
{
    PeerSocketInfo peerSocketInfo = {
        .name = const_cast<char*>(PEER_SESSION_NAME.c_str()),
        .networkId = const_cast<char*>(REMOTE_DEV_ID.c_str()),
        .pkgName = const_cast<char*>(DSCREEN_PKG_NAME_TEST.c_str()),
        .dataType = DATA_TYPE_BYTES
    };
    softbusAdapter_->serverIdMap_.insert(std::make_pair(100000, REMOTE_DEV_ID));
    int32_t actual = softbusAdapter_->OnSoftbusSessionOpened(0, peerSocketInfo);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, actual);
}

/**
 * @tc.name: OnSoftbusSessionOpened_004
 * @tc.desc: Verify the OnSoftbusSessionOpened function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, OnSoftbusSessionOpened_004, TestSize.Level1)
{
    PeerSocketInfo peerSocketInfo = {
        .name = const_cast<char*>(PEER_SESSION_NAME.c_str()),
        .networkId = const_cast<char*>(REMOTE_DEV_ID.c_str()),
        .pkgName = const_cast<char*>(DSCREEN_PKG_NAME_TEST.c_str()),
        .dataType = DATA_TYPE_BYTES
    };
    softbusAdapter_->serverIdMap_.insert(std::make_pair(100000, "not exist id"));
    int32_t actual = softbusAdapter_->OnSoftbusSessionOpened(0, peerSocketInfo);
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, actual);
}

/**
 * @tc.name: OnSoftbusSessionClosed_001
 * @tc.desc: Verify the OnSoftbusSessionClosed function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, OnSoftbusSessionClosed_001, TestSize.Level1)
{
    int32_t sessionId = 0;
    ShutdownReason reason = SHUTDOWN_REASON_UNKNOWN;
    softbusAdapter_->mapSessListeners_[sessionId] = nullptr;
    softbusAdapter_->OnSoftbusSessionClosed(sessionId, reason);
    EXPECT_TRUE(softbusAdapter_->mapSessListeners_.size() > 0);
}

/**
 * @tc.name: OnSoftbusSessionClosed_002
 * @tc.desc: Verify the OnSoftbusSessionClosed function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, OnSoftbusSessionClosed_002, TestSize.Level1)
{
    int32_t sessionId = 0;
    ShutdownReason reason = SHUTDOWN_REASON_UNKNOWN;
    std::shared_ptr<ISoftbusListener> listener = std::make_shared<MockSoftbusListener>();
    softbusAdapter_->mapSessListeners_[sessionId] = listener;
    softbusAdapter_->OnSoftbusSessionClosed(sessionId, reason);
    EXPECT_TRUE(softbusAdapter_->mapSessListeners_.size() == 0);
}

/**
 * @tc.name: GetSoftbusListenerByName_001
 * @tc.desc: Verify the GetSoftbusListenerByName function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, GetSoftbusListenerByName_001, TestSize.Level1)
{
    int32_t sessionId = 0;
    softbusAdapter_->devId2SessIdMap_[sessionId] = "test";
    std::shared_ptr<ISoftbusListener> listener = softbusAdapter_->GetSoftbusListenerByName(sessionId);
    EXPECT_EQ(nullptr, listener);
}

/**
 * @tc.name: GetSoftbusListenerByName_002
 * @tc.desc: Verify the GetSoftbusListenerByName function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, GetSoftbusListenerByName_002, TestSize.Level1)
{
    int32_t sessionId = 0;
    softbusAdapter_->devId2SessIdMap_[sessionId + 1] = "test";
    std::shared_ptr<ISoftbusListener> listener = softbusAdapter_->GetSoftbusListenerByName(sessionId);
    EXPECT_EQ(nullptr, listener);
}

/**
 * @tc.name: GetSoftbusListenerByName_003
 * @tc.desc: Verify the GetSoftbusListenerByName function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, GetSoftbusListenerByName_003, TestSize.Level1)
{
    int32_t sessionId = 0;
    std::shared_ptr<ISoftbusListener> listener = std::make_shared<MockSoftbusListener>();
    softbusAdapter_->devId2SessIdMap_[sessionId] = "strListenerKey";
    softbusAdapter_->mapListeners_["strListenerKey"] = listener;
    std::shared_ptr<ISoftbusListener> mapListener = softbusAdapter_->GetSoftbusListenerByName(sessionId);
    EXPECT_NE(nullptr, mapListener);
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
    softbusAdapter_->mapSessListeners_[sessionId] = nullptr;
    std::shared_ptr<ISoftbusListener> listener = softbusAdapter_->GetSoftbusListenerById(sessionId);
    EXPECT_EQ(nullptr, listener);
}

/**
 * @tc.name: GetSoftbusListenerById_002
 * @tc.desc: Verify the GetSoftbusListenerById function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, GetSoftbusListenerById_002, TestSize.Level1)
{
    int32_t sessionId = 0;
    SessionInfo sessionInfo;
    sessionInfo.sessionName = "hello";
    sessionInfo.peerDevId = "world";
    std::shared_ptr<ISoftbusListener> listener = nullptr;
    softbusAdapter_->mapListeners_["hello_world"] = listener;
    std::shared_ptr<ISoftbusListener> actual = softbusAdapter_->GetSoftbusListenerById(sessionId);
    EXPECT_EQ(nullptr, actual);
}

/**
 * @tc.name: OnNegotiate2_001
 * @tc.desc: Verify the GetSoftbusListenerById function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, OnNegotiate2_001, TestSize.Level1)
{
    PeerSocketInfo info;
    EXPECT_TRUE(softbusAdapter_->OnNegotiate2(0, info, nullptr, nullptr));

    char networkId[10] = "networkId";
    SocketAccessInfo peerInfo;
    info.networkId = networkId;
    g_transCaller = false;
    EXPECT_FALSE(softbusAdapter_->OnNegotiate2(0, info, &peerInfo, nullptr));

    g_transCaller = true;
    g_fillLocal = false;
    EXPECT_FALSE(softbusAdapter_->OnNegotiate2(0, info, &peerInfo, nullptr));

    g_fillLocal = true;
    g_checkSink = false;
    EXPECT_FALSE(softbusAdapter_->OnNegotiate2(0, info, &peerInfo, nullptr));

    g_checkSink = true;
    EXPECT_TRUE(softbusAdapter_->OnNegotiate2(0, info, &peerInfo, nullptr));
}
} // DistributedHardware
} // OHOS

extern "C" __attribute__((constructor)) int Socket(SocketInfo info)
{
    return g_mockSocketReturnIntValue;
}

extern "C" __attribute__((constructor)) int Bind(int32_t socket, const QosTV qos[], uint32_t qosCount,
    const ISocketListener *listener)
{
    return g_mockBindReturnIntValue;
}

extern "C" __attribute__((constructor)) int Listen(int32_t socket, const QosTV qos[], uint32_t qosCount,
    const ISocketListener *listener)
{
    return g_mockListenReturnIntValue;
}

extern "C" __attribute__((constructor)) int SendBytes(int32_t socket, const void *data, uint32_t len)
{
    return g_mockSendBytesReturnIntValue;
}

extern "C" __attribute__((constructor)) void Shutdown(int32_t socket) {}

extern "C" __attribute__((constructor)) int SendStream(int32_t socket, const StreamData *data, const StreamData *ext,
    const StreamFrameInfo *param)
{
    return g_mockSendStreamReturnIntValue;
}