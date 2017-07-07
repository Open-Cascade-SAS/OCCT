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

#include <D3DHost_View.hxx>

#include <D3DHost_GraphicDriver.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(D3DHost_View,OpenGl_View)

// =======================================================================
// function : d3dFormatError
// purpose  :
// =======================================================================
TCollection_AsciiString D3DHost_View::d3dFormatError (const long theErrCode)
{
  switch (theErrCode)
  {
    case D3D_OK:                     return "OK";
    case D3DERR_DEVICELOST:          return "Device lost";
    case D3DERR_DEVICEREMOVED:       return "Device removed";
    case D3DERR_DRIVERINTERNALERROR: return "Driver internal error";
    case D3DERR_OUTOFVIDEOMEMORY:    return "Out of video memory";
    case D3DERR_INVALIDCALL:         return "Invalid call";
    default:                         return TCollection_AsciiString ("Error #") + int(theErrCode) + ")";
  }
}

// =======================================================================
// function : D3DHost_View
// purpose  :
// =======================================================================
D3DHost_View::D3DHost_View (const Handle(Graphic3d_StructureManager)& theMgr,
                            const Handle(D3DHost_GraphicDriver)& theDriver,
                            const Handle(OpenGl_Caps)& theCaps,
                            OpenGl_StateCounter* theCounter)
: OpenGl_View (theMgr, theDriver, theCaps, theCounter),
  myD3dLib      (NULL),
  myD3dDevice   (NULL),
  myD3dParams   (new D3DPRESENT_PARAMETERS()),
  myRefreshRate (D3DPRESENT_RATE_DEFAULT),
  myIsD3dEx     (false)
{
  memset(myD3dParams.operator->(), 0, sizeof(D3DPRESENT_PARAMETERS));

  myD3dParams->Windowed         = TRUE;
  myD3dParams->SwapEffect       = D3DSWAPEFFECT_DISCARD;
  myD3dParams->BackBufferFormat = D3DFMT_X8R8G8B8;
  myD3dParams->BackBufferCount  = 1;
  myD3dParams->BackBufferHeight = 2;
  myD3dParams->BackBufferWidth  = 2;
  myD3dParams->EnableAutoDepthStencil     = FALSE;
  myD3dParams->AutoDepthStencilFormat     = D3DFMT_D16_LOCKABLE;
  myD3dParams->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
  myD3dParams->PresentationInterval       = D3DPRESENT_INTERVAL_DEFAULT;
}

// =======================================================================
// function : ~D3DHost_View
// purpose  :
// =======================================================================
D3DHost_View::~D3DHost_View()
{
  if (!myD3dWglFbo.IsNull())
  {
    myD3dWglFbo->Release (myWorkspace->GetGlContext().operator->());
    myD3dWglFbo.Nullify();
  }
  if (myD3dDevice != NULL)
  {
    myD3dDevice->Release();
    myD3dDevice = NULL;
  }
  if (myD3dLib != NULL)
  {
    myD3dLib->Release();
    myD3dLib = NULL;
  }
}

// =======================================================================
// function : SetWindow
// purpose  :
// =======================================================================
void D3DHost_View::SetWindow (const Handle(Aspect_Window)& theWindow,
                              const Aspect_RenderingContext theContext)
{
  if (!myD3dWglFbo.IsNull())
  {
    myD3dWglFbo->Release (myWorkspace->GetGlContext().operator->());
    myD3dWglFbo.Nullify();
  }
  if (myD3dDevice != NULL)
  {
    myD3dDevice->Release();
    myD3dDevice = NULL;
  }

  OpenGl_View::SetWindow (theWindow, theContext);

  if (!myWindow.IsNull())
  {
    d3dInit();
    d3dCreateRenderTarget();
  }
}

