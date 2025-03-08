// Created by: Kirill GAVRILOV
// Copyright (c) 2019 OPEN CASCADE SAS
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

// activate some C99 macros like UINT64_C in "stdint.h" which used by FFmpeg
#ifndef __STDC_CONSTANT_MACROS
  #define __STDC_CONSTANT_MACROS
#endif

#ifdef _WIN32
  #include <windows.h>
#endif

#include <Media_PlayerContext.hxx>

#include <Image_AlienPixMap.hxx>
#include <Media_BufferPool.hxx>
#include <Media_FormatContext.hxx>
#include <Media_CodecContext.hxx>
#include <Media_Scaler.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OSD.hxx>

#ifdef HAVE_FFMPEG
  #include <Standard_WarningsDisable.hxx>
extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libavutil/imgutils.h>
};
  #include <Standard_WarningsRestore.hxx>
#endif

IMPLEMENT_STANDARD_RTTIEXT(Media_PlayerContext, Standard_Transient)

//=================================================================================================

Media_PlayerContext::Media_PlayerContext(Media_IFrameQueue* theFrameQueue)
    : myFrameQueue(theFrameQueue),
      myThread(doThreadWrapper),
      myWakeEvent(false),
      myNextEvent(false),
      myDuration(0.0),
      myToForceRgb(true),
      myToShutDown(false),
      mySeekTo(0.0),
      myPlayEvent(Media_PlayerEvent_NONE)
{
  myThread.Run(this);

#if defined(_WIN32) && !defined(OCCT_UWP)
  // Adjust system timer
  // By default Windows2K+ timer has ugly precision
  // Thus - Sleep(1) may be long 14ms!
  // We force best available precision to make Sleep() more adequate
  // This affect whole system while running application!
  TIMECAPS aTimeCaps = {0, 0};
  if (timeGetDevCaps(&aTimeCaps, sizeof(aTimeCaps)) == TIMERR_NOERROR)
  {
    timeBeginPeriod(aTimeCaps.wPeriodMin);
  }
  else
  {
    timeBeginPeriod(1);
  }
#endif
}

//=================================================================================================

Media_PlayerContext::~Media_PlayerContext()
{
  myToShutDown = Standard_True;
  myWakeEvent.Set();
  myThread.Wait();

#if defined(_WIN32) && !defined(OCCT_UWP)
  // restore timer adjustments
  TIMECAPS aTimeCaps = {0, 0};
  if (timeGetDevCaps(&aTimeCaps, sizeof(aTimeCaps)) == TIMERR_NOERROR)
  {
    timeEndPeriod(aTimeCaps.wPeriodMin);
  }
  else
  {
    timeEndPeriod(1);
  }
#endif
}

//=================================================================================================

Handle(Media_Frame) Media_PlayerContext::DumpFirstFrame(const TCollection_AsciiString& theSrcVideo,
                                                        TCollection_AsciiString&       theMediaInfo)
{
  theMediaInfo.Clear();
  Handle(Media_FormatContext) aFormatCtx = new Media_FormatContext();
  if (!aFormatCtx->OpenInput(theSrcVideo))
  {
    return Handle(Media_Frame)();
  }

  Handle(Media_CodecContext) aVideoCtx;
#ifdef HAVE_FFMPEG
  for (unsigned int aStreamId = 0; aStreamId < aFormatCtx->NbSteams(); ++aStreamId)
  {
    const AVStream&   aStream    = aFormatCtx->Stream(aStreamId);
    const AVMediaType aCodecType = aStream.codecpar->codec_type;
    if (aCodecType == AVMEDIA_TYPE_VIDEO)
    {
      aVideoCtx = new Media_CodecContext();
      if (!aVideoCtx->Init(aStream, aFormatCtx->PtsStartBase(), 1))
      {
        return Handle(Media_Frame)();
      }

      theMediaInfo = aFormatCtx->StreamInfo(aStreamId, aVideoCtx->Context());
      break;
    }
  }
#endif
  if (aVideoCtx.IsNull())
  {
    Message::SendFail(TCollection_AsciiString("FFmpeg: no video stream in '") + theSrcVideo + "'");
    return Handle(Media_Frame)();
  }

  Handle(Media_Packet) aPacket = new Media_Packet();
  Handle(Media_Frame)  aFrame  = new Media_Frame();
  for (;;)
  {
    if (!aFormatCtx->ReadPacket(aPacket))
    {
      Message::SendFail(TCollection_AsciiString("FFmpeg: unable to read from '") + theSrcVideo
                        + "'");
      return Handle(Media_Frame)();
    }
    if (!aVideoCtx->CanProcessPacket(aPacket))
    {
      continue;
    }

    if (aVideoCtx->SendPacket(aPacket) && aVideoCtx->ReceiveFrame(aFrame))
    {
      break;
    }
  }
  if (aFrame->IsEmpty() || aFrame->SizeX() < 1 || aFrame->SizeY() < 1)
  {
    Message::SendFail(TCollection_AsciiString("FFmpeg: unable to decode first video frame from '")
                      + theSrcVideo + "'");
    return Handle(Media_Frame)();
  }
  return aFrame;
}

//=================================================================================================

bool Media_PlayerContext::DumpFirstFrame(const TCollection_AsciiString& theSrcVideo,
                                         const TCollection_AsciiString& theOutImage,
                                         TCollection_AsciiString&       theMediaInfo,
                                         int                            theMaxSize)
{
  Handle(Media_Frame) aFrame = DumpFirstFrame(theSrcVideo, theMediaInfo);
  if (aFrame.IsNull())
  {
    return false;
  }

  Handle(Image_AlienPixMap) aPixMap   = new Image_AlienPixMap();
  int                       aResSizeX = aFrame->SizeX(), aResSizeY = aFrame->SizeY();
  if (theMaxSize > 0)
  {
    if (aResSizeX > aResSizeY)
    {
      aResSizeX = theMaxSize;
      aResSizeY = int((double(aFrame->SizeY()) / double(aFrame->SizeX())) * double(aResSizeX));
    }
    else
    {
      aResSizeY = theMaxSize;
      aResSizeX = int((double(aFrame->SizeX()) / double(aFrame->SizeY())) * double(aResSizeY));
    }
  }
  if (!aPixMap->InitZero(Image_Format_RGB, aResSizeX, aResSizeY))
  {
    Message::SendFail("FFmpeg: Failed allocation of RGB frame (out of memory)");
    return false;
  }

  // Image_Format aFormat = aFrame->FormatFFmpeg2Occt (aFrame->Format());
  // if (aFormat == Image_Format_UNKNOWN || theMaxSize > 0)
  {
    Handle(Media_Frame) anRgbFrame = new Media_Frame();
    anRgbFrame->InitWrapper(aPixMap);

    Media_Scaler aScaler;
    if (!aScaler.Convert(aFrame, anRgbFrame))
    {
      Message::SendFail(TCollection_AsciiString("FFmpeg: unable to convert frame into RGB '")
                        + theSrcVideo + "'");
      return false;
    }
  }

  aPixMap->SetTopDown(true);
  return aPixMap->Save(theOutImage);
}

//=================================================================================================

void Media_PlayerContext::SetInput(const TCollection_AsciiString& theInputPath,
                                   Standard_Boolean               theToWait)
{
  {
    Standard_Mutex::Sentry aLock(myMutex);
    if (theToWait)
    {
      myNextEvent.Reset();
    }
    myInputPath = theInputPath;
    myPlayEvent = Media_PlayerEvent_NEXT;
    myWakeEvent.Set();
  }
  if (theToWait)
  {
    myNextEvent.Wait();
  }
}

//=================================================================================================

void Media_PlayerContext::PlaybackState(Standard_Boolean& theIsPaused,
                                        Standard_Real&    theProgress,
                                        Standard_Real&    theDuration)
{
  Standard_Mutex::Sentry aLock(myMutex);
  theIsPaused = !myTimer.IsStarted();
  theProgress = myTimer.ElapsedTime();
  theDuration = myDuration;
}

//=================================================================================================

