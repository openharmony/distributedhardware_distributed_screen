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

#include "dscreen_source_stub.h"

#include "iservice_registry.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "dscreen_source_callback_proxy.h"

namespace OHOS {
namespace DistributedHardware {
DScreenSourceStub::DScreenSourceStub()
{
    DHLOGI("DScreenSourceStub construct.");
    memberFuncMap_[INIT_SOURCE] = &DScreenSourceStub::InitSourceInner;
    memberFuncMap_[RELEASE_SOURCE] = &DScreenSourceStub::ReleaseSourceInner;
    memberFuncMap_[REGISTER_DISTRIBUTED_HARDWARE] = &DScreenSourceStub::RegisterDistributedHardwareInner;
    memberFuncMap_[UNREGISTER_DISTRIBUTED_HARDWARE] = &DScreenSourceStub::UnregisterDistributedHardwareInner;
    memberFuncMap_[CONFIG_DISTRIBUTED_HARDWARE] = &DScreenSourceStub::ConfigDistributedHardwareInner;
    memberFuncMap_[DSCREEN_NOTIFY] = &DScreenSourceStub::DScreenNotifyInner;
}

DScreenSourceStub::~DScreenSourceStub()
{
    DHLOGI("DScreenSourceStub deconstruct.");
}

int32_t DScreenSourceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("OnRemoteRequest, code: %d", code);
    std::u16string desc = DScreenSourceStub::GetDescriptor();
    std::u16string remoteDesc = data.ReadInterfaceToken();
    if (desc != remoteDesc) {
        DHLOGE("DScreenSourceStub::OnRemoteRequest remoteDesc is invalid!");
        return ERR_INVALID_DATA;
    }

    std::map<int32_t, DScreenSourceFunc>::iterator iter = memberFuncMap_.find(code);
    if (iter == memberFuncMap_.end()) {
        DHLOGE("invalid request code.");
        return ERR_DH_SCREEN_SA_REQUEST_CODE_INVALID;
    }
    DScreenSourceFunc &func = iter->second;
    return (this->*func)(data, reply, option);
}

int32_t DScreenSourceStub::InitSourceInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("InitSourceInner");
    std::string param = data.ReadString();
    sptr<IRemoteObject> remoteObject = data.ReadRemoteObject();
    if (remoteObject == nullptr) {
        DHLOGE("Read param failed.");
        return ERR_DH_SCREEN_SA_READPARAM_FAILED;
    }

    sptr<DScreenSourceCallbackProxy> dScreenSourceCallbackProxy(new DScreenSourceCallbackProxy(remoteObject));
    int32_t ret = InitSource(param, dScreenSourceCallbackProxy);
    reply.WriteInt32(ret);
    return DH_SUCCESS;
}

int32_t DScreenSourceStub::ReleaseSourceInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("ReleaseSourceInner");
    int32_t ret = ReleaseSource();
    reply.WriteInt32(ret);
    return DH_SUCCESS;
}

int32_t DScreenSourceStub::RegisterDistributedHardwareInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("RegisterDistributedHardwareInner");
    std::string devId = data.ReadString();
    std::string dhId = data.ReadString();
    std::string version = data.ReadString();
    std::string attrs = data.ReadString();
    std::string reqId = data.ReadString();
    EnableParam enableParam;
    enableParam.version = version;
    enableParam.attrs = attrs;

    int32_t ret = RegisterDistributedHardware(devId, dhId, enableParam, reqId);
    reply.WriteInt32(ret);
    return DH_SUCCESS;
}

int32_t DScreenSourceStub::UnregisterDistributedHardwareInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("UnregisterDistributedHardwareInner");
    std::string devId = data.ReadString();
    std::string dhId = data.ReadString();
    std::string reqId = data.ReadString();

    int32_t ret = UnregisterDistributedHardware(devId, dhId, reqId);
    reply.WriteInt32(ret);
    return DH_SUCCESS;
}

int32_t DScreenSourceStub::ConfigDistributedHardwareInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("ConfigDistributedHardwareInner");
    std::string devId = data.ReadString();
    std::string dhId = data.ReadString();
    std::string key = data.ReadString();
    std::string value = data.ReadString();

    int32_t ret = ConfigDistributedHardware(devId, dhId, key, value);
    reply.WriteInt32(ret);
    return DH_SUCCESS;
}

int32_t DScreenSourceStub::DScreenNotifyInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    DHLOGI("DScreenNotifyInner");
    std::string devId = data.ReadString();
    int32_t eventCode = data.ReadInt32();
    std::string eventContent = data.ReadString();

    DScreenNotify(devId, eventCode, eventContent);
    return DH_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS