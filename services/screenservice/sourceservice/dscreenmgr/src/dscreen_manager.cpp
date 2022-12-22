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

#include "dscreen_manager.h"

#include <map>

#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "nlohmann/json.hpp"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_fwkkit.h"
#include "dscreen_json_util.h"
#include "dscreen_log.h"
#include "dscreen_util.h"
#include "idscreen_sink.h"
#include "screen_manager_adapter.h"

using json = nlohmann::json;

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(DScreenManager);

const std::map<DScreenState, std::string> stateMap = {
    { DISABLED, "disabled" },
    { ENABLED, "enabled" },
    { DISABLING, "disabling" },
    { ENABLING, "enabling" },
    { CONNECTING, "connecting" },
    { CONNECTED, "connected" },
    { DISCONNECTING, "disconnecting" }
};

DScreenManager::DScreenManager()
{
    DHLOGI("DScreenMgr construct.");
}

DScreenManager::~DScreenManager()
{
    DHLOGI("DScreenMgr deConstruct.");
}

int32_t DScreenManager::Init()
{
    DHLOGI("DScreenManager::Init");
    if (dScreenGroupListener_ == nullptr) {
        dScreenGroupListener_ = new (std::nothrow) DScreenGroupListener();
        int32_t ret = ScreenMgrAdapter::GetInstance().RegisterScreenGroupListener(dScreenGroupListener_);
        if (ret != DH_SUCCESS) {
            DHLOGE("DScreenManager Init failed, err: %d", ret);
            delete dScreenGroupListener_;
            dScreenGroupListener_ = nullptr;
            return ret;
        }
    }

    if (dScreenCallback_ == nullptr) {
        dScreenCallback_ = std::make_shared<DScreenCallback>();
    }

    return DH_SUCCESS;
}

int32_t DScreenManager::UnInit()
{
    DHLOGI("DScreenManager::UnInit");
    int32_t ret = ScreenMgrAdapter::GetInstance().UnregisterScreenGroupListener(dScreenGroupListener_);
    if (ret != DH_SUCCESS) {
        DHLOGE("DScreenManager UnInit failed, err: %d", ret);
    }

    dScreenCallback_ = nullptr;

    {
        std::lock_guard<std::mutex> lock(dScreenMapMtx_);
        dScreens_.clear();
    }

    {
        std::lock_guard<std::mutex> lock(dScreenMapRelationMtx_);
        mapRelations_.clear();
    }
    DHLOGI("DScreenManager::UnInit success");
    return ret;
}

void DScreenGroupListener::OnChange(const std::vector<uint64_t> &screenIds, Rosen::ScreenGroupChangeEvent event)
{
    DHLOGI("On Screen change, screenIds size: %d", screenIds.size());
    for (uint64_t screenId : screenIds) {
        std::shared_ptr<DScreen> changedScreen = DScreenManager::GetInstance().FindDScreenByScreenId(screenId);
        if (changedScreen == nullptr) {
            DHLOGD("screen change not about remote screen, screenId: %ulld", screenId);
            continue;
        }
        DScreenManager::GetInstance().HandleScreenChange(changedScreen, event);
    }
}

void DScreenManager::HandleScreenChange(const std::shared_ptr<DScreen> &changedScreen,
    Rosen::ScreenGroupChangeEvent event)
{
    if (changedScreen == nullptr) {
        DHLOGE("DScreenManager::HandleScreenChange, dScreen is null.");
        return;
    }

    uint64_t screenId = changedScreen->GetScreenId();
    DHLOGI("DScreenManager::HandleScreenChange, screenId: %ulld, changeEvent: %", screenId, event);
    if (event == Rosen::ScreenGroupChangeEvent::ADD_TO_GROUP) {
        if (DH_SUCCESS != AddToGroup(changedScreen, screenId)) {
            DHLOGE("AddToGroup failed.");
            return;
        }
        NotifyRemoteSinkSetUp(changedScreen);
        PublishMessage(DHTopic::TOPIC_START_DSCREEN, changedScreen);
    } else if (event == Rosen::ScreenGroupChangeEvent::REMOVE_FROM_GROUP) {
        if (DH_SUCCESS != RemoveFromGroup(changedScreen, screenId)) {
            DHLOGE("RemoveFromGroup failed.");
        }
        PublishMessage(DHTopic::TOPIC_STOP_DSCREEN, changedScreen);
    } else if (event == Rosen::ScreenGroupChangeEvent::CHANGE_GROUP) {
        DHLOGE("CHANGE_GROUP not implement.");
    } else {
        DHLOGE("unknown change type.");
    }
}

