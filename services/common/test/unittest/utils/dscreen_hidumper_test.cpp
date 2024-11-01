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

#include "dscreen_hidumper_test.h"
#include "dscreen_hidumper.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {

int32_t DscreenHidumper::DumpScreenData(std::string &result)
{
    return DH_SUCCESS;
}

int32_t DscreenHidumper::ReDumpScreenData(std::string &result)
{
    return DH_SUCCESS;
}

int32_t DscreenHidumper::ShowIllegalInfomation(std::string &result)
{
    return DH_SUCCESS;
}

void DScreenHidumperTest::SetUpTestCase(void) {}

void DScreenHidumperTest::TearDownTestCase(void) {}

void DScreenHidumperTest::SetUp() {}

void DScreenHidumperTest::TearDown() {}

/**
 * @tc.name: Dump_001
 * @tc.desc: Verify the Dump function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenHidumperTest, Dump_001, TestSize.Level1)
{
    std::string result;
    std::string helpStr;
    std::vector<std::string> argsStr;
    DscreenHidumper::GetInstance().ShowHelp(helpStr);
    bool ret = DscreenHidumper::GetInstance().Dump(argsStr, result);
    EXPECT_EQ(true, ret);
    EXPECT_EQ(helpStr, result);
}

/**
 * @tc.name: Dump_002
 * @tc.desc: Verify the Dump function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenHidumperTest, Dump_002, TestSize.Level1)
{
    std::string result;
    std::vector<std::string> argsStr = {"arg1", "arg2"};
    bool ret = DscreenHidumper::GetInstance().Dump(argsStr, result);
    EXPECT_EQ(true, ret);
    EXPECT_EQ("unknown command, -h for help.", result);
}

/**
 * @tc.name: Dump_003
 * @tc.desc: Verify the Dump function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenHidumperTest, Dump_003, TestSize.Level1)
{
    std::string result;
    std::string helpStr;
    std::vector<std::string> argsStr = {"-h"};
    DscreenHidumper::GetInstance().ShowHelp(helpStr);
    bool ret = DscreenHidumper::GetInstance().Dump(argsStr, result);
    EXPECT_EQ(true, ret);
    EXPECT_EQ(helpStr, result);
}

/**
 * @tc.name: ProcessDump_001
 * @tc.desc: Verify the ProcessDump function with HidumpFlag::DUMP_SCREEN_DATA.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenHidumperTest, ProcessDump_001, TestSize.Level1)
{
    std::string result;
    std::string argsStr = "--dump";
    int32_t ret = DscreenHidumper::GetInstance().ProcessDump(argsStr, result);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: ProcessDump_002
 * @tc.desc: Verify the ProcessDump function with HidumpFlag::DUMP_SCREEN_DATA_RESTART.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenHidumperTest, ProcessDump_002, TestSize.Level1)
{
    std::string result;
    std::string argsStr = "--redump";
    int32_t ret = DscreenHidumper::GetInstance().ProcessDump(argsStr, result);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: ProcessDump_003
 * @tc.desc: Verify the ProcessDump function with HidumpFlag::DUMP_SCREEN_DATA_STOP.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenHidumperTest, ProcessDump_003, TestSize.Level1)
{
    std::string result;
    std::string argsStr = "--stopdump";
    int32_t ret = DscreenHidumper::GetInstance().ProcessDump(argsStr, result);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: ProcessDump_004
 * @tc.desc: Verify the ProcessDump function with an unknown command.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DScreenHidumperTest, ProcessDump_004, TestSize.Level1)
{
    std::string result;
    std::string argsStr = "unknown_command";
    int32_t ret = DscreenHidumper::GetInstance().ProcessDump(argsStr, result);
    EXPECT_EQ(DH_SUCCESS, ret);
}
} // namespace DistributedHardware
} // namespace OHOS