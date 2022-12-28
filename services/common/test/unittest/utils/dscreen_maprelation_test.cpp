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
#include "dscreen_maprelation_test.h"
#undef private

using json = nlohmann::json;

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void DScreenMapRelationTest::SetUpTestCase(void) {}

void DScreenMapRelationTest::TearDownTestCase(void) {}

void DScreenMapRelationTest::SetUp()
{
    dscreenMapRelation = std::make_shared<DScreenMapRelation>();
}

void DScreenMapRelationTest::TearDown() {}

/**
 * @tc.name: GetDisplayId_001
 * @tc.desc: Verify the GetDisplayId function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenMapRelationTest, GetDisplayId_001, TestSize.Level1)
{
    uint64_t displayId = 0;
    dscreenMapRelation->SetDisplayId(displayId);
    uint64_t actual = dscreenMapRelation->GetDisplayId();
    EXPECT_EQ(displayId, actual);
}

/**
 * @tc.name: GetScreenId_001
 * @tc.desc: Verify the GetScreenId function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenMapRelationTest, GetScreenId_001, TestSize.Level1)
{
    uint64_t screenId = 0;
    dscreenMapRelation->SetScreenId(screenId);
    uint64_t actual = dscreenMapRelation->GetScreenId();
    EXPECT_EQ(screenId, actual);
}

/**
 * @tc.name: GetDisplayRect_001
 * @tc.desc: Verify the GetDisplayRect function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenMapRelationTest, GetDisplayRect_001, TestSize.Level1)
{
    DisplayRect res;
    int32_t startX = 10;
    res.startX = startX;
    dscreenMapRelation->SetDisplayRect(res);
    DisplayRect actual = dscreenMapRelation->GetDisplayRect();
    EXPECT_EQ(startX, actual.startX);
}

/**
 * @tc.name: GetScreenRect_001
 * @tc.desc: Verify the GetScreenRect function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenMapRelationTest, GetScreenRect_001, TestSize.Level1)
{
    ScreenRect res;
    int32_t startX = 10;
    res.startX = startX;
    dscreenMapRelation->SetScreenRect(res);
    ScreenRect actual = dscreenMapRelation->GetScreenRect();
    EXPECT_EQ(startX, actual.startX);
}

/**
 * @tc.name: to_json_001
 * @tc.desc: Verify the to_json function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenMapRelationTest, to_json_001, TestSize.Level1)
{
    json j;
    uint64_t displayId = 1;
    uint64_t screenId = 2;
    DisplayRect displayRect = {0, 0, 100, 100};
    ScreenRect screenRect = {0, 0, 200, 200};
    DScreenMapRelation dScreenMapRelation;
    dScreenMapRelation.SetDisplayId(displayId);
    dScreenMapRelation.SetScreenId(screenId);
    dScreenMapRelation.SetDisplayRect(displayRect);
    dScreenMapRelation.SetScreenRect(screenRect);
    to_json(j, dScreenMapRelation);

    uint64_t jsonDisplayId = 0;
    j.at(KEY_DISPLAY_ID).get_to(jsonDisplayId);
    EXPECT_EQ(displayId, jsonDisplayId);
}

/**
 * @tc.name: from_json_001
 * @tc.desc: Verify the from_json function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenMapRelationTest, from_json_001, TestSize.Level1)
{
    json j;
    uint64_t displayId = 1;
    uint64_t screenId = 2;
    DisplayRect displayRect = {0, 0, 100, 100};
    ScreenRect screenRect = {0, 0, 200, 200};
    DScreenMapRelation dScreenMapRelation;
    dScreenMapRelation.SetDisplayId(displayId);
    dScreenMapRelation.SetScreenId(screenId);
    dScreenMapRelation.SetDisplayRect(displayRect);
    dScreenMapRelation.SetScreenRect(screenRect);
    to_json(j, dScreenMapRelation);

    DScreenMapRelation jsonDScreenMapRelation;
    from_json(j, jsonDScreenMapRelation);
    EXPECT_EQ(displayId, jsonDScreenMapRelation.GetDisplayId());
}
} // namespace DistributedHardware
} // namespace OHOS