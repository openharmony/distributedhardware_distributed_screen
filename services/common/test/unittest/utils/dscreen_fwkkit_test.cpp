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
#include "dscreen_fwkkit_test.h"
#include "distributed_hardware_fwk_kit.h"
#include "dscreen_fwkkit.h"
#undef private

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void DScreenFwkKitTest::SetUpTestCase(void) {}

void DScreenFwkKitTest::TearDownTestCase(void) {}

void DScreenFwkKitTest::SetUp() {}

void DScreenFwkKitTest::TearDown() {}

/**
 * @tc.name: GetDHFwkKit_001
 * @tc.desc: Verify the GetDHFwkKit function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenFwkKitTest, GetDHFwkKit_001, TestSize.Level1)
{
    auto ret = DScreenFwkKit::GetInstance().GetDHFwkKit();

    EXPECT_NE(nullptr, DScreenFwkKit::GetInstance().dhfwkKit_);
}
} // namespace DistributedHardware
} // namespace OHOS