int32_t DScreenManager::AddToGroup(const std::shared_ptr<DScreen> &changedScreen, uint64_t screenId)
{
    DHLOGI("DScreenManager::ADDToGroup, screenId: %ulld", screenId);
    if (changedScreen == nullptr) {
        DHLOGE("DScreenManager::ADDToGroup, dScreen is null.");
        return ERR_DH_SCREEN_SA_VALUE_NOT_INIT;
    }

    if (changedScreen->GetState() == CONNECTING) {
        DHLOGD("screen is connecting, no need handle change");
        return DH_SUCCESS;
    }
    std::shared_ptr<DScreenMapRelation> mapRelation = ScreenMgrAdapter::GetInstance().GetMapRelation(screenId);
    if (mapRelation == nullptr) {
        DHLOGE("mapRelation construct failed. screenId: %ulld", screenId);
        return ERR_DH_SCREEN_SA_VALUE_NOT_INIT;
    }

    std::shared_ptr<VideoParam> videoParam = changedScreen->GetVideoParam();
    DisplayRect displayRect = mapRelation->GetDisplayRect();
    videoParam->SetVideoWidth(displayRect.width);
    videoParam->SetVideoHeight(displayRect.height);
    changedScreen->SetState(CONNECTING);

    {
        std::lock_guard<std::mutex> lock(dScreenMapRelationMtx_);
        mapRelations_[screenId] = mapRelation;
    }
    return DH_SUCCESS;
}

int32_t DScreenManager::RemoveFromGroup(const std::shared_ptr<DScreen> &changedScreen, uint64_t screenId)
{
    DHLOGI("DScreenManager::RemoveFromGroup, screenId: %ulld", screenId);
    if (changedScreen == nullptr) {
        DHLOGE("DScreenManager::RemoveFromGroup, dScreen is null.");
        return ERR_DH_SCREEN_SA_VALUE_NOT_INIT;
    }

    if (changedScreen->GetState() == DISCONNECTING) {
        DHLOGD("screen is disconnecting, no need handle change");
        return DH_SUCCESS;
    }

    {
        std::lock_guard<std::mutex> lock(dScreenMapRelationMtx_);
        mapRelations_.erase(screenId);
    }
    changedScreen->AddTask(std::make_shared<Task>(TaskType::TASK_DISCONNECT, ""));
    return DH_SUCCESS;
}

void DScreenCallback::OnRegResult(const std::shared_ptr<DScreen> &dScreen,
    const std::string &reqId, const int32_t status, const std::string &data)
{
    DHLOGI("DScreenCallback::OnRegResult, devId: %s, dhId: %s, reqId: %s",
        GetAnonyString(dScreen->GetDevId()).c_str(), GetAnonyString(dScreen->GetDHId()).c_str(), reqId.c_str());
    DScreenManager::GetInstance().OnRegResult(dScreen, reqId, status, data);
}

void DScreenCallback::OnUnregResult(const std::shared_ptr<DScreen> &dScreen,
    const std::string &reqId, const int32_t status, const std::string &data)
{
    DHLOGI("DScreenCallback::OnUnregResult, devId: %s, dhId: %s, reqId: %s",
        GetAnonyString(dScreen->GetDevId()).c_str(), GetAnonyString(dScreen->GetDHId()).c_str(), reqId.c_str());
    DScreenManager::GetInstance().OnUnregResult(dScreen, reqId, status, data);
}

void DScreenManager::OnRegResult(const std::shared_ptr<DScreen> &dScreen,
    const std::string &reqId, const int32_t status, const std::string &data)
{
    DHLOGI("DScreenManager::OnRegResult, devId: %s, dhId: %s, reqId: %s",
        GetAnonyString(dScreen->GetDevId()).c_str(), GetAnonyString(dScreen->GetDHId()).c_str(), reqId.c_str());
    if (dScreenSourceCallbackProxy_ == nullptr) {
        DHLOGE("dScreenSourceCallbackProxy is null");
        return;
    }
    dScreenSourceCallbackProxy_->OnNotifyRegResult(dScreen->GetDevId(), dScreen->GetDHId(), reqId, status, data);
}

void DScreenManager::OnUnregResult(const std::shared_ptr<DScreen> &dScreen,
    const std::string &reqId, const int32_t status, const std::string &data)
{
    DHLOGI("DScreenManager::OnUnregResult, devId: %s, dhId: %s, reqId: %s",
        GetAnonyString(dScreen->GetDevId()).c_str(), GetAnonyString(dScreen->GetDHId()).c_str(), reqId.c_str());
    if (dScreenSourceCallbackProxy_ == nullptr) {
        DHLOGE("dScreenSourceCallbackProxy is null");
        return;
    }
    dScreenSourceCallbackProxy_->OnNotifyUnregResult(dScreen->GetDevId(), dScreen->GetDHId(), reqId, status, data);
}

int32_t DScreenManager::EnableDistributedScreen(const std::string &devId, const std::string &dhId,
    const std::string &attrs, const std::string &reqId)
{
    DHLOGI("EnableDistributedScreen, devId: %s, dhId:%s",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
    if (dScreenCallback_ == nullptr) {
        DHLOGE("dscreen manager not init.");
        return ERR_DH_SCREEN_SA_ENABLE_FAILED;
    }

    std::string dScreenIdx = devId + SEPERATOR + dhId;
    std::lock_guard<std::mutex> lock(dScreenMapMtx_);
    std::shared_ptr<DScreen> dScreen = dScreens_[dScreenIdx];
    if (dScreen == nullptr) {
        dScreen = std::make_shared<DScreen>(devId, dhId, dScreenCallback_);
    }

    int32_t dScreenState = dScreen->GetState();
    if (dScreenState == ENABLED || dScreenState == ENABLING) {
        DHLOGI("dScreen state Already is ENABLED or ENABLING.");
        return DH_SUCCESS;
    }

    dScreens_[dScreenIdx] = dScreen;
    int32_t ret = dScreen->AddTask(std::make_shared<Task>(TaskType::TASK_ENABLE, reqId, attrs));
    if (ret != DH_SUCCESS) {
        DHLOGE("EnableDistributedScreen, add task failed. devId: %s, dhId:%s",
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
    }

    return ret;
}

int32_t DScreenManager::DisableDistributedScreen(const std::string &devId, const std::string &dhId,
    const std::string &reqId)
{
    DHLOGI("DisableDistributedScreen, devId: %s, dhId:%s",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());

    std::string dScreenIdx = devId + SEPERATOR + dhId;
    std::lock_guard<std::mutex> lock(dScreenMapMtx_);
    if (dScreens_.count(dScreenIdx) == 0) {
        DHLOGE("dscreen not found, devId: %s, dhId: %s",
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
        return ERR_DH_SCREEN_SA_DISABLE_FAILED;
    }

    int32_t dScreenState = dScreens_[dScreenIdx]->GetState();
    int32_t ret = DH_SUCCESS;
    switch (dScreenState) {
        case DISABLED:
        case DISABLING:
            DHLOGE("dScreen state is invalid.");
            ret = ERR_DH_SCREEN_SA_DISABLE_FAILED;
            break;
        case ENABLED:
        case ENABLING:
            ret = dScreens_[dScreenIdx]->AddTask(std::make_shared<Task>(TaskType::TASK_DISABLE, reqId, ""));
            break;
        case CONNECTING:
        case CONNECTED:
        case DISCONNECTING:
            ret = dScreens_[dScreenIdx]->AddTask(std::make_shared<Task>(TaskType::TASK_DISCONNECT, ""));
            if (ret == DH_SUCCESS) {
                ret = dScreens_[dScreenIdx]->AddTask(std::make_shared<Task>(TaskType::TASK_DISABLE, reqId, ""));
            }
            break;
        default:
            ret = ERR_DH_SCREEN_SA_DISABLE_FAILED;
            break;
    }
    return ret;
}

void DScreenManager::RegisterDScreenCallback(const sptr<IDScreenSourceCallback> &callback)
{
    DHLOGI("RegisterDScreenCallback");
    dScreenSourceCallbackProxy_ = callback;
}

std::shared_ptr<DScreen> DScreenManager::FindDScreenByScreenId(uint64_t screenId)
{
    DHLOGD("FindDScreenByScreenId, screenId: %ulld", screenId);
    std::lock_guard<std::mutex> lock(dScreenMapMtx_);
    for (const auto &iter : dScreens_) {
        std::shared_ptr<DScreen> dScreen = iter.second;
        if (dScreen == nullptr) {
            continue;
        }

        if (dScreen->GetScreenId() == screenId) {
            return dScreen;
        }
    }
    DHLOGD("DScreen not found, screenId: %ulld", screenId);
    return nullptr;
}

void DScreenManager::GetScreenDumpInfo(std::string &result)
{
    DHLOGI("GetScreenDumpInfo.");
    result.clear();
    result.append("RemoteScreens OnLine:\n[\n");
    std::lock_guard<std::mutex> lock(dScreenMapMtx_);
    if (dScreens_.size() == 0) {
        result.append("]");
        DHLOGD("no virtualscreen");
        return;
    }

    for (const auto &iter : dScreens_) {
        result.append("    {\n");
        std::shared_ptr<DScreen> dScreen = iter.second;
        if (dScreen == nullptr) {
            continue;
        }
        uint64_t screenId = dScreen->GetScreenId();
        std::string devId = dScreen->GetDevId();
        std::shared_ptr<VideoParam> videoParam = dScreen->GetVideoParam();
        if (videoParam == nullptr) {
            continue;
        }
        uint32_t screenHeight = videoParam->GetScreenHeight();
        uint32_t screenWidth = videoParam->GetScreenWidth();
        DScreenState state = dScreen->GetState();
        std::string screenState =
            stateMap.find(state) == stateMap.end() ? "unknown state" : stateMap.find(state)->second;
        std::string screenInfo = "        \"virtualScreenId\" : \"" + std::to_string(screenId) + "\",\n" +
                                 "        \"localDevId\" : \"" + GetAnonyString(localDevId_) + "\",\n" +
                                 "        \"remoteDevId\" : \"" + GetAnonyString(devId) + "\",\n" +
                                 "        \"screenWidth\" : \"" + std::to_string(screenWidth) + "\",\n" +
                                 "        \"screenHeight\" : \"" + std::to_string(screenHeight) + "\",\n" +
                                 "        \"state\" : \"" + screenState + "\"\n";
        result.append(screenInfo);
    }
    result.append("    }\n]");
}

void DScreenManager::HandleDScreenNotify(const std::string &devId, int32_t eventCode,
    const std::string &eventContent)
{
    DHLOGI("HandleDScreenNotify, devId: %s, eventCode: %d", GetAnonyString(devId).c_str(), eventCode);
    if (eventCode == NOTIFY_SOURCE_SETUP_RESULT) {
        HandleNotifySetUpResult(devId, eventContent);
        return;
    }

    DHLOGE("invalid eventCode, eventCode: %d", eventCode);
}

int32_t DScreenManager::NotifyRemoteScreenService(const std::string &devId, int32_t eventCode,
    const std::string &eventContent)
{
    DHLOGI("Notify remote sink screen service, remote devId: %s, eventCode: %d",
        GetAnonyString(devId).c_str(), eventCode);
    sptr<IDScreenSink> remoteSinkSA = GetDScreenSinkSA(devId);
    if (remoteSinkSA == nullptr) {
        DHLOGE("get remote sink sa failed.");
        return ERR_DH_SCREEN_SA_GET_REMOTE_SINK_SERVICE_FAIL;
    }
    std::string localDevId;
    int32_t ret = GetLocalDeviceNetworkId(localDevId);
    if (ret != DH_SUCCESS) {
        DHLOGE("notify remote screen service failed, cannot get local device id");
        return ret;
    }
    localDevId_ = localDevId;
    remoteSinkSA->DScreenNotify(localDevId, eventCode, eventContent);
    return DH_SUCCESS;
}

sptr<IDScreenSink> DScreenManager::GetDScreenSinkSA(const std::string &devId)
{
    DHLOGI("GetDScreenSinkSA, devId: %s", GetAnonyString(devId).c_str());
    sptr<ISystemAbilityManager> samgr =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        DHLOGE("Failed to get system ability mgr.");
        return nullptr;
    }
    auto remoteObject = samgr->GetSystemAbility(DISTRIBUTED_HARDWARE_SCREEN_SINK_SA_ID, devId);
    if (remoteObject == nullptr) {
        DHLOGE("remoteObject is null");
        return nullptr;
    }

    sptr<IDScreenSink> remoteSinkSA = iface_cast<IDScreenSink>(remoteObject);
    if (remoteSinkSA == nullptr) {
        DHLOGE("Failed to get remote dscreen sink sa");
        return nullptr;
    }
    return remoteSinkSA;
}

void DScreenManager::PublishMessage(const DHTopic topic, const std::shared_ptr<DScreen> &dScreen)
{
    DHLOGD("PublishMessage");
    if (DScreenFwkKit::GetInstance().GetDHFwkKit() == nullptr) {
        DHLOGE("GetDHFwkKit fail.");
        return;
    }

    json messageJosn;
    std::string message;
    if (topic == DHTopic::TOPIC_START_DSCREEN) {
        messageJosn[SOURCE_WIN_ID] = dScreen->GetScreenId();
        messageJosn[SINK_DEV_ID] = dScreen->GetDevId();
        std::shared_ptr<VideoParam> videoParam = dScreen->GetVideoParam();
        if (videoParam == nullptr) {
            DHLOGE("videoParam is nullptr");
            return;
        }
        messageJosn[SOURCE_WIN_WIDTH] = videoParam->GetScreenWidth();
        messageJosn[SOURCE_WIN_HEIGHT] = videoParam->GetScreenHeight();
        message = messageJosn.dump();
    } else if (topic == DHTopic::TOPIC_STOP_DSCREEN) {
        messageJosn[SOURCE_WIN_ID] = dScreen->GetScreenId();
        messageJosn[SINK_DEV_ID] = dScreen->GetDevId();
        message = messageJosn.dump();
    }

    DScreenFwkKit::GetInstance().GetDHFwkKit()->PublishMessage(topic, message);
}

void DScreenManager::NotifyRemoteSinkSetUp(const std::shared_ptr<DScreen> &dScreen)
{
    DHLOGI("NotifyRemoteSinkSetUp");
    int32_t eventCode = NOTIFY_SINK_SETUP;
    std::string devId = dScreen->GetDevId();
    json eventContentJson;
    eventContentJson[KEY_SCREEN_ID] = dScreen->GetScreenId();
    eventContentJson[KEY_DH_ID] = dScreen->GetDHId();
    if (dScreen->GetVideoParam() == nullptr) {
        DHLOGE("videoParam is null, back to enabled state screedId: %ulld", dScreen->GetScreenId());
        dScreen->SetState(ENABLED);
        return;
    }
    eventContentJson[KEY_VIDEO_PARAM] = *(dScreen->GetVideoParam());

    if (mapRelations_.count(dScreen->GetScreenId()) == 0) {
        DHLOGE("mapRelation not found, back to enabled state screedId: %ulld", dScreen->GetScreenId());
        dScreen->SetState(ENABLED);
        return;
    }
    eventContentJson[KEY_MAPRELATION] = *(mapRelations_[dScreen->GetScreenId()]);

    std::string eventContent = eventContentJson.dump();
    DHLOGD("start notify remote screen, eventContent: %s", eventContent.c_str());
    NotifyRemoteScreenService(devId, eventCode, eventContent);
}

bool DScreenManager::CheckContent(json &eventContent)
{
    if (!IsString(eventContent, KEY_DH_ID)) {
        return false;
    }
    if (!IsInt32(eventContent, KEY_ERR_CODE)) {
        return false;
    }
    if (!IsString(eventContent, KEY_ERR_CONTENT)) {
        return false;
    }
    return true;
}

void DScreenManager::HandleNotifySetUpResult(const std::string &remoteDevId, const std::string &eventContent)
{
    DHLOGI("HandleNotifySetUpResult, remoteDevId:%s", GetAnonyString(remoteDevId).c_str());
    json eventContentJson = json::parse(eventContent, nullptr, false);
    if (eventContentJson.is_discarded()) {
        DHLOGE("HandleNotifySetUpResult, eventContent is invalid");
        return;
    }

    if (!CheckContent(eventContentJson)) {
        DHLOGE("HandleNotifySetUpResult, eventContent is invalid");
        return;
    }

    std::string dhId = eventContentJson[KEY_DH_ID].get<std::string>();
    int32_t errCode = eventContentJson[KEY_ERR_CODE].get<int32_t>();
    std::string errContent = eventContentJson[KEY_ERR_CONTENT].get<std::string>();

    std::string dScreenIdx = remoteDevId + SEPERATOR + dhId;
    std::lock_guard<std::mutex> lock(dScreenMapMtx_);
    if (dScreens_.count(dScreenIdx) == 0) {
        DHLOGE("dScreen not found, remoteDevId:%s, dhId:%s",
            GetAnonyString(remoteDevId).c_str(), GetAnonyString(dhId).c_str());
        return;
    }

    if (errCode != DH_SUCCESS) {
        DHLOGE("remote sink set up failed, errCode: %d, reason: %s", errCode, errContent.c_str());
        dScreens_[dScreenIdx]->SetState(ENABLED);
        return;
    }

    dScreens_[dScreenIdx]->AddTask(std::make_shared<Task>(TaskType::TASK_CONNECT, ""));
}
} // namespace DistributedHardware
} // namespace OHOS