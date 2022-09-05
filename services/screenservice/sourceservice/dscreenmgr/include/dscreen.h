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

#ifndef OHOS_DSCREEN_H
#define OHOS_DSCREEN_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

#include "dscreen_constants.h"
#include "iscreen_source_trans.h"
#include "iscreen_source_trans_callback.h"
#include "video_param.h"

namespace OHOS {
namespace DistributedHardware {
class DScreen;
class IDScreenCallback {
public:
    virtual ~IDScreenCallback() {};
    virtual void OnRegResult(const std::shared_ptr<DScreen> &dScreen,
        const std::string &reqId, const int32_t status, const std::string &data) = 0;
    virtual void OnUnregResult(const std::shared_ptr<DScreen> &dScreen,
        const std::string &reqId, const int32_t status, const std::string &data) = 0;
};

class ScreenSourceTransCallback : public IScreenSourceTransCallback {
public:
    ~ScreenSourceTransCallback() override {};
    virtual void OnTransError(int32_t err, const std::string &content) = 0;
    virtual void OnError(int32_t err, const std::string &content) override
    {
        OnTransError(err, content);
    }
};

class Task {
public:
    Task(TaskType taskType, const std::string &taskId, const std::string &taskParam) : taskType_(taskType),
        taskId_(taskId), taskParam_(taskParam) {};
    Task(TaskType taskType, const std::string &taskParam) : taskType_(taskType),
        taskId_(""), taskParam_(taskParam) {};
    ~Task() {};

    TaskType GetTaskType()
    {
        return taskType_;
    };
    std::string GetTaskId()
    {
        return taskId_;
    };
    std::string GetTaskParam()
    {
        return taskParam_;
    };

private:
    TaskType taskType_;
    std::string taskId_;
    std::string taskParam_;
};

class DScreen : public ScreenSourceTransCallback, public std::enable_shared_from_this<DScreen> {
public:
    ~DScreen();
    DScreen(const std::string &devId, const std::string &dhId, std::shared_ptr<IDScreenCallback> dscreenCallback);
    void OnTransError(int32_t err, const std::string &content) override;

    void SetVideoParam(std::shared_ptr<VideoParam> &videoParam);
    std::shared_ptr<VideoParam> GetVideoParam();
    void SetState(DScreenState state);
    DScreenState GetState() const;
    uint64_t GetScreenId() const;
    std::string GetDHId() const;
    std::string GetDevId() const;
    int32_t AddTask(const std::shared_ptr<Task> &task);

private:
    void TaskThreadLoop();
    void HandleTask(const std::shared_ptr<Task> &task);
    void HandleEnable(const std::string &param, const std::string &taskId);
    void HandleDisable(const std::string &taskId);
    void HandleConnect();
    void HandleDisconnect();
    int32_t CheckJsonData(json &attrJson);
    int32_t NegotiateCodecType(const std::string &remoteCodecInfoStr);
    int32_t SetUp();
    int32_t Start();
    int32_t Stop();

    static constexpr uint8_t TASK_WAIT_SECONDS = 1;

    std::string devId_;
    std::string dhId_;
    uint64_t screenId_ = SCREEN_ID_INVALID;
    std::shared_ptr<VideoParam> videoParam_ = nullptr;
    std::shared_ptr<IScreenSourceTrans> sourceTrans_ = nullptr;
    std::shared_ptr<IDScreenCallback> dscreenCallback_ = nullptr;

    DScreenState curState_;
    std::mutex stateMtx_;
    std::thread taskQueueThread_;
    std::condition_variable taskQueueCond_;
    std::mutex taskQueueMtx_;
    std::queue<std::shared_ptr<Task>> taskQueue_;
    bool taskThreadRunning_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif