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

#include <Graphic3d_MediaTexture.hxx>

#include <Graphic3d_TextureParams.hxx>
#include <Media_Frame.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>

#ifdef HAVE_FFMPEG
  #include <Standard_WarningsDisable.hxx>
extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavutil/imgutils.h>
};
  #include <Standard_WarningsRestore.hxx>
#endif

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_MediaTexture, Graphic3d_Texture2D)

// ================================================================
// Function : Graphic3d_MediaTexture
// Purpose  :
// ================================================================
Graphic3d_MediaTexture::Graphic3d_MediaTexture(const Handle(Standard_HMutex)& theMutex,
                                               Standard_Integer               thePlane)
    : Graphic3d_Texture2D("", Graphic3d_TypeOfTexture_2D),
      myMutex(theMutex),
      myPlane(thePlane)
{
  myParams->SetModulate(false);
  myParams->SetRepeat(false);
  myParams->SetFilter(Graphic3d_TOTF_BILINEAR);
  myParams->SetTextureUnit(Graphic3d_TextureUnit(int(Graphic3d_TextureUnit_0) + thePlane));
}

// ================================================================
// Function : GetImage
// Purpose  :
// ================================================================
Handle(Image_PixMap) Graphic3d_MediaTexture::GetImage(const Handle(Image_SupportedFormats)&)
{
  Standard_Mutex::Sentry aLock(myMutex.get());
  if (myFrame.IsNull() || myFrame->IsLocked() || myFrame->IsEmpty() || myFrame->SizeX() < 1
      || myFrame->SizeY() < 1)
  {
    return Handle(Image_PixMap)();
  }

  if (myPixMapWrapper.IsNull())
  {
    myPixMapWrapper = new Image_PixMap();
  }

#ifdef HAVE_FFMPEG
  const AVFrame*     aFrame    = myFrame->Frame();
  const Image_Format anOcctFmt = Media_Frame::FormatFFmpeg2Occt(myFrame->Format());
  if (anOcctFmt != Image_Format_UNKNOWN)
  {
    if (myPlane != 0
        || !myPixMapWrapper->InitWrapper(anOcctFmt,
                                         aFrame->data[0],
                                         aFrame->width,
                                         aFrame->height,
                                         aFrame->linesize[0]))
    {
      return Handle(Image_PixMap)();
    }
    return myPixMapWrapper;
  }
  else if (myFrame->Format() == AV_PIX_FMT_YUV420P || myFrame->Format() == AV_PIX_FMT_YUVJ420P)
  {
    const Graphic3d_Vec2i aSize = myPlane == 0 ? myFrame->Size() : myFrame->Size() / 2;
    if (myPlane > 3
        || !myPixMapWrapper->InitWrapper(Image_Format_Gray,
                                         aFrame->data[myPlane],
                                         aSize.x(),
                                         aSize.y(),
                                         aFrame->linesize[myPlane]))
    {
      return Handle(Image_PixMap)();
    }
    return myPixMapWrapper;
  }
#endif

  return Handle(Image_PixMap)();
}
