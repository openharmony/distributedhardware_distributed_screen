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

#include "data_buffer.h"

#include "dscreen_errcode.h"
#include "dscreen_log.h"
#include <new>
#include <securec.h>

namespace OHOS {
namespace DistributedHardware {
DataBuffer::DataBuffer(size_t capacity)
{
    if (capacity != 0) {
        data_ = new (std::nothrow) uint8_t[capacity] {0};
        if (data_ != nullptr) {
            capacity_ = capacity;
        }
    }
    dataType_ = 0;
}

DataBuffer::~DataBuffer()
{
    if (data_ != nullptr) {
        delete [] data_;
        data_ = nullptr;
    }
    dirtyRectVec_.clear();
    dataType_ = 0;
    frameNumber_ = 0;
    capacity_ = 0;
}

size_t DataBuffer::Capacity() const
{
    return capacity_;
}

uint8_t *DataBuffer::Data() const
{
    return data_;
}

void DataBuffer::SetSize(size_t size)
{
    capacity_ = size;
}

void DataBuffer::SetDataType(uint8_t dataType)
{
    dataType_ = dataType;
}

uint8_t DataBuffer::DataType()
{
    return dataType_;
}

void DataBuffer::SetDataNumber(size_t number)
{
    frameNumber_ = number;
}

size_t DataBuffer::DataNumber()
{
    return frameNumber_;
}

void DataBuffer::ResetCapcity(size_t capacity)
{
    DHLOGI("%{public}s: ResetCapcity.", DSCREEN_LOG_TAG);
    if (capacity < capacity_) {
        return;
    }
    delete [] data_;
    data_ = new (std::nothrow) uint8_t[capacity] {0};
    if (data_ == nullptr) {
        capacity_ = 0;
    } else {
        capacity_ = capacity;
    }
}

void DataBuffer::AddData(size_t dataSize, unsigned char* &inputData)
{
    if (inputData == nullptr) {
        return;
    }
    int32_t ret = memcpy_s(data_ + capacity_, dataSize, inputData, dataSize);
    if (ret != EOK) {
        DHLOGE("%{public}s: in AddData memcpy data failed, ret: %{public}" PRId32, DSCREEN_LOG_TAG, ret);
        return;
    }
    capacity_ += dataSize;
}

void DataBuffer::AddDirtyRect(DirtyRect rect)
{
    dirtyRectVec_.push_back(rect);
}

std::vector<DirtyRect> DataBuffer::GetDirtyRectVec()
{
    return dirtyRectVec_;
}

int32_t DataBuffer::GetData(int32_t offset, int32_t datasize, uint8_t* &output)
{
    if (static_cast<unsigned long>(offset + datasize) > capacity_ || output == nullptr) {
        DHLOGE("DataBuffer GetData parameter invalid.");
        return ERR_DH_SCREEN_INPUT_PARAM_INVALID;
    }
    int32_t ret = memcpy_s(output, datasize, data_ + offset, datasize);
    if (ret != EOK) {
        DHLOGE("GetData memcpy data failed, ret: %{public}" PRId32, ret);
        return ret;
    }
    return DH_SUCCESS;
}
} // namespace DistributedHardware
} // namespcae OHOS