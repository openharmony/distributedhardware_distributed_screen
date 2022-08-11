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

#include "dscreen_sa_process_state.h"

#include <cstdint>
#include <cstdlib>
#include <mutex>

#include "dscreen_constants.h"
#include "dscreen_hisysevent.h"
#include "dscreen_log.h"

namespace OHOS {
namespace DistributedHardware {
enum DScreenSAState : uint32_t {
    DSCREEN_SA_EXIT_STATE_START = 0,
    DSCREEN_SA_EXIT_STATE_STOP = 1
};
DScreenSAState g_sinkSAState = DSCREEN_SA_EXIT_STATE_START;
DScreenSAState g_sourceSAState = DSCREEN_SA_EXIT_STATE_START;
std::mutex g_saProcessState;

void SetSinkProcessExit()
{
    DHLOGI("set sink process exit.");
    std::lock_guard<std::mutex> autoLock(g_saProcessState);
    g_sinkSAState = DSCREEN_SA_EXIT_STATE_STOP;
    DHLOGI("g_sourceSAState = %d g_sinkSAState = %d", g_sourceSAState, g_sinkSAState);
    if (g_sourceSAState == DSCREEN_SA_EXIT_STATE_START || g_sinkSAState == DSCREEN_SA_EXIT_STATE_START) {
        return;
    }
    DHLOGI("exit sa process success.");
    ReportSaEvent(DSCREEN_EXIT, DISTRIBUTED_HARDWARE_SCREEN_SINK_SA_ID, "dscreen sink sa exit success.");
    _Exit(0);
}

void SetSourceProcessExit()
{
    DHLOGI("set sources process exit.");
    std::lock_guard<std::mutex> autoLock(g_saProcessState);
    g_sourceSAState = DSCREEN_SA_EXIT_STATE_STOP;
    DHLOGI("g_sourceSAState = %d g_sinkSAState = %d", g_sourceSAState, g_sinkSAState);
    if (g_sourceSAState == DSCREEN_SA_EXIT_STATE_START || g_sinkSAState == DSCREEN_SA_EXIT_STATE_START) {
        return;
    }
    DHLOGI("exit sa process success.");
    ReportSaEvent(DSCREEN_EXIT, DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID, "dscreen source sa exit success.");
    _Exit(0);
}
} // namespace DistributedHardware
} // namespace OHOS