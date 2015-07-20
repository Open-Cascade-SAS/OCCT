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

#ifndef _D3DHost_Workspace_HeaderFile
#define _D3DHost_Workspace_HeaderFile

#include <d3d9.h>

#include <OpenGl_Workspace.hxx>
#include <D3DHost_FrameBuffer.hxx>

//! The D3D host containing OpenGL viewer.
class D3DHost_Workspace : public OpenGl_Workspace
{

public:

  //! Main constructor.
  Standard_EXPORT D3DHost_Workspace (const Handle(OpenGl_GraphicDriver)& theDriver,
                                     const CALL_DEF_WINDOW&              theCWindow,
                                     Aspect_RenderingContext             theGContext,
                                     const Handle(OpenGl_Caps)&          theCaps,
                                     const Handle(OpenGl_Context)&       theShareCtx);

  //! Destroy D3D connection and OpenGL workspace.
  Standard_EXPORT virtual ~D3DHost_Workspace();

  //! Resizes the window.
  Standard_EXPORT virtual void Resize (const CALL_DEF_WINDOW& theCWindow) Standard_OVERRIDE;

  //! Redraw the all content.
  Standard_EXPORT virtual void Redraw (const Graphic3d_CView& theCView,
                                       const Aspect_CLayer2d& theCUnderLayer,
                                       const Aspect_CLayer2d& theCOverLayer) Standard_OVERRIDE;

  //! Redraw only immediate layer.
  Standard_EXPORT virtual void RedrawImmediate (const Graphic3d_CView& theCView,
                                                const Aspect_CLayer2d& theCUnderLayer,
                                                const Aspect_CLayer2d& theCOverLayer) Standard_OVERRIDE;

public:

  //! @return true if IDirect3DDevice9Ex device has been created
  bool isD3dEx() const { return myIsD3dEx; }

  //! Access Direct3D device instance.
  IDirect3DDevice9* D3dDevice() const { return myD3dDevice; }

  //! Return D3D/WGL FBO.
  const Handle(D3DHost_FrameBuffer)& D3dWglBuffer() const { return myD3dWglFbo; }

protected:

  //! Auxiliary method.
  Standard_EXPORT static TCollection_AsciiString d3dFormatError (HRESULT theErrCode);

  //! Initialize the D3D library.
  Standard_EXPORT bool d3dInitLib();

  //! Initialize Direct3D output device.
  Standard_EXPORT bool d3dInit (const CALL_DEF_WINDOW& theCWindow);

  //! Reset Direct3D output settings. Could be used to switch windowed/fullscreen modes.
  //! Use very carefully! Most objects should be released before and recreated after!
  Standard_EXPORT bool d3dReset (const CALL_DEF_WINDOW& theCWindow);

  //! (Re-)create D3D surface.
  Standard_EXPORT bool d3dCreateRenderTarget();

  //! Starts the scene render.
  //! Clears the backbuffer and sets the device to start rendering to it.
  Standard_EXPORT void d3dBeginRender();

  //! Ends the scene render.
  Standard_EXPORT void d3dEndRender();

  //! Presents to the screen.
  Standard_EXPORT bool d3dSwap();

protected:

  IDirect3D9*                 myD3dLib;      //!< Direct3D library instance
  IDirect3DDevice9*           myD3dDevice;   //!< Direct3D device object
  D3DPRESENT_PARAMETERS       myD3dParams;   //!< parameters for created Direct3D device
  D3DDISPLAYMODE              myCurrMode;    //!< temporary variable
  UINT                        myRefreshRate; //!< refresh rate in fullscreen mode
  bool                        myIsD3dEx;     //!< D3dEx flag for WDDM
  Handle(D3DHost_FrameBuffer) myD3dWglFbo;   //!< D3D/WGL interop FBO

public:

  DEFINE_STANDARD_RTTI(D3DHost_Workspace, OpenGl_Workspace)

};

DEFINE_STANDARD_HANDLE(D3DHost_Workspace, OpenGl_Workspace)

#endif // _D3DHost_Workspace_HeaderFile
