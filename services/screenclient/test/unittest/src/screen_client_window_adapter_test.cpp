/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "screen_client_window_adapter_test.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void ScreenClientWindowAdapterTest::SetUpTestCase(void) {}

void ScreenClientWindowAdapterTest::TearDownTestCase(void) {}

void ScreenClientWindowAdapterTest::SetUp() {}

void ScreenClientWindowAdapterTest::TearDown() {}

/**
 * @tc.name: CreateWindow_001
 * @tc.desc: Verify the CreateWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientWindowAdapterTest, CreateWindow_001, TestSize.Level1)
{
    std::shared_ptr<WindowProperty> windowProperty = nullptr;
    EXPECT_EQ(nullptr, ScreenClientWindowAdapter::GetInstance().CreateWindow(windowProperty, 0));
}

/**
 * @tc.name: CreateWindow_002
 * @tc.desc: Verify the CreateWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientWindowAdapterTest, CreateWindow_002, TestSize.Level1)
{
    std::shared_ptr<WindowProperty> windowProperty = std::make_shared<WindowProperty>();
    int32_t windowId = 0;
    sptr<Surface> actualSurface = ScreenClientWindowAdapter::GetInstance().CreateWindow(windowProperty, windowId);
    EXPECT_NE(nullptr, actualSurface);
    int32_t ret = ScreenClientWindowAdapter::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: ShowWindow_001
 * @tc.desc: Verify the ShowWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientWindowAdapterTest, ShowWindow_001, TestSize.Level1)
{
    int32_t ret = ScreenClientWindowAdapter::GetInstance().ShowWindow(100);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_SHOW_WINDOW_ERROR, ret);
}

/**
 * @tc.name: ShowWindow_002
 * @tc.desc: Verify the ShowWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientWindowAdapterTest, ShowWindow_002, TestSize.Level1)
{
    std::shared_ptr<WindowProperty> windowProperty = std::make_shared<WindowProperty>();
    int32_t windowId = 100;
    ScreenClientWindowAdapter::GetInstance().CreateWindow(windowProperty, windowId);
    int32_t ret = ScreenClientWindowAdapter::GetInstance().ShowWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, ret);
    ret = ScreenClientWindowAdapter::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: ShowWindow_003
 * @tc.desc: Verify the ShowWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientWindowAdapterTest, ShowWindow_003, TestSize.Level1)
{
    int32_t windowId = 0;
    ScreenClientWindowAdapter::GetInstance().windowIdMap_.emplace(windowId, nullptr);
    int32_t ret = ScreenClientWindowAdapter::GetInstance().ShowWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_SHOW_WINDOW_ERROR, ret);
    ret = ScreenClientWindowAdapter::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR, ret);
}
/**
 * @tc.name: HideWindow_001
 * @tc.desc: Verify the HideWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientWindowAdapterTest, HideWindow_001, TestSize.Level1)
{
    int32_t ret = ScreenClientWindowAdapter::GetInstance().HideWindow(0);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_HIDE_WINDOW_ERROR, ret);
}

/**
 * @tc.name: HideWindow_001
 * @tc.desc: Verify the HideWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientWindowAdapterTest, HideWindow_002, TestSize.Level1)
{
    std::shared_ptr<WindowProperty> windowProperty = std::make_shared<WindowProperty>();
    int32_t windowId = 0;
    ScreenClientWindowAdapter::GetInstance().CreateWindow(windowProperty, windowId);
    int32_t ret = ScreenClientWindowAdapter::GetInstance().HideWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, ret);
    ret = ScreenClientWindowAdapter::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: HideWindow_003
 * @tc.desc: Verify the HideWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientWindowAdapterTest, HideWindow_003, TestSize.Level1)
{
    int32_t windowId = 0;
    ScreenClientWindowAdapter::GetInstance().windowIdMap_.emplace(windowId, nullptr);
    int32_t ret = ScreenClientWindowAdapter::GetInstance().HideWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_HIDE_WINDOW_ERROR, ret);
    ret = ScreenClientWindowAdapter::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR, ret);
}

/**
 * @tc.name: MoveWindow_001
 * @tc.desc: Verify the MoveWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientWindowAdapterTest, MoveWindow_001, TestSize.Level1)
{
    int32_t ret = ScreenClientWindowAdapter::GetInstance().MoveWindow(0, 0, 0);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_MOVE_WINDOW_ERROR, ret);
}

/**
 * @tc.name: MoveWindow_002
 * @tc.desc: Verify the MoveWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientWindowAdapterTest, MoveWindow_002, TestSize.Level1)
{
    std::shared_ptr<WindowProperty> windowProperty = std::make_shared<WindowProperty>();
    int32_t windowId = 0;
    ScreenClientWindowAdapter::GetInstance().CreateWindow(windowProperty, windowId);
    int32_t ret = ScreenClientWindowAdapter::GetInstance().MoveWindow(windowId, 0, 0);
    EXPECT_EQ(DH_SUCCESS, ret);
    ret = ScreenClientWindowAdapter::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: MoveWindow_003
 * @tc.desc: Verify the MoveWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientWindowAdapterTest, MoveWindow_003, TestSize.Level1)
{
    int32_t windowId = 0;
    ScreenClientWindowAdapter::GetInstance().windowIdMap_.emplace(windowId, nullptr);
    int32_t ret = ScreenClientWindowAdapter::GetInstance().MoveWindow(windowId, 0, 0);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_MOVE_WINDOW_ERROR, ret);
    ret = ScreenClientWindowAdapter::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR, ret);
}

/**
 * @tc.name: RemoveWindow_001
 * @tc.desc: Verify the RemoveWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientWindowAdapterTest, RemoveWindow_001, TestSize.Level1)
{
    int32_t ret = ScreenClientWindowAdapter::GetInstance().RemoveWindow(0);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR, ret);
}

/**
 * @tc.name: RemoveWindow_002
 * @tc.desc: Verify the RemoveWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientWindowAdapterTest, RemoveWindow_002, TestSize.Level1)
{
    std::shared_ptr<WindowProperty> windowProperty = std::make_shared<WindowProperty>();
    int32_t windowId = 0;
    ScreenClientWindowAdapter::GetInstance().CreateWindow(windowProperty, windowId);
    int32_t ret = ScreenClientWindowAdapter::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: RemoveWindow_003
 * @tc.desc: Verify the RemoveWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientWindowAdapterTest, RemoveWindow_003, TestSize.Level1)
{
    int32_t windowId = 0;
    ScreenClientWindowAdapter::GetInstance().windowIdMap_.emplace(windowId, nullptr);
    int32_t ret = ScreenClientWindowAdapter::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR, ret);
}

/**
 * @tc.name: DestroyAllWindow_001
 * @tc.desc: Verify the DestroyAllWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientWindowAdapterTest, DestroyAllWindow_001, TestSize.Level1)
{
    ScreenClientWindowAdapter::GetInstance().windowIdMap_.emplace(0, nullptr);
    int32_t ret = ScreenClientWindowAdapter::GetInstance().DestroyAllWindow();
    EXPECT_EQ(DH_SUCCESS, ret);
}
} // DistributedHardware
} // OHOS