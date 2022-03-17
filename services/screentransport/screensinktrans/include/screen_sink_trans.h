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

#ifndef OHOS_SCREEN_SINK_TRANS_H
#define OHOS_SCREEN_SINK_TRANS_H

#include <string>
#include <memory>

#include "iscreen_sink_trans.h"
#include "iscreen_sink_trans_callback.h"
#include "iimage_sink_processor.h"
#include "iscreen_channel.h"

namespace OHOS {
namespace DistributedHardware {
class ScreenSinkTrans : public IScreenSinkTrans,
    public IScreenChannelListener,
    public IImageSinkProcessorListener,
    public std::enable_shared_from_this<ScreenSinkTrans> {
public:
    ScreenSinkTrans() = default;
    ~ScreenSinkTrans() = default;

    int32_t SetUp(const VideoParam &localParam, const VideoParam &remoteParam, const std::string &peerDevId) override;
    int32_t Release() override;
    int32_t Start() override;
    int32_t Stop() override;
    int32_t RegisterStateCallback(const std::shared_ptr<IScreenSinkTransCallback> &callBack) override;
    int32_t SetImageSurface(const sptr<Surface> &surface) override;

    void OnSessionOpened() override;
    void OnSessionClosed() override;
    void OnDataReceived(const std::shared_ptr<DataBuffer> &data) override;
    void OnProcessorStateNotify(int32_t state) override;

private:
    int32_t CheckVideoParam(const VideoParam &param);
    int32_t CheckTransParam(const VideoParam &localParam, const VideoParam &remoteParam, const std::string &peerDevId);
    int32_t InitScreenTrans(const VideoParam &localParam, const VideoParam &remoteParam, const std::string &peerDevId);
    int32_t RegisterChannelListener();
    int32_t RegisterProcessorListener(const VideoParam &localParam, const VideoParam &remoteParam,
        const std::string &peerDevId);

private:
    static const constexpr char *LOG_TAG = "ScreenSinkTrans";

    sptr<Surface> decoderSurface_;
    std::shared_ptr<IImageSinkProcessor> imageProcessor_;
    std::shared_ptr<IScreenChannel> screenChannel_;
    std::weak_ptr<IScreenSinkTransCallback> transCallback_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif