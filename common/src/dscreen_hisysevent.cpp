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

#include <unistd.h>

#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "dscreen_hisysevent.h"

namespace OHOS {
namespace DistributedHardware {
void ReportScreenEvent(const std::string &eventName, const std::string &errMsg)
{
    int32_t res = OHOS::HiviewDFX::HiSysEvent::Write(
        OHOS::HiviewDFX::HiSysEvent::Domain::DISTRIBUTED_SCREEN,
        eventName,
        OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
        "PID", getpid(),
        "UID", getuid(),
        "MSG", errMsg);
    if (res != DH_SUCCESS) {
        DHLOGE("Write HiSysEvent error, res:%d", res);
    }
}
} // namespace DistributedHardware
} // namespace OHOS
