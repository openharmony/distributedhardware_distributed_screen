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

#include "screenregionmgr.h"

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
IMPLEMENT_SINGLE_INSTANCE(ScreenRegionManager);
ScreenRegionManager::ScreenRegionManager()
{
    DHLOGI("ScreenRegionManager");
}

ScreenRegionManager::~ScreenRegionManager()
{
    DHLOGI("~ScreenRegionManager");
}

int32_t ScreenRegionManager::ReleaseAllRegions()
{
    DHLOGI("ScreenRegionManager::ReleaseAllRegion");
    std::lock_guard<std::mutex> lock(screenRegionsMtx_);
    for (const auto &item : screenRegions_) {
        std::shared_ptr<ScreenRegion> screenRegion = item.second;
        if (screenRegion == nullptr) {
            continue;
        }
        int32_t ret = screenRegion->Stop();
        if (ret != DH_SUCCESS) {
            DHLOGE("Release region failed, remoteDevId: %s, err: %d",
                GetAnonyString(screenRegion->GetRemoteDevId()).c_str(), ret);
        }
    }
    screenRegions_.clear();
    return DH_SUCCESS;
}

void ScreenRegionManager::HandleDScreenNotify(const std::string &remoteDevId, int32_t eventCode,
    const std::string &eventContent)
{
    DHLOGI("HandleDScreenNotify, remoteDevId: %s, eventCode: %d", GetAnonyString(remoteDevId).c_str(), eventCode);
    if (eventCode == NOTIFY_SINK_SETUP) {
        HandleNotifySetUp(remoteDevId, eventContent);
        return;
    }
    DHLOGE("invalid event.");
}

void ScreenRegionManager::GetScreenDumpInfo(std::string &result)
{
    DHLOGI("GetScreenDumpInfo.");
    result.clear();
    result.append("screenRegion OnLine:\n[\n");
    if (screenRegions_.size() == 0) {
        result.append("]");
        DHLOGD("no screenRegion");
        return;
    }

    for (const auto &iter : screenRegions_) {
        result.append("    {\n");
        std::shared_ptr<ScreenRegion> screenRegion = iter.second;
        if (screenRegion == nullptr) {
            continue;
        }
        uint64_t screenId = screenRegion->GetScreenId();
        std::string remoteDevId = screenRegion->GetRemoteDevId();
        std::shared_ptr<VideoParam> videoParam = screenRegion->GetVideoParam();
        if (videoParam == nullptr) {
            continue;
        }
        uint32_t screenHeight = videoParam->GetScreenHeight();
        uint32_t screenWidth = videoParam->GetScreenWidth();
        int32_t windowId = screenRegion->GetWindowId();
        std::string screenInfo = "        \"clientWindowId\" : \"" + std::to_string(windowId) + "\",\n" +
                                 "        \"remoteScreenId\" : \"" + std::to_string(screenId) + "\",\n" +
                                 "        \"localDevId\" : \"" + GetAnonyString(localDevId_) + "\",\n" +
                                 "        \"remoteDevId\" : \"" + GetAnonyString(remoteDevId) + "\",\n" +
                                 "        \"screenWidth\" : \"" + std::to_string(screenWidth) + "\",\n" +
                                 "        \"screenHeight\" : \"" + std::to_string(screenHeight) + "\"\n";
        result.append(screenInfo);
    }
    result.append("    }\n]");
}

bool ScreenRegionManager::CheckContentJson(json &eventContentJson)
{
    if (!IsUInt64(eventContentJson, KEY_SCREEN_ID)) {
        return false;
    }
    if (!IsString(eventContentJson, KEY_DH_ID)) {
        return false;
    }
    return true;
}

void ScreenRegionManager::HandleNotifySetUp(const std::string &remoteDevId, const std::string &eventContent)
{
    DHLOGI("HandleNotifySetUp, remoteDevId: %s", GetAnonyString(remoteDevId).c_str());
    json eventContentJson = json::parse(eventContent, nullptr, false);
    if (eventContentJson.is_discarded()) {
        NotifyRemoteSourceSetUpResult(remoteDevId, "", ERR_DH_SCREEN_SA_SCREENREGION_SETUP_FAIL, "");
        return;
    }

    if (!CheckContentJson(eventContentJson) || !eventContentJson.contains(KEY_VIDEO_PARAM) ||
        !eventContentJson.contains(KEY_MAPRELATION)) {
        NotifyRemoteSourceSetUpResult(remoteDevId, "", ERR_DH_SCREEN_SA_SCREENREGION_SETUP_FAIL, "");
        return;
    }

    uint64_t screenId = eventContentJson[KEY_SCREEN_ID].get<uint64_t>();
    std::string dhId = eventContentJson[KEY_DH_ID].get<std::string>();

    std::shared_ptr<VideoParam> videoParam =
        std::make_shared<VideoParam>(eventContentJson[KEY_VIDEO_PARAM].get<VideoParam>());
    std::shared_ptr<DScreenMapRelation> mapRelation =
        std::make_shared<DScreenMapRelation>(eventContentJson[KEY_MAPRELATION].get<DScreenMapRelation>());

    uint64_t displayId = Rosen::DisplayManager::GetInstance().GetDefaultDisplayId();
    std::shared_ptr<ScreenRegion> screenRegion = std::make_shared<ScreenRegion>(remoteDevId, screenId, displayId);
    screenRegion->SetVideoParam(videoParam);
    screenRegion->SetMapRelation(mapRelation);

    int32_t ret = DH_SUCCESS;
    {
        std::lock_guard<std::mutex> lock(screenRegionsMtx_);
        if (screenRegions_.count(remoteDevId) != 0) {
            ret = screenRegions_[remoteDevId]->Stop();
        }

        if (ret != DH_SUCCESS) {
            DHLOGE("screenRegion stop failed, remoteDevId: %s, err: %d",
                GetAnonyString(screenRegions_[remoteDevId]->GetRemoteDevId()).c_str(), ret);
            NotifyRemoteSourceSetUpResult(remoteDevId, dhId, ERR_DH_SCREEN_SA_SCREENREGION_SETUP_FAIL, "");
            return;
        }
        screenRegions_[remoteDevId] = screenRegion;
    }

    ret = screenRegion->SetUp();
    if (ret != DH_SUCCESS) {
        DHLOGE("screen region setup failed");
        NotifyRemoteSourceSetUpResult(remoteDevId, dhId, ERR_DH_SCREEN_SA_SCREENREGION_SETUP_FAIL, "");
        return;
    }

    ret = screenRegion->Start();
    if (ret != DH_SUCCESS) {
        DHLOGE("screen region start failed");
        NotifyRemoteSourceSetUpResult(remoteDevId, dhId, ERR_DH_SCREEN_SA_SCREENREGION_START_FAIL, "");
        return;
    }

    PublishMessage(DHTopic::TOPIC_SINK_PROJECT_WINDOW_INFO, screenId, remoteDevId, screenRegion->GetWindowId(),
        screenRegion->GetWindowProperty());
    NotifyRemoteSourceSetUpResult(remoteDevId, dhId, DH_SUCCESS, "");
}

void ScreenRegionManager::NotifyRemoteSourceSetUpResult(const std::string &remoteDevId, const std::string &dhId,
    int32_t errCode, const std::string &errContent)
{
    DHLOGI("NotifyRemoteSourceSetUpResult, sourceDevId: %s, dhId: %s, errCode: %d",
        GetAnonyString(remoteDevId).c_str(), GetAnonyString(dhId).c_str(), errCode);
    int32_t eventCode = NOTIFY_SOURCE_SETUP_RESULT;

    json eventContentJson;
    eventContentJson[KEY_DH_ID] = dhId;
    eventContentJson[KEY_ERR_CODE] = errCode;
    eventContentJson[KEY_ERR_CONTENT] = errContent;

    std::string eventContent = eventContentJson.dump();

    NotifyRemoteScreenService(remoteDevId, dhId, eventCode, eventContent);
}

int32_t ScreenRegionManager::NotifyRemoteScreenService(const std::string &remoteDevId, const std::string &dhId,
    int32_t eventCode, const std::string &eventContent)
{
    DHLOGI("Notify remote source screen service, remote devId: %s, eventCode: %d",
        GetAnonyString(remoteDevId).c_str(), eventCode);
    sptr<IDScreenSource> remoteSourceSA = GetDScreenSourceSA(remoteDevId);
    if (remoteSourceSA == nullptr) {
        DHLOGE("get remote source sa failed.");
        return ERR_DH_SCREEN_SA_GET_REMOTE_SOURCE_SERVICE_FAIL;
    }
    std::string localDevId;
    int32_t ret = GetLocalDeviceNetworkId(localDevId);
    if (ret != DH_SUCCESS) {
        DHLOGE("notify remote screen service failed, cannot get local device id");
        return ret;
    }
    localDevId_ = localDevId;
    remoteSourceSA->DScreenNotify(localDevId, eventCode, eventContent);
    return DH_SUCCESS;
}

sptr<IDScreenSource> ScreenRegionManager::GetDScreenSourceSA(const std::string &devId)
{
    DHLOGI("GetDScreenSourceSA, devId: %s", GetAnonyString(devId).c_str());
    sptr<ISystemAbilityManager> samgr =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        DHLOGE("Failed to get system ability mgr.");
        return nullptr;
    }
    auto remoteObject = samgr->GetSystemAbility(DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, devId);
    if (remoteObject == nullptr) {
        DHLOGE("remoteObject is null");
        return nullptr;
    }

    sptr<IDScreenSource> remoteSourceSA = iface_cast<IDScreenSource>(remoteObject);
    if (remoteSourceSA == nullptr) {
        DHLOGE("Failed to get remote dscreen source sa");
        return nullptr;
    }
    return remoteSourceSA;
}

void ScreenRegionManager::PublishMessage(const DHTopic topic, const uint64_t &screenId,
    const std::string &remoteDevId, const int32_t &windowId, std::shared_ptr<WindowProperty> windowProperty)
{
    DHLOGI("ScreenRegionManager PublishMessage");
    if (DScreenFwkKit::GetInstance().GetDHFwkKit() == nullptr) {
        DHLOGE("GetDHFwkKit fail.");
        return;
    }

    json messageJosn;
    std::string message;
    messageJosn[SOURCE_WIN_ID] = screenId;
    messageJosn[SOURCE_DEV_ID] = remoteDevId;
    messageJosn[SINK_SHOW_WIN_ID] = windowId;
    messageJosn[SINK_PROJ_SHOW_WIDTH] = windowProperty->width;
    messageJosn[SINK_PROJ_SHOW_HEIGHT] = windowProperty->height;
    messageJosn[SINK_WIN_SHOW_X] = windowProperty->startX;
    messageJosn[SINK_WIN_SHOW_Y] = windowProperty->startY;
    message = messageJosn.dump();

    DScreenFwkKit::GetInstance().GetDHFwkKit()->PublishMessage(topic, message);
}
} // namespace DistributedHardware
} // namespace OHOS