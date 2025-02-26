/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <memory>

#include "dscreen_source_service_test.h"
#include "1.0/include/dscreen_manager.h"
#include "2.0/include/dscreen_manager.h"
#include "accesstoken_kit.h"
#include "dscreen_constants.h"
#include "dscreen_source_callback.h"
#include "dscreen_source_service.h"
#include "idscreen_source.h"
#include "system_ability.h"

using namespace testing;
using namespace testing::ext;

static int32_t g_mockEnableDistributedScreenReturnInt32Value = -1;
static int32_t g_mockUnregisterDistributedHardwareV1ReturnInt32Value = -1;
static int32_t g_mockUnregisterDistributedHardwareV2ReturnInt32Value = -1;
static int32_t g_mockUnInitV1ReturnInt32Value = -1;
static int32_t g_mockReleaseV2ReturnInt32Value = -1;
static int32_t g_mockInitV1ReturnInt32Value = -1;
static int32_t g_mockInitializeV2ReturnInt32Value = -1;

namespace OHOS {
namespace DistributedHardware {
int32_t V1_0::DScreenManager::EnableDistributedScreen(const std::string &devId, const std::string &dhId,
    const EnableParam &param, const std::string &reqId)
{
    return g_mockEnableDistributedScreenReturnInt32Value;
}

int32_t V2_0::DScreenManager::EnableDistributedScreen(const std::string &devId, const std::string &dhId,
    const EnableParam &param, const std::string &reqId)
{
    return g_mockEnableDistributedScreenReturnInt32Value;
}

int32_t V1_0::DScreenManager::DisableDistributedScreen(const std::string &devId,
    const std::string &dhId, const std::string &reqId)
{
    return g_mockUnregisterDistributedHardwareV1ReturnInt32Value;
}

int32_t V2_0::DScreenManager::DisableDistributedScreen(const std::string &devId,
    const std::string &dhId, const std::string &reqId)
{
    return g_mockUnregisterDistributedHardwareV2ReturnInt32Value;
}

int32_t V1_0::DScreenManager::Init()
{
    return g_mockInitV1ReturnInt32Value;
}

int32_t V2_0::DScreenManager::Initialize()
{
    return g_mockInitializeV2ReturnInt32Value;
}

int32_t V1_0::DScreenManager::UnInit()
{
    return g_mockUnInitV1ReturnInt32Value;
}

int32_t V2_0::DScreenManager::Release()
{
    return g_mockReleaseV2ReturnInt32Value;
}

void V1_0::DScreenManager::RegisterDScreenCallback(const sptr<IDScreenSourceCallback> &callback) {}

void V2_0::DScreenManager::RegisterDScreenCallback(const sptr<IDScreenSourceCallback> &callback) {}

void DScreenSourceServiceTest::SetUpTestCase(void) {}

void DScreenSourceServiceTest::TearDownTestCase(void) {}

void DScreenSourceServiceTest::SetUp(void)
{
    g_mockEnableDistributedScreenReturnInt32Value = -1;
    g_mockUnregisterDistributedHardwareV1ReturnInt32Value = -1;
    g_mockUnregisterDistributedHardwareV2ReturnInt32Value = -1;
    g_mockUnInitV1ReturnInt32Value = -1;
    g_mockReleaseV2ReturnInt32Value = -1;
    g_mockInitV1ReturnInt32Value = -1;
    g_mockInitializeV2ReturnInt32Value = -1;
}

void DScreenSourceServiceTest::TearDown(void) {}

/* *
 * @tc.name: OnStart_001
 * @tc.desc: Verify the OnStart function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceServiceTest, OnStart_001, TestSize.Level1)
{
    std::shared_ptr<DScreenSourceService> sourceService =
        std::make_shared<DScreenSourceService>(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, false);
    sourceService->registerToService_ = true;
    sourceService->OnStart();
    sourceService->OnStop();
    EXPECT_FALSE(sourceService->registerToService_);
}

/* *
 * @tc.name: OnStop_001
 * @tc.desc: Verify the OnStop function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceServiceTest, OnStop_001, TestSize.Level1)
{
    std::shared_ptr<DScreenSourceService> sourceService =
        std::make_shared<DScreenSourceService>(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, false);
    sourceService->registerToService_ = true;
    g_mockReleaseV2ReturnInt32Value = 0;
    g_mockUnInitV1ReturnInt32Value = 0;
    sourceService->OnStop();
    EXPECT_FALSE(sourceService->registerToService_);
}

/* *
 * @tc.name: InitSource_001
 * @tc.desc: Verify the InitSource function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceServiceTest, InitSource_001, TestSize.Level1)
{
    std::shared_ptr<DScreenSourceService> sourceService =
        std::make_shared<DScreenSourceService>(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, false);
    sptr<IDScreenSourceCallback> callback = nullptr;
    std::string params;
    int32_t ret = sourceService->InitSource(params, callback);
    EXPECT_EQ(ret, ERR_DH_SCREEN_SA_INIT_SOURCE_FAIL);
    sourceService->ReleaseSource();
}

/* *
 * @tc.name: InitSource_002
 * @tc.desc: Verify the InitSource function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceServiceTest, InitSource_002, TestSize.Level1)
{
    std::shared_ptr<DScreenSourceService> sourceService =
        std::make_shared<DScreenSourceService>(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, false);
    std::string params;
    g_mockReleaseV2ReturnInt32Value = 0;
    g_mockUnInitV1ReturnInt32Value = 0;
    sptr<IDScreenSourceCallback> callback(new DScreenSourceCallback());
    int32_t ret = sourceService->InitSource(params, callback);
    EXPECT_EQ(ret, ERR_DH_SCREEN_SA_INIT_SOURCE_FAIL);
    sourceService->ReleaseSource();
}

/* *
 * @tc.name: InitSource_003
 * @tc.desc: Verify the InitSource function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceServiceTest, InitSource_003, TestSize.Level1)
{
    std::shared_ptr<DScreenSourceService> sourceService =
        std::make_shared<DScreenSourceService>(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, false);
    std::string params;
    g_mockInitializeV2ReturnInt32Value = 0;
    g_mockInitV1ReturnInt32Value = -1;
    sptr<IDScreenSourceCallback> callback(new DScreenSourceCallback());
    int32_t ret = sourceService->InitSource(params, callback);
    EXPECT_EQ(ret, ERR_DH_SCREEN_SA_INIT_SOURCE_FAIL);

    g_mockInitV1ReturnInt32Value = 0;
    ret = sourceService->InitSource(params, callback);
    EXPECT_EQ(ret, DH_SUCCESS);
    sourceService->ReleaseSource();
}

/* *
 * @tc.name: RegisterDistributedHardware_001
 * @tc.desc: Verify the RegisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceServiceTest, RegisterDistributedHardware_001, TestSize.Level1)
{
    std::shared_ptr<DScreenSourceService> sourceService =
        std::make_shared<DScreenSourceService>(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, false);
    g_mockEnableDistributedScreenReturnInt32Value = DH_SUCCESS;
    std::string devId;
    std::string dhId;
    EnableParam param = { "", "", "1.0", "attrs000" };
    std::string reqId;
    int32_t ret = sourceService->RegisterDistributedHardware(devId, dhId, param, reqId);
    EXPECT_EQ(ret, DH_SUCCESS);
}

/* *
 * @tc.name: RegisterDistributedHardware_002
 * @tc.desc: Verify the RegisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceServiceTest, RegisterDistributedHardware_002, TestSize.Level1)
{
    std::shared_ptr<DScreenSourceService> sourceService =
        std::make_shared<DScreenSourceService>(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, false);
    std::string devId;
    std::string dhId;
    EnableParam param = { "", "", "3.0", "attrs000" };
    std::string reqId;
    int32_t ret = sourceService->RegisterDistributedHardware(devId, dhId, param, reqId);
    EXPECT_EQ(ret, ERR_DH_SCREEN_SA_ENABLE_FAILED);
}

/* *
 * @tc.name: UnregisterDistributedHardware_001
 * @tc.desc: Verify the UnregisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceServiceTest, UnregisterDistributedHardware_001, TestSize.Level1)
{
    std::shared_ptr<DScreenSourceService> sourceService =
        std::make_shared<DScreenSourceService>(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, false);
    std::string devId;
    std::string dhId;
    std::string reqId;
    g_mockUnregisterDistributedHardwareV1ReturnInt32Value = -1;
    int32_t ret = sourceService->UnregisterDistributedHardware(devId, dhId, reqId);
    EXPECT_EQ(ret, ERR_DH_SCREEN_SA_DISABLE_FAILED);
}

/* *
 * @tc.name: UnregisterDistributedHardware_002
 * @tc.desc: Verify the UnregisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceServiceTest, UnregisterDistributedHardware_002, TestSize.Level1)
{
    std::shared_ptr<DScreenSourceService> sourceService =
        std::make_shared<DScreenSourceService>(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, false);
    std::string devId;
    std::string dhId;
    std::string reqId;
    g_mockUnregisterDistributedHardwareV1ReturnInt32Value = 0;
    g_mockUnregisterDistributedHardwareV2ReturnInt32Value = -1;
    int32_t ret = sourceService->UnregisterDistributedHardware(devId, dhId, reqId);
    EXPECT_EQ(ret, ERR_DH_SCREEN_SA_DISABLE_FAILED);
}

/* *
 * @tc.name: UnregisterDistributedHardware_003
 * @tc.desc: Verify the UnregisterDistributedHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSourceServiceTest, UnregisterDistributedHardware_003, TestSize.Level1)
{
    std::shared_ptr<DScreenSourceService> sourceService =
        std::make_shared<DScreenSourceService>(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, false);
    std::string devId;
    std::string dhId;
    std::string reqId;
    g_mockUnregisterDistributedHardwareV1ReturnInt32Value = 0;
    g_mockUnregisterDistributedHardwareV2ReturnInt32Value = 0;
    int32_t ret = sourceService->UnregisterDistributedHardware(devId, dhId, reqId);
    EXPECT_EQ(ret, DH_SUCCESS);
}
} // namespace DistributedHardware
} // namespace OHOS
