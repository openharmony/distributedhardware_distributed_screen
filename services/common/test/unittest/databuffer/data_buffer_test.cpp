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
#include "data_buffer_test.h"
#include "dscreen_errcode.h"
#undef private

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void DataBufferTest::SetUpTestCase(void) {}

void DataBufferTest::TearDownTestCase(void) {}

void DataBufferTest::SetUp()
{
    dataBuffer_ = std::make_shared<DataBuffer>(capacity);
}

void DataBufferTest::TearDown() {}

/**
 * @tc.name: Capacity_001
 * @tc.desc: Verify the Capacity function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DataBufferTest, Capacity_001, TestSize.Level1)
{
    size_t actual = dataBuffer_->Capacity();
    EXPECT_EQ(capacity, actual);
}

/**
 * @tc.name: Data_001
 * @tc.desc: Verify the Data function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DataBufferTest, Data_001, TestSize.Level1)
{
    uint8_t *actual = dataBuffer_->Data();
    EXPECT_NE(nullptr, actual);
}

/**
 * @tc.name: ResetCapcity_001
 * @tc.desc: Verify the ResetCapcity function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DataBufferTest, ResetCapcity_001, TestSize.Level1)
{
    dataBuffer_->ResetCapcity(10);
    EXPECT_EQ(10, dataBuffer_->Capacity());
}

/**
 * @tc.name: ResetCapcity_002
 * @tc.desc: Verify the ResetCapcity function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DataBufferTest, ResetCapcity_002, TestSize.Level1)
{
    dataBuffer_->ResetCapcity(0);
    EXPECT_EQ(1, dataBuffer_->Capacity());
}

/**
 * @tc.name: AddData_001
 * @tc.desc: Verify the AddData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DataBufferTest, AddData_001, TestSize.Level1)
{
    unsigned char *inputData = nullptr;
    dataBuffer_->AddData(10, inputData);
    EXPECT_EQ(1, dataBuffer_->Capacity());
}

/**
 * @tc.name: AddData_002
 * @tc.desc: Verify the AddData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DataBufferTest, AddData_002, TestSize.Level1)
{
    unsigned char *inputData = new unsigned char[10] {0};
    dataBuffer_->ResetCapcity(20);
    dataBuffer_->SetSize(0);
    dataBuffer_->AddData(10, inputData);
    EXPECT_EQ(10, dataBuffer_->Capacity());
    delete [] inputData;
}

/**
 * @tc.name: GetData_001
 * @tc.desc: Verify the GetData function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(DataBufferTest, GetData_001, TestSize.Level1)
{
    uint8_t *outputData = new uint8_t[10] {0};
    EXPECT_EQ(ERR_DH_SCREEN_INPUT_PARAM_INVALID, dataBuffer_->GetData(10, 10, outputData));
    delete [] outputData;
}
} // namespace DistributedHardware
} // namespace OHOS