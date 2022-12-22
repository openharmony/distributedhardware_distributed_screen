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

#include "dscreen_json_util.h"

#include "dscreen_constants.h"
#include "dscreen_log.h"

namespace OHOS {
namespace DistributedHardware {
bool IsString(const nlohmann::json &jsonObj, const std::string &key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_string() && jsonObj[key].size() <= MAX_MESSAGE_LEN;
    if (!res) {
        DHLOGE("the key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}

bool IsUInt8(const nlohmann::json &jsonObj, const std::string &key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_number_unsigned() && jsonObj[key] <= UINT8_MAX;
    if (!res) {
        DHLOGE("the key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}

bool IsInt32(const nlohmann::json &jsonObj, const std::string &key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_number_integer() && jsonObj[key] >= INT32_MIN &&
        jsonObj[key] <= INT32_MAX;
    if (!res) {
        DHLOGE("the key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}

bool IsUInt32(const nlohmann::json &jsonObj, const std::string &key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_number_unsigned() && jsonObj[key] <= UINT32_MAX;
    if (!res) {
        DHLOGE("the key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}

bool IsInt64(const nlohmann::json &jsonObj, const std::string &key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_number_integer() && jsonObj[key] >= INT64_MIN &&
        jsonObj[key] <= INT64_MAX;
    if (!res) {
        DHLOGE("the key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}

bool IsUInt64(const nlohmann::json &jsonObj, const std::string &key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_number_unsigned() && jsonObj[key] <= UINT64_MAX;
    if (!res) {
        DHLOGE("the key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}

bool IsArray(const nlohmann::json &jsonObj, const std::string &key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_array();
    if (!res) {
        DHLOGE("the key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}

bool IsBool(const nlohmann::json &jsonObj, const std::string &key)
{
    bool res = jsonObj.contains(key) && jsonObj[key].is_boolean();
    if (!res) {
        DHLOGE("the key %s in jsonObj is invalid.", key.c_str());
    }
    return res;
}
} // namespace DistributedHardware
} // namespace OHOS