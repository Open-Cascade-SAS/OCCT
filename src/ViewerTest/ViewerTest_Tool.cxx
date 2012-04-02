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
#include <Graphic3d_GraphicDevice.hxx>
#include <Xw_GraphicDevice.hxx>
#include <Xw_WindowQuality.hxx>
#include <Xw_Window.hxx>
#include <Graphic3d_GraphicDevice.hxx>
#include <Xw_GraphicDevice.hxx>
#include <Xw_WindowQuality.hxx>
#include <Xw_Window.hxx>
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


#ifndef WNT
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#else
#include <WNT_WClass.hxx>
#include <WNT_Window.hxx>
#include <Graphic3d_WNTGraphicDevice.hxx>
#endif





//==============================================================================
//  GLOBAL VARIABLES
//==============================================================================
#define ZCLIPWIDTH 1.

#ifdef WNT

static Handle(Graphic3d_WNTGraphicDevice) GetDevice() { 
  static Handle(Graphic3d_WNTGraphicDevice) myDevice;
  static int first = 1;
  if ( first ) myDevice = new Graphic3d_WNTGraphicDevice();
  first = 0;
  return myDevice;
}

#else

static Handle(Graphic3d_GraphicDevice)& GetDevice() {
  static Handle(Graphic3d_GraphicDevice) myDevice;
  static int first = 1;
  if ( first ) myDevice = new Graphic3d_GraphicDevice (getenv("DISPLAY"),Xw_TOM_READONLY);
  first = 0;
  return myDevice;
}

//static Display *display;

#endif


Standard_IMPORT Standard_Boolean Draw_VirtualWindows;

//=======================================================================
//function : MakeViewer
//purpose  : 
//=======================================================================

Handle(V3d_Viewer) ViewerTest_Tool::MakeViewer (const Standard_CString theTitle)
{
#ifdef WNT
  Handle(Aspect_Window) window = new WNT_Window (GetDevice(), theTitle,
                                                 Handle(WNT_WClass)::DownCast (ViewerTest::WClass()),
                                                 WS_OVERLAPPEDWINDOW, 0, 460, 409, 409, Quantity_NOC_BLACK);
#else
  Handle(Aspect_Window) window = new Xw_Window (GetDevice(), theTitle,
                                                0, 460, 409, 409, Xw_WQ_3DQUALITY, Quantity_NOC_BLACK);
#endif
  window->SetVirtual (Draw_VirtualWindows);
  window->Map();

  // Viewer
  Handle(Aspect_GraphicDevice) theDevice = GetDevice();
  TCollection_ExtendedString NameOfWindow("Visu3D");
  Handle(V3d_Viewer) a3DViewer = new V3d_Viewer(theDevice,NameOfWindow.ToExtString());

  a3DViewer->SetDefaultBackgroundColor(Quantity_NOC_BLACK);
  a3DViewer->SetDefaultLights();
  a3DViewer->SetLightOn();

  // View
  Handle (V3d_View) V = a3DViewer->CreateView();
  V->SetDegenerateModeOn();
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

