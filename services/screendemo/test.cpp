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

#include <iostream>

#include "display.h"
#include "display_manager.h"
#include "dscreen_source_handler.h"
#include "dscreen_sink_handler.h"
#include "dscreen_util.h"
#include "idistributed_hardware_sink.h"
#include "idistributed_hardware_source.h"
#include "screen.h"
#include "screen_client.h"
#include "screen_client_common.h"
#include "screen_manager.h"
#include "wm_common.h"
#include "window.h"
#include "window_option.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

#include "decoder_demo.h"
#include "softbus_adapter_mem.h"
#include "softbus_bus_center.h"
#include "softbus_common.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::DistributedHardware;
using namespace OHOS::Rosen;
using namespace OHOS::Media;

namespace {
    static char const *g_pkgName = "ohos.dsoftbus.tool";
    const uint32_t MAX_WINDOW_WIDTH = 2560;
    const uint32_t MAX_WINDOW_HEIGHT = 1600;
}

vector<sptr<Screen>> QueryRemoteScreenInfo()
{
    vector<sptr<Screen>> allScreens = ScreenManager::GetInstance().GetAllScreens();
    sptr<Display> defaultDisplay = DisplayManager::GetInstance().GetDefaultDisplay();
    vector<sptr<Screen>> remoteScreens;
    for (const auto &screen : allScreens) {
        if (screen == nullptr) {
            continue;
        }
        if (!screen->IsReal() && screen->GetWidth() > 0) {
            remoteScreens.push_back(screen);
        }
    }
    cout << "-------------remote screen info---------------" << endl;
    cout << "remote screen Num: " << remoteScreens.size() << endl;
    for (const auto &screen : remoteScreens) {
        if (screen == nullptr) {
            continue;
        }
        cout << endl;
        cout << "--------screen id " << screen->GetId() << "---------" << endl;
        cout << "screen name: " << GetAnonyString(screen->GetName()).c_str() << endl;
        cout << "width: " << screen->GetWidth() << endl;
        cout << "height: " << screen->GetHeight() << endl;
        cout << "-------------------------------------------" << endl;
    }
    return remoteScreens;
}

static void StartMirror()
{
    vector<sptr<Screen>> remoteScreens = QueryRemoteScreenInfo();
    if (remoteScreens.size() == 0) {
        cout << "Error: no remote screens enabled" << endl;
        return;
    }

    cout << "select remote screen id to mirror: " << endl;
    uint64_t mirrorId;
    cin >> mirrorId;

    bool isMirrorIdValid = false;
    for (const auto &screen : remoteScreens) {
        if (screen == nullptr) {
            continue;
        }
        if (screen->GetId() == mirrorId) {
            isMirrorIdValid = true;
            break;
        }
    }

    if (!isMirrorIdValid) {
        cout << "input mirrorId is not valid!" << endl;
        return;
    }

    sptr<Display> defaultDisplay = DisplayManager::GetInstance().GetDefaultDisplay();
    cout << "------------start mirror----------" << endl;
    cout << "mirror screen Id is " << mirrorId << endl;
    vector<uint64_t> mirrorIds;
    mirrorIds.push_back(mirrorId);
    ScreenManager::GetInstance().MakeMirror(defaultDisplay->GetScreenId(), mirrorIds);
}

static void StopMirror()
{
    vector<sptr<Screen>> remoteScreens = QueryRemoteScreenInfo();
    if (remoteScreens.size() == 0) {
        cout << "no remote screens enabled, no need stop mirror" << endl;
        return;
    }

    bool isStopMirrorIdValid = false;
    cout << "select remote screen id to stop mirror: " << endl;
    uint64_t stopMirrorId;
    cin >> stopMirrorId;

    for (const auto &screen : remoteScreens) {
        if (screen == nullptr) {
            continue;
        }
        if (screen->GetId() == stopMirrorId) {
            isStopMirrorIdValid = true;
            break;
        }
    }
    if (!isStopMirrorIdValid) {
        cout << "input screenId is not valid!" << endl;
        return;
    }

    cout << "-------------- stop mirror ------------" << endl;
    cout << "stop mirror screen id is " << stopMirrorId << endl;
    vector<uint64_t> stopMirrorIds;
    stopMirrorIds.push_back(stopMirrorId);
    ScreenManager::GetInstance().RemoveVirtualScreenFromGroup(stopMirrorIds);
}

static void StartExpand()
{
    vector<sptr<Screen>> remoteScreens = QueryRemoteScreenInfo();
    if (remoteScreens.size() == 0) {
        cout << "Error: no remote screens enabled" << endl;
        return;
    }

    cout << "select remote screen id to expand: " << endl;
    uint64_t expandId;
    cin >> expandId;

    bool isExpandIdValid = false;
    for (const auto &screen : remoteScreens) {
        if (screen == nullptr) {
            continue;
        }
        if (screen->GetId() == expandId) {
            isExpandIdValid = true;
            break;
        }
    }

    if (!isExpandIdValid) {
        cout << "input expandId is not valid!" << endl;
        return;
    }

    sptr<Display> defaultDisplay = DisplayManager::GetInstance().GetDefaultDisplay();
    cout << endl << "------------start expand----------" << endl;
    cout << "expand screen Id is " << expandId << endl;
    vector<ExpandOption> options = {{defaultDisplay->GetScreenId(), 0, 0}, {expandId, defaultDisplay->GetWidth(), 0}};
    ScreenManager::GetInstance().MakeExpand(options);
}

static void StopExpand()
{
    vector<sptr<Screen>> remoteScreens = QueryRemoteScreenInfo();
    if (remoteScreens.size() == 0) {
        cout << "no remote screens enabled, no need stop expand" << endl;
        return;
    }

    bool isStopExpandIdValid = false;
    cout << "select remote screen id to stop expand: " << endl;
    uint64_t stopExpandId;
    cin >> stopExpandId;

    for (const auto &screen : remoteScreens) {
        if (screen == nullptr) {
            continue;
        }
        if (screen->GetId() == stopExpandId) {
            isStopExpandIdValid = true;
            break;
        }
    }
    if (!isStopExpandIdValid) {
        cout << "input screenId is not valid!" << endl;
        return;
    }

    cout << "-------------- stop expand ------------" << endl;
    cout << "stop expand screen id is " << stopExpandId << endl;
    vector<uint64_t> stopExpandIds;
    stopExpandIds.push_back(stopExpandId);
    ScreenManager::GetInstance().RemoveVirtualScreenFromGroup(stopExpandIds);
}

static void PrintNodeProperty(NodeBasicInfo *nodeInfo)
{
    if (nodeInfo == nullptr) {
        cout << "nodeInfo is nullptr" << endl;
        return;
    }

    printf("DeviceName = %s\n", nodeInfo->deviceName);
    printf("NetworkId = %s\n", GetAnonyString(nodeInfo->networkId).c_str());
    NodeDeviceInfoKey key = NODE_KEY_UDID;
    unsigned char udid[UDID_BUF_LEN] = {0};
    if (GetNodeKeyInfo(g_pkgName, nodeInfo->networkId, key, udid, UDID_BUF_LEN) != 0) {
        printf("GetNodeKeyInfo Fail!\n");
    } else {
        printf("Udid = %s\n", GetAnonyString(reinterpret_cast<char *>(udid)).c_str());
    }
    key = NODE_KEY_UUID;
    unsigned char uuid[UUID_BUF_LEN] = {0};
    if (GetNodeKeyInfo(g_pkgName, nodeInfo->networkId, key, uuid, UUID_BUF_LEN) != 0) {
        printf("GetNodeKeyInfo Fail!\n");
    } else {
        printf("Uuid = %s\n", GetAnonyString(reinterpret_cast<char *>(udid)).c_str());
    }
}

