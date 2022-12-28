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

#define private public
#include "screen_client_test.h"
#undef private

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
void ScreenClientTest::SetUpTestCase(void) {}

void ScreenClientTest::TearDownTestCase(void) {}

void ScreenClientTest::SetUp() {}

void ScreenClientTest::TearDown() {}

/**
 * @tc.name: AddWindow_001
 * @tc.desc: Verify the AddWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, AddWindow_001, TestSize.Level1)
{
    std::shared_ptr<WindowProperty> windowProperty = nullptr;
    int32_t actual = ScreenClient::GetInstance().AddWindow(windowProperty);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_ADD_WINDOW_ERROR, actual);
}

/**
 * @tc.name: AddWindow_002
 * @tc.desc: Verify the AddWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, AddWindow_002, TestSize.Level1)
{
    std::shared_ptr<WindowProperty> windowProperty = std::make_shared<WindowProperty>();
    int32_t windowId = 0;
    int32_t actual = ScreenClient::GetInstance().AddWindow(windowProperty);
    EXPECT_EQ(windowId, actual);
    ScreenClient::GetInstance().RemoveWindow(actual);
}

/**
 * @tc.name: ShowWindow_001
 * @tc.desc: Verify the ShowWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, ShowWindow_001, TestSize.Level1)
{
    int32_t windowId = 0;
    int32_t actual = ScreenClient::GetInstance().ShowWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_SHOW_WINDOW_ERROR, actual);
}

/**
 * @tc.name: ShowWindow_002
 * @tc.desc: Verify the ShowWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, ShowWindow_002, TestSize.Level1)
{
    std::shared_ptr<WindowProperty> windowProperty = std::make_shared<WindowProperty>();
    int32_t windowId = ScreenClient::GetInstance().AddWindow(windowProperty);
    int32_t actual = ScreenClient::GetInstance().ShowWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, actual);
    ScreenClient::GetInstance().RemoveWindow(windowId);
}

/**
 * @tc.name: ShowWindow_003
 * @tc.desc: Verify the ShowWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, ShowWindow_003, TestSize.Level1)
{
    int32_t windowId = 100;
    sptr<Surface> surface = nullptr;
    ScreenClient::GetInstance().surfaceMap_.emplace(windowId, surface);
    int32_t actual = ScreenClient::GetInstance().ShowWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_SHOW_WINDOW_ERROR, actual);
    ScreenClient::GetInstance().RemoveWindow(windowId);
}

/**
 * @tc.name: HideWindow_001
 * @tc.desc: Verify the HideWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, HideWindow_001, TestSize.Level1)
{
    int32_t windowId = 0;
    int32_t actual = ScreenClient::GetInstance().HideWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_HIDE_WINDOW_ERROR, actual);
}

/**
 * @tc.name: HideWindow_002
 * @tc.desc: Verify the HideWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, HideWindow_002, TestSize.Level1)
{
    std::shared_ptr<WindowProperty> windowProperty = std::make_shared<WindowProperty>();
    int32_t windowId = ScreenClient::GetInstance().AddWindow(windowProperty);
    int32_t actual = ScreenClient::GetInstance().HideWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, actual);
    ScreenClient::GetInstance().RemoveWindow(windowId);
}

/**
 * @tc.name: HideWindow_003
 * @tc.desc: Verify the HideWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, HideWindow_003, TestSize.Level1)
{
    int32_t windowId = 0;
    sptr<Surface> surface = nullptr;
    ScreenClient::GetInstance().surfaceMap_.emplace(windowId, surface);
    int32_t actual = ScreenClient::GetInstance().HideWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_HIDE_WINDOW_ERROR, actual);
    ScreenClient::GetInstance().RemoveWindow(windowId);
}

/**
 * @tc.name: MoveWindow_001
 * @tc.desc: Verify the MoveWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, MoveWindow_001, TestSize.Level1)
{
    int32_t windowId = 0;
    int32_t startX = 0;
    int32_t startY = 0;
    int32_t actual = ScreenClient::GetInstance().MoveWindow(windowId, startX, startY);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_MOVE_WINDOW_ERROR, actual);
}

/**
 * @tc.name: MoveWindow_002
 * @tc.desc: Verify the MoveWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, MoveWindow_002, TestSize.Level1)
{
    std::shared_ptr<WindowProperty> windowProperty = std::make_shared<WindowProperty>();
    int32_t windowId = ScreenClient::GetInstance().AddWindow(windowProperty);
    int32_t startX = 0;
    int32_t startY = 0;
    int32_t actual = ScreenClient::GetInstance().MoveWindow(windowId, startX, startY);
    EXPECT_EQ(DH_SUCCESS, actual);
    ScreenClient::GetInstance().RemoveWindow(windowId);
}

/**
 * @tc.name: MoveWindow_003
 * @tc.desc: Verify the MoveWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, MoveWindow_003, TestSize.Level1)
{
    int32_t windowId = 0;
    sptr<Surface> surface = nullptr;
    ScreenClient::GetInstance().surfaceMap_.emplace(windowId, surface);
    int32_t startX = 0;
    int32_t startY = 0;
    int32_t actual = ScreenClient::GetInstance().MoveWindow(windowId, startX, startY);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_MOVE_WINDOW_ERROR, actual);
    ScreenClient::GetInstance().RemoveWindow(windowId);
}

/**
 * @tc.name: RemoveWindow_001
 * @tc.desc: Verify the RemoveWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, RemoveWindow_001, TestSize.Level1)
{
    int32_t windowId = 0;
    int32_t actual = ScreenClient::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR, actual);
}

/**
 * @tc.name: RemoveWindow_002
 * @tc.desc: Verify the RemoveWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, RemoveWindow_002, TestSize.Level1)
{
    std::shared_ptr<WindowProperty> windowProperty = std::make_shared<WindowProperty>();
    int32_t windowId = ScreenClient::GetInstance().AddWindow(windowProperty);
    int32_t actual = ScreenClient::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: RemoveWindow_002
 * @tc.desc: Verify the RemoveWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, RemoveWindow_003, TestSize.Level1)
{
    int32_t windowId = 0;
    sptr<Surface> surface = nullptr;
    ScreenClient::GetInstance().surfaceMap_.emplace(windowId, surface);
    int32_t actual = ScreenClient::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR, actual);
    ScreenClient::GetInstance().RemoveWindow(windowId);
}

/**
 * @tc.name: GetSurface_001
 * @tc.desc: Verify the GetSurface function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, GetSurface_001, TestSize.Level1)
{
    int32_t windowId = 0;
    sptr<Surface> actualSurface = ScreenClient::GetInstance().GetSurface(windowId);
    EXPECT_EQ(nullptr, actualSurface);
}

/**
 * @tc.name: GetSurface_002
 * @tc.desc: Verify the GetSurface function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, GetSurface_002, TestSize.Level1)
{
    std::shared_ptr<WindowProperty> windowProperty = std::make_shared<WindowProperty>();
    int32_t windowId = ScreenClient::GetInstance().AddWindow(windowProperty);
    sptr<Surface> actualSurface = ScreenClient::GetInstance().GetSurface(windowId);
    EXPECT_NE(nullptr, actualSurface);
    ScreenClient::GetInstance().RemoveWindow(windowId);
}

/**
 * @tc.name: DestroyAllWindow_001
 * @tc.desc: Verify the DestroyAllWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, DestroyAllWindow_001, TestSize.Level1)
{
    std::shared_ptr<WindowProperty> windowProperty = std::make_shared<WindowProperty>();
    ScreenClient::GetInstance().AddWindow(windowProperty);
    int32_t actual = ScreenClient::GetInstance().DestroyAllWindow();
    EXPECT_EQ(DH_SUCCESS, actual);
}

} // DistributedHardware
} // OHOS