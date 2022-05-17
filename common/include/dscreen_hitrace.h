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

#ifndef OHOS_DSCREEN_HITRACE_H
#define OHOS_DSCREEN_HITRACE_H

#include <string>

#include "hitrace_meter.h"

namespace OHOS {
namespace DistributedHardware {
constexpr uint64_t DSCREEN_HITRACE_LABEL = HITRACE_TAG_DISTRIBUTED_SCREEN;

const std::string DSCREEN_SOURCE_LOAD_SYSTEM_ABILITY_START = "DSCREEN_SOURCE_LOAD_SYSTEM_ABILITY_START";
const std::string DSCREEN_SINK_LOAD_SYSTEM_ABILITY_START = "DSCREEN_SINK_LOAD_SYSTEM_ABILITY_START";
const std::string DSCREEN_SOURCE_OPEN_SESSION_START = "DSCREEN_SOURCE_OPEN_SESSION_START";
const std::string DSCREEN_SOURCE_CLOSE_SESSION_START = "DSCREEN_SOURCE_CLOSE_SESSION_START";
const std::string DSCREEN_SOURCE_RELEASE_SESSION_START = "DSCREEN_SOURCE_RELEASE_SESSION_START";
const std::string DSCREEN_SINK_CLOSE_SESSION_START = "DSCREEN_SINK_CLOSE_SESSION_START";
const std::string DSCREEN_SINK_RELEASE_SESSION_START = "DSCREEN_SINK_RELEASE_SESSION_START";
const std::string DSCREEN_START_ENCODER_START = "DSCREEN_START_ENCODER_START";
const std::string DSCREEN_STOP_ENCODER_START =  "DSCREEN_STOP_ENCODER_START";
const std::string DSCREEN_RELESSE_ENCODER_START =  "DSCREEN_RELESSE_ENCODER_START";
const std::string DSCREEN_START_DECODER_START = "DSCREEN_START_DECODER_START";
const std::string DSCREEN_STOP_DECODER_START =  "DSCREEN_STOP_DECODER_START";
const std::string DSCREEN_RELEASE_DECODER_START =  "DSCREEN_RELEASE_DECODER_START";
} // namespace DistributedHardware
} // namespace OHOS
#endif