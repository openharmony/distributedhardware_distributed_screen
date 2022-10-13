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

#include <chrono>
#include <new>
#include <string>

#include "errors.h"
#include "hitrace_meter.h"
#include "idscreen_source_callback.h"
#include "if_system_ability_manager.h"
#include "iremote_broker.h"
#include "isystem_ability_load_callback.h"
#include "iservice_registry.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hitrace.h"
#include "dscreen_hisysevent.h"
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
    if (sourceSvrRecipient_ == nullptr) {
        sourceSvrRecipient_ = new (std::nothrow) DScreenSourceSvrRecipient();
    }

    if (dScreenSourceCallback_ == nullptr) {
        dScreenSourceCallback_ = new (std::nothrow) DScreenSourceCallback();
    }
}

int32_t DScreenSourceHandler::InitSource(const std::string &params)
{
    if (params.empty()) {
        DHLOGE("InitSource params is invalid.");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    DHLOGI("DScreenSourceHandler InitSource");
    std::unique_lock<std::mutex> lock(proxyMutex_);
    if (dScreenSourceProxy_ == nullptr) {
        sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgr == nullptr) {
            DHLOGE("Failed to get system ability mgr.");
            ReportSaFail(DSCREEN_INIT_FAIL, ERR_DH_SCREEN_SA_GET_SAMGR_FAIL, DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID,
                "dscreen source get samgr failed.");
            return ERR_DH_SCREEN_SA_GET_SAMGR_FAIL;
        }
        sptr<DScreenSourceLoadCallback> loadCallback = new DScreenSourceLoadCallback(params);
        StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SOURCE_LOAD_SYSTEM_ABILITY_START);
        int32_t ret = samgr->LoadSystemAbility(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, loadCallback);
        if (ret != ERR_OK) {
            DHLOGE("Failed to Load systemAbility, systemAbilityId:%d, ret code:%d",
                DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, ret);
            ReportSaFail(DSCREEN_INIT_FAIL, ret, DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID,
                "dscreen source LoadSystemAbility call failed.");
            return ERR_DH_SCREEN_SA_GET_SOURCEPROXY_FAIL;
        }
    }

    auto waitStatus = proxyConVar_.wait_for(lock, std::chrono::milliseconds(SCREEN_LOADSA_TIMEOUT_MS),
        [this]() { return (dScreenSourceProxy_ != nullptr); });
    if (!waitStatus) {
        DHLOGE("screen load sa timeout.");
        ReportSaFail(DSCREEN_INIT_FAIL, ERR_DH_SCREEN_SA_LOAD_TIMEOUT, DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID,
            "dscreen source sa load timeout.");
        return ERR_DH_SCREEN_SA_LOAD_TIMEOUT;
    }

    FinishTrace(DSCREEN_HITRACE_LABEL);
    return DH_SUCCESS;
}

void DScreenSourceHandler::FinishStartSA(const std::string &params, const sptr<IRemoteObject> &remoteObject)
{
    DHLOGI("DScreenSourceHandler FinishStartSA");
    std::lock_guard<std::mutex> lock(proxyMutex_);
    if (sourceSvrRecipient_ == nullptr) {
        DHLOGE("sourceSvrRecipient is nullptr.");
        ReportSaFail(DSCREEN_INIT_FAIL, ERR_DH_SCREEN_SA_SOURCEPROXY_NOT_INIT, DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID,
            "sourceSvrRecipient is nullptr.");
        return;
    }
    remoteObject->AddDeathRecipient(sourceSvrRecipient_);
    dScreenSourceProxy_ = iface_cast<IDScreenSource>(remoteObject);
    if ((dScreenSourceProxy_ == nullptr) || (dScreenSourceProxy_->AsObject() == nullptr)) {
        DHLOGE("Failed to get dscreen source proxy.");
        ReportSaFail(DSCREEN_INIT_FAIL, ERR_DH_SCREEN_SA_SOURCEPROXY_NOT_INIT, DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID,
            "dscreen source get proxy failed.");
        return;
    }
    if (dScreenSourceCallback_ == nullptr) {
        DHLOGE("dScreenSourceCallback is nullptr.");
        ReportSaFail(DSCREEN_INIT_FAIL, ERR_DH_SCREEN_SA_SOURCEPROXY_NOT_INIT, DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID,
            "dScreenSourceCallback is nullptr.");
        return;
    }
    dScreenSourceProxy_->InitSource(params, dScreenSourceCallback_);
    proxyConVar_.notify_one();
    ReportSaEvent(DSCREEN_INIT, DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, "dscreen source sa load success.");
}

