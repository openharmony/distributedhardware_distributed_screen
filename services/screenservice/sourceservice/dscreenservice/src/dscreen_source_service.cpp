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

#include "dscreen_source_service.h"

#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_log.h"
#include "dscreen_sa_process_state.h"
#include "dscreen_util.h"

namespace OHOS {
namespace DistributedHardware {
REGISTER_SYSTEM_ABILITY_BY_ID(DScreenSourceService, DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, true);

DScreenSourceService::DScreenSourceService(int32_t saId, bool runOnCreate) : SystemAbility(saId, runOnCreate)
{
    DHLOGI("dscreen source service create.");
}

DScreenSourceService::~DScreenSourceService()
{
    DHLOGI("~DScreenSourceService.");
}

void DScreenSourceService::OnStart()
{
    DHLOGI("dscreen source service start.");
    Init();
}

void DScreenSourceService::OnStop()
{
    DHLOGI("dscreen source service stop.");
    int32_t ret = DScreenManager::GetInstance().UnInit();
    if (ret != DH_SUCCESS) {
        DHLOGE("UnInit DScreenManager failed. err: %d", ret);
    }
    registerToService_ = false;
}

bool DScreenSourceService::Init()
{
    DHLOGI("dscreen source service start init.");
    if (!registerToService_) {
        bool ret = Publish(this);
        if (!ret) {
            DHLOGE("dscreen source publish service failed.");
            return false;
        }
        registerToService_ = true;
    }
    DHLOGI("dscreen init success.");
    return true;
}

int32_t DScreenSourceService::InitSource(const std::string &params, const sptr<IDScreenSourceCallback> &callback)
{
    DHLOGI("InitSource");
    int32_t ret = DScreenManager::GetInstance().Init();
    if (ret != DH_SUCCESS) {
        DHLOGE("Init DScreenManager failed. err: %d", ret);
        return ret;
    }

    DScreenManager::GetInstance().RegisterDScreenCallback(callback);
    return DH_SUCCESS;
}

int32_t DScreenSourceService::ReleaseSource()
{
    DHLOGI("ReleaseSource");
    int32_t ret = DScreenManager::GetInstance().UnInit();
    if (ret != DH_SUCCESS) {
        DHLOGE("UnInit DScreenManager failed. err: %d", ret);
        return ret;
    }
    DHLOGI("exit source sa process");
    SetSourceProcessExit();
    return DH_SUCCESS;
}

int32_t DScreenSourceService::RegisterDistributedHardware(const std::string &devId, const std::string &dhId,
    const EnableParam &param, const std::string &reqId)
{
    std::string attrs = param.attrs;
    int ret = DScreenManager::GetInstance().EnableDistributedScreen(devId, dhId, attrs, reqId);
    if (ret != DH_SUCCESS) {
        DHLOGE("enable distributedScreen failed. devId: %s, dhId: %s, reqId: %s, attrs: %s",
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), reqId.c_str(), attrs.c_str());
        ReportRegisterFail(DSCREEN_REGISTER_FAIL, ret, GetAnonyString(devId).c_str(),
            GetAnonyString(dhId).c_str(), "enable distributedScreen failed.");
        return ERR_DH_SCREEN_SA_ENABLE_FAILED;
    }
    return DH_SUCCESS;
}

int32_t DScreenSourceService::UnregisterDistributedHardware(const std::string &devId, const std::string &dhId,
    const std::string &reqId)
{
    int ret = DScreenManager::GetInstance().DisableDistributedScreen(devId, dhId, reqId);
    if (ret != DH_SUCCESS) {
        DHLOGE("disable distributedScreen failed. devId: %s, dhId: %s, reqId: %s",
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), reqId.c_str());
        ReportUnRegisterFail(DSCREEN_REGISTER_FAIL, ret, GetAnonyString(devId).c_str(),
            GetAnonyString(dhId).c_str(), "disable distributedScreen failed.");
        return ERR_DH_SCREEN_SA_DISABLE_FAILED;
    }
    return DH_SUCCESS;
}

int32_t DScreenSourceService::ConfigDistributedHardware(const std::string &devId, const std::string &dhId,
    const std::string &key, const std::string &value)
{
    (void)devId;
    (void)dhId;
    (void)key;
    (void)value;
    return DH_SUCCESS;
}

void DScreenSourceService::DScreenNotify(const std::string &devId, const int32_t eventCode,
    const std::string &eventContent)
{
    DHLOGI("DScreenNotify, devId: %s, eventCode: %d", GetAnonyString(devId).c_str(), eventCode);
    DScreenManager::GetInstance().HandleDScreenNotify(devId, eventCode, eventContent);
}

int32_t DScreenSourceService::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    DHLOGI("DScreenSourceService  Dump.");
    (void)args;
    std::string result;
    DScreenManager::GetInstance().GetScreenDumpInfo(result);
    int ret = dprintf(fd, "%s\n", result.c_str());
    if (ret < 0) {
        DHLOGE("dprintf error");
        return ERR_DH_SCREEN_SA_HIDUMPER_ERROR;
    }

    return ERR_OK;
}
} // namespace DistributedHardware
} // namespace OHOS