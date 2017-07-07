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
#include <D3DHost_View.hxx>

IMPLEMENT_STANDARD_RTTIEXT(D3DHost_GraphicDriver,OpenGl_GraphicDriver)

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
Handle(Graphic3d_CView) D3DHost_GraphicDriver::CreateView (const Handle(Graphic3d_StructureManager)& theMgr)
{
  Handle(D3DHost_View) aView = new D3DHost_View (theMgr, this, myCaps, &myStateCounter);

  myMapOfView.Add (aView);

  for (TColStd_SequenceOfInteger::Iterator aLayerIt (myLayerSeq); aLayerIt.More(); aLayerIt.Next())
  {
    const Graphic3d_ZLayerId        aLayerID  = aLayerIt.Value();
    const Graphic3d_ZLayerSettings& aSettings = myMapOfZLayerSettings.Find (aLayerID);
    aView->AddZLayer         (aLayerID);
    aView->SetZLayerSettings (aLayerID, aSettings);
  }

  return aView;
}
