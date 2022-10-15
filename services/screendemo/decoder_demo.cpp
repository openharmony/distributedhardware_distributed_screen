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

#include "decoder_demo.h"

#include <iostream>
#include <unistd.h>

#include "avcodec_list.h"
#include "securec.h"
#include "ui/rs_surface_node.h"
#include "wm_common.h"
#include "window.h"
#include "window_option.h"

static const int32_t ES_R[325] = {
    11895, 8109, 1578, 1616, 1313, 572, 805, 837, 755, 706, 952, 879, 13193, 422, 389, 509, 725, 465, 479, 959,
    677, 364, 541, 696, 9306, 322, 318, 767, 590, 422, 530, 403, 505, 566, 445, 508, 7783, 460, 405, 343, 451,
    608, 431, 411, 543, 487, 527, 400, 6287, 385, 418, 391, 592, 434, 412, 398, 504, 492, 479, 561, 5413, 317,
    355, 422, 467, 452, 476, 460, 490, 492, 485, 451, 5036, 312, 408, 460, 432, 502, 388, 475, 407, 544, 401,
    487, 4404, 362, 378, 427, 416, 426, 456, 414, 438, 424, 442, 444, 4310, 362, 388, 393, 390, 441, 398, 423,
    369, 443, 406, 392, 4231, 343, 363, 355, 390, 459, 371, 378, 381, 405, 392, 426, 3975, 387, 337, 393, 439,
    378, 355, 374, 484, 381, 373, 423, 3869, 312, 350, 400, 345, 356, 320, 473, 431, 386, 338, 431, 3426, 268,
    315, 416, 383, 373, 381, 354, 383, 328, 348, 418, 3715, 324, 361, 331, 350, 302, 409, 377, 359, 384, 334,
    326, 3439, 266, 324, 329, 353, 405, 303, 357, 332, 292, 361, 333, 3542, 294, 284, 247, 331, 306, 322, 287,
    367, 341, 276, 258, 3980, 246, 245, 259, 309, 333, 250, 275, 334, 281, 253, 371, 3640, 213, 231, 301, 302,
    228, 289, 290, 281, 201, 284, 277, 4242, 205, 328, 237, 283, 295, 266, 230, 321, 348, 212, 308, 4103, 259,
    238, 245, 298, 330, 265, 271, 287, 267, 286, 290, 3856, 269, 242, 209, 314, 267, 278, 280, 314, 250, 433,
    238, 3654, 195, 246, 301, 298, 250, 270, 320, 269, 305, 258, 368, 3810, 231, 212, 279, 289, 252, 303, 287,
    295, 206, 264, 349, 4071, 242, 296, 271, 231, 307, 265, 254, 267, 317, 232, 348, 4077, 259, 222, 268, 235,
    324, 266, 256, 312, 246, 248, 325, 4000, 266, 201, 230, 293, 264, 265, 273, 301, 304, 253, 266, 3978, 228,
    232, 250, 248, 281, 219, 243, 293, 287, 253, 328, 3719
};

static const int32_t ES_W[183] = {
    2111, 109091, 9316, 969, 13656, 1349, 959, 10484, 1219, 14839, 1092, 23378, 1653, 1725, 1526, 8500, 15407,
    2058, 1346, 21066, 3758, 1734, 1950, 19955, 3997, 1732, 1784, 22687, 4392, 2448, 2180, 17456, 3930, 1851,
    1802, 24227, 4393, 2639, 2778, 18314, 4023, 2392, 2283, 20566, 4118, 2664, 2013, 18964, 2624, 45258, 5860,
    4124, 3473, 27772, 4687, 3140, 2939, 26288, 3808, 2967, 2823, 27219, 3943, 3242, 2667, 27372, 3522, 2899,
    2316, 26608, 3284, 2853, 2285, 19581, 2894, 2436, 24898, 4002, 2876, 2807, 25730, 3903, 2874, 2975, 26309,
    3771, 2763, 2666, 23404, 3826, 2410, 2644, 24629, 4145, 3121, 2878, 50773, 7040, 3945, 3292, 30828, 5210,
    2883, 3277, 31501, 4809, 3068, 3220, 30746, 4715, 3461, 3583, 32278, 4798, 3398, 3389, 31404, 4921, 3382,
    3766, 31821, 5848, 3860, 4047, 37642, 5793, 4271, 4094, 29853, 6163, 4399, 4063, 32151, 6038, 4332, 4041,
    30390, 5679, 4098, 3921, 29401, 5307, 3996, 3945, 45997, 7060, 3716, 4183, 26357, 6190, 3714, 4250, 29086,
    5929, 3491, 4489, 27772, 6656, 4219, 4348, 25851, 6088, 3617, 4477, 25722, 6303, 3856, 4208, 25348, 5896,
    3816, 4521, 22462, 5914, 3673, 4594, 18091, 6474, 3878, 4492, 10890, 4823, 4148
};

using namespace OHOS;
using namespace OHOS::Media;
using namespace std;
namespace {
    constexpr uint32_t DEFAULT_FRAME_RATE = 30;
    constexpr uint32_t MAX_INPUT_BUFFER_SIZE = 30000;
    constexpr uint32_t FRAME_DURATION_US = 33000;
    constexpr uint32_t VIDEO_DATA_FORMAT_NV12 = 2;
    constexpr uint32_t VIDEO_DATA_FORMAT_RGBA = 5;
    constexpr uint32_t SLEEP_THREE_SECOND = 3;
    constexpr uint32_t INDEX_CONSTANT = 10000;
    const string CODEC_NAME_H264 = "OMX_hisi_video_encoder_avc";
    const string CODEC_NAME_MPEG4 = "avenc_mpeg4";
}

void VDecDemo::RunCase()
{
    CheckCodecType();
    CreateVdec();
    Format format;
    format.PutIntValue("width", width_);
    format.PutIntValue("height", height_);
    if (isW) {
        format.PutIntValue("pixel_format", VIDEO_DATA_FORMAT_NV12);
    } else {
        format.PutIntValue("pixel_format", VIDEO_DATA_FORMAT_RGBA);
    }
    format.PutIntValue("frame_rate", DEFAULT_FRAME_RATE);
    format.PutIntValue("max_input_size", MAX_INPUT_BUFFER_SIZE);
    Configure(format);
    SetSurface();
    Prepare();
    Start();
    sleep(SLEEP_THREE_SECOND);
    Stop();
    Release();
}

int32_t VDecDemo::CreateVdec()
{
    if (isW) {
        vdec_ = VideoDecoderFactory::CreateByMime("video/avc");
    } else {
        vdec_ = VideoDecoderFactory::CreateByMime("video/mp4v-es");
    }

    signal_ = make_shared<VDecSignal>();
    cb_ = make_unique<VDecDemoCallback>(signal_);
    vdec_->SetCallback(cb_);
    return 0;
}

int32_t VDecDemo::Configure(const Format &format)
{
    return vdec_->Configure(format);
}

int32_t VDecDemo::Prepare()
{
    return vdec_->Prepare();
}

int32_t VDecDemo::Start()
{
    isRunning_.store(true);

    testFile_ = std::make_unique<std::ifstream>();
    testFile_->open("/data/media/video.es", std::ios::in | std::ios::binary);

    inputLoop_ = make_unique<thread>(&VDecDemo::InputFunc, this);
    outputLoop_ = make_unique<thread>(&VDecDemo::OutputFunc, this);
    return vdec_->Start();
}

int32_t VDecDemo::Stop()
{
    isRunning_.store(false);

    if (inputLoop_ != nullptr && inputLoop_->joinable()) {
        {
            unique_lock<mutex> inLock(signal_->inMutex_);
            signal_->inQueue_.push(INDEX_CONSTANT);
            signal_->inCond_.notify_all();
        }
        inputLoop_->join();
        inputLoop_.reset();
    }

    if (outputLoop_ != nullptr && outputLoop_->joinable()) {
        {
            unique_lock<mutex> outLock(signal_->outMutex_);
            signal_->outQueue_.push(INDEX_CONSTANT);
            signal_->outCond_.notify_all();
        }
        outputLoop_->join();
        outputLoop_.reset();
    }

    return vdec_->Stop();
}

int32_t VDecDemo::Flush()
{
    return vdec_->Flush();
}

int32_t VDecDemo::Reset()
{
    return vdec_->Reset();
}

int32_t VDecDemo::Release()
{
    return vdec_->Release();
}

void VDecDemo::SetOutputSurface(sptr<Surface> surface)
{
    surface_ = surface;
}

void VDecDemo::SetWindowSize(uint32_t width, uint32_t height)
{
    width_ = width;
    height_ = height;
}

int32_t VDecDemo::SetSurface()
{
    return vdec_->SetOutputSurface(surface_);
}

