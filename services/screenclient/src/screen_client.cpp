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

#include "screen_client.h"

#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "screen_client_window_adapter.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(ScreenClient);
int32_t ScreenClient::AddWindow(std::shared_ptr<WindowProperty> windowProperty)
{
    if (windowProperty == nullptr) {
        DHLOGE("windowProperty is nullptr.");
        return ERR_DH_SCREEN_SCREENCLIENT_ADD_WINDOW_ERROR;
    }
    int32_t windowId = ++windowId_;
    sptr<Surface> surface = ScreenClientWindowAdapter::GetInstance().CreateWindow(windowProperty, windowId);
    if (surface == nullptr) {
        DHLOGE("Surface is nullptr.");
        return ERR_DH_SCREEN_SCREENCLIENT_ADD_WINDOW_ERROR;
    }
    {
        std::lock_guard<std::mutex> dataLock(surfaceMapMutex_);
        surfaceMap_.emplace(windowId, surface);
    }
    DHLOGI("Add window (ID = %d) success.", windowId);
    return windowId;
}

int32_t ScreenClient::ShowWindow(int32_t windowId)
{
    {
        std::lock_guard<std::mutex> dataLock(surfaceMapMutex_);
        auto iter = surfaceMap_.find(windowId);
        if (iter == surfaceMap_.end()) {
            DHLOGE("windowId (ID = %d) is non-existent.", windowId);
            return ERR_DH_SCREEN_SCREENCLIENT_SHOW_WINDOW_ERROR;
        }
    }
    int32_t ret = ScreenClientWindowAdapter::GetInstance().ShowWindow(windowId);
    if (DH_SUCCESS != ret) {
        DHLOGE("Show window (ID = %d) failed.", windowId);
        return ret;
    }
    DHLOGI("Show window (ID = %d) success.", windowId);
    return ret;
}

int32_t ScreenClient::HideWindow(int32_t windowId)
{
    {
        std::lock_guard<std::mutex> dataLock(surfaceMapMutex_);
        auto iter = surfaceMap_.find(windowId);
        if (iter == surfaceMap_.end()) {
            DHLOGE("windowId (ID = %d) is non-existent.", windowId);
            return ERR_DH_SCREEN_SCREENCLIENT_HIDE_WINDOW_ERROR;
        }
    }
    int32_t ret = ScreenClientWindowAdapter::GetInstance().HideWindow(windowId);
    if (DH_SUCCESS != ret) {
        DHLOGE("Hide window (ID = %d) failed.", windowId);
        return ret;
    }
    DHLOGI("Hide window (ID = %d) success.", windowId);
    return ret;
}

int32_t ScreenClient::MoveWindow(int32_t windowId, int32_t startX, int32_t startY)
{
    {
        std::lock_guard<std::mutex> dataLock(surfaceMapMutex_);
        auto iter = surfaceMap_.find(windowId);
        if (iter == surfaceMap_.end()) {
            DHLOGE("windowId (ID = %d) is non-existent.", windowId);
            return ERR_DH_SCREEN_SCREENCLIENT_MOVE_WINDOW_ERROR;
        }
    }
    int32_t ret = ScreenClientWindowAdapter::GetInstance().MoveWindow(windowId, startX, startY);
    if (DH_SUCCESS != ret) {
        DHLOGE("Move window (ID = %d) failed.", windowId);
        return ret;
    }
    DHLOGD("Move window (ID = %d) success.", windowId);
    return ret;
}

sptr<Surface> ScreenClient::GetSurface(int32_t windowId)
{
    sptr<Surface> surface = nullptr;
    {
        std::lock_guard<std::mutex> dataLock(surfaceMapMutex_);
        auto iter = surfaceMap_.find(windowId);
        if (iter == surfaceMap_.end()) {
            DHLOGE("windowId (ID = %d) is non-existent.", windowId);
            return nullptr;
        }
        surface = iter->second;
    }
    DHLOGD("Get surface (ID = %d) success.", windowId);
    return surface;
}

int32_t ScreenClient::RemoveWindow(int32_t windowId)
{
    {
        std::lock_guard<std::mutex> dataLock(surfaceMapMutex_);
        auto iter = surfaceMap_.find(windowId);
        if (iter == surfaceMap_.end()) {
            DHLOGE("windowId (ID = %d) is non-existent.", windowId);
            return ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR;
        }
        surfaceMap_.erase(windowId);
    }
    int32_t ret = ScreenClientWindowAdapter::GetInstance().RemoveWindow(windowId);
    if (ret != DH_SUCCESS) {
        DHLOGE("windowId (ID = %d) remove failed.", windowId);
        return ret;
    }
    DHLOGD("windowId (ID = %d) remove success.", windowId);
    return DH_SUCCESS;
}

int32_t ScreenClient::DestroyAllWindow()
{
    DHLOGD("~ScreenClient");
    int32_t ret = ScreenClientWindowAdapter::GetInstance().DestroyAllWindow();
    if (ret != DH_SUCCESS) {
        DHLOGE("DestroyAllWindow failed.");
        return ret;
    }
    std::lock_guard<std::mutex> dataLock(surfaceMapMutex_);
    surfaceMap_.clear();
    windowId_ = INVALID_WINDOW_ID;
    return DH_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS