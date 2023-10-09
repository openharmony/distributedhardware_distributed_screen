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

#ifndef OHOS_DSCREEN_SCREEN_REGION_V2_0_H
#define OHOS_DSCREEN_SCREEN_REGION_V2_0_H

#include <cstdio>
#include <fstream>
#include <mutex>
#include <string>

#include "surface.h"

#include "2.0/include/av_receiver_engine_adapter.h"
#include "distributed_hardware_fwk_kit.h"
#include "dscreen_maprelation.h"
#include "screen_client_common.h"
#include "video_param.h"

namespace OHOS {
namespace DistributedHardware {
namespace V2_0 {
class ScreenRegion :  public AVReceiverAdapterCallback, public std::enable_shared_from_this<ScreenRegion> {
public:
    ScreenRegion(const std::string &remoteDevId);
    ~ScreenRegion();

    // interfaces from AVReceiverAdapterCallback
    void OnEngineEvent(DScreenEventType event, const std::string &content) override;
    void OnEngineMessage(const std::shared_ptr<AVTransMessage> &message) override;
    void OnEngineDataDone(const std::shared_ptr<AVTransBuffer> &buffer) override;
    void GetWSBuffer(sptr<OHOS::SurfaceBuffer> &wsBuffer, const std::shared_ptr<AVTransBuffer> &buffer);

    int32_t Release();
    int32_t InitReceiverEngine(IAVEngineProvider *providerPtr);
    int32_t GetWindowId();
    uint64_t GetScreenId();
    std::string GetRemoteDevId();
    std::shared_ptr<VideoParam> GetVideoParam();
    std::shared_ptr<WindowProperty> GetWindowProperty();

private:
    int32_t StartReceiverEngine(const std::string &content);
    int32_t StopReceiverEngine();
    int32_t SetUp(const std::string &content);
    int32_t ConfigWindow();
    bool CheckContentJson(json &contentJson);
    void PublishMessage(const DHTopic topic, const uint64_t &screenId, const std::string &remoteDevId,
        const int32_t &windowId, std::shared_ptr<WindowProperty> windowProperty);

private:
    uint64_t screenId_;
    uint64_t displayId_;
    std::string remoteDevId_;
    int32_t windowId_ = INVALID_WINDOW_ID;
    uint32_t alignedHeight_ = 0;
    bool isRunning = false;
    std::atomic<uint32_t> frameNumber_;

    sptr<OHOS::Surface> windowSurface_ = nullptr;
    std::shared_ptr<VideoParam> videoParam_ = nullptr;
    std::shared_ptr<WindowProperty> windowProperty_ = nullptr;
    std::shared_ptr<DScreenMapRelation> mapRelation_ = nullptr;
    std::shared_ptr<AVTransReceiverAdapter> receiverAdapter_ = nullptr;
};
} // namespace V2_0
} // namespace DistributedHardware
} // namespace OHOS
#endif