void VDecDemo::CheckCodecType()
{
    std::vector<std::string> localCodecArray;
    std::shared_ptr<Media::AVCodecList> codecList = Media::AVCodecListFactory::CreateAVCodecList();
    std::vector<std::shared_ptr<Media::VideoCaps>> caps = codecList->GetVideoEncoderCaps();
    for (const auto &cap : caps) {
        std::shared_ptr<Media::AVCodecInfo> codecInfo = cap->GetCodecInfo();
        localCodecArray.push_back(codecInfo->GetName());
    }

    if (std::find(localCodecArray.begin(), localCodecArray.end(),
        CODEC_NAME_H264) != localCodecArray.end()) {
        cout << "device is W" << endl;
        isW = true;
    } else if (std::find(localCodecArray.begin(), localCodecArray.end(),
        CODEC_NAME_MPEG4) != localCodecArray.end()) {
        cout << "device is R" << endl;
        isW = false;
    }
}

const int32_t* VDecDemo::GetFrameLen()
{
    const int32_t* frameLen = nullptr;
    if (isW) {
        frameLen = ES_W;
        defaultFrameCount_ = sizeof(ES_W) / sizeof(ES_W[0]);
    } else {
        frameLen = ES_R;
        defaultFrameCount_ = sizeof(ES_R) / sizeof(ES_R[0]);
    }
    return frameLen;
}

void VDecDemo::InputFunc()
{
    const int32_t *frameLen = GetFrameLen();

    while (true) {
        if (!isRunning_.load()) {
            break;
        }

        unique_lock<mutex> lock(signal_->inMutex_);
        signal_->inCond_.wait(lock, [this]() { return signal_->inQueue_.size() > 0; });

        if (!isRunning_.load()) {
            break;
        }

        uint32_t index = signal_->inQueue_.front();
        auto buffer = vdec_->GetInputBuffer(index);

        char *fileBuffer = static_cast<char *>(malloc(sizeof(char) * (*frameLen) + 1));
        if (fileBuffer == nullptr) {
            break;
        }

        (void)testFile_->read(fileBuffer, *frameLen);
        if (memcpy_s(buffer->GetBase(), buffer->GetSize(), fileBuffer, *frameLen) != EOK) {
            free(fileBuffer);
            cout << "Fatal: memcpy fail" << endl;
            break;
        }

        AVCodecBufferInfo info;
        info.size = *frameLen;
        info.offset = 0;
        info.presentationTimeUs = timeStamp_;

        int32_t ret = 0;
        if (isFirstFrame_) {
            ret = vdec_->QueueInputBuffer(index, info, AVCODEC_BUFFER_FLAG_CODEC_DATA);
            isFirstFrame_ = false;
        } else {
            ret = vdec_->QueueInputBuffer(index, info, AVCODEC_BUFFER_FLAG_NONE);
        }

        free(fileBuffer);
        frameLen++;
        timeStamp_ += FRAME_DURATION_US;
        signal_->inQueue_.pop();

        frameCount_++;
        if (frameCount_ == defaultFrameCount_) {
            cout << "Finish decode, exit" << endl;
            break;
        }

        if (ret != 0) {
            cout << "Fatal error, exit" << endl;
            break;
        }
    }
}

void VDecDemo::OutputFunc()
{
    while (true) {
        if (!isRunning_.load()) {
            break;
        }

        unique_lock<mutex> lock(signal_->outMutex_);
        signal_->outCond_.wait(lock, [this]() { return signal_->outQueue_.size() > 0; });

        if (!isRunning_.load()) {
            break;
        }

        uint32_t index = signal_->outQueue_.front();
        if (vdec_->ReleaseOutputBuffer(index, true) != 0) {
            cout << "Fatal: ReleaseOutputBuffer fail" << endl;
            break;
        }

        signal_->outQueue_.pop();
    }
}

VDecDemoCallback::VDecDemoCallback(shared_ptr<VDecSignal> signal)
    : signal_(signal)
{
}

void VDecDemoCallback::OnError(AVCodecErrorType errorType, int32_t errorCode)
{
    cout << "Error received, errorType:" << errorType << " errorCode:" << errorCode << endl;
}

void VDecDemoCallback::OnOutputFormatChanged(const Format &format)
{
    cout << "OnOutputFormatChanged received" << endl;
}

void VDecDemoCallback::OnInputBufferAvailable(uint32_t index)
{
    cout << "OnInputBufferAvailable received, index:" << index << endl;
    unique_lock<mutex> lock(signal_->inMutex_);
    signal_->inQueue_.push(index);
    signal_->inCond_.notify_all();
}

void VDecDemoCallback::OnOutputBufferAvailable(uint32_t index, AVCodecBufferInfo info, AVCodecBufferFlag flag)
{
    cout << "OnOutputBufferAvailable received, index:" << index << endl;
    unique_lock<mutex> lock(signal_->outMutex_);
    signal_->outQueue_.push(index);
    signal_->outCond_.notify_all();
}
