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

#include <Media_BufferPool.hxx>

#include <Media_Frame.hxx>

#ifdef HAVE_FFMPEG
  #include <Standard_WarningsDisable.hxx>
extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavutil/imgutils.h>
};
  #include <Standard_WarningsRestore.hxx>
#endif

IMPLEMENT_STANDARD_RTTIEXT(Media_BufferPool, Standard_Transient)

// =======================================================================
// function : Media_BufferPool
// purpose  :
// =======================================================================
Media_BufferPool::Media_BufferPool()
    : myPool(NULL),
      myBufferSize(0)
{
  //
}

// =======================================================================
// function : ~Media_BufferPool
// purpose  :
// =======================================================================
Media_BufferPool::~Media_BufferPool()
{
  Release();
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void Media_BufferPool::Release()
{
  if (myPool != NULL)
  {
#ifdef HAVE_FFMPEG
    av_buffer_pool_uninit(&myPool);
#endif
    myPool       = NULL;
    myBufferSize = 0;
  }
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
bool Media_BufferPool::Init(int theBufferSize)
{
  if (myBufferSize == theBufferSize)
  {
    return true;
  }

  Release();
  if (theBufferSize == 0)
  {
    return true;
  }

#ifdef HAVE_FFMPEG
  myPool = av_buffer_pool_init(theBufferSize, NULL);
#endif
  myBufferSize = theBufferSize;
  return myPool != NULL;
}

// =======================================================================
// function : GetBuffer
// purpose  :
// =======================================================================
AVBufferRef* Media_BufferPool::GetBuffer()
{
#ifdef HAVE_FFMPEG
  return av_buffer_pool_get(myPool);
#else
  return NULL;
#endif
}
