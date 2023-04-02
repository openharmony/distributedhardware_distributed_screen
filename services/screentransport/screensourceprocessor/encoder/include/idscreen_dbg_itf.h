/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef OHOS_DSCREEN_DBG_ITF_H
#define OHOS_DSCREEN_DBG_ITF_H

#include <string>
#include <cstdint>

namespace OHOS {
namespace DistributedHardware {
const std::string GET_DBG_ITF_FUNC = "GetDBGItf";
const std::string GET_IMAGE_DIRTY_FUNC = "GetImageSetDirty";
class IDScreenDBGItf {
public:
    virtual int32_t Init() = 0;
    virtual int32_t Release() = 0;
};
class IImageSetDirty {
public:
    virtual void SetDamage(std::vector<std::vector<int32_t>> dirtyVecs) = 0;
    virtual std::vector<std::vector<int32_t>> GetDamage() = 0;
};
extern "C" __attribute__((visibility("default"))) IDScreenDBGItf *GetDBGItf();
extern "C" __attribute__((visibility("default"))) IImageSetDirty *GetImageSetDirty();
}
}
#endif