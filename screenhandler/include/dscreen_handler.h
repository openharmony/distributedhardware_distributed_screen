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

#ifndef OHOS_DSCREEN_HANDLER_H
#define OHOS_DSCREEN_HANDLER_H

#include "screen_manager.h"

#include "ihardware_handler.h"
#include "single_instance.h"

namespace OHOS {
namespace DistributedHardware {
class ScreenListener : public Rosen::ScreenManager::IScreenListener {
public:
    void OnConnect(uint64_t screenId) override;
    void OnDisconnect(uint64_t screenId) override;
    void OnChange(uint64_t screenId) override {};
    uint32_t ByteCalculate(uint32_t screenWidth);
};

class DScreenHandler : public IHardwareHandler {
DECLARE_SINGLE_INSTANCE_BASE(DScreenHandler);
public:
    int32_t Initialize() override;
    std::vector<DHItem> QueryMeta() override;
    std::vector<DHItem> Query() override;
    std::map<std::string, std::string> QueryExtraInfo() override;
    bool IsSupportPlugin() override;
    void RegisterPluginListener(std::shared_ptr<PluginListener> listener) override;
    void UnRegisterPluginListener() override;
    void PluginHardware(const std::string &dhId, const std::string &attr, const std::string &subtype);
    void UnPluginHardware(const std::string &dhId);
    std::string QueryCodecInfo();

private:
    DScreenHandler();
    ~DScreenHandler();

    std::shared_ptr<PluginListener> listener_ = nullptr;
    std::string codecInfoStr_;
    sptr<ScreenListener> screenListener_ = nullptr;
};

#ifdef __cplusplus
extern "C" {
#endif
__attribute__((visibility("default"))) IHardwareHandler* GetHardwareHandler();
#ifdef __cplusplus
}
#endif
} // namespace DistributedHardware
} // namespace OHOS
#endif