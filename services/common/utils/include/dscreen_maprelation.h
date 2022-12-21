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

#ifndef OHOS_DSCREEN_MAP_RELATION_H
#define OHOS_DSCREEN_MAP_RELATION_H

#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace OHOS {
namespace DistributedHardware {
struct ScreenRect {
    int32_t startX;
    int32_t startY;
    uint32_t width;
    uint32_t height;
};
struct DisplayRect {
    int32_t startX;
    int32_t startY;
    int32_t width;
    int32_t height;
};

class DScreenMapRelation {
public:
    void SetDisplayId(uint64_t displayId);
    uint64_t GetDisplayId() const;
    void SetScreenId(uint64_t screenId);
    uint64_t GetScreenId() const;
    void SetDisplayRect(const DisplayRect &displayRect);
    DisplayRect& GetDisplayRect();
    void SetScreenRect(const ScreenRect &screenRect);
    ScreenRect& GetScreenRect();
private:
    friend void to_json(json &j, const DScreenMapRelation &dScreenMapRelation);
    friend void from_json(const json &j, DScreenMapRelation &dScreenMapRelation);

    uint64_t displayId_;
    uint64_t screenId_;
    DisplayRect displayRect_;
    ScreenRect screenRect_;
};
void to_json(const json &j, const DScreenMapRelation &dScreenMapRelation);
void from_json(const json &j, DScreenMapRelation &dScreenMapRelation);
void to_json(json &j, const DisplayRect &rect);
void to_json(json &j, const ScreenRect &rect);
void from_json(const json &j, DisplayRect &rect);
void from_json(const json &j, ScreenRect &rect);
} // namespace DistributedHardware
} // namespace OHOS
#endif