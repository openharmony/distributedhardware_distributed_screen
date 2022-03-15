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

#ifndef OHOS_ISCREEN_CHANNEL_LISTENER_H
#define OHOS_ISCREEN_CHANNEL_LISTENER_H

#include <memory>
#include "data_buffer.h"

namespace OHOS {
namespace DistributedHardware {
class IScreenChannelListener {
public:
    virtual ~IScreenChannelListener() = default;

    virtual void OnSessionOpened() = 0;
    virtual void OnSessionClosed() = 0;
    virtual void OnDataReceived(const std::shared_ptr<DataBuffer> &data) = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif