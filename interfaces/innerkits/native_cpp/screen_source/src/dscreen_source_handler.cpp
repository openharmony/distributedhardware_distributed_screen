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

#include "dscreen_source_handler.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "dscreen_source_load_callback.h"
#include "dscreen_util.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(DScreenSourceHandler);

DScreenSourceHandler::DScreenSourceHandler()
{
    DHLOGI("DScreenSourceHandler construct.");
    std::lock_guard<std::mutex> lock(proxyMutex_);
    if (!sourceSvrRecipient_) {
        sourceSvrRecipient_ = new DScreenSourceSvrRecipient();
    }

    if (!dScreenSourceCallback_) {
        dScreenSourceCallback_ = new DScreenSourceCallback();
    }
}

DScreenSourceHandler::~DScreenSourceHandler()
{
    DHLOGI("~DScreenSourceHandler.");
}

int32_t DScreenSourceHandler::InitSource(const std::string &params)
{
    DHLOGD("InitSource");

    if (!dScreenSourceProxy_) {
        sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!samgr) {
            DHLOGE("Failed to get system ability mgr.");
            return ERR_DH_SCREEN_SA_GET_SAMGR_FAIL;
        }
        sptr<DScreenSourceLoadCallback> loadCallback = new DScreenSourceLoadCallback(params);
        int32_t ret = samgr->LoadSystemAbility(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, loadCallback);
        if (ret != ERR_OK) {
            DHLOGE("Failed to Load systemAbility, systemAbilityId:%d, ret code:%d",
                DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, ret);
            return ERR_DH_SCREEN_SA_GET_SOURCEPROXY_FAIL;
        }
    }

    std::unique_lock<std::mutex> lock(proxyMutex_);
    auto waitStatus = proxyConVar_.wait_for(lock, std::chrono::milliseconds(SCREEN_LOADSA_TIMEOUT_MS),
        [this]() { return (dScreenSourceProxy_ != nullptr); });
    if (!waitStatus) {
        DHLOGE("screen load sa timeout.");
        return ERR_DH_SCREEN_SA_LOAD_TIMEOUT;
    }

    return DH_SUCCESS;
}

void DScreenSourceHandler::FinishStartSA(const std::string &params,
    const sptr<IRemoteObject> &remoteObject)
{
    DHLOGD("FinishStartSA");
    std::lock_guard<std::mutex> lock(proxyMutex_);
    remoteObject->AddDeathRecipient(sourceSvrRecipient_);
    dScreenSourceProxy_ = iface_cast<IDScreenSource>(remoteObject);
    if ((!dScreenSourceProxy_) || (!dScreenSourceProxy_->AsObject())) {
        DHLOGE("Failed to get dscreen source proxy.");
        return;
    }
    dScreenSourceProxy_->InitSource(params, dScreenSourceCallback_);
    proxyConVar_.notify_one();
}

int32_t DScreenSourceHandler::ReleaseSource()
{
    DHLOGD("ReleaseSource");
    std::lock_guard<std::mutex> lock(proxyMutex_);
    if (!dScreenSourceProxy_) {
        DHLOGE("screen source proxy not init.");
        return ERR_DH_SCREEN_SA_SOURCEPROXY_NOT_INIT;
    }
    int32_t ret = dScreenSourceProxy_->ReleaseSource();
    dScreenSourceProxy_ = nullptr;
    return ret;
}

int32_t DScreenSourceHandler::RegisterDistributedHardware(const std::string &devId,
    const std::string &dhId, const EnableParam &param, std::shared_ptr<RegisterCallback> callback)
{
    DHLOGD("RegisterDistributedHardware, devId: %s, dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    std::lock_guard<std::mutex> lock(proxyMutex_);
    if (!dScreenSourceProxy_) {
        DHLOGE("screen source proxy not init.");
        return ERR_DH_SCREEN_SA_SOURCEPROXY_NOT_INIT;
    }
    if (!dScreenSourceCallback_) {
        DHLOGE("screen source callback is null.");
        return ERR_DH_SCREEN_SA_SOURCEPCALLBACK_NOT_INIT;
    }

    std::string reqId = GetRandomID();
    dScreenSourceCallback_->PushRegRegisterCallback(reqId, callback);
    int ret = dScreenSourceProxy_->RegisterDistributedHardware(devId, dhId, param, reqId);
    return ret;
}

int32_t DScreenSourceHandler::UnregisterDistributedHardware(const std::string &devId,
    const std::string &dhId, std::shared_ptr<UnregisterCallback> callback)
{
    DHLOGD("UnregisterDistributedHardware, devId: %s, dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    std::lock_guard<std::mutex> lock(proxyMutex_);
    if (!dScreenSourceProxy_) {
        DHLOGE("screen source proxy not init.");
        return ERR_DH_SCREEN_SA_SOURCEPROXY_NOT_INIT;
    }
    if (!dScreenSourceCallback_) {
        DHLOGE("screen source callback is null.");
        return ERR_DH_SCREEN_SA_SOURCEPCALLBACK_NOT_INIT;
    }

    std::string reqId = GetRandomID();
    dScreenSourceCallback_->PushUnregisterCallback(reqId, callback);
    int ret = dScreenSourceProxy_->UnregisterDistributedHardware(devId, dhId, reqId);
    return ret;
}

int32_t DScreenSourceHandler::ConfigDistributedHardware(const std::string &devId,
    const std::string &dhId, const std::string &key, const std::string &value)
{
    DHLOGD("ConfigDistributedHardware");
    return DH_SUCCESS;
}

void DScreenSourceHandler::DScreenSourceSvrRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    DHLOGI("DScreenSourceSvrRecipient OnRemoteDied");
    DScreenSourceHandler::GetInstance().OnRemoteSourceSvrDied(remote);
}

void DScreenSourceHandler::OnRemoteSourceSvrDied(const wptr<IRemoteObject> &remote)
{
    DHLOGI("OnRemoteSourceSvrDied");
    sptr<IRemoteObject> remoteObject = remote.promote();
    if (!remoteObject) {
        DHLOGE("OnRemoteDied remote promoted failed");
        return;
    }
    std::lock_guard<std::mutex> lock(proxyMutex_);
    if (dScreenSinkProxy_ != nullptr) {
        dScreenSourceProxy_->AsObject()->RemoveDeathRecipient(sourceSvrRecipient_);
        dScreenSourceProxy_ = nullptr;
    }
}

IDistributedHardwareSource *GetSourceHardwareHandler()
{
    DHLOGD("GetSourceHardwareHandler");
    return &DScreenSourceHandler::GetInstance();
}
}
}