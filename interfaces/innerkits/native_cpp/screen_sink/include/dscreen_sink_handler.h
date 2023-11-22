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

#ifndef OHOS_DSCREEN_SINK_HANDLER_H
#define OHOS_DSCREEN_SINK_HANDLER_H

#include <condition_variable>
#include <cstdint>
#include <mutex>

#include "idistributed_hardware_sink.h"
#include "iremote_object.h"
#include "refbase.h"
#include "single_instance.h"

#include "idscreen_sink.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenSinkHandler : public IDistributedHardwareSink {
DECLARE_SINGLE_INSTANCE_BASE(DScreenSinkHandler);
public:
    int32_t InitSink(const std::string &params) override;
    int32_t ReleaseSink() override;
    int32_t SubscribeLocalHardware(const std::string &dhId, const std::string &param) override;
    int32_t UnsubscribeLocalHardware(const std::string &dhId) override;
    void OnRemoteSinkSvrDied(const wptr<IRemoteObject> &remote);
    void FinishStartSA(const std::string &params, const sptr<IRemoteObject> &remoteObject);
    int32_t RegisterPrivacyResources(std::shared_ptr<PrivacyResourcesListener> listener) override;
    int32_t PauseDistributedHardware(const std::string &networkId) override;
    int32_t ResumeDistributedHardware(const std::string &networkId) override;
    int32_t StopDistributedHardware(const std::string &networkId) override;
private:
    class DScreenSinkSvrRecipient : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override;
    };

    DScreenSinkHandler();
    ~DScreenSinkHandler();

    std::mutex proxyMutex_;
    std::condition_variable proxyConVar_;
    sptr<IDScreenSink> dScreenSinkProxy_ = nullptr;
    sptr<DScreenSinkSvrRecipient> sinkSvrRecipient_ = nullptr;
};

#ifdef __cplusplus
extern "C" {
#endif
__attribute__((visibility("default"))) IDistributedHardwareSink *GetSinkHardwareHandler();
#ifdef __cplusplus
}
#endif
} // namespace DistributedHardware
} // namespace OHOS
#endif