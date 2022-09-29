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

#ifndef OHOS_DSCREEN_HANDLER_TEST_H
#define OHOS_DSCREEN_HANDLER_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#include "dscreen_handler.h"
#include "dscreen_errcode.h"
#include "ihardware_handler.h"
#undef private

namespace OHOS {
namespace DistributedHardware {
class DScreenHandlerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class MockPluginListener : public PluginListener {
public:
    MockPluginListener() = default;
    virtual ~MockPluginListener() = default;
    void PluginHardware(const std::string &dhId, const std::string &attrs) override
    {
        (void) dhId;
        (void) attrs;
        return;
    }
    void UnPluginHardware(const std::string &dhId) override
    {
        (void) dhId;
        return;
    }
};
} // namespace DistributedHardware
} // namespace OHOS
#endif