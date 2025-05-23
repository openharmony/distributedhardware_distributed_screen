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

#include "dscreen_source_callback_stub.h"

#include <string>

#include "idscreen_source_callback.h"
#include "ipc_types.h"
#include "message_parcel.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_log.h"

namespace OHOS { class MessageOption; }
namespace OHOS {
namespace DistributedHardware {
DScreenSourceCallbackStub::DScreenSourceCallbackStub() {}

int32_t DScreenSourceCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    std::u16string desc = DScreenSourceCallbackStub::GetDescriptor();
    std::u16string remoteDesc = data.ReadInterfaceToken();
    if (desc != remoteDesc) {
        DHLOGE("DScreenSourceCallbackStub::OnRemoteRequest remoteDesc is invalid!");
        return ERR_INVALID_DATA;
    }

    switch (code) {
        case NOTIFY_REG_RESULT:
            return OnNotifyRegResultInner(data, reply, option);
        case NOTIFY_UNREG_RESULT:
            return OnNotifyUnregResultInner(data, reply, option);
        default:
            DHLOGE("invalid request code.");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int32_t DScreenSourceCallbackStub::OnNotifyRegResultInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    (void)reply;
    (void)option;
    std::string devId = data.ReadString();
    std::string dhId = data.ReadString();
    std::string reqId = data.ReadString();
    int32_t status = data.ReadInt32();
    std::string resultDataStr = data.ReadString();
    if (!CheckParams(devId, dhId, reqId, resultDataStr)) {
        DHLOGE("OnNotifyRegResultInner error: Invalid parameter.");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    int32_t ret = OnNotifyRegResult(devId, dhId, reqId, status, resultDataStr);
    return ret;
}

int32_t DScreenSourceCallbackStub::OnNotifyUnregResultInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    (void)reply;
    (void)option;
    std::string devId = data.ReadString();
    std::string dhId = data.ReadString();
    std::string reqId = data.ReadString();
    int32_t status = data.ReadInt32();
    std::string resultData = data.ReadString();
    if (!CheckParams(devId, dhId, reqId, resultData)) {
        DHLOGE("OnNotifyUnregResultInner error: Invalid parameter.");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    int32_t ret = OnNotifyUnregResult(devId, dhId, reqId, status, resultData);
    return ret;
}

bool DScreenSourceCallbackStub::CheckParams(const std::string &devId, const std::string &dhId,
    const std::string &reqId, const std::string &resultData) const
{
    if (devId.empty() || devId.size() > DID_MAX_SIZE || dhId.empty() || dhId.size() > DID_MAX_SIZE) {
        DHLOGE("DScreenSourceCallbackStub CheckParams devId or dhId is invalid.");
        return false;
    }
    if (reqId.empty() || reqId.size() > DID_MAX_SIZE || resultData.empty() || resultData.size() > PARAM_MAX_SIZE) {
        DHLOGE("DScreenSourceCallbackStub CheckParams reqId or resultData is invalid.");
        return false;
    }
    return true;
}
}  // namespace DistributedHardware
}  // namespace OHOS
