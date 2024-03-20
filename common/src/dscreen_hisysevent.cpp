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

#include "dscreen_hisysevent.h"

#include "hisysevent.h"

#include "dscreen_errcode.h"
#include "dscreen_log.h"

namespace OHOS {
namespace DistributedHardware {
void ReportSaFail(const std::string &eventName, int32_t errCode, int32_t saId, const std::string &errMsg)
{
    int32_t res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCREEN,
        eventName,
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "ERRCODE", errCode,
        "SAID", saId,
        "MSG", errMsg);
    if (res != DH_SUCCESS) {
        DHLOGE("Write HiSysEvent error, res:%{public}" PRId32, res);
    }
}

void ReportRegisterFail(const std::string &eventName, int32_t errCode, const std::string &devId,
    const std::string &dhId, const std::string &errMsg)
{
    int32_t res = HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCREEN, eventName,
                                  OHOS::HiviewDFX::HiSysEvent::EventType::FAULT, "ERRCODE", errCode,
                                  "DEVID", devId, "DHID", dhId, "MSG", errMsg);
    if (res != DH_SUCCESS) {
        DHLOGE("Write HiSysEvent error, res:%{public}" PRId32, res);
    }
}

void ReportUnRegisterFail(const std::string &eventName, int32_t errCode, const std::string &devId,
    const std::string &dhId, const std::string &errMsg)
{
    int32_t res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCREEN,
        eventName,
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "ERRCODE", errCode,
        "DEVID", devId,
        "DHID", dhId,
        "MSG", errMsg);
    if (res != DH_SUCCESS) {
        DHLOGE("Write HiSysEvent error, res:%{public}" PRId32, res);
    }
}

void ReportOptFail(const std::string &eventName, int32_t errCode, const std::string &errMsg)
{
    int32_t res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCREEN,
        eventName,
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "ERRCODE", errCode,
        "MSG", errMsg);
    if (res != DH_SUCCESS) {
        DHLOGE("Write HiSysEvent error, res:%{public}" PRId32, res);
    }
}

void ReportSaEvent(const std::string &eventName, int32_t saId, const std::string &errMsg)
{
    int32_t res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCREEN,
        eventName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "SAID", saId,
        "MSG", errMsg);
    if (res != DH_SUCCESS) {
        DHLOGE("Write HiSysEvent error, res:%{public}" PRId32, res);
    }
}

void ReportRegisterScreenEvent(const std::string &eventName, const std::string &devId, const std::string &dhId,
    const std::string &errMsg)
{
    int32_t res = HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCREEN, eventName,
                                  OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "DEVID", devId,
                                  "DHID", dhId, "MSG", errMsg);
    if (res != DH_SUCCESS) {
        DHLOGE("Write HiSysEvent error, res:%{public}" PRId32, res);
    }
}

void ReportUnRegisterScreenEvent(const std::string &eventName, const std::string &devId, const std::string &dhId,
    const std::string &errMsg)
{
    int32_t res = HiSysEventWrite(OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCREEN, eventName,
                                  OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR, "DEVID", devId, "DHID", dhId,
                                  "MSG", errMsg);
    if (res != DH_SUCCESS) {
        DHLOGE("Write HiSysEvent error, res:%{public}" PRId32, res);
    }
}

void ReportScreenMirrorEvent(const std::string &eventName, const std::string &devId, const std::string &dhId,
    const std::string &errMsg)
{
    int32_t res = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCREEN,
        eventName,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "DEVID", devId,
        "DHID", dhId,
        "MSG", errMsg);
    if (res != DH_SUCCESS) {
        DHLOGE("Write HiSysEvent error, res:%{public}" PRId32, res);
    }
}
} // namespace DistributedHardware
} // namespace OHOS
