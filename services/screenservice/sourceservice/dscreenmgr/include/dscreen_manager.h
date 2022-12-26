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

#ifndef OHOS_DSCREEN_MGR_H
#define OHOS_DSCREEN_MGR_H

#include "distributed_hardware_fwk_kit.h"
#include "dm_common.h"
#include "single_instance.h"
#include "screen_manager.h"

#include "dscreen.h"
#include "dscreen_maprelation.h"
#include "idscreen_sink.h"
#include "idscreen_source_callback.h"
#include "video_param.h"

namespace OHOS {
namespace DistributedHardware {
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

class DScreenManager {
DECLARE_SINGLE_INSTANCE_BASE(DScreenManager);
public:
    void OnRegResult(const std::shared_ptr<DScreen> &dScreen, const std::string &reqId,
        const int32_t status, const std::string &data);
    void OnUnregResult(const std::shared_ptr<DScreen> &dScreen, const std::string &reqId,
        const int32_t status, const std::string &data);

    int32_t Init();
    int32_t UnInit();
    int32_t EnableDistributedScreen(const std::string &devId, const std::string &dhId, const  std::string &param,
        const std::string &reqId);
    int32_t DisableDistributedScreen(const std::string &devId, const std::string &dhId, const std::string &reqId);
    void HandleDScreenNotify(const std::string &devId, int32_t eventCode, const std::string &eventContent);
    void RegisterDScreenCallback(const sptr<IDScreenSourceCallback> &callback);
    void HandleScreenChange(const std::shared_ptr<DScreen> &changedScreen, Rosen::ScreenGroupChangeEvent event);
    std::shared_ptr<DScreen> FindDScreenByScreenId(uint64_t screenId);
    int32_t AddToGroup(const std::shared_ptr<DScreen> &changedScreen, uint64_t screenId);
    int32_t RemoveFromGroup(const std::shared_ptr<DScreen> &changedScreen, uint64_t screenId);
    void GetScreenDumpInfo(std::string &result);
    void PublishMessage(const DHTopic topic, const std::shared_ptr<DScreen> &dScreen);
private:
    ~DScreenManager();
    DScreenManager();
    std::mutex dScreenMapMtx_;
    std::mutex dScreenMapRelationMtx_;
    std::map<uint64_t, std::shared_ptr<DScreenMapRelation>> mapRelations_;
    std::map<std::string, std::shared_ptr<DScreen>> dScreens_;
    sptr<IDScreenSourceCallback> dScreenSourceCallbackProxy_ = nullptr;
    sptr<DScreenGroupListener> dScreenGroupListener_ = nullptr;
    std::shared_ptr<IDScreenCallback> dScreenCallback_ = nullptr;
    std::string localDevId_;

    sptr<IDScreenSink> GetDScreenSinkSA(const std::string &devId);
    int32_t NotifyRemoteScreenService(const std::string &devId, int32_t eventCode, const std::string &eventContent);
    void NotifyRemoteSinkSetUp(const std::shared_ptr<DScreen> &dScreen);
    bool CheckContent(json &eventContent);
    void HandleNotifySetUpResult(const std::string &remoteDevId, const std::string &eventContent);
};
} // namespace DistributedHardware
} // namespace OHOS
#endif