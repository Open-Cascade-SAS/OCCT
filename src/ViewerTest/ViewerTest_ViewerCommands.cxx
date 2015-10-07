// Created on: 1998-09-01
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <OpenGl_GlCore20.hxx>
#include <AIS_ColorScale.hxx>
#include <AIS_Shape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <DBRep.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_ExportFormat.hxx>
#include <Graphic3d_NameOfTextureEnv.hxx>
#include <Graphic3d_GraduatedTrihedron.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_TypeOfTextureFilter.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <ViewerTest.hxx>
#include <ViewerTest_AutoUpdater.hxx>
#include <ViewerTest_EventManager.hxx>
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_SpotLight.hxx>
#include <NCollection_DoubleMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Vector.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Aspect_PrintAlgo.hxx>
#include <Image_AlienPixMap.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OSD_Timer.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_MapOfAsciiString.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Image_Diff.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <PrsMgr_PresentableObject.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <NCollection_DataMap.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Root.hxx>

#ifdef _WIN32
#undef DrawText
#endif

#include <cstdlib>

#if defined(_WIN32)
  #include <WNT_WClass.hxx>
  #include <WNT_Window.hxx>

  #if defined(_MSC_VER)
    #define _CRT_SECURE_NO_DEPRECATE
    #pragma warning (disable:4996)
  #endif
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  #include <Cocoa_Window.hxx>
#else
  #include <Xw_Window.hxx>
  #include <X11/Xlib.h> /* contains some dangerous #defines such as Status, True etc. */
  #include <X11/Xutil.h>
  #include <tk.h>
#endif

// Auxiliary definitions
static const char THE_KEY_DELETE = 127;

//==============================================================================
//  VIEWER GLOBAL VARIABLES
//==============================================================================

Standard_IMPORT Standard_Boolean Draw_VirtualWindows;
Standard_IMPORT Standard_Boolean Draw_Interprete (const char* theCommand);

Standard_EXPORT int ViewerMainLoop(Standard_Integer , const char** argv);
extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();

extern int VErase (Draw_Interpretor& theDI,
                   Standard_Integer  theArgNb,
                   const char**      theArgVec);

#if defined(_WIN32)
static Handle(WNT_Window)& VT_GetWindow() {
  static Handle(WNT_Window) WNTWin;
  return WNTWin;
}
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
static Handle(Cocoa_Window)& VT_GetWindow()
{
  static Handle(Cocoa_Window) aWindow;
  return aWindow;
}
extern void ViewerTest_SetCocoaEventManagerView (const Handle(Cocoa_Window)& theWindow);
extern void SetCocoaWindowTitle (const Handle(Cocoa_Window)& theWindow, Standard_CString theTitle);
extern void GetCocoaScreenResolution (Standard_Integer& theWidth, Standard_Integer& theHeight);

#else
static Handle(Xw_Window)& VT_GetWindow(){
  static Handle(Xw_Window) XWWin;
  return XWWin;
}

static void VProcessEvents(ClientData,int);
#endif

static Handle(Aspect_DisplayConnection)& GetDisplayConnection()
{
  static Handle(Aspect_DisplayConnection) aDisplayConnection;
  return aDisplayConnection;
}

static void SetDisplayConnection (const Handle(Aspect_DisplayConnection)& theDisplayConnection)
{
  GetDisplayConnection() = theDisplayConnection;
}

#if defined(_WIN32) || (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
Aspect_Handle GetWindowHandle(const Handle(Aspect_Window)& theWindow)
{
  Aspect_Handle aWindowHandle = (Aspect_Handle)NULL;
#if defined(_WIN32)
  const Handle (WNT_Window) aWindow = Handle(WNT_Window)::DownCast (theWindow);
  if (!aWindow.IsNull())
    return aWindow->HWindow();
#elif (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  const Handle (Xw_Window) aWindow = Handle(Xw_Window)::DownCast (theWindow);
  if (!aWindow.IsNull())
  return aWindow->XWindow();
#endif
  return aWindowHandle;
}
#endif

static Standard_Boolean MyHLRIsOn = Standard_False;

NCollection_DoubleMap <TCollection_AsciiString, Handle(V3d_View)> ViewerTest_myViews;
static NCollection_DoubleMap <TCollection_AsciiString, Handle(AIS_InteractiveContext)>  ViewerTest_myContexts;
static NCollection_DoubleMap <TCollection_AsciiString, Handle(Graphic3d_GraphicDriver)> ViewerTest_myDrivers;
static OpenGl_Caps ViewerTest_myDefaultCaps;

#define ZCLIPWIDTH 1.

static void OSWindowSetup();

static struct
{
  Quantity_Color FlatColor;
  Quantity_Color GradientColor1;
  Quantity_Color GradientColor2;
  Aspect_GradientFillMethod FillMethod;
} ViewerTest_DefaultBackground = { Quantity_NOC_BLACK, Quantity_NOC_BLACK, Quantity_NOC_BLACK, Aspect_GFM_NONE };

//==============================================================================
//  EVENT GLOBAL VARIABLES
//==============================================================================

static int Start_Rot = 0;
static int ZClipIsOn = 0;
int X_Motion = 0; // Current cursor position
int Y_Motion = 0;
int X_ButtonPress = 0; // Last ButtonPress position
int Y_ButtonPress = 0;
Standard_Boolean IsDragged = Standard_False;
Standard_Boolean DragFirst = Standard_False;

//==============================================================================

#ifdef _WIN32
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
#if defined(_WIN32)
  if (theWClass.IsNull())
  {
    theWClass = new WNT_WClass ("GW3D_Class", AdvViewerWindowProc,
      CS_VREDRAW | CS_HREDRAW, 0, 0,
      ::LoadCursor (NULL, IDC_ARROW));
  }
#endif
  return theWClass;
}

//==============================================================================
//function : CreateName
//purpose  : Create numerical name for new object in theMap
//==============================================================================
template <typename ObjectType>
TCollection_AsciiString CreateName (const NCollection_DoubleMap <TCollection_AsciiString, ObjectType>& theObjectMap,
                                    const TCollection_AsciiString& theDefaultString)
{
  if (theObjectMap.IsEmpty())
    return theDefaultString + TCollection_AsciiString(1);

  Standard_Integer aNextKey = 1;
  Standard_Boolean isFound = Standard_False;
  while (!isFound)
  {
    TCollection_AsciiString aStringKey = theDefaultString + TCollection_AsciiString(aNextKey);
    // Look for objects with default names
    if (theObjectMap.IsBound1(aStringKey))
    {
      aNextKey++;
    }
    else
      isFound = Standard_True;
  }

  return theDefaultString + TCollection_AsciiString(aNextKey);
}

//==============================================================================
//structure : ViewerTest_Names
//purpose   : Allow to operate with full view name: driverName/viewerName/viewName
//==============================================================================
struct ViewerTest_Names
{
private:
  TCollection_AsciiString myDriverName;
  TCollection_AsciiString myViewerName;
  TCollection_AsciiString myViewName;

public:

  const TCollection_AsciiString& GetDriverName () const
  {
    return myDriverName;
  }
  void SetDriverName (const TCollection_AsciiString& theDriverName)
  {
    myDriverName = theDriverName;
  }
  const TCollection_AsciiString& GetViewerName () const
  {
    return myViewerName;
  }
  void SetViewerName (const TCollection_AsciiString& theViewerName)
  {
    myViewerName = theViewerName;
  }
  const TCollection_AsciiString& GetViewName () const
  {
    return myViewName;
  }
  void SetViewName (const TCollection_AsciiString& theViewName)
  {
    myViewName = theViewName;
  }

  //===========================================================================
  //function : Constructor for ViewerTest_Names
  //purpose  : Get view, viewer, driver names from custom string
  //===========================================================================

  ViewerTest_Names (const TCollection_AsciiString& theInputString)
  {
    TCollection_AsciiString aName(theInputString);
    if (theInputString.IsEmpty())
    {
      // Get current configuration
      if (ViewerTest_myDrivers.IsEmpty())
        myDriverName = CreateName<Handle(Graphic3d_GraphicDriver)>
          (ViewerTest_myDrivers, TCollection_AsciiString("Driver"));
      else
        myDriverName = ViewerTest_myDrivers.Find2
        (ViewerTest::GetAISContext()->CurrentViewer()->Driver());

      if(ViewerTest_myContexts.IsEmpty())
      {
        myViewerName = CreateName <Handle(AIS_InteractiveContext)>
          (ViewerTest_myContexts, TCollection_AsciiString (myDriverName + "/Viewer"));
      }
      else
        myViewerName = ViewerTest_myContexts.Find2 (ViewerTest::GetAISContext());

        myViewName = CreateName <Handle(V3d_View)>
          (ViewerTest_myViews, TCollection_AsciiString(myViewerName + "/View"));
    }
    else
    {
      // There is at least view name
      Standard_Integer aParserNumber = 0;
      for (Standard_Integer i = 0; i < 3; ++i)
      {
        Standard_Integer aParserPos = aName.SearchFromEnd("/");
        if(aParserPos != -1)
        {
          aParserNumber++;
          aName.Split(aParserPos-1);
        }
        else
          break;
      }
      if (aParserNumber == 0)
      {
        // Only view name
        if (!ViewerTest::GetAISContext().IsNull())
        {
          myDriverName = ViewerTest_myDrivers.Find2
          (ViewerTest::GetAISContext()->CurrentViewer()->Driver());
          myViewerName = ViewerTest_myContexts.Find2
          (ViewerTest::GetAISContext());
        }
        else
        {
          // There is no opened contexts here, need to create names for viewer and driver
          myDriverName = CreateName<Handle(Graphic3d_GraphicDriver)>
            (ViewerTest_myDrivers, TCollection_AsciiString("Driver"));

          myViewerName = CreateName <Handle(AIS_InteractiveContext)>
            (ViewerTest_myContexts, TCollection_AsciiString (myDriverName + "/Viewer"));
        }
        myViewName = TCollection_AsciiString(myViewerName + "/" + theInputString);
      }
      else if (aParserNumber == 1)
      {
        // Here is viewerName/viewName
        if (!ViewerTest::GetAISContext().IsNull())
          myDriverName = ViewerTest_myDrivers.Find2
          (ViewerTest::GetAISContext()->CurrentViewer()->Driver());
        else
        {
          // There is no opened contexts here, need to create name for driver
          myDriverName = CreateName<Handle(Graphic3d_GraphicDriver)>
            (ViewerTest_myDrivers, TCollection_AsciiString("Driver"));
        }
        myViewerName = TCollection_AsciiString(myDriverName + "/" + aName);

        myViewName = TCollection_AsciiString(myDriverName + "/" + theInputString);
      }
      else
      {
        //Here is driverName/viewerName/viewName
        myDriverName = TCollection_AsciiString(aName);

        TCollection_AsciiString aViewerName(theInputString);
        aViewerName.Split(aViewerName.SearchFromEnd("/") - 1);
        myViewerName = TCollection_AsciiString(aViewerName);

        myViewName = TCollection_AsciiString(theInputString);
      }
    }
  }
};

//==============================================================================
//function : FindContextByView
//purpose  : Find AIS_InteractiveContext by View
//==============================================================================

Handle(AIS_InteractiveContext) FindContextByView (const Handle(V3d_View)& theView)
{
  Handle(AIS_InteractiveContext) anAISContext;

  for (NCollection_DoubleMap<TCollection_AsciiString, Handle(AIS_InteractiveContext)>::Iterator
       anIter (ViewerTest_myContexts); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->CurrentViewer() == theView->Viewer())
       return anIter.Key2();
  }
  return anAISContext;
}


//==============================================================================
//function : SetWindowTitle
//purpose  : Set window title
//==============================================================================

void SetWindowTitle (const Handle(Aspect_Window)& theWindow,
                     Standard_CString theTitle)
{
#if defined(_WIN32)
  SetWindowText ((HWND)Handle(WNT_Window)::DownCast(theWindow)->HWindow(),
    theTitle);
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  SetCocoaWindowTitle (Handle(Cocoa_Window)::DownCast(theWindow), theTitle);
#else
  if(GetDisplayConnection()->GetDisplay())
  {
    Window aWindow =
      Handle(Xw_Window)::DownCast(theWindow)->XWindow();
    XStoreName (GetDisplayConnection()->GetDisplay(), aWindow , theTitle);
  }
#endif
}

//==============================================================================
//function : IsWindowOverlapped
//purpose  : Check if theWindow overlapp another view
//==============================================================================

Standard_Boolean IsWindowOverlapped (const Standard_Integer thePxLeft,
                                     const Standard_Integer thePxTop,
                                     const Standard_Integer thePxRight,
                                     const Standard_Integer thePxBottom,
                                     TCollection_AsciiString& theViewId)
{
  for(NCollection_DoubleMap <TCollection_AsciiString, Handle(V3d_View)>::Iterator
      anIter(ViewerTest_myViews); anIter.More(); anIter.Next())
  {
    Standard_Integer aTop = 0,
      aLeft = 0,
      aRight = 0,
      aBottom = 0;
    anIter.Value()->Window()->Position(aLeft, aTop, aRight, aBottom);
    if ((thePxLeft >= aLeft && thePxLeft <= aRight && thePxTop >= aTop && thePxTop <= aBottom) ||
        (thePxLeft >= aLeft && thePxLeft <= aRight && thePxBottom >= aTop && thePxBottom <= aBottom) ||
        (thePxRight >= aLeft && thePxRight <= aRight && thePxTop >= aTop && thePxTop <= aBottom) ||
        (thePxRight >= aLeft && thePxRight <= aRight && thePxBottom >= aTop && thePxBottom <= aBottom))
    {
      theViewId = anIter.Key1();
      return Standard_True;
    }
  }
  return Standard_False;
}

// Workaround: to create and delete non-orthographic views outside ViewerTest
void ViewerTest::RemoveViewName (const TCollection_AsciiString& theName)
{
  ViewerTest_myViews.UnBind1 (theName);
}

void ViewerTest::InitViewName (const TCollection_AsciiString& theName,
                               const Handle(V3d_View)& theView)
{
  ViewerTest_myViews.Bind (theName, theView);
}

TCollection_AsciiString ViewerTest::GetCurrentViewName ()
{
  return ViewerTest_myViews.Find2( ViewerTest::CurrentView());
}
//==============================================================================
//function : ViewerInit
//purpose  : Create the window viewer and initialize all the global variable
//==============================================================================

TCollection_AsciiString ViewerTest::ViewerInit (const Standard_Integer thePxLeft,
                                                const Standard_Integer thePxTop,
                                                const Standard_Integer thePxWidth,
                                                const Standard_Integer thePxHeight,
                                                Standard_CString theViewName,
                                                Standard_CString theDisplayName)
{
  // Default position and dimension of the viewer window.
  // Note that left top corner is set to be sufficiently small to have
  // window fit in the small screens (actual for remote desktops, see #23003).
  // The position corresponds to the window's client area, thus some
  // gap is added for window frame to be visible.
  Standard_Integer aPxLeft   = 20;
  Standard_Integer aPxTop    = 40;
  Standard_Integer aPxWidth  = 409;
  Standard_Integer aPxHeight = 409;
  Standard_Boolean toCreateViewer = Standard_False;

  Handle(OpenGl_GraphicDriver) aGraphicDriver;
  ViewerTest_Names aViewNames(theViewName);
  if (ViewerTest_myViews.IsBound1 (aViewNames.GetViewName ()))
    aViewNames.SetViewName (aViewNames.GetViewerName() + "/" + CreateName<Handle(V3d_View)>(ViewerTest_myViews, "View"));

  if (thePxLeft != 0)
    aPxLeft = thePxLeft;
  if (thePxTop != 0)
    aPxTop = thePxTop;
  if (thePxWidth != 0)
    aPxWidth = thePxWidth;
  if (thePxHeight != 0)
    aPxHeight = thePxHeight;

  // Get graphic driver (create it or get from another view)
  if (!ViewerTest_myDrivers.IsBound1 (aViewNames.GetDriverName()))
  {
    // Get connection string
  #if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
    TCollection_AsciiString aDisplayName(theDisplayName);
    if (!aDisplayName.IsEmpty())
      SetDisplayConnection (new Aspect_DisplayConnection ());
    else
      SetDisplayConnection (new Aspect_DisplayConnection (aDisplayName));
  #else
    (void)theDisplayName; // avoid warning on unused argument
    SetDisplayConnection (new Aspect_DisplayConnection ());
  #endif
    aGraphicDriver = new OpenGl_GraphicDriver (GetDisplayConnection());
    aGraphicDriver->ChangeOptions() = ViewerTest_myDefaultCaps;
    ViewerTest_myDrivers.Bind (aViewNames.GetDriverName(), aGraphicDriver);
    toCreateViewer = Standard_True;
  }
  else
  {
    aGraphicDriver = Handle(OpenGl_GraphicDriver)::DownCast (ViewerTest_myDrivers.Find1 (aViewNames.GetDriverName()));
  }

  //Dispose the window if input parameters are default
  if (!ViewerTest_myViews.IsEmpty() && thePxLeft == 0 && thePxTop == 0)
  {
    Standard_Integer aTop = 0,
                     aLeft = 0,
                     aRight = 0,
                     aBottom = 0,
                     aScreenWidth = 0,
                     aScreenHeight = 0;

    // Get screen resolution
#if defined(_WIN32) || defined(__WIN32__)
    RECT aWindowSize;
    GetClientRect(GetDesktopWindow(), &aWindowSize);
    aScreenHeight = aWindowSize.bottom;
    aScreenWidth = aWindowSize.right;
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
    GetCocoaScreenResolution (aScreenWidth, aScreenHeight);
#else
    Screen *aScreen = DefaultScreenOfDisplay(GetDisplayConnection()->GetDisplay());
    aScreenWidth = WidthOfScreen(aScreen);
    aScreenHeight = HeightOfScreen(aScreen);
#endif

    TCollection_AsciiString anOverlappedViewId("");

    while (IsWindowOverlapped (aPxLeft, aPxTop, aPxLeft + aPxWidth, aPxTop + aPxHeight, anOverlappedViewId))
    {
      ViewerTest_myViews.Find1(anOverlappedViewId)->Window()->Position (aLeft, aTop, aRight, aBottom);

      if (IsWindowOverlapped (aRight + 20, aPxTop, aRight + 20 + aPxWidth, aPxTop + aPxHeight, anOverlappedViewId)
        && aRight + 2*aPxWidth + 40 > aScreenWidth)
      {
        if (aBottom + aPxHeight + 40 > aScreenHeight)
        {
          aPxLeft = 20;
          aPxTop = 40;
          break;
        }
        aPxLeft = 20;
        aPxTop = aBottom + 40;
      }
      else
        aPxLeft = aRight + 20;
    }
  }

  // Get viewer name
  TCollection_AsciiString aTitle("3D View - ");
  aTitle = aTitle + aViewNames.GetViewName() + "(*)";

  // Change name of current active window
  if (!ViewerTest::CurrentView().IsNull())
  {
    TCollection_AsciiString anActiveWindowTitle("3D View - ");
    anActiveWindowTitle = anActiveWindowTitle
      + ViewerTest_myViews.Find2 (ViewerTest::CurrentView());
    SetWindowTitle (ViewerTest::CurrentView()->Window(), anActiveWindowTitle.ToCString());
  }

  // Create viewer
  Handle(V3d_Viewer) a3DViewer;
  // If it's the single view, we first look for empty context
  if (ViewerTest_myViews.IsEmpty() && !ViewerTest_myContexts.IsEmpty())
  {
    NCollection_DoubleMap <TCollection_AsciiString, Handle(AIS_InteractiveContext)>::Iterator
      anIter(ViewerTest_myContexts);
    if (anIter.More())
      ViewerTest::SetAISContext (anIter.Value());
    a3DViewer = ViewerTest::GetAISContext()->CurrentViewer();
  }
  else if (ViewerTest_myContexts.IsBound1(aViewNames.GetViewerName()))
  {
    ViewerTest::SetAISContext(ViewerTest_myContexts.Find1(aViewNames.GetViewerName()));
    a3DViewer = ViewerTest::GetAISContext()->CurrentViewer();
  }
  else if (a3DViewer.IsNull())
  {
    toCreateViewer = Standard_True;
    TCollection_ExtendedString NameOfWindow("Viewer3D");
    a3DViewer = new V3d_Viewer(aGraphicDriver, NameOfWindow.ToExtString());
    a3DViewer->SetDefaultBackgroundColor (ViewerTest_DefaultBackground.FlatColor);
    a3DViewer->SetDefaultBgGradientColors (ViewerTest_DefaultBackground.GradientColor1,
                                           ViewerTest_DefaultBackground.GradientColor2,
                                           ViewerTest_DefaultBackground.FillMethod);
  }

  // AIS context setup
  if (ViewerTest::GetAISContext().IsNull() ||
      !(ViewerTest_myContexts.IsBound1(aViewNames.GetViewerName())))
  {
    Handle(AIS_InteractiveContext) aContext = new AIS_InteractiveContext (a3DViewer);
    ViewerTest::SetAISContext (aContext);
    ViewerTest_myContexts.Bind (aViewNames.GetViewerName(), ViewerTest::GetAISContext());
  }
  else
  {
    ViewerTest::ResetEventManager();
  }

  // Create window
#if defined(_WIN32)
  VT_GetWindow() = new WNT_Window (aTitle.ToCString(),
                                    Handle(WNT_WClass)::DownCast (WClass()),
                                    Draw_VirtualWindows ? WS_POPUP : WS_OVERLAPPEDWINDOW,
                                    aPxLeft, aPxTop,
                                    aPxWidth, aPxHeight,
                                    Quantity_NOC_BLACK);
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  VT_GetWindow() = new Cocoa_Window (aTitle.ToCString(),
                                     aPxLeft, aPxTop,
                                     aPxWidth, aPxHeight);
  ViewerTest_SetCocoaEventManagerView (VT_GetWindow());
#else
  VT_GetWindow() = new Xw_Window (aGraphicDriver->GetDisplayConnection(),
                                  aTitle.ToCString(),
                                  aPxLeft, aPxTop,
                                  aPxWidth, aPxHeight);
#endif
  VT_GetWindow()->SetVirtual (Draw_VirtualWindows);

  // View setup
  Handle(V3d_View) aView = a3DViewer->CreateView();
  aView->SetWindow (VT_GetWindow());
  ViewerTest::GetAISContext()->RedrawImmediate (a3DViewer);

  ViewerTest::CurrentView(aView);
  ViewerTest_myViews.Bind (aViewNames.GetViewName(), aView);

  // Setup for X11 or NT
  OSWindowSetup();

  // Set parameters for V3d_View and V3d_Viewer
  const Handle (V3d_View) aV3dView = ViewerTest::CurrentView();
  aV3dView->SetComputedMode(Standard_False);
  MyHLRIsOn = aV3dView->ComputedMode();
  aV3dView->SetZClippingDepth(0.5);
  aV3dView->SetZClippingWidth(ZCLIPWIDTH/2.);

  a3DViewer->SetDefaultBackgroundColor(Quantity_NOC_BLACK);
  if (toCreateViewer)
  {
    a3DViewer->SetDefaultLights();
    a3DViewer->SetLightOn();
  }

  #if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  #if TCL_MAJOR_VERSION  < 8
  Tk_CreateFileHandler((void*)XConnectionNumber(GetDisplayConnection()->GetDisplay()),
      TK_READABLE, VProcessEvents, (ClientData) VT_GetWindow()->XWindow() );
  #else
  Tk_CreateFileHandler(XConnectionNumber(GetDisplayConnection()->GetDisplay()),
      TK_READABLE, VProcessEvents, (ClientData) VT_GetWindow()->XWindow() );
  #endif
  #endif

  VT_GetWindow()->Map();

  // Set the handle of created view in the event manager
  ViewerTest::ResetEventManager();

  ViewerTest::CurrentView()->Redraw();

  aView.Nullify();
  a3DViewer.Nullify();

  return aViewNames.GetViewName();
}

//==============================================================================
//function : RedrawAllViews
//purpose  : Redraw all created views
//==============================================================================
void ViewerTest::RedrawAllViews()
{
  NCollection_DoubleMap<TCollection_AsciiString, Handle(V3d_View)>::Iterator aViewIt(ViewerTest_myViews);
  for (; aViewIt.More(); aViewIt.Next())
  {
    const Handle(V3d_View)& aView = aViewIt.Key2();
    aView->Redraw();
  }
}

//==============================================================================
//function : Vinit
//purpose  : Create the window viewer and initialize all the global variable
//    Use Tk_CreateFileHandler on UNIX to catch the X11 Viewer event
//==============================================================================

static int VInit (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  if (theArgsNb > 9)
  {
    std::cerr << theArgVec[0] << ": incorrect number of command arguments.\n"
              << "Type help for more information.\n";
    return 1;
  }

  TCollection_AsciiString aViewName, aDisplayName;
  Standard_Integer aPxLeft = 0, aPxTop = 0, aPxWidth = 0, aPxHeight = 0;
  TCollection_AsciiString aName, aValue;
  for (Standard_Integer anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    const TCollection_AsciiString anArg = theArgVec[anArgIt];
    TCollection_AsciiString anArgCase = anArg;
    anArgCase.UpperCase();
    if (ViewerTest::SplitParameter (anArg, aName, aValue))
    {
      aName.UpperCase();
      if (aName.IsEqual ("NAME"))
      {
        aViewName = aValue;
      }
      else if (aName.IsEqual ("L")
            || aName.IsEqual ("LEFT"))
      {
        aPxLeft = aValue.IntegerValue();
      }
      else if (aName.IsEqual ("T")
            || aName.IsEqual ("TOP"))
      {
        aPxTop = aValue.IntegerValue();
      }
    #if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
      else if (aName.IsEqual ("DISP")
            || aName.IsEqual ("DISPLAY"))
      {
        aDisplayName = aValue;
      }
    #endif
      else if (aName.IsEqual ("W")
            || aName.IsEqual ("WIDTH"))
      {
        aPxWidth = aValue.IntegerValue();
      }
      else if (aName.IsEqual ("H")
            || aName.IsEqual ("HEIGHT"))
      {
        aPxHeight = aValue.IntegerValue();
      }
      else
      {
        std::cerr << theArgVec[0] << ": Warning: unknown argument " << anArg << ".\n";
      }
    }
    else if (aViewName.IsEmpty())
    {
      aViewName = anArg;
    }
    else
    {
      std::cerr << theArgVec[0] << ": Warning: unknown argument " << anArg << ".\n";
    }
  }

  ViewerTest_Names aViewNames (aViewName);
  if (ViewerTest_myViews.IsBound1 (aViewNames.GetViewName()))
  {
    TCollection_AsciiString aCommand = TCollection_AsciiString ("vactivate ") + aViewNames.GetViewName();
    theDi.Eval (aCommand.ToCString());
    return 0;
  }

  TCollection_AsciiString aViewId = ViewerTest::ViewerInit (aPxLeft, aPxTop, aPxWidth, aPxHeight,
                                                            aViewName.ToCString(),
                                                            aDisplayName.ToCString());
  theDi << aViewId;
  return 0;
}

//==============================================================================
//function : VHLR
//purpose  : hidden lines removal algorithm
//draw args: vhlr is_enabled={on|off} [show_hidden={1|0}]
//==============================================================================

static int VHLR (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (ViewerTest::CurrentView().IsNull())
  {
    di << argv[0] << ": Call vinit before this command, please.\n";
    return 1;
  }

  if (argc < 2)
  {
    di << argv[0] << ": Wrong number of command arguments.\n"
      << "Type help " << argv[0] << " for more information.\n";
    return 1;
  }

  // Enable or disable HLR mode.
  Standard_Boolean isHLROn =
    (!strcasecmp (argv[1], "on")) ? Standard_True : Standard_False;

  if (isHLROn != MyHLRIsOn)
  {
    MyHLRIsOn = isHLROn;
    ViewerTest::CurrentView()->SetComputedMode (MyHLRIsOn);
  }

  // Show or hide hidden lines in HLR mode.
  Standard_Boolean isCurrentShowHidden
    = ViewerTest::GetAISContext()->DefaultDrawer()->DrawHiddenLine();

  Standard_Boolean isShowHidden =
    (argc == 3) ? (atoi(argv[2]) == 1 ? Standard_True : Standard_False)
                : isCurrentShowHidden;


  if (isShowHidden != isCurrentShowHidden)
  {
    if (isShowHidden)
    {
      ViewerTest::GetAISContext()->DefaultDrawer()->EnableDrawHiddenLine();
    }
    else
    {
      ViewerTest::GetAISContext()->DefaultDrawer()->DisableDrawHiddenLine();
    }

    // Redisplay shapes.
    if (MyHLRIsOn)
    {
      AIS_ListOfInteractive aListOfShapes;
      ViewerTest::GetAISContext()->DisplayedObjects (aListOfShapes);

      for (AIS_ListIteratorOfListOfInteractive anIter(aListOfShapes); anIter.More(); anIter.Next())
      {
        Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast (anIter.Value());
        if (aShape.IsNull())
        {
          continue;
        }
        ViewerTest::GetAISContext()->Redisplay (aShape, Standard_False);
      }
    }
  }

  ViewerTest::CurrentView()->Update();
  return 0;
}

//==============================================================================
//function : VHLRType
//purpose  : change type of using HLR algorithm
//==============================================================================

static int VHLRType (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (ViewerTest::CurrentView().IsNull())
  {
    di << argv[0] << ": Call vinit before this command, please.\n";
    return 1;
  }

  if (argc < 2)
  {
    di << argv[0] << ": Wrong number of command arguments.\n"
      << "Type help " << argv[0] << " for more information.\n";
    return 1;
  }

  Prs3d_TypeOfHLR aTypeOfHLR =
    (!strcasecmp (argv[1], "algo")) ? Prs3d_TOH_Algo : Prs3d_TOH_PolyAlgo;

  if (argc == 2)
  {
    AIS_ListOfInteractive aListOfShapes;
    ViewerTest::GetAISContext()->DisplayedObjects (aListOfShapes);
    ViewerTest::GetAISContext()->DefaultDrawer()->SetTypeOfHLR(aTypeOfHLR);
    for (AIS_ListIteratorOfListOfInteractive anIter(aListOfShapes);
      anIter.More(); anIter.Next())
    {
      Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(anIter.Value());
      if (aShape.IsNull())
        continue;
      if (aShape->TypeOfHLR() != aTypeOfHLR)
        aShape->SetTypeOfHLR (aTypeOfHLR);
      if (MyHLRIsOn)
        ViewerTest::GetAISContext()->Redisplay (aShape, Standard_False);
    }
    ViewerTest::CurrentView()->Update();
    return 0;
  }
  else
  {
    for (Standard_Integer i = 2; i < argc; ++i)
    {
      ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
      TCollection_AsciiString aName (argv[i]);

      if (!aMap.IsBound2 (aName))
      {
        di << argv[0] << ":" << " Wrong shape name:" << aName.ToCString() << ".\n";
        continue;
      }
      Handle(AIS_Shape) anAISObject =
        Handle(AIS_Shape)::DownCast (aMap.Find2(aName));
      if (anAISObject.IsNull())
        continue;
      anAISObject->SetTypeOfHLR (aTypeOfHLR);
      if (MyHLRIsOn)
        ViewerTest::GetAISContext()->Redisplay (anAISObject, Standard_False);
    }
    ViewerTest::CurrentView()->Update();
  }

  return 0;
}

//==============================================================================
//function : FindViewIdByWindowHandle
//purpose  : Find theView Id in the map of views by window handle
//==============================================================================
#if defined(_WIN32) || defined(__WIN32__) || (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
TCollection_AsciiString FindViewIdByWindowHandle(const Aspect_Handle theWindowHandle)
{
  for (NCollection_DoubleMap<TCollection_AsciiString, Handle(V3d_View)>::Iterator
       anIter(ViewerTest_myViews); anIter.More(); anIter.Next())
  {
    Aspect_Handle aWindowHandle = GetWindowHandle(anIter.Value()->Window());
    if (aWindowHandle == theWindowHandle)
      return anIter.Key1();
  }
  return TCollection_AsciiString("");
}
#endif

//==============================================================================
//function : ActivateView
//purpose  : Make the view active
//==============================================================================

void ActivateView (const TCollection_AsciiString& theViewName)
{
  const Handle(V3d_View) aView = ViewerTest_myViews.Find1(theViewName);
  if (!aView.IsNull())
  {
    Handle(AIS_InteractiveContext) anAISContext = FindContextByView(aView);
    if (!anAISContext.IsNull())
    {
      if (!ViewerTest::CurrentView().IsNull())
      {
        TCollection_AsciiString aTitle("3D View - ");
        aTitle = aTitle + ViewerTest_myViews.Find2 (ViewerTest::CurrentView());
        SetWindowTitle (ViewerTest::CurrentView()->Window(), aTitle.ToCString());
      }

      ViewerTest::CurrentView (aView);
      // Update degenerate mode
      MyHLRIsOn = ViewerTest::CurrentView()->ComputedMode();
      ViewerTest::SetAISContext (anAISContext);
      TCollection_AsciiString aTitle = TCollection_AsciiString("3D View - ");
      aTitle = aTitle + theViewName + "(*)";
      SetWindowTitle (ViewerTest::CurrentView()->Window(), aTitle.ToCString());
#if defined(_WIN32) || defined(__WIN32__)
      VT_GetWindow() = Handle(WNT_Window)::DownCast(ViewerTest::CurrentView()->Window());
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
      VT_GetWindow() = Handle(Cocoa_Window)::DownCast(ViewerTest::CurrentView()->Window());
#else
      VT_GetWindow() = Handle(Xw_Window)::DownCast(ViewerTest::CurrentView()->Window());
#endif
      SetDisplayConnection(ViewerTest::CurrentView()->Viewer()->Driver()->GetDisplayConnection());
      ViewerTest::CurrentView()->Redraw();
    }
  }
}

//==============================================================================
//function : RemoveView
//purpose  :
//==============================================================================
void ViewerTest::RemoveView (const Handle(V3d_View)& theView,
                             const Standard_Boolean  theToRemoveContext)
{
  if (!ViewerTest_myViews.IsBound2 (theView))
  {
    return;
  }

  const TCollection_AsciiString aViewName = ViewerTest_myViews.Find2 (theView);
  RemoveView (aViewName, theToRemoveContext);
}

