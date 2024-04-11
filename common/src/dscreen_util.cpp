/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

uint64_t GetCurrentTimeUs()
{
    constexpr int32_t usOneSecond = 1000 * 1000;
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * usOneSecond + tv.tv_usec;
}

int32_t GetLocalDeviceNetworkId(std::string &networkId)
{
    NodeBasicInfo basicInfo = { { 0 } };
    int32_t ret = GetLocalNodeDeviceInfo(PKG_NAME.c_str(), &basicInfo);
    if (ret != DH_SUCCESS) {
        DHLOGE("GetLocalDeviceNetworkId failed ret: %{public}" PRId32, ret);
        return ret;
    }

    networkId = std::string(basicInfo.networkId);
    return DH_SUCCESS;
}

std::string GetRandomID()
{
    static std::random_device randomDevice;
    static std::uniform_int_distribution<uint64_t> dist(0ULL, 0xFFFFFFFFFFFFFFFFULL);
    uint64_t ab = dist(randomDevice);
    uint64_t cd = dist(randomDevice);
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    std::stringstream stringStream;
    ab = (ab & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
    cd = (cd & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;
    a = (ab >> 32U);
    b = (ab & 0xFFFFFFFFU);
    c = (cd >> 32U);
    d = (cd & 0xFFFFFFFFU);
    stringStream << std::hex << std::nouppercase << std::setfill('0');
    stringStream << std::setw(WORD_WIDTH_8) << (a);
    stringStream << std::setw(WORD_WIDTH_4) << (b >> 16U);
    stringStream << std::setw(WORD_WIDTH_4) << (b & 0xFFFFU);
    stringStream << std::setw(WORD_WIDTH_4) << (c >> 16U);
    stringStream << std::setw(WORD_WIDTH_4) << (c & 0xFFFFU);
    stringStream << std::setw(WORD_WIDTH_8) << d;

    return stringStream.str();
}

std::string GetAnonyString(const std::string &value)
{
    constexpr size_t int32ShortIdLength = 20;
    constexpr size_t int32MinIdLength = 3;
    std::string result;
    std::string tmpStr("******");
    size_t strLen = value.length();
    if (strLen < int32MinIdLength) {
        return tmpStr;
    }

    if (strLen <= int32ShortIdLength) {
        result += value[0];
        result += tmpStr;
        result += value[strLen - 1];
    } else {
        constexpr size_t int32PlainTextLength = 4;
        result.append(value, 0, int32PlainTextLength);
        result += tmpStr;
        result.append(value, strLen - int32PlainTextLength, int32PlainTextLength);
    }

    return result;
}

std::string GetInterruptString(const std::string &value)
{
    constexpr size_t int32MinIdLength = 3;
    constexpr size_t stringHalfLength = 2;
    std::string res;
    size_t strlen = value.length();
    if (strlen <= int32MinIdLength) {
        res = value;
    } else  {
        res = value.substr(0, strlen / stringHalfLength);
    }

    return res;
}

bool IsPartialRefreshEnabled()
{
    char tempValue[SYSTEM_PARAM_VALUE_SIZE] = {0};
    auto ret = GetParameter(PARTIAL_REFRESH_PARAM, "-1", tempValue, sizeof(tempValue));
    if (ret <= 0) {
        DHLOGE("get system parameter (dscreen.partial.refresh.enable) failed, ret=%{public}" PRId32, ret);
        return false;
    }
    DHLOGI("get system parameter (dscreen.partial.refresh.enable) success, param value = %{public}s", tempValue);
    return (std::atoi(tempValue) == PARTIAL_REFRESH_ENABLED_VALUE);
}

bool IsSupportAVTransEngine(const std::string &version)
{
    return (std::atoi(version.c_str()) >= AV_TRANS_SUPPORTED_VERSION) && !IsPartialRefreshEnabled();
}
} // namespace DistributedHardware
} // namespace OHOS