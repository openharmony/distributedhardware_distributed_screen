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

#include "2.0/include/screenregion.h"

#include <securec.h>
#include "display_manager.h"
#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_fwkkit.h"
#include "dscreen_hisysevent.h"
#include "dscreen_log.h"
#include "dscreen_hidumper.h"
#include "dscreen_json_util.h"
#include "dscreen_util.h"
#include "screen.h"
#include "screen_client.h"
#include "screen_client_common.h"
#include "2.0/include/screenregionmgr.h"
#include "hitrace_meter.h"
#include "dscreen_hitrace.h"

namespace OHOS {
namespace DistributedHardware {
namespace V2_0 {
ScreenRegion::ScreenRegion(const std::string &remoteDevId) : screenId_(0), displayId_(0), remoteDevId_(remoteDevId)
{
    DHLOGI("ScreenRegion ctor.");
    frameNumber_.store(0);
}

ScreenRegion::~ScreenRegion()
{
    DHLOGI("ScreenRegion dctor.");
    if (receiverAdapter_ != nullptr) {
        receiverAdapter_->Release();
    }
    receiverAdapter_ = nullptr;
    frameNumber_.store(0);
}

int32_t ScreenRegion::InitReceiverEngine(IAVEngineProvider *providerPtr)
{
    DHLOGI("InitReceiverEngine enter.");
    if (receiverAdapter_ == nullptr) {
        receiverAdapter_ = std::make_shared<AVTransReceiverAdapter>();
    }
    int32_t ret = receiverAdapter_->Initialize(providerPtr, remoteDevId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("initialize av receiver adapter failed.");
        return ERR_DH_AV_TRANS_INIT_FAILED;
    }
    return receiverAdapter_->RegisterAdapterCallback(shared_from_this());
}

int32_t ScreenRegion::Release()
{
    DHLOGI("ScreenRegion::Release remoteDevId: %{public}s", GetAnonyString(remoteDevId_).c_str());
    if (!isRunning) {
        DHLOGI("ScreenRegion not running, no need release");
        return DH_SUCCESS;
    }
    ScreenClient::GetInstance().RemoveWindow(windowId_);

    if (receiverAdapter_ == nullptr) {
        DHLOGE("av transport receiver adapter is nullptr.");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }

    int32_t ret = receiverAdapter_->Stop();
    if (ret != DH_SUCCESS) {
        DHLOGE("sink trans stop failed.");
    }

    ret = receiverAdapter_->Release();
    if (ret != DH_SUCCESS) {
        DHLOGE("release av receiver adapter failed.");
    }

    isRunning = false;
    return DH_SUCCESS;
}

int32_t ScreenRegion::StartReceiverEngine(const std::string &content)
{
    DHLOGI("StartReceiverEngine enter.");
    if (receiverAdapter_ == nullptr) {
        DHLOGE("av transport receiver adapter is null.");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    int32_t ret = SetUp(content);
    if (ret != DH_SUCCESS) {
        DHLOGE("set up av receiver parameter failed.");
        return ERR_DH_AV_TRANS_SETUP_FAILED;
    }
    ret = receiverAdapter_->Start();
    if (ret != DH_SUCCESS) {
        DHLOGE("start av receiver engine failed, remove window.");
        (void)ScreenClient::GetInstance().RemoveWindow(windowId_);
        return ERR_DH_AV_TRANS_START_FAILED;
    }
    isRunning = true;
    PublishMessage(DHTopic::TOPIC_SINK_PROJECT_WINDOW_INFO, screenId_, remoteDevId_, windowId_, windowProperty_);
    return DH_SUCCESS;
}

int32_t ScreenRegion::StopReceiverEngine()
{
    DHLOGI("StopReceiverEngine, remoteDevId: %{public}s, screenId is: %{public}" PRIu64,
        GetAnonyString(remoteDevId_).c_str(), screenId_);
    int32_t ret = ScreenClient::GetInstance().RemoveWindow(windowId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("remove window failed.");
    }
    if (receiverAdapter_ == nullptr) {
        DHLOGE("av transport receiver adapter is null.");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    return receiverAdapter_->Stop();
}

int32_t ScreenRegion::SetUp(const std::string &content)
{
    json contentJson = json::parse(content, nullptr, false);
    if (contentJson.is_discarded()) {
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    if (!CheckContentJson(contentJson) || !contentJson.contains(KEY_VIDEO_PARAM) ||
        !contentJson.contains(KEY_MAPRELATION)) {
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    screenId_ = contentJson[KEY_SCREEN_ID].get<uint64_t>();
    displayId_ = Rosen::DisplayManager::GetInstance().GetDefaultDisplayId();
    videoParam_ = std::make_shared<VideoParam>(contentJson[KEY_VIDEO_PARAM].get<VideoParam>());
    mapRelation_ = std::make_shared<DScreenMapRelation>(contentJson[KEY_MAPRELATION].get<DScreenMapRelation>());

    int32_t ret = ConfigWindow();
    if (ret != DH_SUCCESS) {
        DHLOGE("config dscreen region window failed.");
        return ret;
    }
    std::string codecType;
    if (videoParam_->GetCodecType() == VIDEO_CODEC_TYPE_VIDEO_H265) {
        codecType = MIME_VIDEO_H265;
    } else if (videoParam_->GetCodecType() == VIDEO_CODEC_TYPE_VIDEO_H264) {
        codecType = MIME_VIDEO_H264;
    } else {
        codecType = MIME_VIDEO_RAW;
    }
    std::string pixelFormat;
    if (videoParam_->GetVideoFormat() == VIDEO_DATA_FORMAT_YUVI420) {
        pixelFormat = VIDEO_FORMAT_YUVI420;
    } else if (videoParam_->GetVideoFormat() == VIDEO_DATA_FORMAT_NV12) {
        pixelFormat = VIDEO_FORMAT_NV12;
    } else if (videoParam_->GetVideoFormat() == VIDEO_DATA_FORMAT_NV21) {
        pixelFormat = VIDEO_FORMAT_NV21;
    } else {
        pixelFormat = VIDEO_FORMAT_RGBA8888;
    }
    receiverAdapter_->SetParameter(AVTransTag::VIDEO_CODEC_TYPE, codecType);
    receiverAdapter_->SetParameter(AVTransTag::VIDEO_PIXEL_FORMAT, pixelFormat);
    receiverAdapter_->SetParameter(AVTransTag::VIDEO_WIDTH, std::to_string(videoParam_->GetVideoWidth()));
    receiverAdapter_->SetParameter(AVTransTag::VIDEO_HEIGHT, std::to_string(videoParam_->GetVideoHeight()));
    receiverAdapter_->SetParameter(AVTransTag::VIDEO_FRAME_RATE, std::to_string(videoParam_->GetFps()));
    receiverAdapter_->SetParameter(AVTransTag::ENGINE_READY, OWNER_NAME_D_SCREEN);

    alignedHeight_ = videoParam_->GetVideoHeight();
    if (alignedHeight_ % ALIGNEDBITS != 0) {
        alignedHeight_ = ((alignedHeight_ / ALIGNEDBITS) +1) * ALIGNEDBITS;
    }
    return DH_SUCCESS;
}

int32_t ScreenRegion::ConfigWindow()
{
    DHLOGI("ConfigWindow enter");
    if (mapRelation_ == nullptr) {
        DHLOGE("mapRelation is nullptr.");
        return ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED;
    }
    ScreenRect screenRect = mapRelation_->GetScreenRect();
    std::shared_ptr<WindowProperty> windowProperty = std::make_shared<WindowProperty>(WindowProperty {
        .displayId = displayId_,
        .startX = screenRect.startX,
        .startY = screenRect.startY,
        .width = screenRect.width,
        .height = screenRect.height
    });
    windowProperty_ = windowProperty;
    windowId_ = ScreenClient::GetInstance().AddWindow(windowProperty);
    if (windowId_ < 0) {
        DHLOGE("Add window failed.");
        ReportOptFail(DSCREEN_OPT_FAIL, ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED, "Add window failed.");
        return ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED;
    }
    int32_t ret = ScreenClient::GetInstance().ShowWindow(windowId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("Show window failed.");
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "Show window failed.");
        return ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED;
    }

    sptr<Surface> surface = ScreenClient::GetInstance().GetSurface(windowId_);
    if (surface == nullptr) {
        DHLOGE("Get window surface failed.");
        ReportOptFail(DSCREEN_OPT_FAIL, ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED, "Get window surface failed.");
        return ERR_DH_SCREEN_SA_DSCREEN_SCREENGION_SETUP_FAILED;
    }
    windowSurface_ = surface;
    return DH_SUCCESS;
}

void ScreenRegion::PublishMessage(const DHTopic topic, const uint64_t &screenId,
    const std::string &remoteDevId, const int32_t &windowId, std::shared_ptr<WindowProperty> windowProperty)
{
    DHLOGI("ScreenRegion PublishMessage");
    if (DScreenFwkKit::GetInstance().GetDHFwkKit() == nullptr) {
        DHLOGE("GetDHFwkKit fail.");
        return;
    }
    json messageJosn;
    std::string message;
    messageJosn[SOURCE_WIN_ID] = screenId;
    messageJosn[SINK_SHOW_WIN_ID] = windowId;
    messageJosn[SOURCE_DEV_ID] = remoteDevId;
    messageJosn[SINK_PROJ_SHOW_WIDTH] = windowProperty->width;
    messageJosn[SINK_PROJ_SHOW_HEIGHT] = windowProperty->height;
    messageJosn[SINK_WIN_SHOW_X] = windowProperty->startX;
    messageJosn[SINK_WIN_SHOW_Y] = windowProperty->startY;
    message = messageJosn.dump();
    DScreenFwkKit::GetInstance().GetDHFwkKit()->PublishMessage(topic, message);
}

void ScreenRegion::OnEngineEvent(DScreenEventType event, const std::string &content)
{
    if (event == DScreenEventType::ENGINE_ERROR) {
        StopReceiverEngine();
    } else if (event == DScreenEventType::TRANS_CHANNEL_CLOSED) {
        ScreenRegionManager::GetInstance().DestoryDScreenRegion(content);
    }
}

void ScreenRegion::OnEngineMessage(const std::shared_ptr<AVTransMessage> &message)
{
    if (message == nullptr) {
        DHLOGE("received engine message is null.");
        return;
    }

    DHLOGI("On source device engine message received, message type =%{public}d.", message->type_);
    if (message->type_ == DScreenMsgType::START_MIRROR) {
        int32_t ret = StartReceiverEngine(message->content_);
        DScreenMsgType msgType = (ret == DH_SUCCESS) ? DScreenMsgType::START_MIRROR_SUCCESS :
            DScreenMsgType::START_MIRROR_FAIL;

        json paramJson;
        paramJson[KEY_DEV_ID] = remoteDevId_;
        auto avMessage = std::make_shared<AVTransMessage>(msgType, paramJson.dump(), remoteDevId_);
        receiverAdapter_->SendMessageToRemote(avMessage);
    } else if (message->type_ == DScreenMsgType::STOP_MIRROR) {
        StopReceiverEngine();
    }
}

void ScreenRegion::GetWSBuffer(sptr<OHOS::SurfaceBuffer> &wsBuffer, const std::shared_ptr<AVTransBuffer> &buffer)
{
    auto bufferData = buffer->GetBufferData(0);
    auto bufferAddr = bufferData->GetAddress();
    auto wsBufAddr = static_cast<uint8_t*>(wsBuffer->GetVirAddr());
    uint32_t wsBufSize = wsBuffer->GetSize();
    uint32_t srcOffset = 0;
    uint32_t dstOffset = 0;
    uint32_t alignedWidth = videoParam_->GetVideoWidth();
    uint32_t chromaOffset = videoParam_->GetVideoWidth() * videoParam_->GetVideoHeight();

    for (unsigned int yh = 0 ; yh < videoParam_->GetVideoHeight(); yh++) {
        int32_t ret = memcpy_s(wsBufAddr + dstOffset, chromaOffset - dstOffset,
            bufferAddr + srcOffset, videoParam_->GetVideoWidth());
        if (ret != EOK) {
            DHLOGE("memcpy video buffer y data failed,ret: %{public}d.", ret);
            windowSurface_->CancelBuffer(wsBuffer);
            return;
        }
        dstOffset += videoParam_->GetVideoWidth();
        srcOffset += alignedWidth;
    }

    dstOffset = chromaOffset;
    srcOffset = alignedWidth * alignedHeight_;

    for (unsigned int uvh = 0 ; uvh < videoParam_->GetVideoHeight() / TWO; uvh++) {
        int32_t ret = memcpy_s(wsBufAddr + dstOffset, wsBufSize - dstOffset,
            bufferAddr + srcOffset, videoParam_->GetVideoWidth());
        if (ret != EOK) {
            DHLOGE("memcpy video buffer uv data failed,ret: %{public}d.", ret);
            windowSurface_->CancelBuffer(wsBuffer);
            return;
        }
        dstOffset += videoParam_->GetVideoWidth();
        srcOffset += alignedWidth;
    }
}

void ScreenRegion::OnEngineDataDone(const std::shared_ptr<AVTransBuffer> &buffer)
{
    ++frameNumber_;
    DHLOGI("OnEngineDataDone enter");
    if (buffer == nullptr) {
        DHLOGE("received video buffer data is nullptr.");
        return;
    }
    if (windowSurface_ == nullptr) {
        DHLOGE("window surface is nullptr.");
        return;
    }
    sptr<OHOS::SurfaceBuffer> wsBuffer = nullptr;
    int32_t releaseFence = -1;
    OHOS::BufferRequestConfig requestConfig = {
        .width = videoParam_->GetVideoWidth(),
        .height = videoParam_->GetVideoHeight(),
        .strideAlignment = STRIDE_ALIGNMENT,
        .format = GRAPHIC_PIXEL_FMT_YCRCB_420_SP,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
    };
    SurfaceError surfaceErr = windowSurface_->RequestBuffer(wsBuffer, releaseFence, requestConfig);
    if (surfaceErr != SURFACE_ERROR_OK || wsBuffer == nullptr) {
        DHLOGE("surface request buffer failed, surfaceErr: %{public}d.", surfaceErr);
        windowSurface_->CancelBuffer(wsBuffer);
        return;
    }
    GetWSBuffer(wsBuffer, buffer);
#ifdef DUMP_DSCREENREGION_FILE
    uint32_t surBufSize = wsBuffer->GetSize();
    auto surBufAddr = static_cast<uint8_t *>(wsBuffer->GetVirAddr());
    int64_t timestamp = 0;
    VideoData data = {surBufAddr, surBufSize, requestConfig.width, requestConfig.height, timestamp, "ycbcr_sp420"};
    if (DscreenHidumper::GetInstance().GetFlagStatus() == true) {
        DHLOGE("HidumperFlag_ = true, exec SaveFile");
        DscreenHidumper::GetInstance().SaveFile("Screen_AfterEncoding_width(", data);
    }
#endif
    BufferFlushConfig flushConfig = { {0, 0, wsBuffer->GetWidth(), wsBuffer->GetHeight()}, 0};
    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SINK_PUSH_DATA_TO_WINDOW_START);
    surfaceErr = windowSurface_->FlushBuffer(wsBuffer, -1, flushConfig);
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (surfaceErr != SURFACE_ERROR_OK) {
        DHLOGE("surface flush buffer failed, surfaceErr: %{public}d.", surfaceErr);
        windowSurface_->CancelBuffer(wsBuffer);
        return;
    }
    DHLOGI("Fill video buffer data to window surface success. frameNumber: %{public}" PRIu32, frameNumber_.load());
}

uint64_t ScreenRegion::GetScreenId()
{
    return screenId_;
}

std::string ScreenRegion::GetRemoteDevId()
{
    return remoteDevId_;
}

int32_t ScreenRegion::GetWindowId()
{
    return windowId_;
}

std::shared_ptr<VideoParam> ScreenRegion::GetVideoParam()
{
    return videoParam_;
}

std::shared_ptr<WindowProperty> ScreenRegion::GetWindowProperty()
{
    return windowProperty_;
}

bool ScreenRegion::CheckContentJson(json &contentJson)
{
    if (!IsUInt64(contentJson, KEY_SCREEN_ID)) {
        return false;
    }
    return true;
}
} // namespace V1_0
} // namespace DistributedHardware
} // namespace OHOS