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

#include "image_encoder_callback.h"

#include "dscreen_log.h"
#include "image_source_encoder.h"

namespace OHOS {
namespace DistributedHardware {
void ImageEncoderCallback::OnError(MediaAVCodec::AVCodecErrorType errorType, int32_t errorCode)
{
    DHLOGD("%{public}s: OnError.", DSCREEN_LOG_TAG);
    std::shared_ptr<ImageSourceEncoder> encoder = videoEncoder_.lock();
    if (encoder == nullptr) {
        DHLOGE("encoder is nullptr.");
        return;
    }
    encoder->OnError(errorType, errorCode);
}

void ImageEncoderCallback::OnOutputBufferAvailable(uint32_t index, MediaAVCodec::AVCodecBufferInfo info,
    MediaAVCodec::AVCodecBufferFlag flag, std::shared_ptr<Media::AVSharedMemory> buffer)
{
    DHLOGD("%{public}s: OnOutputBufferAvailable.", DSCREEN_LOG_TAG);
    std::shared_ptr<ImageSourceEncoder> encoder = videoEncoder_.lock();
    if (encoder == nullptr) {
        DHLOGE("encoder is nullptr.");
        return;
    }
    encoder->OnOutputBufferAvailable(index, info, flag, buffer);
}

void ImageEncoderCallback::OnInputBufferAvailable(uint32_t index, std::shared_ptr<Media::AVSharedMemory> buffer)
{
    DHLOGD("%{public}s: OnInputBufferAvailable.", DSCREEN_LOG_TAG);
    std::shared_ptr<ImageSourceEncoder> encoder = videoEncoder_.lock();
    if (encoder == nullptr) {
        DHLOGE("encoder is nullptr.");
        return;
    }
    encoder->OnInputBufferAvailable(index, buffer);
}

void ImageEncoderCallback::OnOutputFormatChanged(const Media::Format &format)
{
    DHLOGD("%{public}s: OnOutputFormatChanged.", DSCREEN_LOG_TAG);
    std::shared_ptr<ImageSourceEncoder> encoder = videoEncoder_.lock();
    if (encoder == nullptr) {
        DHLOGE("encoder is nullptr.");
        return;
    }
    encoder->OnOutputFormatChanged(format);
}
} // namespace DistributedHardware
} // namespace OHOS