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

#include <Media_Frame.hxx>

#include <Media_Scaler.hxx>

#ifdef HAVE_FFMPEG
#include <Standard_WarningsDisable.hxx>
extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavutil/imgutils.h>
};
#include <Standard_WarningsRestore.hxx>
#endif

IMPLEMENT_STANDARD_RTTIEXT(Media_Frame, Standard_Transient)

// =======================================================================
// function : FormatFFmpeg2Occt
// purpose  :
// =======================================================================
Image_Format Media_Frame::FormatFFmpeg2Occt (int theFormat)
{
#ifdef HAVE_FFMPEG
  switch (theFormat)
  {
    case AV_PIX_FMT_RGBA:
      return Image_Format_RGBA;
    case AV_PIX_FMT_BGRA:
      return Image_Format_BGRA;
    case AV_PIX_FMT_RGB0:
      return Image_Format_RGB32;
    case AV_PIX_FMT_BGR0:
      return Image_Format_BGR32;
    case AV_PIX_FMT_RGB24:
      return Image_Format_RGB;
    case AV_PIX_FMT_BGR24:
      return Image_Format_BGR;
    case AV_PIX_FMT_GRAY8:
      return Image_Format_Gray;
    case AV_PIX_FMT_GRAY16:
      return Image_Format_Gray16;
    default:
      return Image_Format_UNKNOWN;
  }
#else
  (void )theFormat;
  return Image_Format_UNKNOWN;
#endif
}

// =======================================================================
// function : FormatOcct2FFmpeg
// purpose  :
// =======================================================================
int Media_Frame::FormatOcct2FFmpeg (Image_Format theFormat)
{
#ifdef HAVE_FFMPEG
  switch (theFormat)
  {
    case Image_Format_RGBA:
      return AV_PIX_FMT_RGBA;
    case Image_Format_BGRA:
      return AV_PIX_FMT_BGRA;
    case Image_Format_RGB32:
      return AV_PIX_FMT_RGB0;
    case Image_Format_BGR32:
      return AV_PIX_FMT_BGR0;
    case Image_Format_RGB:
      return AV_PIX_FMT_RGB24;
    case Image_Format_BGR:
      return AV_PIX_FMT_BGR24;
    case Image_Format_Gray:
      return AV_PIX_FMT_GRAY8;
    case Image_Format_Alpha:
      return AV_PIX_FMT_GRAY8;
    case Image_Format_Gray16:
      return AV_PIX_FMT_GRAY16;
    case Image_Format_GrayF:
    case Image_Format_AlphaF:
    case Image_Format_RGF:
    case Image_Format_RGBAF:
    case Image_Format_RGBF:
    case Image_Format_BGRAF:
    case Image_Format_BGRF:
    case Image_Format_GrayF_half:
    case Image_Format_RGF_half:
    case Image_Format_RGBAF_half:
    case Image_Format_UNKNOWN:
      return AV_PIX_FMT_NONE; // unsupported
  }
  return AV_PIX_FMT_NONE;
#else
  (void )theFormat;
  return 0;
#endif
}

// =======================================================================
// function : Media_Frame
// purpose  :
// =======================================================================
Media_Frame::Media_Frame()
: myFrame (NULL),
  myFramePts  (0.0),
  myPixelRatio(1.0f),
  myIsLocked  (false)
{
#ifdef HAVE_FFMPEG
  myFrame = av_frame_alloc();
#endif
  Unref();
}

// =======================================================================
// function : ~Media_Frame
// purpose  :
// =======================================================================
Media_Frame::~Media_Frame()
{
#ifdef HAVE_FFMPEG
  av_frame_free (&myFrame);
#endif
}

// =======================================================================
// function : Unref
// purpose  :
// =======================================================================
void Media_Frame::Unref()
{
#ifdef HAVE_FFMPEG
  av_frame_unref (myFrame);
#endif
}

// =======================================================================
// function : IsFullRangeYUV
// purpose  :
// =======================================================================
bool Media_Frame::IsFullRangeYUV() const
{
#ifdef HAVE_FFMPEG
  return Format() == AV_PIX_FMT_YUVJ420P
      || myFrame->color_range == AVCOL_RANGE_JPEG;
#else
  return true;
#endif
}

// =======================================================================
// function : Swap
// purpose  :
// =======================================================================
void Media_Frame::Swap (const Handle(Media_Frame)& theFrame1,
                        const Handle(Media_Frame)& theFrame2)
{
  std::swap (theFrame1->myFrame, theFrame2->myFrame);
}

// =======================================================================
// function : IsEmpty
// purpose  :
// =======================================================================
bool Media_Frame::IsEmpty() const
{
#ifdef HAVE_FFMPEG
  return myFrame->format == -1; // AV_PIX_FMT_NONE
#else
  return true;
#endif
}

// =======================================================================
// function : SizeX
// purpose  :
// =======================================================================
int Media_Frame::SizeX() const
{
#ifdef HAVE_FFMPEG
  return myFrame->width;
#else
  return 0;
#endif
}

// =======================================================================
// function : SizeY
// purpose  :
// =======================================================================
int Media_Frame::SizeY() const
{
#ifdef HAVE_FFMPEG
  return myFrame->height;
#else
  return 0;
#endif
}

// =======================================================================
// function : Format
// purpose  :
// =======================================================================
int Media_Frame::Format() const
{
#ifdef HAVE_FFMPEG
  return myFrame->format;
#else
  return 0;
#endif
}

// =======================================================================
// function : Plane
// purpose  :
// =======================================================================
uint8_t* Media_Frame::Plane (int thePlaneId) const
{
#ifdef HAVE_FFMPEG
  return myFrame->data[thePlaneId];
#else
  (void )thePlaneId;
  return NULL;
#endif
}

// =======================================================================
// function : LineSize
// purpose  :
// =======================================================================
int Media_Frame::LineSize (int thePlaneId) const
{
#ifdef HAVE_FFMPEG
  return myFrame->linesize[thePlaneId];
#else
  (void )thePlaneId;
  return 0;
#endif
}

// =======================================================================
// function : BestEffortTimestamp
// purpose  :
// =======================================================================
int64_t Media_Frame::BestEffortTimestamp() const
{
#ifdef HAVE_FFMPEG
  return myFrame->best_effort_timestamp;
#else
  return 0;
#endif
}

// =======================================================================
// function : InitWrapper
// purpose  :
// =======================================================================
bool Media_Frame::InitWrapper (const Handle(Image_PixMap)& thePixMap)
{
  Unref();
  if (thePixMap.IsNull())
  {
    return false;
  }

#ifdef HAVE_FFMPEG
  myFrame->format = FormatOcct2FFmpeg (thePixMap->Format());
  if (myFrame->format == AV_PIX_FMT_NONE)
  {
    return false;
  }

  myFrame->width       = (int )thePixMap->SizeX();
  myFrame->height      = (int )thePixMap->SizeY();
  myFrame->data[0]     = (uint8_t* )thePixMap->ChangeData();
  myFrame->linesize[0] = (int      )thePixMap->SizeRowBytes();
  for (int aPlaneIter = 1; aPlaneIter < AV_NUM_DATA_POINTERS; ++aPlaneIter)
  {
    myFrame->data    [aPlaneIter] = NULL;
    myFrame->linesize[aPlaneIter] = 0;
  }
  return true;
#else
  return false;
#endif
}
