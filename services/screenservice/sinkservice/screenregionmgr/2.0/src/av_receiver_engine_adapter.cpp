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

#include "2.0/include/av_receiver_engine_adapter.h"

#include <cinttypes>
#include <dlfcn.h>
#include <fstream>

#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "dscreen_util.h"
#include "dscreen_hidumper.h"
#include "hitrace_meter.h"
#include "dscreen_hitrace.h"

namespace OHOS {
namespace DistributedHardware {
int32_t AVTransReceiverAdapter::Initialize(IAVEngineProvider *providerPtr, const std::string& peerDevId)
{
    DHLOGI("Initialize enter");
    if (initialized_.load()) {
        return DH_SUCCESS;
    }
    if (providerPtr == nullptr) {
        DHLOGE("av transport receiver engine provider ptr is null");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    receiverEngine_ = providerPtr->CreateAVReceiverEngine(peerDevId);
    if (receiverEngine_ == nullptr) {
        DHLOGE("create av transport receiver engine is null");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    receiverEngine_->RegisterReceiverCallback(shared_from_this());
    initialized_ = true;
#ifdef DUMP_DSCREENREGION_FILE
    if (DscreenHidumper::GetInstance().GetFlagStatus()) {
        receiverEngine_->StartDumpMediaData();
    } else {
        receiverEngine_->StopDumpMediaData();
    }
    if (DscreenHidumper::GetInstance().GetTransReDumpFlag()) {
        receiverEngine_->ReStartDumpMediaData();
        DscreenHidumper::GetInstance().SetTransReDumpFlagFalse();
    }
#endif
    return DH_SUCCESS;
}

int32_t AVTransReceiverAdapter::Release()
{
    DHLOGI("Release enter");
    if (receiverEngine_ != nullptr) {
        StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SINK_CLOSE_SESSION_START);
        StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SINK_RELEASE_SESSION_START);
        int32_t ret = receiverEngine_->Release();
        FinishTrace(DSCREEN_HITRACE_LABEL);
        FinishTrace(DSCREEN_HITRACE_LABEL);
        if (ret != DH_AVT_SUCCESS) {
            DHLOGE("Release av transport receiver engine failed");
        }
    }
    initialized_ = false;
    receiverEngine_ = nullptr;
    return DH_SUCCESS;
}

int32_t AVTransReceiverAdapter::Start()
{
    DHLOGI("Start enter");
    if (receiverEngine_ == nullptr) {
        DHLOGE("av transport receiver engine is null");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    return receiverEngine_->Start();
}

int32_t AVTransReceiverAdapter::Stop()
{
    DHLOGI("Stop enter");
    if (receiverEngine_ == nullptr) {
        DHLOGE("av transport receiver engine is null");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    int32_t ret = receiverEngine_->Stop();
    if (ret != DH_AVT_SUCCESS) {
        DHLOGE("stop av transport receiver engine failed, ret:%{public}" PRId32, ret);
        return ERR_DH_AV_TRANS_STOP_FAILED;
    }
    DHLOGI("Stop Success");
    return DH_SUCCESS;
}

int32_t AVTransReceiverAdapter::SetParameter(const AVTransTag &tag, const std::string &param)
{
    DHLOGI("SetParameter enter");
    if (receiverEngine_ == nullptr) {
        DHLOGE("av transport receiver engine is null");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    int32_t ret = receiverEngine_->SetParameter(tag, param);
    if (ret != DH_AVT_SUCCESS) {
        DHLOGE("set av transport receiver parameter failed, ret:%{public}" PRId32, ret);
        return ERR_DH_AV_TRANS_SETUP_FAILED;
    }
    return DH_SUCCESS;
}

int32_t AVTransReceiverAdapter::SendMessageToRemote(const std::shared_ptr<AVTransMessage> &message)
{
    DHLOGI("SendMessageToRemote enter");
    if (receiverEngine_ == nullptr) {
        DHLOGE("av transport receiver engine is null");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    int32_t ret = receiverEngine_->SendMessage(message);
    if (ret != DH_AVT_SUCCESS) {
        DHLOGE("send meassage to remote sender engine failed, ret:%{public}" PRId32, ret);
        return ERR_DH_AV_TRANS_SEND_MSG_FAILED;
    }
    return DH_SUCCESS;
}

int32_t AVTransReceiverAdapter::RegisterAdapterCallback(const std::shared_ptr<AVReceiverAdapterCallback> &callback)
{
    if (callback == nullptr) {
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    adapterCallback_ = callback;
    return DH_SUCCESS;
}

int32_t AVTransReceiverAdapter::OnReceiverEvent(const AVTransEvent& event)
{
    DHLOGI("OnReceiverEvent enter. event type:%{public}" PRId32, event.type);

    if (adapterCallback_ == nullptr) {
        DHLOGE("adapter callback is null.");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }

    switch (event.type) {
        case EventType::EVENT_START_SUCCESS:
            DHLOGI("av transport recevier engine start success.");
            break;
        case EventType::EVENT_CHANNEL_CLOSED:
            adapterCallback_->OnEngineEvent(DScreenEventType::TRANS_CHANNEL_CLOSED, event.peerDevId);
            break;
        case EventType::EVENT_ENGINE_ERROR:
        case EventType::EVENT_REMOTE_ERROR:
            adapterCallback_->OnEngineEvent(DScreenEventType::ENGINE_ERROR, event.content);
            break;
        default:
            DHLOGE("Invalid event type.");
    }
    return DH_AVT_SUCCESS;
}

int32_t AVTransReceiverAdapter::OnMessageReceived(const std::shared_ptr<AVTransMessage> &message)
{
    if (adapterCallback_ != nullptr) {
        adapterCallback_->OnEngineMessage(message);
    }
    return DH_AVT_SUCCESS;
}

int32_t AVTransReceiverAdapter::OnDataAvailable(const std::shared_ptr<AVTransBuffer> &buffer)
{
    if (adapterCallback_ != nullptr) {
        adapterCallback_->OnEngineDataDone(buffer);
    }
    if (receiverEngine_ == nullptr) {
        DHLOGE("av transport receiver engine is null");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
#ifdef DUMP_DSCREENREGION_FILE
    if (DscreenHidumper::GetInstance().GetFlagStatus()) {
        receiverEngine_->StartDumpMediaData();
    } else {
        receiverEngine_->StopDumpMediaData();
    }
    if (DscreenHidumper::GetInstance().GetTransReDumpFlag()) {
        receiverEngine_->ReStartDumpMediaData();
        DscreenHidumper::GetInstance().SetTransReDumpFlagFalse();
    }
#endif
    return DH_AVT_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS