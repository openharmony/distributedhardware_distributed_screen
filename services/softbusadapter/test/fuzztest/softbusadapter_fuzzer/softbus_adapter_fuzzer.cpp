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

#include "softbus_adapter_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string>

#include "isoftbus_listener.h"
#include "screen_data_channel_impl.h"
#include "softbus_adapter.h"

namespace OHOS {
namespace DistributedHardware {
void SoftbusAdapterFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return;
    }

    std::string pkgname(reinterpret_cast<const char*>(data), size);
    std::string sessionName(reinterpret_cast<const char*>(data), size);
    std::string peerDevId(reinterpret_cast<const char*>(data), size);
    std::string mySessionName(reinterpret_cast<const char*>(data), size);
    std::string peerSessionName(reinterpret_cast<const char*>(data), size);
    int32_t sessionId = *(reinterpret_cast<const int32_t*>(data));
    void *datapointer = const_cast<uint8_t*>(data);
    int32_t dataLen = static_cast<int32_t>(size);
    StreamData streamdata = {reinterpret_cast<char*>(const_cast<uint8_t*>(data)), static_cast<int>(size)};
    StreamData ext = {0};
    StreamFrameInfo frameInfo = {0};
    std::shared_ptr<ScreenDataChannelImpl> listener = std::make_shared<ScreenDataChannelImpl>(peerDevId);
    int32_t result = *(reinterpret_cast<const int32_t*>(data));
    unsigned int datalength = static_cast<unsigned int>(size);
    int eventId = *(reinterpret_cast<const int*>(data));
    int tvCount = *(reinterpret_cast<const int*>(data));
    QosTv tvList;

    SoftbusAdapter::GetInstance().CreateSoftbusSessionServer(pkgname, sessionName, peerDevId);
    SoftbusAdapter::GetInstance().RemoveSoftbusSessionServer(pkgname, sessionName, peerDevId);
    SoftbusAdapter::GetInstance().OpenSoftbusSession(mySessionName, peerSessionName, peerDevId);
    SoftbusAdapter::GetInstance().CloseSoftbusSession(sessionId);
    SoftbusAdapter::GetInstance().SendSoftbusBytes(sessionId, datapointer, dataLen);
    SoftbusAdapter::GetInstance().SendSoftbusStream(sessionId, &streamdata, &ext, &frameInfo);
    SoftbusAdapter::GetInstance().RegisterSoftbusListener(listener, sessionName, peerDevId);
    SoftbusAdapter::GetInstance().UnRegisterSoftbusListener(sessionName, peerDevId);
    SoftbusAdapter::GetInstance().OnSoftbusSessionOpened(sessionId, result);
    SoftbusAdapter::GetInstance().OnSoftbusSessionClosed(sessionId);
    SoftbusAdapter::GetInstance().OnBytesReceived(sessionId, datapointer, dataLen);
    SoftbusAdapter::GetInstance().OnStreamReceived(sessionId, &streamdata, &ext, &frameInfo);
    SoftbusAdapter::GetInstance().OnMessageReceived(sessionId, datapointer, datalength);
    SoftbusAdapter::GetInstance().OnQosEvent(sessionId, eventId, tvCount, &tvList);
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