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


#define OCC198 // jfa 06/03/2002 // to prevent exception on bad font index
#define OCC199 // jfa 06/03/2002 // to prevent exception on bad color index

#ifdef WNT
#include <windows.h>
#endif

#include <Viewer2dTest.hxx>
#include <Viewer2dTest_EventManager.hxx>
#include <Viewer2dTest_DataMapOfText.hxx>

#include <AIS2D_InteractiveContext.hxx>

#include <Draw_Interpretor.hxx>
#include <Draw.hxx>

#ifndef WNT

#include <tk.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <Graphic3d_GraphicDevice.hxx>
#include <Xw_Window.hxx>
#include <Xw_Driver.hxx>

#else

#include <Graphic3d_WNTGraphicDevice.hxx>
#include <WNT_WClass.hxx>
#include <WNT_Window.hxx>
#include <WNT_WDriver.hxx>

#include <stdio.h>

#endif

#include <V2d_Viewer.hxx>
#include <V2d_View.hxx>

#include <Graphic2d_Text.hxx>

#include <PS_Driver.hxx>

//==============================================================================
//  VIEWER GLOBALs
//==============================================================================

Standard_IMPORT Standard_Boolean Draw_VirtualWindows;

Standard_EXPORT int ViewerMainLoop2d (Standard_Integer , const char** argv);

#ifdef WNT
static Handle(Graphic3d_WNTGraphicDevice)& GetG2dDevice()
{
  static Handle(Graphic3d_WNTGraphicDevice) GD;
  return GD;
}

static Handle(WNT_Window)& VT_GetWindow2d()
{
  static Handle(WNT_Window) WNTWin;
  return WNTWin;
}

#else
static Handle(Graphic3d_GraphicDevice)& GetG2dDevice()
{
  static Handle(Graphic3d_GraphicDevice) GD;
  return GD;
}

static Handle(Xw_Window)& VT_GetWindow2d()
{
  static Handle(Xw_Window) XWWin;
  return XWWin;
}

static Display* display2d;

static void VProcessEvents2d (ClientData, int);
#endif

#define ZCLIPWIDTH 1.

static Viewer2dTest_DataMapOfText myMapOfText;

static void OSWindowSetup2d();

//==============================================================================
//  EVENT GLOBAL VARIABLES
//==============================================================================
static int Start_Rot = 0;
static int ZClipIsOn = 0;
static int X_Motion = 0, Y_Motion = 0; // Current cursor position
static int X_ButtonPress = 0, Y_ButtonPress = 0; // Last ButtonPress position
//==============================================================================

#ifdef WNT
static LRESULT WINAPI Viewer2dWindowProc (HWND hwnd, UINT uMsg,
                                          WPARAM wParam, LPARAM lParam);
#endif

//==============================================================================
//function : WClass
//purpose  :
//==============================================================================
static Handle(MMgt_TShared)& WClass()
{
  static Handle(MMgt_TShared) theWClass;
#ifdef WNT
  if (theWClass.IsNull()) {
    theWClass = new WNT_WClass ("GW2D_Class", Viewer2dWindowProc,
      CS_VREDRAW | CS_HREDRAW, 0, 0,
      ::LoadCursor (NULL, IDC_ARROW));
  }
#endif
  return theWClass;
}

//==============================================================================
//function : ViewerInit
//purpose  : Create the window viewer and initialize all the global variable
//==============================================================================
void Viewer2dTest::ViewerInit (const Standard_Integer thePxLeft,  const Standard_Integer thePxTop,
                               const Standard_Integer thePxWidth, const Standard_Integer thePxHeight)
{
  static Standard_Boolean isFirst = Standard_True;

  Standard_Integer aPxLeft   = 0;
  Standard_Integer aPxTop    = 50;
  Standard_Integer aPxWidth  = 409;
  Standard_Integer aPxHeight = 409;
  if (thePxWidth != 0 && thePxHeight != 0)
  {
    aPxLeft   = thePxLeft;
    aPxTop    = thePxTop;
    aPxWidth  = thePxWidth;
    aPxHeight = thePxHeight;
  }

  if (isFirst)
  {
    Handle(Aspect_WindowDriver) aWindowDriver;

#ifdef WNT
    // Create the Graphic device
    if (GetG2dDevice().IsNull())
      GetG2dDevice() = new Graphic3d_WNTGraphicDevice();

    // Create window
    if (VT_GetWindow2d().IsNull())
    {
      Handle(WNT_GraphicDevice) g_Device = new WNT_GraphicDevice();

      VT_GetWindow2d() = new WNT_Window (g_Device, "Test2d",
                                         Handle(WNT_WClass)::DownCast (WClass()),
                                         WS_OVERLAPPEDWINDOW,
                                         aPxLeft, aPxTop,
                                         aPxWidth, aPxHeight,
                                         Quantity_NOC_BLACK);
      VT_GetWindow2d()->SetVirtual (Draw_VirtualWindows);
      // force drawing into offscreen bitmap
      VT_GetWindow2d()->SetDoubleBuffer (Standard_True);
    }

    // Create window driver
    aWindowDriver = new WNT_WDriver(VT_GetWindow2d());
#else
    // Create the Graphic device
    if (GetG2dDevice().IsNull())
      GetG2dDevice() = new Graphic3d_GraphicDevice (getenv("DISPLAY"), Xw_TOM_READONLY);

    // Create window
    if (VT_GetWindow2d().IsNull())
    {
      VT_GetWindow2d() = new Xw_Window (GetG2dDevice(), "Test2d",
                                        aPxLeft, aPxTop,
                                        aPxWidth, aPxHeight,
                                        Xw_WQ_3DQUALITY,
                                        Quantity_NOC_BLACK);
      VT_GetWindow2d()->SetVirtual (Draw_VirtualWindows);
      // force drawing into offscreen bitmap
      VT_GetWindow2d()->SetDoubleBuffer (Standard_True);
    }

    // Create window driver
    aWindowDriver = new Xw_Driver(VT_GetWindow2d());
#endif
    TCollection_ExtendedString NameOfWindow("Visu3D");
    Handle(V2d_Viewer) a3DViewer,a3DCollector;
    // Create Viewer and View
    a3DViewer = new V2d_Viewer(GetG2dDevice(), NameOfWindow.ToExtString());
    NameOfWindow = TCollection_ExtendedString("Collector");
    a3DCollector = new V2d_Viewer(GetG2dDevice(), NameOfWindow.ToExtString());
//    a3DViewer->SetDefaultBackgroundColor(Quantity_NOC_BLACK);
//    a3DCollector->SetDefaultBackgroundColor(Quantity_NOC_STEELBLUE);

    if (Viewer2dTest::CurrentView().IsNull())
    {
//      VT_GetWindow2d()->SetBackground(Quantity_NOC_MATRAGRAY);
      Handle(V2d_View) V = new V2d_View(aWindowDriver, a3DViewer);
      Viewer2dTest::CurrentView(V);
    }
    Handle(V2d_View) a3DViewCol = new V2d_View(aWindowDriver, a3DCollector);

    // AIS2D setup
    if (Viewer2dTest::GetAIS2DContext().IsNull())
    {
      Handle(AIS2D_InteractiveContext) C = new AIS2D_InteractiveContext(a3DViewer,a3DCollector);
      Viewer2dTest::SetAIS2DContext(C);
    }

    // Setup for X11 or NT
    OSWindowSetup2d();

    // Viewer and View creation

//    a3DViewer->SetDefaultBackgroundColor(Quantity_NOC_BLACK);

    Handle(V2d_View) V = Viewer2dTest::CurrentView();

//    V->SetDegenerateModeOn();
//    V->SetWindow(VT_GetWindow2d());

//    V->SetZClippingDepth(0.5);
//    V->SetZClippingWidth(ZCLIPWIDTH/2.);

//    a3DViewer->SetDefaultLights();
//    a3DViewer->SetLightOn();

#ifndef WNT
#if TCL_MAJOR_VERSION < 8
    Tk_CreateFileHandler((void*)ConnectionNumber(display2d),
			 TK_READABLE, VProcessEvents2d, (ClientData) VT_GetWindow2d()->XWindow());
#else
    Tk_CreateFileHandler(ConnectionNumber(display2d),
			 TK_READABLE, VProcessEvents2d, (ClientData) VT_GetWindow2d()->XWindow());
#endif
#endif

    isFirst = Standard_False;
  }
  VT_GetWindow2d()->Map();
}

//==============================================================================
//function : V2dinit
//purpose  : Create the window viewer and initialize all the global variable
//		Use Tk_CreateFileHandler on UNIX to cath the X11 Viewer event
//==============================================================================
static int V2dInit (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  Standard_Integer aPxLeft   = (argc > 1) ? atoi (argv[1]) : 0;
  Standard_Integer aPxTop    = (argc > 2) ? atoi (argv[2]) : 0;
  Standard_Integer aPxWidth  = (argc > 3) ? atoi (argv[3]) : 0;
  Standard_Integer aPxHeight = (argc > 4) ? atoi (argv[4]) : 0;
  Viewer2dTest::ViewerInit (aPxLeft, aPxTop, aPxWidth, aPxHeight);
  return 0;
}

//==============================================================================
//function : ProcessKeyPress2d
//purpose  : Handle KeyPress event from a CString
//==============================================================================
static void ProcessKeyPress2d (char *buf_ret)
{
  // Letters
  if (!strcasecmp(buf_ret, "R"))
  {
    // Reset
    Viewer2dTest::CurrentView()->Reset();
  }
  else if (!strcasecmp(buf_ret, "F"))
  {
    // FitAll
    Viewer2dTest::CurrentView()->Fitall();
  }
  else if (!strcasecmp(buf_ret, "H"))
  {
    // HLR
    cout << "HLR" << endl;
//    Viewer2dTest::CurrentView()->SetDegenerateModeOff();
  }
  else if (!strcasecmp(buf_ret, "S"))
  {
    // SHADING
    cout << "passage en mode 1 (shading pour les shapes)" << endl;
//    Viewer2dTest::CurrentView()->SetDegenerateModeOn();
    Handle(AIS2D_InteractiveContext) Ctx = Viewer2dTest::GetAIS2DContext();
//    if (Ctx->NbCurrents() == 0 || Ctx->NbSelected() == 0)
//      Ctx->SetDisplayMode(AIS2D_Shaded);
//    else
    {
      if (Ctx->HasOpenedContext())
      {
//	for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected())
//	  Ctx->SetDisplayMode(Ctx->Interactive(), 1, Standard_False);
      }
      else
      {
//	for(Ctx->InitCurrent();Ctx->MoreCurrent();Ctx->NextCurrent())
//	  Ctx->SetDisplayMode(Ctx->Current(), 1, Standard_False);
      }
      Ctx->UpdateCurrentViewer();
    }
  }
  else if (!strcasecmp(buf_ret, "U"))
  {
    // Unset display mode
    cout << "passage au mode par defaut" << endl;
//    Viewer2dTest::CurrentView()->SetDegenerateModeOn();
    Handle(AIS2D_InteractiveContext) Ctx = Viewer2dTest::GetAIS2DContext();
//    if (Ctx->NbCurrents() == 0 || Ctx->NbSelected() == 0)
//      Ctx->SetDisplayMode(AIS2D_WireFrame);
//    else
    {
      if(Ctx->HasOpenedContext())
      {
//	for (Ctx->InitSelected(); Ctx->MoreSelected(); Ctx->NextSelected())
//	  Ctx->UnsetDisplayMode(Ctx->Interactive(), Standard_False);
      }
      else
      {
//	for (Ctx->InitCurrent(); Ctx->MoreCurrent(); Ctx->NextCurrent())
//	  Ctx->UnsetDisplayMode(Ctx->Current(), Standard_False);
      }
      Ctx->UpdateCurrentViewer();
    }
  }
  else if (!strcasecmp(buf_ret, "W"))
  {
    // WIREFRAME
    cout << "passage en mode 0 (filaire pour les shapes)" << endl;
//    Viewer2dTest::CurrentView()->SetDegenerateModeOn();
    Handle(AIS2D_InteractiveContext) Ctx = Viewer2dTest::GetAIS2DContext();
//    if (Ctx->NbCurrents() == 0 || Ctx->NbSelected() == 0)
//      Ctx->SetDisplayMode(AIS2D_WireFrame);
//    else
    {
      if (Ctx->HasOpenedContext())
      {
//	for (Ctx->InitSelected(); Ctx->MoreSelected(); Ctx->NextSelected())
//	  Ctx->SetDisplayMode(Ctx->Interactive(), 0, Standard_False);
      }
      else
      {
//	for (Ctx->InitCurrent(); Ctx->MoreCurrent(); Ctx->NextCurrent())
//	  Ctx->SetDisplayMode(Ctx->Current(), 0, Standard_False);
      }
      Ctx->UpdateCurrentViewer();
    }
  }
  else if (!strcasecmp(buf_ret, "D"))
  {
    // Delete selected
    cout << "Delete selected object - not yet implemented" << endl;
    Viewer2dTest::CurrentView()->Reset();
  }
  else if (!strcasecmp(buf_ret, "Z"))
  {
    // ZCLIP
    if (ZClipIsOn)
    {
      cout << "ZClipping OFF" << endl;
      ZClipIsOn = 0;

//      Viewer2dTest::CurrentView()->SetZClippingType(V2d_OFF);
//      Viewer2dTest::CurrentView()->Redraw();
    }
    else
    {
      cout << "ZClipping ON" << endl;
      ZClipIsOn = 1;

//      Viewer2dTest::CurrentView()->SetZClippingType(V2d_FRONT);
//      Viewer2dTest::CurrentView()->Redraw();
    }
  }
  else if (!strcasecmp(buf_ret, ","))
  {
//    Viewer2dTest::GetAIS2DContext()->HilightNextDetected(Viewer2dTest::CurrentView());
  }
  else if (!strcasecmp(buf_ret, "."))
  {
//    Viewer2dTest::GetAIS2DContext()->HilightPreviousDetected(Viewer2dTest::CurrentView());
  }
  else // Number
  {
    Standard_Integer Num = atoi(buf_ret);
    if (Num >= 0 && Num <= 7)
      Viewer2dTest::StandardModeActivation(Num);
  }
}

//==============================================================================
//function : ProcessExpose2d
//purpose  : Redraw the View on an Expose Event
//==============================================================================
static void ProcessExpose2d()
{
  Viewer2dTest::CurrentView()->Viewer()->Update();
//  Viewer2dTest::CurrentView()->Redraw();
}

//==============================================================================
//function : ProcessConfigure2d
//purpose  : Resize the View on an Configure Event
//==============================================================================
static void ProcessConfigure2d()
{
  Handle(V2d_View) V = Viewer2dTest::CurrentView();
  V->MustBeResized(V2d_TOWRE_ENLARGE_OBJECTS);
  V->Update();
//  V->Redraw();
  V->Viewer()->Update();
}

//==============================================================================
//function : V2dPickGrid
//purpose  : v2dpickgrid : get coordinates of a grid point near to MB1 click
//==============================================================================
static int V2dHitGrid (int argc, Standard_Integer anX, Standard_Integer anY,
                       const char* xxname = 0, const char* yyname = 0)
{
  if (argc != 1 && argc != 3 && argc != 5)
  {
    cout << "Usage : V2dHitGrid [mouse_X mouse_Y [grid_X grid_Y]]" << endl;
    return 1;
  }

  Handle(V2d_View) V = Viewer2dTest::CurrentView();
  if (V.IsNull()) return 1;

  if (V->Viewer()->IsActive()) // about the grid
  {
//    if (argc == 1)
//    {
//      cout << "Mouse cursor position: X = " << anX << " Y = " << anY << endl;
//    }

    Quantity_Length xx, yy;
    V->Hit(anX, anY, xx, yy);

    Standard_Integer xw, yw;
    V->Convert(xx, yy, xw, yw);
    printf("Window coords of Grid Point: X = %d Y = %d\n", xw, yw);

    if (argc < 5)
    {
      printf("Grid Point: X = %.6g Y = %.6g\n", xx, yy);
    }
    else
    {
      char szbuf[32];
      sprintf(szbuf, "%.6g", xx);
      Draw::Set(xxname, szbuf);
      sprintf(szbuf, "%.6g", yy);
      Draw::Set(yyname, szbuf);
    }
  }
  return 0;
}

Standard_EXPORT int V2dPickGrid (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (argc != 1 && argc != 3 && argc != 5)
  {
    cout << "Usage : v2dpickgrid (MB1 in the viewer)" << endl;
    cout << "Usage : v2dgetgrid  mouse_X mouse_Y [grid_X grid_Y]" << endl;
    return 1;
  }

  if (argc > 1)
  {
    if (argc > 3)
    {
      const char *xxname, *yyname;
      xxname = argv[3];
      yyname = argv[4];
      V2dHitGrid(argc, atoi(argv[1]), atoi(argv[2]), xxname, yyname);
    }
    else
    {
      V2dHitGrid(argc, atoi(argv[1]), atoi(argv[2]));
    }
  }
  else
  {
    while (ViewerMainLoop2d (argc, argv)) {}
  }

  return 0;
}

//==============================================================================
//function : ProcessButton1Press2d
//purpose  : Picking
//==============================================================================
static Standard_Boolean ProcessButton1Press2d
  (Standard_Integer , const char** argv, Standard_Boolean pick, Standard_Boolean shift)
{
  if (pick)
  {
    cout << "Window X = " << X_ButtonPress << " Window Y = " << Y_ButtonPress << endl;
    if (strlen(argv[0]) > 7) // v2dpickgrid
    {
      V2dHitGrid(1, X_ButtonPress, Y_ButtonPress);
    }
    else
    {
      Handle(V2d_View) V = Viewer2dTest::CurrentView();
      if (V.IsNull()) return 1;
      Quantity_Length xx, yy;
      V->Convert(X_ButtonPress, Y_ButtonPress, xx, yy);
      printf("Coord system X = %.6g Y = %.6g\n", xx, yy);
    }
  }

  Handle(Viewer2dTest_EventManager) EM = Viewer2dTest::CurrentEventManager();
  if (shift) EM->ShiftSelect();
  else       EM->Select();

  pick = 0;
  return pick;
}

//==============================================================================
//function : ProcessButton3Press2d
//purpose  : Start Rotation
//==============================================================================
static void ProcessButton3Press2d()
{
  // Start rotation
  Start_Rot = 1;
//  Viewer2dTest::CurrentView()->SetDegenerateModeOn();
//  Viewer2dTest::CurrentView()->StartRotation(X_ButtonPress, Y_ButtonPress);
}

//==============================================================================
//function : ProcessButtonRelease2d
//purpose  : End Rotation
//==============================================================================
static void ProcessButtonRelease2d()
{
  // End rotation
  Start_Rot = 0;
//  Viewer2dTest::CurrentView()->SetDegenerateModeOff();
}

//==============================================================================
//function : ProcessZClipMotion2d
//purpose  : Zoom
//==============================================================================
void ProcessZClipMotion2d()
{
  Handle(V2d_View) a3DView = Viewer2dTest::CurrentView();
  if (Abs(X_Motion - X_ButtonPress) > 2)
  {
    static Standard_Real CurZPos = 0.;
//    Quantity_Length VDX, VDY;

//    a3DView->Size(VDX,VDY);

#ifdef DEB
//    Standard_Real VDZ =
#endif
//      a3DView->ZSize();

    Quantity_Length dx = a3DView->Convert(X_Motion - X_ButtonPress);

    // Front = Depth + width/2.
    Standard_Real D = 0.5;
//    Standard_Real W = 0.1;

    CurZPos += (dx);

    D += CurZPos;

//	a3DView->SetZClippingType(V2d_OFF);
//	a3DView->SetZClippingDepth(D);
//	a3DView->SetZClippingWidth(W);
//	a3DView->SetZClippingType(V2d_FRONT);

//	a3DView->Redraw();

    X_ButtonPress = X_Motion;
    Y_ButtonPress = Y_Motion;
  }
}

//==============================================================================
//function : ProcessControlButton1Motion2d
//purpose  : Zoom on Ctrl+MB1
//==============================================================================
static void ProcessControlButton1Motion2d()
{
  Viewer2dTest::CurrentView()->Zoom(X_ButtonPress, Y_ButtonPress, X_Motion, Y_Motion);

  X_ButtonPress = X_Motion;
  Y_ButtonPress = Y_Motion;
}

//==============================================================================
//function : ProcessControlButton2Motion2d
//purpose  : Pan on Ctrl+MB2
//==============================================================================
static void ProcessControlButton2Motion2d()
{
  Viewer2dTest::CurrentView()->Pan(X_Motion - X_ButtonPress, Y_ButtonPress - Y_Motion);

  X_ButtonPress = X_Motion;
  Y_ButtonPress = Y_Motion;
}

//==============================================================================
//function : ProcessControlButton3Motion2d
//purpose  : Rotation on Ctrl+MB3
//==============================================================================
static void ProcessControlButton3Motion2d()
{
//  if (Start_Rot) Viewer2dTest::CurrentView()->Rotation(X_Motion, Y_Motion);
}

//==============================================================================
//function : ProcessMotion2d
//purpose  : pre-hilights detected objects at mouse position
//==============================================================================
static void ProcessMotion2d()
{
  Handle(Viewer2dTest_EventManager) EM = Viewer2dTest::CurrentEventManager();
  Handle(V2d_View) V = Viewer2dTest::CurrentView();
  EM->MoveTo(X_Motion, Y_Motion, V);
}

//==============================================================================
//function : ProcessGridMotion2d
//purpose  : pre-hilights grid point near mouse position
//==============================================================================
static void ProcessGridMotion2d()
{
  Handle(V2d_View) V = Viewer2dTest::CurrentView();

  static Standard_Boolean LastIsGridActiveStatus = Standard_True;
  if (V->Viewer()->IsActive()) // about the grid
  {
    V->ShowHit(X_Motion, Y_Motion);
    LastIsGridActiveStatus = Standard_True;
  }
  if (!V->Viewer()->IsActive() && LastIsGridActiveStatus) // about the grid
  {
    V->EraseHit();
    LastIsGridActiveStatus = Standard_False;
  }
}

//==============================================================================
//function : GetMousePosition
//purpose  :
//==============================================================================
void Viewer2dTest::GetMousePosition (Standard_Integer& Xpix, Standard_Integer& Ypix)
{
  Xpix = X_Motion;
  Ypix = Y_Motion;
}

//==============================================================================
//function : V2dHelp
//purpose  : Dsiplay help on viewer Keyboead and mouse commands
//Draw arg : No args
//==============================================================================
/*static int V2dHelp (Draw_Interpretor& , Standard_Integer , const char** )
{
  cout << "Q : Quit the application" << endl;
  cout << "=========================" << endl;
  cout << "F : FitAll" << endl;
  cout << "R : ResetView" << endl;
  cout << "=========================" << endl;
  cout << "Selection mode " << endl;
  cout << "0 : OBJECT" << endl;
  cout << "1 : PRIMITIVE" << endl;
  cout << "2 : ELEMENT" << endl;
  cout << "3 : VERTEX" << endl;
  cout << "4 : NONE" << endl;
  cout << "==========================" << endl;
  cout << "D : Remove Selected Object" << endl;
  cout << "==========================" << endl;

  return 0;
}*/

#ifdef WNT

static LRESULT WINAPI Viewer2dWindowProc (HWND hwnd, UINT Msg,
                                          WPARAM wParam, LPARAM lParam )
{
  static Standard_Boolean Ppick = 0;
  static Standard_Integer Pargc = 0;
  static const char**     Pargv = NULL;

  static int Up = 1;

  if (!Viewer2dTest::CurrentView().IsNull())
  {
    PAINTSTRUCT ps;

    switch (Msg)
    {
    case WM_CLOSE:
      // do not destroy the window - just hide it!
      VT_GetWindow2d()->Unmap();
      return 0;
    case WM_PAINT:
      BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
      ProcessExpose2d();
      break;

    case WM_SIZE:
      ProcessConfigure2d();
      break;

    case WM_KEYDOWN:
      char c[2];
      c[0] = (char) wParam;
      c[1] = '\0';
      ProcessKeyPress2d(c);
      break;

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
      Up = 1;
      ProcessButtonRelease2d();
      break;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
      {
	WPARAM fwKeys = wParam;

	Up = 0;

	X_ButtonPress = LOWORD(lParam);
	Y_ButtonPress = HIWORD(lParam);

	if (Msg == WM_LBUTTONDOWN && !(fwKeys & MK_CONTROL))
        {
          Ppick = ProcessButton1Press2d(Pargc, Pargv, Ppick, (fwKeys & MK_SHIFT));
	}
	else if (Msg == WM_RBUTTONDOWN)
        {
	  // Start rotation
	  ProcessButton3Press2d();
	}
      }
      break;

    case WM_MOUSEMOVE:
      {
	WPARAM fwKeys = wParam;
	X_Motion = LOWORD(lParam);
	Y_Motion = HIWORD(lParam);

	if (Up && fwKeys & (MK_LBUTTON|MK_MBUTTON|MK_RBUTTON))
        {
	  Up = 0;
	  X_ButtonPress = LOWORD(lParam);
	  Y_ButtonPress = HIWORD(lParam);

	  if (fwKeys & MK_RBUTTON)
          {
	    // Start rotation
	    ProcessButton3Press2d();
	  }
	}

	if (fwKeys & MK_CONTROL)
        {
	  if (fwKeys & MK_LBUTTON)
          {
	    ProcessControlButton1Motion2d();
	  }
	  else if (fwKeys & MK_MBUTTON || ((fwKeys&MK_LBUTTON) && (fwKeys&MK_RBUTTON)))
          {
	    ProcessControlButton2Motion2d();
	  }
	  else if (fwKeys & MK_RBUTTON)
          {
	    ProcessControlButton3Motion2d();
	  }
	}
	else if (fwKeys & MK_SHIFT)
        {
	  if (fwKeys & MK_MBUTTON || ((fwKeys&MK_LBUTTON) && (fwKeys&MK_RBUTTON)))
          {
	    ProcessZClipMotion2d();
	  }
	}
	else
        {
	  ProcessMotion2d();
	}
      }
      break;

    default:
      return(DefWindowProc(hwnd, Msg, wParam, lParam));
    }
    return 0L;
  }

  return DefWindowProc(hwnd, Msg, wParam, lParam);
}

//==============================================================================
//function : ViewerMainLoop2d
//purpose  : Get a Event on the view and dispatch it
//==============================================================================
static int ViewerMainLoop2d (Standard_Integer argc, const char** argv)
{
  cout << "No yet implemented on WNT" << endl;
  static Standard_Boolean Ppick = 0;
  static Standard_Integer Pargc = 0;
  static const char**     Pargv = NULL;

  Ppick = (argc > 0)? -1 : 0;
  Pargc = argc;
  Pargv = argv;

  if (Ppick)
  {
    MSG msg;
    msg.wParam = 1;

    cout << "Start picking" << endl;

    while (Ppick == -1)
    {
      // Wait for a ProcessButton1Press2d() to toggle pick to 1 or 0
      if (GetMessage(&msg, NULL, 0, 0))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }

    cout << "Picking done" << endl;
  }

  return Ppick;
}

#else

//==============================================================================
//function : ViewerMainLoop2d
//purpose  : Get a Event on the view and dispatch it
//==============================================================================
int ViewerMainLoop2d (Standard_Integer argc, const char** argv)
{
  Standard_Boolean pick = argc > 0;

  // X11 Event loop
  static XEvent report;

  XNextEvent(display2d, &report);

  switch (report.type)
  {
  case Expose:
    ProcessExpose2d();
    break;
  case ConfigureNotify:
    ProcessConfigure2d();
    break;
  case KeyPress:
    {
      KeySym ks_ret;
      char buf_ret[11];
      int ret_len;
      XComposeStatus status_in_out;

      ret_len = XLookupString((XKeyEvent *)&report, (char *) buf_ret,
                              10, &ks_ret, &status_in_out);

      buf_ret[ret_len] = '\0';
      if (ret_len) ProcessKeyPress2d(buf_ret);
    }
    break;
  case ButtonPress:
    {
      X_ButtonPress = report.xbutton.x;
      Y_ButtonPress = report.xbutton.y;

      if (report.xbutton.button == Button1 &&
          !(report.xbutton.state & ControlMask))
      {
        pick = ProcessButton1Press2d(argc, argv, pick,
                                     (report.xbutton.state & ShiftMask));
      }
      else if (report.xbutton.button == Button3)
      {
        // Start rotation
        ProcessButton3Press2d();
      }
    }
    break;
  case ButtonRelease:
    ProcessButtonRelease2d();
    break;
  case MotionNotify:
    {
      X_Motion = report.xmotion.x;
      Y_Motion = report.xmotion.y;

      // remove all the ButtonMotionMask
      while (XCheckMaskEvent(display2d, ButtonMotionMask, &report)) {}

      if (ZClipIsOn && report.xmotion.state & ShiftMask)
      {
        if (Abs(X_Motion - X_ButtonPress) > 2)
        {
          Quantity_Length VDX = 0.0, VDY = 0.0;

//	      Viewer2dTest::CurrentView()->Size(VDX,VDY);
          Standard_Real VDZ =0 ;
//	      VDZ = Viewer2dTest::CurrentView()->ZSize();

          printf("%f,%f,%f\n", VDX, VDY, VDZ);

          Quantity_Length dx = 0 ;
          dx = Viewer2dTest::CurrentView()->Convert(X_Motion - X_ButtonPress);

          cout << dx << endl;

          dx = dx / VDX * VDZ;

          cout << dx << endl;

          // Front = Depth + width/2.
          //Viewer2dTest::CurrentView()->SetZClippingDepth(dx);
          //Viewer2dTest::CurrentView()->SetZClippingWidth(0.);

//	      Viewer2dTest::CurrentView()->Redraw();
        }
      }

      if (report.xmotion.state & ControlMask)
      {
        if (report.xmotion.state & Button1Mask)
        {
          ProcessControlButton1Motion2d();
        }
        else if (report.xmotion.state & Button2Mask)
        {
          ProcessControlButton2Motion2d();
        }
        else if (report.xmotion.state & Button3Mask)
        {
          ProcessControlButton3Motion2d();
        }
      }
      else
      {
        if (pick && strlen(argv[0]) > 7) // v2dpickgrid
          ProcessGridMotion2d();
        else
          ProcessMotion2d();
      }
    }
    break;
  }

  return pick;
}

//==============================================================================
//function : VProcessEvents2d
//purpose  : call by Tk_CreateFileHandler() to be able to manage the
//	     event in the Viewer window
//==============================================================================
static void VProcessEvents2d (ClientData, int)
{
  // test for X Event
  while (XPending(display2d))
  {
    ViewerMainLoop2d(0, NULL);
  }
}
#endif

//==============================================================================
//function : OSWindowSetup2d
//purpose  : Setup for the X11 window to be able to cath the event
//==============================================================================
static void OSWindowSetup2d()
{
#ifndef WNT
  // X11
  Window  window = VT_GetWindow2d()->XWindow();

  Standard_Address theDisplay = GetG2dDevice()->XDisplay();
  display2d = (Display * ) theDisplay;

  XSynchronize(display2d, 1);

  VT_GetWindow2d()->Map();

  // X11 : For keyboard on SUN
  XWMHints wmhints;
  wmhints.flags = InputHint;
  wmhints.input = 1;

  XSetWMHints( display2d, window, &wmhints);

  XSelectInput(display2d, window,
               ExposureMask | StructureNotifyMask | KeyPressMask | ButtonPressMask |
               PointerMotionMask |
               Button1MotionMask | Button2MotionMask | Button3MotionMask);

  XSynchronize(display2d, 0);

#else
  // WNT
#endif

}

//==============================================================================
//function : InitViewer2dTest
//purpose  : initialisation de toutes les variables static de  Viewer2dTest (dp)
//==============================================================================
void Viewer2dTest_InitViewer2dTest (const Handle(AIS2D_InteractiveContext)& context)
{
  Handle(V2d_Viewer) viewer = context->CurrentViewer();
  Viewer2dTest::SetAIS2DContext(context);
  viewer->InitActiveViews();
  Handle(V2d_View) view = viewer->ActiveView();
  if (viewer->MoreActiveViews()) Viewer2dTest::CurrentView(view);
  Viewer2dTest::ResetEventManager();
  Handle(Aspect_GraphicDevice) device = viewer->Device();
  Handle(Aspect_WindowDriver) aWDr = view->Driver();
  Handle(Aspect_Window) window = aWDr->Window();
#ifndef WNT
  // X11
  VT_GetWindow2d() = Handle(Xw_Window)::DownCast(window);
  GetG2dDevice() = Handle(Graphic3d_GraphicDevice)::DownCast(device);
  OSWindowSetup2d();
  static int first = 1;
  if (first)
  {
#if TCL_MAJOR_VERSION  < 8
    Tk_CreateFileHandler((void*)ConnectionNumber(display2d),
			     TK_READABLE, VProcessEvents2d, (ClientData) 0);
#else
    Tk_CreateFileHandler(ConnectionNumber(display2d),
			     TK_READABLE, VProcessEvents2d, (ClientData) 0);
#endif
    first = 0;
  }
#endif
}

//==============================================================================
//function : V2dFit
//purpose  : Fitall, no DRAW arguments
//Draw arg : No args
//==============================================================================
static int V2dFit (Draw_Interpretor& , Standard_Integer , const char** )
{
  Handle(V2d_View) V = Viewer2dTest::CurrentView();
  if (!V.IsNull()) V->Fitall();
  return 0;
}

//==============================================================================
//function : V2dRepaint
//purpose  : refresh viewer
//Draw arg : No args
//==============================================================================
static int V2dRepaint (Draw_Interpretor& , Standard_Integer , const char** )
{
  Handle(V2d_View) V = Viewer2dTest::CurrentView();
  if (!V.IsNull())
//    V->Redraw();
    V->Viewer()->Update();
  return 0;
}

//==============================================================================
//function : V2dClear
//purpose  : Remove all the object from the viewer
//Draw arg : No args
//==============================================================================
static int V2dClear (Draw_Interpretor& , Standard_Integer , const char** )
{
  Handle(V2d_View) V = Viewer2dTest::CurrentView();
  if (V.IsNull()) return 1;

  Viewer2dTest::Clear();
  V->Viewer()->Update();
  return 0;
}

//==============================================================================
//function : V2dPick
//purpose  : v2dpick - print mouse coords and color
//==============================================================================
static int V2dPick (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (argc != 1)
  {
    cout << "Usage: v2dpick (MB1 in the viewer)" << endl;
    return 1;
  }
  if (Viewer2dTest::CurrentView().IsNull()) return 1;

  while (ViewerMainLoop2d (argc, argv)) {}

  return 0;
}

//==============================================================================
//function : V2dSetBG
//purpose  : v2dsetbg imagefile [filltype] : load image as background
//==============================================================================
static int V2dSetBG (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2 || argc > 3)
  {
    di << "Usage : v2dsetbg imagefile [filltype]" << "\n";
    return 1;
  }
  if (Viewer2dTest::CurrentView().IsNull()) return 1;

  Aspect_FillMethod aFillType = Aspect_FM_CENTERED;
  if (argc == 3)
  {
    const char* szType = argv[2];
    if      (strcmp(szType, "NONE"    ) == 0) aFillType = Aspect_FM_NONE;
    else if (strcmp(szType, "CENTERED") == 0) aFillType = Aspect_FM_CENTERED;
    else if (strcmp(szType, "TILED"   ) == 0) aFillType = Aspect_FM_TILED;
    else if (strcmp(szType, "STRETCH" ) == 0) aFillType = Aspect_FM_STRETCH;
    else
    {
      di << "Wrong fill type : " << szType << "\n";
      di << "Must be one of CENTERED, TILED, STRETCH, NONE" << "\n";
      return -1;
    }
  }

  Viewer2dTest::CurrentView()->SetBackground(argv[1], aFillType);
  Viewer2dTest::CurrentView()->Viewer()->Update();

  return 0;
}

//==============================================================================
//function : V2dGrid
//purpose  : v2dgrid [type [Origin(X Y) XStep/RadiusStep YStep/DivisionNb RotationAngle] [drawmode]] : load grid
//         : v2drmgrid : unload grid
//==============================================================================
static int V2dGrid (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (Viewer2dTest::CurrentView().IsNull()) return 1;
  Handle(V2d_Viewer) aViewer = Viewer2dTest::CurrentView()->Viewer();

  const char* szName = argv[0];
  if (strcmp(szName, "v2drmgrid") == 0)
  {
    if (argc > 1)
    {
      di << "Usage : v2drmgrid" << "\n";
      return 1;
    }

    aViewer->DeactivateGrid();
    return 0;
  }

  // v2dgrid. Args nb may be 1,2,3,7,8
  if (argc < 1 || argc > 8 || (argc < 7 && argc > 3))
  {
    di << "Usage : v2dgrid [type [Origin(X Y) XStep/RadiusStep YStep/DivisionNb RotationAngle] [drawmode]]" << "\n";
    return 1;
  }

  Aspect_GridType     aGridT = Aspect_GT_Rectangular;
  Aspect_GridDrawMode aDrawM = Aspect_GDM_Lines;

  if (argc > 1)
  {
    const char* szType = argv[1];
    if      (strcmp(szType, "Rect") == 0) aGridT = Aspect_GT_Rectangular;
    else if (strcmp(szType, "Circ") == 0) aGridT = Aspect_GT_Circular;
    else
    {
      di << "Wrong grid type : " << szType << "\n";
      di << "Must be Rect or Circ" << "\n";
      return -1;
    }

    if (argc == 3 || argc == 8)
    {
      const char* szMode = (argc == 3) ? argv[2] : argv[7];
      if      (strcmp(szMode, "Lines" ) == 0) aDrawM = Aspect_GDM_Lines;
      else if (strcmp(szMode, "Points") == 0) aDrawM = Aspect_GDM_Points;
      else if (strcmp(szMode, "None"  ) == 0) aDrawM = Aspect_GDM_None;
      else
      {
        di << "Wrong grid draw mode : " << szMode << "\n";
        di << "Must be Lines or Points, or None" << "\n";
        return -1;
      }
    }
  }

  aViewer->ActivateGrid(aGridT, aDrawM);

  if (argc > 3)
  {
    Quantity_Length aXOrigin = atof(argv[2]);
    Quantity_Length aYOrigin = atof(argv[3]);
    Quantity_PlaneAngle aRotationAngle = atof(argv[6]);

    if (aGridT == Aspect_GT_Rectangular)
    {
      Quantity_Length aXStep = atof(argv[4]);
      Quantity_Length aYStep = atof(argv[5]);
      aViewer->SetRectangularGridValues
        (aXOrigin, aYOrigin, aXStep, aYStep, aRotationAngle);
    }
    else
    {
      Quantity_Length aRadiusStep = atof(argv[4]);
      Standard_Integer aDivisionNumber = atoi(argv[5]);
      aViewer->SetCircularGridValues
        (aXOrigin, aYOrigin, aRadiusStep, aDivisionNumber, aRotationAngle);
    }
  }

  return 0;
}

//==============================================================================
//function : V2dPSOut
//purpose  : v2dpsout imagefile [Scale TypeOfColorSpace] [Width Height XCenter YCenter] : PostScript Output
//==============================================================================
static int V2dPSOut (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2 || argc > 8)
  {
    di << "Usage : v2dpsout imagefile [Scale TypeOfColorSpace] [Width Height XCenter YCenter]" << "\n";
    return 1;
  }

  Handle(V2d_View) V = Viewer2dTest::CurrentView();
  if (V.IsNull()) return 1;

  Quantity_Factor aScale = 1.0;
  Aspect_TypeOfColorSpace aColorType = Aspect_TOCS_RGB;

  Quantity_Length aWidth = 100.0, aHeight = 100.0;

  Quantity_Length aXCenter, aYCenter;
  V->Center(aXCenter, aYCenter);

  if (argc > 2)
  {
    aScale = atof(argv[2]);

    if (argc > 3)
    {
      const char* szType = argv[3];
      if      (strcmp(szType, "RGB")           == 0) aColorType = Aspect_TOCS_RGB;
      else if (strcmp(szType, "BlackAndWhite") == 0) aColorType = Aspect_TOCS_BlackAndWhite;
      else if (strcmp(szType, "GreyScale")     == 0) aColorType = Aspect_TOCS_GreyScale;
      else
      {
        di << "Wrong color space type : " << szType << "\n";
        di << "Must be one of RGB, BlackAndWhite, GreyScale" << "\n";
        return -1;
      }

      if (argc == 8)
      {
        aWidth  = atof(argv[4]);
        aHeight = atof(argv[5]);
        aXCenter = atof(argv[6]);
        aYCenter = atof(argv[7]);
      }
      else // take current window size
      {
#ifndef WNT
        Handle(Aspect_Window) anAspWindow = V->Driver()->Window();
        Handle(Xw_Window) anXWindow = Handle(Xw_Window)::DownCast(anAspWindow);
        anXWindow->Size(aWidth, aHeight);
#endif
      }
    }
  }

  Viewer2dTest::CurrentView()->PostScriptOutput
    (argv[1], aWidth, aHeight, aXCenter, aYCenter, aScale, aColorType);

  return 0;
}

//==============================================================================
//function : V2dPlot
//purpose  : v2dplot format imagefile [Scale TypeOfColorSpace] [Width Height XCenter YCenter]
//==============================================================================
/*static int V2dPlot (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  if (argc < 3 || argc > 9)
  {
    cout << "Args: format imagefile [Scale TypeOfColorSpace] [Width Height XCenter YCenter]" << endl;
    return 1;
  }
  if (Viewer2dTest::CurrentView().IsNull()) return 1;

  Quantity_Factor aScale = 1.0;
  Aspect_TypeOfColorSpace aColorType = Aspect_TOCS_RGB;

  Quantity_Length aWidth = 100.0, aHeight = 100.0;

  Quantity_Length aXCenter, aYCenter;
  Viewer2dTest::CurrentView()->Center(aXCenter, aYCenter);

  if (argc > 3)
  {
    aScale = atof(argv[3]);

    if (argc > 4)
    {
      char* szType = argv[4];
      if      (strcmp(szType, "RGB")           == 0) aColorType = Aspect_TOCS_RGB;
      else if (strcmp(szType, "BlackAndWhite") == 0) aColorType = Aspect_TOCS_BlackAndWhite;
      else if (strcmp(szType, "GreyScale")     == 0) aColorType = Aspect_TOCS_GreyScale;
      else
      {
        cout << "Wrong color space type : " << szType << endl;
        cout << "Must be one of RGB, BlackAndWhite, GreyScale" << endl;
        return -1;
      }

      if (argc == 9)
      {
        aWidth  = atof(argv[5]);
        aHeight = atof(argv[6]);
        aXCenter = atof(argv[7]);
        aYCenter = atof(argv[8]);
      }
    }
  }

  char* szFormat = argv[1];
  if      (strcmp(szFormat, "PS") == 0)
  {
    Handle(PS_Driver) aPSDriver = new PS_Driver(argv[2], aWidth, aHeight, aColorType);
    Viewer2dTest::CurrentView()->Plot(aPSDriver, aXCenter, aYCenter, aScale);
  }
  else if (strcmp(szFormat, "") == 0)
  {
    Handle(PS_Driver) aPSDriver = new PS_Driver(argv[2], aWidth, aHeight, aColorType);
    Viewer2dTest::CurrentView()->Plot(aPSDriver, aXCenter, aYCenter, aScale);
  }
  else if (strcmp(szFormat, "") == 0)
  {
  }
  else
  {
  }

  return 0;
}*/

//=======================================================================
//function : V2dTextColor
//purpose  : v2dsettextcolor text_name color_index
//=======================================================================
static int V2dTextColor (Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 3)
  {
    di << "Usage : v2dsettextcolor text_name color_index" << "\n";
    return -1;
  }

  TCollection_AsciiString aName (argv[1]);

  if (!myMapOfText.IsBound(aName))
  {
    di << "No such text object: " << aName.ToCString() << "\n";
    return -1;
  }

  int aColInd = atoi(argv[2]);

#ifdef OCC199
  // check color index
  Standard_Integer aColMin, aColMax;
  Viewer2dTest::CurrentView()->Driver()->ColorBoundIndexs(aColMin, aColMax);
  if (aColInd < aColMin || aColMax < aColInd)
  {
    di << "Color index must be between " << aColMin << " and " << aColMax << "\n";
    return -1;
  }
#endif

  myMapOfText(aName)->SetColorIndex(aColInd);
  Viewer2dTest::CurrentView()->Viewer()->Update();
  return 0;
}

//=======================================================================
//function : V2dText
//purpose  : v2dtext text_str position(x y) [angle scale font]
//=======================================================================
static int V2dText (Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 4 && argc != 7)
  {
    di << "Usage : v2dtext text_str position(x y) [angle scale font]" << "\n";
    return -1;
  }

  Handle(AIS2D_InteractiveContext) myAIScontext = Viewer2dTest::GetAIS2DContext();
  if (myAIScontext.IsNull())
  {
    di << "use 'v2dinit' command before " << argv[0] << "\n";
    return -1;
  }
  myAIScontext->SetPickMode(Graphic2d_PM_INTERSECT);
  Handle(V2d_View) V = Viewer2dTest::CurrentView();

  Handle(Graphic2d_GraphicObject) obj =
    new Graphic2d_GraphicObject(V->View());

  Quantity_PlaneAngle anAngle = 0.0;
  Quantity_Factor     aScale = 1.0;
  Standard_Integer    aFontIn = 0;

  if (argc > 4)
  {
    anAngle = atof(argv[4]);
    aScale  = atof(argv[5]);
    aFontIn = atoi(argv[6]);

#ifdef OCC198
    // check font index
    Standard_Integer aFontMin, aFontMax;
    Viewer2dTest::CurrentView()->Driver()->FontBoundIndexs(aFontMin, aFontMax);
    if (aFontIn < aFontMin || aFontMax < aFontIn)
    {
      di << "Font index must be between " << aFontMin << " and " << aFontMax << "\n";
      return -1;
    }
#endif
  }

  Handle(Graphic2d_Text) text =
    new Graphic2d_Text(obj, TCollection_ExtendedString(argv[1]),
                       atof(argv[2]), atof(argv[3]), anAngle,
                       Aspect_TOT_SOLID, aScale);
  text->SetFontIndex(aFontIn);

  obj->Display();
  V->Fitall();

  int i = myMapOfText.Extent();

  TCollection_AsciiString aTxtName ("text_");
  aTxtName += TCollection_AsciiString(i);

  myMapOfText.Bind(aTxtName, text);
  di << "Text " << aTxtName.ToCString() << " created" << "\n";
  return 0;
}

//==============================================================================
//function : V2Dump
//purpose  : To dump the active view snapshot to the image file
//Draw arg : Picture file name with extension corresponding to desired format
//==============================================================================
static Standard_Integer V2dDump (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Use: " << argv[0] << " <filename>.{png|bmp|jpg|gif}\n";
    return 1;
  }

  Handle(AIS2D_InteractiveContext) IC = Viewer2dTest::GetAIS2DContext();
  Handle(V2d_View) view = Viewer2dTest::CurrentView();
  if (view.IsNull())
  {
    di << "Cannot find an active viewer/view\n";
    return 1;
  }
#ifdef WNT
  Handle(WNT_Window) aWindow = Handle(WNT_Window)::DownCast (view->Driver()->Window());
#else
  Handle(Xw_Window) aWindow = Handle(Xw_Window)::DownCast (view->Driver()->Window());
#endif
  aWindow->Dump (argv[1]);
  return 0;
}

//=======================================================================
//function : ViewerCommands
//purpose  :
//=======================================================================
void Viewer2dTest::ViewerCommands(Draw_Interpretor& theCommands)
{
  const char *group = "2D AIS Viewer - Viewer Commands";

  theCommands.Add("v2dinit",
		  "v2dinit                       : v2dinit [leftPx topPx widthPx heightPx] : Create the Viewer window",
		  __FILE__, V2dInit, group);

  theCommands.Add("v2dsetbg",
		  "v2dsetbg imagefile [filltype] : Load image as background",
		  __FILE__, V2dSetBG, group);

  theCommands.Add("v2dgrid",
		  "v2dgrid [type [Origin(X Y) XStep/RadiusStep YStep/DivisionNb RotationAngle(in radians)] [drawmode]] : Load grid",
		  __FILE__, V2dGrid, group);

  theCommands.Add("v2drmgrid",
		  "v2drmgrid                     : Unload grid",
		  __FILE__, V2dGrid, group);

//  theCommands.Add("v2dplot",
//		  "v2dplot format filename       : Dump in file in certain format",
//		  __FILE__, V2dPlot, group);

  theCommands.Add("v2ddump",
                   "<filename>.{png|bmp|jpg|gif}\n\t\t: Dump contents of viewer window to PNG, BMP, JPEG or GIF file",
		  __FILE__, V2dDump, group);

  theCommands.Add("v2dpsout",
		  "v2dpsout imagefile [Scale TypeOfColorSpace] [Width Height XCenter YCenter] : PostScript Output",
		  __FILE__, V2dPSOut, group);

  theCommands.Add("v2dpick",
		  "v2dpick (MB1 in the viewer) : Print pixel coords and color",
		  __FILE__, V2dPick, group);

  theCommands.Add("v2dpickgrid",
		  "v2dpickgrid (MB1 in the viewer) : Print coordinates of a grid point near to MB1 click",
		  __FILE__, V2dPickGrid, group);

  theCommands.Add("v2dfit",
		  "v2dfit or <F>                 : Fit all shapes",
		  __FILE__, V2dFit, group);

  theCommands.Add("v2drepaint",
		  "v2drepaint                    : Force redraw",
		  __FILE__, V2dRepaint, group);

  theCommands.Add("v2dclear",
		  "v2dclear                      : Clear the viewer",
		  __FILE__, V2dClear, group);

  theCommands.Add("v2dtext", "v2dtext text_str position(x y) [angle scale font]", V2dText, group);
  theCommands.Add("v2dsettextcolor", "v2dsettextcolor text_name color_index", V2dTextColor, group);

//  theCommands.Add("v2dhelp",
//		  "v2dhelp                       : Display help on the viewer commands",
//		  __FILE__, V2dHelp, group);
}
