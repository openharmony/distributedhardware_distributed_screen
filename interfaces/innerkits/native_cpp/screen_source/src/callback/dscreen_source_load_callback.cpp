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
#include "dscreen_source_load_callback.h"

#include <unistd.h>

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_hisysevent.h"
#include "dscreen_log.h"
#include "dscreen_source_handler.h"

namespace OHOS {
namespace DistributedHardware {
DScreenSourceLoadCallback::DScreenSourceLoadCallback(
    const std::string &params) : params_(params) {}

void DScreenSourceLoadCallback::OnLoadSystemAbilitySuccess(
    int32_t systemAbilityId, const sptr<IRemoteObject> &remoteObject)
{
    DHLOGI("load screen SA success, systemAbilityId:%d, remoteObject result:%s",
        systemAbilityId, (remoteObject != nullptr) ? "true" : "false");
    if (remoteObject == nullptr) {
        DHLOGE("remoteObject is nullptr");
        return;
    }
    DScreenSourceHandler::GetInstance().FinishStartSA(params_, remoteObject);
}

void DScreenSourceLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    DHLOGE("load screen SA failed, systemAbilityId:%d", systemAbilityId);
    ReportSaFail(SA_ERROR, ERR_DH_SCREEN_SA_SINKPROXY_NOT_INIT, DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID,
        "dscreen source OnLoadSystemAbilityFail.");
}
}
}
