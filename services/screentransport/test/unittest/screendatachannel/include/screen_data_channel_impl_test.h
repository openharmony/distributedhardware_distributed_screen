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

#ifndef OHOS_SCREEN_DATA_CHANNEL_IMPL_TEST_H
#define OHOS_SCREEN_DATA_CHANNEL_IMPL_TEST_H

#include <securec.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "softbus_common.h"
#include "softbus_bus_center.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "screen_data_channel_impl.h"
#include "screentrans_test_utils.h"

namespace OHOS {
namespace DistributedHardware {
class ScreenDataChannelImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    std::shared_ptr<ScreenDataChannelImpl> dataChannelImpl_ = nullptr;
    uint64_t tokenId_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif