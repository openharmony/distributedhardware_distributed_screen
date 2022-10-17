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

#include "image_source_processor.h"

#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_hitrace.h"
#include "dscreen_log.h"

namespace OHOS {
namespace DistributedHardware {
int32_t ImageSourceProcessor::ConfigureImageProcessor(const VideoParam &localParam, const VideoParam &remoteParam,
    const std::shared_ptr<IImageSourceProcessorListener> &listener)
{
    DHLOGI("%s: ConfigureImageProcessor.", LOG_TAG);
    imageEncoder_ = std::make_shared<ImageSourceEncoder>(listener);

    int32_t ret = imageEncoder_->ConfigureEncoder(localParam);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Configure screen encoder failed ret: %d.", LOG_TAG, ret);
        return ret;
    }

    localParam_ = localParam;
    remoteParam_ = remoteParam;
    return DH_SUCCESS;
}

int32_t ImageSourceProcessor::ReleaseImageProcessor()
{
    DHLOGI("%s: ReleaseImageProcessor.", LOG_TAG);
    if (imageEncoder_ == nullptr) {
        DHLOGE("%s: Create screen encoder failed.", LOG_TAG);
        ReportOptFail(DSCREEN_OPT_FAIL, ERR_DH_SCREEN_TRANS_NULL_VALUE, "ReleaseImageProcessor encoder is nullptr.");
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_RELESSE_ENCODER_START);
    int32_t ret = imageEncoder_->ReleaseEncoder();
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Release screen encoder failed ret: %d.", LOG_TAG, ret);
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "Release screen encoder failed.");
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ImageSourceProcessor::StartImageProcessor()
{
    DHLOGI("%s: StartImageProcessor.", LOG_TAG);
    if (imageEncoder_ == nullptr) {
        DHLOGE("%s: Create screen encoder failed.", LOG_TAG);
        ReportOptFail(DSCREEN_OPT_FAIL, ERR_DH_SCREEN_TRANS_NULL_VALUE, "StartImageProcessor encoder is nullptr.");
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_START_ENCODER_START);
    int32_t ret = imageEncoder_->StartEncoder();
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Start screen encoder failed ret: %d.", LOG_TAG, ret);
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "Start screen encoder failed.");
        return ret;
    }

    return DH_SUCCESS;
}

int32_t ImageSourceProcessor::StopImageProcessor()
{
    DHLOGI("%s: StopImageProcessor.", LOG_TAG);
    if (imageEncoder_ == nullptr) {
        DHLOGE("%s: Create screen encoder failed.", LOG_TAG);
        ReportOptFail(DSCREEN_OPT_FAIL, ERR_DH_SCREEN_TRANS_NULL_VALUE, "StopImageProcessor encoder is nullptr.");
        return ERR_DH_SCREEN_TRANS_NULL_VALUE;
    }

    StartTrace(DSCREEN_HITRACE_LABEL, DSCREEN_STOP_ENCODER_START);
    int32_t ret = imageEncoder_->StopEncoder();
    FinishTrace(DSCREEN_HITRACE_LABEL);
    if (ret != DH_SUCCESS) {
        DHLOGE("%s: Stop screen encoder failed ret: %d.", LOG_TAG, ret);
        ReportOptFail(DSCREEN_OPT_FAIL, ret, "Stop screen encoder failed.");
        return ret;
    }

    return DH_SUCCESS;
}

sptr<Surface> &ImageSourceProcessor::GetImageSurface()
{
    DHLOGI("%s: GetImageSurface.", LOG_TAG);
    return imageEncoder_->GetInputSurface();
}
} // namespace DistributedHardware
} // namespace OHOS