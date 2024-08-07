/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef SOFTBUSADAPTER_FUZZER_H
#define SOFTBUSADAPTER_FUZZER_H

#define FUZZ_PROJECT_NAME "softbusadapter_fuzzer"

#include "isoftbus_listener.h"

namespace OHOS {
namespace DistributedHardware {
class MockSoftbusListener : public ISoftbusListener {
public:
    MockSoftbusListener() = default;
    ~MockSoftbusListener() = default;
    void OnSessionOpened(int32_t sessionId, PeerSocketInfo info) override {}
    void OnSessionClosed(int32_t sessionId, ShutdownReason reason) override {}
    void OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen) override {}
    void OnStreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
        const StreamFrameInfo *param) override {}
};
} // namespace DistributedHardware
} // namespace OHOS
#endif
