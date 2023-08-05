// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <inspector/View_Viewer.hxx>

#include <OpenGl_GraphicDriver.hxx>

// =======================================================================
// function : CreateView
// purpose :
// =======================================================================
void View_Viewer::CreateView()
{
  if (myView.IsNull())
    myView = myContext->CurrentViewer()->CreateView();
}

// =======================================================================
// function : CreateView
// purpose :
// =======================================================================
void View_Viewer::SetWindow(const Handle(Aspect_Window)& theWindow)
{
  myView->SetWindow (theWindow);
  if (!theWindow->IsMapped())
    theWindow->Map();
}

// =======================================================================
// function : InitViewer
// purpose :
// =======================================================================
void View_Viewer::InitViewer (const Handle(AIS_InteractiveContext)& theContext)
{
  myContext = theContext;
  myViewer = myContext->CurrentViewer();
}

// =======================================================================
// function : CreateStandardViewer
// purpose :
// =======================================================================
Handle(AIS_InteractiveContext) View_Viewer::CreateStandardViewer()
{
  Handle(Aspect_DisplayConnection) aDisplayConnection = new Aspect_DisplayConnection();
  static Handle(OpenGl_GraphicDriver) aGraphicDriver = new OpenGl_GraphicDriver (aDisplayConnection);

  Handle(V3d_Viewer) aViewer = new V3d_Viewer (aGraphicDriver);
  aViewer->SetDefaultLights();
  aViewer->SetLightOn();
  aViewer->SetDefaultBackgroundColor (Quantity_NOC_GRAY30);

  Handle(AIS_InteractiveContext) aContext = new AIS_InteractiveContext (aViewer);
  aContext->UpdateCurrentViewer();

  return aContext;
}
