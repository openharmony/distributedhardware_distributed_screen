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

#ifndef OHOS_DATA_BUFFER_H
#define OHOS_DATA_BUFFER_H

#include <cstdint>
#include <cstddef>
#include <vector>

#include "dscreen_constants.h"

namespace OHOS {
namespace DistributedHardware {
struct DirtyRect {
    int32_t xPos;
    int32_t yPos;
    int32_t width;
    int32_t height;
    int32_t dirtySize;
};
class DataBuffer {
public:
    explicit DataBuffer(size_t capacity);
    ~DataBuffer();

    size_t Capacity() const;
    uint8_t *Data() const;
    void SetSize(size_t size);
    void SetDataType(uint8_t dataType);
    uint8_t DataType();
    void SetDataNumber(size_t number);
    size_t DataNumber();
    void ResetCapcity(size_t capacity);
    void AddData(size_t dataSize, unsigned char* &inputData);
    void AddDirtyRect(DirtyRect rec);
    std::vector<DirtyRect> GetDirtyRectVec();
    int32_t GetData(int32_t offset, int32_t datasize, uint8_t* &output);
private:
    static const constexpr char *DSCREEN_LOG_TAG = "DataBuffer";
    std::vector<DirtyRect> dirtyRectVec_;
    size_t capacity_ = 0;
    uint8_t *data_ = nullptr;
    uint8_t dataType_ = 0;
    size_t frameNumber_ = 0;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif