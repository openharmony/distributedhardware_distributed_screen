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

#ifndef OHOS_SCREEN_CLIENT_H
#define OHOS_SCREEN_CLIENT_H

#include <atomic>
#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "single_instance.h"
#include "surface.h"

#include "dscreen_constants.h"
#include "screen_client_common.h"

namespace OHOS {
namespace DistributedHardware {
class ScreenClient {
DECLARE_SINGLE_INSTANCE_BASE(ScreenClient);
public:
    int32_t AddWindow(std::shared_ptr<WindowProperty> windowProperty);
    sptr<Surface> GetSurface(int32_t windowId);
    int32_t ShowWindow(int32_t windowId);
    int32_t MoveWindow(int32_t windowId, int32_t startX, int32_t startY);
    int32_t RemoveWindow(int32_t windowId);
    int32_t HideWindow(int32_t windowId);
    int32_t DestroyAllWindow();
private:
    ScreenClient() = default;
    ~ScreenClient() = default;
    std::map<int32_t, sptr<Surface>> surfaceMap_;
    std::atomic<int32_t> windowId_ = INVALID_WINDOW_ID;
    std::mutex surfaceMapMutex_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif