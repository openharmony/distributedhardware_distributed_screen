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

#include "dscreen_hidumper.h"
#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_log.h"
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
    DHLOGI("Init dscreen sink service.");
    if (!registerToService_) {
        bool ret = Publish(this);
        if (!ret) {
            DHLOGE("dscreen sink publish service failed.");
            return false;
        }
        registerToService_ = true;
    }
    DHLOGI("dscreen sink service init success.");
    return true;
}

int32_t DScreenSinkService::InitSink(const std::string &params)
{
    DHLOGI("Init sink dscreen region manager, params: %{public}s", params.c_str());
    int32_t ret = V2_0::ScreenRegionManager::GetInstance().Initialize();
    if (ret != DH_SUCCESS) {
        DHLOGE("Initialize V2_0::ScreenRegionManage failed. err: %{public}d", ret);
    }
    return DH_SUCCESS;
}

int32_t DScreenSinkService::ReleaseSink()
{
    DHLOGI("Release sink dscreen region manager.");
    V2_0::ScreenRegionManager::GetInstance().Release();
    V1_0::ScreenRegionManager::GetInstance().ReleaseAllRegions();

    DHLOGI("exit sink sa process");
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        DHLOGE("systemAbilityMgr is null");
        return DSCREEN_INIT_ERR;
    }
    int32_t ret = systemAbilityMgr->UnloadSystemAbility(DISTRIBUTED_HARDWARE_SCREEN_SINK_SA_ID);
    if (ret != DH_SUCCESS) {
        DHLOGE("sink systemAbilityMgr UnLoadSystemAbility failed, ret: %{public}" PRId32, ret);
        return DSCREEN_BAD_VALUE;
    }
    DHLOGI("sink systemAbilityMgr UnLoadSystemAbility success");
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
    DHLOGI("DScreenNotify, devId:%{public}s, eventCode: %{public}" PRId32 ", eventContent:%{public}s",
        GetAnonyString(devId).c_str(), eventCode, eventContent.c_str());
    V1_0::ScreenRegionManager::GetInstance().HandleDScreenNotify(devId, eventCode, eventContent);
}

int32_t DScreenSinkService::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    DHLOGI("DScreenSinkService Dump.");
    (void)args;
    std::string result_v1;
    V1_0::ScreenRegionManager::GetInstance().GetScreenDumpInfo(result_v1);
    std::string result_v2;
    V2_0::ScreenRegionManager::GetInstance().GetScreenDumpInfo(result_v2);
    int ret = dprintf(fd, "%s\n", (result_v1 + result_v2).c_str());
    if (ret < 0) {
        DHLOGE("dprintf error");
        return ERR_DH_SCREEN_SA_HIDUMPER_ERROR;
    }

    std::string result;
    std::vector<std::string> argsStr;

    std::transform(args.cbegin(), args.cend(), std::back_inserter(argsStr),
        [](const std::u16string &item) { return Str16ToStr8(item); });

    if (!DscreenHidumper::GetInstance().Dump(argsStr, result)) {
        DHLOGE("Hidump error");
        return ERR_DH_SCREEN_SA_HIDUMPER_ERROR;
    }

    ret = dprintf(fd, "%s\n", result.c_str());
    if (ret < 0) {
        DHLOGE("Dprintf error");
        return DSCREEN_BAD_VALUE;
    }
    return ERR_OK;
}
} // namespace DistributedHardware
} // namespace OHOS