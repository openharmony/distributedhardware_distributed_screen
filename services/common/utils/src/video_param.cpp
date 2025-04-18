/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "video_param.h"

#include "dscreen_constants.h"
#include "dscreen_json_util.h"
#include "dscreen_log.h"

using json = nlohmann::json;

namespace OHOS {
namespace DistributedHardware {
void VideoParam::SetScreenWidth(uint32_t screenWidth)
{
    screenWidth_ = screenWidth;
}

uint32_t VideoParam::GetScreenWidth() const
{
    return screenWidth_;
}

void VideoParam::SetScreenHeight(uint32_t screenHeight)
{
    screenHeight_ = screenHeight;
}

uint32_t VideoParam::GetScreenHeight() const
{
    return screenHeight_;
}

void VideoParam::SetVideoWidth(uint32_t videoWidth)
{
    videoWidth_ = videoWidth;
}

uint32_t VideoParam::GetVideoWidth() const
{
    return videoWidth_;
}

void VideoParam::SetPartialRefreshFlag(bool flag)
{
    isPartialRefresh_ = flag;
}

bool VideoParam::GetPartialRefreshFlag() const
{
    return isPartialRefresh_;
}

void VideoParam::SetVideoHeight(uint32_t videoHeight)
{
    videoHeight_ = videoHeight;
}

uint32_t VideoParam::GetVideoHeight() const
{
    return videoHeight_;
}

void VideoParam::SetFps(double fps)
{
    fps_ = fps;
}

double VideoParam::GetFps() const
{
    return fps_;
}

void VideoParam::SetCodecType(uint8_t codecType)
{
    codecType_ = codecType;
}

uint8_t VideoParam::GetCodecType() const
{
    return codecType_;
}

void VideoParam::SetVideoFormat(uint8_t videoFormat)
{
    videoFormat_ = videoFormat;
}

uint8_t VideoParam::GetVideoFormat() const
{
    return videoFormat_;
}

void to_json(json &j, const DistributedHardware::VideoParam &videoParam)
{
    j = json {
        {KEY_SCREEN_WIDTH, videoParam.screenWidth_},
        {KEY_SCREEN_HEIGHT, videoParam.screenHeight_},
        {KEY_VIDEO_WIDTH, videoParam.videoWidth_},
        {KEY_VIDEO_HEIGHT, videoParam.videoHeight_},
        {KEY_FPS, videoParam.fps_},
        {KEY_CODECTYPE, videoParam.codecType_},
        {KEY_COLOR_FORMAT, videoParam.videoFormat_},
        {KEY_PARTIALREFRESH, videoParam.isPartialRefresh_}
    };
}

void from_json(const json &j, DistributedHardware::VideoParam &videoParam)
{
    if (!IsUInt32(j, KEY_SCREEN_WIDTH) || !IsUInt32(j, KEY_SCREEN_HEIGHT) ||
        !IsUInt32(j, KEY_VIDEO_WIDTH) || !IsUInt32(j, KEY_VIDEO_HEIGHT) ||
        !IsFloat(j, KEY_FPS) || !IsUInt8(j, KEY_CODECTYPE) ||
        !IsUInt8(j, KEY_COLOR_FORMAT)) {
        DHLOGE("Invalid JSON value for one or more keys.");
        return;
    }

    const uint32_t screenWidth = j[KEY_SCREEN_WIDTH].get<uint32_t>();
    const uint32_t screenHeight = j[KEY_SCREEN_HEIGHT].get<uint32_t>();
    const uint32_t videoWidth = j[KEY_VIDEO_WIDTH].get<uint32_t>();
    const uint32_t videoHeight = j[KEY_VIDEO_HEIGHT].get<uint32_t>();

    if ((screenWidth > DSCREEN_MAX_SCREEN_DATA_WIDTH) || (screenHeight > DSCREEN_MAX_SCREEN_DATA_HEIGHT)) {
        DHLOGE("Screen width or height exceeds the maximum limit.");
        return;
    }

    if ((videoWidth > DSCREEN_MAX_VIDEO_DATA_WIDTH) || (videoHeight > DSCREEN_MAX_VIDEO_DATA_HEIGHT)) {
        DHLOGE("Video width or height exceeds the maximum limit.");
        return;
    }

    videoParam.screenWidth_ = screenWidth;
    videoParam.screenHeight_ = screenHeight;
    videoParam.videoWidth_ = videoWidth;
    videoParam.videoHeight_ = videoHeight;
    videoParam.fps_ = j[KEY_FPS].get<double>();
    videoParam.codecType_ = j[KEY_CODECTYPE].get<uint8_t>();
    videoParam.videoFormat_ = j[KEY_COLOR_FORMAT].get<uint8_t>();
    videoParam.isPartialRefresh_ = false;
    if (IsBool(j, KEY_PARTIALREFRESH)) {
        videoParam.isPartialRefresh_ = j[KEY_PARTIALREFRESH].get<bool>();
    }
}
} // namespace DistributedHardware
} // namespace OHOS
