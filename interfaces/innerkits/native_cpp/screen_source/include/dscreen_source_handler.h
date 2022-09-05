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

#ifndef OHOS_DSCREEN_SOURCE_HANDLER_H
#define OHOS_DSCREEN_SOURCE_HANDLER_H

#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>

#include "iremote_object.h"
#include "refbase.h"

#include "dscreen_source_callback.h"
#include "idscreen_source.h"
#include "idistributed_hardware_source.h"
#include "single_instance.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenSourceHandler : public IDistributedHardwareSource {
DECLARE_SINGLE_INSTANCE_BASE(DScreenSourceHandler);
public:
    int32_t InitSource(const std::string &params) override;
    int32_t ReleaseSource() override;
    int32_t RegisterDistributedHardware(const std::string &devId, const std::string &dhId,
        const EnableParam &param, std::shared_ptr<RegisterCallback> callback) override;
    int32_t UnregisterDistributedHardware(const std::string &devId, const std::string &dhId,
        std::shared_ptr<UnregisterCallback> callback) override;
    int32_t ConfigDistributedHardware(const std::string &devId, const std::string &dhId,
        const std::string &key, const std::string &value) override;
    void OnRemoteSourceSvrDied(const wptr<IRemoteObject> &remote);
    void FinishStartSA(const std::string &params, const sptr<IRemoteObject> &remoteObject);
private:
    class DScreenSourceSvrRecipient : public IRemoteObject::DeathRecipient {
    public:
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override;
    };

    DScreenSourceHandler();
    ~DScreenSourceHandler() = default;

    std::mutex proxyMutex_;
    std::condition_variable proxyConVar_;
    sptr<IDScreenSource> dScreenSourceProxy_ = nullptr;
    sptr<DScreenSourceCallback> dScreenSourceCallback_ = nullptr;
    sptr<DScreenSourceSvrRecipient> sourceSvrRecipient_ = nullptr;
};

#ifdef __cplusplus
extern "C" {
#endif
__attribute__((visibility("default"))) IDistributedHardwareSource *GetSourceHardwareHandler();
#ifdef __cplusplus
}
#endif
}
}
#endif