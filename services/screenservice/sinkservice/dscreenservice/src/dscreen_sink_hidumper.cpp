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

#include "dscreen_sink_hidumper.h"

#include "dscreen_constants.h"
#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "dscreen_util.h"

#undef DH_LOG_TAG
#define DH_LOG_TAG "DscreenSinkHidumper"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(DscreenSinkHidumper);

namespace {
const std::string ARGS_HELP = "-h";
const std::string ARGS_DUMP_SINK_SCREEN_DATA = "--dump";
const std::string ARGS_DUMP_SINK_SCREEN_DATA_RESTART = "--redump";

const std::map<std::string, HidumpFlag> ARGS_MAP = {
    { ARGS_HELP, HidumpFlag::GET_HELP },
    { ARGS_DUMP_SINK_SCREEN_DATA, HidumpFlag::DUMP_SINK_SCREEN_DATA },
    { ARGS_DUMP_SINK_SCREEN_DATA_RESTART, HidumpFlag::DUMP_SINK_SCREEN_DATA_RESTART },
};
}

DscreenSinkHidumper::DscreenSinkHidumper()
{
    DHLOGI("Distributed screen hidumper constructed.");
}

DscreenSinkHidumper::~DscreenSinkHidumper()
{
    DHLOGI("Distributed screen hidumper deconstructed.");
}

bool DscreenSinkHidumper::Dump(const std::vector<std::string> &args, std::string &result)
{
    DHLOGI("Distributed screen hidumper dump args.size():%d.", args.size());
    result.clear();
    int32_t argsSize = static_cast<int32_t>(args.size());
    for (int32_t i = 0; i < argsSize; i++) {
        DHLOGI("Distributed screen hidumper dump args[%d]: %s.", i, args.at(i).c_str());
    }

    if (args.empty()) {
        ShowHelp(result);
        return true;
    } else if (args.size() > 1) {
        ShowIllegalInfomation(result);
        return true;
    }

    return ProcessDump(args[0], result) == DH_SUCCESS;
}

int32_t DscreenSinkHidumper::ProcessDump(const std::string &args, std::string &result)
{
    DHLOGI("Process dump.");
    HidumpFlag hf = HidumpFlag::UNKNOWN;
    auto operatorIter = ARGS_MAP.find(args);
    if (operatorIter != ARGS_MAP.end()) {
        hf = operatorIter->second;
    }

    if (hf == HidumpFlag::GET_HELP) {
        ShowHelp(result);
        return DH_SUCCESS;
    }
    result.clear();
    switch (hf) {
        case HidumpFlag::DUMP_SINK_SCREEN_DATA: {
            return DumpScreenData(result);
        }
        case HidumpFlag::DUMP_SINK_SCREEN_DATA_RESTART: {
            return ReDumpScreenData(result);
        }
        default: {
            return ShowIllegalInfomation(result);
        }
    }
}

int32_t DscreenSinkHidumper::DumpScreenData(std::string &result)
{
    DHLOGI("Dump screen data.");

    if (access(DUMP_FILE_PATH.c_str(), 0) < 0) {
        if (mkdir(DUMP_FILE_PATH.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
            DHLOGI("Create dir err.");
            DHLOGI("dir path : %s", DUMP_FILE_PATH.c_str());
            return DSCREEN_BAD_VALUE;
        }
    }

    if (FileFullFlag_ == false) {
        result.append("Dump...");
        HidumperFlag_ = true;
    } else {
        result.append("File oversize 300M : stop dump, use parameter \"--redump\" to clear dumpfile and redump");
    }
    return DH_SUCCESS;
}

int32_t DscreenSinkHidumper::ReDumpScreenData(std::string &result)
{
    DHLOGI("Redump screen data.");

    if (access(DUMP_FILE_PATH.c_str(), 0) < 0) {
        if (mkdir(DUMP_FILE_PATH.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
            DHLOGI("Create dir err.");
            DHLOGI("dir path : %s", DUMP_FILE_PATH.c_str());
            return DSCREEN_BAD_VALUE;
        }
    }
    SetFileFlagFalse();
    SetReDumpFlagTrue();
    result.append("ReDumpStart...");
    HidumperFlag_ = true;
    return DH_SUCCESS;
}

bool DscreenSinkHidumper::GetFlagStatus()
{
    return HidumperFlag_;
}

void DscreenSinkHidumper::SetFlagFalse()
{
    HidumperFlag_ = false;
}

bool DscreenSinkHidumper::GetFileFlag()
{
    return FileFullFlag_;
}

void DscreenSinkHidumper::SetFileFlagFalse()
{
    FileFullFlag_ = false;
}

void DscreenSinkHidumper::SetFileFlagTrue()
{
    FileFullFlag_ = true;
}

bool DscreenSinkHidumper::GetReDumpFlag()
{
    return ReDumpFlag_;
}

void DscreenSinkHidumper::SetReDumpFlagFalse()
{
    ReDumpFlag_ = false;
}

void DscreenSinkHidumper::SetReDumpFlagTrue()
{
    ReDumpFlag_ = true;
}

void DscreenSinkHidumper::ShowHelp(std::string &result)
{
    DHLOGI("Show help.");
    result.append("Usage:dump  <command> [options]\n")
        .append("Description:\n")
        .append("-h            ")
        .append(": show help\n")
        .append("--dump        ")
        .append(": dump sink screen data in /data/data/dscreen\n")
        .append("--redump      ")
        .append(": clear file and restart dump sink screen data\n");
}

int32_t DscreenSinkHidumper::ShowIllegalInfomation(std::string &result)
{
    DHLOGI("Show illegal information.");
    result.append("unknown command, -h for help.");
    return DH_SUCCESS;
}
} // namespace DistributedHardware
} // namespace OHOS