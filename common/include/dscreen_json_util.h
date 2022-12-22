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

#ifndef OHOS_DSCREEN_JSON_UTIL_H
#define OHOS_DSCREEN_JSON_UTIL_H

#include "nlohmann/json.hpp"

namespace OHOS {
namespace DistributedHardware {
bool IsString(const nlohmann::json &jsonObj, const std::string &key);
bool IsUInt8(const nlohmann::json &jsonObj, const std::string &key);
bool IsInt32(const nlohmann::json &jsonObj, const std::string &key);
bool IsUInt32(const nlohmann::json &jsonObj, const std::string &key);
bool IsInt64(const nlohmann::json &jsonObj, const std::string &key);
bool IsUInt64(const nlohmann::json &jsonObj, const std::string &key);
bool IsArray(const nlohmann::json &jsonObj, const std::string &key);
bool IsBool(const nlohmann::json &jsonObj, const std::string &key);
} // namespace DistributedHardware
} // namespace OHOS
#endif