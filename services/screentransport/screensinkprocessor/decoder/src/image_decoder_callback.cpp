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

#include "image_decoder_callback.h"

#include "dscreen_log.h"
#include "image_sink_decoder.h"

namespace OHOS {
namespace DistributedHardware {
void ImageDecoderCallback::OnError(MediaAVCodec::AVCodecErrorType errorType, int32_t errorCode)
{
    DHLOGD("%{public}s: OnError.", DSCREEN_LOG_TAG);
    std::shared_ptr<ImageSinkDecoder> decoder = videoDecoder_.lock();
    if (decoder == nullptr) {
        DHLOGE("decoder is nullptr.");
        return;
    }
    decoder->OnError(errorType, errorCode);
}

void ImageDecoderCallback::OnOutputBufferAvailable(uint32_t index, MediaAVCodec::AVCodecBufferInfo info,
    MediaAVCodec::AVCodecBufferFlag flag, std::shared_ptr<Media::AVSharedMemory> buffer)
{
    DHLOGD("%{public}s: OnOutputBufferAvailable.", DSCREEN_LOG_TAG);
    std::shared_ptr<ImageSinkDecoder> decoder = videoDecoder_.lock();
    if (decoder == nullptr) {
        DHLOGE("decoder is nullptr.");
        return;
    }
    decoder->OnOutputBufferAvailable(index, info, flag, buffer);
}

void ImageDecoderCallback::OnInputBufferAvailable(uint32_t index, std::shared_ptr<Media::AVSharedMemory> buffer)
{
    DHLOGD("%{public}s: OnInputBufferAvailable.", DSCREEN_LOG_TAG);
    std::shared_ptr<ImageSinkDecoder> decoder = videoDecoder_.lock();
    if (decoder == nullptr) {
        DHLOGE("decoder is nullptr.");
        return;
    }
    decoder->OnInputBufferAvailable(index, buffer);
}

void ImageDecoderCallback::OnOutputFormatChanged(const Media::Format &format)
{
    DHLOGD("%{public}s: OnOutputFormatChanged.", DSCREEN_LOG_TAG);
    std::shared_ptr<ImageSinkDecoder> decoder = videoDecoder_.lock();
    if (decoder == nullptr) {
        DHLOGE("decoder is nullptr.");
        return;
    }
    decoder->OnOutputFormatChanged(format);
}
} // namespace DistributedHardware
} // namespace OHOS