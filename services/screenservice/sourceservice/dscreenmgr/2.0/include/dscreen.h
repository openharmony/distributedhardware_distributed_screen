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

#ifndef OHOS_DSCREEN_V2_0_H
#define OHOS_DSCREEN_V2_0_H

#include <condition_variable>
#include <cstdio>
#include <fstream>
#include <mutex>
#include <queue>
#include <thread>
#include <string>
#include "sync_fence.h"

#include "surface.h"
#include "iconsumer_surface.h"

#include "dscreen_constants.h"
#include "distributed_hardware_fwk_kit_paras.h"
#include "histreamer_ability_parser.h"
#include "video_param.h"
#include "av_sender_engine_adapter.h"
#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace DistributedHardware {
namespace V2_0 {
class DScreen;
class IDScreenCallback {
public:
    virtual ~IDScreenCallback() {};
    virtual void OnRegResult(const std::shared_ptr<DScreen> &dScreen,
        const std::string &reqId, const int32_t status, const std::string &data) = 0;
    virtual void OnUnregResult(const std::shared_ptr<DScreen> &dScreen,
        const std::string &reqId, const int32_t status, const std::string &data) = 0;
};

class Task {
public:
    Task(TaskType taskType, const std::string &taskId, const std::string &taskParam) : taskType_(taskType),
        taskId_(taskId), taskParam_(taskParam) {};
    Task(TaskType taskType, const std::string &taskParam) : taskType_(taskType),
        taskId_(""), taskParam_(taskParam) {};
    ~Task() {};

    TaskType GetTaskType() const
    {
        return taskType_;
    };
    std::string GetTaskId() const
    {
        return taskId_;
    };
    std::string GetTaskParam() const
    {
        return taskParam_;
    };

private:
    TaskType taskType_;
    std::string taskId_;
    std::string taskParam_;
};

class ConsumBufferListener : public IBufferConsumerListener {
public:
    ConsumBufferListener(const std::shared_ptr<DScreen> dScreen) : dScreen_(dScreen) {};
    ~ConsumBufferListener() = default;
    void OnBufferAvailable() override;
private:
    static const constexpr char *DSCREEN_LOG_TAG = "ConsumBufferListener";
    std::shared_ptr<DScreen> dScreen_;
};

class DScreen : public AVSenderAdapterCallback, public std::enable_shared_from_this<DScreen> {
public:
    DScreen(const std::string &devId, const std::string &dhId, std::shared_ptr<IDScreenCallback> dscreenCallback);
    ~DScreen();

    // interfaces from AVSenderAdapterCallback
    void OnEngineEvent(DScreenEventType event, const std::string &content) override;
    void OnEngineMessage(const std::shared_ptr<AVTransMessage> &message) override;

    int32_t AddTask(const std::shared_ptr<Task> &task);
    int32_t InitSenderEngine(IAVEngineProvider *providerPtr, const std::string &peerDevId);
    void ConsumeSurface();
    std::string GetDHId() const;
    std::string GetDevId() const;
    uint64_t GetScreenId() const;
    DScreenState GetState() const;
    std::shared_ptr<VideoParam> GetVideoParam();

private:
    void TaskThreadLoop();
    void HandleTask(const std::shared_ptr<Task> &task);
    void HandleEnable(const std::string &param, const std::string &taskId);
    void HandleDisable(const std::string &taskId);
    void HandleConnect();
    void HandleDisconnect();
    int32_t StartSenderEngine();
    int32_t StopSenderEngine();
    void ParseInputScreenParam(const std::string &param, const std::string &taskId);

    /*
     * Negotiate the codec format between local to remote device,
     * for DScreen, local encoder screen, remote decoder it.
     * rmtDecoderStr: remote Decoder description in JSON format
     */
    int32_t NegotiateCodecType(const std::string &rmtDecoderStr);
    int32_t ChooseCodecType(const std::vector<VideoEncoder> &localVideoEncoders,
        const std::vector<VideoDecoder> &rmtVideoDecoders);
    int32_t ConfigSurface();
    int32_t RemoveSurface();
    int32_t SetUp();
    void ChooseParameter(std::string &codecType, std::string &pixelFormat);
    bool CheckJsonData(const json &attrJson);
    void SetState(DScreenState state);
    int32_t WaitForSinkStarted();

    std::string devId_;
    std::string dhId_;
    uint64_t screenId_ = SCREEN_ID_INVALID;
    std::shared_ptr<VideoParam> videoParam_ = nullptr;
    std::shared_ptr<IDScreenCallback> dscreenCallback_ = nullptr;
    sptr<Surface> consumerSurface_ = nullptr;
    sptr<IBufferConsumerListener> consumerBufferListener_ = nullptr;

    DScreenState curState_;
    std::mutex stateMtx_;
    std::mutex ableMtx_;
    std::thread taskQueueThread_;
    std::condition_variable taskQueueCond_;
    std::mutex taskQueueMtx_;
    std::queue<std::shared_ptr<Task>> taskQueue_;
    bool taskThreadRunning_;
    bool watchdogFlag_;
    std::shared_ptr<AVTransSenderAdapter> senderAdapter_;
    OHOS::sptr<OHOS::SyncFence> syncFence_ = SyncFence::INVALID_FENCE;

    std::mutex waitSinkMtx_;
    std::condition_variable waitSinkCondVar_;
    std::atomic<bool> sinkStartSuccess_ {false};
};
} // namespace V2_0
} // namespace DistributedHardware
} // namespace OHOS
#endif