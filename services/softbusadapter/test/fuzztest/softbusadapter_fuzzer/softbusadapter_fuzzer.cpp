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

#include "softbusadapter_fuzzer.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_util.h"
#include "isoftbus_listener.h"
#include "softbus_bus_center.h"
#include "softbus_common.h"
#include "softbus_adapter.h"

namespace OHOS {
namespace DistributedHardware {
namespace {
    const std::string PEER_SESSION_NAME = "ohos.dhardware.dscreen.session8647073e02e7a78f09473aa324";
    const std::string REMOTE_DEV_ID = "f6d4c0864707aefte7a78f09473aa122ff57fc81c00981fcf5be989e7d112324";
    const std::string DSCREEN_PKG_NAME_TEST = "ohos.dhardware.dscreen";
}
void SoftbusAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }
    int32_t sessionId = *(reinterpret_cast<const int32_t*>(data));

    PeerSocketInfo peerSocketInfo = {
        .name = const_cast<char*>(PEER_SESSION_NAME.c_str()),
        .networkId = const_cast<char*>(REMOTE_DEV_ID.c_str()),
        .pkgName = const_cast<char*>(DSCREEN_PKG_NAME_TEST.c_str()),
        .dataType = DATA_TYPE_BYTES
    };
    ShutdownReason reason = SHUTDOWN_REASON_UNKNOWN;

    void *adapterData = nullptr;
    uint32_t dataLen = *(reinterpret_cast<const uint32_t*>(data));
    StreamData *stream = nullptr;
    StreamData *ext = nullptr;
    StreamFrameInfo *info = nullptr;
    unsigned int dataLen1 = *(reinterpret_cast<const unsigned int *>(data));

    std::shared_ptr<SoftbusAdapter> adapter = std::make_shared<SoftbusAdapter>();
    adapter->sessListener_.OnBind(sessionId, peerSocketInfo);
    adapter->sessListener_.OnShutdown(sessionId, reason);
    adapter->sessListener_.OnBytes(sessionId, adapterData, dataLen);
    adapter->sessListener_.OnStream(sessionId, stream, ext, info);
    adapter->sessListener_.OnMessage(sessionId, adapterData, dataLen1);
    adapter->SendSoftbusStream(sessionId, stream, ext, info);
    adapter->SendSoftbusBytes(sessionId, adapterData, dataLen1);
    adapter->CloseSoftbusSession(sessionId);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::SoftbusAdapterFuzzTest(data, size);
    return 0;
}
