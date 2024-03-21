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

#ifndef OHOS_DSCREEN_VIDEO_PARAM_H
#define OHOS_DSCREEN_VIDEO_PARAM_H

#include "nlohmann/json.hpp"

#include "dscreen_constants.h"

using json = nlohmann::json;

namespace OHOS {
namespace DistributedHardware {
class VideoParam {
public:
    void SetScreenWidth(uint32_t screenWidth);
    uint32_t GetScreenWidth() const;
    void SetScreenHeight(uint32_t screenHeight);
    uint32_t GetScreenHeight() const;
    void SetVideoWidth(uint32_t videoWidth);
    uint32_t GetVideoWidth() const;
    void SetVideoHeight(uint32_t videoHeight);
    uint32_t GetVideoHeight() const;
    void SetFps(double fps);
    double GetFps() const;
    void SetCodecType(uint8_t codecType);
    uint8_t GetCodecType() const;
    void SetVideoFormat(uint8_t videoFormat);
    uint8_t GetVideoFormat() const;
    void SetPartialRefreshFlag(bool flag);
    bool GetPartialRefreshFlag() const;
private:
    friend void to_json(json &j, const VideoParam &videoParam);
    friend void from_json(const json &j, VideoParam &videoParam);

    uint32_t screenWidth_;
    uint32_t screenHeight_;
    uint32_t videoWidth_;
    uint32_t videoHeight_;
    double fps_ = DEFAULT_FPS;
    uint8_t codecType_ = DEFAULT_CODECTYPE;
    uint8_t videoFormat_ = DEFAULT_VIDEO_FORMAT;
    bool isPartialRefresh_ = false;
};
void to_json(json &j, const VideoParam &videoParam);
void from_json(const json &j, VideoParam &videoParam);
} // namespace DistributedHardware
} // namespace OHOS
#endif