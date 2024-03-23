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

#include "dscreen_source_callback.h"

#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "dscreen_util.h"

namespace OHOS {
namespace DistributedHardware {
int32_t DScreenSourceCallback::OnNotifyRegResult(const std::string &devId, const std::string &dhId,
    const std::string &reqId, int32_t status, const std::string &data)
{
    DHLOGI("DScreenSourceCallback OnNotifyRegResult devId: %{public}s dhId: %{public}s status: %{public}" PRId32,
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), status);
    std::lock_guard<std::mutex> lock(registerMutex_);
    const auto iter = registerCallbackMap_.find(reqId);
    if (iter != registerCallbackMap_.end()) {
        if (iter->second == nullptr) {
            DHLOGE("DScreenSourceCallback Regcallback is null.");
            return ERR_DH_SCREEN_SA_REGISTERCALLBACK_NOT_FOUND;
        }
        iter->second->OnRegisterResult(devId, dhId, status, data);
        registerCallbackMap_.erase(reqId);
        return DH_SUCCESS;
    }

    return ERR_DH_SCREEN_SA_REGISTERCALLBACK_NOT_FOUND;
}

int32_t DScreenSourceCallback::OnNotifyUnregResult(const std::string &devId, const std::string &dhId,
    const std::string &reqId, int32_t status, const std::string &data)
{
    DHLOGI("DScreenSourceCallback OnNotifyUnregResult devId: %{public}s dhId: %{public}s status: %{public}" PRId32,
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), status);
    std::lock_guard<std::mutex> lock(unregisterMutex_);
    const auto iter = unregisterCallbackMap_.find(reqId);
    if (iter != unregisterCallbackMap_.end()) {
        if (iter->second == nullptr) {
            DHLOGE("DScreenSourceCallback Unregcallback is null.");
            return ERR_DH_SCREEN_SA_UNREGISTERCALLBACK_NOT_FOUND;
        }
        iter->second->OnUnregisterResult(devId, dhId, status, data);
        unregisterCallbackMap_.erase(reqId);
        return DH_SUCCESS;
    }

    return ERR_DH_SCREEN_SA_UNREGISTERCALLBACK_NOT_FOUND;
}

void DScreenSourceCallback::PushRegRegisterCallback(const std::string &reqId,
    const std::shared_ptr<RegisterCallback> &callback)
{
    DHLOGD("PushRegRegisterCallback");
    std::lock_guard<std::mutex> lock(registerMutex_);
    registerCallbackMap_.emplace(reqId, callback);
}

void DScreenSourceCallback::PopRegRegisterCallback(const std::string &reqId)
{
    DHLOGD("PopRegRegisterCallback");
    std::lock_guard<std::mutex> lock(registerMutex_);
    registerCallbackMap_.erase(reqId);
}

void DScreenSourceCallback::PushUnregisterCallback(const std::string &reqId,
    const std::shared_ptr<UnregisterCallback> &callback)
{
    DHLOGD("PushUnregisterCallback");
    std::lock_guard<std::mutex> lock(unregisterMutex_);
    unregisterCallbackMap_.emplace(reqId, callback);
}

void DScreenSourceCallback::PopUnregisterCallback(const std::string &reqId)
{
    DHLOGD("PopUnregisterCallback");
    std::lock_guard<std::mutex> lock(unregisterMutex_);
    unregisterCallbackMap_.erase(reqId);
}
}
}