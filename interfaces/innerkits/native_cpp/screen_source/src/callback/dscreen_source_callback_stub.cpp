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

#include "dscreen_source_callback_stub.h"

#include "dscreen_errcode.h"
#include "dscreen_log.h"

namespace OHOS {
namespace DistributedHardware {
DScreenSourceCallbackStub::DScreenSourceCallbackStub()
{
    memberFuncMap_[NOTIFY_REG_RESULT] = &DScreenSourceCallbackStub::OnNotifyRegResultInner;
    memberFuncMap_[NOTIFY_UNREG_RESULT] = &DScreenSourceCallbackStub::OnNotifyUnregResultInner;
}

DScreenSourceCallbackStub::~DScreenSourceCallbackStub()
{
}

int32_t DScreenSourceCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    DHLOGI("OnRemoteRequest, code: %d", code);
    std::u16string desc = DScreenSourceCallbackStub::GetDescriptor();
    std::u16string remoteDesc = data.ReadInterfaceToken();
    if (desc != remoteDesc) {
        DHLOGE("DScreenSourceCallbackStub::OnRemoteRequest remoteDesc is invalid!");
        return ERR_INVALID_DATA;
    }

    std::map<int32_t, DScreenFunc>::iterator iter = memberFuncMap_.find(code);
    if (iter == memberFuncMap_.end()) {
        DHLOGE("invalid request code.");
        return ERR_DH_SCREEN_SA_REQUEST_CODE_INVALID;
    }
    DScreenFunc &func = iter->second;
    return (this->*func)(data, reply, option);
}

int32_t DScreenSourceCallbackStub::OnNotifyRegResultInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::string devId = data.ReadString();
    std::string dhId = data.ReadString();
    std::string reqId = data.ReadString();
    int32_t status = data.ReadInt32();
    std::string resultData = data.ReadString();
    int32_t ret = OnNotifyRegResult(devId, dhId, reqId, status, resultData);
    return ret;
}

int32_t DScreenSourceCallbackStub::OnNotifyUnregResultInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::string devId = data.ReadString();
    std::string dhId = data.ReadString();
    std::string reqId = data.ReadString();
    int32_t status = data.ReadInt32();
    std::string resultData = data.ReadString();
    int32_t ret = OnNotifyUnregResult(devId, dhId, reqId, status, resultData);
    return ret;
}
}
}