void Media_PlayerContext::PlayPause(Standard_Boolean& theIsPaused,
                                    Standard_Real&    theProgress,
                                    Standard_Real&    theDuration)
{
  Standard_Mutex::Sentry aLock(myMutex);
  theProgress = myTimer.ElapsedTime();
  theDuration = myDuration;
  if (myTimer.IsStarted())
  {
    pushPlayEvent(Media_PlayerEvent_PAUSE);
    theIsPaused = true;
  }
  else
  {
    pushPlayEvent(Media_PlayerEvent_RESUME);
    theIsPaused = false;
  }
}

//=================================================================================================

void Media_PlayerContext::Seek(Standard_Real thePosSec)
{
  Standard_Mutex::Sentry aLock(myMutex);
  mySeekTo = thePosSec;
  pushPlayEvent(Media_PlayerEvent_SEEK);
}

//=================================================================================================

void Media_PlayerContext::pushPlayEvent(Media_PlayerEvent thePlayEvent)
{
  Standard_Mutex::Sentry aLock(myMutex);
  myPlayEvent = thePlayEvent;
  myWakeEvent.Set();
}

//=================================================================================================

bool Media_PlayerContext::popPlayEvent(Media_PlayerEvent&                 thePlayEvent,
                                       const Handle(Media_FormatContext)& theFormatCtx,
                                       const Handle(Media_CodecContext)&  theVideoCtx,
                                       const Handle(Media_Frame)&         theFrame)
{
  if (myPlayEvent == Media_PlayerEvent_NONE)
  {
    thePlayEvent = Media_PlayerEvent_NONE;
    return false;
  }

  Standard_Mutex::Sentry aLock(myMutex);
  thePlayEvent = myPlayEvent;
  if (thePlayEvent == Media_PlayerEvent_PAUSE)
  {
    myTimer.Pause();
  }
  else if (thePlayEvent == Media_PlayerEvent_RESUME)
  {
    myTimer.Start();
  }
  else if (thePlayEvent == Media_PlayerEvent_SEEK)
  {
    if (!theFormatCtx.IsNull() && !theVideoCtx.IsNull())
    {
      if (!theFormatCtx->SeekStream(theVideoCtx->StreamIndex(), mySeekTo, false))
      {
        theFormatCtx->Seek(mySeekTo, false);
      }
      theVideoCtx->Flush();
      if (!theFrame.IsNull())
      {
        theFrame->Unref();
      }
      myTimer.Seek(mySeekTo);
    }
  }

  myPlayEvent = Media_PlayerEvent_NONE;
  return thePlayEvent != Media_PlayerEvent_NONE;
}

//! Returns nearest (greater or equal) aligned number.
static int getAligned(size_t theNumber, size_t theAlignment = 32)
{
  return int(theNumber + theAlignment - 1 - (theNumber - 1) % theAlignment);
}

//=================================================================================================

