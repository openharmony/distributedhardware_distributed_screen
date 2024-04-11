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

#include "1.0/include/screen_manager_adapter_test.h"

#include "common/include/screen_manager_adapter.h"
#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_util.h"
#include "video_param.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
constexpr static uint32_t VIDEO_DATA_NUM = 480;
void DScreenManagerAdapterTest::SetUpTestCase(void) {}

void DScreenManagerAdapterTest::TearDownTestCase(void) {}

void DScreenManagerAdapterTest::SetUp(void) {}

void DScreenManagerAdapterTest::TearDown(void) {}

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

    infoInstance.processName = "DscreenMgrTest";
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

void DisablePermissionAccess(const uint64_t &tokenId)
{
    OHOS::Security::AccessToken::AccessTokenKit::DeleteToken(tokenId);
}

/**
 * @tc.name: CreateVirtualScreen_001
 * @tc.desc: Verify the CreateVirtualScreen function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerAdapterTest, CreateVirtualScreen_001, TestSize.Level1)
{
    std::string devId;
    std::string dhId;
    std::shared_ptr<VideoParam> videoParam = nullptr;
    uint64_t ret = ScreenMgrAdapter::GetInstance().CreateVirtualScreen(devId, dhId, videoParam);
    EXPECT_EQ(SCREEN_ID_INVALID, ret);
}

/**
 * @tc.name: CreateVirtualScreen_002
 * @tc.desc: Verify the CreateVirtualScreen function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerAdapterTest, CreateVirtualScreen_002, TestSize.Level1)
{
    std::string devId = "devId";
    std::string dhId = "dhId";
    std::shared_ptr<VideoParam> videoParam = std::make_shared<VideoParam>();
    std::string screenName = DSCREEN_PREFIX + SEPERATOR + GetInterruptString(devId) +
                             SEPERATOR + GetInterruptString(dhId);
    ScreenMgrAdapter::GetInstance().screenIdMap_.emplace(screenName, 100);
    uint64_t ret = ScreenMgrAdapter::GetInstance().CreateVirtualScreen(devId, dhId, videoParam);
    EXPECT_EQ(SCREEN_ID_INVALID, ret);
}

/**
 * @tc.name: CreateVirtualScreen_003
 * @tc.desc: Verify the CreateVirtualScreen function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerAdapterTest, CreateVirtualScreen_003, TestSize.Level1)
{
    std::string devId = "devId";
    std::string dhId = "dhId";
    ScreenMgrAdapter::GetInstance().screenIdMap_.clear();
    std::shared_ptr<VideoParam> videoParam = std::make_shared<VideoParam>();
    uint64_t ret = ScreenMgrAdapter::GetInstance().CreateVirtualScreen(devId, dhId, videoParam);
    EXPECT_EQ(1, ScreenMgrAdapter::GetInstance().screenIdMap_.size());

    ret = ScreenMgrAdapter::GetInstance().CreateVirtualScreen(devId, dhId, videoParam);
    EXPECT_EQ(1, ScreenMgrAdapter::GetInstance().screenIdMap_.size());
    ScreenMgrAdapter::GetInstance().RemoveVirtualScreen(ret);
    ScreenMgrAdapter::GetInstance().screenIdMap_.clear();
}

/**
 * @tc.name: RegisterScreenGroupListener_001
 * @tc.desc: Verify the RegisterScreenGroupListener function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerAdapterTest, RegisterScreenGroupListener_001, TestSize.Level1)
{
    sptr<Rosen::ScreenManager::IScreenGroupListener> listener = nullptr;
    int32_t ret = ScreenMgrAdapter::GetInstance().RegisterScreenGroupListener(listener);
    EXPECT_EQ(ERR_DH_SCREEN_SA_REGISTER_SCREENLISTENER_FAIL, ret);
}

/**
 * @tc.name: RegisterScreenGroupListener_002
 * @tc.desc: Verify the RegisterScreenGroupListener function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerAdapterTest, RegisterScreenGroupListener_002, TestSize.Level1)
{
    sptr<Rosen::ScreenManager::IScreenGroupListener> listener = new MockScreenGroupListener();
    ScreenMgrAdapter::GetInstance().listenerRegistered_ = true;
    int32_t ret = ScreenMgrAdapter::GetInstance().RegisterScreenGroupListener(listener);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: RegisterScreenGroupListener_003
 * @tc.desc: Verify the RegisterScreenGroupListener function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerAdapterTest, RegisterScreenGroupListener_003, TestSize.Level1)
{
    sptr<Rosen::ScreenManager::IScreenGroupListener> listener = new MockScreenGroupListener();
    ScreenMgrAdapter::GetInstance().listenerRegistered_ = false;
    int32_t ret = ScreenMgrAdapter::GetInstance().RegisterScreenGroupListener(listener);
    EXPECT_EQ(DH_SUCCESS, ret);
    ScreenMgrAdapter::GetInstance().UnregisterScreenGroupListener(listener);
}

/**
 * @tc.name: UnregisterScreenGroupListener_001
 * @tc.desc: Verify the UnregisterScreenGroupListener function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerAdapterTest, UnregisterScreenGroupListener_001, TestSize.Level1)
{
    sptr<Rosen::ScreenManager::IScreenGroupListener> listener = nullptr;
    int32_t ret = ScreenMgrAdapter::GetInstance().UnregisterScreenGroupListener(listener);
    ScreenMgrAdapter::GetInstance().RemoveScreenFromGroup(100);
    EXPECT_EQ(ERR_DH_SCREEN_SA_UNREGISTER_SCREENLISTENER_FAIL, ret);
}

/**
 * @tc.name: UnregisterScreenGroupListener_002
 * @tc.desc: Verify the UnregisterScreenGroupListener function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerAdapterTest, UnregisterScreenGroupListener_002, TestSize.Level1)
{
    sptr<Rosen::ScreenManager::IScreenGroupListener> listener = new MockScreenGroupListener();
    ScreenMgrAdapter::GetInstance().listenerRegistered_ = false;
    int32_t ret = ScreenMgrAdapter::GetInstance().UnregisterScreenGroupListener(listener);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: UnregisterScreenGroupListener_003
 * @tc.desc: Verify the UnregisterScreenGroupListener function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerAdapterTest, UnregisterScreenGroupListener_003, TestSize.Level1)
{
    sptr<Rosen::ScreenManager::IScreenGroupListener> listener = new MockScreenGroupListener();
    ScreenMgrAdapter::GetInstance().listenerRegistered_ = true;
    int32_t ret = ScreenMgrAdapter::GetInstance().UnregisterScreenGroupListener(listener);
    EXPECT_EQ(ERR_DH_SCREEN_SA_UNREGISTER_SCREENLISTENER_FAIL, ret);
}

/**
 * @tc.name: RemoveVirtualScreen_001
 * @tc.desc: Verify the RemoveVirtualScreen function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerAdapterTest, RemoveVirtualScreen_001, TestSize.Level1)
{
    int32_t ret = ScreenMgrAdapter::GetInstance().RemoveVirtualScreen(100);
    EXPECT_EQ(ERR_DH_SCREEN_SA_REMOVE_VIRTUALSCREEN_FAIL, ret);
}

/**
 * @tc.name: SetImageSurface_001
 * @tc.desc: Verify the SetImageSurface function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerAdapterTest, SetImageSurface_001, TestSize.Level1)
{
    sptr<OHOS::Surface> surface = nullptr;
    uint64_t screenId = 0;
    int32_t ret = ScreenMgrAdapter::GetInstance().SetImageSurface(screenId, surface);
    EXPECT_EQ(ERR_DH_SCREEN_SA_SET_IMAGESURFACE_FAIL, ret);
}

/**
 * @tc.name: GetMapRelation_001
 * @tc.desc: Verify the GetMapRelation function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerAdapterTest, GetMapRelation_001, TestSize.Level1)
{
    std::shared_ptr<DScreenMapRelation> ret = ScreenMgrAdapter::GetInstance().GetMapRelation(100000);
    EXPECT_EQ(nullptr, ret);
}

/**
 * @tc.name: GetMapRelation_002
 * @tc.desc: Verify the GetMapRelation function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenManagerAdapterTest, GetMapRelation_002, TestSize.Level1)
{
    const char* perms[] = {
        "ohos.permission.DISTRIBUTED_DATASYNC",
        "ohos.permission.CAPTURE_SCREEN",
    };
    EnablePermissionAccess(perms, sizeof(perms) / sizeof(perms[0]), tokenId_);
    std::string devId = "devId";
    std::string dhId = "dhId";
    ScreenMgrAdapter::GetInstance().screenIdMap_.clear();
    std::shared_ptr<VideoParam> videoParam = std::make_shared<VideoParam>();
    videoParam->SetScreenWidth(VIDEO_DATA_NUM);
    videoParam->SetScreenHeight(VIDEO_DATA_NUM);
    uint64_t screenId = ScreenMgrAdapter::GetInstance().CreateVirtualScreen(devId, dhId, videoParam);
    std::shared_ptr<DScreenMapRelation> ret = ScreenMgrAdapter::GetInstance().GetMapRelation(screenId);
    ScreenMgrAdapter::GetInstance().RemoveVirtualScreen(screenId);
    ScreenMgrAdapter::GetInstance().screenIdMap_.clear();
    DisablePermissionAccess(tokenId_);
    EXPECT_NE(nullptr, ret);
}
}
}