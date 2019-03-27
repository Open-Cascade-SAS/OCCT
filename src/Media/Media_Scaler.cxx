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

#include <Media_Scaler.hxx>

#ifdef HAVE_FFMPEG
#include <Standard_WarningsDisable.hxx>
extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavutil/imgutils.h>
  #include <libswscale/swscale.h>
};
#include <Standard_WarningsRestore.hxx>
#endif

IMPLEMENT_STANDARD_RTTIEXT(Media_Scaler, Standard_Transient)

// =======================================================================
// function : Media_Scaler
// purpose  :
// =======================================================================
Media_Scaler::Media_Scaler()
: mySwsContext (NULL),
  mySrcFormat (0),
  myResFormat (0)
{
#ifdef HAVE_FFMPEG
  mySrcFormat = AV_PIX_FMT_NONE;
  myResFormat = AV_PIX_FMT_NONE;
#endif
}

// =======================================================================
// function : ~Media_Scaler
// purpose  :
// =======================================================================
Media_Scaler::~Media_Scaler()
{
  Release();
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void Media_Scaler::Release()
{
  if (mySwsContext != NULL)
  {
  #ifdef HAVE_FFMPEG
    sws_freeContext (mySwsContext);
  #endif
    mySwsContext = NULL;
  }
}

// =======================================================================
// function : Convert
// purpose  :
// =======================================================================
bool Media_Scaler::Init (const Graphic3d_Vec2i& theSrcDims,
                         int theSrcFormat,
                         const Graphic3d_Vec2i& theResDims,
                         int theResFormat)
{
#ifdef HAVE_FFMPEG
  if (theSrcDims.x() < 1
   || theSrcDims.y() < 1
   || theResDims.x() < 1
   || theResDims.y() < 1
   || theSrcFormat == AV_PIX_FMT_NONE
   || theResFormat == AV_PIX_FMT_NONE)
  {
    Release();
    return false;
  }
  else if (mySrcDims   == theSrcDims
        && myResDims   == theResDims
        && mySrcFormat == theSrcFormat
        && myResFormat == theResFormat)
  {
    return mySwsContext != NULL;
  }

  Release();
  mySrcDims    = theSrcDims;
  myResDims    = theResDims;
  mySrcFormat  = theSrcFormat;
  myResFormat  = theResFormat;
  mySwsContext = sws_getContext (theSrcDims.x(), theSrcDims.y(), (AVPixelFormat )theSrcFormat,
                                 theResDims.x(), theResDims.y(), (AVPixelFormat )theResFormat,
                                 SWS_BICUBIC, NULL, NULL, NULL);
  return mySwsContext != NULL;
#else
  (void )theSrcDims;
  (void )theSrcFormat;
  (void )theResDims;
  (void )theResFormat;
  return false;
#endif
}

// =======================================================================
// function : Convert
// purpose  :
// =======================================================================
bool Media_Scaler::Convert (const Handle(Media_Frame)& theSrc,
                            const Handle(Media_Frame)& theRes)
{
  if (theSrc.IsNull()
   || theSrc->IsEmpty()
   || theRes.IsNull()
   || theRes->IsEmpty()
   || theSrc == theRes)
  {
    return false;
  }

  if (!Init (theSrc->Size(), theSrc->Format(),
             theRes->Size(), theRes->Format()))
  {
    return false;
  }

#ifdef HAVE_FFMPEG
  sws_scale (mySwsContext,
             theSrc->Frame()->data, theSrc->Frame()->linesize,
             0, theSrc->SizeY(),
             theRes->ChangeFrame()->data, theRes->Frame()->linesize);
  return true;
#else
  return false;
#endif
}
