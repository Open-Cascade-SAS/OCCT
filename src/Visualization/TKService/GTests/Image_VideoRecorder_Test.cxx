// Copyright (c) 2025 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <Image_VideoRecorder.hxx>
#include <Image_PixMap.hxx>

#include <gtest/gtest.h>

class Image_VideoRecorderTest : public ::testing::Test
{
protected:
  void SetUp() override { myRecorder = new Image_VideoRecorder(); }

  void TearDown() override
  {
    if (!myRecorder.IsNull())
    {
      myRecorder->Close();
    }
  }

  Handle(Image_VideoRecorder) myRecorder;
};

TEST_F(Image_VideoRecorderTest, DefaultConstructor)
{
  EXPECT_FALSE(myRecorder.IsNull());
  EXPECT_EQ(0, myRecorder->FrameCount());
}

TEST_F(Image_VideoRecorderTest, VideoParamsStructure)
{
  Image_VideoParams params;

  // Test default values
  EXPECT_EQ(0, params.Width);
  EXPECT_EQ(0, params.Height);
  EXPECT_EQ(0, params.FpsNum);
  EXPECT_EQ(1, params.FpsDen);
  EXPECT_TRUE(params.Format.IsEmpty());
  EXPECT_TRUE(params.VideoCodec.IsEmpty());
  EXPECT_TRUE(params.PixelFormat.IsEmpty());

  // Test setters
  params.SetFramerate(30);
  EXPECT_EQ(30, params.FpsNum);
  EXPECT_EQ(1, params.FpsDen);

  params.SetFramerate(25, 2);
  EXPECT_EQ(25, params.FpsNum);
  EXPECT_EQ(2, params.FpsDen);
}

TEST_F(Image_VideoRecorderTest, OpenVideoFile)
{
#ifdef HAVE_FFMPEG
  Image_VideoParams params;
  params.Width  = 320;
  params.Height = 240;
  params.SetFramerate(15); // Low framerate for test
  params.Format      = "avi";
  params.VideoCodec  = "mpeg4"; // Use a commonly available codec
  params.PixelFormat = "yuv420p";

  // Test opening a valid video file
  Standard_Boolean isOpened = myRecorder->Open("test_video.avi", params);
  EXPECT_TRUE(isOpened);

  if (isOpened)
  {
    // Test frame access
    Image_PixMap& frame = myRecorder->ChangeFrame();
    EXPECT_EQ(params.Width, frame.Width());
    EXPECT_EQ(params.Height, frame.Height());
    EXPECT_EQ(Image_Format_RGBA, frame.Format());

    myRecorder->Close();
  }
#endif
}

TEST_F(Image_VideoRecorderTest, InvalidParameters)
{
#ifdef HAVE_FFMPEG
  Image_VideoParams params;
  // Leave parameters invalid (width=0, height=0)

  Standard_Boolean isOpened = myRecorder->Open("invalid_test.avi", params);
  EXPECT_FALSE(isOpened);
#endif
}

TEST_F(Image_VideoRecorderTest, WriteFrames)
{
#ifdef HAVE_FFMPEG
  Image_VideoParams params;
  params.Width  = 160; // Small size for fast test
  params.Height = 120;
  params.SetFramerate(10); // Low framerate
  params.Format      = "avi";
  params.VideoCodec  = "mpeg4";
  params.PixelFormat = "yuv420p";

  Standard_Boolean isOpened = myRecorder->Open("test_frames.avi", params);

  if (isOpened)
  {
    // Fill frame with test pattern
    Image_PixMap& frame = myRecorder->ChangeFrame();

    // Create a simple red-to-blue gradient
    for (Standard_Integer y = 0; y < params.Height; ++y)
    {
      for (Standard_Integer x = 0; x < params.Width; ++x)
      {
        Standard_Byte* pixel = frame.ChangeData() + (y * frame.SizeRowBytes()) + (x * 4);
        pixel[0]             = (Standard_Byte)(255 * x / params.Width);  // Red gradient
        pixel[1]             = 0;                                        // Green
        pixel[2]             = (Standard_Byte)(255 * y / params.Height); // Blue gradient
        pixel[3]             = 255;                                      // Alpha
      }
    }

    // Test writing a few frames
    EXPECT_EQ(0, myRecorder->FrameCount());

    EXPECT_TRUE(myRecorder->PushFrame());
    EXPECT_EQ(1, myRecorder->FrameCount());

    EXPECT_TRUE(myRecorder->PushFrame());
    EXPECT_EQ(2, myRecorder->FrameCount());

    EXPECT_TRUE(myRecorder->PushFrame());
    EXPECT_EQ(3, myRecorder->FrameCount());

    myRecorder->Close();
  }
#endif // HAVE_FFMPEG
}

TEST_F(Image_VideoRecorderTest, CloseWithoutOpen)
{
  // Test that closing without opening doesn't crash
  myRecorder->Close();
  EXPECT_EQ(0, myRecorder->FrameCount());
}
