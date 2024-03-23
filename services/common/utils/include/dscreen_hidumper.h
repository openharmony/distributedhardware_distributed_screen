/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_DISTRIBUTED_SCREEN_HIDUMPER_H
#define OHOS_DISTRIBUTED_SCREEN_HIDUMPER_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cstdio>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "single_instance.h"
#include "dscreen_constants.h"

namespace OHOS {
namespace DistributedHardware {
enum class HidumpFlag {
    UNKNOWN = 0,
    GET_HELP,
    DUMP_SCREEN_DATA,
    DUMP_SCREEN_DATA_RESTART,
    DUMP_SCREEN_DATA_STOP,
};

class DscreenHidumper {
    DECLARE_SINGLE_INSTANCE_BASE(DscreenHidumper);

public:
    bool Dump(const std::vector<std::string> &args, std::string &result);
    bool GetFlagStatus();
    void SetFlagFalse();
    bool GetFileFlag();
    bool GetReDumpFlag();
    void SetFileFlagFalse();
    void SetFileFlagTrue();
    void SetReDumpFlagFalse();
    void SetReDumpFlagTrue();
    bool GetTransReDumpFlag();
    void SetTransReDumpFlagFalse();
    void SetTransReDumpFlagTrue();
    void SaveFile(std::string file, const VideoData &video);
    DscreenHidumper();
    ~DscreenHidumper();

private:
    void ShowHelp(std::string &result);
    int32_t ShowIllegalInfomation(std::string &result);
    int32_t ProcessDump(const std::string &args, std::string &result);

    int32_t DumpScreenData(std::string &result);
    int32_t ReDumpScreenData(std::string &result);

private:
    static const constexpr char *DSCREEN_LOG_TAG = "DscreenHidumper";
    bool hidumperFlag_ = false;
    bool fileFullFlag_ = false;
    bool reDumpFlag_ = false;
    bool transReDumpFlag_ = false;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_SCREEN_HIDUMPER_H