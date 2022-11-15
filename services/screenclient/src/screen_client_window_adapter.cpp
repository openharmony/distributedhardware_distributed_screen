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

#include "screen_client_window_adapter.h"

#include "rs_surface_node.h"
#include "transaction/rs_interfaces.h"
#include "window_option.h"
#include "wm_common.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_log.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(ScreenClientWindowAdapter);
sptr<Surface> ScreenClientWindowAdapter::CreateWindow(std::shared_ptr<WindowProperty> windowProperty,
    int32_t windowId)
{
    if (windowProperty == nullptr) {
        DHLOGE("windowProperty is nullptr.");
        return nullptr;
    }
    sptr<Rosen::WindowOption> option = new Rosen::WindowOption();
    if (option == nullptr) {
        DHLOGE("option is nullptr.");
        return nullptr;
    }
    option->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_LAUNCHING);
    option->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    option->SetDisplayId(windowProperty->displayId);
    std::string windowName = SCREEN_CLIENT_WINDOW + std::to_string(windowId);
    sptr<Rosen::Window> window = Rosen::Window::Create(windowName, option);
    if (window == nullptr || window->GetSurfaceNode() == nullptr) {
        DHLOGE("Create screen client window failed.");
        return nullptr;
    }
    auto surface = window->GetSurfaceNode()->GetSurface();
    if (surface == nullptr) {
        window->Destroy();
        DHLOGE("surface is nullptr");
        return nullptr;
    }
    Rosen::RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    std::shared_ptr<Rosen::IInputEventConsumer> listener =
        std::make_shared<ScreenClientInputEventListener>(ScreenClientInputEventListener());
    window->SetInputEventConsumer(listener);
    DHLOGD("Create window name is %s.", windowName.c_str());
    if (OHOS::Rosen::WMError::WM_OK != window->Resize(windowProperty->width, windowProperty->height)) {
        window->Destroy();
        DHLOGE("Window resize failed.");
        return nullptr;
    }
    if (OHOS::Rosen::WMError::WM_OK != window->MoveTo(windowProperty->startX, windowProperty->startY)) {
        window->Destroy();
        DHLOGE("Window moveto failed.");
        return nullptr;
    }
    {
        std::lock_guard<std::mutex> dataLock(windowIdMapMutex_);
        windowIdMap_.emplace(windowId, window);
    }
    DHLOGD("Get surface(windowId = %d) success.", windowId);
    return surface;
}

int32_t ScreenClientWindowAdapter::ShowWindow(int32_t windowId)
{
    sptr<Rosen::Window> window = nullptr;
    {
        std::lock_guard<std::mutex> dataLock(windowIdMapMutex_);
        auto iter = windowIdMap_.find(windowId);
        if (iter == windowIdMap_.end()) {
            DHLOGE("Invalid windowId (windowId = %d).", windowId);
            return ERR_DH_SCREEN_SCREENCLIENT_SHOW_WINDOW_ERROR;
        }
        window = iter->second;
    }
    if (window == nullptr) {
        DHLOGE("Current window is null.");
        return ERR_DH_SCREEN_SCREENCLIENT_SHOW_WINDOW_ERROR;
    }
    if (OHOS::Rosen::WMError::WM_OK != window->Show()) {
        DHLOGE("Show window failed.");
        return ERR_DH_SCREEN_SCREENCLIENT_SHOW_WINDOW_ERROR;
    }
    DHLOGD("Show window (windowId = %d) success.", windowId);
    return DH_SUCCESS;
}

int32_t ScreenClientWindowAdapter::HideWindow(int32_t windowId)
{
    sptr<Rosen::Window> window = nullptr;
    {
        std::lock_guard<std::mutex> dataLock(windowIdMapMutex_);
        auto iter = windowIdMap_.find(windowId);
        if (iter == windowIdMap_.end()) {
            DHLOGE("Invalid windowId (windowId = %d).", windowId);
            return ERR_DH_SCREEN_SCREENCLIENT_HIDE_WINDOW_ERROR;
        }
        window = iter->second;
    }
    if (window == nullptr) {
        DHLOGE("Current window is null.");
        return ERR_DH_SCREEN_SCREENCLIENT_HIDE_WINDOW_ERROR;
    }
    if (OHOS::Rosen::WMError::WM_OK != window->Hide()) {
        DHLOGE("Hide window failed.");
        return ERR_DH_SCREEN_SCREENCLIENT_HIDE_WINDOW_ERROR;
    }
    DHLOGD("Hide window (windowId = %d) success.", windowId);
    return DH_SUCCESS;
}

int32_t ScreenClientWindowAdapter::MoveWindow(int32_t windowId, int32_t startX, int32_t startY)
{
    sptr<Rosen::Window> window = nullptr;
    {
        std::lock_guard<std::mutex> dataLock(windowIdMapMutex_);
        auto iter = windowIdMap_.find(windowId);
        if (iter == windowIdMap_.end()) {
            DHLOGE("Invalid windowId (windowId = %d).", windowId);
            return ERR_DH_SCREEN_SCREENCLIENT_MOVE_WINDOW_ERROR;
        }
        window = iter->second;
    }
    if (window == nullptr) {
        DHLOGE("Current window is null(windowId = %d).", windowId);
        return ERR_DH_SCREEN_SCREENCLIENT_MOVE_WINDOW_ERROR;
    }
    if (OHOS::Rosen::WMError::WM_OK != window->MoveTo(startX, startY)) {
        DHLOGE("Move window failed (windowId = %d).", windowId);
        return ERR_DH_SCREEN_SCREENCLIENT_MOVE_WINDOW_ERROR;
    }
    DHLOGD("MoveTo window (windowId = %d) success.", windowId);
    return DH_SUCCESS;
}

int32_t ScreenClientWindowAdapter::RemoveWindow(int32_t windowId)
{
    sptr<Rosen::Window> window = nullptr;
    {
        std::lock_guard<std::mutex> dataLock(windowIdMapMutex_);
        auto iter = windowIdMap_.find(windowId);
        if (iter == windowIdMap_.end()) {
            DHLOGE("Invalid windowId(windowId = %d).", windowId);
            return ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR;
        }
        window = iter->second;
        windowIdMap_.erase(windowId);
    }
    if (window == nullptr) {
        DHLOGE("Current window is null(windowId = %d).", windowId);
        return ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR;
    }
    if (OHOS::Rosen::WMError::WM_OK != window->Hide()) {
        DHLOGE("Remove window window failed(windowId = %d).", windowId);
        return ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR;
    }
    if (OHOS::Rosen::WMError::WM_OK != window->Destroy()) {
        DHLOGE("Remove window window failed(windowId = %d).", windowId);
        return ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR;
    }
    DHLOGD("Remove window success(windowId = %d).", windowId);
    return DH_SUCCESS;
}

int32_t ScreenClientWindowAdapter::DestroyAllWindow()
{
    DHLOGD("~ScreenClientWindowAdapter");
    std::lock_guard<std::mutex> dataLock(windowIdMapMutex_);
    for (const auto &item : windowIdMap_) {
        auto window = item.second;
        if (window == nullptr) {
            DHLOGE("window is nullptr(windowId = %d).", item.first);
            continue;
        }
        if (OHOS::Rosen::WMError::WM_OK != window->Destroy()) {
            DHLOGE("Destroy window(windowId = %d) failed .", item.first);
            continue;
        }
    }
    windowIdMap_.clear();
    return DH_SUCCESS;
}

bool ScreenClientInputEventListener::OnInputEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const
{
    DHLOGI("onInputEvent, pointerEvent");
    pointerEvent->MarkProcessed();
    return true;
}

bool ScreenClientInputEventListener::OnInputEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const
{
    DHLOGI("onInputEvent, keyEvent");
    keyEvent->MarkProcessed();
    return true;
}

bool ScreenClientInputEventListener::OnInputEvent(const std::shared_ptr<MMI::AxisEvent>& axisEvent) const
{
    DHLOGI("onInputEvent, axisEvent");
    axisEvent->MarkProcessed();
    return true;
}
} // namespace DistributedHardware
} // namespace OHOS