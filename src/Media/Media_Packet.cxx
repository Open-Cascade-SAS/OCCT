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

#include <Media_Packet.hxx>

#ifdef HAVE_FFMPEG
  #include <Standard_WarningsDisable.hxx>
extern "C"
{
  #include <libavcodec/avcodec.h>
};
  #include <Standard_WarningsRestore.hxx>
#endif

IMPLEMENT_STANDARD_RTTIEXT(Media_Packet, Standard_Transient)

// =======================================================================
// function : Media_Packet
// purpose  :
// =======================================================================
Media_Packet::Media_Packet()
    : myPacket(NULL)
{
#ifdef HAVE_FFMPEG
  myPacket = av_packet_alloc();
#endif
}

// =======================================================================
// function : ~Media_Packet
// purpose  :
// =======================================================================
Media_Packet::~Media_Packet()
{
#ifdef HAVE_FFMPEG
  av_packet_free(&myPacket);
#endif
}

// =======================================================================
// function : Unref
// purpose  :
// =======================================================================
void Media_Packet::Unref()
{
#ifdef HAVE_FFMPEG
  av_packet_unref(myPacket);
#endif
}

// =======================================================================
// function : Data
// purpose  :
// =======================================================================
const uint8_t* Media_Packet::Data() const
{
#ifdef HAVE_FFMPEG
  return myPacket->data;
#else
  return NULL;
#endif
}

// =======================================================================
// function : ChangeData
// purpose  :
// =======================================================================
uint8_t* Media_Packet::ChangeData()
{
#ifdef HAVE_FFMPEG
  return myPacket->data;
#else
  return NULL;
#endif
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
int Media_Packet::Size() const
{
#ifdef HAVE_FFMPEG
  return myPacket->size;
#else
  return 0;
#endif
}

// =======================================================================
// function : Pts
// purpose  :
// =======================================================================
int64_t Media_Packet::Pts() const
{
#ifdef HAVE_FFMPEG
  return myPacket->pts;
#else
  return 0;
#endif
}

// =======================================================================
// function : Dts
// purpose  :
// =======================================================================
int64_t Media_Packet::Dts() const
{
#ifdef HAVE_FFMPEG
  return myPacket->dts;
#else
  return 0;
#endif
}

// =======================================================================
// function : Duration
// purpose  :
// =======================================================================
int64_t Media_Packet::Duration() const
{
#ifdef HAVE_FFMPEG
  return myPacket->duration;
#else
  return 0;
#endif
}

// =======================================================================
// function : StreamIndex
// purpose  :
// =======================================================================
int Media_Packet::StreamIndex() const
{
#ifdef HAVE_FFMPEG
  return myPacket->stream_index;
#else
  return 0;
#endif
}

// =======================================================================
// function : IsKeyFrame
// purpose  :
// =======================================================================
bool Media_Packet::IsKeyFrame() const
{
#ifdef HAVE_FFMPEG
  return (myPacket->flags & AV_PKT_FLAG_KEY) != 0;
#else
  return false;
#endif
}

// =======================================================================
// function : SetKeyFrame
// purpose  :
// =======================================================================
void Media_Packet::SetKeyFrame()
{
#ifdef HAVE_FFMPEG
  myPacket->flags |= AV_PKT_FLAG_KEY;
#endif
}
