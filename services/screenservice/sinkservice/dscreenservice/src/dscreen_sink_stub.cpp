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

#include "dscreen_sink_stub.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_log.h"

namespace OHOS {
namespace DistributedHardware {
DScreenSinkStub::DScreenSinkStub()
{
    memberFuncMap_[INIT_SINK] = &DScreenSinkStub::InitSinkInner;
    memberFuncMap_[RELEASE_SINK] = &DScreenSinkStub::ReleaseSinkInner;
    memberFuncMap_[SUBSCRIBE_DISTRIBUTED_HARDWARE] = &DScreenSinkStub::SubscribeDistributedHardwareInner;
    memberFuncMap_[UNSUBSCRIBE_DISTRIBUTED_HARDWARE] = &DScreenSinkStub::UnsubscribeDistributedHardwareInner;
    memberFuncMap_[DSCREEN_NOTIFY] = &DScreenSinkStub::DScreenNotifyInner;
}

int32_t DScreenSinkStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::u16string desc = DScreenSinkStub::GetDescriptor();
    std::u16string remoteDesc = data.ReadInterfaceToken();
    if (desc != remoteDesc) {
        DHLOGE("DScreenSinkStub::OnRemoteRequest remoteDesc is invalid!");
        return ERR_INVALID_DATA;
    }

    const auto &iter = memberFuncMap_.find(code);
    if (iter == memberFuncMap_.end()) {
        DHLOGE("invalid request code.");
        return ERR_DH_SCREEN_SA_INVALID_IPC_CALL;
    }
    DScreenSinkFunc &func = iter->second;
    return (this->*func)(data, reply, option);
}

int32_t DScreenSinkStub::InitSinkInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::string param = data.ReadString();
    if (param.empty() || param.size() > PARAM_MAX_SIZE) {
        DHLOGE("InitSinkInner error: invalid parameter.");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    int32_t ret = InitSink(param);
    reply.WriteInt32(ret);
    return DH_SUCCESS;
}

int32_t DScreenSinkStub::ReleaseSinkInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    int32_t ret = ReleaseSink();
    reply.WriteInt32(ret);
    return DH_SUCCESS;
}

int32_t DScreenSinkStub::SubscribeDistributedHardwareInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::string dhId = data.ReadString();
    std::string param = data.ReadString();
    if (dhId.empty() || dhId.size() > DID_MAX_SIZE || param.empty() || param.size() > PARAM_MAX_SIZE) {
        DHLOGE("SubscribeDistributedHardwareInner error: invalid parameter.");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    int32_t ret = SubscribeLocalHardware(dhId, param);
    reply.WriteInt32(ret);
    return DH_SUCCESS;
}

int32_t DScreenSinkStub::UnsubscribeDistributedHardwareInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::string dhId = data.ReadString();
    if (dhId.empty() || dhId.size() > DID_MAX_SIZE) {
        DHLOGE("UnsubscribeDistributedHardwareInner error: invalid parameter.");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    int32_t ret = UnsubscribeLocalHardware(dhId);
    reply.WriteInt32(ret);
    return DH_SUCCESS;
}

int32_t DScreenSinkStub::DScreenNotifyInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::string devId = data.ReadString();
    int32_t eventCode = data.ReadInt32();
    std::string eventContent = data.ReadString();
    if (devId.empty() || devId.size() > DID_MAX_SIZE || eventContent.empty() ||
        eventContent.size() > PARAM_MAX_SIZE) {
        DHLOGE("DScreenNotifyInner error: invalid parameter.");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    DScreenNotify(devId, eventCode, eventContent);
    return DH_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS