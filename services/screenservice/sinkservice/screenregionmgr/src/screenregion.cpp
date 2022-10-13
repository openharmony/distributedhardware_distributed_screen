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

#include "screenregion.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_log.h"
#include "dscreen_util.h"
#include "screen_client.h"
#include "screen_client_common.h"
#include "screen_sink_trans.h"

namespace OHOS {
namespace DistributedHardware {
ScreenRegion::ScreenRegion(const std::string &remoteDevId, uint64_t screenId, uint64_t displayId)
{
    DHLOGD("ScreenRegion construct");
    remoteDevId_ = remoteDevId;
    screenId_ = screenId;
    displayId_ = displayId;
    windowId_ = INVALID_WINDOW_ID;
}

ScreenRegion::~ScreenRegion()
{
    DHLOGD("ScreenRegion deConstruct");
    if (sinkTrans_ != nullptr) {
        sinkTrans_->Release();
    }
}

void ScreenRegion::OnTransError(int32_t err, const std::string &content)
{
    DHLOGE("OnTransError, err: %d", err);
    Stop();
}

void ScreenRegion::SetVideoParam(std::shared_ptr<VideoParam> &videoParam)
{
    videoParam_ = videoParam;
}

void ScreenRegion::SetMapRelation(std::shared_ptr<DScreenMapRelation> &mapRelation)
{
    mapRelation_ = mapRelation;
}

uint64_t ScreenRegion::GetScreenId()
{
    return screenId_;
}

uint64_t ScreenRegion::GetDisplayId()
{
    return displayId_;
}

std::string ScreenRegion::GetRemoteDevId()
{
    return remoteDevId_;
}

std::shared_ptr<VideoParam> ScreenRegion::GetVideoParam()
{
    return videoParam_;
}

int32_t ScreenRegion::GetWindowId()
{
    return windowId_;
}

std::shared_ptr<WindowProperty> ScreenRegion::GetWindowProperty()
{
    return windowProperty_;
}

int32_t ScreenRegion::SetUpWindow()
{
    DHLOGI("ScreenRegion::SetUpWindow");
    std::shared_ptr<WindowProperty> windowProperty = std::make_shared<WindowProperty>();
    windowProperty->displayId = displayId_;
    if (mapRelation_ == nullptr) {
        DHLOGE("ScreenRegion::SetUp mapRelation is nullptr.");
        return ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED;
    }
    ScreenRect screenRect = mapRelation_->GetScreenRect();
    windowProperty->startX = screenRect.startX;
    windowProperty->startY = screenRect.startY;
    windowProperty->width = screenRect.width;
    windowProperty->height = screenRect.height;
    windowProperty_ = windowProperty;

    windowId_ = ScreenClient::GetInstance().AddWindow(windowProperty);
    if (windowId_ < 0) {
        DHLOGE("AddWindow failed.");
        ReportOptFail(DSCREEN_OPT_FAIL, ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED, "AddWindow failed.");
        return ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED;
    }
    int32_t ret = ScreenClient::GetInstance().ShowWindow(windowId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("show window failed.");
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "show window failed.");
        return ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED;
    }

    sptr<Surface> surface = ScreenClient::GetInstance().GetSurface(windowId_);
    if (surface == nullptr) {
        DHLOGE("get window surface failed.");
        ReportOptFail(DSCREEN_OPT_FAIL, ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED, "get window surface failed.");
        return ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED;
    }
    surface_ = surface;

    return DH_SUCCESS;
}


int32_t ScreenRegion::SetUp()
{
    DHLOGI("ScreenRegion::SetUp, remoteDevId: %s", GetAnonyString(remoteDevId_).c_str());
    int32_t ret = SetUpWindow();
    if (ret != DH_SUCCESS) {
        DHLOGE("SetUpWindow failed.");
        return ret;
    }

    if (sinkTrans_ == nullptr) {
        sinkTrans_ = std::make_shared<ScreenSinkTrans>();
    }

    sinkTrans_->RegisterStateCallback(shared_from_this());
    sinkTrans_->SetImageSurface(surface_);
    ret = sinkTrans_->SetUp(*videoParam_, *videoParam_, remoteDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("sinkTrans setup failed.");
        return ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED;
    }

    return DH_SUCCESS;
}

int32_t ScreenRegion::Start()
{
    DHLOGI("ScreenRegion::Start remoteDevId: %s", GetAnonyString(remoteDevId_).c_str());
    if (sinkTrans_ == nullptr) {
        DHLOGE("sink trans not init.");
        return ERR_DH_SCREEN_SA_SINKTRANS_NOT_INIT;
    }

    int32_t ret = sinkTrans_->Start();
    if (ret != DH_SUCCESS) {
        DHLOGE("sink trans start failed.");
        return ret;
    }
    isRunning = true;
    return DH_SUCCESS;
}

int32_t ScreenRegion::Stop()
{
    DHLOGI("ScreenRegion::Stop remoteDevId: %s", GetAnonyString(remoteDevId_).c_str());
    if (!isRunning) {
        DHLOGI("ScreenRegion not running, no need stop");
        return DH_SUCCESS;
    }

    if (sinkTrans_ == nullptr) {
        DHLOGE("sink trans not init.");
        return ERR_DH_SCREEN_SA_SINKTRANS_NOT_INIT;
    }

    int32_t ret = sinkTrans_->Stop();
    if (ret != DH_SUCCESS) {
        DHLOGE("sink trans stop failed.");
        return ret;
    }

    ret = sinkTrans_->Release();
    if (ret != DH_SUCCESS) {
        DHLOGE("sink trans release failed.");
        return ret;
    }

    ret = ScreenClient::GetInstance().RemoveWindow(windowId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("remove window failed.");
        return ret;
    }

    isRunning = false;
    return DH_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS