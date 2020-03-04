// Created on: 2015-06-10
// Created by: Kirill Gavrilov
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <d3d9.h>

#include <D3DHost_FrameBuffer.hxx>

#include <OpenGl_GlCore20.hxx>
#include <OpenGl_ArbFBO.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(D3DHost_FrameBuffer,OpenGl_FrameBuffer)

// =======================================================================
// function : D3DHost_FrameBuffer
// purpose  :
// =======================================================================
D3DHost_FrameBuffer::D3DHost_FrameBuffer()
: myD3dSurf      (NULL),
  myD3dSurfShare (NULL),
  myGlD3dDevice  (NULL),
  myGlD3dSurf    (NULL),
  myLockCount    (0),
  myD3dFallback  (Standard_False),
  myIsSRGBReady  (Standard_False)
{
  //
}

// =======================================================================
// function : ~D3DHost_FrameBuffer
// purpose  :
// =======================================================================
D3DHost_FrameBuffer::~D3DHost_FrameBuffer()
{
  Release (NULL);
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void D3DHost_FrameBuffer::Release (OpenGl_Context* theCtx)
{
#if !defined(GL_ES_VERSION_2_0)
  if (myGlD3dDevice != NULL)
  {
    const OpenGl_GlFunctions* aFuncs = (theCtx != NULL && theCtx->IsValid())
                                     ? theCtx->Functions()
                                     : NULL;
    if (myGlD3dSurf != NULL)
    {
      if (aFuncs != NULL)
      {
        aFuncs->wglDXUnregisterObjectNV (myGlD3dDevice, myGlD3dSurf);
      }
      myGlD3dSurf = NULL;
    }

    if (aFuncs != NULL)
    {
      aFuncs->wglDXCloseDeviceNV (myGlD3dDevice);
    }
    myGlD3dDevice = NULL;
  }
#endif

  if (myD3dSurf != NULL)
  {
    myD3dSurf->Release();
    myD3dSurf      = NULL;
    myD3dSurfShare = NULL;
  }

  OpenGl_FrameBuffer::Release (theCtx);
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean D3DHost_FrameBuffer::Init (const Handle(OpenGl_Context)& theCtx,
                                            IDirect3DDevice9*             theD3DDevice,
                                            const Standard_Boolean        theIsD3dEx,
                                            const Standard_Integer        theSizeX,
                                            const Standard_Integer        theSizeY)
{
  if (InitD3dInterop (theCtx, theD3DDevice, theIsD3dEx, theSizeX, theSizeY, GL_DEPTH24_STENCIL8))
  {
    return Standard_True;
  }
  return InitD3dFallback (theCtx, theD3DDevice, theIsD3dEx, theSizeX, theSizeY, GL_DEPTH24_STENCIL8);
}

// =======================================================================
// function : InitD3dFallback
// purpose  :
// =======================================================================
Standard_Boolean D3DHost_FrameBuffer::InitD3dFallback (const Handle(OpenGl_Context)& theCtx,
                                                       IDirect3DDevice9*             theD3DDevice,
                                                       const Standard_Boolean        theIsD3dEx,
                                                       const Standard_Integer        theSizeX,
                                                       const Standard_Integer        theSizeY,
                                                       const GLint                   theDepthFormat)
{
  const Standard_Boolean isGlInit = OpenGl_FrameBuffer::Init (theCtx, theSizeX, theSizeY, GL_RGBA8, theDepthFormat, 0);
  myD3dFallback = Standard_True;

  const Standard_Integer aSizeX = theSizeX > 0 ? theSizeX : 2;
  const Standard_Integer aSizeY = theSizeY > 0 ? theSizeY : 2;
  if (theD3DDevice->CreateRenderTarget (aSizeX, aSizeY,
                                        D3DFMT_X8R8G8B8, D3DMULTISAMPLE_NONE, 0, theIsD3dEx ? TRUE : FALSE,
                                        &myD3dSurf, theIsD3dEx ? &myD3dSurfShare : NULL) != D3D_OK)
  {
    Release (theCtx.operator->());
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         TCollection_AsciiString ("D3DHost_FrameBuffer, could not create D3DFMT_X8R8G8B8 render target ") + aSizeX + "x" + aSizeY);
    return Standard_False;
  }
  return isGlInit;
}

// =======================================================================
// function : InitD3dInterop
// purpose  :
// =======================================================================
Standard_Boolean D3DHost_FrameBuffer::InitD3dInterop (const Handle(OpenGl_Context)& theCtx,
                                                      IDirect3DDevice9*             theD3DDevice,
                                                      const Standard_Boolean        theIsD3dEx,
                                                      const Standard_Integer        theSizeX,
                                                      const Standard_Integer        theSizeY,
                                                      const GLint                   theDepthFormat)
{
  Release (theCtx.operator->());
#if !defined(GL_ES_VERSION_2_0)
  myDepthFormat = theDepthFormat;
  myVPSizeX = theSizeX;
  myVPSizeY = theSizeY;
  myInitVPSizeX = theSizeX;
  myInitVPSizeY = theSizeY;
  const Standard_Integer aSizeX = theSizeX > 0 ? theSizeX : 2;
  const Standard_Integer aSizeY = theSizeY > 0 ? theSizeY : 2;

  const OpenGl_GlFunctions* aFuncs = theCtx->Functions();
  if (aFuncs->wglDXOpenDeviceNV == NULL)
  {
    Release (theCtx.operator->());
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         "D3DHost_FrameBuffer, WGL_NV_DX_interop is unavailable!");
    return Standard_False;
  }

  // Render target surface should be lockable on
  // Windows XP and non-lockable on Windows Vista or higher
  if (theD3DDevice->CreateRenderTarget (aSizeX, aSizeY,
                                        D3DFMT_X8R8G8B8, D3DMULTISAMPLE_NONE, 0, theIsD3dEx ? TRUE : FALSE,
                                        &myD3dSurf, theIsD3dEx ? &myD3dSurfShare : NULL) != D3D_OK)
  {
    Release (theCtx.operator->());
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         TCollection_AsciiString ("D3DHost_FrameBuffer, could not create D3DFMT_X8R8G8B8 render target ") + aSizeX + "x" + aSizeY);
    return Standard_False;
  }

  myGlD3dDevice = aFuncs->wglDXOpenDeviceNV (theD3DDevice);
  if (myGlD3dDevice == NULL)
  {
    Release (theCtx.operator->());
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         "D3DHost_FrameBuffer, could not create the GL <-> DirectX Interop using wglDXOpenDeviceNV()");
    return Standard_False;
  }

  if (!registerD3dBuffer (theCtx))
  {
    Release (theCtx.operator->());
    return Standard_False;
  }

  myIsOwnBuffer = true;
  myIsOwnDepth  = true;
  theCtx->arbFBO->glGenFramebuffers (1, &myGlFBufferId);

  const OpenGl_TextureFormat aDepthFormat = OpenGl_TextureFormat::FindSizedFormat (theCtx, myDepthFormat);
  if (aDepthFormat.IsValid()
  && !myDepthStencilTexture->Init (theCtx, aDepthFormat, Graphic3d_Vec2i (aSizeX, aSizeY), Graphic3d_TOT_2D))
  {
    Release (theCtx.get());
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         TCollection_AsciiString("D3DHost_FrameBuffer, could not initialize GL_DEPTH24_STENCIL8 texture ") + aSizeX + "x" + aSizeY);
    return Standard_False;
  }

  myD3dFallback = Standard_False;
  return Standard_True;
#else
  (void )theD3DDevice;
  (void )theIsD3dEx;
  (void )theSizeX;
  (void )theSizeY;
  (void )theDepthFormat;
  return Standard_False;
#endif
}

// =======================================================================
// function : registerD3dBuffer
// purpose  :
// =======================================================================
Standard_Boolean D3DHost_FrameBuffer::registerD3dBuffer (const Handle(OpenGl_Context)& theCtx)
{
#if defined(GL_ES_VERSION_2_0)
  (void )theCtx;
  return Standard_False;
#else
  const OpenGl_GlFunctions* aFuncs = theCtx->Functions();
  if (myGlD3dSurf != NULL)
  {
    if (!aFuncs->wglDXUnregisterObjectNV (myGlD3dDevice, myGlD3dSurf))
    {
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                           "D3DHost_FrameBuffer, can not unregister color buffer");
      return Standard_False;
    }
    myGlD3dSurf = NULL;
  }

  if (!aFuncs->wglDXSetResourceShareHandleNV (myD3dSurf, myD3dSurfShare))
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         "D3DHost_FrameBuffer, wglDXSetResourceShareHandleNV() has failed");
    return Standard_False;
  }

  myColorTextures (0)->Release (theCtx.operator->());
  myColorTextures (0)->Create  (theCtx);

  myGlD3dSurf = aFuncs->wglDXRegisterObjectNV (myGlD3dDevice,
                                               myD3dSurf,
                                               myColorTextures (0)->TextureId(),
                                               GL_TEXTURE_2D,
                                               WGL_ACCESS_WRITE_DISCARD_NV);
  theCtx->ResetErrors (true);
  if (myGlD3dSurf == NULL)
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         "D3DHost_FrameBuffer, can not register color buffer");
    return Standard_False;
  }

  return Standard_True;
#endif
}

// =======================================================================
// function : BindBuffer
// purpose  :
// =======================================================================
void D3DHost_FrameBuffer::BindBuffer (const Handle(OpenGl_Context)& theCtx)
{
  Standard_ProgramError_Raise_if (myLockCount < 1, "D3DHost_FrameBuffer::BindBuffer(), resource should be locked beforehand!");
  if (theCtx->arbFBO == NULL)
  {
    return;
  }

  OpenGl_FrameBuffer::BindBuffer (theCtx);
  theCtx->SetFrameBufferSRGB (true, myIsSRGBReady);
  if (myD3dFallback)
  {
    return;
  }

  theCtx->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                          myColorTextures (0)->GetTarget(), myColorTextures (0)->TextureId(), 0);

  const OpenGl_TextureFormat aDepthFormat = OpenGl_TextureFormat::FindSizedFormat (theCtx, myDepthFormat);
  if (myDepthStencilTexture->IsValid())
  {
  #ifdef GL_DEPTH_STENCIL_ATTACHMENT
    theCtx->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, aDepthFormat.PixelFormat() == GL_DEPTH_STENCIL ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT,
                                            myDepthStencilTexture->GetTarget(), myDepthStencilTexture->TextureId(), 0);
  #else
    theCtx->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                            myDepthStencilTexture->GetTarget(), myDepthStencilTexture->TextureId(), 0);
    if (aDepthFormat.PixelFormat() == GL_DEPTH_STENCIL)
    {
      theCtx->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                              myDepthStencilTexture->GetTarget(), myDepthStencilTexture->TextureId(), 0);
    }
  #endif
  }
  if (theCtx->arbFBO->glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    if (myDepthStencilTexture->IsValid())
    {
    #ifdef GL_DEPTH_STENCIL_ATTACHMENT
      theCtx->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, aDepthFormat.PixelFormat() == GL_DEPTH_STENCIL ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT,
                                              myDepthStencilTexture->GetTarget(), 0, 0);
    #else
      theCtx->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                              myDepthStencilTexture->GetTarget(), 0, 0);
      if (aDepthFormat.PixelFormat() == GL_DEPTH_STENCIL)
      {
        theCtx->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                                myDepthStencilTexture->GetTarget(), 0, 0);
      }
    #endif
    }
    if (theCtx->arbFBO->glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                           "D3DHost_FrameBuffer, OpenGL FBO is incomplete!");
      Release (theCtx.operator->());
    }
    else
    {
      myDepthFormat = 0;
      myDepthStencilTexture->Release (theCtx.get());
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH,
                           "D3DHost_FrameBuffer, OpenGL FBO is created without Depth+Stencil attachements!");
    }
  }
}

// =======================================================================
// function : LockSurface
// purpose  :
// =======================================================================
void D3DHost_FrameBuffer::LockSurface (const Handle(OpenGl_Context)& theCtx)
{
  if (++myLockCount > 1)
  {
    return;
  }
  if (myGlD3dSurf == NULL)
  {
    return;
  }

#if !defined(GL_ES_VERSION_2_0)
  const OpenGl_GlFunctions* aFuncs = theCtx->Functions();
  if (!aFuncs->wglDXLockObjectsNV (myGlD3dDevice, 1, &myGlD3dSurf))
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                         "D3DHost_FrameBuffer::LockSurface(), lock failed!");
  }
#else
  (void )theCtx;
#endif
}

// =======================================================================
// function : UnlockSurface
// purpose  :
// =======================================================================
void D3DHost_FrameBuffer::UnlockSurface (const Handle(OpenGl_Context)& theCtx)
{
  if (--myLockCount != 0)
  {
    return;
  }

  if (myD3dFallback)
  {
    if (myD3dSurf == NULL)
    {
      return;
    }

    D3DLOCKED_RECT aLockedRect;
    if (myD3dSurf->LockRect (&aLockedRect, NULL, 0) != 0)
    {
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                           "D3DHost_FrameBuffer::UnlockSurface(), lock failed!");
      return;
    }

    Image_PixMap anImg;
    if (anImg.InitWrapper (Image_Format_BGRA, (Standard_Byte* )aLockedRect.pBits, myInitVPSizeX, myInitVPSizeY, aLockedRect.Pitch))
    {
      anImg.SetTopDown (!IsValid()); // flip in software if OpenGL FBO is unavailable
      myLockCount = 1;
      if (!BufferDump (theCtx, this, anImg, Graphic3d_BT_RGBA))
      {
        theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                             "D3DHost_FrameBuffer::UnlockSurface(), buffer dump failed!");
      }
      myLockCount = 0;
    }
    else
    {
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                           "D3DHost_FrameBuffer::UnlockSurface(), buffer dump failed!");
    }
    myD3dSurf->UnlockRect();
    return;
  }
  if (myGlD3dSurf == NULL)
  {
    return;
  }

#if !defined(GL_ES_VERSION_2_0)
  const OpenGl_GlFunctions* aFuncs = theCtx->Functions();
  aFuncs->wglDXUnlockObjectsNV (myGlD3dDevice, 1, &myGlD3dSurf);
#endif
}
