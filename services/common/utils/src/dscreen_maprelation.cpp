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

#include "dscreen_maprelation.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_json_util.h"

using json = nlohmann::json;

namespace OHOS {
namespace DistributedHardware {
void DScreenMapRelation::SetDisplayId(uint64_t displayId)
{
    displayId_ = displayId;
}

uint64_t DScreenMapRelation::GetDisplayId() const
{
    return displayId_;
}

void DScreenMapRelation::SetScreenId(uint64_t screenId)
{
    screenId_ = screenId;
}

uint64_t DScreenMapRelation::GetScreenId() const
{
    return screenId_;
}

void DScreenMapRelation::SetDisplayRect(const DisplayRect &displayRect)
{
    displayRect_ = displayRect;
}

DisplayRect& DScreenMapRelation::GetDisplayRect()
{
    return displayRect_;
}

void DScreenMapRelation::SetScreenRect(const ScreenRect &screenRect)
{
    screenRect_ = screenRect;
}

ScreenRect& DScreenMapRelation::GetScreenRect()
{
    return screenRect_;
}

void to_json(json &j, const DScreenMapRelation &dScreenMapRelation)
{
    json displayRectJson;
    json screenRectJson;
    to_json(displayRectJson, dScreenMapRelation.displayRect_);
    to_json(screenRectJson, dScreenMapRelation.screenRect_);
    j = json {
        {KEY_DISPLAY_ID, dScreenMapRelation.displayId_},
        {KEY_SCREEN_ID, dScreenMapRelation.screenId_},
        {KEY_DISPLAY_RECT, displayRectJson},
        {KEY_SCREEN_RECT, screenRectJson}
    };
}

void from_json(const json &j, DScreenMapRelation &dScreenMapRelation)
{
    if (!IsUInt64(j, KEY_DISPLAY_ID) || !IsUInt64(j, KEY_SCREEN_ID)) {
        return;
    }
    dScreenMapRelation.displayId_ = j[KEY_DISPLAY_ID].get<uint64_t>();
    dScreenMapRelation.screenId_ = j[KEY_SCREEN_ID].get<uint64_t>();

    from_json(j.at(KEY_DISPLAY_RECT), dScreenMapRelation.displayRect_);
    from_json(j.at(KEY_SCREEN_RECT), dScreenMapRelation.screenRect_);
}

void to_json(json &j, const DisplayRect &rect)
{
    j = json {
        {KEY_POINT_START_X, rect.startX},
        {KEY_POINT_START_Y, rect.startY},
        {KEY_WIDTH, rect.width},
        {KEY_HEIGHT, rect.height}
    };
}

void from_json(const json &j, DisplayRect &rect)
{
    if (!IsInt32(j, KEY_POINT_START_X) || !IsInt32(j, KEY_POINT_START_Y) ||
        !IsInt32(j, KEY_WIDTH) || !IsInt32(j, KEY_HEIGHT)) {
        return;
    }
    rect.startX = j[KEY_POINT_START_X].get<int32_t>();
    rect.startY = j[KEY_POINT_START_Y].get<int32_t>();
    rect.width = j[KEY_WIDTH].get<int32_t>();
    rect.height = j[KEY_HEIGHT].get<int32_t>();
}

void to_json(json &j, const ScreenRect &rect)
{
    j = json {
        {KEY_POINT_START_X, rect.startX},
        {KEY_POINT_START_Y, rect.startY},
        {KEY_WIDTH, rect.width},
        {KEY_HEIGHT, rect.height}
    };
}

void from_json(const json &j, ScreenRect &rect)
{
    if (!IsInt32(j, KEY_POINT_START_X) || !IsInt32(j, KEY_POINT_START_Y) ||
        !IsUInt32(j, KEY_WIDTH) || !IsUInt32(j, KEY_HEIGHT)) {
        return;
    }
    rect.startX = j[KEY_POINT_START_X].get<int32_t>();
    rect.startY = j[KEY_POINT_START_Y].get<int32_t>();
    rect.width = j[KEY_WIDTH].get<uint32_t>();
    rect.height = j[KEY_HEIGHT].get<uint32_t>();
}
} // namespace DistributedHardware
} // namespace OHOS