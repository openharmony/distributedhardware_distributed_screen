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

#include "dscreen_source_proxy.h"

#include "idscreen_source_callback.h"
#include "iremote_object.h"
#include "message_option.h"
#include "message_parcel.h"

#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "dscreen_util.h"

namespace OHOS {
namespace DistributedHardware {
int32_t DScreenSourceProxy::InitSource(const std::string &params, const sptr<IDScreenSourceCallback> &callback)
{
    if (params.empty() || params.size() > PARAM_MAX_SIZE || callback == nullptr) {
        DHLOGE("InitSource error: invalid parameter");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DScreenSourceProxy remote service null");
        return DSCREEN_BAD_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("WriteInterfaceToken failed");
        return ERR_DH_SCREEN_SA_WRITEINTERFACETOKEN_FAILED;
    }

    if (!data.WriteString(params)
        || !data.WriteRemoteObject(callback->AsObject())) {
        DHLOGE("Write param failed.");
        return ERR_DH_SCREEN_SA_WRITEPARAM_FAILED;
    }

    remote->SendRequest(INIT_SOURCE, data, reply, option);
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t DScreenSourceProxy::ReleaseSource()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DScreenSourceProxy remote service null");
        return DSCREEN_BAD_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("WriteInterfaceToken failed");
        return ERR_DH_SCREEN_SA_WRITEINTERFACETOKEN_FAILED;
    }

    remote->SendRequest(RELEASE_SOURCE, data, reply, option);
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t DScreenSourceProxy::RegisterDistributedHardware(const std::string &devId,
    const std::string &dhId, const EnableParam &param, const std::string &reqId)
{
    if (!CheckRegParams(devId, dhId, param, reqId)) {
        DHLOGE("RegisterDistributedHardware error: invalid parameter");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DScreenSourceProxy remote service null");
        return DSCREEN_BAD_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("WriteInterfaceToken failed");
        return ERR_DH_SCREEN_SA_WRITEINTERFACETOKEN_FAILED;
    }

    if (!data.WriteString(devId) || !data.WriteString(dhId)
        || !data.WriteString(param.version) || !data.WriteString(param.attrs)
        || !data.WriteString(reqId)) {
        DHLOGE("Write param failed.");
        return ERR_DH_SCREEN_SA_WRITEPARAM_FAILED;
    }
    remote->SendRequest(REGISTER_DISTRIBUTED_HARDWARE, data, reply, option);
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t DScreenSourceProxy::UnregisterDistributedHardware(const std::string &devId,
    const std::string &dhId, const std::string &reqId)
{
    if (!CheckUnregParams(devId, dhId, reqId)) {
        DHLOGE("UnregisterDistributedHardware error: invalid parameter");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DScreenSourceProxy remote service null");
        return DSCREEN_BAD_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("WriteInterfaceToken failed");
        return ERR_DH_SCREEN_SA_WRITEINTERFACETOKEN_FAILED;
    }

    if (!data.WriteString(devId) || !data.WriteString(dhId)
        || !data.WriteString(reqId)) {
        DHLOGE("Write param failed.");
        return ERR_DH_SCREEN_SA_WRITEPARAM_FAILED;
    }
    remote->SendRequest(UNREGISTER_DISTRIBUTED_HARDWARE, data, reply, option);
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t DScreenSourceProxy::ConfigDistributedHardware(const std::string &devId,
    const std::string &dhId, const std::string &key, const std::string &value)
{
    if (!CheckConfigParams(devId, dhId, key, value)) {
        DHLOGE("ConfigDistributedHardware error: invalid parameter");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DScreenSourceProxy remote service null");
        return DSCREEN_BAD_VALUE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("WriteInterfaceToken failed");
        return ERR_DH_SCREEN_SA_WRITEINTERFACETOKEN_FAILED;
    }

    if (!data.WriteString(devId) || !data.WriteString(dhId)
        || !data.WriteString(key) || !data.WriteString(value)) {
        DHLOGE("Write param failed.");
        return ERR_DH_SCREEN_SA_WRITEPARAM_FAILED;
    }
    remote->SendRequest(CONFIG_DISTRIBUTED_HARDWARE, data, reply, option);
    int32_t ret = reply.ReadInt32();
    return ret;
}

void DScreenSourceProxy::DScreenNotify(const std::string &devId, int32_t eventCode, const std::string &eventContent)
{
    if (devId.empty() || devId.size() > DID_MAX_SIZE || eventContent.empty() ||
        eventContent.size() > PARAM_MAX_SIZE) {
        DHLOGE("DScreenNotify error: invalid parameter");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DScreenSourceProxy remote service null");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_ASYNC };
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteString(devId) || !data.WriteInt32(eventCode) || !data.WriteString(eventContent)) {
        DHLOGE("Write param failed.");
        return;
    }

    remote->SendRequest(DSCREEN_NOTIFY, data, reply, option);
}

bool DScreenSourceProxy::CheckRegParams(const std::string &devId, const std::string &dhId,
    const EnableParam &param, const std::string &reqId)
{
    if (devId.empty() || devId.size() > DID_MAX_SIZE || dhId.empty() || dhId.size() > DID_MAX_SIZE) {
        DHLOGE("DScreenSourceProxy CheckRegParams devId or dhId is invalid.");
        return false;
    }
    if (reqId.empty() || reqId.size() > DID_MAX_SIZE) {
        DHLOGE("DScreenSourceProxy CheckRegParams reqId is invalid.");
        return false;
    }
    if (param.version.empty() || param.version.size() > PARAM_MAX_SIZE || param.attrs.empty() ||
        param.attrs.size() > PARAM_MAX_SIZE) {
        DHLOGE("DScreenSourceProxy CheckRegParams param is invalid.");
        return false;
    }
    return true;
}

bool DScreenSourceProxy::CheckUnregParams(const std::string &devId, const std::string &dhId, const std::string &reqId)
{
    if (devId.empty() || devId.size() > DID_MAX_SIZE || dhId.empty() || dhId.size() > DID_MAX_SIZE) {
        DHLOGE("DScreenSourceProxy CheckUnregParams devId or dhId is invalid.");
        return false;
    }
    if (reqId.empty() || reqId.size() > DID_MAX_SIZE) {
        DHLOGE("DScreenSourceProxy CheckUnregParams reqId is invalid.");
        return false;
    }
    return true;
}

bool DScreenSourceProxy::CheckConfigParams(const std::string &devId, const std::string &dhId,
    const std::string &key, const std::string &value)
{
    if (devId.empty() || devId.size() > DID_MAX_SIZE || dhId.empty() || dhId.size() > DID_MAX_SIZE) {
        DHLOGE("DScreenSourceProxy CheckConfigParams devId or dhId is invalid.");
        return false;
    }
    if (key.empty() || key.size() > PARAM_MAX_SIZE || value.empty() || value.size() > PARAM_MAX_SIZE) {
        DHLOGE("DScreenSourceProxy CheckConfigParams key or value is invalid.");
        return false;
    }
    return true;
}
}
}
