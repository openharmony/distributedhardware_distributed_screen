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

#include "screen_source_trans_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "dscreen.h"
#include "dscreen_errcode.h"
#include "dscreen_manager.h"
#include "screen_source_trans.h"
#include "video_param.h"
#include "screentrans_test_utils.h"

namespace OHOS {
namespace DistributedHardware {
void ScreenSourceTransFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(uint32_t))) {
        return;
    }

    std::shared_ptr<ScreenSourceTrans> screenSourceTrans = std::make_shared<ScreenSourceTrans>();
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
    std::string devId(reinterpret_cast<const char*>(data), size);
    std::string dhId(reinterpret_cast<const char*>(data), size);
    std::shared_ptr<IDScreenCallback> dScreenCallback = std::make_shared<DScreenCallback>();
    std::shared_ptr<IScreenSourceTransCallback> callBack = std::make_shared<DScreen>(devId, dhId, dScreenCallback);

    screenSourceTrans->SetUp(*param, *param, peerDevId);
    screenSourceTrans->GetImageSurface();
    screenSourceTrans->Start();
    screenSourceTrans->RegisterStateCallback(callBack); 
    screenSourceTrans->Stop();
    screenSourceTrans->Release();
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::ScreenSourceTransFuzzTest(data, size);
    return 0;
}