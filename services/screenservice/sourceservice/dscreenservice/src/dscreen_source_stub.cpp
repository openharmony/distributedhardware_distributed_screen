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
    memberFuncMap_[INIT_SOURCE] = &DScreenSourceStub::InitSourceInner;
    memberFuncMap_[RELEASE_SOURCE] = &DScreenSourceStub::ReleaseSourceInner;
    memberFuncMap_[REGISTER_DISTRIBUTED_HARDWARE] = &DScreenSourceStub::RegisterDistributedHardwareInner;
    memberFuncMap_[UNREGISTER_DISTRIBUTED_HARDWARE] = &DScreenSourceStub::UnregisterDistributedHardwareInner;
    memberFuncMap_[CONFIG_DISTRIBUTED_HARDWARE] = &DScreenSourceStub::ConfigDistributedHardwareInner;
    memberFuncMap_[DSCREEN_NOTIFY] = &DScreenSourceStub::DScreenNotifyInner;
}

int32_t DScreenSourceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
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
    std::string param = data.ReadString();
    if (param.empty() || param.size() > PARAM_MAX_SIZE) {
        DHLOGE("InitSourceInner error: invalid parameter");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    sptr<IRemoteObject> remoteObject = data.ReadRemoteObject();
    if (remoteObject == nullptr) {
        DHLOGE("Read param failed.");
        return ERR_DH_SCREEN_SA_READPARAM_FAILED;
    }

    sptr<DScreenSourceCallbackProxy> dScreenSourceCallbackProxy(new DScreenSourceCallbackProxy(remoteObject));
    if (dScreenSourceCallbackProxy == nullptr) {
        DHLOGE("dScreenSourceCallbackProxy is nullptr.");
        return ERR_DH_SCREEN_SA_READPARAM_FAILED;
    }
    int32_t ret = InitSource(param, dScreenSourceCallbackProxy);
    reply.WriteInt32(ret);
    return DH_SUCCESS;
}

int32_t DScreenSourceStub::ReleaseSourceInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    int32_t ret = ReleaseSource();
    reply.WriteInt32(ret);
    return DH_SUCCESS;
}

int32_t DScreenSourceStub::RegisterDistributedHardwareInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::string devId = data.ReadString();
    std::string dhId = data.ReadString();
    std::string version = data.ReadString();
    std::string attrs = data.ReadString();
    std::string reqId = data.ReadString();
    if (!CheckRegParams(devId, dhId, version, attrs, reqId)) {
        DHLOGE("RegisterDistributedHardwareInner error: invalid parameter");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
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
    std::string devId = data.ReadString();
    std::string dhId = data.ReadString();
    std::string reqId = data.ReadString();
    if (!CheckUnregParams(devId, dhId, reqId)) {
        DHLOGE("UnregisterDistributedHardwareInner error: invalid parameter");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }

    int32_t ret = UnregisterDistributedHardware(devId, dhId, reqId);
    reply.WriteInt32(ret);
    return DH_SUCCESS;
}

int32_t DScreenSourceStub::ConfigDistributedHardwareInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::string devId = data.ReadString();
    std::string dhId = data.ReadString();
    std::string key = data.ReadString();
    std::string value = data.ReadString();
    if (!CheckConfigParams(devId, dhId, key, value)) {
        DHLOGE("ConfigDistributedHardwareInner error: invalid parameter");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }

    int32_t ret = ConfigDistributedHardware(devId, dhId, key, value);
    reply.WriteInt32(ret);
    return DH_SUCCESS;
}

int32_t DScreenSourceStub::DScreenNotifyInner(MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::string devId = data.ReadString();
    int32_t eventCode = data.ReadInt32();
    std::string eventContent = data.ReadString();
    if (devId.empty() || devId.size() > DID_MAX_SIZE || eventContent.empty() ||
        eventContent.size() > PARAM_MAX_SIZE) {
        DHLOGE("DScreenNotifyInner error: invalid parameter");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }

    DScreenNotify(devId, eventCode, eventContent);
    return DH_SUCCESS;
}

bool DScreenSourceStub::CheckRegParams(const std::string &devId, const std::string &dhId,
    const std::string &version, const std::string &attrs, const std::string &reqId)
{
    if (devId.empty() || devId.size() > DID_MAX_SIZE || dhId.empty() || dhId.size() > DID_MAX_SIZE) {
        DHLOGE("DScreenSourceStub CheckRegParams devId or dhId is inlvalid.");
        return false;
    }
    if (version.empty() || version.size() > PARAM_MAX_SIZE || attrs.empty() || attrs.size() > PARAM_MAX_SIZE) {
        DHLOGE("DScreenSourceStub CheckRegParams version or attrs is inlvalid.");
        return false;
    }
    if (reqId.empty() || reqId.size() > DID_MAX_SIZE) {
        DHLOGE("DScreenSourceStub CheckRegParams reqId is inlvalid.");
        return false;
    }
    return true;
}

bool DScreenSourceStub::CheckUnregParams(const std::string &devId, const std::string &dhId, const std::string &reqId)
{
    if (devId.empty() || devId.size() > DID_MAX_SIZE || dhId.empty() || dhId.size() > DID_MAX_SIZE) {
        DHLOGE("DScreenSourceStub CheckUnregParams devId or dhId is invalid.");
        return false;
    }
    if (reqId.empty() || reqId.size() > DID_MAX_SIZE) {
        DHLOGE("DScreenSourceStub CheckUnregParams reqId is invalid.");
        return false;
    }
    return true;
}

bool DScreenSourceStub::CheckConfigParams(const std::string &devId, const std::string &dhId,
    const std::string &key, const std::string &value)
{
    if (devId.empty() || devId.size() > DID_MAX_SIZE || dhId.empty() || dhId.size() > DID_MAX_SIZE) {
        DHLOGE("DScreenSourceStub CheckConfigParams devId or dhId is invalid.");
        return false;
    }
    if (key.empty() || key.size() > PARAM_MAX_SIZE || value.empty() || value.size() > PARAM_MAX_SIZE) {
        DHLOGE("DScreenSourceStub CheckConfigParams key or value is invalid.");
        return false;
    }
    return true;
}
} // namespace DistributedHardware
} // namespace OHOS