//==============================================================================
//function : RemoveView
//purpose  : Close and remove view from display, clear maps if neccessary
//==============================================================================
void ViewerTest::RemoveView (const TCollection_AsciiString& theViewName, const Standard_Boolean isContextRemoved)
{
  if (!ViewerTest_myViews.IsBound1(theViewName))
  {
    cout << "Wrong view name\n";
    return;
  }

  // Activate another view if it's active now
  if (ViewerTest_myViews.Find1(theViewName) == ViewerTest::CurrentView())
  {
    if (ViewerTest_myViews.Extent() > 1)
    {
      TCollection_AsciiString aNewViewName;
      for (NCollection_DoubleMap <TCollection_AsciiString, Handle(V3d_View)> :: Iterator
           anIter(ViewerTest_myViews); anIter.More(); anIter.Next())
        if (anIter.Key1() != theViewName)
        {
          aNewViewName = anIter.Key1();
          break;
        }
        ActivateView (aNewViewName);
    }
    else
    {
      Handle(V3d_View) anEmptyView;
#if defined(_WIN32) || defined(__WIN32__)
      Handle(WNT_Window) anEmptyWindow;
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
      Handle(Cocoa_Window) anEmptyWindow;
#else
      Handle(Xw_Window) anEmptyWindow;
#endif
      VT_GetWindow() = anEmptyWindow;
      ViewerTest::CurrentView (anEmptyView);
      if (isContextRemoved)
      {
        Handle(AIS_InteractiveContext) anEmptyContext;
        ViewerTest::SetAISContext(anEmptyContext);
      }
    }
  }

  // Delete view
  Handle(V3d_View) aView = ViewerTest_myViews.Find1(theViewName);
  Handle(AIS_InteractiveContext) aCurrentContext = FindContextByView(aView);

  // Remove view resources
  ViewerTest_myViews.UnBind1(theViewName);
  aView->Remove();

#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  XFlush (GetDisplayConnection()->GetDisplay());
#endif

  // Keep context opened only if the closed view is last to avoid
  // unused empty contexts
  if (!aCurrentContext.IsNull())
  {
    // Check if there are more difined views in the viewer
    aCurrentContext->CurrentViewer()->InitDefinedViews();
    if ((isContextRemoved || ViewerTest_myContexts.Size() != 1) && !aCurrentContext->CurrentViewer()->MoreDefinedViews())
    {
      // Remove driver if there is no viewers that use it
      Standard_Boolean isRemoveDriver = Standard_True;
      for(NCollection_DoubleMap<TCollection_AsciiString, Handle(AIS_InteractiveContext)>::Iterator
          anIter(ViewerTest_myContexts); anIter.More(); anIter.Next())
      {
        if (aCurrentContext != anIter.Key2() &&
          aCurrentContext->CurrentViewer()->Driver() == anIter.Value()->CurrentViewer()->Driver())
        {
          isRemoveDriver = Standard_False;
          break;
        }
      }
      if(isRemoveDriver)
      {
        ViewerTest_myDrivers.UnBind2 (aCurrentContext->CurrentViewer()->Driver());
      #if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
        #if TCL_MAJOR_VERSION  < 8
        Tk_DeleteFileHandler((void*)XConnectionNumber(aCurrentContext->CurrentViewer()->Driver()->GetDisplayConnection()->GetDisplay()));
        #else
        Tk_DeleteFileHandler(XConnectionNumber(aCurrentContext->CurrentViewer()->Driver()->GetDisplayConnection()->GetDisplay()));
        #endif
      #endif
      }

      ViewerTest_myContexts.UnBind2(aCurrentContext);
    }
  }
  cout << "3D View - " << theViewName << " was deleted.\n";

}

//==============================================================================
//function : VClose
//purpose  : Remove the view defined by its name
//==============================================================================

static int VClose (Draw_Interpretor& /*theDi*/,
                   Standard_Integer  theArgsNb,
                   const char**      theArgVec)
{
  NCollection_List<TCollection_AsciiString> aViewList;
  if (theArgsNb > 1)
  {
    TCollection_AsciiString anArg (theArgVec[1]);
    anArg.UpperCase();
    if (anArg.IsEqual ("ALL")
     || anArg.IsEqual ("*"))
    {
      for (NCollection_DoubleMap<TCollection_AsciiString, Handle(V3d_View)>::Iterator anIter (ViewerTest_myViews);
           anIter.More(); anIter.Next())
      {
        aViewList.Append (anIter.Key1());
      }
      if (aViewList.IsEmpty())
      {
        std::cout << "No view to close\n";
        return 0;
      }
    }
    else
    {
      ViewerTest_Names aViewName (theArgVec[1]);
      if (!ViewerTest_myViews.IsBound1 (aViewName.GetViewName()))
      {
        std::cerr << "The view with name '" << theArgVec[1] << "' does not exist\n";
        return 1;
      }
      aViewList.Append (aViewName.GetViewName());
    }
  }
  else
  {
    // close active view
    if (ViewerTest::CurrentView().IsNull())
    {
      std::cerr << "No active view!\n";
      return 1;
    }
    aViewList.Append (ViewerTest_myViews.Find2 (ViewerTest::CurrentView()));
  }

  Standard_Boolean toRemoveContext = (theArgsNb != 3 || Draw::Atoi (theArgVec[2]) != 1);
  for (NCollection_List<TCollection_AsciiString>::Iterator anIter(aViewList);
       anIter.More(); anIter.Next())
  {
    ViewerTest::RemoveView (anIter.Value(), toRemoveContext);
  }

  return 0;
}

//==============================================================================
//function : VActivate
//purpose  : Activate the view defined by its ID
//==============================================================================

static int VActivate (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  if (theArgsNb > 2)
  {
    theDi << theArgVec[0] << ": wrong number of command arguments.\n"
    << "Usage: " << theArgVec[0] << " ViewID\n";
    return 1;
  }
  if(theArgsNb == 1)
  {
    theDi.Eval("vviewlist");
    return 0;
  }

  TCollection_AsciiString aNameString(theArgVec[1]);
  if ( strcasecmp( aNameString.ToCString(), "NONE" ) == 0 )
  {
    TCollection_AsciiString aTitle("3D View - ");
    aTitle = aTitle + ViewerTest_myViews.Find2(ViewerTest::CurrentView());
    SetWindowTitle (ViewerTest::CurrentView()->Window(), aTitle.ToCString());
    Handle(V3d_View) anEmptyView;
#if defined(_WIN32) || defined(__WIN32__)
    Handle(WNT_Window) anEmptyWindow;
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
    Handle(Cocoa_Window) anEmptyWindow;
#else
    Handle(Xw_Window) anEmptyWindow;
#endif
    VT_GetWindow() = anEmptyWindow;
    ViewerTest::CurrentView (anEmptyView);
    ViewerTest::ResetEventManager();
    theDi << theArgVec[0] << ": all views are inactive\n";
    return 0;
  }

  ViewerTest_Names aViewNames(aNameString);

  // Check if this view exists in the viewer with the driver
  if (!ViewerTest_myViews.IsBound1(aViewNames.GetViewName()))
  {
    theDi << "Wrong view name\n";
    return 1;
  }

  // Check if it is active already
  if (ViewerTest::CurrentView() == ViewerTest_myViews.Find1(aViewNames.GetViewName()))
  {
    theDi << theArgVec[0] << ": the view is active already\n";
    return 0;
  }

  ActivateView (aViewNames.GetViewName());
  return 0;
}

//==============================================================================
//function : VViewList
//purpose  : Print current list of views per viewer and graphic driver ID
//           shared between viewers
//==============================================================================

static int VViewList (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  if (theArgsNb > 2)
  {
    theDi << theArgVec[0] << ": Wrong number of command arguments\n"
          << "Usage: " << theArgVec[0] << " name";
    return 1;
  }
  if (ViewerTest_myContexts.Size() < 1)
    return 0;

  Standard_Boolean isTreeView =
    (( theArgsNb==1 ) || ( strcasecmp( theArgVec[1], "long" ) != 0 ));

  if (isTreeView)
    theDi << theArgVec[0] <<":\n";

    for (NCollection_DoubleMap <TCollection_AsciiString, Handle(Graphic3d_GraphicDriver)>::Iterator
      aDriverIter(ViewerTest_myDrivers); aDriverIter.More(); aDriverIter.Next())
    {
      if (isTreeView)
        theDi << aDriverIter.Key1() << ":\n";

      for (NCollection_DoubleMap <TCollection_AsciiString, Handle(AIS_InteractiveContext)>::Iterator
        aContextIter(ViewerTest_myContexts); aContextIter.More(); aContextIter.Next())
      {
        if (aContextIter.Key1().Search(aDriverIter.Key1()) != -1)
        {
          if (isTreeView)
          {
            TCollection_AsciiString aContextName(aContextIter.Key1());
            theDi << " " << aContextName.Split(aDriverIter.Key1().Length() + 1) << ":" << "\n";
          }

          for (NCollection_DoubleMap <TCollection_AsciiString, Handle(V3d_View)>::Iterator
            aViewIter(ViewerTest_myViews); aViewIter.More(); aViewIter.Next())
          {
            if (aViewIter.Key1().Search(aContextIter.Key1()) != -1)
            {
              TCollection_AsciiString aViewName(aViewIter.Key1());
              if (isTreeView)
              {
                if (aViewIter.Value() == ViewerTest::CurrentView())
                  theDi << "  " << aViewName.Split(aContextIter.Key1().Length() + 1) << "(*)" << "\n";
                else
                  theDi << "  " << aViewName.Split(aContextIter.Key1().Length() + 1) << "\n";
              }
              else
              {
                theDi << aViewName << " ";
              }
            }
          }
        }
      }
    }
  return 0;
}

//==============================================================================
//function : VT_ProcessKeyPress
//purpose  : Handle KeyPress event from a CString
//==============================================================================
void VT_ProcessKeyPress (const char* buf_ret)
{
  //cout << "KeyPress" << endl;
  const Handle(V3d_View) aView = ViewerTest::CurrentView();
  // Letter in alphabetic order

  if (!strcasecmp (buf_ret, "A"))
  {
    // AXO
    aView->SetProj(V3d_XposYnegZpos);
  }
  else if (!strcasecmp (buf_ret, "D"))
  {
    // Reset
    aView->Reset();
  }
  else if (!strcasecmp (buf_ret, "F"))
  {
    if (ViewerTest::GetAISContext()->NbSelected() > 0)
    {
      ViewerTest::GetAISContext()->FitSelected (aView);
    }
    else
    {
      // FitAll
      aView->FitAll();
    }
  }
  else if (!strcasecmp (buf_ret, "H"))
  {
    // HLR
    cout << "HLR" << endl;
    aView->SetComputedMode (!aView->ComputedMode());
    MyHLRIsOn = aView->ComputedMode();
  }
  else if (!strcasecmp (buf_ret, "P"))
  {
    // Type of HLR
    Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
    if (aContext->DefaultDrawer()->TypeOfHLR() == Prs3d_TOH_Algo)
      aContext->DefaultDrawer()->SetTypeOfHLR(Prs3d_TOH_PolyAlgo);
    else
      aContext->DefaultDrawer()->SetTypeOfHLR(Prs3d_TOH_Algo);
    if (aContext->NbSelected()==0)
    {
      AIS_ListOfInteractive aListOfShapes;
      aContext->DisplayedObjects(aListOfShapes);
      for (AIS_ListIteratorOfListOfInteractive anIter(aListOfShapes);
        anIter.More(); anIter.Next())
      {
        Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(anIter.Value());
        if (aShape.IsNull())
          continue;
        if (aShape->TypeOfHLR() == Prs3d_TOH_PolyAlgo)
          aShape->SetTypeOfHLR (Prs3d_TOH_Algo);
        else
          aShape->SetTypeOfHLR (Prs3d_TOH_PolyAlgo);
        aContext->Redisplay (aShape, Standard_False);
      }
    }
    else
    {
      for (aContext->InitSelected();aContext->MoreSelected();aContext->NextSelected())
      {
        Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(aContext->SelectedInteractive());
        if (aShape.IsNull())
          continue;
        if(aShape->TypeOfHLR() == Prs3d_TOH_PolyAlgo)
          aShape->SetTypeOfHLR (Prs3d_TOH_Algo);
        else
          aShape->SetTypeOfHLR (Prs3d_TOH_PolyAlgo);
        aContext->Redisplay (aShape, Standard_False);
      }
    }

    aContext->UpdateCurrentViewer();

  }
  else if (!strcasecmp (buf_ret, "S"))
  {
    std::cout << "setup Shaded display mode" << std::endl;

    Handle(AIS_InteractiveContext) Ctx = ViewerTest::GetAISContext();
    if(Ctx->NbSelected()==0)
      Ctx->SetDisplayMode(AIS_Shaded);
    else{
      for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected())
        Ctx->SetDisplayMode(Ctx->SelectedInteractive(),1,Standard_False);
      Ctx->UpdateCurrentViewer();
    }
  }
  else if (!strcasecmp (buf_ret, "U"))
  {
    // Unset display mode
    std::cout << "reset display mode to defaults" << std::endl;

    Handle(AIS_InteractiveContext) Ctx = ViewerTest::GetAISContext();
    if(Ctx->NbSelected()==0)
      Ctx->SetDisplayMode(AIS_WireFrame);
    else{
      for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected())
        Ctx->UnsetDisplayMode(Ctx->SelectedInteractive(),Standard_False);
      Ctx->UpdateCurrentViewer();
    }

  }
  else if (!strcasecmp (buf_ret, "T"))
  {
    // Top
    aView->SetProj(V3d_Zpos);
  }
  else if (!strcasecmp (buf_ret, "B"))
  {
    // Bottom
    aView->SetProj(V3d_Zneg);
  }
  else if (!strcasecmp (buf_ret, "L"))
  {
    // Left
    aView->SetProj(V3d_Xneg);
  }
  else if (!strcasecmp (buf_ret, "R"))
  {
    // Right
    aView->SetProj(V3d_Xpos);
  }
  else if (!strcasecmp (buf_ret, "W"))
  {
    std::cout << "setup WireFrame display mode" << std::endl;
    Handle(AIS_InteractiveContext) Ctx = ViewerTest::GetAISContext();
    if(Ctx->NbSelected()==0)
      Ctx->SetDisplayMode(AIS_WireFrame);
    else{
      for(Ctx->InitSelected();Ctx->MoreSelected();Ctx->NextSelected())
        Ctx->SetDisplayMode(Ctx->SelectedInteractive(),0,Standard_False);
      Ctx->UpdateCurrentViewer();
    }
  }
  else if (!strcasecmp (buf_ret, "Z"))
  {
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
  else if (!strcasecmp (buf_ret, ","))
  {
    ViewerTest::GetAISContext()->HilightNextDetected(ViewerTest::CurrentView());
  }
  else if (!strcasecmp (buf_ret, "."))
  {
    ViewerTest::GetAISContext()->HilightPreviousDetected(ViewerTest::CurrentView());
  }
  else if (!strcasecmp (buf_ret, "/"))
  {
    Handle(Graphic3d_Camera) aCamera = aView->Camera();
    if (aCamera->IsStereo())
    {
      aCamera->SetIOD (aCamera->GetIODType(), aCamera->IOD() - 0.01);
      aView->Redraw();
    }
  }
  else if (!strcasecmp (buf_ret, "*"))
  {
    Handle(Graphic3d_Camera) aCamera = aView->Camera();
    if (aCamera->IsStereo())
    {
      aCamera->SetIOD (aCamera->GetIODType(), aCamera->IOD() + 0.01);
      aView->Redraw();
    }
  }
  else if (*buf_ret == THE_KEY_DELETE)
  {
    Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
    if (!aCtx.IsNull()
     && aCtx->NbSelected() > 0)
    {
      Draw_Interprete ("verase");
    }
  }
  else
  {
    // Number
    Standard_Integer Num = Draw::Atoi(buf_ret);
    if(Num>=0 && Num<=7)
      ViewerTest::StandardModeActivation(Num);
  }
}

//==============================================================================
//function : VT_ProcessExpose
//purpose  : Redraw the View on an Expose Event
//==============================================================================
void VT_ProcessExpose()
{
  Handle(V3d_View) aView3d = ViewerTest::CurrentView();
  if (!aView3d.IsNull())
  {
    aView3d->Redraw();
  }
}

//==============================================================================
//function : VT_ProcessConfigure
//purpose  : Resize the View on an Configure Event
//==============================================================================
void VT_ProcessConfigure()
{
  Handle(V3d_View) aView3d = ViewerTest::CurrentView();
  if (aView3d.IsNull())
  {
    return;
  }

  aView3d->MustBeResized();
  aView3d->Update();
  aView3d->Redraw();
}

//==============================================================================
//function : VT_ProcessButton1Press
//purpose  : Picking
//==============================================================================
Standard_Boolean VT_ProcessButton1Press (Standard_Integer ,
                                         const char**     theArgVec,
                                         Standard_Boolean theToPick,
                                         Standard_Boolean theIsShift)
{
  if (theToPick)
  {
    Standard_Real X, Y, Z;
    ViewerTest::CurrentView()->Convert (X_Motion, Y_Motion, X, Y, Z);

    Draw::Set (theArgVec[1], X);
    Draw::Set (theArgVec[2], Y);
    Draw::Set (theArgVec[3], Z);
  }

  if (theIsShift)
  {
    ViewerTest::CurrentEventManager()->ShiftSelect();
  }
  else
  {
    ViewerTest::CurrentEventManager()->Select();
  }

  return Standard_False;
}

//==============================================================================
//function : VT_ProcessButton1Release
//purpose  : End selecting
//==============================================================================
void VT_ProcessButton1Release (Standard_Boolean theIsShift)
{
  if (IsDragged)
  {
    IsDragged = Standard_False;
    Handle(ViewerTest_EventManager) EM = ViewerTest::CurrentEventManager();
    if (theIsShift)
    {
      EM->ShiftSelect (X_ButtonPress, Y_ButtonPress,
                       X_Motion, Y_Motion);
    }
    else
    {
      EM->Select (X_ButtonPress, Y_ButtonPress,
                  X_Motion, Y_Motion);
    }
  }
}

//==============================================================================
//function : VT_ProcessButton3Press
//purpose  : Start Rotation
//==============================================================================
void VT_ProcessButton3Press()
{
  Start_Rot = 1;
  if (MyHLRIsOn)
  {
    ViewerTest::CurrentView()->SetComputedMode (Standard_False);
  }
  ViewerTest::CurrentView()->StartRotation( X_ButtonPress, Y_ButtonPress );
}

//==============================================================================
//function : VT_ProcessButton3Release
//purpose  : End rotation
//==============================================================================
void VT_ProcessButton3Release()
{
  if (Start_Rot)
  {
    Start_Rot = 0;
    if (MyHLRIsOn)
    {
      ViewerTest::CurrentView()->SetComputedMode (Standard_True);
    }
  }
}

//==============================================================================
//function : ProcessZClipMotion
//purpose  : Zoom
//==============================================================================

void ProcessZClipMotion()
{
  Handle(V3d_View)  a3DView = ViewerTest::CurrentView();
  if ( Abs(X_Motion - X_ButtonPress) > 2 ) {

    //Quantity_Length VDX, VDY;
    //a3DView->Size(VDX,VDY);
    //Standard_Real VDZ = a3DView->ZSize();
    //printf("View size (%lf,%lf,%lf)\n", VDX, VDY, VDZ);

    Quantity_Length aDx = a3DView->Convert(X_Motion - X_ButtonPress);

    // Front = Depth + width/2.
    Standard_Real aDepth = 0.5;
    Standard_Real aWidth = 0.1;
    a3DView->ZClipping(aDepth,aWidth);

    aDepth += aDx;

    //printf("dx %lf Depth %lf Width %lf\n", dx, D, W);

    a3DView->SetZClippingDepth(aDepth);

    a3DView->Redraw();

    X_ButtonPress = X_Motion;
    Y_ButtonPress = Y_Motion;
  }
}

//==============================================================================
//function : ProcessControlButton1Motion
//purpose  : Zoom
//==============================================================================

#if defined(_WIN32) || ! defined(__APPLE__) || defined(MACOSX_USE_GLX)
static void ProcessControlButton1Motion()
{
  ViewerTest::CurrentView()->Zoom( X_ButtonPress, Y_ButtonPress, X_Motion, Y_Motion);

  X_ButtonPress = X_Motion;
  Y_ButtonPress = Y_Motion;
}
#endif

//==============================================================================
//function : VT_ProcessControlButton2Motion
//purpose  : Panning
//==============================================================================
void VT_ProcessControlButton2Motion()
{
  Standard_Integer aDx = X_Motion - X_ButtonPress;
  Standard_Integer aDy = Y_Motion - Y_ButtonPress;

  aDy = -aDy; // Xwindow Y axis is from top to Bottom

  ViewerTest::CurrentView()->Pan (aDx, aDy);

  X_ButtonPress = X_Motion;
  Y_ButtonPress = Y_Motion;
}

//==============================================================================
//function : VT_ProcessControlButton3Motion
//purpose  : Rotation
//==============================================================================
void VT_ProcessControlButton3Motion()
{
  if (Start_Rot)
  {
    ViewerTest::CurrentView()->Rotation (X_Motion, Y_Motion);
  }
}

//==============================================================================
//function : VT_ProcessMotion
//purpose  :
//==============================================================================
void VT_ProcessMotion()
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
//function : ViewProject: implements VAxo, VTop, VLeft, ...
//purpose  : Switches to an axonometric, top, left and other views
//==============================================================================

static int ViewProject(Draw_Interpretor& di, const V3d_TypeOfOrientation ori)
{
  if ( ViewerTest::CurrentView().IsNull() )
  {
    di<<"Call vinit before this command, please"<<"\n";
    return 1;
  }

  ViewerTest::CurrentView()->SetProj(ori);
  return 0;
}

//==============================================================================
//function : VAxo
//purpose  : Switch to an Axonometric view
//Draw arg : No args
//==============================================================================

static int VAxo(Draw_Interpretor& di, Standard_Integer , const char** )
{
  return ViewProject(di, V3d_XposYnegZpos);
}

//==============================================================================
//function : VTop
//purpose  : Switch to a Top View
//Draw arg : No args
//==============================================================================

static int VTop(Draw_Interpretor& di, Standard_Integer , const char** )
{
  return ViewProject(di, V3d_Zpos);
}

//==============================================================================
//function : VBottom
//purpose  : Switch to a Bottom View
//Draw arg : No args
//==============================================================================

static int VBottom(Draw_Interpretor& di, Standard_Integer , const char** )
{
  return ViewProject(di, V3d_Zneg);
}

//==============================================================================
//function : VLeft
//purpose  : Switch to a Left View
//Draw arg : No args
//==============================================================================

static int VLeft(Draw_Interpretor& di, Standard_Integer , const char** )
{
  return ViewProject(di, V3d_Xneg);
}

//==============================================================================
//function : VRight
//purpose  : Switch to a Right View
//Draw arg : No args
//==============================================================================

static int VRight(Draw_Interpretor& di, Standard_Integer , const char** )
{
  return ViewProject(di, V3d_Xpos);
}

//==============================================================================
//function : VFront
//purpose  : Switch to a Front View
//Draw arg : No args
//==============================================================================

static int VFront(Draw_Interpretor& di, Standard_Integer , const char** )
{
  return ViewProject(di, V3d_Yneg);
}

//==============================================================================
//function : VBack
//purpose  : Switch to a Back View
//Draw arg : No args
//==============================================================================

static int VBack(Draw_Interpretor& di, Standard_Integer , const char** )
{
  return ViewProject(di, V3d_Ypos);
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
  di << "B : BottomView" << "\n";
  di << "R : RightView" << "\n";
  di << "L : LeftView" << "\n";
  di << "A : AxonometricView" << "\n";
  di << "D : ResetView" << "\n";

  di << "========================="<<"\n";
  di << "S : Shading" << "\n";
  di << "W : Wireframe" << "\n";
  di << "H : HidelLineRemoval" << "\n";
  di << "U : Unset display mode" << "\n";
  di << "Delete : Remove selection from viewer" << "\n";

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

  di << "========================="<<"\n";
  di << "Z : Switch Z clipping On/Off" << "\n";
  di << ", : Hilight next detected" << "\n";
  di << ". : Hilight previous detected" << "\n";

  return 0;
}

#ifdef _WIN32

static Standard_Boolean Ppick = 0;
static Standard_Integer Pargc = 0;
static const char**           Pargv = NULL;


static LRESULT WINAPI AdvViewerWindowProc( HWND hwnd,
                                          UINT Msg,
                                          WPARAM wParam,
                                          LPARAM lParam )
{
  if (!ViewerTest_myViews.IsEmpty()) {

    WPARAM fwKeys = wParam;

    switch( Msg ) {
    case WM_CLOSE:
       {
         // Delete view from map of views
         ViewerTest::RemoveView(FindViewIdByWindowHandle(hwnd));
         return 0;
       }
       break;
    case WM_ACTIVATE:
      if(LOWORD(wParam) == WA_CLICKACTIVE || LOWORD(wParam) == WA_ACTIVE
        || ViewerTest::CurrentView().IsNull())
      {
        // Activate inactive window
        if(GetWindowHandle(VT_GetWindow()) != hwnd)
        {
          ActivateView (FindViewIdByWindowHandle(hwnd));
        }
      }
      break;
    case WM_LBUTTONUP:
      if (!DragFirst)
      {
        HDC hdc = GetDC( hwnd );
        SelectObject( hdc, GetStockObject( HOLLOW_BRUSH ) );
        SetROP2( hdc, R2_NOT );
        Rectangle( hdc, X_ButtonPress, Y_ButtonPress, X_Motion, Y_Motion );
        ReleaseDC( hwnd, hdc );
        VT_ProcessButton1Release (fwKeys & MK_SHIFT);
      }
      IsDragged = Standard_False;
      return ViewerWindowProc( hwnd, Msg, wParam, lParam );

    case WM_LBUTTONDOWN:
      if( fwKeys == MK_LBUTTON || fwKeys == ( MK_LBUTTON | MK_SHIFT ) )
      {
        IsDragged = Standard_True;
        DragFirst = Standard_True;
        X_ButtonPress = LOWORD(lParam);
        Y_ButtonPress = HIWORD(lParam);
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
          Rectangle( hdc, X_ButtonPress, Y_ButtonPress, X_Motion, Y_Motion );

        DragFirst = Standard_False;
        X_Motion = LOWORD(lParam);
        Y_Motion = HIWORD(lParam);

        Rectangle( hdc, X_ButtonPress, Y_ButtonPress, X_Motion, Y_Motion );

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
  static int Up = 1;
  const Handle(V3d_View)& aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    return DefWindowProc( hwnd, Msg, wParam, lParam );
  }

    PAINTSTRUCT    ps;

    switch( Msg ) {
    case WM_PAINT:
      BeginPaint(hwnd, &ps);
      EndPaint(hwnd, &ps);
      VT_ProcessExpose();
      break;

    case WM_SIZE:
      VT_ProcessConfigure();
      break;
    case WM_MOVE:
    case WM_MOVING:
    case WM_SIZING:
      switch (aView->RenderingParams().StereoMode)
      {
        case Graphic3d_StereoMode_RowInterlaced:
        case Graphic3d_StereoMode_ColumnInterlaced:
        case Graphic3d_StereoMode_ChessBoard:
          VT_ProcessConfigure(); // track window moves to reverse stereo pair
          break;
        default:
          break;
      }
      break;

    case WM_KEYDOWN:
      if ((wParam != VK_SHIFT) && (wParam != VK_CONTROL))
      {
        char c[2];
        c[0] = (char) wParam;
        c[1] = '\0';
        if (wParam == VK_DELETE)
        {
          c[0] = THE_KEY_DELETE;
        }
        // comma
        else if (wParam == VK_OEM_COMMA)
        {
          c[0] = ',';
        }
        // dot
        else if (wParam == VK_OEM_PERIOD)
        {
          c[0] = '.';
        }
        else if (wParam == VK_DIVIDE)
        {
          c[0] = '/';
        }
        // dot
        else if (wParam == VK_MULTIPLY)
        {
          c[0] = '*';
        }
        VT_ProcessKeyPress (c);
      }
      break;

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
      Up = 1;
      VT_ProcessButton3Release();
      break;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
      {
        WPARAM fwKeys = wParam;

        Up = 0;

        X_ButtonPress = LOWORD(lParam);
        Y_ButtonPress = HIWORD(lParam);

        if (Msg == WM_LBUTTONDOWN)
        {
          if (fwKeys & MK_CONTROL)
          {
            Ppick = VT_ProcessButton1Press (Pargc, Pargv, Ppick, (fwKeys & MK_SHIFT));
          }
          else
          {
            VT_ProcessButton1Press (Pargc, Pargv, Ppick, (fwKeys & MK_SHIFT));
          }
        }
        else if (Msg == WM_RBUTTONDOWN)
        {
          // Start rotation
          VT_ProcessButton3Press();
        }
      }
      break;

    case WM_MOUSEWHEEL:
    {
      int aDelta = GET_WHEEL_DELTA_WPARAM (wParam);
      if (wParam & MK_CONTROL)
      {
        if (aView->Camera()->IsStereo())
        {
          Standard_Real aFocus = aView->Camera()->ZFocus() + (aDelta > 0 ? 0.05 : -0.05);
          if (aFocus > 0.2
           && aFocus < 2.0)
          {
            aView->Camera()->SetZFocus (aView->Camera()->ZFocusType(), aFocus);
            aView->Redraw();
          }
        }
      }
      else
      {
        aView->Zoom (0, 0, aDelta / 40, aDelta / 40);
      }
      break;
    }

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
              VT_ProcessButton3Press();
            }
          }

          if ( fwKeys & MK_CONTROL ) {
            if ( fwKeys & MK_LBUTTON ) {
              ProcessControlButton1Motion();
            }
            else if ( fwKeys & MK_MBUTTON ||
              ((fwKeys&MK_LBUTTON) &&
              (fwKeys&MK_RBUTTON) ) ){
                VT_ProcessControlButton2Motion();
              }
            else if ( fwKeys & MK_RBUTTON ) {
              VT_ProcessControlButton3Motion();
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
          else if (GetWindowHandle (VT_GetWindow()) == hwnd)
          {
            if ((fwKeys & MK_MBUTTON
            || ((fwKeys & MK_LBUTTON) && (fwKeys & MK_RBUTTON))))
            {
              ProcessZClipMotion();
            }
            else
            {
              VT_ProcessMotion();
            }
          }
      }
      break;

    default:
      return( DefWindowProc( hwnd, Msg, wParam, lParam ));
    }
    return 0L;
}




//==============================================================================
//function : ViewerMainLoop
//purpose  : Get a Event on the view and dispatch it
//==============================================================================


int ViewerMainLoop(Standard_Integer argc, const char** argv)
{
  Ppick = (argc > 0)? 1 : 0;
  Pargc = argc;
  Pargv = argv;

  if ( Ppick ) {
    MSG msg;
    msg.wParam = 1;

    cout << "Start picking" << endl;

    while ( Ppick == 1 ) {
      // Wait for a VT_ProcessButton1Press() to toggle pick to 1 or 0
      if (GetMessage(&msg, NULL, 0, 0) ) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }

    cout << "Picking done" << endl;
  }

  return Ppick;
}

#elif !defined(__APPLE__) || defined(MACOSX_USE_GLX)

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

{
  static XEvent aReport;
  Standard_Boolean pick = argc > 0;
  Display *aDisplay = GetDisplayConnection()->GetDisplay();
  XNextEvent (aDisplay, &aReport);

  // Handle event for the chosen display connection
  switch (aReport.type) {
      case ClientMessage:
        {
          if((Atom)aReport.xclient.data.l[0] == GetDisplayConnection()->GetAtom(Aspect_XA_DELETE_WINDOW))
          {
            // Close the window
            ViewerTest::RemoveView(FindViewIdByWindowHandle (aReport.xclient.window));
          }
        }
        return 0;
     case FocusIn:
      {
         // Activate inactive view
         Window aWindow = GetWindowHandle(VT_GetWindow());
         if(aWindow != aReport.xfocus.window)
         {
           ActivateView (FindViewIdByWindowHandle (aReport.xfocus.window));
         }
      }
      break;
      case Expose:
        {
          VT_ProcessExpose();
        }
        break;
      case ConfigureNotify:
        {
          VT_ProcessConfigure();
        }
        break;
      case KeyPress:
        {

          KeySym ks_ret ;
          char buf_ret[11] ;
          int ret_len ;
          XComposeStatus status_in_out;

          ret_len = XLookupString( ( XKeyEvent *)&aReport ,
            (char *) buf_ret , 10 ,
            &ks_ret , &status_in_out ) ;


          buf_ret[ret_len] = '\0' ;

          if (ret_len)
          {
            VT_ProcessKeyPress (buf_ret);
          }
        }
        break;
      case ButtonPress:
        {
          X_ButtonPress = aReport.xbutton.x;
          Y_ButtonPress = aReport.xbutton.y;

          if (aReport.xbutton.button == Button1)
          {
            if (aReport.xbutton.state & ControlMask)
            {
              pick = VT_ProcessButton1Press (argc, argv, pick, (aReport.xbutton.state & ShiftMask));
            }
            else
            {
              IsDragged = Standard_True;
              DragFirst = Standard_True;
            }
          }
          else if (aReport.xbutton.button == Button3)
          {
            // Start rotation
            VT_ProcessButton3Press();
          }
        }
        break;
      case ButtonRelease:
        {
          if( IsDragged )
          {
            if( !DragFirst )
            {
              Aspect_Handle aWindow = VT_GetWindow()->XWindow();
              GC gc = XCreateGC( aDisplay, aWindow, 0, 0 );
              XDrawRectangle( aDisplay, aWindow, gc, min( X_ButtonPress, X_Motion ), min( Y_ButtonPress, Y_Motion ), abs( X_Motion-X_ButtonPress ), abs( Y_Motion-Y_ButtonPress ) );
            }

            Handle( AIS_InteractiveContext ) aContext = ViewerTest::GetAISContext();
            if( aContext.IsNull() )
            {
              cout << "The context is null. Please use vinit before createmesh" << endl;
              return 0;
            }

            Standard_Boolean ShiftPressed = ( aReport.xbutton.state & ShiftMask );
            if( aReport.xbutton.button==1 )
              if( DragFirst )
                if( ShiftPressed )
                {
                  aContext->ShiftSelect();
                }
                else
                {
                  aContext->Select();
                }
              else
                if( ShiftPressed )
                {
                  aContext->ShiftSelect( min( X_ButtonPress, X_Motion ), min( Y_ButtonPress, Y_Motion ),
                    max( X_ButtonPress, X_Motion ), max( Y_ButtonPress, Y_Motion ),
                    ViewerTest::CurrentView());
                }
                else
                {
                  aContext->Select( min( X_ButtonPress, X_Motion ), min( Y_ButtonPress, Y_Motion ),
                    max( X_ButtonPress, X_Motion ), max( Y_ButtonPress, Y_Motion ),
                    ViewerTest::CurrentView() );
                }
            else
              VT_ProcessButton3Release();

            IsDragged = Standard_False;
          }
          else
            VT_ProcessButton3Release();
        }
        break;
      case MotionNotify:
        {
          if (GetWindowHandle (VT_GetWindow()) != aReport.xmotion.window)
          {
            break;
          }
          if( IsDragged )
          {
            Aspect_Handle aWindow = VT_GetWindow()->XWindow();
            GC gc = XCreateGC( aDisplay, aWindow, 0, 0 );
            XSetFunction( aDisplay, gc, GXinvert );

            if( !DragFirst )
              XDrawRectangle(aDisplay, aWindow, gc, min( X_ButtonPress, X_Motion ), min( Y_ButtonPress, Y_Motion ), abs( X_Motion-X_ButtonPress ), abs( Y_Motion-Y_ButtonPress ) );

            X_Motion = aReport.xmotion.x;
            Y_Motion = aReport.xmotion.y;
            DragFirst = Standard_False;

            XDrawRectangle( aDisplay, aWindow, gc, min( X_ButtonPress, X_Motion ), min( Y_ButtonPress, Y_Motion ), abs( X_Motion-X_ButtonPress ), abs( Y_Motion-Y_ButtonPress ) );
          }
          else
          {
            X_Motion = aReport.xmotion.x;
            Y_Motion = aReport.xmotion.y;

            // remove all the ButtonMotionMaskr
            while( XCheckMaskEvent( aDisplay, ButtonMotionMask, &aReport) ) ;

            if ( ZClipIsOn && aReport.xmotion.state & ShiftMask ) {
              if ( Abs(X_Motion - X_ButtonPress) > 2 ) {

                Quantity_Length VDX, VDY;

                ViewerTest::CurrentView()->Size(VDX,VDY);
                Standard_Real VDZ =0 ;
                VDZ = ViewerTest::CurrentView()->ZSize();

                printf("%f,%f,%f\n", VDX, VDY, VDZ);

                Quantity_Length dx = 0 ;
                dx = ViewerTest::CurrentView()->Convert(X_Motion - X_ButtonPress);

                cout << dx << endl;

                dx = dx / VDX * VDZ;

                cout << dx << endl;

                ViewerTest::CurrentView()->Redraw();
              }
            }

            if ( aReport.xmotion.state & ControlMask ) {
              if ( aReport.xmotion.state & Button1Mask ) {
                ProcessControlButton1Motion();
              }
              else if ( aReport.xmotion.state & Button2Mask ) {
                VT_ProcessControlButton2Motion();
              }
              else if ( aReport.xmotion.state & Button3Mask ) {
                VT_ProcessControlButton3Motion();
              }
            }
            else
            {
              VT_ProcessMotion();
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
  NCollection_Vector<int> anEventNumbers;
  // Get number of messages from every display
  for (NCollection_DoubleMap <TCollection_AsciiString, Handle(Graphic3d_GraphicDriver)>::Iterator
       anIter (ViewerTest_myDrivers); anIter.More(); anIter.Next())
  {
    anEventNumbers.Append(XPending (anIter.Key2()->GetDisplayConnection()->GetDisplay()));
  }
    // Handle events for every display
  int anEventIter = 0;
  for (NCollection_DoubleMap <TCollection_AsciiString, Handle(Graphic3d_GraphicDriver)>::Iterator
       anIter (ViewerTest_myDrivers); anIter.More(); anIter.Next(), anEventIter++)
  {
    for (int i = 0; i < anEventNumbers.Value(anEventIter) &&
         XPending (anIter.Key2()->GetDisplayConnection()->GetDisplay()) > 0; ++i)
    {
      SetDisplayConnection (anIter.Key2()->GetDisplayConnection());
      int anEventResult = ViewerMainLoop( 0, NULL);
      // If window is closed or context was not found finish current event processing loop
      if (!anEventResult)
	return;
    }
  }

  SetDisplayConnection (ViewerTest::GetAISContext()->CurrentViewer()->Driver()->GetDisplayConnection());

}
#endif

//==============================================================================
//function : OSWindowSetup
//purpose  : Setup for the X11 window to be able to cath the event
//==============================================================================


static void OSWindowSetup()
{
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  // X11

  Window  window   = VT_GetWindow()->XWindow();
  SetDisplayConnection (ViewerTest::CurrentView()->Viewer()->Driver()->GetDisplayConnection());
  Display *aDisplay = GetDisplayConnection()->GetDisplay();
  XSynchronize(aDisplay, 1);

  // X11 : For keyboard on SUN
  XWMHints wmhints;
  wmhints.flags = InputHint;
  wmhints.input = 1;

  XSetWMHints( aDisplay, window, &wmhints);

  XSelectInput( aDisplay, window,  ExposureMask | KeyPressMask |
    ButtonPressMask | ButtonReleaseMask |
    StructureNotifyMask |
    PointerMotionMask |
    Button1MotionMask | Button2MotionMask |
    Button3MotionMask | FocusChangeMask
    );
  Atom aDeleteWindowAtom = GetDisplayConnection()->GetAtom(Aspect_XA_DELETE_WINDOW);
  XSetWMProtocols(aDisplay, window, &aDeleteWindowAtom, 1);

  XSynchronize(aDisplay, 0);

#else
  // _WIN32
#endif

}

//==============================================================================
//function : VFit

//purpose  : Fitall, no DRAW arguments
//Draw arg : No args
//==============================================================================

static int VFit (Draw_Interpretor& /*theDi*/, Standard_Integer theArgc, const char** theArgv)
{
  if (theArgc > 2)
  {
    std::cout << "Wrong number of arguments! Use: vfit [-selected]" << std::endl;
  }

  const Handle(V3d_View) aView = ViewerTest::CurrentView();

  if (theArgc == 2)
  {
    TCollection_AsciiString anArg (theArgv[1]);
    anArg.LowerCase();
    if (anArg == "-selected")
    {
      ViewerTest::GetAISContext()->FitSelected (aView);
      return 0;
    }
  }
  if (aView.IsNull() == Standard_False) {

    aView->FitAll();
  }
  return 0;
}

//=======================================================================
//function : VFitArea
//purpose  : Fit view to show area located between two points
//         : given in world 2D or 3D coordinates.
//=======================================================================
static int VFitArea (Draw_Interpretor& theDI, Standard_Integer  theArgNb, const char** theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << theArgVec[0] << "Error: No active view.\n";
    return 1;
  }

  // Parse arguments.
  gp_Pnt aWorldPnt1 (0.0, 0.0, 0.0);
  gp_Pnt aWorldPnt2 (0.0, 0.0, 0.0);

  if (theArgNb == 5)
  {
    aWorldPnt1.SetX (Draw::Atof (theArgVec[1]));
    aWorldPnt1.SetY (Draw::Atof (theArgVec[2]));
    aWorldPnt2.SetX (Draw::Atof (theArgVec[3]));
    aWorldPnt2.SetY (Draw::Atof (theArgVec[4]));
  }
  else if (theArgNb == 7)
  {
    aWorldPnt1.SetX (Draw::Atof (theArgVec[1]));
    aWorldPnt1.SetY (Draw::Atof (theArgVec[2]));
    aWorldPnt1.SetZ (Draw::Atof (theArgVec[3]));
    aWorldPnt2.SetX (Draw::Atof (theArgVec[4]));
    aWorldPnt2.SetY (Draw::Atof (theArgVec[5]));
    aWorldPnt2.SetZ (Draw::Atof (theArgVec[6]));
  }
  else
  {
    std::cerr << theArgVec[0] << "Error: Invalid number of arguments.\n";
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }

  // Convert model coordinates to view space
  Handle(Graphic3d_Camera) aCamera = aView->Camera();
  gp_Pnt aViewPnt1 = aCamera->ConvertWorld2View (aWorldPnt1);
  gp_Pnt aViewPnt2 = aCamera->ConvertWorld2View (aWorldPnt2);

  // Determine fit area
  gp_Pnt2d aMinCorner (Min (aViewPnt1.X(), aViewPnt2.X()), Min (aViewPnt1.Y(), aViewPnt2.Y()));
  gp_Pnt2d aMaxCorner (Max (aViewPnt1.X(), aViewPnt2.X()), Max (aViewPnt1.Y(), aViewPnt2.Y()));

  Standard_Real aDiagonal = aMinCorner.Distance (aMaxCorner);

  if (aDiagonal < Precision::Confusion())
  {
    std::cerr << theArgVec[0] << "Error: view area is too small.\n";
    return 1;
  }

  aView->FitAll (aMinCorner.X(), aMinCorner.Y(), aMaxCorner.X(), aMaxCorner.Y());
  return 0;
}

//==============================================================================
//function : VZFit
//purpose  : ZFitall, no DRAW arguments
//Draw arg : No args
//==============================================================================
static int VZFit (Draw_Interpretor& /*theDi*/, Standard_Integer theArgsNb, const char** theArgVec)
{
  const Handle(V3d_View)& aCurrentView = ViewerTest::CurrentView();

  if (aCurrentView.IsNull())
  {
    std::cout << theArgVec[0] << ": Call vinit before this command, please.\n";
    return 1;
  }

  if (theArgsNb == 1)
  {
    aCurrentView->ZFitAll();
    aCurrentView->Redraw();
    return 0;
  }

  Standard_Real aScale = 1.0;

  if (theArgsNb >= 2)
  {
    aScale = Draw::Atoi (theArgVec[1]);
  }

  aCurrentView->ZFitAll (aScale);
  aCurrentView->Redraw();

  return 0;
}

//==============================================================================
//function : VRepaint
//purpose  :
//==============================================================================
static int VRepaint (Draw_Interpretor& , Standard_Integer , const char** )
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
//function : VSetBg
//purpose  : Load image as background
//==============================================================================

static int VSetBg(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 2 || argc > 3)
  {
    di << "Usage : " << argv[0] << " imagefile [filltype] : Load image as background" << "\n";
    di << "filltype can be one of CENTERED, TILED, STRETCH, NONE" << "\n";
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
//function : VSetBgMode
//purpose  : Change background image fill type
//==============================================================================

static int VSetBgMode(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " filltype : Change background image mode" << "\n";
    di << "filltype must be one of CENTERED, TILED, STRETCH, NONE" << "\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) AISContext = ViewerTest::GetAISContext();
  if(AISContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  Aspect_FillMethod aFillType = Aspect_FM_NONE;
  const char* szType = argv[1];
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
  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  V3dView->SetBgImageStyle(aFillType, Standard_True);
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

    Standard_Real R1 = Draw::Atof(argv[1])/255.;
    Standard_Real G1 = Draw::Atof(argv[2])/255.;
    Standard_Real B1 = Draw::Atof(argv[3])/255.;
    Quantity_Color aColor1(R1,G1,B1,Quantity_TOC_RGB);

    Standard_Real R2 = Draw::Atof(argv[4])/255.;
    Standard_Real G2 = Draw::Atof(argv[5])/255.;
    Standard_Real B2 = Draw::Atof(argv[6])/255.;

    Quantity_Color aColor2(R2,G2,B2,Quantity_TOC_RGB);
    int aType = Draw::Atoi(argv[7]);
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
//function : VSetGradientBgMode
//purpose  : Change gradient background fill style
//==============================================================================
static int VSetGradientBgMode(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 2 )
  {
    di << "Usage : " << argv[0] << " Type : Change gradient background fill type" << "\n";
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
  if (argc == 2)
  {
    int aType = Draw::Atoi(argv[1]);
    if( aType < 0 || aType > 8 )
    {
      di << "Wrong fill type " << "\n";
      di << "Must be one of 0 = NONE, 1 = HOR, 2 = VER, 3 = DIAG1, 4 = DIAG2" << "\n";
      di << "               5 = CORNER1, 6 = CORNER2, 7 = CORNER3, 8 = CORNER4" << "\n";
      return 1;
    }

    Aspect_GradientFillMethod aMethod = Aspect_GradientFillMethod(aType);

    Handle(V3d_View) V3dView = ViewerTest::CurrentView();
    V3dView->SetBgGradientStyle( aMethod, 1 );
  }

  return 0;
}

//==============================================================================
//function : VSetColorBg
//purpose  : Set color background
//==============================================================================
static int VSetColorBg(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 4 )
  {
    di << "Usage : " << argv[0] << " R G B : Set color background" << "\n";
    di << "R,G,B = [0..255]" << "\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) AISContext = ViewerTest::GetAISContext();
  if(AISContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if (argc == 4)
  {

    Standard_Real R = Draw::Atof(argv[1])/255.;
    Standard_Real G = Draw::Atof(argv[2])/255.;
    Standard_Real B = Draw::Atof(argv[3])/255.;
    Quantity_Color aColor(R,G,B,Quantity_TOC_RGB);

    Handle(V3d_View) V3dView = ViewerTest::CurrentView();
    V3dView->SetBackgroundColor( aColor );
    V3dView->Update();
  }

  return 0;
}

//==============================================================================
//function : VSetDefaultBg
//purpose  : Set default viewer background fill color
//==============================================================================
static int VSetDefaultBg (Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb != 4
   && theArgNb != 8)
  {
    std::cout << "Error: wrong syntax! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  ViewerTest_DefaultBackground.FillMethod =
    theArgNb == 4 ? Aspect_GFM_NONE
                  : (Aspect_GradientFillMethod) Draw::Atoi (theArgVec[7]);

  if (theArgNb == 4)
  {
    Standard_Real R = Draw::Atof (theArgVec[1]) / 255.;
    Standard_Real G = Draw::Atof (theArgVec[2]) / 255.;
    Standard_Real B = Draw::Atof (theArgVec[3]) / 255.;
    ViewerTest_DefaultBackground.FlatColor.SetValues (R, G, B, Quantity_TOC_RGB);
  }
  else
  {
    Standard_Real R1 = Draw::Atof (theArgVec[1]) / 255.;
    Standard_Real G1 = Draw::Atof (theArgVec[2]) / 255.;
    Standard_Real B1 = Draw::Atof (theArgVec[3]) / 255.;
    ViewerTest_DefaultBackground.GradientColor1.SetValues (R1, G1, B1, Quantity_TOC_RGB);

    Standard_Real R2 = Draw::Atof (theArgVec[4]) / 255.;
    Standard_Real G2 = Draw::Atof (theArgVec[5]) / 255.;
    Standard_Real B2 = Draw::Atof (theArgVec[6]) / 255.;
    ViewerTest_DefaultBackground.GradientColor2.SetValues (R2, G2, B2, Quantity_TOC_RGB);
  }

  for (NCollection_DoubleMap<TCollection_AsciiString, Handle(AIS_InteractiveContext)>::Iterator
       anIter (ViewerTest_myContexts); anIter.More(); anIter.Next())
  {
    const Handle(V3d_Viewer)& aViewer = anIter.Value()->CurrentViewer();
    aViewer->SetDefaultBackgroundColor (ViewerTest_DefaultBackground.FlatColor);
    aViewer->SetDefaultBgGradientColors (ViewerTest_DefaultBackground.GradientColor1,
                                         ViewerTest_DefaultBackground.GradientColor2,
                                         ViewerTest_DefaultBackground.FillMethod);
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
  V3dView->SetAxialScale( Draw::Atof(argv[1]),  Draw::Atof(argv[2]),  Draw::Atof(argv[3]) );
  return 0;
}
//==============================================================================
//function : VZBuffTrihedron
//purpose  :
//==============================================================================

static int VZBuffTrihedron (Draw_Interpretor& /*theDI*/,
                            Standard_Integer  theArgNb,
                            const char**      theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cout << "Error: no active viewer!\n";
    return 1;
  }

  ViewerTest_AutoUpdater anUpdateTool (ViewerTest::GetAISContext(), aView);

  Aspect_TypeOfTriedronPosition aPosition     = Aspect_TOTP_LEFT_LOWER;
  V3d_TypeOfVisualization       aVisType      = V3d_ZBUFFER;
  Quantity_Color                aLabelsColor  = Quantity_NOC_WHITE;
  Quantity_Color                anArrowColorX = Quantity_NOC_RED;
  Quantity_Color                anArrowColorY = Quantity_NOC_GREEN;
  Quantity_Color                anArrowColorZ = Quantity_NOC_BLUE1;
  Standard_Real                 aScale        = 0.1;
  Standard_Real                 aSizeRatio    = 0.8;
  Standard_Real                 anArrowDiam   = 0.05;
  Standard_Integer              aNbFacets     = 12;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg = theArgVec[anArgIter];
    TCollection_AsciiString aFlag (anArg);
    aFlag.LowerCase();
    if (anUpdateTool.parseRedrawMode (aFlag))
    {
      continue;
    }
    else if (aFlag == "-on")
    {
      continue;
    }
    else if (aFlag == "-off")
    {
      aView->TriedronErase();
      return 0;
    }
    else if (aFlag == "-pos"
          || aFlag == "-position"
          || aFlag == "-corner")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cerr << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }

      TCollection_AsciiString aPosName (theArgVec[anArgIter]);
      aPosName.LowerCase();
      if (aPosName == "center")
      {
        aPosition = Aspect_TOTP_CENTER;
      }
      else if (aPosName == "left_lower"
            || aPosName == "lower_left"
            || aPosName == "leftlower"
            || aPosName == "lowerleft")
      {
        aPosition = Aspect_TOTP_LEFT_LOWER;
      }
      else if (aPosName == "left_upper"
            || aPosName == "upper_left"
            || aPosName == "leftupper"
            || aPosName == "upperleft")
      {
        aPosition = Aspect_TOTP_LEFT_UPPER;
      }
      else if (aPosName == "right_lower"
            || aPosName == "lower_right"
            || aPosName == "rightlower"
            || aPosName == "lowerright")
      {
        aPosition = Aspect_TOTP_RIGHT_LOWER;
      }
      else if (aPosName == "right_upper"
            || aPosName == "upper_right"
            || aPosName == "rightupper"
            || aPosName == "upperright")
      {
        aPosition = Aspect_TOTP_RIGHT_UPPER;
      }
      else
      {
        std::cerr << "Error: wrong syntax at '" << anArg << "' - unknown position '" << aPosName << "'\n";
        return 1;
      }
    }
    else if (aFlag == "-type")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cerr << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }

      TCollection_AsciiString aTypeName (theArgVec[anArgIter]);
      aTypeName.LowerCase();
      if (aTypeName == "wireframe"
       || aTypeName == "wire")
      {
        aVisType = V3d_WIREFRAME;
      }
      else if (aTypeName == "zbuffer"
            || aTypeName == "shaded")
      {
        aVisType = V3d_ZBUFFER;
      }
      else
      {
        std::cerr << "Error: wrong syntax at '" << anArg << "' - unknown type '" << aTypeName << "'\n";
      }
    }
    else if (aFlag == "-scale")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cerr << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }

      aScale = Draw::Atof (theArgVec[anArgIter]);
    }
    else if (aFlag == "-size"
          || aFlag == "-sizeratio")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cerr << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }

      aSizeRatio = Draw::Atof (theArgVec[anArgIter]);
    }
    else if (aFlag == "-arrowdiam"
          || aFlag == "-arrowdiameter")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cerr << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }

      anArrowDiam = Draw::Atof (theArgVec[anArgIter]);
    }
    else if (aFlag == "-nbfacets")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cerr << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }

      aNbFacets = Draw::Atoi (theArgVec[anArgIter]);
    }
    else if (aFlag == "-colorlabel"
          || aFlag == "-colorlabels")
    {
      Standard_Integer aNbParsed = ViewerTest::ParseColor (theArgNb  - anArgIter - 1,
                                                           theArgVec + anArgIter + 1,
                                                           aLabelsColor);
      if (aNbParsed == 0)
      {
        std::cerr << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }
      anArgIter += aNbParsed;
    }
    else if (aFlag == "-colorarrowx")
    {
      Standard_Integer aNbParsed = ViewerTest::ParseColor (theArgNb  - anArgIter - 1,
                                                           theArgVec + anArgIter + 1,
                                                           anArrowColorX);
      if (aNbParsed == 0)
      {
        std::cerr << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }
      anArgIter += aNbParsed;
    }
    else if (aFlag == "-colorarrowy")
    {
      Standard_Integer aNbParsed = ViewerTest::ParseColor (theArgNb  - anArgIter - 1,
                                                           theArgVec + anArgIter + 1,
                                                           anArrowColorY);
      if (aNbParsed == 0)
      {
        std::cerr << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }
      anArgIter += aNbParsed;
    }
    else if (aFlag == "-colorarrowz")
    {
      Standard_Integer aNbParsed = ViewerTest::ParseColor (theArgNb  - anArgIter - 1,
                                                           theArgVec + anArgIter + 1,
                                                           anArrowColorZ);
      if (aNbParsed == 0)
      {
        std::cerr << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }
      anArgIter += aNbParsed;
    }
    else
    {
      std::cerr << "Error: wrong syntax at '" << anArg << "'\n";
      return 1;
    }
  }

  aView->ZBufferTriedronSetup (anArrowColorX.Name(), anArrowColorY.Name(), anArrowColorZ.Name(),
                               aSizeRatio, anArrowDiam, aNbFacets);
  aView->TriedronDisplay (aPosition, aLabelsColor.Name(), aScale, aVisType);
  aView->ZFitAll();
  return 0;
}

//==============================================================================
//function : VRotate
//purpose  : Camera Rotating
//==============================================================================

static int VRotate (Draw_Interpretor& /*theDi*/, Standard_Integer theArgNb, const char** theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cout << "No active view!\n";
    return 1;
  }

  Standard_Boolean hasFlags = Standard_False;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg (theArgVec[anArgIter]);
    TCollection_AsciiString aFlag (anArg);
    aFlag.LowerCase();
    if (aFlag == "-mousestart"
     || aFlag == "-mousefrom")
    {
      hasFlags = Standard_True;
      if (anArgIter + 2 >= theArgNb)
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }

      Standard_Integer anX = Draw::Atoi (theArgVec[++anArgIter]);
      Standard_Integer anY = Draw::Atoi (theArgVec[++anArgIter]);
      aView->StartRotation (anX, anY);
    }
    else if (aFlag == "-mousemove")
    {
      hasFlags = Standard_True;
      if (anArgIter + 2 >= theArgNb)
      {
        std::cout << "Error: wrong syntax at '" << anArg << "'\n";
        return 1;
      }

      Standard_Integer anX = Draw::Atoi (theArgVec[++anArgIter]);
      Standard_Integer anY = Draw::Atoi (theArgVec[++anArgIter]);
      aView->Rotation (anX, anY);
    }
    else if (theArgNb != 4
          && theArgNb != 7)
    {
      std::cout << "Error: wrong syntax at '" << anArg << "'\n";
      return 1;
    }
  }

  if (hasFlags)
  {
    return 0;
  }
  else if (theArgNb == 4)
  {
    Standard_Real anAX = Draw::Atof (theArgVec[1]);
    Standard_Real anAY = Draw::Atof (theArgVec[2]);
    Standard_Real anAZ = Draw::Atof (theArgVec[3]);
    aView->Rotate (anAX, anAY, anAZ);
    return 0;
  }
  else if (theArgNb == 7)
  {
    Standard_Real anAX = Draw::Atof (theArgVec[1]);
    Standard_Real anAY = Draw::Atof (theArgVec[2]);
    Standard_Real anAZ = Draw::Atof (theArgVec[3]);

    Standard_Real anX = Draw::Atof (theArgVec[4]);
    Standard_Real anY = Draw::Atof (theArgVec[5]);
    Standard_Real anZ = Draw::Atof (theArgVec[6]);

    aView->Rotate (anAX, anAY, anAZ, anX, anY, anZ);
    return 0;
  }

  std::cout << "Error: Invalid number of arguments\n";
  return 1;
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
    Standard_Real coef = Draw::Atof(argv[1]);
    if ( coef <= 0.0 ) {
      di << argv[1] << "Invalid value" << "\n";
      return 1;
    }
    V3dView->SetZoom( Draw::Atof(argv[1]) );
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
    V3dView->Pan( Draw::Atoi(argv[1]), Draw::Atoi(argv[2]) );
    return 0;
  } else {
    di << argv[0] << " Invalid number of arguments" << "\n";
    return 1;
  }
}

//==============================================================================
//function : VPlace
//purpose  : Place the point (in pixels) at the center of the window
//==============================================================================
static int VPlace (Draw_Interpretor& /*theDi*/, Standard_Integer theArgNb, const char** theArgs)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << theArgs[0] << "Error: no active view." << std::endl;
    return 1;
  }

  if (theArgNb != 3)
  {
    std::cerr << theArgs[0] << "Error: invalid number of arguments." << std::endl;
    return 1;
  }

  aView->Place (Draw::Atoi (theArgs[1]), Draw::Atoi (theArgs[2]), aView->Scale());

  return 0;
}

//==============================================================================
//function : VExport
//purpose  : Export the view to a vector graphic format (PS, EMF, PDF)
//==============================================================================

static int VExport(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  if (V3dView.IsNull())
    return 1;

  if (argc == 1)
  {
    std::cout << "Usage: " << argv[0] << " Filename [Format]\n";
    return 1;
  }

  Graphic3d_ExportFormat anExpFormat = Graphic3d_EF_PDF;
  TCollection_AsciiString aFormatStr;

  TCollection_AsciiString aFileName (argv[1]);
  Standard_Integer aLen = aFileName.Length();

  if (argc > 2)
  {
    aFormatStr = TCollection_AsciiString (argv[2]);
  }
  else if (aLen >= 4)
  {
    if (aFileName.Value (aLen - 2) == '.')
    {
      aFormatStr = aFileName.ToCString() + aLen - 2;
    }
    else if (aFileName.Value (aLen - 3) == '.')
    {
      aFormatStr = aFileName.ToCString() + aLen - 3;
    }
    else
    {
      std::cout << "Export format couln't be detected from filename '" << argv[1] << "'\n";
      return 1;
    }
  }
  else
  {
    std::cout << "Export format couln't be detected from filename '" << argv[1] << "'\n";
    return 1;
  }

  aFormatStr.UpperCase();
  if (aFormatStr == "PS")
    anExpFormat = Graphic3d_EF_PostScript;
  else if (aFormatStr == "EPS")
    anExpFormat = Graphic3d_EF_EnhPostScript;
  else if (aFormatStr == "TEX")
    anExpFormat = Graphic3d_EF_TEX;
  else if (aFormatStr == "PDF")
    anExpFormat = Graphic3d_EF_PDF;
  else if (aFormatStr == "SVG")
    anExpFormat = Graphic3d_EF_SVG;
  else if (aFormatStr == "PGF")
    anExpFormat = Graphic3d_EF_PGF;
  else if (aFormatStr == "EMF")
    anExpFormat = Graphic3d_EF_EMF;
  else
  {
    std::cout << "Invalid export format '" << aFormatStr << "'\n";
    return 1;
  }

  try
  {
    if (!V3dView->Export (argv[1], anExpFormat))
    {
      di << "Error: export of image to " << aFormatStr << " failed!\n";
    }
  }
  catch (Standard_Failure)
  {
    di << "Error: export of image to " << aFormatStr << " failed";
    di << " (exception: " << Standard_Failure::Caught()->GetMessageString() << ")";
  }
  return 0;
}

//==============================================================================
//function : VColorScale
//purpose  : representation color scale
//==============================================================================

static Standard_Boolean checkColor (const TCollection_AsciiString& theRed,
                                    const TCollection_AsciiString& theGreen,
                                    const TCollection_AsciiString& theBlue,
                                                    Standard_Real& theRedValue,
                                                    Standard_Real& theGreenValue,
                                                    Standard_Real& theBlueValue)
{
  if (!theRed.IsRealValue()
   || !theGreen.IsRealValue()
   || !theBlue.IsRealValue())
  {
    std::cout << "Error: RGB color values should be real!\n";
    return Standard_True;
  }
  theRedValue = theRed    .RealValue();
  theGreenValue = theGreen.RealValue();
  theBlueValue = theBlue  .RealValue();
  if (theRedValue < 0.0 || theRedValue > 1.0
   || theGreenValue < 0.0 || theGreenValue > 1.0
   || theBlueValue < 0.0 || theBlueValue > 1.0)
  {
    std::cout << "Error: RGB color values should be within range 0..1!\n";
    return Standard_True;
  }
  return Standard_False;
}

static int VColorScale (Draw_Interpretor& theDI,
                        Standard_Integer  theArgNb,
                        const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  Handle(V3d_View)               aView    = ViewerTest::CurrentView();
  if (aContext.IsNull())
  {
    std::cout << "Error: no active view!\n";
    return 1;
  }

  Handle(AIS_ColorScale) aCS;
  // find object
  Handle(AIS_InteractiveObject) anIObj;
  if (GetMapOfAIS().IsBound2 (theArgVec[1]))
  {
    aCS = Handle(AIS_ColorScale)::DownCast (GetMapOfAIS().Find2 (theArgVec[1]));
    if (aCS.IsNull())
    {
      std::cout << "Error: object '" << theArgVec[1] << "'is already defined and is not a color scale!\n";
      return 1;
    }
  }
  else
  {
    aCS = new AIS_ColorScale();
    GetMapOfAIS().Bind (aCS,theArgVec[1]);
  }

  if (aCS->ZLayer() != Graphic3d_ZLayerId_TopOSD)
  {
    aCS->SetZLayer (Graphic3d_ZLayerId_TopOSD);
  }
  if (aCS->GetTransformPersistenceMode() != Graphic3d_TMF_2d)
  {
    aCS->SetTransformPersistence (Graphic3d_TMF_2d, gp_Pnt (-1,-1,0));
  }

  Standard_Real                   aMinRange    = aCS->GetMin();
  Standard_Real                   aMaxRange    = aCS->GetMax();
  Standard_Integer                aWidth       = aCS->GetWidth();
  Standard_Integer                aHeight      = aCS->GetHeight();
  Standard_Integer                aNbIntervals = aCS->GetNumberOfIntervals();
  Standard_Integer                aTextHeight  = aCS->GetTextHeight();
  Aspect_TypeOfColorScalePosition aLabPosition = aCS->GetLabelPosition();
  Standard_Integer                aPosX = aCS->GetXPosition();
  Standard_Integer                aPosY = aCS->GetYPosition();

  ViewerTest_AutoUpdater anUpdateTool (aContext, aView);

  if (theArgNb <= 1)
  {
    std::cout << "Error: wrong syntax at command '" << theArgVec[0] << "'!\n";
    return 1;
  }
  if (theArgNb <= 2)
  {
    theDI << "Color scale parameters for '"<< theArgVec[1] << "':\n"
          << "Min range: " << aMinRange << "\n"
          << "Max range: " << aMaxRange << "\n"
          << "Number of intervals: " << aNbIntervals << "\n"
          << "Text height: " << aTextHeight << "\n"
          << "Color scale position: " << aPosX <<" "<< aPosY<< "\n"
          << "Color scale title: " << aCS->GetTitle() << "\n"
          << "Label position: ";
    switch (aLabPosition)
    {
      case Aspect_TOCSP_NONE:
        theDI << "None\n";
        break;
      case Aspect_TOCSP_LEFT:
        theDI << "Left\n";
        break;
      case Aspect_TOCSP_RIGHT:
        theDI << "Right\n";
        break;
      case Aspect_TOCSP_CENTER:
        theDI << "Center\n";
        break;
    }
    return 0;
  }

  for (Standard_Integer anArgIter = 2; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg = theArgVec[anArgIter];
    TCollection_AsciiString aFlag (anArg);
    aFlag.LowerCase();
    if (anUpdateTool.parseRedrawMode (aFlag))
    {
      continue;
    }
    else if (aFlag == "-range")
    {
      if (anArgIter + 3 >= theArgNb)
      {
        std::cout << "Error: wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      TCollection_AsciiString anArg1 (theArgVec[++anArgIter]);
      TCollection_AsciiString anArg2 (theArgVec[++anArgIter]);
      TCollection_AsciiString anArg3 (theArgVec[++anArgIter]);
      if (!anArg1.IsRealValue())
      {
        std::cout << "Error: the minRange value should be real!\n";
        return 1;
      }
      else if (!anArg2.IsRealValue())
      {
        std::cout << "Error: the maxRange value should be real!\n";
        return 1;
      }
      else if (!anArg3.IsIntegerValue())
      {
        std::cout << "Error: the number of intervals should be integer!\n";
        return 1;
      }

      aMinRange    = anArg1.RealValue();
      aMaxRange    = anArg2.RealValue();
      aNbIntervals = anArg3.IntegerValue();
    }
    else if (aFlag == "-font")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        std::cout << "Error: wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }
      TCollection_AsciiString aFontArg(theArgVec[anArgIter + 1]);
      if (!aFontArg.IsIntegerValue())
      {
        std::cout << "Error: HeightFont value should be integer!\n";
        return 1;
      }

      aTextHeight = aFontArg.IntegerValue();
      anArgIter += 1;
    }
    else if (aFlag == "-textpos")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        std::cout << "Error: wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }
      TCollection_AsciiString aTextPosArg(theArgVec[++anArgIter]);
      aTextPosArg.LowerCase();
      if (aTextPosArg == "none")
      {
        aLabPosition = Aspect_TOCSP_NONE;
      }
      else if (aTextPosArg == "left")
      {
        aLabPosition = Aspect_TOCSP_LEFT;
      }
      else if (aTextPosArg == "right")
      {
        aLabPosition = Aspect_TOCSP_RIGHT;
      }
      else if (aTextPosArg == "center")
      {
        aLabPosition = Aspect_TOCSP_CENTER;
      }
      else
      {
        std::cout << "Error: unknown position '" << aTextPosArg << "'!\n";
        return 1;
      }
    }
    else if (aFlag == "-xy")
    {
      if (anArgIter + 2 >= theArgNb)
      {
        std::cout << "Error: wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      TCollection_AsciiString aX (theArgVec[++anArgIter]);
      TCollection_AsciiString aY (theArgVec[++anArgIter]);
      if (!aX.IsIntegerValue()
       || !aY.IsIntegerValue())
      {
        std::cout << "Error: coordinates should be integer values!\n";
        return 1;
      }

      aPosX = aX.IntegerValue();
      aPosY = aY.IntegerValue();
    }
    else if (aFlag == "-width"
          || aFlag == "-w")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        std::cout << "Error: wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      TCollection_AsciiString aW (theArgVec[++anArgIter]);
      if (!aW.IsIntegerValue())
      {
        std::cout << "Error: a width should be an integer value!\n";
        return 1;
      }

      aWidth = aW.IntegerValue();
    }
    else if (aFlag == "-height"
          || aFlag == "-h")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        std::cout << "Error: wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      TCollection_AsciiString aH (theArgVec[++anArgIter]);
      if (!aH.IsIntegerValue())
      {
        std::cout << "Error: a width should be an integer value!\n";
        return 1;
      }

      aHeight = aH.IntegerValue();
    }
    else if (aFlag == "-color")
    {
      if (aCS->GetColorType() != Aspect_TOCSD_USER)
      {
        std::cout << "Error: wrong color type! Call -colors before to set user-specified colors!\n";
        return 1;
      }

      Quantity_NameOfColor aColorName;
      if (anArgIter + 4 >= theArgNb)
      {
        if (anArgIter + 2 >= theArgNb)
        {
          std::cout << "Error: wrong syntax at argument '" << anArg << "'!\n";
          return 1;
        }
        else if (!Quantity_Color::ColorFromName (theArgVec[anArgIter + 2], aColorName))
        {
          std::cout << "Error: wrong color name: '" << theArgVec[anArgIter + 2] << "' !\n";
          return 1;
        }
      }

      TCollection_AsciiString anInd (theArgVec[anArgIter + 1]);
      if (!anInd.IsIntegerValue())
      {
        std::cout << "Error: Index value should be integer!\n";
        return 1;
      }

      Standard_Integer anIndex = anInd.IntegerValue();
      if (anIndex < 0
       || anIndex > aNbIntervals - 1)
      {
        std::cout << "Error: Index value should be within range 0..." << (aNbIntervals - 1) <<"!\n";
        return 1;
      }

      if (Quantity_Color::ColorFromName (theArgVec[anArgIter + 2], aColorName))
      {
        aCS->SetColor    (Quantity_Color (aColorName), anIndex);
        aCS->SetColorType (Aspect_TOCSD_USER);
        anArgIter += 2;
        continue;
      }

      TCollection_AsciiString aRed   (theArgVec[anArgIter + 2]);
      TCollection_AsciiString aGreen (theArgVec[anArgIter + 3]);
      TCollection_AsciiString aBlue  (theArgVec[anArgIter + 4]);
      Standard_Real aRedValue,aGreenValue, aBlueValue;
      if(checkColor (aRed, aGreen, aBlue, aRedValue, aGreenValue, aBlueValue))
      {
        return 1;
      }
      aCS->SetColor     (Quantity_Color (aRedValue, aGreenValue, aBlueValue, Quantity_TOC_RGB), anIndex);
      aCS->SetColorType (Aspect_TOCSD_USER);
      anArgIter += 4;
    }
    else if (aFlag == "-label")
    {
      if (aCS->GetColorType() != Aspect_TOCSD_USER)
      {
        std::cout << "Error: wrong label type! Call -labels before to set user-specified labels!\n";
        return 1;
      }
      else if (anArgIter + 2 >= theArgNb)
      {
        std::cout << "Error: wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      Standard_Integer anIndex = Draw::Atoi (theArgVec[anArgIter + 1]);
      if (anIndex < 0
       || anIndex > aNbIntervals)
      {
        std::cout << "Error: Index value should be within range 0..." << aNbIntervals <<"!\n";
        return 1;
      }

      TCollection_ExtendedString aText (theArgVec[anArgIter + 2]);
      aCS->SetLabel     (aText, anIndex);
      aCS->SetLabelType (Aspect_TOCSD_USER);
      anArgIter += 2;
    }
    else if (aFlag == "-colors")
    {
      Aspect_SequenceOfColor aSeq;
      if (anArgIter + aNbIntervals + 1 > theArgNb)
      {
        std::cout << "Error: not enough arguments! You should provide color names or RGB color values for every interval of the "
                  << aNbIntervals << " intervals\n";
        return 1;
      }

      Standard_Integer aColorIter = anArgIter + 1;
      while (aColorIter < theArgNb)
      {
        if (theArgVec[aColorIter][0] == '-')
        {
          break;
        }

        else if (theArgVec[aColorIter][0] >= 97
              && theArgVec[aColorIter][0] <= 122)
        {
          Quantity_NameOfColor aColorName;
          if (!Quantity_Color::ColorFromName (theArgVec[aColorIter], aColorName))
          {
            std::cout << "Error: wrong color name: " << theArgVec[aColorIter] << " !\n";
            return 1;
          }
          aSeq.Append (Quantity_Color (aColorName));
          aColorIter++;
          anArgIter++;
        }
        else
        {
          TCollection_AsciiString aRed   (theArgVec[aColorIter]);
          TCollection_AsciiString aGreen (theArgVec[aColorIter + 1]);
          TCollection_AsciiString aBlue  (theArgVec[aColorIter + 2]);
          Standard_Real aRedValue,aGreenValue, aBlueValue;
          if (checkColor (aRed, aGreen, aBlue, aRedValue, aGreenValue, aBlueValue))
          {
            return 1;
          }
          aSeq.Append (Quantity_Color (aRedValue, aGreenValue, aBlueValue, Quantity_TOC_RGB));
          aColorIter += 3;
          anArgIter += 3;
        }
      }
      if (aSeq.Length() < aNbIntervals)
      {
        std::cout << "Error: not enough arguments! You should provide color names or RGB color values for every interval of the "
                  << aNbIntervals << " intervals\n";
        return 1;
      }

      aCS->SetColors    (aSeq);
      aCS->SetColorType (Aspect_TOCSD_USER);
    }
    else if (aFlag == "-labels")
    {
      if (anArgIter + aNbIntervals + 1 >= theArgNb)
      {
        std::cout << "Error: not enough arguments! You should provide " << (aNbIntervals + 1)
                  << " text labels for " << aNbIntervals << " intervals.\n";
        return 1;
      }

      TColStd_SequenceOfExtendedString aSeq;
      for (int aLabelIter = anArgIter + 1; aLabelIter <= anArgIter + aNbIntervals + 1; aLabelIter += 1)
      {
        aSeq.Append (TCollection_ExtendedString (theArgVec[aLabelIter]));
      }
      aCS->SetLabels (aSeq);
      aCS->SetLabelType (Aspect_TOCSD_USER);
      anArgIter += aSeq.Length();
    }
    else if (aFlag == "-title")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        std::cout << "Error: wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      Standard_Boolean isTwoArgs = Standard_False;
      if (anArgIter + 2 < theArgNb)
      {
        TCollection_AsciiString aSecondArg (theArgVec[anArgIter + 2]);
        aSecondArg.LowerCase();
        if (aSecondArg == "none")
        {
          aCS->SetTitlePosition (Aspect_TOCSP_NONE);
          isTwoArgs = Standard_True;
        }
        else if (aSecondArg == "left")
        {
          aCS->SetTitlePosition (Aspect_TOCSP_LEFT);
          isTwoArgs = Standard_True;
        }
        else if (aSecondArg == "right")
        {
          aCS->SetTitlePosition (Aspect_TOCSP_RIGHT);
          isTwoArgs = Standard_True;
        }
        else if (aSecondArg == "center")
        {
          aCS->SetTitlePosition (Aspect_TOCSP_CENTER);
          isTwoArgs = Standard_True;
        }
      }

      aCS->SetTitle (theArgVec[anArgIter + 1]);
      if (isTwoArgs)
      {
        anArgIter += 1;
      }
      anArgIter += 1;
    }
    else if (aFlag == "-demoversion"
          || aFlag == "-demo")
    {
      aPosX        = 0;
      aPosY        = 0;
      aTextHeight  = 16;
      aMinRange    = 0.0;
      aMaxRange    = 100;
      aNbIntervals = 10;
      aWidth       = 0;
      aHeight      = 0;
      aLabPosition = Aspect_TOCSP_RIGHT;
      aCS->SetColorType (Aspect_TOCSD_AUTO);
      aCS->SetLabelType (Aspect_TOCSD_AUTO);
    }
    else
    {
      std::cout << "Error: wrong syntax at " << anArg << " - unknown argument!\n";
      return 1;
    }
  }
  if (!aWidth || !aHeight)
  {
    Standard_Integer aWinWidth, aWinHeight;
    aView->Window()->Size (aWinWidth, aWinHeight);
    if (!aWidth)
    {
      aWidth = aWinWidth;
    }
    if (!aHeight)
    {
      aHeight = aWinHeight;
    }
  }
  aCS->SetSize              (aWidth, aHeight);
  aCS->SetPosition          (aPosX, aPosY);
  aCS->SetTextHeight        (aTextHeight);
  aCS->SetRange             (aMinRange, aMaxRange);
  aCS->SetNumberOfIntervals (aNbIntervals);
  aCS->SetLabelPosition     (aLabPosition);
  aCS->SetBGColor           (aView->BackgroundColor());
  aCS->SetToUpdate();
  aContext->Display (aCS);

  return 0;
}

//==============================================================================
//function : VGraduatedTrihedron
//purpose  : Displays or hides a graduated trihedron
//==============================================================================
static Standard_Boolean GetColor (const TCollection_AsciiString& theValue,
                                  Quantity_Color& theColor)
{
  Quantity_NameOfColor aColorName;
  TCollection_AsciiString aVal = theValue;
  aVal.UpperCase();
  if (!Quantity_Color::ColorFromName (aVal.ToCString(), aColorName))
  {
    return Standard_False;
  }
  theColor = Quantity_Color (aColorName);
  return Standard_True;
}

static int VGraduatedTrihedron (Draw_Interpretor& /*theDi*/, Standard_Integer theArgNum, const char** theArgs)
{
  if (theArgNum < 2)
  {
    std::cout << theArgs[0] << " error: wrong number of parameters. Type 'help"
              << theArgs[0] <<"' for more information.\n";
    return 1;  //TCL_ERROR
  }

  NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HSequenceOfAsciiString)> aMapOfArgs;
  TCollection_AsciiString aParseKey;
  for (Standard_Integer anArgIt = 1; anArgIt < theArgNum; ++anArgIt)
  {
    TCollection_AsciiString anArg (theArgs [anArgIt]);

    if (anArg.Value (1) == '-' && !anArg.IsRealValue())
    {
      aParseKey = anArg;
      aParseKey.Remove (1);
      aParseKey.LowerCase();
      aMapOfArgs.Bind (aParseKey, new TColStd_HSequenceOfAsciiString);
      continue;
    }

    if (aParseKey.IsEmpty())
    {
      continue;
    }

    aMapOfArgs(aParseKey)->Append (anArg);
  }

  // Check parameters
  for (NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HSequenceOfAsciiString)>::Iterator aMapIt (aMapOfArgs);
       aMapIt.More(); aMapIt.Next())
  {
    const TCollection_AsciiString& aKey = aMapIt.Key();
    const Handle(TColStd_HSequenceOfAsciiString)& anArgs = aMapIt.Value();

    // Bool key, without arguments
    if ((aKey.IsEqual ("on") || aKey.IsEqual ("off"))
        && anArgs->IsEmpty())
    {
      continue;
    }

    // One argument
    if ( (aKey.IsEqual ("xname") || aKey.IsEqual ("yname") || aKey.IsEqual ("zname"))
          && anArgs->Length() == 1)
    {
      continue;
    }

    // On/off arguments
    if ((aKey.IsEqual ("xdrawname") || aKey.IsEqual ("ydrawname") || aKey.IsEqual ("zdrawname")
        || aKey.IsEqual ("xdrawticks") || aKey.IsEqual ("ydrawticks") || aKey.IsEqual ("zdrawticks")
        || aKey.IsEqual ("xdrawvalues") || aKey.IsEqual ("ydrawvalues") || aKey.IsEqual ("zdrawvalues")
        || aKey.IsEqual ("drawgrid") || aKey.IsEqual ("drawaxes"))
        && anArgs->Length() == 1 && (anArgs->Value(1).IsEqual ("on") || anArgs->Value(1).IsEqual ("off")))
    {
      continue;
    }

    // One string argument
    if ( (aKey.IsEqual ("xnamecolor") || aKey.IsEqual ("ynamecolor") || aKey.IsEqual ("znamecolor")
          || aKey.IsEqual ("xcolor") || aKey.IsEqual ("ycolor") || aKey.IsEqual ("zcolor"))
          && anArgs->Length() == 1 && !anArgs->Value(1).IsIntegerValue() && !anArgs->Value(1).IsRealValue())
    {
      continue;
    }

    // One integer argument
    if ( (aKey.IsEqual ("xticks") || aKey.IsEqual ("yticks") || aKey.IsEqual ("zticks")
          || aKey.IsEqual ("xticklength") || aKey.IsEqual ("yticklength") || aKey.IsEqual ("zticklength")
          || aKey.IsEqual ("xnameoffset") || aKey.IsEqual ("ynameoffset") || aKey.IsEqual ("znameoffset")
          || aKey.IsEqual ("xvaluesoffset") || aKey.IsEqual ("yvaluesoffset") || aKey.IsEqual ("zvaluesoffset"))
         && anArgs->Length() == 1 && anArgs->Value(1).IsIntegerValue())
    {
      continue;
    }

    // One real argument
    if ( aKey.IsEqual ("arrowlength")
         && anArgs->Length() == 1 && (anArgs->Value(1).IsIntegerValue() || anArgs->Value(1).IsRealValue()))
    {
      continue;
    }

    // Two string arguments
    if ( (aKey.IsEqual ("namefont") || aKey.IsEqual ("valuesfont"))
         && anArgs->Length() == 1 && !anArgs->Value(1).IsIntegerValue() && !anArgs->Value(1).IsRealValue())
    {
      continue;
    }

    TCollection_AsciiString aLowerKey;
    aLowerKey  = "-";
    aLowerKey += aKey;
    aLowerKey.LowerCase();
    std::cout << theArgs[0] << ": " << aLowerKey << " is unknown option, or the arguments are unacceptable.\n";
    std::cout << "Type help for more information.\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  if (anAISContext.IsNull())
  {
    std::cout << theArgs[0] << ": " << " please use 'vinit' command to initialize view.\n";
    return 1;
  }

  Standard_Boolean toDisplay = Standard_True;
  Quantity_Color aColor;
  Graphic3d_GraduatedTrihedron aTrihedronData;
  // Process parameters
  Handle(TColStd_HSequenceOfAsciiString) aValues;
  if (aMapOfArgs.Find ("off", aValues))
  {
    toDisplay = Standard_False;
  }

  // AXES NAMES
  if (aMapOfArgs.Find ("xname", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetName (aValues->Value(1));
  }
  if (aMapOfArgs.Find ("yname", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetName (aValues->Value(1));
  }
  if (aMapOfArgs.Find ("zname", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetName (aValues->Value(1));
  }
  if (aMapOfArgs.Find ("xdrawname", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetDrawName (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("ydrawname", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetDrawName (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("zdrawname", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetDrawName (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("xnameoffset", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetNameOffset (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("ynameoffset", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetNameOffset (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("znameoffset", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetNameOffset (aValues->Value(1).IntegerValue());
  }

  // COLORS
  if (aMapOfArgs.Find ("xnamecolor", aValues))
  {
    if (!GetColor (aValues->Value(1), aColor))
    {
      std::cout << theArgs[0] << "error: -xnamecolor wrong color name.\n";
      return 1;
    }
    aTrihedronData.ChangeXAxisAspect().SetNameColor (aColor);
  }
  if (aMapOfArgs.Find ("ynamecolor", aValues))
  {
    if (!GetColor (aValues->Value(1), aColor))
    {
      std::cout << theArgs[0] << "error: -ynamecolor wrong color name.\n";
      return 1;
    }
    aTrihedronData.ChangeYAxisAspect().SetNameColor (aColor);
  }
  if (aMapOfArgs.Find ("znamecolor", aValues))
  {
    if (!GetColor (aValues->Value(1), aColor))
    {
      std::cout << theArgs[0] << "error: -znamecolor wrong color name.\n";
      return 1;
    }
    aTrihedronData.ChangeZAxisAspect().SetNameColor (aColor);
  }
  if (aMapOfArgs.Find ("xcolor", aValues))
  {
    if (!GetColor (aValues->Value(1), aColor))
    {
      std::cout << theArgs[0] << "error: -xcolor wrong color name.\n";
      return 1;
    }
    aTrihedronData.ChangeXAxisAspect().SetColor (aColor);
  }
  if (aMapOfArgs.Find ("ycolor", aValues))
  {
    if (!GetColor (aValues->Value(1), aColor))
    {
      std::cout << theArgs[0] << "error: -ycolor wrong color name.\n";
      return 1;
    }
    aTrihedronData.ChangeYAxisAspect().SetColor (aColor);
  }
  if (aMapOfArgs.Find ("zcolor", aValues))
  {
    if (!GetColor (aValues->Value(1), aColor))
    {
      std::cout << theArgs[0] << "error: -zcolor wrong color name.\n";
      return 1;
    }
    aTrihedronData.ChangeZAxisAspect().SetColor (aColor);
  }

  // TICKMARKS
  if (aMapOfArgs.Find ("xticks", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetTickmarksNumber (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("yticks", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetTickmarksNumber (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("zticks", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetTickmarksNumber (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("xticklength", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetTickmarksLength (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("yticklength", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetTickmarksLength (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("zticklength", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetTickmarksLength (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("xdrawticks", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetDrawTickmarks (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("ydrawticks", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetDrawTickmarks (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("zdrawticks", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetDrawTickmarks (aValues->Value(1).IsEqual ("on"));
  }

  // VALUES
  if (aMapOfArgs.Find ("xdrawvalues", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetDrawValues (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("ydrawvalues", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetDrawValues (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("zdrawvalues", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetDrawValues (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("xvaluesoffset", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetValuesOffset (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("yvaluesoffset", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetValuesOffset (aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find ("zvaluesoffset", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetValuesOffset (aValues->Value(1).IntegerValue());
  }

  // ARROWS
  if (aMapOfArgs.Find ("arrowlength", aValues))
  {
    aTrihedronData.SetArrowsLength ((Standard_ShortReal) aValues->Value(1).RealValue());
  }

  // FONTS
  if (aMapOfArgs.Find ("namefont", aValues))
  {
    aTrihedronData.SetNamesFont (aValues->Value(1));
  }
  if (aMapOfArgs.Find ("valuesfont", aValues))
  {
    aTrihedronData.SetValuesFont (aValues->Value(1));
  }

  if (aMapOfArgs.Find ("drawgrid", aValues))
  {
    aTrihedronData.SetDrawGrid (aValues->Value(1).IsEqual ("on"));
  }
  if (aMapOfArgs.Find ("drawaxes", aValues))
  {
    aTrihedronData.SetDrawAxes (aValues->Value(1).IsEqual ("on"));
  }

  // The final step: display of erase trihedron
  if (toDisplay)
  {
    ViewerTest::CurrentView()->GraduatedTrihedronDisplay (aTrihedronData);
  }
  else
  {
    ViewerTest::CurrentView()->GraduatedTrihedronErase();
  }

  ViewerTest::GetAISContext()->UpdateCurrentViewer();
  ViewerTest::CurrentView()->Redraw();

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
#ifndef _WIN32
  di << "Printing implemented only for WNT!\n";
  return 0;
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
    di << " width height filename [print algo=0] [tile_width tile_height]\n";
    di << "width, height of the intermediate buffer for operation\n";
    di << "algo : {0|1}\n";
    di << "        0 - stretch algorithm\n";
    di << "        1 - tile algorithm\n";
    di << "test printing algorithms into an intermediate buffer\n";
    di << "using specific tile size if provided\n";
    di << "with saving output to an image file\n";
    return 1;
  }

  // get the input params
  Standard_Integer aWidth  = Draw::Atoi (argv[1]);
  Standard_Integer aHeight = Draw::Atoi (argv[2]);
  Standard_Integer aMode   = 0;
  TCollection_AsciiString aFileName = TCollection_AsciiString (argv[3]);
  if (argc >= 5)
    aMode = Draw::Atoi (argv[4]);

  Standard_Integer aTileWidth  = 0;
  Standard_Integer aTileHeight = 0;
  Standard_Boolean isTileSizeProvided = Standard_False;
  if (argc == 7)
  {
    isTileSizeProvided = Standard_True;
    aTileWidth  = Draw::Atoi (argv[5]);
    aTileHeight = Draw::Atoi (argv[6]);
  }

  // check the input parameters
  if (aWidth <= 0 || aHeight <= 0)
  {
    di << "Width and height must be positive values!\n";
    return 1;
  }
  if (aMode != 0 && aMode != 1)
    aMode = 0;

  // define compatible bitmap
  HDC anDC = CreateCompatibleDC(0);
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
  void* aBitsOut = NULL;
  HBITMAP aMemoryBitmap = CreateDIBSection (anDC, &aBitmapData, DIB_RGB_COLORS,
                                            &aBitsOut, NULL, 0);
  HGDIOBJ anOldBitmap   = SelectObject(anDC, aMemoryBitmap);

  Standard_Boolean isSaved = Standard_False, isPrinted = Standard_False;
  if (aBitsOut != NULL)
  {
    if (aMode == 0)
      isPrinted = aView->Print(anDC,1,1,0,Aspect_PA_STRETCH);
    else
    {
      if (isTileSizeProvided)
      {
        Handle(Graphic3d_CView) aGraphicView = ViewerTest::CurrentView()->View();
        Graphic3d_PtrFrameBuffer anOldBuffer = aGraphicView->FBO();
        Graphic3d_PtrFrameBuffer aNewBuffer  = aGraphicView->FBOCreate (aTileWidth, aTileHeight);
        aGraphicView->SetFBO (aNewBuffer);

        isPrinted = aView->Print (anDC, 1, 1, 0, Aspect_PA_TILE);

        aGraphicView->FBORelease (aNewBuffer);
        aGraphicView->SetFBO (anOldBuffer);
      }
      else
      {
        isPrinted = aView->Print (anDC, 1, 1, 0, Aspect_PA_TILE);
      }
    }

    // successfully printed into an intermediate buffer
    if (isPrinted)
    {
      Image_PixMap aWrapper;
      aWrapper.InitWrapper (Image_PixMap::ImgBGR, (Standard_Byte* )aBitsOut, aWidth, aHeight, aWidth * 3 + aWidth % 4);
      aWrapper.SetTopDown (false);

      Image_AlienPixMap anImageBitmap;
      anImageBitmap.InitCopy (aWrapper);
      isSaved = anImageBitmap.Save (aFileName);
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

//==============================================================================
//function : VZLayer
//purpose  : Test z layer operations for v3d viewer
//==============================================================================
static int VZLayer (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext ();
  if (aContextAIS.IsNull())
  {
    di << "Call vinit before!\n";
    return 1;
  }
  else if (argc < 2)
  {
    di << "Use: vzlayer ";
    di << " add/del/get/settings/enable/disable [id]\n";
    di << " add - add new z layer to viewer and print its id\n";
    di << " del - del z layer by its id\n";
    di << " get - print sequence of z layers in increasing order of their overlay level\n";
    di << " settings - print status of z layer settings\n";
    di << " enable ([depth]test/[depth]write/[depth]clear/[depth]offset) \n    enables given setting for the z layer\n";
    di << " enable (p[ositive]offset/n[egative]offset) \n    enables given setting for the z layer\n";
    di << " disable ([depth]test/[depth]write/[depth]clear/[depth]offset) \n    disables given setting for the z layer\n";
    di << "\nWhere id is the layer identificator\n";
    di << "\nExamples:\n";
    di << "   vzlayer add\n";
    di << "   vzlayer enable poffset 1\n";
    di << "   vzlayer disable depthtest 1\n";
    di << "   vzlayer del 1\n";
    return 1;
  }

  const Handle(V3d_Viewer)& aViewer = aContextAIS->CurrentViewer();
  if (aViewer.IsNull())
  {
    di << "No active viewer!\n";
    return 1;
  }

  // perform operation
  TCollection_AsciiString anOp = TCollection_AsciiString (argv[1]);
  if (anOp == "add")
  {
    Standard_Integer aNewId;
    if (!aViewer->AddZLayer (aNewId))
    {
      di << "Impossible to add new z layer!\n";
      return 1;
    }

    di << "New z layer added with index: " << aNewId << "\n";
  }
  else if (anOp == "del")
  {
    if (argc < 3)
    {
      di << "Please also provide as argument id of z layer to remove\n";
      return 1;
    }

    Standard_Integer aDelId = Draw::Atoi (argv[2]);
    if (!aViewer->RemoveZLayer (aDelId))
    {
      di << "Impossible to remove the z layer or invalid id!\n";
      return 1;
    }

    for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anObjIter (GetMapOfAIS());
         anObjIter.More(); anObjIter.Next())
    {
      Handle(PrsMgr_PresentableObject) aPrs = Handle(PrsMgr_PresentableObject)::DownCast (anObjIter.Key1());
      if (aPrs.IsNull()
       || aPrs->ZLayer() != aDelId)
      {
        continue;
      }
      aPrs->SetZLayer (Graphic3d_ZLayerId_Default);
    }

    di << "Z layer " << aDelId << " has been removed\n";
  }
  else if (anOp == "get")
  {
    TColStd_SequenceOfInteger anIds;
    aViewer->GetAllZLayers (anIds);
    for (Standard_Integer aSeqIdx = 1; aSeqIdx <= anIds.Length(); aSeqIdx++)
    {
      di << anIds.Value (aSeqIdx) << " ";
    }

    di << "\n";
  }
  else if (anOp == "settings")
  {
    if (argc < 3)
    {
      di << "Please also provide an id\n";
      return 1;
    }

    Standard_Integer anId = Draw::Atoi (argv[2]);
    Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings (anId);

    di << "Depth test - " << (aSettings.IsSettingEnabled (Graphic3d_ZLayerDepthTest) ? "enabled" : "disabled") << "\n";
    di << "Depth write - " << (aSettings.IsSettingEnabled (Graphic3d_ZLayerDepthWrite) ? "enabled" : "disabled") << "\n";
    di << "Depth buffer clearing - " << (aSettings.IsSettingEnabled (Graphic3d_ZLayerDepthClear) ? "enabled" : "disabled") << "\n";
    di << "Depth offset - " << (aSettings.IsSettingEnabled (Graphic3d_ZLayerDepthOffset) ? "enabled" : "disabled") << "\n";

  }
  else if (anOp == "enable")
  {
    if (argc < 3)
    {
      di << "Please also provide an option to enable\n";
      return 1;
    }

    if (argc < 4)
    {
      di << "Please also provide a layer id\n";
      return 1;
    }

    TCollection_AsciiString aSubOp = TCollection_AsciiString (argv[2]);
    Standard_Integer anId = Draw::Atoi (argv[3]);
    Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings (anId);

    if (aSubOp == "depthtest" || aSubOp == "test")
    {
      aSettings.EnableSetting (Graphic3d_ZLayerDepthTest);
    }
    else if (aSubOp == "depthwrite" || aSubOp == "write")
    {
      aSettings.EnableSetting (Graphic3d_ZLayerDepthWrite);
    }
    else if (aSubOp == "depthclear" || aSubOp == "clear")
    {
      aSettings.EnableSetting (Graphic3d_ZLayerDepthClear);
    }
    else if (aSubOp == "depthoffset" || aSubOp == "offset")
    {
      if (argc < 6)
      {
        di << "Please also provide a factor and units values for depth offset\n";
        di << "Format is: vzlayer enable offset [factor] [units] [layerId]\n";
        return 1;
      }

      Standard_ShortReal aFactor = static_cast<Standard_ShortReal> (Draw::Atof (argv[3]));
      Standard_ShortReal aUnits  = static_cast<Standard_ShortReal> (Draw::Atof (argv[4]));
      anId = Draw::Atoi (argv[5]);
      aSettings = aViewer->ZLayerSettings (anId);

      aSettings.DepthOffsetFactor = aFactor;
      aSettings.DepthOffsetUnits  = aUnits;

      aSettings.EnableSetting (Graphic3d_ZLayerDepthOffset);
    }
    else if (aSubOp == "positiveoffset" || aSubOp == "poffset")
    {
      aSettings.SetDepthOffsetPositive();
    }
    else if (aSubOp == "negativeoffset" || aSubOp == "noffset")
    {
      aSettings.SetDepthOffsetNegative();
    }

    aViewer->SetZLayerSettings (anId, aSettings);
  }
  else if (anOp == "disable")
  {
    if (argc < 3)
    {
      di << "Please also provide an option to disable\n";
      return 1;
    }

    if (argc < 4)
    {
      di << "Please also provide a layer id\n";
      return 1;
    }

    TCollection_AsciiString aSubOp = TCollection_AsciiString (argv[2]);
    Standard_Integer anId = Draw::Atoi (argv[3]);
    Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings (anId);

    if (aSubOp == "depthtest" || aSubOp == "test")
    {
      aSettings.DisableSetting (Graphic3d_ZLayerDepthTest);
    }
    else if (aSubOp == "depthwrite" || aSubOp == "write")
    {
      aSettings.DisableSetting (Graphic3d_ZLayerDepthWrite);
    }
    else if (aSubOp == "depthclear" || aSubOp == "clear")
    {
      aSettings.DisableSetting (Graphic3d_ZLayerDepthClear);
    }
    else if (aSubOp == "depthoffset" || aSubOp == "offset")
    {
      aSettings.DisableSetting (Graphic3d_ZLayerDepthOffset);
    }

    aViewer->SetZLayerSettings (anId, aSettings);
  }
  else
  {
    di << "Invalid operation, please use { add / del / get / settings / enable / disable}\n";
    return 1;
  }

  return 0;
}

// The interactive presentation of 2d layer item
// for "vlayerline" command it provides a presentation of
// line with user-defined linewidth, linetype and transparency.
class V3d_LineItem : public AIS_InteractiveObject
{
public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI(V3d_LineItem, AIS_InteractiveObject)

  // constructor
  Standard_EXPORT V3d_LineItem(Standard_Real X1, Standard_Real Y1,
                               Standard_Real X2, Standard_Real Y2,
                               Aspect_TypeOfLine theType = Aspect_TOL_SOLID,
                               Standard_Real theWidth    = 0.5,
                               Standard_Real theTransp   = 1.0);

  private:

  void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                const Handle(Prs3d_Presentation)& thePresentation,
                const Standard_Integer theMode);

  void ComputeSelection (const Handle(SelectMgr_Selection)& /*aSelection*/,
                         const Standard_Integer /*aMode*/){};

private:

  Standard_Real       myX1, myY1, myX2, myY2;
  Aspect_TypeOfLine   myType;
  Standard_Real       myWidth;
};

// default constructor for line item
V3d_LineItem::V3d_LineItem(Standard_Real X1, Standard_Real Y1,
                           Standard_Real X2, Standard_Real Y2,
                           Aspect_TypeOfLine theType,
                           Standard_Real theWidth,
                           Standard_Real theTransp) :
  myX1(X1), myY1(Y1), myX2(X2), myY2(Y2),
  myType(theType), myWidth(theWidth)
{
  SetTransparency (1-theTransp);
}

// render line
void V3d_LineItem::Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePresentationManager*/,
                            const Handle(Prs3d_Presentation)& thePresentation,
                            const Standard_Integer /*theMode*/)
{
  thePresentation->Clear();
  Quantity_Color aColor (1.0, 0, 0, Quantity_TOC_RGB);
  Standard_Integer aWidth, aHeight;
  ViewerTest::CurrentView()->Window()->Size (aWidth, aHeight);
  Handle (Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (thePresentation);
  Handle(Graphic3d_ArrayOfPolylines) aPrim = new Graphic3d_ArrayOfPolylines(5);
  aPrim->AddVertex(myX1, aHeight-myY1, 0.);
  aPrim->AddVertex(myX2, aHeight-myY2, 0.);
  Handle(Prs3d_LineAspect) anAspect = new Prs3d_LineAspect (aColor, (Aspect_TypeOfLine)myType, myWidth);
  aGroup->SetPrimitivesAspect (anAspect->Aspect());
  aGroup->AddPrimitiveArray (aPrim);
}

//=============================================================================
//function : VLayerLine
//purpose  : Draws line in the v3d view layer with given attributes: linetype,
//         : linewidth, transparency coefficient
//============================================================================
static int VLayerLine(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    di << "Call vinit before!\n";
    return 1;
  }
  else if (argc < 5)
  {
    di << "Use: " << argv[0];
    di << " x1 y1 x2 y2 [linewidth = 0.5] [linetype = 0] [transparency = 1]\n";
    di << " linetype : { 0 | 1 | 2 | 3 } \n";
    di << "              0 - solid  \n";
    di << "              1 - dashed \n";
    di << "              2 - dot    \n";
    di << "              3 - dashdot\n";
    di << " transparency : { 0.0 - 1.0 } \n";
    di << "                  0.0 - transparent\n";
    di << "                  1.0 - visible    \n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  // get the input params
  Standard_Real X1 = Draw::Atof(argv[1]);
  Standard_Real Y1 = Draw::Atof(argv[2]);
  Standard_Real X2 = Draw::Atof(argv[3]);
  Standard_Real Y2 = Draw::Atof(argv[4]);

  Standard_Real    aWidth = 0.5;
  Standard_Integer aType  = 0;
  Standard_Real    aTransparency = 1.0;

  // has width
  if (argc > 5)
    aWidth = Draw::Atof(argv[5]);

  // has type
  if (argc > 6)
     aType = (Standard_Integer) Draw::Atoi(argv[6]);

  // has transparency
  if (argc > 7)
  {
    aTransparency = Draw::Atof(argv[7]);
    if (aTransparency < 0 || aTransparency > 1.0)
      aTransparency = 1.0;
  }

  // select appropriate line type
  Aspect_TypeOfLine aLineType;
  switch (aType)
  {
    case 1:
      aLineType = Aspect_TOL_DASH;
    break;

    case 2:
      aLineType = Aspect_TOL_DOT;
    break;

    case 3:
      aLineType = Aspect_TOL_DOTDASH;
    break;

    default:
      aLineType = Aspect_TOL_SOLID;
  }

  static Handle (V3d_LineItem) aLine;
  if (!aLine.IsNull())
  {
    aContext->Erase (aLine);
  }
  aLine = new V3d_LineItem (X1, Y1, X2, Y2,
                            aLineType, aWidth,
                            aTransparency);

  aLine->SetTransformPersistence (Graphic3d_TMF_2d,gp_Pnt(-1,-1,0));
  aLine->SetZLayer (Graphic3d_ZLayerId_TopOSD);
  aLine->SetToUpdate();
  aContext->Display (aLine, Standard_True);

  return 0;
}


//==============================================================================
//function : VGrid
//purpose  :
//==============================================================================

static int VGrid (Draw_Interpretor& /*theDI*/,
                  Standard_Integer  theArgNb,
                  const char**      theArgVec)
{
  // get the active view
  Handle(V3d_View)   aView   = ViewerTest::CurrentView();
  Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext();
  if (aView.IsNull() || aViewer.IsNull())
  {
    std::cerr << "No active view. Please call vinit.\n";
    return 1;
  }

  Aspect_GridType     aType = aViewer->GridType();
  Aspect_GridDrawMode aMode = aViewer->GridDrawMode();

  Standard_Integer anIter = 1;
  for (; anIter < theArgNb; ++anIter)
  {
    const char* aValue = theArgVec[anIter];
    if (*aValue == 'r')
    {
      aType = Aspect_GT_Rectangular;
    }
    else if (*aValue == 'c')
    {
      aType = Aspect_GT_Circular;
    }
    else if (*aValue == 'l')
    {
      aMode = Aspect_GDM_Lines;
    }
    else if (*aValue == 'p')
    {
      aMode = Aspect_GDM_Points;
    }
    else if (strcmp (aValue, "off" ) == 0)
    {
      aViewer->DeactivateGrid();
      return 0;
    }
    else
    {
      break;
    }
  }

  Standard_Integer aTail = (theArgNb - anIter);
  if (aTail == 0)
  {
    aViewer->ActivateGrid (aType, aMode);
    return 0;
  }
  else if (aTail != 2 && aTail != 5)
  {
    std::cerr << "Incorrect arguments number! Usage:\n"
              << "vgrid [off] [Mode={r|c}] [Type={l|p}] [OriginX OriginY [StepX/StepRadius StepY/DivNb RotAngle]]\n";
    return 1;
  }

  Quantity_Length anOriginX, anOriginY;
  Quantity_PlaneAngle aRotAngle;
  if (aType == Aspect_GT_Rectangular)
  {
    Quantity_Length aRStepX, aRStepY;
    aViewer->RectangularGridValues (anOriginX, anOriginY, aRStepX, aRStepY, aRotAngle);

    anOriginX = Draw::Atof (theArgVec[anIter++]);
    anOriginY = Draw::Atof (theArgVec[anIter++]);
    if (aTail == 5)
    {
      aRStepX   = Draw::Atof (theArgVec[anIter++]);
      aRStepY   = Draw::Atof (theArgVec[anIter++]);
      aRotAngle = Draw::Atof (theArgVec[anIter++]);
    }
    aViewer->SetRectangularGridValues (anOriginX, anOriginY, aRStepX, aRStepY, aRotAngle);
    aViewer->ActivateGrid (aType, aMode);
  }
  else if (aType == Aspect_GT_Circular)
  {
    Quantity_Length aRadiusStep;
    Standard_Integer aDivisionNumber;
    aViewer->CircularGridValues (anOriginX, anOriginY, aRadiusStep, aDivisionNumber, aRotAngle);

    anOriginX = Draw::Atof (theArgVec[anIter++]);
    anOriginY = Draw::Atof (theArgVec[anIter++]);
    if (aTail == 5)
    {
      aRadiusStep     = Draw::Atof (theArgVec[anIter++]);
      aDivisionNumber = Draw::Atoi (theArgVec[anIter++]);
      aRotAngle       = Draw::Atof (theArgVec[anIter++]);
    }

    aViewer->SetCircularGridValues (anOriginX, anOriginY, aRadiusStep, aDivisionNumber, aRotAngle);
    aViewer->ActivateGrid (aType, aMode);
  }

  return 0;
}

//==============================================================================
//function : VPriviledgedPlane
//purpose  :
//==============================================================================

static int VPriviledgedPlane (Draw_Interpretor& theDI,
                              Standard_Integer  theArgNb,
                              const char**      theArgVec)
{
  if (theArgNb != 1 && theArgNb != 7 && theArgNb != 10)
  {
    std::cerr << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  // get the active viewer
  Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext();
  if (aViewer.IsNull())
  {
    std::cerr << "Error: no active viewer. Please call vinit.\n";
    return 1;
  }

  if (theArgNb == 1)
  {
    gp_Ax3 aPriviledgedPlane = aViewer->PrivilegedPlane();
    const gp_Pnt& anOrig = aPriviledgedPlane.Location();
    const gp_Dir& aNorm = aPriviledgedPlane.Direction();
    const gp_Dir& aXDir = aPriviledgedPlane.XDirection();
    theDI << "Origin: " << anOrig.X() << " " << anOrig.Y() << " " << anOrig.Z() << " "
          << "Normal: " << aNorm.X() << " " << aNorm.Y() << " " << aNorm.Z() << " "
          << "X-dir: "  << aXDir.X() << " " << aXDir.Y() << " " << aXDir.Z() << "\n";
    return 0;
  }

  Standard_Integer anArgIdx = 1;
  Standard_Real anOrigX = Draw::Atof (theArgVec[anArgIdx++]);
  Standard_Real anOrigY = Draw::Atof (theArgVec[anArgIdx++]);
  Standard_Real anOrigZ = Draw::Atof (theArgVec[anArgIdx++]);
  Standard_Real aNormX  = Draw::Atof (theArgVec[anArgIdx++]);
  Standard_Real aNormY  = Draw::Atof (theArgVec[anArgIdx++]);
  Standard_Real aNormZ  = Draw::Atof (theArgVec[anArgIdx++]);

  gp_Ax3 aPriviledgedPlane;
  gp_Pnt anOrig (anOrigX, anOrigY, anOrigZ);
  gp_Dir aNorm (aNormX, aNormY, aNormZ);
  if (theArgNb > 7)
  {
    Standard_Real aXDirX = Draw::Atof (theArgVec[anArgIdx++]);
    Standard_Real aXDirY = Draw::Atof (theArgVec[anArgIdx++]);
    Standard_Real aXDirZ = Draw::Atof (theArgVec[anArgIdx++]);
    gp_Dir aXDir (aXDirX, aXDirY, aXDirZ);
    aPriviledgedPlane = gp_Ax3 (anOrig, aNorm, aXDir);
  }
  else
  {
    aPriviledgedPlane = gp_Ax3 (anOrig, aNorm);
  }

  aViewer->SetPrivilegedPlane (aPriviledgedPlane);

  return 0;
}

//==============================================================================
//function : VConvert
//purpose  :
//==============================================================================

static int VConvert (Draw_Interpretor& theDI,
                     Standard_Integer  theArgNb,
                     const char**      theArgVec)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << "Error: no active view. Please call vinit.\n";
    return 1;
  }

  enum { Model, Ray, View, Window, Grid } aMode = Model;

  // access coordinate arguments
  TColStd_SequenceOfReal aCoord;
  Standard_Integer anArgIdx = 1;
  for (; anArgIdx < 4 && anArgIdx < theArgNb; ++anArgIdx)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIdx]);
    if (!anArg.IsRealValue())
    {
      break;
    }
    aCoord.Append (anArg.RealValue());
  }

  // non-numeric argument too early
  if (aCoord.IsEmpty())
  {
    std::cerr << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  // collect all other arguments and options
  for (; anArgIdx < theArgNb; ++anArgIdx)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIdx]);
    anArg.LowerCase();
    if      (anArg == "window") aMode = Window;
    else if (anArg == "view")   aMode = View;
    else if (anArg == "grid")   aMode = Grid;
    else if (anArg == "ray")    aMode = Ray;
    else
    {
      std::cerr << "Error: wrong argument " << anArg << "! See usage:\n";
      theDI.PrintHelp (theArgVec[0]);
      return 1;
    }
  }

  // complete input checks
  if ((aCoord.Length() == 1 && theArgNb > 3) ||
      (aCoord.Length() == 2 && theArgNb > 4) ||
      (aCoord.Length() == 3 && theArgNb > 5))
  {
    std::cerr << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  Standard_Real aXYZ[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  Standard_Integer aXYp[2] = {0, 0};

  // convert one-dimensional coordinate
  if (aCoord.Length() == 1)
  {
    switch (aMode)
    {
      case View   : theDI << "View Vv: "   << aView->Convert ((Standard_Integer) aCoord (1)); return 0;
      case Window : theDI << "Window Vp: " << aView->Convert ((Quantity_Length) aCoord (1));  return 0;
      default:
        std::cerr << "Error: wrong arguments! See usage:\n";
        theDI.PrintHelp (theArgVec[0]);
        return 1;
    }
  }

  // convert 2D coordinates from projection or view reference space
  if (aCoord.Length() == 2)
  {
    switch (aMode)
    {
      case Model :
        aView->Convert ((Standard_Integer) aCoord (1), (Standard_Integer) aCoord (2), aXYZ[0], aXYZ[1], aXYZ[2]);
        theDI << "Model X,Y,Z: " << aXYZ[0] << " " << aXYZ[1] << " " << aXYZ[2] << "\n";
        return 0;

      case View :
        aView->Convert ((Standard_Integer) aCoord (1), (Standard_Integer) aCoord (2), aXYZ[0], aXYZ[1]);
        theDI << "View Xv,Yv: " << aXYZ[0] << " " << aXYZ[1] << "\n";
        return 0;

      case Window :
        aView->Convert ((V3d_Coordinate) aCoord (1), (V3d_Coordinate) aCoord (2), aXYp[0], aXYp[1]);
        theDI << "Window Xp,Yp: " << aXYp[0] << " " << aXYp[1] << "\n";
        return 0;

      case Grid :
        aView->Convert ((Standard_Integer) aCoord (1), (Standard_Integer) aCoord (2), aXYZ[0], aXYZ[1], aXYZ[2]);
        aView->ConvertToGrid (aXYZ[0], aXYZ[1], aXYZ[2], aXYZ[3], aXYZ[4], aXYZ[5]);
        theDI << "Model X,Y,Z: " << aXYZ[3] << " " << aXYZ[4] << " " << aXYZ[5] << "\n";
        return 0;

      case Ray :
        aView->ConvertWithProj ((Standard_Integer) aCoord (1),
                                (Standard_Integer) aCoord (2),
                                aXYZ[0], aXYZ[1], aXYZ[2],
                                aXYZ[3], aXYZ[4], aXYZ[5]);
        theDI << "Model DX,DY,DZ: " << aXYZ[3] << " " << aXYZ[4] << " " << aXYZ[5] << "\n";
        return 0;

      default:
        std::cerr << "Error: wrong arguments! See usage:\n";
        theDI.PrintHelp (theArgVec[0]);
        return 1;
    }
  }

  // convert 3D coordinates from view reference space
  else if (aCoord.Length() == 3)
  {
    switch (aMode)
    {
      case Window :
        aView->Convert (aCoord (1), aCoord (2), aCoord (3), aXYp[0], aXYp[1]);
        theDI << "Window Xp,Yp: " << aXYp[0] << " " << aXYp[1] << "\n";
        return 0;

      case Grid :
        aView->ConvertToGrid (aCoord (1), aCoord (2), aCoord (3), aXYZ[0], aXYZ[1], aXYZ[2]);
        theDI << "Model X,Y,Z: " << aXYZ[0] << " " << aXYZ[1] << " " << aXYZ[2] << "\n";
        return 0;

      default:
        std::cerr << "Error: wrong arguments! See usage:\n";
        theDI.PrintHelp (theArgVec[0]);
        return 1;
    }
  }

  return 0;
}

//==============================================================================
//function : VFps
//purpose  :
//==============================================================================

static int VFps (Draw_Interpretor& theDI,
                 Standard_Integer  theArgNb,
                 const char**      theArgVec)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << "No active view. Please call vinit.\n";
    return 1;
  }

  Standard_Integer aFramesNb = (theArgNb > 1) ? Draw::Atoi(theArgVec[1]) : 100;
  if (aFramesNb <= 0)
  {
    std::cerr << "Incorrect arguments!\n";
    return 1;
  }

  // the time is meaningless for first call
  // due to async OpenGl rendering
  aView->Redraw();

  // redraw view in loop to estimate average values
  OSD_Timer aTimer;
  aTimer.Start();
  for (Standard_Integer anInter = 0; anInter < aFramesNb; ++anInter)
  {
    aView->Redraw();
  }
  aTimer.Stop();
  Standard_Real aCpu;
  const Standard_Real aTime = aTimer.ElapsedTime();
  aTimer.OSD_Chronometer::Show (aCpu);

  const Standard_Real aFpsAver = Standard_Real(aFramesNb) / aTime;
  const Standard_Real aCpuAver = aCpu / Standard_Real(aFramesNb);

  // return statistics
  theDI << "FPS: " << aFpsAver << "\n"
        << "CPU: " << (1000.0 * aCpuAver) << " msec\n";

  // compute additional statistics in ray-tracing mode
  Graphic3d_RenderingParams& aParams = aView->ChangeRenderingParams();

  if (aParams.Method == Graphic3d_RM_RAYTRACING)
  {
    Standard_Integer aSizeX;
    Standard_Integer aSizeY;

    aView->Window()->Size (aSizeX, aSizeY);

    // 1 shadow ray and 1 secondary ray pew each bounce
    const Standard_Real aMRays = aSizeX * aSizeY * aFpsAver * aParams.RaytracingDepth * 2 / 1.0e6f;

    theDI << "MRays/sec (upper bound): " << aMRays << "\n";
  }

  return 0;
}

//==============================================================================
//function : VGlDebug
//purpose  :
//==============================================================================

static int VGlDebug (Draw_Interpretor& theDI,
                     Standard_Integer  theArgNb,
                     const char**      theArgVec)
{
  Handle(OpenGl_GraphicDriver) aDriver;
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (!aView.IsNull())
  {
    aDriver = Handle(OpenGl_GraphicDriver)::DownCast (aView->Viewer()->Driver());
  }
  OpenGl_Caps* aDefCaps = &ViewerTest_myDefaultCaps;
  OpenGl_Caps* aCaps    = !aDriver.IsNull() ? &aDriver->ChangeOptions() : NULL;

  if (theArgNb < 2)
  {
    TCollection_AsciiString aDebActive, aSyncActive;
    if (aCaps == NULL)
    {
      aCaps = aDefCaps;
    }
    else
    {
      Standard_Boolean isActive = OpenGl_Context::CheckExtension ((const char* )::glGetString (GL_EXTENSIONS),
                                                                  "GL_ARB_debug_output");
      aDebActive = isActive ? " (active)" : " (inactive)";
      if (isActive)
      {
        // GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB
        aSyncActive = ::glIsEnabled (0x8242) == GL_TRUE ? " (active)" : " (inactive)";
      }
    }

    theDI << "debug:   " << (aCaps->contextDebug      ? "1" : "0") << aDebActive  << "\n"
          << "sync:    " << (aCaps->contextSyncDebug  ? "1" : "0") << aSyncActive << "\n"
          << "glslWarn:" << (aCaps->glslWarnings      ? "1" : "0") << "\n"
          << "extraMsg:" << (aCaps->suppressExtraMsg  ? "0" : "1") << "\n";
    return 0;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg     = theArgVec[anArgIter];
    TCollection_AsciiString anArgCase (anArg);
    anArgCase.LowerCase();
    Standard_Boolean toEnableDebug = Standard_True;
    if (anArgCase == "-glsl"
     || anArgCase == "-glslwarn"
     || anArgCase == "-glslwarns"
     || anArgCase == "-glslwarnings")
    {
      Standard_Boolean toShowWarns = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toShowWarns))
      {
        --anArgIter;
      }
      aDefCaps->glslWarnings = toShowWarns;
      if (aCaps != NULL)
      {
        aCaps->glslWarnings = toShowWarns;
      }
    }
    else if (anArgCase == "-extra"
          || anArgCase == "-extramsg"
          || anArgCase == "-extramessages")
    {
      Standard_Boolean toShow = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toShow))
      {
        --anArgIter;
      }
      aDefCaps->suppressExtraMsg = !toShow;
      if (aCaps != NULL)
      {
        aCaps->suppressExtraMsg = !toShow;
      }
    }
    else if (anArgCase == "-noextra"
          || anArgCase == "-noextramsg"
          || anArgCase == "-noextramessages")
    {
      Standard_Boolean toSuppress = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toSuppress))
      {
        --anArgIter;
      }
      aDefCaps->suppressExtraMsg = toSuppress;
      if (aCaps != NULL)
      {
        aCaps->suppressExtraMsg = toSuppress;
      }
    }
    else if (anArgCase == "-sync")
    {
      Standard_Boolean toSync = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toSync))
      {
        --anArgIter;
      }
      aDefCaps->contextSyncDebug = toSync;
      if (toSync)
      {
        aDefCaps->contextDebug = Standard_True;
      }
    }
    else if (anArgCase == "-debug")
    {
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnableDebug))
      {
        --anArgIter;
      }
      aDefCaps->contextDebug = toEnableDebug;
    }
    else if (ViewerTest::ParseOnOff (anArg, toEnableDebug)
          && (anArgIter + 1 == theArgNb))
    {
      // simple alias to turn on almost everything
      aDefCaps->contextDebug     = toEnableDebug;
      aDefCaps->contextSyncDebug = toEnableDebug;
      aDefCaps->glslWarnings     = toEnableDebug;
    }
    else
    {
      std::cout << "Error: wrong syntax at '" << anArg << "'\n";
      return 1;
    }
  }

  return 0;
}

//==============================================================================
//function : VVbo
//purpose  :
//==============================================================================

static int VVbo (Draw_Interpretor& theDI,
                 Standard_Integer  theArgNb,
                 const char**      theArgVec)
{
  const Standard_Boolean toSet    = (theArgNb > 1);
  const Standard_Boolean toUseVbo = toSet ? (Draw::Atoi (theArgVec[1]) == 0) : 1;
  if (toSet)
  {
    ViewerTest_myDefaultCaps.vboDisable = toUseVbo;
  }

  // get the context
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    if (!toSet)
    {
      std::cerr << "No active view!\n";
    }
    return 1;
  }
  Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast (aContextAIS->CurrentViewer()->Driver());
  if (!aDriver.IsNull())
  {
    if (!toSet)
    {
      theDI << (aDriver->Options().vboDisable ? "0" : "1") << "\n";
    }
    else
    {
      aDriver->ChangeOptions().vboDisable = toUseVbo;
    }
  }

  return 0;
}

//==============================================================================
//function : VCaps
//purpose  :
//==============================================================================

static int VCaps (Draw_Interpretor& theDI,
                  Standard_Integer  theArgNb,
                  const char**      theArgVec)
{
  OpenGl_Caps* aCaps = &ViewerTest_myDefaultCaps;
  Handle(OpenGl_GraphicDriver)   aDriver;
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (!aContext.IsNull())
  {
    aDriver = Handle(OpenGl_GraphicDriver)::DownCast (aContext->CurrentViewer()->Driver());
    aCaps   = &aDriver->ChangeOptions();
  }

  if (theArgNb < 2)
  {
    theDI << "VBO:     " << (aCaps->vboDisable        ? "0" : "1") << "\n";
    theDI << "Sprites: " << (aCaps->pntSpritesDisable ? "0" : "1") << "\n";
    theDI << "SoftMode:" << (aCaps->contextNoAccel    ? "1" : "0") << "\n";
    theDI << "FFP:     " << (aCaps->ffpEnable         ? "1" : "0") << "\n";
    theDI << "VSync:   " <<  aCaps->swapInterval                   << "\n";
    theDI << "Compatible:" << (aCaps->contextCompatible ? "1" : "0") << "\n";
    theDI << "Stereo:  " << (aCaps->contextStereo ? "1" : "0") << "\n";
    return 0;
  }

  ViewerTest_AutoUpdater anUpdateTool (aContext, ViewerTest::CurrentView());
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg     = theArgVec[anArgIter];
    TCollection_AsciiString anArgCase (anArg);
    anArgCase.LowerCase();
    if (anUpdateTool.parseRedrawMode (anArg))
    {
      continue;
    }
    else if (anArgCase == "-vsync"
          || anArgCase == "-swapinterval")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->swapInterval = toEnable;
    }
    else if (anArgCase == "-ffp")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->ffpEnable = toEnable;
    }
    else if (anArgCase == "-vbo")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->vboDisable = !toEnable;
    }
    else if (anArgCase == "-sprite"
          || anArgCase == "-sprites")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->pntSpritesDisable = !toEnable;
    }
    else if (anArgCase == "-softmode")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->contextNoAccel = toEnable;
    }
    else if (anArgCase == "-accel"
          || anArgCase == "-acceleration")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->contextNoAccel = !toEnable;
    }
    else if (anArgCase == "-compat"
          || anArgCase == "-compatprofile"
          || anArgCase == "-compatible"
          || anArgCase == "-compatibleprofile")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->contextCompatible = toEnable;
      if (!aCaps->contextCompatible)
      {
        aCaps->ffpEnable = Standard_False;
      }
    }
    else if (anArgCase == "-core"
          || anArgCase == "-coreprofile")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->contextCompatible = !toEnable;
      if (!aCaps->contextCompatible)
      {
        aCaps->ffpEnable = Standard_False;
      }
    }
    else if (anArgCase == "-stereo"
          || anArgCase == "-quadbuffer")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aCaps->contextStereo = toEnable;
    }
    else
    {
      std::cout << "Error: unknown argument '" << anArg << "'\n";
      return 1;
    }
  }
  if (aCaps != &ViewerTest_myDefaultCaps)
  {
    ViewerTest_myDefaultCaps = *aCaps;
  }
  return 0;
}

//==============================================================================
//function : VMemGpu
//purpose  :
//==============================================================================

static int VMemGpu (Draw_Interpretor& theDI,
                    Standard_Integer  theArgNb,
                    const char**      theArgVec)
{
  // get the context
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    std::cerr << "No active view. Please call vinit.\n";
    return 1;
  }

  Handle(Graphic3d_GraphicDriver) aDriver = aContextAIS->CurrentViewer()->Driver();
  if (aDriver.IsNull())
  {
    std::cerr << "Graphic driver not available.\n";
    return 1;
  }

  Standard_Size aFreeBytes = 0;
  TCollection_AsciiString anInfo;
  if (!aDriver->MemoryInfo (aFreeBytes, anInfo))
  {
    std::cerr << "Information not available.\n";
    return 1;
  }

  if (theArgNb > 1 && *theArgVec[1] == 'f')
  {
    theDI << Standard_Real (aFreeBytes);
  }
  else
  {
    theDI << anInfo;
  }

  return 0;
}

// ==============================================================================
// function : VReadPixel
// purpose  :
// ==============================================================================
static int VReadPixel (Draw_Interpretor& theDI,
                       Standard_Integer  theArgNb,
                       const char**      theArgVec)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << "No active view. Please call vinit.\n";
    return 1;
  }
  else if (theArgNb < 3)
  {
    std::cerr << "Usage : " << theArgVec[0] << " xPixel yPixel [{rgb|rgba|depth|hls|rgbf|rgbaf}=rgba] [name]\n";
    return 1;
  }

  Image_PixMap::ImgFormat aFormat     = Image_PixMap::IsBigEndianHost() ? Image_PixMap::ImgRGBA : Image_PixMap::ImgBGRA;
  Graphic3d_BufferType    aBufferType = Graphic3d_BT_RGBA;

  Standard_Integer aWidth, aHeight;
  aView->Window()->Size (aWidth, aHeight);
  const Standard_Integer anX = Draw::Atoi (theArgVec[1]);
  const Standard_Integer anY = Draw::Atoi (theArgVec[2]);
  if (anX < 0 || anX >= aWidth || anY < 0 || anY > aHeight)
  {
    std::cerr << "Pixel coordinates (" << anX << "; " << anY << ") are out of view (" << aWidth << " x " << aHeight << ")\n";
    return 1;
  }

  Standard_Boolean toShowName = Standard_False;
  Standard_Boolean toShowHls  = Standard_False;
  for (Standard_Integer anIter = 3; anIter < theArgNb; ++anIter)
  {
    const char* aParam = theArgVec[anIter];
    if ( strcasecmp( aParam, "rgb" ) == 0 )
    {
      aFormat     = Image_PixMap::IsBigEndianHost() ? Image_PixMap::ImgRGB : Image_PixMap::ImgBGR;
      aBufferType = Graphic3d_BT_RGB;
    }
    else if ( strcasecmp( aParam, "hls" ) == 0 )
    {
      aFormat     = Image_PixMap::IsBigEndianHost() ? Image_PixMap::ImgRGB : Image_PixMap::ImgBGR;
      aBufferType = Graphic3d_BT_RGB;
      toShowHls   = Standard_True;
    }
    else if ( strcasecmp( aParam, "rgbf" ) == 0 )
    {
      aFormat     = Image_PixMap::ImgRGBF;
      aBufferType = Graphic3d_BT_RGB;
    }
    else if ( strcasecmp( aParam, "rgba" ) == 0 )
    {
      aFormat     = Image_PixMap::IsBigEndianHost() ? Image_PixMap::ImgRGBA : Image_PixMap::ImgBGRA;
      aBufferType = Graphic3d_BT_RGBA;
    }
    else if ( strcasecmp( aParam, "rgbaf" ) == 0 )
    {
      aFormat     = Image_PixMap::ImgRGBAF;
      aBufferType = Graphic3d_BT_RGBA;
    }
    else if ( strcasecmp( aParam, "depth" ) == 0 )
    {
      aFormat     = Image_PixMap::ImgGrayF;
      aBufferType = Graphic3d_BT_Depth;
    }
    else if ( strcasecmp( aParam, "name" ) == 0 )
    {
      toShowName = Standard_True;
    }
  }

  Image_PixMap anImage;
  if (!anImage.InitTrash (aFormat, aWidth, aHeight))
  {
    std::cerr << "Image allocation failed\n";
    return 1;
  }
  else if (!aView->ToPixMap (anImage, aWidth, aHeight, aBufferType))
  {
    std::cerr << "Image dump failed\n";
    return 1;
  }

  Quantity_Parameter anAlpha;
  Quantity_Color aColor = anImage.PixelColor (anX, anY, anAlpha);
  if (toShowName)
  {
    if (aBufferType == Graphic3d_BT_RGBA)
    {
      theDI << Quantity_Color::StringName (aColor.Name()) << " " << anAlpha;
    }
    else
    {
      theDI << Quantity_Color::StringName (aColor.Name());
    }
  }
  else
  {
    switch (aBufferType)
    {
      default:
      case Graphic3d_BT_RGB:
      {
        if (toShowHls)
        {
          theDI << aColor.Hue() << " " << aColor.Light() << " " << aColor.Saturation();
        }
        else
        {
          theDI << aColor.Red() << " " << aColor.Green() << " " << aColor.Blue();
        }
        break;
      }
      case Graphic3d_BT_RGBA:
      {
        theDI << aColor.Red() << " " << aColor.Green() << " " << aColor.Blue() << " " << anAlpha;
        break;
      }
      case Graphic3d_BT_Depth:
      {
        theDI << aColor.Red();
        break;
      }
    }
  }

  return 0;
}

//==============================================================================
//function : VDiffImage
//purpose  : The draw-command compares two images.
//==============================================================================

static int VDiffImage (Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb < 6)
  {
    theDI << "Not enough arguments.\n";
    return 1;
  }

  // image file names
  const char* anImgPathRef = theArgVec[1];
  const char* anImgPathNew = theArgVec[2];

  // get string tolerance and check its validity
  Standard_Real aTolColor = Draw::Atof (theArgVec[3]);
  if (aTolColor < 0.0)
    aTolColor = 0.0;
  if (aTolColor > 1.0)
    aTolColor = 1.0;

  Standard_Boolean toBlackWhite     = (Draw::Atoi (theArgVec[4]) == 1);
  Standard_Boolean isBorderFilterOn = (Draw::Atoi (theArgVec[5]) == 1);

  // image file of difference
  const char* aDiffImagePath = (theArgNb >= 7) ? theArgVec[6] : NULL;

  // compare the images
  Image_Diff aComparer;
  if (!aComparer.Init (anImgPathRef, anImgPathNew, toBlackWhite))
  {
    return 1;
  }

  aComparer.SetColorTolerance (aTolColor);
  aComparer.SetBorderFilterOn (isBorderFilterOn);
  Standard_Integer aDiffColorsNb = aComparer.Compare();
  theDI << aDiffColorsNb << "\n";

  // save image of difference
  if (aDiffColorsNb >0 && aDiffImagePath != NULL)
  {
    aComparer.SaveDiffImage (aDiffImagePath);
  }

  return 0;
}

//=======================================================================
//function : VSelect
//purpose  : Emulates different types of selection by mouse:
//           1) single click selection
//           2) selection with rectangle having corners at pixel positions (x1,y1) and (x2,y2)
//           3) selection with polygon having corners at
//           pixel positions (x1,y1),...,(xn,yn)
//           4) any of these selections with shift button pressed
//=======================================================================
static Standard_Integer VSelect (Draw_Interpretor& di,
                                 Standard_Integer argc,
                                 const char ** argv)
{
  if(argc < 3)
  {
    di << "Usage : " << argv[0] << " x1 y1 [x2 y2 [... xn yn]] [shift_selection = 1|0]" << "\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) myAIScontext = ViewerTest::GetAISContext();
  if(myAIScontext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  const Standard_Boolean isShiftSelection = (argc > 3 && !(argc % 2) && (atoi (argv[argc - 1]) == 1));
  Standard_Integer aCoordsNb = isShiftSelection ? argc - 2 : argc - 1;
  TCollection_AsciiString anArg;
  anArg = isShiftSelection ? argv[argc - 3] : argv[argc - 2];
  anArg.LowerCase();
  if (anArg == "-allowoverlap")
  {
    Standard_Boolean isValidated = isShiftSelection ? argc == 8
      : argc == 7;
    if (!isValidated)
    {
      di << "Wrong number of arguments! -allowoverlap key is applied only for rectangle selection";
      return 1;
    }

    Standard_Integer isToAllow = isShiftSelection ? Draw::Atoi(argv[argc - 2]) : Draw::Atoi(argv[argc - 1]);
    myAIScontext->MainSelector()->AllowOverlapDetection((Standard_Boolean)isToAllow);
    aCoordsNb -= 2;
  }

  Handle(ViewerTest_EventManager) aCurrentEventManager = ViewerTest::CurrentEventManager();
  aCurrentEventManager->MoveTo(atoi(argv[1]),atoi(argv[2]));
  if(aCoordsNb == 2)
  {
    if(isShiftSelection)
      aCurrentEventManager->ShiftSelect();
    else
      aCurrentEventManager->Select();
  }
  else if(aCoordsNb == 4)
  {
    if(isShiftSelection)
      aCurrentEventManager->ShiftSelect (atoi (argv[1]), atoi (argv[2]), atoi (argv[3]), atoi (argv[4]), Standard_False);
    else
      aCurrentEventManager->Select (atoi (argv[1]), atoi (argv[2]), atoi (argv[3]), atoi (argv[4]), Standard_False);
  }
  else
  {
    TColgp_Array1OfPnt2d aPolyline (1,aCoordsNb / 2);

    for(Standard_Integer i=1;i<=aCoordsNb / 2;++i)
      aPolyline.SetValue(i,gp_Pnt2d(atoi(argv[2*i-1]),atoi(argv[2*i])));

    if(isShiftSelection)
      aCurrentEventManager->ShiftSelect(aPolyline);
    else
      aCurrentEventManager->Select(aPolyline);
  }
  return 0;
}

//=======================================================================
//function : VMoveTo
//purpose  : Emulates cursor movement to defined pixel position
//=======================================================================
static Standard_Integer VMoveTo (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  if(argc != 3)
  {
    di << "Usage : " << argv[0] << " x y" << "\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  ViewerTest::CurrentEventManager()->MoveTo(atoi(argv[1]),atoi(argv[2]));
  return 0;
}

//=================================================================================================
//function : VViewParams
//purpose  : Gets or sets AIS View characteristics
//=================================================================================================
static int VViewParams (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  Handle(V3d_View) anAISView = ViewerTest::CurrentView();
  if (anAISView.IsNull())
  {
    std::cout << theArgVec[0] << ": please initialize or activate view.\n";
    return 1;
  }

  if (theArgsNb == 1)
  {
    // print all of the available view parameters
    Quantity_Factor anAISViewScale = anAISView->Scale();

    Standard_Real anAISViewProjX = 0.0;
    Standard_Real anAISViewProjY = 0.0;
    Standard_Real anAISViewProjZ = 0.0;
    anAISView->Proj (anAISViewProjX, anAISViewProjY, anAISViewProjZ);

    Standard_Real anAISViewUpX = 0.0;
    Standard_Real anAISViewUpY = 0.0;
    Standard_Real anAISViewUpZ = 0.0;
    anAISView->Up (anAISViewUpX, anAISViewUpY, anAISViewUpZ);

    Standard_Real anAISViewAtX = 0.0;
    Standard_Real anAISViewAtY = 0.0;
    Standard_Real anAISViewAtZ = 0.0;
    anAISView->At (anAISViewAtX, anAISViewAtY, anAISViewAtZ);

    Standard_Real anAISViewEyeX = 0.0;
    Standard_Real anAISViewEyeY = 0.0;
    Standard_Real anAISViewEyeZ = 0.0;
    anAISView->Eye (anAISViewEyeX, anAISViewEyeY, anAISViewEyeZ);

    theDi << "Scale of current view: " << anAISViewScale << "\n";
    theDi << "Proj on X : " << anAISViewProjX << "; on Y: " << anAISViewProjY << "; on Z: " << anAISViewProjZ << "\n";
    theDi << "Up on X : " << anAISViewUpX << "; on Y: " << anAISViewUpY << "; on Z: " << anAISViewUpZ << "\n";
    theDi << "At on X : " << anAISViewAtX << "; on Y: " << anAISViewAtY << "; on Z: " << anAISViewAtZ << "\n";
    theDi << "Eye on X : " << anAISViewEyeX << "; on Y: " << anAISViewEyeY << "; on Z: " << anAISViewEyeZ << "\n";
    return 0;
  }

  // -------------------------
  //  Parse options and values
  // -------------------------

  NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HSequenceOfAsciiString)> aMapOfKeysByValues;
  TCollection_AsciiString aParseKey;
  for (Standard_Integer anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    TCollection_AsciiString anArg (theArgVec [anArgIt]);

    if (anArg.Value (1) == '-' && !anArg.IsRealValue())
    {
      aParseKey = anArg;
      aParseKey.Remove (1);
      aParseKey.UpperCase();
      aMapOfKeysByValues.Bind (aParseKey, new TColStd_HSequenceOfAsciiString);
      continue;
    }

    if (aParseKey.IsEmpty())
    {
      std::cout << theArgVec[0] << ": values should be passed with key.\n";
      std::cout << "Type help for more information.\n";
      return 1;
    }

    aMapOfKeysByValues(aParseKey)->Append (anArg);
  }

  // ---------------------------------------------
  //  Change or print parameters, order plays role
  // ---------------------------------------------

  // Check arguments for validity
  NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HSequenceOfAsciiString)>::Iterator aMapIt (aMapOfKeysByValues);
  for (; aMapIt.More(); aMapIt.Next())
  {
    const TCollection_AsciiString& aKey = aMapIt.Key();
    const Handle(TColStd_HSequenceOfAsciiString)& aValues = aMapIt.Value();

    if (!(aKey.IsEqual ("SCALE")  && (aValues->Length() == 1 || aValues->IsEmpty()))
     && !(aKey.IsEqual ("SIZE")   && (aValues->Length() == 1 || aValues->IsEmpty()))
     && !(aKey.IsEqual ("EYE")    && (aValues->Length() == 3 || aValues->IsEmpty()))
     && !(aKey.IsEqual ("AT")     && (aValues->Length() == 3 || aValues->IsEmpty()))
     && !(aKey.IsEqual ("UP")     && (aValues->Length() == 3 || aValues->IsEmpty()))
     && !(aKey.IsEqual ("PROJ")   && (aValues->Length() == 3 || aValues->IsEmpty()))
     && !(aKey.IsEqual ("CENTER") &&  aValues->Length() == 2))
    {
      TCollection_AsciiString aLowerKey;
      aLowerKey  = "-";
      aLowerKey += aKey;
      aLowerKey.LowerCase();
      std::cout << theArgVec[0] << ": " << aLowerKey << " is unknown option, or number of arguments is invalid.\n";
      std::cout << "Type help for more information.\n";
      return 1;
    }
  }

  Handle(TColStd_HSequenceOfAsciiString) aValues;

  // Change view parameters in proper order
  if (aMapOfKeysByValues.Find ("SCALE", aValues))
  {
    if (aValues->IsEmpty())
    {
      theDi << "Scale: " << anAISView->Scale() << "\n";
    }
    else
    {
      anAISView->SetScale (aValues->Value(1).RealValue());
    }
  }
  if (aMapOfKeysByValues.Find ("SIZE", aValues))
  {
    if (aValues->IsEmpty())
    {
      Standard_Real aSizeX = 0.0;
      Standard_Real aSizeY = 0.0;
      anAISView->Size (aSizeX, aSizeY);
      theDi << "Size X: " << aSizeX << " Y: " << aSizeY << "\n";
    }
    else
    {
      anAISView->SetSize (aValues->Value(1).RealValue());
    }
  }
  if (aMapOfKeysByValues.Find ("EYE", aValues))
  {
    if (aValues->IsEmpty())
    {
      Standard_Real anEyeX = 0.0;
      Standard_Real anEyeY = 0.0;
      Standard_Real anEyeZ = 0.0;
      anAISView->Eye (anEyeX, anEyeY, anEyeZ);
      theDi << "Eye X: " << anEyeX << " Y: " << anEyeY << " Z: " << anEyeZ << "\n";
    }
    else
    {
      anAISView->SetEye (aValues->Value(1).RealValue(), aValues->Value(2).RealValue(), aValues->Value(3).RealValue());
    }
  }
  if (aMapOfKeysByValues.Find ("AT", aValues))
  {
    if (aValues->IsEmpty())
    {
      Standard_Real anAtX = 0.0;
      Standard_Real anAtY = 0.0;
      Standard_Real anAtZ = 0.0;
      anAISView->At (anAtX, anAtY, anAtZ);
      theDi << "At X: " << anAtX << " Y: " << anAtY << " Z: " << anAtZ << "\n";
    }
    else
    {
      anAISView->SetAt (aValues->Value(1).RealValue(), aValues->Value(2).RealValue(), aValues->Value(3).RealValue());
    }
  }
  if (aMapOfKeysByValues.Find ("PROJ", aValues))
  {
    if (aValues->IsEmpty())
    {
      Standard_Real aProjX = 0.0;
      Standard_Real aProjY = 0.0;
      Standard_Real aProjZ = 0.0;
      anAISView->Proj (aProjX, aProjY, aProjZ);
      theDi << "Proj X: " << aProjX << " Y: " << aProjY << " Z: " << aProjZ << "\n";
    }
    else
    {
      anAISView->SetProj (aValues->Value(1).RealValue(), aValues->Value(2).RealValue(), aValues->Value(3).RealValue());
    }
  }
  if (aMapOfKeysByValues.Find ("UP", aValues))
  {
    if (aValues->IsEmpty())
    {
      Standard_Real anUpX = 0.0;
      Standard_Real anUpY = 0.0;
      Standard_Real anUpZ = 0.0;
      anAISView->Up (anUpX, anUpY, anUpZ);
      theDi << "Up X: " << anUpX << " Y: " << anUpY << " Z: " << anUpZ << "\n";
    }
    else
    {
      anAISView->SetUp (aValues->Value(1).RealValue(), aValues->Value(2).RealValue(), aValues->Value(3).RealValue());
    }
  }
  if (aMapOfKeysByValues.Find ("CENTER", aValues))
  {
    anAISView->SetCenter (aValues->Value(1).IntegerValue(), aValues->Value(2).IntegerValue());
  }

  return 0;
}

//=======================================================================
//function : VChangeSelected
//purpose  : Adds the shape to selection or remove one from it
//=======================================================================
static Standard_Integer VChangeSelected (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  if(argc != 2)
  {
    di<<"Usage : " << argv[0] << " shape \n";
    return 1;
  }
  //get AIS_Shape:
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  TCollection_AsciiString aName(argv[1]);
  Handle(AIS_InteractiveObject) anAISObject;

  if(!aMap.IsBound2(aName))
  {
    di<<"Use 'vdisplay' before";
    return 1;
  }
  else
  {
    anAISObject = Handle(AIS_InteractiveObject)::DownCast(aMap.Find2(aName));
    if(anAISObject.IsNull()){
      di<<"No interactive object \n";
      return 1;
    }

    aContext->AddOrRemoveSelected(anAISObject);
  }
  return 0;
}

//=======================================================================
//function : VZClipping
//purpose  : Gets or sets ZClipping mode, width and depth
//=======================================================================
static Standard_Integer VZClipping (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  if(argc>4)
  {
    di << "Usage : " << argv[0] << " [mode] [depth  width]" << "\n"
      <<"mode = OFF|BACK|FRONT|SLICE depth = [0..1] width = [0..1]" << "\n";
    return -1;
  }
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  V3d_TypeOfZclipping aZClippingMode = V3d_OFF;
  if(argc==1)
  {
    TCollection_AsciiString aZClippingModeString;
    Quantity_Length aDepth, aWidth;
    aZClippingMode = aView->ZClipping(aDepth, aWidth);
    switch (aZClippingMode)
    {
    case V3d_OFF:
      aZClippingModeString.Copy("OFF");
      break;
    case V3d_BACK:
      aZClippingModeString.Copy("BACK");
      break;
    case V3d_FRONT:
      aZClippingModeString.Copy("FRONT");
      break;
    case V3d_SLICE:
      aZClippingModeString.Copy("SLICE");
      break;
    default:
      aZClippingModeString.Copy(TCollection_AsciiString(aZClippingMode));
      break;
    }
    di << "ZClippingMode = " << aZClippingModeString.ToCString() << "\n"
      << "ZClipping depth = " << aDepth << "\n"
      << "ZClipping width = " << aWidth << "\n";
  }
  else
  {
    if(argc !=3)
    {
      Standard_Integer aStatus = 0;
      if ( strcmp (argv [1], "OFF") == 0 ) {
        aStatus = 1;
        aZClippingMode = V3d_OFF;
      }
      if ( strcmp (argv [1], "BACK") == 0 ) {
        aStatus = 1;
        aZClippingMode = V3d_BACK;
      }
      if ( strcmp (argv [1], "FRONT") == 0 ) {
        aStatus = 1;
        aZClippingMode = V3d_FRONT;
      }
      if ( strcmp (argv [1], "SLICE") == 0 ) {
        aStatus = 1;
        aZClippingMode = V3d_SLICE;
      }
      if (aStatus != 1)
      {
        di << "Bad mode; Usage : " << argv[0] << " [mode] [depth width]" << "\n"
          << "mode = OFF|BACK|FRONT|SLICE depth = [0..1] width = [0..1]" << "\n";
        return 1;
      }
      aView->SetZClippingType(aZClippingMode);
    }
    if(argc >2)
    {
      Quantity_Length aDepth = 0., aWidth = 1.;
      if(argc == 3)
      {
        aDepth = Draw::Atof (argv[1]);
        aWidth = Draw::Atof (argv[2]);
      }
      else if(argc == 4)
      {
        aDepth = Draw::Atof (argv[2]);
        aWidth = Draw::Atof (argv[3]);
      }

      if(aDepth<0. || aDepth>1.)
      {
        di << "Bad depth; Usage : " << argv[0] << " [mode] [depth width]" << "\n"
        << "mode = OFF|BACK|FRONT|SLICE depth = [0..1] width = [0..1]" << "\n";
        return 1;
      }
      if(aWidth<0. || aWidth>1.)
      {
        di << "Bad width; Usage : " << argv[0] << " [mode] [depth width]" << "\n"
        << "mode = OFF|BACK|FRONT|SLICE depth = [0..1] width = [0..1]" << "\n";
        return 1;
      }

      aView->SetZClippingDepth(aDepth);
      aView->SetZClippingWidth(aWidth);
    }
    aView->Redraw();
  }
  return 0;
}

//=======================================================================
//function : VNbSelected
//purpose  : Returns number of selected objects
//=======================================================================
static Standard_Integer VNbSelected (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  if(argc != 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  di << aContext->NbSelected() << "\n";
  return 0;
}

//=======================================================================
//function : VAntialiasing
//purpose  : Switches altialiasing on or off
//=======================================================================
static Standard_Integer VAntialiasing (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  if(argc > 2)
  {
    di << "Usage : " << argv[0] << " [1|0]" << "\n";
    return 1;
  }

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();

  if((argc == 2) && (atof(argv[1]) == 0))
    aView->SetAntialiasingOff();
  else
    aView->SetAntialiasingOn();
  aView->Update();
  return 0;
}

//=======================================================================
//function : VPurgeDisplay
//purpose  : Switches altialiasing on or off
//=======================================================================
static Standard_Integer VPurgeDisplay (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  if (argc > 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  aContext->CloseAllContexts(Standard_False);
  di << aContext->PurgeDisplay() << "\n";
  return 0;
}

//=======================================================================
//function : VSetViewSize
//purpose  :
//=======================================================================
static Standard_Integer VSetViewSize (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if(argc != 2)
  {
    di<<"Usage : " << argv[0] << " Size\n";
    return 1;
  }
  Standard_Real aSize = Draw::Atof (argv[1]);
  if (aSize <= 0.)
  {
    di<<"Bad Size value  : " << aSize << "\n";
    return 1;
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->SetSize(aSize);
  return 0;
}

//=======================================================================
//function : VMoveView
//purpose  :
//=======================================================================
static Standard_Integer VMoveView (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if(argc < 4 || argc > 5)
  {
    di<<"Usage : " << argv[0] << " Dx Dy Dz [Start = 1|0]\n";
    return 1;
  }
  Standard_Real Dx = Draw::Atof (argv[1]);
  Standard_Real Dy = Draw::Atof (argv[2]);
  Standard_Real Dz = Draw::Atof (argv[3]);
  Standard_Boolean aStart = Standard_True;
  if (argc == 5)
  {
      aStart = (Draw::Atoi (argv[4]) > 0);
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->Move(Dx,Dy,Dz,aStart);
  return 0;
}

//=======================================================================
//function : VTranslateView
//purpose  :
//=======================================================================
static Standard_Integer VTranslateView (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if(argc < 4 || argc > 5)
  {
    di<<"Usage : " << argv[0] << " Dx Dy Dz [Start = 1|0]\n";
    return 1;
  }
  Standard_Real Dx = Draw::Atof (argv[1]);
  Standard_Real Dy = Draw::Atof (argv[2]);
  Standard_Real Dz = Draw::Atof (argv[3]);
  Standard_Boolean aStart = Standard_True;
  if (argc == 5)
  {
      aStart = (Draw::Atoi (argv[4]) > 0);
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->Translate(Dx,Dy,Dz,aStart);
  return 0;
}

//=======================================================================
//function : VTurnView
//purpose  :
//=======================================================================
static Standard_Integer VTurnView (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if(argc < 4 || argc > 5){
    di<<"Usage : " << argv[0] << " Ax Ay Az [Start = 1|0]\n";
    return 1;
  }
  Standard_Real Ax = Draw::Atof (argv[1]);
  Standard_Real Ay = Draw::Atof (argv[2]);
  Standard_Real Az = Draw::Atof (argv[3]);
  Standard_Boolean aStart = Standard_True;
  if (argc == 5)
  {
      aStart = (Draw::Atoi (argv[4]) > 0);
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->Turn(Ax,Ay,Az,aStart);
  return 0;
}

//==============================================================================
//function : VTextureEnv
//purpose  : ENables or disables environment mapping
//==============================================================================
class OCC_TextureEnv : public Graphic3d_TextureEnv
{
public:
  OCC_TextureEnv(const Standard_CString FileName);
  OCC_TextureEnv(const Graphic3d_NameOfTextureEnv aName);
  void SetTextureParameters(const Standard_Boolean theRepeatFlag,
                            const Standard_Boolean theModulateFlag,
                            const Graphic3d_TypeOfTextureFilter theFilter,
                            const Standard_ShortReal theXScale,
                            const Standard_ShortReal theYScale,
                            const Standard_ShortReal theXShift,
                            const Standard_ShortReal theYShift,
                            const Standard_ShortReal theAngle);
  DEFINE_STANDARD_RTTI(OCC_TextureEnv, Graphic3d_TextureEnv);
};
DEFINE_STANDARD_HANDLE(OCC_TextureEnv, Graphic3d_TextureEnv);




OCC_TextureEnv::OCC_TextureEnv(const Standard_CString theFileName)
  : Graphic3d_TextureEnv(theFileName)
{
}

OCC_TextureEnv::OCC_TextureEnv(const Graphic3d_NameOfTextureEnv theTexId)
  : Graphic3d_TextureEnv(theTexId)
{
}

void OCC_TextureEnv::SetTextureParameters(const Standard_Boolean theRepeatFlag,
                                          const Standard_Boolean theModulateFlag,
                                          const Graphic3d_TypeOfTextureFilter theFilter,
                                          const Standard_ShortReal theXScale,
                                          const Standard_ShortReal theYScale,
                                          const Standard_ShortReal theXShift,
                                          const Standard_ShortReal theYShift,
                                          const Standard_ShortReal theAngle)
{
  myParams->SetRepeat     (theRepeatFlag);
  myParams->SetModulate   (theModulateFlag);
  myParams->SetFilter     (theFilter);
  myParams->SetScale      (Graphic3d_Vec2(theXScale, theYScale));
  myParams->SetTranslation(Graphic3d_Vec2(theXShift, theYShift));
  myParams->SetRotation   (theAngle);
}

static int VTextureEnv (Draw_Interpretor& /*theDI*/, Standard_Integer theArgNb, const char** theArgVec)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << "No active view. Please call vinit.\n";
    return 1;
  }

  // Checking the input arguments
  Standard_Boolean anEnableFlag = Standard_False;
  Standard_Boolean isOk         = theArgNb >= 2;
  if (isOk)
  {
    TCollection_AsciiString anEnableOpt(theArgVec[1]);
    anEnableFlag = anEnableOpt.IsEqual("on");
    isOk         = anEnableFlag || anEnableOpt.IsEqual("off");
  }
  if (anEnableFlag)
  {
    isOk = (theArgNb == 3 || theArgNb == 11);
    if (isOk)
    {
      TCollection_AsciiString aTextureOpt(theArgVec[2]);
      isOk = (!aTextureOpt.IsIntegerValue() ||
             (aTextureOpt.IntegerValue() >= 0 && aTextureOpt.IntegerValue() < Graphic3d_NOT_ENV_UNKNOWN));

      if (isOk && theArgNb == 11)
      {
        TCollection_AsciiString aRepeatOpt  (theArgVec[3]),
                                aModulateOpt(theArgVec[4]),
                                aFilterOpt  (theArgVec[5]),
                                aSScaleOpt  (theArgVec[6]),
                                aTScaleOpt  (theArgVec[7]),
                                aSTransOpt  (theArgVec[8]),
                                aTTransOpt  (theArgVec[9]),
                                anAngleOpt  (theArgVec[10]);
        isOk = ((aRepeatOpt.  IsEqual("repeat")   || aRepeatOpt.  IsEqual("clamp")) &&
                (aModulateOpt.IsEqual("modulate") || aModulateOpt.IsEqual("decal")) &&
                (aFilterOpt.  IsEqual("nearest")  || aFilterOpt.  IsEqual("bilinear") || aFilterOpt.IsEqual("trilinear")) &&
                aSScaleOpt.IsRealValue() && aTScaleOpt.IsRealValue() &&
                aSTransOpt.IsRealValue() && aTTransOpt.IsRealValue() &&
                anAngleOpt.IsRealValue());
      }
    }
  }

  if (!isOk)
  {
    std::cerr << "Usage :" << std::endl;
    std::cerr << theArgVec[0] << " off" << std::endl;
    std::cerr << theArgVec[0] << " on {index_of_std_texture(0..7)|texture_file_name} [{clamp|repeat} {decal|modulate} {nearest|bilinear|trilinear} scale_s scale_t translation_s translation_t rotation_degrees]" << std::endl;
    return 1;
  }

  if (anEnableFlag)
  {
    TCollection_AsciiString aTextureOpt(theArgVec[2]);
    Handle(OCC_TextureEnv) aTexEnv = aTextureOpt.IsIntegerValue() ?
                                     new OCC_TextureEnv((Graphic3d_NameOfTextureEnv)aTextureOpt.IntegerValue()) :
                                     new OCC_TextureEnv(theArgVec[2]);

    if (theArgNb == 11)
    {
      TCollection_AsciiString aRepeatOpt(theArgVec[3]), aModulateOpt(theArgVec[4]), aFilterOpt(theArgVec[5]);
      aTexEnv->SetTextureParameters(
        aRepeatOpt.  IsEqual("repeat"),
        aModulateOpt.IsEqual("modulate"),
        aFilterOpt.  IsEqual("nearest") ? Graphic3d_TOTF_NEAREST :
                                          aFilterOpt.IsEqual("bilinear") ? Graphic3d_TOTF_BILINEAR :
                                                                           Graphic3d_TOTF_TRILINEAR,
        (Standard_ShortReal)Draw::Atof(theArgVec[6]),
        (Standard_ShortReal)Draw::Atof(theArgVec[7]),
        (Standard_ShortReal)Draw::Atof(theArgVec[8]),
        (Standard_ShortReal)Draw::Atof(theArgVec[9]),
        (Standard_ShortReal)Draw::Atof(theArgVec[10])
        );
    }
    aView->SetTextureEnv(aTexEnv);
    aView->SetSurfaceDetail(V3d_TEX_ENVIRONMENT);
  }
  else // Disabling environment mapping
  {
    aView->SetSurfaceDetail(V3d_TEX_NONE);
    Handle(Graphic3d_TextureEnv) aTexture;
    aView->SetTextureEnv(aTexture); // Passing null handle to clear the texture data
  }

  aView->Redraw();
  return 0;
}

//===============================================================================================
//function : VClipPlane
//purpose  :
//===============================================================================================
static int VClipPlane (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  // use short-cut for created clip planes map of created (or "registered by name") clip planes
  typedef NCollection_DataMap<TCollection_AsciiString, Handle(Graphic3d_ClipPlane)> MapOfPlanes;
  static MapOfPlanes aRegPlanes;

  if (theArgsNb < 2)
  {
    theDi << theArgVec[0] << ": command argument is required. Type help for more information.\n";
    return 1;
  }

  TCollection_AsciiString aCommand (theArgVec[1]);

  // print maximum number of planes for current viewer
  if (aCommand == "maxplanes")
  {
    if (theArgsNb < 3)
    {
      theDi << theArgVec[0] << ": view name is required. Type help for more information.\n";
      return 1;
    }

    TCollection_AsciiString aViewName (theArgVec[2]);

    if (!ViewerTest_myViews.IsBound1 (aViewName))
    {
      theDi << theArgVec[0] << ": view is not found.\n";
      return 1;
    }

    const Handle(V3d_View)& aView = ViewerTest_myViews.Find1 (aViewName);

    theDi << theArgVec[0] << ": "
                          << aView->Viewer()->Driver()->InquirePlaneLimit()
                          << " plane slots provided by driver."
                          << " Note that 2 more planes might be used (reserved for z-clipping).\n";

    return 0;
  }

  // create / delete plane instance
  if (aCommand == "create" || aCommand == "delete" || aCommand == "clone")
  {
    if (theArgsNb < 3)
    {
      theDi << theArgVec[0] << ": plane name is required. Type help for more information.\n";
      return 1;
    }

    Standard_Boolean toCreate = (aCommand == "create");
    Standard_Boolean toClone  = (aCommand == "clone");
    TCollection_AsciiString aPlane (theArgVec[2]);

    if (toCreate)
    {
      if (aRegPlanes.IsBound (aPlane))
      {
        theDi << theArgVec[0] << ": plane name is in use.\n";
        return 1;
      }

      aRegPlanes.Bind (aPlane, new Graphic3d_ClipPlane());
    }
    else if (toClone) // toClone
    {
      if (!aRegPlanes.IsBound (aPlane))
      {
        theDi << theArgVec[0] << ": no such plane.\n";
        return 1;
      }

      if (theArgsNb < 4)
      {
        theDi << theArgVec[0] << ": enter name for new plane. Type help for more information.\n";
        return 1;
      }

      TCollection_AsciiString aClone (theArgVec[3]);
      if (aRegPlanes.IsBound (aClone))
      {
        theDi << theArgVec[0] << ": plane name is in use.\n";
        return 1;
      }

      const Handle(Graphic3d_ClipPlane)& aClipPlane = aRegPlanes.Find (aPlane);

      aRegPlanes.Bind (aClone, aClipPlane->Clone());
    }
    else// toDelete
    {
      if (!aRegPlanes.IsBound (aPlane))
      {
        theDi << theArgVec[0] << ": no such plane.\n";
        return 1;
      }

      Handle(Graphic3d_ClipPlane) aClipPlane = aRegPlanes.Find (aPlane);
      aRegPlanes.UnBind (aPlane);

      ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIObjIt (GetMapOfAIS());
      for (; anIObjIt.More(); anIObjIt.Next())
      {
        Handle(PrsMgr_PresentableObject) aPrs = Handle(PrsMgr_PresentableObject)::DownCast (anIObjIt.Key1());
        aPrs->RemoveClipPlane(aClipPlane);
      }

      NCollection_DoubleMap<TCollection_AsciiString, Handle(V3d_View)>::Iterator aViewIt(ViewerTest_myViews);
      for (; aViewIt.More(); aViewIt.Next())
      {
        const Handle(V3d_View)& aView = aViewIt.Key2();
        aView->RemoveClipPlane(aClipPlane);
      }

      ViewerTest::RedrawAllViews();
    }

    return 0;
  }

  // set / unset plane command
  if (aCommand == "set" || aCommand == "unset")
  {
    if (theArgsNb < 4)
    {
      theDi << theArgVec[0] << ": need more arguments. Type help for more information.\n";
      return 1;
    }

    Standard_Boolean toSet = (aCommand == "set");
    TCollection_AsciiString aPlane (theArgVec [2]);
    if (!aRegPlanes.IsBound (aPlane))
    {
      theDi << theArgVec[0] << ": no such plane.\n";
      return 1;
    }

    const Handle(Graphic3d_ClipPlane)& aClipPlane = aRegPlanes.Find (aPlane);

    TCollection_AsciiString aTarget (theArgVec [3]);
    if (aTarget != "object" && aTarget != "view")
    {
      theDi << theArgVec[0] << ": invalid target.\n";
      return 1;
    }

    if (aTarget == "object" || aTarget == "view")
    {
      if (theArgsNb < 5)
      {
        theDi << theArgVec[0] << ": need more arguments. Type help for more information.\n";
        return 1;
      }

      Standard_Boolean isObject = (aTarget == "object");

      for (Standard_Integer anIt = 4; anIt < theArgsNb; ++anIt)
      {
        TCollection_AsciiString anEntityName (theArgVec[anIt]);
        if (isObject) // to object
        {
          if (!GetMapOfAIS().IsBound2 (anEntityName))
          {
            theDi << theArgVec[0] << ": can not find IO with name " << anEntityName << ".\n";
            continue;
          }

          Handle(AIS_InteractiveObject) aIObj =
            Handle(AIS_InteractiveObject)::DownCast (GetMapOfAIS().Find2 (anEntityName));

          if (toSet)
            aIObj->AddClipPlane (aClipPlane);
          else
            aIObj->RemoveClipPlane (aClipPlane);
        }
        else // to view
        {
          if (!ViewerTest_myViews.IsBound1 (anEntityName))
          {
            theDi << theArgVec[0] << ": can not find View with name " << anEntityName << ".\n";
            continue;
          }

          Handle(V3d_View) aView = ViewerTest_myViews.Find1(anEntityName);
          if (toSet)
            aView->AddClipPlane (aClipPlane);
          else
            aView->RemoveClipPlane (aClipPlane);
        }
      }

      ViewerTest::RedrawAllViews();
    }

    return 0;
  }

  // change plane command
  if (aCommand == "change")
  {
    if (theArgsNb < 4)
    {
      theDi << theArgVec[0] << ": need more arguments. Type help for more information.\n";
      return 1;
    }

    TCollection_AsciiString aPlane (theArgVec [2]);
    if (!aRegPlanes.IsBound (aPlane))
    {
      theDi << theArgVec[0] << ": no such plane.\n";
      return 1;
    }

    const Handle(Graphic3d_ClipPlane)& aClipPlane = aRegPlanes.Find (aPlane);

    TCollection_AsciiString aChangeArg (theArgVec [3]);
    if (aChangeArg != "on" && aChangeArg != "off" && aChangeArg != "capping" && aChangeArg != "equation")
    {
      theDi << theArgVec[0] << ": invalid arguments. Type help for more information.\n";
      return 1;
    }

    if (aChangeArg == "on" || aChangeArg == "off") // on / off
    {
      aClipPlane->SetOn (aChangeArg == "on");
    }
    else if (aChangeArg == "equation") // change equation
    {
      if (theArgsNb < 8)
      {
        theDi << theArgVec[0] << ": need more arguments. Type help for more information.\n";
        return 1;
      }

      Standard_Real aCoeffA = Draw::Atof (theArgVec [4]);
      Standard_Real aCoeffB = Draw::Atof (theArgVec [5]);
      Standard_Real aCoeffC = Draw::Atof (theArgVec [6]);
      Standard_Real aCoeffD = Draw::Atof (theArgVec [7]);
      aClipPlane->SetEquation (gp_Pln (aCoeffA, aCoeffB, aCoeffC, aCoeffD));
    }
    else if (aChangeArg == "capping") // change capping aspects
    {
      if (theArgsNb < 5)
      {
        theDi << theArgVec[0] << ": need more arguments. Type help for more information.\n";
        return 1;
      }

      TCollection_AsciiString aCappingArg (theArgVec [4]);
      if (aCappingArg != "on" && aCappingArg != "off" &&
          aCappingArg != "color" && aCappingArg != "texname" &&
          aCappingArg != "texscale" && aCappingArg != "texorigin" &&
          aCappingArg != "texrotate" && aCappingArg != "hatch")
      {
        theDi << theArgVec[0] << ": invalid arguments. Type help for more information.\n";
        return 1;
      }

      if (aCappingArg == "on" || aCappingArg == "off") // on / off capping
      {
        aClipPlane->SetCapping (aCappingArg == "on");
      }
      else if (aCappingArg == "color") // color aspect for capping
      {
        if (theArgsNb < 8)
        {
          theDi << theArgVec[0] << ": need more arguments. Type help for more information.\n";
          return 1;
        }

        Standard_Real aRed = Draw::Atof (theArgVec [5]);
        Standard_Real aGrn = Draw::Atof (theArgVec [6]);
        Standard_Real aBlu = Draw::Atof (theArgVec [7]);

        Graphic3d_MaterialAspect aMat = aClipPlane->CappingMaterial();
        Quantity_Color aColor (aRed, aGrn, aBlu, Quantity_TOC_RGB);
        aMat.SetAmbientColor (aColor);
        aMat.SetDiffuseColor (aColor);
        aClipPlane->SetCappingMaterial (aMat);
      }
      else if (aCappingArg == "texname") // texture name
      {
        if (theArgsNb < 6)
        {
          theDi << theArgVec[0] << ": need more arguments. Type help for more information.\n";
          return 1;
        }

        TCollection_AsciiString aTextureName (theArgVec [5]);

        Handle(Graphic3d_Texture2Dmanual) aTexture = new Graphic3d_Texture2Dmanual(aTextureName);
        if (!aTexture->IsDone ())
        {
          aClipPlane->SetCappingTexture (NULL);
        }
        else
        {
          aTexture->EnableModulate();
          aTexture->EnableRepeat();
          aClipPlane->SetCappingTexture (aTexture);
        }
      }
      else if (aCappingArg == "texscale") // texture scale
      {
        if (aClipPlane->CappingTexture().IsNull())
        {
          theDi << theArgVec[0] << ": no texture is set.\n";
          return 1;
        }

        if (theArgsNb < 7)
        {
          theDi << theArgVec[0] << ": need more arguments. Type help for more information.\n";
          return 1;
        }

        Standard_ShortReal aSx = (Standard_ShortReal)atof (theArgVec [5]);
        Standard_ShortReal aSy = (Standard_ShortReal)atof (theArgVec [6]);

        aClipPlane->CappingTexture()->GetParams()->SetScale (Graphic3d_Vec2 (aSx, aSy));
      }
      else if (aCappingArg == "texorigin") // texture origin
      {
        if (aClipPlane->CappingTexture().IsNull())
        {
          theDi << theArgVec[0] << ": no texture is set.\n";
          return 1;
        }

        if (theArgsNb < 7)
        {
          theDi << theArgVec[0] << ": need more arguments. Type help for more information.\n";
          return 1;
        }

        Standard_ShortReal aTx = (Standard_ShortReal)atof (theArgVec [5]);
        Standard_ShortReal aTy = (Standard_ShortReal)atof (theArgVec [6]);

        aClipPlane->CappingTexture()->GetParams()->SetTranslation (Graphic3d_Vec2 (aTx, aTy));
      }
      else if (aCappingArg == "texrotate") // texture rotation
      {
        if (aClipPlane->CappingTexture().IsNull())
        {
          theDi << theArgVec[0] << ": no texture is set.\n";
          return 1;
        }

        if (theArgsNb < 6)
        {
          theDi << theArgVec[0] << ": need more arguments. Type help for more information.\n";
          return 1;
        }

        Standard_ShortReal aRot = (Standard_ShortReal)atof (theArgVec[5]);

        aClipPlane->CappingTexture()->GetParams()->SetRotation (aRot);
      }
      else if (aCappingArg == "hatch") // hatch style
      {
        if (theArgsNb < 6)
        {
          theDi << theArgVec[0] << ": need more arguments. Type help for more information.\n";
          return 1;
        }

        TCollection_AsciiString aHatchStr (theArgVec [5]);
        if (aHatchStr == "on")
        {
          aClipPlane->SetCappingHatchOn();
        }
        else if (aHatchStr == "off")
        {
          aClipPlane->SetCappingHatchOff();
        }
        else
        {
          aClipPlane->SetCappingHatch ((Aspect_HatchStyle)atoi (theArgVec[5]));
        }
      }
    }

    ViewerTest::RedrawAllViews();

    return 0;
  }

  theDi << theArgVec[0] << ": invalid command. Type help for more information.\n";
  return 1;
}

//===============================================================================================
//function : VSetTextureMode
//purpose  :
//===============================================================================================
static int VSetTextureMode (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  if (theArgsNb < 3)
  {
    theDi << theArgVec[0] << ": insufficient command arguments. Type help for more information.\n";
    return 1;
  }

  TCollection_AsciiString aViewName (theArgVec[1]);
  if (!ViewerTest_myViews.IsBound1 (aViewName))
  {
    theDi << theArgVec[0] << ": view is not found.\n";
    return 1;
  }

  const Handle(V3d_View)& aView = ViewerTest_myViews.Find1 (aViewName);
  switch (atoi (theArgVec[2]))
  {
    case 0: aView->SetSurfaceDetail (V3d_TEX_NONE); break;
    case 1: aView->SetSurfaceDetail (V3d_TEX_ENVIRONMENT); break;
    case 2: aView->SetSurfaceDetail (V3d_TEX_ALL); break;
    default:
      theDi << theArgVec[0] << ": invalid mode.\n";
      return 1;
  }

  aView->Redraw();
  return 0;
}

//===============================================================================================
//function : VZRange
//purpose  :
//===============================================================================================
static int VZRange (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  const Handle(V3d_View)& aCurrentView = ViewerTest::CurrentView();

  if (aCurrentView.IsNull())
  {
    std::cout << theArgVec[0] << ": Call vinit before this command, please.\n";
    return 1;
  }

  Handle(Graphic3d_Camera) aCamera = aCurrentView->Camera();

  if (theArgsNb < 2)
  {
    theDi << "ZNear: " << aCamera->ZNear() << "\n";
    theDi << "ZFar: " << aCamera->ZFar() << "\n";
    return 0;
  }

  if (theArgsNb == 3)
  {
    Standard_Real aNewZNear = Draw::Atof (theArgVec[1]);
    Standard_Real aNewZFar  = Draw::Atof (theArgVec[2]);

    if (aNewZNear >= aNewZFar)
    {
      std::cout << theArgVec[0] << ": invalid arguments: znear should be less than zfar.\n";
      return 1;
    }

    if (!aCamera->IsOrthographic() && (aNewZNear <= 0.0 || aNewZFar <= 0.0))
    {
      std::cout << theArgVec[0] << ": invalid arguments: ";
      std::cout << "znear, zfar should be positive for perspective camera.\n";
      return 1;
    }

    aCamera->SetZRange (aNewZNear, aNewZFar);
  }
  else
  {
    std::cout << theArgVec[0] << ": wrong command arguments. Type help for more information.\n";
    return 1;
  }

  aCurrentView->Redraw();

  return 0;
}

//===============================================================================================
//function : VAutoZFit
//purpose  :
//===============================================================================================
static int VAutoZFit (Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  const Handle(V3d_View)& aCurrentView = ViewerTest::CurrentView();

  if (aCurrentView.IsNull())
  {
    std::cout << theArgVec[0] << ": Call vinit before this command, please.\n";
    return 1;
  }

  Standard_Real aScale = aCurrentView->AutoZFitScaleFactor();

  if (theArgsNb > 3)
  {
    std::cout << theArgVec[0] << ": wrong command arguments. Type help for more information.\n";
    return 1;
  }

  if (theArgsNb < 2)
  {
    theDi << "Auto z-fit mode: " << "\n"
          << "On: " << (aCurrentView->AutoZFitMode() ? "enabled" : "disabled") << "\n"
          << "Scale: " << aScale << "\n";
    return 0;
  }

  Standard_Boolean isOn = Draw::Atoi (theArgVec[1]) == 1;

  if (theArgsNb >= 3)
  {
    aScale = Draw::Atoi (theArgVec[2]);
  }

  aCurrentView->SetAutoZFitMode (isOn, aScale);
  aCurrentView->AutoZFit();
  aCurrentView->Redraw();

  return 0;
}

//! Auxiliary function to print projection type
inline const char* projTypeName (Graphic3d_Camera::Projection theProjType)
{
  switch (theProjType)
  {
    case Graphic3d_Camera::Projection_Orthographic: return "orthographic";
    case Graphic3d_Camera::Projection_Perspective:  return "perspective";
    case Graphic3d_Camera::Projection_Stereo:       return "stereoscopic";
    case Graphic3d_Camera::Projection_MonoLeftEye:  return "monoLeftEye";
    case Graphic3d_Camera::Projection_MonoRightEye: return "monoRightEye";
  }
  return "UNKNOWN";
}

//===============================================================================================
//function : VCamera
//purpose  :
//===============================================================================================
static int VCamera (Draw_Interpretor& theDI,
                    Standard_Integer  theArgsNb,
                    const char**      theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cout << "Error: no active view.\n";
    return 1;
  }

  Handle(Graphic3d_Camera) aCamera = aView->Camera();
  if (theArgsNb < 2)
  {
    theDI << "ProjType:   " << projTypeName (aCamera->ProjectionType()) << "\n";
    theDI << "FOVy:       " << aCamera->FOVy() << "\n";
    theDI << "Distance:   " << aCamera->Distance() << "\n";
    theDI << "IOD:        " << aCamera->IOD() << "\n";
    theDI << "IODType:    " << (aCamera->GetIODType() == Graphic3d_Camera::IODType_Absolute   ? "absolute" : "relative") << "\n";
    theDI << "ZFocus:     " << aCamera->ZFocus() << "\n";
    theDI << "ZFocusType: " << (aCamera->ZFocusType() == Graphic3d_Camera::FocusType_Absolute ? "absolute" : "relative") << "\n";
    return 0;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    Standard_CString        anArg = theArgVec[anArgIter];
    TCollection_AsciiString anArgCase (anArg);
    anArgCase.LowerCase();
    if (anArgCase == "-proj"
     || anArgCase == "-projection"
     || anArgCase == "-projtype"
     || anArgCase == "-projectiontype")
    {
      theDI << projTypeName (aCamera->ProjectionType()) << " ";
    }
    else if (anArgCase == "-ortho"
          || anArgCase == "-orthographic")
    {
      aCamera->SetProjectionType (Graphic3d_Camera::Projection_Orthographic);
    }
    else if (anArgCase == "-persp"
          || anArgCase == "-perspective"
          || anArgCase == "-perspmono"
          || anArgCase == "-perspectivemono"
          || anArgCase == "-mono")
    {
      aCamera->SetProjectionType (Graphic3d_Camera::Projection_Perspective);
    }
    else if (anArgCase == "-stereo"
          || anArgCase == "-stereoscopic"
          || anArgCase == "-perspstereo"
          || anArgCase == "-perspectivestereo")
    {
      aCamera->SetProjectionType (Graphic3d_Camera::Projection_Stereo);
    }
    else if (anArgCase == "-left"
          || anArgCase == "-lefteye"
          || anArgCase == "-monoleft"
          || anArgCase == "-monolefteye"
          || anArgCase == "-perpsleft"
          || anArgCase == "-perpslefteye")
    {
      aCamera->SetProjectionType (Graphic3d_Camera::Projection_MonoLeftEye);
    }
    else if (anArgCase == "-right"
          || anArgCase == "-righteye"
          || anArgCase == "-monoright"
          || anArgCase == "-monorighteye"
          || anArgCase == "-perpsright")
    {
      aCamera->SetProjectionType (Graphic3d_Camera::Projection_MonoRightEye);
    }
    else if (anArgCase == "-dist"
          || anArgCase == "-distance")
    {
      Standard_CString anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : NULL;
      if (anArgValue != NULL
      && *anArgValue != '-')
      {
        ++anArgIter;
        aCamera->SetDistance (Draw::Atof (anArgValue));
        continue;
      }
      theDI << aCamera->Distance() << " ";
    }
    else if (anArgCase == "-iod")
    {
      Standard_CString anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : NULL;
      if (anArgValue != NULL
      && *anArgValue != '-')
      {
        ++anArgIter;
        aCamera->SetIOD (aCamera->GetIODType(), Draw::Atof (anArgValue));
        continue;
      }
      theDI << aCamera->IOD() << " ";
    }
    else if (anArgCase == "-iodtype")
    {
      Standard_CString        anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : "";
      TCollection_AsciiString anValueCase (anArgValue);
      anValueCase.LowerCase();
      if (anValueCase == "abs"
       || anValueCase == "absolute")
      {
        ++anArgIter;
        aCamera->SetIOD (Graphic3d_Camera::IODType_Absolute, aCamera->IOD());
        continue;
      }
      else if (anValueCase == "rel"
            || anValueCase == "relative")
      {
        ++anArgIter;
        aCamera->SetIOD (Graphic3d_Camera::IODType_Relative, aCamera->IOD());
        continue;
      }
      else if (*anArgValue != '-')
      {
        std::cout << "Error: unknown IOD type '" << anArgValue << "'\n";
        return 1;
      }
      switch (aCamera->GetIODType())
      {
        case Graphic3d_Camera::IODType_Absolute: theDI << "absolute "; break;
        case Graphic3d_Camera::IODType_Relative: theDI << "relative "; break;
      }
    }
    else if (anArgCase == "-zfocus")
    {
      Standard_CString anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : NULL;
      if (anArgValue != NULL
      && *anArgValue != '-')
      {
        ++anArgIter;
        aCamera->SetZFocus (aCamera->ZFocusType(), Draw::Atof (anArgValue));
        continue;
      }
      theDI << aCamera->ZFocus() << " ";
    }
    else if (anArgCase == "-zfocustype")
    {
      Standard_CString        anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : "";
      TCollection_AsciiString anValueCase (anArgValue);
      anValueCase.LowerCase();
      if (anValueCase == "abs"
       || anValueCase == "absolute")
      {
        ++anArgIter;
        aCamera->SetZFocus (Graphic3d_Camera::FocusType_Absolute, aCamera->ZFocus());
        continue;
      }
      else if (anValueCase == "rel"
            || anValueCase == "relative")
      {
        ++anArgIter;
        aCamera->SetZFocus (Graphic3d_Camera::FocusType_Relative, aCamera->ZFocus());
        continue;
      }
      else if (*anArgValue != '-')
      {
        std::cout << "Error: unknown ZFocus type '" << anArgValue << "'\n";
        return 1;
      }
      switch (aCamera->ZFocusType())
      {
        case Graphic3d_Camera::FocusType_Absolute: theDI << "absolute "; break;
        case Graphic3d_Camera::FocusType_Relative: theDI << "relative "; break;
      }
    }
    else if (anArgCase == "-fov"
          || anArgCase == "-fovy")
    {
      Standard_CString anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : NULL;
      if (anArgValue != NULL
      && *anArgValue != '-')
      {
        ++anArgIter;
        aCamera->SetFOVy (Draw::Atof (anArgValue));
        continue;
      }
      theDI << aCamera->FOVy() << " ";
    }
    else
    {
      std::cout << "Error: unknown argument '" << anArg << "'\n";
      return 1;
    }
  }

  aView->AutoZFit();
  aView->Redraw();

  return 0;
}

//! Parse stereo output mode
inline Standard_Boolean parseStereoMode (Standard_CString      theArg,
                                         Graphic3d_StereoMode& theMode)
{
  TCollection_AsciiString aFlag (theArg);
  aFlag.LowerCase();
  if (aFlag == "quadbuffer")
  {
    theMode = Graphic3d_StereoMode_QuadBuffer;
  }
  else if (aFlag == "anaglyph")
  {
    theMode = Graphic3d_StereoMode_Anaglyph;
  }
  else if (aFlag == "row"
        || aFlag == "rowinterlaced")
  {
    theMode = Graphic3d_StereoMode_RowInterlaced;
  }
  else if (aFlag == "col"
        || aFlag == "colinterlaced"
        || aFlag == "columninterlaced")
  {
    theMode = Graphic3d_StereoMode_ColumnInterlaced;
  }
  else if (aFlag == "chess"
        || aFlag == "chessboard")
  {
    theMode = Graphic3d_StereoMode_ChessBoard;
  }
  else if (aFlag == "sbs"
        || aFlag == "sidebyside")
  {
    theMode = Graphic3d_StereoMode_SideBySide;
  }
  else if (aFlag == "ou"
        || aFlag == "overunder")
  {
    theMode = Graphic3d_StereoMode_OverUnder;
  }
  else if (aFlag == "pageflip"
        || aFlag == "softpageflip")
  {
    theMode = Graphic3d_StereoMode_SoftPageFlip;
  }
  else
  {
    return Standard_False;
  }
  return Standard_True;
}

//! Parse anaglyph filter
inline Standard_Boolean parseAnaglyphFilter (Standard_CString                     theArg,
                                             Graphic3d_RenderingParams::Anaglyph& theFilter)
{
  TCollection_AsciiString aFlag (theArg);
  aFlag.LowerCase();
  if (aFlag == "redcyansimple")
  {
    theFilter = Graphic3d_RenderingParams::Anaglyph_RedCyan_Simple;
  }
  else if (aFlag == "redcyan"
        || aFlag == "redcyanoptimized")
  {
    theFilter = Graphic3d_RenderingParams::Anaglyph_RedCyan_Optimized;
  }
  else if (aFlag == "yellowbluesimple")
  {
    theFilter = Graphic3d_RenderingParams::Anaglyph_YellowBlue_Simple;
  }
  else if (aFlag == "yellowblue"
        || aFlag == "yellowblueoptimized")
  {
    theFilter = Graphic3d_RenderingParams::Anaglyph_YellowBlue_Optimized;
  }
  else if (aFlag == "greenmagenta"
        || aFlag == "greenmagentasimple")
  {
    theFilter = Graphic3d_RenderingParams::Anaglyph_GreenMagenta_Simple;
  }
  else
  {
    return Standard_False;
  }
  return Standard_True;
}

//==============================================================================
//function : VStereo
//purpose  :
//==============================================================================

static int VStereo (Draw_Interpretor& theDI,
                    Standard_Integer  theArgNb,
                    const char**      theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (theArgNb < 2)
  {
    if (aView.IsNull())
    {
      std::cout << "Error: no active viewer!\n";
      return 0;
    }

    Standard_Boolean isActive = ViewerTest_myDefaultCaps.contextStereo;
    theDI << "Stereo " << (isActive ? "ON" : "OFF") << "\n";
    return 0;
  }

  Handle(Graphic3d_Camera) aCamera;
  Graphic3d_RenderingParams*   aParams   = NULL;
  Graphic3d_StereoMode         aMode     = Graphic3d_StereoMode_QuadBuffer;
  if (!aView.IsNull())
  {
    aParams   = &aView->ChangeRenderingParams();
    aMode     = aParams->StereoMode;
    aCamera   = aView->Camera();
  }

  ViewerTest_AutoUpdater anUpdateTool (ViewerTest::GetAISContext(), aView);
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg = theArgVec[anArgIter];
    TCollection_AsciiString aFlag (anArg);
    aFlag.LowerCase();
    if (anUpdateTool.parseRedrawMode (aFlag))
    {
      continue;
    }
    else if (aFlag == "0"
          || aFlag == "off")
    {
      if (++anArgIter < theArgNb)
      {
        std::cout << "Error: wrong number of arguments!\n";
        return 1;
      }

      if (!aCamera.IsNull()
       &&  aCamera->ProjectionType() == Graphic3d_Camera::Projection_Stereo)
      {
        aCamera->SetProjectionType (Graphic3d_Camera::Projection_Perspective);
      }
      ViewerTest_myDefaultCaps.contextStereo = Standard_False;
      return 0;
    }
    else if (aFlag == "1"
          || aFlag == "on")
    {
      if (++anArgIter < theArgNb)
      {
        std::cout << "Error: wrong number of arguments!\n";
        return 1;
      }

      if (!aCamera.IsNull())
      {
        aCamera->SetProjectionType (Graphic3d_Camera::Projection_Stereo);
      }
      ViewerTest_myDefaultCaps.contextStereo = Standard_True;
      return 0;
    }
    else if (aFlag == "-reverse"
          || aFlag == "-reversed"
          || aFlag == "-swap")
    {
      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams->ToReverseStereo = toEnable;
    }
    else if (aFlag == "-noreverse"
          || aFlag == "-noswap")
    {
      Standard_Boolean toDisable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toDisable))
      {
        --anArgIter;
      }
      aParams->ToReverseStereo = !toDisable;
    }
    else if (aFlag == "-mode"
          || aFlag == "-stereomode")
    {
      if (++anArgIter >= theArgNb
      || !parseStereoMode (theArgVec[anArgIter], aMode))
      {
        std::cout << "Error: syntax error at '" << anArg << "'\n";
        return 1;
      }

      if (aMode == Graphic3d_StereoMode_QuadBuffer)
      {
        ViewerTest_myDefaultCaps.contextStereo = Standard_True;
      }
    }
    else if (aFlag == "-anaglyph"
          || aFlag == "-anaglyphfilter")
    {
      Graphic3d_RenderingParams::Anaglyph aFilter = Graphic3d_RenderingParams::Anaglyph_RedCyan_Simple;
      if (++anArgIter >= theArgNb
      || !parseAnaglyphFilter (theArgVec[anArgIter], aFilter))
      {
        std::cout << "Error: syntax error at '" << anArg << "'\n";
        return 1;
      }

      aMode = Graphic3d_StereoMode_Anaglyph;
      aParams->AnaglyphFilter = aFilter;
    }
    else if (parseStereoMode (anArg, aMode)) // short syntax
    {
      if (aMode == Graphic3d_StereoMode_QuadBuffer)
      {
        ViewerTest_myDefaultCaps.contextStereo = Standard_True;
      }
    }
    else
    {
      std::cout << "Error: syntax error at '" << anArg << "'\n";
      return 1;
    }
  }

  if (!aView.IsNull())
  {
    aParams->StereoMode = aMode;
    aCamera->SetProjectionType (Graphic3d_Camera::Projection_Stereo);
  }
  return 0;
}

//===============================================================================================
//function : VDefaults
//purpose  :
//===============================================================================================
static int VDefaults (Draw_Interpretor& theDi,
                      Standard_Integer  theArgsNb,
                      const char**      theArgVec)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cerr << "No active viewer!\n";
    return 1;
  }

  Handle(Prs3d_Drawer) aDefParams = aCtx->DefaultDrawer();
  if (theArgsNb < 2)
  {
    if (aDefParams->TypeOfDeflection() == Aspect_TOD_RELATIVE)
    {
      theDi << "DeflType:           relative\n"
            << "DeviationCoeff:     " << aDefParams->DeviationCoefficient() << "\n";
    }
    else
    {
      theDi << "DeflType:           absolute\n"
            << "AbsoluteDeflection: " << aDefParams->MaximalChordialDeviation() << "\n";
    }
    theDi << "AngularDeflection:  " << (180.0 * aDefParams->HLRAngle() / M_PI) << "\n";
    theDi << "AutoTriangulation:  " << (aDefParams->IsAutoTriangulation() ? "on" : "off") << "\n";
    return 0;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.UpperCase();
    if (anArg == "-ABSDEFL"
     || anArg == "-ABSOLUTEDEFLECTION"
     || anArg == "-DEFL"
     || anArg == "-DEFLECTION")
    {
      if (++anArgIter >= theArgsNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      aDefParams->SetTypeOfDeflection         (Aspect_TOD_ABSOLUTE);
      aDefParams->SetMaximalChordialDeviation (Draw::Atof (theArgVec[anArgIter]));
    }
    else if (anArg == "-RELDEFL"
          || anArg == "-RELATIVEDEFLECTION"
          || anArg == "-DEVCOEFF"
          || anArg == "-DEVIATIONCOEFF"
          || anArg == "-DEVIATIONCOEFFICIENT")
    {
      if (++anArgIter >= theArgsNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      aDefParams->SetTypeOfDeflection     (Aspect_TOD_RELATIVE);
      aDefParams->SetDeviationCoefficient (Draw::Atof (theArgVec[anArgIter]));
    }
    else if (anArg == "-ANGDEFL"
          || anArg == "-ANGULARDEFL"
          || anArg == "-ANGULARDEFLECTION")
    {
      if (++anArgIter >= theArgsNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      // currently HLRDeviationAngle is used instead of DeviationAngle in most places
      aDefParams->SetHLRAngle (M_PI * Draw::Atof (theArgVec[anArgIter]) / 180.0);
    }
    else if (anArg == "-AUTOTR"
          || anArg == "-AUTOTRIANG"
          || anArg == "-AUTOTRIANGULATION")
    {
      if (++anArgIter >= theArgsNb)
      {
        std::cout << "Error: wrong syntax at " << anArg << "\n";
        return 1;
      }
      TCollection_AsciiString aValue (theArgVec[anArgIter]);
      aValue.LowerCase();
      if (aValue == "on"
       || aValue == "1")
      {
        aDefParams->SetAutoTriangulation (Standard_True);
      }
      else if (aValue == "off"
            || aValue == "0")
      {
        aDefParams->SetAutoTriangulation (Standard_False);
      }
    }
    else
    {
      std::cerr << "Warning, unknown argument '" << anArg.ToCString() << "'\n";
    }
  }

  return 0;
}

//! Auxiliary method
inline void addLight (const Handle(V3d_Light)& theLightNew,
                      const Standard_Boolean   theIsGlobal)
{
  if (theLightNew.IsNull())
  {
    return;
  }

  if (theIsGlobal)
  {
    ViewerTest::GetViewerFromContext()->SetLightOn (theLightNew);
  }
  else
  {
    ViewerTest::CurrentView()->SetLightOn (theLightNew);
  }
}

//! Auxiliary method
inline Standard_Integer getLightId (const TCollection_AsciiString& theArgNext)
{
  TCollection_AsciiString anArgNextCase (theArgNext);
  anArgNextCase.UpperCase();
  if (anArgNextCase.Length() > 5
   && anArgNextCase.SubString (1, 5).IsEqual ("LIGHT"))
  {
    return theArgNext.SubString (6, theArgNext.Length()).IntegerValue();
  }
  else
  {
    return theArgNext.IntegerValue();
  }
}

//===============================================================================================
//function : VLight
//purpose  :
//===============================================================================================
static int VLight (Draw_Interpretor& theDi,
                   Standard_Integer  theArgsNb,
                   const char**      theArgVec)
{
  Handle(V3d_View)   aView   = ViewerTest::CurrentView();
  Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext();
  if (aView.IsNull()
   || aViewer.IsNull())
  {
    std::cerr << "No active viewer!\n";
    return 1;
  }

  Standard_Real        anXYZ[3];
  Quantity_Coefficient anAtten[2];
  if (theArgsNb < 2)
  {
    // print lights info
    Standard_Integer aLightId = 0;
    for (aView->InitActiveLights(); aView->MoreActiveLights(); aView->NextActiveLights(), ++aLightId)
    {
      Handle(V3d_Light) aLight = aView->ActiveLight();
      const Quantity_Color aColor = aLight->Color();
      theDi << "Light" << aLightId << "\n";
      switch (aLight->Type())
      {
        case V3d_AMBIENT:
        {
          theDi << "  Type:       Ambient\n";
          theDi << "  Intensity:  " << aLight->Intensity() << "\n";
          break;
        }
        case V3d_DIRECTIONAL:
        {
          Handle(V3d_DirectionalLight) aLightDir = Handle(V3d_DirectionalLight)::DownCast (aLight);
          theDi << "  Type:       Directional\n";
          theDi << "  Intensity:  " << aLight->Intensity() << "\n";
          theDi << "  Headlight:  " << (aLight->Headlight() ? "TRUE" : "FALSE") << "\n";
          theDi << "  Smoothness: " << aLight->Smoothness() << "\n";
          if (!aLightDir.IsNull())
          {
            aLightDir->Position  (anXYZ[0], anXYZ[1], anXYZ[2]);
            theDi << "  Position:   " << anXYZ[0] << ", " << anXYZ[1] << ", " << anXYZ[2] << "\n";
            aLightDir->Direction (anXYZ[0], anXYZ[1], anXYZ[2]);
            theDi << "  Direction:  " << anXYZ[0] << ", " << anXYZ[1] << ", " << anXYZ[2] << "\n";
          }
          break;
        }
        case V3d_POSITIONAL:
        {
          Handle(V3d_PositionalLight) aLightPos = Handle(V3d_PositionalLight)::DownCast (aLight);
          theDi << "  Type:       Positional\n";
          theDi << "  Intensity:  " << aLight->Intensity() << "\n";
          theDi << "  Headlight:  " << (aLight->Headlight() ? "TRUE" : "FALSE") << "\n";
          theDi << "  Smoothness: " << aLight->Smoothness() << "\n";
          if (!aLightPos.IsNull())
          {
            aLightPos->Position  (anXYZ[0], anXYZ[1], anXYZ[2]);
            theDi << "  Position:   " << anXYZ[0] << ", " << anXYZ[1] << ", " << anXYZ[2] << "\n";
            aLightPos->Attenuation (anAtten[0], anAtten[1]);
            theDi << "  Atten.:     " << anAtten[0] << " " << anAtten[1] << "\n";
          }
          break;
        }
        case V3d_SPOT:
        {
          Handle(V3d_SpotLight) aLightSpot = Handle(V3d_SpotLight)::DownCast (aLight);
          theDi << "  Type:       Spot\n";
          theDi << "  Intensity:  " << aLight->Intensity() << "\n";
          theDi << "  Headlight:  " << (aLight->Headlight() ? "TRUE" : "FALSE") << "\n";
          if (!aLightSpot.IsNull())
          {
            aLightSpot->Position  (anXYZ[0], anXYZ[1], anXYZ[2]);
            theDi << "  Position:   " << anXYZ[0] << ", " << anXYZ[1] << ", " << anXYZ[2] << "\n";
            aLightSpot->Direction (anXYZ[0], anXYZ[1], anXYZ[2]);
            theDi << "  Direction:  " << anXYZ[0] << ", " << anXYZ[1] << ", " << anXYZ[2] << "\n";
            aLightSpot->Attenuation (anAtten[0], anAtten[1]);
            theDi << "  Atten.:     " << anAtten[0] << " " << anAtten[1] << "\n";
            theDi << "  Angle:      " << (aLightSpot->Angle() * 180.0 / M_PI) << "\n";
            theDi << "  Exponent:   " << aLightSpot->Concentration() << "\n";
          }
          break;
        }
        default:
        {
          theDi << "  Type:       UNKNOWN\n";
          break;
        }
      }
      theDi << "  Color:     " << aColor.Red() << ", " << aColor.Green() << ", " << aColor.Blue() << "\n";
    }
  }

  Handle(V3d_Light) aLightNew;
  Handle(V3d_Light) aLightOld;
  Standard_Boolean  isGlobal = Standard_True;
  Standard_Boolean  toCreate = Standard_False;
  for (Standard_Integer anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    Handle(V3d_Light)            aLightCurr = aLightNew.IsNull() ? aLightOld : aLightNew;
    Handle(V3d_AmbientLight)     aLightAmb  = Handle(V3d_AmbientLight)    ::DownCast (aLightCurr);
    Handle(V3d_DirectionalLight) aLightDir  = Handle(V3d_DirectionalLight)::DownCast (aLightCurr);
    Handle(V3d_PositionalLight)  aLightPos  = Handle(V3d_PositionalLight) ::DownCast (aLightCurr);
    Handle(V3d_SpotLight)        aLightSpot = Handle(V3d_SpotLight)       ::DownCast (aLightCurr);

    TCollection_AsciiString aName, aValue;
    const TCollection_AsciiString anArg (theArgVec[anArgIt]);
    TCollection_AsciiString anArgCase (anArg);
    anArgCase.UpperCase();
    if (anArgCase.IsEqual ("NEW")
     || anArgCase.IsEqual ("ADD")
     || anArgCase.IsEqual ("CREATE"))
    {
      toCreate = Standard_True;
    }
    else if (anArgCase.IsEqual ("GLOB")
          || anArgCase.IsEqual ("GLOBAL"))
    {
      isGlobal = Standard_True;
    }
    else if (anArgCase.IsEqual ("LOC")
          || anArgCase.IsEqual ("LOCAL"))
    {
      isGlobal = Standard_False;
    }
    else if (anArgCase.IsEqual ("DEF")
          || anArgCase.IsEqual ("DEFAULTS"))
    {
      toCreate = Standard_False;
      aViewer->SetDefaultLights();
    }
    else if (anArgCase.IsEqual ("CLR")
          || anArgCase.IsEqual ("CLEAR"))
    {
      toCreate = Standard_False;
      aView->InitActiveLights();
      while (aView->MoreActiveLights())
      {
        aViewer->DelLight (aView->ActiveLight());
        aView->InitActiveLights();
      }
    }
    else if (anArgCase.IsEqual ("AMB")
          || anArgCase.IsEqual ("AMBIENT")
          || anArgCase.IsEqual ("AMBLIGHT"))
    {
      addLight (aLightNew, isGlobal);
      if (!toCreate)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }
      toCreate  = Standard_False;
      aLightNew = new V3d_AmbientLight (aViewer);
    }
    else if (anArgCase.IsEqual ("DIRECTIONAL")
          || anArgCase.IsEqual ("DIRLIGHT"))
    {
      addLight (aLightNew, isGlobal);
      if (!toCreate)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }
      toCreate  = Standard_False;
      aLightNew = new V3d_DirectionalLight (aViewer);
    }
    else if (anArgCase.IsEqual ("SPOT")
          || anArgCase.IsEqual ("SPOTLIGHT"))
    {
      addLight (aLightNew, isGlobal);
      if (!toCreate)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }
      toCreate  = Standard_False;
      aLightNew = new V3d_SpotLight (aViewer, 0.0, 0.0, 0.0);
    }
    else if (anArgCase.IsEqual ("POSLIGHT")
          || anArgCase.IsEqual ("POSITIONAL"))
    {
      addLight (aLightNew, isGlobal);
      if (!toCreate)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }
      toCreate  = Standard_False;
      aLightNew = new V3d_PositionalLight (aViewer, 0.0, 0.0, 0.0);
    }
    else if (anArgCase.IsEqual ("CHANGE"))
    {
      addLight (aLightNew, isGlobal);
      aLightNew.Nullify();
      if (++anArgIt >= theArgsNb)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      const Standard_Integer aLightId = getLightId (theArgVec[anArgIt]);
      Standard_Integer aLightIt = 0;
      for (aView->InitActiveLights(); aView->MoreActiveLights(); aView->NextActiveLights(), ++aLightIt)
      {
        if (aLightIt == aLightId)
        {
          aLightOld = aView->ActiveLight();
          break;
        }
      }

      if (aLightOld.IsNull())
      {
        std::cerr << "Light " << theArgVec[anArgIt] << " is undefined!\n";
        return 1;
      }
    }
    else if (anArgCase.IsEqual ("DEL")
          || anArgCase.IsEqual ("DELETE"))
    {
      Handle(V3d_Light) aLightDel;
      if (++anArgIt >= theArgsNb)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      const TCollection_AsciiString anArgNext (theArgVec[anArgIt]);
      const Standard_Integer aLightDelId = getLightId (theArgVec[anArgIt]);
      Standard_Integer aLightIt = 0;
      for (aView->InitActiveLights(); aView->MoreActiveLights(); aView->NextActiveLights(), ++aLightIt)
      {
        aLightDel = aView->ActiveLight();
        if (aLightIt == aLightDelId)
        {
          break;
        }
      }
      if (!aLightDel.IsNull())
      {
        aViewer->DelLight (aLightDel);
      }
    }
    else if (anArgCase.IsEqual ("COLOR")
          || anArgCase.IsEqual ("COLOUR"))
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      TCollection_AsciiString anArgNext (theArgVec[anArgIt]);
      anArgNext.UpperCase();
      const Quantity_Color aColor = ViewerTest::GetColorFromName (anArgNext.ToCString());
      if (!aLightCurr.IsNull())
      {
        aLightCurr->SetColor (aColor);
      }
    }
    else if (anArgCase.IsEqual ("POS")
          || anArgCase.IsEqual ("POSITION"))
    {
      if ((anArgIt + 3) >= theArgsNb)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      anXYZ[0] = Atof (theArgVec[++anArgIt]);
      anXYZ[1] = Atof (theArgVec[++anArgIt]);
      anXYZ[2] = Atof (theArgVec[++anArgIt]);
      if (!aLightDir.IsNull())
      {
        aLightDir->SetPosition (anXYZ[0], anXYZ[1], anXYZ[2]);
      }
      else if (!aLightPos.IsNull())
      {
        aLightPos->SetPosition (anXYZ[0], anXYZ[1], anXYZ[2]);
      }
      else if (!aLightSpot.IsNull())
      {
        aLightSpot->SetPosition (anXYZ[0], anXYZ[1], anXYZ[2]);
      }
      else
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }
    }
    else if (anArgCase.IsEqual ("DIR")
          || anArgCase.IsEqual ("DIRECTION"))
    {
      if ((anArgIt + 3) >= theArgsNb)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      anXYZ[0] = Atof (theArgVec[++anArgIt]);
      anXYZ[1] = Atof (theArgVec[++anArgIt]);
      anXYZ[2] = Atof (theArgVec[++anArgIt]);
      if (!aLightDir.IsNull())
      {
        aLightDir->SetDirection (anXYZ[0], anXYZ[1], anXYZ[2]);
      }
      else if (!aLightSpot.IsNull())
      {
        aLightSpot->SetDirection (anXYZ[0], anXYZ[1], anXYZ[2]);
      }
      else
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }
    }
    else if (anArgCase.IsEqual ("SM")
          || anArgCase.IsEqual ("SMOOTHNESS"))
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      Standard_Real aSmoothness = Atof (theArgVec[anArgIt]);

      if (fabs (aSmoothness) < Precision::Confusion())
      {
        aLightCurr->SetIntensity (1.f);
      }
      else if (fabs (aLightCurr->Smoothness()) < Precision::Confusion())
      {
        aLightCurr->SetIntensity ((aSmoothness * aSmoothness) / 3.f);
      }
      else
      {
        Standard_ShortReal aSmoothnessRatio = static_cast<Standard_ShortReal> (aSmoothness / aLightCurr->Smoothness());
        aLightCurr->SetIntensity (aLightCurr->Intensity() / (aSmoothnessRatio * aSmoothnessRatio));
      }

      if (!aLightPos.IsNull())
      {
        aLightPos->SetSmoothRadius (aSmoothness);
      }
      else if (!aLightDir.IsNull())
      {
        aLightDir->SetSmoothAngle (aSmoothness);
      }
    }
    else if (anArgCase.IsEqual ("INT")
          || anArgCase.IsEqual ("INTENSITY"))
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      Standard_Real aIntensity = Atof (theArgVec[anArgIt]);

      if (!aLightCurr.IsNull())
      {
        aLightCurr->SetIntensity (aIntensity);
      }
    }
    else if (anArgCase.IsEqual ("ANG")
          || anArgCase.IsEqual ("ANGLE"))
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      Standard_Real anAngle = Atof (theArgVec[anArgIt]);

      if (!aLightSpot.IsNull())
      {
        aLightSpot->SetAngle (anAngle / 180.0 * M_PI);
      }
    }
    else if (anArgCase.IsEqual ("CONSTATTEN")
          || anArgCase.IsEqual ("CONSTATTENUATION"))
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      if (!aLightPos.IsNull())
      {
        aLightPos->Attenuation (anAtten[0], anAtten[1]);
        anAtten[0] = Atof (theArgVec[anArgIt]);
        aLightPos->SetAttenuation (anAtten[0], anAtten[1]);
      }
      else if (!aLightSpot.IsNull())
      {
        aLightSpot->Attenuation (anAtten[0], anAtten[1]);
        anAtten[0] = Atof (theArgVec[anArgIt]);
        aLightSpot->SetAttenuation (anAtten[0], anAtten[1]);
      }
      else
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }
    }
    else if (anArgCase.IsEqual ("LINATTEN")
          || anArgCase.IsEqual ("LINEARATTEN")
          || anArgCase.IsEqual ("LINEARATTENUATION"))
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      if (!aLightPos.IsNull())
      {
        aLightPos->Attenuation (anAtten[0], anAtten[1]);
        anAtten[1] = Atof (theArgVec[anArgIt]);
        aLightPos->SetAttenuation (anAtten[0], anAtten[1]);
      }
      else if (!aLightSpot.IsNull())
      {
        aLightSpot->Attenuation (anAtten[0], anAtten[1]);
        anAtten[1] = Atof (theArgVec[anArgIt]);
        aLightSpot->SetAttenuation (anAtten[0], anAtten[1]);
      }
      else
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }
    }
    else if (anArgCase.IsEqual ("EXP")
          || anArgCase.IsEqual ("EXPONENT")
          || anArgCase.IsEqual ("SPOTEXP")
          || anArgCase.IsEqual ("SPOTEXPONENT"))
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      if (!aLightSpot.IsNull())
      {
        aLightSpot->SetConcentration (Atof (theArgVec[anArgIt]));
      }
      else
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }
    }
    else if (anArgCase.IsEqual ("HEAD")
          || anArgCase.IsEqual ("HEADLIGHT"))
    {
      if (++anArgIt >= theArgsNb)
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }

      if (aLightAmb.IsNull()
       && !aLightCurr.IsNull())
      {
        aLightCurr->SetHeadlight (Draw::Atoi (theArgVec[anArgIt]) != 0);
      }
      else
      {
        std::cerr << "Wrong syntax at argument '" << anArg << "'!\n";
        return 1;
      }
    }
    else
    {
      std::cerr << "Warning: unknown argument '" << anArg << "'\n";
    }
  }

  addLight (aLightNew, isGlobal);
  aViewer->UpdateLights();

  return 0;
}

//=======================================================================
//function : VRenderParams
//purpose  : Enables/disables rendering features
//=======================================================================

static Standard_Integer VRenderParams (Draw_Interpretor& theDI,
                                       Standard_Integer  theArgNb,
                                       const char**      theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << "Error: no active viewer!\n";
    return 1;
  }

  Graphic3d_RenderingParams& aParams = aView->ChangeRenderingParams();
  TCollection_AsciiString aCmdName (theArgVec[0]);
  aCmdName.LowerCase();
  if (aCmdName == "vraytrace")
  {
    if (theArgNb == 1)
    {
      theDI << (aParams.Method == Graphic3d_RM_RAYTRACING ? "on" : "off") << " ";
      return 0;
    }
    else if (theArgNb == 2)
    {
      TCollection_AsciiString aValue (theArgVec[1]);
      aValue.LowerCase();
      if (aValue == "on"
       || aValue == "1")
      {
        aParams.Method = Graphic3d_RM_RAYTRACING;
        aView->Redraw();
        return 0;
      }
      else if (aValue == "off"
            || aValue == "0")
      {
        aParams.Method = Graphic3d_RM_RASTERIZATION;
        aView->Redraw();
        return 0;
      }
      else
      {
        std::cout << "Error: unknown argument '" << theArgVec[1] << "'\n";
        return 1;
      }
    }
    else
    {
      std::cout << "Error: wrong number of arguments\n";
      return 1;
    }
  }

  if (theArgNb < 2)
  {
    theDI << "renderMode:  ";
    switch (aParams.Method)
    {
      case Graphic3d_RM_RASTERIZATION: theDI << "rasterization "; break;
      case Graphic3d_RM_RAYTRACING:    theDI << "raytrace ";      break;
    }
    theDI << "\n";
    theDI << "fsaa:         " << (aParams.IsAntialiasingEnabled       ? "on" : "off") << "\n";
    theDI << "shadows:      " << (aParams.IsShadowEnabled             ? "on" : "off") << "\n";
    theDI << "reflections:  " << (aParams.IsReflectionEnabled         ? "on" : "off") << "\n";
    theDI << "rayDepth:     " <<  aParams.RaytracingDepth                             << "\n";
    theDI << "gleam:        " << (aParams.IsTransparentShadowEnabled  ? "on" : "off") << "\n";
    theDI << "GI:           " << (aParams.IsGlobalIlluminationEnabled ? "on" : "off") << "\n";
    theDI << "blocked RNG:  " << (aParams.CoherentPathTracingMode     ? "on" : "off") << "\n";
    theDI << "shadingModel: ";
    switch (aView->ShadingModel())
    {
      case V3d_COLOR:   theDI << "color";   break;
      case V3d_FLAT:    theDI << "flat";    break;
      case V3d_GOURAUD: theDI << "gouraud"; break;
      case V3d_PHONG:   theDI << "phong";   break;
    }
    theDI << "\n";
    return 0;
  }

  Standard_Boolean toPrint = Standard_False;
  ViewerTest_AutoUpdater anUpdateTool (ViewerTest::GetAISContext(), aView);
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg (theArgVec[anArgIter]);
    TCollection_AsciiString aFlag (anArg);
    aFlag.LowerCase();
    if (anUpdateTool.parseRedrawMode (aFlag))
    {
      continue;
    }
    else if (aFlag == "-echo"
          || aFlag == "-print")
    {
      toPrint = Standard_True;
      anUpdateTool.Invalidate();
    }
    else if (aFlag == "-mode"
          || aFlag == "-rendermode"
          || aFlag == "-render_mode")
    {
      if (toPrint)
      {
        switch (aParams.Method)
        {
          case Graphic3d_RM_RASTERIZATION: theDI << "rasterization "; break;
          case Graphic3d_RM_RAYTRACING:    theDI << "ray-tracing ";   break;
        }
        continue;
      }
      else
      {
        std::cerr << "Error: wrong syntax at argument '" << anArg << "'\n";
        return 1;
      }
    }
    else if (aFlag == "-ray"
          || aFlag == "-raytrace")
    {
      if (toPrint)
      {
        theDI << (aParams.Method == Graphic3d_RM_RAYTRACING ? "true" : "false") << " ";
        continue;
      }

      aParams.Method = Graphic3d_RM_RAYTRACING;
    }
    else if (aFlag == "-rast"
          || aFlag == "-raster"
          || aFlag == "-rasterization")
    {
      if (toPrint)
      {
        theDI << (aParams.Method == Graphic3d_RM_RASTERIZATION ? "true" : "false") << " ";
        continue;
      }

      aParams.Method = Graphic3d_RM_RASTERIZATION;
    }
    else if (aFlag == "-raydepth"
          || aFlag == "-ray_depth")
    {
      if (toPrint)
      {
        theDI << aParams.RaytracingDepth << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        std::cerr << "Error: wrong syntax at argument '" << anArg << "'\n";
        return 1;
      }

      const Standard_Integer aDepth = Draw::Atoi (theArgVec[anArgIter]);

      // We allow RaytracingDepth be more than 10 in case of GI enabled
      if (aDepth < 1 || (aDepth > 10 && !aParams.IsGlobalIlluminationEnabled))
      {
        std::cerr << "Error: invalid ray-tracing depth " << aDepth << ". Should be within range [1; 10]\n";
        return 1;
      }
      else
      {
        aParams.RaytracingDepth = aDepth;
      }
    }
    else if (aFlag == "-shad"
          || aFlag == "-shadows")
    {
      if (toPrint)
      {
        theDI << (aParams.IsShadowEnabled ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.IsShadowEnabled = toEnable;
    }
    else if (aFlag == "-refl"
          || aFlag == "-reflections")
    {
      if (toPrint)
      {
        theDI << (aParams.IsReflectionEnabled ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.IsReflectionEnabled = toEnable;
    }
    else if (aFlag == "-fsaa")
    {
      if (toPrint)
      {
        theDI << (aParams.IsAntialiasingEnabled ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.IsAntialiasingEnabled = toEnable;
    }
    else if (aFlag == "-gleam")
    {
      if (toPrint)
      {
        theDI << (aParams.IsTransparentShadowEnabled ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.IsTransparentShadowEnabled = toEnable;
    }
    else if (aFlag == "-gi")
    {
      if (toPrint)
      {
        theDI << (aParams.IsGlobalIlluminationEnabled ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
      && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.IsGlobalIlluminationEnabled = toEnable;
      if (!toEnable)
      {
        aParams.RaytracingDepth = Min (aParams.RaytracingDepth, 10);
      }
    }
    else if (aFlag == "-blockedrng"
          || aFlag == "-brng")
    {
      if (toPrint)
      {
        theDI << (aParams.CoherentPathTracingMode ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
        && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.CoherentPathTracingMode = toEnable;
    }
    else if (aFlag == "-env")
    {
      if (toPrint)
      {
        theDI << (aParams.UseEnvironmentMapBackground ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb
        && !ViewerTest::ParseOnOff (theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.UseEnvironmentMapBackground = toEnable;
    }
    else if (aFlag == "-shademodel"
          || aFlag == "-shadingmodel"
          || aFlag == "-shading")
    {
      if (toPrint)
      {
        switch (aView->ShadingModel())
        {
          case V3d_COLOR:   theDI << "color ";   break;
          case V3d_FLAT:    theDI << "flat ";    break;
          case V3d_GOURAUD: theDI << "gouraud "; break;
          case V3d_PHONG:   theDI << "phong ";   break;
        }
        continue;
      }

      if (++anArgIter >= theArgNb)
      {
        std::cerr << "Error: wrong syntax at argument '" << anArg << "'\n";
      }

      TCollection_AsciiString aMode (theArgVec[anArgIter]);
      aMode.LowerCase();
      if (aMode == "color"
       || aMode == "none")
      {
        aView->SetShadingModel (V3d_COLOR);
      }
      else if (aMode == "flat"
            || aMode == "facet")
      {
        aView->SetShadingModel (V3d_FLAT);
      }
      else if (aMode == "gouraud"
            || aMode == "vertex"
            || aMode == "vert")
      {
        aView->SetShadingModel (V3d_GOURAUD);
      }
      else if (aMode == "phong"
            || aMode == "fragment"
            || aMode == "frag"
            || aMode == "pixel")
      {
        aView->SetShadingModel (V3d_PHONG);
      }
      else
      {
        std::cout << "Error: unknown shading model '" << aMode << "'\n";
        return 1;
      }
    }
    else if (aFlag == "-resolution")
    {
      if (++anArgIter >= theArgNb)
      {
        std::cerr << "Error: wrong syntax at argument '" << anArg << "'\n";
        return 1;
      }

      TCollection_AsciiString aResolution (theArgVec[anArgIter]);
      if (aResolution.IsIntegerValue())
      {
        aView->ChangeRenderingParams().Resolution = static_cast<unsigned int> (Draw::Atoi (aResolution.ToCString()));
      }
      else
      {
        std::cout << "Error: wrong syntax at argument'" << anArg << "'.\n";
        return 1;
      }
    }
    else
    {
      std::cout << "Error: wrong syntax, unknown flag '" << anArg << "'\n";
      return 1;
    }
  }

  return 0;
}

//=======================================================================
//function : VProgressiveMode
//purpose  :
//=======================================================================
#if defined(_WIN32)
static Standard_Integer VProgressiveMode (Draw_Interpretor& /*theDI*/,
                                          Standard_Integer  /*theNbArgs*/,
                                          const char**      /*theArgs*/)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << "Error: no active viewer!\n";
    return 1;
  }

  std::cout << "Press Enter or Escape key to exit progressive rendering mode" << std::endl;

  for (;;)
  {
    aView->Redraw();

    Standard_Boolean toExit = Standard_False;

    MSG aMsg;
    while (PeekMessage (&aMsg, NULL, NULL, NULL, PM_REMOVE))
    {
      if (aMsg.message == WM_KEYDOWN && (aMsg.wParam == 0x0d || aMsg.wParam == 0x1b))
      {
        toExit = Standard_True;
      }

      TranslateMessage (&aMsg);
      DispatchMessage  (&aMsg);
    }

    if (toExit)
    {
      break;
    }
  }

  return 0;
}
#endif

//=======================================================================
//function : VFrustumCulling
//purpose  : enables/disables view volume's culling.
//=======================================================================
static int VFrustumCulling (Draw_Interpretor& theDI,
                            Standard_Integer  theArgNb,
                            const char**      theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cout << theArgVec[0] << " Error: Use 'vinit' command before\n";
    return 1;
  }

  if (theArgNb < 2)
  {
    theDI << (aView->IsCullingEnabled() ? "on" : "off");
    return 0;
  }
  else if (theArgNb != 2)
  {
    std::cout << theArgVec[0] << " Syntax error: Specify the mode\n";
    return 1;
  }

  TCollection_AsciiString aModeStr (theArgVec[1]);
  aModeStr.LowerCase();
  Standard_Boolean toEnable = 0;
  if (aModeStr == "on")
  {
    toEnable = 1;
  }
  else if (aModeStr == "off")
  {
    toEnable = 0;
  }
  else
  {
    toEnable = Draw::Atoi (theArgVec[1]) != 0;
  }

  aView->SetFrustumCulling (toEnable);
  aView->Redraw();
  return 0;
}

//=======================================================================
//function : VHighlightSelected
//purpose  : 
//=======================================================================
static int VHighlightSelected (Draw_Interpretor& theDI,
                               Standard_Integer  theArgNb,
                               const char**      theArgVec)
{
  if (ViewerTest::GetAISContext().IsNull())
  {
    std::cout << theArgVec[0] << " error : Context is not created. Please call vinit before.\n";
    return 1;
  }

  const Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();

  if (theArgNb < 2)
  {
    theDI << (aContext->ToHilightSelected() ? "on" : "off");
    return 0;
  }

  if (theArgNb != 2)
  {
    std::cout  << theArgVec[0] << " error : wrong number of parameters."
          << "Type 'help" << theArgVec[0] << "' for more information.";
    return 1;
  }

  // Parse parameter
  TCollection_AsciiString aMode (theArgVec[1]);
  aMode.LowerCase();
  Standard_Boolean toEnable = Standard_False;
  if (aMode.IsEqual ("on"))
  {
    toEnable = Standard_True;
  }
  else if (aMode.IsEqual ("off"))
  {
    toEnable = Standard_False;
  }
  else
  {
    toEnable = Draw::Atoi (theArgVec[1]) != 0;
  }

  if (toEnable != aContext->ToHilightSelected())
  {
    aContext->SetToHilightSelected (toEnable);

    // Move cursor to null position and  back to process updating of detection
    // and highlighting of selected object immediatly.
    Standard_Integer aPixX = 0;
    Standard_Integer aPixY = 0;
    const Handle(ViewerTest_EventManager)& anEventManager =  ViewerTest::CurrentEventManager();

    anEventManager->GetCurrentPosition (aPixX, aPixY);
    anEventManager->MoveTo (0, 0);
    anEventManager->MoveTo (aPixX, aPixY);
  }

  return 0;
}

//=======================================================================
//function : VXRotate
//purpose  :
//=======================================================================
static Standard_Integer VXRotate (Draw_Interpretor& di,
                                   Standard_Integer argc,
                                   const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << argv[0] << "ERROR : use 'vinit' command before " << "\n";
    return 1;
  }
  
  if (argc != 3)
  {
    di << "ERROR : Usage : " << argv[0] << " name angle" << "\n";
    return 1;
  }

  TCollection_AsciiString aName (argv[1]);
  Standard_Real anAngle = Draw::Atof (argv[2]);

  // find object
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  Handle(AIS_InteractiveObject) anIObj;
  if (!aMap.IsBound2 (aName) )
  {
    di << "Use 'vdisplay' before" << "\n";
    return 1;
  }
  else
  {
    anIObj = Handle(AIS_InteractiveObject)::DownCast (aMap.Find2 (aName));

    gp_Trsf aTransform;
    aTransform.SetRotation (gp_Ax1 (gp_Pnt (0.0, 0.0, 0.0), gp_Vec (1.0, 0.0, 0.0)), anAngle);
    aTransform.SetTranslationPart (anIObj->LocalTransformation().TranslationPart());

    aContext->SetLocation (anIObj, aTransform);
    aContext->UpdateCurrentViewer();
  }

  return 0;
}

//=======================================================================
//function : ViewerCommands
//purpose  :
//=======================================================================

void ViewerTest::ViewerCommands(Draw_Interpretor& theCommands)
{

  const char *group = "ZeViewer";
  theCommands.Add("vinit",
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
    "[name=view_name] [display=display_name] [l=leftPx t=topPx] [w=widthPx h=heightPx]\n"
#else
    "[name=view_name] [l=leftPx t=topPx] [w=widthPx h=heightPx]\n"
#endif
    " - Creates new View window with specified name view_name.\n"
    "By default the new view is created in the viewer and in"
    " graphic driver shared with active view.\n"
    " - name = {driverName/viewerName/viewName | viewerName/viewName | viewName}.\n"
    "If driverName isn't specified the driver will be shared with active view.\n"
    "If viewerName isn't specified the viewer will be shared with active view.\n"
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
    " - display = HostName.DisplayNumber[:ScreenNumber] : if specified"
    "is used in creation of graphic driver\n"
#endif
    " - l, t: pixel position of left top corner of the window\n"
    " - w,h: width and heigth of window respectively.\n"
    "Additional commands for operations with views: vclose, vactivate, vviewlist.\n",
    __FILE__,VInit,group);
  theCommands.Add("vclose" ,
    "[view_id [keep_context=0|1]]\n"
    "or vclose ALL - to remove all created views\n"
    " - removes view(viewer window) defined by its view_id.\n"
    " - keep_context: by default 0; if 1 and the last view is deleted"
    " the current context is not removed.",
    __FILE__,VClose,group);
  theCommands.Add("vactivate" ,
    "view_id"
    " - activates view(viewer window) defined by its view_id",
    __FILE__,VActivate,group);
  theCommands.Add("vviewlist",
    "vviewlist [format={tree, long}]"
    " - prints current list of views per viewer and graphic_driver ID shared between viewers"
    " - format: format of result output, if tree the output is a tree view;"
    "otherwise it's a list of full view names. By default format = tree",
    __FILE__,VViewList,group);
  theCommands.Add("vhelp" ,
    "vhelp            : display help on the viewer commands",
    __FILE__,VHelp,group);
  theCommands.Add("vtop" ,
    "vtop or <T>      : Top view. Orientation +X+Y" ,
    __FILE__,VTop,group);
  theCommands.Add("vbottom" ,
    "vbottom          : Bottom view. Orientation +X-Y" ,
    __FILE__,VBottom,group);
  theCommands.Add("vleft" ,
    "vleft            : Left view. Orientation -Y+Z" ,
    __FILE__,VLeft,group);
  theCommands.Add("vright" ,
    "vright           : Right view. Orientation +Y+Z" ,
    __FILE__,VRight,group);
  theCommands.Add("vaxo" ,
    " vaxo or <A>     : Axonometric view. Orientation +X-Y+Z",
    __FILE__,VAxo,group);
  theCommands.Add("vfront" ,
    "vfront           : Front view. Orientation +X+Z" ,
    __FILE__,VFront,group);
  theCommands.Add("vback" ,
    "vback            : Back view. Orientation -X+Z" ,
    __FILE__,VBack,group);
  theCommands.Add("vpick" ,
    "vpick           : vpick X Y Z [shape subshape] ( all variables as string )",
    VPick,group);
  theCommands.Add("vfit"    ,
    "vfit or <F> [-selected]"
    "\n\t\t: [-selected] fits the scene according to bounding box of currently selected objects",
    __FILE__,VFit,group);
  theCommands.Add ("vfitarea",
    "vfitarea x1 y1 x2 y2"
    "\n\t\t: vfitarea x1 y1 z1 x2 y2 z2"
    "\n\t\t: Fit view to show area located between two points"
    "\n\t\t: given in world 2D or 3D corrdinates.",
    __FILE__, VFitArea, group);
  theCommands.Add ("vzfit", "vzfit [scale]\n"
    "   Matches Z near, Z far view volume planes to the displayed objects.\n"
    "   \"scale\" - specifies factor to scale computed z range.\n",
    __FILE__, VZFit, group);
  theCommands.Add("vrepaint",
    "vrepaint        : vrepaint, force redraw",
    __FILE__,VRepaint,group);
  theCommands.Add("vclear",
    "vclear          : vclear"
    "\n\t\t: remove all the object from the viewer",
    __FILE__,VClear,group);
  theCommands.Add("vsetbg",
    "vsetbg          : vsetbg imagefile [filltype] : Load image as background",
    __FILE__,VSetBg,group);
  theCommands.Add("vsetbgmode",
    "vsetbgmode      : vsetbgmode filltype : Change background image fill type",
    __FILE__,VSetBgMode,group);
  theCommands.Add("vsetgradientbg",
    "vsetgradientbg  : vsetgradientbg r1 g1 b1 r2 g2 b2 filltype : Mount gradient background",
    __FILE__,VSetGradientBg,group);
  theCommands.Add("vsetgrbgmode",
    "vsetgrbgmode    : vsetgrbgmode filltype : Change gradient background fill type",
    __FILE__,VSetGradientBgMode,group);
  theCommands.Add("vsetcolorbg",
    "vsetcolorbg     : vsetcolorbg r g b : Set background color",
    __FILE__,VSetColorBg,group);
  theCommands.Add("vsetdefaultbg",
    "vsetdefaultbg r g b\n"
    "\n\t\t: vsetdefaultbg r1 g1 b1 r2 g2 b2 fillmode"
    "\n\t\t: Set default viewer background fill color (flat/gradient).",
    __FILE__,VSetDefaultBg,group);
  theCommands.Add("vscale",
    "vscale          : vscale X Y Z",
    __FILE__,VScale,group);
  theCommands.Add("vzbufftrihedron",
            "vzbufftrihedron [{-on|-off}=-on] [-type {wireframe|zbuffer}=zbuffer]"
    "\n\t\t:       [-position center|left_lower|left_upper|right_lower|right_upper]"
    "\n\t\t:       [-scale value=0.1] [-size value=0.8] [-arrowDiam value=0.05]"
    "\n\t\t:       [-colorArrowX color=RED] [-colorArrowY color=GREEN] [-colorArrowZ color=BLUE]"
    "\n\t\t:       [-nbfacets value=12] [-colorLabels color=WHITE]"
    "\n\t\t: Displays a trihedron",
    __FILE__,VZBuffTrihedron,group);
  theCommands.Add("vrotate",
    "vrotate [[-mouseStart X Y] [-mouseMove X Y]]|[AX AY AZ [X Y Z]]"
    "\n                : Option -mouseStart starts rotation according to the mouse position"
    "\n                : Option -mouseMove continues rotation with angle computed"
    "\n                : from last and new mouse position."
    "\n                : vrotate AX AY AZ [X Y Z]",
    __FILE__,VRotate,group);
  theCommands.Add("vzoom",
    "vzoom           : vzoom coef",
    __FILE__,VZoom,group);
  theCommands.Add("vpan",
    "vpan            : vpan dx dy",
    __FILE__,VPan,group);
  theCommands.Add("vexport",
    "vexport         : vexport full_file_path {PS | EPS | TEX | PDF | SVG | PGF | EMF }"
    " : exports the view to a vector file of a given format"
    " : notice that EMF format requires patched gl2ps",
    __FILE__,VExport,group);
  theCommands.Add("vcolorscale",
    "vcolorscale     : vcolorscale name [-range RangeMin = 0 RangeMax = 100 Intervals = 10 -font HeightFont = 16  -textpos "
    "Position = left -xy X = 0 Y = 0] [-noupdate|-update]: draw color scale\n"
    "-demo/-demoversion draw a demoversion of color scale.\n"
    "-show/display display color scale.\n"
    "-hide/erase erase color scale.\n"
    "Please note that -show/-hide option must be the first argument!\n"
    "-color Index R G B: set color for indexed interval\n"
    "-color Index ColorName: set color for indexed interval\n"
    "-colors R G B R G B ...: set colors for all intervals\n"
    "-colors ColorName1 ColorName2 ...: set colors for all intervals\n"
    "-colors supports both color names and rgb values in one call\n"
    "-label Index Text: set label for indexed interval\n"
    "-labels Text Text Text ...: set labels for all intervals\n"
    "-title Title [Position]: set the title for color scale with certain position. Default position = center;\n"
    "Available text positions: left, right, center, none;\n",
    __FILE__,VColorScale,group);
  theCommands.Add("vgraduatedtrihedron",
    "vgraduatedtrihedron : -on/-off [-xname Name] [-yname Name] [-zname Name] [-arrowlength Value]\n"
    "\t[-namefont Name] [-valuesfont Name]\n"
    "\t[-xdrawname on/off] [-ydrawname on/off] [-zdrawname on/off]\n"
    "\t[-xnameoffset IntVal] [-ynameoffset IntVal] [-znameoffset IntVal]"
    "\t[-xnamecolor Color] [-ynamecolor Color] [-znamecolor Color]\n"
    "\t[-xdrawvalues on/off] [-ydrawvalues on/off] [-zdrawvalues on/off]\n"
    "\t[-xvaluesoffset IntVal] [-yvaluesoffset IntVal] [-zvaluesoffset IntVal]"
    "\t[-xcolor Color] [-ycolor Color] [-zcolor Color]\n"
    "\t[-xdrawticks on/off] [-ydrawticks on/off] [-zdrawticks on/off]\n"
    "\t[-xticks Number] [-yticks Number] [-zticks Number]\n"
    "\t[-xticklength IntVal] [-yticklength IntVal] [-zticklength IntVal]\n"
    "\t[-drawgrid on/off] [-drawaxes on/off]\n"
    " - Displays or erases graduated trihedron"
    " - xname, yname, zname - names of axes, default: X, Y, Z\n"
    " - namefont - font of axes names. Default: Arial\n"
    " - xnameoffset, ynameoffset, znameoffset - offset of name from values or tickmarks or axis. Default: 30\n"
    " - xnamecolor, ynamecolor, znamecolor - colors of axes names\n"
    " - xvaluesoffset, yvaluesoffset, zvaluesoffset - offset of values from tickmarks or axis. Default: 10\n"
    " - valuesfont - font of axes values. Default: Arial\n"
    " - xcolor, ycolor, zcolor - color of axis and values\n"
    " - xticks, yticks, xzicks - number of tickmark on axes. Default: 5\n"
    " - xticklength, yticklength, xzicklength - length of tickmark on axes. Default: 10\n",
    __FILE__,VGraduatedTrihedron,group);
  theCommands.Add("vprintview" ,
    "vprintview : width height filename [algo=0] [tile_width tile_height] : Test print algorithm: algo = 0 - stretch, algo = 1 - tile",
    __FILE__,VPrintView,group);
  theCommands.Add("vzlayer",
    "vzlayer add/del/get/settings/enable/disable [id]\n"
    " add - add new z layer to viewer and print its id\n"
    " del - del z layer by its id\n"
    " get - print sequence of z layers in increasing order of their overlay level\n"
    " settings - print status of z layer settings\n"
    " enable ([depth]test/[depth]write/[depth]clear/[depth]offset) \n    enables given setting for the z layer\n"
    " enable (p[ositive]offset/n[egative]offset) \n    enables given setting for the z layer\n"
    " disable ([depth]test/[depth]write/[depth]clear/[depth]offset) \n    disables given setting for the z layer\n"
    "\nWhere id is the layer identificator\n"
    "\nExamples:\n"
    "   vzlayer add\n"
    "   vzlayer enable poffset 1\n"
    "   vzlayer disable depthtest 1\n"
    "   vzlayer del 1\n",
    __FILE__,VZLayer,group);
  theCommands.Add("vlayerline",
    "vlayerline : vlayerline x1 y1 x2 y2 [linewidth=0.5] [linetype=0] [transparency=1.0]",
    __FILE__,VLayerLine,group);
  theCommands.Add ("vgrid",
    "vgrid [off] [Mode={r|c}] [Type={l|p}] [OriginX OriginY [StepX/StepRadius StepY/DivNb RotAngle]]"
    " : Mode - rectangular or circular"
    " : Type - lines or points",
    __FILE__, VGrid, group);
  theCommands.Add ("vpriviledgedplane",
    "vpriviledgedplane [Ox Oy Oz Nx Ny Nz [Xx Xy Xz]]"
    "\n\t\t:   Ox, Oy, Oz - plane origin"
    "\n\t\t:   Nx, Ny, Nz - plane normal direction"
    "\n\t\t:   Xx, Xy, Xz - plane x-reference axis direction"
    "\n\t\t: Sets or prints viewer's priviledged plane geometry.",
    __FILE__, VPriviledgedPlane, group);
  theCommands.Add ("vconvert",
    "vconvert v [Mode={window|view}]"
    "\n\t\t: vconvert x y [Mode={window|view|grid|ray}]"
    "\n\t\t: vconvert x y z [Mode={window|grid}]"
    "\n\t\t:   window - convert to window coordinates, pixels"
    "\n\t\t:   view   - convert to view projection plane"
    "\n\t\t:   grid   - convert to model coordinates, given on grid"
    "\n\t\t:   ray    - convert projection ray to model coordiantes"
    "\n\t\t: - vconvert v window : convert view to window;"
    "\n\t\t: - vconvert v view   : convert window to view;"
    "\n\t\t: - vconvert x y window : convert view to window;"
    "\n\t\t: - vconvert x y view : convert window to view;"
    "\n\t\t: - vconvert x y : convert window to model;"
    "\n\t\t: - vconvert x y grid : convert window to model using grid;"
    "\n\t\t: - vconvert x y ray : convert window projection line to model;"
    "\n\t\t: - vconvert x y z window : convert model to window;"
    "\n\t\t: - vconvert x y z grid : convert view to model using grid;"
    "\n\t\t: Converts the given coordinates to window/view/model space.",
    __FILE__, VConvert, group);
  theCommands.Add ("vfps",
    "vfps [framesNb=100] : estimate average frame rate for active view",
    __FILE__, VFps, group);
  theCommands.Add ("vgldebug",
            "vgldebug [-sync {0|1}] [-debug {0|1}] [-glslWarn {0|1}]"
    "\n\t\t:          [-extraMsg {0|1}] [{0|1}]"
    "\n\t\t: Request debug GL context. Should be called BEFORE vinit."
    "\n\t\t: Debug context can be requested only on Windows"
    "\n\t\t: with GL_ARB_debug_output extension implemented by GL driver!"
    "\n\t\t:  -sync     - request synchronized debug GL context"
    "\n\t\t:  -glslWarn - log GLSL compiler/linker warnings,"
    "\n\t\t:              which are suppressed by default,"
    "\n\t\t:  -extraMsg - log extra diagnostic messages from GL context,"
    "\n\t\t:              which are suppressed by default",
    __FILE__, VGlDebug, group);
  theCommands.Add ("vvbo",
    "vvbo [{0|1}] : turn VBO usage On/Off; affects only newly displayed objects",
    __FILE__, VVbo, group);
  theCommands.Add ("vstereo",
            "vstereo [0|1] [-mode Mode] [-reverse {0|1}]"
    "\n\t\t:         [-anaglyph Filter]"
    "\n\t\t: Control stereo output mode. Available modes for -mode:"
    "\n\t\t:  quadBuffer        - OpenGL QuadBuffer stereo,"
    "\n\t\t:                     requires driver support."
    "\n\t\t:                     Should be called BEFORE vinit!"
    "\n\t\t:  anaglyph         - Anaglyph glasses"
    "\n\t\t:  rowInterlaced    - row-interlaced display"
    "\n\t\t:  columnInterlaced - column-interlaced display"
    "\n\t\t:  chessBoard       - chess-board output"
    "\n\t\t:  sideBySide       - horizontal pair"
    "\n\t\t:  overUnder        - vertical   pair"
    "\n\t\t: Available Anaglyph filters for -anaglyph:"
    "\n\t\t:  redCyan, redCyanSimple, yellowBlue, yellowBlueSimple,"
    "\n\t\t:  greenMagentaSimple",
    __FILE__, VStereo, group);
  theCommands.Add ("vcaps",
            "vcaps [-vbo {0|1}] [-sprites {0|1}] [-ffp {0|1}]"
    "\n\t\t:       [-compatibleProfile {0|1}]"
    "\n\t\t:       [-vsync {0|1}]"
    "\n\t\t:       [-quadBuffer {0|1}] [-stereo {0|1}]"
    "\n\t\t:       [-softMode {0|1}] [-noupdate|-update]"
    "\n\t\t: Modify particular graphic driver options:"
    "\n\t\t:  FFP      - use fixed-function pipeline instead of"
    "\n\t\t:             built-in GLSL programs"
    "\n\t\t:            (requires compatible profile)"
    "\n\t\t:  VBO      - use Vertex Buffer Object (copy vertex"
    "\n\t\t:             arrays to GPU memory)"
    "\n\t\t:  sprite   - use textured sprites instead of bitmaps"
    "\n\t\t:  vsync    - switch VSync on or off"
    "\n\t\t: Context creation options:"
    "\n\t\t:  softMode          - software OpenGL implementation"
    "\n\t\t:  compatibleProfile - backward-compatible profile"
    "\n\t\t:  quadbuffer        - QuadBuffer"
    "\n\t\t: Unlike vrenderparams, these parameters control alternative"
    "\n\t\t: rendering paths producing the same visual result when"
    "\n\t\t: possible."
    "\n\t\t: Command is intended for testing old hardware compatibility.",
    __FILE__, VCaps, group);
  theCommands.Add ("vmemgpu",
    "vmemgpu [f]: print system-dependent GPU memory information if available;"
    " with f option returns free memory in bytes",
    __FILE__, VMemGpu, group);
  theCommands.Add ("vreadpixel",
    "vreadpixel xPixel yPixel [{rgb|rgba|depth|hls|rgbf|rgbaf}=rgba] [name]"
    " : Read pixel value for active view",
    __FILE__, VReadPixel, group);
  theCommands.Add("diffimage",
    "diffimage     : diffimage imageFile1 imageFile2 toleranceOfColor(0..1) blackWhite(1|0) borderFilter(1|0) [diffImageFile]",
    __FILE__, VDiffImage, group);
  theCommands.Add ("vselect",
    "vselect x1 y1 [x2 y2 [x3 y3 ... xn yn]] [-allowoverlap 0|1] [shift_selection = 0|1]\n"
    "- emulates different types of selection:\n"
    "- 1) single click selection\n"
    "- 2) selection with rectangle having corners at pixel positions (x1,y1) and (x2,y2)\n"
    "- 3) selection with polygon having corners in pixel positions (x1,y1), (x2,y2),...,(xn,yn)\n"
    "- 4) -allowoverlap manages overlap and inclusion detection in rectangular selection.\n"
    "     If the flag is set to 1, both sensitives that were included completely and overlapped partially by defined rectangle will be detected,\n"
    "     otherwise algorithm will chose only fully included sensitives. Default behavior is to detect only full inclusion. "
    " (partial inclusion - overlap - is not allowed by default)\n"
    "- 5) any of these selections with shift button pressed",
    __FILE__, VSelect, group);
  theCommands.Add ("vmoveto",
    "vmoveto x y"
    "- emulates cursor movement to pixel postion (x,y)",
    __FILE__, VMoveTo, group);
  theCommands.Add ("vviewparams", "vviewparams usage:\n"
    "- vviewparams\n"
    "- vviewparams [-scale [s]] [-eye [x y z]] [-at [x y z]] [-up [x y z]]\n"
    "              [-proj [x y z]] [-center x y] [-size sx]\n"
    "-   Gets or sets current view parameters.\n"
    "-   If called without arguments, all view parameters are printed.\n"
    "-   The options are:\n"
    "      -scale [s]    : prints or sets viewport relative scale.\n"
    "      -eye [x y z]  : prints or sets eye location.\n"
    "      -at [x y z]   : prints or sets center of look.\n"
    "      -up [x y z]   : prints or sets direction of up vector.\n"
    "      -proj [x y z] : prints or sets direction of look.\n"
    "      -center x y   : sets location of center of the screen in pixels.\n"
    "      -size [sx]    : prints viewport projection width and height sizes\n"
    "                    : or changes the size of its maximum dimension.\n",
    __FILE__, VViewParams, group);
  theCommands.Add("vchangeselected",
    "vchangeselected shape"
    "- adds to shape to selection or remove one from it",
		__FILE__, VChangeSelected, group);
  theCommands.Add("vzclipping",
    "vzclipping [mode] [depth width]\n"
    "- mode = OFF|BACK|FRONT|SLICE depth = [0..1] width = [0..1]\n"
    "- gets or sets ZClipping mode, width and depth",
    __FILE__,VZClipping,group);
  theCommands.Add ("vnbselected",
    "vnbselected"
    "\n\t\t: Returns number of selected objects", __FILE__, VNbSelected, group);
  theCommands.Add ("vcamera",
              "vcamera [-ortho] [-projtype]"
      "\n\t\t:         [-persp]"
      "\n\t\t:         [-fovy   [Angle]] [-distance [Distance]]"
      "\n\t\t:         [-stereo] [-leftEye] [-rightEye]"
      "\n\t\t:         [-iod [Distance]] [-iodType    [absolute|relative]]"
      "\n\t\t:         [-zfocus [Value]] [-zfocusType [absolute|relative]]"
      "\n\t\t: Manage camera parameters."
      "\n\t\t: Prints current value when option called without argument."
      "\n\t\t: Orthographic camera:"
      "\n\t\t:   -ortho      activate orthographic projection"
      "\n\t\t: Perspective camera:"
      "\n\t\t:   -persp      activate perspective  projection (mono)"
      "\n\t\t:   -fovy       field of view in y axis, in degrees"
      "\n\t\t:   -distance   distance of eye from camera center"
      "\n\t\t: Stereoscopic camera:"
      "\n\t\t:   -stereo     perspective  projection (stereo)"
      "\n\t\t:   -leftEye    perspective  projection (left  eye)"
      "\n\t\t:   -rightEye   perspective  projection (right eye)"
      "\n\t\t:   -iod        intraocular distance value"
      "\n\t\t:   -iodType    distance type, absolute or relative"
      "\n\t\t:   -zfocus     stereographic focus value"
      "\n\t\t:   -zfocusType focus type, absolute or relative",
    __FILE__, VCamera, group);
  theCommands.Add ("vautozfit", "command to enable or disable automatic z-range adjusting\n"
    "- vautozfit [on={1|0}] [scale]\n"
    "    Prints or changes parameters of automatic z-fit mode:\n"
    "   \"on\" - turns automatic z-fit on or off\n"
    "   \"scale\" - specifies factor to scale computed z range.\n",
    __FILE__, VAutoZFit, group);
  theCommands.Add ("vzrange", "command to manually access znear and zfar values\n"
    "   vzrange                - without parameters shows current values\n"
    "   vzrange [znear] [zfar] - applies provided values to view",
    __FILE__,VZRange, group);
  theCommands.Add("vantialiasing",
    "vantialiasing 1|0"
    "\n\t\t: Switches altialiasing on or off",
    __FILE__,VAntialiasing,group);
  theCommands.Add ("vpurgedisplay",
    "vpurgedisplay"
    "- removes structures which don't belong to objects displayed in neutral point",
    __FILE__, VPurgeDisplay, group);
  theCommands.Add("vsetviewsize",
    "vsetviewsize size",
    __FILE__,VSetViewSize,group);
  theCommands.Add("vmoveview",
    "vmoveview Dx Dy Dz [Start = 1|0]",
    __FILE__,VMoveView,group);
  theCommands.Add("vtranslateview",
    "vtranslateview Dx Dy Dz [Start = 1|0)]",
    __FILE__,VTranslateView,group);
  theCommands.Add("vturnview",
    "vturnview Ax Ay Az [Start = 1|0]",
    __FILE__,VTurnView,group);
  theCommands.Add("vtextureenv",
    "Enables or disables environment mapping in the 3D view, loading the texture from the given standard "
    "or user-defined file and optionally applying texture mapping parameters\n"
    "                  Usage:\n"
    "                  vtextureenv off - disables environment mapping\n"
    "                  vtextureenv on {std_texture|texture_file_name} [rep mod flt ss st ts tt rot] - enables environment mapping\n"
    "                              std_texture = (0..7)\n"
    "                              rep         = {clamp|repeat}\n"
    "                              mod         = {decal|modulate}\n"
    "                              flt         = {nearest|bilinear|trilinear}\n"
    "                              ss, st      - scale factors for s and t texture coordinates\n"
    "                              ts, tt      - translation for s and t texture coordinates\n"
    "                              rot         - texture rotation angle in degrees",
    __FILE__, VTextureEnv, group);
  theCommands.Add("vhlr" ,
    "is_enabled={on|off} [show_hidden={1|0}]"
    " - Hidden line removal algorithm:"
    " - is_enabled: if is on HLR algorithm is applied\n"
    " - show_hidden: if equals to 1, hidden lines are drawn as dotted ones.\n",
    __FILE__,VHLR,group);
  theCommands.Add("vhlrtype" ,
    "algo_type={algo|polyalgo} [shape_1 ... shape_n]"
    " - Changes the type of HLR algorithm using for shapes."
    " - algo_type: if equals to algo, exact HLR algorithm is applied;\n"
    "   if equals to polyalgo, polygonal HLR algorithm is applied."
    "If shapes are not given HLR algoithm of given type is applied"
    " to all shapes in the view\n",
    __FILE__,VHLRType,group);
  theCommands.Add("vclipplane", "vclipplane usage: \n"
    "  maxplanes <view_name> - get plane limit for view.\n"
    "  create <plane_name> - create new plane.\n"
    "  delete <plane_name> - delete plane.\n"
    "  clone <source_plane> <plane_name> - clone the plane definition.\n"
    "  set/unset <plane_name> object <object list> - set/unset plane for IO.\n"
    "  set/unset <plane_name> view <view list> - set/unset plane for view.\n"
    "  change <plane_name> on/off - turn clipping on/off.\n"
    "  change <plane_name> equation <a> <b> <c> <d> - change plane equation.\n"
    "  change <plane_name> capping on/off - turn capping on/off.\n"
    "  change <plane_name> capping color <r> <g> <b> - set color.\n"
    "  change <plane name> capping texname <texture> - set texture.\n"
    "  change <plane_name> capping texscale <sx> <sy> - set tex scale.\n"
    "  change <plane_name> capping texorigin <tx> <ty> - set tex origin.\n"
    "  change <plane_name> capping texrotate <angle> - set tex rotation.\n"
    "  change <plane_name> capping hatch on/off/<id> - set hatching mask.\n"
    "  please use VSetTextureMode command to enable texture rendering in view.\n"
    , __FILE__, VClipPlane, group);
  theCommands.Add("vsettexturemode", "vsettexturemode view_name mode \n"
    "  mode can be:\n"
    "  0 - no textures enabled in view.\n"
    "  1 - only environment textures enabled.\n"
    "  2 - all textures enabled.\n"
    "  this command sets texture details mode for the specified view.\n"
    , __FILE__, VSetTextureMode, group);
  theCommands.Add("vdefaults",
               "vdefaults [-absDefl value]"
       "\n\t\t:           [-devCoeff value]"
       "\n\t\t:           [-angDefl value]"
       "\n\t\t:           [-autoTriang {off/on | 0/1}]"
    , __FILE__, VDefaults, group);
  theCommands.Add("vlight",
    "tool to manage light sources, without arguments shows list of lights."
    "\n    Main commands: "
    "\n      'clear' to clear lights"
    "\n      '{def}aults' to load deafault lights"
    "\n      'add' (or 'new') <type> to add any light source"
    "\n          where <type> is one of {amb}ient|directional|{spot}light|positional"
    "\n      'change' <lightId> to edit light source with specified lightId"
    "\n\n      In addition to 'add' and 'change' commands you can use light parameters:"
    "\n        {pos}ition X Y Z"
    "\n        {dir}ection X Y Z (for directional light or for spotlight)"
    "\n        color colorName"
    "\n        {head}light 0|1"
    "\n        {sm}oothness value"
    "\n        {int}ensity value"
    "\n        {constAtten}uation value"
    "\n        {linearAtten}uation value"
    "\n        angle angleDeg"
    "\n        {spotexp}onent value"
    "\n        local|global"
    "\n\n        example: vlight add positional head 1 pos 0 1 1 color red"
    "\n        example: vlight change 0 direction 0 -1 0 linearAttenuation 0.2",
    __FILE__, VLight, group);
  theCommands.Add("vraytrace",
            "vraytrace [0|1]"
    "\n\t\t: Turns on/off ray-tracing renderer."
    "\n\t\t:   'vraytrace 0' alias for 'vrenderparams -raster'."
    "\n\t\t:   'vraytrace 1' alias for 'vrenderparams -rayTrace'.",
    __FILE__, VRenderParams, group);
  theCommands.Add("vrenderparams",
    "\n    Manages rendering parameters: "
    "\n      '-rayTrace'             Enables  GPU ray-tracing"
    "\n      '-raster'               Disables GPU ray-tracing"
    "\n      '-rayDepth     0..10'   Defines maximum ray-tracing depth"
    "\n      '-shadows      on|off'  Enables/disables shadows rendering"
    "\n      '-reflections  on|off'  Enables/disables specular reflections"
    "\n      '-fsaa         on|off'  Enables/disables adaptive anti-aliasing"
    "\n      '-gleam        on|off'  Enables/disables transparency shadow effects"
    "\n      '-gi           on|off'  Enables/disables global illumination effects"
    "\n      '-brng         on|off'  Enables/disables blocked RNG (fast coherent PT)"
    "\n      '-env          on|off'  Enables/disables environment map background"
    "\n      '-shadingModel model'   Controls shading model from enumeration"
    "\n                              color, flat, gouraud, phong"
    "\n      '-resolution   value'   Sets a new pixels density (PPI), defines scaling factor for parameters like text size"
    "\n    Unlike vcaps, these parameters dramatically change visual properties."
    "\n    Command is intended to control presentation quality depending on"
    "\n    hardware capabilities and performance.",
    __FILE__, VRenderParams, group);
  theCommands.Add("vfrustumculling",
    "vfrustumculling [toEnable]: enables/disables objects clipping",
    __FILE__,VFrustumCulling,group);
  theCommands.Add("vhighlightselected",
    "vhighlightselected [0|1] or vhighlightselected [on|off]: enables/disables highlighting of selected objects.\n"
    "Without arguments it shows if highlighting of selected objects is enabled now.",
    __FILE__,VHighlightSelected,group);
  theCommands.Add ("vplace",
            "vplace dx dy"
    "\n\t\t: Places the point (in pixels) at the center of the window",
    __FILE__, VPlace, group);
  theCommands.Add("vxrotate",
    "vxrotate",
    __FILE__,VXRotate,group);

#if defined(_WIN32)
  theCommands.Add("vprogressive",
    "vprogressive",
    __FILE__, VProgressiveMode, group);
#endif
}
