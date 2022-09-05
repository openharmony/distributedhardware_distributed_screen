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

#ifndef OHOS_DSCREEN_SOURCE_CALLBACK_H
#define OHOS_DSCREEN_SOURCE_CALLBACK_H

#include <cstdint>
#include <map>
#include <string>

#include "idistributed_hardware_source.h"

#include "dscreen_source_callback_stub.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenSourceCallback : public DScreenSourceCallbackStub {
public:
    DScreenSourceCallback() = default;
    ~DScreenSourceCallback() = default;

    int32_t OnNotifyRegResult(const std::string &devId, const std::string &dhId,
        const std::string &reqId, int32_t status, const std::string &data) override;
    int32_t OnNotifyUnregResult(const std::string &devId, const std::string &dhId,
        const std::string &reqId, int32_t status, const std::string &data) override;

    void PushRegRegisterCallback(const std::string &reqId, const std::shared_ptr<RegisterCallback> &callback);
    void PopRegRegisterCallback(const std::string &reqId);
    void PushUnregisterCallback(const std::string &reqId, const std::shared_ptr<UnregisterCallback> &callback);
    void PopUnregisterCallback(const std::string &reqId);

private:
    std::map<std::string, std::shared_ptr<RegisterCallback>> registerCallbackMap_;
    std::map<std::string, std::shared_ptr<UnregisterCallback>> unregisterCallbackMap_;
};
}
}
#endif