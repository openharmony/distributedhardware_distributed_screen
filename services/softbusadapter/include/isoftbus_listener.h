/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_ISOFTBUS_LISTENER
#define OHOS_ISOFTBUS_LISTENER

#include "data_buffer.h"
#include "transport/socket.h"
#include "transport/trans_type.h"

namespace OHOS {
namespace DistributedHardware {
class ISoftbusListener {
public:
    virtual ~ISoftbusListener() = default;

    virtual void OnSessionOpened(int32_t sessionId, PeerSocketInfo info) = 0;
    virtual void OnSessionClosed(int32_t sessionId, ShutdownReason reason) = 0;
    virtual void OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen) = 0;
    virtual void OnStreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
        const StreamFrameInfo *param) = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif