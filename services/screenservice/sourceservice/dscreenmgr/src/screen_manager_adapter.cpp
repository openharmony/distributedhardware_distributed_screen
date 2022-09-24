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

#include "screen_manager_adapter.h"

#include <memory>

#include "display_manager.h"
#include "dm_common.h"
#include "screen.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_log.h"
#include "dscreen_util.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(ScreenMgrAdapter);
ScreenMgrAdapter::~ScreenMgrAdapter()
{
    screenIdMap_.clear();
    DHLOGI("~ScreenMgrAdapter");
}

uint64_t ScreenMgrAdapter::CreateVirtualScreen(const std::string &devId, const std::string &dhId,
    const std::shared_ptr<VideoParam> &videoParam)
{
    DHLOGI("CreateVirtualScreen, width: %u, height: %u", videoParam->GetScreenWidth(),
        videoParam->GetScreenHeight());
    std::string screenName = DSCREEN_PREFIX + SEPERATOR + GetInterruptString(devId) +
                             SEPERATOR + GetInterruptString(dhId);
    auto iter = screenIdMap_.find(screenName);
    if (iter != screenIdMap_.end()) {
        DHLOGI("remove an exist virtual screen.");
        Rosen::DMError err = Rosen::ScreenManager::GetInstance().DestroyVirtualScreen(iter->second);
        if (err != Rosen::DMError::DM_OK) {
            DHLOGE("remove virtual screen failed, screenId:%ulld", iter->second);
            return SCREEN_ID_INVALID;
        }
        screenIdMap_.erase(screenName);
    }

    Rosen::VirtualScreenOption option = {
        screenName,
        videoParam->GetScreenWidth(),
        videoParam->GetScreenHeight(),
        DEFAULT_DENSITY,
        nullptr,
        DEFAULT_SCREEN_FLAGS,
        false
    };

    uint64_t screenId = Rosen::ScreenManager::GetInstance().CreateVirtualScreen(option);
    DHLOGI("virtualScreen id is: %" PRIu64, screenId);
    screenIdMap_.emplace(screenName, screenId);
    return screenId;
}

int32_t ScreenMgrAdapter::RegisterScreenGroupListener(sptr<Rosen::ScreenManager::IScreenGroupListener> listener)
{
    DHLOGI("RegisterScreenGroupListener");
    if (listenerRegistered_) {
        DHLOGI("already registered listener.");
        return DH_SUCCESS;
    }
    bool ret = Rosen::ScreenManager::GetInstance().RegisterScreenGroupListener(listener);
    if (!ret) {
        DHLOGE("RegisterScreenGroupListener Failed.");
        return ERR_DH_SCREEN_SA_REGISTER_SCREENLISTENER_FAIL;
    }
    listenerRegistered_ = true;
    return DH_SUCCESS;
}

int32_t ScreenMgrAdapter::UnregisterScreenGroupListener(sptr<Rosen::ScreenManager::IScreenGroupListener> listener)
{
    DHLOGI("UnregisterScreenGroupListener");
    if (!listenerRegistered_) {
        DHLOGI("listener already unregistered.");
        return DH_SUCCESS;
    }
    bool ret = Rosen::ScreenManager::GetInstance().UnregisterScreenGroupListener(listener);
    if (!ret) {
        DHLOGE("UnregisterScreenGroupListener Failed.");
        return ERR_DH_SCREEN_SA_UNREGISTER_SCREENLISTENER_FAIL;
    }
    listenerRegistered_ = false;
    return DH_SUCCESS;
}

void ScreenMgrAdapter::RemoveScreenFromGroup(uint64_t screenId)
{
    DHLOGI("remove screen from group, screenId: %ulld", screenId);
    std::vector<uint64_t> screenIds;
    screenIds.push_back(screenId);
    Rosen::ScreenManager::GetInstance().RemoveVirtualScreenFromGroup(screenIds);
}

int32_t ScreenMgrAdapter::RemoveVirtualScreen(uint64_t screenId)
{
    DHLOGI("remove virtual screen, screenId: %ulld", screenId);
    Rosen::DMError err = Rosen::ScreenManager::GetInstance().DestroyVirtualScreen(screenId);
    if (err != Rosen::DMError::DM_OK) {
        DHLOGE("remove virtual screen failed, screenId:%ulld", screenId);
        return ERR_DH_SCREEN_SA_REMOVE_VIRTUALSCREEN_FAIL;
    }
    return DH_SUCCESS;
}

int32_t ScreenMgrAdapter::SetImageSurface(uint64_t screenId, sptr<OHOS::Surface> surface)
{
    DHLOGI("SetImageSurface for virtualscreen, screenId: %ulld", screenId);
    Rosen::ScreenManager::GetInstance().SetVirtualScreenSurface(screenId, surface);
    return DH_SUCCESS;
}

std::shared_ptr<DScreenMapRelation> ScreenMgrAdapter::GetMapRelation(uint64_t screenId)
{
    DHLOGI("GetMapRelation screenId: %ulld", screenId);
    std::shared_ptr<DScreenMapRelation> mapRelation = std::make_shared<DScreenMapRelation>();
    sptr<Rosen::Screen> screen = Rosen::ScreenManager::GetInstance().GetScreenById(screenId);
    if (screen == nullptr) {
        DHLOGE("screen is nullptr.");
        return nullptr;
    }
    sptr<Rosen::Display> display = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    if (display == nullptr) {
        DHLOGE("display is nullptr.");
        return nullptr;
    }
    mapRelation->SetDisplayId(display->GetId());
    mapRelation->SetScreenId(screenId);

    ScreenRect screenRect = {0, 0, screen->GetWidth(), screen->GetHeight()};
    DisplayRect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    mapRelation->SetDisplayRect(displayRect);
    mapRelation->SetScreenRect(screenRect);
    return mapRelation;
}
} // namespace DistributedHardware
} // namespace OHOS