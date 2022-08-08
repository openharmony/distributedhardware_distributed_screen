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

#ifndef OHOS_DSCREEN_FWKKIT_H
#define OHOS_DSCREEN_FWKKIT_H

#include <mutex>

#include "distributed_hardware_fwk_kit.h"
#include "single_instance.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenFwkKit {
DECLARE_SINGLE_INSTANCE_BASE(DScreenFwkKit);
public:
    std::shared_ptr<DistributedHardwareFwkKit> GetDHFwkKit();
private:
    DScreenFwkKit() = default;
    ~DScreenFwkKit() = default;
    std::shared_ptr<DistributedHardwareFwkKit> dhfwkKit_ = nullptr;
    std::mutex kitMtx_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif