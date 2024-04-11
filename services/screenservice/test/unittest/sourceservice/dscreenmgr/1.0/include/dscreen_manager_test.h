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

#ifndef OHOS_DSCREEN_MANAGER_TEST_V1_0_H
#define OHOS_DSCREEN_MANAGER_TEST_V1_0_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "dscreen_errcode.h"
#include "1.0/include/dscreen_manager.h"

namespace OHOS {
namespace DistributedHardware {
namespace V1_0 {
class DScreenManagerTestV1 : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};
} // namespace V1_0
} // namespace DistributedHardware
} // namespace OHOS
#endif