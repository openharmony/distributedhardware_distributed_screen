/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "2.0/include/screenregionmgr_test.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
namespace V2_0 {
void ScreenRegionManagerTestV2::SetUpTestCase(void) {}

void ScreenRegionManagerTestV2::TearDownTestCase(void) {}

void ScreenRegionManagerTestV2::SetUp(void) {}

void ScreenRegionManagerTestV2::TearDown(void)
{
    sleep(1);
}

/**
 * @tc.name: Initialize_001
 * @tc.desc: Verify the Initialize function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionManagerTestV2, Initialize_001, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, ScreenRegionManager::GetInstance().Initialize());
    EXPECT_EQ(DH_SUCCESS, ScreenRegionManager::GetInstance().Release());
}

/**
 * @tc.name: CreateDScreenRegion_001
 * @tc.desc: Verify the CreateDScreenRegion function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionManagerTestV2, CreateDScreenRegion_001, TestSize.Level1)
{
    std::string peerDevId = "peerDevId";
    int32_t ret =ScreenRegionManager::GetInstance().CreateDScreenRegion(peerDevId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: DestoryDScreenRegion_001
 * @tc.desc: Verify the DestoryDScreenRegion function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionManagerTestV2, DestoryDScreenRegion_001, TestSize.Level1)
{
    std::string peerDevId = "peerDevId";
    int32_t ret =ScreenRegionManager::GetInstance().DestoryDScreenRegion(peerDevId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: GetScreenDumpInfo_001
 * @tc.desc: Verify the GetScreenDumpInfo function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionManagerTestV2, GetScreenDumpInfo_001, TestSize.Level1)
{
    std::string result;
    ScreenRegionManager::GetInstance().GetScreenDumpInfo(result);
    EXPECT_EQ(false, result.empty());
}

/**
 * @tc.name: GetScreenDumpInfo_002
 * @tc.desc: Verify the GetScreenDumpInfo function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionManagerTestV2, GetScreenDumpInfo_002, TestSize.Level1)
{
    std::string result;
    ScreenRegionManager::GetInstance().GetScreenDumpInfo(result);
    EXPECT_EQ(false, result.empty());
}

/**
 * @tc.name: OnProviderEvent_002
 * @tc.desc: Verify the OnProviderEvent function failed.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenRegionManagerTestV2, OnProviderEvent_002, TestSize.Level1)
{
    AVTransEvent event;
    EngineProviderListener engineProviderListener;
    int32_t ret = engineProviderListener.OnProviderEvent(event);
    EXPECT_EQ(DH_SUCCESS, ret);
}
}
}
}