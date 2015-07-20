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

#include <D3DHost_GraphicDriver.hxx>

#include <D3DHost_Workspace.hxx>
#include <OpenGl_CView.hxx>
#include <Visual3d_View.hxx>

#ifdef _MSC_VER
  #pragma comment (lib, "D3D9.lib")
#endif

// =======================================================================
// function : D3DHost_GraphicDriver
// purpose  :
// =======================================================================
D3DHost_GraphicDriver::D3DHost_GraphicDriver()
: OpenGl_GraphicDriver (Handle(Aspect_DisplayConnection)(), Standard_False)
{
  //
}

// =======================================================================
// function : ~D3DHost_GraphicDriver
// purpose  :
// =======================================================================
D3DHost_GraphicDriver::~D3DHost_GraphicDriver()
{
  //
}

// =======================================================================
// function : View
// purpose  :
// =======================================================================
Standard_Boolean D3DHost_GraphicDriver::View (Graphic3d_CView& theCView)
{
  Handle(OpenGl_Context) aShareCtx = GetSharedContext();
  OpenGl_CView*          aCView = (OpenGl_CView* )theCView.ptrView;
  if (aCView != NULL
   && myMapOfView.Contains (aCView->View))
  {
    Handle(D3DHost_Workspace) anOldWS = Handle(D3DHost_Workspace)::DownCast (aCView->WS);
    Handle(D3DHost_Workspace) aWS     = new D3DHost_Workspace (this, theCView.DefWindow, theCView.GContext, myCaps, aShareCtx);
    theCView.ptrFBO = aWS->D3dWglBuffer().operator->();
    aCView->WS = aWS;
    aWS->SetActiveView (aCView->View, theCView.ViewId);

    myMapOfWS.Remove (anOldWS);
    myMapOfWS.Add    (aWS);
    return Standard_True;
  }

  Handle(D3DHost_Workspace) aWS       = new D3DHost_Workspace (this, theCView.DefWindow, theCView.GContext, myCaps, aShareCtx);
  Handle(OpenGl_View)       aView     = new OpenGl_View (theCView.Context, &myStateCounter);
  myMapOfWS  .Add (aWS);
  myMapOfView.Add (aView);

  aCView = new OpenGl_CView();
  aCView->View = aView;
  aCView->WS   = aWS;
  theCView.ptrFBO  = aWS->D3dWglBuffer().operator->();
  theCView.ptrView = aCView;
  aWS->SetActiveView (aCView->View, theCView.ViewId);
  return Standard_True;
}

// =======================================================================
// function : D3dColorSurface
// purpose  :
// =======================================================================
IDirect3DSurface9* D3DHost_GraphicDriver::D3dColorSurface (const Handle(Visual3d_View)& theView) const
{
  Graphic3d_CView* aCView = (Graphic3d_CView* )(theView->CView());
  if (aCView == NULL
   || aCView->ptrView == NULL)
  {
    return NULL;
  }

  Handle(D3DHost_Workspace) aWS = Handle(D3DHost_Workspace)::DownCast (((OpenGl_CView* )aCView->ptrView)->WS);
  if (aWS.IsNull()
   || aWS->D3dWglBuffer().IsNull())
  {
    return NULL;
  }

  return aWS->D3dWglBuffer()->D3dColorSurface();
}
