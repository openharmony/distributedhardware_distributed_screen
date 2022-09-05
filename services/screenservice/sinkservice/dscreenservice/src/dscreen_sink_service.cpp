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

#include "dscreen_sink_service.h"

#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "dscreen_sa_process_state.h"
#include "dscreen_util.h"

namespace OHOS {
namespace DistributedHardware {
REGISTER_SYSTEM_ABILITY_BY_ID(DScreenSinkService, DISTRIBUTED_HARDWARE_SCREEN_SINK_SA_ID, true);

DScreenSinkService::DScreenSinkService(int32_t saId, bool runOnCreate) : SystemAbility(saId, runOnCreate)
{
    DHLOGI("DScreenSinkService construct.");
}

void DScreenSinkService::OnStart()
{
    DHLOGI("dscreen sink service start.");
    if (!Init()) {
        DHLOGE("DScreenSinkService init failed.");
        return;
    }
    DHLOGI("DScreenSinkService start success.");
}

void DScreenSinkService::OnStop()
{
    DHLOGI("dscreen sink service stop.");
    registerToService_ = false;
}

bool DScreenSinkService::Init()
{
    DHLOGI("dscreen sink service start init.");
    if (!registerToService_) {
        bool ret = Publish(this);
        if (!ret) {
            DHLOGE("dscreen sink publish service failed.");
            return false;
        }
        registerToService_ = true;
    }
    DHLOGI("dscreenService init success.");
    return true;
}

int32_t DScreenSinkService::InitSink(const std::string &params)
{
    DHLOGI("InitSink");
    (void)params;
    return DH_SUCCESS;
}

int32_t DScreenSinkService::ReleaseSink()
{
    DHLOGI("ReleaseSink");
    ScreenRegionManager::GetInstance().ReleaseAllRegions();
    DHLOGI("exit sink sa process");
    SetSinkProcessExit();
    return DH_SUCCESS;
}

int32_t DScreenSinkService::SubscribeLocalHardware(const std::string &dhId, const std::string &param)
{
    DHLOGI("SubscribeLocalHardware");
    (void)dhId;
    (void)param;
    return DH_SUCCESS;
}

int32_t DScreenSinkService::UnsubscribeLocalHardware(const std::string &dhId)
{
    DHLOGI("UnsubscribeLocalHardware");
    (void)dhId;
    return DH_SUCCESS;
}

void DScreenSinkService::DScreenNotify(const std::string &devId, int32_t eventCode, const std::string &eventContent)
{
    DHLOGI("DScreenNotify, devId:%s, eventCode: %d, eventContent:%s", GetAnonyString(devId).c_str(),
        eventCode, eventContent.c_str());
    ScreenRegionManager::GetInstance().HandleDScreenNotify(devId, eventCode, eventContent);
}

int32_t DScreenSinkService::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    DHLOGI("DScreenSinkService  Dump.");
    (void)args;
    std::string result;
    ScreenRegionManager::GetInstance().GetScreenDumpInfo(result);
    int ret = dprintf(fd, "%s\n", result.c_str());
    if (ret < 0) {
        DHLOGE("dprintf error");
        return ERR_DH_SCREEN_SA_HIDUMPER_ERROR;
    }

    return ERR_OK;
}
} // namespace DistributedHardware
} // namespace OHOS