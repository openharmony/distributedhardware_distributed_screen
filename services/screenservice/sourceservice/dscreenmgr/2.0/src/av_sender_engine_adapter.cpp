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

#include "2.0/include/av_sender_engine_adapter.h"

#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "dscreen_util.h"
#include "dscreen_hidumper.h"
#include "hitrace_meter.h"
#include "dscreen_hitrace.h"

namespace OHOS {
namespace DistributedHardware {
int32_t AVTransSenderAdapter::Initialize(IAVEngineProvider *providerPtr, const std::string &peerDevId)
{
    DHLOGI("Initialize enter");
    if (initialized_.load()) {
        return DH_SUCCESS;
    }
    if (providerPtr == nullptr) {
        DHLOGE("av transport sender engine provider ptr is null");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    senderEngine_ = providerPtr->CreateAVSenderEngine(peerDevId);
    if (senderEngine_ == nullptr) {
        DHLOGE("create av transport sender engine is null");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    senderEngine_->RegisterSenderCallback(shared_from_this());
    initialized_ = true;
    return DH_SUCCESS;
}

int32_t AVTransSenderAdapter::Release()
{
    DHLOGI("Release enter");
    if (senderEngine_ != nullptr) {
        StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SOURCE_CLOSE_SESSION_START);
        StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SOURCE_RELEASE_SESSION_START);
        int32_t ret = senderEngine_->Release();
        FinishTrace(DSCREEN_HITRACE_LABEL);
        FinishTrace(DSCREEN_HITRACE_LABEL);
        if (ret != DH_AVT_SUCCESS) {
            DHLOGE("release av transport sender engine failed");
        }
        senderEngine_ = nullptr;
    }
    initialized_ = false;
    chnCreateSuccess_ = false;
    transStartSuccess_ = false;
    return DH_SUCCESS;
}

int32_t AVTransSenderAdapter::Start()
{
    DHLOGI("Start enter");
    if (transStartSuccess_.load()) {
        DHLOGI("av transport sender channel already created");
        return DH_SUCCESS;
    }
    if (senderEngine_ == nullptr) {
        DHLOGE("av transport sender engine is null");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    int32_t ret = senderEngine_->Start();
    if (ret != DH_AVT_SUCCESS) {
        DHLOGE("start av transport sender engine failed, ret:%{public}" PRId32, ret);
        return ERR_DH_AV_TRANS_START_FAILED;
    }
    DHLOGI("Start Success");
    return DH_SUCCESS;
}

int32_t AVTransSenderAdapter::Stop()
{
    DHLOGI("Stop enter");
    if (senderEngine_ == nullptr) {
        DHLOGE("av transport sender engine is null");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    int32_t ret = senderEngine_->Stop();
    if (ret != DH_AVT_SUCCESS) {
        DHLOGE("stop av transport sender engine failed, ret:%{public}" PRId32, ret);
        return ERR_DH_AV_TRANS_STOP_FAILED;
    }
    DHLOGI("Stop Success");
    transStartSuccess_ = false;
    return DH_SUCCESS;
}

int32_t AVTransSenderAdapter::CreateControlChannel(const std::string& peerDevId)
{
    DHLOGI("CreateControlChannel enter, peerDevId:%{public}s", GetAnonyString(peerDevId).c_str());
    if (chnCreateSuccess_.load()) {
        DHLOGI("av transport sender channel already created");
        return DH_SUCCESS;
    }
    if (senderEngine_ == nullptr) {
        DHLOGE("av transport sender engine is null");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    std::vector<std::string> dstDevIds = {peerDevId};
    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_SOURCE_OPEN_SESSION_START);
    int32_t ret = senderEngine_->CreateControlChannel(dstDevIds, ChannelAttribute{TransStrategy::LOW_LATANCY_STRATEGY});
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_AVT_SUCCESS) {
        DHLOGE("create av transport sender channel failed, ret:%{public}" PRId32, ret);
        return ERR_DH_AV_TRANS_CREATE_CHANNEL_FAILED;
    }
    ret = WaitForChannelCreated();
    if (ret != DH_SUCCESS) {
        DHLOGE("wait for create av transport sender channel failed, ret:%{public}" PRId32, ret);
        return ERR_DH_AV_TRANS_CREATE_CHANNEL_FAILED;
    }
    DHLOGI("CreateControlChannel Success, peerDevId:%{public}s", GetAnonyString(peerDevId).c_str());
    return DH_SUCCESS;
}

int32_t AVTransSenderAdapter::SetParameter(const AVTransTag &tag, const std::string &param)
{
    DHLOGI("AVTransSenderAdapter::SetParameter enter");
    if (senderEngine_ == nullptr) {
        DHLOGE("av transport sender engine is null");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    int32_t ret = senderEngine_->SetParameter(tag, param);
    if (ret != DH_AVT_SUCCESS) {
        DHLOGE("set av transport sender parameter failed, ret:%{public}" PRId32, ret);
        return ERR_DH_AV_TRANS_SETUP_FAILED;
    }
    return DH_SUCCESS;
}

int32_t AVTransSenderAdapter::PushData(const VideoData &video)
{
    if (senderEngine_ == nullptr) {
        DHLOGE("av transport sender engine is null");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    auto transBuffer = std::make_shared<AVTransBuffer>(MetaType::VIDEO);
    auto bufferData = transBuffer->WrapBufferData(video.data, video.size, video.size);
    auto bufferMata = transBuffer->GetBufferMeta();
    bufferMata->SetMetaItem(AVTransTag::BUFFER_DATA_TYPE, std::to_string(static_cast<uint32_t>(MetaType::VIDEO)));
    bufferMata->SetMetaItem(AVTransTag::VIDEO_WIDTH, std::to_string(video.width));
    bufferMata->SetMetaItem(AVTransTag::VIDEO_HEIGHT, std::to_string(video.height));
    bufferMata->SetMetaItem(AVTransTag::VIDEO_PIXEL_FORMAT, video.format);
    bufferMata->SetMetaItem(AVTransTag::PRE_TIMESTAMP, std::to_string(video.timestamp));
#ifdef DUMP_DSCREEN_FILE
    if (DscreenHidumper::GetInstance().GetFlagStatus()) {
        senderEngine_->StartDumpMediaData();
    } else {
        senderEngine_->StopDumpMediaData();
    }
    if (DscreenHidumper::GetInstance().GetTransReDumpFlag()) {
        senderEngine_->ReStartDumpMediaData();
        DscreenHidumper::GetInstance().SetTransReDumpFlagFalse();
    }
#endif
    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_START_ENCODER_START);
    int32_t ret = senderEngine_->PushData(transBuffer);
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_AVT_SUCCESS) {
        DHLOGE("feed data to av transport sender failed, ret:%{public}" PRId32, ret);
        return ERR_DH_AV_TRANS_FEED_DATA_FAILED;
    }
    return DH_SUCCESS;
}

int32_t AVTransSenderAdapter::SendMessageToRemote(const std::shared_ptr<AVTransMessage> &message)
{
    DHLOGI("AVTransSenderAdapter::SendMessageToRemote enter");
    if (senderEngine_ == nullptr) {
        DHLOGE("av transport sender engine is null");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    int32_t ret = senderEngine_->SendMessage(message);
    if (ret != DH_AVT_SUCCESS) {
        DHLOGE("send meassage to remote receiver engine failed, ret:%{public}" PRId32, ret);
        return ERR_DH_AV_TRANS_SEND_MSG_FAILED;
    }
    return DH_SUCCESS;
}

int32_t AVTransSenderAdapter::RegisterAdapterCallback(const std::shared_ptr<AVSenderAdapterCallback> &callback)
{
    if (callback == nullptr) {
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    adapterCallback_ = callback;
    return DH_SUCCESS;
}

int32_t AVTransSenderAdapter::WaitForChannelCreated()
{
    std::unique_lock<std::mutex> lock(chnCreatedMtx_);
    auto status = chnCreatedCondVar_.wait_for(lock, std::chrono::milliseconds(WAIT_TIMEOUT_MS),
        [this]() { return chnCreateSuccess_.load(); });
    if (!status) {
        DHLOGE("wait for av transport sender channel created timeout");
        return ERR_DH_AV_TRANS_TIMEOUT;
    }
    if (!chnCreateSuccess_.load()) {
        DHLOGE("create av transport sender channel failed");
        return ERR_DH_AV_TRANS_CREATE_CHANNEL_FAILED;
    }
    return DH_SUCCESS;
}

int32_t AVTransSenderAdapter::WaitForAVTransStarted()
{
    std::unique_lock<std::mutex> lock(transStartedMtx_);
    auto status = transStartedCondVar_.wait_for(lock, std::chrono::milliseconds(WAIT_TIMEOUT_MS),
        [this]() { return transStartSuccess_.load(); });
    if (!status) {
        DHLOGE("wait for av transport sender started timeout");
        return ERR_DH_AV_TRANS_TIMEOUT;
    }
    if (!transStartSuccess_.load()) {
        DHLOGE("start av transport sender engine failed");
        return ERR_DH_AV_TRANS_START_FAILED;
    }
    return DH_SUCCESS;
}

int32_t AVTransSenderAdapter::OnSenderEvent(const AVTransEvent& event)
{
    DHLOGI("OnSenderEvent enter. event type:%{public}" PRId32, event.type);
    switch (event.type) {
        case EventType::EVENT_CHANNEL_OPEN_FAIL:
        case EventType::EVENT_CHANNEL_OPENED: {
            chnCreateSuccess_ = (event.type == EventType::EVENT_CHANNEL_OPENED);
            chnCreatedCondVar_.notify_one();
            break;
        }
        case EventType::EVENT_CHANNEL_CLOSED: {
            chnCreateSuccess_ = false;
            if (adapterCallback_ != nullptr) {
                adapterCallback_->OnEngineEvent(DScreenEventType::TRANS_CHANNEL_CLOSED, event.peerDevId);
            }
            break;
        }
        case EventType::EVENT_START_FAIL:
        case EventType::EVENT_START_SUCCESS: {
            transStartSuccess_ = (event.type == EventType::EVENT_START_SUCCESS);
            transStartedCondVar_.notify_one();
            break;
        }
        case EventType::EVENT_ENGINE_ERROR:
        case EventType::EVENT_REMOTE_ERROR:
            if (adapterCallback_ != nullptr) {
                adapterCallback_->OnEngineEvent(DScreenEventType::ENGINE_ERROR, event.content);
            }
            break;
        default:
            DHLOGE("Invalid event type.");
    }
    return DH_AVT_SUCCESS;
}

int32_t AVTransSenderAdapter::OnMessageReceived(const std::shared_ptr<AVTransMessage> &message)
{
    if (adapterCallback_ != nullptr) {
        adapterCallback_->OnEngineMessage(message);
    }
    return DH_AVT_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS