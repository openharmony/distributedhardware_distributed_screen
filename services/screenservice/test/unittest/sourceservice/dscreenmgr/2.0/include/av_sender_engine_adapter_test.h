/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
#include <gmock/gmock.h>

#include "2.0/include/av_sender_engine_adapter.h"
#include "dscreen_errcode.h"
#include "i_av_receiver_engine.h"
#include "i_av_sender_engine.h"

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

class MockAVSenderEngine : public IAVSenderEngine {
public:
    MOCK_METHOD(int32_t, Initialize, (), (override));
    MOCK_METHOD(int32_t, Release, (), (override));
    MOCK_METHOD(int32_t, Start, (), (override));
    MOCK_METHOD(int32_t, Stop, (), (override));
    MOCK_METHOD(int32_t, PushData, (const std::shared_ptr<AVTransBuffer> &buffer), (override));
    MOCK_METHOD(int32_t, SetParameter, (AVTransTag tag, const std::string &value), (override));
    MOCK_METHOD(int32_t, SendMessage, (const std::shared_ptr<AVTransMessage> &message), (override));
    MOCK_METHOD(int32_t, CreateControlChannel,
        (const std::vector<std::string> &dstDevIds, const ChannelAttribute &attribution), (override));
    MOCK_METHOD(int32_t, RegisterSenderCallback, (const std::shared_ptr<IAVSenderEngineCallback> &callback),
        (override));

    bool StartDumpMediaData() override
    {
        return false;
    }

    bool StopDumpMediaData() override
    {
        return false;
    }

    bool ReStartDumpMediaData() override
    {
        return false;
    }
};

class MockAVSenderAdapterCallback : public AVSenderAdapterCallback {
public:
    explicit MockAVSenderAdapterCallback() {}
    ~MockAVSenderAdapterCallback() {}
    void OnEngineEvent(DScreenEventType event, const std::string &content) override {}
    void OnEngineMessage(const std::shared_ptr<AVTransMessage> &message) override {}
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DAUDIO_AV_SENDER_ENGINE_ADAPTER_TEST_H
