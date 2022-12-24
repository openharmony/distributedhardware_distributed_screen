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

#include "dscreen.h"

#include "avcodec_info.h"
#include "avcodec_list.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_json_util.h"
#include "dscreen_log.h"
#include "dscreen_util.h"
#include "screen_manager_adapter.h"
#include "screen_source_trans.h"

namespace OHOS {
namespace DistributedHardware {
DScreen::DScreen(const std::string &devId, const std::string &dhId,
    std::shared_ptr<IDScreenCallback> dscreenCallback)
{
    DHLOGD("DScreen construct, devId: %s, dhId: %s", GetAnonyString(devId).c_str(),
        GetAnonyString(dhId).c_str());
    devId_ = devId;
    dhId_ = dhId;
    dscreenCallback_ = dscreenCallback;
    SetState(DISABLED);
    taskThreadRunning_ = true;
    taskQueueThread_ = std::thread(&DScreen::TaskThreadLoop, this);
}

DScreen::~DScreen()
{
    DHLOGD("DScreen deconstruct, devId: %s, dhId: %s", GetAnonyString(devId_).c_str(),
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
        DHLOGE("source trans release failed. ret: %d", ret);
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
    DHLOGD("OnTransError, err: %d", err);
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
    DHLOGI("DScreen::AddTask, devId: %s, dhId: %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    if (task == nullptr) {
        DHLOGE("AddTask, task is invalid.");
        return ERR_DH_SCREEN_SA_DSCREEN_TASK_NOT_VALID;
    }
    DHLOGI("AddTask, task type: %d", task->GetTaskType());
    {
        std::lock_guard<std::mutex> lock(taskQueueMtx_);
        taskQueue_.push(task);
    }
    taskQueueCond_.notify_all();
    return DH_SUCCESS;
}

void DScreen::TaskThreadLoop()
{
    DHLOGI("DScreen taskThread start. devId: %s, dhId: %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    while (taskThreadRunning_) {
        std::shared_ptr<Task> task;
        {
            std::unique_lock<std::mutex> lock(taskQueueMtx_);
            taskQueueCond_.wait_for(lock, std::chrono::seconds(TASK_WAIT_SECONDS),
                [this]() { return !taskQueue_.empty(); });
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

        DHLOGD("run task, task queue size: %zu", taskQueue_.size());
        HandleTask(task);
    }
}

void DScreen::HandleTask(const std::shared_ptr<Task> &task)
{
    int32_t taskType = task->GetTaskType();
    DHLOGI("HandleTask, devId: %s, dhId: %s, task type: %d", GetAnonyString(devId_).c_str(),
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
    DHLOGI("HandleEnable, devId: %s, dhId: %s", GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
    if (curState_ == ENABLED || curState_ == ENABLING || curState_ == CONNECTING || curState_ == CONNECTED) {
        dscreenCallback_->OnRegResult(shared_from_this(), taskId, DH_SUCCESS, "dscreen enable success.");
        return;
    }

    SetState(ENABLING);
    if (videoParam_ == nullptr) {
        videoParam_ = std::make_shared<VideoParam>();
    }

    json attrJson = json::parse(param, nullptr, false);
    int32_t ret = CheckJsonData(attrJson);
    if (ret != DH_SUCCESS) {
        DHLOGE("check json data failed.");
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
        DHLOGE("negotiate codec type failed.");
        dscreenCallback_->OnRegResult(shared_from_this(), taskId, ERR_DH_SCREEN_SA_ENABLE_FAILED,
            "negotiate codec type failed.");
        ReportRegisterFail(DSCREEN_REGISTER_FAIL, ERR_DH_SCREEN_SA_ENABLE_FAILED, GetAnonyString(devId_).c_str(),
            GetAnonyString(dhId_).c_str(), "negotiate codec type failed.");
        return;
    }

    uint64_t screenId = ScreenMgrAdapter::GetInstance().CreateVirtualScreen(devId_, dhId_, videoParam_);
    if (screenId == SCREEN_ID_INVALID) {
        DHLOGE("create virtual screen failed.");
        dscreenCallback_->OnRegResult(shared_from_this(), taskId, ERR_DH_SCREEN_SA_ENABLE_FAILED,
            "create virtual screen failed.");
        ReportRegisterFail(DSCREEN_REGISTER_FAIL, ERR_DH_SCREEN_SA_ENABLE_FAILED, GetAnonyString(devId_).c_str(),
            GetAnonyString(dhId_).c_str(), "create virtual screen failed.");
        return;
    }
    screenId_ = screenId;
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
        !IsString(attrJson, KEY_CODECTYPE)) {
        DHLOGE("enable param is invalid.");
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
    std::shared_ptr<Media::AVCodecList> codecList = Media::AVCodecListFactory::CreateAVCodecList();
    if (codecList == nullptr) {
        DHLOGE("codecList is nullptr.");
        return ERR_DH_SCREEN_SA_DSCREEN_NEGOTIATE_CODEC_FAIL;
    }
    std::vector<std::shared_ptr<Media::VideoCaps>> caps = codecList->GetVideoEncoderCaps();
    for (const auto &cap : caps) {
        std::shared_ptr<Media::AVCodecInfo> codecInfo = cap->GetCodecInfo();
        if (codecInfo == nullptr) {
            continue;
        }
        localCodecArray.push_back(codecInfo->GetName());
    }

    std::vector<std::string> codecTypeCandidates;
    for (const auto &remoteCodecType : remoteCodecArray) {
        if (std::find(localCodecArray.begin(), localCodecArray.end(),
            remoteCodecType) != localCodecArray.end()) {
            codecTypeCandidates.push_back(remoteCodecType);
        }
    }

    if (std::find(codecTypeCandidates.begin(), codecTypeCandidates.end(),
        CODEC_NAME_H264) != codecTypeCandidates.end()) {
        videoParam_->SetCodecType(VIDEO_CODEC_TYPE_VIDEO_H264);
        videoParam_->SetVideoFormat(VIDEO_DATA_FORMAT_NV12);
    } else if (std::find(codecTypeCandidates.begin(), codecTypeCandidates.end(),
        CODEC_NAME_MPEG4) != codecTypeCandidates.end()) {
        videoParam_->SetCodecType(VIDEO_CODEC_TYPE_VIDEO_MPEG4);
        videoParam_->SetVideoFormat(VIDEO_DATA_FORMAT_RGBA8888);
    } else {
        DHLOGI("codec type not support.");
        return ERR_DH_SCREEN_SA_DSCREEN_NEGOTIATE_CODEC_FAIL;
    }

    return DH_SUCCESS;
}

void DScreen::HandleDisable(const std::string &taskId)
{
    DHLOGI("HandleDisable, devId: %s, dhId: %s", GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
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
    dscreenCallback_->OnUnregResult(shared_from_this(), taskId, DH_SUCCESS, "");
    ReportUnRegisterScreenEvent(DSCREEN_UNREGISTER, GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(),
        "dscreen disable success.");
}

void DScreen::HandleConnect()
{
    DHLOGI("HandleConnect, devId: %s, dhId: %s", GetAnonyString(devId_).c_str(),
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
    DHLOGD("HandleDisconnect, devId: %s, dhId: %s", GetAnonyString(devId_).c_str(),
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
    ReportScreenMirrorEvent(DSCREEN_PROJECT_END, GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(),
        "dscreen disconnect success");
}

int32_t DScreen::SetUp()
{
    DHLOGD("SetUp, devId: %s, dhId: %s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());

    if (sourceTrans_ == nullptr) {
        sourceTrans_ = std::make_shared<ScreenSourceTrans>();
    }

    sourceTrans_->RegisterStateCallback(shared_from_this());
    int32_t ret = sourceTrans_->SetUp(*videoParam_, *videoParam_, devId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("source trans SetUp failed.");
        return ret;
    }

    sptr<OHOS::Surface> surface = sourceTrans_->GetImageSurface();
    if (surface == nullptr) {
        DHLOGE("DScreen SetUp failed.");
        return ERR_DH_SCREEN_SA_DSCREEN_SETUP_FAILED;
    }

    ScreenMgrAdapter::GetInstance().SetImageSurface(screenId_, surface);
    DHLOGI("DScreen SetUp success.");
    return DH_SUCCESS;
}

int32_t DScreen::Start()
{
    DHLOGD("Start, devId: %s, dhId: %s", GetAnonyString(devId_).c_str(),
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
    return DH_SUCCESS;
}

int32_t DScreen::Stop()
{
    DHLOGD("Stop, devId: %s, dhId: %s", GetAnonyString(devId_).c_str(),
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
} // namespace DistributedHardware
} // namespace OHOS