/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "softbusonstreamreceived_fuzzer.h"

#include "accesstoken_kit.h"
#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_util.h"
#include "isoftbus_listener.h"
#include "nativetoken_kit.h"
#include "softbus_adapter.h"
#include "softbus_bus_center.h"
#include "softbus_common.h"
#include "token_setproc.h"


namespace OHOS {
namespace DistributedHardware {
void SoftbusOnStreamReceivedFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < (sizeof(int32_t)))) {
        return;
    }
    std::string peerDevId(reinterpret_cast<const char*>(data), size);
    int32_t sessionId = *(reinterpret_cast<const int32_t*>(data));
    StreamData *stream = nullptr;
    StreamData *ext = nullptr;
    StreamFrameInfo *info = nullptr;
    std::shared_ptr<SoftbusAdapter> adapter = std::make_shared<SoftbusAdapter>();
    adapter->OnStreamReceived(sessionId, stream, ext, info);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::SoftbusOnStreamReceivedFuzzTest(data, size);
    return 0;
}

