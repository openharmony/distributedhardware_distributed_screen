/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "1.0/include/dscreen.h"

#include "avcodec_info.h"
#include "avcodec_list.h"
#include <pthread.h>

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_json_util.h"
#include "dscreen_log.h"
#include "dscreen_util.h"
#include "common/include/screen_manager_adapter.h"
#include "screen_source_trans.h"

namespace OHOS {
namespace DistributedHardware {
namespace V1_0 {
constexpr const char* TASK_THREAD = "TaskThread";
DScreen::DScreen(const std::string &devId, const std::string &dhId,
    std::shared_ptr<IDScreenCallback> dscreenCallback)
{
    DHLOGD("DScreen construct, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    devId_ = devId;
    dhId_ = dhId;
    dscreenCallback_ = dscreenCallback;
    SetState(DISABLED);
    taskThreadRunning_ = true;
    taskQueueThread_ = std::thread([this] { this->TaskThreadLoop(); });
}

DScreen::~DScreen()
{
    DHLOGD("DScreen deconstruct, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    taskThreadRunning_ = false;
    taskQueueCond_.notify_all();
    if (taskQueueThread_.joinable()) {
        taskQueueThread_.join();
    }
    int32_t ret = DH_SUCCESS;
    if (sourceTrans_ != nullptr) {
        ret = sourceTrans_->Release();
    }
    if (ret != DH_SUCCESS) {
        DHLOGE("source trans release failed. ret: %{public}" PRId32, ret);
    }

    if (screenId_ != SCREEN_ID_INVALID) {
        ret = ScreenMgrAdapter::GetInstance().RemoveVirtualScreen(screenId_);
    }

    if (ret != DH_SUCCESS) {
        DHLOGE("remove virtual screen failed.");
    }
    videoParam_ = nullptr;
    sourceTrans_ = nullptr;
    DHLOGD("DScreen deconstruct end.");
}

void DScreen::OnTransError(int32_t err, const std::string &content)
{
    DHLOGW("OnTransError, err: %{public}" PRId32, err);
    AddTask(std::make_shared<Task>(TaskType::TASK_DISCONNECT, ""));
    ScreenMgrAdapter::GetInstance().RemoveScreenFromGroup(screenId_);
}

void DScreen::SetVideoParam(std::shared_ptr<VideoParam> &videoParam)
{
    videoParam_ = videoParam;
}

std::shared_ptr<VideoParam> DScreen::GetVideoParam()
{
    return videoParam_;
}

void DScreen::SetState(DScreenState state)
{
    std::lock_guard<std::mutex> lock(stateMtx_);
    curState_ = state;
}

DScreenState DScreen::GetState() const
{
    return curState_;
}

uint64_t DScreen::GetScreenId() const
{
    return screenId_;
}

void DScreen::SetScreenVersion(const std::string &version)
{
    version_ = version;
}

std::string DScreen::GetScreenVersion()
{
    return version_;
}

std::string DScreen::GetDHId() const
{
    return dhId_;
}

std::string DScreen::GetDevId() const
{
    return devId_;
}

int32_t DScreen::AddTask(const std::shared_ptr<Task> &task)
{
    DHLOGI("DScreen::AddTask, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    if (task == nullptr) {
        DHLOGE("AddTask, task is invalid.");
        return ERR_DH_SCREEN_SA_DSCREEN_TASK_NOT_VALID;
    }
    DHLOGI("AddTask, task type: %{public}" PRId32, task->GetTaskType());
    {
        std::lock_guard<std::mutex> lock(taskQueueMtx_);
        taskQueue_.push(task);
    }
    taskQueueCond_.notify_all();
    return DH_SUCCESS;
}

void DScreen::TaskThreadLoop()
{
    DHLOGI("DScreen taskThread start. devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    int32_t ret = pthread_setname_np(pthread_self(), TASK_THREAD);
    if (ret != DH_SUCCESS) {
        DHLOGE("Dscreen set thread name failed, ret %{public}" PRId32, ret);
    }
    while (taskThreadRunning_) {
        std::shared_ptr<Task> task;
        {
            std::unique_lock<std::mutex> lock(taskQueueMtx_);
            auto status = taskQueueCond_.wait_for(lock, std::chrono::seconds(TASK_WAIT_SECONDS),
                [this]() { return !taskQueue_.empty(); });
            if (!status) {
                DHLOGD("Task queue wait timeout after %{public}d seconds.", TASK_WAIT_SECONDS);
                continue;
            }
            if (taskQueue_.empty()) {
                continue;
            }
            task = taskQueue_.front();
            taskQueue_.pop();
        }

        if (task == nullptr) {
            DHLOGD("task is null.");
            continue;
        }

        DHLOGD("run task, task queue size: %{public}zu", taskQueue_.size());
        HandleTask(task);
    }
}

void DScreen::HandleTask(const std::shared_ptr<Task> &task)
{
    int32_t taskType = task->GetTaskType();
    DHLOGI("HandleTask, devId: %{public}s, dhId: %{public}s, task type: %{public}" PRId32,
        GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str(), taskType);
    switch (taskType) {
        case TaskType::TASK_ENABLE:
            HandleEnable(task->GetTaskParam(), task->GetTaskId());
            break;
        case TaskType::TASK_DISABLE:
            HandleDisable(task->GetTaskId());
            break;
        case TaskType::TASK_CONNECT:
            HandleConnect();
            break;
        case TaskType::TASK_DISCONNECT:
            HandleDisconnect();
            break;
        default:
            DHLOGD("task type unkown.");
    }
}

void DScreen::HandleEnable(const std::string &param, const std::string &taskId)
{
    json attrJson = json::parse(param, nullptr, false);
    if (attrJson.is_discarded()) {
        DHLOGE("HandleEnable attrJson is invalid");
        return;
    }
    if (dscreenCallback_ == nullptr) {
        DHLOGE("DScreen::HandleEnable, dscreenCallback_ is nullptr");
        return;
    }
    DHLOGI("HandleEnable, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    if (curState_ == ENABLED || curState_ == ENABLING || curState_ == CONNECTING || curState_ == CONNECTED) {
        dscreenCallback_->OnRegResult(shared_from_this(), taskId, DH_SUCCESS, "dscreen enable success.");
        return;
    }
    SetState(ENABLING);
    if (videoParam_ == nullptr) {
        videoParam_ = std::make_shared<VideoParam>();
    }

    int32_t ret = CheckJsonData(attrJson);
    if (ret != DH_SUCCESS) {
        dscreenCallback_->OnRegResult(shared_from_this(), taskId, ERR_DH_SCREEN_SA_ENABLE_FAILED,
            "enable param json is invalid.");
        ReportRegisterFail(DSCREEN_REGISTER_FAIL, ERR_DH_SCREEN_SA_ENABLE_FAILED, GetAnonyString(devId_).c_str(),
            GetAnonyString(dhId_).c_str(), "check json data failed.");
        return;
    }

    videoParam_->SetScreenWidth(attrJson[KEY_SCREEN_WIDTH].get<uint32_t>());
    videoParam_->SetScreenHeight(attrJson[KEY_SCREEN_HEIGHT].get<uint32_t>());

    // negotiate codecType
    ret = NegotiateCodecType(attrJson[KEY_CODECTYPE]);
    if (ret != DH_SUCCESS) {
        dscreenCallback_->OnRegResult(shared_from_this(), taskId, ERR_DH_SCREEN_SA_ENABLE_FAILED,
            "negotiate codec type failed.");
        ReportRegisterFail(DSCREEN_REGISTER_FAIL, ERR_DH_SCREEN_SA_ENABLE_FAILED, GetAnonyString(devId_).c_str(),
            GetAnonyString(dhId_).c_str(), "negotiate codec type failed.");
        return;
    }

    screenId_ = ScreenMgrAdapter::GetInstance().CreateVirtualScreen(devId_, dhId_, videoParam_);
    if (screenId_ == SCREEN_ID_INVALID) {
        dscreenCallback_->OnRegResult(shared_from_this(), taskId, ERR_DH_SCREEN_SA_ENABLE_FAILED,
            "create virtual screen failed.");
        ReportRegisterFail(DSCREEN_REGISTER_FAIL, ERR_DH_SCREEN_SA_ENABLE_FAILED, GetAnonyString(devId_).c_str(),
            GetAnonyString(dhId_).c_str(), "create virtual screen failed.");
        return;
    }
    SetState(ENABLED);
    dscreenCallback_->OnRegResult(shared_from_this(), taskId, DH_SUCCESS, "dscreen enable success.");
    ReportRegisterScreenEvent(DSCREEN_REGISTER, GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(),
        "dscreen enable success.");
}

int32_t DScreen::CheckJsonData(json &attrJson)
{
    if (attrJson.is_discarded()) {
        DHLOGE("enable param json is invalid.");
        return ERR_DH_SCREEN_SA_ENABLE_JSON_ERROR;
    }

    if (!IsUInt32(attrJson, KEY_SCREEN_WIDTH) || !IsUInt32(attrJson, KEY_SCREEN_HEIGHT) ||
        !attrJson.contains(KEY_CODECTYPE)) {
        DHLOGE("enable param is invalid.");
        return ERR_DH_SCREEN_SA_ENABLE_JSON_ERROR;
    }

    if ((attrJson[KEY_SCREEN_WIDTH].get<uint32_t>() > DSCREEN_MAX_SCREEN_DATA_WIDTH) ||
        (attrJson[KEY_SCREEN_HEIGHT].get<uint32_t>() > DSCREEN_MAX_SCREEN_DATA_HEIGHT)) {
        DHLOGE("Screen width or height is out of range.");
        return ERR_DH_SCREEN_SA_ENABLE_JSON_ERROR;
    }
    return DH_SUCCESS;
}

int32_t DScreen::NegotiateCodecType(const std::string &remoteCodecInfoStr)
{
    json remoteCodecArray = json::parse(remoteCodecInfoStr, nullptr, false);
    if (remoteCodecArray.is_discarded() || !remoteCodecArray.is_array()) {
        DHLOGE("remoteCodecInfoStrjson is invalid.");
        return ERR_DH_SCREEN_SA_DSCREEN_NEGOTIATE_CODEC_FAIL;
    }

    std::vector<std::string> localCodecArray;
    // query local support encoder type
    std::shared_ptr<MediaAVCodec::AVCodecList> codecList = MediaAVCodec::AVCodecListFactory::CreateAVCodecList();
    if (codecList == nullptr) {
        DHLOGE("Create avCodecList failed.");
        return ERR_DH_SCREEN_SA_DSCREEN_NEGOTIATE_CODEC_FAIL;
    }
    const std::vector<std::string> encoderName = {std::string(MediaAVCodec::CodecMimeType::VIDEO_AVC),
                                                  std::string(MediaAVCodec::CodecMimeType::VIDEO_HEVC)};
    for (const auto &coder : encoderName) {
        MediaAVCodec::CapabilityData *capData = codecList->GetCapability(coder, true,
            MediaAVCodec::AVCodecCategory::AVCODEC_HARDWARE);
            if (capData == nullptr) {
                continue;
            }
        std::string mimeType = capData->mimeType;
        localCodecArray.push_back(mimeType);
    }

    std::vector<std::string> codecTypeCandidates;
    for (const auto &remoteCodecType : remoteCodecArray) {
        if (std::find(localCodecArray.begin(), localCodecArray.end(),
            remoteCodecType) != localCodecArray.end()) {
            codecTypeCandidates.push_back(remoteCodecType);
        }
    }

    if (std::find(codecTypeCandidates.begin(), codecTypeCandidates.end(),
        std::string(MediaAVCodec::CodecMimeType::VIDEO_AVC)) != codecTypeCandidates.end()) {
        videoParam_->SetCodecType(VIDEO_CODEC_TYPE_VIDEO_H264);
    } else if (std::find(codecTypeCandidates.begin(), codecTypeCandidates.end(),
        std::string(MediaAVCodec::CodecMimeType::VIDEO_HEVC)) != codecTypeCandidates.end()) {
        videoParam_->SetCodecType(VIDEO_CODEC_TYPE_VIDEO_H265);
    } else {
        DHLOGI("codec type not support.");
        return ERR_DH_SCREEN_SA_DSCREEN_NEGOTIATE_CODEC_FAIL;
    }
    videoParam_->SetVideoFormat(VIDEO_DATA_FORMAT_RGBA8888);
    videoParam_->SetPartialRefreshFlag(true);
    return DH_SUCCESS;
}

void DScreen::HandleDisable(const std::string &taskId)
{
    if (dscreenCallback_ == nullptr) {
        DHLOGE("DScreen::HandleDisable, dscreenCallback_ is nullptr");
        return;
    }
    DHLOGI("HandleDisable, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    SetState(DISABLING);
    int32_t ret = ScreenMgrAdapter::GetInstance().RemoveVirtualScreen(screenId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("remove virtual screen failed.");
        dscreenCallback_->OnUnregResult(shared_from_this(), taskId, ERR_DH_SCREEN_SA_DISABLE_FAILED,
            "remove virtual screen failed.");
        ReportUnRegisterFail(DSCREEN_UNREGISTER_FAIL, ERR_DH_SCREEN_SA_DISABLE_FAILED, GetAnonyString(devId_).c_str(),
            GetAnonyString(dhId_).c_str(), "remove virtual screen failed.");
        return;
    }
    SetState(DISABLED);
    dscreenCallback_->OnUnregResult(shared_from_this(), taskId, DH_SUCCESS, "dscreen disable success.");
    ReportUnRegisterScreenEvent(DSCREEN_UNREGISTER, GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(),
        "dscreen disable success.");
}

void DScreen::HandleConnect()
{
    DHLOGI("HandleConnect, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());

    int32_t ret = SetUp();
    if (ret != DH_SUCCESS) {
        SetState(ENABLED);
        ScreenMgrAdapter::GetInstance().RemoveScreenFromGroup(screenId_);
        DHLOGE("dScreen SetUp failed.");
        return;
    }

    ret = Start();
    if (ret != DH_SUCCESS) {
        SetState(ENABLED);
        ScreenMgrAdapter::GetInstance().RemoveScreenFromGroup(screenId_);
        DHLOGE("dScreen Start failed.");
        return;
    }
    SetState(CONNECTED);
    ReportScreenMirrorEvent(DSCREEN_PROJECT_START, GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(),
        "dscreen connect success");
}

void DScreen::HandleDisconnect()
{
    DHLOGD("HandleDisconnect, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    if (curState_ != CONNECTED) {
        DHLOGE("dscreen is not connected, cannot disconnect");
        return;
    }
    SetState(DISCONNECTING);
    int32_t ret = Stop();
    if (ret != DH_SUCCESS) {
        SetState(CONNECTED);
        DHLOGE("dScreen Stop failed.");
        return;
    }
    SetState(ENABLED);
    Rosen::RSInterfaces::GetInstance().SetVirtualScreenUsingStatus(false);
    ReportScreenMirrorEvent(DSCREEN_PROJECT_END, GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(),
        "dscreen disconnect success");
}

int32_t DScreen::SetUp()
{
    DHLOGD("SetUp, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());

    if (sourceTrans_ == nullptr) {
        sourceTrans_ = std::make_shared<ScreenSourceTrans>();
    }
    sourceTrans_->SetScreenVersion(version_);
    sourceTrans_->RegisterStateCallback(shared_from_this());
    int32_t ret = sourceTrans_->SetUp(*videoParam_, *videoParam_, devId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("source trans SetUp failed.");
        return ret;
    }
    DHLOGI("DScreen SetUp success.");
    return DH_SUCCESS;
}

int32_t DScreen::Start()
{
    DHLOGD("Start, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    if (sourceTrans_ == nullptr) {
        DHLOGE("source trans not init.");
        return ERR_DH_SCREEN_SA_SOURCETRANS_NOT_INIT;
    }
    int32_t ret = sourceTrans_->Start();
    if (ret != DH_SUCCESS) {
        DHLOGE("source trans start failed.");
        return ret;
    }
    sptr<OHOS::Surface> windowSurface = sourceTrans_->GetImageSurface();
    if (windowSurface == nullptr) {
        DHLOGE("DScreen SetUp failed.");
        return ERR_DH_SCREEN_SA_DSCREEN_SETUP_FAILED;
    }
    ScreenMgrAdapter::GetInstance().SetImageSurface(screenId_, windowSurface);
    return DH_SUCCESS;
}

int32_t DScreen::Stop()
{
    DHLOGD("Stop, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    if (sourceTrans_ == nullptr) {
        DHLOGE("source trans not init.");
        return ERR_DH_SCREEN_SA_SOURCETRANS_NOT_INIT;
    }

    int32_t ret = sourceTrans_->Stop();
    if (ret != DH_SUCCESS) {
        DHLOGE("source trans stop failed.");
        return ret;
    }

    ret = sourceTrans_->Release();
    if (ret != DH_SUCCESS) {
        DHLOGE("source trans release failed.");
        return ret;
    }

    sourceTrans_ = nullptr;
    return DH_SUCCESS;
}
} // namespace V1_0
} // namespace DistributedHardware
} // namespace OHOS