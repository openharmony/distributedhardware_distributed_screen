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

#include "dscreen_sink_proxy.h"

#include "iremote_object.h"
#include "message_option.h"
#include "message_parcel.h"

#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "dscreen_util.h"

namespace OHOS {
namespace DistributedHardware {
int32_t DScreenSinkProxy::InitSink(const std::string &params)
{
    if (params.empty() || params.size() > PARAM_MAX_SIZE) {
        DHLOGE("InitSink error: invalid parameter.");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DScreenSinkProxy remote service null");
        return DSCREEN_BAD_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("WriteInterfaceToken failed");
        return ERR_DH_SCREEN_SA_WRITEINTERFACETOKEN_FAILED;
    }

    if (!data.WriteString(params)) {
        DHLOGE("Write param failed.");
        return ERR_DH_SCREEN_SA_WRITEPARAM_FAILED;
    }

    remote->SendRequest(INIT_SINK, data, reply, option);
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t DScreenSinkProxy::ReleaseSink()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DScreenSinkProxy remote service null");
        return DSCREEN_BAD_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("WriteInterfaceToken failed");
        return ERR_DH_SCREEN_SA_WRITEINTERFACETOKEN_FAILED;
    }

    remote->SendRequest(RELEASE_SINK, data, reply, option);
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t DScreenSinkProxy::SubscribeLocalHardware(const std::string &dhId, const std::string &param)
{
    if (dhId.empty() || dhId.size() > DID_MAX_SIZE || param.empty() || param.size() > PARAM_MAX_SIZE) {
        DHLOGE("SubscribeLocalHardware error: invalid parameter.");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DScreenSinkProxy remote service null");
        return DSCREEN_BAD_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("WriteInterfaceToken failed");
        return ERR_DH_SCREEN_SA_WRITEINTERFACETOKEN_FAILED;
    }

    if (!data.WriteString(dhId) || !data.WriteString(param)) {
        DHLOGE("Write param failed.");
        return ERR_DH_SCREEN_SA_WRITEPARAM_FAILED;
    }

    remote->SendRequest(SUBSCRIBE_DISTRIBUTED_HARDWARE, data, reply, option);
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t DScreenSinkProxy::UnsubscribeLocalHardware(const std::string &dhId)
{
    if (dhId.empty() || dhId.size() > DID_MAX_SIZE) {
        DHLOGE("UnsubscribeLocalHardware error: invalid parameter.");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DScreenSinkProxy remote service null");
        return DSCREEN_BAD_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DHLOGE("WriteInterfaceToken failed");
        return ERR_DH_SCREEN_SA_WRITEINTERFACETOKEN_FAILED;
    }

    if (!data.WriteString(dhId)) {
        DHLOGE("Write param failed.");
        return ERR_DH_SCREEN_SA_WRITEPARAM_FAILED;
    }

    remote->SendRequest(UNSUBSCRIBE_DISTRIBUTED_HARDWARE, data, reply, option);
    int32_t ret = reply.ReadInt32();
    return ret;
}

void DScreenSinkProxy::DScreenNotify(const std::string &devId, int32_t eventCode, const std::string &eventContent)
{
    if (devId.empty() || devId.size() > DID_MAX_SIZE || eventContent.empty() ||
        eventContent.size() > PARAM_MAX_SIZE) {
            DHLOGE("DScreenNotify error: invalid parameter.");
            return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        DHLOGE("DScreenSinkProxy remote service null");
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
} // namespace DistributedHardware
} // namespace OHOS