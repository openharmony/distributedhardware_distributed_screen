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

#ifndef OHOS_DSCREEN_UTIL_H
#define OHOS_DSCREEN_UTIL_H

#include <cstdint>
#include <string>

namespace OHOS {
namespace DistributedHardware {
uint64_t GetCurrentTimeUs();
int32_t GetLocalDeviceNetworkId(std::string &networkId);
std::string GetRandomID();
std::string GetAnonyString(const std::string &value);
std::string GetInterruptString(const std::string &value);

bool IsPartialRefreshEnabled();
bool IsSupportAVTransEngine(const std::string &version);
} // namespace DistributedHardware
} // namespace OHOS
#endif