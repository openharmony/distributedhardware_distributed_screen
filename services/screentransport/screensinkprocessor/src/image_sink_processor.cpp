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

#include "image_sink_processor.h"

#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_hitrace.h"
#include "dscreen_log.h"

namespace OHOS {
namespace DistributedHardware {
int32_t ImageSinkProcessor::ConfigureImageProcessor(
    const VideoParam &localParam, const VideoParam &remoteParam,
    const std::shared_ptr<IImageSinkProcessorListener> &imageListener)
{
    DHLOGI("%{public}s: ConfigureImageProcessor.", DSCREEN_LOG_TAG);
    localParam_ = localParam;
    remoteParam_ = remoteParam;

    imageDecoder_ = std::make_shared<ImageSinkDecoder>(imageListener);
    imageJpeg_ = std::make_shared<JpegImageProcessor>(remoteParam);
    int32_t ret = imageDecoder_->ConfigureDecoder(localParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: ConfigureDecoder failed ret:%{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ImageSinkProcessor::ReleaseImageProcessor()
{
    DHLOGI("%{public}s: ReleaseImageProcessor.", DSCREEN_LOG_TAG);
    if (imageDecoder_ == nullptr) {
        DHLOGE("%{public}s: Decoder is null.", DSCREEN_LOG_TAG);
        ReportOptFail(DSCREEN_OPT_FAIL, ERR_DH_SCREEN_TRANS_NULL_VALUE, "ReleaseImageProcessor Decoder is null.");
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_RELEASE_DECODER_START);
    int32_t ret = imageDecoder_->ReleaseDecoder();
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: ReleaseDecoder failed.", DSCREEN_LOG_TAG);
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "ReleaseDecoder failed.");
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ImageSinkProcessor::StartImageProcessor()
{
    DHLOGI("%{public}s: StartImageProcessor.", DSCREEN_LOG_TAG);
    if (imageDecoder_ == nullptr) {
        DHLOGE("%{public}s: Decoder is null.", DSCREEN_LOG_TAG);
        ReportOptFail(DSCREEN_OPT_FAIL, ERR_DH_SCREEN_TRANS_NULL_VALUE, "StartImageProcessor Decoder is null.");
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_START_DECODER_START);
    int32_t ret = imageDecoder_->StartDecoder();
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: StartDecoder failed ret:%{public}" PRId32, DSCREEN_LOG_TAG, ret);
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "StartDecoder failed.");
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ImageSinkProcessor::StopImageProcessor()
{
    DHLOGI("%{public}s: StopImageProcessor.", DSCREEN_LOG_TAG);
    if (imageDecoder_ == nullptr) {
        DHLOGE("%{public}s: Decoder is null.", DSCREEN_LOG_TAG);
        ReportOptFail(DSCREEN_OPT_FAIL, ERR_DH_SCREEN_TRANS_NULL_VALUE, "StopImageProcessor Decoder is null.");
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_STOP_DECODER_START);
    int32_t ret = imageDecoder_->StopDecoder();
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: StopDecoder failed ret:%{public}" PRId32, DSCREEN_LOG_TAG, ret);
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "StopDecoder failed.");
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ImageSinkProcessor::SetImageSurface(sptr<Surface> &surface)
{
    DHLOGI("%{public}s: SetImageSurface.", DSCREEN_LOG_TAG);
    if (imageDecoder_ == nullptr || surface == nullptr) {
        DHLOGE("%{public}s: Decoder or surface is null.", DSCREEN_LOG_TAG);
        ReportOptFail(DSCREEN_OPT_FAIL, ERR_DH_SCREEN_TRANS_NULL_VALUE, "SetImageSurface Decoder is null.");
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = imageDecoder_->SetOutputSurface(surface);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: SetOutputSurface failed ret:%{public}" PRId32, DSCREEN_LOG_TAG, ret);
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "SetOutputSurface failed.");
        return ret;
    }

    ret = imageJpeg_->SetOutputSurface(surface);
    if (ret != DH_SUCCESS) {
        DHLOGE("%{public}s: imageJpeg SetImageSurface failed ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "imageJpeg SetOutputSurface failed.");
        return ret;
    }
    return DH_SUCCESS;
}

int32_t ImageSinkProcessor::ProcessImage(const std::shared_ptr<DataBuffer> &data)
{
    DHLOGI("%{public}s: ProcessImage.", DSCREEN_LOG_TAG);
    if (imageDecoder_ == nullptr || data == nullptr) {
        DHLOGE("%{public}s: Decoder or data is null.", DSCREEN_LOG_TAG);
        ReportOptFail(DSCREEN_OPT_FAIL, ERR_DH_SCREEN_TRANS_NULL_VALUE, "processImage Decoder is null.");
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    if (data->DataType() == VIDEO_FULL_SCREEN_DATA) {
        int32_t ret = imageDecoder_->InputScreenData(data);
        if (ret != DH_SUCCESS) {
            DHLOGE("%{public}s: InputScreenData failed ret:%{public}" PRId32, DSCREEN_LOG_TAG, ret);
            ReportOptFail(DSCREEN_OPT_FAIL, ret, "InputScreenData failed.");
            return ret;
        }
    } else if (data->DataType() == VIDEO_PART_SCREEN_DATA) {
        int32_t ret = imageJpeg_->FillDirtyImages2Surface(data, imageDecoder_->GetLastFrame());
        if (ret != DH_SUCCESS) {
            DHLOGE("%{public}s: FillDirtyImages2Surface failed ret:%{public}" PRId32, DSCREEN_LOG_TAG, ret);
            ReportOptFail(DSCREEN_OPT_FAIL, ret, "FillDirtyImages2Surface failed.");
            return ret;
        }
    } else {
        DHLOGE("%{public}s: data type is invalid.", DSCREEN_LOG_TAG);
        return ERR_DH_SCREEN_DATA_TYPE_INVALID;
    }
    return DH_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS