// File:  ViewerTest_ViewerCommands.cxx
// Created: Tue Sep  1 10:28:35 1998
// Author:  Robert COUBLANC
//    <rob@robox.paris1.matra-dtv.fr>

// Robert Boehne 30 May 2000 : Dec Osf

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef WNT
#include <windows.h>
#endif

#include <Graphic3d_ExportFormat.hxx>
#include <ViewerTest.hxx>
#include <ViewerTest_EventManager.hxx>
#include <Visual3d_View.hxx>
#include <NIS_View.hxx>
#include <NIS_Triangulated.hxx>
#include <NIS_InteractiveContext.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Aspect_PrintAlgo.hxx>
#include <Image_PixMap.hxx>

#ifndef WNT
#include <Graphic3d_GraphicDevice.hxx>
#include <Xw_GraphicDevice.hxx>
#include <Xw_WindowQuality.hxx>
#include <Xw_Window.hxx>
#include <X11/Xlib.h> /* contains some dangerous #defines such as Status, True etc. */
#include <X11/Xutil.h>
#include <tk.h>

#else

#include <Graphic3d_WNTGraphicDevice.hxx>
#include <WNT_WClass.hxx>
#include <WNT_Window.hxx>

#define _CRT_SECURE_NO_DEPRECATE
#pragma warning (disable:4996)

#endif

#define OCC120

//==============================================================================

//==============================================================================
//  VIEWER GLOBAL VARIABLES
//==============================================================================

Standard_IMPORT Standard_Boolean Draw_VirtualWindows;

Standard_EXPORT int ViewerMainLoop(Standard_Integer , const char** argv);
extern const Handle(NIS_InteractiveContext)& TheNISContext();

#ifdef WNT
static Handle(Graphic3d_WNTGraphicDevice)& GetG3dDevice(){
  static Handle(Graphic3d_WNTGraphicDevice) GD;
  return GD;
}

static Handle(WNT_Window)& VT_GetWindow() {
  static Handle(WNT_Window) WNTWin;
  return WNTWin;
}

#else
static Handle(Graphic3d_GraphicDevice)& GetG3dDevice(){
  static Handle(Graphic3d_GraphicDevice) GD;
  return GD;
}
static Handle(Xw_Window)& VT_GetWindow(){
  static Handle(Xw_Window) XWWin;
  return XWWin;
}
static Display *display;

static void VProcessEvents(ClientData,int);
#endif

#ifdef OCC120
static Standard_Boolean DegenerateMode = Standard_True;
#endif

#define ZCLIPWIDTH 1.

static void OSWindowSetup();

//==============================================================================
//  EVENT GLOBAL VARIABLES
//==============================================================================

static int Start_Rot = 0;
static int ZClipIsOn = 0;
static int X_Motion= 0,Y_Motion=0; // Current cursor position
static int X_ButtonPress = 0, Y_ButtonPress = 0; // Last ButtonPress position


//==============================================================================

#ifdef WNT
static LRESULT WINAPI ViewerWindowProc(
                                       HWND hwnd,
                                       UINT uMsg,
                                       WPARAM wParam,
                                       LPARAM lParam );
static LRESULT WINAPI AdvViewerWindowProc(
  HWND hwnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam );
#endif


//==============================================================================
//function : WClass
//purpose  :
//==============================================================================

const Handle(MMgt_TShared)& ViewerTest::WClass()
{
  static Handle(MMgt_TShared) theWClass;
#ifdef WNT
  if (theWClass.IsNull()) {
    theWClass = new WNT_WClass ("GW3D_Class", AdvViewerWindowProc,
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

void ViewerTest::ViewerInit (const Standard_Integer thePxLeft,  const Standard_Integer thePxTop,
                             const Standard_Integer thePxWidth, const Standard_Integer thePxHeight)
{
  static Standard_Boolean isFirst = Standard_True;

  Standard_Integer aPxLeft   = 0;
  Standard_Integer aPxTop    = 460;
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
    // Create the Graphic device
#ifdef WNT
    if (GetG3dDevice().IsNull()) GetG3dDevice() = new Graphic3d_WNTGraphicDevice();
    if (VT_GetWindow().IsNull())
    {
      // Create the Graphic device and the window
      Handle(WNT_GraphicDevice) g_Device = new WNT_GraphicDevice();

      VT_GetWindow() = new WNT_Window (g_Device, "Test3d",
                                       Handle(WNT_WClass)::DownCast (WClass()),
                                       WS_OVERLAPPEDWINDOW,
                                       aPxLeft, aPxTop,
                                       aPxWidth, aPxHeight,
                                       Quantity_NOC_BLACK);
      VT_GetWindow()->SetVirtual (Draw_VirtualWindows);
    }
#else
    if (GetG3dDevice().IsNull()) GetG3dDevice() =
      new Graphic3d_GraphicDevice (getenv ("DISPLAY"), Xw_TOM_READONLY);
    if (VT_GetWindow().IsNull())
    {
      VT_GetWindow() = new Xw_Window (GetG3dDevice(),
                                      "Test3d",
                                      aPxLeft, aPxTop,
                                      aPxWidth, aPxHeight,
                                      Xw_WQ_3DQUALITY,
                                      Quantity_NOC_BLACK);
      VT_GetWindow()->SetVirtual (Draw_VirtualWindows);
    }
#endif

    Handle(V3d_Viewer) a3DViewer, a3DCollector;
    // Viewer and View creation

    TCollection_ExtendedString NameOfWindow("Visu3D");

    a3DViewer = new V3d_Viewer(GetG3dDevice(), NameOfWindow.ToExtString());
    NameOfWindow = TCollection_ExtendedString("Collector");
    a3DCollector = new V3d_Viewer(GetG3dDevice(), NameOfWindow.ToExtString());
    a3DViewer->SetDefaultBackgroundColor(Quantity_NOC_BLACK);
    a3DCollector->SetDefaultBackgroundColor(Quantity_NOC_STEELBLUE);
    Handle(NIS_View) aView =
      Handle(NIS_View)::DownCast(ViewerTest::CurrentView());
    if ( aView.IsNull() ) {
      //       Handle (V3d_View) V = a3DViewer->CreateView();
      aView = new NIS_View (a3DViewer, VT_GetWindow());
      ViewerTest::CurrentView(aView);
      TheNISContext()->AttachView (aView);
    }
    Handle(V3d_View) a3DViewCol;
    if ( a3DViewCol.IsNull() ) a3DViewCol    = a3DViewer->CreateView();

    // AIS setup
    if ( ViewerTest::GetAISContext().IsNull() ) {
      Handle(AIS_InteractiveContext) C =
        new AIS_InteractiveContext(a3DViewer,a3DCollector);
      ViewerTest::SetAISContext(C);
    }

    // Setup for X11 or NT
    OSWindowSetup();
    // Viewer and View creation

    a3DViewer->SetDefaultBackgroundColor(Quantity_NOC_BLACK);

    Handle (V3d_View) V = ViewerTest::CurrentView();

    V->SetDegenerateModeOn();
#ifdef OCC120
    DegenerateMode = V->DegenerateModeIsOn();
#endif
    //    V->SetWindow(VT_GetWindow(), NULL, MyViewProc, NULL);

    V->SetZClippingDepth(0.5);
    V->SetZClippingWidth(ZCLIPWIDTH/2.);
    a3DViewer->SetDefaultLights();
    a3DViewer->SetLightOn();

#ifndef WNT
#if TCL_MAJOR_VERSION  < 8
    Tk_CreateFileHandler((void*)ConnectionNumber(display),
      TK_READABLE, VProcessEvents, (ClientData) VT_GetWindow()->XWindow() );
#else
    Tk_CreateFileHandler(ConnectionNumber(display),
      TK_READABLE, VProcessEvents, (ClientData) VT_GetWindow()->XWindow() );
#endif
#endif

    isFirst = Standard_False;
  }
  VT_GetWindow()->Map();
}

//==============================================================================
//function : Vinit
//purpose  : Create the window viewer and initialize all the global variable
//    Use Tk_CreateFileHandler on UNIX to cath the X11 Viewer event
//==============================================================================

static int VInit (Draw_Interpretor& , Standard_Integer argc, const char** argv)
{
  Standard_Integer aPxLeft   = (argc > 1) ? atoi (argv[1]) : 0;
  Standard_Integer aPxTop    = (argc > 2) ? atoi (argv[2]) : 0;
  Standard_Integer aPxWidth  = (argc > 3) ? atoi (argv[3]) : 0;
  Standard_Integer aPxHeight = (argc > 4) ? atoi (argv[4]) : 0;
  ViewerTest::ViewerInit (aPxLeft, aPxTop, aPxWidth, aPxHeight);
  return 0;
}

//==============================================================================
//function : ProcessKeyPress
//purpose  : Handle KeyPress event from a CString
//==============================================================================

static void ProcessKeyPress( char *buf_ret )
{
  //cout << "KeyPress" << endl;
  const Handle(V3d_View) aView = ViewerTest::CurrentView();
  const Handle(NIS_View) aNisView = Handle(NIS_View)::DownCast (aView);
  // Letter in alphabetic order

  if ( !strcasecmp(buf_ret, "A") ) {
    // AXO
    aView->SetProj(V3d_XposYnegZpos);
  }
  else if ( !strcasecmp(buf_ret, "D") ) {
    // Reset
    aView->Reset();
  }
  else if ( !strcasecmp(buf_ret, "F") ) {
    // FitAll
    if (aNisView.IsNull())
      aView->FitAll();
    else
      aNisView->FitAll3d();
  }
  else if ( !strcasecmp(buf_ret, "H") ) {
    // HLR
    cout << "HLR" << endl;
#ifdef OCC120
    if (aView->DegenerateModeIsOn()) ViewerTest::CurrentView()->SetDegenerateModeOff();
    else aView->SetDegenerateModeOn();
    DegenerateMode = aView->DegenerateModeIsOn();
#else
    ViewerTest::CurrentView()->SetDegenerateModeOff();
#endif
  }
  else if ( !strcasecmp(buf_ret, "D") ) {
    // Reset
    aView->Reset();
  }
  else if ( !strcasecmp(buf_ret, "S") ) {
    // SHADING
    cout << "passage en mode 1 (shading pour les shapes)" << endl;
#ifndef OCC120
    ViewerTest::CurrentView()->SetDegenerateModeOn();
#endif
    Handle(AIS_InteractiveContext) Ctx = ViewerTest::GetAISContext();
    if(Ctx->NbCurrents()==0 ||
      Ctx->NbSelected()==0)
      Ctx->SetDisplayMode(AIS_Shaded);
    else{
      if(Ctx->HasOpenedContext()){
        for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected())
          Ctx->SetDisplayMode(Ctx->Interactive(),1,Standard_False);
      }
      else{
        for(Ctx->InitCurrent();Ctx->MoreCurrent();Ctx->NextCurrent())
          Ctx->SetDisplayMode(Ctx->Current(),1,Standard_False);
      }
      Ctx->UpdateCurrentViewer();
    }
  }
  else if ( !strcasecmp(buf_ret, "U") ) {
    // SHADING
    cout<<"passage au mode par defaut"<<endl;
#ifndef OCC120
    ViewerTest::CurrentView()->SetDegenerateModeOn();
#endif
    Handle(AIS_InteractiveContext) Ctx = ViewerTest::GetAISContext();
    if(Ctx->NbCurrents()==0 ||
      Ctx->NbSelected()==0)
      Ctx->SetDisplayMode(AIS_WireFrame);
    else{
      if(Ctx->HasOpenedContext()){
        for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected())
          Ctx->UnsetDisplayMode(Ctx->Interactive(),Standard_False);
      }
      else{
        for(Ctx->InitCurrent();Ctx->MoreCurrent();Ctx->NextCurrent())
          Ctx->UnsetDisplayMode(Ctx->Current(),Standard_False);
      }
      Ctx->UpdateCurrentViewer();
    }

  }
  else if ( !strcasecmp(buf_ret, "T") ) {
    // Top
    aView->SetProj(V3d_Zpos);
  }
  else if ( !strcasecmp(buf_ret, "B") ) {
    // Top
    aView->SetProj(V3d_Zneg);
  }
  else if ( !strcasecmp(buf_ret, "L") ) {
    // Top
    aView->SetProj(V3d_Xneg);
  }
  else if ( !strcasecmp(buf_ret, "R") ) {
    // Top
    aView->SetProj(V3d_Xpos);
  }

  else if ( !strcasecmp(buf_ret, "W") ) {
    // WIREFRAME
#ifndef OCC120
    ViewerTest::CurrentView()->SetDegenerateModeOn();
#endif
    cout << "passage en mode 0 (filaire pour les shapes)" << endl;
#ifndef OCC120
    ViewerTest::CurrentView()->SetDegenerateModeOn();
#endif
    Handle(AIS_InteractiveContext) Ctx = ViewerTest::GetAISContext();
    if(Ctx->NbCurrents()==0 ||
      Ctx->NbSelected()==0)
      Ctx->SetDisplayMode(AIS_WireFrame);
    else{
      if(Ctx->HasOpenedContext()){
        for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected())
          Ctx->SetDisplayMode(Ctx->Interactive(),0,Standard_False);
      }
      else{
        for(Ctx->InitCurrent();Ctx->MoreCurrent();Ctx->NextCurrent())
          Ctx->SetDisplayMode(Ctx->Current(),0,Standard_False);
      }
      Ctx->UpdateCurrentViewer();
    }
  }
  else if ( !strcasecmp(buf_ret, "Z") ) {
    // ZCLIP

    if ( ZClipIsOn ) {
      cout << "ZClipping OFF" << endl;
      ZClipIsOn = 0;

      aView->SetZClippingType(V3d_OFF);
      aView->Redraw();
    }
    else {
      cout << "ZClipping ON" << endl;
      ZClipIsOn = 1;

      aView->SetZClippingType(V3d_FRONT);
      aView->Redraw();
    }
  }
  else if ( !strcasecmp(buf_ret, ",") ) {
    ViewerTest::GetAISContext()->HilightNextDetected(ViewerTest::CurrentView());


  }
  else if ( !strcasecmp(buf_ret, ".") ) {
    ViewerTest::GetAISContext()->HilightPreviousDetected(ViewerTest::CurrentView());
  }
  // Number
  else{
    Standard_Integer Num = atoi(buf_ret);
    if(Num>=0 && Num<=7)
      ViewerTest::StandardModeActivation(Num);
  }
}

//==============================================================================
//function : ProcessExpose
//purpose  : Redraw the View on an Expose Event
//==============================================================================

static void ProcessExpose(  )
{ //cout << "Expose" << endl;
  ViewerTest::CurrentView()->Redraw();
}

//==============================================================================
//function : ProcessConfigure
//purpose  : Resize the View on an Configure Event
//==============================================================================

static void ProcessConfigure()
{
  Handle(V3d_View) V = ViewerTest::CurrentView();
  V->MustBeResized();
  V->Update();
  V->Redraw();
}

//==============================================================================
//function : ProcessButton1Press
//purpose  : Picking
//==============================================================================

static Standard_Boolean ProcessButton1Press(
  Standard_Integer ,
  const char** argv,
  Standard_Boolean pick,
  Standard_Boolean shift )
{
  Handle(ViewerTest_EventManager) EM = ViewerTest::CurrentEventManager();
  if ( pick ) {
    Standard_Real X, Y, Z;

    ViewerTest::CurrentView()->Convert(X_Motion, Y_Motion, X, Y, Z);

    Draw::Set(argv[1], X);
    Draw::Set(argv[2], Y);
    Draw::Set(argv[3], Z);}

  if(shift)
    EM->ShiftSelect();
  else
    EM->Select();

  pick = 0;
  return pick;
}

//==============================================================================
//function : ProcessButton3Press
//purpose  : Start Rotation
//==============================================================================

static void ProcessButton3Press()

{ // Start rotation
  Start_Rot = 1;
  ViewerTest::CurrentView()->SetDegenerateModeOn();
  ViewerTest::CurrentView()->StartRotation( X_ButtonPress, Y_ButtonPress );

}
//==============================================================================
//function : ProcessButtonRelease
//purpose  : Start Rotation
//==============================================================================

static void ProcessButtonRelease()

{ // End rotation
#ifdef OCC120
  if (Start_Rot) {
    Start_Rot = 0;
    if (!DegenerateMode) ViewerTest::CurrentView()->SetDegenerateModeOff();
  }
#else
  Start_Rot = 0;
  ViewerTest::CurrentView()->SetDegenerateModeOff();
#endif
}

//==============================================================================
//function : ProcessZClipMotion
//purpose  : Zoom
//==============================================================================

void ProcessZClipMotion()
{
  Handle(V3d_View)  a3DView = ViewerTest::CurrentView();
  if ( Abs(X_Motion - X_ButtonPress) > 2 ) {
    static Standard_Real CurZPos = 0.;

    //Quantity_Length VDX, VDY;
    //a3DView->Size(VDX,VDY);
    //Standard_Real VDZ = a3DView->ZSize();
    //printf("View size (%lf,%lf,%lf)\n", VDX, VDY, VDZ);

    Quantity_Length dx = a3DView->Convert(X_Motion - X_ButtonPress);

    // Front = Depth + width/2.
    Standard_Real D = 0.5;
    Standard_Real W = 0.1;

    CurZPos += (dx);

    D += CurZPos;

    //printf("dx %lf Depth %lf Width %lf\n", dx, D, W);

    a3DView->SetZClippingType(V3d_OFF);
    a3DView->SetZClippingDepth(D);
    a3DView->SetZClippingWidth(W);
    a3DView->SetZClippingType(V3d_FRONT);

    a3DView->Redraw();

    X_ButtonPress = X_Motion;
    Y_ButtonPress = Y_Motion;
  }
}

//==============================================================================
//function : ProcessControlButton1Motion
//purpose  : Zoom
//==============================================================================

static void ProcessControlButton1Motion()
{
  ViewerTest::CurrentView()->Zoom( X_ButtonPress, Y_ButtonPress, X_Motion, Y_Motion);

  X_ButtonPress = X_Motion;
  Y_ButtonPress = Y_Motion;
}

//==============================================================================
//function : ProcessControlButton2Motion
//purpose  : Pann
//==============================================================================

static void ProcessControlButton2Motion()
{
  Quantity_Length dx = ViewerTest::CurrentView()->Convert(X_Motion - X_ButtonPress);
  Quantity_Length dy = ViewerTest::CurrentView()->Convert(Y_Motion - Y_ButtonPress);

  dy = -dy; // Xwindow Y axis is from top to Bottom

  ViewerTest::CurrentView()->Panning( dx, dy );

  X_ButtonPress = X_Motion;
  Y_ButtonPress = Y_Motion;
}

//==============================================================================
//function : ProcessControlButton3Motion
//purpose  : Rotation
//==============================================================================

static void ProcessControlButton3Motion()
{
  if ( Start_Rot ) ViewerTest::CurrentView()->Rotation( X_Motion, Y_Motion);
}

//==============================================================================
//function : ProcessPointerMotion
//purpose  : Rotation
//==============================================================================

static void ProcessMotion()
{
  //pre-hilights detected objects at mouse position

  Handle(ViewerTest_EventManager) EM = ViewerTest::CurrentEventManager();
  EM->MoveTo(X_Motion, Y_Motion);
}


void ViewerTest::GetMousePosition(Standard_Integer& Xpix,Standard_Integer& Ypix)
{
  Xpix = X_Motion;Ypix=Y_Motion;
}

//==============================================================================
//function : VAxo
//purpose  : Switch to an Axonometric view
//Draw arg : No args
//==============================================================================

static int VAxo(Draw_Interpretor& di, Standard_Integer , const char** )
{ if ( ViewerTest::CurrentView().IsNull() ) {
  di<<"La commande vinit n'a pas ete appele avant"<<"\n";
  //  VInit(di, argc, argv);
}

ViewerTest::CurrentView()->SetProj(V3d_XposYnegZpos);

return 0;
}

//==============================================================================
//function : VTop
//purpose  : Switch to a Top View
//Draw arg : No args
//==============================================================================

static int VTop(Draw_Interpretor& di, Standard_Integer , const char** )
{

  if ( ViewerTest::CurrentView().IsNull() ) {
    di<<"La commande vinit n'a pas ete appele avant"<<"\n";

    //  VInit(di, , argv);
  }

  ViewerTest::CurrentView()->SetProj(V3d_Zpos);
  return 0;

}

//==============================================================================
//function : VHelp
//purpose  : Dsiplay help on viewer Keyboead and mouse commands
//Draw arg : No args
//==============================================================================

static int VHelp(Draw_Interpretor& di, Standard_Integer , const char** )
{

  di << "Q : Quit the application" << "\n";

  di << "========================="<<"\n";
  di << "F : FitAll" << "\n";
  di << "T : TopView" << "\n";
  di << "A : AxonometricView" << "\n";
  di << "R : ResetView" << "\n";

  di << "========================="<<"\n";
  di << "S : Shading" << "\n";
  di << "W : Wireframe" << "\n";
  di << "H : HidelLineRemoval" << "\n";

  di << "========================="<<"\n";
  di << "Selection mode "<<"\n";
  di << "0 : Shape" <<"\n";
  di << "1 : Vertex" <<"\n";
  di << "2 : Edge" <<"\n";
  di << "3 : Wire" <<"\n";
  di << "4 : Face" <<"\n";
  di << "5 : Shell" <<"\n";
  di << "6 : Solid" <<"\n";
  di << "7 : Compound" <<"\n";

  di << "=========================="<<"\n";
  di << "D : Remove Selected Object"<<"\n";
  di << "=========================="<<"\n";

  return 0;
}

Standard_Boolean IsDragged = Standard_False;

Standard_Integer xx1, yy1, xx2, yy2;
//the first and last point in viewer co-ordinates

Standard_Boolean DragFirst;


#ifdef WNT

static Standard_Boolean Ppick = 0;
static Standard_Integer Pargc = 0;
static const char**           Pargv = NULL;


static LRESULT WINAPI AdvViewerWindowProc( HWND hwnd,
                                          UINT Msg,
                                          WPARAM wParam,
                                          LPARAM lParam )
{
  if ( !ViewerTest::CurrentView().IsNull() ) {

    WPARAM fwKeys = wParam;

    switch( Msg ) {

    case WM_LBUTTONUP:
      IsDragged = Standard_False;
      if( !DragFirst )
      {
        HDC hdc = GetDC( hwnd );
        HGDIOBJ anObj = SelectObject( hdc, GetStockObject( WHITE_PEN ) );
        SelectObject( hdc, GetStockObject( HOLLOW_BRUSH ) );
        SetROP2( hdc, R2_NOT );
        Rectangle( hdc, xx1, yy1, xx2, yy2 );
        ReleaseDC( hwnd, hdc );

        const Handle(ViewerTest_EventManager) EM =
          ViewerTest::CurrentEventManager();
        if ( fwKeys & MK_SHIFT )
          EM->ShiftSelect( min( xx1, xx2 ), max( yy1, yy2 ),
          max( xx1, xx2 ), min( yy1, yy2 ));
        else
          EM->Select( min( xx1, xx2 ), max( yy1, yy2 ),
          max( xx1, xx2 ), min( yy1, yy2 ));
      }
      return ViewerWindowProc( hwnd, Msg, wParam, lParam );

    case WM_LBUTTONDOWN:
      if( fwKeys == MK_LBUTTON || fwKeys == ( MK_LBUTTON | MK_SHIFT ) )
      {
        IsDragged = Standard_True;
        DragFirst = Standard_True;
        xx1 = LOWORD(lParam);
        yy1 = HIWORD(lParam);
      }
      return ViewerWindowProc( hwnd, Msg, wParam, lParam );

      break;

    case WM_MOUSEMOVE:
      if( IsDragged )
      {
        HDC hdc = GetDC( hwnd );

        HGDIOBJ anObj = SelectObject( hdc, GetStockObject( WHITE_PEN ) );
        SelectObject( hdc, GetStockObject( HOLLOW_BRUSH ) );
        SetROP2( hdc, R2_NOT );

        if( !DragFirst )
          Rectangle( hdc, xx1, yy1, xx2, yy2 );

        DragFirst = Standard_False;
        xx2 = LOWORD(lParam);
        yy2 = HIWORD(lParam);

        Rectangle( hdc, xx1, yy1, xx2, yy2 );

        SelectObject( hdc, anObj );

        ReleaseDC( hwnd, hdc );
      }
      else
        return ViewerWindowProc( hwnd, Msg, wParam, lParam );
      break;

    default:
      return ViewerWindowProc( hwnd, Msg, wParam, lParam );
    }
    return 0;
  }
  return ViewerWindowProc( hwnd, Msg, wParam, lParam );
}


static LRESULT WINAPI ViewerWindowProc( HWND hwnd,
                                       UINT Msg,
                                       WPARAM wParam,
                                       LPARAM lParam )
{
  /*static Standard_Boolean Ppick = 0;
  static Standard_Integer Pargc = 0;
  static char**           Pargv = NULL;*/

  static int Up = 1;

  if ( !ViewerTest::CurrentView().IsNull() ) {
    PAINTSTRUCT    ps;

    switch( Msg ) {
    case WM_CLOSE:
      // do not destroy the window - just hide it!
      VT_GetWindow()->Unmap();
      return 0;
    case WM_PAINT:
      //cout << "\t WM_PAINT" << endl;
      BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
      ProcessExpose();
      break;

    case WM_SIZE:
      //cout << "\t WM_SIZE" << endl;
      ProcessConfigure();
      break;

    case WM_KEYDOWN:
      //cout << "\t WM_KEYDOWN " << (int) wParam << endl;

      if ( (wParam != VK_SHIFT) && (wParam != VK_CONTROL) ) {
        char c[2];
        c[0] = (char) wParam;
        c[1] = '\0';
        ProcessKeyPress( c);
      }
      break;

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
      //cout << "\t WM_xBUTTONUP" << endl;
      Up = 1;
      ProcessButtonRelease();
      break;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
      {
        //cout << "\t WM_xBUTTONDOWN" << endl;
        WPARAM fwKeys = wParam;

        Up = 0;

        X_ButtonPress = LOWORD(lParam);
        Y_ButtonPress = HIWORD(lParam);

        if ( Msg == WM_LBUTTONDOWN) {
          if(fwKeys & MK_CONTROL) {
            Ppick = ProcessButton1Press( Pargc, Pargv, Ppick,  (fwKeys & MK_SHIFT) );
          } else
            ProcessButton1Press( Pargc, Pargv, Ppick,  (fwKeys & MK_SHIFT) );
        }
        else if ( Msg == WM_RBUTTONDOWN ) {
          // Start rotation
          ProcessButton3Press( );
        }
      }
      break;

    case WM_MOUSEMOVE:
      {
        //cout << "\t WM_MOUSEMOVE" << endl;
        WPARAM fwKeys = wParam;
        X_Motion = LOWORD(lParam);
        Y_Motion = HIWORD(lParam);

        if ( Up &&
          fwKeys & ( MK_LBUTTON|MK_MBUTTON|MK_RBUTTON ) ) {
            Up = 0;
            X_ButtonPress = LOWORD(lParam);
            Y_ButtonPress = HIWORD(lParam);

            if ( fwKeys & MK_RBUTTON ) {
              // Start rotation
              ProcessButton3Press();
            }
          }

          if ( fwKeys & MK_CONTROL ) {
            if ( fwKeys & MK_LBUTTON ) {
              ProcessControlButton1Motion();
            }
            else if ( fwKeys & MK_MBUTTON ||
              ((fwKeys&MK_LBUTTON) &&
              (fwKeys&MK_RBUTTON) ) ){
                ProcessControlButton2Motion();
              }
            else if ( fwKeys & MK_RBUTTON ) {
              ProcessControlButton3Motion();
            }
          }
#ifdef BUG
          else if ( fwKeys & MK_SHIFT ) {
            if ( fwKeys & MK_MBUTTON ||
              ((fwKeys&MK_LBUTTON) &&
              (fwKeys&MK_RBUTTON) ) ) {
                cout << "ProcessZClipMotion()" << endl;
                ProcessZClipMotion();
              }
          }
#endif
          else
            if (( fwKeys & MK_MBUTTON || ((fwKeys&MK_LBUTTON) && (fwKeys&MK_RBUTTON) ) )){
              ProcessZClipMotion();
            }
            else {
              ProcessMotion();
            }
      }
      break;

    default:
      return( DefWindowProc( hwnd, Msg, wParam, lParam ));
    }
    return 0L;
  }

  return DefWindowProc( hwnd, Msg, wParam, lParam );
}




//==============================================================================
//function : ViewerMainLoop
//purpose  : Get a Event on the view and dispatch it
//==============================================================================


static int ViewerMainLoop(Standard_Integer argc, const char** argv)
{

  //cout << "No yet implemented on WNT" << endl;
  /*static Standard_Boolean Ppick = 0;
  static Standard_Integer Pargc = 0;
  static char**           Pargv = NULL;*/

  //Ppick = (argc > 0)? -1 : 0;
  Ppick = (argc > 0)? 1 : 0;
  Pargc = argc;
  Pargv = argv;

  if ( Ppick ) {
    MSG msg;
    msg.wParam = 1;

    cout << "Start picking" << endl;

    //while ( Ppick == -1 ) {
    while ( Ppick == 1 ) {
      // Wait for a ProcessButton1Press() to toggle pick to 1 or 0
      if (GetMessage(&msg, NULL, 0, 0) ) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }

    cout << "Picking done" << endl;
  }

  return Ppick;
}


#else

int min( int a, int b )
{
  if( a<b )
    return a;
  else
    return b;
}

int max( int a, int b )
{
  if( a>b )
    return a;
  else
    return b;
}

int ViewerMainLoop(Standard_Integer argc, const char** argv)

{ Standard_Boolean pick = argc > 0;

// X11 Event loop

static XEvent report;

XNextEvent( display, &report );
//    cout << "rep type = " << report.type << endl;
//    cout << "rep button = " << report.xbutton.button << endl;

switch ( report.type ) {
      case Expose:
        {
          ProcessExpose();
        }
        break;
      case ConfigureNotify:
        {
          ProcessConfigure();
        }
        break;
      case KeyPress:
        {

          KeySym ks_ret ;
          char buf_ret[11] ;
          int ret_len ;
          XComposeStatus status_in_out;

          ret_len = XLookupString( ( XKeyEvent *)&report ,
            (char *) buf_ret , 10 ,
            &ks_ret , &status_in_out ) ;


          buf_ret[ret_len] = '\0' ;

          if ( ret_len ) {
            ProcessKeyPress( buf_ret);
          }
        }
        break;
      case ButtonPress:
        //  cout << "ButtonPress" << endl;
        {
          X_ButtonPress = report.xbutton.x;
          Y_ButtonPress = report.xbutton.y;

          if ( report.xbutton.button == Button1 )
            if(  report.xbutton.state & ControlMask )
              pick = ProcessButton1Press( argc, argv, pick,
              ( report.xbutton.state & ShiftMask) );
            else
            {
              IsDragged = Standard_True;
              xx1 = X_ButtonPress;
              yy1 = Y_ButtonPress;
              DragFirst = Standard_True;
            }
          else if ( report.xbutton.button == Button3 )
            // Start rotation
            ProcessButton3Press();
        }
        break;
      case ButtonRelease:
        {
          //    cout<<"relachement du bouton "<<(report.xbutton.button==3 ? "3": "on s'en fout") <<endl;
          //    cout << IsDragged << endl;
          //    cout << DragFirst << endl;

          if( IsDragged )
          {
            if( !DragFirst )
            {
              Aspect_Handle aWindow = VT_GetWindow()->XWindow();
              GC gc = XCreateGC( display, aWindow, 0, 0 );
              //  XSetFunction( display, gc, GXinvert );
              XDrawRectangle( display, aWindow, gc, min( xx1, xx2 ), min( yy1, yy2 ), abs( xx2-xx1 ), abs( yy2-yy1 ) );
            }

            Handle( AIS_InteractiveContext ) aContext = ViewerTest::GetAISContext();
            if( aContext.IsNull() )
            {
              cout << "The context is null. Please use vinit before createmesh" << endl;
              return 0;
            }

            Standard_Boolean ShiftPressed = ( report.xbutton.state & ShiftMask );
            if( report.xbutton.button==1 )
              if( DragFirst )
                if( ShiftPressed )
                {
                  aContext->ShiftSelect();
                  //                   cout << "shift select" << endl;
                }
                else
                {
                  aContext->Select();
                  //                   cout << "select" << endl;
                }
              else
                if( ShiftPressed )
                {
                  aContext->ShiftSelect( min( xx1, xx2 ), min( yy1, yy2 ),
                    max( xx1, xx2 ), max( yy1, yy2 ),
                    ViewerTest::CurrentView());
                  //                   cout << "shift select" << endl;
                }
                else
                {
                  aContext->Select( min( xx1, xx2 ), min( yy1, yy2 ),
                    max( xx1, xx2 ), max( yy1, yy2 ),
                    ViewerTest::CurrentView() );
                  //                   cout << "select" << endl;
                }
            else
              ProcessButtonRelease();

            IsDragged = Standard_False;
          }
          else
            ProcessButtonRelease();
        }
        break;
      case MotionNotify:
        {
          //    XEvent dummy;

          X_Motion = report.xmotion.x;
          Y_Motion = report.xmotion.y;

          if( IsDragged )
          {
            Aspect_Handle aWindow = VT_GetWindow()->XWindow();
            GC gc = XCreateGC( display, aWindow, 0, 0 );
            XSetFunction( display, gc, GXinvert );

            if( !DragFirst )
              XDrawRectangle( display, aWindow, gc, min( xx1, xx2 ), min( yy1, yy2 ), abs( xx2-xx1 ), abs( yy2-yy1 ) );

            xx2 = X_Motion;
            yy2 = Y_Motion;
            DragFirst = Standard_False;

            //cout << "draw rect : " << xx2 << ", " << yy2 << endl;
            XDrawRectangle( display, aWindow, gc, min( xx1, xx2 ), min( yy1, yy2 ), abs( xx2-xx1 ), abs( yy2-yy1 ) );
          }
          else
          {

            //cout << "MotionNotify " << X_Motion << "," << Y_Motion << endl;

            // remove all the ButtonMotionMask
            while( XCheckMaskEvent( display, ButtonMotionMask, &report) ) ;

            if ( ZClipIsOn && report.xmotion.state & ShiftMask ) {
              if ( Abs(X_Motion - X_ButtonPress) > 2 ) {

                Quantity_Length VDX, VDY;

                ViewerTest::CurrentView()->Size(VDX,VDY);
                Standard_Real VDZ =0 ;
                VDZ = ViewerTest::CurrentView()->ZSize();

                //          printf("%lf,%lf,%lf\n", VDX, VDY, VDZ);
                printf("%f,%f,%f\n", VDX, VDY, VDZ);

                Quantity_Length dx = 0 ;
                dx = ViewerTest::CurrentView()->Convert(X_Motion - X_ButtonPress);

                cout << dx << endl;

                dx = dx / VDX * VDZ;

                cout << dx << endl;

                // Front = Depth + width/2.
                //ViewerTest::CurrentView()->SetZClippingDepth(dx);
                //ViewerTest::CurrentView()->SetZClippingWidth(0.);

                ViewerTest::CurrentView()->Redraw();
              }
            }

            if ( report.xmotion.state & ControlMask ) {
              if ( report.xmotion.state & Button1Mask ) {
                ProcessControlButton1Motion();
              }
              else if ( report.xmotion.state & Button2Mask ) {
                ProcessControlButton2Motion();
              }
              else if ( report.xmotion.state & Button3Mask ) {
                ProcessControlButton3Motion();
              }
            }
            else {
              ProcessMotion();
            }
          }
        }
        break;
}


return pick;
}

//==============================================================================
//function : VProcessEvents
//purpose  : call by Tk_CreateFileHandler() to be able to manage the
//       event in the Viewer window
//==============================================================================

static void VProcessEvents(ClientData,int)
{
  //cout << "VProcessEvents" << endl;

  // test for X Event
  while (XPending(display)) {
    ViewerMainLoop( 0, NULL);
  }
}
#endif

//==============================================================================
//function : OSWindowSetup
//purpose  : Setup for the X11 window to be able to cath the event
//==============================================================================


static void OSWindowSetup()
{
#ifndef WNT
  // X11

  Window  window   = VT_GetWindow()->XWindow();

  Standard_Address theDisplay = GetG3dDevice()->XDisplay();
  display = (Display * ) theDisplay;
  //  display = (Display *)GetG3dDevice()->XDisplay();

  XSynchronize(display, 1);

  VT_GetWindow()->Map();

  // X11 : For keyboard on SUN
  XWMHints wmhints;
  wmhints.flags = InputHint;
  wmhints.input = 1;

  XSetWMHints( display, window, &wmhints);

  XSelectInput( display, window,  ExposureMask | KeyPressMask |
    ButtonPressMask | ButtonReleaseMask |
    StructureNotifyMask |
    PointerMotionMask |
    Button1MotionMask | Button2MotionMask |
    Button3MotionMask
    );

  XSynchronize(display, 0);

#else
  // WNT
#endif

}


//==============================================================================
//function : VFit

//purpose  : Fitall, no DRAW arguments
//Draw arg : No args
//==============================================================================

static int VFit(Draw_Interpretor& , Standard_Integer , const char** )
{
  const Handle(V3d_View) aView = ViewerTest::CurrentView();
  Handle(NIS_View) V = Handle(NIS_View)::DownCast(aView);
  if (V.IsNull() == Standard_False) {
    V->FitAll3d();
  } else if (aView.IsNull() == Standard_False) {
    aView->FitAll();
  }
  return 0;
}

//==============================================================================
//function : VZFit
//purpose  : ZFitall, no DRAW arguments
//Draw arg : No args
//==============================================================================

static int VZFit(Draw_Interpretor& , Standard_Integer , const char** )
{
  Handle(V3d_View) V = ViewerTest::CurrentView();
  if ( !V.IsNull() ) V->ZFitAll(); return 0; }


static int VRepaint(Draw_Interpretor& , Standard_Integer , const char** )
{
  Handle(V3d_View) V = ViewerTest::CurrentView();
  if ( !V.IsNull() ) V->Redraw(); return 0;
}


//==============================================================================
//function : VClear
//purpose  : Remove all the object from the viewer
//Draw arg : No args
//==============================================================================

static int VClear(Draw_Interpretor& , Standard_Integer , const char** )
{
  Handle(V3d_View) V = ViewerTest::CurrentView();
  if(!V.IsNull())
    ViewerTest::Clear();
  return 0;
}

//==============================================================================
//function : VPick
//purpose  :
//==============================================================================

static int VPick(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{ if (ViewerTest::CurrentView().IsNull() ) return 1;

if ( argc < 4 ) {
  di << argv[0] << "Invalid number of arguments" << "\n";
  return 1;
}

while (ViewerMainLoop( argc, argv)) {
}

return 0;
}



//==============================================================================
//function : InitViewerTest
//purpose  : initialisation de toutes les variables static de  ViewerTest (dp)
//==============================================================================

void ViewerTest_InitViewerTest (const Handle(AIS_InteractiveContext)& context)
{
  Handle(V3d_Viewer) viewer = context->CurrentViewer();
  ViewerTest::SetAISContext(context);
  viewer->InitActiveViews();
  Handle(V3d_View) view = viewer->ActiveView();
  if (viewer->MoreActiveViews()) ViewerTest::CurrentView(view);
  ViewerTest::ResetEventManager();
  Handle(Aspect_GraphicDevice) device = viewer->Device();
  Handle(Aspect_Window) window = view->Window();
#ifndef WNT
  // X11
  VT_GetWindow() = Handle(Xw_Window)::DownCast(window);
  GetG3dDevice() = Handle(Graphic3d_GraphicDevice)::DownCast(device);
  OSWindowSetup();
  static int first = 1;
  if ( first ) {
#if TCL_MAJOR_VERSION  < 8
    Tk_CreateFileHandler((void*)ConnectionNumber(display),
      TK_READABLE, VProcessEvents, (ClientData) 0);
#else
    Tk_CreateFileHandler(ConnectionNumber(display),
      TK_READABLE, VProcessEvents, (ClientData) 0);
#endif
    first = 0;
  }
#endif
}


//==============================================================================
//function : VSetBg
//purpose  : Load image as background
//==============================================================================

static int VSetBg(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2 || argc > 3)
  {
    di << "Usage : " << argv[0] << " imagefile [filltype] : Load image as background" << "\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) AISContext = ViewerTest::GetAISContext();
  if(AISContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

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
      return 1;
    }
  }

  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  V3dView->SetBackgroundImage(argv[1], aFillType, Standard_True);

  return 0;
}

//==============================================================================
//function : VSetGradientBg
//purpose  : Mount gradient background
//==============================================================================
static int VSetGradientBg(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 8 )
  {
    di << "Usage : " << argv[0] << " R1 G1 B1 R2 G2 B2 Type : Mount gradient background" << "\n";
    di << "R1,G1,B1,R2,G2,B2 = [0..255]" << "\n";
    di << "Type must be one of 0 = NONE, 1 = HOR, 2 = VER, 3 = DIAG1, 4 = DIAG2" << "\n";
    di << "                    5 = CORNER1, 6 = CORNER2, 7 = CORNER3, 8 = CORNER4" << "\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) AISContext = ViewerTest::GetAISContext();
  if(AISContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if (argc == 8)
  {

    Standard_Real R1 = atof(argv[1])/255.;
    Standard_Real G1 = atof(argv[2])/255.;
    Standard_Real B1 = atof(argv[3])/255.;
    Quantity_Color aColor1(R1,G1,B1,Quantity_TOC_RGB);

    Standard_Real R2 = atof(argv[4])/255.;
    Standard_Real G2 = atof(argv[5])/255.;
    Standard_Real B2 = atof(argv[6])/255.;

    Quantity_Color aColor2(R2,G2,B2,Quantity_TOC_RGB);
    int aType = atoi(argv[7]);
    if( aType < 0 || aType > 8 )
    {
      di << "Wrong fill type " << "\n";
      di << "Must be one of 0 = NONE, 1 = HOR, 2 = VER, 3 = DIAG1, 4 = DIAG2" << "\n";
      di << "               5 = CORNER1, 6 = CORNER2, 7 = CORNER3, 8 = CORNER4" << "\n";
      return 1;
    }

    Aspect_GradientFillMethod aMethod = Aspect_GradientFillMethod(aType);

    Handle(V3d_View) V3dView = ViewerTest::CurrentView();
    V3dView->SetBgGradientColors( aColor1, aColor2, aMethod, 1);
  }

  return 0;
}

//==============================================================================
//function : VScale
//purpose  : View Scaling
//==============================================================================

static int VScale(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  if ( V3dView.IsNull() ) return 1;

  if ( argc != 4 ) {
    di << argv[0] << "Invalid number of arguments" << "\n";
    return 1;
  }
  V3dView->SetAxialScale( atof(argv[1]),  atof(argv[2]),  atof(argv[3]) );
  return 0;
}
//==============================================================================
//function : VTestZBuffTrihedron
//purpose  : Displays a V3d_ZBUFFER'ed trihedron at the bottom left corner of the view
//==============================================================================

static int VTestZBuffTrihedron(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  if ( V3dView.IsNull() ) return 1;

  // Set up default trihedron parameters
  V3dView->ZBufferTriedronSetup();
  V3dView->TriedronDisplay( Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.1, V3d_ZBUFFER );
  V3dView->ZFitAll();
  return 0;
}

//==============================================================================
//function : VRotate
//purpose  : Camera Rotating
//==============================================================================

static int VRotate( Draw_Interpretor& di, Standard_Integer argc, const char** argv ) {
  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  if ( V3dView.IsNull() ) {
    return 1;
  }

  if ( argc == 4 ) {
    V3dView->Rotate( atof(argv[1]), atof(argv[2]), atof(argv[3]) );
    return 0;
  } else if ( argc == 7 ) {
    V3dView->Rotate( atof(argv[1]), atof(argv[2]), atof(argv[3]), atof(argv[4]), atof(argv[5]), atof(argv[6]) );
    return 0;
  } else {
    di << argv[0] << " Invalid number of arguments" << "\n";
    return 1;
  }
}

//==============================================================================
//function : VZoom
//purpose  : View zoom in / out (relative to current zoom)
//==============================================================================

static int VZoom( Draw_Interpretor& di, Standard_Integer argc, const char** argv ) {
  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  if ( V3dView.IsNull() ) {
    return 1;
  }

  if ( argc == 2 ) {
    Standard_Real coef = atof(argv[1]);
    if ( coef <= 0.0 ) {
      di << argv[1] << "Invalid value" << "\n";
      return 1;
    }
    V3dView->SetZoom( atof(argv[1]) );
    return 0;
  } else {
    di << argv[0] << " Invalid number of arguments" << "\n";
    return 1;
  }
}

//==============================================================================
//function : VPan
//purpose  : View panning (in pixels)
//==============================================================================

static int VPan( Draw_Interpretor& di, Standard_Integer argc, const char** argv ) {
  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  if ( V3dView.IsNull() ) return 1;

  if ( argc == 3 ) {
    V3dView->Pan( atoi(argv[1]), atoi(argv[2]) );
    return 0;
  } else {
    di << argv[0] << " Invalid number of arguments" << "\n";
    return 1;
  }
}


//==============================================================================
//function : VExport
//purpose  : Export teh view to a vector graphic format (PS, EMF, PDF)
//==============================================================================

static int VExport(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  if ( V3dView.IsNull() ) return 1;

  if ( argc != 3 ) {
    di << argv[0] << "Invalid number of arguments" << "\n";
    return 1;
  }

  TCollection_AsciiString aFormat( argv[2] );
  aFormat.UpperCase();
  Graphic3d_ExportFormat exFormat;
  if ( aFormat == "PS" )
    exFormat = Graphic3d_EF_PostScript;
  if ( aFormat == "EPS" )
    exFormat = Graphic3d_EF_EnhPostScript;
  if ( aFormat == "TEX" )
    exFormat = Graphic3d_EF_TEX;
  if ( aFormat == "PDF" )
    exFormat = Graphic3d_EF_PDF;
  if ( aFormat == "SVG" )
    exFormat = Graphic3d_EF_SVG;
  if ( aFormat == "PGF" )
    exFormat = Graphic3d_EF_PGF;
  V3dView->View()->Export( argv[1], exFormat );
  return 0;
}

//==============================================================================
//function : VColorScale
//purpose  : representation color scale
//==============================================================================
#include <V3d_ColorScale.hxx>

static int VColorScale (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1 && argc != 4 && argc != 5 && argc != 6 && argc != 8 )
  {
    di << "Usage : " << argv[0] << " [RangeMin = 0 RangeMax = 100 Intervals = 10 HeightFont = 16 Position = Right X = 0 Y = 0]  " << "\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) {
    di << argv[0] << " ERROR : use 'vinit' command before " << "\n";
    return -1;
  }

  Standard_Real minRange = 0. , maxRange = 100. ;

  Standard_Integer numIntervals = 10 ;
  Standard_Real textHeight = 16. ;
  Aspect_TypeOfColorScalePosition position = Aspect_TOCSP_RIGHT;
  Standard_Real X = 0., Y = 0. ;

  if ( argc < 9 )
  {
     if( argc > 3 )
     {
       minRange = atof( argv[1] );
       maxRange = atof( argv[2] );
       numIntervals = atoi( argv[3] );
     }
     if ( argc > 4 )
       textHeight = atof( argv[4] );
     if ( argc > 5 )
       position = (Aspect_TypeOfColorScalePosition)atoi( argv[5] );
     if ( argc > 7 )
     {
       X = atof( argv[6] );
       Y = atof( argv[7] );
     }
  }
  Handle(V3d_View) curView = ViewerTest::CurrentView( );
  if ( curView.IsNull( ) )
    return 1;
  Handle(Aspect_ColorScale) aCSV = curView->ColorScale( );
  Handle(V3d_ColorScale) aCS = ( Handle( V3d_ColorScale )::DownCast( aCSV ) );
  if( ! aCS.IsNull( ) )
  {
    aCS->SetPosition( X , Y );
    aCS->SetHeight( 0.95) ;
    aCS->SetTextHeight( textHeight );
    aCS->SetRange( minRange , maxRange );
    aCS->SetNumberOfIntervals( numIntervals );
    aCS->SetLabelPosition( position );
    if( !curView->ColorScaleIsDisplayed() )
      curView->ColorScaleDisplay( );
  }
  return 0;
}

//==============================================================================
//function : VGraduatedTrihedron
//purpose  : Displays a graduated trihedron
//==============================================================================

static void AddMultibyteString (TCollection_ExtendedString &name, const char *arg)
{
  const char *str = arg;
  while (*str)
  {
    unsigned short c1 = *str++;
    unsigned short c2 = *str++;
    if (!c1 || !c2) break;
    name += (Standard_ExtCharacter)((c1 << 8) | c2);
  }
}

static int VGraduatedTrihedron(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  // Check arguments
  if (argc != 2 && argc < 5)
  {
    di<<"Error: "<<argv[0]<<" - invalid number of arguments\n";
    di<<"Usage: type help "<<argv[0]<<"\n";
    return 1; //TCL_ERROR
  }

  Handle(V3d_View) aV3dView = ViewerTest::CurrentView();

  // Create 3D view if it doesn't exist
  if ( aV3dView.IsNull() )
  {
    ViewerTest::ViewerInit(); 
    aV3dView = ViewerTest::CurrentView();
    if( aV3dView.IsNull() )
    {
      di << "Error: Cannot create a 3D view\n";
      return 1; //TCL_ERROR
    }
  }

  // Erase (==0) or display (!=0)
  const int display = atoi(argv[1]);

  if (display)
  {
    // Text font
    TCollection_AsciiString font;
    if (argc < 6)
      font.AssignCat("Courier");
    else
      font.AssignCat(argv[5]);

    // Text is multibyte
    const Standard_Boolean isMultibyte = (argc < 7)? Standard_False : (atoi(argv[6]) != 0);

    // Set axis names
    TCollection_ExtendedString xname, yname, zname;
    if (argc >= 5)
    {
      if (isMultibyte)
      {
        AddMultibyteString(xname, argv[2]);
        AddMultibyteString(yname, argv[3]);
        AddMultibyteString(zname, argv[4]);
      }
      else
      {
        xname += argv[2];
        yname += argv[3];
        zname += argv[4];
      }
    }
    else
    {
      xname += "X (mm)";
      yname += "Y (mm)";
      zname += "Z (mm)";
    }

    aV3dView->GraduatedTrihedronDisplay(xname, yname, zname,
                                        Standard_True/*xdrawname*/, Standard_True/*ydrawname*/, Standard_True/*zdrawname*/,
                                        Standard_True/*xdrawvalues*/, Standard_True/*ydrawvalues*/, Standard_True/*zdrawvalues*/,
                                        Standard_True/*drawgrid*/,
                                        Standard_True/*drawaxes*/,
                                        5/*nbx*/, 5/*nby*/, 5/*nbz*/,
                                        10/*xoffset*/, 10/*yoffset*/, 10/*zoffset*/,
                                        30/*xaxisoffset*/, 30/*yaxisoffset*/, 30/*zaxisoffset*/,
                                        Standard_True/*xdrawtickmarks*/, Standard_True/*ydrawtickmarks*/, Standard_True/*zdrawtickmarks*/,
                                        10/*xtickmarklength*/, 10/*ytickmarklength*/, 10/*ztickmarklength*/,
                                        Quantity_NOC_WHITE/*gridcolor*/,
                                        Quantity_NOC_RED/*xnamecolor*/,Quantity_NOC_GREEN/*ynamecolor*/,Quantity_NOC_BLUE1/*znamecolor*/,
                                        Quantity_NOC_RED/*xcolor*/,Quantity_NOC_GREEN/*ycolor*/,Quantity_NOC_BLUE1/*zcolor*/,font);
  }
  else
    aV3dView->GraduatedTrihedronErase();

  ViewerTest::GetAISContext()->UpdateCurrentViewer();
  aV3dView->Redraw();

  return 0;
}

//==============================================================================
//function : VPrintView
//purpose  : Test printing algorithm, print the view to image file with given
//           width and height. Printing implemented only for WNT.
//==============================================================================
static int VPrintView (Draw_Interpretor& di, Standard_Integer argc, 
                       const char** argv)
{
#ifndef WNT
  di << "Printing implemented only for wnt!\n";
  return 1;
#else

  Handle(AIS_InteractiveContext) aContextAIS = NULL;
  Handle(V3d_View) aView = NULL;
  aContextAIS = ViewerTest::GetAISContext();
  if (!aContextAIS.IsNull())
  {
    const Handle(V3d_Viewer)& Vwr = aContextAIS->CurrentViewer();
    Vwr->InitActiveViews();
    if(Vwr->MoreActiveViews())
      aView = Vwr->ActiveView();
  }

  // check for errors
  if (aView.IsNull())
  {
    di << "Call vinit before!\n";
    return 1;
  }
  else if (argc < 4)
  {
    di << "Use: " << argv[0];
    di << " width height filename [print algo=0]\n";
    di << "width, height of the intermediate buffer for operation\n";
    di << "algo : {0|1}\n";
    di << "        0 - stretch algorithm\n";
    di << "        1 - tile algorithm\n";
    di << "test printing algorithms into an intermediate buffer\n";
    di << "with saving output to an image file\n";
    return 1;
  }

  // get the input params
  Standard_Integer aWidth  = atoi (argv[1]);
  Standard_Integer aHeight = atoi (argv[2]);
  Standard_Integer aMode   = 0;
  TCollection_AsciiString aFileName = TCollection_AsciiString (argv[3]);
  if (argc==5)
    aMode = atoi (argv[4]);

  // check the input parameters
  if (aWidth <= 0 || aHeight <= 0)
  {
    di << "Width and height must be positive values!\n";
    return 1;
  }
  if (aMode != 0 && aMode != 1)
    aMode = 0;

  Image_CRawBufferData aRawBuffer;
  HDC anDC = CreateCompatibleDC(0);

  // define compatible bitmap
  BITMAPINFO aBitmapData;
  memset (&aBitmapData, 0, sizeof (BITMAPINFOHEADER));
  aBitmapData.bmiHeader.biSize          = sizeof (BITMAPINFOHEADER);
  aBitmapData.bmiHeader.biWidth         = aWidth ;
  aBitmapData.bmiHeader.biHeight        = aHeight;
  aBitmapData.bmiHeader.biPlanes        = 1;
  aBitmapData.bmiHeader.biBitCount      = 24;
  aBitmapData.bmiHeader.biXPelsPerMeter = 0;
  aBitmapData.bmiHeader.biYPelsPerMeter = 0;
  aBitmapData.bmiHeader.biClrUsed       = 0;
  aBitmapData.bmiHeader.biClrImportant  = 0;
  aBitmapData.bmiHeader.biCompression   = BI_RGB;
  aBitmapData.bmiHeader.biSizeImage     = 0;

  // Create Device Independent Bitmap
  HBITMAP aMemoryBitmap = CreateDIBSection (anDC, &aBitmapData, DIB_RGB_COLORS,
                                            &aRawBuffer.dataPtr, NULL, 0);
  HGDIOBJ anOldBitmap   = SelectObject(anDC, aMemoryBitmap);

  Standard_Boolean isSaved = Standard_False, isPrinted = Standard_False;
  if (aRawBuffer.dataPtr != 0)
  {    
    if (aMode == 0)
      isPrinted = aView->Print(anDC,1,1,0,Aspect_PA_STRETCH);
    else
      isPrinted = aView->Print(anDC,1,1,0,Aspect_PA_TILE);

    // succesfully printed into an intermediate buffer
    if (isPrinted)
    {
      Handle(Image_PixMap) anImageBitmap =
                         new Image_PixMap ((Standard_PByte)aRawBuffer.dataPtr,
                                           aWidth, aHeight,
                                           aWidth*3 + aWidth%4, 24, 0);
      isSaved = anImageBitmap->Dump(aFileName.ToCString());
    }
    else
    {
      di << "Print operation failed due to printing errors or\n";
      di << "insufficient memory available\n";
      di << "Please, try to use smaller dimensions for this test\n";
      di << "command, as it allocates intermediate buffer for storing\n";
      di << "the result\n";
    }
  }
  else
  {
    di << "Can't allocate memory for intermediate buffer\n";
    di << "Please use smaller dimensions\n";
  }

  if (aMemoryBitmap)
  {
    SelectObject (anDC, anOldBitmap);
    DeleteObject (aMemoryBitmap);
    DeleteDC(anDC);
  }

  if (!isSaved)
  {
    di << "Save to file operation failed. This operation may fail\n";
    di << "if you don't have enough available memory, then you can\n";
    di << "use smaller dimensions for the output file\n";
    return 1;
  }

  return 0;

#endif
}

//=======================================================================
//function : ViewerCommands
//purpose  :
//=======================================================================

void ViewerTest::ViewerCommands(Draw_Interpretor& theCommands)
{

  const char *group = "ZeViewer";
  theCommands.Add("vinit" ,
    "vinit            : vinit [leftPx topPx widthPx heightPx] : Create the Viewer window",
    __FILE__,VInit,group);
  theCommands.Add("vhelp" ,
    "vhelp            : display help on the viewer commands",
    __FILE__,VHelp,group);
  theCommands.Add("vtop" ,
    "vtop or <T>         : Top view" ,
    __FILE__,VTop,group);
  theCommands.Add("vaxo" ,
    " vaxo or <A>     : Axonometric view ",
    __FILE__,VAxo,group);
  theCommands.Add("vpick" ,
    "vpick           : vpick X Y Z [shape subshape] ( all variables as string )",
    VPick,group);
  theCommands.Add("vfit"    ,
    "vfit or <F>         : vfit",
    __FILE__,VFit,group);
  theCommands.Add("vzfit"    ,
    "vzfit",
    __FILE__,VZFit,group);
  theCommands.Add("vrepaint",
    "vrepaint        : vrepaint, force redraw",
    __FILE__,VRepaint,group);
  theCommands.Add("vclear",
    "vclear          : vclear",
    __FILE__,VClear,group);
  theCommands.Add("vsetbg",
    "vsetbg          : vsetbg imagefile [filltype] : Load image as background",
    __FILE__,VSetBg,group);
  theCommands.Add("vsetgradientbg",
    "vsetgradientbg          : vsetgradientbg r1 g1 b1 r2 g2 b2 filltype : Mount gradient background",
    __FILE__,VSetGradientBg,group);
  theCommands.Add("vscale",
    "vscale          : vscale X Y Z",
    __FILE__,VScale,group);
  theCommands.Add("vzbufftrihedron",
    "vzbufftrihedron : Displays a V3d_ZBUFFER'ed trihedron at the bottom left corner of the view",
    __FILE__,VTestZBuffTrihedron,group);
  theCommands.Add("vrotate",
    "vrotate         : vrotate AX AY AZ [X Y Z]",
    __FILE__,VRotate,group);
  theCommands.Add("vzoom",
    "vzoom           : vzoom coef",
    __FILE__,VZoom,group);
  theCommands.Add("vpan",
    "vpan            : vpan dx dy",
    __FILE__,VPan,group);
  theCommands.Add("vexport",
    "vexport         : vexport full_file_path {PS | EPS | TEX | PDF | SVG | PGV } : exports the view to a vector file of a given format",
    __FILE__,VExport,group);
  theCommands.Add("vcolorscale",
    "vcolorscale     : vcolorscale [RangeMin = 0 RangeMax = 100 Intervals = 10 HeightFont = 16 Position = 2 X = 0 Y = 0]: draw color scale",
    __FILE__,VColorScale,group);
  theCommands.Add("vgraduatedtrihedron",
    "vgraduatedtrihedron : 1/0 (display/erase) [Xname Yname Zname [Font [isMultibyte]]]",
    __FILE__,VGraduatedTrihedron,group);
  theCommands.Add("vprintview" ,
    "vprintview : width height filename [algo=0] : Test print algorithm: algo = 0 - stretch, algo = 1 - tile",
    __FILE__,VPrintView,group);

}
