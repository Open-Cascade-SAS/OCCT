// Created on: 1998-09-01
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <OpenGl_GlCore20.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Drawer.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <DBRep.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_ExportFormat.hxx>
#include <Graphic3d_NameOfTextureEnv.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_TypeOfTextureFilter.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <ViewerTest.hxx>
#include <ViewerTest_EventManager.hxx>
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>
#include <Visual3d_View.hxx>
#include <Visual3d_ViewManager.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_LayerMgr.hxx>
#include <V3d_LayerMgrPointer.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_SpotLight.hxx>
#include <NCollection_DoubleMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Vector.hxx>
#include <NIS_View.hxx>
#include <NIS_Triangulated.hxx>
#include <NIS_InteractiveContext.hxx>
#include <AIS_InteractiveContext.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw.hxx>
#include <Draw_Appli.hxx>
#include <Aspect_PrintAlgo.hxx>
#include <Image_AlienPixMap.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OSD_Timer.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Visual3d_LayerItem.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Image_Diff.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <PrsMgr_PresentableObject.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <NCollection_DataMap.hxx>
#include <Graphic3d_Texture2Dmanual.hxx>
#include <Prs3d_ShadingAspect.hxx>

#ifdef WNT
#undef DrawText
#endif

#include <Visual3d_Layer.hxx>
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
extern const Handle(NIS_InteractiveContext)& TheNISContext();
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
Standard_Boolean DragFirst;

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
    TCollection_AsciiString aTitle("3D View - ");
    aTitle = aTitle
      + ViewerTest_myViews.Find2 (ViewerTest::CurrentView());
    SetWindowTitle (ViewerTest::CurrentView()->Window(), aTitle.ToCString());
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

    NameOfWindow = TCollection_ExtendedString("Collector");

    a3DViewer->SetDefaultBackgroundColor(Quantity_NOC_BLACK);
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
                                    Draw_VirtualWindows ? WS_POPUPWINDOW : WS_OVERLAPPEDWINDOW,
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

  // NIS setup
  Handle(NIS_View) aView = new NIS_View (a3DViewer, VT_GetWindow());

  ViewerTest::CurrentView(aView);
  ViewerTest_myViews.Bind (aViewNames.GetViewName(), aView);
  TheNISContext()->AttachView (aView);

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
//==============================================================================

static int VHLR (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (ViewerTest::CurrentView().IsNull())
  {
    di << argv[0] << ": Call vinit before this command, please.\n";
    return 1;
  }

  if (argc != 2)
  {
    di << argv[0] << ": Wrong number of command arguments.\n"
      << "Type help " << argv[0] << " for more information.\n";
    return 1;
  }

  Standard_Boolean isHLROn =
    (!strcasecmp (argv[1], "on")) ? Standard_True : Standard_False;

  if (isHLROn == MyHLRIsOn)
  {
    return 0;
  }

  MyHLRIsOn = isHLROn;
  ViewerTest::CurrentView()->SetComputedMode (MyHLRIsOn);

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
        aShape->Redisplay();
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
        anAISObject->Redisplay();
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
  TheNISContext()->DetachView(Handle(NIS_View)::DownCast(aView));
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
  const Handle(NIS_View) aNisView = Handle(NIS_View)::DownCast (aView);
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
    // FitAll
    if (aNisView.IsNull())
      aView->FitAll();
    else
      aNisView->FitAll3d();
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
    if (aContext->NbCurrents()==0 || aContext->NbSelected() == 0)
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
        aShape->Redisplay();
      }
    }
    else
    {
      for (aContext->InitCurrent();aContext->MoreCurrent();aContext->NextCurrent())
      {
        Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(aContext->Current());
        if (aShape.IsNull())
          continue;
        if(aShape->TypeOfHLR() == Prs3d_TOH_PolyAlgo)
          aShape->SetTypeOfHLR (Prs3d_TOH_Algo);
        else
          aShape->SetTypeOfHLR (Prs3d_TOH_PolyAlgo);
        aShape->Redisplay();
      }
    }

    aContext->UpdateCurrentViewer();

  }
  else if (!strcasecmp (buf_ret, "S"))
  {
    std::cout << "setup Shaded display mode" << std::endl;

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
  else if (!strcasecmp (buf_ret, "U"))
  {
    // Unset display mode
    std::cout << "reset display mode to defaults" << std::endl;

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
  else if (*buf_ret == THE_KEY_DELETE)
  {
    Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
    if (!aCtx.IsNull()
     && aCtx->NbCurrents() > 0
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
      EM->ShiftSelect (Min (X_ButtonPress, X_Motion), Max (Y_ButtonPress, Y_Motion),
                       Max (X_ButtonPress, X_Motion), Min (Y_ButtonPress, Y_Motion));
    }
    else
    {
      EM->Select (Min (X_ButtonPress, X_Motion), Max (Y_ButtonPress, Y_Motion),
                  Max (X_ButtonPress, X_Motion), Min (Y_ButtonPress, Y_Motion));
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
//function : VT_ProcessControlButton2Motion
//purpose  : Panning
//==============================================================================
void VT_ProcessControlButton2Motion()
{
  Quantity_Length dx = ViewerTest::CurrentView()->Convert(X_Motion - X_ButtonPress);
  Quantity_Length dy = ViewerTest::CurrentView()->Convert(Y_Motion - Y_ButtonPress);

  dy = -dy; // Xwindow Y axis is from top to Bottom

  ViewerTest::CurrentView()->Panning( dx, dy );

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
  return ViewProject(di, V3d_Ypos);
}

//==============================================================================
//function : VRight
//purpose  : Switch to a Right View
//Draw arg : No args
//==============================================================================

static int VRight(Draw_Interpretor& di, Standard_Integer , const char** )
{
  return ViewProject(di, V3d_Yneg);
}

//==============================================================================
//function : VFront
//purpose  : Switch to a Front View
//Draw arg : No args
//==============================================================================

static int VFront(Draw_Interpretor& di, Standard_Integer , const char** )
{
  return ViewProject(di, V3d_Xpos);
}

//==============================================================================
//function : VBack
//purpose  : Switch to a Back View
//Draw arg : No args
//==============================================================================

static int VBack(Draw_Interpretor& di, Standard_Integer , const char** )
{
  return ViewProject(di, V3d_Xneg);
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

#ifdef WNT

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
      IsDragged = Standard_False;
      if( !DragFirst )
      {
        HDC hdc = GetDC( hwnd );
        SelectObject( hdc, GetStockObject( HOLLOW_BRUSH ) );
        SetROP2( hdc, R2_NOT );
        Rectangle( hdc, X_ButtonPress, Y_ButtonPress, X_Motion, Y_Motion );
        ReleaseDC( hwnd, hdc );

        const Handle(ViewerTest_EventManager) EM =
          ViewerTest::CurrentEventManager();
        if ( fwKeys & MK_SHIFT )
          EM->ShiftSelect( min( X_ButtonPress, X_Motion ), max( Y_ButtonPress, Y_Motion ),
          max( X_ButtonPress, X_Motion ), min( Y_ButtonPress, Y_Motion ));
        else
          EM->Select( min( X_ButtonPress, X_Motion ), max( Y_ButtonPress, Y_Motion ),
          max( X_ButtonPress, X_Motion ), min( Y_ButtonPress, Y_Motion ));
      }
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

  if ( !ViewerTest::CurrentView().IsNull() ) {
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

  return DefWindowProc( hwnd, Msg, wParam, lParam );
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
          if(aReport.xclient.data.l[0] == GetDisplayConnection()->GetAtom(Aspect_XA_DELETE_WINDOW))
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

void ViewerTest_InitViewerTest (const Handle(AIS_InteractiveContext)& theContext)
{
  Handle(V3d_Viewer) aViewer = theContext->CurrentViewer();
  ViewerTest::SetAISContext(theContext);
  aViewer->InitActiveViews();
  Handle(V3d_View) aView = aViewer->ActiveView();
  if (aViewer->MoreActiveViews()) ViewerTest::CurrentView(aView);
  ViewerTest::ResetEventManager();
  Handle(Aspect_Window) aWindow = aView->Window();
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  // X11
  VT_GetWindow() = Handle(Xw_Window)::DownCast(aWindow);
  OSWindowSetup();
  static int aFirst = 1;
  if ( aFirst ) {
#if TCL_MAJOR_VERSION  < 8
    Tk_CreateFileHandler((void*)XConnectionNumber(GetDisplayConnection()->GetDisplay()),
      TK_READABLE, VProcessEvents, (ClientData) 0);
#else
    Tk_CreateFileHandler(XConnectionNumber(GetDisplayConnection()->GetDisplay()),
      TK_READABLE, VProcessEvents, (ClientData) 0);
#endif
    aFirst = 0;
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
//function : VTestZBuffTrihedron
//purpose  : Displays a V3d_ZBUFFER'ed or V3d_WIREFRAME'd trihedron
//==============================================================================

static int VTestZBuffTrihedron(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  if ( V3dView.IsNull() ) return 1;

  V3dView->ZBufferTriedronSetup();

  if ( argc == 1 ) {
    // Set up default trihedron parameters
    V3dView->TriedronDisplay( Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.1, V3d_ZBUFFER );
  } else
  if ( argc == 7 )
  {
    Aspect_TypeOfTriedronPosition aPosition = Aspect_TOTP_LEFT_LOWER;
    const char* aPosType = argv[1];

    if ( strcmp(aPosType, "center") == 0 )
    {
      aPosition = Aspect_TOTP_CENTER;
    } else
    if (strcmp(aPosType, "left_lower") == 0)
    {
      aPosition = Aspect_TOTP_LEFT_LOWER;
    } else
    if (strcmp(aPosType, "left_upper") == 0)
    {
      aPosition = Aspect_TOTP_LEFT_UPPER;
    } else
    if (strcmp(aPosType, "right_lower") == 0)
    {
      aPosition = Aspect_TOTP_RIGHT_LOWER;
    } else
    if (strcmp(aPosType, "right_upper") == 0)
    {
      aPosition = Aspect_TOTP_RIGHT_UPPER;
    } else
    {
      di << argv[1] << " Invalid type of alignment"  << "\n";
      di << "Must be one of [ center, left_lower,"   << "\n";
      di << "left_upper, right_lower, right_upper ]" << "\n";
      return 1;
    }

    Standard_Real R = Draw::Atof(argv[2])/255.;
    Standard_Real G = Draw::Atof(argv[3])/255.;
    Standard_Real B = Draw::Atof(argv[4])/255.;
    Quantity_Color aColor(R, G, B, Quantity_TOC_RGB);

    Standard_Real aScale = Draw::Atof(argv[5]);

    if( aScale <= 0.0 )
    {
      di << argv[5] << " Invalid value. Must be > 0" << "\n";
      return 1;
    }

    V3d_TypeOfVisualization aPresentation = V3d_ZBUFFER;
    const char* aPresType = argv[6];

    if ( strcmp(aPresType, "wireframe") == 0 )
    {
      aPresentation = V3d_WIREFRAME;
    } else
    if (strcmp(aPresType, "zbuffer") == 0)
    {
      aPresentation = V3d_ZBUFFER;
    } else
    {
      di << argv[6] << " Invalid type of visualization" << "\n";
      di << "Must be one of [ wireframe, zbuffer ]"     << "\n";
      return 1;
    }

    V3dView->TriedronDisplay( aPosition, aColor.Name(), aScale, aPresentation );

  } else
  {
    di << argv[0] << " Invalid number of arguments" << "\n";
    return 1;
  }

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
    V3dView->Rotate( Draw::Atof(argv[1]), Draw::Atof(argv[2]), Draw::Atof(argv[3]) );
    return 0;
  } else if ( argc == 7 ) {
    V3dView->Rotate( Draw::Atof(argv[1]), Draw::Atof(argv[2]), Draw::Atof(argv[3]), Draw::Atof(argv[4]), Draw::Atof(argv[5]), Draw::Atof(argv[6]) );
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
      aFormatStr = aFileName.SubString (aLen - 1, aLen);
    }
    else if (aFileName.Value (aLen - 3) == '.')
    {
      aFormatStr = aFileName.SubString (aLen - 2, aLen);
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

  try {
    if (!V3dView->View()->Export (argv[1], anExpFormat))
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
  Standard_Integer textHeight = 16;
  Aspect_TypeOfColorScalePosition position = Aspect_TOCSP_RIGHT;
  Standard_Real X = 0., Y = 0. ;

  if ( argc < 9 )
  {
     if( argc > 3 )
     {
       minRange = Draw::Atof( argv[1] );
       maxRange = Draw::Atof( argv[2] );
       numIntervals = Draw::Atoi( argv[3] );
     }
     if ( argc > 4 )
       textHeight = Draw::Atoi( argv[4] );
     if ( argc > 5 )
       position = (Aspect_TypeOfColorScalePosition)Draw::Atoi( argv[5] );
     if ( argc > 7 )
     {
       X = Draw::Atof( argv[6] );
       Y = Draw::Atof( argv[7] );
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
  const int display = Draw::Atoi(argv[1]);

  if (display)
  {
    // Text font
    TCollection_AsciiString font;
    if (argc < 6)
      font.AssignCat("Courier");
    else
      font.AssignCat(argv[5]);

    // Text is multibyte
    const Standard_Boolean isMultibyte = (argc < 7)? Standard_False : (Draw::Atoi(argv[6]) != 0);

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
  Standard_Integer aWidth  = Draw::Atoi (argv[1]);
  Standard_Integer aHeight = Draw::Atoi (argv[2]);
  Standard_Integer aMode   = 0;
  TCollection_AsciiString aFileName = TCollection_AsciiString (argv[3]);
  if (argc==5)
    aMode = Draw::Atoi (argv[4]);

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
      isPrinted = aView->Print(anDC,1,1,0,Aspect_PA_TILE);

    // succesfully printed into an intermediate buffer
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
    di << "Use: vzlayer " << argv[0];
    di << " add/del/get [id]\n";
    di << " add - add new z layer to viewer and print its id\n";
    di << " del - del z layer by its id\n";
    di << " get - print sequence of z layers in increasing order of their overlay level\n";
    di << "id - the layer identificator value defined when removing z layer\n";
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
  else
  {
    di << "Invalid operation, please use { add / del / get }\n";
    return 1;
  }

  return 0;
}

DEFINE_STANDARD_HANDLE(V3d_TextItem, Visual3d_LayerItem)

// this class provides a presentation of text item in v3d view under-/overlayer
class V3d_TextItem : public Visual3d_LayerItem
{
public:

  // CASCADE RTTI
  DEFINE_STANDARD_RTTI(V3d_TextItem)

  // constructor
  Standard_EXPORT V3d_TextItem(const TCollection_AsciiString& theText,
                               const Standard_Real theX1,
                               const Standard_Real theY1,
                               const Standard_Real theHeight,
                               const TCollection_AsciiString& theFontName,
                               const Quantity_Color& theColor,
                               const Quantity_Color& theSubtitleColor,
                               const Aspect_TypeOfDisplayText& theTypeOfDisplay,
                               const Handle(Visual3d_Layer)& theLayer);

  // redraw method
  Standard_EXPORT void RedrawLayerPrs();

private:

  Standard_Real            myX1;
  Standard_Real            myY1;
  TCollection_AsciiString  myText;
  Standard_Real            myHeight;
  Handle(Visual3d_Layer)   myLayer;
  Quantity_Color           myColor;
  Quantity_Color           mySubtitleColor;
  Aspect_TypeOfDisplayText myType;
  TCollection_AsciiString  myFontName;
};

IMPLEMENT_STANDARD_HANDLE(V3d_TextItem, Visual3d_LayerItem)
IMPLEMENT_STANDARD_RTTIEXT(V3d_TextItem, Visual3d_LayerItem)

// create and add to display the text item
V3d_TextItem::V3d_TextItem (const TCollection_AsciiString& theText,
                            const Standard_Real theX1,
                            const Standard_Real theY1,
                            const Standard_Real theHeight,
                            const TCollection_AsciiString& theFontName,
                            const Quantity_Color& theColor,
                            const Quantity_Color& theSubtitleColor,
                            const Aspect_TypeOfDisplayText& theTypeOfDisplay,
                            const Handle(Visual3d_Layer)& theLayer)
 : myX1 (theX1), myY1 (theY1),
   myText (theText),
   myHeight (theHeight),
   myLayer (theLayer),
   myColor (theColor),
   mySubtitleColor (theSubtitleColor),
   myType (theTypeOfDisplay),
   myFontName (theFontName)
{
  if (!myLayer.IsNull ())
    myLayer->AddLayerItem (this);
}

// render item
void V3d_TextItem::RedrawLayerPrs ()
{
  if (myLayer.IsNull ())
    return;

  myLayer->SetColor (myColor);
  myLayer->SetTextAttributes (myFontName.ToCString (), myType, mySubtitleColor);
  myLayer->DrawText (myText.ToCString (), myX1, myY1, myHeight);
}

DEFINE_STANDARD_HANDLE(V3d_LineItem, Visual3d_LayerItem)

// The Visual3d_LayerItem line item for "vlayerline" command
// it provides a presentation of line with user-defined
// linewidth, linetype and transparency.
class V3d_LineItem : public Visual3d_LayerItem
{
public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI(V3d_LineItem)

  // constructor
  Standard_EXPORT V3d_LineItem(Standard_Real X1, Standard_Real Y1,
                               Standard_Real X2, Standard_Real Y2,
                               V3d_LayerMgrPointer theLayerMgr,
                               Aspect_TypeOfLine theType = Aspect_TOL_SOLID,
                               Standard_Real theWidth    = 0.5,
                               Standard_Real theTransp   = 1.0);

  // redraw method
  Standard_EXPORT   void RedrawLayerPrs();

private:

  Standard_Real       myX1, myY1, myX2, myY2;
  V3d_LayerMgrPointer myLayerMgr;
  Aspect_TypeOfLine   myType;
  Standard_Real       myWidth;
  Standard_Real       myTransparency;
};

IMPLEMENT_STANDARD_HANDLE(V3d_LineItem, Visual3d_LayerItem)
IMPLEMENT_STANDARD_RTTIEXT(V3d_LineItem, Visual3d_LayerItem)

// default constructor for line item
V3d_LineItem::V3d_LineItem(Standard_Real X1, Standard_Real Y1,
                           Standard_Real X2, Standard_Real Y2,
                           V3d_LayerMgrPointer theLayerMgr,
                           Aspect_TypeOfLine theType,
                           Standard_Real theWidth,
                           Standard_Real theTransp) :
  myX1(X1), myY1(Y1), myX2(X2), myY2(Y2), myLayerMgr(theLayerMgr),
  myType(theType), myWidth(theWidth), myTransparency(theTransp)
{
  if (myLayerMgr && !myLayerMgr->Overlay().IsNull())
    myLayerMgr->Overlay()->AddLayerItem (this);
}

// render line
void V3d_LineItem::RedrawLayerPrs ()
{
  Handle (Visual3d_Layer) aOverlay;

  if (myLayerMgr)
    aOverlay = myLayerMgr->Overlay();

  if (!aOverlay.IsNull())
  {
    Quantity_Color aColor(1.0, 0, 0, Quantity_TOC_RGB);
    aOverlay->SetColor(aColor);
    aOverlay->SetTransparency((Standard_ShortReal)myTransparency);
    aOverlay->SetLineAttributes((Aspect_TypeOfLine)myType, myWidth);
    aOverlay->BeginPolyline();
    aOverlay->AddVertex(myX1, myY1);
    aOverlay->AddVertex(myX2, myY2);
    aOverlay->ClosePrimitive();
  }
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

  // replace layer manager
  Handle(V3d_LayerMgr) aMgr = new V3d_LayerMgr(aView);
  aView->SetLayerMgr(aMgr);

  // add line item
  Handle (V3d_LineItem) anItem = new V3d_LineItem(X1, Y1, X2, Y2,
                                                  aMgr.operator->(),
                                                  aLineType, aWidth,
                                                  aTransparency);

  // update view
  aView->MustBeResized();
  aView->Redraw();

  return 0;
}

//=======================================================================
//function : VOverlayText
//purpose  : Test text displaying in view overlay
//=======================================================================
static int VOverlayText (Draw_Interpretor& di, Standard_Integer argc, const char**argv)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    di << "No active view. Please call vinit.\n";
    return 1;
  }
  else if (argc < 4 || argc > 13)
  {
    di << "Use: " << argv[0];
    di << " text x y [height] [font_name] [text_color: R G B] [displayType]\n";
    di << "[background_color: R G B]\n";
    di << "  height - pixel height of the text (default=10.0)\n";
    di << "  font_name - name of font (default=courier)\n";
    di << "  text_color - R G B values of text color (default=255.0 255.0 255.0)\n";
    di << "  display_type = {normal/subtitle/decal/blend/dimension}, (default=normal)\n";
    di << "  background_color- R G B values used for subtitle and decal text\n";
    di << "(default=255.0 255.0 255.0)\n";
    return 1;
  }

  TCollection_AsciiString aText (argv[1]);
  Standard_Real aPosX = Draw::Atof(argv[2]);
  Standard_Real aPosY = Draw::Atof(argv[3]);
  Standard_Real aHeight = (argc >= 5) ? Draw::Atof (argv[4]) : 10.0;

  // font name
  TCollection_AsciiString aFontName = "Courier";
  if (argc >= 6)
    aFontName = TCollection_AsciiString (argv[5]);

  // text colors
  Quantity_Parameter aColorRed   = 1.0;
  Quantity_Parameter aColorGreen = 1.0;
  Quantity_Parameter aColorBlue  = 1.0;
  if (argc >= 9)
  {
    aColorRed   = Draw::Atof (argv[6])/255.;
    aColorGreen = Draw::Atof (argv[7])/255.;
    aColorBlue  = Draw::Atof (argv[8])/255.;
  }

  // display type
  TCollection_AsciiString aDispStr;
  if (argc >= 10)
    aDispStr = TCollection_AsciiString (argv[9]);

  Aspect_TypeOfDisplayText aTextType = Aspect_TODT_NORMAL;
  if (aDispStr.IsEqual ("subtitle"))
    aTextType = Aspect_TODT_SUBTITLE;
  else if (aDispStr.IsEqual ("decal"))
    aTextType = Aspect_TODT_DEKALE;
  else if (aDispStr.IsEqual ("blend"))
    aTextType = Aspect_TODT_BLEND;
  else if (aDispStr.IsEqual ("dimension"))
    aTextType = Aspect_TODT_DIMENSION;

  // subtitle color
  Quantity_Parameter aSubRed   = 1.0;
  Quantity_Parameter aSubGreen = 1.0;
  Quantity_Parameter aSubBlue  = 1.0;
  if (argc == 13)
  {
    aSubRed   = Draw::Atof (argv[10])/255.;
    aSubGreen = Draw::Atof (argv[11])/255.;
    aSubBlue  = Draw::Atof (argv[12])/255.;
  }

  // check fo current overlay
  Handle(Visual3d_Layer) anOverlay = aView->Viewer()->Viewer()->OverLayer ();
  if (anOverlay.IsNull ())
  {
    Handle(V3d_LayerMgr) aMgr = new V3d_LayerMgr (aView);
    anOverlay = aMgr->Overlay ();
    aView->SetLayerMgr (aMgr);
  }

  Quantity_Color aTextColor (aColorRed, aColorGreen,
    aColorBlue, Quantity_TOC_RGB);
  Quantity_Color aSubtColor (aSubRed, aSubGreen,
    aSubBlue, Quantity_TOC_RGB);

  // add text item
  Handle(V3d_TextItem) anItem = new V3d_TextItem (aText, aPosX, aPosY,
    aHeight, aFontName, aTextColor, aSubtColor, aTextType, anOverlay);

  // update view
  aView->MustBeResized();
  aView->Redraw();

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
  if (theArgNb < 2)
  {
    Handle(V3d_View) aView = ViewerTest::CurrentView();
    if (aView.IsNull())
    {
      std::cerr << "No active view. Please call vinit.\n";
      return 0;
    }

    Standard_Boolean isActive = OpenGl_Context::CheckExtension ((const char* )glGetString (GL_EXTENSIONS),
                                                                "GL_ARB_debug_output");
    std::cout << "Active graphic driver: debug " << (isActive ? "ON" : "OFF") << "\n";
    theDI << (isActive ? "1" : "0");
    return 0;
  }

  ViewerTest_myDefaultCaps.contextDebug = Draw::Atoi (theArgVec[1]) != 0;
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
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (!aContextAIS.IsNull())
  {
    aDriver = Handle(OpenGl_GraphicDriver)::DownCast (aContextAIS->CurrentViewer()->Driver());
    aCaps   = &aDriver->ChangeOptions();
  }

  if (theArgNb < 2)
  {
    theDI << "VBO:     " << (aCaps->vboDisable        ? "0" : "1") << "\n";
    theDI << "Sprites: " << (aCaps->pntSpritesDisable ? "0" : "1") << "\n";
    theDI << "SoftMode:" << (aCaps->contextNoAccel    ? "1" : "0") << "\n";
    return 0;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    const TCollection_AsciiString anArg (theArgVec[anArgIter]);
    if (anArg.Search ("vbo=") > -1)
    {
      aCaps->vboDisable        = anArg.Token ("=", 2).IntegerValue() == 0;
    }
    else if (anArg.Search ("sprites=") > -1)
    {
      aCaps->pntSpritesDisable = anArg.Token ("=", 2).IntegerValue() == 0;
    }
    else if (anArg.Search ("soft=") > -1)
    {
      aCaps->contextNoAccel = anArg.Token ("=", 2).IntegerValue() != 0;
    }
    else
    {
      std::cerr << "Unknown argument: " << anArg << "\n";
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
  if (aDiffImagePath != NULL)
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
  const Standard_Boolean isShiftSelection = (argc>3 && !(argc%2) && (atoi(argv[argc-1])==1));
  Handle(ViewerTest_EventManager) aCurrentEventManager = ViewerTest::CurrentEventManager();
  aCurrentEventManager->MoveTo(atoi(argv[1]),atoi(argv[2]));
  if(argc <= 4)
  {
    if(isShiftSelection)
      aCurrentEventManager->ShiftSelect();
    else
      aCurrentEventManager->Select();
  }
  else if(argc <= 6)
  {
    if(isShiftSelection)
      aCurrentEventManager->ShiftSelect(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
    else
      aCurrentEventManager->Select(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
  }
  else
  {
    Standard_Integer anUpper = 0;

    if(isShiftSelection)
      anUpper = (argc-1)/2;
    else
      anUpper = argc/2;
    TColgp_Array1OfPnt2d aPolyline(1,anUpper);

    for(Standard_Integer i=1;i<=anUpper;++i)
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

//=======================================================================
//function : VViewParams
//purpose  : Gets or sets AIS View characteristics
//=======================================================================
static Standard_Integer VViewParams (Draw_Interpretor& di,
                                Standard_Integer argc,
                                const char ** argv)
{
  if ( argc != 1 && argc != 13)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  Handle (V3d_View) anAISView = ViewerTest::CurrentView ();
  if ( anAISView.IsNull () )
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if(argc==1){
    Quantity_Factor anAISViewScale = anAISView -> V3d_View::Scale ();
    Standard_Real anAISViewCenterCoordinateX = 0.0;
    Standard_Real anAISViewCenterCoordinateY = 0.0;
    anAISView -> V3d_View::Center (anAISViewCenterCoordinateX, anAISViewCenterCoordinateY);
    Standard_Real anAISViewProjX = 0.0;
    Standard_Real anAISViewProjY = 0.0;
    Standard_Real anAISViewProjZ = 0.0;
    anAISView -> V3d_View::Proj (anAISViewProjX, anAISViewProjY, anAISViewProjZ);
    Standard_Real anAISViewUpX = 0.0;
    Standard_Real anAISViewUpY = 0.0;
    Standard_Real anAISViewUpZ = 0.0;
    anAISView -> V3d_View::Up (anAISViewUpX, anAISViewUpY, anAISViewUpZ);
    Standard_Real anAISViewAtX = 0.0;
    Standard_Real anAISViewAtY = 0.0;
    Standard_Real anAISViewAtZ = 0.0;
    anAISView -> V3d_View::At (anAISViewAtX, anAISViewAtY, anAISViewAtZ);
    di << "Scale of current view: " << anAISViewScale << "\n";
    di << "Center on X : "<< anAISViewCenterCoordinateX << "; on Y: " << anAISViewCenterCoordinateY << "\n";
    di << "Proj on X : " << anAISViewProjX << "; on Y: " << anAISViewProjY << "; on Z: " << anAISViewProjZ << "\n";
    di << "Up on X : " << anAISViewUpX << "; on Y: " << anAISViewUpY << "; on Z: " << anAISViewUpZ << "\n";
    di << "At on X : " << anAISViewAtX << "; on Y: " << anAISViewAtY << "; on Z: " << anAISViewAtZ << "\n";
  }
  else
  {
    Quantity_Factor anAISViewScale = atof (argv [1]);
    Standard_Real anAISViewCenterCoordinateX = atof (argv [2]);
    Standard_Real anAISViewCenterCoordinateY = atof (argv [3]);
    Standard_Real anAISViewProjX = atof (argv [4]);
    Standard_Real anAISViewProjY = atof (argv [5]);
    Standard_Real anAISViewProjZ = atof (argv [6]);
    Standard_Real anAISViewUpX = atof (argv [7]);
    Standard_Real anAISViewUpY = atof (argv [8]);
    Standard_Real anAISViewUpZ = atof (argv [9]);
    Standard_Real anAISViewAtX = atof (argv [10]);
    Standard_Real anAISViewAtY = atof (argv [11]);
    Standard_Real anAISViewAtZ = atof (argv [12]);
    anAISView -> V3d_View::SetScale (anAISViewScale);
    anAISView -> V3d_View::SetCenter (anAISViewCenterCoordinateX, anAISViewCenterCoordinateY);
    anAISView -> V3d_View::SetAt (anAISViewAtX, anAISViewAtY, anAISViewAtZ);
    anAISView -> V3d_View::SetProj (anAISViewProjX, anAISViewProjY, anAISViewProjZ);
    anAISView -> V3d_View::SetUp (anAISViewUpX, anAISViewUpY, anAISViewUpZ);
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

    if(aContext->HasOpenedContext())
    {
      aContext->AddOrRemoveSelected(anAISObject);
    }
    else
    {
      aContext->AddOrRemoveCurrentObject(anAISObject);
    }
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
        aDepth = atof(argv[1]);
        aWidth = atof(argv[2]);
      }
      else if(argc == 4)
      {
        aDepth = atof(argv[2]);
        aWidth = atof(argv[3]);
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
  Standard_Real aSize = atof(argv[1]);
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
  Standard_Real Dx = atof(argv[1]);
  Standard_Real Dy = atof(argv[2]);
  Standard_Real Dz = atof(argv[3]);
  Standard_Boolean aStart = Standard_True;
  if (argc == 5)
  {
      aStart = (atoi(argv[4]) > 0);
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
  Standard_Real Dx = atof(argv[1]);
  Standard_Real Dy = atof(argv[2]);
  Standard_Real Dz = atof(argv[3]);
  Standard_Boolean aStart = Standard_True;
  if (argc == 5)
  {
      aStart = (atoi(argv[4]) > 0);
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
  Standard_Real Ax = atof(argv[1]);
  Standard_Real Ay = atof(argv[2]);
  Standard_Real Az = atof(argv[3]);
  Standard_Boolean aStart = Standard_True;
  if (argc == 5)
  {
      aStart = (atoi(argv[4]) > 0);
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
  DEFINE_STANDARD_RTTI(OCC_TextureEnv);
};
DEFINE_STANDARD_HANDLE(OCC_TextureEnv, Graphic3d_TextureEnv);
IMPLEMENT_STANDARD_HANDLE(OCC_TextureEnv, Graphic3d_TextureEnv);
IMPLEMENT_STANDARD_RTTIEXT(OCC_TextureEnv, Graphic3d_TextureEnv);

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

      Standard_Real aCoeffA = atof (theArgVec [4]);
      Standard_Real aCoeffB = atof (theArgVec [5]);
      Standard_Real aCoeffC = atof (theArgVec [6]);
      Standard_Real aCoeffD = atof (theArgVec [7]);
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

        Standard_Real aRed = atof (theArgVec [5]);
        Standard_Real aGrn = atof (theArgVec [6]);
        Standard_Real aBlu = atof (theArgVec [7]);

        Graphic3d_MaterialAspect aMat = aClipPlane->CappingMaterial();
        aMat.SetColor (Quantity_Color (aRed, aGrn, aBlu, Quantity_TOC_RGB));
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
    return 0;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    TCollection_AsciiString aKey, aValue;
    if (!ViewerTest::SplitParameter (anArg, aKey, aValue)
     || aValue.IsEmpty())
    {
      std::cerr << "Error, wrong syntax at: '" << anArg.ToCString() << "'!\n";
      return 1;
    }

    aKey.UpperCase();
    if (aKey == "ABSDEFL"
     || aKey == "ABSOLUTEDEFLECTION"
     || aKey == "DEFL"
     || aKey == "DEFLECTION")
    {
      aDefParams->SetTypeOfDeflection         (Aspect_TOD_ABSOLUTE);
      aDefParams->SetMaximalChordialDeviation (aValue.RealValue());
    }
    else if (aKey == "RELDEFL"
          || aKey == "RELATIVEDEFLECTION"
          || aKey == "DEVCOEFF"
          || aKey == "DEVIATIONCOEFF"
          || aKey == "DEVIATIONCOEFFICIENT")
    {
      aDefParams->SetTypeOfDeflection     (Aspect_TOD_RELATIVE);
      aDefParams->SetDeviationCoefficient (aValue.RealValue());
    }
    else if (aKey == "ANGDEFL"
          || aKey == "ANGULARDEFL"
          || aKey == "ANGULARDEFLECTION")
    {
      // currently HLRDeviationAngle is used instead of DeviationAngle in most places
      aDefParams->SetHLRAngle (M_PI * aValue.RealValue() / 180.0);
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
          theDi << "  Type:      Ambient\n";
          break;
        }
        case V3d_DIRECTIONAL:
        {
          Handle(V3d_DirectionalLight) aLightDir = Handle(V3d_DirectionalLight)::DownCast (aLight);
          theDi << "  Type:      Directional\n";
          theDi << "  Headlight: " << (aLight->Headlight() ? "TRUE" : "FALSE") << "\n";
          if (!aLightDir.IsNull())
          {
            aLightDir->Position  (anXYZ[0], anXYZ[1], anXYZ[2]);
            theDi << "  Position:  " << anXYZ[0] << ", " << anXYZ[1] << ", " << anXYZ[2] << "\n";
            aLightDir->Direction (anXYZ[0], anXYZ[1], anXYZ[2]);
            theDi << "  Direction: " << anXYZ[0] << ", " << anXYZ[1] << ", " << anXYZ[2] << "\n";
          }
          break;
        }
        case V3d_POSITIONAL:
        {
          Handle(V3d_PositionalLight) aLightPos = Handle(V3d_PositionalLight)::DownCast (aLight);
          theDi << "  Type:      Positional\n";
          theDi << "  Headlight: " << (aLight->Headlight() ? "TRUE" : "FALSE") << "\n";
          if (!aLightPos.IsNull())
          {
            aLightPos->Position  (anXYZ[0], anXYZ[1], anXYZ[2]);
            theDi << "  Position:  " << anXYZ[0] << ", " << anXYZ[1] << ", " << anXYZ[2] << "\n";
            aLightPos->Attenuation (anAtten[0], anAtten[1]);
            theDi << "  Atten.:    " << anAtten[0] << " " << anAtten[1] << "\n";
          }
          break;
        }
        case V3d_SPOT:
        {
          Handle(V3d_SpotLight) aLightSpot = Handle(V3d_SpotLight)::DownCast (aLight);
          theDi << "  Type:      Spot\n";
          theDi << "  Headlight: " << (aLight->Headlight() ? "TRUE" : "FALSE") << "\n";
          if (!aLightSpot.IsNull())
          {
            aLightSpot->Position  (anXYZ[0], anXYZ[1], anXYZ[2]);
            theDi << "  Position:  " << anXYZ[0] << ", " << anXYZ[1] << ", " << anXYZ[2] << "\n";
            aLightSpot->Direction (anXYZ[0], anXYZ[1], anXYZ[2]);
            theDi << "  Direction: " << anXYZ[0] << ", " << anXYZ[1] << ", " << anXYZ[2] << "\n";
            aLightSpot->Attenuation (anAtten[0], anAtten[1]);
            theDi << "  Atten.:    " << anAtten[0] << " " << anAtten[1] << "\n";
            theDi << "  Angle:     " << (aLightSpot->Angle() * 180.0 / M_PI) << "\n";
            theDi << "  Exponent:  " << aLightSpot->Concentration() << "\n";
          }
          break;
        }
        default:
        {
          theDi << "  Type:      UNKNOWN\n";
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



//==============================================================================
//function : VClInfo
//purpose  : Prints info about active OpenCL device
//==============================================================================

static Standard_Integer VClInfo (Draw_Interpretor& theDi,
                                 Standard_Integer,
                                 const char**)
{
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    std::cerr << "Call vinit before!\n";
    return 1;
  }

  Handle(OpenGl_GraphicDriver) aDrv = Handle(OpenGl_GraphicDriver)::DownCast (aContextAIS->CurrentViewer()->Driver());
  Graphic3d_CView* aCView = static_cast<Graphic3d_CView*> (ViewerTest::CurrentView()->View()->CView());
  NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString> anInfo;
  if (aDrv.IsNull()
   || aCView == NULL
   || !aDrv->GetOpenClDeviceInfo (*aCView, anInfo))
  {
    theDi << "OpenCL device info is unavailable!\n";
    return 0;
  }

  theDi << "OpenCL device info:\n";
  for (NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>::Iterator anIter (anInfo);
       anIter.More(); anIter.Next())
  {
    theDi << anIter.Key() << ": \t" << anIter.Value() << "\n";
  }
  return 0;
}

//=======================================================================
//function : VRaytrace
//purpose  : Enables/disables OpenCL-based ray-tracing
//=======================================================================

static Standard_Integer VRaytrace (Draw_Interpretor& ,
                                   Standard_Integer  theArgNb,
                                   const char**      theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << "Use 'vinit' command before " << theArgVec[0] << "\n";
    return 1;
  }

  if (theArgNb < 2
   || Draw::Atoi (theArgVec[1]))
  {
    aView->SetRaytracingMode();
  }
  else
  {
    aView->SetRasterizationMode();
  }
  aView->Redraw();
  return 0;
}

//=======================================================================
//function : VSetRaytraceMode
//purpose  : Enables/disables features of OpenCL-based ray-tracing
//=======================================================================

static Standard_Integer VSetRaytraceMode (Draw_Interpretor&,
                                          Standard_Integer theArgNb,
                                          const char ** theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << "Use 'vinit' command before " << theArgVec[0] << "\n";
    return 1;
  }
  else if (theArgNb < 2)
  {
    std::cerr << "Usage : " << theArgVec[0] << " [shad=0|1] [refl=0|1] [aa=0|1]\n";
    return 1;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    const TCollection_AsciiString anArg (theArgVec[anArgIter]);

    if (anArg.Search ("shad=") > -1)
    {
      if (anArg.Token ("=", 2).IntegerValue() != 0)
        aView->EnableRaytracedShadows();
      else
        aView->DisableRaytracedShadows();
    }
    else if (anArg.Search ("refl=") > -1)
    {
      if (anArg.Token ("=", 2).IntegerValue() != 0)
        aView->EnableRaytracedReflections();
      else
        aView->DisableRaytracedReflections();
    }
    else if (anArg.Search ("aa=") > -1)
    {
      if (anArg.Token ("=", 2).IntegerValue() != 0)
        aView->EnableRaytracedAntialiasing();
      else
        aView->DisableRaytracedAntialiasing();
    }
    else
    {
      std::cerr << "Unknown argument: " << anArg << "\n";
    }
  }

  aView->Redraw();
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
    "vtop or <T>      : Top view" ,
    __FILE__,VTop,group);
  theCommands.Add("vbottom" ,
    "vbottom          : Bottom view" ,
    __FILE__,VBottom,group);
  theCommands.Add("vleft" ,
    "vleft            : Left view" ,
    __FILE__,VLeft,group);
  theCommands.Add("vright" ,
    "vright           : Right view" ,
    __FILE__,VRight,group);
  theCommands.Add("vaxo" ,
    " vaxo or <A>     : Axonometric view ",
    __FILE__,VAxo,group);
  theCommands.Add("vfront" ,
    "vfront           : Front view" ,
    __FILE__,VFront,group);
  theCommands.Add("vback" ,
    "vback            : Back view" ,
    __FILE__,VBack,group);
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
  theCommands.Add("vscale",
    "vscale          : vscale X Y Z",
    __FILE__,VScale,group);
  theCommands.Add("vzbufftrihedron",
    "vzbufftrihedron [center|left_lower|left_upper|right_lower|right_upper"
    " textR=255 textG=255 textB=255 scale=0.1 wireframe|zbuffer]"
    " : Displays a V3d_ZBUFFER'ed or V3d_WIREFRAME'd trihedron",
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
    "vexport         : vexport full_file_path {PS | EPS | TEX | PDF | SVG | PGF | EMF }"
    " : exports the view to a vector file of a given format"
    " : notice that EMF format requires patched gl2ps",
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
  theCommands.Add("vzlayer",
    "vzlayer : add/del/get [id] : Z layer operations in v3d viewer: add new z layer, delete z layer, get z layer ids",
    __FILE__,VZLayer,group);
  theCommands.Add("voverlaytext",
    "voverlaytext : text x y [height] [font_name] [text_color: R G B] [display_type] [background_color: R G B]"
    " : height - pixel height of the text (default=10.0)"
    " : font_name - name of font (default=courier)"
    " : text_color - three values: RedColor GreenColor BlueColor (default = 255.0 255.0 255.0) "
    " : display_type = {normal/subtitle/decal/blend}, (default=normal) "
    " : background_color - three values: RedColor GreenColor BlueColor (default = 255.0 255.0 255.0), the parameter is defined for subtitle and decal display types ",
    __FILE__,VOverlayText,group);
  theCommands.Add("vlayerline",
    "vlayerline : vlayerline x1 y1 x2 y2 [linewidth=0.5] [linetype=0] [transparency=1.0]",
    __FILE__,VLayerLine,group);
  theCommands.Add ("vgrid",
    "vgrid [off] [Mode={r|c}] [Type={l|p}] [OriginX OriginY [StepX/StepRadius StepY/DivNb RotAngle]]"
    " : Mode - rectangular or circular"
    " : Type - lines or points",
    __FILE__, VGrid, group);
  theCommands.Add ("vfps",
    "vfps [framesNb=100] : estimate average frame rate for active view",
    __FILE__, VFps, group);
  theCommands.Add ("vgldebug",
    "vgldebug [{0|1}] : request debug GL context, should be called before vinit\n"
    "                : this function is implemented only for Windows\n"
    "                : GL_ARB_debug_output extension should be exported by OpenGL driver!",
    __FILE__, VGlDebug, group);
  theCommands.Add ("vvbo",
    "vvbo [{0|1}] : turn VBO usage On/Off; affects only newly displayed objects",
    __FILE__, VVbo, group);
  theCommands.Add ("vcaps",
    "vcaps [vbo={0|1}] [sprites={0|1}] [soft={0|1}] : modify particular graphic driver options",
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
    "vselect x1 y1 [x2 y2 [x3 y3 ... xn yn]] [shift_selection = 0|1]\n"
    "- emulates different types of selection:\n"
    "- 1) single click selection\n"
    "- 2) selection with rectangle having corners at pixel positions (x1,y1) and (x2,y2)\n"
    "- 3) selection with polygon having corners in pixel positions (x1,y1), (x2,y2),...,(xn,yn)\n"
    "- 4) any of these selections with shift button pressed",
    __FILE__, VSelect, group);
  theCommands.Add ("vmoveto",
    "vmoveto x y"
    "- emulates cursor movement to pixel postion (x,y)",
    __FILE__, VMoveTo, group);
  theCommands.Add("vviewparams",
    "vviewparams [scale center_X center_Y proj_X proj_Y proj_Z up_X up_Y up_Z at_X at_Y at_Z]"
    "- gets or sets current view characteristics",
    __FILE__,VViewParams, group);
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
    "vnbselected", __FILE__, VNbSelected, group);
  theCommands.Add("vantialiasing",
    "vantialiasing 1|0",
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
    "is_enabled={on|off}"
    " - Hidden line removal algorithm:"
    " - is_enabled: if is on HLR algorithm is applied\n",
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
    "vdefaults [absDefl=value] [devCoeff=value] [angDefl=value]",
    __FILE__, VDefaults, group);
  theCommands.Add("vlight",
            "vlight [add|new {amb}ient|directional|{spot}light|positional]"
    "\n\t\t:        [{def}aults] [clear]"
    "\n\t\t:        [{del}ete|change lightId] [local|global]"
    "\n\t\t:        [{pos}ition X Y Z] [color colorName] [{head}light 0|1]"
    "\n\t\t:        [{constAtten}uation value] [{linearAtten}uation value]"
    "\n\t\t:        [angle angleDeg] [{spotexp}onent value]",
    __FILE__, VLight, group);
  theCommands.Add("vraytrace",
    "vraytrace 0|1",
    __FILE__,VRaytrace,group);
  theCommands.Add("vclinfo",
    "vclinfo",
    __FILE__,VClInfo,group);
  theCommands.Add("vsetraytracemode",
    "vsetraytracemode [shad=0|1] [refl=0|1] [aa=0|1]",
    __FILE__,VSetRaytraceMode,group);
}
