/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_SOFTBUS_ADAPTER
#define OHOS_SOFTBUS_ADAPTER

#include <mutex>
#include <map>
#include <set>

#include "transport/socket.h"
#include "transport/trans_type.h"
#include "single_instance.h"

#include "dscreen_constants.h"
#include "dscreen_log.h"
#include "isoftbus_listener.h"

namespace OHOS {
namespace DistributedHardware {
typedef struct {
    std::string sessionName;
    std::string peerDevId;
} SessionInfo;

class SoftbusAdapter {
    DECLARE_SINGLE_INSTANCE_BASE(SoftbusAdapter);
public:
    int32_t CreateSoftbusSessionServer(const std::string &pkgname, const std::string &sessionName,
        const std::string &peerDevId);
    int32_t RemoveSoftbusSessionServer(const std::string &pkgname, const std::string &sessionName,
        const std::string &peerDevId);
    int32_t OpenSoftbusSession(const std::string &mySessionName, const std::string &peerSessionName,
        const std::string &peerDevId);
    int32_t CloseSoftbusSession(const int32_t sessionId);
    int32_t SendSoftbusBytes(int32_t sessionId, const void *data, int32_t dataLen) const;
    int32_t SendSoftbusStream(int32_t sessionId, const StreamData *data, const StreamData *ext,
        const StreamFrameInfo *param) const;
    int32_t RegisterSoftbusListener(const std::shared_ptr<ISoftbusListener> &listener, const std::string &sessionName,
        const std::string &peerDevId);
    int32_t UnRegisterSoftbusListener(const std::string &sessionName, const std::string &peerDevId);

    int32_t OnSoftbusSessionOpened(int32_t sessionId, PeerSocketInfo info);
    void OnSoftbusSessionClosed(int32_t sessionId, ShutdownReason reason);
    void OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen);
    void OnStreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
        const StreamFrameInfo *frameInfo);
    void OnMessageReceived(int sessionId, const void *data, unsigned int dataLen) const;

private:
    SoftbusAdapter();
    ~SoftbusAdapter();
    std::shared_ptr<ISoftbusListener> &GetSoftbusListenerByName(int32_t sessionId);
    std::shared_ptr<ISoftbusListener> &GetSoftbusListenerById(int32_t sessionId);

private:
    static const constexpr char *DSCREEN_LOG_TAG = "SoftbusAdapter";
    std::mutex listenerMtx_;
    std::mutex idMapMutex_;
    std::mutex serverIdMapMutex_;

    ISocketListener sessListener_;
    /* while can not find the listener in mapListeners_, return nullListener_ point to null ptr. */
    std::map<int32_t, std::string> serverIdMap_;
    std::map<int32_t, std::string> devId2SessIdMap_;
    std::shared_ptr<ISoftbusListener> nullListener_;
    std::map<std::string, std::shared_ptr<ISoftbusListener>> mapListeners_;
    std::map<int32_t, std::shared_ptr<ISoftbusListener>> mapSessListeners_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif