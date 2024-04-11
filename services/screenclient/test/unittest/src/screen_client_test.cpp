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

#include "screen_client_test.h"

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
    int32_t ret = ScreenClient::GetInstance().AddWindow(windowProperty);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_ADD_WINDOW_ERROR, ret);
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
    int32_t expectId = ScreenClient::GetInstance().AddWindow(windowProperty);
    int32_t ret = ScreenClient::GetInstance().RemoveWindow(expectId);
    EXPECT_EQ(DH_SUCCESS, ret);
}

/**
 * @tc.name: ShowWindow_001
 * @tc.desc: Verify the ShowWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, ShowWindow_001, TestSize.Level1)
{
    int32_t ret = ScreenClient::GetInstance().ShowWindow(0);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_SHOW_WINDOW_ERROR, ret);
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
    int32_t ret = ScreenClient::GetInstance().ShowWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, ret);
    ret = ScreenClient::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, ret);
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
    ScreenClient::GetInstance().surfaceMap_.emplace(windowId, nullptr);
    int32_t ret = ScreenClient::GetInstance().ShowWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_SHOW_WINDOW_ERROR, ret);
    ret = ScreenClient::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR, ret);
}

/**
 * @tc.name: HideWindow_001
 * @tc.desc: Verify the HideWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, HideWindow_001, TestSize.Level1)
{
    int32_t ret = ScreenClient::GetInstance().HideWindow(0);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_HIDE_WINDOW_ERROR, ret);
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
    int32_t ret = ScreenClient::GetInstance().HideWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, ret);
    ret = ScreenClient::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, ret);
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
    ScreenClient::GetInstance().surfaceMap_.emplace(windowId, nullptr);
    int32_t ret = ScreenClient::GetInstance().HideWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_HIDE_WINDOW_ERROR, ret);
    ret = ScreenClient::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR, ret);
}

/**
 * @tc.name: MoveWindow_001
 * @tc.desc: Verify the MoveWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, MoveWindow_001, TestSize.Level1)
{
    int32_t ret = ScreenClient::GetInstance().MoveWindow(0, 0, 0);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_MOVE_WINDOW_ERROR, ret);
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
    int32_t ret = ScreenClient::GetInstance().MoveWindow(windowId, 0, 0);
    EXPECT_EQ(DH_SUCCESS, ret);
    ret = ScreenClient::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, ret);
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
    ScreenClient::GetInstance().surfaceMap_.emplace(windowId, nullptr);
    int32_t ret = ScreenClient::GetInstance().MoveWindow(windowId, 0, 0);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_MOVE_WINDOW_ERROR, ret);
    ret = ScreenClient::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR, ret);
}

/**
 * @tc.name: RemoveWindow_001
 * @tc.desc: Verify the RemoveWindow function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, RemoveWindow_001, TestSize.Level1)
{
    int32_t ret = ScreenClient::GetInstance().RemoveWindow(0);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR, ret);
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
    int32_t ret = ScreenClient::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, ret);
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
    ScreenClient::GetInstance().surfaceMap_.emplace(windowId, nullptr);
    int32_t ret = ScreenClient::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(ERR_DH_SCREEN_SCREENCLIENT_REMOVE_WINDOW_ERROR, ret);
}

/**
 * @tc.name: GetSurface_001
 * @tc.desc: Verify the GetSurface function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenClientTest, GetSurface_001, TestSize.Level1)
{
    EXPECT_EQ(nullptr, ScreenClient::GetInstance().GetSurface(0));
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
    int32_t ret = ScreenClient::GetInstance().RemoveWindow(windowId);
    EXPECT_EQ(DH_SUCCESS, ret);
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
    int32_t ret = ScreenClient::GetInstance().DestroyAllWindow();
    EXPECT_EQ(DH_SUCCESS, ret);
}

} // DistributedHardware
} // OHOS