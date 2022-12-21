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
#include "video_param.h"

#include "dscreen_constants.h"
#include "dscreen_json_util.h"

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

void VideoParam::SetVideoHeight(uint32_t videoHeight)
{
    videoHeight_ = videoHeight;
}

uint32_t VideoParam::GetVideoHeight() const
{
    return videoHeight_;
}

void VideoParam::SetFps(uint32_t fps)
{
    fps_ = fps;
}

uint32_t VideoParam::GetFps() const
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
        {KEY_COLOR_FORMAT, videoParam.videoFormat_}
    };
}

void from_json(const json &j, DistributedHardware::VideoParam &videoParam)
{
    if (!IsUInt32(j, KEY_SCREEN_WIDTH) || !IsUInt32(j, KEY_SCREEN_HEIGHT) ||
        !IsUInt32(j, KEY_VIDEO_WIDTH) || !IsUInt32(j, KEY_VIDEO_HEIGHT) ||
        !IsUInt32(j, KEY_FPS) || !IsUInt8(j, KEY_CODECTYPE) || !IsUInt8(j, KEY_COLOR_FORMAT)) {
        return;
    }

    videoParam.screenWidth_ = j[KEY_SCREEN_WIDTH].get<uint32_t>();
    videoParam.screenHeight_ = j[KEY_SCREEN_HEIGHT].get<uint32_t>();
    videoParam.videoWidth_ = j[KEY_VIDEO_WIDTH].get<uint32_t>();
    videoParam.videoHeight_ = j[KEY_VIDEO_HEIGHT].get<uint32_t>();
    videoParam.fps_ = j[KEY_FPS].get<uint32_t>();
    videoParam.codecType_ = j[KEY_CODECTYPE].get<uint8_t>();
    videoParam.videoFormat_ = j[KEY_COLOR_FORMAT].get<uint8_t>();
}
} // namespace DistributedHardware
} // namespace OHOS
