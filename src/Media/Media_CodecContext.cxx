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

#include <Media_CodecContext.hxx>

#include <Media_Frame.hxx>
#include <Media_FormatContext.hxx>

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OSD_Parallel.hxx>

#ifdef HAVE_FFMPEG
  #include <Standard_WarningsDisable.hxx>
extern "C"
{
  #include <libavformat/avformat.h>
};
  #include <Standard_WarningsRestore.hxx>
#endif

IMPLEMENT_STANDARD_RTTIEXT(Media_CodecContext, Standard_Transient)

// =======================================================================
// function : Media_CodecContext
// purpose  :
// =======================================================================
Media_CodecContext::Media_CodecContext()
    : myCodecCtx(NULL),
      myCodec(NULL),
      myPtsStartBase(0.0),
      myPtsStartStream(0.0),
      myTimeBase(1.0),
      myStreamIndex(0),
      myPixelAspectRatio(1.0f)
{
#ifdef HAVE_FFMPEG
  myCodecCtx = avcodec_alloc_context3(NULL);
#endif
}

// =======================================================================
// function : ~Media_CodecContext
// purpose  :
// =======================================================================
Media_CodecContext::~Media_CodecContext()
{
  Close();
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool Media_CodecContext::Init(const AVStream& theStream, double thePtsStartBase, int theNbThreads)
{
#ifdef HAVE_FFMPEG
  return Init(theStream, thePtsStartBase, theNbThreads, AV_CODEC_ID_NONE);
#else
  return Init(theStream, thePtsStartBase, theNbThreads, 0);
#endif
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool Media_CodecContext::Init(const AVStream& theStream,
                              double          thePtsStartBase,
                              int             theNbThreads,
                              int             theCodecId)
{
#ifdef HAVE_FFMPEG
  myStreamIndex = theStream.index;
  if (avcodec_parameters_to_context(myCodecCtx, theStream.codecpar) < 0)
  {
    Message::SendFail("Internal error: unable to copy codec parameters");
    Close();
    return false;
  }

  myTimeBase       = av_q2d(theStream.time_base);
  myPtsStartBase   = thePtsStartBase;
  myPtsStartStream = Media_FormatContext::StreamUnitsToSeconds(theStream, theStream.start_time);

  const AVCodecID aCodecId =
    theCodecId != AV_CODEC_ID_NONE ? (AVCodecID)theCodecId : theStream.codecpar->codec_id;
  myCodec = avcodec_find_decoder(aCodecId);
  if (myCodec == NULL)
  {
    Message::Send("FFmpeg: unable to find decoder", Message_Fail);
    Close();
    return false;
  }

  myCodecCtx->codec_id = aCodecId;
  AVDictionary* anOpts = NULL;
  av_dict_set(&anOpts, "refcounted_frames", "1", 0);
  if (theStream.codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
  {
    myCodecCtx->thread_count =
      theNbThreads <= -1 ? OSD_Parallel::NbLogicalProcessors() : theNbThreads;
  }

  if (avcodec_open2(myCodecCtx, myCodec, &anOpts) < 0)
  {
    Message::SendFail("FFmpeg: unable to open decoder");
    Close();
    return false;
  }

  myPixelAspectRatio = 1.0f;
  if (theStream.sample_aspect_ratio.num
      && av_cmp_q(theStream.sample_aspect_ratio, myCodecCtx->sample_aspect_ratio))
  {
    myPixelAspectRatio =
      float(theStream.sample_aspect_ratio.num) / float(theStream.sample_aspect_ratio.den);
  }
  else
  {
    if (myCodecCtx->sample_aspect_ratio.num == 0 || myCodecCtx->sample_aspect_ratio.den == 0)
    {
      myPixelAspectRatio = 1.0f;
    }
    else
    {
      myPixelAspectRatio =
        float(myCodecCtx->sample_aspect_ratio.num) / float(myCodecCtx->sample_aspect_ratio.den);
    }
  }

  if (theStream.codecpar->codec_type == AVMEDIA_TYPE_VIDEO
      && (myCodecCtx->width <= 0 || myCodecCtx->height <= 0))
  {
    Message::SendFail("FFmpeg: video stream has invalid dimensions");
    Close();
    return false;
  }

  return true;
#else
  (void)&theStream;
  (void)thePtsStartBase;
  (void)theNbThreads;
  (void)theCodecId;
  return false;
#endif
}

// =======================================================================
// function : Close
// purpose  :
// =======================================================================
void Media_CodecContext::Close()
{
  if (myCodecCtx != NULL)
  {
#ifdef HAVE_FFMPEG
    avcodec_free_context(&myCodecCtx);
#endif
  }
}

// =======================================================================
// function : Flush
// purpose  :
// =======================================================================
void Media_CodecContext::Flush()
{
  if (myCodecCtx != NULL)
  {
#ifdef HAVE_FFMPEG
    avcodec_flush_buffers(myCodecCtx);
#endif
  }
}

// =======================================================================
// function : SizeX
// purpose  :
// =======================================================================
int Media_CodecContext::SizeX() const
{
#ifdef HAVE_FFMPEG
  return (myCodecCtx != NULL) ? myCodecCtx->width : 0;
#else
  return 0;
#endif
}

// =======================================================================
// function : SizeY
// purpose  :
// =======================================================================
int Media_CodecContext::SizeY() const
{
#ifdef HAVE_FFMPEG
  return (myCodecCtx != NULL) ? myCodecCtx->height : 0;
#else
  return 0;
#endif
}

// =======================================================================
// function : CanProcessPacket
// purpose  :
// =======================================================================
bool Media_CodecContext::CanProcessPacket(const Handle(Media_Packet)& thePacket) const
{
  return !thePacket.IsNull() && myStreamIndex == thePacket->StreamIndex();
}

// =======================================================================
// function : SendPacket
// purpose  :
// =======================================================================
bool Media_CodecContext::SendPacket(const Handle(Media_Packet)& thePacket)
{
  if (!CanProcessPacket(thePacket))
  {
    return false;
  }

#ifdef HAVE_FFMPEG
  const int aRes = avcodec_send_packet(myCodecCtx, thePacket->Packet());
  if (aRes < 0 && aRes != AVERROR_EOF)
  {
    return false;
  }
  return true;
#else
  return false;
#endif
}

// =======================================================================
// function : ReceiveFrame
// purpose  :
// =======================================================================
bool Media_CodecContext::ReceiveFrame(const Handle(Media_Frame)& theFrame)
{
  if (theFrame.IsNull())
  {
    return false;
  }

#ifdef HAVE_FFMPEG
  const int aRes2 = avcodec_receive_frame(myCodecCtx, theFrame->ChangeFrame());
  if (aRes2 < 0)
  {
    return false;
  }

  const int64_t aPacketPts =
    theFrame->BestEffortTimestamp() != AV_NOPTS_VALUE ? theFrame->BestEffortTimestamp() : 0;
  const double aFramePts = double(aPacketPts) * myTimeBase - myPtsStartBase;
  theFrame->SetPts(aFramePts);
  return true;
#else
  return false;
#endif
}
