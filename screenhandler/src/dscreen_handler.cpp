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

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "dscreen_util.h"

using json = nlohmann::json;

namespace OHOS {
namespace DistributedHardware {
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
    if (!screenListener_) {
        screenListener_ = new ScreenListener();
    }
    bool ret = Rosen::ScreenManager::GetInstance().RegisterScreenListener(screenListener_);
    if (!ret) {
        DHLOGE("register screen listener failed.");
    }
    return DH_SUCCESS;
}

void ScreenListener::OnConnect(uint64_t screenId)
{
    DHLOGI("on screen connect");
    if (screenId != SCREEN_ID_DEFAULT) {
        return;
    }
    sptr<Rosen::Screen> screen = Rosen::ScreenManager::GetInstance().GetScreenById(screenId);
    if (screen == nullptr) {
        DHLOGE("screen not found, screenId: %ulld", screenId);
        return;
    }

    std::string dhId = DSCREEN_PREFIX + SEPERATOR + std::to_string(screenId);

    uint32_t screenWidth = screen->GetWidth();
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
    if (listener_ != nullptr) {
        listener_->PluginHardware(dhId, attr);
    }
}

void DScreenHandler::UnPluginHardware(const std::string &dhId)
{
    if (listener_ != nullptr) {
        listener_->UnPluginHardware(dhId);
    }
}

std::vector<DHItem> DScreenHandler::Query()
{
    DHLOGI("DScreenHandler query hardware info");
    std::vector<DHItem> dhItemVec;
    std::vector<sptr<Rosen::Screen>> screens = Rosen::ScreenManager::GetInstance().GetAllScreens();
    sptr<Rosen::Screen> screen = screens[SCREEN_ID_DEFAULT];
    std::string dhId = SCREEN_PREFIX + SEPERATOR + std::to_string(screen->GetId());
    uint32_t screenWidth = screen->GetWidth();
    uint32_t screenHeight = screen->GetHeight();

    json attrJson;
    attrJson[KEY_VERSION] = DSCREEN_VERSION;
    attrJson[KEY_SCREEN_WIDTH] = screenWidth;
    attrJson[KEY_SCREEN_HEIGHT] = screenHeight;
    attrJson[KEY_CODECTYPE] = QueryCodecInfo();

    DHItem dhItem;
    dhItem.dhId = dhId;
    dhItem.attrs = attrJson.dump();
    dhItemVec.push_back(dhItem);
    DHLOGD("query result: dhId: %s, attrs: %s", GetAnonyString(dhId).c_str(), attrJson.dump().c_str());

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
    listener_ = listener;
}

std::string DScreenHandler::QueryCodecInfo()
{
    DHLOGD("DScreenHandler QueryCodecInfo");
    if (!codecInfoStr_.empty()) {
        return codecInfoStr_;
    }

    // query codec info
    std::shared_ptr<Media::AVCodecList> codecList = Media::AVCodecListFactory::CreateAVCodecList();
    std::vector<std::shared_ptr<Media::VideoCaps>> caps = codecList->GetVideoEncoderCaps();
    for (const auto &cap : caps) {
        std::shared_ptr<Media::AVCodecInfo> codecInfo = cap->GetCodecInfo();
        codecInfoStr_.append(codecInfo->GetName());
        codecInfoStr_.append(SEPERATOR);
    }
    return codecInfoStr_;
}

IHardwareHandler* GetHardwareHandler()
{
    return &DScreenHandler::GetInstance();
}
} // namespace DistributedHardware
} // namespace OHOS