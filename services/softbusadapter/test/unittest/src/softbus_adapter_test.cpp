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

#include "softbus_adapter_test.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void SoftbusAdapterTest::SetUpTestCase(void) {}

void SoftbusAdapterTest::TearDownTestCase(void) {}

void SoftbusAdapterTest::SetUp(void) {}

void SoftbusAdapterTest::TearDown(void) {}

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
    std::string pkgname = PKG_NAME;
    std::string sessionName = DATA_SESSION_NAME;
    std::string peerDevId = "testDevId";

    int32_t actual = softbusAdapter.RemoveSoftbusSessionServer(pkgname, sessionName, peerDevId);

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ILLEGAL_OPERATION, actual);
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

    EXPECT_EQ(ERR_DH_SCREEN_ADAPTER_OPEN_SESSION_FAIL, actual);
}

/**
 * @tc.name: OpenSoftbusSession_002
 * @tc.desc: Verify the OpenSoftbusSession function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, OpenSoftbusSession_002, TestSize.Level1)
{
    std::string mySessionName = DATA_SESSION_NAME;
    std::string peerSessionName = DATA_SESSION_NAME;
    std::string peerDevId = "testDevId";

    int32_t actual = softbusAdapter.OpenSoftbusSession(mySessionName, peerSessionName, peerDevId);

    EXPECT_EQ(ERR_DH_SCREEN_ADAPTER_OPEN_SESSION_FAIL, actual);
}

/**
 * @tc.name: CloseSoftbusSession_001
 * @tc.desc: Verify the CloseSoftbusSession function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, CloseSoftbusSession_001, TestSize.Level1)
{
    int32_t sessionId = 0;

    int32_t actual = softbusAdapter.CloseSoftbusSession(sessionId);

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

    std::shared_ptr<ISoftbusListener> actual = softbusAdapter.GetSoftbusListenerById(sessionId);

    EXPECT_EQ(nullptr, actual);
}

/**
 * @tc.name: GetSoftbusListener_002
 * @tc.desc: Verify the GetSoftbusListener function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(SoftbusAdapterTest, GetSoftbusListener_002, TestSize.Level1)
{
    int32_t sessionId = 0;

    SessionInfo sessionInfo;
    sessionInfo.sessionName = "hello";
    sessionInfo.peerDevId = "world";
    softbusAdapter.mapSessionInfos_[sessionId] = sessionInfo;
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
    int32_t sessionId = 0;
    int32_t result = 0;

    int32_t actual = softbusAdapter.OnSoftbusSessionOpened(sessionId, result);

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

    softbusAdapter.OnSoftbusSessionClosed(sessionId);
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

    SessionInfo sessionInfo;
    sessionInfo.sessionName = "hello";
    sessionInfo.peerDevId = "world";
    softbusAdapter.mapSessionInfos_[sessionId] = sessionInfo;
    std::shared_ptr<ISoftbusListener> listener = nullptr;
    softbusAdapter.mapListeners_["hello_world"] = listener;

    softbusAdapter.OnSoftbusSessionClosed(sessionId);
}
} // DistributedHardware
} // OHOS