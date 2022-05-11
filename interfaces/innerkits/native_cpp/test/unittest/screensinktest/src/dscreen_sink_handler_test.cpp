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
#include "dscreen_sink_handler_test.h"
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void DScreenSinkHandlerTest::SetUpTestCase(void) {}

void DScreenSinkHandlerTest::TearDownTestCase(void) {}

void DScreenSinkHandlerTest::SetUp(void)
{
    DScreenSinkHandler::GetInstance().InitSink("DScreenSinkHandlerTest");
}

void DScreenSinkHandlerTest::TearDown(void)
{
    DScreenSinkHandler::GetInstance().ReleaseSink();
}
/**
 * @tc.name: LocalHardware_001
 * @tc.desc: Verify the SubscribeLocalHardware function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenSinkHandlerTest, LocalHardware_001, TestSize.Level1)
{
    const std::string dhId = "DScreenSinkHandlerTest";
    const std::string param = "DScreenSinkHandlerTest";
    int32_t ret = DScreenSinkHandler::GetInstance().SubscribeLocalHardware(dhId, param);
    EXPECT_EQ(DH_SUCCESS, ret);
    ret = DScreenSinkHandler::GetInstance().UnsubscribeLocalHardware(dhId);
    EXPECT_EQ(DH_SUCCESS, ret);
}
}
}