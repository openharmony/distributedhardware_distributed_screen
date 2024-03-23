/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "2.0/include/dscreen_manager.h"

#include <cinttypes>
#include <dlfcn.h>
#include <fstream>
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
#include "common/include/screen_manager_adapter.h"

using json = nlohmann::json;

namespace OHOS {
namespace DistributedHardware {
namespace V2_0 {
IMPLEMENT_SINGLE_INSTANCE(DScreenManager);

using AVTransProviderClass = IAVEngineProvider *(*)(const std::string);

const std::string SENDER_SO_NAME = "libdistributed_av_sender.z.so";
const std::string GET_PROVIDER_FUNC = "GetAVSenderEngineProvider";
#ifdef __LP64__
const std::string LIB_LOAD_PATH = "/system/lib64/";
#else
const std::string LIB_LOAD_PATH = "/system/lib/";
#endif

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

int32_t DScreenManager::Initialize()
{
    DHLOGI("DScreenManager::Init2.0");
    int32_t ret = LoadAVSenderEngineProvider();
    if (ret != DH_SUCCESS) {
        DHLOGE("Load av transport sender engine provider failed.");
    }
    return ret;
}

int32_t DScreenManager::Release()
{
    DHLOGI("DScreenManager::UnInit");
    UnloadAVSenderEngineProvider();
    ScreenMgrAdapter::GetInstance().UnregisterScreenGroupListener(dScreenGroupListener_);
    {
        std::lock_guard<std::mutex> lock(dScreenMapMtx_);
        dScreens_.clear();
    }
    providerPtr_ = nullptr;
    dScreenCallback_ = nullptr;
    DHLOGI("DScreenManager::UnInit success");
    return DH_SUCCESS;
}

void DScreenGroupListener::OnChange(const std::vector<uint64_t> &screenIds, Rosen::ScreenGroupChangeEvent event)
{
    DHLOGI("On Screen change, screenIds size: %{public}zu", screenIds.size());
    for (uint64_t screenId : screenIds) {
        std::shared_ptr<DScreen> changedScreen = DScreenManager::GetInstance().FindDScreenById(screenId);
        if (changedScreen == nullptr) {
            DHLOGD("screen change not about remote screen, screenId: %{public}" PRIu64, screenId);
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
    DHLOGI("DScreenManager::HandleScreenChange, screenId: %{public}" PRIu64 " changeEvent: %{public}" PRIu32,
        changedScreen->GetScreenId(), event);
    if (event == Rosen::ScreenGroupChangeEvent::ADD_TO_GROUP) {
        if (StartDScreenMirror(changedScreen) != DH_SUCCESS) {
            DHLOGE("start dScreen mirror failed.");
            return;
        }
        PublishMessage(DHTopic::TOPIC_START_DSCREEN, changedScreen);
    } else if (event == Rosen::ScreenGroupChangeEvent::REMOVE_FROM_GROUP) {
        if (StopDScreenMirror(changedScreen) != DH_SUCCESS) {
            DHLOGE("stop dScreen mirror failed.");
        }
        PublishMessage(DHTopic::TOPIC_STOP_DSCREEN, changedScreen);
    } else if (event == Rosen::ScreenGroupChangeEvent::CHANGE_GROUP) {
        DHLOGE("CHANGE_GROUP not implement.");
    } else {
        DHLOGE("unknown change type.");
    }
}

int32_t DScreenManager::StartDScreenMirror(const std::shared_ptr<DScreen> &dScreen)
{
    if (dScreen == nullptr) {
        DHLOGE("DScreenManager::StartDScreenMirror, dScreen is null.");
        return ERR_DH_SCREEN_SA_VALUE_NOT_INIT;
    }
    uint64_t screenId = dScreen->GetScreenId();
    DHLOGI("DScreenManager::StartDScreenMirror, screenId: %{public}" PRIu64, screenId);
    if (dScreen->GetState() == CONNECTING) {
        DHLOGD("screen is connecting, no need handle change");
        return DH_SUCCESS;
    }
    int32_t ret = dScreen->InitSenderEngine(providerPtr_, dScreen->GetDevId());
    if (ret != DH_SUCCESS) {
        DHLOGE("init av transport sender engine failed.");
        return ret;
    }
    dScreen->AddTask(std::make_shared<Task>(TaskType::TASK_CONNECT, ""));
    DHLOGI("StartDScreenMirror success, screenId:%{public}" PRIu64 ", peerDeviceId:%{public}s", screenId,
        GetAnonyString(dScreen->GetDevId()).c_str());
    return DH_SUCCESS;
}

int32_t DScreenManager::StopDScreenMirror(const std::shared_ptr<DScreen> &dScreen)
{
    if (dScreen == nullptr) {
        DHLOGE("DScreenManager::StopDScreenMirror, dScreen is null.");
        return ERR_DH_SCREEN_SA_VALUE_NOT_INIT;
    }
    DHLOGI("DScreenManager::StopDScreenMirror, screenId: %{public}" PRIu64, dScreen->GetScreenId());
    if (dScreen->GetState() == DISCONNECTING) {
        DHLOGD("screen is disconnecting, no need handle change");
        return DH_SUCCESS;
    }
    dScreen->AddTask(std::make_shared<Task>(TaskType::TASK_DISCONNECT, ""));
    return DH_SUCCESS;
}

void DScreenCallback::OnRegResult(const std::shared_ptr<DScreen> &dScreen,
    const std::string &reqId, const int32_t status, const std::string &data)
{
    if (dScreen == nullptr) {
        DHLOGE("DScreenCallback::OnRegResult, dScreen id nullptr");
        return;
    }
    DHLOGI("DScreenCallback::OnRegResult, devId: %{public}s, dhId: %{public}s, reqId: %{public}s",
        GetAnonyString(dScreen->GetDevId()).c_str(), GetAnonyString(dScreen->GetDHId()).c_str(),
        GetAnonyString(reqId).c_str());
    DScreenManager::GetInstance().OnRegResult(dScreen, reqId, status, data);
}

void DScreenCallback::OnUnregResult(const std::shared_ptr<DScreen> &dScreen,
    const std::string &reqId, const int32_t status, const std::string &data)
{
    if (dScreen == nullptr) {
        DHLOGE("DScreenCallback::OnUnregResult, dScreen id nullptr");
        return;
    }
    DHLOGI("DScreenCallback::OnUnregResult, devId: %{public}s, dhId: %{public}s, reqId: %{public}s",
        GetAnonyString(dScreen->GetDevId()).c_str(), GetAnonyString(dScreen->GetDHId()).c_str(),
        GetAnonyString(reqId).c_str());
    DScreenManager::GetInstance().OnUnregResult(dScreen, reqId, status, data);
}

void DScreenManager::OnRegResult(const std::shared_ptr<DScreen> &dScreen,
    const std::string &reqId, int32_t status, const std::string &data)
{
    DHLOGI("DScreenManager::OnRegResult, devId: %{public}s, dhId: %{public}s, reqId: %{public}s",
        GetAnonyString(dScreen->GetDevId()).c_str(), GetAnonyString(dScreen->GetDHId()).c_str(),
        GetAnonyString(reqId).c_str());
    if (dScreenSourceCallbackProxy_ == nullptr) {
        DHLOGE("dScreenSourceCallbackProxy is null");
        return;
    }
    dScreenSourceCallbackProxy_->OnNotifyRegResult(dScreen->GetDevId(), dScreen->GetDHId(), reqId, status, data);
}

void DScreenManager::OnUnregResult(const std::shared_ptr<DScreen> &dScreen,
    const std::string &reqId, int32_t status, const std::string &data)
{
    DHLOGI("DScreenManager::OnUnregResult, devId: %{public}s, dhId: %{public}s, reqId: %{public}s",
        GetAnonyString(dScreen->GetDevId()).c_str(), GetAnonyString(dScreen->GetDHId()).c_str(),
        GetAnonyString(reqId).c_str());
    if (dScreenSourceCallbackProxy_ == nullptr) {
        DHLOGE("dScreenSourceCallbackProxy is null");
        return;
    }
    dScreenSourceCallbackProxy_->OnNotifyUnregResult(dScreen->GetDevId(), dScreen->GetDHId(), reqId, status, data);
}

int32_t DScreenManager::EnableDistributedScreen(const std::string &devId, const std::string &dhId,
    const EnableParam &param, const std::string &reqId)
{
    DHLOGI("EnableDistributedScreen2.0, devId: %{public}s, dhId:%{public}s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    if (devId.empty() || dhId.empty() || param.sinkVersion.empty() || param.sinkAttrs.empty() || reqId.empty()) {
        DHLOGE("EnableDistributedScreen2.0 CheckRegParams is inlvalid.");
        return ERR_DH_SCREEN_SA_ENABLE_FAILED;
    }
    if (dScreenGroupListener_ == nullptr) {
        dScreenGroupListener_ = new (std::nothrow) DScreenGroupListener();
        int32_t ret = ScreenMgrAdapter::GetInstance().RegisterScreenGroupListener(dScreenGroupListener_);
        if (ret != DH_SUCCESS) {
            DHLOGE("DScreenManager2.0 EnableDistributedScreen failed, err: %{public}" PRId32, ret);
            dScreenGroupListener_ = nullptr;
            return ERR_DH_SCREEN_SA_ENABLE_FAILED;
        }
    }
    if (dScreenCallback_ == nullptr) {
        dScreenCallback_ = std::make_shared<DScreenCallback>();
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
    int32_t ret = dScreen->AddTask(std::make_shared<Task>(TaskType::TASK_ENABLE, reqId, param.sinkAttrs));
    if (ret != DH_SUCCESS) {
        DHLOGE("EnableDistributedScreen, add task failed. devId: %{public}s, dhId:%{public}s",
            GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str());
    }
    return ret;
}

int32_t DScreenManager::DisableDistributedScreen(const std::string &devId, const std::string &dhId,
    const std::string &reqId)
{
    DHLOGI("DisableDistributedScreen, devId: %{public}s, dhId:%{public}s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    std::string dScreenIdx = devId + SEPERATOR + dhId;
    std::lock_guard<std::mutex> lock(dScreenMapMtx_);
    if (dScreens_.count(dScreenIdx) == 0) {
        DHLOGE("dscreen has already disabled, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId).c_str(),
            GetAnonyString(dhId).c_str());
        return DH_SUCCESS;
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

std::shared_ptr<DScreen> DScreenManager::FindDScreenById(uint64_t screenId)
{
    DHLOGD("FindDScreenById, screenId: %{public}" PRIu64, screenId);
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
    DHLOGD("DScreen not found, screenId: %{public}" PRIu64, screenId);
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
        DHLOGD("no virtual screen enabled in V2_0::DScreenManager.");
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

int32_t DScreenManager::LoadAVSenderEngineProvider()
{
    DHLOGI("LoadAVSenderEngineProvider enter");
    char path[PATH_MAX + 1] = {0x00};
    if ((LIB_LOAD_PATH.length() + SENDER_SO_NAME.length()) > PATH_MAX ||
        realpath((LIB_LOAD_PATH + SENDER_SO_NAME).c_str(), path) == nullptr) {
        DHLOGE("File canonicalization failed");
        return ERR_DH_AV_TRANS_LOAD_ERROR;
    }
    void *pHandler = dlopen(path, RTLD_LAZY | RTLD_NODELETE);
    if (pHandler == nullptr) {
        DHLOGE("%{public}s handler load failed, failed reason : %{public}s", path, dlerror());
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    AVTransProviderClass getEngineFactoryFunc = (AVTransProviderClass)dlsym(pHandler, GET_PROVIDER_FUNC.c_str());
    if (getEngineFactoryFunc == nullptr) {
        DHLOGE("av transport engine factory function handler is null, failed reason : %{public}s", dlerror());
        dlclose(pHandler);
        pHandler = nullptr;
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    providerPtr_ = getEngineFactoryFunc(OWNER_NAME_D_SCREEN);
    return DH_SUCCESS;
}

int32_t DScreenManager::UnloadAVSenderEngineProvider()
{
    DHLOGI("UnloadAVSenderEngineProvider enter");
    char path[PATH_MAX + 1] = {0x00};
    if ((LIB_LOAD_PATH.length() + SENDER_SO_NAME.length()) > PATH_MAX ||
        realpath((LIB_LOAD_PATH + SENDER_SO_NAME).c_str(), path) == nullptr) {
        DHLOGE("File canonicalization failed");
        return ERR_DH_AV_TRANS_LOAD_ERROR;
    }
    void *pHandler = dlopen(path, RTLD_LAZY | RTLD_NODELETE);
    if (pHandler != nullptr) {
        dlclose(pHandler);
        pHandler = nullptr;
    }
    return DH_SUCCESS;
}
} // namespace V2_0
} // namespace DistributedHardware
} // namespace OHOS