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
#include "dscreen_errcode.h"

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
    j.at(KEY_SCREEN_WIDTH).get_to(videoParam.screenWidth_);
    j.at(KEY_SCREEN_HEIGHT).get_to(videoParam.screenHeight_);
    j.at(KEY_VIDEO_WIDTH).get_to(videoParam.videoWidth_);
    j.at(KEY_VIDEO_HEIGHT).get_to(videoParam.videoHeight_);
    j.at(KEY_FPS).get_to(videoParam.fps_);
    j.at(KEY_CODECTYPE).get_to(videoParam.codecType_);
    j.at(KEY_COLOR_FORMAT).get_to(videoParam.videoFormat_);
}
} // namespace DistributedHardware
} // namespace OHOS
