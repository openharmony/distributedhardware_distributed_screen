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
#include "dscreen_source_handler_test.h"
#include "if_system_ability_manager.h"
#include "dscreen_source_load_callback.h"
#include "iservice_registry.h"
#include "dscreen_constants.h"
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void DScreenSourceHandlerTest::SetUpTestCase(void) {}

void DScreenSourceHandlerTest::TearDownTestCase(void) {}

void DScreenSourceHandlerTest::SetUp(void)
{
    DScreenSourceHandler::GetInstance().InitSource("DScreenSourceHandlerTest");
}

void DScreenSourceHandlerTest::TearDown(void)
{
    DScreenSourceHandler::GetInstance().ReleaseSource();
}

/**
 * @tc.name: InitSource_001
 * @tc.desc: Verify the InitSource function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, InitSource_001, TestSize.Level1)
{
    std::string params = "";
    int32_t ret = DScreenSourceHandler::GetInstance().InitSource(params);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);
}

/**
 * @tc.name: InitSource_002
 * @tc.desc: Verify the InitSource function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, InitSource_002, TestSize.Level1)
{
    std::string params = "DScreenSourceHandlerTestDScreenSourceHandlerTestDScreenSourceHandlerTest \
        DScreenSourceHandlerTestDScreenSourceHandlerTestDScreenSourceHandlerTestDScreenSourceHandlerTest \
        DScreenSourceHandlerTestDScreenSourceHandlerTestDScreenSourceHandlerTestDScreenSourceHandlerTest \
        DScreenSourceHandlerTestDScreenSourceHandlerTestDScreenSourceHandlerTestDScreenSourceHandlerTest";
    int32_t ret = DScreenSourceHandler::GetInstance().InitSource(params);
    EXPECT_EQ(DH_SUCCESS, ret);
}


/**
 * @tc.name: RegisterDistributedHardware_001
 * @tc.desc: Verify the RegisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, RegisterDistributedHardware_001, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    EnableParam param;
    param.version = "1";
    param.attrs = "attrs";
    std::string callbackParam = "callbackParam";
    sptr<DScreenSourceLoadCallback> loadCallback = new DScreenSourceLoadCallback(callbackParam);
    loadCallback->OnLoadSystemAbilitySuccess(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, nullptr);
    std::shared_ptr<RegisterCallback> callback = std::make_shared<RegisterCallbackTest>();
    int32_t ret = DScreenSourceHandler::GetInstance().RegisterDistributedHardware(devId, dhId, param, callback);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: RegisterDistributedHardware_002
 * @tc.desc: Verify the RegisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, RegisterDistributedHardware_002, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    EnableParam param;
    param.version = "1";
    param.attrs = "attrs";
    std::string callbackParam = "callbackParam";
    sptr<DScreenSourceLoadCallback> loadCallback = new DScreenSourceLoadCallback(callbackParam);
    loadCallback->OnLoadSystemAbilitySuccess(DISTRIBUTED_HARDWARE_SCREEN_SINK_SA_ID, nullptr);
    std::shared_ptr<RegisterCallback> callback = std::make_shared<RegisterCallbackTest>();
    DScreenSourceHandler::GetInstance().dScreenSourceProxy_ = nullptr;
    int32_t ret = DScreenSourceHandler::GetInstance().RegisterDistributedHardware(devId, dhId, param, callback);
    EXPECT_EQ(ERR_DH_SCREEN_SA_SOURCEPROXY_NOT_INIT, ret);
}

/**
 * @tc.name: RegisterDistributedHardware_003
 * @tc.desc: Verify the RegisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, RegisterDistributedHardware_003, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    EnableParam param;
    param.version = "1";
    param.attrs = "attrs";
    std::shared_ptr<RegisterCallback> callback = std::make_shared<RegisterCallbackTest>();
    DScreenSourceHandler::GetInstance().dScreenSourceCallback_ = nullptr;
    int32_t ret = DScreenSourceHandler::GetInstance().RegisterDistributedHardware(devId, dhId, param, callback);
    EXPECT_EQ(ERR_DH_SCREEN_SA_SOURCEPCALLBACK_NOT_INIT, ret);
}

/**
 * @tc.name: RegisterDistributedHardware_004
 * @tc.desc: Verify the RegisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, RegisterDistributedHardware_004, TestSize.Level1)
{
    const std::string devId = "";
    const std::string dhId = "";
    EnableParam param;
    param.version = "1";
    param.attrs = "attrs";
    std::shared_ptr<RegisterCallback> callback = std::make_shared<RegisterCallbackTest>();
    int32_t ret = DScreenSourceHandler::GetInstance().RegisterDistributedHardware(devId, dhId, param, callback);
    EXPECT_EQ(ERR_DH_SCREEN_STRING_PARAM_EMPTY, ret);
}

/**
 * @tc.name: RegisterDistributedHardware_005
 * @tc.desc: Verify the RegisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, RegisterDistributedHardware_005, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    EnableParam param;
    param.version = "1";
    param.attrs = "attrs";
    std::shared_ptr<RegisterCallback> callback = nullptr;
    int32_t ret = DScreenSourceHandler::GetInstance().RegisterDistributedHardware(devId, dhId, param, callback);
    EXPECT_EQ(ERR_DH_SCREEN_REGISTER_CALLBACK_NOT_INIT, ret);
}

/**
 * @tc.name: RegisterDistributedHardware_006
 * @tc.desc: Verify the RegisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, RegisterDistributedHardware_006, TestSize.Level1)
{
    const std::string devId = "devIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevId \
        devIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevId \
        devIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevId \
        devIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevId";
    const std::string dhId = "dhId";
    EnableParam param;
    param.version = "1";
    param.attrs = "attrs";
    std::shared_ptr<RegisterCallback> callback = std::make_shared<RegisterCallbackTest>();
    if (DScreenSourceHandler::GetInstance().dScreenSourceCallback_ == nullptr) {
        DScreenSourceHandler::GetInstance().dScreenSourceCallback_ = new (std::nothrow) DScreenSourceCallback();
    }
    int32_t ret = DScreenSourceHandler::GetInstance().RegisterDistributedHardware(devId, dhId, param, callback);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);
}

/**
 * @tc.name: UnregisterDistributedHardware_001
 * @tc.desc: Verify the UnregisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, UnregisterDistributedHardware_001, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    std::shared_ptr<UnregisterCallbackTest> callback = std::make_shared<UnregisterCallbackTest>();
    if (DScreenSourceHandler::GetInstance().dScreenSourceCallback_ == nullptr) {
        DScreenSourceHandler::GetInstance().dScreenSourceCallback_ = new (std::nothrow) DScreenSourceCallback();
    }
    int32_t ret = DScreenSourceHandler::GetInstance().UnregisterDistributedHardware(devId, dhId, callback);
    EXPECT_EQ(ERR_DH_SCREEN_SA_DISABLE_FAILED, ret);
}

/**
 * @tc.name: UnregisterDistributedHardware_002
 * @tc.desc: Verify the UnregisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, UnregisterDistributedHardware_002, TestSize.Level1)
{
    const std::string devId = "";
    const std::string dhId = "";
    std::shared_ptr<UnregisterCallbackTest> callback = std::make_shared<UnregisterCallbackTest>();
    int32_t ret = DScreenSourceHandler::GetInstance().UnregisterDistributedHardware(devId, dhId, callback);
    EXPECT_EQ(ERR_DH_SCREEN_STRING_PARAM_EMPTY, ret);
}

/**
 * @tc.name: UnregisterDistributedHardware_003
 * @tc.desc: Verify the UnregisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, UnregisterDistributedHardware_003, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    std::shared_ptr<UnregisterCallbackTest> callback = nullptr;
    int32_t ret = DScreenSourceHandler::GetInstance().UnregisterDistributedHardware(devId, dhId, callback);
    EXPECT_EQ(ERR_DH_SCREEN_REGISTER_CALLBACK_NOT_INIT, ret);
}

/**
 * @tc.name: UnregisterDistributedHardware_004
 * @tc.desc: Verify the UnregisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, UnregisterDistributedHardware_004, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    std::shared_ptr<UnregisterCallbackTest> callback = std::make_shared<UnregisterCallbackTest>();
    DScreenSourceHandler::GetInstance().dScreenSourceCallback_ = nullptr;

    int32_t ret = DScreenSourceHandler::GetInstance().UnregisterDistributedHardware(devId, dhId, callback);
    EXPECT_EQ(ERR_DH_SCREEN_SA_SOURCEPCALLBACK_NOT_INIT, ret);
}

/**
 * @tc.name: UnregisterDistributedHardware_005
 * @tc.desc: Verify the UnregisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, UnregisterDistributedHardware_005, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    std::shared_ptr<UnregisterCallbackTest> callback = std::make_shared<UnregisterCallbackTest>();
    DScreenSourceHandler::GetInstance().dScreenSourceProxy_ = nullptr;

    int32_t ret = DScreenSourceHandler::GetInstance().UnregisterDistributedHardware(devId, dhId, callback);
    EXPECT_EQ(ERR_DH_SCREEN_SA_SOURCEPROXY_NOT_INIT, ret);
}

/**
 * @tc.name: UnregisterDistributedHardware_006
 * @tc.desc: Verify the UnregisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, UnregisterDistributedHardware_006, TestSize.Level1)
{
    const std::string devId = "devIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevId \
        devIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevId \
        devIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevId \
        devIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevIddevId";
    const std::string dhId = "dhId";
    std::shared_ptr<UnregisterCallbackTest> callback = std::make_shared<UnregisterCallbackTest>();
    if (DScreenSourceHandler::GetInstance().dScreenSourceCallback_ == nullptr) {
        DScreenSourceHandler::GetInstance().dScreenSourceCallback_ = new (std::nothrow) DScreenSourceCallback();
    }
    int32_t ret = DScreenSourceHandler::GetInstance().UnregisterDistributedHardware(devId, dhId, callback);
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, ret);
}

/**
 * @tc.name: ConfigDistributedHardware_001
 * @tc.desc: Verify the ConfigDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, ConfigDistributedHardware_001, TestSize.Level1)
{
    const std::string devId = "devId";
    const std::string dhId = "dhId";
    const std::string key = "key";
    const std::string value = "value";

    int32_t ret = DScreenSourceHandler::GetInstance().ConfigDistributedHardware(devId, dhId, key, value);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: ReleaseSource_001
 * @tc.desc: Verify the ReleaseSource function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, ReleaseSource_001, TestSize.Level1)
{
    DScreenSourceHandler::GetInstance().dScreenSourceProxy_ = nullptr;
    int32_t ret = DScreenSourceHandler::GetInstance().ReleaseSource();
    EXPECT_EQ(ERR_DH_SCREEN_SA_SOURCEPROXY_NOT_INIT, ret);
}

/**
 * @tc.name: OnRemoteDied_001
 * @tc.desc: Verify the OnRemoteDied function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceHandlerTest, OnRemoteDied_001, TestSize.Level1)
{
    sptr<ISystemAbilityManager> samgr =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();

    sptr<IRemoteObject> remoteObject = samgr->GetSystemAbility(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID);
    wptr<IRemoteObject> remote(remoteObject);
    DScreenSourceHandler::GetInstance().sourceSvrRecipient_->OnRemoteDied(remote);
    EXPECT_EQ(nullptr, DScreenSourceHandler::GetInstance().dScreenSourceProxy_);
}
}
}