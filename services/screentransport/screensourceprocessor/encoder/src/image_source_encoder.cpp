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
#include <stddef.h>
#include <string>
#include <ctime>

#include "display_type.h"
#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_log.h"
#include "jpeglib.h"
#ifdef __LP64__
const std::string LIB_LOAD_PATH = "/system/lib64/libdistributed_screen_dbg_itf.z.so";
#else
const std::string LIB_LOAD_PATH = "/system/lib/libdistributed_screen_dbg_itf.z.so";
#endif
using GetDscreenDBGItfFunc = OHOS::DistributedHardware::IDScreenDBGItf* (*)();
using GetImageDirtyFunc = OHOS::DistributedHardware::IImageSetDirty* (*)();
namespace OHOS {
namespace DistributedHardware {
void ConsumerBufferListener::OnBufferAvailable()
{
    DHLOGI("%s: OnBufferAvailable.", LOG_TAG);
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
        DHLOGE("%s: handler load failed, fail reason: %s.", path, dlerror());
        return;
    }
    GetDscreenDBGItfFunc getDscreenDBGItfFunc = (GetDscreenDBGItfFunc)dlsym(pHandler_, GET_DBG_ITF_FUNC.c_str());
    GetImageDirtyFunc getImageDirtyFunc = (GetImageDirtyFunc)dlsym(pHandler_, GET_IMAGE_DIRTY_FUNC.c_str());
    if (getDscreenDBGItfFunc == nullptr || getImageDirtyFunc ==nullptr) {
        DHLOGE("get FUNC failed, failed reason: %s.", dlerror());
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
    DHLOGI("%s: ConsumeSurface.", LOG_TAG);
    std::unique_lock<std::mutex> bufLock(bufferMtx_);
    if (consumerSurface_ == nullptr) {
        DHLOGE("%s: consumerSurface_ is nullptr.", LOG_TAG);
        return;
    }
    sptr<SurfaceBuffer> surfaceBuffer = nullptr;
    int32_t fence = -1;
    int64_t timestamp = 0;
    OHOS::Rect damage = {0, 0, 0, 0}; 
    SurfaceError surfaceErr = consumerSurface_->AcquireBuffer(surfaceBuffer, fence, timestamp, damage);
    if (surfaceErr != SURFACE_ERROR_OK) {
        DHLOGE("%s: consumerSurface_ acquire buffer failed, errcode: %" PRId32, LOG_TAG, surfaceErr);
        consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
        return;
    }
    if (pHandler_ != nullptr) {
        eventContent_.clear();
        eventContent_ = imageSetDirtyPtr_->GetDamage();
    }
    std::vector<OHOS::Rect> damages = VecToDamage(eventContent_);
    std::shared_ptr<IImageSourceProcessorListener> listener = imageProcessorListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%s: Processor listener is null", LOG_TAG);
        consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
        return;
    }
    listener->OnDamageProcessDone(surfaceBuffer, damages);
    consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
}

int32_t ImageSourceEncoder::ConfigureEncoder(const VideoParam &configParam)
{
    DHLOGI("%s: ConfigureEncoder.", LOG_TAG);
    int32_t ret = InitVideoEncoder(configParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Init encoder failed ret: %" PRId32, LOG_TAG, ret);
        return ret;
    }

    ret = SetEncoderFormat(configParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Set encoder format failed ret: %" PRId32, LOG_TAG, ret);
        return ret;
    }

    encoderSurface_ = videoEncoder_->CreateInputSurface();
    if (encoderSurface_ == nullptr) {
        DHLOGE("%s: Create encoder surface failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }
    configParam_ = configParam;
    ret = AddSurface();
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Add surface failed ret: %." PRId32, LOG_TAG, ret);
        consumerSurface_ = nullptr;
        producerSurface_ = nullptr;
        return ret;
    }
    return DH_SUCCESS;
}

int32_t ImageSourceEncoder::AddSurface()
{
    DHLOGI("%s: AddSurface.", LOG_TAG);
    consumerSurface_ = IConsumerSurface::Create();
    if (consumerSurface_ == nullptr) {
        DHLOGE("%s: creat consumer surface failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }
    sptr<IBufferProducer> producer = consumerSurface_->GetProducer();
    if (producer == nullptr) {
        DHLOGE("%s: Creat producer surface failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }
    producerSurface_ = Surface::CreateSurfaceAsProducer(producer);
    if (producerSurface_ == nullptr) {
        DHLOGE("%s: Create preducer surface failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }
    consumerBufferListener_ = new ConsumerBufferListener(shared_from_this());
    consumerSurface_->RegisterConsumerListener(consumerBufferListener_);
    return DH_SUCCESS;
}

sptr<Surface> ImageSourceEncoder::GetConsumerSurface()
{
    DHLOGI("%s: GetConsumerSurface.", LOG_TAG);
    return consumerSurface_;
}

sptr<Surface> &ImageSourceEncoder::GetInputSurface()
{
    DHLOGI("%s: GetInputSurface.", LOG_TAG);
    if (producerSurface_ == nullptr) {
        return encoderSurface_;
    }
    return producerSurface_;
}

std::vector<OHOS::Rect> ImageSourceEncoder::VecToDamage(std::vector<std::vector<int32_t>> eventContent)
{
    DHLOGI("%s: VecToDamage.", LOG_TAG);
    std::vector<OHOS::Rect> damages;
    for (auto item : eventContent) {
        OHOS::Rect damage = {0, 0, 0, 0};
        damage.x = item.at(ZERO);
        damage.y = item.at(ONE);
        damage.w = item.at(TWO);
        damage.h = item.at(THREE);
        damages.push_back(damage);
    }
    return damages;
}

sptr<SurfaceBuffer> ImageSourceEncoder::GetEncoderInputSurfaceBuffer()
{
    DHLOGI("%s: GetEncoderInputSurfaceBuffer.", LOG_TAG);
    OHOS::BufferRequestConfig requestConfig;
    requestConfig.width = configParam_.GetVideoWidth();
    requestConfig.height = configParam_.GetVideoHeight();
    requestConfig.usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA;
    requestConfig.strideAlignment = STRIDE_ALIGNMENT;
    requestConfig.format = PixelFormat::PIXEL_FMT_RGBA_8888;
    requestConfig.timeout = 0;
    sptr<OHOS::SurfaceBuffer> encoderSurfaceBuffer = nullptr;
    int32_t releaseFence = -1;
    SurfaceError surfaceErr = encoderSurface_->RequestBuffer(encoderSurfaceBuffer, releaseFence, requestConfig);
    if (surfaceErr != GSERROR_OK || encoderSurfaceBuffer == nullptr) {
        DHLOGE("%s: RequestBuffer failed.", LOG_TAG);
        encoderSurface_->CancelBuffer(encoderSurfaceBuffer);
    }
    return encoderSurfaceBuffer;
}

int32_t ImageSourceEncoder::FeedEncoderData(sptr<SurfaceBuffer> &surfaceBuffer)
{
    DHLOGI("%s: FeedEncoderData.", LOG_TAG);
    sptr<OHOS::SurfaceBuffer> encoderSurfaceBuffer = GetEncoderInputSurfaceBuffer();
    if (encoderSurfaceBuffer == nullptr) {
        DHLOGE("Get encoder input producer surface buffer failed.");
        return ERR_DH_SCREEN_CODEC_SURFACE_ERROR;
    }
    int32_t screenDataSize = configParam_.GetVideoWidth() * configParam_.GetVideoHeight() * RGBA_CHROMA;
    auto encoderSurfaceAddr = static_cast<uint8_t*>(encoderSurfaceBuffer->GetVirAddr());
    auto surfaceAddr = static_cast<uint8_t*>(surfaceBuffer->GetVirAddr());
    int32_t ret = memcpy_s(encoderSurfaceAddr, screenDataSize, surfaceAddr, screenDataSize);
    if (ret != EOK) {
        DHLOGE("%s: surfaceBuffer memcpy_s run failed.", LOG_TAG);
        consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
        encoderSurface_->CancelBuffer(encoderSurfaceBuffer);
        return ret;
    }
    BufferFlushConfig flushConfig = { {0, 0, encoderSurfaceBuffer->GetWidth(), encoderSurfaceBuffer->GetHeight()}, 0};
    SurfaceError surfaceErr = encoderSurface_->FlushBuffer(encoderSurfaceBuffer, -1, flushConfig);
    if (surfaceErr != SURFACE_ERROR_OK) {
        DHLOGE("%s: encoderSurface_ flush buffer failed.", LOG_TAG);
        consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
        encoderSurface_->CancelBuffer(encoderSurfaceBuffer);
        return surfaceErr;
    }
    consumerSurface_->ReleaseBuffer(surfaceBuffer, -1);
    return DH_SUCCESS;
}

int32_t ImageSourceEncoder::ReleaseEncoder()
{
    DHLOGI("%s: ReleaseEncoder.", LOG_TAG);
    if (videoEncoder_ == nullptr) {
        DHLOGE("%s: Encoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = videoEncoder_->Release();
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Release encoder failed.", LOG_TAG);
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
    DHLOGI("%s: StartEncoder.", LOG_TAG);
    if (videoEncoder_ == nullptr) {
        DHLOGE("%s: Encoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = videoEncoder_->Prepare();
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Prepare encoder failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_PREPARE_FAILED;
    }

    ret = videoEncoder_->Start();
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Start encoder failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_START_FAILED;
    }
    InitDscreenDBG();
    return DH_SUCCESS;
}

int32_t ImageSourceEncoder::StopEncoder()
{
    DHLOGI("%s: StopEncoder.", LOG_TAG);
    if (videoEncoder_ == nullptr) {
        DHLOGE("%s: Encoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = videoEncoder_->Flush();
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Flush encoder failed.", LOG_TAG);
    }

    ret = videoEncoder_->Stop();
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Stop encoder failed.", LOG_TAG);
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
    DHLOGI("%s: InitVideoEncoder.", LOG_TAG);
    switch (configParam.GetCodecType()) {
        case VIDEO_CODEC_TYPE_VIDEO_H264:
            videoEncoder_ = Media::VideoEncoderFactory::CreateByMime("video/avc");
            break;
        case VIDEO_CODEC_TYPE_VIDEO_H265:
            videoEncoder_ = Media::VideoEncoderFactory::CreateByMime("video/hevc");
            break;
        case VIDEO_CODEC_TYPE_VIDEO_MPEG4:
            videoEncoder_ = Media::VideoEncoderFactory::CreateByMime("video/mp4v-es");
            break;
        default:
            DHLOGE("%s: codecType is invalid!", LOG_TAG);
            videoEncoder_ = nullptr;
    }

    if (videoEncoder_ == nullptr) {
        DHLOGE("%s: Create videoEncoder failed.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_CREATE_CODEC_FAILED;
    }

    encodeVideoCallback_ = std::make_shared<ImageEncoderCallback>(shared_from_this());
    int32_t ret = videoEncoder_->SetCallback(encodeVideoCallback_);
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Set codec callback failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_SET_CALLBACK_FAILED;
    }

    return DH_SUCCESS;
}

int32_t ImageSourceEncoder::SetEncoderFormat(const VideoParam &configParam)
{
    DHLOGI("%s: SetEncoderFormat.", LOG_TAG);
    if (videoEncoder_ == nullptr) {
        DHLOGE("%s: Encoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    switch (configParam.GetCodecType()) {
        case VIDEO_CODEC_TYPE_VIDEO_H264:
            imageFormat_.PutStringValue("codec_mime", "video/avc");
            break;
        case VIDEO_CODEC_TYPE_VIDEO_H265:
            imageFormat_.PutStringValue("codec_mime", "video/hevc");
            break;
        case VIDEO_CODEC_TYPE_VIDEO_MPEG4:
            imageFormat_.PutStringValue("codec_mime", "video/mp4v-es");
            break;
        default:
            DHLOGE("%s: Codec type is invalid.", LOG_TAG);
            return ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM;
    }
    switch (configParam.GetVideoFormat()) {
        case VIDEO_DATA_FORMAT_YUVI420:
            imageFormat_.PutIntValue("pixel_format", Media::VideoPixelFormat::YUVI420);
            break;
        case VIDEO_DATA_FORMAT_NV12:
            imageFormat_.PutIntValue("pixel_format", Media::VideoPixelFormat::NV12);
            break;
        case VIDEO_DATA_FORMAT_NV21:
            imageFormat_.PutIntValue("pixel_format", Media::VideoPixelFormat::NV21);
            break;
        case VIDEO_DATA_FORMAT_RGBA8888:
            imageFormat_.PutIntValue("pixel_format", Media::VideoPixelFormat::RGBA);
            break;
        default:
            DHLOGE("%s: Video format is invalid.", LOG_TAG);
            return ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM;
    }
    imageFormat_.PutLongValue("max_input_size", MAX_YUV420_BUFFER_SIZE);
    imageFormat_.PutIntValue("width", configParam.GetVideoWidth());
    imageFormat_.PutIntValue("height", configParam.GetVideoHeight());
    imageFormat_.PutIntValue("frame_rate", configParam.GetFps());

    int32_t ret = videoEncoder_->Configure(imageFormat_);
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: Configure encoder failed.", LOG_TAG);
        return ERR_DH_SCREEN_CODEC_CONFIGURE_FAILED;
    }
    return DH_SUCCESS;
}

void ImageSourceEncoder::OnError(Media::AVCodecErrorType errorType, int32_t errorCode)
{
    DHLOGI("%s: Encoder error, errorType:%" PRId32", errorCode:%" PRId32, LOG_TAG, errorType, errorCode);
    std::shared_ptr<IImageSourceProcessorListener> listener = imageProcessorListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%s: Processor listener is null", LOG_TAG);
        return;
    }
    listener->OnProcessorStateNotify(errorCode);
}

void ImageSourceEncoder::OnOutputBufferAvailable(uint32_t index, Media::AVCodecBufferInfo info,
    Media::AVCodecBufferFlag flag)
{
    DHLOGD("%s: OnOutputBufferAvailable.", LOG_TAG);
    std::shared_ptr<IImageSourceProcessorListener> listener = imageProcessorListener_.lock();
    if (listener == nullptr) {
        DHLOGE("%s: Processor listener is null", LOG_TAG);
        return;
    }
    if (videoEncoder_ == nullptr) {
        DHLOGE("%s: Encoder is null.", LOG_TAG);
        return;
    }

    encoderBufferInfo_ = info;
    videoSharedMemory_ = videoEncoder_->GetOutputBuffer(index);
    if (videoSharedMemory_ == nullptr) {
        DHLOGE("%s: GetOutputBuffer failed.", LOG_TAG);
        return;
    }

    size_t dataSize = static_cast<size_t>(info.size);
    if (dataSize == 0 || dataSize > DATA_BUFFER_MAX_SIZE) {
        DHLOGE("%s:OnOutputBufferAvailable params invalid, size: %" PRId32, LOG_TAG, dataSize);
        return;
    }
    auto dataBuf = std::make_shared<DataBuffer>(dataSize);
    int32_t ret = memcpy_s(dataBuf->Data(), dataBuf->Capacity(), videoSharedMemory_->GetBase(), dataSize);
    if (ret != EOK) {
        DHLOGE("%s: Copy data failed.", LOG_TAG);
        return;
    }
    dataBuf->SetDataType(VIDEO_FULL_SCREEN_DATA);
    dataBuf->SetSize(dataSize);
    listener->OnImageProcessDone(dataBuf);
    ret = videoEncoder_->ReleaseOutputBuffer(index);
    if (ret != Media::MSERR_OK) {
        DHLOGE("%s: videoEncoder ReleaseOutputBuffer failed.", LOG_TAG);
    }
}

void ImageSourceEncoder::OnInputBufferAvailable(uint32_t index)
{
    (void) index;
}

void ImageSourceEncoder::OnOutputFormatChanged(const Media::Format &format)
{
    (void) format;
}
} // namespace DistributedHardware
} // namespace OHOS