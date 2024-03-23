/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "image_source_encoder.h"

#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <securec.h>
#include <csetjmp>
#include <cstddef>
#include <string>
#include <ctime>

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_log.h"
#include "jpeglib.h"
#ifdef __LP64__
static const std::string LIB_LOAD_PATH = "/system/lib64/libdistributed_screen_dbg_itf.z.so";
#else
static const std::string LIB_LOAD_PATH = "/system/lib/libdistributed_screen_dbg_itf.z.so";
#endif
using GetDscreenDBGItfFunc = OHOS::DistributedHardware::IDScreenDBGItf* (*)();
using GetImageDirtyFunc = OHOS::DistributedHardware::IImageSetDirty* (*)();
namespace OHOS {
namespace DistributedHardware {
void ConsumerBufferListener::OnBufferAvailable()
{
    DHLOGI("%{public}s: OnBufferAvailable, receiv data from RS.", DSCREEN_LOG_TAG);
    encoder_->ConsumeSurface();
}

void ImageSourceEncoder::InitDscreenDBG()
{
    char path[PATH_MAX + 1] = {0x00};
    if (LIB_LOAD_PATH.length() > PATH_MAX || realpath(LIB_LOAD_PATH.c_str(), path) == nullptr) {
        DHLOGE("File connicailization failed.");
        return;
    }
    pHandler_ = dlopen(path, RTLD_LAZY | RTLD_NODELETE | RTLD_GLOBAL);
    if (pHandler_ == nullptr) {
        DHLOGE("%{public}s: handler load failed, fail reason: %{public}s.", path, dlerror());
        return;
    }
    GetDscreenDBGItfFunc getDscreenDBGItfFunc = (GetDscreenDBGItfFunc)dlsym(pHandler_, GET_DBG_ITF_FUNC.c_str());
    GetImageDirtyFunc getImageDirtyFunc = (GetImageDirtyFunc)dlsym(pHandler_, GET_IMAGE_DIRTY_FUNC.c_str());
    if (getDscreenDBGItfFunc == nullptr || getImageDirtyFunc ==nullptr) {
        DHLOGE("get FUNC failed, failed reason: %{public}s.", dlerror());
        return;
    }
    dscreenDbgItfPtr_ = getDscreenDBGItfFunc();
    imageSetDirtyPtr_ = getImageDirtyFunc();
    if (dscreenDbgItfPtr_ == nullptr || imageSetDirtyPtr_ == nullptr) {
        DHLOGE("Get interface failed.");
        return;
    }
    DHLOGI("InitDscreenDBG success.");
    dscreenDbgItfPtr_->Init();
}

void ImageSourceEncoder::ConsumeSurface()
{
    DHLOGI("%{public}s: ConsumeSurface.", DSCREEN_LOG_TAG);
    std::unique_lock<std::mutex> bufLock(bufferMtx_);
    if (consumerSurface_ == nullptr) {
        DHLOGE("%{public}s: consumerSurface_ is nullptr.", DSCREEN_LOG_TAG);
        return;
    }
    sptr<SurfaceBuffer> surfaceBuffer = nullptr;
    syncFence_ = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    OHOS::Rect damage = {0, 0, 0, 0};
    SurfaceError surfaceErr = consumerSurface_->AcquireBuffer(surfaceBuffer, syncFence_, timestamp, damage);
    if (surfaceErr != SURFACE_ERROR_OK) {
        DHLOGE("%{public}s: consumerSurface_ acquire buffer failed, errcode: %{public}" PRId32, DSCREEN_LOG_TAG,
            surfaceErr);
        consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
        return;
    }
    int32_t retcode = syncFence_->Wait(SURFACE_SYNC_FENCE_TIMEOUT);
    if (retcode == -ETIME) {
        DHLOGE("%{public}s: Sync fence wait timeout, retcode is %{public}" PRId32, DSCREEN_LOG_TAG, retcode);
        return;
    }
    if (pHandler_ != nullptr) {
        eventContent_.clear();
        eventContent_ = imageSetDirtyPtr_->GetDamage();
    }
    std::vector<OHOS::Rect> damages = VecToDamage(eventContent_);
    std::shared_ptr<IImageSourceProcessorListener> listener = imageProcessorListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%{public}s: Processor listener is null", DSCREEN_LOG_TAG);
        consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
        return;
    }
    listener->OnDamageProcessDone(surfaceBuffer, damages);
    consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
}

int32_t ImageSourceEncoder::ConfigureEncoder(const VideoParam &configParam)
{
    DHLOGI("%{public}s: ConfigureEncoder.", DSCREEN_LOG_TAG);
    int32_t ret = InitVideoEncoder(configParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Init encoder failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }

    ret = SetEncoderFormat(configParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Set encoder format failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }

    encoderSurface_ = videoEncoder_->CreateInputSurface();
    if (encoderSurface_ == nullptr) {
        DHLOGE("%{public}s: Create encoder surface failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }
    configParam_ = configParam;
    ret = AddSurface();
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: Add surface failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        consumerSurface_ = nullptr;
        producerSurface_ = nullptr;
        return ret;
    }
    return DH_SUCCESS;
}

int32_t ImageSourceEncoder::AddSurface()
{
    DHLOGI("%{public}s: AddSurface.", DSCREEN_LOG_TAG);
    consumerSurface_ = IConsumerSurface::Create();
    if (consumerSurface_ == nullptr) {
        DHLOGE("%{public}s: creat consumer surface failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }
    sptr<IBufferProducer> producer = consumerSurface_->GetProducer();
    if (producer == nullptr) {
        DHLOGE("%{public}s: Creat producer surface failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }
    producerSurface_ = Surface::CreateSurfaceAsProducer(producer);
    if (producerSurface_ == nullptr) {
        DHLOGE("%{public}s: Create preducer surface failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }
    consumerBufferListener_ = new ConsumerBufferListener(shared_from_this());
    consumerSurface_->RegisterConsumerListener(consumerBufferListener_);
    return DH_SUCCESS;
}

sptr<Surface> ImageSourceEncoder::GetConsumerSurface()
{
    DHLOGI("%{public}s: GetConsumerSurface.", DSCREEN_LOG_TAG);
    return consumerSurface_;
}

sptr<Surface> &ImageSourceEncoder::GetInputSurface()
{
    DHLOGI("%{public}s: GetInputSurface.", DSCREEN_LOG_TAG);
    if (producerSurface_ == nullptr || !configParam_.GetPartialRefreshFlag()) {
        return encoderSurface_;
    }
    return producerSurface_;
}

std::vector<OHOS::Rect> ImageSourceEncoder::VecToDamage(std::vector<std::vector<int32_t>> eventContent)
{
    DHLOGI("%{public}s: VecToDamage.", DSCREEN_LOG_TAG);
    std::vector<OHOS::Rect> damages;
    for (auto item : eventContent) {
        OHOS::Rect damage = {0, 0, 0, 0};
        damage.x = item[0];
        damage.y = item[1];
        damage.w = item[2];
        damage.h = item[3];
        damages.push_back(damage);
    }
    return damages;
}

sptr<SurfaceBuffer> ImageSourceEncoder::GetEncoderInputSurfaceBuffer()
{
    DHLOGI("%{public}s: GetEncoderInputSurfaceBuffer.", DSCREEN_LOG_TAG);
    OHOS::BufferRequestConfig requestConfig;
    requestConfig.width = static_cast<int32_t>(configParam_.GetVideoWidth());
    requestConfig.height = static_cast<int32_t>(configParam_.GetVideoHeight());
    requestConfig.usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA;
    requestConfig.strideAlignment = STRIDE_ALIGNMENT;
    requestConfig.format = GraphicPixelFormat::GRAPHIC_PIXEL_FMT_RGBA_8888;
    requestConfig.timeout = 0;
    sptr<OHOS::SurfaceBuffer> encoderSurfaceBuffer = nullptr;
    int32_t releaseFence = -1;
    SurfaceError surfaceErr = encoderSurface_->RequestBuffer(encoderSurfaceBuffer, releaseFence, requestConfig);
    if (surfaceErr != GSERROR_OK || encoderSurfaceBuffer == nullptr) {
        DHLOGE("%{public}s: RequestBuffer failed.", DSCREEN_LOG_TAG);
        encoderSurface_->CancelBuffer(encoderSurfaceBuffer);
    }
    return encoderSurfaceBuffer;
}

int32_t ImageSourceEncoder::FeedEncoderData(sptr<SurfaceBuffer> &surfaceBuffer)
{
    sptr<OHOS::SurfaceBuffer> encoderSurfaceBuffer = GetEncoderInputSurfaceBuffer();
    if (encoderSurfaceBuffer == nullptr) {
        DHLOGE("Get encoder input producer surface buffer failed.");
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }
    uint32_t screenDataSize = configParam_.GetVideoWidth() * configParam_.GetVideoHeight() * RGBA_CHROMA;
    auto encoderSurfaceAddr = static_cast<uint8_t*>(encoderSurfaceBuffer->GetVirAddr());
    auto surfaceAddr = static_cast<uint8_t*>(surfaceBuffer->GetVirAddr());
    int32_t ret = memcpy_s(encoderSurfaceAddr, screenDataSize, surfaceAddr, screenDataSize);
    if (ret != EOK) {
        DHLOGE("%{public}s: surfaceBuffer memcpy_s run failed.", DSCREEN_LOG_TAG);
        consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
        encoderSurface_->CancelBuffer(encoderSurfaceBuffer);
        return ret;
    }
    BufferFlushConfig flushConfig = { {0, 0, encoderSurfaceBuffer->GetWidth(), encoderSurfaceBuffer->GetHeight()}, 0};
    DHLOGI("%{public}s: FeedEncoderData to encoder.", DSCREEN_LOG_TAG);
    SurfaceError surfaceErr = encoderSurface_->FlushBuffer(encoderSurfaceBuffer, -1, flushConfig);
    if (surfaceErr != SURFACE_ERROR_OK) {
        DHLOGE("%{public}s: encoderSurface_ flush buffer failed.", DSCREEN_LOG_TAG);
        consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
        encoderSurface_->CancelBuffer(encoderSurfaceBuffer);
        return surfaceErr;
    }
    consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
    return DH_SUCCESS;
}

int32_t ImageSourceEncoder::ReleaseEncoder()
{
    DHLOGI("%{public}s: ReleaseEncoder.", DSCREEN_LOG_TAG);
    if (videoEncoder_ == nullptr) {
        DHLOGE("%{public}s: Encoder is null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = videoEncoder_->Release();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: Release encoder failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_RELEASE_FAILED;
    }
    if (pHandler_ != nullptr) {
        dscreenDbgItfPtr_->Release();
        dlclose(pHandler_);
    }
    encodeVideoCallback_ = nullptr;
    videoEncoder_ = nullptr;
    consumerBufferListener_ = nullptr;
    return DH_SUCCESS;
}

int32_t ImageSourceEncoder::StartEncoder()
{
    DHLOGI("%{public}s: StartEncoder.", DSCREEN_LOG_TAG);
    if (videoEncoder_ == nullptr) {
        DHLOGE("%{public}s: Encoder is null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = videoEncoder_->Prepare();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: Prepare encoder failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_PREPARE_FAILED;
    }

    ret = videoEncoder_->Start();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: Start encoder failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_START_FAILED;
    }
    InitDscreenDBG();
    return DH_SUCCESS;
}

int32_t ImageSourceEncoder::StopEncoder()
{
    DHLOGI("%{public}s: StopEncoder.", DSCREEN_LOG_TAG);
    if (videoEncoder_ == nullptr) {
        DHLOGE("%{public}s: Encoder is null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = videoEncoder_->Flush();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: Flush encoder failed.", DSCREEN_LOG_TAG);
    }

    ret = videoEncoder_->Stop();
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: Stop encoder failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_STOP_FAILED;
    }
    ret = consumerSurface_->UnregisterConsumerListener();
    if (ret != SURFACE_ERROR_OK) {
        DHLOGE("Unregister Consumer Listener failed.");
    }
    consumerSurface_ = nullptr;
    producerSurface_ = nullptr;
    return DH_SUCCESS;
}

int32_t ImageSourceEncoder::InitVideoEncoder(const VideoParam &configParam)
{
    DHLOGI("%{public}s: InitVideoEncoder.", DSCREEN_LOG_TAG);
    switch (configParam.GetCodecType()) {
        case VIDEO_CODEC_TYPE_VIDEO_H264:
            videoEncoder_ = MediaAVCodec::VideoEncoderFactory::CreateByMime(
                std::string(MediaAVCodec::CodecMimeType::VIDEO_AVC));
            break;
        case VIDEO_CODEC_TYPE_VIDEO_H265:
            videoEncoder_ = MediaAVCodec::VideoEncoderFactory::CreateByMime(
                std::string(MediaAVCodec::CodecMimeType::VIDEO_HEVC));
            break;
        default:
            DHLOGE("%{public}s: codecType is invalid!", DSCREEN_LOG_TAG);
            videoEncoder_ = nullptr;
    }

    if (videoEncoder_ == nullptr) {
        DHLOGE("%{public}s: Create videoEncoder failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_CREATE_CODEC_FAILED;
    }

    encodeVideoCallback_ = std::make_shared<ImageEncoderCallback>(shared_from_this());
    int32_t ret = videoEncoder_->SetCallback(encodeVideoCallback_);
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: Set codec callback failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SET_CALLBACK_FAILED;
    }

    return DH_SUCCESS;
}

int32_t ImageSourceEncoder::SetEncoderFormat(const VideoParam &configParam)
{
    DHLOGI("%{public}s: SetEncoderFormat.", DSCREEN_LOG_TAG);
    if (videoEncoder_ == nullptr) {
        DHLOGE("%{public}s: Encoder is null.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    switch (configParam.GetCodecType()) {
        case VIDEO_CODEC_TYPE_VIDEO_H264:
            imageFormat_.PutStringValue("codec_mime", MediaAVCodec::CodecMimeType::VIDEO_AVC);
            break;
        case VIDEO_CODEC_TYPE_VIDEO_H265:
            imageFormat_.PutStringValue("codec_mime", MediaAVCodec::CodecMimeType::VIDEO_HEVC);
            break;
        default:
            DHLOGE("%{public}s: Codec type is invalid.", DSCREEN_LOG_TAG);
            return ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM;
    }

    imageFormat_.PutIntValue("pixel_format", static_cast<int32_t>(MediaAVCodec::VideoPixelFormat::RGBA));
    imageFormat_.PutLongValue("max_input_size", MAX_YUV420_BUFFER_SIZE);
    imageFormat_.PutIntValue("width", configParam.GetVideoWidth());
    imageFormat_.PutIntValue("height", configParam.GetVideoHeight());
    imageFormat_.PutDoubleValue("frame_rate", configParam.GetFps());

    int32_t ret = videoEncoder_->Configure(imageFormat_);
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: Configure encoder failed.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_CODEC_CONFIGURE_FAILED;
    }
    return DH_SUCCESS;
}

void ImageSourceEncoder::OnError(MediaAVCodec::AVCodecErrorType errorType, int32_t errorCode)
{
    DHLOGI("%{public}s: Encoder error, errorType:%{public}" PRId32 ", errorCode:%{public}" PRId32, DSCREEN_LOG_TAG,
        errorType, errorCode);
    std::shared_ptr<IImageSourceProcessorListener> listener = imageProcessorListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%{public}s: Processor listener is null", DSCREEN_LOG_TAG);
        return;
    }
    listener->OnProcessorStateNotify(errorCode);
}

void ImageSourceEncoder::OnOutputBufferAvailable(uint32_t index, MediaAVCodec::AVCodecBufferInfo info,
    MediaAVCodec::AVCodecBufferFlag flag, std::shared_ptr<Media::AVSharedMemory> buffer)
{
    DHLOGD("%{public}s: OnOutputBufferAvailable, receiv H264 data from encoder.", DSCREEN_LOG_TAG);
    std::shared_ptr<IImageSourceProcessorListener> listener = imageProcessorListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%{public}s: Processor listener is null", DSCREEN_LOG_TAG);
        return;
    }
    if (videoEncoder_ == nullptr) {
        DHLOGE("%{public}s: Encoder is null.", DSCREEN_LOG_TAG);
        return;
    }

    encoderBufferInfo_ = info;
    if (buffer == nullptr) {
        DHLOGE("%{public}s: Buffer is null, index = %{public}d", DSCREEN_LOG_TAG, index);
        return;
    }

    size_t dataSize = static_cast<size_t>(info.size);
    if (dataSize == 0 || dataSize > DATA_BUFFER_MAX_SIZE) {
        DHLOGE("%{public}s:OnOutputBufferAvailable params invalid, size: %{public}zu", DSCREEN_LOG_TAG, dataSize);
        return;
    }
    auto dataBuf = std::make_shared<DataBuffer>(dataSize);
    int32_t ret = memcpy_s(dataBuf->Data(), dataBuf->Capacity(), buffer->GetBase(), dataSize);
    if (ret != EOK) {
        DHLOGE("%{public}s: Copy data failed.", DSCREEN_LOG_TAG);
        return;
    }
    dataBuf->SetDataType(VIDEO_FULL_SCREEN_DATA);
    dataBuf->SetSize(dataSize);
    listener->OnImageProcessDone(dataBuf);
    ret = videoEncoder_->ReleaseOutputBuffer(index);
    if (ret != MediaAVCodec::AVCS_ERR_OK) {
        DHLOGE("%{public}s: videoEncoder ReleaseOutputBuffer failed.", DSCREEN_LOG_TAG);
    }
}

void ImageSourceEncoder::OnInputBufferAvailable(uint32_t index, std::shared_ptr<Media::AVSharedMemory> buffer)
{
    (void) index;
}

void ImageSourceEncoder::OnOutputFormatChanged(const Media::Format &format)
{
    (void) format;
}
} // namespace DistributedHardware
} // namespace OHOS