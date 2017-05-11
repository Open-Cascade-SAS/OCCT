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
: OpenGl_FrameBuffer(),
  myD3dSurf      (NULL),
  myD3dSurfShare (NULL),
  myGlD3dDevice  (NULL),
  myGlD3dSurf    (NULL)
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
  Release (theCtx.operator->());

  myVPSizeX = theSizeX;
  myVPSizeY = theSizeY;
  myInitVPSizeX = theSizeX;
  myInitVPSizeY = theSizeY;
  const Standard_Integer aSizeX = theSizeX > 0 ? theSizeX : 2;
  const Standard_Integer aSizeY = theSizeY > 0 ? theSizeY : 2;

  // Render target surface should be lockable on
  // Windows XP and non-lockable on Windows Vista or higher
  if (theD3DDevice->CreateRenderTarget (aSizeX, aSizeY,
                                        D3DFMT_X8R8G8B8, D3DMULTISAMPLE_NONE, 0, theIsD3dEx ? TRUE : FALSE,
                                        &myD3dSurf, theIsD3dEx ? &myD3dSurfShare : NULL) != D3D_OK)
  {
    TCollection_ExtendedString aMsg = TCollection_ExtendedString()
      + "D3DHost_FrameBuffer, could not D3DFMT_X8R8G8B8 render target " + aSizeX + "x" + aSizeY;
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                         GL_DEBUG_TYPE_ERROR_ARB,
                         0,
                         GL_DEBUG_SEVERITY_HIGH_ARB,
                         aMsg);
    return Standard_False;
  }

  const OpenGl_GlFunctions* aFuncs = theCtx->Functions();
  if (aFuncs->wglDXOpenDeviceNV == NULL)
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                         GL_DEBUG_TYPE_ERROR_ARB,
                         0,
                         GL_DEBUG_SEVERITY_HIGH_ARB,
                         "D3DHost_FrameBuffer, WGL_NV_DX_interop is unavailable!");
    return Standard_False;
  }

  myGlD3dDevice = aFuncs->wglDXOpenDeviceNV (theD3DDevice);
  if (myGlD3dDevice == NULL)
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                         GL_DEBUG_TYPE_ERROR_ARB,
                         0,
                         GL_DEBUG_SEVERITY_HIGH_ARB,
                         "D3DHost_FrameBuffer, could not create the GL <-> DirectX Interop using wglDXOpenDeviceNV()");
    return Standard_False;
  }

  if (!registerD3dBuffer (theCtx))
  {
    return Standard_False;
  }

  theCtx->arbFBO->glGenFramebuffers (1, &myGlFBufferId);

  if (!myDepthStencilTexture->Init (theCtx, GL_DEPTH24_STENCIL8,
                                    GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8,
                                    aSizeX, aSizeY, Graphic3d_TOT_2D))
  {
    TCollection_ExtendedString aMsg = TCollection_ExtendedString()
      + "D3DHost_FrameBuffer, could not initialize GL_DEPTH24_STENCIL8 texture " + aSizeX + "x" + aSizeY;
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                         GL_DEBUG_TYPE_ERROR_ARB,
                         0,
                         GL_DEBUG_SEVERITY_HIGH_ARB,
                         aMsg);
    return Standard_False;
  }

  return Standard_True;
}

// =======================================================================
// function : registerD3dBuffer
// purpose  :
// =======================================================================
Standard_Boolean D3DHost_FrameBuffer::registerD3dBuffer (const Handle(OpenGl_Context)& theCtx)
{
  const OpenGl_GlFunctions* aFuncs = theCtx->Functions();
  if (myGlD3dSurf != NULL)
  {
    if (!aFuncs->wglDXUnregisterObjectNV (myGlD3dDevice, myGlD3dSurf))
    {
      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                           GL_DEBUG_TYPE_ERROR_ARB,
                           0,
                           GL_DEBUG_SEVERITY_HIGH_ARB,
                           "D3DHost_FrameBuffer, can not unregister color buffer");
      return Standard_False;
    }
    myGlD3dSurf = NULL;
  }

  if (!aFuncs->wglDXSetResourceShareHandleNV (myD3dSurf, myD3dSurfShare))
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                         GL_DEBUG_TYPE_ERROR_ARB,
                         0,
                         GL_DEBUG_SEVERITY_HIGH_ARB,
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

  if (myGlD3dSurf == NULL)
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                         GL_DEBUG_TYPE_ERROR_ARB,
                         0,
                         GL_DEBUG_SEVERITY_HIGH_ARB,
                         "D3DHost_FrameBuffer, can not register color buffer");
    return Standard_False;
  }

  return Standard_True;
}

// =======================================================================
// function : BindBuffer
// purpose  :
// =======================================================================
void D3DHost_FrameBuffer::BindBuffer (const Handle(OpenGl_Context)& theCtx)
{
  Standard_ProgramError_Raise_if (myLockCount < 1, "D3DHost_FrameBuffer::BindBuffer(), resource should be locked beforehand!");

  OpenGl_FrameBuffer::BindBuffer (theCtx);
  theCtx->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                          GL_TEXTURE_2D, myColorTextures (0)->TextureId(), 0);
  theCtx->arbFBO->glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                          GL_TEXTURE_2D, myDepthStencilTexture->TextureId(), 0);
  if (theCtx->arbFBO->glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                         GL_DEBUG_TYPE_ERROR_ARB,
                         0,
                         GL_DEBUG_SEVERITY_HIGH_ARB,
                         "D3DHost_FrameBuffer, OpenGL FBO is incomplete!");
    Release (theCtx.operator->());
  }
}

// =======================================================================
// function : LockSurface
// purpose  :
// =======================================================================
void D3DHost_FrameBuffer::LockSurface (const Handle(OpenGl_Context)& theCtx)
{
  if (myGlD3dSurf == NULL)
  {
    return;
  }

  if (++myLockCount > 1)
  {
    return;
  }

  const OpenGl_GlFunctions* aFuncs = theCtx->Functions();
  if (!aFuncs->wglDXLockObjectsNV (myGlD3dDevice, 1, &myGlD3dSurf))
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                         GL_DEBUG_TYPE_ERROR_ARB,
                         0,
                         GL_DEBUG_SEVERITY_HIGH_ARB,
                         "D3DHost_FrameBuffer::LockSurface(), lock failed!");
  }
}

// =======================================================================
// function : UnlockSurface
// purpose  :
// =======================================================================
void D3DHost_FrameBuffer::UnlockSurface (const Handle(OpenGl_Context)& theCtx)
{
  if (myGlD3dSurf == NULL)
  {
    return;
  }

  if (--myLockCount != 0)
  {
    return;
  }

  const OpenGl_GlFunctions* aFuncs = theCtx->Functions();
  aFuncs->wglDXUnlockObjectsNV (myGlD3dDevice, 1, &myGlD3dSurf);
}
