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

#ifndef DISTRIBUTED_SCREEN_TEST_SA_H
#define DISTRIBUTED_SCREEN_TEST_SA_H

#include <climits>
#include <fcntl.h>
#include <iostream>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

#include "idistributed_hardware_source.h"

namespace OHOS {
namespace DistributedHardware {
class DScreenMockRegisterCallback : public RegisterCallback {
public:
    virtual ~DScreenMockRegisterCallback() = default;
    int32_t OnRegisterResult(const std::string &devId, const std::string &dhId, int32_t status, const std::string &data)
    {
        std::cout << "Register devId: " << devId << " dhId: " << dhId << ", status: " << status << std::endl;
        return 0;
    }
};

class DScreenMockUnRegisterCallback : public UnregisterCallback {
public:
    virtual ~DScreenMockUnRegisterCallback() = default;
    int32_t OnUnregisterResult(const std::string &devId, const std::string &dhId, int32_t status, const std::string &data)
    {
        std::cout << "Unregister devId: " << devId << " dhId: " << dhId << ", status: " << status << std::endl;
        return 0;
    }
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // DISTRIBUTED_SCREEN_TEST_SA_H