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

#ifndef _D3DHost_View_HeaderFile
#define _D3DHost_View_HeaderFile

#include <D3DHost_FrameBuffer.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

class D3DHost_GraphicDriver;
struct IDirect3D9;
struct IDirect3DDevice9;
typedef struct _D3DPRESENT_PARAMETERS_ D3DPRESENT_PARAMETERS;

//! The D3D host view implementation that overrides rendering methods.
class D3DHost_View : public OpenGl_View
{

public:

  //! Constructor.
  Standard_EXPORT D3DHost_View (const Handle(Graphic3d_StructureManager)& theMgr,
                                const Handle(D3DHost_GraphicDriver)& theDriver,
                                const Handle(OpenGl_Caps)& theCaps,
                                OpenGl_StateCounter* theCounter);

  //! Default destructor.
  Standard_EXPORT virtual ~D3DHost_View();

  //! Creates and maps rendering window to the view.
  //! @param theWindow [in] the window.
  //! @param theContext [in] the rendering context. If NULL the context will be created internally.
  //! @param theDisplayCB [in] the display callback function. If is not a NULL value, then the callback will be
  //! invoked at the end of the OCC graphic traversal and just before the swap of buffers.
  //! @param theClientData [in] the client data for the callback.
  Standard_EXPORT virtual void SetWindow (const Handle(Aspect_Window)&  theWindow,
                                          const Aspect_RenderingContext theContext) Standard_OVERRIDE;

  //! Resizes the window.
  Standard_EXPORT virtual void Resized() Standard_OVERRIDE;

  //! Redraw the all content.
  Standard_EXPORT virtual void Redraw() Standard_OVERRIDE;

  //! Redraw only immediate layer.
  Standard_EXPORT virtual void RedrawImmediate() Standard_OVERRIDE;

public:

  //! @return true if IDirect3DDevice9Ex device has been created
  bool isD3dEx() const { return myIsD3dEx; }

  //! Access Direct3D device instance.
  IDirect3DDevice9* D3dDevice() const { return myD3dDevice; }

  //! Return D3D/WGL FBO.
  const Handle(D3DHost_FrameBuffer)& D3dWglBuffer() const { return myD3dWglFbo; }

  //! Return D3D surface.
  IDirect3DSurface9* D3dColorSurface() const { return myD3dWglFbo->D3dColorSurface(); }

protected:

  //! Auxiliary method.
  Standard_EXPORT static TCollection_AsciiString d3dFormatError (const long theErrCode);

  //! Initialize the D3D library.
  Standard_EXPORT bool d3dInitLib();

  //! Initialize Direct3D output device.
  Standard_EXPORT bool d3dInit();

  //! Reset Direct3D output settings. Could be used to switch windowed/fullscreen modes.
  //! Use very carefully! Most objects should be released before and recreated after!
  Standard_EXPORT bool d3dReset();

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
  NCollection_Handle<D3DPRESENT_PARAMETERS>
                              myD3dParams;   //!< parameters for created Direct3D device
  unsigned int                myRefreshRate; //!< refresh rate in fullscreen mode
  bool                        myIsD3dEx;     //!< D3dEx flag for WDDM
  Handle(D3DHost_FrameBuffer) myD3dWglFbo;   //!< D3D/WGL interop FBO

public:

  DEFINE_STANDARD_RTTIEXT(D3DHost_View,OpenGl_View)

};

DEFINE_STANDARD_HANDLE(D3DHost_View, OpenGl_View)

#endif // #ifndef _D3DHost_View_HeaderFile
