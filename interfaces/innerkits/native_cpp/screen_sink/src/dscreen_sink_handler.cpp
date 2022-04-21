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

#include "dscreen_sink_handler.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_log.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(DScreenSinkHandler);

DScreenSinkHandler::DScreenSinkHandler()
{
    DHLOGI("DScreenSinkHandler construct.");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!sinkSvrRecipient_) {
        sinkSvrRecipient_ = new DScreenSinkSvrRecipient();
    }
}

DScreenSinkHandler::~DScreenSinkHandler()
{
    DHLOGI("~DScreenSinkHandler.");
}

int32_t DScreenSinkHandler::InitSink(const std::string &params)
{
    DHLOGD("InitSink");
    std::lock_guard<std::mutex> lock(mutex_);

    if (!dScreenSinkProxy_) {
        sptr<ISystemAbilityManager> samgr =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!samgr) {
            DHLOGE("Failed to get system ability mgr.");
            return ERR_DH_SCREEN_SA_GET_SAMGR_FAIL;
        }
        sptr<IRemoteObject> remoteObject = samgr->GetSystemAbility(DISTRIBUTED_HARDWARE_SCREEN_SINK_SA_ID);
        if (!remoteObject) {
            DHLOGE("Failed to get dscreen sink service.");
            return ERR_DH_SCREEN_SA_GET_SINKSERVICE_FAIL;
        }

        remoteObject->AddDeathRecipient(sinkSvrRecipient_);
        dScreenSinkProxy_ = iface_cast<IDScreenSink>(remoteObject);
        if ((!dScreenSinkProxy_) || (!dScreenSinkProxy_->AsObject())) {
            DHLOGE("Failed to get dscreen sink proxy.");
            return ERR_DH_SCREEN_SA_GET_SINKPROXY_FAIL;
        }
    }
    int32_t ret = dScreenSinkProxy_->InitSink(params);
    return ret;
}

int32_t DScreenSinkHandler::ReleaseSink()
{
    DHLOGD("ReleaseSink");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!dScreenSinkProxy_) {
        DHLOGE("screen sink proxy not init.");
        return ERR_DH_SCREEN_SA_SINKPROXY_NOT_INIT;
    }

    int32_t ret = dScreenSinkProxy_->ReleaseSink();
    return ret;
}

int32_t DScreenSinkHandler::SubscribeLocalHardware(const std::string &dhId, const std::string &param)
{
    DHLOGD("SubscribeLocalHardware");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!dScreenSinkProxy_) {
        DHLOGE("screen sink proxy not init.");
        return ERR_DH_SCREEN_SA_SINKPROXY_NOT_INIT;
    }
    int32_t ret = dScreenSinkProxy_->SubscribeLocalHardware(dhId, param);
    return ret;
}

int32_t DScreenSinkHandler::UnsubscribeLocalHardware(const std::string &dhId)
{
    DHLOGD("UnsubscribeLocalHardware");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!dScreenSinkProxy_) {
        DHLOGE("screen sink proxy not init.");
        return ERR_DH_SCREEN_SA_SINKPROXY_NOT_INIT;
    }
    int32_t ret = dScreenSinkProxy_->UnsubscribeLocalHardware(dhId);
    return ret;
}

void DScreenSinkHandler::DScreenSinkSvrRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    DHLOGI("DScreenSinkSvrRecipient OnRemoteDied.");
    DScreenSinkHandler::GetInstance().OnRemoteSinkSvrDied(remote);
}

void DScreenSinkHandler::OnRemoteSinkSvrDied(const wptr<IRemoteObject> &remote)
{
    DHLOGI("OnRemoteSinkSvrDied");
    sptr<IRemoteObject> remoteObject = remote.promote();
    if (!remoteObject) {
        DHLOGE("OnRemoteDied remote promoted failed");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (dScreenSinkProxy_) {
        dScreenSinkProxy_->AsObject()->RemoveDeathRecipient(sinkSvrRecipient_);
        dScreenSinkProxy_ = nullptr;
    }
}

IDistributedHardwareSink *GetSinkHardwareHandler()
{
    DHLOGD("GetSinkHardwareHandler");
    return &DScreenSinkHandler::GetInstance();
}
} // namespace DistributedHardware
} // namespace OHOS