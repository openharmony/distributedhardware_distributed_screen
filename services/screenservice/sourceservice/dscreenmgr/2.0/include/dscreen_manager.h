/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_DSCREEN_MGR_V2_0_H
#define OHOS_DSCREEN_MGR_V2_0_H

#include "distributed_hardware_fwk_kit.h"
#include "dm_common.h"
#include "single_instance.h"
#include "screen_manager.h"

#include "av_sender_engine_adapter.h"
#include "dscreen.h"
#include "dscreen_maprelation.h"
#include "idistributed_hardware_source.h"
#include "idscreen_sink.h"
#include "idscreen_source_callback.h"
#include "video_param.h"

namespace OHOS {
namespace DistributedHardware {
namespace V2_0 {
class DScreenCallback : public IDScreenCallback {
public:
    void OnRegResult(const std::shared_ptr<DScreen> &dScreen, const std::string &reqId,
        const int32_t status, const std::string &data) override;
    void OnUnregResult(const std::shared_ptr<DScreen> &dScreen, const std::string &reqId,
        const int32_t status, const std::string &data) override;
};

class DScreenGroupListener : public Rosen::ScreenManager::IScreenGroupListener {
public:
    void OnChange(const std::vector<uint64_t> &screenIds, Rosen::ScreenGroupChangeEvent event) override;
};

class DScreenManager : public std::enable_shared_from_this<DScreenManager> {
DECLARE_SINGLE_INSTANCE_BASE(DScreenManager);

public:
    DScreenManager();
    ~DScreenManager();

    void OnRegResult(const std::shared_ptr<DScreen> &dScreen, const std::string &reqId,
        int32_t status, const std::string &data);
    void OnUnregResult(const std::shared_ptr<DScreen> &dScreen, const std::string &reqId,
        int32_t status, const std::string &data);

    int32_t Initialize();
    int32_t Release();
    int32_t EnableDistributedScreen(const std::string &devId, const std::string &dhId, const EnableParam &param,
        const std::string &reqId);
    int32_t DisableDistributedScreen(const std::string &devId, const std::string &dhId, const std::string &reqId);
    void RegisterDScreenCallback(const sptr<IDScreenSourceCallback> &callback);
    void HandleScreenChange(const std::shared_ptr<DScreen> &changedScreen, Rosen::ScreenGroupChangeEvent event);
    std::shared_ptr<DScreen> FindDScreenById(uint64_t screenId);
    void GetScreenDumpInfo(std::string &result);

private:
    int32_t StartDScreenMirror(const std::shared_ptr<DScreen> &dScreen);
    int32_t StopDScreenMirror(const std::shared_ptr<DScreen> &dScreen);
    int32_t LoadAVSenderEngineProvider();
    int32_t UnloadAVSenderEngineProvider();
    void PublishMessage(const DHTopic topic, const std::shared_ptr<DScreen> &dScreen);

private:
    std::string localDevId_;
    std::mutex dScreenMapMtx_;
    std::map<std::string, std::shared_ptr<DScreen>> dScreens_;

    IAVEngineProvider *providerPtr_ = nullptr;
    sptr<DScreenGroupListener> dScreenGroupListener_ = nullptr;
    std::shared_ptr<IDScreenCallback> dScreenCallback_ = nullptr;
    sptr<IDScreenSourceCallback> dScreenSourceCallbackProxy_ = nullptr;
};
} // namespace V2_0
} // namespace DistributedHardware
} // namespace OHOS
#endif