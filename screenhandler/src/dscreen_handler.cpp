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

#include "dscreen_handler.h"

#include "avcodec_info.h"
#include "avcodec_list.h"
#include "nlohmann/json.hpp"
#include "screen.h"
#include "histreamer_query_tool.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "dscreen_util.h"

using json = nlohmann::json;

namespace OHOS {
namespace DistributedHardware {
namespace {
const std::string KEY_HISTREAMER_VIDEO_ENCODER = "histmVidEnc";
const std::string KEY_HISTREAMER_VIDEO_DECODER = "histmVidDec";
}
IMPLEMENT_SINGLE_INSTANCE(DScreenHandler);

DScreenHandler::DScreenHandler()
{
    DHLOGI("DScreenHandler constructor.");
}

DScreenHandler::~DScreenHandler()
{
    DHLOGI("~DScreenHandler");
    Rosen::ScreenManager::GetInstance().UnregisterScreenListener(screenListener_);
}

int32_t DScreenHandler::Initialize()
{
    DHLOGI("DScreenHandler Initialize");
    if (screenListener_ == nullptr) {
        screenListener_ = new (std::nothrow) ScreenListener();
    }
    Rosen::DMError ret = Rosen::ScreenManager::GetInstance().RegisterScreenListener(screenListener_);
    if (ret != Rosen::DMError::DM_OK) {
        DHLOGE("register screen listener failed.");
        return DSCREEN_INIT_ERR;
    }
    return DH_SUCCESS;
}

uint32_t ScreenListener::ByteCalculate(std::uint32_t screenWidth)
{
    if (screenWidth % BYTE_ALIGNMENT == 0) {
        return screenWidth;
    }
    uint32_t alignedInt = (screenWidth + BYTE_ALIGNMENT_CALCULATION) / BYTE_ALIGNMENT * BYTE_ALIGNMENT;
    return alignedInt;
}

void ScreenListener::OnConnect(uint64_t screenId)
{
    DHLOGI("on screen connect");
    if (screenId != SCREEN_ID_DEFAULT) {
        DHLOGI("screenId is invalid, screenId is: %" PRIu64, screenId);
        return;
    }
    sptr<Rosen::Screen> screen = Rosen::ScreenManager::GetInstance().GetScreenById(screenId);
    if (screen == nullptr) {
        DHLOGE("screen not found, screenId is: %" PRIu64, screenId);
        return;
    }

    std::string dhId = DSCREEN_PREFIX + SEPERATOR + std::to_string(screenId);

    uint32_t screenWidth = screen->GetWidth();
    screenWidth = ByteCalculate(screenWidth);
    DHLOGI("screenWidth is : %" PRIu32, screenWidth);
    uint32_t screenHeight = screen->GetHeight();

    json attrJson;
    attrJson[KEY_VERSION] = DSCREEN_VERSION;
    attrJson[KEY_SCREEN_WIDTH] = screenWidth;
    attrJson[KEY_SCREEN_HEIGHT] = screenHeight;
    attrJson[KEY_CODECTYPE] = DScreenHandler::GetInstance().QueryCodecInfo();

    DScreenHandler::GetInstance().PluginHardware(dhId, attrJson.dump());
}

void ScreenListener::OnDisconnect(uint64_t screenId)
{
    DHLOGI("on screen disconnect");
    std::string dhId = DSCREEN_PREFIX + SEPERATOR + std::to_string(screenId);
    DScreenHandler::GetInstance().UnPluginHardware(dhId);
}

void DScreenHandler::PluginHardware(const std::string &dhId, const std::string &attr)
{
    DHLOGI("DScreenHandler PluginHardware");
    if (listener_ != nullptr) {
        listener_->PluginHardware(dhId, attr);
    }
}

void DScreenHandler::UnPluginHardware(const std::string &dhId)
{
    DHLOGI("DScreenHandler UnPluginHardware");
    if (listener_ != nullptr) {
        listener_->UnPluginHardware(dhId);
    }
}

std::vector<DHItem> DScreenHandler::Query()
{
    DHLOGI("DScreenHandler query hardware info");
    std::vector<DHItem> dhItemVec;
    std::vector<sptr<Rosen::Screen>> screens;
    Rosen::ScreenManager::GetInstance().GetAllScreens(screens);
    DHLOGI("screens size is: %" PRId32, screens.size());
    for (const auto &screen : screens) {
        if (screen == nullptr) {
            DHLOGE("screen is nullptr.");
            continue;
        }
        if (screen->GetWidth() <= 0) {
            continue;
        }
        std::string dhId = SCREEN_PREFIX + SEPERATOR + std::to_string(screen->GetId());
        uint32_t screenWidth = screen->GetWidth();
        if (screenListener_ == nullptr) {
            screenListener_ = new (std::nothrow) ScreenListener();
        }
        screenWidth = screenListener_->ByteCalculate(screenWidth);
        DHLOGI("screenWidth is : %" PRIu32, screenWidth);
        uint32_t screenHeight = screen->GetHeight();

        json attrJson;
        attrJson[KEY_VERSION] = DSCREEN_VERSION;
        attrJson[KEY_SCREEN_WIDTH] = screenWidth;
        attrJson[KEY_SCREEN_HEIGHT] = screenHeight;
        attrJson[KEY_CODECTYPE] = QueryCodecInfo();
        std::string videoEncoders =
            HiStreamerQueryTool::GetInstance().QueryHiStreamerPluginInfo(HISTREAM_PLUGIN_TYPE::VIDEO_ENCODER);
        DHLOGI("DScreen QueryVideoEncoderAbility info: %s", videoEncoders.c_str());
        attrJson[KEY_HISTREAMER_VIDEO_ENCODER] = videoEncoders;
        std::string videoDecoders =
            HiStreamerQueryTool::GetInstance().QueryHiStreamerPluginInfo(HISTREAM_PLUGIN_TYPE::VIDEO_DECODER);
        DHLOGI("DScreen QueryVideoDecoderAbility info: %s", videoDecoders.c_str());
        attrJson[KEY_HISTREAMER_VIDEO_DECODER] = videoDecoders;

        DHItem dhItem;
        dhItem.dhId = dhId;
        dhItem.attrs = attrJson.dump();
        dhItemVec.push_back(dhItem);
        DHLOGD("query result: dhId: %s, attrs: %s", GetAnonyString(dhId).c_str(), attrJson.dump().c_str());
    }
    return dhItemVec;
}

std::map<std::string, std::string> DScreenHandler::QueryExtraInfo()
{
    DHLOGD("DScreenHandler queryExtraInfo");
    std::map<std::string, std::string> extraInfo;
    return extraInfo;
}

bool DScreenHandler::IsSupportPlugin()
{
    DHLOGD("DScreenHandler IsSupportPlugin");
    return true;
}

void DScreenHandler::RegisterPluginListener(std::shared_ptr<PluginListener> listener)
{
    DHLOGD("DScreenHandler register plugin listener");
    if (listener == nullptr) {
        DHLOGE("DScreenHandler unregistering plugin listener");
        return;
    }
    listener_ = listener;
}

void DScreenHandler::UnRegisterPluginListener()
{
    DHLOGI("DScreenHandler unRegister plugin listener");
    listener_ = nullptr;
}

std::string DScreenHandler::QueryCodecInfo()
{
    DHLOGD("DScreenHandler QueryCodecInfo");
    if (!codecInfoStr_.empty()) {
        return codecInfoStr_;
    }

    // query codec info
    std::shared_ptr<Media::AVCodecList> codecList = Media::AVCodecListFactory::CreateAVCodecList();
    if (codecList == nullptr) {
        return codecInfoStr_;
    }
    std::vector<std::shared_ptr<Media::VideoCaps>> caps = codecList->GetVideoEncoderCaps();
    json codecTypeArray = json::array();

    for (const auto &cap : caps) {
        if (cap == nullptr) {
            continue;
        }
        std::shared_ptr<Media::AVCodecInfo> codecInfo = cap->GetCodecInfo();
        if (codecInfo == nullptr) {
            continue;
        }
        codecTypeArray.push_back(codecInfo->GetName());
    }

    codecInfoStr_ = codecTypeArray.dump();
    return codecInfoStr_;
}

IHardwareHandler* GetHardwareHandler()
{
    DHLOGI("DScreenHandler::GetHardwareHandler");
    return &DScreenHandler::GetInstance();
}
} // namespace DistributedHardware
} // namespace OHOS