bool Media_PlayerContext::receiveFrame(const Handle(Media_Frame)&        theFrame,
                                       const Handle(Media_CodecContext)& theVideoCtx)
{
  if (myFrameTmp.IsNull())
  {
    myFrameTmp = new Media_Frame();
  }
  if (!theVideoCtx->ReceiveFrame(myFrameTmp))
  {
    return false;
  }

  theFrame->SetPts(myFrameTmp->Pts());
  theFrame->SetPixelAspectRatio(myFrameTmp->PixelAspectRatio());

  Image_Format anOcctFmt = Media_Frame::FormatFFmpeg2Occt(myFrameTmp->Format());
  if (anOcctFmt != Image_Format_UNKNOWN)
  {
    Media_Frame::Swap(theFrame, myFrameTmp);
    return true;
  }
#ifdef HAVE_FFMPEG
  else if (!myToForceRgb
           && (myFrameTmp->Format() == AV_PIX_FMT_YUV420P
               || myFrameTmp->Format() == AV_PIX_FMT_YUVJ420P))
  {
    Media_Frame::Swap(theFrame, myFrameTmp);
    return true;
  }
#endif

  theFrame->Unref();
  if (myFrameTmp->IsEmpty() || myFrameTmp->Size().x() < 1 || myFrameTmp->Size().y() < 1)
  {
    theFrame->Unref();
    return false;
  }

  const Graphic3d_Vec2i aSize   = myFrameTmp->Size();
  const Graphic3d_Vec2i aSizeUV = myFrameTmp->Size() / 2;
  AVFrame*              aFrame  = theFrame->ChangeFrame();
  if (myToForceRgb)
  {
    if (myBufferPools[0].IsNull())
    {
      myBufferPools[0] = new Media_BufferPool();
    }

    const int aLineSize = getAligned(aSize.x() * 3);
    const int aBufSize  = aLineSize * aSize.y();
    if (!myBufferPools[0]->Init(aBufSize))
    {
      Message::SendFail("FFmpeg: unable to allocate RGB24 frame buffer");
      return false;
    }

#ifdef HAVE_FFMPEG
    aFrame->buf[0] = myBufferPools[0]->GetBuffer();
    if (aFrame->buf[0] == NULL)
    {
      theFrame->Unref();
      Message::SendFail("FFmpeg: unable to allocate RGB24 frame buffer");
      return false;
    }

    aFrame->format      = AV_PIX_FMT_RGB24;
    aFrame->width       = aSize.x();
    aFrame->height      = aSize.y();
    aFrame->linesize[0] = aLineSize;
    aFrame->data[0]     = aFrame->buf[0]->data;
#else
    (void)aFrame;
#endif
  }
  else
  {
    for (int aPlaneIter = 0; aPlaneIter < 3; ++aPlaneIter)
    {
      if (myBufferPools[aPlaneIter].IsNull())
      {
        myBufferPools[aPlaneIter] = new Media_BufferPool();
      }
    }

    const int aLineSize   = getAligned(aSize.x());
    const int aLineSizeUV = getAligned(aSizeUV.x());
    const int aBufSize    = aLineSize * aSize.y();
    const int aBufSizeUV  = aLineSizeUV * aSizeUV.y();
    if (!myBufferPools[0]->Init(aBufSize) || !myBufferPools[1]->Init(aBufSizeUV)
        || !myBufferPools[2]->Init(aBufSizeUV))
    {
      Message::SendFail("FFmpeg: unable to allocate YUV420P frame buffers");
      return false;
    }

#ifdef HAVE_FFMPEG
    aFrame->buf[0] = myBufferPools[0]->GetBuffer();
    aFrame->buf[1] = myBufferPools[1]->GetBuffer();
    aFrame->buf[2] = myBufferPools[2]->GetBuffer();
    if (aFrame->buf[0] == NULL || aFrame->buf[1] == NULL || aFrame->buf[2] == NULL)
    {
      theFrame->Unref();
      Message::SendFail("FFmpeg: unable to allocate YUV420P frame buffers");
      return false;
    }

    aFrame->format      = AV_PIX_FMT_YUV420P;
    aFrame->width       = aSize.x();
    aFrame->height      = aSize.y();
    aFrame->linesize[0] = aLineSize;
    aFrame->linesize[1] = aLineSizeUV;
    aFrame->linesize[2] = aLineSizeUV;
    aFrame->data[0]     = aFrame->buf[0]->data;
    aFrame->data[1]     = aFrame->buf[1]->data;
    aFrame->data[2]     = aFrame->buf[2]->data;
#endif
  }

  if (myScaler.IsNull())
  {
    myScaler = new Media_Scaler();
  }
  if (!myScaler->Convert(myFrameTmp, theFrame))
  {
    return false;
  }
  myFrameTmp->Unref();
  return true;
}

//=================================================================================================

void Media_PlayerContext::doThreadLoop()
{
  // always set OCCT signal handler to catch signals if any;
  // this is safe (for thread local handler) since the thread
  // is owned by this class
  OSD::SetThreadLocalSignal(OSD_SignalMode_Set, false);

  Handle(Media_Frame) aFrame;
  bool                wasSeeked = false;
  for (;;)
  {
    myWakeEvent.Wait();
    myWakeEvent.Reset();
    if (myToShutDown)
    {
      return;
    }

    TCollection_AsciiString anInput;
    {
      Standard_Mutex::Sentry aLock(myMutex);
      std::swap(anInput, myInputPath);
      if (myPlayEvent == Media_PlayerEvent_NEXT)
      {
        myPlayEvent = Media_PlayerEvent_NONE;
      }
    }
    myNextEvent.Set();
    if (anInput.IsEmpty())
    {
      continue;
    }

    Handle(Media_FormatContext) aFormatCtx = new Media_FormatContext();
    if (!aFormatCtx->OpenInput(anInput))
    {
      continue;
    }

    Handle(Media_CodecContext) aVideoCtx;
#ifdef HAVE_FFMPEG
    for (unsigned int aStreamId = 0; aStreamId < aFormatCtx->NbSteams(); ++aStreamId)
    {
      const AVStream&   aStream    = aFormatCtx->Stream(aStreamId);
      const AVMediaType aCodecType = aStream.codecpar->codec_type;
      if (aCodecType == AVMEDIA_TYPE_VIDEO)
      {
        aVideoCtx = new Media_CodecContext();
        if (!aVideoCtx->Init(aStream, aFormatCtx->PtsStartBase(), 1))
        {
          aVideoCtx.Nullify();
        }
        else
        {
          break;
        }
      }
    }
#endif
    if (aVideoCtx.IsNull())
    {
      Message::SendFail(TCollection_AsciiString("FFmpeg: no video stream in '") + anInput + "'");
      continue;
    }

    Handle(Media_Packet) aPacket    = new Media_Packet();
    Media_PlayerEvent    aPlayEvent = Media_PlayerEvent_NONE;
    {
      Standard_Mutex::Sentry aLock(myMutex);
      myTimer.Stop();
      myTimer.Start();
      myDuration = aFormatCtx->Duration();
    }
    if (!aFrame.IsNull())
    {
      aFrame->Unref();
    }
    const double anUploadDelaySec = 1.0 / 60.0 + 0.0001;
    for (;;)
    {
      if (myToShutDown)
      {
        return;
      }
      else if (!aFormatCtx->ReadPacket(aPacket))
      {
        break;
      }

      popPlayEvent(aPlayEvent, aFormatCtx, aVideoCtx, aFrame);
      if (aPlayEvent == Media_PlayerEvent_NEXT)
      {
        break;
      }
      else if (aPlayEvent == Media_PlayerEvent_SEEK)
      {
        wasSeeked = true;
      }

      bool isAccepted = false;
      if (aVideoCtx->CanProcessPacket(aPacket))
      {
        isAccepted = true;
        aVideoCtx->SendPacket(aPacket);
      }
      aPacket->Unref();
      if (!isAccepted)
      {
        continue;
      }
      for (;;)
      {
        if (myToShutDown)
        {
          return;
        }
        else if (popPlayEvent(aPlayEvent, aFormatCtx, aVideoCtx, aFrame))
        {
          if (aPlayEvent == Media_PlayerEvent_NEXT)
          {
            break;
          }
          else if (aPlayEvent == Media_PlayerEvent_SEEK)
          {
            wasSeeked = true;
          }
        }

        if (aFrame.IsNull())
        {
          aFrame = myFrameQueue->LockFrame();
          if (aFrame.IsNull())
          {
            OSD::MilliSecSleep(1);
            continue;
          }
          aFrame->Unref();
        }
        if (aFrame->IsEmpty() && !receiveFrame(aFrame, aVideoCtx))
        {
          break;
        }

        const double aTime = myTimer.ElapsedTime() - anUploadDelaySec;
        if (wasSeeked || (aFrame->Pts() <= aTime && myTimer.IsStarted()))
        {
          wasSeeked = false;
          myFrameQueue->ReleaseFrame(aFrame);
          aFrame.Nullify();
          break;
        }

        OSD::MilliSecSleep(1);
      }
      if (aPlayEvent == Media_PlayerEvent_NEXT)
      {
        break;
      }
    }
  }
}
