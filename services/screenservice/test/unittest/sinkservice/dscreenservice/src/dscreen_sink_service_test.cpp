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

#include "dscreen_sink_service_test.h"
#include "1.0/include/screenregionmgr.h"
#include "2.0/include/screenregionmgr.h"
#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_sink_service.h"
#include "idscreen_sink.h"

using namespace testing;
using namespace testing::ext;

static int32_t g_mockInitializeV2ReturnInt32Value = -1;
static int32_t g_mockReleaseV2ReturnInt32Value = -1;

namespace OHOS {
namespace DistributedHardware {
int32_t V2_0::ScreenRegionManager::Initialize()
{
    return g_mockInitializeV2ReturnInt32Value;
}

int32_t V2_0::ScreenRegionManager::Release()
{
    return g_mockReleaseV2ReturnInt32Value;
}

void DScreenSinkServiceTest::SetUpTestCase(void) {}

void DScreenSinkServiceTest::TearDownTestCase(void) {}

void DScreenSinkServiceTest::SetUp(void)
{
    g_mockInitializeV2ReturnInt32Value = -1;
    g_mockReleaseV2ReturnInt32Value = -1;
}

void DScreenSinkServiceTest::TearDown(void) {}

/* *
 * @tc.name: OnStart_001
 * @tc.desc: Verify the OnStart function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkServiceTest, OnStart_001, TestSize.Level1)
{
    std::shared_ptr<DScreenSinkService> sinkService =
        std::make_shared<DScreenSinkService>(DISTRIBUTED_HARDWARE_SCREEN_SINK_SA_ID, false);
    sinkService->registerToService_ = true;
    sinkService->OnStart();
    sinkService->OnStop();
    EXPECT_FALSE(sinkService->registerToService_);
}

/* *
 * @tc.name: InitSink_001
 * @tc.desc: Verify the InitSink function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkServiceTest, InitSink_001, TestSize.Level1)
{
    std::shared_ptr<DScreenSinkService> sinkService =
        std::make_shared<DScreenSinkService>(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, false);
    std::string params;
    int32_t ret = sinkService->InitSink(params);
    EXPECT_EQ(ret, DH_SUCCESS);
    sinkService->ReleaseSink();
}

/* *
 * @tc.name: InitSink_002
 * @tc.desc: Verify the InitSink function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkServiceTest, InitSink_002, TestSize.Level1)
{
    std::shared_ptr<DScreenSinkService> sinkService =
        std::make_shared<DScreenSinkService>(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, false);
    std::string params;
    g_mockInitializeV2ReturnInt32Value = 0;
    g_mockReleaseV2ReturnInt32Value = 0;
    int32_t ret = sinkService->InitSink(params);
    EXPECT_EQ(ret, DH_SUCCESS);
    sinkService->ReleaseSink();
}
} // namespace DistributedHardware
} // namespace OHOS
