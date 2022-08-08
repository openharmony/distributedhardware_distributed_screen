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

#include "dscreen_fwkkit.h"
#include "dscreen_log.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(DScreenFwkKit);
std::shared_ptr<DistributedHardwareFwkKit> DScreenFwkKit::GetDHFwkKit()
{
    DHLOGI("GetDHFwkKit");
    std::lock_guard<std::mutex> lck(kitMtx_);
    if (dhfwkKit_ == nullptr) {
        dhfwkKit_ =  std::make_shared<DistributedHardwareFwkKit>();
    }
    return dhfwkKit_;
}
} // namespace DistributedHardware
} // namespace OHOS