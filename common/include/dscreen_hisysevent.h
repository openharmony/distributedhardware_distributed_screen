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

#ifndef OHOS_DSCREEN_HISYSEVENT_H
#define OHOS_DSCREEN_HISYSEVENT_H

#include <string>

#include "hisysevent.h"

namespace OHOS {
namespace DistributedHardware {
const std::string SA_ERROR = "DSCREEN_INIT_FAIL";
const std::string REGISTER_ERROR = "DSCREEN_REGISTER_FAIL";
const std::string UNREGISTER_ERROR = "DSCREEN_UNREGISTER_FAIL";
const std::string WINDOW_ERROR = "DSCREEN_WINDOW_FAIL";
const std::string VIDEO_ENCODER_ERROR = "DSCREEN_ENCODE_FAIL";
const std::string VIDEO_DECODER_ERROR = "DSCREEN_DECODE_FAIL";
const std::string SOFTBUS_SESSIONSERVER_ERROR = "DSCREEN_CREATE_SESSIONSVR_FAIL";
const std::string SOFTBUS_SESSION_ERROR = "DSCREEN_OPEN_SESSION_FAIL";

const std::string SA_START = "DSCREEN_INIT";
const std::string ENABLE_REGISTER = "DSCREEN_REGISTER";
const std::string DISABLE_UNREGISTER = "DSCREEN_UNREGISTER";
const std::string PROJECT_START = "DSCREEN_PROJECT_START";
const std::string PROJECT_END = "DSCREEN_PROJECT_END";
const std::string SA_EXIT = "DSCREEN_EXIT";

void ReportSaFail(const std::string &eventName, int32_t errCode, int32_t saId, const std::string &errMsg);
void ReportRegisterFail(const std::string &eventName, int32_t errCode, const std::string &devId,
    const std::string &dhId, const std::string &errMsg);
void ReportUnRegisterFail(const std::string &eventName, int32_t errCode, const std::string &devId,
    const std::string &dhId, const std::string &errMsg);
void ReportWindowFail(const std::string &eventName, int32_t errCode, const std::string &errMsg);
void ReportVideoEncoderFail(const std::string &eventName, int32_t errCode, int32_t width, int32_t height,
    uint8_t format, const std::string &errMsg);
void ReportVideoDecoderFail(const std::string eventName, int32_t errCode, int32_t width, int32_t height,
    uint8_t format, const std::string &errMsg);
void ReportSoftbusSessionServerFail(const std::string &eventName, int32_t errCode, const std::string &pkgName,
    const std::string &sessionName, const std::string &errMsg);
void ReportSoftbusSessionFail(const std::string &eventName, int32_t errCode, const std::string &sessName,
    const std::string &errMsg);

void ReportSaEvent(const std::string &eventName, int32_t saId, const std::string &errMsg);
void ReportRegisterScreenEvent(const std::string &eventName, const std::string &devId, const std::string &dhId,
    const std::string &errMsg);
void ReportUnRegisterScreenEvent(const std::string &eventName, const std::string &devId, const std::string &dhId,
    const std::string &errMsg);
void ReportScreenMirrorEvent(const std::string &eventName, const std::string &devId, const std::string &dhId,
    const std::string &errMsg);
} // namespace DistributedHardware
} // namespace OHOS
#endif