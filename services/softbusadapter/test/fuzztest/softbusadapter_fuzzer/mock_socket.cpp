/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "socket.h"

constexpr int32_t DH_SUCCESS = 0;

int Socket(SocketInfo info)
{
    return DH_SUCCESS;
}

int Listen(int32_t socket, const QosTV qos[], uint32_t qosCount, const ISocketListener *listener)
{
    return DH_SUCCESS;
}

int Bind(int32_t socket, const QosTV qos[], uint32_t qosCount, const ISocketListener *listener)
{
    return DH_SUCCESS;
}

void Shutdown(int32_t socket) {}

int SendBytes(int32_t socket, const void *data, uint32_t len)
{
    return DH_SUCCESS;
}

int SendStream(int32_t socket, const StreamData *data, const StreamData *ext, const StreamFrameInfo *param)
{
    return DH_SUCCESS;
}
