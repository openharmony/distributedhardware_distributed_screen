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

#ifndef OHOS_DSCREEN_CONSTANTS_H
#define OHOS_DSCREEN_CONSTANTS_H

#include <cstdint>
#include <string>
#include <unistd.h>

namespace OHOS {
namespace DistributedHardware {
enum RpcEventType {
    NOTIFY_SINK_SETUP = 1,
    NOTIFY_SOURCE_SETUP_RESULT = 2,
};

enum DScreenState {
    DISABLED,
    ENABLED,
    DISABLING,
    ENABLING,
    CONNECTING,
    CONNECTED,
    DISCONNECTING,
};

enum TaskType {
    TASK_ENABLE,
    TASK_DISABLE,
    TASK_CONNECT,
    TASK_DISCONNECT,
};

enum CodecType : uint8_t {
    VIDEO_CODEC_TYPE_VIDEO_H264 = 0,
    VIDEO_CODEC_TYPE_VIDEO_H265 = 1,
    VIDEO_CODEC_TYPE_VIDEO_MPEG4 = 2,
};

enum VideoFormat : uint8_t {
    VIDEO_DATA_FORMAT_YUVI420 = 0,
    VIDEO_DATA_FORMAT_NV12 = 1,
    VIDEO_DATA_FORMAT_NV21 = 2,
    VIDEO_DATA_FORMAT_RGBA8888 = 3,
};

/* Screen package name */
const std::string PKG_NAME = "ohos.dhardware.dscreen";

/* Screen data session name */
const std::string DATA_SESSION_NAME = "ohos.dhardware.dscreen.data";

/* Screen session name max len */
constexpr uint32_t DSCREEN_MAX_SESSION_NAME_LEN = 50;

constexpr uint32_t DSCREEN_MAX_DEVICE_ID_LEN = 100;
/* Screen data received max length */
constexpr uint32_t DSCREEN_MAX_RECV_DATA_LEN = 104857600;

/* Screen max video data width */
constexpr uint32_t DSCREEN_MAX_VIDEO_DATA_WIDTH = 2560;

/* Screen max video data height */
constexpr uint32_t DSCREEN_MAX_VIDEO_DATA_HEIGHT = 2772;

/* Screen max screen data width */
constexpr uint32_t DSCREEN_MAX_SCREEN_DATA_WIDTH = 2560;

/* Screen max screen data height */
constexpr uint32_t DSCREEN_MAX_SCREEN_DATA_HEIGHT = 2772;

/* YUV420 buffer size max size */
constexpr int64_t MAX_YUV420_BUFFER_SIZE = 2560 * 1600 * (3 / 2) * 2;
constexpr int32_t LOG_MAX_LEN = 4096;

constexpr int32_t INVALID_WINDOW_ID = -1;

const std::string DSCREEN_LOG_TITLE_TAG = "DSCREEN";
const std::string DSCREEN_PREFIX = "DISTRIBUTED_SCREEN";
const std::string SCREEN_PREFIX = "SCREEN";
const std::string DSCREEN_VERSION = "1.0";
const std::string DSCREEN_PROCESS_NAME = "dscreen";
const std::string SEPERATOR = "#";
const std::string KEY_VERSION = "screenVersion";
const std::string KEY_DISPLAY_ID = "displayId";
const std::string KEY_SCREEN_ID = "screenId";
const std::string KEY_DISPLAY_RECT = "displayRect";
const std::string KEY_SCREEN_RECT = "screenRect";
const std::string KEY_POINT_START_X = "startX";
const std::string KEY_POINT_START_Y = "startY";
const std::string KEY_WIDTH = "width";
const std::string KEY_HEIGHT = "height";
const std::string KEY_SCREEN_WIDTH = "screenWidth";
const std::string KEY_SCREEN_HEIGHT = "screenHeight";
const std::string KEY_VIDEO_WIDTH = "videoWidth";
const std::string KEY_VIDEO_HEIGHT = "videoHeight";
const std::string KEY_COLOR_FORMAT = "colorFormat";
const std::string KEY_FPS = "fps";
const std::string KEY_CODECTYPE = "codecType";
const std::string SCREEN_CLIENT_WINDOW = "screenClientWindow";
const std::string KEY_DH_ID = "dhId";
const std::string KEY_ERR_CODE = "errCode";
const std::string KEY_ERR_CONTENT = "errContent";
const std::string KEY_VIDEO_PARAM = "videoParam";
const std::string KEY_MAPRELATION = "mapRelation";
const std::string CODEC_NAME_H264 = "OMX_hisi_video_encoder_avc";
const std::string CODEC_NAME_MPEG4 = "avenc_mpeg4";
// Publish massage
const std::string SOURCE_WIN_ID = "sourceWinId";
const std::string SOURCE_DEV_ID = "sourceDevId";
const std::string SINK_DEV_ID = "sinkDevId";
const std::string SOURCE_WIN_WIDTH = "sourceWinWidth";
const std::string SOURCE_WIN_HEIGHT = "sourceWinHeight";
const std::string SINK_SHOW_WIN_ID = "sinkShowWinId";
const std::string SINK_PROJ_SHOW_WIDTH = "sinkProjShowWidth";
const std::string SINK_PROJ_SHOW_HEIGHT = "sinkProjShowHeight";
const std::string SINK_WIN_SHOW_X = "sinkWinShowX";
const std::string SINK_WIN_SHOW_Y = "sinkWinShowY";

constexpr uint32_t MAX_MESSAGE_LEN = 40 * 1024 * 1024;
constexpr float DEFAULT_DENSITY = 2.0;
constexpr int32_t DEFAULT_SCREEN_FLAGS = 0;
constexpr uint32_t DEFAULT_FPS = 30;
constexpr uint8_t DEFAULT_CODECTYPE = VIDEO_CODEC_TYPE_VIDEO_H264;
constexpr uint8_t DEFAULT_VIDEO_FORMAT = VIDEO_DATA_FORMAT_NV12;
constexpr int32_t DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID = 4807;
constexpr int32_t DISTRIBUTED_HARDWARE_SCREEN_SINK_SA_ID = 4808;
constexpr uint64_t SCREEN_ID_INVALID = -1ULL;
constexpr uint64_t SCREEN_ID_DEFAULT = 0;
constexpr int32_t SCREEN_LOADSA_TIMEOUT_MS = 10000;
} // namespace DistributedHardware
} // namespace OHOS
#endif