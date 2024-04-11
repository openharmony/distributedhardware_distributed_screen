/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_DAUDIO_AV_SENDER_ENGINE_ADAPTER_TEST_H
#define OHOS_DAUDIO_AV_SENDER_ENGINE_ADAPTER_TEST_H

#include <gtest/gtest.h>

#include "dscreen_errcode.h"
#include "2.0/include/av_sender_engine_adapter.h"

namespace OHOS {
namespace DistributedHardware {
class AVSenderEngineAdapterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();

    std::shared_ptr<AVTransSenderAdapter> senderAdapter_ = nullptr;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DAUDIO_AV_SENDER_ENGINE_ADAPTER_TEST_H