// =======================================================================
// function : d3dInitLib
// purpose  :
// =======================================================================
bool D3DHost_View::d3dInitLib()
{
  if (myD3dLib == NULL)
  {
    IDirect3D9Ex* aD3dLibEx = NULL;
    // we link against d3d (using Direct3DCreate9 symbol), thus it should be already loaded
    HMODULE aLib = GetModuleHandleW (L"d3d9");
    if (aLib != NULL)
    {
      // retrieve D3D9Ex function dynamically (available only since Vista+)
      typedef HRESULT (WINAPI* Direct3DCreate9Ex_t)(UINT , IDirect3D9Ex** );
      Direct3DCreate9Ex_t Direct3DCreate9ExProc = (Direct3DCreate9Ex_t )GetProcAddress (aLib, "Direct3DCreate9Ex");
      if (Direct3DCreate9ExProc != NULL)
      {
        Direct3DCreate9ExProc(D3D_SDK_VERSION, &aD3dLibEx);
      }
    }
    myD3dLib  = aD3dLibEx;
    myIsD3dEx = aD3dLibEx != NULL;
    if (myD3dLib == NULL)
    {
      myD3dLib = Direct3DCreate9 (D3D_SDK_VERSION);
    }
  }
  return myD3dLib != NULL;
}

// =======================================================================
// function : d3dInit
// purpose  :
// =======================================================================
bool D3DHost_View::d3dInit()
{
  if (!d3dInitLib())
  {
    myWorkspace->GetGlContext()->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                                              GL_DEBUG_TYPE_ERROR_ARB,
                                              0,
                                              GL_DEBUG_SEVERITY_HIGH_ARB,
                                              "Direct3DCreate9 failed!");
    return false;
  }

  UINT anAdapterId = D3DADAPTER_DEFAULT;

  // setup the present parameters
  D3DDISPLAYMODE aCurrMode;
  memset(&aCurrMode, 0, sizeof(aCurrMode));
  if (myD3dLib->GetAdapterDisplayMode (anAdapterId, &aCurrMode) == D3D_OK)
  {
    myD3dParams->BackBufferFormat = aCurrMode.Format;
    myRefreshRate = aCurrMode.RefreshRate;
  }
  myD3dParams->Windowed         = TRUE;
  myD3dParams->BackBufferWidth  = myWindow->Width();
  myD3dParams->BackBufferHeight = myWindow->Height();
  myD3dParams->hDeviceWindow    = (HWND )myWindow->PlatformWindow()->NativeHandle();

  // create the Video Device
  HRESULT isOK = myD3dLib->CreateDevice (anAdapterId, D3DDEVTYPE_HAL,
                                         (HWND )myWindow->PlatformWindow()->NativeHandle(),
                                         D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
                                         myD3dParams.get(), &myD3dDevice);
  if (isOK < 0)
  {
    return false;
  }

  return myD3dDevice != NULL;
}

// =======================================================================
// function : d3dReset
// purpose  :
// =======================================================================
bool D3DHost_View::d3dReset()
{
  if (myD3dDevice == NULL)
  {
    return false;
  }

  myD3dParams->Windowed         = TRUE;
  myD3dParams->BackBufferWidth  = myWindow->Width();
  myD3dParams->BackBufferHeight = myWindow->Height();
  myD3dParams->hDeviceWindow    = (HWND )myWindow->PlatformWindow()->NativeHandle();
  myD3dParams->FullScreen_RefreshRateInHz = !myD3dParams->Windowed ? myRefreshRate : 0;

  HRESULT isOK = myD3dDevice->Reset(myD3dParams.get());
  return isOK == D3D_OK;
}

// =======================================================================
// function : d3dCreateRenderTarget
// purpose  :
// =======================================================================
bool D3DHost_View::d3dCreateRenderTarget()
{
  if (myD3dWglFbo.IsNull())
  {
    myD3dWglFbo = new D3DHost_FrameBuffer();
  }
  if (!myD3dWglFbo->Init (myWorkspace->GetGlContext(),
                          myD3dDevice,
                          myIsD3dEx,
                          myWindow->Width(),
                          myWindow->Height()))
  {
    return false;
  }

  myD3dDevice->SetRenderTarget (0, myD3dWglFbo->D3dColorSurface());
  return true;
}

// =======================================================================
// function : d3dBeginRender
// purpose  :
// =======================================================================
void D3DHost_View::d3dBeginRender()
{
  if (myD3dDevice == NULL)
  {
    return;
  }

  // clear the back buffer
  myD3dDevice->Clear (0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
  myD3dDevice->BeginScene();
}

// =======================================================================
// function : d3dEndRender
// purpose  :
// =======================================================================
void D3DHost_View::d3dEndRender()
{
  if (myD3dDevice != NULL)
  {
    myD3dDevice->EndScene();
  }
}

// =======================================================================
// function : d3dSwap
// purpose  :
// =======================================================================
bool D3DHost_View::d3dSwap()
{
  if (myD3dDevice == NULL)
  {
    return false;
  }

  const HRESULT isOK = myD3dDevice->Present (NULL, NULL, NULL, NULL);
  if (isOK != D3D_OK)
  {
    TCollection_ExtendedString aMsg = TCollection_ExtendedString()
      + "Direct3D9, Present device failed, " + d3dFormatError (isOK);
    myWorkspace->GetGlContext()->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                                              GL_DEBUG_TYPE_ERROR_ARB,
                                              0,
                                              GL_DEBUG_SEVERITY_HIGH_ARB,
                                              aMsg);
  }
  return isOK == D3D_OK;
}

// =======================================================================
// function : Redraw
// purpose  :
// =======================================================================
void D3DHost_View::Redraw()
{
  if (!myWorkspace->Activate()
    || myD3dDevice == NULL)
  {
    return;
  }
  else if (!myFBO.IsNull())
  {
    OpenGl_View::Redraw();
    return;
  }

  Handle(OpenGl_Context) aCtx = myWorkspace->GetGlContext();
  myToFlipOutput = Standard_True;
  myD3dWglFbo->LockSurface   (aCtx);
  myFBO = myD3dWglFbo;
  OpenGl_View::Redraw();
  myFBO.Nullify();
  myD3dWglFbo->UnlockSurface (aCtx);
  myToFlipOutput = Standard_False;
  if (aCtx->caps->buffersNoSwap)
  {
    return;
  }

  // blit result to the D3D back buffer and swap
  d3dBeginRender();

  IDirect3DSurface9* aBackbuffer = NULL;
  myD3dDevice->GetBackBuffer (0, 0, D3DBACKBUFFER_TYPE_MONO, &aBackbuffer);
  myD3dDevice->StretchRect (myD3dWglFbo->D3dColorSurface(), NULL, aBackbuffer, NULL, D3DTEXF_LINEAR);
  aBackbuffer->Release();

  d3dEndRender();
  d3dSwap();
}

// =======================================================================
// function : RedrawImmediate
// purpose  :
// =======================================================================
void D3DHost_View::RedrawImmediate()
{
  Handle(OpenGl_Context) aCtx = myWorkspace->GetGlContext();
  if (!myTransientDrawToFront
   || !myBackBufferRestored
   || (aCtx->caps->buffersNoSwap && !myMainSceneFbos[0]->IsValid()))
  {
    Redraw();
    return;
  }
  else if (!myWorkspace->Activate()
         || myD3dDevice == NULL)
  {
    return;
  }
  else if (!myFBO.IsNull())
  {
    OpenGl_View::Redraw();
    return;
  }

  myToFlipOutput = Standard_True;
  myD3dWglFbo->LockSurface   (aCtx);
  myFBO = myD3dWglFbo;
  OpenGl_View::RedrawImmediate();
  myFBO.Nullify();
  myD3dWglFbo->UnlockSurface (aCtx);
  myToFlipOutput = Standard_False;
  if (aCtx->caps->buffersNoSwap)
  {
    return;
  }

  // blit result to the D3D back buffer and swap
  d3dBeginRender();

  IDirect3DSurface9* aBackbuffer = NULL;
  myD3dDevice->GetBackBuffer (0, 0, D3DBACKBUFFER_TYPE_MONO, &aBackbuffer);
  myD3dDevice->StretchRect (myD3dWglFbo->D3dColorSurface(), NULL, aBackbuffer, NULL, D3DTEXF_LINEAR);
  aBackbuffer->Release();

  d3dEndRender();
  d3dSwap();
}

// =======================================================================
// function : Resize
// purpose  :
// =======================================================================
void D3DHost_View::Resized()
{
  const Standard_Integer aWidthOld  = myWindow->Width();
  const Standard_Integer aHeightOld = myWindow->Height();
  OpenGl_View::Resized();
  if (aWidthOld  == myWindow->Width()
   && aHeightOld == myWindow->Height())
  {
    return;
  }

  if (!myD3dWglFbo.IsNull())
  {
    myD3dWglFbo->Release (myWorkspace->GetGlContext().operator->());
  }
  if (!myWorkspace->GetGlContext()->caps->buffersNoSwap)
  {
    d3dReset();
  }
  d3dCreateRenderTarget();
}
