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

#ifndef OHOS_DSCREEN_MANAGER_ADAPTER_TEST_H
#define OHOS_DSCREEN_MANAGER_ADAPTER_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "dm_common.h"
#include "screen_manager.h"
namespace OHOS {
namespace DistributedHardware {
class DScreenManagerAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    uint64_t tokenId_;
};

class MockScreenGroupListener : public Rosen::ScreenManager::IScreenGroupListener {
public:
    void OnChange(const std::vector<uint64_t> &screenIds, Rosen::ScreenGroupChangeEvent event) override
    {
        (void) screenIds;
        (void) event;
    }
};
} // namespace DistributedHardware
} // namespace OHOS
#endif