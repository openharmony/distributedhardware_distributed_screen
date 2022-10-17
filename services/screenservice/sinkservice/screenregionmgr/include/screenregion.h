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

#ifndef OHOS_DSCREEN_SCREEN_REGION_H
#define OHOS_DSCREEN_SCREEN_REGION_H

#include <mutex>

#include "surface.h"

#include "iscreen_sink_trans_callback.h"
#include "iscreen_sink_trans.h"
#include "dscreen_maprelation.h"
#include "screen_client_common.h"
#include "video_param.h"

namespace OHOS {
namespace DistributedHardware {
class ScreenSinkTransCallback : public IScreenSinkTransCallback {
public:
    ~ScreenSinkTransCallback() override {};
    virtual void OnTransError(int32_t err, const std::string &content) = 0;
    virtual void OnError(int32_t err, const std::string &content) override
    {
        OnTransError(err, content);
    }
};

class ScreenRegion : public ScreenSinkTransCallback, public std::enable_shared_from_this<ScreenRegion> {
public:
    ScreenRegion(const std::string &remoteDevId, uint64_t screenId, uint64_t displayId);
    ~ScreenRegion();
    void OnTransError(int32_t err, const std::string &content) override;
    void SetVideoParam(std::shared_ptr<VideoParam> &videoParam);
    void SetMapRelation(std::shared_ptr<DScreenMapRelation> &mapRElation);
    std::string GetRemoteDevId();
    uint64_t GetScreenId();
    uint64_t GetDisplayId();
    std::shared_ptr<VideoParam> GetVideoParam();
    int32_t GetWindowId();
    int32_t SetUpWindow();
    int32_t SetUp();
    int32_t Start();
    int32_t Stop();
    std::shared_ptr<WindowProperty> GetWindowProperty();

private:
    std::string remoteDevId_;
    uint64_t screenId_; // local screen id
    uint64_t displayId_; // local display id
    bool isRunning = false;

    std::shared_ptr<VideoParam> videoParam_ = nullptr;
    std::shared_ptr<DScreenMapRelation> mapRelation_ = nullptr;
    sptr<OHOS::Surface> surface_ = nullptr;
    int32_t windowId_ = INVALID_WINDOW_ID;
    std::shared_ptr<IScreenSinkTrans> sinkTrans_ = nullptr;
    std::shared_ptr<WindowProperty> windowProperty_ = nullptr;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif