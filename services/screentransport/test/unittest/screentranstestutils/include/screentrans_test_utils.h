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

#ifndef OHOS_SCREEN_TRANS_TEST_UTILS_H
#define OHOS_SCREEN_TRANS_TEST_UTILS_H

#include <stddef.h>

#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "iimage_sink_processor_listener.h"
#include "iscreen_sink_trans_callback.h"
#include "iscreen_source_trans_callback.h"
#include "iscreen_channel_listener.h"
#include "iscreen_channel.h"

namespace OHOS {
namespace DistributedHardware {
constexpr int32_t VIDEO_CODEC_TYPE_INVALID = -1;
constexpr int32_t VIDEO_DATA_FORMAT_INVALID = -1;
constexpr int32_t WIDTH_INVALID = 9999;
constexpr int32_t HEIGHT_INVALID = 9999;
constexpr int32_t FPS = 30;
constexpr size_t DATA_LEN = 128;

class MockIScreenSinkTransCallback : public IScreenSinkTransCallback {
public:
    explicit MockIScreenSinkTransCallback()
    {
    }
    ~MockIScreenSinkTransCallback() override = default;

    virtual void OnError(int32_t err, const std::string &content) override
    {
        (void) err;
        (void) content;
    }
};

class MockIImageSinkProcessorListener : public IImageSinkProcessorListener {
public:
    explicit MockIImageSinkProcessorListener()
    {
    }
    ~MockIImageSinkProcessorListener() override = default;

    virtual void OnProcessorStateNotify(int32_t state) override
    {
        (void) state;
    }
};

class MockIScreenSourceTransCallback : public IScreenSourceTransCallback {
public:
    ~MockIScreenSourceTransCallback() override {};

    virtual void OnError(int32_t err, const std::string &content) override
    {
        (void) err;
        (void) content;
    }
};

class MockIScreenChannelListener : public IScreenChannelListener {
public:
    void OnSessionOpened() override {}
    void OnSessionClosed() override {}
    void OnDataReceived(const std::shared_ptr<DataBuffer> &data) override {}
};

class MockScreenDataChannelImpl : public IScreenChannel {
public:
    int32_t CreateSession(const std::shared_ptr<IScreenChannelListener> &listener) override
    {
        return DH_SUCCESS;
    }
    int32_t ReleaseSession() override
    {
        return DH_SUCCESS;
    }
    int32_t OpenSession() override
    {
        return DH_SUCCESS;
    }
    int32_t CloseSession() override
    {
        return ERR_DH_SCREEN_TRANS_SESSION_NOT_OPEN;
    }
    int32_t SendData(const std::shared_ptr<DataBuffer> &data) override
    {
        (void) data;
        return DH_SUCCESS;
    }
};

} // namespace DistributedHardware
} // namespace OHOS
#endif