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

#include "screen_source_trans_test.h"
#include "screentrans_test_utils.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
constexpr static uint32_t VIDEO_DATA_NUM = 480;
void ScreenSourceTransTest::SetUpTestCase(void) {}

void ScreenSourceTransTest::TearDownTestCase(void) {}

void ScreenSourceTransTest::SetUp(void)
{
    trans = std::make_shared<ScreenSourceTrans>();
}

void ScreenSourceTransTest::TearDown(void) {}

void EnablePermissionAccess(const char* perms[], size_t permsNum, uint64_t &tokenId)
{
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = permsNum,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .aplStr = "system_basic",
    };

    infoInstance.processName = "SourceTransTest";
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

void DisablePermissionAccess(const uint64_t &tokenId)
{
    OHOS::Security::AccessToken::AccessTokenKit::DeleteToken(tokenId);
}

/**
 * @tc.name: RegisterChannelListener_001
 * @tc.desc: Verify the RegisterChannelListener function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, RegisterChannelListener_001, TestSize.Level1)
{
    int32_t actual = trans->RegisterChannelListener();
    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: RegisterProcessorListener_001
 * @tc.desc: Verify the RegisterChannelListener function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, RegisterProcessorListener_001, TestSize.Level1)
{
    VideoParam localParam;
    VideoParam remoteParam;
    int32_t actual = trans->RegisterProcessorListener(localParam, remoteParam);

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: SetUp_001
 * @tc.desc: Verify the SetUp function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, SetUp_001, TestSize.Level1)
{
    VideoParam localParam;
    VideoParam remoteParam;
    std::string peerDevId;

    int32_t actual = trans->SetUp(localParam, remoteParam, peerDevId);

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: SetUp_002
 * @tc.desc: Verify the SetUp function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, SetUp_002, TestSize.Level1)
{
    const char* perms[] = {
        "ohos.permission.DISTRIBUTED_DATASYNC",
        "ohos.permission.CAPTURE_SCREEN",
    };
    EnablePermissionAccess(perms, sizeof(perms) / sizeof(perms[0]), tokenId_);
    VideoParam localParam;
    localParam.SetCodecType(VIDEO_CODEC_TYPE_VIDEO_H264);
    localParam.SetVideoFormat(VIDEO_DATA_FORMAT_YUVI420);
    localParam.SetVideoHeight(VIDEO_DATA_NUM);
    localParam.SetVideoWidth(VIDEO_DATA_NUM);
    localParam.SetScreenHeight(VIDEO_DATA_NUM);
    localParam.SetScreenWidth(VIDEO_DATA_NUM);
    VideoParam remoteParam;
    remoteParam.SetCodecType(VIDEO_CODEC_TYPE_VIDEO_H264);
    remoteParam.SetVideoFormat(VIDEO_DATA_FORMAT_YUVI420);
    remoteParam.SetVideoHeight(VIDEO_DATA_NUM);
    remoteParam.SetVideoWidth(VIDEO_DATA_NUM);
    remoteParam.SetScreenHeight(VIDEO_DATA_NUM);
    remoteParam.SetScreenWidth(VIDEO_DATA_NUM);

    int32_t actual = trans->SetUp(localParam, remoteParam, "peerDevId");
    DisablePermissionAccess(tokenId_);

    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: InitScreenTrans_001
 * @tc.desc: Verify the InitScreenTrans function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, InitScreenTrans_001, TestSize.Level1)
{
    const char* perms[] = {
        "ohos.permission.DISTRIBUTED_DATASYNC",
        "ohos.permission.CAPTURE_SCREEN",
    };
    EnablePermissionAccess(perms, sizeof(perms) / sizeof(perms[0]), tokenId_);
    VideoParam localParam;
    VideoParam remoteParam;
    std::string peerDevId = "sinkDevId";
    trans->screenChannel_ = std::make_shared<MockScreenDataChannelImpl>();
    int32_t actual = trans->InitScreenTrans(localParam, remoteParam, peerDevId);
    DisablePermissionAccess(tokenId_);

    EXPECT_EQ(ERR_DH_SCREEN_CODEC_CONFIGURE_FAILED, actual);
}

/**
 * @tc.name: InitScreenTrans_002
 * @tc.desc: Verify the InitScreenTrans function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, InitScreenTrans_002, TestSize.Level1)
{
    const char* perms[] = {
        "ohos.permission.DISTRIBUTED_DATASYNC",
        "ohos.permission.CAPTURE_SCREEN",
    };
    EnablePermissionAccess(perms, sizeof(perms) / sizeof(perms[0]), tokenId_);
    VideoParam localParam;
    VideoParam remoteParam;
    std::string peerDevId = "sinkDevId";
    trans->SetScreenVersion("0");
    trans->screenChannel_ = std::make_shared<MockScreenDataChannelImpl>();
    int32_t actual = trans->InitScreenTrans(localParam, remoteParam, peerDevId);
    DisablePermissionAccess(tokenId_);

    EXPECT_EQ(ERR_DH_SCREEN_CODEC_CONFIGURE_FAILED, actual);
}

/**
 * @tc.name: Release_001
 * @tc.desc: Verify the Release function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, Release_001, TestSize.Level1)
{
    int32_t actual = trans->Release();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: Release_002
 * @tc.desc: Verify the Release function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, Release_002, TestSize.Level1)
{
    trans->imageProcessor_ = std::make_shared<ImageSourceProcessor>();
    std::string peerDevId = "sinkDevId";
    trans->screenChannel_ = std::make_shared<ScreenDataChannelImpl>(peerDevId);
    int32_t actual = trans->Release();

    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: Start_001
 * @tc.desc: Verify the Start function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, Start_001, TestSize.Level1)
{
    int32_t actual = trans->Start();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: Start_002
 * @tc.desc: Verify the Start function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, Start_002, TestSize.Level1)
{
    trans->imageProcessor_ = std::make_shared<ImageSourceProcessor>();
    std::string peerDevId = "sinkDevId";
    trans->screenChannel_ = std::make_shared<ScreenDataChannelImpl>(peerDevId);
    int32_t actual = trans->Start();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, actual);
}

/**
 * @tc.name: Start_003
 * @tc.desc: Verify the Start function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, Start_003, TestSize.Level1)
{
    trans->imageProcessor_ = std::make_shared<ImageSourceProcessor>();
    std::string peerDevId = "sinkDevId";
    trans->screenChannel_ = std::make_shared<MockScreenDataChannelImpl>();
    int32_t actual = trans->Start();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_TIMEOUT, actual);
}

/**
 * @tc.name: Stop_001
 * @tc.desc: Verify the Stop function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, Stop_001, TestSize.Level1)
{
    int32_t actual = trans->Stop();
    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: Stop_002
 * @tc.desc: Verify the Stop function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, Stop_002, TestSize.Level1)
{
    trans->imageProcessor_ = std::make_shared<ImageSourceProcessor>();
    std::string peerDevId = "sinkDevId";
    trans->screenChannel_ = std::make_shared<MockScreenDataChannelImpl>();
    int32_t actual = trans->Stop();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, actual);
}

/**
 * @tc.name: Stop_003
 * @tc.desc: Verify the Stop function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, Stop_003, TestSize.Level1)
{
    trans->imageProcessor_ = std::make_shared<ImageSourceProcessor>();
    std::string peerDevId = "sinkDevId";
    trans->screenChannel_ = std::make_shared<ScreenDataChannelImpl>(peerDevId);
    int32_t actual = trans->Stop();

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ERROR, actual);
}

/**
 * @tc.name: RegisterStateCallback_001
 * @tc.desc: Verify the RegisterStateCallback function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, RegisterStateCallback_001, TestSize.Level1)
{
    std::shared_ptr<IScreenSourceTransCallback> callback = nullptr;
    int32_t actual = trans->RegisterStateCallback(callback);

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: RegisterStateCallback_002
 * @tc.desc: Verify the RegisterStateCallback function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, RegisterStateCallback_002, TestSize.Level1)
{
    std::shared_ptr<IScreenSourceTransCallback> callback =
        std::make_shared<MockIScreenSourceTransCallback>();
    int32_t actual = trans->RegisterStateCallback(callback);

    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: CheckVideoParam_001
 * @tc.desc: Verify the CheckVideoParam function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, CheckVideoParam_001, TestSize.Level1)
{
    VideoParam localParam;
    localParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_MPEG4 + 1;
    int32_t actual = trans->CheckVideoParam(localParam);

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM, actual);
}

/**
 * @tc.name: CheckVideoParam
 * @tc.desc: Verify the CheckVideoParam function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, CheckVideoParam_002, TestSize.Level1)
{
    VideoParam localParam;
    localParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    localParam.videoFormat_ = VIDEO_DATA_FORMAT_RGBA8888 + 1;
    VideoParam remoteParam;
    std::string peerDevId;
    int32_t actual = trans->CheckVideoParam(localParam);

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM, actual);
}

/**
 * @tc.name: CheckTransParam_001
 * @tc.desc: Verify the CheckTransParam function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, CheckTransParam_001, TestSize.Level1)
{
    VideoParam localParam;
    localParam.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    localParam.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    localParam.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    localParam.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    localParam.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    VideoParam remoteParam;
    remoteParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    remoteParam.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    remoteParam.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    remoteParam.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    remoteParam.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    remoteParam.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    std::string peerDevId;
    int32_t actual = trans->CheckTransParam(localParam, remoteParam, peerDevId);

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_NULL_VALUE, actual);
}

/**
 * @tc.name: CheckTransParam_002
 * @tc.desc: Verify the CheckTransParam function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, CheckTransParam_002, TestSize.Level1)
{
    VideoParam localParam;
    localParam.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    localParam.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    localParam.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    localParam.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH + 1;
    localParam.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    VideoParam remoteParam;
    remoteParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    remoteParam.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    remoteParam.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    remoteParam.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    remoteParam.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    remoteParam.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    std::string peerDevId = "sinkDevId";
    int32_t actual = trans->CheckTransParam(localParam, remoteParam, peerDevId);

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM, actual);
}

/**
 * @tc.name: CheckTransParam_003
 * @tc.desc: Verify the CheckTransParam function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, CheckTransParam_003, TestSize.Level1)
{
    VideoParam localParam;
    localParam.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    localParam.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    localParam.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    localParam.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    localParam.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    VideoParam remoteParam;
    remoteParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    remoteParam.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    remoteParam.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH + 1;
    remoteParam.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    remoteParam.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    remoteParam.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    std::string peerDevId = "sinkDevId";
    int32_t actual = trans->CheckTransParam(localParam, remoteParam, peerDevId);

    EXPECT_EQ(ERR_DH_SCREEN_TRANS_ILLEGAL_PARAM, actual);
}

/**
 * @tc.name: CheckTransParam_004
 * @tc.desc: Verify the CheckTransParam function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, CheckTransParam_004, TestSize.Level1)
{
    VideoParam localParam;
    localParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    localParam.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    localParam.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    localParam.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    localParam.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    localParam.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    VideoParam remoteParam;
    remoteParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    remoteParam.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    remoteParam.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    remoteParam.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    remoteParam.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    remoteParam.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    std::string peerDevId = "sinkDevId";
    int32_t actual = trans->CheckTransParam(localParam, remoteParam, peerDevId);

    EXPECT_EQ(DH_SUCCESS, actual);
}

/**
 * @tc.name: GetImageSurface_001
 * @tc.desc: Verify the GetImageSurface function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, GetImageSurface_001, TestSize.Level1)
{
    VideoParam localParam;
    localParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    localParam.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    localParam.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    localParam.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    localParam.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    localParam.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    VideoParam remoteParam;
    remoteParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    remoteParam.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    remoteParam.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    remoteParam.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    remoteParam.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    remoteParam.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    trans->imageProcessor_ = std::make_shared<ImageSourceProcessor>();
    trans->screenDecisionCenter_ = std::make_shared<ScreenDecisionCenter>(localParam);
    trans->RegisterProcessorListener(localParam, remoteParam);

    EXPECT_NE(nullptr, trans->GetImageSurface());
}

/**
 * @tc.name: OnSessionOpened_001
 * @tc.desc: Verify the OnSessionOpened function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, OnSessionOpened_001, TestSize.Level1)
{
    trans->imageProcessor_ = std::make_shared<ImageSourceProcessor>();

    trans->OnSessionOpened();

    EXPECT_EQ(false, trans->isChannelReady_);
}

/**
 * @tc.name: OnSessionOpened_002
 * @tc.desc: Verify the OnSessionOpened function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, OnSessionOpened_002, TestSize.Level1)
{
    trans->imageProcessor_ = nullptr;

    trans->OnSessionOpened();

    EXPECT_EQ(false, trans->isChannelReady_);
}

/**
 * @tc.name: OnSessionClosed_001
 * @tc.desc: Verify the OnSessionClosed function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, OnSessionClosed_001, TestSize.Level1)
{
    trans->transCallback_ = std::make_shared<MockIScreenSourceTransCallback>();

    trans->OnSessionClosed();

    EXPECT_EQ(false, trans->isChannelReady_);
}

/**
 * @tc.name: OnImageProcessDone_001
 * @tc.desc: Verify the OnImageProcessDone function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, OnImageProcessDone_001, TestSize.Level1)
{
    std::shared_ptr<DataBuffer> data = nullptr;
    uint32_t queueSize = trans->dataQueue_.size();
    trans->OnImageProcessDone(data);

    EXPECT_NE(queueSize, trans->dataQueue_.size());
}

/**
 * @tc.name: OnImageProcessDone_002
 * @tc.desc: Verify the OnImageProcessDone function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, OnImageProcessDone_002, TestSize.Level1)
{
    std::shared_ptr<DataBuffer> data = nullptr;
    uint32_t queueSize = trans->dataQueue_.size();
    for (uint32_t i = 0; i < DATA_QUEUE_MAX_SIZE + 1; i++) {
        trans->dataQueue_.push(data);
    }
    trans->OnImageProcessDone(data);

    EXPECT_NE(queueSize, trans->dataQueue_.size());
}

/**
 * @tc.name: SetConsumerSurface_001
 * @tc.desc: Verify the SetConsumerSurface function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, SetConsumerSurface_001, TestSize.Level1)
{
    VideoParam videoParam;
    std::vector<OHOS::Rect> damages;
    sptr<SurfaceBuffer> surfaceBuffer = nullptr;
    trans->OnDamageProcessDone(surfaceBuffer, damages);
    trans->imageProcessor_ = std::make_shared<ImageSourceProcessor>();
    std::shared_ptr<IImageSourceProcessorListener> listener = std::make_shared<ScreenSourceTrans>();
    trans->imageProcessor_->ConfigureImageProcessor(videoParam, videoParam, listener);
    int32_t ret = trans->SetConsumerSurface();
    EXPECT_EQ(ERR_DH_SCREEN_SURFACE_INVALIED, ret);
}

/**
 * @tc.name: SetConsumerSurface_002
 * @tc.desc: Verify the SetConsumerSurface function.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(ScreenSourceTransTest, SetConsumerSurface_002, TestSize.Level1)
{
    VideoParam remoteParam;
    remoteParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    remoteParam.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    remoteParam.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    remoteParam.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    remoteParam.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    remoteParam.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    VideoParam localParam;
    localParam.videoFormat_ = VIDEO_DATA_FORMAT_YUVI420;
    localParam.codecType_ = VIDEO_CODEC_TYPE_VIDEO_H264;
    localParam.videoWidth_ = DSCREEN_MAX_VIDEO_DATA_WIDTH;
    localParam.videoHeight_ = DSCREEN_MAX_VIDEO_DATA_HEIGHT;
    localParam.screenWidth_ = DSCREEN_MAX_SCREEN_DATA_WIDTH;
    localParam.screenHeight_ = DSCREEN_MAX_SCREEN_DATA_HEIGHT;
    trans->imageProcessor_ = std::make_shared<ImageSourceProcessor>();
    std::shared_ptr<IImageSourceProcessorListener> listener = std::make_shared<ScreenSourceTrans>();
    trans->imageProcessor_->ConfigureImageProcessor(localParam, remoteParam, listener);
    int32_t ret = trans->SetConsumerSurface();
    EXPECT_EQ(DH_SUCCESS, ret);
}
}
}
