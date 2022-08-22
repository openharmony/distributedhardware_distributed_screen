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

#include "dscreen_source_callback_proxy.h"

#include "parcel.h"

#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "dscreen_util.h"

namespace OHOS {
namespace DistributedHardware {
int32_t DScreenSourceCallbackProxy::OnNotifyRegResult(const std::string &devId, const std::string &dhId,
    const std::string &reqId, int32_t status, const std::string &resultData)
{
    DHLOGD("OnNotifyRegResult, devId: %s, dhId: %s, reqId: %s, status: %d",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), reqId.c_str(), status);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DScreenSourceCallbackProxy remote service null");
        return DSCREEN_BAD_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("WriteInterfaceToken failed.");
        return ERR_DH_SCREEN_SA_WRITEINTERFACETOKEN_FAILED;
    }

    if (!data.WriteString(devId) || !data.WriteString(dhId) ||
        !data.WriteString(reqId) || !data.WriteInt32(status) ||
        !data.WriteString(resultData)) {
        DHLOGE("Write param failed.");
        return ERR_DH_SCREEN_SA_WRITEPARAM_FAILED;
    }

    remote->SendRequest(NOTIFY_REG_RESULT, data, reply, option);
    return reply.ReadInt32();
}

int32_t DScreenSourceCallbackProxy::OnNotifyUnregResult(const std::string &devId, const std::string &dhId,
    const std::string &reqId, int32_t status, const std::string &resultData)
{
    DHLOGD("OnNotifyUnregResult, devId: %s, dhId: %s, reqId: %s, status: %d",
        GetAnonyString(devId).c_str(), GetAnonyString(dhId).c_str(), reqId.c_str(), status);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DScreenSourceCallbackProxy remote service null");
        return DSCREEN_BAD_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("WriteInterfaceToken failed.");
        return ERR_DH_SCREEN_SA_WRITEINTERFACETOKEN_FAILED;
    }

    if (!data.WriteString(devId) || !data.WriteString(dhId) ||
        !data.WriteString(reqId) || !data.WriteInt32(status) ||
        !data.WriteString(resultData)) {
        DHLOGE("Write param failed.");
        return ERR_DH_SCREEN_SA_WRITEPARAM_FAILED;
    }

    remote->SendRequest(NOTIFY_UNREG_RESULT, data, reply, option);
    return reply.ReadInt32();
}
} // namespace DistributedHardware
} // namespace OHOS