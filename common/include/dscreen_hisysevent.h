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
const std::string SA_ERROR = "DSCREEN_SA_START_ERROR";
const std::string WINDOW_ERROR = "DSCREEN_WINDOW_ERROR";
const std::string VIRTUALSCREEN_ERROR = "DSCREEN_VIRTUALSCREEN_ERROR";
const std::string VIDEO_DECODER_ERROR = "DSCREEN_VIDEO_DECODER_ERROR";
const std::string VIDEO_ENCODER_ERROR = "DSCREEN_VIDEO_ENCODER_ERROR";
const std::string SOFTBUS_SESSIONSERVER_ERROR = "DSCREEN_SOFTBUS_SESSIONSERVER_ERROR";
const std::string SOFTBUS_SESSION_ERROR = "DSCREEN_SOFTBUS_SESSION_ERROR";

void ReportScreenEvent(const std::string &eventName, const std::string &errMsg);
} // namespace DistributedHardware
} // namespace OHOS
#endif