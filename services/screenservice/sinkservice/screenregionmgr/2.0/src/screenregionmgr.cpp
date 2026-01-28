/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "2.0/include/screenregionmgr.h"

#include <cinttypes>
#include <dlfcn.h>
#include <fstream>
#include <sstream>

#include "display_manager.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "nlohmann/json.hpp"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_fwkkit.h"
#include "dscreen_json_util.h"
#include "dscreen_log.h"
#include "dscreen_maprelation.h"
#include "dscreen_util.h"
#include "idscreen_source.h"
#include "screen_client.h"
#include "screen_client_common.h"

using json = nlohmann::json;

namespace OHOS {
namespace DistributedHardware {
namespace V2_0 {
FWK_IMPLEMENT_SINGLE_INSTANCE(ScreenRegionManager);

using AVTransProviderClass = IAVEngineProvider *(*)(const std::string);

const std::string RECEIVER_SO_NAME = "libdistributed_av_receiver.z.so";
const std::string GET_PROVIDER_FUNC = "GetAVReceiverEngineProvider";

ScreenRegionManager::ScreenRegionManager()
{
    DHLOGI("ScreenRegionManager");
}

ScreenRegionManager::~ScreenRegionManager()
{
    DHLOGI("~ScreenRegionManager");
}

int32_t ScreenRegionManager::Initialize()
{
    DHLOGI("Enter ScreenRegionManager::Initialize 2.0");
    int32_t ret = LoadAVReceiverEngineProvider();
    if ((ret != DH_SUCCESS) || (providerPtr_ == nullptr)) {
        DHLOGE("Load av transport receiver engine provider failed.");
        return ERR_DH_AV_TRANS_LOAD_ERROR;
    }
    return providerPtr_->RegisterProviderCallback(std::make_shared<EngineProviderListener>());
}

int32_t ScreenRegionManager::Release()
{
    DHLOGI("ScreenRegionManager::ReleaseAllRegion");
    UnloadAVReceiverEngineProvider();
    std::lock_guard<std::mutex> lock(screenRegionsMtx_);
    for (const auto &screenRegion : screenRegions_) {
        if (screenRegion == nullptr) {
            continue;
        }
        int32_t ret = screenRegion->Release();
        if (ret != DH_SUCCESS) {
            DHLOGE("Release region failed, remoteDevId: %{public}s, err: %{public}" PRId32,
                GetAnonyString(screenRegion->GetRemoteDevId()).c_str(), ret);
        }
    }
    screenRegions_.clear();
    providerPtr_ = nullptr;
    return DH_SUCCESS;
}

int32_t ScreenRegionManager::CreateDScreenRegion(const std::string &peerDevId)
{
    DHLOGI("CreateDScreenRegion for peerDevId: %{public}s", GetAnonyString(peerDevId).c_str());
    auto screenRegion = std::make_shared<ScreenRegion>(peerDevId);
    screenRegion->InitReceiverEngine(providerPtr_);
    screenRegions_.push_back(screenRegion);
    return DH_SUCCESS;
}

int32_t ScreenRegionManager::DestoryDScreenRegion(const std::string &peerDevId)
{
    DHLOGI("DestoryDScreenRegion for peerDevId: %{public}s", GetAnonyString(peerDevId).c_str());
    std::lock_guard<std::mutex> lock(screenRegionsMtx_);
    for (const auto &region : screenRegions_) {
        if (region != nullptr) {
            region->Release();
        }
    }
    screenRegions_.clear();
    return DH_SUCCESS;
}

void ScreenRegionManager::GetScreenDumpInfo(std::string &result)
{
    DHLOGI("GetScreenDumpInfo.");

    result.clear();
    result.append("screenRegion OnLine:\n[\n");

    if (screenRegions_.empty()) {
        result.append("]");
        DHLOGD("no screenRegion");
        return;
    }

    std::stringstream screenInfo;
    std::string localDevIdAnony = GetAnonyString(localDevId_);
    for (const auto &screenRegion : screenRegions_) {
        if (!screenRegion) {
            continue;
        }
        uint64_t screenId = screenRegion->GetScreenId();
        std::string remoteDevId = screenRegion->GetRemoteDevId();
        std::shared_ptr<VideoParam> videoParam = screenRegion->GetVideoParam();
        if (!videoParam) {
            continue;
        }
        uint32_t screenHeight = videoParam->GetScreenHeight();
        uint32_t screenWidth = videoParam->GetScreenWidth();
        int32_t windowId = screenRegion->GetWindowId();
        screenInfo << "    {\n"
                   << "        \"clientWindowId\" : \"" << windowId << "\",\n"
                   << "        \"remoteScreenId\" : \"" << screenId << "\",\n"
                   << "        \"localDevId\" : \"" << localDevIdAnony << "\",\n"
                   << "        \"remoteDevId\" : \"" << GetAnonyString(remoteDevId) << "\",\n"
                   << "        \"screenWidth\" : \"" << screenWidth << "\",\n"
                   << "        \"screenHeight\" : \"" << screenHeight << "\"\n"
                   << "    }\n";
    }

    result.append(screenInfo.str());
    result.append("]");
}

int32_t ScreenRegionManager::LoadAVReceiverEngineProvider()
{
    DHLOGI("LoadAVReceiverEngineProvider enter");
    pHandler_ = dlopen(RECEIVER_SO_NAME.c_str(), RTLD_LAZY | RTLD_NODELETE);
    if (pHandler_ == nullptr) {
        DHLOGE("so: %{public}s load failed, failed reason : %{public}s", RECEIVER_SO_NAME.c_str(), dlerror());
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    AVTransProviderClass getEngineFactoryFunc = (AVTransProviderClass)dlsym(pHandler_, GET_PROVIDER_FUNC.c_str());
    if (getEngineFactoryFunc == nullptr) {
        DHLOGE("av transport engine factory function handler is null, failed reason : %{public}s", dlerror());
        dlclose(pHandler_);
        pHandler_ = nullptr;
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    providerPtr_ = getEngineFactoryFunc(OWNER_NAME_D_SCREEN);
    return DH_SUCCESS;
}

int32_t ScreenRegionManager::UnloadAVReceiverEngineProvider()
{
    DHLOGI("UnloadAVReceiverEngineProvider enter");
    if (providerPtr_ != nullptr) {
        delete providerPtr_;
        providerPtr_ = nullptr;
    }
    if (pHandler_ != nullptr) {
        dlclose(pHandler_);
        pHandler_ = nullptr;
    }
    return DH_SUCCESS;
}

int32_t EngineProviderListener::OnProviderEvent(const AVTransEvent& event)
{
    DHLOGI("OnProviderEvent enter. event type:%{public}" PRId32, event.type);
    if (event.type == EventType::EVENT_CHANNEL_OPENED) {
        ScreenRegionManager::GetInstance().CreateDScreenRegion(event.peerDevId);
    } else if (event.type == EventType::EVENT_CHANNEL_CLOSED) {
        ScreenRegionManager::GetInstance().DestoryDScreenRegion(event.peerDevId);
    } else {
        DHLOGE("Invalid event type.");
    }
    return DH_AVT_SUCCESS;
}
} // namespace V2_0
} // namespace DistributedHardware
} // namespace OHOS
