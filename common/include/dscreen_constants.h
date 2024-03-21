/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

enum DataType : uint8_t {
    VIDEO_FULL_SCREEN_DATA = 0,
    VIDEO_PART_SCREEN_DATA = 1,
};

enum DScreenMsgType : uint32_t {
    START_MIRROR = 0,
    START_MIRROR_SUCCESS = 1,
    START_MIRROR_FAIL = 2,
    STOP_MIRROR = 3,
};

enum DScreenEventType : uint32_t {
    ENGINE_ERROR = 0,
    TRANS_CHANNEL_CLOSED = 1,
};

struct VideoData {
    uint8_t *data;
    size_t size;
    uint32_t width;
    uint32_t height;
    int64_t timestamp;
    std::string format;
};

/* Screen package name */
const std::string PKG_NAME = "ohos.dhardware.dscreen";

/* Screen data session name */
const std::string DATA_SESSION_NAME = "ohos.dhardware.dscreen.data";
const std::string JPEG_SESSION_NAME = "ohos.dhardware.dscreen.jpeg";

/* Screen dump data file path name */
const std::string DUMP_FILE_PATH = "/data/data/dscreen";

/* YUV */
constexpr int32_t YR_PARAM = 66;
constexpr int32_t YG_PARAM = 129;
constexpr int32_t YB_PARAM = 25;
constexpr int32_t UR_PARAM = 38;
constexpr int32_t UG_PARAM = 74;
constexpr int32_t UB_PARAM = 112;
constexpr int32_t VG_PARAM = 94;
constexpr int32_t VB_PARAM = 18;
constexpr int32_t YA_PARAM = 16;
constexpr int32_t UA_PARAM = 128;
constexpr int32_t YUV_PARAM = 255;
constexpr int32_t MOVEBITS = 8;

/* RGBA */
constexpr uint32_t RGBA_CHROMA = 4;
constexpr uint32_t RGB_CHROMA = 3;
constexpr uint32_t ONE = 1;
constexpr uint32_t TWO = 2;
constexpr uint32_t THREE = 3;
constexpr uint32_t ZERO = 0;
constexpr uint32_t ALIGNEDBITS = 32;

/* Screen byte alignment size*/
constexpr uint32_t BYTE_ALIGNMENT = 32;
constexpr uint32_t BYTE_ALIGNMENT_CALCULATION = 31;

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

/* Screen dump data max file size */
constexpr uint32_t DUMP_FILE_MAX_SIZE = 295 * 1024 * 1024;

/* YUV420 buffer size max size */
constexpr int64_t MAX_YUV420_BUFFER_SIZE = 2560 * 1600 * (3 / 2) * 2;
constexpr int32_t DSCREEN_MAX_LEN = 4096;

constexpr int32_t INVALID_WINDOW_ID = -1;
constexpr int32_t STRIDE_ALIGNMENT = 0x8;
constexpr int32_t SURFACE_SYNC_FENCE_TIMEOUT = 100;

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
const std::string KEY_POINT_DIRTY_X = "dirtyX";
const std::string KEY_POINT_DIRTY_Y = "dirtyY";
const std::string KEY_POINT_DIRTY_W = "dirtyW";
const std::string KEY_POINT_DIRTY_H = "dirtyH";
const std::string KEY_POINT_DIRTY_SIZE = "dirtySize";
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
const std::string KEY_DEV_ID = "devId";
const std::string KEY_DH_ID = "dhId";
const std::string KEY_ERR_CODE = "errCode";
const std::string KEY_ERR_CONTENT = "errContent";
const std::string KEY_VIDEO_PARAM = "videoParam";
const std::string KEY_MAPRELATION = "mapRelation";
const std::string KEY_PARTIALREFREAH = "PartialRefresh";
const std::string CODEC_NAME_H264 = "OMX_hisi_video_encoder_avc";
const std::string CODEC_NAME_H265 = "OMX_hisi_video_encoder_hevc";
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
constexpr const char* PARTIAL_REFRESH_PARAM = "persist.distributedhardware.dscreen.partial.refresh.enable";

constexpr int32_t DSCREEN_MIN_VERSION = 1;
constexpr int32_t AV_TRANS_SUPPORTED_VERSION = 3;
constexpr int32_t PARTIAL_REFRESH_ENABLED_VALUE = 1;
constexpr uint32_t MAX_MESSAGES_LEN = 40 * 1024 * 1024;
constexpr float DEFAULT_DENSITY = 2.0;
constexpr int32_t DEFAULT_SCREEN_FLAGS = 0;
constexpr double DEFAULT_FPS = 60.0;
constexpr int32_t DIRTY_MAX_SIZE = 10;
constexpr uint8_t DEFAULT_CODECTYPE = VIDEO_CODEC_TYPE_VIDEO_H264;
constexpr uint8_t DEFAULT_VIDEO_FORMAT = VIDEO_DATA_FORMAT_NV12;
constexpr int32_t DISTRIBUTED_HARDWARE_SCREEN_SOURCE_SA_ID = 4807;
constexpr int32_t DISTRIBUTED_HARDWARE_SCREEN_SINK_SA_ID = 4808;
constexpr uint64_t SCREEN_ID_INVALID = -1ULL;
constexpr uint64_t SCREEN_ID_DEFAULT = 0;
constexpr int32_t SCREEN_LOADSA_TIMEOUT_MS = 10000;
constexpr size_t DID_MAX_SIZE = 256;
constexpr size_t PARAM_MAX_SIZE = 50 * 1024 * 1024;
constexpr uint8_t SESSION_WAIT_SECONDS = 5;
constexpr uint8_t DATA_WAIT_SECONDS = 1;
constexpr size_t DATA_QUEUE_MAX_SIZE = 1000;
constexpr uint32_t DECODE_WAIT_MILLISECONDS = 5000;
constexpr size_t DATA_BUFFER_MAX_SIZE = 10 * 1024 * 1024;
constexpr uint8_t TASK_WAIT_SECONDS = 1;
constexpr int32_t JPEG_QUALITY = 80;
constexpr uint32_t BIT_RATE = 12000000;
constexpr int32_t WAIT_TIMEOUT_MS = 5000;
constexpr uint32_t SYSTEM_PARAM_VALUE_SIZE = 50;
constexpr uint64_t WATCHDOG_INTERVAL_TIME_MS = 20 * 1000;
constexpr uint64_t WATCHDOG_DELAY_TIME_MS = 5 * 1000;
constexpr int32_t INVALID_SESSION_ID = -1;
} // namespace DistributedHardware
} // namespace OHOS
#endif