int32_t DScreenSourceHandler::ReleaseSource()
{
    DHLOGI("DScreenSourceHandler ReleaseSource");
    std::lock_guard<std::mutex> lock(proxyMutex_);
    if (dScreenSourceProxy_ == nullptr) {
        DHLOGE("screen source proxy not init.");
        ReportSaFail(DSCREEN_INIT_FAIL, ERR_DH_SCREEN_SA_SOURCEPROXY_NOT_INIT, DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID,
            "dscreen source proxy not init.");
        return ERR_DH_SCREEN_SA_SOURCEPROXY_NOT_INIT;
    }
    int32_t ret = dScreenSourceProxy_->ReleaseSource();
    dScreenSourceProxy_ = nullptr;
    return ret;
}

int32_t DScreenSourceHandler::RegisterDistributedHardware(const std::string &devId,
    const std::string &dhId, const EnableParam &param, std::shared_ptr<RegisterCallback> callback)
{
    if (devId.empty() || dhId.empty()) {
        DHLOGE("device id or dh id empty.");
        return ERR_DH_SCREEN_STRING_PARAM_EMPTY;
    }
    if (callback == nullptr) {
        DHLOGE("callback is nullptr.");
        return ERR_DH_SCREEN_REGISTER_CALLBACK_NOT_INIT;
    }
    DHLOGI("RegisterDistributedHardware, devId: %s, dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    std::lock_guard<std::mutex> lock(proxyMutex_);
    if (dScreenSourceProxy_ == nullptr) {
        DHLOGE("screen source proxy not init.");
        return ERR_DH_SCREEN_SA_SOURCEPROXY_NOT_INIT;
    }
    if (dScreenSourceCallback_ == nullptr) {
        DHLOGE("screen source callback is null.");
        return ERR_DH_SCREEN_SA_SOURCEPCALLBACK_NOT_INIT;
    }

    std::string reqId = GetRandomID();
    dScreenSourceCallback_->PushRegRegisterCallback(reqId, callback);
    return dScreenSourceProxy_->RegisterDistributedHardware(devId, dhId, param, reqId);
}

int32_t DScreenSourceHandler::UnregisterDistributedHardware(const std::string &devId,
    const std::string &dhId, std::shared_ptr<UnregisterCallback> callback)
{
    if (devId.empty() || dhId.empty()) {
        DHLOGE("device id or dh id empty.");
        return ERR_DH_SCREEN_STRING_PARAM_EMPTY;
    }
    if (callback == nullptr) {
        DHLOGE("callback is nullptr.");
        return ERR_DH_SCREEN_REGISTER_CALLBACK_NOT_INIT;
    }
    DHLOGI("UnregisterDistributedHardware, devId: %s, dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    std::lock_guard<std::mutex> lock(proxyMutex_);
    if (dScreenSourceProxy_ == nullptr) {
        DHLOGE("screen source proxy not init.");
        return ERR_DH_SCREEN_SA_SOURCEPROXY_NOT_INIT;
    }
    if (dScreenSourceCallback_ == nullptr) {
        DHLOGE("screen source callback is null.");
        return ERR_DH_SCREEN_SA_SOURCEPCALLBACK_NOT_INIT;
    }

    std::string reqId = GetRandomID();
    dScreenSourceCallback_->PushUnregisterCallback(reqId, callback);
    return dScreenSourceProxy_->UnregisterDistributedHardware(devId, dhId, reqId);
}

int32_t DScreenSourceHandler::ConfigDistributedHardware(const std::string &devId,
    const std::string &dhId, const std::string &key, const std::string &value)
{
    DHLOGI("ConfigDistributedHardware");
    return DH_SUCCESS;
}

void DScreenSourceHandler::DScreenSourceSvrRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    if (remote == nullptr) {
        DHLOGI("OnRemoteDied remote is nullptr.");
        return;
    }
    DHLOGI("DScreenSourceSvrRecipient OnRemoteDied");
    DScreenSourceHandler::GetInstance().OnRemoteSourceSvrDied(remote);
}

void DScreenSourceHandler::OnRemoteSourceSvrDied(const wptr<IRemoteObject> &remote)
{
    DHLOGI("OnRemoteSourceSvrDied");
    std::lock_guard<std::mutex> lock(proxyMutex_);
    if (dScreenSourceProxy_ == nullptr) {
        DHLOGE("dScreenSourceProxy is nullptr.");
        return;
    }
    sptr<IRemoteObject> remoteObject = remote.promote();
    if (remoteObject == nullptr) {
        DHLOGE("OnRemoteDied remote promoted failed");
        return;
    }

    if (dScreenSourceProxy_->AsObject() != remoteObject) {
        DHLOGE("OnRemoteSourceSvrDied not found remote object.");
        return;
    }

    dScreenSourceProxy_->AsObject()->RemoveDeathRecipient(sourceSvrRecipient_);
    dScreenSourceProxy_ = nullptr;
}

IDistributedHardwareSource *GetSourceHardwareHandler()
{
    DHLOGI("GetSourceHardwareHandler");
    return &DScreenSourceHandler::GetInstance();
}
}
}