static void QueryRemoteDeviceInfo()
{
    uint64_t tokenId;
    const char **perms = new const char *[1];
    perms[0] = "ohos.permission.DISTRIBUTED_DATASYNC";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .dcaps = nullptr,
        .perms = perms,
        .aplStr = "system_basic",
    };

    infoInstance.acls = NULL;
    infoInstance.aclsNum = 0;
    infoInstance.processName = "test_attest";
    tokenId = GetAccessTokenId(&infoInstance);
    cout << "tokenId ====== " << tokenId << endl;
    int32_t ret = SetSelfTokenID(tokenId);
    cout << "ret ======" << ret << endl;

    NodeBasicInfo localNodeinfo;
    NodeBasicInfo *remoteNodeInfo = nullptr;
    int32_t infoNum = 0;
    printf("-----------Local Device Info------\n");
    if (GetLocalNodeDeviceInfo(g_pkgName, &localNodeinfo) != 0) {
        printf("LnnGetLocalNodeInfo Fail!\n");
        return;
    }
    PrintNodeProperty(&localNodeinfo);
    printf("-------Remote Device info---------\n");
    if (GetAllNodeDeviceInfo(g_pkgName, &remoteNodeInfo, &infoNum) != 0) {
        printf("GetAllNodeDeviceInfo Fail!\n");
        return;
    }
    printf("Device Num = %d\n", infoNum);
    for (int i = 0; i < infoNum; ++i) {
        printf("\n[No.%d]", i + 1);
        PrintNodeProperty(remoteNodeInfo + i);
    }
    FreeNodeInfo(remoteNodeInfo);
    printf("SoftBusDumpDeviceInfo complete!\n");
}

static void CreateWindow()
{
    cout << "create window, please input window size" << endl;
    cout << "width: ";
    uint32_t windowWidth;
    cin >> windowWidth;
    cout << "height: ";
    uint32_t windowHeight;
    cin >> windowHeight;

    if (windowWidth == 0 || windowWidth >= MAX_WINDOW_WIDTH ||
        windowHeight == 0 || windowHeight >= MAX_WINDOW_HEIGHT) {
        cout << "Invalid window size." << endl;
        return;
    }

    sptr<Display> defaultDisplay = DisplayManager::GetInstance().GetDefaultDisplay();
    shared_ptr<WindowProperty> windowProperty = make_shared<WindowProperty>();
    windowProperty->displayId = defaultDisplay->GetId();
    windowProperty->startX = 0;
    windowProperty->startY = 0;
    windowProperty->width = windowWidth;
    windowProperty->height = windowHeight;
    int32_t windowId = ScreenClient::GetInstance().AddWindow(windowProperty);
    ScreenClient::GetInstance().ShowWindow(windowId);
    sptr<Surface> surface = ScreenClient::GetInstance().GetSurface(windowId);
    cout << "create window success." << endl;

    auto vdec = make_shared<VDecDemo>();

    vdec->SetWindowSize(windowWidth, windowHeight);
    vdec->SetOutputSurface(surface);
    cout << "start run decoder" << endl;
    vdec->RunCase();
    cout << "create window success, window id: " << windowId
         << ", width: " << windowWidth
         << ", height: " << windowHeight << endl;
    ScreenClient::GetInstance().RemoveWindow(windowId);
    _Exit(0);
}

int main()
{
    cout << "Please select a test scenario number(default StartMirror): " << endl;
    cout << "0:StartMirror" << endl;
    cout << "1:StopMirror" << endl;
    cout << "2:StartExpand" << endl;
    cout << "3:StopExpand" << endl;
    cout << "4:CreateWindow" << endl;
    cout << "5:QueryRemoteDeviceInfo" << endl;
    cout << "6:QueryRemoteScreenInfo" << endl;
    string mode;
    (void)getline(cin, mode);
    if (mode == "" || mode == "0") {
        (void)StartMirror();
    } else if (mode == "1") {
        (void)StopMirror();
    } else if (mode == "2") {
        (void)StartExpand();
    } else if (mode == "3") {
        (void)StopExpand();
    } else if (mode == "4") {
        (void)CreateWindow();
    } else if (mode == "5") {
        (void)QueryRemoteDeviceInfo();
    } else if (mode == "6") {
        (void)QueryRemoteScreenInfo();
    } else {
        cout << "no that selection" << endl;
    }
    return 0;
}
