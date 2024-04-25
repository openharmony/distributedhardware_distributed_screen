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

#include "2.0/include/dscreen.h"

#include "avcodec_info.h"
#include "avcodec_list.h"
#include "2.0/include/av_sender_engine_adapter.h"
#include "distributed_hardware_fwk_kit.h"
#include "histreamer_query_tool.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_fwkkit.h"
#include "dscreen_hisysevent.h"
#include "dscreen_json_util.h"
#include "dscreen_log.h"
#include "dscreen_hidumper.h"
#include "dscreen_util.h"
#include "xcollie/watchdog.h"
#include "common/include/screen_manager_adapter.h"
#include "cJSON.h"

namespace OHOS {
namespace DistributedHardware {
namespace V2_0 {
/* <<ecoder, decoder>, codec> */
static const std::map<std::pair<std::string, std::string>, std::string> CODECS_MAP = {
    {{"HdiCodecAdapter.OMX.rk.video_encoder.hevc", "HdiCodecAdapter.OMX.rk.video_decoder.hevc"}, CODEC_NAME_H265},
    {{"HdiCodecAdapter.OMX.hisi.video.encoder.hevc", "HdiCodecAdapter.OMX.hisi.video.decoder.hevc"}, CODEC_NAME_H265},
    {{"HdiCodecAdapter.OMX.rk.video_encoder.avc", "HdiCodecAdapter.OMX.rk.video_decoder.avc"}, CODEC_NAME_H264},
    {{"HdiCodecAdapter.OMX.hisi.video.encoder.avc", "HdiCodecAdapter.OMX.hisi.video.decoder.avc"}, CODEC_NAME_H264},
};

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
    watchdogFlag_ = false;
    taskQueueThread_ = std::thread(&DScreen::TaskThreadLoop, this);
    auto taskFunc = [this]() {
        if (watchdogFlag_) {
            watchdogFlag_ = false;
        } else {
            DHLOGE("Watchdog : Dscreen TaskThreadLoop dead.");
            _Exit(0);
        }
    };
    OHOS::HiviewDFX::Watchdog::GetInstance().RunPeriodicalTask("dscreenwatchdog", taskFunc, WATCHDOG_INTERVAL_TIME_MS,
        WATCHDOG_DELAY_TIME_MS);
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
    StopSenderEngine();
    ScreenMgrAdapter::GetInstance().RemoveVirtualScreen(screenId_);
    videoParam_ = nullptr;
    senderAdapter_ = nullptr;
    sinkStartSuccess_ = false;
    DHLOGD("DScreen deconstruct end.");
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

void DScreen::HandleTask(const std::shared_ptr<Task> &task)
{
    int32_t taskType = task->GetTaskType();
    DHLOGI("HandleTask, devId: %{public}s, dhId: %{public}s, task type: %{public}" PRId32,
        GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(), taskType);
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
    DHLOGI("HandleEnable, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    if (dscreenCallback_ == nullptr) {
        DHLOGE("DScreen::HandleEnable, dscreenCallback_ is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(ableMtx_);
    if ((curState_ == ENABLED) || (curState_ == ENABLING) || (curState_ == CONNECTING) ||
         (curState_ == CONNECTED)) {
        dscreenCallback_->OnRegResult(shared_from_this(), taskId, DH_SUCCESS, "dscreen enable success.");
        return;
    }
    SetState(ENABLING);
    ParseInputScreenParam(param, taskId);
    if (videoParam_ == nullptr) {
        DHLOGE("DScreen::HandleEnable, videoParam_ is nullptr");
        return;
    }
    uint64_t screenId = ScreenMgrAdapter::GetInstance().CreateVirtualScreen(devId_, dhId_, videoParam_);
    if (screenId == SCREEN_ID_INVALID) {
        DHLOGE("HandleEnable, create virtual screen failed.");
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

void DScreen::ParseInputScreenParam(const std::string &param, const std::string &taskId)
{
    json attrJson = json::parse(param, nullptr, false);
    if (!CheckJsonData(attrJson)) {
        DHLOGE("HandleEnable, check json data failed.");
        dscreenCallback_->OnRegResult(shared_from_this(), taskId, ERR_DH_SCREEN_SA_ENABLE_FAILED,
            "enable param json is invalid.");
        ReportRegisterFail(DSCREEN_REGISTER_FAIL, ERR_DH_SCREEN_SA_ENABLE_FAILED, GetAnonyString(devId_).c_str(),
            GetAnonyString(dhId_).c_str(), "check json data failed.");
        SetState(DISABLED);
        return;
    }
    if (videoParam_ == nullptr) {
        videoParam_ = std::make_shared<VideoParam>();
    }
    int32_t ret = NegotiateCodecType(attrJson[KEY_HISTREAMER_VIDEO_DECODER]);
    if (ret != DH_SUCCESS) {
        DHLOGE("HandleEnable, negotiate codec type failed.");
        dscreenCallback_->OnRegResult(shared_from_this(), taskId, ERR_DH_SCREEN_SA_ENABLE_FAILED,
            "negotiate codec type failed.");
        ReportRegisterFail(DSCREEN_REGISTER_FAIL, ERR_DH_SCREEN_SA_ENABLE_FAILED, GetAnonyString(devId_).c_str(),
            GetAnonyString(dhId_).c_str(), "negotiate codec type failed.");
        return;
    }
    videoParam_->SetScreenWidth(attrJson[KEY_SCREEN_WIDTH].get<uint32_t>());
    videoParam_->SetScreenHeight(attrJson[KEY_SCREEN_HEIGHT].get<uint32_t>());
}

void DScreen::HandleDisable(const std::string &taskId)
{
    DHLOGI("HandleDisable, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    if (dscreenCallback_ == nullptr) {
        DHLOGE("DScreen::HandleDisable, dscreenCallback_ is nullptr");
        return;
    }
    SetState(DISABLING);
    int32_t ret = ScreenMgrAdapter::GetInstance().RemoveVirtualScreen(screenId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("HandleDisable, remove virtual screen failed.");
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
    DHLOGI("HandleConnect, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    if (GetState() != ENABLED) {
        DHLOGE("GetState is not ENABLED, HandleConnect failed.");
        return;
    }
    SetState(CONNECTING);
    int32_t ret = StartSenderEngine();
    if (ret != DH_SUCCESS) {
        SetState(ENABLED);
        ScreenMgrAdapter::GetInstance().RemoveScreenFromGroup(screenId_);
        DHLOGE("HandleConnect, start av transport sender engine failed.");
        return;
    }
    ret = ConfigSurface();
    if (ret != DH_SUCCESS) {
        SetState(ENABLED);
        ScreenMgrAdapter::GetInstance().RemoveScreenFromGroup(screenId_);
        DHLOGE("HandleConnect, config image surface failed.");
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
    int32_t ret = StopSenderEngine();
    if (ret != DH_SUCCESS) {
        SetState(CONNECTED);
        DHLOGE("dScreen Stop failed.");
        return;
    }
    SetState(ENABLED);
    RemoveSurface();
    Rosen::RSInterfaces::GetInstance().SetVirtualScreenUsingStatus(false);
    ReportScreenMirrorEvent(DSCREEN_PROJECT_END, GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str(),
        "dscreen disconnect success");
}

int32_t DScreen::ConfigSurface()
{
    DHLOGD("ConfigSurface, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    consumerSurface_ = Surface::CreateSurfaceAsConsumer();
    if (consumerSurface_ == nullptr) {
        DHLOGE("Create consumer surface failed.");
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }
    sptr<IBufferProducer> producer = consumerSurface_->GetProducer();
    if (producer == nullptr) {
        DHLOGE("Get preducer surface failed.");
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }
    sptr<OHOS::Surface> producerSurface = Surface::CreateSurfaceAsProducer(producer);
    if (producerSurface == nullptr) {
        DHLOGE("Create preducer surface failed.");
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }
    if (consumerBufferListener_ == nullptr) {
        consumerBufferListener_ = new ConsumBufferListener(shared_from_this());
    }
    consumerSurface_->RegisterConsumerListener(consumerBufferListener_);
    ScreenMgrAdapter::GetInstance().SetImageSurface(screenId_, producerSurface);
    DHLOGI("ConfigSurface success.");
    return DH_SUCCESS;
}

int32_t DScreen::RemoveSurface()
{
    if (consumerSurface_ == nullptr) {
        DHLOGE("consumerSurface_ is nullptr.");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    consumerSurface_->UnregisterConsumerListener();

    consumerSurface_ = nullptr;
    DHLOGI("RemoveSurface success.");
    return DH_SUCCESS;
}

void ConsumBufferListener::OnBufferAvailable()
{
    DHLOGI("OnBufferAvailable enter.");
    if (dScreen_ == nullptr) {
        DHLOGE("dScreen is nullptr, cannot consume surface buffer.");
        return;
    }
    if (dScreen_->GetState() != CONNECTED) {
        DHLOGD("screen is not connected, no need consume surface buffer.");
        return;
    }
    dScreen_->ConsumeSurface();
}

void DScreen::ConsumeSurface()
{
    DHLOGI("ConsumeSurface enter.");
    if (senderAdapter_ == nullptr) {
        DHLOGE("av transport sender adapter is null.");
        return;
    }
    if (consumerSurface_ == nullptr) {
        DHLOGE("consumerSurface_ is nullptr, cannot consume surface buffer.");
        return;
    }
    sptr<SurfaceBuffer> surfaceBuffer = nullptr;
    syncFence_ = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    OHOS::Rect damage = {0, 0, 0, 0};
    SurfaceError surfaceErr = consumerSurface_->AcquireBuffer(surfaceBuffer, syncFence_, timestamp, damage);
    if (surfaceErr != SURFACE_ERROR_OK) {
        DHLOGE("consumerSurface_ acquire buffer failed, errcode: %{public}d", surfaceErr);
        consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
        return;
    }
    int32_t retcode = syncFence_->Wait(SURFACE_SYNC_FENCE_TIMEOUT);
    if (retcode == -ETIME) {
        DHLOGE("Sync fence wait timeout, retcode is %{public}" PRId32, retcode);
        consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
        return;
    }
    uint32_t surBufSize = surfaceBuffer->GetSize();
    auto surBufAddr = static_cast<uint8_t*>(surfaceBuffer->GetVirAddr());
    uint32_t videoWidth = videoParam_->GetVideoWidth();
    uint32_t videoHeight = videoParam_->GetVideoHeight();
    VideoData data = { surBufAddr, surBufSize, videoWidth, videoHeight, timestamp, VIDEO_FORMAT_RGBA8888 };
#ifdef DUMP_DSCREEN_FILE
    if (DscreenHidumper::GetInstance().GetFlagStatus() == true) {
        DHLOGE("HidumperFlag_ = true, exec SaveFile");
        DscreenHidumper::GetInstance().SaveFile("Screen_BeforeCoding_width(", data);
    }
#endif
    int32_t ret = senderAdapter_->PushData(data);
    if (ret != DH_SUCCESS) {
        DHLOGE("feed buffer to av transport sender failed.");
    }
    consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
    DHLOGI("ConsumeSurface success. timestamp=%{public}" PRId64, timestamp);
}

int32_t DScreen::InitSenderEngine(IAVEngineProvider *providerPtr, const std::string &peerDevId)
{
    DHLOGI("InitSenderEngine enter.");
    if (senderAdapter_ == nullptr) {
        senderAdapter_ = std::make_shared<AVTransSenderAdapter>();
    }
    int32_t ret = senderAdapter_->Initialize(providerPtr, peerDevId);
    if (ret != DH_SUCCESS) {
        DHLOGE("initialize av sender adapter failed.");
        return ERR_DH_AV_TRANS_INIT_FAILED;
    }
    return senderAdapter_->RegisterAdapterCallback(shared_from_this());
}

int32_t DScreen::StartSenderEngine()
{
    DHLOGI("StartSenderEngine, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    if (senderAdapter_ == nullptr) {
        DHLOGE("av transport sender adapter is null.");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    int32_t ret = senderAdapter_->CreateControlChannel(devId_);
    if (ret != DH_SUCCESS) {
        DHLOGE("create av sender control channel failed.");
        return ERR_DH_AV_TRANS_CREATE_CHANNEL_FAILED;
    }
    ret = SetUp();
    if (ret != DH_SUCCESS) {
        DHLOGE("set up av sender engine failed.");
        return ERR_DH_AV_TRANS_SETUP_FAILED;
    }
    ret = senderAdapter_->Start();
    if (ret != DH_SUCCESS) {
        DHLOGE("start av sender engine failed.");
        return ERR_DH_AV_TRANS_START_FAILED;
    }
    return DH_SUCCESS;
}

int32_t DScreen::StopSenderEngine()
{
    DHLOGI("StopSenderEngine, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    if (senderAdapter_ == nullptr) {
        DHLOGE("av transport sender adapter is null.");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }

    json paramJson;
    paramJson[KEY_DEV_ID] = devId_;
    paramJson[KEY_DH_ID] = dhId_;

    auto avMessage = std::make_shared<AVTransMessage>(DScreenMsgType::STOP_MIRROR, paramJson.dump(), devId_);
    senderAdapter_->SendMessageToRemote(avMessage);

    int32_t ret = senderAdapter_->Stop();
    if (ret != DH_SUCCESS) {
        DHLOGE("stop av sender adapter failed.");
        return ERR_DH_AV_TRANS_STOP_FAILED;
    }
    ret = senderAdapter_->Release();
    if (ret != DH_SUCCESS) {
        DHLOGE("release av sender adapter failed.");
        return ERR_DH_AV_TRANS_STOP_FAILED;
    }
    return DH_SUCCESS;
}

void DScreen::ChooseParameter(std::string &codecType, std::string &pixelFormat)
{
    if (videoParam_->GetCodecType() == VIDEO_CODEC_TYPE_VIDEO_H265) {
        codecType = MIME_VIDEO_H265;
    } else if (videoParam_->GetCodecType() == VIDEO_CODEC_TYPE_VIDEO_H264) {
        codecType = MIME_VIDEO_H264;
    } else {
        codecType = MIME_VIDEO_RAW;
    }
    if (videoParam_->GetVideoFormat() == VIDEO_DATA_FORMAT_YUVI420) {
        pixelFormat = VIDEO_FORMAT_YUVI420;
    } else if (videoParam_->GetVideoFormat() == VIDEO_DATA_FORMAT_NV12) {
        pixelFormat = VIDEO_FORMAT_NV12;
    } else if (videoParam_->GetVideoFormat() == VIDEO_DATA_FORMAT_NV21) {
        pixelFormat = VIDEO_FORMAT_NV21;
    } else {
        pixelFormat = VIDEO_FORMAT_RGBA8888;
    }
    senderAdapter_->SetParameter(AVTransTag::VIDEO_CODEC_TYPE, codecType);
    senderAdapter_->SetParameter(AVTransTag::VIDEO_PIXEL_FORMAT, pixelFormat);
    senderAdapter_->SetParameter(AVTransTag::VIDEO_WIDTH, std::to_string(videoParam_->GetVideoWidth()));
    senderAdapter_->SetParameter(AVTransTag::VIDEO_HEIGHT, std::to_string(videoParam_->GetVideoHeight()));
    senderAdapter_->SetParameter(AVTransTag::VIDEO_FRAME_RATE, std::to_string(videoParam_->GetFps()));
    senderAdapter_->SetParameter(AVTransTag::VIDEO_BIT_RATE, std::to_string(BIT_RATE));
}

int32_t DScreen::SetUp()
{
    DHLOGI("SetUp, devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(), GetAnonyString(dhId_).c_str());
    if (senderAdapter_ == nullptr) {
        DHLOGE("av transport sender adapter is null.");
        return ERR_DH_AV_TRANS_NULL_VALUE;
    }
    if (videoParam_ == nullptr) {
        DHLOGE("videoParam is nullptr.");
        return ERR_DH_SCREEN_SA_VALUE_NOT_INIT;
    }
    auto mapRelation = ScreenMgrAdapter::GetInstance().GetMapRelation(screenId_);
    if (mapRelation == nullptr) {
        DHLOGE("get map relation failed.");
        return ERR_DH_AV_TRANS_SETUP_FAILED;
    }
    DisplayRect displayRect = mapRelation->GetDisplayRect();
    videoParam_->SetVideoWidth(displayRect.width);
    videoParam_->SetVideoHeight(displayRect.height);

    json paramJson;
    paramJson[KEY_DH_ID] = dhId_;
    paramJson[KEY_SCREEN_ID] = screenId_;
    paramJson[KEY_VIDEO_PARAM] = *videoParam_;
    paramJson[KEY_MAPRELATION] = *mapRelation;

    auto avMessage = std::make_shared<AVTransMessage>(DScreenMsgType::START_MIRROR, paramJson.dump(), devId_);
    int32_t ret = senderAdapter_->SendMessageToRemote(avMessage);
    if (ret != DH_SUCCESS) {
        DHLOGE("send message to remote engine failed.");
        return ret;
    }

    ret = WaitForSinkStarted();
    if (ret != DH_SUCCESS) {
        DHLOGE("send message to start remote device engine failed.");
        return ret;
    }

    std::string codecType;
    std::string pixelFormat;
    ChooseParameter(codecType, pixelFormat);
    return senderAdapter_->SetParameter(AVTransTag::ENGINE_READY, OWNER_NAME_D_SCREEN);
}

int32_t DScreen::WaitForSinkStarted()
{
    std::unique_lock<std::mutex> lock(waitSinkMtx_);
    auto status = waitSinkCondVar_.wait_for(lock, std::chrono::milliseconds(WAIT_TIMEOUT_MS),
        [this]() { return sinkStartSuccess_.load(); });
    if (!status) {
        DHLOGE("wait for sink device engine start timeout");
        return ERR_DH_AV_TRANS_TIMEOUT;
    }
    if (!sinkStartSuccess_.load()) {
        DHLOGE("start sink device engine failed");
        return ERR_DH_AV_TRANS_SINK_START_FAILED;
    }
    return DH_SUCCESS;
}

int32_t DScreen::NegotiateCodecType(const std::string &rmtDecoderStr)
{
    DHLOGI("Start NegotiateCodecType, remote decoder: %{public}s", rmtDecoderStr.c_str());
    cJSON *rmtDecoderJson = cJSON_Parse(rmtDecoderStr.c_str());
    if (rmtDecoderJson == nullptr) {
        DHLOGE("remote Decoder Json is invalid.");
        return ERR_DH_SCREEN_SA_DSCREEN_NEGOTIATE_CODEC_FAIL;
    }

    std::vector<VideoDecoder> rmtVideoDecoders;
    FromJson<VideoDecoder>(VIDEO_DECODERS, rmtDecoderJson, rmtVideoDecoders);
    cJSON_Delete(rmtDecoderJson);

    std::shared_ptr<DistributedHardwareFwkKit> dhFwkKit = DScreenFwkKit::GetInstance().GetDHFwkKit();
    if (dhFwkKit == nullptr) {
        DHLOGE("Get DhFwkKit return null");
        return ERR_DH_SCREEN_SA_DSCREEN_NEGOTIATE_CODEC_FAIL;
    }
    std::string localVideoEncodersJsonStr =
        dhFwkKit->QueryLocalSysSpec(QueryLocalSysSpecType::HISTREAMER_VIDEO_ENCODER);
    if (localVideoEncodersJsonStr.empty()) {
        DHLOGE("Query local Codec info failed");
        return ERR_DH_SCREEN_SA_DSCREEN_NEGOTIATE_CODEC_FAIL;
    }
    DHLOGI("DScreen Negotiate QueryVideoEncoderAbility info: %{public}s", localVideoEncodersJsonStr.c_str());

    cJSON *localVideoEncodersJson = cJSON_Parse(localVideoEncodersJsonStr.c_str());
    if (localVideoEncodersJson == nullptr) {
        DHLOGE("localVideoEncodersJson is invalid.");
        return ERR_DH_SCREEN_SA_DSCREEN_NEGOTIATE_CODEC_FAIL;
    }

    std::vector<VideoEncoder> localVideoEncoders;
    FromJson<VideoEncoder>(VIDEO_ENCODERS, localVideoEncodersJson, localVideoEncoders);
    cJSON_Delete(localVideoEncodersJson);

    return ChooseCodecType(localVideoEncoders, rmtVideoDecoders);
}

int32_t DScreen::ChooseCodecType(const std::vector<VideoEncoder> &localVideoEncoders,
    const std::vector<VideoDecoder> &rmtVideoDecoders)
{
    std::vector<std::string> codecTypeCandidates;
    for (const auto &rmtDec : rmtVideoDecoders) {
        for (const auto &locEnc : localVideoEncoders) {
            std::pair<std::string, std::string> comb = {locEnc.name, rmtDec.name};
            if (CODECS_MAP.find(comb) != CODECS_MAP.end()) {
                std::string codec = CODECS_MAP.at(comb);
                DHLOGI("Find match comb, local encoder: %{public}s, remote decoder: %{public}s, codec: %{public}s",
                    locEnc.name.c_str(), rmtDec.name.c_str(), codec.c_str());
                codecTypeCandidates.push_back(codec);
            }
        }
    }
    if (std::find(codecTypeCandidates.begin(), codecTypeCandidates.end(),
        CODEC_NAME_H265) != codecTypeCandidates.end()) {
        videoParam_->SetCodecType(VIDEO_CODEC_TYPE_VIDEO_H265);
        videoParam_->SetVideoFormat(VIDEO_DATA_FORMAT_NV12);
    } else if (std::find(codecTypeCandidates.begin(), codecTypeCandidates.end(),
        CODEC_NAME_H264) != codecTypeCandidates.end()) {
        videoParam_->SetCodecType(VIDEO_CODEC_TYPE_VIDEO_H264);
        videoParam_->SetVideoFormat(VIDEO_DATA_FORMAT_NV12);
    } else {
        DHLOGI("codec type not support.");
        return ERR_DH_SCREEN_SA_DSCREEN_NEGOTIATE_CODEC_FAIL;
    }
    return DH_SUCCESS;
}

void DScreen::TaskThreadLoop()
{
    DHLOGI("DScreen taskThread start. devId: %{public}s, dhId: %{public}s", GetAnonyString(devId_).c_str(),
        GetAnonyString(dhId_).c_str());
    while (taskThreadRunning_) {
        watchdogFlag_ = true;
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
        DHLOGD("run task, task queue size: %{public}zu", taskQueue_.size());
        HandleTask(task);
    }
}

bool DScreen::CheckJsonData(const json &attrJson)
{
    if (attrJson.is_discarded()) {
        DHLOGE("enable param json is invalid.");
        return false;
    }
    if (!IsUInt32(attrJson, KEY_SCREEN_WIDTH) || !IsUInt32(attrJson, KEY_SCREEN_HEIGHT) ||
        !attrJson.contains(KEY_CODECTYPE)) {
        DHLOGE("enable param is invalid.");
        return false;
    }
    return true;
}

void DScreen::OnEngineEvent(DScreenEventType event, const std::string &content)
{
    (void)content;
    if (event == DScreenEventType::ENGINE_ERROR) {
        StopSenderEngine();
    } else if (event == DScreenEventType::TRANS_CHANNEL_CLOSED) {
        HandleDisconnect();
    }
}

void DScreen::OnEngineMessage(const std::shared_ptr<AVTransMessage> &message)
{
    if (message == nullptr) {
        DHLOGE("received engine message is null.");
        return;
    }
    DHLOGI("On sink device engine message received, message type =%{public}d.", message->type_);
    if ((message->type_ == DScreenMsgType::START_MIRROR_SUCCESS) ||
        (message->type_ == DScreenMsgType::START_MIRROR_FAIL)) {
        sinkStartSuccess_ = (message->type_ == DScreenMsgType::START_MIRROR_SUCCESS);
        waitSinkCondVar_.notify_one();
    }
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
} // namespace V2_0
} // namespace DistributedHardware
} // namespace OHOS