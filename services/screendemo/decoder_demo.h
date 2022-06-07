/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef AVCODEC_VDEC_DEMO_H
#define AVCODEC_VDEC_DEMO_H

#include <atomic>
#include <fstream>
#include <queue>
#include <string>
#include <thread>

#include "avcodec_video_decoder.h"
#include "nocopyable.h"

namespace OHOS {
namespace Media {
class VDecSignal {
public:
    std::mutex inMutex_;
    std::mutex outMutex_;
    std::condition_variable inCond_;
    std::condition_variable outCond_;
    std::queue<uint32_t> inQueue_;
    std::queue<uint32_t> outQueue_;
};

class VDecDemoCallback : public AVCodecCallback, public NoCopyable {
public:
    explicit VDecDemoCallback(std::shared_ptr<VDecSignal> signal);
    virtual ~VDecDemoCallback() = default;

    void OnError(AVCodecErrorType errorType, int32_t errorCode) override;
    void OnOutputFormatChanged(const Format &format) override;
    void OnInputBufferAvailable(uint32_t index) override;
    void OnOutputBufferAvailable(uint32_t index, AVCodecBufferInfo info, AVCodecBufferFlag flag) override;

private:
    std::shared_ptr<VDecSignal> signal_;
};

class VDecDemo : public NoCopyable {
public:
    VDecDemo() = default;
    virtual ~VDecDemo() = default;
    void RunCase();
    void SetOutputSurface(sptr<Surface> surface);
    void SetWindowSize(uint32_t width, uint32_t height);

private:
    int32_t CreateVdec();
    int32_t Configure(const Format &format);
    int32_t Prepare();
    int32_t Start();
    int32_t Stop();
    int32_t Flush();
    int32_t Reset();
    int32_t Release();
    int32_t SetSurface();
    const int32_t *GetFrameLen();
    void InputFunc();
    void OutputFunc();
    void CheckCodecType();

    std::atomic<bool> isRunning_ = false;
    sptr<Surface> surface_ = nullptr;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    std::unique_ptr<std::ifstream> testFile_;
    std::unique_ptr<std::thread> inputLoop_;
    std::unique_ptr<std::thread> outputLoop_;
    std::shared_ptr<Media::AVCodecVideoDecoder> vdec_;
    std::shared_ptr<VDecSignal> signal_;
    std::shared_ptr<VDecDemoCallback> cb_;
    bool isFirstFrame_ = true;
    bool isW = true;
    int64_t timeStamp_ = 0;
    uint32_t frameCount_ = 0;
    uint32_t defaultFrameCount_ = 0;
};
} // namespace Media
} // namespace OHOS
#endif // AVCODEC_VDEC_DEMO_H