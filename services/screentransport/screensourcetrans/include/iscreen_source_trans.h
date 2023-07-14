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

#ifndef OHOS_ISCREEN_SOURCE_TRANS_H
#define OHOS_ISCREEN_SOURCE_TRANS_H

#include "surface.h"

#include "iscreen_source_trans_callback.h"
#include "video_param.h"

namespace OHOS {
namespace DistributedHardware {
class IScreenSourceTrans {
public:
    virtual ~IScreenSourceTrans() = default;

    virtual int32_t SetUp(const VideoParam &localParam, const VideoParam &remoteParam,
        const std::string &peerDevId) = 0;
    virtual int32_t Release() = 0;
    virtual int32_t Start() = 0;
    virtual int32_t Stop() = 0;
    virtual int32_t RegisterStateCallback(const std::shared_ptr<IScreenSourceTransCallback> &callBack) = 0;
    virtual sptr<Surface> GetImageSurface() = 0;
    virtual int32_t SetConsumerSurface() = 0;
    virtual void SetScreenVersion(const std::string &version) = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif