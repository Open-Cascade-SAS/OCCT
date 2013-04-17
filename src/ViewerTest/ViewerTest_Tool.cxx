// Created on: 1998-10-15
// Created by: Denis PASCAL
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#ifdef WNT
#include <windows.h>
#endif

#include <ViewerTest_Tool.ixx>

#include <V3d_View.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <string.h>
#include <Draw_Interpretor.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <DBRep.hxx>
#include <TCollection_AsciiString.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>
#include <V3d.hxx>
#include <AIS_DisplayMode.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <AIS_MapOfInteractive.hxx>
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_EventManager.hxx>
#include <Draw_Window.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <Aspect_Window.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d.hxx>
#include <Graphic3d_GraphicDriver.hxx>

#if defined(_WIN32) || defined(__WIN32__)
#include <WNT_WClass.hxx>
#include <WNT_Window.hxx>
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
#include <Cocoa_Window.hxx>
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Xw_Window.hxx>
#endif

//==============================================================================
//  GLOBAL VARIABLES
//==============================================================================
#define ZCLIPWIDTH 1.

static Handle(Aspect_DisplayConnection)& GetDisplayConnection()
{
  static Handle(Aspect_DisplayConnection) aDisplayConnection;
  static Standard_Boolean isFirst = Standard_True;
  if (isFirst)
  {
    aDisplayConnection = new Aspect_DisplayConnection();
    isFirst = Standard_False;
  }
  return aDisplayConnection;
}

Standard_IMPORT Standard_Boolean Draw_VirtualWindows;

//=======================================================================
//function : MakeViewer
//purpose  :
//=======================================================================

Handle(V3d_Viewer) ViewerTest_Tool::MakeViewer (const Standard_CString theTitle)
{
#if defined(_WIN32) || defined(__WIN32__)
  Handle(Aspect_Window) window = new WNT_Window (theTitle,
                                                 Handle(WNT_WClass)::DownCast (ViewerTest::WClass()),
                                                 WS_OVERLAPPEDWINDOW, 0, 460, 409, 409, Quantity_NOC_BLACK);
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  Handle(Aspect_Window) window = new Cocoa_Window (theTitle, 0, 460, 409, 409);
#else
  Handle(Aspect_Window) window = new Xw_Window (GetDisplayConnection(), theTitle, 0, 460, 409, 409);
#endif
  window->SetVirtual (Draw_VirtualWindows);
  window->Map();

  // Viewer
  Handle(Graphic3d_GraphicDriver) aDriver = Graphic3d::InitGraphicDriver (GetDisplayConnection());
  TCollection_ExtendedString NameOfWindow("Visu3D");
  Handle(V3d_Viewer) a3DViewer = new V3d_Viewer(aDriver,NameOfWindow.ToExtString());

  a3DViewer->SetDefaultBackgroundColor(Quantity_NOC_BLACK);
  a3DViewer->SetDefaultLights();
  a3DViewer->SetLightOn();

  // View
  Handle (V3d_View) V = a3DViewer->CreateView();
  V->SetWindow(window);
  V->SetZClippingDepth(0.5);
  V->SetZClippingWidth(ZCLIPWIDTH/2.);

  return a3DViewer;
}

//=======================================================================
//function : MakeContext
//purpose  :
//=======================================================================

Handle(AIS_InteractiveContext)  ViewerTest_Tool::MakeContext (const Standard_CString title)
{
  return new AIS_InteractiveContext (MakeViewer(title));
}



//=======================================================================
//function : InitViewerTest
//purpose  :
//=======================================================================

// ********* next method is defined in ViewerTest_ViewerCommands.hxx ****
extern void ViewerTest_InitViewerTest (const Handle(AIS_InteractiveContext)&);
// **********************************************************************

void  ViewerTest_Tool::InitViewerTest (const Handle(AIS_InteractiveContext)& context)
{
  ViewerTest_InitViewerTest (context);
}

