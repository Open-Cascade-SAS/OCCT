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

#ifndef _D3DHost_FrameBuffer_HeaderFile
#define _D3DHost_FrameBuffer_HeaderFile

#include <OpenGl_FrameBuffer.hxx>

struct IDirect3DDevice9;
struct IDirect3DSurface9;

//! Implements bridge FBO for direct rendering to Direct3D surfaces.
class D3DHost_FrameBuffer : public OpenGl_FrameBuffer
{
public:
  //! Empty constructor.
  Standard_EXPORT D3DHost_FrameBuffer();

  //! Destructor, should be called after Release().
  Standard_EXPORT ~D3DHost_FrameBuffer();

  //! Releases D3D and OpenGL resources.
  Standard_EXPORT virtual void Release(OpenGl_Context* theCtx) Standard_OVERRIDE;

  //! Initializes OpenGL FBO for Direct3D interoperability or in fallback mode.
  //! Color pixel format is always GL_RGBA8/D3DFMT_X8R8G8B8, no MSAA; depth-stencil pixel format is
  //! GL_DEPTH24_STENCIL8.
  //! @param theGlCtx       currently bound OpenGL context
  //! @param theD3DDevice   d3d9 device
  //! @param theIsD3dEx     d3d9 extended flag (for creating shared texture resource)
  //! @param theSizeX       texture width
  //! @param theSizeY       texture height
  //! @return true on success
  Standard_EXPORT Standard_Boolean Init(const Handle(OpenGl_Context)& theCtx,
                                        IDirect3DDevice9*             theD3DDevice,
                                        const Standard_Boolean        theIsD3dEx,
                                        const Standard_Integer        theSizeX,
                                        const Standard_Integer        theSizeY);

  //! Initializes OpenGL FBO for Direct3D interoperability.
  //! Color pixel format is always GL_RGBA8/D3DFMT_X8R8G8B8, no MSAA.
  //! @param theGlCtx       currently bound OpenGL context
  //! @param theD3DDevice   d3d9 device
  //! @param theIsD3dEx     d3d9 extended flag (for creating shared texture resource)
  //! @param theSizeX       texture width
  //! @param theSizeY       texture height
  //! @param theDepthFormat depth-stencil texture sized format (0 means no depth attachment), e.g.
  //! GL_DEPTH24_STENCIL8
  //! @return true on success
  Standard_EXPORT Standard_Boolean InitD3dInterop(const Handle(OpenGl_Context)& theCtx,
                                                  IDirect3DDevice9*             theD3DDevice,
                                                  const Standard_Boolean        theIsD3dEx,
                                                  const Standard_Integer        theSizeX,
                                                  const Standard_Integer        theSizeY,
                                                  const Standard_Integer        theDepthFormat);

  //! Initializes OpenGL FBO + Direct3D surface for copying memory using fallback.
  //! Color pixel format is always GL_RGBA8/D3DFMT_X8R8G8B8, no MSAA.
  //! @param theGlCtx       currently bound OpenGL context
  //! @param theD3DDevice   d3d9 device
  //! @param theIsD3dEx     d3d9 extended flag (for creating shared texture resource)
  //! @param theSizeX       texture width
  //! @param theSizeY       texture height
  //! @param theDepthFormat depth-stencil texture sized format (0 means no depth attachment), e.g.
  //! GL_DEPTH24_STENCIL8
  //! @return true on success
  Standard_EXPORT Standard_Boolean InitD3dFallback(const Handle(OpenGl_Context)& theCtx,
                                                   IDirect3DDevice9*             theD3DDevice,
                                                   const Standard_Boolean        theIsD3dEx,
                                                   const Standard_Integer        theSizeX,
                                                   const Standard_Integer        theSizeY,
                                                   const Standard_Integer        theDepthFormat);

  //! Binds Direct3D color buffer to OpenGL texture.
  Standard_EXPORT Standard_Boolean registerD3dBuffer(const Handle(OpenGl_Context)& theCtx);

  //! Binds Direct3D objects for OpenGL drawing.
  //! Should be called before LockSurface() and followed by UnlockSurface();
  Standard_EXPORT virtual void BindBuffer(const Handle(OpenGl_Context)& theCtx) Standard_OVERRIDE;

  //! Acquires D3D resource for OpenGL usage.
  Standard_EXPORT virtual void LockSurface(const Handle(OpenGl_Context)& theCtx);

  //! Releases D3D resource.
  Standard_EXPORT virtual void UnlockSurface(const Handle(OpenGl_Context)& theCtx);

  //! Returns D3D surface used as color buffer.
  IDirect3DSurface9* D3dColorSurface() { return myD3dSurf; }

  //! Returns WDDM handle for D3D color surface.
  void* D3dColorSurfaceShare() { return myD3dSurfShare; }

  //! Returns TRUE if FBO has been initialized without WGL/D3D interop.
  Standard_Boolean D3dFallback() const { return myD3dFallback; }

  //! Returns TRUE if color buffer is sRGB ready; FALSE by default.
  //! Requires D3DSAMP_SRGBTEXTURE sampler parameter being set on D3D level for rendering D3D
  //! surface.
  Standard_Boolean IsSRGBReady() const { return myIsSRGBReady; }

  //! Set if color buffer is sRGB ready.
  void SetSRGBReady(Standard_Boolean theIsReady) { myIsSRGBReady = theIsReady; }

protected:
  using OpenGl_FrameBuffer::Init;

protected:
  IDirect3DSurface9* myD3dSurf;      //!< D3D surface
  void*              myD3dSurfShare; //!< D3D surface share handle in WDDM
  void*              myGlD3dDevice;  //!< WGL/D3D device  handle
  void*              myGlD3dSurf;    //!< WGL/D3D surface handle
  Standard_Integer   myLockCount;    //!< locking counter
                                     // clang-format off
  Standard_Boolean   myD3dFallback;  //!< indicates that FBO has been initialized without WGL/D3D interop
                                     // clang-format on
  Standard_Boolean myIsSRGBReady;    //!< indicates that color buffer is sRGB ready

public:
  DEFINE_STANDARD_RTTIEXT(D3DHost_FrameBuffer, OpenGl_FrameBuffer)
};

DEFINE_STANDARD_HANDLE(D3DHost_FrameBuffer, OpenGl_FrameBuffer)

#endif // _D3DHost_FrameBuffer_HeaderFile
