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

#ifndef OHOS_SCREEN_CLIENT_WINDOW_H
#define OHOS_SCREEN_CLIENT_WINDOW_H

#include <cstdint>
#include <string>
#include <map>
#include <memory>
#include <refbase.h>

#include "axis_event.h"
#include "key_event.h"
#include "pointer_event.h"
#include "single_instance.h"
#include "surface.h"
#include "window.h"

#include "screen_client.h"
#include "screen_client_common.h"

namespace OHOS {
namespace DistributedHardware {
class ScreenClientWindowAdapter {
DECLARE_SINGLE_INSTANCE_BASE(ScreenClientWindowAdapter);
public:
    sptr<Surface> CreateWindow(std::shared_ptr<WindowProperty> windowProperty, int32_t windowId);
    int32_t ShowWindow(int32_t windowId);
    int32_t HideWindow(int32_t windowId);
    int32_t MoveWindow(int32_t windowId, int32_t startX, int32_t startY);
    int32_t RemoveWindow(int32_t windowId);
    int32_t DestroyAllWindow();
private:
    ScreenClientWindowAdapter() = default;
    ~ScreenClientWindowAdapter() = default;
    std::map<int32_t, sptr<Rosen::Window>> windowIdMap_;
    std::mutex windowIdMapMutex_;
};

class ScreenClientInputEventListener : public Rosen::IInputEventConsumer {
public:
    ScreenClientInputEventListener() = default;
    ~ScreenClientInputEventListener() override = default;
    bool OnInputEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) const override;
    bool OnInputEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const override;
    bool OnInputEvent(const std::shared_ptr<MMI::AxisEvent>& axisEvent) const override;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif