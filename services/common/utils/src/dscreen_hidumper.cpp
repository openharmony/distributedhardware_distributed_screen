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

#include "dscreen_hidumper.h"

#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include "dscreen_util.h"

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(DscreenHidumper);

namespace {
const std::string ARGS_HELP = "-h";
const std::string ARGS_DUMP_SCREEN_DATA = "--dump";
const std::string ARGS_DUMP_SCREEN_DATA_RESTART = "--redump";
const std::string ARGS_DUMP_SCREEN_DATA_STOP = "--stopdump";

const std::map<std::string, HidumpFlag> ARGS_MAP = {
    { ARGS_HELP, HidumpFlag::GET_HELP },
    { ARGS_DUMP_SCREEN_DATA, HidumpFlag::DUMP_SCREEN_DATA },
    { ARGS_DUMP_SCREEN_DATA_RESTART, HidumpFlag::DUMP_SCREEN_DATA_RESTART },
    { ARGS_DUMP_SCREEN_DATA_STOP, HidumpFlag::DUMP_SCREEN_DATA_STOP },
};
}

DscreenHidumper::DscreenHidumper()
{
    DHLOGI("Distributed screen hidumper constructed.");
}

DscreenHidumper::~DscreenHidumper()
{
    DHLOGI("Distributed screen hidumper deconstructed.");
}

bool DscreenHidumper::Dump(const std::vector<std::string> &args, std::string &result)
{
    DHLOGI("Distributed screen hidumper dump args.size():%{public}zu.", args.size());
    result.clear();
    int32_t argsSize = static_cast<int32_t>(args.size());
    for (int32_t i = 0; i < argsSize; i++) {
        DHLOGI("Distributed screen hidumper dump args[%{public}d]: %{public}s.", i, args.at(i).c_str());
    }

    if (args.empty()) {
        ShowHelp(result);
        return true;
    }

    if (args.size() > 1) {
        ShowIllegalInfomation(result);
        return true;
    }

    return ProcessDump(args[0], result) == DH_SUCCESS;
}

int32_t DscreenHidumper::ProcessDump(const std::string &args, std::string &result)
{
    DHLOGI("Process dump.");
    HidumpFlag hidumpFlag = HidumpFlag::UNKNOWN;
    auto operatorIter = ARGS_MAP.find(args);
    if (operatorIter != ARGS_MAP.end()) {
        hidumpFlag = operatorIter->second;
    }

    if (hidumpFlag == HidumpFlag::GET_HELP) {
        ShowHelp(result);
        return DH_SUCCESS;
    }
    result.clear();
    switch (hidumpFlag) {
        case HidumpFlag::DUMP_SCREEN_DATA: {
            return DumpScreenData(result);
        }
        case HidumpFlag::DUMP_SCREEN_DATA_RESTART: {
            return ReDumpScreenData(result);
        }
        case HidumpFlag::DUMP_SCREEN_DATA_STOP: {
            SetFlagFalse();
            return DH_SUCCESS;
        }
        default: {
            return ShowIllegalInfomation(result);
        }
    }
}

int32_t DscreenHidumper::DumpScreenData(std::string &result)
{
    DHLOGI("Dump screen data.");

    if (access(DUMP_FILE_PATH.c_str(), 0) < 0) {
        if (mkdir(DUMP_FILE_PATH.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
            DHLOGI("Create dir err.");
            DHLOGI("dir path : %{public}s", DUMP_FILE_PATH.c_str());
            return DSCREEN_BAD_VALUE;
        }
    }

    if (fileFullFlag_ == false) {
        result.append("Dump...");
        hidumperFlag_ = true;
    } else {
        result.append("File oversize 300M : stop dump, use parameter \"--redump\" to clear dumpfile and redump");
    }
    return DH_SUCCESS;
}

int32_t DscreenHidumper::ReDumpScreenData(std::string &result)
{
    DHLOGI("Redump screen data.");

    if (access(DUMP_FILE_PATH.c_str(), 0) < 0) {
        if (mkdir(DUMP_FILE_PATH.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
            DHLOGI("Create dir err.");
            DHLOGI("dir path : %{public}s", DUMP_FILE_PATH.c_str());
            return DSCREEN_BAD_VALUE;
        }
    }
    SetFileFlagFalse();
    SetReDumpFlagTrue();
    result.append("ReDumpStart...");
    hidumperFlag_ = true;
    SetTransReDumpFlagTrue();
    return DH_SUCCESS;
}

bool DscreenHidumper::GetFlagStatus()
{
    return hidumperFlag_;
}

void DscreenHidumper::SetFlagFalse()
{
    hidumperFlag_ = false;
}

bool DscreenHidumper::GetFileFlag()
{
    return fileFullFlag_;
}

void DscreenHidumper::SetFileFlagFalse()
{
    fileFullFlag_ = false;
}

void DscreenHidumper::SetFileFlagTrue()
{
    fileFullFlag_ = true;
}

bool DscreenHidumper::GetReDumpFlag()
{
    return reDumpFlag_;
}

void DscreenHidumper::SetReDumpFlagFalse()
{
    reDumpFlag_ = false;
}

void DscreenHidumper::SetReDumpFlagTrue()
{
    reDumpFlag_ = true;
}

bool DscreenHidumper::GetTransReDumpFlag()
{
    return transReDumpFlag_;
}

void DscreenHidumper::SetTransReDumpFlagFalse()
{
    transReDumpFlag_ = false;
}

void DscreenHidumper::SetTransReDumpFlagTrue()
{
    transReDumpFlag_ = true;
}

void DscreenHidumper::ShowHelp(std::string &result)
{
    DHLOGI("Show help.");
    result.append("Usage:dump  <command> [options]\n")
        .append("Description:\n")
        .append("-h            ")
        .append(": show help\n")
        .append("--dump        ")
        .append(": dump screen data in /data/data/dscreen\n")
        .append("--redump      ")
        .append(": clear file and restart dump screen data\n")
        .append("--stopdump    ")
        .append(": stop dump screen data\n");
}

int32_t DscreenHidumper::ShowIllegalInfomation(std::string &result)
{
    DHLOGI("Show illegal information.");
    result.append("unknown command, -h for help.");
    return DH_SUCCESS;
}

void DscreenHidumper::SaveFile(std::string file, const VideoData &video)
{
    DHLOGE("Saving File.");
    std::string fileName = DUMP_FILE_PATH + "/" + file + std::to_string(video.width) + ")_height(" +
         std::to_string(video.height) + ")_" + video.format + ".jpg";
    DHLOGE("fileName = %{public}s", fileName.c_str());
    if (GetReDumpFlag() == true) {
        std::remove(fileName.c_str());
        SetReDumpFlagFalse();
    }
    std::ofstream ofs(fileName, std::ios::binary | std::ios::out | std::ios::app);

    if (!ofs.is_open()) {
        DHLOGE("open file failed.");
        return;
    }
    DHLOGE("open Hidumper SaveFile file success.");
    ofs.seekp(0, std::ios::end);
    std::ofstream::pos_type fileSize = ofs.tellp();
    if (fileSize < 0) {
        DHLOGE("filesize get err");
        fileSize = 0;
    }
    if ((static_cast<size_t>(fileSize) + video.size) < DUMP_FILE_MAX_SIZE) {
        SetFileFlagFalse();
        ofs.write(reinterpret_cast<const char *>(video.data), video.size);
    } else {
        SetFileFlagTrue();
    }
    ofs.close();
}
} // namespace DistributedHardware
} // namespace OHOS