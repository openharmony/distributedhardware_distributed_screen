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

#include "screenregionmgr_test.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void ScreenRegionManagerTest::SetUpTestCase(void) {}

void ScreenRegionManagerTest::TearDownTestCase(void) {}

void ScreenRegionManagerTest::SetUp(void) {}

void ScreenRegionManagerTest::TearDown(void)
{
    sleep(1);
}

/**
 * @tc.name: ReleaseAllRegions_001
 * @tc.desc: Verify the ReleaseAllRegions function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionManagerTest, ReleaseAllRegions_001, TestSize.Level1)
{
    const std::string remoteDevId = "sourceDevId";
    uint64_t screenId = 0;
    uint64_t displayId = 0;
    std::shared_ptr<ScreenRegion> screenRegion = std::make_shared<ScreenRegion>(remoteDevId, screenId, displayId);
    ScreenRegionManager::GetInstance().screenRegions_[remoteDevId] = screenRegion;
    int32_t ret = ScreenRegionManager::GetInstance().ReleaseAllRegions();

    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: ReleaseAllRegions_002
 * @tc.desc: Verify the ReleaseAllRegions function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionManagerTest, ReleaseAllRegions_002, TestSize.Level1)
{
    const std::string remoteDevId = "sourceDevId";
    std::shared_ptr<ScreenRegion> screenRegion = nullptr;
    ScreenRegionManager::GetInstance().screenRegions_[remoteDevId] = screenRegion;
    int32_t ret = ScreenRegionManager::GetInstance().ReleaseAllRegions();

    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: NotifyRemoteScreenService_001
 * @tc.desc: Verify the NotifyRemoteScreenService function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionManagerTest, NotifyRemoteScreenService_001, TestSize.Level1)
{
    const std::string remoteDevId = "sourceDevId";
    const std::string dhId = "sinkDhId";
    int32_t eventCode = 0;
    const std::string eventContent = "sourceEvent";
    int32_t ret =
        ScreenRegionManager::GetInstance().NotifyRemoteScreenService(remoteDevId, dhId, eventCode, eventContent);

    EXPECT_EQ(ERR_DH_SCREEN_SA_GET_REMOTE_SOURCE_SERVICE_FAIL, ret);
}

/**
 * @tc.name: HandleNotifySetUp_001
 * @tc.desc: Verify the HandleNotifySetUp function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionManagerTest, HandleNotifySetUp_001, TestSize.Level1)
{
    ScreenRegionManager::GetInstance().screenRegions_.clear();
    const std::string remoteDevId = "sourceDevId";
    const std::string eventContent = "{\"dhId\":\"SCREEN#0\",\"mapRelation\":{\"displayId\":0,\
        \"displayRect\":{\"height\":1280,\"startX\":0,\"startY\":0,\"width\":720},\"screenId\":2,\
        \"screenRect\":{\"height\":1280,\"startX\":0,\"startY\":0,\"width\":720}},\"screenId\":2,\
        \"videoParam\":{\"codecType\":2,\"colorFormat\":3,\"fps\":30,\"screenHeight\":1280,\
        \"screenWidth\":720,\"videoHeight\":1280,\"videoWidth\":720}}";
    ScreenRegionManager::GetInstance().HandleNotifySetUp(remoteDevId, eventContent);

    EXPECT_NE(0, ScreenRegionManager::GetInstance().screenRegions_.size());
    ScreenRegionManager::GetInstance().screenRegions_.clear();
}

/**
 * @tc.name: HandleNotifySetUp_002
 * @tc.desc: Verify the HandleNotifySetUp function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionManagerTest, HandleNotifySetUp_002, TestSize.Level1)
{
    ScreenRegionManager::GetInstance().screenRegions_.clear();
    const std::string remoteDevId = "sourceDevId";
    const std::string eventContent = "{\"dhIds\":\"SCREEN#0\",\"mapRelations\":{\"displayIds\":0,\
        \"displayRects\":{\"heights\":1280,\"startXs\":0,\"startYs\":0,\"widths\":720},\"screenIds\":2,\
        \"screenRects\":{\"heights\":1280,\"startXs\":0,\"startYs\":0,\"widths\":720}},\"screenIds\":2,\
        \"videoParams\":{\"codecTypes\":2,\"colorFormats\":3,\"fpss\":30,\"screenHeights\":1280,\
        \"screenWidths\":720,\"videoHeights\":1280,\"videoWidths\":720}}";
    ScreenRegionManager::GetInstance().HandleNotifySetUp(remoteDevId, eventContent);

    EXPECT_EQ(0, ScreenRegionManager::GetInstance().screenRegions_.size());
}

/**
 * @tc.name: GetDScreenSourceSA_001
 * @tc.desc: Verify the GetDScreenSourceSA function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionManagerTest, GetDScreenSourceSA_001, TestSize.Level1)
{
    const std::string remoteDevId = "sourceDevId";
    sptr<IDScreenSource> ret = ScreenRegionManager::GetInstance().GetDScreenSourceSA(remoteDevId);

    EXPECT_EQ(nullptr, ret);
}
}
}