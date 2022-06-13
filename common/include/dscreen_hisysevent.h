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
const std::string DSCREEN_INIT_FAIL = "DSCREEN_INIT_FAIL";
const std::string DSCREEN_REGISTER_FAIL = "DSCREEN_REGISTER_FAIL";
const std::string DSCREEN_UNREGISTER_FAIL = "DSCREEN_UNREGISTER_FAIL";
const std::string DSCREEN_OPT_FAIL = "DSCREEN_OPT_FAIL";

const std::string DSCREEN_INIT = "DSCREEN_INIT";
const std::string DSCREEN_REGISTER = "DSCREEN_REGISTER";
const std::string DSCREEN_UNREGISTER = "DSCREEN_UNREGISTER";
const std::string DSCREEN_PROJECT_START = "DSCREEN_PROJECT_START";
const std::string DSCREEN_PROJECT_END = "DSCREEN_PROJECT_END";
const std::string DSCREEN_EXIT = "DSCREEN_EXIT";

void ReportSaFail(const std::string &eventName, int32_t errCode, int32_t saId, const std::string &errMsg);
void ReportRegisterFail(const std::string &eventName, int32_t errCode, const std::string &devId,
    const std::string &dhId, const std::string &errMsg);
void ReportUnRegisterFail(const std::string &eventName, int32_t errCode, const std::string &devId,
    const std::string &dhId, const std::string &errMsg);
void ReportOptFail(const std::string &eventName, int32_t errCode, const std::string &errMsg);

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