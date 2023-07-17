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

#include "dscreen_util.h"

#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <random>
#include <sstream>
#include <sys/time.h>

#include "softbus_bus_center.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "parameter.h"

namespace OHOS {
namespace DistributedHardware {
constexpr int32_t WORD_WIDTH_8 = 8;
constexpr int32_t WORD_WIDTH_4 = 4;

int32_t GetLocalDeviceNetworkId(std::string &networkId)
{
    NodeBasicInfo basicInfo = { { 0 } };
    int32_t ret = GetLocalNodeDeviceInfo(PKG_NAME.c_str(), &basicInfo);
    if (ret != DH_SUCCESS) {
        DHLOGE("GetLocalDeviceNetworkId failed ret: %" PRId32, ret);
        return ret;
    }

    networkId = std::string(basicInfo.networkId);
    return DH_SUCCESS;
}

std::string GetRandomID()
{
    static std::random_device rd;
    static std::uniform_int_distribution<uint64_t> dist(0ULL, 0xFFFFFFFFFFFFFFFFULL);
    uint64_t ab = dist(rd);
    uint64_t cd = dist(rd);
    uint32_t a, b, c, d;
    std::stringstream ss;
    ab = (ab & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
    cd = (cd & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;
    a = (ab >> 32U);
    b = (ab & 0xFFFFFFFFU);
    c = (cd >> 32U);
    d = (cd & 0xFFFFFFFFU);
    ss << std::hex << std::nouppercase << std::setfill('0');
    ss << std::setw(WORD_WIDTH_8) << (a);
    ss << std::setw(WORD_WIDTH_4) << (b >> 16U);
    ss << std::setw(WORD_WIDTH_4) << (b & 0xFFFFU);
    ss << std::setw(WORD_WIDTH_4) << (c >> 16U);
    ss << std::setw(WORD_WIDTH_4) << (c & 0xFFFFU);
    ss << std::setw(WORD_WIDTH_8) << d;

    return ss.str();
}

std::string GetAnonyString(const std::string &value)
{
    constexpr size_t INT32_SHORT_ID_LENGTH = 20;
    constexpr size_t INT32_MIN_ID_LENGTH = 3;
    std::string res;
    std::string tmpStr("******");
    size_t strLen = value.length();
    if (strLen < INT32_MIN_ID_LENGTH) {
        return tmpStr;
    }

    if (strLen <= INT32_SHORT_ID_LENGTH) {
        res += value[0];
        res += tmpStr;
        res += value[strLen - 1];
    } else {
        constexpr size_t INT32_PLAINTEXT_LENGTH = 4;
        res.append(value, 0, INT32_PLAINTEXT_LENGTH);
        res += tmpStr;
        res.append(value, strLen - INT32_PLAINTEXT_LENGTH, INT32_PLAINTEXT_LENGTH);
    }

    return res;
}

std::string GetInterruptString(const std::string &value)
{
    constexpr size_t INT32_MIN_ID_LENGTH = 3;
    constexpr size_t STRING_HALF_LENGTH = 2;
    std::string res;
    size_t strlen = value.length();
    if (strlen <= INT32_MIN_ID_LENGTH) {
        res = value;
    } else  {
        res = value.substr(0, strlen / STRING_HALF_LENGTH);
    }

    return res;
}

bool IsPartialRefreshEnabled()
{
    char tempValue[SYSTEM_PARAM_VALUE_SIZE] = {0};
    auto ret = GetParameter(PARTIAL_REFRESH_PARAM, "-1", tempValue, sizeof(tempValue));
    if (ret <= 0) {
        DHLOGE("get system parameter (dscreen.partial.refresh.enable) failed, ret=%" PRId32, ret);
        return false;
    }
    DHLOGI("get system parameter (dscreen.partial.refresh.enable) success, param value = %s", tempValue);
    return (std::atoi(tempValue) == PARTIAL_REFRESH_ENABLED_VALUE);
}

bool IsSupportAVTransEngine(const std::string &version)
{
    
    return (std::atoi(const_cast<char *>(version.c_str())) >= AV_TRANS_SUPPORTED_VERSION) && !IsPartialRefreshEnabled();
}
} // namespace DistributedHardware
} // namespace OHOS