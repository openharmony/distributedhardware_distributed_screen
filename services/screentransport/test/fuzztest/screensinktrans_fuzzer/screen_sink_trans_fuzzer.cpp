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

#include "screen_sink_trans_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <surface.h>

#include "screenregion.h"
#include "screen_sink_trans.h"
#include "screentrans_test_utils.h"

namespace OHOS {
namespace DistributedHardware {
void ScreenSinkTransFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }

    std::shared_ptr<ScreenSinkTrans> screenSinkTrans = std::make_shared<ScreenSinkTrans>();
    std::shared_ptr<VideoParam> param = std::make_shared<VideoParam>();
    uint32_t screenWidth = *(reinterpret_cast<const uint32_t*>(data));
    uint32_t screenHeight = *(reinterpret_cast<const uint32_t*>(data));
    uint32_t videoWidth = *(reinterpret_cast<const uint32_t*>(data));
    uint32_t videoHeight = *(reinterpret_cast<const uint32_t*>(data));
    param->SetScreenWidth(screenWidth);
    param->SetScreenHeight(screenHeight);
    param->SetVideoWidth(videoWidth);
    param->SetScreenHeight(videoHeight);
    std::string peerDevId(reinterpret_cast<const char*>(data), size);
    std::string remoteDevId(reinterpret_cast<const char*>(data), size);
    uint64_t screenId = *(reinterpret_cast<const uint64_t*>(data));
    uint64_t displayId = *(reinterpret_cast<const uint64_t*>(data));
    std::shared_ptr<IScreenSinkTransCallback> callBack =
        std::make_shared<ScreenRegion>(remoteDevId, screenId, displayId);
    std::string name(reinterpret_cast<const char*>(data), size);
    sptr<Surface> surface = Surface::CreateSurfaceAsConsumer(name);

    screenSinkTrans->SetUp(*param, *param, peerDevId);
    screenSinkTrans->SetImageSurface(surface);
    screenSinkTrans->Start();
    screenSinkTrans->RegisterStateCallback(callBack);
    screenSinkTrans->Stop();
    screenSinkTrans->Release();
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::ScreenSinkTransFuzzTest(data, size);
    return 0;
}