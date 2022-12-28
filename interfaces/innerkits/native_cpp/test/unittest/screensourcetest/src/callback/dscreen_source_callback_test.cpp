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

#define private public
#include "dscreen_source_callback_test.h"
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void DScreenSourceCallbackTest::SetUpTestCase(void) {}

void DScreenSourceCallbackTest::TearDownTestCase(void) {}

void DScreenSourceCallbackTest::SetUp(void)
{
    dScreenSourceCallback_ = new DScreenSourceCallback();
}

void DScreenSourceCallbackTest::TearDown(void)
{
    dScreenSourceCallback_ = nullptr;
}

/**
 * @tc.name: OnNotifyRegResult_001
 * @tc.desc: Verify the OnNotifyRegResult function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceCallbackTest, OnNotifyRegResult_001, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    const std::string reqId = "reqIdReg";
    int32_t status = 0;
    const std::string data = "data";
    std::shared_ptr<RegisterCallback> callback = std::make_shared<RegisterCallbackTest>();
    dScreenSourceCallback_->PushRegRegisterCallback(reqId, callback);
    int32_t ret = dScreenSourceCallback_->OnNotifyRegResult(devId, dhId, reqId, status, data);
    dScreenSourceCallback_->PopRegRegisterCallback(reqId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: OnNotifyRegResult_002
 * @tc.desc: Verify the OnNotifyRegResult function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceCallbackTest, OnNotifyRegResult_002, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    const std::string reqId = "reqId";
    int32_t status = 0;
    const std::string data = "data";
    int32_t ret = dScreenSourceCallback_->OnNotifyRegResult(devId, dhId, reqId, status, data);
    EXPECT_EQ(ERR_DH_SCREEN_SA_REGISTERCALLBACK_NOT_FOUND, ret);
}

/**
 * @tc.name: OnNotifyRegResult_003
 * @tc.desc: Verify the OnNotifyRegResult function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceCallbackTest, OnNotifyRegResult_003, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    const std::string reqId = "reqIdReg";
    int32_t status = 0;
    const std::string data = "data";
    std::shared_ptr<RegisterCallback> callback = nullptr;
    dScreenSourceCallback_->PushRegRegisterCallback(reqId, callback);
    int32_t ret = dScreenSourceCallback_->OnNotifyRegResult(devId, dhId, reqId, status, data);
    dScreenSourceCallback_->PopRegRegisterCallback(reqId);
    EXPECT_EQ(ERR_DH_SCREEN_SA_REGISTERCALLBACK_NOT_FOUND, ret);
}

/**
 * @tc.name: OnNotifyUnregResult_001
 * @tc.desc: Verify the OnNotifyUnregResult function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceCallbackTest, OnNotifyUnregResult_001, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    const std::string reqId = "reqIdUnreg";
    int32_t status = 0;
    const std::string data = "data";
    std::shared_ptr<UnregisterCallback> callback = std::make_shared<UnregisterCallbackTest>();
    dScreenSourceCallback_->PushUnregisterCallback(reqId, callback);
    int32_t ret = dScreenSourceCallback_->OnNotifyUnregResult(devId, dhId, reqId, status, data);
    dScreenSourceCallback_->PopUnregisterCallback(reqId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: OnNotifyUnregResult_002
 * @tc.desc: Verify the OnNotifyUnregResult function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceCallbackTest, OnNotifyUnregResult_002, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    const std::string reqId = "reqId";
    int32_t status = 0;
    const std::string data = "data";
    int32_t ret = dScreenSourceCallback_->OnNotifyUnregResult(devId, dhId, reqId, status, data);
    EXPECT_EQ(ERR_DH_SCREEN_SA_UNREGISTERCALLBACK_NOT_FOUND, ret);
}

/**
 * @tc.name: OnNotifyUnregResult_003
 * @tc.desc: Verify the OnNotifyUnregResult function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceCallbackTest, OnNotifyUnregResult_003, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    const std::string reqId = "reqIdUnreg";
    int32_t status = 0;
    const std::string data = "data";
    std::shared_ptr<UnregisterCallback> callback = nullptr;
    dScreenSourceCallback_->PushUnregisterCallback(reqId, callback);
    int32_t ret = dScreenSourceCallback_->OnNotifyUnregResult(devId, dhId, reqId, status, data);
    dScreenSourceCallback_->PopUnregisterCallback(reqId);
    EXPECT_EQ(ERR_DH_SCREEN_SA_UNREGISTERCALLBACK_NOT_FOUND, ret);
}

/**
 * @tc.name: PushRegRegisterCallback_001
 * @tc.desc: Verify the PushRegRegisterCallback function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceCallbackTest, PushRegRegisterCallback_001, TestSize.Level1)
{
    const std::string reqId = "reqIdReg";
    std::shared_ptr<RegisterCallback> callback = std::make_shared<RegisterCallbackTest>();
    int32_t sizeFront = dScreenSourceCallback_->registerCallbackMap_.size();
    dScreenSourceCallback_->PushRegRegisterCallback(reqId, callback);
    int32_t sizeEnd = dScreenSourceCallback_->registerCallbackMap_.size();
    dScreenSourceCallback_->PopRegRegisterCallback(reqId);
    EXPECT_GT(sizeEnd, sizeFront);
}

/**
 * @tc.name: PopRegRegisterCallback_001
 * @tc.desc: Verify the PopRegRegisterCallback function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceCallbackTest, PopRegRegisterCallback_001, TestSize.Level1)
{
    const std::string reqId = "reqId";
    std::shared_ptr<RegisterCallback> callback = std::make_shared<RegisterCallbackTest>();
    dScreenSourceCallback_->PushRegRegisterCallback(reqId, callback);
    int32_t sizeFront = dScreenSourceCallback_->registerCallbackMap_.size();
    dScreenSourceCallback_->PopRegRegisterCallback(reqId);
    int32_t sizeEnd = dScreenSourceCallback_->registerCallbackMap_.size();
    EXPECT_GT(sizeFront, sizeEnd);
}


/**
 * @tc.name: PushRegRegisterCallback_001
 * @tc.desc: Verify the PushRegRegisterCallback function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceCallbackTest, PushUnregRegisterCallback_001, TestSize.Level1)
{
    const std::string reqId = "reqIdUnreg";
    std::shared_ptr<UnregisterCallback> callback = std::make_shared<UnregisterCallbackTest>();
    int32_t sizeFront = dScreenSourceCallback_->unregisterCallbackMap_.size();
    dScreenSourceCallback_->PushUnregisterCallback(reqId, callback);
    int32_t sizeEnd = dScreenSourceCallback_->unregisterCallbackMap_.size();
    dScreenSourceCallback_->PopUnregisterCallback(reqId);
    EXPECT_GT(sizeEnd, sizeFront);
}

/**
 * @tc.name: PopRegRegisterCallback_001
 * @tc.desc: Verify the PopRegRegisterCallback function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceCallbackTest, PopUnregRegisterCallback_001, TestSize.Level1)
{
    const std::string reqId = "reqId";
    std::shared_ptr<UnregisterCallback> callback = std::make_shared<UnregisterCallbackTest>();
    dScreenSourceCallback_->PushUnregisterCallback(reqId, callback);
    int32_t sizeFront = dScreenSourceCallback_->unregisterCallbackMap_.size();
    dScreenSourceCallback_->PopUnregisterCallback(reqId);
    int32_t sizeEnd = dScreenSourceCallback_->unregisterCallbackMap_.size();
    EXPECT_GT(sizeFront, sizeEnd);
}
}
}