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

#include "image_sink_processor.h"

#include "dscreen_errcode.h"
#include "dscreen_log.h"

namespace OHOS {
namespace DistributedHardware {
int32_t ImageSinkProcessor::ConfigureImageProcessor(
    const VideoParam &localParam, const VideoParam &remoteParam,
    const std::shared_ptr<IImageSinkProcessorListener> &imageListener)
{
    DHLOGI("%s: ConfigureImageProcessor.", LOG_TAG);
    localParam_ = localParam;
    remoteParam_ = remoteParam;

    imageDecoder_ = std::make_shared<ImageSinkDecoder>(imageListener);
    if (imageDecoder_ == nullptr) {
        DHLOGE("%s: Decoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = imageDecoder_->ConfigureDecoder(localParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: ConfigureDecoder failed ret:%d.", LOG_TAG, ret);
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ImageSinkProcessor::ReleaseImageProcessor()
{
    DHLOGI("%s: ReleaseImageProcessor.", LOG_TAG);
    if (imageDecoder_ == nullptr) {
        DHLOGE("%s: Decoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = imageDecoder_->ReleaseDecoder();
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: ReleaseDecoder failed.", LOG_TAG);
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ImageSinkProcessor::StartImageProcessor()
{
    DHLOGI("%s: StartImageProcessor.", LOG_TAG);
    if (imageDecoder_ == nullptr) {
        DHLOGE("%s: Decoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = imageDecoder_->StartDecoder();
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: StartDecoder failed ret:%d.", LOG_TAG, ret);
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ImageSinkProcessor::StopImageProcessor()
{
    DHLOGI("%s: StopImageProcessor.", LOG_TAG);
    if (imageDecoder_ == nullptr) {
        DHLOGE("%s: Decoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = imageDecoder_->StopDecoder();
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: StopDecoder failed ret:%d.", LOG_TAG, ret);
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ImageSinkProcessor::SetImageSurface(sptr<Surface> &surface)
{
    DHLOGI("%s: SetImageSurface.", LOG_TAG);
    if (imageDecoder_ == nullptr) {
        DHLOGE("%s: Decoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = imageDecoder_->SetOutputSurface(surface);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: SetOutputSurface failed ret:%d.", LOG_TAG, ret);
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ImageSinkProcessor::ProcessImage(const std::shared_ptr<DataBuffer> &data)
{
    DHLOGI("%s: ProcessImage.", LOG_TAG);
    if (imageDecoder_ == nullptr) {
        DHLOGE("%s: Decoder is null.", LOG_TAG);
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    int32_t ret = imageDecoder_->InputScreenData(data);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: InputScreenData failed ret:%d.", LOG_TAG, ret);
        return ret;
    }

    return DH_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS