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

#if defined(_WIN32)
  #include <windows.h>
#endif

#include <ViewerTest.hxx>

#include <AIS_AnimationAxisRotation.hxx>
#include <AIS_AnimationCamera.hxx>
#include <AIS_AnimationObject.hxx>
#include <AIS_Axis.hxx>
#include <AIS_CameraFrustum.hxx>
#include <AIS_ColorScale.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_LightSource.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_Manipulator.hxx>
#include <AIS_ViewCube.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Point.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_Grid.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Draw_Appli.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <Graphic3d_CubeMapPacked.hxx>
#include <Graphic3d_CubeMapSeparate.hxx>
#include <Graphic3d_GraduatedTrihedron.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_GraphicDriverFactory.hxx>
#include <Graphic3d_NameOfTextureEnv.hxx>
#include <Graphic3d_Texture2D.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_TextureParams.hxx>
#include <Graphic3d_TypeOfTextureFilter.hxx>
#include <Image_AlienPixMap.hxx>
#include <Image_Diff.hxx>
#include <Image_VideoRecorder.hxx>
#include <Message.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <NCollection_LocalArray.hxx>
#include <OSD_Parallel.hxx>
#include <OSD_Timer.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Text.hxx>
#include <Select3D_SensitivePrimitiveArray.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <ViewerTest_AutoUpdater.hxx>
#include <ViewerTest_ContinuousRedrawer.hxx>
#include <ViewerTest_EventManager.hxx>
#include <ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_CmdParser.hxx>
#include <ViewerTest_V3dView.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_SpotLight.hxx>
#include <V3d_Trihedron.hxx>
#include <V3d_Viewer.hxx>
#include <UnitsAPI.hxx>

#include <tcl.h>

#if defined(_WIN32)
  #include <WNT_WClass.hxx>
  #include <WNT_Window.hxx>
#elif defined(HAVE_XLIB)
  #include <Xw_Window.hxx>
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
#elif defined(__APPLE__)
  #include <Cocoa_Window.hxx>
#elif defined(__EMSCRIPTEN__)
  #include <Wasm_Window.hxx>
  #include <emscripten/emscripten.h>
#else
  #include <Aspect_NeutralWindow.hxx>
#endif

//==============================================================================
//  VIEWER GLOBAL VARIABLES
//==============================================================================

Standard_IMPORT Standard_Boolean Draw_VirtualWindows;
Standard_IMPORT Standard_Boolean Draw_Interprete(const char* theCommand);

Standard_EXPORT int ViewerMainLoop(Standard_Integer, const char** argv);
extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();

#if defined(_WIN32)
typedef WNT_Window ViewerTest_Window;
#elif defined(HAVE_XLIB)
typedef Xw_Window ViewerTest_Window;
static void       VProcessEvents(ClientData, int);
#elif defined(__APPLE__)
typedef Cocoa_Window ViewerTest_Window;
extern void          ViewerTest_SetCocoaEventManagerView(const Handle(Cocoa_Window)& theWindow);
extern void GetCocoaScreenResolution(Standard_Integer& theWidth, Standard_Integer& theHeight);
#elif defined(__EMSCRIPTEN__)
typedef Wasm_Window ViewerTest_Window;
#else
typedef Aspect_NeutralWindow ViewerTest_Window;
#endif

#if defined(__EMSCRIPTEN__)
//! Return DOM id of default WebGL canvas from Module.canvas.
EM_JS(char*, occJSModuleCanvasId, (), {
  const aCanvasId = Module.canvas.id;
  const aNbBytes  = lengthBytesUTF8(aCanvasId) + 1;
  const aStrPtr   = Module._malloc(aNbBytes);
  stringToUTF8(aCanvasId, aStrPtr, aNbBytes);
  return aStrPtr;
});

//! Return DOM id of default WebGL canvas from Module.canvas.
static TCollection_AsciiString getModuleCanvasId()
{
  char*                   aRawId = occJSModuleCanvasId();
  TCollection_AsciiString anId(aRawId != NULL ? aRawId : "");
  free(aRawId);
  return anId;
}
#endif

namespace
{
static Handle(ViewerTest_Window)& VT_GetWindow()
{
  static Handle(ViewerTest_Window) aWindow;
  return aWindow;
}

static Handle(Aspect_DisplayConnection)& GetDisplayConnection()
{
  static Handle(Aspect_DisplayConnection) aDisplayConnection;
  return aDisplayConnection;
}

using ViewerTest_ViewerCommandsViewMap =
  NCollection_DoubleMap<TCollection_AsciiString, Handle(V3d_View)>;
using ViewerTest_ViewerCommandsInteractiveContextMap =
  NCollection_DoubleMap<TCollection_AsciiString, Handle(AIS_InteractiveContext)>;
using ViewerTest_ViewerCommandsGraphicDriverMap =
  NCollection_DoubleMap<TCollection_AsciiString, Handle(Graphic3d_GraphicDriver)>;

static void SetDisplayConnection(const Handle(Aspect_DisplayConnection)& theDisplayConnection)
{
  GetDisplayConnection() = theDisplayConnection;
}

static ViewerTest_ViewerCommandsInteractiveContextMap ViewerTest_myContexts;
static ViewerTest_ViewerCommandsGraphicDriverMap      ViewerTest_myDrivers;
} // namespace

ViewerTest_ViewerCommandsViewMap ViewerTest_myViews;

static struct
{
  Quantity_Color            FlatColor;
  Quantity_Color            GradientColor1;
  Quantity_Color            GradientColor2;
  Aspect_GradientFillMethod FillMethod;

  //! Sets the gradient filling for a background in a default viewer.
  void SetDefaultGradient()
  {
    for (ViewerTest_ViewerCommandsInteractiveContextMap::Iterator aCtxIter(ViewerTest_myContexts);
         aCtxIter.More();
         aCtxIter.Next())
    {
      const Handle(V3d_Viewer)& aViewer = aCtxIter.Key2()->CurrentViewer();
      aViewer->SetDefaultBgGradientColors(GradientColor1, GradientColor2, FillMethod);
    }
  }

  //! Sets the color used for filling a background in a default viewer.
  void SetDefaultColor()
  {
    for (ViewerTest_ViewerCommandsInteractiveContextMap::Iterator aCtxIter(ViewerTest_myContexts);
         aCtxIter.More();
         aCtxIter.Next())
    {
      const Handle(V3d_Viewer)& aViewer = aCtxIter.Key2()->CurrentViewer();
      aViewer->SetDefaultBackgroundColor(FlatColor);
    }
  }

} ViewerTest_DefaultBackground = {Quantity_NOC_BLACK,
                                  Quantity_NOC_BLACK,
                                  Quantity_NOC_BLACK,
                                  Aspect_GradientFillMethod_None};

//==============================================================================
//  EVENT GLOBAL VARIABLES
//==============================================================================

#ifdef _WIN32
static LRESULT WINAPI AdvViewerWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

//=================================================================================================

const Handle(WNT_WClass)& ViewerTest::WClass()
{
  static Handle(WNT_WClass) theWClass;
#if defined(_WIN32)
  if (theWClass.IsNull())
  {
    theWClass = new WNT_WClass("GW3D_Class",
                               (Standard_Address)AdvViewerWindowProc,
                               CS_VREDRAW | CS_HREDRAW,
                               0,
                               0,
                               ::LoadCursor(NULL, IDC_ARROW));
  }
#endif
  return theWClass;
}

//==============================================================================
// function : CreateName
// purpose  : Create numerical name for new object in theMap
//==============================================================================
template <typename ObjectType>
TCollection_AsciiString CreateName(
  const NCollection_DoubleMap<TCollection_AsciiString, ObjectType>& theObjectMap,
  const TCollection_AsciiString&                                    theDefaultString)
{
  if (theObjectMap.IsEmpty())
    return theDefaultString + TCollection_AsciiString(1);

  Standard_Integer aNextKey = 1;
  Standard_Boolean isFound  = Standard_False;
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
// structure : ViewerTest_Names
// purpose   : Allow to operate with full view name: driverName/viewerName/viewName
//==============================================================================
struct ViewerTest_Names
{
private:
  TCollection_AsciiString myDriverName;
  TCollection_AsciiString myViewerName;
  TCollection_AsciiString myViewName;

public:
  const TCollection_AsciiString& GetDriverName() const { return myDriverName; }

  void SetDriverName(const TCollection_AsciiString& theDriverName) { myDriverName = theDriverName; }

  const TCollection_AsciiString& GetViewerName() const { return myViewerName; }

  void SetViewerName(const TCollection_AsciiString& theViewerName) { myViewerName = theViewerName; }

  const TCollection_AsciiString& GetViewName() const { return myViewName; }

  void SetViewName(const TCollection_AsciiString& theViewName) { myViewName = theViewName; }

  //===========================================================================
  // function : Constructor for ViewerTest_Names
  // purpose  : Get view, viewer, driver names from custom string
  //===========================================================================

  ViewerTest_Names(const TCollection_AsciiString& theInputString)
  {
    TCollection_AsciiString aName(theInputString);
    if (theInputString.IsEmpty())
    {
      // Get current configuration
      if (ViewerTest_myDrivers.IsEmpty())
        myDriverName =
          CreateName<Handle(Graphic3d_GraphicDriver)>(ViewerTest_myDrivers,
                                                      TCollection_AsciiString("Driver"));
      else
        myDriverName =
          ViewerTest_myDrivers.Find2(ViewerTest::GetAISContext()->CurrentViewer()->Driver());

      if (ViewerTest_myContexts.IsEmpty())
      {
        myViewerName = CreateName<Handle(AIS_InteractiveContext)>(
          ViewerTest_myContexts,
          TCollection_AsciiString(myDriverName + "/Viewer"));
      }
      else
      {
        myViewerName = ViewerTest_myContexts.Find2(ViewerTest::GetAISContext());
      }

      myViewName = CreateName<Handle(V3d_View)>(ViewerTest_myViews,
                                                TCollection_AsciiString(myViewerName + "/View"));
    }
    else
    {
      // There is at least view name
      Standard_Integer aParserNumber = 0;
      for (Standard_Integer i = 0; i < 3; ++i)
      {
        Standard_Integer aParserPos = aName.SearchFromEnd("/");
        if (aParserPos != -1)
        {
          aParserNumber++;
          aName.Split(aParserPos - 1);
        }
        else
          break;
      }
      if (aParserNumber == 0)
      {
        // Only view name
        if (!ViewerTest::GetAISContext().IsNull())
        {
          myDriverName =
            ViewerTest_myDrivers.Find2(ViewerTest::GetAISContext()->CurrentViewer()->Driver());
          myViewerName = ViewerTest_myContexts.Find2(ViewerTest::GetAISContext());
        }
        else
        {
          // There is no opened contexts here, need to create names for viewer and driver
          myDriverName =
            CreateName<Handle(Graphic3d_GraphicDriver)>(ViewerTest_myDrivers,
                                                        TCollection_AsciiString("Driver"));

          myViewerName = CreateName<Handle(AIS_InteractiveContext)>(
            ViewerTest_myContexts,
            TCollection_AsciiString(myDriverName + "/Viewer"));
        }
        myViewName = TCollection_AsciiString(myViewerName + "/" + theInputString);
      }
      else if (aParserNumber == 1)
      {
        // Here is viewerName/viewName
        if (!ViewerTest::GetAISContext().IsNull())
          myDriverName =
            ViewerTest_myDrivers.Find2(ViewerTest::GetAISContext()->CurrentViewer()->Driver());
        else
        {
          // There is no opened contexts here, need to create name for driver
          myDriverName =
            CreateName<Handle(Graphic3d_GraphicDriver)>(ViewerTest_myDrivers,
                                                        TCollection_AsciiString("Driver"));
        }
        myViewerName = TCollection_AsciiString(myDriverName + "/" + aName);

        myViewName = TCollection_AsciiString(myDriverName + "/" + theInputString);
      }
      else
      {
        // Here is driverName/viewerName/viewName
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
// function : FindContextByView
// purpose  : Find AIS_InteractiveContext by View
//==============================================================================

Handle(AIS_InteractiveContext) FindContextByView(const Handle(V3d_View)& theView)
{
  Handle(AIS_InteractiveContext) anAISContext;

  for (ViewerTest_ViewerCommandsInteractiveContextMap::Iterator anIter(ViewerTest_myContexts);
       anIter.More();
       anIter.Next())
  {
    if (anIter.Key2()->CurrentViewer() == theView->Viewer())
      return anIter.Key2();
  }
  return anAISContext;
}

//==============================================================================
// function : IsWindowOverlapped
// purpose  : Check if theWindow overlapp another view
//==============================================================================

Standard_Boolean IsWindowOverlapped(const Standard_Integer   thePxLeft,
                                    const Standard_Integer   thePxTop,
                                    const Standard_Integer   thePxRight,
                                    const Standard_Integer   thePxBottom,
                                    TCollection_AsciiString& theViewId)
{
  for (ViewerTest_ViewerCommandsViewMap::Iterator anIter(ViewerTest_myViews); anIter.More();
       anIter.Next())
  {
    Standard_Integer aTop = 0, aLeft = 0, aRight = 0, aBottom = 0;
    anIter.Key2()->Window()->Position(aLeft, aTop, aRight, aBottom);
    if ((thePxLeft >= aLeft && thePxLeft <= aRight && thePxTop >= aTop && thePxTop <= aBottom)
        || (thePxLeft >= aLeft && thePxLeft <= aRight && thePxBottom >= aTop
            && thePxBottom <= aBottom)
        || (thePxRight >= aLeft && thePxRight <= aRight && thePxTop >= aTop && thePxTop <= aBottom)
        || (thePxRight >= aLeft && thePxRight <= aRight && thePxBottom >= aTop
            && thePxBottom <= aBottom))
    {
      theViewId = anIter.Key1();
      return Standard_True;
    }
  }
  return Standard_False;
}

// Workaround: to create and delete non-orthographic views outside ViewerTest
void ViewerTest::RemoveViewName(const TCollection_AsciiString& theName)
{
  ViewerTest_myViews.UnBind1(theName);
}

void ViewerTest::InitViewName(const TCollection_AsciiString& theName,
                              const Handle(V3d_View)&        theView)
{
  ViewerTest_myViews.Bind(theName, theView);
}

TCollection_AsciiString ViewerTest::GetCurrentViewName()
{
  return ViewerTest_myViews.Find2(ViewerTest::CurrentView());
}

//==============================================================================
// function : ViewerInit
// purpose  : Create the window viewer and initialize all the global variable
//==============================================================================

TCollection_AsciiString ViewerTest::ViewerInit(const ViewerTest_VinitParams& theParams)
{
  // Default position and dimension of the viewer window.
  // Note that left top corner is set to be sufficiently small to have
  // window fit in the small screens (actual for remote desktops, see #23003).
  // The position corresponds to the window's client area, thus some
  // gap is added for window frame to be visible.
  Graphic3d_Vec2d        aPxTopLeft(20, 40);
  Graphic3d_Vec2d        aPxSize(409, 409);
  Standard_Boolean       isDefViewSize  = Standard_True;
  Standard_Boolean       toCreateViewer = Standard_False;
  const Standard_Boolean isVirtual      = Draw_VirtualWindows || theParams.IsVirtual;
  if (!theParams.ViewToClone.IsNull())
  {
    Graphic3d_Vec2i aCloneSize;
    theParams.ViewToClone->Window()->Size(aCloneSize.x(), aCloneSize.y());
    aPxSize       = Graphic3d_Vec2d(aCloneSize);
    isDefViewSize = Standard_False;
#if !defined(__EMSCRIPTEN__)
    (void)isDefViewSize;
#endif
  }

  Handle(Graphic3d_GraphicDriverFactory) aFactory =
    Graphic3d_GraphicDriverFactory::DefaultDriverFactory();
  if (aFactory.IsNull())
  {
    Draw::GetInterpretor().Eval("pload OPENGL");
    aFactory = Graphic3d_GraphicDriverFactory::DefaultDriverFactory();
    if (aFactory.IsNull())
    {
      Draw::GetInterpretor().Eval("pload GLES");
      aFactory = Graphic3d_GraphicDriverFactory::DefaultDriverFactory();
      if (aFactory.IsNull())
      {
        throw Standard_ProgramError("Error: no graphic driver factory found");
      }
    }
  }

  Handle(Graphic3d_GraphicDriver) aGraphicDriver;
  ViewerTest_Names                aViewNames(theParams.ViewName);
  if (ViewerTest_myViews.IsBound1(aViewNames.GetViewName()))
  {
    aViewNames.SetViewName(aViewNames.GetViewerName() + "/"
                           + CreateName<Handle(V3d_View)>(ViewerTest_myViews, "View"));
  }

  // Get graphic driver (create it or get from another view)
  const bool isNewDriver = !ViewerTest_myDrivers.IsBound1(aViewNames.GetDriverName());
  if (isNewDriver)
  {
    // Get connection string
#if defined(HAVE_XLIB)
    if (!theParams.DisplayName.IsEmpty())
    {
      SetDisplayConnection(new Aspect_DisplayConnection(theParams.DisplayName));
    }
    else
    {
      Aspect_XDisplay* aDispX = NULL;
      // create dedicated display connection instead of reusing Tk connection
      // so that to proceed events independently through VProcessEvents()/ViewerMainLoop() callbacks
      /*Draw_Interpretor& aCommands = Draw::GetInterpretor();
      Tcl_Interp* aTclInterp = aCommands.Interp();
      Tk_Window aMainWindow = Tk_MainWindow (aTclInterp);
      aDispX = aMainWindow != NULL ? Tk_Display (aMainWindow) : NULL;*/
      SetDisplayConnection(new Aspect_DisplayConnection(aDispX));
    }
#else
    SetDisplayConnection(new Aspect_DisplayConnection());
#endif

    aGraphicDriver = aFactory->CreateDriver(GetDisplayConnection());
    if (isVirtual)
    {
      // don't waste the time waiting for VSync when window is not displayed on the screen
      aGraphicDriver->SetVerticalSync(false);
    }

    ViewerTest_myDrivers.Bind(aViewNames.GetDriverName(), aGraphicDriver);
    toCreateViewer = Standard_True;
  }
  else
  {
    aGraphicDriver = ViewerTest_myDrivers.Find1(aViewNames.GetDriverName());
  }

  // Get screen resolution
  Graphic3d_Vec2i aScreenSize;
#if defined(_WIN32)
  RECT aWindowSize;
  GetClientRect(GetDesktopWindow(), &aWindowSize);
  aScreenSize.SetValues(aWindowSize.right, aWindowSize.bottom);
#elif defined(HAVE_XLIB)
  ::Display* aDispX  = (::Display*)GetDisplayConnection()->GetDisplayAspect();
  Screen*    aScreen = DefaultScreenOfDisplay(aDispX);
  aScreenSize.x()    = WidthOfScreen(aScreen);
  aScreenSize.y()    = HeightOfScreen(aScreen);
#elif defined(__APPLE__)
  GetCocoaScreenResolution(aScreenSize.x(), aScreenSize.y());
#else
  // not implemented
#endif

  if (!theParams.ParentView.IsNull())
  {
    aPxTopLeft.SetValues(0, 0);
  }
  if (theParams.Offset.x() != 0)
  {
    aPxTopLeft.x() = theParams.Offset.x();
  }
  if (theParams.Offset.y() != 0)
  {
    aPxTopLeft.y() = theParams.Offset.y();
  }
  if (theParams.Size.x() != 0)
  {
    isDefViewSize = Standard_False;
    aPxSize.x()   = theParams.Size.x();
    if (aPxSize.x() <= 1.0 && aScreenSize.x() > 0 && theParams.ParentView.IsNull())
    {
      aPxSize.x() = aPxSize.x() * double(aScreenSize.x());
    }
  }
  if (theParams.Size.y() != 0)
  {
    isDefViewSize = Standard_False;
    aPxSize.y()   = theParams.Size.y();
    if (aPxSize.y() <= 1.0 && aScreenSize.y() > 0 && theParams.ParentView.IsNull())
    {
      aPxSize.y() = aPxSize.y() * double(aScreenSize.y());
    }
  }

  // Dispose the window if input parameters are default
  if (!ViewerTest_myViews.IsEmpty() && theParams.ParentView.IsNull() && theParams.Offset.x() == 0
      && theParams.Offset.y() == 0)
  {
    Standard_Integer        aTop = 0, aLeft = 0, aRight = 0, aBottom = 0;
    TCollection_AsciiString anOverlappedViewId("");
    while (IsWindowOverlapped((int)aPxTopLeft.x(),
                              (int)aPxTopLeft.y(),
                              (int)aPxTopLeft.x() + (int)aPxSize.x(),
                              (int)aPxTopLeft.y() + (int)aPxSize.y(),
                              anOverlappedViewId))
    {
      ViewerTest_myViews.Find1(anOverlappedViewId)
        ->Window()
        ->Position(aLeft, aTop, aRight, aBottom);

      if (IsWindowOverlapped(aRight + 20,
                             (int)aPxTopLeft.y(),
                             aRight + 20 + (int)aPxSize.x(),
                             (int)aPxTopLeft.y() + (int)aPxSize.y(),
                             anOverlappedViewId)
          && aRight + 2 * aPxSize.x() + 40 > aScreenSize.x())
      {
        if (aBottom + aPxSize.y() + 40 > aScreenSize.y())
        {
          aPxTopLeft.x() = 20;
          aPxTopLeft.y() = 40;
          break;
        }
        aPxTopLeft.x() = 20;
        aPxTopLeft.y() = aBottom + 40;
      }
      else
      {
        aPxTopLeft.x() = aRight + 20;
      }
    }
  }

  // Get viewer name
  TCollection_AsciiString aTitle("3D View - ");
  aTitle = aTitle + aViewNames.GetViewName() + "(*)";

  // Change name of current active window
  if (const Handle(V3d_View)& aCurrentView = ViewerTest::CurrentView())
  {
    aCurrentView->Window()->SetTitle(TCollection_AsciiString("3D View - ")
                                     + ViewerTest_myViews.Find2(aCurrentView));
  }

  // Create viewer
  Handle(V3d_Viewer) a3DViewer;
  // If it's the single view, we first look for empty context
  if (ViewerTest_myViews.IsEmpty() && !ViewerTest_myContexts.IsEmpty())
  {
    ViewerTest_ViewerCommandsInteractiveContextMap::Iterator anIter(ViewerTest_myContexts);
    if (anIter.More())
      ViewerTest::SetAISContext(anIter.Key2());
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
    a3DViewer      = new V3d_Viewer(aGraphicDriver);
    a3DViewer->SetDefaultBackgroundColor(ViewerTest_DefaultBackground.FlatColor);
    a3DViewer->SetDefaultBgGradientColors(ViewerTest_DefaultBackground.GradientColor1,
                                          ViewerTest_DefaultBackground.GradientColor2,
                                          ViewerTest_DefaultBackground.FillMethod);
  }

  // AIS context setup
  if (ViewerTest::GetAISContext().IsNull()
      || !(ViewerTest_myContexts.IsBound1(aViewNames.GetViewerName())))
  {
    Handle(AIS_InteractiveContext) aContext = new AIS_InteractiveContext(a3DViewer);
    ViewerTest::SetAISContext(aContext);
    ViewerTest_myContexts.Bind(aViewNames.GetViewerName(), ViewerTest::GetAISContext());
  }
  else
  {
    ViewerTest::ResetEventManager();
  }

  // Create window
  if (!theParams.ParentView.IsNull())
  {
    VT_GetWindow() = Handle(ViewerTest_Window)::DownCast(theParams.ParentView->Window());
  }
  else
  {
#if defined(_WIN32)
    VT_GetWindow() = new WNT_Window(aTitle.ToCString(),
                                    WClass(),
                                    isVirtual ? WS_POPUP : WS_OVERLAPPEDWINDOW,
                                    (int)aPxTopLeft.x(),
                                    (int)aPxTopLeft.y(),
                                    (int)aPxSize.x(),
                                    (int)aPxSize.y(),
                                    Quantity_NOC_BLACK);
    VT_GetWindow()->RegisterRawInputDevices(WNT_Window::RawInputMask_SpaceMouse);
#elif defined(HAVE_XLIB)
    VT_GetWindow() = new Xw_Window(aGraphicDriver->GetDisplayConnection(),
                                   aTitle.ToCString(),
                                   (int)aPxTopLeft.x(),
                                   (int)aPxTopLeft.y(),
                                   (int)aPxSize.x(),
                                   (int)aPxSize.y());
#elif defined(__APPLE__)
    VT_GetWindow() = new Cocoa_Window(aTitle.ToCString(),
                                      (int)aPxTopLeft.x(),
                                      (int)aPxTopLeft.y(),
                                      (int)aPxSize.x(),
                                      (int)aPxSize.y());
    ViewerTest_SetCocoaEventManagerView(VT_GetWindow());
#elif defined(__EMSCRIPTEN__)
    // current EGL implementation in Emscripten supports only one global WebGL canvas returned by
    // Module.canvas property; the code should be revised for handling multiple canvas elements
    // (which is technically also possible)
    TCollection_AsciiString aCanvasId = getModuleCanvasId();
    if (!aCanvasId.IsEmpty())
    {
      aCanvasId = TCollection_AsciiString("#") + aCanvasId;
    }

    VT_GetWindow() = new Wasm_Window(aCanvasId);
    Graphic3d_Vec2i aRealSize;
    VT_GetWindow()->Size(aRealSize.x(), aRealSize.y());
    if (!isDefViewSize || (aRealSize.x() <= 0 && aRealSize.y() <= 0))
    {
      // Wasm_Window wraps an existing HTML element without creating a new one.
      // Keep size defined on a web page instead of defaulting to 409x409 (as in case of other
      // platform), but resize canvas if vinit has been called with explicitly specified dimensions.
      VT_GetWindow()->SetSizeLogical(Graphic3d_Vec2d(aPxSize));
    }
#else
    // not implemented
    VT_GetWindow() = new Aspect_NeutralWindow();
    VT_GetWindow()->SetSize((int)aPxSize.x(), (int)aPxSize.y());
#endif
    VT_GetWindow()->SetVirtual(isVirtual);
  }

  // View setup
  Handle(V3d_View) aView;
  if (!theParams.ViewToClone.IsNull())
  {
    aView = new ViewerTest_V3dView(a3DViewer, theParams.ViewToClone);
  }
  else
  {
    aView = new ViewerTest_V3dView(a3DViewer, a3DViewer->DefaultTypeOfView());
  }

  aView->View()->SetSubviewComposer(theParams.IsComposer);
  if (!theParams.ParentView.IsNull())
  {
    aView->SetWindow(theParams.ParentView,
                     aPxSize,
                     theParams.Corner,
                     aPxTopLeft,
                     theParams.SubviewMargins);
  }
  else
  {
    aView->SetWindow(VT_GetWindow());
  }
  ViewerTest::GetAISContext()->RedrawImmediate(a3DViewer);

  ViewerTest::CurrentView(aView);
  ViewerTest_myViews.Bind(aViewNames.GetViewName(), aView);

  // Setup for X11 or NT
  SetDisplayConnection(ViewerTest::CurrentView()->Viewer()->Driver()->GetDisplayConnection());
  ViewerTest_EventManager::SetupWindowCallbacks(VT_GetWindow());

  // Set parameters for V3d_View and V3d_Viewer
  const Handle(V3d_View) aV3dView = ViewerTest::CurrentView();
  aV3dView->SetComputedMode(Standard_False);

  a3DViewer->SetDefaultBackgroundColor(Quantity_NOC_BLACK);
  if (toCreateViewer)
  {
    a3DViewer->SetDefaultLights();
    a3DViewer->SetLightOn();
  }

#if defined(HAVE_XLIB)
  if (isNewDriver)
  {
    ::Display* aDispX = (::Display*)GetDisplayConnection()->GetDisplayAspect();
    Tcl_CreateFileHandler(XConnectionNumber(aDispX),
                          TCL_READABLE,
                          VProcessEvents,
                          (ClientData)aDispX);
  }
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
// function : RedrawAllViews
// purpose  : Redraw all created views
//==============================================================================
void ViewerTest::RedrawAllViews()
{
  ViewerTest_ViewerCommandsViewMap::Iterator aViewIt(ViewerTest_myViews);
  for (; aViewIt.More(); aViewIt.Next())
  {
    const Handle(V3d_View)& aView = aViewIt.Key2();
    aView->Redraw();
  }
}

//=================================================================================================

static int VDriver(Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  if (theArgsNb == 1)
  {
    theDi << "Registered: ";
    for (Graphic3d_GraphicDriverFactoryList::Iterator aFactoryIter(
           Graphic3d_GraphicDriverFactory::DriverFactories());
         aFactoryIter.More();
         aFactoryIter.Next())
    {
      const Handle(Graphic3d_GraphicDriverFactory)& aFactory = aFactoryIter.Value();
      theDi << aFactory->Name() << " ";
    }

    theDi << "\n";
    theDi << "Default: ";
    if (Handle(Graphic3d_GraphicDriverFactory) aFactory =
          Graphic3d_GraphicDriverFactory::DefaultDriverFactory())
    {
      theDi << aFactory->Name();
    }
    else
    {
      theDi << "NONE";
    }
    return 0;
  }

  TCollection_AsciiString aNewActive;
  bool                    toLoad = false;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if (anArgCase == "-list")
    {
      for (Graphic3d_GraphicDriverFactoryList::Iterator aFactoryIter(
             Graphic3d_GraphicDriverFactory::DriverFactories());
           aFactoryIter.More();
           aFactoryIter.Next())
      {
        const Handle(Graphic3d_GraphicDriverFactory)& aFactory = aFactoryIter.Value();
        theDi << aFactory->Name() << " ";
      }
    }
    else if ((anArgCase == "-default" || anArgCase == "-load") && aNewActive.IsEmpty())
    {
      toLoad = (anArgCase == "-load");
      if (anArgIter + 1 < theArgsNb)
      {
        aNewActive = theArgVec[++anArgIter];
      }
      else if (toLoad)
      {
        theDi << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }
      else
      {
        if (Handle(Graphic3d_GraphicDriverFactory) aFactory =
              Graphic3d_GraphicDriverFactory::DefaultDriverFactory())
        {
          theDi << aFactory->Name();
        }
        else
        {
          theDi << "NONE";
        }
      }
    }
    else if (aNewActive.IsEmpty())
    {
      aNewActive = theArgVec[anArgIter];
    }
    else
    {
      theDi << "Syntax error: unknown argument '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (!aNewActive.IsEmpty())
  {
    const TCollection_AsciiString aNameCopy = aNewActive;
    if (TCollection_AsciiString::IsSameString(aNewActive, "gl", false)
        || TCollection_AsciiString::IsSameString(aNewActive, "opengl", false)
        || TCollection_AsciiString::IsSameString(aNewActive, "tkopengl", false))
    {
      aNewActive = "tkopengl";
    }
    else if (TCollection_AsciiString::IsSameString(aNewActive, "gles", false)
             || TCollection_AsciiString::IsSameString(aNewActive, "opengles", false)
             || TCollection_AsciiString::IsSameString(aNewActive, "tkopengles", false))
    {
      aNewActive = "tkopengles";
    }
    else if (TCollection_AsciiString::IsSameString(aNewActive, "d3d", false)
             || TCollection_AsciiString::IsSameString(aNewActive, "d3dhost", false)
             || TCollection_AsciiString::IsSameString(aNewActive, "tkd3dhost", false))
    {
      aNewActive = "tkd3dhost";
    }

    if (toLoad)
    {
      if (aNewActive == "tkopengl")
      {
        Draw::GetInterpretor().Eval("pload OPENGL");
      }
      else if (aNewActive == "tkopengles")
      {
        Draw::GetInterpretor().Eval("pload GLES");
      }
      else if (aNewActive == "tkd3dhost")
      {
        Draw::GetInterpretor().Eval("pload D3DHOST");
      }
      else
      {
        theDi << "Syntax error: unable to load plugin for unknown driver factory '" << aNameCopy
              << "'";
        return 1;
      }
    }

    bool isFound = false;
    for (Graphic3d_GraphicDriverFactoryList::Iterator aFactoryIter(
           Graphic3d_GraphicDriverFactory::DriverFactories());
         aFactoryIter.More();
         aFactoryIter.Next())
    {
      Handle(Graphic3d_GraphicDriverFactory) aFactory = aFactoryIter.Value();
      if (TCollection_AsciiString::IsSameString(aFactory->Name(), aNewActive, false))
      {
        Graphic3d_GraphicDriverFactory::RegisterFactory(aFactory, true);
        isFound = true;
        break;
      }
    }

    if (!isFound)
    {
      theDi << "Syntax error: driver factory '" << aNameCopy << "' not found";
      return 1;
    }
  }

  return 0;
}

//==============================================================================
// function : Vinit
// purpose  : Create the window viewer and initialize all the global variable
//    Use Tcl_CreateFileHandler on UNIX to catch the X11 Viewer event
//==============================================================================
static int VInit(Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  ViewerTest_VinitParams  aParams;
  TCollection_AsciiString aName, aValue;
  int                     is2dMode = -1, aDpiAware = -1;
  for (Standard_Integer anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    const TCollection_AsciiString anArg     = theArgVec[anArgIt];
    TCollection_AsciiString       anArgCase = anArg;
    anArgCase.LowerCase();
    if (anArgIt + 1 < theArgsNb && anArgCase == "-name")
    {
      aParams.ViewName = theArgVec[++anArgIt];
    }
    else if (anArgIt + 1 < theArgsNb && (anArgCase == "-left" || anArgCase == "-l")
             && Draw::ParseReal(theArgVec[anArgIt + 1], aParams.Offset.x()))
    {
      ++anArgIt;
    }
    else if (anArgIt + 1 < theArgsNb && (anArgCase == "-top" || anArgCase == "-t")
             && Draw::ParseReal(theArgVec[anArgIt + 1], aParams.Offset.y()))
    {
      ++anArgIt;
    }
    else if (anArgIt + 1 < theArgsNb && (anArgCase == "-width" || anArgCase == "-w")
             && Draw::ParseReal(theArgVec[anArgIt + 1], aParams.Size.x()))
    {
      ++anArgIt;
    }
    else if (anArgIt + 1 < theArgsNb && (anArgCase == "-height" || anArgCase == "-h")
             && Draw::ParseReal(theArgVec[anArgIt + 1], aParams.Size.y()))
    {
      ++anArgIt;
    }
    else if (anArgIt + 1 < theArgsNb
             && (anArgCase == "-pos" || anArgCase == "-position" || anArgCase == "-corner")
             && ViewerTest::ParseCorner(theArgVec[anArgIt + 1], aParams.Corner))
    {
      ++anArgIt;
    }
    else if (anArgIt + 2 < theArgsNb && anArgCase == "-margins"
             && Draw::ParseInteger(theArgVec[anArgIt + 1], aParams.SubviewMargins.x())
             && Draw::ParseInteger(theArgVec[anArgIt + 2], aParams.SubviewMargins.y()))
    {
      anArgIt += 2;
    }
    else if (anArgCase == "-virtual" || anArgCase == "-offscreen")
    {
      aParams.IsVirtual = Draw::ParseOnOffIterator(theArgsNb, theArgVec, anArgIt);
      ;
    }
    else if (anArgCase == "-composer")
    {
      aParams.IsComposer = Draw::ParseOnOffIterator(theArgsNb, theArgVec, anArgIt);
    }
    else if (anArgCase == "-exitonclose")
    {
      ViewerTest_EventManager::ToExitOnCloseView() =
        Draw::ParseOnOffIterator(theArgsNb, theArgVec, anArgIt);
      ;
    }
    else if (anArgCase == "-closeonescape" || anArgCase == "-closeonesc")
    {
      ViewerTest_EventManager::ToCloseViewOnEscape() =
        Draw::ParseOnOffIterator(theArgsNb, theArgVec, anArgIt);
      ;
    }
    else if (anArgCase == "-2d_mode" || anArgCase == "-2dmode" || anArgCase == "-2d")
    {
      bool toEnable = Draw::ParseOnOffIterator(theArgsNb, theArgVec, anArgIt);
      ;
      is2dMode = toEnable ? 1 : 0;
    }
    else if (anArgIt + 1 < theArgsNb && (anArgCase == "-disp" || anArgCase == "-display"))
    {
      aParams.DisplayName = theArgVec[++anArgIt];
    }
    else if (anArgCase == "-dpiaware")
    {
      aDpiAware = Draw::ParseOnOffIterator(theArgsNb, theArgVec, anArgIt) ? 1 : 0;
    }
    else if (!ViewerTest::CurrentView().IsNull() && aParams.ViewToClone.IsNull()
             && (anArgCase == "-copy" || anArgCase == "-clone" || anArgCase == "-cloneactive"
                 || anArgCase == "-cloneactiveview"))
    {
      aParams.ViewToClone = ViewerTest::CurrentView();
    }
    else if (!ViewerTest::CurrentView().IsNull() && aParams.ParentView.IsNull()
             && anArgCase == "-subview")
    {
      aParams.ParentView = ViewerTest::CurrentView();
      if (aParams.ParentView.IsNull())
      {
        Message::SendFail() << "Syntax error: cannot create of subview without parent";
        return 1;
      }
      if (aParams.ParentView->IsSubview())
      {
        aParams.ParentView = aParams.ParentView->ParentView();
      }
    }
    else if (!ViewerTest::CurrentView().IsNull() && aParams.ParentView.IsNull()
             && anArgCase == "-parent" && anArgIt + 1 < theArgsNb)
    {
      TCollection_AsciiString aParentStr(theArgVec[++anArgIt]);
      ViewerTest_Names        aViewNames(aParentStr);
      if (!ViewerTest_myViews.IsBound1(aViewNames.GetViewName()))
      {
        Message::SendFail() << "Syntax error: parent view '" << aParentStr << "' not found";
        return 1;
      }

      aParams.ParentView = ViewerTest_myViews.Find1(aViewNames.GetViewName());
      if (aParams.ParentView->IsSubview())
      {
        aParams.ParentView = aParams.ParentView->ParentView();
      }
    }
    // old syntax
    else if (ViewerTest::SplitParameter(anArg, aName, aValue))
    {
      aName.LowerCase();
      if (aName == "name")
      {
        aParams.ViewName = aValue;
      }
      else if (aName == "l" || aName == "left")
      {
        aParams.Offset.x() = (float)aValue.RealValue();
      }
      else if (aName == "t" || aName == "top")
      {
        aParams.Offset.y() = (float)aValue.RealValue();
      }
      else if (aName == "disp" || aName == "display")
      {
        aParams.DisplayName = aValue;
      }
      else if (aName == "w" || aName == "width")
      {
        aParams.Size.x() = (float)aValue.RealValue();
      }
      else if (aName == "h" || aName == "height")
      {
        aParams.Size.y() = (float)aValue.RealValue();
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown argument " << anArg;
        return 1;
      }
    }
    else if (aParams.ViewName.IsEmpty())
    {
      aParams.ViewName = anArg;
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument " << anArg;
      return 1;
    }
  }

#if defined(_WIN32)
  if (aDpiAware != -1)
  {
    typedef void*(WINAPI * SetThreadDpiAwarenessContext_t)(void*);
    if (HMODULE aUser32Module = GetModuleHandleW(L"User32"))
    {
      SetThreadDpiAwarenessContext_t aSetDpiAware =
        (SetThreadDpiAwarenessContext_t)GetProcAddress(aUser32Module,
                                                       "SetThreadDpiAwarenessContext");
      if (aDpiAware == 1)
      {
        // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
        if (aSetDpiAware((void*)-4) == NULL)
        {
          // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE for older systems
          if (aSetDpiAware((void*)-3) == NULL)
          {
            Message::SendFail() << "Error: unable to enable DPI awareness";
          }
        }
      }
      else
      {
        // DPI_AWARENESS_CONTEXT_UNAWARE
        if (aSetDpiAware((void*)-1) == NULL)
        {
          Message::SendFail() << "Error: unable to disable DPI awareness";
        }
      }
    }
  }
#else
  (void)aDpiAware;
  #if !defined(HAVE_XLIB)
  if (!aParams.DisplayName.IsEmpty())
  {
    aParams.DisplayName.Clear();
    Message::SendWarning() << "Warning: display parameter will be ignored.\n";
  }
  #endif
#endif

  ViewerTest_Names aViewNames(aParams.ViewName);
  if (ViewerTest_myViews.IsBound1(aViewNames.GetViewName()))
  {
    TCollection_AsciiString aCommand =
      TCollection_AsciiString("vactivate ") + aViewNames.GetViewName();
    theDi.Eval(aCommand.ToCString());
    if (is2dMode != -1)
    {
      ViewerTest_V3dView::SetCurrentView2DMode(is2dMode == 1);
    }
    return 0;
  }

  TCollection_AsciiString aViewId = ViewerTest::ViewerInit(aParams);
  if (is2dMode != -1)
  {
    ViewerTest_V3dView::SetCurrentView2DMode(is2dMode == 1);
  }
  theDi << aViewId;
  return 0;
}

//! Parse HLR algo type.
static Standard_Boolean parseHlrAlgoType(const char* theName, Prs3d_TypeOfHLR& theType)
{
  TCollection_AsciiString aName(theName);
  aName.LowerCase();
  if (aName == "polyalgo")
  {
    theType = Prs3d_TOH_PolyAlgo;
  }
  else if (aName == "algo")
  {
    theType = Prs3d_TOH_Algo;
  }
  else
  {
    return Standard_False;
  }
  return Standard_True;
}

//==============================================================================
// function : VHLR
// purpose  : hidden lines removal algorithm
//==============================================================================

static int VHLR(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  const Handle(V3d_View)               aView = ViewerTest::CurrentView();
  const Handle(AIS_InteractiveContext) aCtx  = ViewerTest::GetAISContext();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Standard_Boolean       hasHlrOnArg      = Standard_False;
  Standard_Boolean       hasShowHiddenArg = Standard_False;
  Standard_Boolean       isHLROn          = Standard_False;
  Standard_Boolean       toShowHidden     = aCtx->DefaultDrawer()->DrawHiddenLine();
  Prs3d_TypeOfHLR        aTypeOfHLR       = Prs3d_TOH_NotSet;
  ViewerTest_AutoUpdater anUpdateTool(Handle(AIS_InteractiveContext)(), aView);
  for (Standard_Integer anArgIter = 1; anArgIter < argc; ++anArgIter)
  {
    TCollection_AsciiString anArg(argv[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode(anArg))
    {
      continue;
    }
    else if (anArg == "-showhidden" && anArgIter + 1 < argc
             && Draw::ParseOnOff(argv[anArgIter + 1], toShowHidden))
    {
      ++anArgIter;
      hasShowHiddenArg = Standard_True;
      continue;
    }
    else if ((anArg == "-type" || anArg == "-algo" || anArg == "-algotype") && anArgIter + 1 < argc
             && parseHlrAlgoType(argv[anArgIter + 1], aTypeOfHLR))
    {
      ++anArgIter;
      continue;
    }
    else if (!hasHlrOnArg && Draw::ParseOnOff(argv[anArgIter], isHLROn))
    {
      hasHlrOnArg = Standard_True;
      continue;
    }
    // old syntax
    else if (!hasShowHiddenArg && Draw::ParseOnOff(argv[anArgIter], toShowHidden))
    {
      hasShowHiddenArg = Standard_True;
      continue;
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << argv[anArgIter] << "'";
      return 1;
    }
  }
  if (!hasHlrOnArg)
  {
    di << "HLR:        " << aView->ComputedMode() << "\n";
    di << "HiddenLine: " << aCtx->DefaultDrawer()->DrawHiddenLine() << "\n";
    di << "HlrAlgo:    ";
    switch (aCtx->DefaultDrawer()->TypeOfHLR())
    {
      case Prs3d_TOH_NotSet:
        di << "NotSet\n";
        break;
      case Prs3d_TOH_PolyAlgo:
        di << "PolyAlgo\n";
        break;
      case Prs3d_TOH_Algo:
        di << "Algo\n";
        break;
    }
    anUpdateTool.Invalidate();
    return 0;
  }

  Standard_Boolean toRecompute = Standard_False;
  if (aTypeOfHLR != Prs3d_TOH_NotSet && aTypeOfHLR != aCtx->DefaultDrawer()->TypeOfHLR())
  {
    toRecompute = Standard_True;
    aCtx->DefaultDrawer()->SetTypeOfHLR(aTypeOfHLR);
  }
  if (toShowHidden != aCtx->DefaultDrawer()->DrawHiddenLine())
  {
    toRecompute = Standard_True;
    if (toShowHidden)
    {
      aCtx->DefaultDrawer()->EnableDrawHiddenLine();
    }
    else
    {
      aCtx->DefaultDrawer()->DisableDrawHiddenLine();
    }
  }

  // redisplay shapes
  if (aView->ComputedMode() && isHLROn && toRecompute)
  {
    AIS_ListOfInteractive aListOfShapes;
    aCtx->DisplayedObjects(aListOfShapes);
    for (AIS_ListIteratorOfListOfInteractive anIter(aListOfShapes); anIter.More(); anIter.Next())
    {
      if (Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(anIter.Value()))
      {
        aCtx->Redisplay(aShape, Standard_False);
      }
    }
  }

  aView->SetComputedMode(isHLROn);
  return 0;
}

//==============================================================================
// function : VHLRType
// purpose  : change type of using HLR algorithm
//==============================================================================

static int VHLRType(Draw_Interpretor&, Standard_Integer argc, const char** argv)
{
  const Handle(V3d_View)               aView = ViewerTest::CurrentView();
  const Handle(AIS_InteractiveContext) aCtx  = ViewerTest::GetAISContext();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Prs3d_TypeOfHLR        aTypeOfHLR = Prs3d_TOH_NotSet;
  ViewerTest_AutoUpdater anUpdateTool(Handle(AIS_InteractiveContext)(), aView);
  AIS_ListOfInteractive  aListOfShapes;
  for (Standard_Integer anArgIter = 1; anArgIter < argc; ++anArgIter)
  {
    TCollection_AsciiString anArg(argv[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode(anArg))
    {
      continue;
    }
    else if ((anArg == "-type" || anArg == "-algo" || anArg == "-algotype") && anArgIter + 1 < argc
             && parseHlrAlgoType(argv[anArgIter + 1], aTypeOfHLR))
    {
      ++anArgIter;
      continue;
    }
    // old syntax
    else if (aTypeOfHLR == Prs3d_TOH_NotSet && parseHlrAlgoType(argv[anArgIter], aTypeOfHLR))
    {
      continue;
    }
    else
    {
      ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
      TCollection_AsciiString                   aName(argv[anArgIter]);
      if (!aMap.IsBound2(aName))
      {
        Message::SendFail() << "Syntax error: Wrong shape name '" << aName << "'";
        return 1;
      }

      Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(aMap.Find2(aName));
      if (aShape.IsNull())
      {
        Message::SendFail() << "Syntax error: '" << aName << "' is not a shape presentation";
        return 1;
      }
      aListOfShapes.Append(aShape);
      continue;
    }
  }
  if (aTypeOfHLR == Prs3d_TOH_NotSet)
  {
    Message::SendFail("Syntax error: wrong number of arguments");
    return 1;
  }

  const Standard_Boolean isGlobal = aListOfShapes.IsEmpty();
  if (isGlobal)
  {
    aCtx->DisplayedObjects(aListOfShapes);
    aCtx->DefaultDrawer()->SetTypeOfHLR(aTypeOfHLR);
  }

  for (AIS_ListIteratorOfListOfInteractive anIter(aListOfShapes); anIter.More(); anIter.Next())
  {
    Handle(AIS_Shape) aShape = Handle(AIS_Shape)::DownCast(anIter.Value());
    if (aShape.IsNull())
    {
      continue;
    }

    const bool toUpdateShape = aShape->TypeOfHLR() != aTypeOfHLR && aView->ComputedMode();
    if (!isGlobal || aShape->TypeOfHLR() != aTypeOfHLR)
    {
      aShape->SetTypeOfHLR(aTypeOfHLR);
    }
    if (toUpdateShape)
    {
      aCtx->Redisplay(aShape, Standard_False);
    }
  }
  return 0;
}

//==============================================================================
// function : FindViewIdByWindowHandle
// purpose  : Find theView Id in the map of views by window handle
//==============================================================================
#if defined(_WIN32) || defined(HAVE_XLIB)
static TCollection_AsciiString FindViewIdByWindowHandle(Aspect_Drawable theWindowHandle)
{
  for (ViewerTest_ViewerCommandsViewMap::Iterator anIter(ViewerTest_myViews); anIter.More();
       anIter.Next())
  {
    Aspect_Drawable aWindowHandle = anIter.Key2()->Window()->NativeHandle();
    if (aWindowHandle == theWindowHandle)
      return anIter.Key1();
  }
  return TCollection_AsciiString("");
}
#endif

//! Make the view active
void ActivateView(const TCollection_AsciiString& theViewName,
                  Standard_Boolean               theToUpdate = Standard_True)
{
  if (const Handle(V3d_View)& aView = ViewerTest_myViews.Find1(theViewName))
  {
    ViewerTest::ActivateView(aView, theToUpdate);
  }
}

//=================================================================================================

void ViewerTest::ActivateView(const Handle(V3d_View)& theView, Standard_Boolean theToUpdate)
{
  const Handle(V3d_View)&        aView     = theView;
  const TCollection_AsciiString* aViewName = ViewerTest_myViews.Seek2(aView);
  if (aViewName == nullptr)
  {
    return;
  }

  Handle(AIS_InteractiveContext) anAISContext = FindContextByView(aView);
  if (anAISContext.IsNull())
  {
    return;
  }

  if (const Handle(V3d_View)& aCurrentView = ViewerTest::CurrentView())
  {
    if (!aCurrentView->Window().IsNull())
    {
      aCurrentView->Window()->SetTitle(TCollection_AsciiString("3D View - ")
                                       + ViewerTest_myViews.Find2(aCurrentView));
    }
  }

  ViewerTest::CurrentView(aView);
  ViewerTest::SetAISContext(anAISContext);
  if (aView->IsSubview())
  {
    aView->ParentView()->Window()->SetTitle(TCollection_AsciiString("3D View - ") + *aViewName
                                            + "(*)");
    VT_GetWindow() = Handle(ViewerTest_Window)::DownCast(aView->View()->ParentView()->Window());
  }
  else
  {
    VT_GetWindow() = Handle(ViewerTest_Window)::DownCast(aView->Window());
  }
  if (!VT_GetWindow().IsNull())
  {
    VT_GetWindow()->SetTitle(TCollection_AsciiString("3D View - ") + *aViewName + "(*)");
  }
  SetDisplayConnection(aView->Viewer()->Driver()->GetDisplayConnection());
  if (theToUpdate)
  {
    aView->Redraw();
  }
}

//=================================================================================================

void ViewerTest::RemoveView(const Handle(V3d_View)& theView,
                            const Standard_Boolean  theToRemoveContext)
{
  if (!ViewerTest_myViews.IsBound2(theView))
  {
    return;
  }

  const TCollection_AsciiString& aViewName = ViewerTest_myViews.Find2(theView);
  RemoveView(aViewName, theToRemoveContext);
}

//==============================================================================
// function : RemoveView
// purpose  : Close and remove view from display, clear maps if necessary
//==============================================================================
void ViewerTest::RemoveView(const TCollection_AsciiString& theViewName,
                            const Standard_Boolean         isContextRemoved)
{
  if (!ViewerTest_myViews.IsBound1(theViewName))
  {
    Message::SendFail() << "Wrong view name";
    return;
  }

  Handle(V3d_View)               aView           = ViewerTest_myViews.Find1(theViewName);
  Handle(AIS_InteractiveContext) aCurrentContext = FindContextByView(aView);
  ViewerTest_ContinuousRedrawer& aRedrawer       = ViewerTest_ContinuousRedrawer::Instance();
  aRedrawer.Stop(aView);
  if (!aView->Subviews().IsEmpty())
  {
    NCollection_Sequence<Handle(V3d_View)> aSubviews = aView->Subviews();
    for (const Handle(V3d_View)& aSubviewIter : aSubviews)
    {
      RemoveView(aSubviewIter, isContextRemoved);
    }
  }

  // Activate another view if it's active now
  if (ViewerTest_myViews.Find1(theViewName) == ViewerTest::CurrentView())
  {
    if (ViewerTest_myViews.Extent() > 1)
    {
      for (ViewerTest_ViewerCommandsViewMap::Iterator anIter(ViewerTest_myViews); anIter.More();
           anIter.Next())
      {
        if (anIter.Key1() != theViewName)
        {
          ActivateView(anIter.Key2(), true);
          break;
        }
      }
    }
    else
    {
      VT_GetWindow().Nullify();
      ViewerTest::CurrentView(Handle(V3d_View)());
      if (isContextRemoved)
      {
        Handle(AIS_InteractiveContext) anEmptyContext;
        ViewerTest::SetAISContext(anEmptyContext);
      }
    }
  }

  // Delete view, name will be removed too
  const TCollection_AsciiString aCopyString(theViewName);
  ViewerTest_myViews.UnBind1(theViewName);
  if (!aView->Window().IsNull())
  {
    aView->Window()->Unmap();
  }
  aView->Remove();

#if defined(HAVE_XLIB)
  XFlush((::Display*)GetDisplayConnection()->GetDisplayAspect());
#endif

  // Keep context opened only if the closed view is last to avoid
  // unused empty contexts
  if (!aCurrentContext.IsNull())
  {
    // Check if there are more defined views in the viewer
    if ((isContextRemoved || ViewerTest_myContexts.Size() != 1)
        && aCurrentContext->CurrentViewer()->DefinedViews().IsEmpty())
    {
      // Remove driver if there is no viewers that use it
      Standard_Boolean isRemoveDriver = Standard_True;
      for (ViewerTest_ViewerCommandsInteractiveContextMap::Iterator anIter(ViewerTest_myContexts);
           anIter.More();
           anIter.Next())
      {
        if (aCurrentContext != anIter.Key2()
            && aCurrentContext->CurrentViewer()->Driver()
                 == anIter.Key2()->CurrentViewer()->Driver())
        {
          isRemoveDriver = Standard_False;
          break;
        }
      }

      aCurrentContext->RemoveAll(Standard_False);
      if (isRemoveDriver)
      {
        ViewerTest_myDrivers.UnBind2(aCurrentContext->CurrentViewer()->Driver());
#if defined(HAVE_XLIB)
        Tcl_DeleteFileHandler(XConnectionNumber((::Display*)aCurrentContext->CurrentViewer()
                                                  ->Driver()
                                                  ->GetDisplayConnection()
                                                  ->GetDisplayAspect()));
#endif
      }

      ViewerTest_myContexts.UnBind2(aCurrentContext);
    }
  }
  Message::SendInfo() << "3D View - " << aCopyString << " was deleted.\n";
  if (ViewerTest_EventManager::ToExitOnCloseView())
  {
    Draw_Interprete("exit");
  }
}

//==============================================================================
// function : VClose
// purpose  : Remove the view defined by its name
//==============================================================================

static int VClose(Draw_Interpretor& /*theDi*/, Standard_Integer theArgsNb, const char** theArgVec)
{
  NCollection_List<TCollection_AsciiString> aViewList;
  if (theArgsNb > 1)
  {
    TCollection_AsciiString anArg(theArgVec[1]);
    anArg.UpperCase();
    if (anArg.IsEqual("ALL") || anArg.IsEqual("*"))
    {
      for (ViewerTest_ViewerCommandsViewMap::Iterator anIter(ViewerTest_myViews); anIter.More();
           anIter.Next())
      {
        aViewList.Append(anIter.Key1());
      }
      if (aViewList.IsEmpty())
      {
        std::cout << "No view to close\n";
        return 0;
      }
    }
    else
    {
      ViewerTest_Names aViewName(theArgVec[1]);
      if (!ViewerTest_myViews.IsBound1(aViewName.GetViewName()))
      {
        Message::SendFail() << "Error: the view with name '" << theArgVec[1] << "' does not exist";
        return 1;
      }
      aViewList.Append(aViewName.GetViewName());
    }
  }
  else
  {
    // close active view
    if (ViewerTest::CurrentView().IsNull())
    {
      Message::SendFail("Error: no active view");
      return 1;
    }
    aViewList.Append(ViewerTest_myViews.Find2(ViewerTest::CurrentView()));
  }

  Standard_Boolean toRemoveContext = (theArgsNb != 3 || Draw::Atoi(theArgVec[2]) != 1);
  for (NCollection_List<TCollection_AsciiString>::Iterator anIter(aViewList); anIter.More();
       anIter.Next())
  {
    ViewerTest::RemoveView(anIter.Value(), toRemoveContext);
  }

  return 0;
}

//==============================================================================
// function : VActivate
// purpose  : Activate the view defined by its ID
//==============================================================================

static int VActivate(Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  if (theArgsNb == 1)
  {
    theDi.Eval("vviewlist");
    return 0;
  }

  TCollection_AsciiString aNameString;
  Standard_Boolean        toUpdate   = Standard_True;
  Standard_Boolean        toActivate = Standard_True;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (toUpdate && anArg == "-noupdate")
    {
      toUpdate = Standard_False;
    }
    else if (toActivate && aNameString.IsEmpty() && anArg == "none")
    {
      ViewerTest::CurrentView()->Window()->SetTitle(
        TCollection_AsciiString("3D View - ")
        + ViewerTest_myViews.Find2(ViewerTest::CurrentView()));
      VT_GetWindow().Nullify();
      ViewerTest::CurrentView(Handle(V3d_View)());
      ViewerTest::ResetEventManager();
      theDi << theArgVec[0] << ": all views are inactive\n";
      toActivate = Standard_False;
    }
    else if (toActivate && aNameString.IsEmpty())
    {
      aNameString = theArgVec[anArgIter];
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (!toActivate)
  {
    return 0;
  }
  else if (aNameString.IsEmpty())
  {
    Message::SendFail("Syntax error: wrong number of arguments");
    return 1;
  }

  // Check if this view exists in the viewer with the driver
  ViewerTest_Names aViewNames(aNameString);
  if (!ViewerTest_myViews.IsBound1(aViewNames.GetViewName()))
  {
    theDi << "Syntax error: wrong view name '" << aNameString << "'\n";
    return 1;
  }

  // Check if it is active already
  if (ViewerTest::CurrentView() == ViewerTest_myViews.Find1(aViewNames.GetViewName()))
  {
    theDi << theArgVec[0] << ": the view is active already\n";
    return 0;
  }

  ActivateView(aViewNames.GetViewName(), toUpdate);
  return 0;
}

//==============================================================================
// function : VViewList
// purpose  : Print current list of views per viewer and graphic driver ID
//           shared between viewers
//==============================================================================

static int VViewList(Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  if (theArgsNb > 2)
  {
    theDi << theArgVec[0] << ": Wrong number of command arguments\n"
          << "Usage: " << theArgVec[0] << " name";
    return 1;
  }
  if (ViewerTest_myContexts.Size() < 1)
    return 0;

  Standard_Boolean isTreeView = ((theArgsNb == 1) || (strcasecmp(theArgVec[1], "long") != 0));

  if (isTreeView)
  {
    theDi << theArgVec[0] << ":\n";
  }

  for (ViewerTest_ViewerCommandsGraphicDriverMap::Iterator aDriverIter(ViewerTest_myDrivers);
       aDriverIter.More();
       aDriverIter.Next())
  {
    if (isTreeView)
      theDi << aDriverIter.Key1() << ":\n";

    for (ViewerTest_ViewerCommandsInteractiveContextMap::Iterator aContextIter(
           ViewerTest_myContexts);
         aContextIter.More();
         aContextIter.Next())
    {
      if (aContextIter.Key1().Search(aDriverIter.Key1()) != -1)
      {
        if (isTreeView)
        {
          TCollection_AsciiString aContextName(aContextIter.Key1());
          theDi << " " << aContextName.Split(aDriverIter.Key1().Length() + 1) << ":\n";
        }

        for (ViewerTest_ViewerCommandsViewMap::Iterator aViewIter(ViewerTest_myViews);
             aViewIter.More();
             aViewIter.Next())
        {
          if (aViewIter.Key1().Search(aContextIter.Key1()) != -1)
          {
            TCollection_AsciiString aViewName(aViewIter.Key1());
            if (isTreeView)
            {
              if (aViewIter.Key2() == ViewerTest::CurrentView())
                theDi << "  " << aViewName.Split(aContextIter.Key1().Length() + 1) << "(*)\n";
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

//=================================================================================================

void ViewerTest::GetMousePosition(Standard_Integer& theX, Standard_Integer& theY)
{
  if (Handle(ViewerTest_EventManager) aViewCtrl = ViewerTest::CurrentEventManager())
  {
    theX = aViewCtrl->LastMousePosition().x();
    theY = aViewCtrl->LastMousePosition().y();
  }
}

//==============================================================================
// function : VViewProj
// purpose  : Switch view projection
//==============================================================================
static int VViewProj(Draw_Interpretor&, Standard_Integer theNbArgs, const char** theArgVec)
{
  static Standard_Boolean isYup = Standard_False;
  const Handle(V3d_View)& aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  TCollection_AsciiString aCmdName(theArgVec[0]);
  Standard_Boolean        isGeneralCmd = Standard_False;
  if (aCmdName == "vfront")
  {
    aView->SetProj(isYup ? V3d_TypeOfOrientation_Yup_Front : V3d_TypeOfOrientation_Zup_Front,
                   isYup);
  }
  else if (aCmdName == "vback")
  {
    aView->SetProj(isYup ? V3d_TypeOfOrientation_Yup_Back : V3d_TypeOfOrientation_Zup_Back, isYup);
  }
  else if (aCmdName == "vtop")
  {
    aView->SetProj(isYup ? V3d_TypeOfOrientation_Yup_Top : V3d_TypeOfOrientation_Zup_Top, isYup);
  }
  else if (aCmdName == "vbottom")
  {
    aView->SetProj(isYup ? V3d_TypeOfOrientation_Yup_Bottom : V3d_TypeOfOrientation_Zup_Bottom,
                   isYup);
  }
  else if (aCmdName == "vleft")
  {
    aView->SetProj(isYup ? V3d_TypeOfOrientation_Yup_Left : V3d_TypeOfOrientation_Zup_Left, isYup);
  }
  else if (aCmdName == "vright")
  {
    aView->SetProj(isYup ? V3d_TypeOfOrientation_Yup_Right : V3d_TypeOfOrientation_Zup_Right,
                   isYup);
  }
  else if (aCmdName == "vaxo")
  {
    aView->SetProj(isYup ? V3d_TypeOfOrientation_Yup_AxoRight : V3d_TypeOfOrientation_Zup_AxoRight,
                   isYup);
  }
  else
  {
    isGeneralCmd = Standard_True;
    for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
    {
      TCollection_AsciiString anArgCase(theArgVec[anArgIter]);
      anArgCase.LowerCase();
      if (anArgCase == "-zup")
      {
        isYup = Standard_False;
      }
      else if (anArgCase == "-yup")
      {
        isYup = Standard_True;
      }
      else if (anArgCase == "-front" || anArgCase == "front" || anArgCase == "-f"
               || anArgCase == "f")
      {
        aView->SetProj(isYup ? V3d_TypeOfOrientation_Yup_Front : V3d_TypeOfOrientation_Zup_Front,
                       isYup);
      }
      else if (anArgCase == "-back" || anArgCase == "back" || anArgCase == "-b" || anArgCase == "b")
      {
        aView->SetProj(isYup ? V3d_TypeOfOrientation_Yup_Back : V3d_TypeOfOrientation_Zup_Back,
                       isYup);
      }
      else if (anArgCase == "-top" || anArgCase == "top" || anArgCase == "-t" || anArgCase == "t")
      {
        aView->SetProj(isYup ? V3d_TypeOfOrientation_Yup_Top : V3d_TypeOfOrientation_Zup_Top,
                       isYup);
      }
      else if (anArgCase == "-bottom" || anArgCase == "bottom" || anArgCase == "-bot"
               || anArgCase == "bot" || anArgCase == "-b" || anArgCase == "b")
      {
        aView->SetProj(isYup ? V3d_TypeOfOrientation_Yup_Bottom : V3d_TypeOfOrientation_Zup_Bottom,
                       isYup);
      }
      else if (anArgCase == "-left" || anArgCase == "left" || anArgCase == "-l" || anArgCase == "l")
      {
        aView->SetProj(isYup ? V3d_TypeOfOrientation_Yup_Left : V3d_TypeOfOrientation_Zup_Left,
                       isYup);
      }
      else if (anArgCase == "-right" || anArgCase == "right" || anArgCase == "-r"
               || anArgCase == "r")
      {
        aView->SetProj(isYup ? V3d_TypeOfOrientation_Yup_Right : V3d_TypeOfOrientation_Zup_Right,
                       isYup);
      }
      else if (anArgCase == "-axoleft" || anArgCase == "-leftaxo" || anArgCase == "axoleft"
               || anArgCase == "leftaxo")
      {
        aView->SetProj(isYup ? V3d_TypeOfOrientation_Yup_AxoLeft
                             : V3d_TypeOfOrientation_Zup_AxoLeft,
                       isYup);
      }
      else if (anArgCase == "-axo" || anArgCase == "axo" || anArgCase == "-a" || anArgCase == "a"
               || anArgCase == "-axoright" || anArgCase == "-rightaxo" || anArgCase == "axoright"
               || anArgCase == "rightaxo")
      {
        aView->SetProj(isYup ? V3d_TypeOfOrientation_Yup_AxoRight
                             : V3d_TypeOfOrientation_Zup_AxoRight,
                       isYup);
      }
      else if (anArgCase == "+x")
      {
        aView->SetProj(V3d_Xpos, isYup);
      }
      else if (anArgCase == "-x")
      {
        aView->SetProj(V3d_Xneg, isYup);
      }
      else if (anArgCase == "+y")
      {
        aView->SetProj(V3d_Ypos, isYup);
      }
      else if (anArgCase == "-y")
      {
        aView->SetProj(V3d_Yneg, isYup);
      }
      else if (anArgCase == "+z")
      {
        aView->SetProj(V3d_Zpos, isYup);
      }
      else if (anArgCase == "-z")
      {
        aView->SetProj(V3d_Zneg, isYup);
      }
      else if (anArgCase == "+x+y+z")
      {
        aView->SetProj(V3d_XposYposZpos, isYup);
      }
      else if (anArgCase == "+x+y-z")
      {
        aView->SetProj(V3d_XposYposZneg, isYup);
      }
      else if (anArgCase == "+x-y+z")
      {
        aView->SetProj(V3d_XposYnegZpos, isYup);
      }
      else if (anArgCase == "+x-y-z")
      {
        aView->SetProj(V3d_XposYnegZneg, isYup);
      }
      else if (anArgCase == "-x+y+z")
      {
        aView->SetProj(V3d_XnegYposZpos, isYup);
      }
      else if (anArgCase == "-x+y-z")
      {
        aView->SetProj(V3d_XnegYposZneg, isYup);
      }
      else if (anArgCase == "-x-y+z")
      {
        aView->SetProj(V3d_XnegYnegZpos, isYup);
      }
      else if (anArgCase == "-x-y-z")
      {
        aView->SetProj(V3d_XnegYnegZneg, isYup);
      }
      else if (anArgCase == "+x+y")
      {
        aView->SetProj(V3d_XposYpos, isYup);
      }
      else if (anArgCase == "+x-y")
      {
        aView->SetProj(V3d_XposYneg, isYup);
      }
      else if (anArgCase == "-x+y")
      {
        aView->SetProj(V3d_XnegYpos, isYup);
      }
      else if (anArgCase == "-x-y")
      {
        aView->SetProj(V3d_XnegYneg, isYup);
      }
      else if (anArgCase == "+x+z")
      {
        aView->SetProj(V3d_XposZpos, isYup);
      }
      else if (anArgCase == "+x-z")
      {
        aView->SetProj(V3d_XposZneg, isYup);
      }
      else if (anArgCase == "-x+z")
      {
        aView->SetProj(V3d_XnegZpos, isYup);
      }
      else if (anArgCase == "-x-z")
      {
        aView->SetProj(V3d_XnegZneg, isYup);
      }
      else if (anArgCase == "+y+z")
      {
        aView->SetProj(V3d_YposZpos, isYup);
      }
      else if (anArgCase == "+y-z")
      {
        aView->SetProj(V3d_YposZneg, isYup);
      }
      else if (anArgCase == "-y+z")
      {
        aView->SetProj(V3d_YnegZpos, isYup);
      }
      else if (anArgCase == "-y-z")
      {
        aView->SetProj(V3d_YnegZneg, isYup);
      }
      else if (anArgIter + 1 < theNbArgs && anArgCase == "-frame"
               && TCollection_AsciiString(theArgVec[anArgIter + 1]).Length() == 4)
      {
        TCollection_AsciiString aFrameDef(theArgVec[++anArgIter]);
        aFrameDef.LowerCase();
        gp_Dir aRight, anUp;
        if (aFrameDef.Value(2) == aFrameDef.Value(4))
        {
          Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
          return 1;
        }

        if (aFrameDef.Value(2) == 'x')
        {
          aRight = aFrameDef.Value(1) == '+' ? gp::DX() : -gp::DX();
        }
        else if (aFrameDef.Value(2) == 'y')
        {
          aRight = aFrameDef.Value(1) == '+' ? gp::DY() : -gp::DY();
        }
        else if (aFrameDef.Value(2) == 'z')
        {
          aRight = aFrameDef.Value(1) == '+' ? gp::DZ() : -gp::DZ();
        }
        else
        {
          Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
          return 1;
        }

        if (aFrameDef.Value(4) == 'x')
        {
          anUp = aFrameDef.Value(3) == '+' ? gp::DX() : -gp::DX();
        }
        else if (aFrameDef.Value(4) == 'y')
        {
          anUp = aFrameDef.Value(3) == '+' ? gp::DY() : -gp::DY();
        }
        else if (aFrameDef.Value(4) == 'z')
        {
          anUp = aFrameDef.Value(3) == '+' ? gp::DZ() : -gp::DZ();
        }
        else
        {
          Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
          return 1;
        }

        const Handle(Graphic3d_Camera)& aCamera     = aView->Camera();
        const gp_Pnt                    anOriginVCS = aCamera->ConvertWorld2View(gp::Origin());
        const gp_Dir                    aDir        = anUp.Crossed(aRight);
        aCamera->SetCenter(gp_Pnt(0, 0, 0));
        aCamera->SetDirection(aDir);
        aCamera->SetUp(anUp);
        aCamera->OrthogonalizeUp();

        aView->Panning(anOriginVCS.X(), anOriginVCS.Y());
        aView->Update();
      }
      else
      {
        Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }
    }
  }

  if (!isGeneralCmd && theNbArgs != 1)
  {
    Message::SendFail("Syntax error: wrong number of arguments");
    return 1;
  }
  return 0;
}

//==============================================================================
// function : VHelp
// purpose  : Dsiplay help on viewer Keyboead and mouse commands
// Draw arg : No args
//==============================================================================

static int VHelp(Draw_Interpretor& di, Standard_Integer, const char**)
{
  di << "=========================\n";
  di << "F : FitAll\n";
  di << "T : TopView\n";
  di << "B : BottomView\n";
  di << "R : RightView\n";
  di << "L : LeftView\n";
  di << "Backspace : AxonometricView\n";

  di << "=========================\n";
  di << "W, S : Fly   forward/backward\n";
  di << "A, D : Slide left/right\n";
  di << "Q, E : Bank  left/right\n";
  di << "-, + : Change flying speed\n";
  di << "Arrows : look left/right/up/down\n";
  di << "Arrows+Shift : slide left/right/up/down\n";

  di << "=========================\n";
  di << "S + Ctrl : Shading\n";
  di << "W + Ctrl : Wireframe\n";
  di << "H : HiddenLineRemoval\n";
  di << "U : Unset display mode\n";
  di << "Delete : Remove selection from viewer\n";

  di << "=========================\n";
  di << "Selection mode \n";
  di << "0 : Shape\n";
  di << "1 : Vertex\n";
  di << "2 : Edge\n";
  di << "3 : Wire\n";
  di << "4 : Face\n";
  di << "5 : Shell\n";
  di << "6 : Solid\n";
  di << "7 : Compound\n";

  di << "=========================\n";
  di << "< : Hilight next detected\n";
  di << "> : Hilight previous detected\n";

  return 0;
}

#ifdef _WIN32

static LRESULT WINAPI AdvViewerWindowProc(HWND   theWinHandle,
                                          UINT   theMsg,
                                          WPARAM wParam,
                                          LPARAM lParam)
{
  if (ViewerTest_myViews.IsEmpty())
  {
    return DefWindowProcW(theWinHandle, theMsg, wParam, lParam);
  }

  switch (theMsg)
  {
    case WM_CLOSE: {
      // Delete view from map of views
      ViewerTest::RemoveView(FindViewIdByWindowHandle(theWinHandle));
      return 0;
    }
    case WM_ACTIVATE: {
      if (LOWORD(wParam) == WA_CLICKACTIVE || LOWORD(wParam) == WA_ACTIVE
          || ViewerTest::CurrentView().IsNull())
      {
        // Activate inactive window
        if (VT_GetWindow().IsNull() || (HWND)VT_GetWindow()->HWindow() != theWinHandle)
        {
          ActivateView(FindViewIdByWindowHandle(theWinHandle));
        }
      }
      return 0;
    }
    default: {
      const Handle(V3d_View)& aView = ViewerTest::CurrentView();
      if (!aView.IsNull() && !VT_GetWindow().IsNull())
      {
        MSG aMsg     = {};
        aMsg.hwnd    = theWinHandle;
        aMsg.message = theMsg;
        aMsg.wParam  = wParam;
        aMsg.lParam  = lParam;
        if (VT_GetWindow()->ProcessMessage(*ViewerTest::CurrentEventManager(), aMsg))
        {
          return 0;
        }
      }
    }
  }
  return DefWindowProcW(theWinHandle, theMsg, wParam, lParam);
}

//==============================================================================
// function : ViewerMainLoop
// purpose  : Get a Event on the view and dispatch it
//==============================================================================

int ViewerMainLoop(Standard_Integer theNbArgs, const char** theArgVec)
{
  Handle(ViewerTest_EventManager) aViewCtrl = ViewerTest::CurrentEventManager();
  if (aViewCtrl.IsNull() || theNbArgs < 4)
  {
    return 0;
  }

  aViewCtrl->StartPickPoint(theArgVec[1], theArgVec[2], theArgVec[3]);

  std::cout << "Start picking\n";

  MSG aMsg;
  aMsg.wParam = 1;
  while (aViewCtrl->ToPickPoint())
  {
    // Wait for a VT_ProcessButton1Press() to toggle pick to 1 or 0
    if (GetMessageW(&aMsg, NULL, 0, 0))
    {
      TranslateMessage(&aMsg);
      DispatchMessageW(&aMsg);
    }
  }

  std::cout << "Picking done\n";
  return 0;
}

#elif defined(HAVE_XLIB)

int ViewerMainLoop(Standard_Integer theNbArgs, const char** theArgVec)
{
  static XEvent          aReport;
  const Standard_Boolean toPick = theNbArgs > 0;
  if (theNbArgs > 0)
  {
    if (ViewerTest::CurrentEventManager().IsNull())
    {
      return 0;
    }
    ViewerTest::CurrentEventManager()->StartPickPoint(theArgVec[1], theArgVec[2], theArgVec[3]);
  }

  Display* aDisplay = (Display*)GetDisplayConnection()->GetDisplayAspect();
  XNextEvent(aDisplay, &aReport);

  // Handle event for the chosen display connection
  switch (aReport.type)
  {
    case ClientMessage: {
      if ((Atom)aReport.xclient.data.l[0]
          == GetDisplayConnection()->GetAtom(Aspect_XA_DELETE_WINDOW))
      {
        // Close the window
        ViewerTest::RemoveView(FindViewIdByWindowHandle(aReport.xclient.window));
        return toPick ? 0 : 1;
      }
      break;
    }
    case FocusIn: {
      // Activate inactive view
      Window aWindow = !VT_GetWindow().IsNull() ? VT_GetWindow()->XWindow() : 0;
      if (aWindow != aReport.xfocus.window)
      {
        ActivateView(FindViewIdByWindowHandle(aReport.xfocus.window));
      }
      break;
    }
    default: {
      const Handle(V3d_View)& aView = ViewerTest::CurrentView();
      if (!aView.IsNull() && !VT_GetWindow().IsNull())
      {
        VT_GetWindow()->ProcessMessage(*ViewerTest::CurrentEventManager(), aReport);
      }
      break;
    }
  }
  return (!toPick || ViewerTest::CurrentEventManager()->ToPickPoint()) ? 1 : 0;
}

//==============================================================================
// function : VProcessEvents
// purpose  : manage the event in the Viewer window (see Tcl_CreateFileHandler())
//==============================================================================
static void VProcessEvents(ClientData theDispX, int)
{
  Display*                         aDispX = (Display*)theDispX;
  Handle(Aspect_DisplayConnection) aDispConn;
  for (ViewerTest_ViewerCommandsGraphicDriverMap::Iterator aDriverIter(ViewerTest_myDrivers);
       aDriverIter.More();
       aDriverIter.Next())
  {
    const Handle(Aspect_DisplayConnection)& aDispConnTmp =
      aDriverIter.Key2()->GetDisplayConnection();
    if ((Display*)aDispConnTmp->GetDisplayAspect() == aDispX)
    {
      aDispConn = aDispConnTmp;
      break;
    }
  }
  if (aDispConn.IsNull())
  {
    Message::SendFail("Error: ViewerTest is unable processing messages for unknown X Display");
    return;
  }

  // process new events in queue
  SetDisplayConnection(aDispConn);
  int aNbRemain = 0;
  for (int aNbEventsMax = XPending(aDispX), anEventIter(0);;)
  {
    const int anEventResult = ViewerMainLoop(0, NULL);
    if (anEventResult == 0)
    {
      return;
    }

    aNbRemain = XPending(aDispX);
    if (++anEventIter >= aNbEventsMax || aNbRemain <= 0)
    {
      break;
    }
  }

  // Listening X events through Tcl_CreateFileHandler() callback is fragile,
  // it is possible that new events will arrive to queue before the end of this callback
  // so that either this callback should go into an infinite loop (blocking processing of other
  // events) or to keep unprocessed events till the next queue update (which can arrive not soon).
  // Sending a dummy event in this case is a simple workaround (still, it is possible that new event
  // will be queued in-between).
  if (aNbRemain != 0)
  {
    XEvent aDummyEvent;
    memset(&aDummyEvent, 0, sizeof(aDummyEvent));
    aDummyEvent.type           = ClientMessage;
    aDummyEvent.xclient.format = 32;
    XSendEvent(aDispX, InputFocus, False, 0, &aDummyEvent);
    XFlush(aDispX);
  }

  if (const Handle(AIS_InteractiveContext)& anActiveCtx = ViewerTest::GetAISContext())
  {
    SetDisplayConnection(anActiveCtx->CurrentViewer()->Driver()->GetDisplayConnection());
  }
}
#elif !defined(__APPLE__)
// =======================================================================
// function : ViewerMainLoop
// purpose  :
// =======================================================================
int ViewerMainLoop(Standard_Integer, const char**)
{
  // unused
  return 0;
}
#endif

//=================================================================================================

static int VFit(Draw_Interpretor& /*theDi*/, Standard_Integer theArgNb, const char** theArgv)
{
  const Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Standard_Boolean       toFit = Standard_True;
  ViewerTest_AutoUpdater anUpdateTool(Handle(AIS_InteractiveContext)(), aView);
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgv[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode(anArg))
    {
      continue;
    }
    else if (anArg == "-selected")
    {
      ViewerTest::GetAISContext()->FitSelected(aView, 0.01, Standard_False);
      toFit = Standard_False;
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
    }
  }

  if (toFit)
  {
    aView->FitAll(0.01, Standard_False);
  }
  return 0;
}

//=======================================================================
// function : VFitArea
// purpose  : Fit view to show area located between two points
//         : given in world 2D or 3D coordinates.
//=======================================================================
static int VFitArea(Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: No active viewer");
    return 1;
  }

  // Parse arguments.
  gp_Pnt aWorldPnt1(0.0, 0.0, 0.0);
  gp_Pnt aWorldPnt2(0.0, 0.0, 0.0);

  if (theArgNb == 5)
  {
    aWorldPnt1.SetX(Draw::Atof(theArgVec[1]));
    aWorldPnt1.SetY(Draw::Atof(theArgVec[2]));
    aWorldPnt2.SetX(Draw::Atof(theArgVec[3]));
    aWorldPnt2.SetY(Draw::Atof(theArgVec[4]));
  }
  else if (theArgNb == 7)
  {
    aWorldPnt1.SetX(Draw::Atof(theArgVec[1]));
    aWorldPnt1.SetY(Draw::Atof(theArgVec[2]));
    aWorldPnt1.SetZ(Draw::Atof(theArgVec[3]));
    aWorldPnt2.SetX(Draw::Atof(theArgVec[4]));
    aWorldPnt2.SetY(Draw::Atof(theArgVec[5]));
    aWorldPnt2.SetZ(Draw::Atof(theArgVec[6]));
  }
  else
  {
    Message::SendFail("Syntax error: Invalid number of arguments");
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }

  // Convert model coordinates to view space
  Handle(Graphic3d_Camera) aCamera   = aView->Camera();
  gp_Pnt                   aViewPnt1 = aCamera->ConvertWorld2View(aWorldPnt1);
  gp_Pnt                   aViewPnt2 = aCamera->ConvertWorld2View(aWorldPnt2);

  // Determine fit area
  gp_Pnt2d aMinCorner(Min(aViewPnt1.X(), aViewPnt2.X()), Min(aViewPnt1.Y(), aViewPnt2.Y()));
  gp_Pnt2d aMaxCorner(Max(aViewPnt1.X(), aViewPnt2.X()), Max(aViewPnt1.Y(), aViewPnt2.Y()));

  Standard_Real aDiagonal = aMinCorner.Distance(aMaxCorner);

  if (aDiagonal < Precision::Confusion())
  {
    Message::SendFail("Error: view area is too small");
    return 1;
  }

  aView->FitAll(aMinCorner.X(), aMinCorner.Y(), aMaxCorner.X(), aMaxCorner.Y());
  return 0;
}

//==============================================================================
// function : VZFit
// purpose  : ZFitall, no DRAW arguments
// Draw arg : No args
//==============================================================================
static int VZFit(Draw_Interpretor& /*theDi*/, Standard_Integer theArgsNb, const char** theArgVec)
{
  const Handle(V3d_View)& aCurrentView = ViewerTest::CurrentView();

  if (aCurrentView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
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
    aScale = Draw::Atoi(theArgVec[1]);
  }

  aCurrentView->ZFitAll(aScale);
  aCurrentView->Redraw();

  return 0;
}

//=================================================================================================

static int VRepaint(Draw_Interpretor&, Standard_Integer theArgNb, const char** theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Standard_Boolean isImmediateUpdate = Standard_False;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-immediate" || anArg == "-imm")
    {
      isImmediateUpdate = Standard_True;
      if (anArgIter + 1 < theArgNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], isImmediateUpdate))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-continuous" || anArg == "-cont" || anArg == "-fps" || anArg == "-framerate")
    {
      Standard_Real aFps = -1.0;
      if (anArgIter + 1 < theArgNb
          && TCollection_AsciiString(theArgVec[anArgIter + 1]).IsRealValue(Standard_True))
      {
        aFps = Draw::Atof(theArgVec[++anArgIter]);
      }

      ViewerTest_ContinuousRedrawer& aRedrawer = ViewerTest_ContinuousRedrawer::Instance();
      ViewerTest::CurrentEventManager()->SetContinuousRedraw(false);
      if (aFps >= 1.0)
      {
        aRedrawer.Start(aView, aFps);
      }
      else if (aFps < 0.0)
      {
        if (ViewerTest::GetViewerFromContext()->ActiveViews().Extent() == 1)
        {
          aRedrawer.Stop();
          ViewerTest::CurrentEventManager()->SetContinuousRedraw(true);
          ViewerTest::CurrentEventManager()->FlushViewEvents(ViewerTest::GetAISContext(),
                                                             ViewerTest::CurrentView(),
                                                             true);
          continue;
        }
        aRedrawer.Start(aView, aFps);
      }
      else
      {
        aRedrawer.Stop();
      }
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  if (isImmediateUpdate)
  {
    aView->RedrawImmediate();
  }
  else
  {
    aView->Redraw();
  }
  return 0;
}

//==============================================================================
// function : VClear
// purpose  : Remove all the object from the viewer
// Draw arg : No args
//==============================================================================

static int VClear(Draw_Interpretor&, Standard_Integer, const char**)
{
  Handle(V3d_View) V = ViewerTest::CurrentView();
  if (!V.IsNull())
    ViewerTest::Clear();
  return 0;
}

//=================================================================================================

static int VPick(Draw_Interpretor&, Standard_Integer theNbArgs, const char** theArgVec)
{
  if (ViewerTest::CurrentView().IsNull())
  {
    return 1;
  }

  if (theNbArgs < 4)
  {
    Message::SendFail("Syntax error: wrong number of arguments");
    return 1;
  }

  while (ViewerMainLoop(theNbArgs, theArgVec))
  {
    //
  }

  return 0;
}

//! Parse image fill method.
static bool parseImageMode(const TCollection_AsciiString& theName, Aspect_FillMethod& theMode)
{
  TCollection_AsciiString aName = theName;
  aName.LowerCase();
  if (aName == "none")
  {
    theMode = Aspect_FM_NONE;
  }
  else if (aName == "centered")
  {
    theMode = Aspect_FM_CENTERED;
  }
  else if (aName == "tiled")
  {
    theMode = Aspect_FM_TILED;
  }
  else if (aName == "stretch")
  {
    theMode = Aspect_FM_STRETCH;
  }
  else
  {
    return false;
  }
  return true;
}

//! Parse gradient fill method.
static bool parseGradientMode(const TCollection_AsciiString& theName,
                              Aspect_GradientFillMethod&     theMode)
{
  TCollection_AsciiString aName = theName;
  aName.LowerCase();
  if (aName == "none")
  {
    theMode = Aspect_GradientFillMethod_None;
  }
  else if (aName == "hor" || aName == "horizontal")
  {
    theMode = Aspect_GradientFillMethod_Horizontal;
  }
  else if (aName == "ver" || aName == "vert" || aName == "vertical")
  {
    theMode = Aspect_GradientFillMethod_Vertical;
  }
  else if (aName == "diag" || aName == "diagonal" || aName == "diag1" || aName == "diagonal1")
  {
    theMode = Aspect_GradientFillMethod_Diagonal1;
  }
  else if (aName == "diag2" || aName == "diagonal2")
  {
    theMode = Aspect_GradientFillMethod_Diagonal2;
  }
  else if (aName == "corner1")
  {
    theMode = Aspect_GradientFillMethod_Corner1;
  }
  else if (aName == "corner2")
  {
    theMode = Aspect_GradientFillMethod_Corner2;
  }
  else if (aName == "corner3")
  {
    theMode = Aspect_GradientFillMethod_Corner3;
  }
  else if (aName == "corner4")
  {
    theMode = Aspect_GradientFillMethod_Corner4;
  }
  else if (aName == "ellip" || aName == "elliptical")
  {
    theMode = Aspect_GradientFillMethod_Elliptical;
  }
  else
  {
    return false;
  }
  return true;
}

//=================================================================================================

static int VBackground(Draw_Interpretor& theDI, Standard_Integer theNbArgs, const char** theArgVec)
{
  if (theNbArgs < 2)
  {
    theDI << "Syntax error: wrong number of arguments";
    return 1;
  }

  const TCollection_AsciiString aCmdName(theArgVec[0]);
  bool                          isDefault = aCmdName == "vsetdefaultbg";
  Standard_Integer              aNbColors = 0;
  Quantity_ColorRGBA            aColors[2];

  Aspect_GradientFillMethod aGradientMode   = Aspect_GradientFillMethod_None;
  bool                      hasGradientMode = false;

  TCollection_AsciiString anImagePath;
  Aspect_FillMethod       anImageMode  = Aspect_FM_CENTERED;
  bool                    hasImageMode = false;

  bool                     isSkydomeBg = false;
  Aspect_SkydomeBackground aSkydomeAspect;

  NCollection_Sequence<TCollection_AsciiString> aCubeMapSeq;
  Graphic3d_CubeMapOrder                        aCubeOrder      = Graphic3d_CubeMapOrder::Default();
  bool                                          isCubeZInverted = false;
  bool                                          isSRgb          = true;

  int toUseIBL = 1;

  Handle(V3d_View)       aView = ViewerTest::CurrentView();
  ViewerTest_AutoUpdater anUpdateTool(ViewerTest::GetAISContext(), aView);
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode(anArg))
    {
      continue;
    }
    else if (anArg == "-default" || anArg == "-def")
    {
      isDefault = true;
    }
    else if (anArgIter + 1 < theNbArgs
             && (anArg == "-imagefile" || anArg == "-imgfile" || anArg == "-image"
                 || anArg == "-img"))
    {
      anImagePath = theArgVec[++anArgIter];
    }
    else if (anArg == "-skydome" || anArg == "-sky")
    {
      isSkydomeBg = true;
    }
    else if (anArgIter + 3 < theNbArgs && isSkydomeBg && anArg == "-sundir")
    {
      float aX = (float)Draw::Atof(theArgVec[++anArgIter]);
      float aY = (float)Draw::Atof(theArgVec[++anArgIter]);
      float aZ = (float)Draw::Atof(theArgVec[++anArgIter]);
      aSkydomeAspect.SetSunDirection(gp_Dir(aX, aY, aZ));
    }
    else if (anArgIter + 1 < theNbArgs && isSkydomeBg && anArg == "-cloud")
    {
      float aCloudy = (float)Draw::Atof(theArgVec[++anArgIter]);
      aSkydomeAspect.SetCloudiness(aCloudy);
    }
    else if (anArgIter + 1 < theNbArgs && isSkydomeBg && anArg == "-time")
    {
      float aTime = (float)Draw::Atof(theArgVec[++anArgIter]);
      aSkydomeAspect.SetTimeParameter(aTime);
    }
    else if (anArgIter + 1 < theNbArgs && isSkydomeBg && anArg == "-fog")
    {
      float aFoggy = (float)Draw::Atof(theArgVec[++anArgIter]);
      aSkydomeAspect.SetFogginess(aFoggy);
    }
    else if (anArgIter + 1 < theNbArgs && isSkydomeBg && anArg == "-size")
    {
      Standard_Integer aSize = Draw::Atoi(theArgVec[++anArgIter]);
      aSkydomeAspect.SetSize(aSize);
    }
    else if (anArgIter + 1 < theNbArgs && aCubeMapSeq.IsEmpty()
             && (anArg == "-cubemap" || anArg == "-cmap" || anArg == "-cm"))
    {
      aCubeMapSeq.Append(theArgVec[++anArgIter]);
      for (Standard_Integer aCubeSideIter = 1; anArgIter + aCubeSideIter < theNbArgs;
           ++aCubeSideIter)
      {
        TCollection_AsciiString aSideArg(theArgVec[anArgIter + aCubeSideIter]);
        if (!aSideArg.IsEmpty() && aSideArg.Value(1) == '-')
        {
          break;
        }

        aCubeMapSeq.Append(aSideArg);
        if (aCubeMapSeq.Size() == 6)
        {
          anArgIter += 5;
          break;
        }
      }

      if (aCubeMapSeq.Size() > 1 && aCubeMapSeq.Size() < 6)
      {
        aCubeMapSeq.Remove(2, aCubeMapSeq.Size());
      }
    }
    else if (anArgIter + 6 < theNbArgs && anArg == "-order")
    {
      for (Standard_Integer aCubeSideIter = 0; aCubeSideIter < 6; ++aCubeSideIter)
      {
        Standard_Integer aSideArg = 0;
        if (!Draw::ParseInteger(theArgVec[anArgIter + aCubeSideIter + 1], aSideArg) || aSideArg < 0
            || aSideArg > 5)
        {
          theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
          return 1;
        }
        aCubeOrder.Set((Graphic3d_CubeMapSide)aCubeSideIter, (unsigned char)aSideArg);
      }
      if (!aCubeOrder.IsValid())
      {
        theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }
      anArgIter += 6;
    }
    else if (anArg == "-invertedz" || anArg == "-noinvertedz" || anArg == "-invz"
             || anArg == "-noinvz")
    {
      isCubeZInverted = Draw::ParseOnOffNoIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (anArg == "-pbrenv" || anArg == "-nopbrenv" || anArg == "-ibl" || anArg == "-noibl")
    {
      toUseIBL = !anArg.StartsWith("-no") ? 1 : 0;
      if (anArgIter + 1 < theNbArgs)
      {
        TCollection_AsciiString anIblArg(theArgVec[anArgIter + 1]);
        anIblArg.LowerCase();
        if (anIblArg == "keep" || anIblArg == "-1")
        {
          toUseIBL = -1;
          ++anArgIter;
        }
        else if (anIblArg == "ibl" || anIblArg == "1" || anIblArg == "on")
        {
          toUseIBL = !anArg.StartsWith("-no") ? 1 : 0;
          ++anArgIter;
        }
        else if (anIblArg == "noibl" || anIblArg == "0" || anIblArg == "off")
        {
          toUseIBL = !anArg.StartsWith("-no") ? 0 : 1;
          ++anArgIter;
        }
      }
    }
    else if (anArg == "-srgb" || anArg == "-nosrgb")
    {
      isSRgb = Draw::ParseOnOffNoIterator(theNbArgs, theArgVec, anArgIter);
    }
    else if (aNbColors < 2 && (anArg == "-color" || anArg == "-col"))
    {
      Standard_Integer aNbParsed = Draw::ParseColor(theNbArgs - (anArgIter + 1),
                                                    theArgVec + (anArgIter + 1),
                                                    aColors[aNbColors].ChangeRGB());
      if (aNbParsed == 0)
      {
        theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }
      anArgIter += aNbParsed;
      ++aNbColors;
    }
    else if (anArgIter + 1 < theNbArgs
             && (anArg == "-gradientmode" || anArg == "-gradmode" || anArg == "-gradmd"
                 || anArg == "-grmode" || anArg == "-grmd")
             && parseGradientMode(theArgVec[anArgIter + 1], aGradientMode))
    {
      ++anArgIter;
      hasGradientMode = true;
    }
    else if (anArgIter + 1 < theNbArgs
             && (anArg == "-imagemode" || anArg == "-imgmode" || anArg == "-imagemd"
                 || anArg == "-imgmd")
             && parseImageMode(theArgVec[anArgIter + 1], anImageMode))
    {
      ++anArgIter;
      hasImageMode = true;
    }
    else if (aNbColors == 0 && anArgIter + 2 < theNbArgs
             && (anArg == "-gradient" || anArg == "-grad" || anArg == "-gr"))
    {
      Standard_Integer aNbParsed1 = Draw::ParseColor(theNbArgs - (anArgIter + 1),
                                                     theArgVec + (anArgIter + 1),
                                                     aColors[aNbColors].ChangeRGB());
      anArgIter += aNbParsed1;
      ++aNbColors;
      if (aNbParsed1 == 0)
      {
        theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }
      Standard_Integer aNbParsed2 = Draw::ParseColor(theNbArgs - (anArgIter + 1),
                                                     theArgVec + (anArgIter + 1),
                                                     aColors[aNbColors].ChangeRGB());
      anArgIter += aNbParsed2;
      ++aNbColors;
      if (aNbParsed2 == 0)
      {
        theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }
    }
    else if (parseGradientMode(theArgVec[anArgIter], aGradientMode))
    {
      hasGradientMode = true;
    }
    else if (aNbColors < 2
             && (Quantity_ColorRGBA::ColorFromName(theArgVec[anArgIter], aColors[aNbColors])
                 || Quantity_ColorRGBA::ColorFromHex(theArgVec[anArgIter], aColors[aNbColors])))
    {
      ++aNbColors;
    }
    else if (anImagePath.IsEmpty() && aNbColors == 0 && !hasGradientMode && aCubeMapSeq.IsEmpty())
    {
      anImagePath = theArgVec[anArgIter];
    }
    else
    {
      theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (!isDefault && aView.IsNull())
  {
    theDI << "Error: no active viewer";
    return 1;
  }
  else if (isDefault && aNbColors == 0 && !hasGradientMode)
  {
    theDI << "Syntax error at '-default'";
    return 1;
  }

  if (aNbColors == 1)
  {
    if (isDefault)
    {
      ViewerTest_DefaultBackground.GradientColor1 = Quantity_Color();
      ViewerTest_DefaultBackground.GradientColor2 = Quantity_Color();
      ViewerTest_DefaultBackground.FillMethod     = Aspect_GradientFillMethod_None;
      ViewerTest_DefaultBackground.FlatColor      = aColors[0].GetRGB();
      ViewerTest_DefaultBackground.SetDefaultGradient();
      ViewerTest_DefaultBackground.SetDefaultColor();
    }
    else
    {
      aView->SetBgGradientStyle(hasGradientMode ? aGradientMode : Aspect_GradientFillMethod_None);
      aView->SetBackgroundColor(aColors[0].GetRGB());
      if (toUseIBL != -1)
      {
        aView->SetBackgroundCubeMap(Handle(Graphic3d_CubeMap)(), true);
      }
    }
  }
  else if (aNbColors == 2)
  {
    if (isDefault)
    {
      ViewerTest_DefaultBackground.GradientColor1 = aColors[0].GetRGB();
      ViewerTest_DefaultBackground.GradientColor2 = aColors[1].GetRGB();
      if (hasGradientMode)
      {
        ViewerTest_DefaultBackground.FillMethod = aGradientMode;
      }
      else if (ViewerTest_DefaultBackground.FillMethod == Aspect_GradientFillMethod_None)
      {
        ViewerTest_DefaultBackground.FillMethod = Aspect_GradientFillMethod_Vertical;
      }
      ViewerTest_DefaultBackground.SetDefaultGradient();
    }
    else
    {
      if (!hasGradientMode)
      {
        aGradientMode = aView->GradientBackground().BgGradientFillMethod();
        if (aGradientMode == Aspect_GradientFillMethod_None)
        {
          aGradientMode = Aspect_GradientFillMethod_Vertical;
        }
      }
      aView->SetBgGradientColors(aColors[0].GetRGB(), aColors[1].GetRGB(), aGradientMode);
      if (toUseIBL != -1)
      {
        aView->SetBackgroundCubeMap(Handle(Graphic3d_CubeMap)(), true);
      }
    }
  }
  else if (hasGradientMode)
  {
    if (isDefault)
    {
      ViewerTest_DefaultBackground.FillMethod = aGradientMode;
      ViewerTest_DefaultBackground.SetDefaultGradient();
    }
    else
    {
      aView->SetBgGradientStyle(aGradientMode);
    }
  }

  if (!anImagePath.IsEmpty())
  {
    Handle(Graphic3d_Texture2D) aTextureMap = new Graphic3d_Texture2D(anImagePath);
    aTextureMap->DisableModulate();
    aTextureMap->SetColorMap(isSRgb);
    if (!aTextureMap->IsDone())
    {
      theDI << "Syntax error at '" << anImagePath << "'";
      return 1;
    }
    aView->SetBackgroundImage(aTextureMap, anImageMode);
  }
  else if (hasImageMode)
  {
    aView->SetBgImageStyle(anImageMode);
  }

  if (isSkydomeBg)
  {
    aView->SetBackgroundSkydome(aSkydomeAspect, toUseIBL != -1);
  }

  if (!aCubeMapSeq.IsEmpty())
  {
    Handle(Graphic3d_CubeMap) aCubeMap;
    if (aCubeMapSeq.Size() == 1)
    {
      aCubeMap = new Graphic3d_CubeMapPacked(aCubeMapSeq.First(), aCubeOrder.Validated());
    }
    else
    {
      NCollection_Array1<TCollection_AsciiString> aCubeMapArr(0, 5);
      Standard_Integer                            aCubeSide = 0;
      for (NCollection_Sequence<TCollection_AsciiString>::Iterator aFileIter(aCubeMapSeq);
           aFileIter.More();
           aFileIter.Next(), ++aCubeSide)
      {
        aCubeMapArr[aCubeSide] = aFileIter.Value();
      }
      aCubeMap = new Graphic3d_CubeMapSeparate(aCubeMapArr);
    }

    aCubeMap->SetZInversion(isCubeZInverted);
    aCubeMap->SetColorMap(isSRgb);

    aCubeMap->GetParams()->SetFilter(Graphic3d_TOTF_BILINEAR);
    aCubeMap->GetParams()->SetRepeat(false);
    aCubeMap->GetParams()->SetTextureUnit(Graphic3d_TextureUnit_EnvMap);

    aView->SetBackgroundCubeMap(aCubeMap, toUseIBL != -1);
  }
  if (toUseIBL != -1 && !aView.IsNull())
  {
    aView->SetImageBasedLighting(toUseIBL == 1);
  }

  return 0;
}

//=================================================================================================

static int VScale(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  if (V3dView.IsNull())
    return 1;

  if (argc != 4)
  {
    di << argv[0] << "Invalid number of arguments\n";
    return 1;
  }
  V3dView->SetAxialScale(Draw::Atof(argv[1]), Draw::Atof(argv[2]), Draw::Atof(argv[3]));
  return 0;
}

//=================================================================================================

static int VZBuffTrihedron(Draw_Interpretor& /*theDI*/,
                           Standard_Integer theArgNb,
                           const char**     theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  ViewerTest_AutoUpdater anUpdateTool(ViewerTest::GetAISContext(), aView);

  Aspect_TypeOfTriedronPosition aPosition     = Aspect_TOTP_LEFT_LOWER;
  V3d_TypeOfVisualization       aVisType      = V3d_ZBUFFER;
  Quantity_Color                aLabelsColorX = Quantity_NOC_WHITE;
  Quantity_Color                aLabelsColorY = Quantity_NOC_WHITE;
  Quantity_Color                aLabelsColorZ = Quantity_NOC_WHITE;
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
    TCollection_AsciiString aFlag(anArg);
    aFlag.LowerCase();
    if (anUpdateTool.parseRedrawMode(aFlag))
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
    else if (aFlag == "-pos" || aFlag == "-position" || aFlag == "-corner")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }

      if (!ViewerTest::ParseCorner(theArgVec[anArgIter], aPosition))
      {
        Message::SendFail() << "Syntax error at '" << anArg << "' - unknown position '"
                            << theArgVec[anArgIter] << "'";
        return 1;
      }
    }
    else if (aFlag == "-type")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aTypeName(theArgVec[anArgIter]);
      aTypeName.LowerCase();
      if (aTypeName == "wireframe" || aTypeName == "wire")
      {
        aVisType = V3d_WIREFRAME;
      }
      else if (aTypeName == "zbuffer" || aTypeName == "shaded")
      {
        aVisType = V3d_ZBUFFER;
      }
      else
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "' - unknown type '"
                            << aTypeName << "'";
      }
    }
    else if (aFlag == "-scale")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      aScale = Draw::Atof(theArgVec[anArgIter]);
    }
    else if (aFlag == "-size" || aFlag == "-sizeratio")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      aSizeRatio = Draw::Atof(theArgVec[anArgIter]);
    }
    else if (aFlag == "-arrowdiam" || aFlag == "-arrowdiameter")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      anArrowDiam = Draw::Atof(theArgVec[anArgIter]);
    }
    else if (aFlag == "-nbfacets")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      aNbFacets = Draw::Atoi(theArgVec[anArgIter]);
    }
    else if (aFlag == "-colorlabel" || aFlag == "-colorlabels" || aFlag == "-colorlabelx"
             || aFlag == "-colorlabely" || aFlag == "-colorlabelz" || aFlag == "-colorarrowx"
             || aFlag == "-colorarrowy" || aFlag == "-colorarrowz")
    {
      Quantity_Color   aColor;
      Standard_Integer aNbParsed =
        Draw::ParseColor(theArgNb - anArgIter - 1, theArgVec + anArgIter + 1, aColor);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      if (aFlag == "-colorarrowx")
      {
        anArrowColorX = aColor;
      }
      else if (aFlag == "-colorarrowy")
      {
        anArrowColorY = aColor;
      }
      else if (aFlag == "-colorarrowz")
      {
        anArrowColorZ = aColor;
      }
      else if (aFlag == "-colorlabelx")
      {
        aLabelsColorX = aColor;
      }
      else if (aFlag == "-colorlabely")
      {
        aLabelsColorY = aColor;
      }
      else if (aFlag == "-colorlabelz")
      {
        aLabelsColorZ = aColor;
      }
      else
      {
        aLabelsColorZ = aLabelsColorY = aLabelsColorX = aColor;
      }
      anArgIter += aNbParsed;
    }
    else
    {
      Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
      return 1;
    }
  }

  const Handle(V3d_Trihedron)& aTrihedron = aView->Trihedron();
  aTrihedron->SetArrowsColor(anArrowColorX, anArrowColorY, anArrowColorZ);
  aTrihedron->SetLabelsColor(aLabelsColorX, aLabelsColorY, aLabelsColorZ);
  aTrihedron->SetSizeRatio(aSizeRatio);
  aTrihedron->SetNbFacets(aNbFacets);
  aTrihedron->SetArrowDiameter(anArrowDiam);
  aTrihedron->SetScale(aScale);
  aTrihedron->SetPosition(aPosition);
  aTrihedron->SetWireframe(aVisType == V3d_WIREFRAME);
  aTrihedron->Display(aView);

  aView->ZFitAll();
  return 0;
}

//==============================================================================
// function : VRotate
// purpose  : Camera Rotating
//==============================================================================

static int VRotate(Draw_Interpretor& /*theDi*/, Standard_Integer theArgNb, const char** theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Standard_Boolean hasFlags = Standard_False;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg(theArgVec[anArgIter]);
    TCollection_AsciiString aFlag(anArg);
    aFlag.LowerCase();
    if (aFlag == "-mousestart" || aFlag == "-mousefrom")
    {
      hasFlags = Standard_True;
      if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      Standard_Integer anX = Draw::Atoi(theArgVec[++anArgIter]);
      Standard_Integer anY = Draw::Atoi(theArgVec[++anArgIter]);
      aView->StartRotation(anX, anY);
    }
    else if (aFlag == "-mousemove")
    {
      hasFlags = Standard_True;
      if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      Standard_Integer anX = Draw::Atoi(theArgVec[++anArgIter]);
      Standard_Integer anY = Draw::Atoi(theArgVec[++anArgIter]);
      aView->Rotation(anX, anY);
    }
    else if (theArgNb != 4 && theArgNb != 7)
    {
      Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
      return 1;
    }
  }

  if (hasFlags)
  {
    return 0;
  }
  else if (theArgNb == 4)
  {
    Standard_Real anAX = Draw::Atof(theArgVec[1]);
    Standard_Real anAY = Draw::Atof(theArgVec[2]);
    Standard_Real anAZ = Draw::Atof(theArgVec[3]);
    aView->Rotate(anAX, anAY, anAZ);
    return 0;
  }
  else if (theArgNb == 7)
  {
    Standard_Real anAX = Draw::Atof(theArgVec[1]);
    Standard_Real anAY = Draw::Atof(theArgVec[2]);
    Standard_Real anAZ = Draw::Atof(theArgVec[3]);

    Standard_Real anX = Draw::Atof(theArgVec[4]);
    Standard_Real anY = Draw::Atof(theArgVec[5]);
    Standard_Real anZ = Draw::Atof(theArgVec[6]);

    aView->Rotate(anAX, anAY, anAZ, anX, anY, anZ);
    return 0;
  }

  Message::SendFail("Error: Invalid number of arguments");
  return 1;
}

//==============================================================================
// function : VZoom
// purpose  : View zoom in / out (relative to current zoom)
//==============================================================================

static int VZoom(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  if (V3dView.IsNull())
  {
    return 1;
  }

  if (argc == 2)
  {
    Standard_Real coef = Draw::Atof(argv[1]);
    if (coef <= 0.0)
    {
      di << argv[1] << "Invalid value\n";
      return 1;
    }
    V3dView->SetZoom(Draw::Atof(argv[1]));
    return 0;
  }
  else
  {
    di << argv[0] << " Invalid number of arguments\n";
    return 1;
  }
}

//==============================================================================
// function : VPan
// purpose  : View panning (in pixels)
//==============================================================================

static int VPan(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(V3d_View) V3dView = ViewerTest::CurrentView();
  if (V3dView.IsNull())
    return 1;

  if (argc == 3)
  {
    V3dView->Pan(Draw::Atoi(argv[1]), Draw::Atoi(argv[2]));
    return 0;
  }
  else
  {
    di << argv[0] << " Invalid number of arguments\n";
    return 1;
  }
}

//==============================================================================
// function : VPlace
// purpose  : Place the point (in pixels) at the center of the window
//==============================================================================
static int VPlace(Draw_Interpretor& /*theDi*/, Standard_Integer theArgNb, const char** theArgs)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  if (theArgNb != 3)
  {
    Message::SendFail("Syntax error: wrong number of arguments");
    return 1;
  }

  aView->Place(Draw::Atoi(theArgs[1]), Draw::Atoi(theArgs[2]), aView->Scale());

  return 0;
}

static int VColorScale(Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  Handle(V3d_View)               aView    = ViewerTest::CurrentView();
  if (aContext.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }
  if (theArgNb <= 1)
  {
    Message::SendFail() << "Error: wrong syntax at command '" << theArgVec[0] << "'";
    return 1;
  }

  Handle(AIS_ColorScale) aColorScale;
  if (GetMapOfAIS().IsBound2(theArgVec[1]))
  {
    // find existing object
    aColorScale = Handle(AIS_ColorScale)::DownCast(GetMapOfAIS().Find2(theArgVec[1]));
    if (aColorScale.IsNull())
    {
      Message::SendFail() << "Error: object '" << theArgVec[1]
                          << "'is already defined and is not a color scale";
      return 1;
    }
  }

  if (theArgNb <= 2)
  {
    if (aColorScale.IsNull())
    {
      Message::SendFail() << "Syntax error: colorscale with a given name does not exist";
      return 1;
    }

    theDI << "Color scale parameters for '" << theArgVec[1] << "':\n"
          << "Min range: " << aColorScale->GetMin() << "\n"
          << "Max range: " << aColorScale->GetMax() << "\n"
          << "Number of intervals: " << aColorScale->GetNumberOfIntervals() << "\n"
          << "Text height: " << aColorScale->GetTextHeight() << "\n"
          << "Color scale position: " << aColorScale->GetXPosition() << " "
          << aColorScale->GetYPosition() << "\n"
          << "Color scale title: " << aColorScale->GetTitle() << "\n"
          << "Label position: ";
    switch (aColorScale->GetLabelPosition())
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

  if (aColorScale.IsNull())
  {
    aColorScale = new AIS_ColorScale();
    aColorScale->SetZLayer(Graphic3d_ZLayerId_TopOSD);
    aContext->SetTransformPersistence(
      aColorScale,
      new Graphic3d_TransformPers(Graphic3d_TMF_2d, Aspect_TOTP_LEFT_LOWER));
  }

  ViewerTest_AutoUpdater anUpdateTool(aContext, aView);
  for (Standard_Integer anArgIter = 2; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg = theArgVec[anArgIter];
    TCollection_AsciiString aFlag(anArg);
    aFlag.LowerCase();
    if (anUpdateTool.parseRedrawMode(aFlag))
    {
      continue;
    }
    else if (aFlag == "-range")
    {
      if (anArgIter + 3 >= theArgNb)
      {
        Message::SendFail() << "Error: wrong syntax at argument '" << anArg << "'";
        return 1;
      }

      const TCollection_AsciiString aRangeMin(theArgVec[++anArgIter]);
      const TCollection_AsciiString aRangeMax(theArgVec[++anArgIter]);
      const TCollection_AsciiString aNbIntervals(theArgVec[++anArgIter]);
      if (!aRangeMin.IsRealValue(Standard_True) || !aRangeMax.IsRealValue(Standard_True))
      {
        Message::SendFail("Syntax error: the range values should be real");
        return 1;
      }
      else if (!aNbIntervals.IsIntegerValue())
      {
        Message::SendFail("Syntax error: the number of intervals should be integer");
        return 1;
      }

      aColorScale->SetRange(aRangeMin.RealValue(), aRangeMax.RealValue());
      aColorScale->SetNumberOfIntervals(aNbIntervals.IntegerValue());
    }
    else if (aFlag == "-font")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      TCollection_AsciiString aFontArg(theArgVec[anArgIter + 1]);
      if (!aFontArg.IsIntegerValue())
      {
        Message::SendFail("Syntax error: HeightFont value should be integer");
        return 1;
      }

      aColorScale->SetTextHeight(aFontArg.IntegerValue());
      anArgIter += 1;
    }
    else if (aFlag == "-textpos")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aTextPosArg(theArgVec[++anArgIter]);
      aTextPosArg.LowerCase();
      Aspect_TypeOfColorScalePosition aLabPosition = Aspect_TOCSP_NONE;
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
        Message::SendFail() << "Syntax error: unknown position '" << aTextPosArg << "'";
        return 1;
      }
      aColorScale->SetLabelPosition(aLabPosition);
    }
    else if (aFlag == "-logarithmic" || aFlag == "-log")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Synta error at argument '" << anArg << "'";
        return 1;
      }

      Standard_Boolean IsLog;
      if (!Draw::ParseOnOff(theArgVec[++anArgIter], IsLog))
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      aColorScale->SetLogarithmic(IsLog);
    }
    else if (aFlag == "-huerange" || aFlag == "-hue")
    {
      if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Real aHueMin = Draw::Atof(theArgVec[++anArgIter]);
      const Standard_Real aHueMax = Draw::Atof(theArgVec[++anArgIter]);
      aColorScale->SetHueRange(aHueMin, aHueMax);
    }
    else if (aFlag == "-colorrange")
    {
      Quantity_Color   aColorMin, aColorMax;
      Standard_Integer aNbParsed1 =
        Draw::ParseColor(theArgNb - (anArgIter + 1), theArgVec + (anArgIter + 1), aColorMin);
      anArgIter += aNbParsed1;
      Standard_Integer aNbParsed2 =
        Draw::ParseColor(theArgNb - (anArgIter + 1), theArgVec + (anArgIter + 1), aColorMax);
      anArgIter += aNbParsed2;
      if (aNbParsed1 == 0 || aNbParsed2 == 0)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }

      aColorScale->SetColorRange(aColorMin, aColorMax);
    }
    else if (aFlag == "-reversed" || aFlag == "-inverted" || aFlag == "-topdown"
             || aFlag == "-bottomup")
    {
      Standard_Boolean toEnable = Standard_True;
      if (anArgIter + 1 < theArgNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      aColorScale->SetReversed((aFlag == "-topdown") ? !toEnable : toEnable);
    }
    else if (aFlag == "-smooth" || aFlag == "-smoothtransition")
    {
      Standard_Boolean toEnable = Standard_True;
      if (anArgIter + 1 < theArgNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      aColorScale->SetSmoothTransition(toEnable);
    }
    else if (aFlag == "-xy")
    {
      if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const TCollection_AsciiString anX(theArgVec[++anArgIter]);
      const TCollection_AsciiString anY(theArgVec[++anArgIter]);
      if (!anX.IsIntegerValue() || !anY.IsIntegerValue())
      {
        Message::SendFail("Syntax error: coordinates should be integer values");
        return 1;
      }

      aColorScale->SetPosition(anX.IntegerValue(), anY.IntegerValue());
    }
    else if (aFlag == "-width" || aFlag == "-w" || aFlag == "-breadth")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const TCollection_AsciiString aBreadth(theArgVec[++anArgIter]);
      if (!aBreadth.IsIntegerValue())
      {
        Message::SendFail("Syntax error: a width should be an integer value");
        return 1;
      }
      aColorScale->SetBreadth(aBreadth.IntegerValue());
    }
    else if (aFlag == "-height" || aFlag == "-h")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const TCollection_AsciiString aHeight(theArgVec[++anArgIter]);
      if (!aHeight.IsIntegerValue())
      {
        Message::SendFail("Syntax error: a width should be an integer value");
        return 1;
      }
      aColorScale->SetHeight(aHeight.IntegerValue());
    }
    else if (aFlag == "-color")
    {
      if (aColorScale->GetColorType() != Aspect_TOCSD_USER)
      {
        Message::SendFail(
          "Syntax error: wrong color type. Call -colors before to set user-specified colors");
        return 1;
      }
      else if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const TCollection_AsciiString anInd(theArgVec[++anArgIter]);
      if (!anInd.IsIntegerValue())
      {
        Message::SendFail("Syntax error: Index value should be integer");
        return 1;
      }
      const Standard_Integer anIndex = anInd.IntegerValue();
      if (anIndex <= 0 || anIndex > aColorScale->GetNumberOfIntervals())
      {
        Message::SendFail() << "Syntax error: Index value should be within range 1.."
                            << aColorScale->GetNumberOfIntervals();
        return 1;
      }

      Quantity_Color   aColor;
      Standard_Integer aNbParsed =
        Draw::ParseColor(theArgNb - (anArgIter + 1), theArgVec + (anArgIter + 1), aColor);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Error: wrong syntax at '" << anArg << "'";
        return 1;
      }
      aColorScale->SetIntervalColor(aColor, anIndex);
      aColorScale->SetColorType(Aspect_TOCSD_USER);
      anArgIter += aNbParsed;
    }
    else if (aFlag == "-label")
    {
      if (aColorScale->GetColorType() != Aspect_TOCSD_USER)
      {
        Message::SendFail(
          "Syntax error: wrong label type. Call -labels before to set user-specified labels");
        return 1;
      }
      else if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      Standard_Integer anIndex = Draw::Atoi(theArgVec[anArgIter + 1]);
      if (anIndex <= 0 || anIndex > aColorScale->GetNumberOfIntervals() + 1)
      {
        Message::SendFail() << "Syntax error: Index value should be within range 1.."
                            << (aColorScale->GetNumberOfIntervals() + 1);
        return 1;
      }

      TCollection_ExtendedString aText(theArgVec[anArgIter + 2], Standard_True);
      aColorScale->SetLabel(aText, anIndex);
      aColorScale->SetLabelType(Aspect_TOCSD_USER);
      anArgIter += 2;
    }
    else if (aFlag == "-labelat" || aFlag == "-labat" || aFlag == "-labelatborder"
             || aFlag == "-labatborder" || aFlag == "-labelatcenter" || aFlag == "-labatcenter")
    {
      Standard_Boolean toEnable = Standard_True;
      if (aFlag == "-labelat" || aFlag == "-labat")
      {
        Standard_Integer aLabAtBorder = -1;
        if (++anArgIter >= theArgNb)
        {
          TCollection_AsciiString anAtBorder(theArgVec[anArgIter]);
          anAtBorder.LowerCase();
          if (anAtBorder == "border")
          {
            aLabAtBorder = 1;
          }
          else if (anAtBorder == "center")
          {
            aLabAtBorder = 0;
          }
        }
        if (aLabAtBorder == -1)
        {
          Message::SendFail() << "Syntax error at argument '" << anArg << "'";
          return 1;
        }
        toEnable = (aLabAtBorder == 1);
      }
      else if (anArgIter + 1 < theArgNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      aColorScale->SetLabelAtBorder(
        aFlag == "-labelatcenter" || aFlag == "-labatcenter" ? !toEnable : toEnable);
    }
    else if (aFlag == "-colors")
    {
      Aspect_SequenceOfColor aSeq;
      for (;;)
      {
        Quantity_Color   aColor;
        Standard_Integer aNbParsed =
          Draw::ParseColor(theArgNb - (anArgIter + 1), theArgVec + (anArgIter + 1), aColor);
        if (aNbParsed == 0)
        {
          break;
        }
        anArgIter += aNbParsed;
        aSeq.Append(aColor);
      }
      if (aSeq.Length() != aColorScale->GetNumberOfIntervals())
      {
        Message::SendFail() << "Error: not enough arguments! You should provide color names or RGB "
                               "color values for every interval of the "
                            << aColorScale->GetNumberOfIntervals() << " intervals";
        return 1;
      }

      aColorScale->SetColors(aSeq);
      aColorScale->SetColorType(Aspect_TOCSD_USER);
    }
    else if (aFlag == "-uniform")
    {
      const Standard_Real aLightness = Draw::Atof(theArgVec[++anArgIter]);
      const Standard_Real aHueStart  = Draw::Atof(theArgVec[++anArgIter]);
      const Standard_Real aHueEnd    = Draw::Atof(theArgVec[++anArgIter]);
      aColorScale->SetUniformColors(aLightness, aHueStart, aHueEnd);
      aColorScale->SetColorType(Aspect_TOCSD_USER);
    }
    else if (aFlag == "-labels" || aFlag == "-freelabels")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      Standard_Integer aNbLabels = aColorScale->IsLabelAtBorder()
                                     ? aColorScale->GetNumberOfIntervals() + 1
                                     : aColorScale->GetNumberOfIntervals();
      if (aFlag == "-freelabels")
      {
        ++anArgIter;
        aNbLabels = Draw::Atoi(theArgVec[anArgIter]);
      }
      if (anArgIter + aNbLabels >= theArgNb)
      {
        Message::SendFail() << "Syntax error: not enough arguments. " << aNbLabels
                            << " text labels are expected";
        return 1;
      }

      TColStd_SequenceOfExtendedString aSeq;
      for (Standard_Integer aLabelIter = 0; aLabelIter < aNbLabels; ++aLabelIter)
      {
        aSeq.Append(TCollection_ExtendedString(theArgVec[++anArgIter], Standard_True));
      }
      aColorScale->SetLabels(aSeq);
      aColorScale->SetLabelType(Aspect_TOCSD_USER);
    }
    else if (aFlag == "-title")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      Standard_Boolean isTwoArgs = Standard_False;
      if (anArgIter + 2 < theArgNb)
      {
        TCollection_AsciiString aSecondArg(theArgVec[anArgIter + 2]);
        aSecondArg.LowerCase();
        Standard_DISABLE_DEPRECATION_WARNINGS if (aSecondArg == "none")
        {
          aColorScale->SetTitlePosition(Aspect_TOCSP_NONE);
          isTwoArgs = Standard_True;
        }
        else if (aSecondArg == "left")
        {
          aColorScale->SetTitlePosition(Aspect_TOCSP_LEFT);
          isTwoArgs = Standard_True;
        }
        else if (aSecondArg == "right")
        {
          aColorScale->SetTitlePosition(Aspect_TOCSP_RIGHT);
          isTwoArgs = Standard_True;
        }
        else if (aSecondArg == "center")
        {
          aColorScale->SetTitlePosition(Aspect_TOCSP_CENTER);
          isTwoArgs = Standard_True;
        }
        Standard_ENABLE_DEPRECATION_WARNINGS
      }

      TCollection_ExtendedString aTitle(theArgVec[anArgIter + 1], Standard_True);
      aColorScale->SetTitle(aTitle);
      if (isTwoArgs)
      {
        anArgIter += 1;
      }
      anArgIter += 1;
    }
    else if (aFlag == "-demoversion" || aFlag == "-demo")
    {
      aColorScale->SetPosition(0, 0);
      aColorScale->SetTextHeight(16);
      aColorScale->SetRange(0.0, 100.0);
      aColorScale->SetNumberOfIntervals(10);
      aColorScale->SetBreadth(0);
      aColorScale->SetHeight(0);
      aColorScale->SetLabelPosition(Aspect_TOCSP_RIGHT);
      aColorScale->SetColorType(Aspect_TOCSD_AUTO);
      aColorScale->SetLabelType(Aspect_TOCSD_AUTO);
    }
    else if (aFlag == "-findcolor")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString anArg1(theArgVec[++anArgIter]);

      if (!anArg1.IsRealValue(Standard_True))
      {
        Message::SendFail("Syntax error: the value should be real");
        return 1;
      }

      Quantity_Color aColor;
      aColorScale->FindColor(anArg1.RealValue(), aColor);
      theDI << Quantity_Color::StringName(aColor.Name());
      return 0;
    }
    else
    {
      Message::SendFail() << "Syntax error at " << anArg << " - unknown argument";
      return 1;
    }
  }

  Standard_Integer aWinWidth = 0, aWinHeight = 0;
  aView->Window()->Size(aWinWidth, aWinHeight);
  if (aColorScale->GetBreadth() == 0)
  {
    aColorScale->SetBreadth(aWinWidth);
  }
  if (aColorScale->GetHeight() == 0)
  {
    aColorScale->SetHeight(aWinHeight);
  }
  aColorScale->SetToUpdate();
  ViewerTest::Display(theArgVec[1], aColorScale, Standard_False, Standard_True);
  return 0;
}

//==============================================================================
// function : VGraduatedTrihedron
// purpose  : Displays or hides a graduated trihedron
//==============================================================================
static Standard_Boolean GetColor(const TCollection_AsciiString& theValue, Quantity_Color& theColor)
{
  Quantity_NameOfColor    aColorName;
  TCollection_AsciiString aVal = theValue;
  aVal.UpperCase();
  if (!Quantity_Color::ColorFromName(aVal.ToCString(), aColorName))
  {
    return Standard_False;
  }
  theColor = Quantity_Color(aColorName);
  return Standard_True;
}

static int VGraduatedTrihedron(Draw_Interpretor& /*theDi*/,
                               Standard_Integer theArgNum,
                               const char**     theArgs)
{
  if (theArgNum < 2)
  {
    Message::SendFail() << "Syntax error: wrong number of parameters. Type 'help" << theArgs[0]
                        << "' for more information";
    return 1;
  }

  NCollection_DataMap<TCollection_AsciiString, Handle(TColStd_HSequenceOfAsciiString)> aMapOfArgs;
  TCollection_AsciiString                                                              aParseKey;
  for (Standard_Integer anArgIt = 1; anArgIt < theArgNum; ++anArgIt)
  {
    TCollection_AsciiString anArg(theArgs[anArgIt]);

    if (anArg.Value(1) == '-' && !anArg.IsRealValue(Standard_True))
    {
      aParseKey = anArg;
      aParseKey.Remove(1);
      aParseKey.LowerCase();
      aMapOfArgs.Bind(aParseKey, new TColStd_HSequenceOfAsciiString);
      continue;
    }

    if (aParseKey.IsEmpty())
    {
      continue;
    }

    aMapOfArgs(aParseKey)->Append(anArg);
  }

  // Check parameters
  for (NCollection_DataMap<TCollection_AsciiString,
                           Handle(TColStd_HSequenceOfAsciiString)>::Iterator aMapIt(aMapOfArgs);
       aMapIt.More();
       aMapIt.Next())
  {
    const TCollection_AsciiString&                aKey   = aMapIt.Key();
    const Handle(TColStd_HSequenceOfAsciiString)& anArgs = aMapIt.Value();

    // Bool key, without arguments
    if ((aKey.IsEqual("on") || aKey.IsEqual("off")) && anArgs->IsEmpty())
    {
      continue;
    }

    // One argument
    if ((aKey.IsEqual("xname") || aKey.IsEqual("yname") || aKey.IsEqual("zname"))
        && anArgs->Length() == 1)
    {
      continue;
    }

    // On/off arguments
    if ((aKey.IsEqual("xdrawname") || aKey.IsEqual("ydrawname") || aKey.IsEqual("zdrawname")
         || aKey.IsEqual("xdrawticks") || aKey.IsEqual("ydrawticks") || aKey.IsEqual("zdrawticks")
         || aKey.IsEqual("xdrawvalues") || aKey.IsEqual("ydrawvalues")
         || aKey.IsEqual("zdrawvalues") || aKey.IsEqual("drawgrid") || aKey.IsEqual("drawaxes"))
        && anArgs->Length() == 1
        && (anArgs->Value(1).IsEqual("on") || anArgs->Value(1).IsEqual("off")))
    {
      continue;
    }

    // One string argument
    if ((aKey.IsEqual("xnamecolor") || aKey.IsEqual("ynamecolor") || aKey.IsEqual("znamecolor")
         || aKey.IsEqual("xcolor") || aKey.IsEqual("ycolor") || aKey.IsEqual("zcolor"))
        && anArgs->Length() == 1 && !anArgs->Value(1).IsIntegerValue()
        && !anArgs->Value(1).IsRealValue(Standard_True))
    {
      continue;
    }

    // One integer argument
    if ((aKey.IsEqual("xticks") || aKey.IsEqual("yticks") || aKey.IsEqual("zticks")
         || aKey.IsEqual("xticklength") || aKey.IsEqual("yticklength")
         || aKey.IsEqual("zticklength") || aKey.IsEqual("xnameoffset")
         || aKey.IsEqual("ynameoffset") || aKey.IsEqual("znameoffset")
         || aKey.IsEqual("xvaluesoffset") || aKey.IsEqual("yvaluesoffset")
         || aKey.IsEqual("zvaluesoffset"))
        && anArgs->Length() == 1 && anArgs->Value(1).IsIntegerValue())
    {
      continue;
    }

    // One real argument
    if (aKey.IsEqual("arrowlength") && anArgs->Length() == 1
        && (anArgs->Value(1).IsIntegerValue() || anArgs->Value(1).IsRealValue(Standard_True)))
    {
      continue;
    }

    // Two string arguments
    if ((aKey.IsEqual("namefont") || aKey.IsEqual("valuesfont")) && anArgs->Length() == 1
        && !anArgs->Value(1).IsIntegerValue() && !anArgs->Value(1).IsRealValue(Standard_True))
    {
      continue;
    }

    TCollection_AsciiString aLowerKey;
    aLowerKey = "-";
    aLowerKey += aKey;
    aLowerKey.LowerCase();
    Message::SendFail() << "Syntax error: " << aLowerKey
                        << " is unknown option, or the arguments are unacceptable.\n"
                        << "Type help for more information";
    return 1;
  }

  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  if (anAISContext.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Standard_Boolean             toDisplay = Standard_True;
  Quantity_Color               aColor;
  Graphic3d_GraduatedTrihedron aTrihedronData;
  // Process parameters
  Handle(TColStd_HSequenceOfAsciiString) aValues;
  if (aMapOfArgs.Find("off", aValues))
  {
    toDisplay = Standard_False;
  }

  // AXES NAMES
  if (aMapOfArgs.Find("xname", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetName(aValues->Value(1));
  }
  if (aMapOfArgs.Find("yname", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetName(aValues->Value(1));
  }
  if (aMapOfArgs.Find("zname", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetName(aValues->Value(1));
  }
  if (aMapOfArgs.Find("xdrawname", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetDrawName(aValues->Value(1).IsEqual("on"));
  }
  if (aMapOfArgs.Find("ydrawname", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetDrawName(aValues->Value(1).IsEqual("on"));
  }
  if (aMapOfArgs.Find("zdrawname", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetDrawName(aValues->Value(1).IsEqual("on"));
  }
  if (aMapOfArgs.Find("xnameoffset", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetNameOffset(aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find("ynameoffset", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetNameOffset(aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find("znameoffset", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetNameOffset(aValues->Value(1).IntegerValue());
  }

  // COLORS
  if (aMapOfArgs.Find("xnamecolor", aValues))
  {
    if (!GetColor(aValues->Value(1), aColor))
    {
      Message::SendFail("Syntax error: -xnamecolor wrong color name");
      return 1;
    }
    aTrihedronData.ChangeXAxisAspect().SetNameColor(aColor);
  }
  if (aMapOfArgs.Find("ynamecolor", aValues))
  {
    if (!GetColor(aValues->Value(1), aColor))
    {
      Message::SendFail("Syntax error: -ynamecolor wrong color name");
      return 1;
    }
    aTrihedronData.ChangeYAxisAspect().SetNameColor(aColor);
  }
  if (aMapOfArgs.Find("znamecolor", aValues))
  {
    if (!GetColor(aValues->Value(1), aColor))
    {
      Message::SendFail("Syntax error: -znamecolor wrong color name");
      return 1;
    }
    aTrihedronData.ChangeZAxisAspect().SetNameColor(aColor);
  }
  if (aMapOfArgs.Find("xcolor", aValues))
  {
    if (!GetColor(aValues->Value(1), aColor))
    {
      Message::SendFail("Syntax error: -xcolor wrong color name");
      return 1;
    }
    aTrihedronData.ChangeXAxisAspect().SetColor(aColor);
  }
  if (aMapOfArgs.Find("ycolor", aValues))
  {
    if (!GetColor(aValues->Value(1), aColor))
    {
      Message::SendFail("Syntax error: -ycolor wrong color name");
      return 1;
    }
    aTrihedronData.ChangeYAxisAspect().SetColor(aColor);
  }
  if (aMapOfArgs.Find("zcolor", aValues))
  {
    if (!GetColor(aValues->Value(1), aColor))
    {
      Message::SendFail("Syntax error: -zcolor wrong color name");
      return 1;
    }
    aTrihedronData.ChangeZAxisAspect().SetColor(aColor);
  }

  // TICKMARKS
  if (aMapOfArgs.Find("xticks", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetTickmarksNumber(aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find("yticks", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetTickmarksNumber(aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find("zticks", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetTickmarksNumber(aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find("xticklength", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetTickmarksLength(aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find("yticklength", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetTickmarksLength(aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find("zticklength", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetTickmarksLength(aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find("xdrawticks", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetDrawTickmarks(aValues->Value(1).IsEqual("on"));
  }
  if (aMapOfArgs.Find("ydrawticks", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetDrawTickmarks(aValues->Value(1).IsEqual("on"));
  }
  if (aMapOfArgs.Find("zdrawticks", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetDrawTickmarks(aValues->Value(1).IsEqual("on"));
  }

  // VALUES
  if (aMapOfArgs.Find("xdrawvalues", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetDrawValues(aValues->Value(1).IsEqual("on"));
  }
  if (aMapOfArgs.Find("ydrawvalues", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetDrawValues(aValues->Value(1).IsEqual("on"));
  }
  if (aMapOfArgs.Find("zdrawvalues", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetDrawValues(aValues->Value(1).IsEqual("on"));
  }
  if (aMapOfArgs.Find("xvaluesoffset", aValues))
  {
    aTrihedronData.ChangeXAxisAspect().SetValuesOffset(aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find("yvaluesoffset", aValues))
  {
    aTrihedronData.ChangeYAxisAspect().SetValuesOffset(aValues->Value(1).IntegerValue());
  }
  if (aMapOfArgs.Find("zvaluesoffset", aValues))
  {
    aTrihedronData.ChangeZAxisAspect().SetValuesOffset(aValues->Value(1).IntegerValue());
  }

  // ARROWS
  if (aMapOfArgs.Find("arrowlength", aValues))
  {
    aTrihedronData.SetArrowsLength((Standard_ShortReal)aValues->Value(1).RealValue());
  }

  // FONTS
  if (aMapOfArgs.Find("namefont", aValues))
  {
    aTrihedronData.SetNamesFont(aValues->Value(1));
  }
  if (aMapOfArgs.Find("valuesfont", aValues))
  {
    aTrihedronData.SetValuesFont(aValues->Value(1));
  }

  if (aMapOfArgs.Find("drawgrid", aValues))
  {
    aTrihedronData.SetDrawGrid(aValues->Value(1).IsEqual("on"));
  }
  if (aMapOfArgs.Find("drawaxes", aValues))
  {
    aTrihedronData.SetDrawAxes(aValues->Value(1).IsEqual("on"));
  }

  // The final step: display of erase trihedron
  if (toDisplay)
  {
    ViewerTest::CurrentView()->GraduatedTrihedronDisplay(aTrihedronData);
  }
  else
  {
    ViewerTest::CurrentView()->GraduatedTrihedronErase();
  }

  ViewerTest::GetAISContext()->UpdateCurrentViewer();
  ViewerTest::CurrentView()->Redraw();

  return 0;
}

//=================================================================================================

static int VTile(Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Graphic3d_CameraTile aTile = aView->Camera()->Tile();
  if (theArgNb < 2)
  {
    theDI << "Total size: " << aTile.TotalSize.x() << " " << aTile.TotalSize.y() << "\n"
          << "Tile  size: " << aTile.TileSize.x() << " " << aTile.TileSize.y() << "\n"
          << "Lower left: " << aTile.Offset.x() << " " << aTile.Offset.y() << "\n";
    return 0;
  }

  aView->Window()->Size(aTile.TileSize.x(), aTile.TileSize.y());
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-lowerleft" || anArg == "-upperleft")
    {
      if (anArgIter + 3 < theArgNb)
      {
        Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }
      aTile.IsTopDown  = (anArg == "-upperleft") == Standard_True;
      aTile.Offset.x() = Draw::Atoi(theArgVec[anArgIter + 1]);
      aTile.Offset.y() = Draw::Atoi(theArgVec[anArgIter + 2]);
    }
    else if (anArg == "-total" || anArg == "-totalsize" || anArg == "-viewsize")
    {
      if (anArgIter + 3 < theArgNb)
      {
        Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }
      aTile.TotalSize.x() = Draw::Atoi(theArgVec[anArgIter + 1]);
      aTile.TotalSize.y() = Draw::Atoi(theArgVec[anArgIter + 2]);
      if (aTile.TotalSize.x() < 1 || aTile.TotalSize.y() < 1)
      {
        Message::SendFail("Error: total size is incorrect");
        return 1;
      }
    }
    else if (anArg == "-tilesize")
    {
      if (anArgIter + 3 < theArgNb)
      {
        Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }

      aTile.TileSize.x() = Draw::Atoi(theArgVec[anArgIter + 1]);
      aTile.TileSize.y() = Draw::Atoi(theArgVec[anArgIter + 2]);
      if (aTile.TileSize.x() < 1 || aTile.TileSize.y() < 1)
      {
        Message::SendFail("Error: tile size is incorrect");
        return 1;
      }
    }
    else if (anArg == "-unset")
    {
      aView->Camera()->SetTile(Graphic3d_CameraTile());
      aView->Redraw();
      return 0;
    }
  }

  if (aTile.TileSize.x() < 1 || aTile.TileSize.y() < 1)
  {
    Message::SendFail("Error: tile size is undefined");
    return 1;
  }
  else if (aTile.TotalSize.x() < 1 || aTile.TotalSize.y() < 1)
  {
    Message::SendFail("Error: total size is undefined");
    return 1;
  }

  aView->Camera()->SetTile(aTile);
  aView->Redraw();
  return 0;
}

//! Format ZLayer ID.
inline const char* formZLayerId(const Standard_Integer theLayerId)
{
  switch (theLayerId)
  {
    case Graphic3d_ZLayerId_UNKNOWN:
      return "[INVALID]";
    case Graphic3d_ZLayerId_Default:
      return "[DEFAULT]";
    case Graphic3d_ZLayerId_Top:
      return "[TOP]";
    case Graphic3d_ZLayerId_Topmost:
      return "[TOPMOST]";
    case Graphic3d_ZLayerId_TopOSD:
      return "[OVERLAY]";
    case Graphic3d_ZLayerId_BotOSD:
      return "[UNDERLAY]";
  }
  return "";
}

//! Print the ZLayer information.
inline void printZLayerInfo(Draw_Interpretor& theDI, const Graphic3d_ZLayerSettings& theLayer)
{
  if (!theLayer.Name().IsEmpty())
  {
    theDI << "  Name: " << theLayer.Name() << "\n";
  }
  if (theLayer.IsImmediate())
  {
    theDI << "  Immediate: TRUE\n";
  }
  theDI << "  Origin: " << theLayer.Origin().X() << " " << theLayer.Origin().Y() << " "
        << theLayer.Origin().Z() << "\n";
  theDI << "  Culling distance: " << theLayer.CullingDistance() << "\n";
  theDI << "  Culling size: " << theLayer.CullingSize() << "\n";
  theDI << "  Depth test:   " << (theLayer.ToEnableDepthTest() ? "enabled" : "disabled") << "\n";
  theDI << "  Depth write:  " << (theLayer.ToEnableDepthWrite() ? "enabled" : "disabled") << "\n";
  theDI << "  Depth buffer clearing: " << (theLayer.ToClearDepth() ? "enabled" : "disabled")
        << "\n";
  if (theLayer.PolygonOffset().Mode != Aspect_POM_None)
  {
    theDI << "  Depth offset: " << theLayer.PolygonOffset().Factor << " "
          << theLayer.PolygonOffset().Units << "\n";
  }
}

//==============================================================================
// function : VZLayer
// purpose  : Test z layer operations for v3d viewer
//==============================================================================
static int VZLayer(Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  const Handle(V3d_Viewer)& aViewer = aContextAIS->CurrentViewer();
  if (theArgNb < 2)
  {
    TColStd_SequenceOfInteger aLayers;
    aViewer->GetAllZLayers(aLayers);
    for (TColStd_SequenceOfInteger::Iterator aLayeriter(aLayers); aLayeriter.More();
         aLayeriter.Next())
    {
      theDI << "ZLayer " << aLayeriter.Value() << " " << formZLayerId(aLayeriter.Value()) << "\n";
      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings(aLayeriter.Value());
      printZLayerInfo(theDI, aSettings);
    }
    return 0;
  }

  Standard_Integer       anArgIter = 1;
  Standard_Integer       aLayerId  = Graphic3d_ZLayerId_UNKNOWN;
  ViewerTest_AutoUpdater anUpdateTool(aContextAIS, ViewerTest::CurrentView());
  if (anUpdateTool.parseRedrawMode(theArgVec[anArgIter]))
  {
    ++anArgIter;
  }

  {
    TCollection_AsciiString aFirstArg(theArgVec[anArgIter]);
    if (aFirstArg.IsIntegerValue())
    {
      ++anArgIter;
      aLayerId = aFirstArg.IntegerValue();
    }
    else
    {
      if (ViewerTest::ParseZLayerName(aFirstArg.ToCString(), aLayerId))
      {
        ++anArgIter;
      }
    }
  }

  Graphic3d_ZLayerId anOtherLayerId = Graphic3d_ZLayerId_UNKNOWN;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    // perform operation
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode(anArg))
    {
      //
    }
    else if (anArg == "-add" || anArg == "add")
    {
      aLayerId = Graphic3d_ZLayerId_UNKNOWN;
      if (!aViewer->AddZLayer(aLayerId))
      {
        Message::SendFail("Error: can not add a new z layer");
        return 0;
      }

      theDI << aLayerId;
    }
    else if (anArg == "-insertbefore" && anArgIter + 1 < theArgNb
             && ViewerTest::ParseZLayer(theArgVec[anArgIter + 1], anOtherLayerId))
    {
      ++anArgIter;
      aLayerId = Graphic3d_ZLayerId_UNKNOWN;
      if (!aViewer->InsertLayerBefore(aLayerId, Graphic3d_ZLayerSettings(), anOtherLayerId))
      {
        Message::SendFail("Error: can not add a new z layer");
        return 0;
      }

      theDI << aLayerId;
    }
    else if (anArg == "-insertafter" && anArgIter + 1 < theArgNb
             && ViewerTest::ParseZLayer(theArgVec[anArgIter + 1], anOtherLayerId))
    {
      ++anArgIter;
      aLayerId = Graphic3d_ZLayerId_UNKNOWN;
      if (!aViewer->InsertLayerAfter(aLayerId, Graphic3d_ZLayerSettings(), anOtherLayerId))
      {
        Message::SendFail("Error: can not add a new z layer");
        return 0;
      }

      theDI << aLayerId;
    }
    else if (anArg == "-del" || anArg == "-delete" || anArg == "del")
    {
      if (aLayerId == Graphic3d_ZLayerId_UNKNOWN)
      {
        if (++anArgIter >= theArgNb)
        {
          Message::SendFail("Syntax error: id of z layer to remove is missing");
          return 1;
        }

        aLayerId = Draw::Atoi(theArgVec[anArgIter]);
      }

      if (aLayerId == Graphic3d_ZLayerId_UNKNOWN || aLayerId == Graphic3d_ZLayerId_Default
          || aLayerId == Graphic3d_ZLayerId_Top || aLayerId == Graphic3d_ZLayerId_Topmost
          || aLayerId == Graphic3d_ZLayerId_TopOSD || aLayerId == Graphic3d_ZLayerId_BotOSD)
      {
        Message::SendFail("Syntax error: standard Z layer can not be removed");
        return 1;
      }

      // move all object displayed in removing layer to default layer
      for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anObjIter(GetMapOfAIS());
           anObjIter.More();
           anObjIter.Next())
      {
        const Handle(AIS_InteractiveObject)& aPrs = anObjIter.Key1();
        if (aPrs.IsNull() || aPrs->ZLayer() != aLayerId)
        {
          continue;
        }
        aPrs->SetZLayer(Graphic3d_ZLayerId_Default);
      }

      if (!aViewer->RemoveZLayer(aLayerId))
      {
        Message::SendFail("Z layer can not be removed");
      }
      else
      {
        theDI << aLayerId << " ";
      }
    }
    else if (anArg == "-get" || anArg == "get")
    {
      TColStd_SequenceOfInteger aLayers;
      aViewer->GetAllZLayers(aLayers);
      for (TColStd_SequenceOfInteger::Iterator aLayeriter(aLayers); aLayeriter.More();
           aLayeriter.Next())
      {
        theDI << aLayeriter.Value() << " ";
      }

      theDI << "\n";
    }
    else if (anArg == "-name")
    {
      if (aLayerId == Graphic3d_ZLayerId_UNKNOWN)
      {
        Message::SendFail("Syntax error: id of Z layer is missing");
        return 1;
      }

      if (++anArgIter >= theArgNb)
      {
        Message::SendFail("Syntax error: name is missing");
        return 1;
      }

      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings(aLayerId);
      aSettings.SetName(theArgVec[anArgIter]);
      aViewer->SetZLayerSettings(aLayerId, aSettings);
    }
    else if (anArg == "-origin")
    {
      if (aLayerId == Graphic3d_ZLayerId_UNKNOWN)
      {
        Message::SendFail("Syntax error: id of Z layer is missing");
        return 1;
      }

      if (anArgIter + 2 >= theArgNb)
      {
        Message::SendFail("Syntax error: origin coordinates are missing");
        return 1;
      }

      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings(aLayerId);
      gp_XYZ                   anOrigin;
      anOrigin.SetX(Draw::Atof(theArgVec[anArgIter + 1]));
      anOrigin.SetY(Draw::Atof(theArgVec[anArgIter + 2]));
      anOrigin.SetZ(0.0);
      if (anArgIter + 3 < theArgNb)
      {
        anOrigin.SetZ(Draw::Atof(theArgVec[anArgIter + 3]));
        anArgIter += 3;
      }
      else
      {
        anArgIter += 2;
      }
      aSettings.SetOrigin(anOrigin);
      aViewer->SetZLayerSettings(aLayerId, aSettings);
    }
    else if (aLayerId != Graphic3d_ZLayerId_UNKNOWN && anArgIter + 1 < theArgNb
             && (anArg == "-cullingdistance" || anArg == "-cullingdist" || anArg == "-culldistance"
                 || anArg == "-culldist" || anArg == "-distcull" || anArg == "-distculling"
                 || anArg == "-distanceculling"))
    {
      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings(aLayerId);
      const Standard_Real      aDist     = Draw::Atof(theArgVec[++anArgIter]);
      aSettings.SetCullingDistance(aDist);
      aViewer->SetZLayerSettings(aLayerId, aSettings);
    }
    else if (aLayerId != Graphic3d_ZLayerId_UNKNOWN && anArgIter + 1 < theArgNb
             && (anArg == "-cullingsize" || anArg == "-cullsize" || anArg == "-sizecull"
                 || anArg == "-sizeculling"))
    {
      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings(aLayerId);
      const Standard_Real      aSize     = Draw::Atof(theArgVec[++anArgIter]);
      aSettings.SetCullingSize(aSize);
      aViewer->SetZLayerSettings(aLayerId, aSettings);
    }
    else if (anArg == "-settings" || anArg == "settings")
    {
      if (aLayerId == Graphic3d_ZLayerId_UNKNOWN)
      {
        if (++anArgIter >= theArgNb)
        {
          Message::SendFail("Syntax error: id of Z layer is missing");
          return 1;
        }

        aLayerId = Draw::Atoi(theArgVec[anArgIter]);
      }

      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings(aLayerId);
      printZLayerInfo(theDI, aSettings);
    }
    else if (anArg == "-enable" || anArg == "enable" || anArg == "-disable" || anArg == "disable")
    {
      const Standard_Boolean toEnable = anArg == "-enable" || anArg == "enable";
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail("Syntax error: option name is missing");
        return 1;
      }

      TCollection_AsciiString aSubOp(theArgVec[anArgIter]);
      aSubOp.LowerCase();
      if (aLayerId == Graphic3d_ZLayerId_UNKNOWN)
      {
        if (++anArgIter >= theArgNb)
        {
          Message::SendFail("Syntax error: id of Z layer is missing");
          return 1;
        }

        aLayerId = Draw::Atoi(theArgVec[anArgIter]);
      }

      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings(aLayerId);
      if (aSubOp == "depthtest" || aSubOp == "test")
      {
        aSettings.SetEnableDepthTest(toEnable);
      }
      else if (aSubOp == "depthwrite" || aSubOp == "write")
      {
        aSettings.SetEnableDepthWrite(toEnable);
      }
      else if (aSubOp == "depthclear" || aSubOp == "clear")
      {
        aSettings.SetClearDepth(toEnable);
      }
      else if (aSubOp == "depthoffset" || aSubOp == "offset")
      {
        Graphic3d_PolygonOffset aParams;
        aParams.Mode = toEnable ? Aspect_POM_Fill : Aspect_POM_None;
        if (toEnable)
        {
          if (anArgIter + 2 >= theArgNb)
          {
            Message::SendFail("Syntax error: factor and units values for depth offset are missing");
            return 1;
          }

          aParams.Factor = static_cast<Standard_ShortReal>(Draw::Atof(theArgVec[++anArgIter]));
          aParams.Units  = static_cast<Standard_ShortReal>(Draw::Atof(theArgVec[++anArgIter]));
        }
        aSettings.SetPolygonOffset(aParams);
      }
      else if (aSubOp == "positiveoffset" || aSubOp == "poffset")
      {
        if (toEnable)
        {
          aSettings.SetDepthOffsetPositive();
        }
        else
        {
          aSettings.SetPolygonOffset(Graphic3d_PolygonOffset());
        }
      }
      else if (aSubOp == "negativeoffset" || aSubOp == "noffset")
      {
        if (toEnable)
        {
          aSettings.SetDepthOffsetNegative();
        }
        else
        {
          aSettings.SetPolygonOffset(Graphic3d_PolygonOffset());
        }
      }
      else if (aSubOp == "textureenv")
      {
        aSettings.SetEnvironmentTexture(toEnable);
      }
      else if (aSubOp == "raytracing")
      {
        aSettings.SetRaytracable(toEnable);
      }

      aViewer->SetZLayerSettings(aLayerId, aSettings);
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown option " << theArgVec[anArgIter];
      return 1;
    }
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
  DEFINE_STANDARD_RTTI_INLINE(V3d_LineItem, AIS_InteractiveObject)

  // constructor
  Standard_EXPORT V3d_LineItem(Standard_Real     X1,
                               Standard_Real     Y1,
                               Standard_Real     X2,
                               Standard_Real     Y2,
                               Aspect_TypeOfLine theType   = Aspect_TOL_SOLID,
                               Standard_Real     theWidth  = 0.5,
                               Standard_Real     theTransp = 1.0);

private:
  virtual void Compute(const Handle(PrsMgr_PresentationManager)& thePrsMgr,
                       const Handle(Prs3d_Presentation)&         thePrs,
                       const Standard_Integer                    theMode) Standard_OVERRIDE;

  virtual void ComputeSelection(const Handle(SelectMgr_Selection)&,
                                const Standard_Integer) Standard_OVERRIDE
  {
  }

private:
  Standard_Real     myX1, myY1, myX2, myY2;
  Aspect_TypeOfLine myType;
  Standard_Real     myWidth;
};

// default constructor for line item
V3d_LineItem::V3d_LineItem(Standard_Real     X1,
                           Standard_Real     Y1,
                           Standard_Real     X2,
                           Standard_Real     Y2,
                           Aspect_TypeOfLine theType,
                           Standard_Real     theWidth,
                           Standard_Real     theTransp)
    : myX1(X1),
      myY1(Y1),
      myX2(X2),
      myY2(Y2),
      myType(theType),
      myWidth(theWidth)
{
  SetTransparency(1 - theTransp);
}

// render line
void V3d_LineItem::Compute(const Handle(PrsMgr_PresentationManager)&,
                           const Handle(Prs3d_Presentation)& thePresentation,
                           const Standard_Integer)
{
  thePresentation->Clear();
  Quantity_Color   aColor(Quantity_NOC_RED);
  Standard_Integer aWidth, aHeight;
  ViewerTest::CurrentView()->Window()->Size(aWidth, aHeight);
  Handle(Graphic3d_Group)            aGroup = thePresentation->CurrentGroup();
  Handle(Graphic3d_ArrayOfPolylines) aPrim  = new Graphic3d_ArrayOfPolylines(5);
  aPrim->AddVertex(myX1, aHeight - myY1, 0.);
  aPrim->AddVertex(myX2, aHeight - myY2, 0.);
  Handle(Prs3d_LineAspect) anAspect =
    new Prs3d_LineAspect(aColor, (Aspect_TypeOfLine)myType, myWidth);
  aGroup->SetPrimitivesAspect(anAspect->Aspect());
  aGroup->AddPrimitiveArray(aPrim);
}

//=============================================================================
// function : VLayerLine
// purpose  : Draws line in the v3d view layer with given attributes: linetype,
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

  Standard_Real aWidth        = 0.5;
  Standard_Real aTransparency = 1.0;

  // has width
  if (argc > 5)
    aWidth = Draw::Atof(argv[5]);

  // select appropriate line type
  Aspect_TypeOfLine aLineType = Aspect_TOL_SOLID;
  if (argc > 6 && !ViewerTest::ParseLineType(argv[6], aLineType))
  {
    Message::SendFail() << "Syntax error: unknown line type '" << argv[6] << "'";
    return 1;
  }

  // has transparency
  if (argc > 7)
  {
    aTransparency = Draw::Atof(argv[7]);
    if (aTransparency < 0 || aTransparency > 1.0)
      aTransparency = 1.0;
  }

  static Handle(V3d_LineItem) aLine;
  if (!aLine.IsNull())
  {
    aContext->Erase(aLine, Standard_False);
  }
  aLine = new V3d_LineItem(X1, Y1, X2, Y2, aLineType, aWidth, aTransparency);

  aContext->SetTransformPersistence(
    aLine,
    new Graphic3d_TransformPers(Graphic3d_TMF_2d, Aspect_TOTP_LEFT_LOWER));
  aLine->SetZLayer(Graphic3d_ZLayerId_TopOSD);
  aLine->SetToUpdate();
  aContext->Display(aLine, Standard_True);

  return 0;
}

//=================================================================================================

static int VGrid(Draw_Interpretor& /*theDI*/, Standard_Integer theArgNb, const char** theArgVec)
{
  Handle(V3d_View)   aView   = ViewerTest::CurrentView();
  Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext();
  if (aView.IsNull() || aViewer.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Aspect_GridType     aType = aViewer->GridType();
  Aspect_GridDrawMode aMode = aViewer->GridDrawMode();
  Graphic3d_Vec2d     aNewOriginXY, aNewStepXY, aNewSizeXY;
  Standard_Real       aNewRotAngle = 0.0, aNewZOffset = 0.0;
  bool hasOrigin = false, hasStep = false, hasRotAngle = false, hasSize = false, hasZOffset = false;
  ViewerTest_AutoUpdater anUpdateTool(ViewerTest::GetAISContext(), aView);
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode(theArgVec[anArgIter]))
    {
      continue;
    }
    else if (anArgIter + 1 < theArgNb && anArg == "-type")
    {
      TCollection_AsciiString anArgNext(theArgVec[++anArgIter]);
      anArgNext.LowerCase();
      if (anArgNext == "r" || anArgNext == "rect" || anArgNext == "rectangular")
      {
        aType = Aspect_GT_Rectangular;
      }
      else if (anArgNext == "c" || anArgNext == "circ" || anArgNext == "circular")
      {
        aType = Aspect_GT_Circular;
      }
      else
      {
        Message::SendFail() << "Syntax error at '" << anArgNext << "'";
        return 1;
      }
    }
    else if (anArgIter + 1 < theArgNb && anArg == "-mode")
    {
      TCollection_AsciiString anArgNext(theArgVec[++anArgIter]);
      anArgNext.LowerCase();
      if (anArgNext == "l" || anArgNext == "line" || anArgNext == "lines")
      {
        aMode = Aspect_GDM_Lines;
      }
      else if (anArgNext == "p" || anArgNext == "point" || anArgNext == "points")
      {
        aMode = Aspect_GDM_Points;
      }
      else
      {
        Message::SendFail() << "Syntax error at '" << anArgNext << "'";
        return 1;
      }
    }
    else if (anArgIter + 2 < theArgNb && (anArg == "-origin" || anArg == "-orig"))
    {
      hasOrigin = true;
      aNewOriginXY.SetValues(Draw::Atof(theArgVec[anArgIter + 1]),
                             Draw::Atof(theArgVec[anArgIter + 2]));
      anArgIter += 2;
    }
    else if (anArgIter + 2 < theArgNb && anArg == "-step")
    {
      hasStep = true;
      aNewStepXY.SetValues(Draw::Atof(theArgVec[anArgIter + 1]),
                           Draw::Atof(theArgVec[anArgIter + 2]));
      if (aNewStepXY.x() <= 0.0 || aNewStepXY.y() <= 0.0)
      {
        Message::SendFail() << "Syntax error: wrong step '" << theArgVec[anArgIter + 1] << " "
                            << theArgVec[anArgIter + 2] << "'";
        return 1;
      }
      anArgIter += 2;
    }
    else if (anArgIter + 1 < theArgNb
             && (anArg == "-angle" || anArg == "-rotangle" || anArg == "-rotationangle"))
    {
      hasRotAngle  = true;
      aNewRotAngle = Draw::Atof(theArgVec[++anArgIter]);
    }
    else if (anArgIter + 1 < theArgNb && (anArg == "-zoffset" || anArg == "-dz"))
    {
      hasZOffset  = true;
      aNewZOffset = Draw::Atof(theArgVec[++anArgIter]);
    }
    else if (anArgIter + 1 < theArgNb && anArg == "-radius")
    {
      hasSize = true;
      ++anArgIter;
      aNewSizeXY.SetValues(Draw::Atof(theArgVec[anArgIter]), 0.0);
      if (aNewStepXY.x() <= 0.0)
      {
        Message::SendFail() << "Syntax error: wrong size '" << theArgVec[anArgIter] << "'";
        return 1;
      }
    }
    else if (anArgIter + 2 < theArgNb && anArg == "-size")
    {
      hasSize = true;
      aNewSizeXY.SetValues(Draw::Atof(theArgVec[anArgIter + 1]),
                           Draw::Atof(theArgVec[anArgIter + 2]));
      if (aNewStepXY.x() <= 0.0 || aNewStepXY.y() <= 0.0)
      {
        Message::SendFail() << "Syntax error: wrong size '" << theArgVec[anArgIter + 1] << " "
                            << theArgVec[anArgIter + 2] << "'";
        return 1;
      }
      anArgIter += 2;
    }
    else if (anArg == "r" || anArg == "rect" || anArg == "rectangular")
    {
      aType = Aspect_GT_Rectangular;
    }
    else if (anArg == "c" || anArg == "circ" || anArg == "circular")
    {
      aType = Aspect_GT_Circular;
    }
    else if (anArg == "l" || anArg == "line" || anArg == "lines")
    {
      aMode = Aspect_GDM_Lines;
    }
    else if (anArg == "p" || anArg == "point" || anArg == "points")
    {
      aMode = Aspect_GDM_Points;
    }
    else if (anArgIter + 1 >= theArgNb && anArg == "off")
    {
      aViewer->DeactivateGrid();
      return 0;
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  if (aType == Aspect_GT_Rectangular)
  {
    Graphic3d_Vec2d anOrigXY, aStepXY;
    Standard_Real   aRotAngle = 0.0;
    aViewer->RectangularGridValues(anOrigXY.x(), anOrigXY.y(), aStepXY.x(), aStepXY.y(), aRotAngle);
    if (hasOrigin)
    {
      anOrigXY = aNewOriginXY;
    }
    if (hasStep)
    {
      aStepXY = aNewStepXY;
    }
    if (hasRotAngle)
    {
      aRotAngle = aNewRotAngle;
    }
    aViewer->SetRectangularGridValues(anOrigXY.x(),
                                      anOrigXY.y(),
                                      aStepXY.x(),
                                      aStepXY.y(),
                                      aRotAngle);
    if (hasSize || hasZOffset)
    {
      Graphic3d_Vec3d aSize;
      aViewer->RectangularGridGraphicValues(aSize.x(), aSize.y(), aSize.z());
      if (hasSize)
      {
        aSize.x() = aNewSizeXY.x();
        aSize.y() = aNewSizeXY.y();
      }
      if (hasZOffset)
      {
        aSize.z() = aNewZOffset;
      }
      aViewer->SetRectangularGridGraphicValues(aSize.x(), aSize.y(), aSize.z());
    }
  }
  else if (aType == Aspect_GT_Circular)
  {
    Graphic3d_Vec2d  anOrigXY;
    Standard_Real    aRadiusStep;
    Standard_Integer aDivisionNumber;
    Standard_Real    aRotAngle = 0.0;
    aViewer->CircularGridValues(anOrigXY.x(),
                                anOrigXY.y(),
                                aRadiusStep,
                                aDivisionNumber,
                                aRotAngle);
    if (hasOrigin)
    {
      anOrigXY = aNewOriginXY;
    }
    if (hasStep)
    {
      aRadiusStep     = aNewStepXY[0];
      aDivisionNumber = (int)aNewStepXY[1];
      if (aDivisionNumber < 1)
      {
        Message::SendFail() << "Syntax error: invalid division number '" << aNewStepXY[1] << "'";
        return 1;
      }
    }
    if (hasRotAngle)
    {
      aRotAngle = aNewRotAngle;
    }

    aViewer->SetCircularGridValues(anOrigXY.x(),
                                   anOrigXY.y(),
                                   aRadiusStep,
                                   aDivisionNumber,
                                   aRotAngle);
    if (hasSize || hasZOffset)
    {
      Standard_Real aRadius = 0.0, aZOffset = 0.0;
      aViewer->CircularGridGraphicValues(aRadius, aZOffset);
      if (hasSize)
      {
        aRadius = aNewSizeXY.x();
        if (aNewSizeXY.y() != 0.0)
        {
          Message::SendFail("Syntax error: circular size should be specified as radius");
          return 1;
        }
      }
      if (hasZOffset)
      {
        aZOffset = aNewZOffset;
      }
      aViewer->SetCircularGridGraphicValues(aRadius, aZOffset);
    }
  }
  aViewer->ActivateGrid(aType, aMode);
  return 0;
}

//=================================================================================================

static int VPriviledgedPlane(Draw_Interpretor& theDI,
                             Standard_Integer  theArgNb,
                             const char**      theArgVec)
{
  if (theArgNb != 1 && theArgNb != 7 && theArgNb != 10)
  {
    Message::SendFail("Error: wrong number of arguments! See usage:");
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }

  // get the active viewer
  Handle(V3d_Viewer) aViewer = ViewerTest::GetViewerFromContext();
  if (aViewer.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  if (theArgNb == 1)
  {
    gp_Ax3        aPriviledgedPlane = aViewer->PrivilegedPlane();
    const gp_Pnt& anOrig            = aPriviledgedPlane.Location();
    const gp_Dir& aNorm             = aPriviledgedPlane.Direction();
    const gp_Dir& aXDir             = aPriviledgedPlane.XDirection();
    theDI << "Origin: " << anOrig.X() << " " << anOrig.Y() << " " << anOrig.Z() << " "
          << "Normal: " << aNorm.X() << " " << aNorm.Y() << " " << aNorm.Z() << " "
          << "X-dir: " << aXDir.X() << " " << aXDir.Y() << " " << aXDir.Z() << "\n";
    return 0;
  }

  Standard_Integer anArgIdx = 1;
  Standard_Real    anOrigX  = Draw::Atof(theArgVec[anArgIdx++]);
  Standard_Real    anOrigY  = Draw::Atof(theArgVec[anArgIdx++]);
  Standard_Real    anOrigZ  = Draw::Atof(theArgVec[anArgIdx++]);
  Standard_Real    aNormX   = Draw::Atof(theArgVec[anArgIdx++]);
  Standard_Real    aNormY   = Draw::Atof(theArgVec[anArgIdx++]);
  Standard_Real    aNormZ   = Draw::Atof(theArgVec[anArgIdx++]);

  gp_Ax3 aPriviledgedPlane;
  gp_Pnt anOrig(anOrigX, anOrigY, anOrigZ);
  gp_Dir aNorm(aNormX, aNormY, aNormZ);
  if (theArgNb > 7)
  {
    Standard_Real aXDirX = Draw::Atof(theArgVec[anArgIdx++]);
    Standard_Real aXDirY = Draw::Atof(theArgVec[anArgIdx++]);
    Standard_Real aXDirZ = Draw::Atof(theArgVec[anArgIdx++]);
    gp_Dir        aXDir(aXDirX, aXDirY, aXDirZ);
    aPriviledgedPlane = gp_Ax3(anOrig, aNorm, aXDir);
  }
  else
  {
    aPriviledgedPlane = gp_Ax3(anOrig, aNorm);
  }

  aViewer->SetPrivilegedPlane(aPriviledgedPlane);

  return 0;
}

//=================================================================================================

static int VConvert(Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  enum
  {
    Model,
    Ray,
    View,
    Window,
    Grid
  } aMode = Model;

  // access coordinate arguments
  TColStd_SequenceOfReal aCoord;
  Standard_Integer       anArgIdx = 1;
  for (; anArgIdx < 4 && anArgIdx < theArgNb; ++anArgIdx)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIdx]);
    if (!anArg.IsRealValue(Standard_True))
    {
      break;
    }
    aCoord.Append(anArg.RealValue());
  }

  // non-numeric argument too early
  if (aCoord.IsEmpty())
  {
    Message::SendFail("Error: wrong number of arguments! See usage:");
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }

  // collect all other arguments and options
  for (; anArgIdx < theArgNb; ++anArgIdx)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIdx]);
    anArg.LowerCase();
    if (anArg == "window")
      aMode = Window;
    else if (anArg == "view")
      aMode = View;
    else if (anArg == "grid")
      aMode = Grid;
    else if (anArg == "ray")
      aMode = Ray;
    else
    {
      Message::SendFail() << "Error: wrong argument " << anArg << "! See usage:";
      theDI.PrintHelp(theArgVec[0]);
      return 1;
    }
  }

  // complete input checks
  if ((aCoord.Length() == 1 && theArgNb > 3) || (aCoord.Length() == 2 && theArgNb > 4)
      || (aCoord.Length() == 3 && theArgNb > 5))
  {
    Message::SendFail("Error: wrong number of arguments! See usage:");
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }

  Standard_Real    aXYZ[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  Standard_Integer aXYp[2] = {0, 0};

  // convert one-dimensional coordinate
  if (aCoord.Length() == 1)
  {
    switch (aMode)
    {
      case View:
        theDI << "View Vv: " << aView->Convert((Standard_Integer)aCoord(1));
        return 0;
      case Window:
        theDI << "Window Vp: " << aView->Convert(aCoord(1));
        return 0;
      default:
        Message::SendFail("Error: wrong arguments! See usage:");
        theDI.PrintHelp(theArgVec[0]);
        return 1;
    }
  }

  // convert 2D coordinates from projection or view reference space
  if (aCoord.Length() == 2)
  {
    switch (aMode)
    {
      case Model:
        aView->Convert((Standard_Integer)aCoord(1),
                       (Standard_Integer)aCoord(2),
                       aXYZ[0],
                       aXYZ[1],
                       aXYZ[2]);
        theDI << "Model X,Y,Z: " << aXYZ[0] << " " << aXYZ[1] << " " << aXYZ[2] << "\n";
        return 0;

      case View:
        aView->Convert((Standard_Integer)aCoord(1), (Standard_Integer)aCoord(2), aXYZ[0], aXYZ[1]);
        theDI << "View Xv,Yv: " << aXYZ[0] << " " << aXYZ[1] << "\n";
        return 0;

      case Window:
        aView->Convert(aCoord(1), aCoord(2), aXYp[0], aXYp[1]);
        theDI << "Window Xp,Yp: " << aXYp[0] << " " << aXYp[1] << "\n";
        return 0;

      case Grid:
        aView->Convert((Standard_Integer)aCoord(1),
                       (Standard_Integer)aCoord(2),
                       aXYZ[0],
                       aXYZ[1],
                       aXYZ[2]);
        aView->ConvertToGrid(aXYZ[0], aXYZ[1], aXYZ[2], aXYZ[3], aXYZ[4], aXYZ[5]);
        theDI << "Model X,Y,Z: " << aXYZ[3] << " " << aXYZ[4] << " " << aXYZ[5] << "\n";
        return 0;

      case Ray:
        aView->ConvertWithProj((Standard_Integer)aCoord(1),
                               (Standard_Integer)aCoord(2),
                               aXYZ[0],
                               aXYZ[1],
                               aXYZ[2],
                               aXYZ[3],
                               aXYZ[4],
                               aXYZ[5]);
        theDI << "Model DX,DY,DZ: " << aXYZ[3] << " " << aXYZ[4] << " " << aXYZ[5] << "\n";
        return 0;

      default:
        Message::SendFail("Error: wrong arguments! See usage:");
        theDI.PrintHelp(theArgVec[0]);
        return 1;
    }
  }

  // convert 3D coordinates from view reference space
  else if (aCoord.Length() == 3)
  {
    switch (aMode)
    {
      case Window:
        aView->Convert(aCoord(1), aCoord(2), aCoord(3), aXYp[0], aXYp[1]);
        theDI << "Window Xp,Yp: " << aXYp[0] << " " << aXYp[1] << "\n";
        return 0;

      case Grid:
        aView->ConvertToGrid(aCoord(1), aCoord(2), aCoord(3), aXYZ[0], aXYZ[1], aXYZ[2]);
        theDI << "Model X,Y,Z: " << aXYZ[0] << " " << aXYZ[1] << " " << aXYZ[2] << "\n";
        return 0;

      default:
        Message::SendFail("Error: wrong arguments! See usage:");
        theDI.PrintHelp(theArgVec[0]);
        return 1;
    }
  }

  return 0;
}

//=================================================================================================

static int VFps(Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Standard_Integer aFramesNb = -1;
  Standard_Real    aDuration = -1.0;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (aDuration < 0.0 && anArgIter + 1 < theArgNb
        && (anArg == "-duration" || anArg == "-dur" || anArg == "-time"))
    {
      aDuration = Draw::Atof(theArgVec[++anArgIter]);
    }
    else if (aFramesNb < 0 && anArg.IsIntegerValue())
    {
      aFramesNb = anArg.IntegerValue();
      if (aFramesNb <= 0)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }
  if (aFramesNb < 0 && aDuration < 0.0)
  {
    aFramesNb = 100;
  }

  // the time is meaningless for first call
  // due to async OpenGl rendering
  aView->Redraw();

  // redraw view in loop to estimate average values
  OSD_Timer aTimer;
  aTimer.Start();
  Standard_Integer aFrameIter = 1;
  for (;; ++aFrameIter)
  {
    aView->Redraw();
    if ((aFramesNb > 0 && aFrameIter >= aFramesNb)
        || (aDuration > 0.0 && aTimer.ElapsedTime() >= aDuration))
    {
      break;
    }
  }
  aTimer.Stop();
  Standard_Real       aCpu;
  const Standard_Real aTime = aTimer.ElapsedTime();
  aTimer.OSD_Chronometer::Show(aCpu);

  const Standard_Real aFpsAver = Standard_Real(aFrameIter) / aTime;
  const Standard_Real aCpuAver = aCpu / Standard_Real(aFrameIter);

  // return statistics
  theDI << "FPS: " << aFpsAver << "\n"
        << "CPU: " << (1000.0 * aCpuAver) << " msec\n";

  // compute additional statistics in ray-tracing mode
  const Graphic3d_RenderingParams& aParams = aView->RenderingParams();
  if (aParams.Method == Graphic3d_RM_RAYTRACING)
  {
    Graphic3d_Vec2i aWinSize(0, 0);
    aView->Window()->Size(aWinSize.x(), aWinSize.y());

    // 1 shadow ray and 1 secondary ray pew each bounce
    const Standard_Real aMRays =
      aWinSize.x() * aWinSize.y() * aFpsAver * aParams.RaytracingDepth * 2 / 1.0e6f;
    theDI << "MRays/sec (upper bound): " << aMRays << "\n";
  }

  return 0;
}

//=================================================================================================

static int VMemGpu(Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  // get the context
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Handle(Graphic3d_GraphicDriver) aDriver = aContextAIS->CurrentViewer()->Driver();
  if (aDriver.IsNull())
  {
    Message::SendFail("Error: graphic driver not available");
    return 1;
  }

  Standard_Size           aFreeBytes = 0;
  TCollection_AsciiString anInfo;
  if (!aDriver->MemoryInfo(aFreeBytes, anInfo))
  {
    Message::SendFail("Error: information not available");
    return 1;
  }

  if (theArgNb > 1 && *theArgVec[1] == 'f')
  {
    theDI << Standard_Real(aFreeBytes);
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
static int VReadPixel(Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }
  else if (theArgNb < 3)
  {
    Message::SendFail() << "Syntax error: wrong number of arguments.\n"
                        << "Usage: " << theArgVec[0]
                        << " xPixel yPixel [{rgb|rgba|depth|hls|rgbf|rgbaf}=rgba] [name]";
    return 1;
  }

  Image_Format         aFormat     = Image_Format_RGBA;
  Graphic3d_BufferType aBufferType = Graphic3d_BT_RGBA;

  Standard_Integer aWidth, aHeight;
  aView->Window()->Size(aWidth, aHeight);
  const Standard_Integer anX = Draw::Atoi(theArgVec[1]);
  const Standard_Integer anY = Draw::Atoi(theArgVec[2]);
  if (anX < 0 || anX >= aWidth || anY < 0 || anY > aHeight)
  {
    Message::SendFail() << "Error: pixel coordinates (" << anX << "; " << anY
                        << ") are out of view (" << aWidth << " x " << aHeight << ")";
    return 1;
  }

  bool toShowName = false, toShowHls = false, toShowHex = false, toShow_sRGB = false;
  for (Standard_Integer anIter = 3; anIter < theArgNb; ++anIter)
  {
    TCollection_AsciiString aParam(theArgVec[anIter]);
    aParam.LowerCase();
    if (aParam == "-rgb" || aParam == "rgb" || aParam == "-srgb" || aParam == "srgb")
    {
      aFormat     = Image_Format_RGB;
      aBufferType = Graphic3d_BT_RGB;
      toShow_sRGB = aParam == "-srgb" || aParam == "srgb";
    }
    else if (aParam == "-hls" || aParam == "hls")
    {
      aFormat     = Image_Format_RGB;
      aBufferType = Graphic3d_BT_RGB;
      toShowHls   = Standard_True;
    }
    else if (aParam == "-rgbf" || aParam == "rgbf")
    {
      aFormat     = Image_Format_RGBF;
      aBufferType = Graphic3d_BT_RGB;
    }
    else if (aParam == "-rgba" || aParam == "rgba" || aParam == "-srgba" || aParam == "srgba")
    {
      aFormat     = Image_Format_RGBA;
      aBufferType = Graphic3d_BT_RGBA;
      toShow_sRGB = aParam == "-srgba" || aParam == "srgba";
    }
    else if (aParam == "-rgbaf" || aParam == "rgbaf")
    {
      aFormat     = Image_Format_RGBAF;
      aBufferType = Graphic3d_BT_RGBA;
    }
    else if (aParam == "-depth" || aParam == "depth")
    {
      aFormat     = Image_Format_GrayF;
      aBufferType = Graphic3d_BT_Depth;
    }
    else if (aParam == "-name" || aParam == "name")
    {
      toShowName = Standard_True;
    }
    else if (aParam == "-hex" || aParam == "hex")
    {
      toShowHex = Standard_True;
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << aParam << "'";
      return 1;
    }
  }

  Image_PixMap anImage;
  if (!anImage.InitTrash(aFormat, aWidth, aHeight))
  {
    Message::SendFail("Error: image allocation failed");
    return 1;
  }
  else if (!aView->ToPixMap(anImage, aWidth, aHeight, aBufferType))
  {
    Message::SendFail("Error: image dump failed");
    return 1;
  }

  // redirect possible warning messages that could have been added by ToPixMap
  // into the Tcl interpretor (via DefaultMessenger) to cout, so that they do not
  // contaminate result of the command
  Standard_CString aWarnLog = theDI.Result();
  if (aWarnLog != NULL && aWarnLog[0] != '\0')
  {
    std::cout << aWarnLog << std::endl;
  }
  theDI.Reset();

  Quantity_ColorRGBA aColor = anImage.PixelColor(anX, anY, true);
  if (toShowName)
  {
    if (aBufferType == Graphic3d_BT_RGBA)
    {
      theDI << Quantity_Color::StringName(aColor.GetRGB().Name()) << " " << aColor.Alpha();
    }
    else
    {
      theDI << Quantity_Color::StringName(aColor.GetRGB().Name());
    }
  }
  else if (toShowHex)
  {
    if (aBufferType == Graphic3d_BT_RGBA)
    {
      theDI << Quantity_ColorRGBA::ColorToHex(aColor);
    }
    else
    {
      theDI << Quantity_Color::ColorToHex(aColor.GetRGB());
    }
  }
  else
  {
    switch (aBufferType)
    {
      default:
      case Graphic3d_BT_RGB: {
        if (toShowHls)
        {
          theDI << aColor.GetRGB().Hue() << " " << aColor.GetRGB().Light() << " "
                << aColor.GetRGB().Saturation();
        }
        else if (toShow_sRGB)
        {
          const Graphic3d_Vec4 aColor_sRGB =
            Quantity_ColorRGBA::Convert_LinearRGB_To_sRGB((Graphic3d_Vec4)aColor);
          theDI << aColor_sRGB.r() << " " << aColor_sRGB.g() << " " << aColor_sRGB.b();
        }
        else
        {
          theDI << aColor.GetRGB().Red() << " " << aColor.GetRGB().Green() << " "
                << aColor.GetRGB().Blue();
        }
        break;
      }
      case Graphic3d_BT_RGBA: {
        const Graphic3d_Vec4 aVec4 =
          toShow_sRGB ? Quantity_ColorRGBA::Convert_LinearRGB_To_sRGB((Graphic3d_Vec4)aColor)
                      : (Graphic3d_Vec4)aColor;
        theDI << aVec4.r() << " " << aVec4.g() << " " << aVec4.b() << " " << aVec4.a();
        break;
      }
      case Graphic3d_BT_Depth: {
        theDI << aColor.GetRGB().Red();
        break;
      }
    }
  }

  return 0;
}

//! Auxiliary presentation for an image plane.
class ViewerTest_ImagePrs : public AIS_InteractiveObject
{
public:
  //! Main constructor.
  ViewerTest_ImagePrs(const Handle(Image_PixMap)&    theImage,
                      const Standard_Real            theWidth,
                      const Standard_Real            theHeight,
                      const TCollection_AsciiString& theLabel)
      : myLabel(theLabel),
        myWidth(theWidth),
        myHeight(theHeight)
  {
    SetDisplayMode(0);
    SetHilightMode(1);
    myDynHilightDrawer->SetZLayer(Graphic3d_ZLayerId_Topmost);
    {
      myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());
      const Handle(Graphic3d_AspectFillArea3d)& aFillAspect = myDrawer->ShadingAspect()->Aspect();
      Graphic3d_MaterialAspect                  aMat;
      aMat.SetMaterialType(Graphic3d_MATERIAL_PHYSIC);
      aMat.SetAmbientColor(Quantity_NOC_BLACK);
      aMat.SetDiffuseColor(Quantity_NOC_WHITE);
      aMat.SetSpecularColor(Quantity_NOC_BLACK);
      aMat.SetEmissiveColor(Quantity_NOC_BLACK);
      aFillAspect->SetFrontMaterial(aMat);
      aFillAspect->SetTextureMap(new Graphic3d_Texture2D(theImage));
      aFillAspect->SetTextureMapOn();
    }
    {
      Handle(Prs3d_TextAspect) aTextAspect = new Prs3d_TextAspect();
      aTextAspect->SetHorizontalJustification(Graphic3d_HTA_CENTER);
      aTextAspect->SetVerticalJustification(Graphic3d_VTA_CENTER);
      myDrawer->SetTextAspect(aTextAspect);
    }
    {
      const gp_Dir aNorm(0.0, 0.0, 1.0);
      myTris = new Graphic3d_ArrayOfTriangles(4, 6, true, false, true);
      myTris->AddVertex(gp_Pnt(-myWidth * 0.5, -myHeight * 0.5, 0.0), aNorm, gp_Pnt2d(0.0, 0.0));
      myTris->AddVertex(gp_Pnt(myWidth * 0.5, -myHeight * 0.5, 0.0), aNorm, gp_Pnt2d(1.0, 0.0));
      myTris->AddVertex(gp_Pnt(-myWidth * 0.5, myHeight * 0.5, 0.0), aNorm, gp_Pnt2d(0.0, 1.0));
      myTris->AddVertex(gp_Pnt(myWidth * 0.5, myHeight * 0.5, 0.0), aNorm, gp_Pnt2d(1.0, 1.0));
      myTris->AddEdge(1);
      myTris->AddEdge(2);
      myTris->AddEdge(3);
      myTris->AddEdge(3);
      myTris->AddEdge(2);
      myTris->AddEdge(4);

      myRect = new Graphic3d_ArrayOfPolylines(4);
      myRect->AddVertex(myTris->Vertice(1));
      myRect->AddVertex(myTris->Vertice(3));
      myRect->AddVertex(myTris->Vertice(4));
      myRect->AddVertex(myTris->Vertice(2));
    }
  }

  //! Returns TRUE for accepted display modes.
  virtual Standard_Boolean AcceptDisplayMode(const Standard_Integer theMode) const Standard_OVERRIDE
  {
    return theMode == 0 || theMode == 1;
  }

  //! Compute presentation.
  virtual void Compute(const Handle(PrsMgr_PresentationManager)&,
                       const Handle(Prs3d_Presentation)& thePrs,
                       const Standard_Integer            theMode) Standard_OVERRIDE
  {
    switch (theMode)
    {
      case 0: {
        Handle(Graphic3d_Group) aGroup = thePrs->NewGroup();
        aGroup->AddPrimitiveArray(myTris);
        aGroup->SetGroupPrimitivesAspect(myDrawer->ShadingAspect()->Aspect());
        aGroup->AddPrimitiveArray(myRect);
        aGroup->SetGroupPrimitivesAspect(myDrawer->LineAspect()->Aspect());
        return;
      }
      case 1: {
        Prs3d_Text::Draw(thePrs->NewGroup(),
                         myDrawer->TextAspect(),
                         myLabel,
                         gp_Pnt(0.0, 0.0, 0.0));
        Handle(Graphic3d_Group) aGroup = thePrs->NewGroup();
        aGroup->AddPrimitiveArray(myRect);
        aGroup->SetGroupPrimitivesAspect(myDrawer->LineAspect()->Aspect());
        return;
      }
    }
  }

  //! Compute selection.
  virtual void ComputeSelection(const Handle(SelectMgr_Selection)& theSel,
                                const Standard_Integer             theMode) Standard_OVERRIDE
  {
    if (theMode == 0)
    {
      Handle(SelectMgr_EntityOwner)            anEntityOwner = new SelectMgr_EntityOwner(this, 5);
      Handle(Select3D_SensitivePrimitiveArray) aSensitive =
        new Select3D_SensitivePrimitiveArray(anEntityOwner);
      aSensitive->InitTriangulation(myTris->Attributes(), myTris->Indices(), TopLoc_Location());
      theSel->Add(aSensitive);
    }
  }

private:
  Handle(Graphic3d_ArrayOfTriangles) myTris;
  Handle(Graphic3d_ArrayOfPolylines) myRect;
  TCollection_AsciiString            myLabel;
  Standard_Real                      myWidth;
  Standard_Real                      myHeight;
};

//==============================================================================
// function : VDiffImage
// purpose  : The draw-command compares two images.
//==============================================================================

static int VDiffImage(Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb < 3)
  {
    Message::SendFail("Syntax error: not enough arguments");
    return 1;
  }

  Standard_Integer        anArgIter = 1;
  TCollection_AsciiString anImgPathRef(theArgVec[anArgIter++]);
  TCollection_AsciiString anImgPathNew(theArgVec[anArgIter++]);
  TCollection_AsciiString aDiffImagePath;
  Standard_Real           aTolColor        = -1.0;
  Standard_Integer        toBlackWhite     = -1;
  Standard_Integer        isBorderFilterOn = -1;
  Standard_Boolean        isOldSyntax      = Standard_False;
  TCollection_AsciiString aViewName, aPrsNameRef, aPrsNameNew, aPrsNameDiff;
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArgIter + 1 < theArgNb
        && (anArg == "-toleranceofcolor" || anArg == "-tolerancecolor" || anArg == "-tolerance"
            || anArg == "-toler"))
    {
      aTolColor = Atof(theArgVec[++anArgIter]);
      if (aTolColor < 0.0 || aTolColor > 1.0)
      {
        Message::SendFail() << "Syntax error at '" << anArg << " " << theArgVec[anArgIter] << "'";
        return 1;
      }
    }
    else if (anArg == "-blackwhite")
    {
      Standard_Boolean toEnable = Standard_True;
      if (anArgIter + 1 < theArgNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      toBlackWhite = toEnable ? 1 : 0;
    }
    else if (anArg == "-borderfilter")
    {
      Standard_Boolean toEnable = Standard_True;
      if (anArgIter + 1 < theArgNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      isBorderFilterOn = toEnable ? 1 : 0;
    }
    else if (anArg == "-exitonclose")
    {
      ViewerTest_EventManager::ToExitOnCloseView() = true;
      if (anArgIter + 1 < theArgNb
          && Draw::ParseOnOff(theArgVec[anArgIter + 1],
                              ViewerTest_EventManager::ToExitOnCloseView()))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-closeonescape" || anArg == "-closeonesc")
    {
      ViewerTest_EventManager::ToCloseViewOnEscape() = true;
      if (anArgIter + 1 < theArgNb
          && Draw::ParseOnOff(theArgVec[anArgIter + 1],
                              ViewerTest_EventManager::ToCloseViewOnEscape()))
      {
        ++anArgIter;
      }
    }
    else if (anArgIter + 3 < theArgNb && anArg == "-display")
    {
      aViewName   = theArgVec[++anArgIter];
      aPrsNameRef = theArgVec[++anArgIter];
      aPrsNameNew = theArgVec[++anArgIter];
      if (anArgIter + 1 < theArgNb && *theArgVec[anArgIter + 1] != '-')
      {
        aPrsNameDiff = theArgVec[++anArgIter];
      }
    }
    else if (aTolColor < 0.0 && anArg.IsRealValue(Standard_True))
    {
      isOldSyntax = Standard_True;
      aTolColor   = anArg.RealValue();
      if (aTolColor < 0.0 || aTolColor > 1.0)
      {
        Message::SendFail() << "Syntax error at '" << anArg << " " << theArgVec[anArgIter] << "'";
        return 1;
      }
    }
    else if (isOldSyntax && toBlackWhite == -1 && (anArg == "0" || anArg == "1"))
    {
      toBlackWhite = anArg == "1" ? 1 : 0;
    }
    else if (isOldSyntax && isBorderFilterOn == -1 && (anArg == "0" || anArg == "1"))
    {
      isBorderFilterOn = anArg == "1" ? 1 : 0;
    }
    else if (aDiffImagePath.IsEmpty())
    {
      aDiffImagePath = theArgVec[anArgIter];
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  Handle(Image_AlienPixMap) anImgRef = new Image_AlienPixMap();
  Handle(Image_AlienPixMap) anImgNew = new Image_AlienPixMap();
  if (!anImgRef->Load(anImgPathRef))
  {
    Message::SendFail() << "Error: image file '" << anImgPathRef << "' cannot be read";
    return 1;
  }
  if (!anImgNew->Load(anImgPathNew))
  {
    Message::SendFail() << "Error: image file '" << anImgPathNew << "' cannot be read";
    return 1;
  }

  // compare the images
  Image_Diff       aComparer;
  Standard_Integer aDiffColorsNb = -1;
  if (aComparer.Init(anImgRef, anImgNew, toBlackWhite == 1))
  {
    aComparer.SetColorTolerance(aTolColor >= 0.0 ? aTolColor : 0.0);
    aComparer.SetBorderFilterOn(isBorderFilterOn == 1);
    aDiffColorsNb = aComparer.Compare();
    theDI << aDiffColorsNb << "\n";
  }

  // save image of difference
  Handle(Image_AlienPixMap) aDiff;
  if (aDiffColorsNb > 0 && (!aDiffImagePath.IsEmpty() || !aPrsNameDiff.IsEmpty()))
  {
    aDiff = new Image_AlienPixMap();
    if (!aDiff->InitTrash(Image_Format_Gray, anImgRef->SizeX(), anImgRef->SizeY()))
    {
      Message::SendFail() << "Error: cannot allocate memory for diff image " << anImgRef->SizeX()
                          << "x" << anImgRef->SizeY();
      return 1;
    }
    aComparer.SaveDiffImage(*aDiff);
    if (!aDiffImagePath.IsEmpty() && !aDiff->Save(aDiffImagePath))
    {
      Message::SendFail() << "Error: diff image file '" << aDiffImagePath << "' cannot be written";
      return 1;
    }
  }

  if (aViewName.IsEmpty())
  {
    return 0;
  }

  ViewerTest_Names aViewNames(aViewName);
  if (ViewerTest_myViews.IsBound1(aViewNames.GetViewName()))
  {
    TCollection_AsciiString aCommand =
      TCollection_AsciiString("vclose ") + aViewNames.GetViewName();
    theDI.Eval(aCommand.ToCString());
  }

  ViewerTest_VinitParams aParams;
  aParams.ViewName = aViewName;
  aParams.Size.x() = float(anImgRef->SizeX() * 2);
  aParams.Size.y() = !aDiff.IsNull() && !aPrsNameDiff.IsEmpty() ? float(anImgRef->SizeY() * 2)
                                                                : float(anImgRef->SizeY());
  TCollection_AsciiString aViewId = ViewerTest::ViewerInit(aParams);

  Standard_Real aRatio = anImgRef->Ratio();
  Standard_Real aSizeX = 1.0;
  Standard_Real aSizeY = aSizeX / aRatio;
  {
    OSD_Path                aPath(anImgPathRef);
    TCollection_AsciiString aLabelRef;
    if (!aPath.Name().IsEmpty())
    {
      aLabelRef = aPath.Name() + aPath.Extension();
    }
    aLabelRef +=
      TCollection_AsciiString() + "\n" + int(anImgRef->SizeX()) + "x" + int(anImgRef->SizeY());

    Handle(ViewerTest_ImagePrs) anImgRefPrs =
      new ViewerTest_ImagePrs(anImgRef, aSizeX, aSizeY, aLabelRef);
    gp_Trsf aTrsfRef;
    aTrsfRef.SetTranslationPart(gp_Vec(-aSizeX * 0.5, 0.0, 0.0));
    anImgRefPrs->SetLocalTransformation(aTrsfRef);
    ViewerTest::Display(aPrsNameRef, anImgRefPrs, false, true);
  }
  {
    OSD_Path                aPath(anImgPathNew);
    TCollection_AsciiString aLabelNew;
    if (!aPath.Name().IsEmpty())
    {
      aLabelNew = aPath.Name() + aPath.Extension();
    }
    aLabelNew +=
      TCollection_AsciiString() + "\n" + int(anImgNew->SizeX()) + "x" + int(anImgNew->SizeY());

    Handle(ViewerTest_ImagePrs) anImgNewPrs =
      new ViewerTest_ImagePrs(anImgNew, aSizeX, aSizeY, aLabelNew);
    gp_Trsf aTrsfRef;
    aTrsfRef.SetTranslationPart(gp_Vec(aSizeX * 0.5, 0.0, 0.0));
    anImgNewPrs->SetLocalTransformation(aTrsfRef);
    ViewerTest::Display(aPrsNameNew, anImgNewPrs, false, true);
  }
  Handle(ViewerTest_ImagePrs) anImgDiffPrs;
  if (!aDiff.IsNull())
  {
    anImgDiffPrs = new ViewerTest_ImagePrs(aDiff,
                                           aSizeX,
                                           aSizeY,
                                           TCollection_AsciiString()
                                             + "Difference: " + aDiffColorsNb + " pixels");
    gp_Trsf aTrsfDiff;
    aTrsfDiff.SetTranslationPart(gp_Vec(0.0, -aSizeY, 0.0));
    anImgDiffPrs->SetLocalTransformation(aTrsfDiff);
  }
  if (!aPrsNameDiff.IsEmpty())
  {
    ViewerTest::Display(aPrsNameDiff, anImgDiffPrs, false, true);
  }
  ViewerTest::CurrentView()->SetProj(V3d_Zpos);
  ViewerTest::CurrentView()->FitAll();
  return 0;
}

//=======================================================================
// function : VSelect
// purpose  : Emulates different types of selection by mouse:
//           1) single click selection
//           2) selection with rectangle having corners at pixel positions (x1,y1) and (x2,y2)
//           3) selection with polygon having corners at
//           pixel positions (x1,y1),...,(xn,yn)
//           4) any of these selections with shift button pressed
//=======================================================================
static Standard_Integer VSelect(Draw_Interpretor&,
                                Standard_Integer theNbArgs,
                                const char**     theArgVec)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  NCollection_Sequence<Graphic3d_Vec2i> aPnts;
  bool                                  toAllowOverlap = false;
  AIS_SelectionScheme                   aSelScheme     = AIS_SelectionScheme_Replace;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-allowoverlap")
    {
      toAllowOverlap = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], toAllowOverlap))
      {
        ++anArgIter;
      }
    }
    else if (anArg == "-replace")
    {
      aSelScheme = AIS_SelectionScheme_Replace;
    }
    else if (anArg == "-replaceextra")
    {
      aSelScheme = AIS_SelectionScheme_ReplaceExtra;
    }
    else if (anArg == "-xor" || anArg == "-shift")
    {
      aSelScheme = AIS_SelectionScheme_XOR;
    }
    else if (anArg == "-add")
    {
      aSelScheme = AIS_SelectionScheme_Add;
    }
    else if (anArg == "-remove")
    {
      aSelScheme = AIS_SelectionScheme_Remove;
    }
    else if (anArgIter + 1 < theNbArgs && anArg.IsIntegerValue()
             && TCollection_AsciiString(theArgVec[anArgIter + 1]).IsIntegerValue())
    {
      const TCollection_AsciiString anArgNext(theArgVec[++anArgIter]);
      aPnts.Append(Graphic3d_Vec2i(anArg.IntegerValue(), anArgNext.IntegerValue()));
    }
    else if (anArgIter + 1 == theNbArgs && anArg.IsIntegerValue())
    {
      if (anArg.IntegerValue() == 1)
      {
        aSelScheme = AIS_SelectionScheme_XOR;
      }
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  if (toAllowOverlap)
  {
    aCtx->MainSelector()->AllowOverlapDetection(toAllowOverlap);
  }

  Handle(ViewerTest_EventManager) aCurrentEventManager = ViewerTest::CurrentEventManager();
  if (aPnts.IsEmpty())
  {
    aCtx->SelectDetected(aSelScheme);
    aCtx->CurrentViewer()->Invalidate();
  }
  else if (aPnts.Length() == 2)
  {
    if (toAllowOverlap && aPnts.First().y() < aPnts.Last().y())
    {
      std::swap(aPnts.ChangeFirst(), aPnts.ChangeLast());
    }
    else if (!toAllowOverlap && aPnts.First().y() > aPnts.Last().y())
    {
      std::swap(aPnts.ChangeFirst(), aPnts.ChangeLast());
    }

    aCurrentEventManager->SelectInViewer(aPnts, aSelScheme);
  }
  else
  {
    aCurrentEventManager->SelectInViewer(aPnts, aSelScheme);
  }
  aCurrentEventManager->FlushViewEvents(aCtx, ViewerTest::CurrentView(), true);
  return 0;
}

//=======================================================================
// function : VSelectPriority
// purpose  : Prints or sets the selection priority for an object
//=======================================================================
static Standard_Integer VSelectPriority(Draw_Interpretor& theDI,
                                        Standard_Integer  theNbArgs,
                                        const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  ViewerTest_AutoUpdater         anUpdateTool(aContext, ViewerTest::CurrentView());
  if (aContext.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  TCollection_AsciiString aLastArg(theArgVec[theNbArgs - 1]);
  Standard_Integer        aPriority = -1;
  Standard_Integer        aNbArgs   = theNbArgs;
  if (aNbArgs < 2 || aNbArgs > 3)
  {
    Message::SendFail("Syntax error: wrong number of arguments! See usage:");
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }

  if (aLastArg.IsIntegerValue())
  {
    TCollection_AsciiString aFocusArg(theArgVec[1]);
    aPriority = aLastArg.IntegerValue();
    --aNbArgs;
    if (aPriority < 0)
    {
      Message::SendFail() << "Syntax error: the specified selection priority value '" << aLastArg
                          << "' is invalid ]";
      return 1;
    }
  }
  else
  {
    anUpdateTool.Invalidate();
  }

  for (Standard_Integer anArgIter = 1; anArgIter < aNbArgs; ++anArgIter)
  {
    TCollection_AsciiString       aName(theArgVec[anArgIter]);
    Handle(AIS_InteractiveObject) anIObj;
    GetMapOfAIS().Find2(aName, anIObj);
    if (anIObj.IsNull())
    {
      Message::SendFail() << "Error: the object '" << theArgVec[1] << "' is not displayed";
      return 1;
    }

    Handle(SelectMgr_EntityOwner) anOwner = anIObj->GlobalSelOwner();
    if (!anOwner.IsNull())
    {
      if (aPriority == Graphic3d_DisplayPriority_INVALID)
      {
        theDI << anOwner->Priority() << " ";
      }
      else
      {
        anOwner->SetPriority(aPriority);
      }
    }
    else
    {
      Message::SendFail() << "Error: the object '" << theArgVec[1]
                          << "' is does not have a selection priority attached.";
      return 1;
    }
  }
  return 0;
}

//=======================================================================
// function : VMoveTo
// purpose  : Emulates cursor movement to defined pixel position
//=======================================================================
static Standard_Integer VMoveTo(Draw_Interpretor& theDI,
                                Standard_Integer  theNbArgs,
                                const char**      theArgVec)
{
  const Handle(AIS_InteractiveContext)& aContext = ViewerTest::GetAISContext();
  const Handle(V3d_View)&               aView    = ViewerTest::CurrentView();
  if (aContext.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Graphic3d_Vec2i aMousePos(IntegerLast(), IntegerLast());
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArgStr(theArgVec[anArgIter]);
    anArgStr.LowerCase();
    if (anArgStr == "-reset" || anArgStr == "-clear")
    {
      if (anArgIter + 1 < theNbArgs)
      {
        Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter + 1] << "'";
        return 1;
      }

      const Standard_Boolean toEchoGrid =
        aContext->CurrentViewer()->IsGridActive() && aContext->CurrentViewer()->GridEcho();
      if (toEchoGrid)
      {
        aContext->CurrentViewer()->HideGridEcho(aView);
      }
      if (aContext->ClearDetected() || toEchoGrid)
      {
        aContext->CurrentViewer()->RedrawImmediate();
      }
      return 0;
    }
    else if (aMousePos.x() == IntegerLast() && anArgStr.IsIntegerValue())
    {
      aMousePos.x() = anArgStr.IntegerValue();
    }
    else if (aMousePos.y() == IntegerLast() && anArgStr.IsIntegerValue())
    {
      aMousePos.y() = anArgStr.IntegerValue();
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (aMousePos.x() == IntegerLast() || aMousePos.y() == IntegerLast())
  {
    Message::SendFail("Syntax error: wrong number of arguments");
    return 1;
  }

  ViewerTest::CurrentEventManager()->ResetPreviousMoveTo();
  ViewerTest::CurrentEventManager()->UpdateMousePosition(aMousePos,
                                                         Aspect_VKeyMouse_NONE,
                                                         Aspect_VKeyFlags_NONE,
                                                         false);
  ViewerTest::CurrentEventManager()->FlushViewEvents(ViewerTest::GetAISContext(), aView, true);

  gp_Pnt                               aTopPnt(RealLast(), RealLast(), RealLast());
  const Handle(SelectMgr_EntityOwner)& aDetOwner = aContext->DetectedOwner();
  for (Standard_Integer aDetIter = 1; aDetIter <= aContext->MainSelector()->NbPicked(); ++aDetIter)
  {
    if (aContext->MainSelector()->Picked(aDetIter) == aDetOwner)
    {
      aTopPnt = aContext->MainSelector()->PickedPoint(aDetIter);
      break;
    }
  }
  theDI << aTopPnt.X() << " " << aTopPnt.Y() << " " << aTopPnt.Z();
  return 0;
}

//=================================================================================================

static Standard_Integer VSelectByAxis(Draw_Interpretor& theDI,
                                      Standard_Integer  theNbArgs,
                                      const char**      theArgVec)
{
  const Handle(AIS_InteractiveContext)& aContext = ViewerTest::GetAISContext();
  const Handle(V3d_View)&               aView    = ViewerTest::CurrentView();
  if (aContext.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  TCollection_AsciiString aName;
  gp_XYZ                  anAxisLocation(RealLast(), RealLast(), RealLast());
  gp_XYZ                  anAxisDirection(RealLast(), RealLast(), RealLast());
  Standard_Boolean        isOnlyTop    = true;
  Standard_Boolean        toShowNormal = false;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArgStr(theArgVec[anArgIter]);
    anArgStr.LowerCase();
    if (anArgStr == "-display")
    {
      if (anArgIter + 1 >= theNbArgs)
      {
        Message::SendFail() << "Syntax error at argument '" << anArgStr << "'";
        return 1;
      }
      aName = theArgVec[++anArgIter];
    }
    else if (anArgStr == "-onlytop")
    {
      isOnlyTop = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], isOnlyTop))
      {
        ++anArgIter;
      }
    }
    else if (anArgStr == "-shownormal")
    {
      toShowNormal = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], toShowNormal))
      {
        ++anArgIter;
      }
    }
    else if (Precision::IsInfinite(anAxisLocation.X()) && anArgStr.IsRealValue())
    {
      anAxisLocation.SetX(anArgStr.RealValue());
    }
    else if (Precision::IsInfinite(anAxisLocation.Y()) && anArgStr.IsRealValue())
    {
      anAxisLocation.SetY(anArgStr.RealValue());
    }
    else if (Precision::IsInfinite(anAxisLocation.Z()) && anArgStr.IsRealValue())
    {
      anAxisLocation.SetZ(anArgStr.RealValue());
    }
    else if (Precision::IsInfinite(anAxisDirection.X()) && anArgStr.IsRealValue())
    {
      anAxisDirection.SetX(anArgStr.RealValue());
    }
    else if (Precision::IsInfinite(anAxisDirection.Y()) && anArgStr.IsRealValue())
    {
      anAxisDirection.SetY(anArgStr.RealValue());
    }
    else if (Precision::IsInfinite(anAxisDirection.Z()) && anArgStr.IsRealValue())
    {
      anAxisDirection.SetZ(anArgStr.RealValue());
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (Precision::IsInfinite(anAxisLocation.X()) || Precision::IsInfinite(anAxisLocation.Y())
      || Precision::IsInfinite(anAxisLocation.Z()) || Precision::IsInfinite(anAxisDirection.X())
      || Precision::IsInfinite(anAxisDirection.Y()) || Precision::IsInfinite(anAxisDirection.Z()))
  {
    Message::SendFail() << "Invalid axis location and direction";
    return 1;
  }

  gp_Ax1 anAxis(anAxisLocation, anAxisDirection);
  gp_Pnt aTopPnt;
  if (!ViewerTest::CurrentEventManager()->PickAxis(aTopPnt, aContext, aView, anAxis))
  {
    theDI << "There are no any intersections with this axis.";
    return 0;
  }
  NCollection_Sequence<gp_Pnt>         aPoints;
  NCollection_Sequence<Graphic3d_Vec3> aNormals;
  NCollection_Sequence<Standard_Real>  aNormalLengths;
  for (Standard_Integer aPickIter = 1; aPickIter <= aContext->MainSelector()->NbPicked();
       ++aPickIter)
  {
    const SelectMgr_SortCriterion& aPickedData = aContext->MainSelector()->PickedData(aPickIter);
    aPoints.Append(aPickedData.Point);
    aNormals.Append(aPickedData.Normal);
    Standard_Real aNormalLength = 1.0;
    if (!aPickedData.Entity.IsNull())
    {
      aNormalLength = 0.2 * aPickedData.Entity->BoundingBox().Size().maxComp();
    }
    aNormalLengths.Append(aNormalLength);
  }
  if (!aName.IsEmpty())
  {
    Standard_Boolean wasAuto = aContext->GetAutoActivateSelection();
    aContext->SetAutoActivateSelection(false);

    // Display axis
    Quantity_Color              anAxisColor = Quantity_NOC_GREEN;
    Handle(Geom_Axis2Placement) anAx2Axis =
      new Geom_Axis2Placement(gp_Ax2(anAxisLocation, anAxisDirection));
    Handle(AIS_Axis) anAISAxis = new AIS_Axis(gp_Ax1(anAxisLocation, anAxisDirection));
    anAISAxis->SetColor(anAxisColor);
    ViewerTest::Display(TCollection_AsciiString(aName) + "_axis", anAISAxis, false);

    // Display axis start point
    Handle(AIS_Point) anAISStartPnt = new AIS_Point(new Geom_CartesianPoint(anAxisLocation));
    anAISStartPnt->SetMarker(Aspect_TOM_O);
    anAISStartPnt->SetColor(anAxisColor);
    ViewerTest::Display(TCollection_AsciiString(aName) + "_start", anAISStartPnt, false);

    Standard_Integer anIndex = 0;
    for (NCollection_Sequence<gp_Pnt>::Iterator aPntIter(aPoints); aPntIter.More();
         aPntIter.Next(), anIndex++)
    {
      const gp_Pnt& aPoint = aPntIter.Value();

      // Display normals in intersection points
      if (toShowNormal)
      {
        const Graphic3d_Vec3& aNormal       = aNormals.Value(anIndex + 1);
        Standard_Real         aNormalLength = aNormalLengths.Value(anIndex + 1);
        if (aNormal.SquareModulus() > ShortRealEpsilon())
        {
          gp_Dir           aNormalDir((Standard_Real)aNormal.x(),
                            (Standard_Real)aNormal.y(),
                            (Standard_Real)aNormal.z());
          Handle(AIS_Axis) anAISNormal = new AIS_Axis(gp_Ax1(aPoint, aNormalDir), aNormalLength);
          anAISNormal->SetColor(Quantity_NOC_BLUE);
          anAISNormal->SetInfiniteState(false);
          ViewerTest::Display(TCollection_AsciiString(aName) + "_normal_" + anIndex,
                              anAISNormal,
                              false);
        }
      }

      // Display intersection points
      Handle(Geom_CartesianPoint) anIntersectPnt      = new Geom_CartesianPoint(aPoint);
      Handle(AIS_Point)           anAISIntersectPoint = new AIS_Point(anIntersectPnt);
      anAISIntersectPoint->SetMarker(Aspect_TOM_PLUS);
      anAISIntersectPoint->SetColor(Quantity_NOC_RED);
      ViewerTest::Display(TCollection_AsciiString(aName) + "_intersect_" + anIndex,
                          anAISIntersectPoint,
                          true);
    }

    aContext->SetAutoActivateSelection(wasAuto);
  }

  Standard_Integer anIndex = 0;
  for (NCollection_Sequence<gp_Pnt>::Iterator anIter(aPoints); anIter.More();
       anIter.Next(), anIndex++)
  {
    const gp_Pnt& aPnt = anIter.Value();
    theDI << aPnt.X() << " " << aPnt.Y() << " " << aPnt.Z() << "\n";
  }
  return 0;
}

namespace
{
//! Global map storing all animations registered in ViewerTest.
static NCollection_DataMap<TCollection_AsciiString, Handle(AIS_Animation)>
  ViewerTest_AnimationTimelineMap;

//! The animation calling the Draw Harness command.
class ViewerTest_AnimationProc : public AIS_Animation
{
  DEFINE_STANDARD_RTTI_INLINE(ViewerTest_AnimationProc, AIS_Animation)
public:
  //! Main constructor.
  ViewerTest_AnimationProc(const TCollection_AsciiString& theAnimationName,
                           Draw_Interpretor*              theDI,
                           const TCollection_AsciiString& theCommand)
      : AIS_Animation(theAnimationName),
        myDrawInter(theDI),
        myCommand(theCommand)
  {
    //
  }

protected:
  //! Evaluate the command.
  virtual void update(const AIS_AnimationProgress& theProgress) Standard_OVERRIDE
  {
    TCollection_AsciiString aCmd = myCommand;
    replace(aCmd, "%pts", TCollection_AsciiString(theProgress.Pts));
    replace(aCmd, "%localpts", TCollection_AsciiString(theProgress.LocalPts));
    replace(aCmd, "%ptslocal", TCollection_AsciiString(theProgress.LocalPts));
    replace(aCmd, "%normalized", TCollection_AsciiString(theProgress.LocalNormalized));
    replace(aCmd, "%localnormalized", TCollection_AsciiString(theProgress.LocalNormalized));
    myDrawInter->Eval(aCmd.ToCString());
  }

  //! Find the keyword in the command and replace it with value.
  //! @return the position of the keyword to pass value
  void replace(TCollection_AsciiString&       theCmd,
               const TCollection_AsciiString& theKey,
               const TCollection_AsciiString& theVal)
  {
    TCollection_AsciiString aCmd(theCmd);
    aCmd.LowerCase();
    const Standard_Integer aPos = aCmd.Search(theKey);
    if (aPos == -1)
    {
      return;
    }

    TCollection_AsciiString aPart1, aPart2;
    Standard_Integer        aPart1To = aPos - 1;
    if (aPart1To >= 1 && aPart1To <= theCmd.Length())
    {
      aPart1 = theCmd.SubString(1, aPart1To);
    }

    Standard_Integer aPart2From = aPos + theKey.Length();
    if (aPart2From >= 1 && aPart2From <= theCmd.Length())
    {
      aPart2 = theCmd.SubString(aPart2From, theCmd.Length());
    }

    theCmd = aPart1 + theVal + aPart2;
  }

protected:
  Draw_Interpretor*       myDrawInter;
  TCollection_AsciiString myCommand;
};

//! Auxiliary animation holder.
class ViewerTest_AnimationHolder : public AIS_AnimationCamera
{
  DEFINE_STANDARD_RTTI_INLINE(ViewerTest_AnimationHolder, AIS_AnimationCamera)
public:
  ViewerTest_AnimationHolder(const Handle(AIS_Animation)& theAnim,
                             const Handle(V3d_View)&      theView,
                             const Standard_Boolean       theIsFreeView)
      : AIS_AnimationCamera("ViewerTest_AnimationHolder", Handle(V3d_View)())
  {
    if (theAnim->Timer().IsNull())
    {
      theAnim->SetTimer(new Media_Timer());
    }
    myTimer = theAnim->Timer();
    myView  = theView;
    if (theIsFreeView)
    {
      myCamStart = new Graphic3d_Camera(theView->Camera());
    }
    Add(theAnim);
  }

  //! Start playback.
  virtual void StartTimer(const Standard_Real    theStartPts,
                          const Standard_Real    thePlaySpeed,
                          const Standard_Boolean theToUpdate,
                          const Standard_Boolean theToStopTimer) Standard_OVERRIDE
  {
    base_type::StartTimer(theStartPts, thePlaySpeed, theToUpdate, theToStopTimer);
    if (theToStopTimer)
    {
      abortPlayback();
    }
  }

  //! Pause animation.
  virtual void Pause() Standard_OVERRIDE
  {
    myState = AnimationState_Paused;
    // default implementation would stop all children,
    // but we want to keep wrapped animation paused
    myAnimations.First()->Pause();
    abortPlayback();
  }

  //! Stop animation.
  virtual void Stop() Standard_OVERRIDE
  {
    base_type::Stop();
    abortPlayback();
  }

  //! Process one step of the animation according to the input time progress, including all
  //! children.
  virtual void updateWithChildren(const AIS_AnimationProgress& thePosition) Standard_OVERRIDE
  {
    Handle(V3d_View) aView = myView;
    if (!aView.IsNull() && !myCamStart.IsNull())
    {
      myCamStart->Copy(aView->Camera());
    }
    base_type::updateWithChildren(thePosition);
    if (!aView.IsNull() && !myCamStart.IsNull())
    {
      aView->Camera()->Copy(myCamStart);
    }
  }

private:
  void abortPlayback()
  {
    if (!myView.IsNull())
    {
      myView.Nullify();
    }
  }
};

//! Replace the animation with the new one.
static void replaceAnimation(const Handle(AIS_Animation)& theParentAnimation,
                             Handle(AIS_Animation)&       theAnimation,
                             const Handle(AIS_Animation)& theAnimationNew)
{
  theAnimationNew->CopyFrom(theAnimation);
  if (!theParentAnimation.IsNull())
  {
    theParentAnimation->Replace(theAnimation, theAnimationNew);
  }
  else
  {
    ViewerTest_AnimationTimelineMap.UnBind(theAnimationNew->Name());
    ViewerTest_AnimationTimelineMap.Bind(theAnimationNew->Name(), theAnimationNew);
  }
  theAnimation = theAnimationNew;
}

//! Parse the point.
static Standard_Boolean parseXYZ(const char** theArgVec, gp_XYZ& thePnt)
{
  const TCollection_AsciiString anXYZ[3] = {theArgVec[0], theArgVec[1], theArgVec[2]};
  if (!anXYZ[0].IsRealValue(Standard_True) || !anXYZ[1].IsRealValue(Standard_True)
      || !anXYZ[2].IsRealValue(Standard_True))
  {
    return Standard_False;
  }

  thePnt.SetCoord(anXYZ[0].RealValue(), anXYZ[1].RealValue(), anXYZ[2].RealValue());
  return Standard_True;
}

//! Parse the quaternion.
static Standard_Boolean parseQuaternion(const char** theArgVec, gp_Quaternion& theQRot)
{
  const TCollection_AsciiString anXYZW[4] = {theArgVec[0],
                                             theArgVec[1],
                                             theArgVec[2],
                                             theArgVec[3]};
  if (!anXYZW[0].IsRealValue(Standard_True) || !anXYZW[1].IsRealValue(Standard_True)
      || !anXYZW[2].IsRealValue(Standard_True) || !anXYZW[3].IsRealValue(Standard_True))
  {
    return Standard_False;
  }

  theQRot.Set(anXYZW[0].RealValue(),
              anXYZW[1].RealValue(),
              anXYZW[2].RealValue(),
              anXYZW[3].RealValue());
  return Standard_True;
}

//! Auxiliary class for flipping image upside-down.
class ImageFlipper
{
public:
  //! Empty constructor.
  ImageFlipper()
      : myTmp(NCollection_BaseAllocator::CommonBaseAllocator())
  {
  }

  //! Perform flipping.
  Standard_Boolean FlipY(Image_PixMap& theImage)
  {
    if (theImage.IsEmpty() || theImage.SizeX() == 0 || theImage.SizeY() == 0)
    {
      return Standard_False;
    }

    const Standard_Size aRowSize = theImage.SizeRowBytes();
    if (myTmp.Size() < aRowSize && !myTmp.Allocate(aRowSize))
    {
      return Standard_False;
    }

    // for odd height middle row should be left as is
    Standard_Size aNbRowsHalf = theImage.SizeY() / 2;
    for (Standard_Size aRowT = 0, aRowB = theImage.SizeY() - 1; aRowT < aNbRowsHalf;
         ++aRowT, --aRowB)
    {
      Standard_Byte* aTop = theImage.ChangeRow(aRowT);
      Standard_Byte* aBot = theImage.ChangeRow(aRowB);
      memcpy(myTmp.ChangeData(), aTop, aRowSize);
      memcpy(aTop, aBot, aRowSize);
      memcpy(aBot, myTmp.Data(), aRowSize);
    }
    return Standard_True;
  }

private:
  NCollection_Buffer myTmp;
};

} // namespace

//=================================================================================================
// function : VViewParams
// purpose  : Gets or sets AIS View characteristics
//=================================================================================================
static int VViewParams(Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Standard_Boolean toSetProj     = Standard_False;
  Standard_Boolean toSetUp       = Standard_False;
  Standard_Boolean toSetAt       = Standard_False;
  Standard_Boolean toSetEye      = Standard_False;
  Standard_Boolean toSetScale    = Standard_False;
  Standard_Boolean toSetSize     = Standard_False;
  Standard_Boolean toSetCenter2d = Standard_False;
  Standard_Real    aViewScale    = aView->Scale();
  Standard_Real    aViewAspect   = aView->Camera()->Aspect();
  Standard_Real    aViewSize     = 1.0;
  Graphic3d_Vec2i  aCenter2d;
  gp_XYZ           aViewProj, aViewUp, aViewAt, aViewEye;
  aView->Proj(aViewProj.ChangeCoord(1), aViewProj.ChangeCoord(2), aViewProj.ChangeCoord(3));
  aView->Up(aViewUp.ChangeCoord(1), aViewUp.ChangeCoord(2), aViewUp.ChangeCoord(3));
  aView->At(aViewAt.ChangeCoord(1), aViewAt.ChangeCoord(2), aViewAt.ChangeCoord(3));
  aView->Eye(aViewEye.ChangeCoord(1), aViewEye.ChangeCoord(2), aViewEye.ChangeCoord(3));
  const Graphic3d_Mat4d& anOrientMat = aView->Camera()->OrientationMatrix();
  const Graphic3d_Mat4d& aProjMat    = aView->Camera()->ProjectionMatrix();
  if (theArgsNb == 1)
  {
    // print all of the available view parameters
    char aText[4096];
    Sprintf(aText,
            "Scale:  %g\n"
            "Aspect: %g\n"
            "Proj:   %12g %12g %12g\n"
            "Up:     %12g %12g %12g\n"
            "At:     %12g %12g %12g\n"
            "Eye:    %12g %12g %12g\n"
            "OrientMat:    %12g %12g %12g %12g\n"
            "              %12g %12g %12g %12g\n"
            "              %12g %12g %12g %12g\n"
            "              %12g %12g %12g %12g\n"
            "ProjMat:      %12g %12g %12g %12g\n"
            "              %12g %12g %12g %12g\n"
            "              %12g %12g %12g %12g\n"
            "              %12g %12g %12g %12g\n",
            aViewScale,
            aViewAspect,
            aViewProj.X(),
            aViewProj.Y(),
            aViewProj.Z(),
            aViewUp.X(),
            aViewUp.Y(),
            aViewUp.Z(),
            aViewAt.X(),
            aViewAt.Y(),
            aViewAt.Z(),
            aViewEye.X(),
            aViewEye.Y(),
            aViewEye.Z(),
            anOrientMat.GetValue(0, 0),
            anOrientMat.GetValue(0, 1),
            anOrientMat.GetValue(0, 2),
            anOrientMat.GetValue(0, 3),
            anOrientMat.GetValue(1, 0),
            anOrientMat.GetValue(1, 1),
            anOrientMat.GetValue(1, 2),
            anOrientMat.GetValue(1, 3),
            anOrientMat.GetValue(2, 0),
            anOrientMat.GetValue(2, 1),
            anOrientMat.GetValue(2, 2),
            anOrientMat.GetValue(2, 3),
            anOrientMat.GetValue(3, 0),
            anOrientMat.GetValue(3, 1),
            anOrientMat.GetValue(3, 2),
            anOrientMat.GetValue(3, 3),
            aProjMat.GetValue(0, 0),
            aProjMat.GetValue(0, 1),
            aProjMat.GetValue(0, 2),
            aProjMat.GetValue(0, 3),
            aProjMat.GetValue(1, 0),
            aProjMat.GetValue(1, 1),
            aProjMat.GetValue(1, 2),
            aProjMat.GetValue(1, 3),
            aProjMat.GetValue(2, 0),
            aProjMat.GetValue(2, 1),
            aProjMat.GetValue(2, 2),
            aProjMat.GetValue(2, 3),
            aProjMat.GetValue(3, 0),
            aProjMat.GetValue(3, 1),
            aProjMat.GetValue(3, 2),
            aProjMat.GetValue(3, 3));
    theDi << aText;
    return 0;
  }

  ViewerTest_AutoUpdater anUpdateTool(ViewerTest::GetAISContext(), aView);
  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode(anArg))
    {
      continue;
    }
    else if (anArg == "-cmd" || anArg == "-command" || anArg == "-args")
    {
      char aText[4096];
      Sprintf(aText,
              "-scale %g "
              "-proj %g %g %g "
              "-up %g %g %g "
              "-at %g %g %g\n",
              aViewScale,
              aViewProj.X(),
              aViewProj.Y(),
              aViewProj.Z(),
              aViewUp.X(),
              aViewUp.Y(),
              aViewUp.Z(),
              aViewAt.X(),
              aViewAt.Y(),
              aViewAt.Z());
      theDi << aText;
    }
    else if (anArg == "-scale" || anArg == "-size")
    {
      if (anArgIter + 1 < theArgsNb && *theArgVec[anArgIter + 1] != '-')
      {
        const TCollection_AsciiString aValueArg(theArgVec[anArgIter + 1]);
        if (aValueArg.IsRealValue(Standard_True))
        {
          ++anArgIter;
          if (anArg == "-scale")
          {
            toSetScale = Standard_True;
            aViewScale = aValueArg.RealValue();
          }
          else if (anArg == "-size")
          {
            toSetSize = Standard_True;
            aViewSize = aValueArg.RealValue();
          }
          continue;
        }
      }
      if (anArg == "-scale")
      {
        theDi << "Scale: " << aView->Scale() << "\n";
      }
      else if (anArg == "-size")
      {
        Graphic3d_Vec2d aSizeXY;
        aView->Size(aSizeXY.x(), aSizeXY.y());
        theDi << "Size: " << aSizeXY.x() << " " << aSizeXY.y() << "\n";
      }
    }
    else if (anArg == "-eye" || anArg == "-at" || anArg == "-up" || anArg == "-proj")
    {
      if (anArgIter + 3 < theArgsNb)
      {
        gp_XYZ anXYZ;
        if (parseXYZ(theArgVec + anArgIter + 1, anXYZ))
        {
          anArgIter += 3;
          if (anArg == "-eye")
          {
            toSetEye = Standard_True;
            aViewEye = anXYZ;
          }
          else if (anArg == "-at")
          {
            toSetAt = Standard_True;
            aViewAt = anXYZ;
          }
          else if (anArg == "-up")
          {
            toSetUp = Standard_True;
            aViewUp = anXYZ;
          }
          else if (anArg == "-proj")
          {
            toSetProj = Standard_True;
            aViewProj = anXYZ;
          }
          continue;
        }
      }

      if (anArg == "-eye")
      {
        theDi << "Eye:  " << aViewEye.X() << " " << aViewEye.Y() << " " << aViewEye.Z() << "\n";
      }
      else if (anArg == "-at")
      {
        theDi << "At:   " << aViewAt.X() << " " << aViewAt.Y() << " " << aViewAt.Z() << "\n";
      }
      else if (anArg == "-up")
      {
        theDi << "Up:   " << aViewUp.X() << " " << aViewUp.Y() << " " << aViewUp.Z() << "\n";
      }
      else if (anArg == "-proj")
      {
        theDi << "Proj: " << aViewProj.X() << " " << aViewProj.Y() << " " << aViewProj.Z() << "\n";
      }
    }
    else if (anArg == "-center")
    {
      if (anArgIter + 2 < theArgsNb)
      {
        const TCollection_AsciiString anX(theArgVec[anArgIter + 1]);
        const TCollection_AsciiString anY(theArgVec[anArgIter + 2]);
        if (anX.IsIntegerValue() && anY.IsIntegerValue())
        {
          toSetCenter2d = Standard_True;
          aCenter2d     = Graphic3d_Vec2i(anX.IntegerValue(), anY.IntegerValue());
        }
      }
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  // change view parameters in proper order
  if (toSetScale)
  {
    aView->SetScale(aViewScale);
  }
  if (toSetSize)
  {
    aView->SetSize(aViewSize);
  }
  if (toSetEye)
  {
    aView->SetEye(aViewEye.X(), aViewEye.Y(), aViewEye.Z());
  }
  if (toSetAt)
  {
    aView->SetAt(aViewAt.X(), aViewAt.Y(), aViewAt.Z());
  }
  if (toSetProj)
  {
    aView->SetProj(aViewProj.X(), aViewProj.Y(), aViewProj.Z());
  }
  if (toSetUp)
  {
    aView->SetUp(aViewUp.X(), aViewUp.Y(), aViewUp.Z());
  }
  if (toSetCenter2d)
  {
    aView->SetCenter(aCenter2d.x(), aCenter2d.y());
  }

  return 0;
}

//=================================================================================================

static Standard_Integer V2DMode(Draw_Interpretor&,
                                Standard_Integer theArgsNb,
                                const char**     theArgVec)
{
  bool                       is2dMode = true;
  Handle(ViewerTest_V3dView) aV3dView =
    Handle(ViewerTest_V3dView)::DownCast(ViewerTest::CurrentView());
  if (aV3dView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }
  for (Standard_Integer anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    const TCollection_AsciiString anArg     = theArgVec[anArgIt];
    TCollection_AsciiString       anArgCase = anArg;
    anArgCase.LowerCase();
    if (anArgIt + 1 < theArgsNb && anArgCase == "-name")
    {
      ViewerTest_Names               aViewNames(theArgVec[++anArgIt]);
      const TCollection_AsciiString& aViewName = aViewNames.GetViewName();
      if (!ViewerTest_myViews.IsBound1(aViewName))
      {
        Message::SendFail() << "Syntax error: unknown view '" << theArgVec[anArgIt - 1] << "'";
        return 1;
      }
      aV3dView = Handle(ViewerTest_V3dView)::DownCast(ViewerTest_myViews.Find1(aViewName));
    }
    else if (anArgCase == "-mode")
    {
      if (anArgIt + 1 < theArgsNb && Draw::ParseOnOff(theArgVec[anArgIt + 1], is2dMode))
      {
        ++anArgIt;
      }
    }
    else if (Draw::ParseOnOff(theArgVec[anArgIt], is2dMode))
    {
      //
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument " << anArg;
      return 1;
    }
  }

  aV3dView->SetView2DMode(is2dMode);
  return 0;
}

//=================================================================================================

static Standard_Integer VAnimation(Draw_Interpretor& theDI,
                                   Standard_Integer  theArgNb,
                                   const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (theArgNb < 2)
  {
    for (NCollection_DataMap<TCollection_AsciiString, Handle(AIS_Animation)>::Iterator anAnimIter(
           ViewerTest_AnimationTimelineMap);
         anAnimIter.More();
         anAnimIter.Next())
    {
      theDI << anAnimIter.Key() << " " << anAnimIter.Value()->Duration() << " sec\n";
    }
    return 0;
  }
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Standard_Integer        anArgIter = 1;
  TCollection_AsciiString aNameArg(theArgVec[anArgIter++]);
  if (aNameArg.IsEmpty())
  {
    Message::SendFail("Syntax error: animation name is not defined");
    return 1;
  }

  TCollection_AsciiString aNameArgLower = aNameArg;
  aNameArgLower.LowerCase();
  if (aNameArgLower == "-reset" || aNameArgLower == "-clear")
  {
    ViewerTest_AnimationTimelineMap.Clear();
    return 0;
  }
  else if (aNameArg.Value(1) == '-')
  {
    Message::SendFail() << "Syntax error: invalid animation name '" << aNameArg << "'";
    return 1;
  }

  const char*      aNameSplitter = "/";
  Standard_Integer aSplitPos     = aNameArg.Search(aNameSplitter);
  if (aSplitPos == -1)
  {
    aNameSplitter = ".";
    aSplitPos     = aNameArg.Search(aNameSplitter);
  }

  // find existing or create a new animation by specified name within syntax "parent.child".
  Handle(AIS_Animation) aRootAnimation, aParentAnimation, anAnimation;
  for (; !aNameArg.IsEmpty();)
  {
    TCollection_AsciiString aNameParent;
    if (aSplitPos != -1)
    {
      if (aSplitPos == aNameArg.Length())
      {
        Message::SendFail("Syntax error: animation name is not defined");
        return 1;
      }

      aNameParent = aNameArg.SubString(1, aSplitPos - 1);
      aNameArg    = aNameArg.SubString(aSplitPos + 1, aNameArg.Length());

      aSplitPos = aNameArg.Search(aNameSplitter);
    }
    else
    {
      aNameParent = aNameArg;
      aNameArg.Clear();
    }

    if (anAnimation.IsNull())
    {
      if (!ViewerTest_AnimationTimelineMap.Find(aNameParent, anAnimation))
      {
        anAnimation = new AIS_Animation(aNameParent);
        ViewerTest_AnimationTimelineMap.Bind(aNameParent, anAnimation);
      }
      aRootAnimation = anAnimation;
    }
    else
    {
      aParentAnimation = anAnimation;
      anAnimation      = aParentAnimation->Find(aNameParent);
      if (anAnimation.IsNull())
      {
        anAnimation = new AIS_Animation(aNameParent);
        aParentAnimation->Add(anAnimation);
      }
    }
  }
  if (anAnimation.IsNull())
  {
    Message::SendFail() << "Syntax error: wrong number of arguments";
    return 1;
  }

  if (anArgIter >= theArgNb)
  {
    // just print the list of children
    for (NCollection_Sequence<Handle(AIS_Animation)>::Iterator anAnimIter(anAnimation->Children());
         anAnimIter.More();
         anAnimIter.Next())
    {
      theDI << anAnimIter.Value()->Name() << " " << anAnimIter.Value()->Duration() << " sec\n";
    }
    return 0;
  }

  // animation parameters
  Standard_Boolean toPlay             = Standard_False;
  Standard_Real    aPlaySpeed         = 1.0;
  Standard_Real    aPlayStartTime     = anAnimation->StartPts();
  Standard_Real    aPlayDuration      = anAnimation->Duration();
  Standard_Boolean isFreeCamera       = Standard_False;
  Standard_Boolean toPauseOnClick     = Standard_True;
  Standard_Boolean isLockLoop         = Standard_False;
  Standard_Boolean toPrintElapsedTime = Standard_False;

  // video recording parameters
  TCollection_AsciiString aRecFile;
  Image_VideoParams       aRecParams;

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    // general options
    if (anArg == "-reset" || anArg == "-clear")
    {
      anAnimation->Clear();
    }
    else if (anArg == "-remove" || anArg == "-del" || anArg == "-delete")
    {
      if (aParentAnimation.IsNull())
      {
        ViewerTest_AnimationTimelineMap.UnBind(anAnimation->Name());
      }
      else
      {
        aParentAnimation->Remove(anAnimation);
      }
    }
    // playback options
    else if (anArg == "-play")
    {
      toPlay = Standard_True;
      if (++anArgIter < theArgNb)
      {
        if (*theArgVec[anArgIter] == '-')
        {
          --anArgIter;
          continue;
        }
        aPlayStartTime = Draw::Atof(theArgVec[anArgIter]);

        if (++anArgIter < theArgNb)
        {
          if (*theArgVec[anArgIter] == '-')
          {
            --anArgIter;
            continue;
          }
          aPlayDuration = Draw::Atof(theArgVec[anArgIter]);
        }
      }
    }
    else if (anArg == "-elapsedtime" || anArg == "-elapsed")
    {
      toPrintElapsedTime = Standard_True;
    }
    else if (anArg == "-resume")
    {
      toPlay         = Standard_True;
      aPlayStartTime = anAnimation->ElapsedTime();
      if (++anArgIter < theArgNb)
      {
        if (*theArgVec[anArgIter] == '-')
        {
          --anArgIter;
          continue;
        }

        aPlayDuration = Draw::Atof(theArgVec[anArgIter]);
      }
    }
    else if (anArg == "-pause")
    {
      anAnimation->Pause();
    }
    else if (anArg == "-stop")
    {
      anAnimation->Stop();
    }
    else if (anArg == "-playspeed" || anArg == "-speed")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg << "";
        return 1;
      }
      aPlaySpeed = Draw::Atof(theArgVec[anArgIter]);
    }
    else if (anArg == "-lock" || anArg == "-lockloop" || anArg == "-playlockloop")
    {
      isLockLoop = Draw::ParseOnOffIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArg == "-freecamera" || anArg == "-nofreecamera" || anArg == "-playfreecamera"
             || anArg == "-noplayfreecamera" || anArg == "-freelook" || anArg == "-nofreelook")
    {
      isFreeCamera = Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArg == "-pauseonclick" || anArg == "-nopauseonclick" || anArg == "-nopause")
    {
      toPauseOnClick = Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
    }
    // video recodring options
    else if (anArg == "-rec" || anArg == "-record")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      aRecFile = theArgVec[anArgIter];
      if (aRecParams.FpsNum <= 0)
      {
        aRecParams.FpsNum = 24;
      }

      if (anArgIter + 2 < theArgNb && *theArgVec[anArgIter + 1] != '-'
          && *theArgVec[anArgIter + 2] != '-')
      {
        TCollection_AsciiString aWidthArg(theArgVec[anArgIter + 1]);
        TCollection_AsciiString aHeightArg(theArgVec[anArgIter + 2]);
        if (aWidthArg.IsIntegerValue() && aHeightArg.IsIntegerValue())
        {
          aRecParams.Width  = aWidthArg.IntegerValue();
          aRecParams.Height = aHeightArg.IntegerValue();
          anArgIter += 2;
        }
      }
    }
    else if (anArg == "-fps")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      TCollection_AsciiString aFpsArg(theArgVec[anArgIter]);
      Standard_Integer        aSplitIndex = aFpsArg.FirstLocationInSet("/", 1, aFpsArg.Length());
      if (aSplitIndex == 0)
      {
        aRecParams.FpsNum = aFpsArg.IntegerValue();
      }
      else
      {
        const TCollection_AsciiString aDenStr = aFpsArg.Split(aSplitIndex);
        aFpsArg.Split(aFpsArg.Length() - 1);
        const TCollection_AsciiString aNumStr = aFpsArg;
        aRecParams.FpsNum                     = aNumStr.IntegerValue();
        aRecParams.FpsDen                     = aDenStr.IntegerValue();
        if (aRecParams.FpsDen < 1)
        {
          Message::SendFail() << "Syntax error at " << anArg;
          return 1;
        }
      }
    }
    else if (anArg == "-format")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      aRecParams.Format = theArgVec[anArgIter];
    }
    else if (anArg == "-pix_fmt" || anArg == "-pixfmt" || anArg == "-pixelformat")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      aRecParams.PixelFormat = theArgVec[anArgIter];
    }
    else if (anArg == "-codec" || anArg == "-vcodec" || anArg == "-videocodec")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      aRecParams.VideoCodec = theArgVec[anArgIter];
    }
    else if (anArg == "-crf" || anArg == "-preset" || anArg == "-qp")
    {
      const TCollection_AsciiString aParamName = anArg.SubString(2, anArg.Length());
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      aRecParams.VideoCodecParams.Bind(aParamName, theArgVec[anArgIter]);
    }
    // animation definition options
    else if (anArg == "-start" || anArg == "-starttime" || anArg == "-startpts")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      anAnimation->SetStartPts(Draw::Atof(theArgVec[anArgIter]));
      aRootAnimation->UpdateTotalDuration();
    }
    else if (anArg == "-end" || anArg == "-endtime" || anArg == "-endpts")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      anAnimation->SetOwnDuration(Draw::Atof(theArgVec[anArgIter]) - anAnimation->StartPts());
      aRootAnimation->UpdateTotalDuration();
    }
    else if (anArg == "-dur" || anArg == "-duration")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      anAnimation->SetOwnDuration(Draw::Atof(theArgVec[anArgIter]));
      aRootAnimation->UpdateTotalDuration();
    }
    else if (anArg == "-command" || anArg == "-cmd" || anArg == "-invoke" || anArg == "-eval"
             || anArg == "-proc")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      Handle(ViewerTest_AnimationProc) aCmdAnimation =
        new ViewerTest_AnimationProc(anAnimation->Name(), &theDI, theArgVec[anArgIter]);
      replaceAnimation(aParentAnimation, anAnimation, aCmdAnimation);
    }
    else if (anArg == "-objecttrsf" || anArg == "-objectransformation"
             || anArg == "-objtransformation" || anArg == "-objtrsf" || anArg == "-object"
             || anArg == "-obj")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }

      TCollection_AsciiString                         anObjName(theArgVec[anArgIter]);
      const ViewerTest_DoubleMapOfInteractiveAndName& aMapOfAIS = GetMapOfAIS();
      Handle(AIS_InteractiveObject)                   anObject;
      if (!aMapOfAIS.Find2(anObjName, anObject))
      {
        Message::SendFail() << "Syntax error: wrong object name at " << anArg;
        return 1;
      }

      gp_Trsf       aTrsfs[2] = {anObject->LocalTransformation(), anObject->LocalTransformation()};
      gp_Quaternion aRotQuats[2] = {aTrsfs[0].GetRotation(), aTrsfs[1].GetRotation()};
      gp_XYZ        aLocPnts[2]  = {aTrsfs[0].TranslationPart(), aTrsfs[1].TranslationPart()};
      Standard_Real aScales[2]   = {aTrsfs[0].ScaleFactor(), aTrsfs[1].ScaleFactor()};
      Standard_Boolean isTrsfSet = Standard_False;

      gp_Ax1           anAxis;
      Standard_Real    anAngles[2]       = {0.0, 0.0};
      Standard_Boolean isAxisRotationSet = Standard_False;

      Standard_Integer aTrsfArgIter = anArgIter + 1;
      for (; aTrsfArgIter < theArgNb; ++aTrsfArgIter)
      {
        TCollection_AsciiString aTrsfArg(theArgVec[aTrsfArgIter]);
        aTrsfArg.LowerCase();
        const Standard_Integer anIndex = aTrsfArg.EndsWith("1") ? 0 : 1;
        if (aTrsfArg.StartsWith("-rotation") || aTrsfArg.StartsWith("-rot"))
        {
          isTrsfSet = Standard_True;
          if (aTrsfArgIter + 4 >= theArgNb
              || !parseQuaternion(theArgVec + aTrsfArgIter + 1, aRotQuats[anIndex]))
          {
            Message::SendFail() << "Syntax error at " << aTrsfArg;
            return 1;
          }
          aTrsfArgIter += 4;
        }
        else if (aTrsfArg.StartsWith("-location") || aTrsfArg.StartsWith("-loc"))
        {
          isTrsfSet = Standard_True;
          if (aTrsfArgIter + 3 >= theArgNb
              || !parseXYZ(theArgVec + aTrsfArgIter + 1, aLocPnts[anIndex]))
          {
            Message::SendFail() << "Syntax error at " << aTrsfArg;
            return 1;
          }
          aTrsfArgIter += 3;
        }
        else if (aTrsfArg.StartsWith("-scale"))
        {
          isTrsfSet = Standard_True;
          if (++aTrsfArgIter >= theArgNb)
          {
            Message::SendFail() << "Syntax error at " << aTrsfArg;
            return 1;
          }

          const TCollection_AsciiString aScaleStr(theArgVec[aTrsfArgIter]);
          if (!aScaleStr.IsRealValue(Standard_True))
          {
            Message::SendFail() << "Syntax error at " << aTrsfArg;
            return 1;
          }
          aScales[anIndex] = aScaleStr.RealValue();
        }
        else if (aTrsfArg == "-axis")
        {
          isAxisRotationSet = Standard_True;
          gp_XYZ anOrigin, aDirection;
          if (aTrsfArgIter + 6 >= theArgNb || !parseXYZ(theArgVec + aTrsfArgIter + 1, anOrigin)
              || !parseXYZ(theArgVec + aTrsfArgIter + 4, aDirection))
          {
            Message::SendFail() << "Syntax error at " << aTrsfArg;
            return 1;
          }
          anAxis.SetLocation(anOrigin);
          anAxis.SetDirection(aDirection);
          aTrsfArgIter += 6;
        }
        else if (aTrsfArg.StartsWith("-ang"))
        {
          isAxisRotationSet = Standard_True;
          if (++aTrsfArgIter >= theArgNb)
          {
            Message::SendFail() << "Syntax error at " << aTrsfArg;
            return 1;
          }

          const TCollection_AsciiString anAngleStr(theArgVec[aTrsfArgIter]);
          if (!anAngleStr.IsRealValue(Standard_True))
          {
            Message::SendFail() << "Syntax error at " << aTrsfArg;
            return 1;
          }
          anAngles[anIndex] = anAngleStr.RealValue();
        }
        else
        {
          anArgIter = aTrsfArgIter - 1;
          break;
        }
      }
      if (!isTrsfSet && !isAxisRotationSet)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      else if (aTrsfArgIter >= theArgNb)
      {
        anArgIter = theArgNb;
      }
      Handle(AIS_BaseAnimationObject) anObjAnimation;
      if (isTrsfSet)
      {
        aTrsfs[0].SetRotation(aRotQuats[0]);
        aTrsfs[1].SetRotation(aRotQuats[1]);
        aTrsfs[0].SetTranslationPart(aLocPnts[0]);
        aTrsfs[1].SetTranslationPart(aLocPnts[1]);
        aTrsfs[0].SetScaleFactor(aScales[0]);
        aTrsfs[1].SetScaleFactor(aScales[1]);

        anObjAnimation =
          new AIS_AnimationObject(anAnimation->Name(), aCtx, anObject, aTrsfs[0], aTrsfs[1]);
      }
      else
      {
        anObjAnimation = new AIS_AnimationAxisRotation(anAnimation->Name(),
                                                       aCtx,
                                                       anObject,
                                                       anAxis,
                                                       anAngles[0] * (M_PI / 180.0),
                                                       anAngles[1] * (M_PI / 180.0));
      }
      replaceAnimation(aParentAnimation, anAnimation, anObjAnimation);
    }
    else if (anArg == "-viewtrsf" || anArg == "-view")
    {
      Handle(AIS_AnimationCamera) aCamAnimation =
        Handle(AIS_AnimationCamera)::DownCast(anAnimation);
      if (aCamAnimation.IsNull())
      {
        aCamAnimation = new AIS_AnimationCamera(anAnimation->Name(), aView);
        replaceAnimation(aParentAnimation, anAnimation, aCamAnimation);
      }

      Handle(Graphic3d_Camera) aCams[2] = {new Graphic3d_Camera(aCamAnimation->View()->Camera()),
                                           new Graphic3d_Camera(aCamAnimation->View()->Camera())};

      Standard_Boolean isTrsfSet    = Standard_False;
      Standard_Integer aViewArgIter = anArgIter + 1;
      for (; aViewArgIter < theArgNb; ++aViewArgIter)
      {
        TCollection_AsciiString aViewArg(theArgVec[aViewArgIter]);
        aViewArg.LowerCase();
        const Standard_Integer anIndex = aViewArg.EndsWith("1") ? 0 : 1;
        if (aViewArg.StartsWith("-scale"))
        {
          isTrsfSet = Standard_True;
          if (++aViewArgIter >= theArgNb)
          {
            Message::SendFail() << "Syntax error at " << anArg;
            return 1;
          }

          const TCollection_AsciiString aScaleStr(theArgVec[aViewArgIter]);
          if (!aScaleStr.IsRealValue(Standard_True))
          {
            Message::SendFail() << "Syntax error at " << aViewArg;
            return 1;
          }
          Standard_Real aScale = aScaleStr.RealValue();
          aScale               = aCamAnimation->View()->DefaultCamera()->Scale() / aScale;
          aCams[anIndex]->SetScale(aScale);
        }
        else if (aViewArg.StartsWith("-eye") || aViewArg.StartsWith("-center")
                 || aViewArg.StartsWith("-at") || aViewArg.StartsWith("-up"))
        {
          isTrsfSet = Standard_True;
          gp_XYZ anXYZ;
          if (aViewArgIter + 3 >= theArgNb || !parseXYZ(theArgVec + aViewArgIter + 1, anXYZ))
          {
            Message::SendFail() << "Syntax error at " << aViewArg;
            return 1;
          }
          aViewArgIter += 3;

          if (aViewArg.StartsWith("-eye"))
          {
            aCams[anIndex]->SetEye(anXYZ);
          }
          else if (aViewArg.StartsWith("-center") || aViewArg.StartsWith("-at"))
          {
            aCams[anIndex]->SetCenter(anXYZ);
          }
          else if (aViewArg.StartsWith("-up"))
          {
            aCams[anIndex]->SetUp(anXYZ);
          }
        }
        else
        {
          anArgIter = aViewArgIter - 1;
          break;
        }
      }
      if (!isTrsfSet)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      else if (aViewArgIter >= theArgNb)
      {
        anArgIter = theArgNb;
      }

      aCamAnimation->SetCameraStart(aCams[0]);
      aCamAnimation->SetCameraEnd(aCams[1]);
    }
    else
    {
      Message::SendFail() << "Syntax error at " << anArg;
      return 1;
    }
  }

  if (anAnimation.IsNull() || anAnimation->IsStopped())
  {
    ViewerTest::CurrentEventManager()->AbortViewAnimation();
    ViewerTest::CurrentEventManager()->SetObjectsAnimation(Handle(AIS_Animation)());
  }

  if (toPrintElapsedTime)
  {
    theDI << "Elapsed Time: " << anAnimation->ElapsedTime() << " s\n";
  }

  if (!toPlay && aRecFile.IsEmpty())
  {
    return 0;
  }

  // Start animation timeline and process frame updating.
  if (aRecParams.FpsNum <= 0 && !isLockLoop)
  {
    Handle(ViewerTest_AnimationHolder) aHolder =
      new ViewerTest_AnimationHolder(anAnimation, aView, isFreeCamera);
    aHolder->StartTimer(aPlayStartTime, aPlaySpeed, Standard_True, aPlayDuration <= 0.0);
    ViewerTest::CurrentEventManager()->SetPauseObjectsAnimation(toPauseOnClick);
    ViewerTest::CurrentEventManager()->SetObjectsAnimation(aHolder);
    ViewerTest::CurrentEventManager()->ProcessExpose();
    return 0;
  }

  // Perform video recording
  const Standard_Boolean wasImmediateUpdate = aView->SetImmediateUpdate(Standard_False);
  const Standard_Real    anUpperPts         = aPlayStartTime + aPlayDuration;
  anAnimation->StartTimer(aPlayStartTime, aPlaySpeed, Standard_True, aPlayDuration <= 0.0);

  OSD_Timer aPerfTimer;
  aPerfTimer.Start();

  Handle(Image_VideoRecorder)    aRecorder;
  ImageFlipper                   aFlipper;
  Handle(Draw_ProgressIndicator) aProgress;
  if (!aRecFile.IsEmpty())
  {
    if (aRecParams.Width <= 0 || aRecParams.Height <= 0)
    {
      aView->Window()->Size(aRecParams.Width, aRecParams.Height);
    }

    aRecorder = new Image_VideoRecorder();
    if (!aRecorder->Open(aRecFile.ToCString(), aRecParams))
    {
      Message::SendFail("Error: failed to open video file for recording");
      return 0;
    }

    aProgress = new Draw_ProgressIndicator(theDI, 1);
  }

  // Manage frame-rated animation here
  Standard_Real         aPts      = aPlayStartTime;
  int64_t               aNbFrames = 0;
  Message_ProgressScope aPS(Message_ProgressIndicator::Start(aProgress),
                            "Video recording, sec",
                            Max(1, Standard_Integer(aPlayDuration / aPlaySpeed)));
  Standard_Integer      aSecondsProgress = 0;
  for (; aPts <= anUpperPts && aPS.More();)
  {
    Standard_Real aRecPts = 0.0;
    if (aRecParams.FpsNum > 0)
    {
      aRecPts = aPlaySpeed
                * ((Standard_Real(aRecParams.FpsDen) / Standard_Real(aRecParams.FpsNum))
                   * Standard_Real(aNbFrames));
    }
    else
    {
      aRecPts = aPlaySpeed * aPerfTimer.ElapsedTime();
    }

    aPts = aPlayStartTime + aRecPts;
    ++aNbFrames;
    if (!anAnimation->Update(aPts))
    {
      break;
    }

    if (!aRecorder.IsNull())
    {
      V3d_ImageDumpOptions aDumpParams;
      aDumpParams.Width          = aRecParams.Width;
      aDumpParams.Height         = aRecParams.Height;
      aDumpParams.BufferType     = Graphic3d_BT_RGBA;
      aDumpParams.StereoOptions  = V3d_SDO_MONO;
      aDumpParams.ToAdjustAspect = Standard_True;
      if (!aView->ToPixMap(aRecorder->ChangeFrame(), aDumpParams))
      {
        Message::SendFail("Error: view dump is failed");
        return 0;
      }
      aFlipper.FlipY(aRecorder->ChangeFrame());
      if (!aRecorder->PushFrame())
      {
        return 0;
      }
    }
    else
    {
      aView->Redraw();
    }

    while (aSecondsProgress < Standard_Integer(aRecPts / aPlaySpeed))
    {
      aPS.Next();
      ++aSecondsProgress;
    }
  }

  aPerfTimer.Stop();
  anAnimation->Stop();
  const Standard_Real aRecFps = Standard_Real(aNbFrames) / aPerfTimer.ElapsedTime();
  theDI << "Average FPS: " << aRecFps << "\n"
        << "Nb. Frames: " << Standard_Real(aNbFrames);

  aView->Redraw();
  aView->SetImmediateUpdate(wasImmediateUpdate);
  return 0;
}

//=======================================================================
// function : VChangeSelected
// purpose  : Adds the shape to selection or remove one from it
//=======================================================================
static Standard_Integer VChangeSelected(Draw_Interpretor& di,
                                        Standard_Integer  argc,
                                        const char**      argv)
{
  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " shape \n";
    return 1;
  }
  // get AIS_Shape:
  TCollection_AsciiString       aName(argv[1]);
  Handle(AIS_InteractiveObject) anAISObject;
  if (!GetMapOfAIS().Find2(aName, anAISObject) || anAISObject.IsNull())
  {
    di << "Use 'vdisplay' before";
    return 1;
  }

  ViewerTest::GetAISContext()->AddOrRemoveSelected(anAISObject, Standard_True);
  return 0;
}

//=======================================================================
// function : VNbSelected
// purpose  : Returns number of selected objects
//=======================================================================
static Standard_Integer VNbSelected(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 1)
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
  di << aContext->NbSelected() << "\n";
  return 0;
}

//=================================================================================================

static Standard_Integer VSetViewSize(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " Size\n";
    return 1;
  }
  Standard_Real aSize = Draw::Atof(argv[1]);
  if (aSize <= 0.)
  {
    di << "Bad Size value  : " << aSize << "\n";
    return 1;
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->SetSize(aSize);
  return 0;
}

//=================================================================================================

static Standard_Integer VMoveView(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if (argc < 4 || argc > 5)
  {
    di << "Usage : " << argv[0] << " Dx Dy Dz [Start = 1|0]\n";
    return 1;
  }
  Standard_Real    Dx     = Draw::Atof(argv[1]);
  Standard_Real    Dy     = Draw::Atof(argv[2]);
  Standard_Real    Dz     = Draw::Atof(argv[3]);
  Standard_Boolean aStart = Standard_True;
  if (argc == 5)
  {
    aStart = (Draw::Atoi(argv[4]) > 0);
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->Move(Dx, Dy, Dz, aStart);
  return 0;
}

//=================================================================================================

static Standard_Integer VTranslateView(Draw_Interpretor& di,
                                       Standard_Integer  argc,
                                       const char**      argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if (argc < 4 || argc > 5)
  {
    di << "Usage : " << argv[0] << " Dx Dy Dz [Start = 1|0]\n";
    return 1;
  }
  Standard_Real    Dx     = Draw::Atof(argv[1]);
  Standard_Real    Dy     = Draw::Atof(argv[2]);
  Standard_Real    Dz     = Draw::Atof(argv[3]);
  Standard_Boolean aStart = Standard_True;
  if (argc == 5)
  {
    aStart = (Draw::Atoi(argv[4]) > 0);
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->Translate(Dx, Dy, Dz, aStart);
  return 0;
}

//=================================================================================================

static Standard_Integer VTurnView(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if (argc < 4 || argc > 5)
  {
    di << "Usage : " << argv[0] << " Ax Ay Az [Start = 1|0]\n";
    return 1;
  }
  Standard_Real    Ax     = Draw::Atof(argv[1]);
  Standard_Real    Ay     = Draw::Atof(argv[2]);
  Standard_Real    Az     = Draw::Atof(argv[3]);
  Standard_Boolean aStart = Standard_True;
  if (argc == 5)
  {
    aStart = (Draw::Atoi(argv[4]) > 0);
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  aView->Turn(Ax, Ay, Az, aStart);
  return 0;
}

//==============================================================================
// function : VTextureEnv
// purpose  : ENables or disables environment mapping
//==============================================================================
class OCC_TextureEnv : public Graphic3d_TextureEnv
{
public:
  OCC_TextureEnv(const Standard_CString FileName);
  OCC_TextureEnv(const Graphic3d_NameOfTextureEnv aName);
  void SetTextureParameters(const Standard_Boolean              theRepeatFlag,
                            const Standard_Boolean              theModulateFlag,
                            const Graphic3d_TypeOfTextureFilter theFilter,
                            const Standard_ShortReal            theXScale,
                            const Standard_ShortReal            theYScale,
                            const Standard_ShortReal            theXShift,
                            const Standard_ShortReal            theYShift,
                            const Standard_ShortReal            theAngle);
  DEFINE_STANDARD_RTTI_INLINE(OCC_TextureEnv, Graphic3d_TextureEnv)
};
DEFINE_STANDARD_HANDLE(OCC_TextureEnv, Graphic3d_TextureEnv)

OCC_TextureEnv::OCC_TextureEnv(const Standard_CString theFileName)
    : Graphic3d_TextureEnv(theFileName)
{
}

OCC_TextureEnv::OCC_TextureEnv(const Graphic3d_NameOfTextureEnv theTexId)
    : Graphic3d_TextureEnv(theTexId)
{
}

void OCC_TextureEnv::SetTextureParameters(const Standard_Boolean              theRepeatFlag,
                                          const Standard_Boolean              theModulateFlag,
                                          const Graphic3d_TypeOfTextureFilter theFilter,
                                          const Standard_ShortReal            theXScale,
                                          const Standard_ShortReal            theYScale,
                                          const Standard_ShortReal            theXShift,
                                          const Standard_ShortReal            theYShift,
                                          const Standard_ShortReal            theAngle)
{
  myParams->SetRepeat(theRepeatFlag);
  myParams->SetModulate(theModulateFlag);
  myParams->SetFilter(theFilter);
  myParams->SetScale(Graphic3d_Vec2(theXScale, theYScale));
  myParams->SetTranslation(Graphic3d_Vec2(theXShift, theYShift));
  myParams->SetRotation(theAngle);
}

static int VTextureEnv(Draw_Interpretor& /*theDI*/,
                       Standard_Integer theArgNb,
                       const char**     theArgVec)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
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
      isOk = (!aTextureOpt.IsIntegerValue()
              || (aTextureOpt.IntegerValue() >= 0
                  && aTextureOpt.IntegerValue() < Graphic3d_NOT_ENV_UNKNOWN));

      if (isOk && theArgNb == 11)
      {
        TCollection_AsciiString aRepeatOpt(theArgVec[3]), aModulateOpt(theArgVec[4]),
          aFilterOpt(theArgVec[5]), aSScaleOpt(theArgVec[6]), aTScaleOpt(theArgVec[7]),
          aSTransOpt(theArgVec[8]), aTTransOpt(theArgVec[9]), anAngleOpt(theArgVec[10]);
        isOk = ((aRepeatOpt.IsEqual("repeat") || aRepeatOpt.IsEqual("clamp"))
                && (aModulateOpt.IsEqual("modulate") || aModulateOpt.IsEqual("decal"))
                && (aFilterOpt.IsEqual("nearest") || aFilterOpt.IsEqual("bilinear")
                    || aFilterOpt.IsEqual("trilinear"))
                && aSScaleOpt.IsRealValue(Standard_True) && aTScaleOpt.IsRealValue(Standard_True)
                && aSTransOpt.IsRealValue(Standard_True) && aTTransOpt.IsRealValue(Standard_True)
                && anAngleOpt.IsRealValue(Standard_True));
      }
    }
  }

  if (!isOk)
  {
    Message::SendFail() << "Usage:\n"
                        << theArgVec[0] << " off\n"
                        << theArgVec[0]
                        << " on {index_of_std_texture(0..7)|texture_file_name} [{clamp|repeat} "
                           "{decal|modulate} {nearest|bilinear|trilinear} scale_s scale_t "
                           "translation_s translation_t rotation_degrees]";
    return 1;
  }

  if (anEnableFlag)
  {
    TCollection_AsciiString aTextureOpt(theArgVec[2]);
    Handle(OCC_TextureEnv)  aTexEnv =
      aTextureOpt.IsIntegerValue()
         ? new OCC_TextureEnv((Graphic3d_NameOfTextureEnv)aTextureOpt.IntegerValue())
         : new OCC_TextureEnv(theArgVec[2]);

    if (theArgNb == 11)
    {
      TCollection_AsciiString aRepeatOpt(theArgVec[3]), aModulateOpt(theArgVec[4]),
        aFilterOpt(theArgVec[5]);
      aTexEnv->SetTextureParameters(aRepeatOpt.IsEqual("repeat"),
                                    aModulateOpt.IsEqual("modulate"),
                                    aFilterOpt.IsEqual("nearest")    ? Graphic3d_TOTF_NEAREST
                                    : aFilterOpt.IsEqual("bilinear") ? Graphic3d_TOTF_BILINEAR
                                                                     : Graphic3d_TOTF_TRILINEAR,
                                    (Standard_ShortReal)Draw::Atof(theArgVec[6]),
                                    (Standard_ShortReal)Draw::Atof(theArgVec[7]),
                                    (Standard_ShortReal)Draw::Atof(theArgVec[8]),
                                    (Standard_ShortReal)Draw::Atof(theArgVec[9]),
                                    (Standard_ShortReal)Draw::Atof(theArgVec[10]));
    }
    aView->SetTextureEnv(aTexEnv);
  }
  else // Disabling environment mapping
  {
    Handle(Graphic3d_TextureEnv) aTexture;
    aView->SetTextureEnv(aTexture); // Passing null handle to clear the texture data
  }

  aView->Redraw();
  return 0;
}

namespace
{
typedef NCollection_DataMap<TCollection_AsciiString, Handle(Graphic3d_ClipPlane)> MapOfPlanes;

//! Remove registered clipping plane from all views and objects.
static void removePlane(MapOfPlanes& theRegPlanes, const TCollection_AsciiString& theName)
{
  Handle(Graphic3d_ClipPlane) aClipPlane;
  if (!theRegPlanes.Find(theName, aClipPlane))
  {
    Message::SendWarning("Warning: no such plane");
    return;
  }

  theRegPlanes.UnBind(theName);
  for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIObjIt(GetMapOfAIS());
       anIObjIt.More();
       anIObjIt.Next())
  {
    const Handle(AIS_InteractiveObject)& aPrs = anIObjIt.Key1();
    aPrs->RemoveClipPlane(aClipPlane);
  }

  for (ViewerTest_ViewerCommandsViewMap::Iterator aViewIt(ViewerTest_myViews); aViewIt.More();
       aViewIt.Next())
  {
    const Handle(V3d_View)& aView = aViewIt.Key2();
    aView->RemoveClipPlane(aClipPlane);
  }

  ViewerTest::RedrawAllViews();
}
} // namespace

//=================================================================================================

static int VClipPlane(Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  // use short-cut for created clip planes map of created (or "registered by name") clip planes
  static MapOfPlanes aRegPlanes;

  if (theArgsNb < 2)
  {
    for (MapOfPlanes::Iterator aPlaneIter(aRegPlanes); aPlaneIter.More(); aPlaneIter.Next())
    {
      theDi << aPlaneIter.Key() << " ";
    }
    return 0;
  }

  TCollection_AsciiString aCommand(theArgVec[1]);
  aCommand.LowerCase();
  const Handle(V3d_View)& anActiveView = ViewerTest::CurrentView();
  if (anActiveView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  // print maximum number of planes for current viewer
  if (aCommand == "-maxplanes" || aCommand == "maxplanes")
  {
    theDi << anActiveView->Viewer()->Driver()->InquirePlaneLimit()
          << " plane slots provided by driver.\n";
    return 0;
  }

  // create / delete plane instance
  if (aCommand == "-create" || aCommand == "create" || aCommand == "-delete" || aCommand == "delete"
      || aCommand == "-clone" || aCommand == "clone")
  {
    if (theArgsNb < 3)
    {
      Message::SendFail("Syntax error: plane name is required");
      return 1;
    }

    Standard_Boolean        toCreate = aCommand == "-create" || aCommand == "create";
    Standard_Boolean        toClone  = aCommand == "-clone" || aCommand == "clone";
    Standard_Boolean        toDelete = aCommand == "-delete" || aCommand == "delete";
    TCollection_AsciiString aPlane(theArgVec[2]);

    if (toCreate)
    {
      if (aRegPlanes.IsBound(aPlane))
      {
        std::cout << "Warning: existing plane has been overridden.\n";
        toDelete = true;
      }
      else
      {
        aRegPlanes.Bind(aPlane, new Graphic3d_ClipPlane());
        return 0;
      }
    }
    else if (toClone) // toClone
    {
      if (!aRegPlanes.IsBound(aPlane))
      {
        Message::SendFail("Error: no such plane");
        return 1;
      }
      else if (theArgsNb < 4)
      {
        Message::SendFail("Syntax error: enter name for new plane");
        return 1;
      }

      TCollection_AsciiString aClone(theArgVec[3]);
      if (aRegPlanes.IsBound(aClone))
      {
        Message::SendFail("Error: plane name is in use");
        return 1;
      }

      const Handle(Graphic3d_ClipPlane)& aClipPlane = aRegPlanes.Find(aPlane);

      aRegPlanes.Bind(aClone, aClipPlane->Clone());
      return 0;
    }

    if (toDelete)
    {
      if (aPlane == "ALL" || aPlane == "all" || aPlane == "*")
      {
        for (MapOfPlanes::Iterator aPlaneIter(aRegPlanes); aPlaneIter.More();)
        {
          aPlane = aPlaneIter.Key();
          removePlane(aRegPlanes, aPlane);
          aPlaneIter = MapOfPlanes::Iterator(aRegPlanes);
        }
      }
      else
      {
        removePlane(aRegPlanes, aPlane);
      }
    }

    if (toCreate)
    {
      aRegPlanes.Bind(aPlane, new Graphic3d_ClipPlane());
    }
    return 0;
  }

  // set / unset plane command
  if (aCommand == "set" || aCommand == "unset")
  {
    if (theArgsNb < 5)
    {
      Message::SendFail("Syntax error: need more arguments");
      return 1;
    }

    // redirect to new syntax
    NCollection_Array1<const char*> anArgVec(1, theArgsNb - 1);
    anArgVec.SetValue(1, theArgVec[0]);
    anArgVec.SetValue(2, theArgVec[2]);
    anArgVec.SetValue(3, aCommand == "set" ? "-set" : "-unset");
    for (Standard_Integer anIt = 4; anIt < theArgsNb; ++anIt)
    {
      anArgVec.SetValue(anIt, theArgVec[anIt]);
    }

    return VClipPlane(theDi, anArgVec.Length(), &anArgVec.ChangeFirst());
  }

  // change plane command
  TCollection_AsciiString     aPlaneName;
  Handle(Graphic3d_ClipPlane) aClipPlane;
  Standard_Integer            anArgIter = 0;
  if (aCommand == "-change" || aCommand == "change")
  {
    // old syntax support
    if (theArgsNb < 3)
    {
      Message::SendFail("Syntax error: need more arguments");
      return 1;
    }

    anArgIter  = 3;
    aPlaneName = theArgVec[2];
    if (!aRegPlanes.Find(aPlaneName, aClipPlane))
    {
      Message::SendFail() << "Error: no such plane '" << aPlaneName << "'";
      return 1;
    }
  }
  else if (aRegPlanes.Find(theArgVec[1], aClipPlane))
  {
    anArgIter  = 2;
    aPlaneName = theArgVec[1];
  }
  else
  {
    anArgIter  = 2;
    aPlaneName = theArgVec[1];
    aClipPlane = new Graphic3d_ClipPlane();
    aRegPlanes.Bind(aPlaneName, aClipPlane);
    theDi << "Created new plane " << aPlaneName << ".\n";
  }

  if (theArgsNb - anArgIter < 1)
  {
    Message::SendFail("Syntax error: need more arguments");
    return 1;
  }

  for (; anArgIter < theArgsNb; ++anArgIter)
  {
    const char**            aChangeArgs   = theArgVec + anArgIter;
    Standard_Integer        aNbChangeArgs = theArgsNb - anArgIter;
    TCollection_AsciiString aChangeArg(aChangeArgs[0]);
    aChangeArg.LowerCase();

    Standard_Boolean toEnable = Standard_True;
    if (Draw::ParseOnOff(aChangeArgs[0], toEnable))
    {
      aClipPlane->SetOn(toEnable);
    }
    else if (aChangeArg.StartsWith("-equation") || aChangeArg.StartsWith("equation"))
    {
      if (aNbChangeArgs < 5)
      {
        Message::SendFail("Syntax error: need more arguments");
        return 1;
      }

      Standard_Integer aSubIndex  = 1;
      Standard_Integer aPrefixLen = 8 + (aChangeArg.Value(1) == '-' ? 1 : 0);
      if (aPrefixLen < aChangeArg.Length())
      {
        TCollection_AsciiString aSubStr = aChangeArg.SubString(aPrefixLen + 1, aChangeArg.Length());
        if (!aSubStr.IsIntegerValue() || aSubStr.IntegerValue() <= 0)
        {
          Message::SendFail() << "Syntax error: unknown argument '" << aChangeArg << "'";
          return 1;
        }
        aSubIndex = aSubStr.IntegerValue();
      }

      Standard_Real               aCoeffA = Draw::Atof(aChangeArgs[1]);
      Standard_Real               aCoeffB = Draw::Atof(aChangeArgs[2]);
      Standard_Real               aCoeffC = Draw::Atof(aChangeArgs[3]);
      Standard_Real               aCoeffD = Draw::Atof(aChangeArgs[4]);
      Handle(Graphic3d_ClipPlane) aSubPln = aClipPlane;
      for (Standard_Integer aSubPlaneIter = 1; aSubPlaneIter < aSubIndex; ++aSubPlaneIter)
      {
        if (aSubPln->ChainNextPlane().IsNull())
        {
          aSubPln->SetChainNextPlane(new Graphic3d_ClipPlane(*aSubPln));
        }
        aSubPln = aSubPln->ChainNextPlane();
      }
      aSubPln->SetChainNextPlane(Handle(Graphic3d_ClipPlane)());
      aSubPln->SetEquation(gp_Pln(aCoeffA, aCoeffB, aCoeffC, aCoeffD));
      anArgIter += 4;
    }
    else if ((aChangeArg == "-boxinterior" || aChangeArg == "-boxint" || aChangeArg == "-box")
             && aNbChangeArgs >= 7)
    {
      Graphic3d_BndBox3d aBndBox;
      aBndBox.Add(Graphic3d_Vec3d(Draw::Atof(aChangeArgs[1]),
                                  Draw::Atof(aChangeArgs[2]),
                                  Draw::Atof(aChangeArgs[3])));
      aBndBox.Add(Graphic3d_Vec3d(Draw::Atof(aChangeArgs[4]),
                                  Draw::Atof(aChangeArgs[5]),
                                  Draw::Atof(aChangeArgs[6])));
      anArgIter += 6;

      Standard_Integer      aNbSubPlanes = 6;
      const Graphic3d_Vec3d aDirArray[6] = {
        Graphic3d_Vec3d(-1, 0, 0),
        Graphic3d_Vec3d(1, 0, 0),
        Graphic3d_Vec3d(0, -1, 0),
        Graphic3d_Vec3d(0, 1, 0),
        Graphic3d_Vec3d(0, 0, -1),
        Graphic3d_Vec3d(0, 0, 1),
      };
      Handle(Graphic3d_ClipPlane) aSubPln = aClipPlane;
      for (Standard_Integer aSubPlaneIter = 0; aSubPlaneIter < aNbSubPlanes; ++aSubPlaneIter)
      {
        const Graphic3d_Vec3d& aDir = aDirArray[aSubPlaneIter];
        const Standard_Real    aW =
          -aDir.Dot((aSubPlaneIter % 2 == 1) ? aBndBox.CornerMax() : aBndBox.CornerMin());
        aSubPln->SetEquation(gp_Pln(aDir.x(), aDir.y(), aDir.z(), aW));
        if (aSubPlaneIter + 1 == aNbSubPlanes)
        {
          aSubPln->SetChainNextPlane(Handle(Graphic3d_ClipPlane)());
        }
        else
        {
          aSubPln->SetChainNextPlane(new Graphic3d_ClipPlane(*aSubPln));
        }
        aSubPln = aSubPln->ChainNextPlane();
      }
    }
    else if (aChangeArg == "-capping" || aChangeArg == "capping")
    {
      if (aNbChangeArgs < 2)
      {
        Message::SendFail("Syntax error: need more arguments");
        return 1;
      }

      if (Draw::ParseOnOff(aChangeArgs[1], toEnable))
      {
        aClipPlane->SetCapping(toEnable);
        anArgIter += 1;
      }
      else
      {
        // just skip otherwise (old syntax)
      }
    }
    else if (aChangeArg == "-useobjectmaterial" || aChangeArg == "-useobjectmat"
             || aChangeArg == "-useobjmat" || aChangeArg == "-useobjmaterial")
    {
      if (aNbChangeArgs < 2)
      {
        Message::SendFail("Syntax error: need more arguments");
        return 1;
      }

      if (Draw::ParseOnOff(aChangeArgs[1], toEnable))
      {
        aClipPlane->SetUseObjectMaterial(toEnable == Standard_True);
        anArgIter += 1;
      }
    }
    else if (aChangeArg == "-useobjecttexture" || aChangeArg == "-useobjecttex"
             || aChangeArg == "-useobjtexture" || aChangeArg == "-useobjtex")
    {
      if (aNbChangeArgs < 2)
      {
        Message::SendFail("Syntax error: need more arguments");
        return 1;
      }

      if (Draw::ParseOnOff(aChangeArgs[1], toEnable))
      {
        aClipPlane->SetUseObjectTexture(toEnable == Standard_True);
        anArgIter += 1;
      }
    }
    else if (aChangeArg == "-useobjectshader" || aChangeArg == "-useobjshader")
    {
      if (aNbChangeArgs < 2)
      {
        Message::SendFail("Syntax error: need more arguments");
        return 1;
      }

      if (Draw::ParseOnOff(aChangeArgs[1], toEnable))
      {
        aClipPlane->SetUseObjectShader(toEnable == Standard_True);
        anArgIter += 1;
      }
    }
    else if (aChangeArg == "-color" || aChangeArg == "color")
    {
      Quantity_Color   aColor;
      Standard_Integer aNbParsed = Draw::ParseColor(aNbChangeArgs - 1, aChangeArgs + 1, aColor);
      if (aNbParsed == 0)
      {
        Message::SendFail("Syntax error: need more arguments");
        return 1;
      }
      aClipPlane->SetCappingColor(aColor);
      anArgIter += aNbParsed;
    }
    else if (aNbChangeArgs >= 1 && (aChangeArg == "-material" || aChangeArg == "material"))
    {
      ++anArgIter;
      Graphic3d_NameOfMaterial aMatName;
      if (!Graphic3d_MaterialAspect::MaterialFromName(aChangeArgs[1], aMatName))
      {
        Message::SendFail() << "Syntax error: unknown material '" << aChangeArgs[1] << "'";
        return 1;
      }
      aClipPlane->SetCappingMaterial(aMatName);
    }
    else if ((aChangeArg == "-transparency" || aChangeArg == "-transp") && aNbChangeArgs >= 2)
    {
      TCollection_AsciiString            aValStr(aChangeArgs[1]);
      Handle(Graphic3d_AspectFillArea3d) anAspect = aClipPlane->CappingAspect();
      if (aValStr.IsRealValue(Standard_True))
      {
        Graphic3d_MaterialAspect aMat = aClipPlane->CappingMaterial();
        aMat.SetTransparency((float)aValStr.RealValue());
        anAspect->SetAlphaMode(Graphic3d_AlphaMode_BlendAuto);
        aClipPlane->SetCappingMaterial(aMat);
      }
      else
      {
        aValStr.LowerCase();
        Graphic3d_AlphaMode aMode = Graphic3d_AlphaMode_BlendAuto;
        if (aValStr == "opaque")
        {
          aMode = Graphic3d_AlphaMode_Opaque;
        }
        else if (aValStr == "mask")
        {
          aMode = Graphic3d_AlphaMode_Mask;
        }
        else if (aValStr == "blend")
        {
          aMode = Graphic3d_AlphaMode_Blend;
        }
        else if (aValStr == "maskblend" || aValStr == "blendmask")
        {
          aMode = Graphic3d_AlphaMode_MaskBlend;
        }
        else if (aValStr == "blendauto")
        {
          aMode = Graphic3d_AlphaMode_BlendAuto;
        }
        else
        {
          Message::SendFail() << "Syntax error at '" << aValStr << "'";
          return 1;
        }
        anAspect->SetAlphaMode(aMode);
        aClipPlane->SetCappingAspect(anAspect);
      }
      anArgIter += 1;
    }
    else if (aChangeArg == "-texname" || aChangeArg == "texname")
    {
      if (aNbChangeArgs < 2)
      {
        Message::SendFail("Syntax error: need more arguments");
        return 1;
      }

      TCollection_AsciiString     aTextureName(aChangeArgs[1]);
      Handle(Graphic3d_Texture2D) aTexture = new Graphic3d_Texture2D(aTextureName);
      if (!aTexture->IsDone())
      {
        aClipPlane->SetCappingTexture(NULL);
      }
      else
      {
        aTexture->EnableModulate();
        aTexture->EnableRepeat();
        aClipPlane->SetCappingTexture(aTexture);
      }
      anArgIter += 1;
    }
    else if (aChangeArg == "-texscale" || aChangeArg == "texscale")
    {
      if (aClipPlane->CappingTexture().IsNull())
      {
        Message::SendFail("Error: no texture is set");
        return 1;
      }

      if (aNbChangeArgs < 3)
      {
        Message::SendFail("Syntax error: need more arguments");
        return 1;
      }

      Standard_ShortReal aSx = (Standard_ShortReal)Draw::Atof(aChangeArgs[1]);
      Standard_ShortReal aSy = (Standard_ShortReal)Draw::Atof(aChangeArgs[2]);
      aClipPlane->CappingTexture()->GetParams()->SetScale(Graphic3d_Vec2(aSx, aSy));
      anArgIter += 2;
    }
    else if (aChangeArg == "-texorigin" || aChangeArg == "texorigin") // texture origin
    {
      if (aClipPlane->CappingTexture().IsNull())
      {
        Message::SendFail("Error: no texture is set");
        return 1;
      }

      if (aNbChangeArgs < 3)
      {
        Message::SendFail("Syntax error: need more arguments");
        return 1;
      }

      Standard_ShortReal aTx = (Standard_ShortReal)Draw::Atof(aChangeArgs[1]);
      Standard_ShortReal aTy = (Standard_ShortReal)Draw::Atof(aChangeArgs[2]);

      aClipPlane->CappingTexture()->GetParams()->SetTranslation(Graphic3d_Vec2(aTx, aTy));
      anArgIter += 2;
    }
    else if (aChangeArg == "-texrotate" || aChangeArg == "texrotate") // texture rotation
    {
      if (aClipPlane->CappingTexture().IsNull())
      {
        Message::SendFail("Error: no texture is set");
        return 1;
      }

      if (aNbChangeArgs < 2)
      {
        Message::SendFail("Syntax error: need more arguments");
        return 1;
      }

      Standard_ShortReal aRot = (Standard_ShortReal)Draw::Atof(aChangeArgs[1]);
      aClipPlane->CappingTexture()->GetParams()->SetRotation(aRot);
      anArgIter += 1;
    }
    else if (aChangeArg == "-hatch" || aChangeArg == "hatch")
    {
      if (aNbChangeArgs < 2)
      {
        Message::SendFail("Syntax error: need more arguments");
        return 1;
      }

      TCollection_AsciiString aHatchStr(aChangeArgs[1]);
      aHatchStr.LowerCase();
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
        aClipPlane->SetCappingHatch((Aspect_HatchStyle)Draw::Atoi(aChangeArgs[1]));
      }
      anArgIter += 1;
    }
    else if (aChangeArg == "-delete" || aChangeArg == "delete")
    {
      removePlane(aRegPlanes, aPlaneName);
      return 0;
    }
    else if (aChangeArg == "-set" || aChangeArg == "-unset" || aChangeArg == "-setoverrideglobal")
    {
      // set / unset plane command
      const Standard_Boolean toSet            = aChangeArg.StartsWith("-set");
      const Standard_Boolean toOverrideGlobal = aChangeArg == "-setoverrideglobal";
      Standard_Integer       anIt             = 1;
      for (; anIt < aNbChangeArgs; ++anIt)
      {
        TCollection_AsciiString anEntityName(aChangeArgs[anIt]);
        if (anEntityName.IsEmpty() || anEntityName.Value(1) == '-')
        {
          break;
        }
        else if (!toOverrideGlobal && ViewerTest_myViews.IsBound1(anEntityName))
        {
          const Handle(V3d_View)& aView = ViewerTest_myViews.Find1(anEntityName);
          if (toSet)
          {
            aView->AddClipPlane(aClipPlane);
          }
          else
          {
            aView->RemoveClipPlane(aClipPlane);
          }
          continue;
        }
        else if (GetMapOfAIS().IsBound2(anEntityName))
        {
          Handle(AIS_InteractiveObject) aIObj = GetMapOfAIS().Find2(anEntityName);
          if (toSet)
          {
            aIObj->AddClipPlane(aClipPlane);
          }
          else
          {
            aIObj->RemoveClipPlane(aClipPlane);
          }
          if (!aIObj->ClipPlanes().IsNull())
          {
            aIObj->ClipPlanes()->SetOverrideGlobal(toOverrideGlobal);
          }
        }
        else
        {
          Message::SendFail() << "Error: object/view '" << anEntityName << "' is not found";
          return 1;
        }
      }

      if (anIt == 1)
      {
        // apply to active view
        if (toSet)
        {
          anActiveView->AddClipPlane(aClipPlane);
        }
        else
        {
          anActiveView->RemoveClipPlane(aClipPlane);
        }
      }
      else
      {
        anArgIter = anArgIter + anIt - 1;
      }
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << aChangeArg << "'";
      return 1;
    }
  }

  ViewerTest::RedrawAllViews();
  return 0;
}

//=================================================================================================

static int VZRange(Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  const Handle(V3d_View)& aCurrentView = ViewerTest::CurrentView();

  if (aCurrentView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
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
    Standard_Real aNewZNear = Draw::Atof(theArgVec[1]);
    Standard_Real aNewZFar  = Draw::Atof(theArgVec[2]);

    if (aNewZNear >= aNewZFar)
    {
      Message::SendFail("Syntax error: invalid arguments: znear should be less than zfar");
      return 1;
    }

    if (!aCamera->IsOrthographic() && (aNewZNear <= 0.0 || aNewZFar <= 0.0))
    {
      Message::SendFail(
        "Syntax error: invalid arguments: znear, zfar should be positive for perspective camera");
      return 1;
    }

    aCamera->SetZRange(aNewZNear, aNewZFar);
  }
  else
  {
    Message::SendFail("Syntax error: wrong command arguments");
    return 1;
  }

  aCurrentView->Redraw();

  return 0;
}

//=================================================================================================

static int VAutoZFit(Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  const Handle(V3d_View)& aCurrentView = ViewerTest::CurrentView();

  if (aCurrentView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Standard_Real aScale = aCurrentView->AutoZFitScaleFactor();

  if (theArgsNb > 3)
  {
    Message::SendFail("Syntax error: wrong command arguments");
    return 1;
  }

  if (theArgsNb < 2)
  {
    theDi << "Auto z-fit mode: \n"
          << "On: " << (aCurrentView->AutoZFitMode() ? "enabled" : "disabled") << "\n"
          << "Scale: " << aScale << "\n";
    return 0;
  }

  Standard_Boolean isOn = Draw::Atoi(theArgVec[1]) == 1;

  if (theArgsNb >= 3)
  {
    aScale = Draw::Atoi(theArgVec[2]);
  }

  aCurrentView->SetAutoZFitMode(isOn, aScale);
  aCurrentView->Redraw();
  return 0;
}

//! Auxiliary function to print projection type
inline const char* projTypeName(Graphic3d_Camera::Projection theProjType)
{
  switch (theProjType)
  {
    case Graphic3d_Camera::Projection_Orthographic:
      return "orthographic";
    case Graphic3d_Camera::Projection_Perspective:
      return "perspective";
    case Graphic3d_Camera::Projection_Stereo:
      return "stereoscopic";
    case Graphic3d_Camera::Projection_MonoLeftEye:
      return "monoLeftEye";
    case Graphic3d_Camera::Projection_MonoRightEye:
      return "monoRightEye";
  }
  return "UNKNOWN";
}

//=================================================================================================

static int VCamera(Draw_Interpretor& theDI, Standard_Integer theArgsNb, const char** theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Handle(Graphic3d_Camera) aCamera = aView->Camera();
  if (theArgsNb < 2)
  {
    theDI << "ProjType:   " << projTypeName(aCamera->ProjectionType()) << "\n";
    theDI << "FOVy:       " << aCamera->FOVy() << "\n";
    theDI << "FOVx:       " << aCamera->FOVx() << "\n";
    theDI << "FOV2d:      " << aCamera->FOV2d() << "\n";
    theDI << "Distance:   " << aCamera->Distance() << "\n";
    theDI << "IOD:        " << aCamera->IOD() << "\n";
    theDI << "IODType:    "
          << (aCamera->GetIODType() == Graphic3d_Camera::IODType_Absolute ? "absolute" : "relative")
          << "\n";
    theDI << "ZFocus:     " << aCamera->ZFocus() << "\n";
    theDI << "ZFocusType: "
          << (aCamera->ZFocusType() == Graphic3d_Camera::FocusType_Absolute ? "absolute"
                                                                            : "relative")
          << "\n";
    theDI << "ZNear:      " << aCamera->ZNear() << "\n";
    theDI << "ZFar:       " << aCamera->ZFar() << "\n";
    return 0;
  }

  TCollection_AsciiString aPrsName;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    Standard_CString        anArg = theArgVec[anArgIter];
    TCollection_AsciiString anArgCase(anArg);
    anArgCase.LowerCase();
    if (anArgCase == "-proj" || anArgCase == "-projection" || anArgCase == "-projtype"
        || anArgCase == "-projectiontype")
    {
      theDI << projTypeName(aCamera->ProjectionType()) << " ";
    }
    else if (anArgCase == "-ortho" || anArgCase == "-orthographic")
    {
      aCamera->SetProjectionType(Graphic3d_Camera::Projection_Orthographic);
    }
    else if (anArgCase == "-persp" || anArgCase == "-perspective" || anArgCase == "-perspmono"
             || anArgCase == "-perspectivemono" || anArgCase == "-mono")
    {
      aCamera->SetProjectionType(Graphic3d_Camera::Projection_Perspective);
    }
    else if (anArgCase == "-stereo" || anArgCase == "-stereoscopic" || anArgCase == "-perspstereo"
             || anArgCase == "-perspectivestereo")
    {
      aCamera->SetProjectionType(Graphic3d_Camera::Projection_Stereo);
    }
    else if (anArgCase == "-left" || anArgCase == "-lefteye" || anArgCase == "-monoleft"
             || anArgCase == "-monolefteye" || anArgCase == "-perpsleft"
             || anArgCase == "-perpslefteye")
    {
      aCamera->SetProjectionType(Graphic3d_Camera::Projection_MonoLeftEye);
    }
    else if (anArgCase == "-right" || anArgCase == "-righteye" || anArgCase == "-monoright"
             || anArgCase == "-monorighteye" || anArgCase == "-perpsright")
    {
      aCamera->SetProjectionType(Graphic3d_Camera::Projection_MonoRightEye);
    }
    else if (anArgCase == "-dist" || anArgCase == "-distance")
    {
      Standard_CString anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : NULL;
      if (anArgValue != NULL && *anArgValue != '-')
      {
        ++anArgIter;
        aCamera->SetDistance(Draw::Atof(anArgValue));
        continue;
      }
      theDI << aCamera->Distance() << " ";
    }
    else if (anArgCase == "-iod")
    {
      Standard_CString anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : NULL;
      if (anArgValue != NULL && *anArgValue != '-')
      {
        ++anArgIter;
        aCamera->SetIOD(aCamera->GetIODType(), Draw::Atof(anArgValue));
        continue;
      }
      theDI << aCamera->IOD() << " ";
    }
    else if (anArgCase == "-iodtype")
    {
      Standard_CString anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : "";
      TCollection_AsciiString anValueCase(anArgValue);
      anValueCase.LowerCase();
      if (anValueCase == "abs" || anValueCase == "absolute")
      {
        ++anArgIter;
        aCamera->SetIOD(Graphic3d_Camera::IODType_Absolute, aCamera->IOD());
        continue;
      }
      else if (anValueCase == "rel" || anValueCase == "relative")
      {
        ++anArgIter;
        aCamera->SetIOD(Graphic3d_Camera::IODType_Relative, aCamera->IOD());
        continue;
      }
      else if (*anArgValue != '-')
      {
        Message::SendFail() << "Error: unknown IOD type '" << anArgValue << "'";
        return 1;
      }
      switch (aCamera->GetIODType())
      {
        case Graphic3d_Camera::IODType_Absolute:
          theDI << "absolute ";
          break;
        case Graphic3d_Camera::IODType_Relative:
          theDI << "relative ";
          break;
      }
    }
    else if (anArgCase == "-zfocus")
    {
      Standard_CString anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : NULL;
      if (anArgValue != NULL && *anArgValue != '-')
      {
        ++anArgIter;
        aCamera->SetZFocus(aCamera->ZFocusType(), Draw::Atof(anArgValue));
        continue;
      }
      theDI << aCamera->ZFocus() << " ";
    }
    else if (anArgCase == "-zfocustype")
    {
      Standard_CString anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : "";
      TCollection_AsciiString anValueCase(anArgValue);
      anValueCase.LowerCase();
      if (anValueCase == "abs" || anValueCase == "absolute")
      {
        ++anArgIter;
        aCamera->SetZFocus(Graphic3d_Camera::FocusType_Absolute, aCamera->ZFocus());
        continue;
      }
      else if (anValueCase == "rel" || anValueCase == "relative")
      {
        ++anArgIter;
        aCamera->SetZFocus(Graphic3d_Camera::FocusType_Relative, aCamera->ZFocus());
        continue;
      }
      else if (*anArgValue != '-')
      {
        Message::SendFail() << "Error: unknown ZFocus type '" << anArgValue << "'";
        return 1;
      }
      switch (aCamera->ZFocusType())
      {
        case Graphic3d_Camera::FocusType_Absolute:
          theDI << "absolute ";
          break;
        case Graphic3d_Camera::FocusType_Relative:
          theDI << "relative ";
          break;
      }
    }
    else if (anArgCase == "-lockzup" || anArgCase == "-turntable")
    {
      bool toLockUp = true;
      if (++anArgIter < theArgsNb && !Draw::ParseOnOff(theArgVec[anArgIter], toLockUp))
      {
        --anArgIter;
      }
      ViewerTest::CurrentEventManager()->SetLockOrbitZUp(toLockUp);
    }
    else if (anArgCase == "-rotationmode" || anArgCase == "-rotmode")
    {
      AIS_RotationMode        aRotMode = AIS_RotationMode_BndBoxActive;
      TCollection_AsciiString aRotStr((anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : "");
      aRotStr.LowerCase();
      if (aRotStr == "bndboxactive" || aRotStr == "active")
      {
        aRotMode = AIS_RotationMode_BndBoxActive;
      }
      else if (aRotStr == "picklast" || aRotStr == "pick")
      {
        aRotMode = AIS_RotationMode_PickLast;
      }
      else if (aRotStr == "pickcenter")
      {
        aRotMode = AIS_RotationMode_PickCenter;
      }
      else if (aRotStr == "cameraat" || aRotStr == "cameracenter")
      {
        aRotMode = AIS_RotationMode_CameraAt;
      }
      else if (aRotStr == "bndboxscene" || aRotStr == "boxscene")
      {
        aRotMode = AIS_RotationMode_BndBoxScene;
      }
      else
      {
        Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }

      ViewerTest::CurrentEventManager()->SetRotationMode(aRotMode);
      ++anArgIter;
    }
    else if (anArgCase == "-navigationmode" || anArgCase == "-navmode")
    {
      AIS_NavigationMode      aNavMode = AIS_NavigationMode_Orbit;
      TCollection_AsciiString aNavStr((anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : "");
      aNavStr.LowerCase();
      if (aNavStr == "orbit")
      {
        aNavMode = AIS_NavigationMode_Orbit;
      }
      else if (aNavStr == "flight" || aNavStr == "fly" || aNavStr == "copter"
               || aNavStr == "helicopter")
      {
        aNavMode = AIS_NavigationMode_FirstPersonFlight;
      }
      else if (aNavStr == "walk" || aNavStr == "shooter")
      {
        aNavMode = AIS_NavigationMode_FirstPersonWalk;
      }
      else
      {
        Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }

      Handle(ViewerTest_EventManager) aViewMgr = ViewerTest::CurrentEventManager();
      aViewMgr->SetNavigationMode(aNavMode);
      if (aNavMode == AIS_NavigationMode_Orbit)
      {
        aViewMgr->ChangeMouseGestureMap().Bind(Aspect_VKeyMouse_LeftButton,
                                               AIS_MouseGesture_RotateOrbit);
      }
      else
      {
        aViewMgr->ChangeMouseGestureMap().Bind(Aspect_VKeyMouse_LeftButton,
                                               AIS_MouseGesture_RotateView);
      }
      ++anArgIter;
    }
    else if (anArgCase == "-fov" || anArgCase == "-fovy" || anArgCase == "-fovx"
             || anArgCase == "-fov2d")
    {
      Standard_CString anArgValue = (anArgIter + 1 < theArgsNb) ? theArgVec[anArgIter + 1] : NULL;
      if (anArgValue != NULL && *anArgValue != '-')
      {
        ++anArgIter;
        if (anArgCase == "-fov2d")
        {
          aCamera->SetFOV2d(Draw::Atof(anArgValue));
        }
        else if (anArgCase == "-fovx")
        {
          aCamera->SetFOVy(Draw::Atof(anArgValue) / aCamera->Aspect()); ///
        }
        else
        {
          aCamera->SetFOVy(Draw::Atof(anArgValue));
        }
        continue;
      }
      if (anArgCase == "-fov2d")
      {
        theDI << aCamera->FOV2d() << " ";
      }
      else if (anArgCase == "-fovx")
      {
        theDI << aCamera->FOVx() << " ";
      }
      else
      {
        theDI << aCamera->FOVy() << " ";
      }
    }
    else if (anArgIter + 1 < theArgsNb && anArgCase == "-xrpose")
    {
      TCollection_AsciiString anXRArg(theArgVec[++anArgIter]);
      anXRArg.LowerCase();
      if (anXRArg == "base")
      {
        aCamera = aView->View()->BaseXRCamera();
      }
      else if (anXRArg == "head")
      {
        aCamera = aView->View()->PosedXRCamera();
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown XR pose '" << anXRArg << "'";
        return 1;
      }
      if (aCamera.IsNull())
      {
        Message::SendFail() << "Error: undefined XR pose";
        return 0;
      }
      if (aView->AutoZFitMode())
      {
        const Bnd_Box aMinMaxBox  = aView->View()->MinMaxValues(false);
        const Bnd_Box aGraphicBox = aView->View()->MinMaxValues(true);
        aCamera->ZFitAll(aView->AutoZFitScaleFactor(), aMinMaxBox, aGraphicBox);
      }
    }
    else if (aPrsName.IsEmpty() && !anArgCase.StartsWith("-"))
    {
      aPrsName = anArg;
    }
    else
    {
      Message::SendFail() << "Error: unknown argument '" << anArg << "'";
      return 1;
    }
  }

  if (aPrsName.IsEmpty() || theArgsNb > 2)
  {
    aView->Redraw();
  }

  if (!aPrsName.IsEmpty())
  {
    Handle(AIS_CameraFrustum) aCameraFrustum;
    if (GetMapOfAIS().IsBound2(aPrsName))
    {
      // find existing object
      aCameraFrustum = Handle(AIS_CameraFrustum)::DownCast(GetMapOfAIS().Find2(theArgVec[1]));
      if (aCameraFrustum.IsNull())
      {
        Message::SendFail() << "Error: object '" << aPrsName
                            << "'is already defined and is not a camera frustum";
        return 1;
      }
    }

    if (aCameraFrustum.IsNull())
    {
      aCameraFrustum = new AIS_CameraFrustum();
    }
    else
    {
      // not include displayed object of old camera frustum in the new one.
      ViewerTest::GetAISContext()->Erase(aCameraFrustum, false);
      aView->ZFitAll();
    }
    aCameraFrustum->SetCameraFrustum(aCamera);

    ViewerTest::Display(aPrsName, aCameraFrustum);
  }

  return 0;
}

//! Parse stereo output mode
inline Standard_Boolean parseStereoMode(Standard_CString theArg, Graphic3d_StereoMode& theMode)
{
  TCollection_AsciiString aFlag(theArg);
  aFlag.LowerCase();
  if (aFlag == "quadbuffer")
  {
    theMode = Graphic3d_StereoMode_QuadBuffer;
  }
  else if (aFlag == "anaglyph")
  {
    theMode = Graphic3d_StereoMode_Anaglyph;
  }
  else if (aFlag == "row" || aFlag == "rowinterlaced")
  {
    theMode = Graphic3d_StereoMode_RowInterlaced;
  }
  else if (aFlag == "col" || aFlag == "colinterlaced" || aFlag == "columninterlaced")
  {
    theMode = Graphic3d_StereoMode_ColumnInterlaced;
  }
  else if (aFlag == "chess" || aFlag == "chessboard")
  {
    theMode = Graphic3d_StereoMode_ChessBoard;
  }
  else if (aFlag == "sbs" || aFlag == "sidebyside")
  {
    theMode = Graphic3d_StereoMode_SideBySide;
  }
  else if (aFlag == "ou" || aFlag == "overunder")
  {
    theMode = Graphic3d_StereoMode_OverUnder;
  }
  else if (aFlag == "pageflip" || aFlag == "softpageflip")
  {
    theMode = Graphic3d_StereoMode_SoftPageFlip;
  }
  else if (aFlag == "openvr" || aFlag == "vr")
  {
    theMode = Graphic3d_StereoMode_OpenVR;
  }
  else
  {
    return Standard_False;
  }
  return Standard_True;
}

//! Parse anaglyph filter
inline Standard_Boolean parseAnaglyphFilter(Standard_CString                     theArg,
                                            Graphic3d_RenderingParams::Anaglyph& theFilter)
{
  TCollection_AsciiString aFlag(theArg);
  aFlag.LowerCase();
  if (aFlag == "redcyansimple")
  {
    theFilter = Graphic3d_RenderingParams::Anaglyph_RedCyan_Simple;
  }
  else if (aFlag == "redcyan" || aFlag == "redcyanoptimized")
  {
    theFilter = Graphic3d_RenderingParams::Anaglyph_RedCyan_Optimized;
  }
  else if (aFlag == "yellowbluesimple")
  {
    theFilter = Graphic3d_RenderingParams::Anaglyph_YellowBlue_Simple;
  }
  else if (aFlag == "yellowblue" || aFlag == "yellowblueoptimized")
  {
    theFilter = Graphic3d_RenderingParams::Anaglyph_YellowBlue_Optimized;
  }
  else if (aFlag == "greenmagenta" || aFlag == "greenmagentasimple")
  {
    theFilter = Graphic3d_RenderingParams::Anaglyph_GreenMagenta_Simple;
  }
  else
  {
    return Standard_False;
  }
  return Standard_True;
}

//=================================================================================================

static int VStereo(Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 0;
  }

  Handle(Graphic3d_Camera)   aCamera = aView->Camera();
  Graphic3d_RenderingParams* aParams = &aView->ChangeRenderingParams();
  if (theArgNb < 2)
  {
    Standard_Boolean isActive = aCamera->ProjectionType() == Graphic3d_Camera::Projection_Stereo;
    theDI << "Stereo " << (isActive ? "ON" : "OFF") << "\n";
    if (isActive)
    {
      TCollection_AsciiString aMode;
      switch (aView->RenderingParams().StereoMode)
      {
        case Graphic3d_StereoMode_QuadBuffer: {
          aMode = "quadBuffer";
          break;
        }
        case Graphic3d_StereoMode_RowInterlaced: {
          aMode = "rowInterlaced";
          if (aView->RenderingParams().ToSmoothInterlacing)
          {
            aMode.AssignCat(" (smoothed)");
          }
          break;
        }
        case Graphic3d_StereoMode_ColumnInterlaced: {
          aMode = "columnInterlaced";
          if (aView->RenderingParams().ToSmoothInterlacing)
          {
            aMode.AssignCat(" (smoothed)");
          }
          break;
        }
        case Graphic3d_StereoMode_ChessBoard: {
          aMode = "chessBoard";
          if (aView->RenderingParams().ToSmoothInterlacing)
          {
            aMode.AssignCat(" (smoothed)");
          }
          break;
        }
        case Graphic3d_StereoMode_SideBySide: {
          aMode = "sideBySide";
          break;
        }
        case Graphic3d_StereoMode_OverUnder: {
          aMode = "overUnder";
          break;
        }
        case Graphic3d_StereoMode_SoftPageFlip: {
          aMode = "softPageFlip";
          break;
        }
        case Graphic3d_StereoMode_OpenVR: {
          aMode = "openVR";
          break;
        }
        case Graphic3d_StereoMode_Anaglyph: {
          aMode = "anaglyph";
          switch (aView->RenderingParams().AnaglyphFilter)
          {
            case Graphic3d_RenderingParams::Anaglyph_RedCyan_Simple:
              aMode.AssignCat(" (redCyanSimple)");
              break;
            case Graphic3d_RenderingParams::Anaglyph_RedCyan_Optimized:
              aMode.AssignCat(" (redCyan)");
              break;
            case Graphic3d_RenderingParams::Anaglyph_YellowBlue_Simple:
              aMode.AssignCat(" (yellowBlueSimple)");
              break;
            case Graphic3d_RenderingParams::Anaglyph_YellowBlue_Optimized:
              aMode.AssignCat(" (yellowBlue)");
              break;
            case Graphic3d_RenderingParams::Anaglyph_GreenMagenta_Simple:
              aMode.AssignCat(" (greenMagentaSimple)");
              break;
            case Graphic3d_RenderingParams::Anaglyph_UserDefined:
              aMode.AssignCat(" (userDefined)");
              break;
          }
        }
      }
      theDI << "Mode " << aMode << "\n";
    }
    return 0;
  }

  Graphic3d_StereoMode   aMode = aParams->StereoMode;
  ViewerTest_AutoUpdater anUpdateTool(ViewerTest::GetAISContext(), aView);
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg = theArgVec[anArgIter];
    TCollection_AsciiString aFlag(anArg);
    aFlag.LowerCase();
    if (anUpdateTool.parseRedrawMode(aFlag))
    {
      continue;
    }
    else if (aFlag == "0" || aFlag == "off")
    {
      if (++anArgIter < theArgNb)
      {
        Message::SendFail("Error: wrong number of arguments");
        return 1;
      }

      if (aCamera->ProjectionType() == Graphic3d_Camera::Projection_Stereo)
      {
        aCamera->SetProjectionType(Graphic3d_Camera::Projection_Perspective);
      }
      return 0;
    }
    else if (aFlag == "1" || aFlag == "on")
    {
      if (++anArgIter < theArgNb)
      {
        Message::SendFail("Error: wrong number of arguments");
        return 1;
      }

      aCamera->SetProjectionType(Graphic3d_Camera::Projection_Stereo);
      if (aParams->StereoMode != Graphic3d_StereoMode_OpenVR)
      {
        return 0;
      }
    }
    else if (aFlag == "-reverse" || aFlag == "-noreverse" || aFlag == "-reversed"
             || aFlag == "-swap" || aFlag == "-noswap")
    {
      aParams->ToReverseStereo = Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (aFlag == "-mode" || aFlag == "-stereomode")
    {
      if (++anArgIter >= theArgNb || !parseStereoMode(theArgVec[anArgIter], aMode))
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }

      if (aMode == Graphic3d_StereoMode_QuadBuffer)
      {
        Message::SendInfo()
          << "Warning: make sure to call 'vcaps -stereo 1' before creating a view";
      }
    }
    else if (aFlag == "-anaglyph" || aFlag == "-anaglyphfilter")
    {
      Graphic3d_RenderingParams::Anaglyph aFilter =
        Graphic3d_RenderingParams::Anaglyph_RedCyan_Simple;
      if (++anArgIter >= theArgNb || !parseAnaglyphFilter(theArgVec[anArgIter], aFilter))
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }

      aMode                   = Graphic3d_StereoMode_Anaglyph;
      aParams->AnaglyphFilter = aFilter;
    }
    else if (parseStereoMode(anArg, aMode)) // short syntax
    {
      if (aMode == Graphic3d_StereoMode_QuadBuffer)
      {
        Message::SendInfo()
          << "Warning: make sure to call 'vcaps -stereo 1' before creating a view";
      }
    }
    else if (anArgIter + 1 < theArgNb && aFlag == "-hmdfov2d")
    {
      aParams->HmdFov2d = (float)Draw::Atof(theArgVec[++anArgIter]);
      if (aParams->HmdFov2d < 10.0f || aParams->HmdFov2d > 180.0f)
      {
        Message::SendFail() << "Error: FOV is out of range";
        return 1;
      }
    }
    else if (aFlag == "-mirror" || aFlag == "-mirrorcomposer")
    {
      aParams->ToMirrorComposer = Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
      ;
    }
    else if (aFlag == "-smooth" || aFlag == "-nosmooth" || aFlag == "-smoothinterlacing"
             || aFlag == "-nosmoothinterlacing")
    {
      aParams->ToSmoothInterlacing = Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgIter + 1 < theArgNb && (aFlag == "-unitfactor" || aFlag == "-unitscale"))
    {
      aView->View()->SetUnitFactor(Draw::Atof(theArgVec[++anArgIter]));
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  aParams->StereoMode = aMode;
  aCamera->SetProjectionType(Graphic3d_Camera::Projection_Stereo);
  if (aParams->StereoMode == Graphic3d_StereoMode_OpenVR)
  {
    // initiate implicit continuous rendering
    ViewerTest::CurrentEventManager()->FlushViewEvents(ViewerTest::GetAISContext(), aView, true);
  }
  return 0;
}

//=================================================================================================

static int VDefaults(Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: no active viewer");
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
    theDi << "AngularDeflection:  " << (180.0 * aDefParams->DeviationAngle() / M_PI) << "\n";
    theDi << "AutoTriangulation:  " << (aDefParams->IsAutoTriangulation() ? "on" : "off") << "\n";
    return 0;
  }

  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.UpperCase();
    if (anArg == "-ABSDEFL" || anArg == "-ABSOLUTEDEFLECTION" || anArg == "-DEFL"
        || anArg == "-DEFLECTION")
    {
      if (++anArgIter >= theArgsNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      aDefParams->SetTypeOfDeflection(Aspect_TOD_ABSOLUTE);
      aDefParams->SetMaximalChordialDeviation(Draw::Atof(theArgVec[anArgIter]));
    }
    else if (anArg == "-RELDEFL" || anArg == "-RELATIVEDEFLECTION" || anArg == "-DEVCOEFF"
             || anArg == "-DEVIATIONCOEFF" || anArg == "-DEVIATIONCOEFFICIENT")
    {
      if (++anArgIter >= theArgsNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      aDefParams->SetTypeOfDeflection(Aspect_TOD_RELATIVE);
      aDefParams->SetDeviationCoefficient(Draw::Atof(theArgVec[anArgIter]));
    }
    else if (anArg == "-ANGDEFL" || anArg == "-ANGULARDEFL" || anArg == "-ANGULARDEFLECTION")
    {
      if (++anArgIter >= theArgsNb)
      {
        Message::SendFail() << "Syntax error at " << anArg;
        return 1;
      }
      aDefParams->SetDeviationAngle(M_PI * Draw::Atof(theArgVec[anArgIter]) / 180.0);
    }
    else if (anArg == "-AUTOTR" || anArg == "-AUTOTRIANG" || anArg == "-AUTOTRIANGULATION")
    {
      ++anArgIter;
      bool toTurnOn = true;
      if (anArgIter >= theArgsNb || !Draw::ParseOnOff(theArgVec[anArgIter], toTurnOn))
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }
      aDefParams->SetAutoTriangulation(toTurnOn);
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << anArg << "'";
      return 1;
    }
  }

  return 0;
}

//! Parse light source type from string.
static bool parseLightSourceType(const TCollection_AsciiString& theTypeName,
                                 Graphic3d_TypeOfLightSource&   theType)
{
  TCollection_AsciiString aType(theTypeName);
  aType.LowerCase();
  if (aType == "amb" || aType == "ambient" || aType == "amblight")
  {
    theType = Graphic3d_TypeOfLightSource_Ambient;
  }
  else if (aType == "directional" || aType == "dirlight")
  {
    theType = Graphic3d_TypeOfLightSource_Directional;
  }
  else if (aType == "spot" || aType == "spotlight")
  {
    theType = Graphic3d_TypeOfLightSource_Spot;
  }
  else if (aType == "poslight" || aType == "positional" || aType == "point" || aType == "pnt")
  {
    theType = Graphic3d_TypeOfLightSource_Positional;
  }
  else
  {
    return false;
  }
  return true;
}

//! Find existing light by name or index.
static Handle(V3d_Light) findLightSource(const TCollection_AsciiString& theName)
{
  Handle(V3d_Light) aLight;
  Standard_Integer  aLightIndex = -1;
  Draw::ParseInteger(theName.ToCString(), aLightIndex);
  Standard_Integer aLightIt = 0;
  Handle(V3d_View) aView    = ViewerTest::CurrentView();
  for (V3d_ListOfLightIterator aLightIter(aView->ActiveLightIterator()); aLightIter.More();
       aLightIter.Next(), ++aLightIt)
  {
    if (aLightIndex != -1)
    {
      if (aLightIt == aLightIndex)
      {
        return aLightIter.Value();
      }
    }
    else if (aLightIter.Value()->GetId() == theName || aLightIter.Value()->Name() == theName)
    {
      if (!aLight.IsNull())
      {
        Message::SendWarning() << "Warning: ambiguous light name '" << theName << "'";
        break;
      }
      aLight = aLightIter.Value();
    }
  }
  return aLight;
}

//=================================================================================================

static int VLight(Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  Handle(V3d_View)                      aView   = ViewerTest::CurrentView();
  Handle(V3d_Viewer)                    aViewer = ViewerTest::GetViewerFromContext();
  const Handle(AIS_InteractiveContext)& aCtx    = ViewerTest::GetAISContext();
  if (aView.IsNull() || aViewer.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  if (theArgsNb < 2)
  {
    // print lights info
    Standard_Integer aLightId = 0;
    for (V3d_ListOfLightIterator aLightIter(aView->ActiveLightIterator()); aLightIter.More();
         aLightIter.Next(), ++aLightId)
    {
      Handle(V3d_Light)    aLight = aLightIter.Value();
      const Quantity_Color aColor = aLight->Color();
      theDi << "Light #" << aLightId
            << (!aLight->Name().IsEmpty() ? (TCollection_AsciiString(" ") + aLight->Name()) : "")
            << " [" << aLight->GetId() << "] " << (aLight->IsEnabled() ? "ON" : "OFF") << "\n";
      switch (aLight->Type())
      {
        case Graphic3d_TypeOfLightSource_Ambient: {
          theDi << "  Type:       Ambient\n"
                << "  Intensity:  " << aLight->Intensity() << "\n";
          break;
        }
        case Graphic3d_TypeOfLightSource_Directional: {
          theDi << "  Type:       Directional\n"
                << "  Intensity:  " << aLight->Intensity() << "\n"
                << "  Headlight:  " << (aLight->Headlight() ? "TRUE" : "FALSE") << "\n"
                << "  CastShadows:" << (aLight->ToCastShadows() ? "TRUE" : "FALSE") << "\n"
                << "  Smoothness: " << aLight->Smoothness() << "\n"
                << "  Direction:  " << aLight->PackedDirection().x() << " "
                << aLight->PackedDirection().y() << " " << aLight->PackedDirection().z() << "\n";
          break;
        }
        case Graphic3d_TypeOfLightSource_Positional: {
          theDi << "  Type:       Positional\n"
                << "  Intensity:  " << aLight->Intensity() << "\n"
                << "  Headlight:  " << (aLight->Headlight() ? "TRUE" : "FALSE") << "\n"
                << "  CastShadows:" << (aLight->ToCastShadows() ? "TRUE" : "FALSE") << "\n"
                << "  Smoothness: " << aLight->Smoothness() << "\n"
                << "  Position:   " << aLight->Position().X() << " " << aLight->Position().Y()
                << " " << aLight->Position().Z() << "\n"
                << "  Atten.:     " << aLight->ConstAttenuation() << " "
                << aLight->LinearAttenuation() << "\n"
                << "  Range:      " << aLight->Range() << "\n";
          break;
        }
        case Graphic3d_TypeOfLightSource_Spot: {
          theDi << "  Type:       Spot\n"
                << "  Intensity:  " << aLight->Intensity() << "\n"
                << "  Headlight:  " << (aLight->Headlight() ? "TRUE" : "FALSE") << "\n"
                << "  CastShadows:" << (aLight->ToCastShadows() ? "TRUE" : "FALSE") << "\n"
                << "  Position:   " << aLight->Position().X() << " " << aLight->Position().Y()
                << " " << aLight->Position().Z() << "\n"
                << "  Direction:  " << aLight->PackedDirection().x() << " "
                << aLight->PackedDirection().y() << " " << aLight->PackedDirection().z() << "\n"
                << "  Atten.:     " << aLight->ConstAttenuation() << " "
                << aLight->LinearAttenuation() << "\n"
                << "  Angle:      " << (aLight->Angle() * 180.0 / M_PI) << "\n"
                << "  Exponent:   " << aLight->Concentration() << "\n"
                << "  Range:      " << aLight->Range() << "\n";
          break;
        }
        default: {
          theDi << "  Type:       UNKNOWN\n";
          break;
        }
      }
      theDi << "  Color:      " << aColor.Red() << " " << aColor.Green() << " " << aColor.Blue()
            << " [" << Quantity_Color::StringName(aColor.Name()) << "]\n";
    }
  }

  Handle(V3d_Light)       aLightOld, aLightNew;
  Graphic3d_ZLayerId      aLayer   = Graphic3d_ZLayerId_UNKNOWN;
  bool                    isGlobal = true;
  ViewerTest_AutoUpdater  anUpdateTool(aCtx, aView);
  Handle(AIS_LightSource) aLightPrs;
  for (Standard_Integer anArgIt = 1; anArgIt < theArgsNb; ++anArgIt)
  {
    const TCollection_AsciiString anArg(theArgVec[anArgIt]);
    TCollection_AsciiString       anArgCase(anArg);
    anArgCase.LowerCase();
    if (anUpdateTool.parseRedrawMode(anArg))
    {
      continue;
    }
    else if (anArgCase == "-new" || anArgCase == "-add" || anArgCase == "-create"
             || anArgCase == "-type" || (anArgCase == "-reset" && !aLightNew.IsNull())
             || (anArgCase == "-defaults" && !aLightNew.IsNull()) || anArgCase == "add"
             || anArgCase == "new" || anArgCase == "create")
    {
      Graphic3d_TypeOfLightSource aType = Graphic3d_TypeOfLightSource_Ambient;
      if (anArgCase == "-reset")
      {
        aType = aLightNew->Type();
      }
      else if (anArgIt + 1 >= theArgsNb || !parseLightSourceType(theArgVec[++anArgIt], aType))
      {
        theDi << "Syntax error at '" << theArgVec[anArgIt] << "'\n";
        return 1;
      }

      TCollection_AsciiString aName;
      if (!aLightNew.IsNull())
      {
        aName = aLightNew->Name();
      }
      switch (aType)
      {
        case Graphic3d_TypeOfLightSource_Ambient: {
          aLightNew = new V3d_AmbientLight();
          break;
        }
        case Graphic3d_TypeOfLightSource_Directional: {
          aLightNew = new V3d_DirectionalLight();
          break;
        }
        case Graphic3d_TypeOfLightSource_Spot: {
          aLightNew = new V3d_SpotLight(gp_Pnt(0.0, 0.0, 0.0));
          break;
        }
        case Graphic3d_TypeOfLightSource_Positional: {
          aLightNew = new V3d_PositionalLight(gp_Pnt(0.0, 0.0, 0.0));
          break;
        }
      }

      if (anArgCase == "-type" && !aLightOld.IsNull())
      {
        aLightNew->CopyFrom(aLightOld);
      }
      aLightNew->SetName(aName);
    }
    else if ((anArgCase == "-layer" || anArgCase == "-zlayer") && anArgIt + 1 < theArgsNb)
    {
      if (!ViewerTest::ParseZLayer(theArgVec[++anArgIt], aLayer)
          || aLayer == Graphic3d_ZLayerId_UNKNOWN)
      {
        Message::SendFail() << "Error: wrong syntax at '" << theArgVec[anArgIt] << "'";
        return 1;
      }
    }
    else if (anArgCase == "-glob" || anArgCase == "-global" || anArgCase == "-loc"
             || anArgCase == "-local")
    {
      isGlobal = anArgCase.StartsWith("-glob");
    }
    else if (anArgCase == "-def" || anArgCase == "-defaults" || anArgCase == "-reset")
    {
      aViewer->SetDefaultLights();
      aLightOld.Nullify();
      aLightNew.Nullify();
      aLightPrs.Nullify();
    }
    else if (anArgCase == "-clr" || anArgCase == "-clear" || anArgCase == "clear")
    {
      TColStd_SequenceOfInteger aLayers;
      aViewer->GetAllZLayers(aLayers);
      for (TColStd_SequenceOfInteger::Iterator aLayeriter(aLayers); aLayeriter.More();
           aLayeriter.Next())
      {
        if (aLayeriter.Value() == aLayer || aLayer == Graphic3d_ZLayerId_UNKNOWN)
        {
          Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings(aLayeriter.Value());
          aSettings.SetLights(Handle(Graphic3d_LightSet)());
          aViewer->SetZLayerSettings(aLayeriter.Value(), aSettings);
          if (aLayer != Graphic3d_ZLayerId_UNKNOWN)
          {
            break;
          }
        }
      }

      if (aLayer == Graphic3d_ZLayerId_UNKNOWN)
      {
        ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
        for (V3d_ListOfLightIterator aLightIter(aView->ActiveLightIterator()); aLightIter.More();)
        {
          Handle(V3d_Light)             aLight = aLightIter.Value();
          Handle(AIS_InteractiveObject) aPrsObject;
          GetMapOfAIS().Find2(aLight->Name(), aPrsObject);
          if (Handle(AIS_LightSource) aLightSourceDel =
                Handle(AIS_LightSource)::DownCast(aPrsObject))
          {
            aCtx->Remove(aLightSourceDel, false);
            aMap.UnBind1(aLightSourceDel);
          }
          aViewer->DelLight(aLight);
          aLightIter = aView->ActiveLightIterator();
        }
      }

      aLightOld.Nullify();
      aLightNew.Nullify();
      aLightPrs.Nullify();
    }
    else if (!aLightNew.IsNull()
             && (anArgCase == "-display" || anArgCase == "-disp" || anArgCase == "-presentation"
                 || anArgCase == "-prs"))
    {
      TCollection_AsciiString aLightName = aLightNew->Name();
      if (anArgIt + 1 < theArgsNb && theArgVec[anArgIt + 1][0] != '-')
      {
        // old syntax
        aLightName = theArgVec[++anArgIt];
        if (aLightNew->Name() != aLightName)
        {
          if (Handle(V3d_Light) anOtherLight = findLightSource(aLightName))
          {
            theDi << "Syntax error: light with name '" << aLightName << "' is already defined";
            return 1;
          }
          aLightNew->SetName(aLightName);
        }
      }
      if (aLightName.IsEmpty())
      {
        Message::SendFail() << "Error: nameless light source cannot be displayed";
        return 1;
      }
      if (aLightPrs.IsNull())
      {
        aLightPrs = new AIS_LightSource(aLightNew);
      }
      theDi << aLightName << " ";
    }
    else if (!aLightNew.IsNull()
             && (anArgCase == "-disable" || anArgCase == "-disabled" || anArgCase == "-enable"
                 || anArgCase == "-enabled"))
    {
      bool toEnable = Draw::ParseOnOffIterator(theArgsNb, theArgVec, anArgIt);
      if (anArgCase == "-disable" || anArgCase == "-disabled")
      {
        toEnable = !toEnable;
      }
      aLightNew->SetEnabled(toEnable);
    }
    else if (!aLightNew.IsNull()
             && (anArgCase == "-color" || anArgCase == "-colour" || anArgCase == "color"))
    {
      Quantity_Color   aColor;
      Standard_Integer aNbParsed =
        Draw::ParseColor(theArgsNb - anArgIt - 1, theArgVec + anArgIt + 1, aColor);
      anArgIt += aNbParsed;
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      aLightNew->SetColor(aColor);
    }
    else if (!aLightNew.IsNull()
             && (anArgCase == "-pos" || anArgCase == "-position" || anArgCase == "-prsposition"
                 || anArgCase == "-prspos" || anArgCase == "pos" || anArgCase == "position")
             && (anArgIt + 3) < theArgsNb)
    {
      gp_XYZ aPosXYZ;
      if (!parseXYZ(theArgVec + anArgIt + 1, aPosXYZ))
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      anArgIt += 3;
      if (anArgCase == "-prsposition" || anArgCase == "-prspos")
      {
        aLightNew->SetDisplayPosition(aPosXYZ);
      }
      else
      {
        if (aLightNew->Type() != Graphic3d_TypeOfLightSource_Positional
            && aLightNew->Type() != Graphic3d_TypeOfLightSource_Spot)
        {
          Message::SendFail() << "Syntax error at argument '" << anArg << "'";
          return 1;
        }

        aLightNew->SetPosition(aPosXYZ);
      }
    }
    else if (!aLightNew.IsNull()
             && (aLightNew->Type() == Graphic3d_TypeOfLightSource_Directional
                 || aLightNew->Type() == Graphic3d_TypeOfLightSource_Spot)
             && (anArgCase == "-dir" || anArgCase == "-direction") && (anArgIt + 3) < theArgsNb)
    {
      gp_XYZ aDirXYZ;
      if (!parseXYZ(theArgVec + anArgIt + 1, aDirXYZ))
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      anArgIt += 3;
      aLightNew->SetDirection(gp_Dir(aDirXYZ));
    }
    else if (!aLightNew.IsNull()
             && (anArgCase == "-smoothangle" || anArgCase == "-smoothradius" || anArgCase == "-sm"
                 || anArgCase == "-smoothness")
             && anArgIt + 1 < theArgsNb)
    {
      Standard_ShortReal aSmoothness = (Standard_ShortReal)Atof(theArgVec[++anArgIt]);
      if (aLightNew->Type() == Graphic3d_TypeOfLightSource_Directional)
      {
        aSmoothness = Standard_ShortReal(aSmoothness * M_PI / 180.0);
      }
      if (Abs(aSmoothness) <= ShortRealEpsilon())
      {
        aLightNew->SetIntensity(1.f);
      }
      else if (Abs(aLightNew->Smoothness()) <= ShortRealEpsilon())
      {
        aLightNew->SetIntensity((aSmoothness * aSmoothness) / 3.f);
      }
      else
      {
        Standard_ShortReal aSmoothnessRatio =
          static_cast<Standard_ShortReal>(aSmoothness / aLightNew->Smoothness());
        aLightNew->SetIntensity(aLightNew->Intensity() / (aSmoothnessRatio * aSmoothnessRatio));
      }

      if (aLightNew->Type() == Graphic3d_TypeOfLightSource_Positional)
      {
        aLightNew->SetSmoothRadius(aSmoothness);
      }
      else if (aLightNew->Type() == Graphic3d_TypeOfLightSource_Directional)
      {
        aLightNew->SetSmoothAngle(aSmoothness);
      }
    }
    else if (!aLightNew.IsNull() && (anArgCase == "-int" || anArgCase == "-intensity")
             && anArgIt + 1 < theArgsNb)
    {
      Standard_ShortReal aIntensity = (Standard_ShortReal)Atof(theArgVec[++anArgIt]);
      aLightNew->SetIntensity(aIntensity);
    }
    else if (!aLightNew.IsNull() && aLightNew->Type() == Graphic3d_TypeOfLightSource_Spot
             && (anArgCase == "-spotangle" || anArgCase == "-ang" || anArgCase == "-angle")
             && anArgIt + 1 < theArgsNb)
    {
      Standard_ShortReal anAngle = (Standard_ShortReal)Atof(theArgVec[++anArgIt]);
      anAngle                    = (Standard_ShortReal(anAngle / 180.0 * M_PI));
      aLightNew->SetAngle(anAngle);
    }
    else if (!aLightNew.IsNull()
             && (aLightNew->Type() == Graphic3d_TypeOfLightSource_Positional
                 || aLightNew->Type() == Graphic3d_TypeOfLightSource_Spot)
             && (anArgCase == "-constatten" || anArgCase == "-constattenuation")
             && anArgIt + 1 < theArgsNb)
    {
      const Standard_ShortReal aConstAtten = (Standard_ShortReal)Atof(theArgVec[++anArgIt]);
      aLightNew->SetAttenuation(aConstAtten, aLightNew->LinearAttenuation());
    }
    else if (!aLightNew.IsNull()
             && (aLightNew->Type() == Graphic3d_TypeOfLightSource_Positional
                 || aLightNew->Type() == Graphic3d_TypeOfLightSource_Spot)
             && (anArgCase == "-linatten" || anArgCase == "-linearatten"
                 || anArgCase == "-linearattenuation")
             && anArgIt + 1 < theArgsNb)
    {
      const Standard_ShortReal aLinAtten = (Standard_ShortReal)Atof(theArgVec[++anArgIt]);
      aLightNew->SetAttenuation(aLightNew->ConstAttenuation(), aLinAtten);
    }
    else if (!aLightNew.IsNull() && aLightNew->Type() == Graphic3d_TypeOfLightSource_Spot
             && (anArgCase == "-spotexp" || anArgCase == "-spotexponent" || anArgCase == "-exp"
                 || anArgCase == "-exponent")
             && anArgIt + 1 < theArgsNb)
    {
      aLightNew->SetConcentration((Standard_ShortReal)Atof(theArgVec[++anArgIt]));
    }
    else if (!aLightNew.IsNull() && aLightNew->Type() != Graphic3d_TypeOfLightSource_Ambient
             && aLightNew->Type() != Graphic3d_TypeOfLightSource_Directional
             && anArgCase == "-range" && anArgIt + 1 < theArgsNb)
    {
      Standard_ShortReal aRange((Standard_ShortReal)Atof(theArgVec[++anArgIt]));
      aLightNew->SetRange(aRange);
    }
    else if (!aLightNew.IsNull() && aLightNew->Type() != Graphic3d_TypeOfLightSource_Ambient
             && (anArgCase == "-head" || anArgCase == "-headlight"))
    {
      Standard_Boolean isHeadLight = Standard_True;
      if (anArgIt + 1 < theArgsNb && Draw::ParseOnOff(theArgVec[anArgIt + 1], isHeadLight))
      {
        ++anArgIt;
      }
      aLightNew->SetHeadlight(isHeadLight);
    }
    else if (!aLightNew.IsNull() && anArgCase == "-name" && anArgIt + 1 < theArgsNb)
    {
      const TCollection_AsciiString aName = theArgVec[++anArgIt];
      if (aLightNew->Name() == aName)
      {
        continue;
      }

      if (Handle(V3d_Light) anOtherLight = findLightSource(aName))
      {
        theDi << "Syntax error: light with name '" << aName << "' is already defined";
        return 1;
      }
      aLightNew->SetName(aName);
    }
    else if (!aLightPrs.IsNull()
             && (anArgCase == "-showzoomable" || anArgCase == "-prszoomable"
                 || anArgCase == "-zoomable"))
    {
      const bool isZoomable = Draw::ParseOnOffIterator(theArgsNb, theArgVec, anArgIt);
      aLightPrs->SetZoomable(isZoomable);
    }
    else if (!aLightPrs.IsNull()
             && (anArgCase == "-showdraggable" || anArgCase == "-prsdraggable"
                 || anArgCase == "-draggable"))
    {
      const bool isDraggable = Draw::ParseOnOffIterator(theArgsNb, theArgVec, anArgIt);
      aLightPrs->SetDraggable(isDraggable);
    }
    else if (!aLightPrs.IsNull() && (anArgCase == "-showname" || anArgCase == "-prsname"))
    {
      const bool toDisplay = Draw::ParseOnOffIterator(theArgsNb, theArgVec, anArgIt);
      aLightPrs->SetDisplayName(toDisplay);
    }
    else if (!aLightPrs.IsNull()
             && (aLightNew->Type() == Graphic3d_TypeOfLightSource_Spot
                 || aLightNew->Type() == Graphic3d_TypeOfLightSource_Positional)
             && (anArgCase == "-showrange" || anArgCase == "-prsrange"))
    {
      const bool toDisplay = Draw::ParseOnOffIterator(theArgsNb, theArgVec, anArgIt);
      aLightPrs->SetDisplayRange(toDisplay);
    }
    else if (!aLightPrs.IsNull() && (anArgCase == "-showsize" || anArgCase == "-prssize")
             && anArgIt + 1 < theArgsNb)
    {
      Standard_Real aSize = 0.0;
      if (!Draw::ParseReal(theArgVec[++anArgIt], aSize) || aSize <= 0.0 || aLightPrs.IsNull())
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      aLightPrs->SetSize(aSize);
    }
    else if (!aLightPrs.IsNull()
             && (anArgCase == "-dirarcsize" || anArgCase == "-arcsize" || anArgCase == "-arc")
             && anArgIt + 1 < theArgsNb)
    {
      Standard_Integer aSize = 0;
      if (!Draw::ParseInteger(theArgVec[anArgIt + 1], aSize) || aSize <= 0
          || aLightPrs->Light()->Type() != Graphic3d_TypeOfLightSource_Directional)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      ++anArgIt;
      aLightPrs->SetArcSize(aSize);
    }
    else if (!aLightNew.IsNull() && aLightNew->Type() != Graphic3d_TypeOfLightSource_Ambient
             && (anArgCase == "-castshadow" || anArgCase == "-castshadows"
                 || anArgCase == "-shadows"))
    {
      bool toCastShadows = true;
      if (anArgIt + 1 < theArgsNb && Draw::ParseOnOff(theArgVec[anArgIt + 1], toCastShadows))
      {
        ++anArgIt;
      }
      aLightNew->SetCastShadows(toCastShadows);
    }
    else if (anArgCase == "-del" || anArgCase == "-delete" || anArgCase == "-remove"
             || anArgCase == "del" || anArgCase == "delete" || anArgCase == "remove")
    {
      if (aLightOld.IsNull())
      {
        if (!aLightNew.IsNull())
        {
          aLightNew.Nullify();
          continue;
        }

        if (++anArgIt >= theArgsNb)
        {
          Message::SendFail() << "Syntax error at argument '" << anArg << "'";
          return 1;
        }

        const TCollection_AsciiString anOldName(theArgVec[anArgIt]);
        aLightOld = findLightSource(anOldName);
        if (aLightOld.IsNull())
        {
          Message::SendWarning() << "Warning: light '" << anOldName << "' not found";
          continue;
        }
      }

      aLightNew.Nullify();
      aLightPrs.Nullify();
    }
    else if (anArgCase == "-change" || anArgCase == "change")
    {
      // just skip old syntax
    }
    else if (aLightNew.IsNull() && !anArgCase.StartsWith("-"))
    {
      if (!aLightNew.IsNull())
      {
        continue;
      }

      const TCollection_AsciiString anOldName(theArgVec[anArgIt]);
      aLightOld = findLightSource(anOldName);
      if (!aLightOld.IsNull())
      {
        aLightNew = aLightOld;

        Handle(AIS_InteractiveObject) aPrsObject;
        GetMapOfAIS().Find2(aLightOld->Name(), aPrsObject);
        aLightPrs = Handle(AIS_LightSource)::DownCast(aPrsObject);
      }
      else
      {
        Standard_Integer aLightIndex = -1;
        Draw::ParseInteger(anOldName.ToCString(), aLightIndex);
        if (aLightIndex != -1)
        {
          Message::SendFail() << "Syntax error: light source with index '" << aLightIndex
                              << "' is not found";
          return 1;
        }

        aLightNew = new V3d_AmbientLight();
        aLightNew->SetName(anOldName);
      }
    }
    else
    {
      Message::SendFail() << "Warning: unknown argument '" << anArg << "'";
      return 1;
    }
  }

  // delete old light source
  if (!aLightOld.IsNull() && aLightOld != aLightNew)
  {
    TColStd_SequenceOfInteger aLayers;
    aViewer->GetAllZLayers(aLayers);
    for (TColStd_SequenceOfInteger::Iterator aLayerIter(aLayers); aLayerIter.More();
         aLayerIter.Next())
    {
      if (aLayerIter.Value() == aLayer || aLayer == Graphic3d_ZLayerId_UNKNOWN)
      {
        Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings(aLayerIter.Value());
        if (!aSettings.Lights().IsNull())
        {
          aSettings.Lights()->Remove(aLightOld);
          if (aSettings.Lights()->IsEmpty())
          {
            aSettings.SetLights(Handle(Graphic3d_LightSet)());
          }
        }
        aViewer->SetZLayerSettings(aLayerIter.Value(), aSettings);
        if (aLayer != Graphic3d_ZLayerId_UNKNOWN)
        {
          break;
        }
      }
    }

    if (aLayer == Graphic3d_ZLayerId_UNKNOWN)
    {
      Handle(AIS_InteractiveObject) aPrsObject;
      GetMapOfAIS().Find2(aLightOld->Name(), aPrsObject);
      if (Handle(AIS_LightSource) aLightSourceDel = Handle(AIS_LightSource)::DownCast(aPrsObject))
      {
        aCtx->Remove(aLightSourceDel, false);
        GetMapOfAIS().UnBind1(aLightSourceDel);
      }
      aViewer->DelLight(aLightOld);
    }
    aLightOld.Nullify();
  }

  // add new light source
  if (!aLightNew.IsNull())
  {
    if (aLayer == Graphic3d_ZLayerId_UNKNOWN)
    {
      aViewer->AddLight(aLightNew);
      if (isGlobal)
      {
        aViewer->SetLightOn(aLightNew);
      }
      else
      {
        aView->SetLightOn(aLightNew);
      }
    }
    else
    {
      Graphic3d_ZLayerSettings aSettings = aViewer->ZLayerSettings(aLayer);
      if (aSettings.Lights().IsNull())
      {
        aSettings.SetLights(new Graphic3d_LightSet());
      }
      aSettings.Lights()->Add(aLightNew);
      aViewer->SetZLayerSettings(aLayer, aSettings);
    }

    if (!aLightPrs.IsNull())
    {
      aLightPrs->SetLight(aLightNew);
      ViewerTest::Display(aLightNew->Name(), aLightPrs, false);
    }
  }

  // manage presentations
  struct LightPrsSort
  {
    bool operator()(const Handle(AIS_LightSource)& theLeft, const Handle(AIS_LightSource)& theRight)
    {
      return theLeft->Light()->GetId() < theRight->Light()->GetId();
    }
  };

  AIS_ListOfInteractive aPrsList;
  aCtx->DisplayedObjects(AIS_KindOfInteractive_LightSource, -1, aPrsList);
  if (!aPrsList.IsEmpty())
  {
    // update light source presentations
    std::vector<Handle(AIS_LightSource)> aLightPrsVec;
    for (AIS_ListOfInteractive::Iterator aPrsIter(aPrsList); aPrsIter.More(); aPrsIter.Next())
    {
      if (Handle(AIS_LightSource) aLightPrs2 = Handle(AIS_LightSource)::DownCast(aPrsIter.Value()))
      {
        aLightPrsVec.push_back(aLightPrs2);
      }
    }

    // sort objects by id as AIS_InteractiveContext stores them in unordered map
    std::sort(aLightPrsVec.begin(), aLightPrsVec.end(), LightPrsSort());

    Standard_Integer aTopStack = 0;
    for (std::vector<Handle(AIS_LightSource)>::iterator aPrsIter = aLightPrsVec.begin();
         aPrsIter != aLightPrsVec.end();
         ++aPrsIter)
    {
      Handle(AIS_LightSource) aLightPrs2 = *aPrsIter;
      if (!aLightPrs2->TransformPersistence().IsNull()
          && aLightPrs2->TransformPersistence()->IsTrihedronOr2d())
      {
        const Standard_Integer aPrsSize = (Standard_Integer)aLightPrs2->Size();
        aLightPrs2->TransformPersistence()->SetOffset2d(
          Graphic3d_Vec2i(aTopStack + aPrsSize, aPrsSize));
        aTopStack += aPrsSize + aPrsSize / 2;
      }
      aCtx->Redisplay(aLightPrs2, false);
      aCtx->SetTransformPersistence(aLightPrs2, aLightPrs2->TransformPersistence());
    }
  }
  return 0;
}

//=================================================================================================

static int VPBREnvironment(Draw_Interpretor&, Standard_Integer theArgsNb, const char** theArgVec)
{
  if (theArgsNb > 2)
  {
    Message::SendFail("Syntax error: 'vpbrenv' command has only one argument");
    return 1;
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  TCollection_AsciiString anArg = TCollection_AsciiString(theArgVec[1]);
  anArg.LowerCase();

  if (anArg == "-generate" || anArg == "-gen")
  {
    aView->GeneratePBREnvironment(Standard_True);
  }
  else if (anArg == "-clear")
  {
    aView->ClearPBREnvironment(Standard_True);
  }
  else
  {
    Message::SendFail() << "Syntax error: unknown argument [" << theArgVec[1]
                        << "] for 'vpbrenv' command";
    return 1;
  }

  return 0;
}

//! Read Graphic3d_RenderingParams::PerfCounters flag.
static Standard_Boolean parsePerfStatsFlag(const TCollection_AsciiString&           theValue,
                                           Standard_Boolean&                        theToReset,
                                           Graphic3d_RenderingParams::PerfCounters& theFlagsRem,
                                           Graphic3d_RenderingParams::PerfCounters& theFlagsAdd)
{
  Graphic3d_RenderingParams::PerfCounters aFlag     = Graphic3d_RenderingParams::PerfCounters_NONE;
  TCollection_AsciiString                 aVal      = theValue;
  Standard_Boolean                        toReverse = Standard_False;
  if (aVal == "none")
  {
    theToReset = Standard_True;
    return Standard_True;
  }
  else if (aVal.StartsWith("-"))
  {
    toReverse = Standard_True;
    aVal      = aVal.SubString(2, aVal.Length());
  }
  else if (aVal.StartsWith("no"))
  {
    toReverse = Standard_True;
    aVal      = aVal.SubString(3, aVal.Length());
  }
  else if (aVal.StartsWith("+"))
  {
    aVal = aVal.SubString(2, aVal.Length());
  }
  else
  {
    theToReset = Standard_True;
  }

  if (aVal == "fps" || aVal == "framerate")
    aFlag = Graphic3d_RenderingParams::PerfCounters_FrameRate;
  else if (aVal == "cpu")
    aFlag = Graphic3d_RenderingParams::PerfCounters_CPU;
  else if (aVal == "layers")
    aFlag = Graphic3d_RenderingParams::PerfCounters_Layers;
  else if (aVal == "structs" || aVal == "structures" || aVal == "objects")
    aFlag = Graphic3d_RenderingParams::PerfCounters_Structures;
  else if (aVal == "groups")
    aFlag = Graphic3d_RenderingParams::PerfCounters_Groups;
  else if (aVal == "arrays")
    aFlag = Graphic3d_RenderingParams::PerfCounters_GroupArrays;
  else if (aVal == "tris" || aVal == "triangles")
    aFlag = Graphic3d_RenderingParams::PerfCounters_Triangles;
  else if (aVal == "pnts" || aVal == "points")
    aFlag = Graphic3d_RenderingParams::PerfCounters_Points;
  else if (aVal == "lines")
    aFlag = Graphic3d_RenderingParams::PerfCounters_Lines;
  else if (aVal == "mem" || aVal == "gpumem" || aVal == "estimmem")
    aFlag = Graphic3d_RenderingParams::PerfCounters_EstimMem;
  else if (aVal == "skipimmediate" || aVal == "noimmediate")
    aFlag = Graphic3d_RenderingParams::PerfCounters_SkipImmediate;
  else if (aVal == "frametime" || aVal == "frametimers" || aVal == "time")
    aFlag = Graphic3d_RenderingParams::PerfCounters_FrameTime;
  else if (aVal == "basic")
    aFlag = Graphic3d_RenderingParams::PerfCounters_Basic;
  else if (aVal == "extended" || aVal == "verbose" || aVal == "extra")
    aFlag = Graphic3d_RenderingParams::PerfCounters_Extended;
  else if (aVal == "full" || aVal == "all")
    aFlag = Graphic3d_RenderingParams::PerfCounters_All;
  else
  {
    return Standard_False;
  }

  if (toReverse)
  {
    theFlagsRem = Graphic3d_RenderingParams::PerfCounters(theFlagsRem | aFlag);
  }
  else
  {
    theFlagsAdd = Graphic3d_RenderingParams::PerfCounters(theFlagsAdd | aFlag);
  }
  return Standard_True;
}

//! Read Graphic3d_RenderingParams::PerfCounters flags.
static Standard_Boolean convertToPerfStatsFlags(const TCollection_AsciiString&           theValue,
                                                Graphic3d_RenderingParams::PerfCounters& theFlags)
{
  TCollection_AsciiString                 aValue    = theValue;
  Graphic3d_RenderingParams::PerfCounters aFlagsRem = Graphic3d_RenderingParams::PerfCounters_NONE;
  Graphic3d_RenderingParams::PerfCounters aFlagsAdd = Graphic3d_RenderingParams::PerfCounters_NONE;
  Standard_Boolean                        toReset   = Standard_False;
  for (;;)
  {
    Standard_Integer aSplitPos = aValue.Search("|");
    if (aSplitPos <= 0)
    {
      if (!parsePerfStatsFlag(aValue, toReset, aFlagsRem, aFlagsAdd))
      {
        return Standard_False;
      }
      if (toReset)
      {
        theFlags = Graphic3d_RenderingParams::PerfCounters_NONE;
      }
      theFlags = Graphic3d_RenderingParams::PerfCounters(theFlags | aFlagsAdd);
      theFlags = Graphic3d_RenderingParams::PerfCounters(theFlags & ~aFlagsRem);
      return Standard_True;
    }

    if (aSplitPos > 1)
    {
      TCollection_AsciiString aSubValue = aValue.SubString(1, aSplitPos - 1);
      if (!parsePerfStatsFlag(aSubValue, toReset, aFlagsRem, aFlagsAdd))
      {
        return Standard_False;
      }
    }
    aValue = aValue.SubString(aSplitPos + 1, aValue.Length());
  }
}

//=======================================================================
// function : VRenderParams
// purpose  : Enables/disables rendering features
//=======================================================================

static Standard_Integer VRenderParams(Draw_Interpretor& theDI,
                                      Standard_Integer  theArgNb,
                                      const char**      theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Graphic3d_RenderingParams& aParams = aView->ChangeRenderingParams();
  TCollection_AsciiString    aCmdName(theArgVec[0]);
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
      TCollection_AsciiString aValue(theArgVec[1]);
      aValue.LowerCase();
      if (aValue == "on" || aValue == "1")
      {
        aParams.Method = Graphic3d_RM_RAYTRACING;
        aView->Redraw();
        return 0;
      }
      else if (aValue == "off" || aValue == "0")
      {
        aParams.Method = Graphic3d_RM_RASTERIZATION;
        aView->Redraw();
        return 0;
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown argument '" << theArgVec[1] << "'";
        return 1;
      }
    }
    else
    {
      Message::SendFail("Syntax error: wrong number of arguments");
      return 1;
    }
  }

  if (theArgNb < 2)
  {
    theDI << "renderMode:  ";
    switch (aParams.Method)
    {
      case Graphic3d_RM_RASTERIZATION:
        theDI << "rasterization ";
        break;
      case Graphic3d_RM_RAYTRACING:
        theDI << "raytrace ";
        break;
    }
    theDI << "\n";
    theDI << "transparency:  ";
    switch (aParams.TransparencyMethod)
    {
      case Graphic3d_RTM_BLEND_UNORDERED:
        theDI << "Basic blended transparency with non-commuting operator ";
        break;
      case Graphic3d_RTM_BLEND_OIT:
        theDI << "Weighted Blended Order-Independent Transparency, depth weight factor: "
              << TCollection_AsciiString(aParams.OitDepthFactor);
        break;
      case Graphic3d_RTM_DEPTH_PEELING_OIT:
        theDI << "Depth Peeling Order-Independent Transparency, Nb.Layers: "
              << TCollection_AsciiString(aParams.NbOitDepthPeelingLayers);
        break;
    }
    theDI << "\n";
    theDI << "msaa:           " << aParams.NbMsaaSamples << "\n";
    theDI << "rendScale:      " << aParams.RenderResolutionScale << "\n";
    theDI << "rayDepth:       " << aParams.RaytracingDepth << "\n";
    theDI << "fsaa:           " << (aParams.IsAntialiasingEnabled ? "on" : "off") << "\n";
    theDI << "shadows:        " << (aParams.IsShadowEnabled ? "on" : "off") << "\n";
    theDI << "shadowMapRes:   " << aParams.ShadowMapResolution << "\n";
    theDI << "shadowMapBias:  " << aParams.ShadowMapBias << "\n";
    theDI << "reflections:    " << (aParams.IsReflectionEnabled ? "on" : "off") << "\n";
    theDI << "gleam:          " << (aParams.IsTransparentShadowEnabled ? "on" : "off") << "\n";
    theDI << "GI:             " << (aParams.IsGlobalIlluminationEnabled ? "on" : "off") << "\n";
    theDI << "blocked RNG:    " << (aParams.CoherentPathTracingMode ? "on" : "off") << "\n";
    theDI << "iss:            " << (aParams.AdaptiveScreenSampling ? "on" : "off") << "\n";
    theDI << "iss debug:      " << (aParams.ShowSamplingTiles ? "on" : "off") << "\n";
    theDI << "two-sided BSDF: " << (aParams.TwoSidedBsdfModels ? "on" : "off") << "\n";
    theDI << "max radiance:   " << aParams.RadianceClampingValue << "\n";
    theDI << "nb tiles (iss): " << aParams.NbRayTracingTiles << "\n";
    theDI << "tile size (iss):" << aParams.RayTracingTileSize << "x" << aParams.RayTracingTileSize
          << "\n";
    theDI << "shadingModel: ";
    switch (aView->ShadingModel())
    {
      case Graphic3d_TypeOfShadingModel_DEFAULT:
        theDI << "default";
        break;
      case Graphic3d_TypeOfShadingModel_Unlit:
        theDI << "unlit";
        break;
      case Graphic3d_TypeOfShadingModel_PhongFacet:
        theDI << "flat";
        break;
      case Graphic3d_TypeOfShadingModel_Gouraud:
        theDI << "gouraud";
        break;
      case Graphic3d_TypeOfShadingModel_Phong:
        theDI << "phong";
        break;
      case Graphic3d_TypeOfShadingModel_Pbr:
        theDI << "pbr";
        break;
      case Graphic3d_TypeOfShadingModel_PbrFacet:
        theDI << "pbr_facet";
        break;
    }
    theDI << "\n";
    {
      theDI << "perfCounters:";
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_FrameRate) != 0)
      {
        theDI << " fps";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_CPU) != 0)
      {
        theDI << " cpu";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_Structures) != 0)
      {
        theDI << " structs";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_Groups) != 0)
      {
        theDI << " groups";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_GroupArrays) != 0)
      {
        theDI << " arrays";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_Triangles) != 0)
      {
        theDI << " tris";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_Lines) != 0)
      {
        theDI << " lines";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_Points) != 0)
      {
        theDI << " pnts";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_EstimMem) != 0)
      {
        theDI << " gpumem";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_FrameTime) != 0)
      {
        theDI << " frameTime";
      }
      if ((aParams.CollectedStats & Graphic3d_RenderingParams::PerfCounters_SkipImmediate) != 0)
      {
        theDI << " skipimmediate";
      }
      if (aParams.CollectedStats == Graphic3d_RenderingParams::PerfCounters_NONE)
      {
        theDI << " none";
      }
      theDI << "\n";
    }
    theDI << "depth pre-pass: " << (aParams.ToEnableDepthPrepass ? "on" : "off") << "\n";
    theDI << "alpha to coverage: " << (aParams.ToEnableAlphaToCoverage ? "on" : "off") << "\n";
    theDI << "frustum culling: "
          << (aParams.FrustumCullingState == Graphic3d_RenderingParams::FrustumCulling_On ? "on"
              : aParams.FrustumCullingState == Graphic3d_RenderingParams::FrustumCulling_Off
                ? "off"
                : "noUpdate")
          << "\n";
    theDI << "\n";
    return 0;
  }

  bool                   toPrint = false, toSyncDefaults = false, toSyncAllViews = false;
  ViewerTest_AutoUpdater anUpdateTool(ViewerTest::GetAISContext(), aView);
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg(theArgVec[anArgIter]);
    TCollection_AsciiString aFlag(anArg);
    aFlag.LowerCase();
    if (anUpdateTool.parseRedrawMode(aFlag))
    {
      continue;
    }
    else if (aFlag == "-echo" || aFlag == "-print")
    {
      toPrint = Standard_True;
      anUpdateTool.Invalidate();
    }
    else if (aFlag == "-reset")
    {
      aParams = ViewerTest::GetViewerFromContext()->DefaultRenderingParams();
    }
    else if (aFlag == "-sync" && (anArgIter + 1 < theArgNb))
    {
      TCollection_AsciiString aSyncFlag(theArgVec[++anArgIter]);
      aSyncFlag.LowerCase();
      if (aSyncFlag == "default" || aSyncFlag == "defaults" || aSyncFlag == "viewer")
      {
        toSyncDefaults = true;
      }
      else if (aSyncFlag == "allviews" || aSyncFlag == "views")
      {
        toSyncAllViews = true;
      }
      else
      {
        Message::SendFail("Syntax error: unknown parameter to -sync argument");
        return 1;
      }
    }
    else if (aFlag == "-mode" || aFlag == "-rendermode" || aFlag == "-render_mode")
    {
      if (toPrint)
      {
        switch (aParams.Method)
        {
          case Graphic3d_RM_RASTERIZATION:
            theDI << "rasterization ";
            break;
          case Graphic3d_RM_RAYTRACING:
            theDI << "ray-tracing ";
            break;
        }
        continue;
      }
      else
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-ray" || aFlag == "-raytrace")
    {
      if (toPrint)
      {
        theDI << (aParams.Method == Graphic3d_RM_RAYTRACING ? "true" : "false") << " ";
        continue;
      }

      bool isRayTrace = true;
      if (anArgIter + 1 < theArgNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], isRayTrace))
      {
        ++anArgIter;
      }
      aParams.Method = isRayTrace ? Graphic3d_RM_RAYTRACING : Graphic3d_RM_RASTERIZATION;
    }
    else if (aFlag == "-rast" || aFlag == "-raster" || aFlag == "-rasterization")
    {
      if (toPrint)
      {
        theDI << (aParams.Method == Graphic3d_RM_RASTERIZATION ? "true" : "false") << " ";
        continue;
      }

      bool isRaster = true;
      if (anArgIter + 1 < theArgNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], isRaster))
      {
        ++anArgIter;
      }
      aParams.Method = isRaster ? Graphic3d_RM_RASTERIZATION : Graphic3d_RM_RAYTRACING;
    }
    else if (aFlag == "-msaa")
    {
      if (toPrint)
      {
        theDI << aParams.NbMsaaSamples << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Integer aNbSamples = Draw::Atoi(theArgVec[anArgIter]);
      if (aNbSamples < 0)
      {
        Message::SendFail() << "Syntax error: invalid number of MSAA samples " << aNbSamples << "";
        return 1;
      }
      else
      {
        aParams.NbMsaaSamples = aNbSamples;
      }
    }
    else if (aFlag == "-linefeather" || aFlag == "-edgefeather" || aFlag == "-feather")
    {
      if (toPrint)
      {
        theDI << " " << aParams.LineFeather << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString  aParam   = theArgVec[anArgIter];
      const Standard_ShortReal aFeather = (Standard_ShortReal)Draw::Atof(theArgVec[anArgIter]);
      if (aFeather <= 0.0f)
      {
        Message::SendFail() << "Syntax error: invalid value of line width feather " << aFeather
                            << ". Should be > 0";
        return 1;
      }
      aParams.LineFeather = aFeather;
    }
    else if (aFlag == "-oit")
    {
      if (toPrint)
      {
        if (aParams.TransparencyMethod == Graphic3d_RTM_BLEND_OIT)
        {
          theDI << "on, depth weight factor: " << TCollection_AsciiString(aParams.OitDepthFactor)
                << " ";
        }
        else if (aParams.TransparencyMethod == Graphic3d_RTM_DEPTH_PEELING_OIT)
        {
          theDI << "on, depth peeling layers: "
                << TCollection_AsciiString(aParams.NbOitDepthPeelingLayers) << " ";
        }
        else
        {
          theDI << "off" << " ";
        }
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aParam = theArgVec[anArgIter];
      aParam.LowerCase();
      if (aParam == "peeling" || aParam == "peel")
      {
        aParams.TransparencyMethod = Graphic3d_RTM_DEPTH_PEELING_OIT;
        if (anArgIter + 1 < theArgNb
            && TCollection_AsciiString(theArgVec[anArgIter + 1]).IsIntegerValue())
        {
          ++anArgIter;
          const Standard_Integer aNbLayers =
            TCollection_AsciiString(theArgVec[anArgIter]).IntegerValue();
          if (aNbLayers < 2)
          {
            Message::SendFail()
              << "Syntax error: invalid layers number specified for Depth Peeling OIT "
              << aNbLayers;
            return 1;
          }
          aParams.NbOitDepthPeelingLayers =
            TCollection_AsciiString(theArgVec[anArgIter]).IntegerValue();
        }
      }
      else if (aParam == "weighted" || aParam == "weight")
      {
        aParams.TransparencyMethod = Graphic3d_RTM_BLEND_OIT;
        if (anArgIter + 1 < theArgNb
            && TCollection_AsciiString(theArgVec[anArgIter + 1]).IsRealValue())
        {
          ++anArgIter;
          const Standard_ShortReal aWeight =
            (Standard_ShortReal)TCollection_AsciiString(theArgVec[anArgIter]).RealValue();
          if (aWeight < 0.f || aWeight > 1.f)
          {
            Message::SendFail() << "Syntax error: invalid value of Weighted Order-Independent "
                                   "Transparency depth weight factor "
                                << aWeight << ". Should be within range [0.0; 1.0]";
            return 1;
          }
          aParams.OitDepthFactor = aWeight;
        }
      }
      else if (aParam.IsRealValue())
      {
        const Standard_ShortReal aWeight = (Standard_ShortReal)Draw::Atof(theArgVec[anArgIter]);
        if (aWeight < 0.f || aWeight > 1.f)
        {
          Message::SendFail() << "Syntax error: invalid value of Weighted Order-Independent "
                                 "Transparency depth weight factor "
                              << aWeight << ". Should be within range [0.0; 1.0]";
          return 1;
        }

        aParams.TransparencyMethod = Graphic3d_RTM_BLEND_OIT;
        aParams.OitDepthFactor     = aWeight;
      }
      else if (aParam == "off")
      {
        aParams.TransparencyMethod = Graphic3d_RTM_BLEND_UNORDERED;
      }
      else
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-fonthinting" || aFlag == "-fonthint")
    {
      if (toPrint)
      {
        if ((aParams.FontHinting & Font_Hinting_Normal) != 0)
        {
          theDI << "normal" << " ";
        }
        else if ((aParams.FontHinting & Font_Hinting_Normal) != 0)
        {
          theDI << "light" << " ";
        }
        else
        {
          theDI << "off" << " ";
        }
        continue;
      }
      else if (anArgIter + 1 >= theArgNb)
      {
        theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }

      TCollection_AsciiString aHintStyle(theArgVec[++anArgIter]);
      aHintStyle.LowerCase();
      if (aHintStyle == "normal" || aHintStyle == "on" || aHintStyle == "1")
      {
        aParams.FontHinting = Font_Hinting(aParams.FontHinting & ~Font_Hinting_Light);
        aParams.FontHinting = Font_Hinting(aParams.FontHinting | Font_Hinting_Normal);
      }
      else if (aHintStyle == "light")
      {
        aParams.FontHinting = Font_Hinting(aParams.FontHinting & ~Font_Hinting_Normal);
        aParams.FontHinting = Font_Hinting(aParams.FontHinting | Font_Hinting_Light);
      }
      else if (aHintStyle == "no" || aHintStyle == "off" || aHintStyle == "0")
      {
        aParams.FontHinting = Font_Hinting(aParams.FontHinting & ~Font_Hinting_Normal);
        aParams.FontHinting = Font_Hinting(aParams.FontHinting & ~Font_Hinting_Light);
      }
      else
      {
        theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }
    }
    else if (aFlag == "-fontautohinting" || aFlag == "-fontautohint")
    {
      if (toPrint)
      {
        if ((aParams.FontHinting & Font_Hinting_ForceAutohint) != 0)
        {
          theDI << "force" << " ";
        }
        else if ((aParams.FontHinting & Font_Hinting_NoAutohint) != 0)
        {
          theDI << "disallow" << " ";
        }
        else
        {
          theDI << "auto" << " ";
        }
        continue;
      }
      else if (anArgIter + 1 >= theArgNb)
      {
        theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }

      TCollection_AsciiString aHintStyle(theArgVec[++anArgIter]);
      aHintStyle.LowerCase();
      if (aHintStyle == "force")
      {
        aParams.FontHinting = Font_Hinting(aParams.FontHinting & ~Font_Hinting_NoAutohint);
        aParams.FontHinting = Font_Hinting(aParams.FontHinting | Font_Hinting_ForceAutohint);
      }
      else if (aHintStyle == "disallow" || aHintStyle == "no")
      {
        aParams.FontHinting = Font_Hinting(aParams.FontHinting & ~Font_Hinting_ForceAutohint);
        aParams.FontHinting = Font_Hinting(aParams.FontHinting | Font_Hinting_NoAutohint);
      }
      else if (aHintStyle == "auto")
      {
        aParams.FontHinting = Font_Hinting(aParams.FontHinting & ~Font_Hinting_ForceAutohint);
        aParams.FontHinting = Font_Hinting(aParams.FontHinting & ~Font_Hinting_NoAutohint);
      }
      else
      {
        theDI << "Syntax error at '" << theArgVec[anArgIter] << "'";
        return 1;
      }
    }
    else if (aFlag == "-depthprepass")
    {
      if (toPrint)
      {
        theDI << (aParams.ToEnableDepthPrepass ? "on " : "off ");
        continue;
      }
      aParams.ToEnableDepthPrepass = Standard_True;
      if (anArgIter + 1 < theArgNb
          && Draw::ParseOnOff(theArgVec[anArgIter + 1], aParams.ToEnableDepthPrepass))
      {
        ++anArgIter;
      }
    }
    else if (aFlag == "-samplealphatocoverage" || aFlag == "-alphatocoverage")
    {
      if (toPrint)
      {
        theDI << (aParams.ToEnableAlphaToCoverage ? "on " : "off ");
        continue;
      }
      aParams.ToEnableAlphaToCoverage = Standard_True;
      if (anArgIter + 1 < theArgNb
          && Draw::ParseOnOff(theArgVec[anArgIter + 1], aParams.ToEnableAlphaToCoverage))
      {
        ++anArgIter;
      }
    }
    else if (aFlag == "-rendscale" || aFlag == "-renderscale" || aFlag == "-renderresolutionscale")
    {
      if (toPrint)
      {
        theDI << aParams.RenderResolutionScale << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Real aScale = Draw::Atof(theArgVec[anArgIter]);
      if (aScale < 0.01)
      {
        Message::SendFail() << "Syntax error: invalid rendering resolution scale " << aScale << "";
        return 1;
      }
      else
      {
        aParams.RenderResolutionScale = Standard_ShortReal(aScale);
      }
    }
    else if (aFlag == "-raydepth" || aFlag == "-ray_depth")
    {
      if (toPrint)
      {
        theDI << aParams.RaytracingDepth << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Integer aDepth = Draw::Atoi(theArgVec[anArgIter]);

      // We allow RaytracingDepth be more than 10 in case of GI enabled
      if (aDepth < 1 || (aDepth > 10 && !aParams.IsGlobalIlluminationEnabled))
      {
        Message::SendFail() << "Syntax error: invalid ray-tracing depth " << aDepth
                            << ". Should be within range [1; 10]";
        return 1;
      }
      else
      {
        aParams.RaytracingDepth = aDepth;
      }
    }
    else if (aFlag == "-shad" || aFlag == "-shadows")
    {
      if (toPrint)
      {
        theDI << (aParams.IsShadowEnabled ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.IsShadowEnabled = toEnable;
    }
    else if (aFlag == "-shadowmapresolution" || aFlag == "-shadowmap")
    {
      if (toPrint)
      {
        theDI << aParams.ShadowMapResolution << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      aParams.ShadowMapResolution = Draw::Atoi(theArgVec[anArgIter]);
    }
    else if (aFlag == "-shadowmapbias")
    {
      if (toPrint)
      {
        theDI << aParams.ShadowMapBias << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      aParams.ShadowMapBias = (float)Draw::Atof(theArgVec[anArgIter]);
    }
    else if (aFlag == "-refl" || aFlag == "-reflections")
    {
      if (toPrint)
      {
        theDI << (aParams.IsReflectionEnabled ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toEnable))
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
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toEnable))
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
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toEnable))
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
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.IsGlobalIlluminationEnabled = toEnable;
      if (!toEnable)
      {
        aParams.RaytracingDepth = Min(aParams.RaytracingDepth, 10);
      }
    }
    else if (aFlag == "-blockedrng" || aFlag == "-brng")
    {
      if (toPrint)
      {
        theDI << (aParams.CoherentPathTracingMode ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.CoherentPathTracingMode = toEnable;
    }
    else if (aFlag == "-maxrad")
    {
      if (toPrint)
      {
        theDI << aParams.RadianceClampingValue << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const TCollection_AsciiString aMaxRadStr = theArgVec[anArgIter];
      if (!aMaxRadStr.IsRealValue(Standard_True))
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Real aMaxRadiance = aMaxRadStr.RealValue();
      if (aMaxRadiance <= 0.0)
      {
        Message::SendFail() << "Syntax error: invalid radiance clamping value " << aMaxRadiance;
        return 1;
      }
      else
      {
        aParams.RadianceClampingValue = static_cast<Standard_ShortReal>(aMaxRadiance);
      }
    }
    else if (aFlag == "-iss")
    {
      if (toPrint)
      {
        theDI << (aParams.AdaptiveScreenSampling ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.AdaptiveScreenSampling = toEnable;
    }
    else if (aFlag == "-issatomic")
    {
      if (toPrint)
      {
        theDI << (aParams.AdaptiveScreenSamplingAtomic ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.AdaptiveScreenSamplingAtomic = toEnable;
    }
    else if (aFlag == "-issd")
    {
      if (toPrint)
      {
        theDI << (aParams.ShowSamplingTiles ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.ShowSamplingTiles = toEnable;
    }
    else if (aFlag == "-tilesize")
    {
      if (toPrint)
      {
        theDI << aParams.RayTracingTileSize << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Integer aTileSize = Draw::Atoi(theArgVec[anArgIter]);
      if (aTileSize < 1)
      {
        Message::SendFail() << "Syntax error: invalid size of ISS tile " << aTileSize;
        return 1;
      }
      aParams.RayTracingTileSize = aTileSize;
    }
    else if (aFlag == "-nbtiles")
    {
      if (toPrint)
      {
        theDI << aParams.NbRayTracingTiles << " ";
        continue;
      }
      else if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Integer aNbTiles = Draw::Atoi(theArgVec[anArgIter]);
      if (aNbTiles < -1)
      {
        Message::SendFail() << "Syntax error: invalid number of ISS tiles " << aNbTiles;
        return 1;
      }
      else if (aNbTiles > 0 && (aNbTiles < 64 || aNbTiles > 1024))
      {
        Message::SendWarning() << "Warning: suboptimal number of ISS tiles " << aNbTiles
                               << ". Recommended range: [64, 1024].";
      }
      aParams.NbRayTracingTiles = aNbTiles;
    }
    else if (aFlag == "-env")
    {
      if (toPrint)
      {
        theDI << (aParams.UseEnvironmentMapBackground ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.UseEnvironmentMapBackground = toEnable;
    }
    else if (aFlag == "-ignorenormalmap")
    {
      if (toPrint)
      {
        theDI << (aParams.ToIgnoreNormalMapInRayTracing ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.ToIgnoreNormalMapInRayTracing = toEnable;
    }
    else if (aFlag == "-twoside")
    {
      if (toPrint)
      {
        theDI << (aParams.TwoSidedBsdfModels ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.TwoSidedBsdfModels = toEnable;
    }
    else if (aFlag == "-shademodel" || aFlag == "-shadingmodel" || aFlag == "-shading")
    {
      if (toPrint)
      {
        switch (aView->ShadingModel())
        {
          case Graphic3d_TypeOfShadingModel_DEFAULT:
            theDI << "default";
            break;
          case Graphic3d_TypeOfShadingModel_Unlit:
            theDI << "unlit ";
            break;
          case Graphic3d_TypeOfShadingModel_PhongFacet:
            theDI << "flat ";
            break;
          case Graphic3d_TypeOfShadingModel_Gouraud:
            theDI << "gouraud ";
            break;
          case Graphic3d_TypeOfShadingModel_Phong:
            theDI << "phong ";
            break;
          case Graphic3d_TypeOfShadingModel_Pbr:
            theDI << "pbr";
            break;
          case Graphic3d_TypeOfShadingModel_PbrFacet:
            theDI << "pbr_facet";
            break;
        }
        continue;
      }

      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
      }

      Graphic3d_TypeOfShadingModel aModel = Graphic3d_TypeOfShadingModel_DEFAULT;
      if (ViewerTest::ParseShadingModel(theArgVec[anArgIter], aModel)
          && aModel != Graphic3d_TypeOfShadingModel_DEFAULT)
      {
        aView->SetShadingModel(aModel);
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown shading model '" << theArgVec[anArgIter]
                            << "'";
        return 1;
      }
    }
    else if (aFlag == "-pbrenvpow2size" || aFlag == "-pbrenvp2s" || aFlag == "-pep2s")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Integer aPbrEnvPow2Size = Draw::Atoi(theArgVec[anArgIter]);
      if (aPbrEnvPow2Size < 1)
      {
        Message::SendFail(
          "Syntax error: 'Pow2Size' of PBR Environment has to be greater or equal 1");
        return 1;
      }
      aParams.PbrEnvPow2Size = aPbrEnvPow2Size;
    }
    else if (aFlag == "-pbrenvspecmaplevelsnumber" || aFlag == "-pbrenvspecmapnblevels"
             || aFlag == "-pbrenvspecmaplevels" || aFlag == "-pbrenvsmln" || aFlag == "-pesmln")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Integer aPbrEnvSpecMapNbLevels = Draw::Atoi(theArgVec[anArgIter]);
      if (aPbrEnvSpecMapNbLevels < 2)
      {
        Message::SendFail(
          "Syntax error: 'SpecMapLevelsNumber' of PBR Environment has to be greater or equal 2");
        return 1;
      }
      aParams.PbrEnvSpecMapNbLevels = aPbrEnvSpecMapNbLevels;
    }
    else if (aFlag == "-pbrenvbakngdiffsamplesnumber" || aFlag == "-pbrenvbakingdiffsamples"
             || aFlag == "-pbrenvbdsn")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Integer aPbrEnvBakingDiffNbSamples = Draw::Atoi(theArgVec[anArgIter]);
      if (aPbrEnvBakingDiffNbSamples < 1)
      {
        Message::SendFail("Syntax error: 'BakingDiffSamplesNumber' of PBR Environment has to be "
                          "greater or equal 1");
        return 1;
      }
      aParams.PbrEnvBakingDiffNbSamples = aPbrEnvBakingDiffNbSamples;
    }
    else if (aFlag == "-pbrenvbakngspecsamplesnumber" || aFlag == "-pbrenvbakingspecsamples"
             || aFlag == "-pbrenvbssn")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      const Standard_Integer aPbrEnvBakingSpecNbSamples = Draw::Atoi(theArgVec[anArgIter]);
      if (aPbrEnvBakingSpecNbSamples < 1)
      {
        Message::SendFail("Syntax error: 'BakingSpecSamplesNumber' of PBR Environment has to be "
                          "greater or equal 1");
        return 1;
      }
      aParams.PbrEnvBakingSpecNbSamples = aPbrEnvBakingSpecNbSamples;
    }
    else if (aFlag == "-pbrenvbakingprobability" || aFlag == "-pbrenvbp")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      const Standard_ShortReal aPbrEnvBakingProbability =
        static_cast<Standard_ShortReal>(Draw::Atof(theArgVec[anArgIter]));
      if (aPbrEnvBakingProbability < 0.f || aPbrEnvBakingProbability > 1.f)
      {
        Message::SendFail(
          "Syntax error: 'BakingProbability' of PBR Environment has to be in range of [0, 1]");
        return 1;
      }
      aParams.PbrEnvBakingProbability = aPbrEnvBakingProbability;
    }
    else if (aFlag == "-resolution")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aResolution(theArgVec[anArgIter]);
      if (aResolution.IsIntegerValue())
      {
        aView->ChangeRenderingParams().Resolution =
          static_cast<unsigned int>(Draw::Atoi(aResolution.ToCString()));
      }
      else
      {
        Message::SendFail() << "Syntax error: wrong syntax at argument'" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-rebuildglsl" || aFlag == "-rebuild")
    {
      if (toPrint)
      {
        theDI << (aParams.RebuildRayTracingShaders ? "on" : "off") << " ";
        continue;
      }

      Standard_Boolean toEnable = Standard_True;
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toEnable))
      {
        --anArgIter;
      }
      aParams.RebuildRayTracingShaders = toEnable;
    }
    else if (aFlag == "-focal")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aParam(theArgVec[anArgIter]);
      if (aParam.IsRealValue(Standard_True))
      {
        float aFocalDist = static_cast<float>(aParam.RealValue());
        if (aFocalDist < 0)
        {
          Message::SendFail() << "Error: parameter can't be negative at argument '" << anArg << "'";
          return 1;
        }
        aView->ChangeRenderingParams().CameraFocalPlaneDist = aFocalDist;
      }
      else
      {
        Message::SendFail() << "Syntax error at argument'" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-aperture")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aParam(theArgVec[anArgIter]);
      if (aParam.IsRealValue(Standard_True))
      {
        float aApertureSize = static_cast<float>(aParam.RealValue());
        if (aApertureSize < 0)
        {
          Message::SendFail() << "Error: parameter can't be negative at argument '" << anArg << "'";
          return 1;
        }
        aView->ChangeRenderingParams().CameraApertureRadius = aApertureSize;
      }
      else
      {
        Message::SendFail() << "Syntax error at argument'" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-exposure")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString anExposure(theArgVec[anArgIter]);
      if (anExposure.IsRealValue(Standard_True))
      {
        aView->ChangeRenderingParams().Exposure = static_cast<float>(anExposure.RealValue());
      }
      else
      {
        Message::SendFail() << "Syntax error at argument'" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-whitepoint")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aWhitePoint(theArgVec[anArgIter]);
      if (aWhitePoint.IsRealValue(Standard_True))
      {
        aView->ChangeRenderingParams().WhitePoint = static_cast<float>(aWhitePoint.RealValue());
      }
      else
      {
        Message::SendFail() << "Syntax error at argument'" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-tonemapping")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aMode(theArgVec[anArgIter]);
      aMode.LowerCase();

      if (aMode == "disabled")
      {
        aView->ChangeRenderingParams().ToneMappingMethod = Graphic3d_ToneMappingMethod_Disabled;
      }
      else if (aMode == "filmic")
      {
        aView->ChangeRenderingParams().ToneMappingMethod = Graphic3d_ToneMappingMethod_Filmic;
      }
      else
      {
        Message::SendFail() << "Syntax error at argument'" << anArg << "'";
        return 1;
      }
    }
    else if (aFlag == "-performancestats" || aFlag == "-performancecounters"
             || aFlag == "-perfstats" || aFlag == "-perfcounters" || aFlag == "-stats")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }

      TCollection_AsciiString aFlagsStr(theArgVec[anArgIter]);
      aFlagsStr.LowerCase();
      Graphic3d_RenderingParams::PerfCounters aFlags =
        aView->ChangeRenderingParams().CollectedStats;
      if (!convertToPerfStatsFlags(aFlagsStr, aFlags))
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      aView->ChangeRenderingParams().CollectedStats = aFlags;
      aView->ChangeRenderingParams().ToShowStats =
        aFlags != Graphic3d_RenderingParams::PerfCounters_NONE;
    }
    else if (aFlag == "-perfupdateinterval" || aFlag == "-statsupdateinterval")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      aView->ChangeRenderingParams().StatsUpdateInterval =
        (Standard_ShortReal)Draw::Atof(theArgVec[anArgIter]);
    }
    else if (aFlag == "-perfchart" || aFlag == "-statschart")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      aView->ChangeRenderingParams().StatsNbFrames = Draw::Atoi(theArgVec[anArgIter]);
    }
    else if (aFlag == "-perfchartmax" || aFlag == "-statschartmax")
    {
      if (++anArgIter >= theArgNb)
      {
        Message::SendFail() << "Syntax error at argument '" << anArg << "'";
        return 1;
      }
      aView->ChangeRenderingParams().StatsMaxChartTime =
        (Standard_ShortReal)Draw::Atof(theArgVec[anArgIter]);
    }
    else if (aFlag == "-frustumculling" || aFlag == "-culling")
    {
      if (toPrint)
      {
        theDI << ((aParams.FrustumCullingState == Graphic3d_RenderingParams::FrustumCulling_On)
                    ? "on"
                  : (aParams.FrustumCullingState == Graphic3d_RenderingParams::FrustumCulling_Off)
                    ? "off"
                    : "noUpdate")
              << " ";
        continue;
      }

      Graphic3d_RenderingParams::FrustumCulling aState =
        Graphic3d_RenderingParams::FrustumCulling_On;
      if (++anArgIter < theArgNb)
      {
        TCollection_AsciiString aStateStr(theArgVec[anArgIter]);
        aStateStr.LowerCase();
        bool toEnable = true;
        if (Draw::ParseOnOff(aStateStr.ToCString(), toEnable))
        {
          aState = toEnable ? Graphic3d_RenderingParams::FrustumCulling_On
                            : Graphic3d_RenderingParams::FrustumCulling_Off;
        }
        else if (aStateStr == "noupdate" || aStateStr == "freeze")
        {
          aState = Graphic3d_RenderingParams::FrustumCulling_NoUpdate;
        }
        else
        {
          --anArgIter;
        }
      }
      aParams.FrustumCullingState = aState;
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown flag '" << anArg << "'";
      return 1;
    }
  }

  // set current view parameters as defaults
  if (toSyncDefaults)
  {
    ViewerTest::GetViewerFromContext()->SetDefaultRenderingParams(aParams);
  }
  if (toSyncAllViews)
  {
    for (V3d_ListOfViewIterator aViewIter =
           ViewerTest::GetViewerFromContext()->DefinedViewIterator();
         aViewIter.More();
         aViewIter.Next())
    {
      aViewIter.Value()->ChangeRenderingParams() = aParams;
    }
  }
  return 0;
}

//=================================================================================================

inline TCollection_AsciiString searchInfo(const TColStd_IndexedDataMapOfStringString& theDict,
                                          const TCollection_AsciiString&              theKey)
{
  for (TColStd_IndexedDataMapOfStringString::Iterator anIter(theDict); anIter.More(); anIter.Next())
  {
    if (TCollection_AsciiString::IsSameString(anIter.Key(), theKey, Standard_False))
    {
      return anIter.Value();
    }
  }
  return TCollection_AsciiString();
}

//=================================================================================================

static Standard_Integer VStatProfiler(Draw_Interpretor& theDI,
                                      Standard_Integer  theArgNb,
                                      const char**      theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Standard_Boolean                        toRedraw = Standard_True;
  Graphic3d_RenderingParams::PerfCounters aPrevCounters =
    aView->ChangeRenderingParams().CollectedStats;
  Standard_ShortReal aPrevUpdInterval = aView->ChangeRenderingParams().StatsUpdateInterval;
  Graphic3d_RenderingParams::PerfCounters aRenderParams =
    Graphic3d_RenderingParams::PerfCounters_NONE;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    Standard_CString        anArg(theArgVec[anArgIter]);
    TCollection_AsciiString aFlag(anArg);
    aFlag.LowerCase();
    if (aFlag == "-noredraw")
    {
      toRedraw = Standard_False;
    }
    else
    {
      Graphic3d_RenderingParams::PerfCounters aParam = Graphic3d_RenderingParams::PerfCounters_NONE;
      if (aFlag == "fps")
        aParam = Graphic3d_RenderingParams::PerfCounters_FrameRate;
      else if (aFlag == "cpu")
        aParam = Graphic3d_RenderingParams::PerfCounters_CPU;
      else if (aFlag == "alllayers" || aFlag == "layers")
        aParam = Graphic3d_RenderingParams::PerfCounters_Layers;
      else if (aFlag == "allstructs" || aFlag == "allstructures" || aFlag == "structs"
               || aFlag == "structures")
        aParam = Graphic3d_RenderingParams::PerfCounters_Structures;
      else if (aFlag == "groups")
        aParam = Graphic3d_RenderingParams::PerfCounters_Groups;
      else if (aFlag == "allarrays" || aFlag == "fillarrays" || aFlag == "linearrays"
               || aFlag == "pointarrays" || aFlag == "textarrays")
        aParam = Graphic3d_RenderingParams::PerfCounters_GroupArrays;
      else if (aFlag == "triangles")
        aParam = Graphic3d_RenderingParams::PerfCounters_Triangles;
      else if (aFlag == "lines")
        aParam = Graphic3d_RenderingParams::PerfCounters_Lines;
      else if (aFlag == "points")
        aParam = Graphic3d_RenderingParams::PerfCounters_Points;
      else if (aFlag == "geommem" || aFlag == "texturemem" || aFlag == "framemem")
        aParam = Graphic3d_RenderingParams::PerfCounters_EstimMem;
      else if (aFlag == "elapsedframe" || aFlag == "cpuframeaverage" || aFlag == "cpupickingaverage"
               || aFlag == "cpucullingaverage" || aFlag == "cpudynaverage" || aFlag == "cpuframemax"
               || aFlag == "cpupickingmax" || aFlag == "cpucullingmax" || aFlag == "cpudynmax")
        aParam = Graphic3d_RenderingParams::PerfCounters_FrameTime;
      else
      {
        Message::SendFail() << "Error: unknown argument '" << theArgVec[anArgIter] << "'";
        continue;
      }

      aRenderParams = Graphic3d_RenderingParams::PerfCounters(aRenderParams | aParam);
    }
  }

  if (aRenderParams != Graphic3d_RenderingParams::PerfCounters_NONE)
  {
    aView->ChangeRenderingParams().CollectedStats = Graphic3d_RenderingParams::PerfCounters(
      aView->RenderingParams().CollectedStats | aRenderParams);

    if (toRedraw)
    {
      aView->ChangeRenderingParams().StatsUpdateInterval = -1;
      aView->Redraw();
      aView->ChangeRenderingParams().StatsUpdateInterval = aPrevUpdInterval;
    }

    TColStd_IndexedDataMapOfStringString aDict;
    aView->StatisticInformation(aDict);

    aView->ChangeRenderingParams().CollectedStats = aPrevCounters;

    for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
    {
      Standard_CString        anArg(theArgVec[anArgIter]);
      TCollection_AsciiString aFlag(anArg);
      aFlag.LowerCase();
      if (aFlag == "fps")
      {
        theDI << searchInfo(aDict, "FPS") << " ";
      }
      else if (aFlag == "cpu")
      {
        theDI << searchInfo(aDict, "CPU FPS") << " ";
      }
      else if (aFlag == "alllayers")
      {
        theDI << searchInfo(aDict, "Layers") << " ";
      }
      else if (aFlag == "layers")
      {
        theDI << searchInfo(aDict, "Rendered layers") << " ";
      }
      else if (aFlag == "allstructs" || aFlag == "allstructures")
      {
        theDI << searchInfo(aDict, "Structs") << " ";
      }
      else if (aFlag == "structs" || aFlag == "structures")
      {
        TCollection_AsciiString aRend = searchInfo(aDict, "Rendered structs");
        if (aRend.IsEmpty()) // all structures rendered
        {
          aRend = searchInfo(aDict, "Structs");
        }
        theDI << aRend << " ";
      }
      else if (aFlag == "groups")
      {
        theDI << searchInfo(aDict, "Rendered groups") << " ";
      }
      else if (aFlag == "allarrays")
      {
        theDI << searchInfo(aDict, "Rendered arrays") << " ";
      }
      else if (aFlag == "fillarrays")
      {
        theDI << searchInfo(aDict, "Rendered [fill] arrays") << " ";
      }
      else if (aFlag == "linearrays")
      {
        theDI << searchInfo(aDict, "Rendered [line] arrays") << " ";
      }
      else if (aFlag == "pointarrays")
      {
        theDI << searchInfo(aDict, "Rendered [point] arrays") << " ";
      }
      else if (aFlag == "textarrays")
      {
        theDI << searchInfo(aDict, "Rendered [text] arrays") << " ";
      }
      else if (aFlag == "triangles")
      {
        theDI << searchInfo(aDict, "Rendered triangles") << " ";
      }
      else if (aFlag == "points")
      {
        theDI << searchInfo(aDict, "Rendered points") << " ";
      }
      else if (aFlag == "geommem")
      {
        theDI << searchInfo(aDict, "GPU Memory [geometry]") << " ";
      }
      else if (aFlag == "texturemem")
      {
        theDI << searchInfo(aDict, "GPU Memory [textures]") << " ";
      }
      else if (aFlag == "framemem")
      {
        theDI << searchInfo(aDict, "GPU Memory [frames]") << " ";
      }
      else if (aFlag == "elapsedframe")
      {
        theDI << searchInfo(aDict, "Elapsed Frame (average)") << " ";
      }
      else if (aFlag == "cpuframe_average")
      {
        theDI << searchInfo(aDict, "CPU Frame (average)") << " ";
      }
      else if (aFlag == "cpupicking_average")
      {
        theDI << searchInfo(aDict, "CPU Picking (average)") << " ";
      }
      else if (aFlag == "cpuculling_average")
      {
        theDI << searchInfo(aDict, "CPU Culling (average)") << " ";
      }
      else if (aFlag == "cpudyn_average")
      {
        theDI << searchInfo(aDict, "CPU Dynamics (average)") << " ";
      }
      else if (aFlag == "cpuframe_max")
      {
        theDI << searchInfo(aDict, "CPU Frame (max)") << " ";
      }
      else if (aFlag == "cpupicking_max")
      {
        theDI << searchInfo(aDict, "CPU Picking (max)") << " ";
      }
      else if (aFlag == "cpuculling_max")
      {
        theDI << searchInfo(aDict, "CPU Culling (max)") << " ";
      }
      else if (aFlag == "cpudyn_max")
      {
        theDI << searchInfo(aDict, "CPU Dynamics (max)") << " ";
      }
    }
  }
  else
  {
    if (toRedraw)
    {
      aView->ChangeRenderingParams().StatsUpdateInterval = -1;
      aView->Redraw();
      aView->ChangeRenderingParams().StatsUpdateInterval = aPrevUpdInterval;
    }
    theDI << "Statistic info:\n" << aView->StatisticInformation();
  }
  return 0;
}

//=================================================================================================

static Standard_Integer VXRotate(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << argv[0] << "ERROR : use 'vinit' command before \n";
    return 1;
  }

  if (argc != 3)
  {
    di << "ERROR : Usage : " << argv[0] << " name angle\n";
    return 1;
  }

  TCollection_AsciiString aName(argv[1]);
  Standard_Real           anAngle = Draw::Atof(argv[2]);

  // find object
  ViewerTest_DoubleMapOfInteractiveAndName& aMap = GetMapOfAIS();
  Handle(AIS_InteractiveObject)             anIObj;
  if (!aMap.Find2(aName, anIObj))
  {
    di << "Use 'vdisplay' before\n";
    return 1;
  }

  gp_Trsf aTransform;
  aTransform.SetRotation(gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Vec(1.0, 0.0, 0.0)), anAngle);
  aTransform.SetTranslationPart(anIObj->LocalTransformation().TranslationPart());

  aContext->SetLocation(anIObj, aTransform);
  aContext->UpdateCurrentViewer();
  return 0;
}

namespace
{
//! Structure for setting AIS_Manipulator::SetPart() property.
struct ManipAxisModeOnOff
{
  Standard_Integer    Axis;
  AIS_ManipulatorMode Mode;
  Standard_Boolean    ToEnable;

  ManipAxisModeOnOff()
      : Axis(-1),
        Mode(AIS_MM_None),
        ToEnable(false)
  {
  }
};

enum ManipAjustPosition
{
  ManipAjustPosition_Off,
  ManipAjustPosition_Center,
  ManipAjustPosition_Location,
  ManipAjustPosition_ShapeLocation,
};
} // namespace

//=================================================================================================

static int VManipulator(Draw_Interpretor& theDi, Standard_Integer theArgsNb, const char** theArgVec)
{
  Handle(V3d_View)   aCurrentView = ViewerTest::CurrentView();
  Handle(V3d_Viewer) aViewer      = ViewerTest::GetViewerFromContext();
  if (aCurrentView.IsNull() || aViewer.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  ViewerTest_AutoUpdater  anUpdateTool(ViewerTest::GetAISContext(), ViewerTest::CurrentView());
  Standard_Integer        anArgIter = 1;
  Handle(AIS_Manipulator) aManipulator;
  ViewerTest_DoubleMapOfInteractiveAndName& aMapAIS = GetMapOfAIS();
  TCollection_AsciiString                   aName;
  // parameters
  Standard_Integer toAutoActivate = -1, toFollowTranslation = -1, toFollowRotation = -1,
                   toFollowDragging = -1, isZoomable = -1;
  Standard_Real                            aGap = -1.0, aSize = -1.0;
  NCollection_Sequence<ManipAxisModeOnOff> aParts;
  gp_XYZ aLocation(RealLast(), RealLast(), RealLast()), aVDir, anXDir;
  //
  bool                              toDetach = false;
  AIS_Manipulator::OptionsForAttach anAttachOptions;
  Handle(AIS_InteractiveObject)     anAttachObject;
  Handle(V3d_View)                  aViewAffinity;
  ManipAjustPosition                anAttachPos = ManipAjustPosition_Off;
  //
  Graphic3d_Vec2i  aMousePosFrom(IntegerLast(), IntegerLast());
  Graphic3d_Vec2i  aMousePosTo(IntegerLast(), IntegerLast());
  Standard_Integer toStopMouseTransform = -1;
  // explicit transformation
  gp_Trsf       aTrsf;
  gp_XYZ        aTmpXYZ;
  Standard_Real aTmpReal = 0.0;
  gp_XYZ        aRotPnt, aRotAxis;
  for (; anArgIter < theArgsNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode(anArg))
    {
      continue;
    }
    else if (anArg == "-help")
    {
      theDi.PrintHelp(theArgVec[0]);
      return 0;
    }
    //
    else if (anArg == "-autoactivate" || anArg == "-noautoactivate")
    {
      toAutoActivate = Draw::ParseOnOffNoIterator(theArgsNb, theArgVec, anArgIter) ? 1 : 0;
    }
    else if (anArg == "-followtranslation" || anArg == "-nofollowtranslation")
    {
      toFollowTranslation = Draw::ParseOnOffNoIterator(theArgsNb, theArgVec, anArgIter) ? 1 : 0;
    }
    else if (anArg == "-followrotation" || anArg == "-nofollowrotation")
    {
      toFollowRotation = Draw::ParseOnOffNoIterator(theArgsNb, theArgVec, anArgIter) ? 1 : 0;
    }
    else if (anArg == "-followdragging" || anArg == "-nofollowdragging")
    {
      toFollowDragging = Draw::ParseOnOffNoIterator(theArgsNb, theArgVec, anArgIter) ? 1 : 0;
    }
    else if (anArg == "-gap" && anArgIter + 1 < theArgsNb
             && Draw::ParseReal(theArgVec[anArgIter + 1], aGap) && aGap >= 0.0)
    {
      ++anArgIter;
    }
    else if (anArg == "-size" && anArgIter + 1 < theArgsNb
             && Draw::ParseReal(theArgVec[anArgIter + 1], aSize) && aSize > 0.0)
    {
      ++anArgIter;
    }
    else if ((anArg == "-part" && anArgIter + 3 < theArgsNb)
             || (anArg == "-parts" && anArgIter + 2 < theArgsNb))
    {
      ManipAxisModeOnOff aPart;
      Standard_Integer   aMode = 0;
      if (anArg == "-part")
      {
        if (!Draw::ParseInteger(theArgVec[++anArgIter], aPart.Axis) || aPart.Axis < 0
            || aPart.Axis > 3)
        {
          Message::SendFail() << "Syntax error: -part axis '" << theArgVec[anArgIter]
                              << "' is out of range [1, 3]";
          return 1;
        }
      }
      if (!Draw::ParseInteger(theArgVec[++anArgIter], aMode) || aMode < 1 || aMode > 4)
      {
        Message::SendFail() << "Syntax error: -part mode '" << theArgVec[anArgIter]
                            << "' is out of range [1, 4]";
        return 1;
      }
      if (!Draw::ParseOnOff(theArgVec[++anArgIter], aPart.ToEnable))
      {
        Message::SendFail() << "Syntax error: -part value on/off '" << theArgVec[anArgIter]
                            << "' is incorrect";
        return 1;
      }
      aPart.Mode = static_cast<AIS_ManipulatorMode>(aMode);
      aParts.Append(aPart);
    }
    else if (anArg == "-pos" && anArgIter + 3 < theArgsNb
             && parseXYZ(theArgVec + anArgIter + 1, aLocation))
    {
      anArgIter += 3;
      if (anArgIter + 3 < theArgsNb && parseXYZ(theArgVec + anArgIter + 1, aVDir)
          && aVDir.Modulus() > Precision::Confusion())
      {
        anArgIter += 3;
      }
      if (anArgIter + 3 < theArgsNb && parseXYZ(theArgVec + anArgIter + 1, anXDir)
          && anXDir.Modulus() > Precision::Confusion())
      {
        anArgIter += 3;
      }
    }
    else if (anArg == "-zoomable" || anArg == "-notzoomable")
    {
      isZoomable = Draw::ParseOnOffNoIterator(theArgsNb, theArgVec, anArgIter) ? 1 : 0;
    }
    //
    else if (anArg == "-adjustposition" || anArg == "-noadjustposition")
    {
      anAttachPos = ManipAjustPosition_Center;
      if (anArgIter + 1 < theArgsNb)
      {
        TCollection_AsciiString aPosName(theArgVec[++anArgIter]);
        aPosName.LowerCase();
        if (aPosName == "0")
        {
          anAttachPos = ManipAjustPosition_Off;
        }
        else if (aPosName == "1" || aPosName == "center")
        {
          anAttachPos = ManipAjustPosition_Center;
        }
        else if (aPosName == "transformation" || aPosName == "trsf" || aPosName == "location"
                 || aPosName == "loc")
        {
          anAttachPos = ManipAjustPosition_Location;
        }
        else if (aPosName == "shapelocation" || aPosName == "shapeloc")
        {
          anAttachPos = ManipAjustPosition_ShapeLocation;
        }
        else
        {
          --anArgIter;
        }
      }
      anAttachOptions.SetAdjustPosition(anAttachPos == ManipAjustPosition_Center);
    }
    else if (anArg == "-adjustsize" || anArg == "-noadjustsize")
    {
      anAttachOptions.SetAdjustSize(
        Draw::ParseOnOffNoIterator(theArgsNb, theArgVec, anArgIter) ? 1 : 0);
    }
    else if (anArg == "-enablemodes")
    {
      anAttachOptions.SetEnableModes(
        Draw::ParseOnOffNoIterator(theArgsNb, theArgVec, anArgIter) ? 1 : 0);
    }
    //
    else if (anArg == "-starttransform" && anArgIter + 2 < theArgsNb
             && Draw::ParseInteger(theArgVec[anArgIter + 1], aMousePosFrom.x())
             && Draw::ParseInteger(theArgVec[anArgIter + 2], aMousePosFrom.y()))
    {
      anArgIter += 2;
    }
    else if (anArg == "-transform" && anArgIter + 2 < theArgsNb
             && Draw::ParseInteger(theArgVec[anArgIter + 1], aMousePosTo.x())
             && Draw::ParseInteger(theArgVec[anArgIter + 2], aMousePosTo.y()))
    {
      anArgIter += 2;
    }
    else if (anArg == "-stoptransform")
    {
      toStopMouseTransform = 1;
      if (anArgIter + 1 < theArgsNb
          && TCollection_AsciiString::IsSameString(theArgVec[anArgIter + 1], "abort", false))
      {
        ++anArgIter;
        toStopMouseTransform = 0;
      }
    }
    //
    else if (anArg == "-move" && anArgIter + 3 < theArgsNb
             && parseXYZ(theArgVec + anArgIter + 1, aTmpXYZ))
    {
      anArgIter += 3;
      aTrsf.SetTranslationPart(aTmpXYZ);
    }
    else if (anArg == "-scale" && anArgIter + 1 < theArgsNb
             && Draw::ParseReal(theArgVec[anArgIter + 1], aTmpReal))
    {
      ++anArgIter;
      aTrsf.SetScale(gp_Pnt(), aTmpReal);
    }
    else if (anArg == "-rotate" && anArgIter + 7 < theArgsNb
             && parseXYZ(theArgVec + anArgIter + 1, aRotPnt)
             && parseXYZ(theArgVec + anArgIter + 4, aRotAxis)
             && Draw::ParseReal(theArgVec[anArgIter + 7], aTmpReal))
    {
      anArgIter += 7;
      aTrsf.SetRotation(gp_Ax1(gp_Pnt(aRotPnt), gp_Dir(aRotAxis)), aTmpReal);
    }
    //
    else if (anArg == "-detach")
    {
      toDetach = true;
    }
    else if (anArg == "-attach" && anArgIter + 1 < theArgsNb)
    {
      TCollection_AsciiString anObjName(theArgVec[++anArgIter]);
      if (!aMapAIS.Find2(anObjName, anAttachObject))
      {
        Message::SendFail() << "Syntax error: AIS object '" << anObjName << "' does not exist";
        return 1;
      }

      for (ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName anIter(aMapAIS);
           anIter.More();
           anIter.Next())
      {
        Handle(AIS_Manipulator) aManip = Handle(AIS_Manipulator)::DownCast(anIter.Key1());
        if (!aManip.IsNull() && aManip->IsAttached() && aManip->Object() == anAttachObject)
        {
          Message::SendFail() << "Syntax error: AIS object '" << anObjName
                              << "' already has manipulator";
          return 1;
        }
      }
    }
    else if (anArg == "-view" && anArgIter + 1 < theArgsNb && aViewAffinity.IsNull())
    {
      TCollection_AsciiString aViewString(theArgVec[++anArgIter]);
      if (aViewString == "active")
      {
        aViewAffinity = ViewerTest::CurrentView();
      }
      else // Check view name
      {
        ViewerTest_Names aViewNames(aViewString);
        if (!ViewerTest_myViews.IsBound1(aViewNames.GetViewName()))
        {
          Message::SendFail() << "Syntax error: wrong view name '" << aViewString << "'";
          return 1;
        }
        aViewAffinity = ViewerTest_myViews.Find1(aViewNames.GetViewName());
        if (aViewAffinity.IsNull())
        {
          Message::SendFail() << "Syntax error: cannot find view with name '" << aViewString << "'";
          return 1;
        }
      }
    }
    else if (aName.IsEmpty())
    {
      aName = theArgVec[anArgIter];
      if (!aMapAIS.IsBound2(aName))
      {
        aManipulator = new AIS_Manipulator();
        aManipulator->SetModeActivationOnDetection(true);
        aMapAIS.Bind(aManipulator, aName);
      }
      else
      {
        aManipulator = Handle(AIS_Manipulator)::DownCast(aMapAIS.Find2(aName));
        if (aManipulator.IsNull())
        {
          Message::SendFail() << "Syntax error: \"" << aName << "\" is not an AIS manipulator";
          return 1;
        }
      }
    }
    else
    {
      theDi << "Syntax error: unknown argument '" << theArgVec[anArgIter] << "'";
    }
  }

  if (aName.IsEmpty())
  {
    Message::SendFail("Syntax error: please specify AIS manipulator's name as the first argument");
    return 1;
  }
  if (!toDetach && aManipulator.IsNull())
  {
    aManipulator = new AIS_Manipulator();
    aManipulator->SetModeActivationOnDetection(true);
    aMapAIS.Bind(aManipulator, aName);
  }

  // -----------------------------------------
  // change properties of manipulator instance
  // -----------------------------------------

  if (toAutoActivate != -1)
  {
    aManipulator->SetModeActivationOnDetection(toAutoActivate == 1);
  }
  if (toFollowTranslation != -1)
  {
    aManipulator->ChangeTransformBehavior().SetFollowTranslation(toFollowTranslation == 1);
  }
  if (toFollowRotation != -1)
  {
    aManipulator->ChangeTransformBehavior().SetFollowRotation(toFollowRotation == 1);
  }
  if (toFollowDragging != -1)
  {
    aManipulator->ChangeTransformBehavior().SetFollowDragging(toFollowDragging == 1);
  }
  if (aGap >= 0.0f)
  {
    aManipulator->SetGap((float)aGap);
  }

  for (NCollection_Sequence<ManipAxisModeOnOff>::Iterator aPartIter(aParts); aPartIter.More();
       aPartIter.Next())
  {
    const ManipAxisModeOnOff& aPart = aPartIter.Value();
    if (aPart.Axis == -1)
    {
      aManipulator->SetPart(aPart.Mode, aPart.ToEnable);
    }
    else
    {
      aManipulator->SetPart(aPart.Axis, aPart.Mode, aPart.ToEnable);
    }
  }

  if (aSize > 0.0)
  {
    aManipulator->SetSize((float)aSize);
  }
  if (isZoomable != -1)
  {
    aManipulator->SetZoomPersistence(isZoomable == 0);

    if (ViewerTest::GetAISContext()->IsDisplayed(aManipulator))
    {
      ViewerTest::GetAISContext()->Remove(aManipulator, Standard_False);
      ViewerTest::GetAISContext()->Display(aManipulator, Standard_False);
    }
  }

  // ----------------------------------
  // detach existing manipulator object
  // ----------------------------------

  if (toDetach)
  {
    aManipulator->Detach();
    aMapAIS.UnBind2(aName);
    ViewerTest::GetAISContext()->Remove(aManipulator, false);
  }

  // ---------------------------------------------------
  // attach, detach or access manipulator from an object
  // ---------------------------------------------------

  if (!anAttachObject.IsNull())
  {
    aManipulator->Attach(anAttachObject, anAttachOptions);
  }
  if (!aViewAffinity.IsNull())
  {
    for (ViewerTest_ViewerCommandsViewMap::Iterator anIter(ViewerTest_myViews); anIter.More();
         anIter.Next())
    {
      ViewerTest::GetAISContext()->SetViewAffinity(aManipulator, anIter.Key2(), false);
    }
    ViewerTest::GetAISContext()->SetViewAffinity(aManipulator, aViewAffinity, true);
  }

  if (anAttachPos != ManipAjustPosition_Off && aManipulator->IsAttached()
      && (anAttachObject.IsNull() || anAttachPos != ManipAjustPosition_Center))
  {
    gp_Ax2        aPosition = gp::XOY();
    const gp_Trsf aBaseTrsf = aManipulator->Object()->LocalTransformation();
    switch (anAttachPos)
    {
      case ManipAjustPosition_Off: {
        break;
      }
      case ManipAjustPosition_Location: {
        aPosition = gp::XOY().Transformed(aBaseTrsf);
        break;
      }
      case ManipAjustPosition_ShapeLocation: {
        if (Handle(AIS_Shape) aShapePrs = Handle(AIS_Shape)::DownCast(aManipulator->Object()))
        {
          aPosition = gp::XOY().Transformed(aBaseTrsf * aShapePrs->Shape().Location());
        }
        else
        {
          Message::SendFail() << "Syntax error: manipulator is not attached to shape";
          return 1;
        }
        break;
      }
      case ManipAjustPosition_Center: {
        Bnd_Box aBox;
        for (AIS_ManipulatorObjectSequence::Iterator anObjIter(*aManipulator->Objects());
             anObjIter.More();
             anObjIter.Next())
        {
          Bnd_Box anObjBox;
          anObjIter.Value()->BoundingBox(anObjBox);
          aBox.Add(anObjBox);
        }
        aBox = aBox.FinitePart();
        if (!aBox.IsVoid())
        {
          const gp_Pnt aCenter = (aBox.CornerMin().XYZ() + aBox.CornerMax().XYZ()) * 0.5;
          aPosition.SetLocation(aCenter);
        }
        break;
      }
    }
    aManipulator->SetPosition(aPosition);
  }
  if (!Precision::IsInfinite(aLocation.X()))
  {
    if (aVDir.Modulus() <= Precision::Confusion())
    {
      aVDir = aManipulator->Position().Direction().XYZ();
    }
    if (anXDir.Modulus() <= Precision::Confusion())
    {
      anXDir = aManipulator->Position().XDirection().XYZ();
    }
    aManipulator->SetPosition(gp_Ax2(gp_Pnt(aLocation), gp_Dir(aVDir), gp_Dir(anXDir)));
  }

  // --------------------------------------
  // apply transformation using manipulator
  // --------------------------------------

  if (aMousePosFrom.x() != IntegerLast())
  {
    aManipulator->StartTransform(aMousePosFrom.x(), aMousePosFrom.y(), ViewerTest::CurrentView());
  }
  if (aMousePosTo.x() != IntegerLast())
  {
    aManipulator->Transform(aMousePosTo.x(), aMousePosTo.y(), ViewerTest::CurrentView());
  }
  if (toStopMouseTransform != -1)
  {
    aManipulator->StopTransform(toStopMouseTransform == 1);
  }

  if (aTrsf.Form() != gp_Identity)
  {
    aManipulator->Transform(aTrsf);
  }

  if (ViewerTest::GetAISContext()->IsDisplayed(aManipulator))
  {
    ViewerTest::GetAISContext()->Redisplay(aManipulator, true);
  }
  return 0;
}

//=================================================================================================

static int VSelectionProperties(Draw_Interpretor& theDi,
                                Standard_Integer  theArgsNb,
                                const char**      theArgVec)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  if (TCollection_AsciiString(theArgVec[0]) == "vhighlightselected")
  {
    // handle obsolete alias
    bool toEnable = true;
    if (theArgsNb < 2)
    {
      theDi << (aCtx->ToHilightSelected() ? "on" : "off");
      return 0;
    }
    else if (theArgsNb != 2 || !Draw::ParseOnOff(theArgVec[1], toEnable))
    {
      Message::SendFail("Syntax error: wrong number of parameters");
      return 1;
    }
    if (toEnable != aCtx->ToHilightSelected())
    {
      aCtx->ClearDetected();
      aCtx->SetToHilightSelected(toEnable);
    }
    return 0;
  }

  Standard_Boolean      toPrint   = theArgsNb == 1;
  Standard_Boolean      toRedraw  = Standard_False;
  Standard_Integer      anArgIter = 1;
  Prs3d_TypeOfHighlight aType     = Prs3d_TypeOfHighlight_None;
  if (anArgIter < theArgsNb)
  {
    TCollection_AsciiString anArgFirst(theArgVec[anArgIter]);
    anArgFirst.LowerCase();
    ++anArgIter;
    if (anArgFirst == "dynhighlight" || anArgFirst == "dynhilight"
        || anArgFirst == "dynamichighlight" || anArgFirst == "dynamichilight")
    {
      aType = Prs3d_TypeOfHighlight_Dynamic;
    }
    else if (anArgFirst == "localdynhighlight" || anArgFirst == "localdynhilight"
             || anArgFirst == "localdynamichighlight" || anArgFirst == "localdynamichilight")
    {
      aType = Prs3d_TypeOfHighlight_LocalDynamic;
    }
    else if (anArgFirst == "selhighlight" || anArgFirst == "selhilight"
             || anArgFirst == "selectedhighlight" || anArgFirst == "selectedhilight")
    {
      aType = Prs3d_TypeOfHighlight_Selected;
    }
    else if (anArgFirst == "localselhighlight" || anArgFirst == "localselhilight"
             || anArgFirst == "localselectedhighlight" || anArgFirst == "localselectedhilight")
    {
      aType = Prs3d_TypeOfHighlight_LocalSelected;
    }
    else
    {
      --anArgIter;
    }
  }
  for (; anArgIter < theArgsNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-help")
    {
      theDi.PrintHelp(theArgVec[0]);
      return 0;
    }
    else if (anArg == "-print")
    {
      toPrint = Standard_True;
    }
    else if (anArg == "-autoactivate")
    {
      Standard_Boolean toEnable = Standard_True;
      if (anArgIter + 1 < theArgsNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      aCtx->SetAutoActivateSelection(toEnable);
    }
    else if (anArg == "-automatichighlight" || anArg == "-automatichilight"
             || anArg == "-autohighlight" || anArg == "-autohilight")
    {
      Standard_Boolean toEnable = Standard_True;
      if (anArgIter + 1 < theArgsNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      aCtx->ClearSelected(false);
      aCtx->ClearDetected();
      aCtx->SetAutomaticHilight(toEnable);
      toRedraw = true;
    }
    else if (anArg == "-highlightselected" || anArg == "-hilightselected")
    {
      Standard_Boolean toEnable = Standard_True;
      if (anArgIter + 1 < theArgsNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], toEnable))
      {
        ++anArgIter;
      }
      aCtx->ClearDetected();
      aCtx->SetToHilightSelected(toEnable);
      toRedraw = true;
    }
    else if (anArg == "-pickstrategy" || anArg == "-pickingstrategy")
    {
      if (++anArgIter >= theArgsNb)
      {
        Message::SendFail("Syntax error: type of highlighting is undefined");
        return 1;
      }

      SelectMgr_PickingStrategy aStrategy = SelectMgr_PickingStrategy_FirstAcceptable;
      TCollection_AsciiString   aVal(theArgVec[anArgIter]);
      aVal.LowerCase();
      if (aVal == "first" || aVal == "firstaccepted" || aVal == "firstacceptable")
      {
        aStrategy = SelectMgr_PickingStrategy_FirstAcceptable;
      }
      else if (aVal == "topmost" || aVal == "onlyTopmost")
      {
        aStrategy = SelectMgr_PickingStrategy_OnlyTopmost;
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown picking strategy '" << aVal << "'";
        return 1;
      }

      aCtx->SetPickingStrategy(aStrategy);
    }
    else if (anArg == "-pixtol" && anArgIter + 1 < theArgsNb)
    {
      aCtx->SetPixelTolerance(Draw::Atoi(theArgVec[++anArgIter]));
    }
    else if (anArg == "-preferclosest")
    {
      bool toPreferClosest = true;
      if (anArgIter + 1 < theArgsNb && Draw::ParseOnOff(theArgVec[anArgIter + 1], toPreferClosest))
      {
        ++anArgIter;
      }
      aCtx->MainSelector()->SetPickClosest(toPreferClosest);
    }
    else if ((anArg == "-depthtol" || anArg == "-depthtolerance") && anArgIter + 1 < theArgsNb)
    {
      TCollection_AsciiString aTolType(theArgVec[++anArgIter]);
      aTolType.LowerCase();
      if (aTolType == "uniform")
      {
        if (anArgIter + 1 >= theArgsNb)
        {
          Message::SendFail() << "Syntax error: wrong number of arguments";
          return 1;
        }
        aCtx->MainSelector()->SetDepthTolerance(SelectMgr_TypeOfDepthTolerance_Uniform,
                                                Draw::Atof(theArgVec[++anArgIter]));
      }
      else if (aTolType == "uniformpx")
      {
        if (anArgIter + 1 >= theArgsNb)
        {
          Message::SendFail() << "Syntax error: wrong number of arguments";
          return 1;
        }
        aCtx->MainSelector()->SetDepthTolerance(SelectMgr_TypeOfDepthTolerance_UniformPixels,
                                                Draw::Atof(theArgVec[++anArgIter]));
      }
      else if (aTolType == "sensfactor")
      {
        aCtx->MainSelector()->SetDepthTolerance(SelectMgr_TypeOfDepthTolerance_SensitivityFactor,
                                                0.0);
      }
      else
      {
        Message::SendFail() << "Syntax error at '" << aTolType << "'";
        return 1;
      }
    }
    else if ((anArg == "-mode" || anArg == "-dispmode") && anArgIter + 1 < theArgsNb)
    {
      if (aType == Prs3d_TypeOfHighlight_None)
      {
        Message::SendFail("Syntax error: type of highlighting is undefined");
        return 1;
      }

      const Standard_Integer      aDispMode = Draw::Atoi(theArgVec[++anArgIter]);
      const Handle(Prs3d_Drawer)& aStyle    = aCtx->HighlightStyle(aType);
      aStyle->SetDisplayMode(aDispMode);
      toRedraw = Standard_True;
    }
    else if (anArg == "-layer" && anArgIter + 1 < theArgsNb)
    {
      if (aType == Prs3d_TypeOfHighlight_None)
      {
        Message::SendFail("Syntax error: type of highlighting is undefined");
        return 1;
      }

      ++anArgIter;
      Graphic3d_ZLayerId aNewLayer = Graphic3d_ZLayerId_UNKNOWN;
      if (!ViewerTest::ParseZLayer(theArgVec[anArgIter], aNewLayer))
      {
        Message::SendFail() << "Syntax error at " << theArgVec[anArgIter];
        return 1;
      }

      const Handle(Prs3d_Drawer)& aStyle = aCtx->HighlightStyle(aType);
      aStyle->SetZLayer(aNewLayer);
      toRedraw = Standard_True;
    }
    else if (anArg == "-hicolor" || anArg == "-selcolor" || anArg == "-color")
    {
      if (anArg.StartsWith("-hi"))
      {
        aType = Prs3d_TypeOfHighlight_Dynamic;
      }
      else if (anArg.StartsWith("-sel"))
      {
        aType = Prs3d_TypeOfHighlight_Selected;
      }
      else if (aType == Prs3d_TypeOfHighlight_None)
      {
        Message::SendFail("Syntax error: type of highlighting is undefined");
        return 1;
      }

      Quantity_Color   aColor;
      Standard_Integer aNbParsed =
        Draw::ParseColor(theArgsNb - anArgIter - 1, theArgVec + anArgIter + 1, aColor);
      if (aNbParsed == 0)
      {
        Message::SendFail("Syntax error: need more arguments");
        return 1;
      }
      anArgIter += aNbParsed;

      const Handle(Prs3d_Drawer)& aStyle = aCtx->HighlightStyle(aType);
      aStyle->SetColor(aColor);
      toRedraw = Standard_True;
    }
    else if ((anArg == "-transp" || anArg == "-transparency" || anArg == "-hitransp"
              || anArg == "-seltransp" || anArg == "-hitransplocal" || anArg == "-seltransplocal")
             && anArgIter + 1 < theArgsNb)
    {
      if (anArg.StartsWith("-hi"))
      {
        aType = Prs3d_TypeOfHighlight_Dynamic;
      }
      else if (anArg.StartsWith("-sel"))
      {
        aType = Prs3d_TypeOfHighlight_Selected;
      }
      else if (aType == Prs3d_TypeOfHighlight_None)
      {
        Message::SendFail("Syntax error: type of highlighting is undefined");
        return 1;
      }

      const Standard_Real         aTransp = Draw::Atof(theArgVec[++anArgIter]);
      const Handle(Prs3d_Drawer)& aStyle  = aCtx->HighlightStyle(aType);
      aStyle->SetTransparency((Standard_ShortReal)aTransp);
      toRedraw = Standard_True;
    }
    else if ((anArg == "-mat" || anArg == "-material") && anArgIter + 1 < theArgsNb)
    {
      if (aType == Prs3d_TypeOfHighlight_None)
      {
        Message::SendFail("Syntax error: type of highlighting is undefined");
        return 1;
      }

      const Handle(Prs3d_Drawer)& aStyle = aCtx->HighlightStyle(aType);
      Graphic3d_NameOfMaterial    aMatName =
        Graphic3d_MaterialAspect::MaterialFromName(theArgVec[anArgIter + 1]);
      if (aMatName != Graphic3d_NameOfMaterial_DEFAULT)
      {
        ++anArgIter;
        Handle(Graphic3d_AspectFillArea3d) anAspect = new Graphic3d_AspectFillArea3d();
        *anAspect = *aCtx->DefaultDrawer()->ShadingAspect()->Aspect();
        Graphic3d_MaterialAspect aMat(aMatName);
        aMat.SetColor(aStyle->Color());
        aMat.SetTransparency(aStyle->Transparency());
        anAspect->SetFrontMaterial(aMat);
        anAspect->SetInteriorColor(aStyle->Color());
        aStyle->SetBasicFillAreaAspect(anAspect);
      }
      else
      {
        aStyle->SetBasicFillAreaAspect(Handle(Graphic3d_AspectFillArea3d)());
      }
      toRedraw = Standard_True;
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (toPrint)
  {
    const Handle(Prs3d_Drawer)& aHiStyle  = aCtx->HighlightStyle();
    const Handle(Prs3d_Drawer)& aSelStyle = aCtx->SelectionStyle();
    theDi << "Auto-activation                : "
          << (aCtx->GetAutoActivateSelection() ? "On" : "Off") << "\n";
    theDi << "Auto-highlight                 : " << (aCtx->AutomaticHilight() ? "On" : "Off")
          << "\n";
    theDi << "Highlight selected             : " << (aCtx->ToHilightSelected() ? "On" : "Off")
          << "\n";
    theDi << "Selection pixel tolerance      : " << aCtx->MainSelector()->PixelTolerance() << "\n";
    theDi << "Selection color                : "
          << Quantity_Color::StringName(aSelStyle->Color().Name()) << "\n";
    theDi << "Dynamic highlight color        : "
          << Quantity_Color::StringName(aHiStyle->Color().Name()) << "\n";
    theDi << "Selection transparency         : " << aSelStyle->Transparency() << "\n";
    theDi << "Dynamic highlight transparency : " << aHiStyle->Transparency() << "\n";
    theDi << "Selection mode                 : " << aSelStyle->DisplayMode() << "\n";
    theDi << "Dynamic highlight mode         : " << aHiStyle->DisplayMode() << "\n";
    theDi << "Selection layer                : " << aSelStyle->ZLayer() << "\n";
    theDi << "Dynamic layer                  : " << aHiStyle->ZLayer() << "\n";
  }

  if (aCtx->NbSelected() != 0 && toRedraw)
  {
    aCtx->HilightSelected(Standard_True);
  }

  return 0;
}

//=================================================================================================

static int VDumpSelectionImage(Draw_Interpretor& /*theDi*/,
                               Standard_Integer theArgsNb,
                               const char**     theArgVec)
{
  const Handle(AIS_InteractiveContext)& aContext = ViewerTest::GetAISContext();
  const Handle(V3d_View)&               aView    = ViewerTest::CurrentView();
  if (aContext.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  TCollection_AsciiString        aFile;
  StdSelect_TypeOfSelectionImage aType = StdSelect_TypeOfSelectionImage_NormalizedDepth;
  Handle(Graphic3d_Camera)       aCustomCam;
  Image_Format                   anImgFormat  = Image_Format_BGR;
  Standard_Integer               aPickedIndex = 1;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    TCollection_AsciiString aParam(theArgVec[anArgIter]);
    aParam.LowerCase();
    if (aParam == "-type")
    {
      if (++anArgIter >= theArgsNb)
      {
        Message::SendFail("Syntax error: wrong number parameters of flag '-type'");
        return 1;
      }

      TCollection_AsciiString aValue(theArgVec[anArgIter]);
      aValue.LowerCase();
      if (aValue == "depth" || aValue == "normdepth" || aValue == "normalizeddepth")
      {
        aType       = StdSelect_TypeOfSelectionImage_NormalizedDepth;
        anImgFormat = Image_Format_GrayF;
      }
      else if (aValue == "depthinverted" || aValue == "normdepthinverted"
               || aValue == "normalizeddepthinverted" || aValue == "inverted")
      {
        aType       = StdSelect_TypeOfSelectionImage_NormalizedDepthInverted;
        anImgFormat = Image_Format_GrayF;
      }
      else if (aValue == "unnormdepth" || aValue == "unnormalizeddepth")
      {
        aType       = StdSelect_TypeOfSelectionImage_UnnormalizedDepth;
        anImgFormat = Image_Format_GrayF;
      }
      else if (aValue == "objectcolor" || aValue == "object" || aValue == "color")
      {
        aType = StdSelect_TypeOfSelectionImage_ColoredDetectedObject;
      }
      else if (aValue == "entitycolor" || aValue == "entity")
      {
        aType = StdSelect_TypeOfSelectionImage_ColoredEntity;
      }
      else if (aValue == "entitytypecolor" || aValue == "entitytype")
      {
        aType = StdSelect_TypeOfSelectionImage_ColoredEntityType;
      }
      else if (aValue == "ownercolor" || aValue == "owner")
      {
        aType = StdSelect_TypeOfSelectionImage_ColoredOwner;
      }
      else if (aValue == "selectionmodecolor" || aValue == "selectionmode"
               || aValue == "selmodecolor" || aValue == "selmode")
      {
        aType = StdSelect_TypeOfSelectionImage_ColoredSelectionMode;
      }
      else if (aValue == "surfnormal" || aValue == "surfacenormal" || aValue == "normal")
      {
        aType = StdSelect_TypeOfSelectionImage_SurfaceNormal;
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown type '" << aValue << "'";
        return 1;
      }
    }
    else if (aParam == "-picked" || aParam == "-pickeddepth" || aParam == "-pickedindex")
    {
      if (++anArgIter >= theArgsNb)
      {
        Message::SendFail() << "Syntax error: wrong number parameters at '" << aParam << "'";
        return 1;
      }

      aPickedIndex = Draw::Atoi(theArgVec[anArgIter]);
    }
    else if (anArgIter + 1 < theArgsNb && aParam == "-xrpose")
    {
      TCollection_AsciiString anXRArg(theArgVec[++anArgIter]);
      anXRArg.LowerCase();
      if (anXRArg == "base")
      {
        aCustomCam = aView->View()->BaseXRCamera();
      }
      else if (anXRArg == "head")
      {
        aCustomCam = aView->View()->PosedXRCamera();
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown XR pose '" << anXRArg << "'";
        return 1;
      }
      if (aCustomCam.IsNull())
      {
        Message::SendFail() << "Error: undefined XR pose";
        return 0;
      }
    }
    else if (aFile.IsEmpty())
    {
      aFile = theArgVec[anArgIter];
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }
  if (aFile.IsEmpty())
  {
    Message::SendFail("Syntax error: image file name is missing");
    return 1;
  }

  Standard_Integer aWidth = 0, aHeight = 0;
  aView->Window()->Size(aWidth, aHeight);

  Image_AlienPixMap aPixMap;
  if (!aPixMap.InitZero(anImgFormat, aWidth, aHeight))
  {
    Message::SendFail("Error: can't allocate image");
    return 1;
  }

  const bool               wasImmUpdate = aView->SetImmediateUpdate(false);
  Handle(Graphic3d_Camera) aCamBack     = aView->Camera();
  if (!aCustomCam.IsNull())
  {
    aView->SetCamera(aCustomCam);
  }
  if (!aContext->MainSelector()->ToPixMap(aPixMap, aView, aType, aPickedIndex))
  {
    Message::SendFail("Error: can't generate selection image");
    return 1;
  }
  if (!aCustomCam.IsNull())
  {
    aView->SetCamera(aCamBack);
  }
  aView->SetImmediateUpdate(wasImmUpdate);

  if (!aPixMap.Save(aFile))
  {
    Message::SendFail("Error: can't save selection image");
    return 0;
  }
  return 0;
}

//=================================================================================================

static int VViewCube(Draw_Interpretor&, Standard_Integer theNbArgs, const char** theArgVec)
{
  const Handle(AIS_InteractiveContext)& aContext = ViewerTest::GetAISContext();
  const Handle(V3d_View)&               aView    = ViewerTest::CurrentView();
  if (aContext.IsNull() || aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }
  else if (theNbArgs < 2)
  {
    Message::SendFail("Syntax error: wrong number arguments");
    return 1;
  }

  Handle(AIS_ViewCube)    aViewCube;
  ViewerTest_AutoUpdater  anUpdateTool(aContext, aView);
  Quantity_Color          aColorRgb;
  TCollection_AsciiString aName;
  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anUpdateTool.parseRedrawMode(anArg))
    {
      //
    }
    else if (aViewCube.IsNull())
    {
      aName = theArgVec[anArgIter];
      if (aName.StartsWith("-"))
      {
        Message::SendFail("Syntax error: object name should be specified");
        return 1;
      }
      Handle(AIS_InteractiveObject) aPrs;
      GetMapOfAIS().Find2(aName, aPrs);
      aViewCube = Handle(AIS_ViewCube)::DownCast(aPrs);
      if (aViewCube.IsNull())
      {
        aViewCube = new AIS_ViewCube();
        aViewCube->SetBoxColor(Quantity_NOC_GRAY50);
        aViewCube->SetViewAnimation(ViewerTest::CurrentEventManager()->ViewAnimation());
        aViewCube->SetFixedAnimationLoop(false);
      }
    }
    else if (anArg == "-reset")
    {
      aViewCube->ResetStyles();
    }
    else if (anArg == "-color" || anArg == "-boxcolor" || anArg == "-boxsidecolor"
             || anArg == "-sidecolor" || anArg == "-boxedgecolor" || anArg == "-edgecolor"
             || anArg == "-boxcornercolor" || anArg == "-cornercolor" || anArg == "-innercolor"
             || anArg == "-textcolor" || anArg == "-xaxistextcolor" || anArg == "-yaxistextcolor"
             || anArg == "-zaxistextcolor")
    {
      Standard_Integer aNbParsed =
        Draw::ParseColor(theNbArgs - anArgIter - 1, theArgVec + anArgIter + 1, aColorRgb);
      if (aNbParsed == 0)
      {
        Message::SendFail() << "Syntax error at '" << anArg << "'";
        return 1;
      }
      anArgIter += aNbParsed;
      if (anArg == "-boxcolor")
      {
        aViewCube->SetBoxColor(aColorRgb);
      }
      else if (anArg == "-boxsidecolor" || anArg == "-sidecolor")
      {
        aViewCube->BoxSideStyle()->SetColor(aColorRgb);
        aViewCube->SynchronizeAspects();
      }
      else if (anArg == "-boxedgecolor" || anArg == "-edgecolor")
      {
        aViewCube->BoxEdgeStyle()->SetColor(aColorRgb);
        aViewCube->SynchronizeAspects();
      }
      else if (anArg == "-boxcornercolor" || anArg == "-cornercolor")
      {
        aViewCube->BoxCornerStyle()->SetColor(aColorRgb);
        aViewCube->SynchronizeAspects();
      }
      else if (anArg == "-innercolor")
      {
        aViewCube->SetInnerColor(aColorRgb);
      }
      else if (anArg == "-textcolor")
      {
        aViewCube->SetTextColor(aColorRgb);
      }
      else if (anArg == "-xaxistextcolor" || anArg == "-yaxistextcolor"
               || anArg == "-zaxistextcolor")
      {
        Prs3d_DatumParts aDatum =
          anArg.Value(2) == 'x'
            ? Prs3d_DatumParts_XAxis
            : (anArg.Value(2) == 'y' ? Prs3d_DatumParts_YAxis : Prs3d_DatumParts_ZAxis);
        aViewCube->Attributes()->SetOwnDatumAspects();
        aViewCube->Attributes()->DatumAspect()->TextAspect(aDatum)->SetColor(aColorRgb);
      }
      else
      {
        aViewCube->SetColor(aColorRgb);
      }
    }
    else if (anArgIter + 1 < theNbArgs && (anArg == "-transparency" || anArg == "-boxtransparency"))
    {
      const Standard_Real aValue = Draw::Atof(theArgVec[++anArgIter]);
      if (aValue < 0.0 || aValue > 1.0)
      {
        Message::SendFail() << "Syntax error: invalid transparency value " << theArgVec[anArgIter];
        return 1;
      }

      if (anArg == "-boxtransparency")
      {
        aViewCube->SetBoxTransparency(aValue);
      }
      else
      {
        aViewCube->SetTransparency(aValue);
      }
    }
    else if (anArg == "-axes" || anArg == "-edges" || anArg == "-vertices" || anArg == "-vertexes"
             || anArg == "-fixedanimation")
    {
      bool toShow = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], toShow))
      {
        ++anArgIter;
      }
      if (anArg == "-fixedanimation")
      {
        aViewCube->SetFixedAnimationLoop(toShow);
      }
      else if (anArg == "-axes")
      {
        aViewCube->SetDrawAxes(toShow);
      }
      else if (anArg == "-edges")
      {
        aViewCube->SetDrawEdges(toShow);
      }
      else
      {
        aViewCube->SetDrawVertices(toShow);
      }
    }
    else if (anArg == "-yup" || anArg == "-zup")
    {
      bool isOn = true;
      if (anArgIter + 1 < theNbArgs && Draw::ParseOnOff(theArgVec[anArgIter + 1], isOn))
      {
        ++anArgIter;
      }
      if (anArg == "-yup")
      {
        aViewCube->SetYup(isOn);
      }
      else
      {
        aViewCube->SetYup(!isOn);
      }
    }
    else if (anArgIter + 1 < theNbArgs && anArg == "-font")
    {
      aViewCube->SetFont(theArgVec[++anArgIter]);
    }
    else if (anArgIter + 1 < theNbArgs && anArg == "-fontheight")
    {
      aViewCube->SetFontHeight(Draw::Atof(theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs && (anArg == "-size" || anArg == "-boxsize"))
    {
      aViewCube->SetSize(Draw::Atof(theArgVec[++anArgIter]), anArg != "-boxsize");
    }
    else if (anArgIter + 1 < theNbArgs
             && (anArg == "-boxfacet" || anArg == "-boxfacetextension" || anArg == "-facetextension"
                 || anArg == "-extension"))
    {
      aViewCube->SetBoxFacetExtension(Draw::Atof(theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs && (anArg == "-boxedgegap" || anArg == "-edgegap"))
    {
      aViewCube->SetBoxEdgeGap(Draw::Atof(theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs && (anArg == "-boxedgeminsize" || anArg == "-edgeminsize"))
    {
      aViewCube->SetBoxEdgeMinSize(Draw::Atof(theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs
             && (anArg == "-boxcornerminsize" || anArg == "-cornerminsize"))
    {
      aViewCube->SetBoxCornerMinSize(Draw::Atof(theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs && anArg == "-axespadding")
    {
      aViewCube->SetAxesPadding(Draw::Atof(theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs && anArg == "-roundradius")
    {
      aViewCube->SetRoundRadius(Draw::Atof(theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs && anArg == "-duration")
    {
      aViewCube->SetDuration(Draw::Atof(theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs && anArg == "-axesradius")
    {
      aViewCube->SetAxesRadius(Draw::Atof(theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs && anArg == "-axesconeradius")
    {
      aViewCube->SetAxesConeRadius(Draw::Atof(theArgVec[++anArgIter]));
    }
    else if (anArgIter + 1 < theNbArgs && anArg == "-axessphereradius")
    {
      aViewCube->SetAxesSphereRadius(Draw::Atof(theArgVec[++anArgIter]));
    }
    else if (anArg == "-orthopers")
    {
      const Handle(Graphic3d_TransformPers)& aTrsfPers = aViewCube->TransformPersistence();
      Handle(Graphic3d_TransformPers)        anOrthoPers =
        new Graphic3d_TransformPers(Graphic3d_TMF_TriedronPers | Graphic3d_TMF_OrthoPers,
                                    aTrsfPers->Corner2d(),
                                    aTrsfPers->Offset2d());
      aViewCube->SetTransformPersistence(anOrthoPers);
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << anArg << "'";
      return 1;
    }
  }
  if (aViewCube.IsNull())
  {
    Message::SendFail("Syntax error: wrong number of arguments");
    return 1;
  }

  ViewerTest::Display(aName, aViewCube, false);
  return 0;
}

//! Parse color type argument.
static bool parseColorType(const char* theString, Quantity_TypeOfColor& theType)
{
  TCollection_AsciiString aType(theString);
  aType.LowerCase();
  if (aType == "rgb")
  {
    theType = Quantity_TOC_RGB;
  }
  else if (aType == "srgb")
  {
    theType = Quantity_TOC_sRGB;
  }
  else if (aType == "hex")
  {
    theType = Quantity_TOC_sRGB;
  }
  else if (aType == "name")
  {
    theType = Quantity_TOC_sRGB;
  }
  else if (aType == "hls")
  {
    theType = Quantity_TOC_HLS;
  }
  else if (aType == "lab")
  {
    theType = Quantity_TOC_CIELab;
  }
  else if (aType == "lch")
  {
    theType = Quantity_TOC_CIELch;
  }
  else
  {
    return false;
  }
  return true;
}

//=================================================================================================

static int VColorConvert(Draw_Interpretor& theDI,
                         Standard_Integer  theNbArgs,
                         const char**      theArgVec)
{
  Quantity_TypeOfColor aTypeFrom = Quantity_TOC_RGB, aTypeTo = Quantity_TOC_RGB;
  double               anInput[4] = {};
  Quantity_ColorRGBA   aColor(0.0f, 0.0f, 0.0f, 1.0f);
  bool                 toPrintHex = false, toPrintName = false, hasAlpha = false;
  for (int anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArgCase(theArgVec[anArgIter]);
    anArgCase.LowerCase();
    if ((anArgCase == "-from" || anArgCase == "from") && anArgIter + 1 < theNbArgs
        && parseColorType(theArgVec[anArgIter + 1], aTypeFrom))
    {
      ++anArgIter;
    }
    else if ((anArgCase == "-to" || anArgCase == "to") && anArgIter + 1 < theNbArgs
             && parseColorType(theArgVec[anArgIter + 1], aTypeTo))
    {
      TCollection_AsciiString aToStr(theArgVec[++anArgIter]);
      aToStr.LowerCase();
      toPrintHex  = (aToStr == "hex");
      toPrintName = (aToStr == "name");
    }
    else if (Quantity_ColorRGBA::ColorFromHex(theArgVec[anArgIter], aColor))
    {
      hasAlpha = anArgCase.Length() >= 8;
    }
    else if (Quantity_Color::ColorFromName(theArgVec[anArgIter], aColor.ChangeRGB()))
    {
      //
    }
    else if (anArgIter + 2 < theNbArgs && Draw::ParseReal(theArgVec[anArgIter + 0], anInput[0])
             && Draw::ParseReal(theArgVec[anArgIter + 1], anInput[1])
             && Draw::ParseReal(theArgVec[anArgIter + 2], anInput[2]))
    {
      if (anArgIter + 3 < theNbArgs && Draw::ParseReal(theArgVec[anArgIter + 3], anInput[3]))
      {
        anArgIter += 1;
        aColor.SetAlpha((float)anInput[3]);
        hasAlpha = true;
      }
      anArgIter += 2;
      aColor.ChangeRGB().SetValues(anInput[0], anInput[1], anInput[2], aTypeFrom);
    }
    else
    {
      theDI << "Syntax error: unknown argument '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }

  if (toPrintHex)
  {
    if (hasAlpha || aColor.Alpha() < 1.0f)
    {
      theDI << Quantity_ColorRGBA::ColorToHex(aColor);
    }
    else
    {
      theDI << Quantity_Color::ColorToHex(aColor.GetRGB());
    }
  }
  else if (toPrintName)
  {
    theDI << Quantity_Color::StringName(aColor.GetRGB().Name());
  }
  else
  {
    double anOutput[3] = {};
    aColor.GetRGB().Values(anOutput[0], anOutput[1], anOutput[2], aTypeTo);

    // print values with 6 decimal digits
    char aBuffer[1024];
    if (hasAlpha || aColor.Alpha() < 1.0f)
    {
      Sprintf(aBuffer,
              "%.6f %.6f %.6f %.6f",
              anOutput[0],
              anOutput[1],
              anOutput[2],
              aColor.Alpha());
    }
    else
    {
      Sprintf(aBuffer, "%.6f %.6f %.6f", anOutput[0], anOutput[1], anOutput[2]);
    }
    theDI << aBuffer;
  }
  return 0;
}

//=================================================================================================

static int VColorDiff(Draw_Interpretor& theDI, Standard_Integer theNbArgs, const char** theArgVec)
{
  if (theNbArgs != 7)
  {
    std::cerr << "Error: command syntax is incorrect, see help" << std::endl;
    return 1;
  }

  double aR1 = Draw::Atof(theArgVec[1]);
  double aG1 = Draw::Atof(theArgVec[2]);
  double aB1 = Draw::Atof(theArgVec[3]);
  double aR2 = Draw::Atof(theArgVec[4]);
  double aG2 = Draw::Atof(theArgVec[5]);
  double aB2 = Draw::Atof(theArgVec[6]);

  Quantity_Color aColor1(aR1, aG1, aB1, Quantity_TOC_RGB);
  Quantity_Color aColor2(aR2, aG2, aB2, Quantity_TOC_RGB);

  theDI << aColor1.DeltaE2000(aColor2);

  return 0;
}

//=================================================================================================

static int VSelBvhBuild(Draw_Interpretor& /*theDI*/,
                        Standard_Integer theNbArgs,
                        const char**     theArgVec)
{
  const Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  if (theNbArgs < 2)
  {
    Message::SendFail("Error: command syntax is incorrect, see help");
    return 1;
  }

  Standard_Integer toEnable   = -1;
  Standard_Integer aThreadsNb = -1;
  Standard_Boolean toWait     = Standard_False;

  for (Standard_Integer anArgIter = 1; anArgIter < theNbArgs; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();

    if (anArg == "-nbthreads" && anArgIter + 1 < theNbArgs)
    {
      aThreadsNb = Draw::Atoi(theArgVec[++anArgIter]);
      if (aThreadsNb < 1)
      {
        aThreadsNb = Max(1, OSD_Parallel::NbLogicalProcessors() - 1);
      }
    }
    else if (anArg == "-wait")
    {
      toWait = Standard_True;
    }
    else if (toEnable == -1)
    {
      Standard_Boolean toEnableValue = Standard_True;
      if (Draw::ParseOnOff(anArg.ToCString(), toEnableValue))
      {
        toEnable = toEnableValue ? 1 : 0;
      }
      else
      {
        Message::SendFail() << "Syntax error: unknown argument '" << anArg << "'";
        return 1;
      }
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << anArg << "'";
      return 1;
    }
  }

  if (aThreadsNb == -1)
  {
    aThreadsNb = 1;
  }
  if (toEnable != -1)
  {
    aCtx->MainSelector()->SetToPrebuildBVH(toEnable == 1, aThreadsNb);
  }
  if (toWait)
  {
    aCtx->MainSelector()->WaitForBVHBuild();
  }

  return 0;
}

//=================================================================================================

static int VChangeMouseGesture(Draw_Interpretor&,
                               Standard_Integer theArgsNb,
                               const char**     theArgVec)
{
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  NCollection_DoubleMap<TCollection_AsciiString, AIS_MouseGesture> aGestureMap;
  {
    aGestureMap.Bind("none", AIS_MouseGesture_NONE);
    aGestureMap.Bind("selectrectangle", AIS_MouseGesture_SelectRectangle);
    aGestureMap.Bind("selectlasso", AIS_MouseGesture_SelectLasso);
    aGestureMap.Bind("zoom", AIS_MouseGesture_Zoom);
    aGestureMap.Bind("zoomwindow", AIS_MouseGesture_ZoomWindow);
    aGestureMap.Bind("pan", AIS_MouseGesture_Pan);
    aGestureMap.Bind("rotateorbit", AIS_MouseGesture_RotateOrbit);
    aGestureMap.Bind("rotateview", AIS_MouseGesture_RotateView);
    aGestureMap.Bind("drag", AIS_MouseGesture_Drag);
  }
  NCollection_DoubleMap<TCollection_AsciiString, Standard_UInteger> aMouseButtonMap;
  {
    aMouseButtonMap.Bind("none", (Standard_UInteger)Aspect_VKeyMouse_NONE);
    aMouseButtonMap.Bind("left", (Standard_UInteger)Aspect_VKeyMouse_LeftButton);
    aMouseButtonMap.Bind("middle", (Standard_UInteger)Aspect_VKeyMouse_MiddleButton);
    aMouseButtonMap.Bind("right", (Standard_UInteger)Aspect_VKeyMouse_RightButton);
  }

  Standard_UInteger aButton  = (Standard_UInteger)Aspect_VKeyMouse_LeftButton;
  AIS_MouseGesture  aGesture = AIS_MouseGesture_RotateOrbit;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgsNb; ++anArgIter)
  {
    Standard_CString        anArg = theArgVec[anArgIter];
    TCollection_AsciiString anArgCase(anArg);
    anArgCase.LowerCase();
    if (anArgCase == "-button")
    {
      TCollection_AsciiString aButtonStr = theArgVec[++anArgIter];
      aButtonStr.LowerCase();
      aButton = aMouseButtonMap.Find1(aButtonStr);
    }
    else if (anArgCase == "-gesture")
    {
      TCollection_AsciiString aGestureStr = theArgVec[++anArgIter];
      aGestureStr.LowerCase();
      aGesture = aGestureMap.Find1(aGestureStr);
    }
    else
    {
      Message::SendFail() << "Error: unknown argument '" << anArg << "'";
      return 1;
    }
  }

  Handle(ViewerTest_EventManager) aViewMgr = ViewerTest::CurrentEventManager();
  aViewMgr->ChangeMouseGestureMap().Bind(aButton, aGesture);

  return 0;
}

//=================================================================================================

static void ViewerTest_ExitProc(ClientData)
{
  NCollection_List<TCollection_AsciiString> aViewList;
  for (NCollection_DoubleMap<TCollection_AsciiString, Handle(V3d_View)>::Iterator anIter(
         ViewerTest_myViews);
       anIter.More();
       anIter.Next())
  {
    aViewList.Append(anIter.Key1());
  }

  for (NCollection_List<TCollection_AsciiString>::Iterator anIter(aViewList); anIter.More();
       anIter.Next())
  {
    ViewerTest::RemoveView(anIter.Value(), true);
  }
}

//=================================================================================================

void ViewerTest::ViewerCommands(Draw_Interpretor& theCommands)
{
  static bool TheIsInitialized = false;
  if (TheIsInitialized)
  {
    return;
  }

  TheIsInitialized = true;
  // define destruction callback to destroy views in a well-defined order
  Tcl_CreateExitHandler(ViewerTest_ExitProc, 0);

  const char* aGroup    = "AIS Viewer";
  const char* aFileName = __FILE__;
  auto        addCmd =
    [&](const char* theName, Draw_Interpretor::CommandFunction theFunc, const char* theHelp) {
      theCommands.Add(theName, theHelp, aFileName, theFunc, aGroup);
    };

  addCmd("vdriver", VDriver, /* [vdriver] */ R"(
vdriver [-list] [-default DriverName] [-load DriverName]
Manages active graphic driver factory.
Prints current active driver when called without arguments.
Makes specified driver active when ActiveName argument is specified.
 -list    print registered factories
 -default define which factory should be used by default (to be used by next vinit call)
 -load    try loading factory plugin and set it as default one
)" /* [vdriver] */);

  addCmd("vinit", VInit, /* [vinit] */ R"(
vinit [-name viewName] [-left leftPx] [-top topPx] [-width widthPx] [-height heightPx]
      [-exitOnClose] [-closeOnEscape] [-cloneActive] [-virtual {0|1}]=0 [-2d_mode {0|1}]=0
      [-display displayName] [-dpiAware {0|1}]=0
      [-subview] [-parent OtherView] [-composer {0|1}]=0 [-margins DX DY]=0
Creates new View window with specified name viewName.
By default the new view is created in the viewer and in graphic driver shared with active view.
 -name {driverName/viewerName/viewName | viewerName/viewName | viewName}
       if driverName isn't specified the driver will be shared with active view;
       if viewerName isn't specified the viewer will be shared with active view.
 -display HostName.DisplayNumber[:ScreenNumber]

Display name will be used within creation of graphic driver, when specified.
 -left,  -top    pixel position of left top corner of the window.
 -width, -height width and height of window respectively.
 -cloneActive flag to copy camera and dimensions of active view.
 -exitOnClose when specified, closing the view will exit application.
 -closeOnEscape when specified, view will be closed on pressing Escape.
 -virtual create an offscreen window within interactive session
 -subview create a subview within another view
 -2d_mode when on, view will not react on rotate scene events
 -dpiAware override dpi aware hint (Windows platform)
Additional commands for operations with views: vclose, vactivate, vviewlist.
)" /* [vinit] */);

  addCmd("vclose", VClose, /* [vclose] */ R"(
vclose [view_id [keep_context=0|1]]
or vclose ALL - to remove all created views
 - removes view(viewer window) defined by its view_id.
 - keep_context: by default 0; if 1 and the last view is deleted the current context is not removed.
)" /* [vclose] */);

  addCmd("vactivate", VActivate, /* [vactivate] */ R"(
vactivate view_id [-noUpdate]
Activates view(viewer window) defined by its view_id.
)" /* [vactivate] */);

  addCmd("vviewlist", VViewList, /* [vviewlist] */ R"(
vviewlist [format={tree, long}]=tree
Prints current list of views per viewer and graphic_driver ID shared between viewers
 - format: format of result output, if tree the output is a tree view;
           otherwise it's a list of full view names.
)" /* [vviewlist] */);

  addCmd("vhelp", VHelp, /* [vhelp] */ R"(
vhelp : display help on the viewer commands and list of hotkeys.
)" /* [vhelp] */);

  addCmd("vviewproj", VViewProj, /* [vviewproj] */ R"(
vviewproj [top|bottom|left|right|front|back|axoLeft|axoRight]
          [+-X+-Y+-Z] [-Zup|-Yup] [-frame +-X+-Y]
Setup view direction
 -Yup      use Y-up convention instead of Zup (which is default).
 +-X+-Y+-Z define direction as combination of DX, DY and DZ;
           for example '+Z' will show front of the model,
           '-X-Y+Z' will define left axonometric view.
 -frame    define camera Up and Right directions (regardless Up convention);
           for example '+X+Z' will show front of the model with Z-up.
)" /* [vviewproj] */);

  addCmd("vtop", VViewProj, /* [vtop] */ R"(
vtop or <T> : Display top view (+X+Y) in the 3D viewer window.
)" /* [vtop] */);

  addCmd("vbottom", VViewProj, /* [vbottom] */ R"(
vbottom : Display bottom view (+X-Y) in the 3D viewer window.
)" /* [vbottom] */);

  addCmd("vleft", VViewProj, /* [vleft] */ R"(
vleft : Display left view (-Y+Z) in the 3D viewer window.
)" /* [vleft] */);

  addCmd("vright", VViewProj, /* [vright] */ R"(
vright : Display right view (+Y+Z) in the 3D viewer window.
)" /* [vright] */);

  addCmd("vaxo", VViewProj, /* [vaxo] */ R"(
vaxo or <A> : Display axonometric view (+X-Y+Z) in the 3D viewer window.
)" /* [vaxo] */);

  addCmd("vfront", VViewProj, /* [vfront] */ R"(
vfront : Display front view (+X+Z) in the 3D viewer window.
)" /* [vfront] */);

  addCmd("vback", VViewProj, /* [vfront] */ R"(
vback : Display back view (-X+Z) in the 3D viewer window.
)" /* [vback] */);

  addCmd("vpick", VPick, /* [vpick] */ R"(
vpick X Y Z [shape subshape]
)" /* [vpick] */);

  addCmd("vfit", VFit, /* [vfit] */ R"(
vfit or <F> [-selected] [-noupdate]
Fit all / selected. Objects in the view are visualized to occupy the maximum surface.
)" /* [vfit] */);

  addCmd("vfitarea", VFitArea, /* [vfitarea] */ R"(
vfitarea [x1 y1 x2 y2] [x1 y1 z1 x2 y2 z2]
Fit view to show area located between two points
given in world 2D or 3D coordinates.
)" /* [vfitarea] */);

  addCmd("vzfit", VZFit, /* [vzfit] */ R"(
vzfit [scale]
Automatic depth panning.
Matches Z near, Z far view volume planes to the displayed objects.
 - "scale" specifies factor to scale computed z range.
)" /* [vzfit] */);

  addCmd("vrepaint", VRepaint, /* [vrepaint] */ R"(
vrepaint [-immediate] [-continuous FPS]
Force redraw of active View.
 -immediate  flag performs redraw of immediate layers only;
 -continuous activates/deactivates continuous redraw of active View,
             0 means no continuous rendering,
            -1 means non-stop redraws,
            >0 specifies target framerate.
)" /* [vrepaint] */);

  addCmd("vclear", VClear, /* [vclear] */ R"(
vclear : Remove all the object from the viewer
)" /* [vclear] */);

  addCmd("vbackground", VBackground, /* [vbackground] */ R"(
vbackground [-color Color [-default]]
    [-gradient Color1 Color2 [-default]
    [-gradientMode {NONE|HORIZONTAL|VERTICAL|DIAG1|DIAG2|CORNER1|CORNER2|CORNER3|ELLIPTICAL}]=VERT]
    [-imageFile ImageFile [-imageMode {CENTERED|TILED|STRETCH|NONE}]=CENTERED [-srgb {0|1}]=1]
    [-cubemap CubemapFile1 [CubeMapFiles2-5] [-order TilesIndexes1-6] [-invertedz]=0]
    [-skydome [-sunDir X Y Z=0 1 0] [-cloud Cloudy=0.2] [-time Time=0.0]
              [-fog Haze=0.0] [-size SizePx=512]]
    [-pbrEnv {ibl|noibl|keep}]
Changes background or some background settings.
 -color        sets background color
 -gradient     sets background gradient starting and ending colors
 -gradientMode sets gradient fill method
 -default      sets background default gradient or color
 -imageFile    sets filename of image used as background
 -imageMode    sets image fill type
 -cubemap      sets environment cubemap as background
 -invertedz    sets inversion of Z axis for background cubemap rendering; FALSE when unspecified
 -pbrEnv       sets on/off Image Based Lighting (IBL) from background cubemap for PBR
 -srgb         prefer sRGB texture format when applicable; TRUE when unspecified"
 -order        defines order of tiles in one image cubemap
               TileIndexi defubes an index in range [0, 5] for i tile of one image packed cubemap
               (has no effect in case of multi-image cubemaps).
Skydome background parameters (generated cubemap):
 -skydome      sets procedurally generated skydome as background
 -sunDir       sets direction to the sun, direction with negative y component represents moon direction (-x, -y, -z)
 -cloud        sets cloud intensity (0.0 - clear sky, 1.0 - very high cloudy)
 -time         might be tweaked to slightly change appearance of clouds
 -fog          sets mist intensity (0.0 - no mist at all, 1.0 - high mist)
 -size         sets size in pixels of cubemap side
)" /* [vbackground] */);

  addCmd("vsetbg", VBackground, /* [vsetbg] */ R"(
Alias for 'vbackground -imageFile ImageFile [-imageMode FillType]'.
)" /* [vsetbg] */);

  addCmd("vsetbgmode", VBackground, /* [vsetbgmode] */ R"(
Alias for 'vbackground -imageMode FillType'.
)" /* [vsetbgmode] */);

  addCmd("vsetgradientbg", VBackground, /* [vsetgradientbg] */ R"(
Alias for 'vbackground -gradient Color1 Color2 -gradientMode FillMethod'.
)" /* [vsetgradientbg] */);

  addCmd("vsetgrbgmode", VBackground, /* [vsetgrbgmode] */ R"(
Alias for 'vbackground -gradientMode FillMethod'.
)" /* [vsetgrbgmode] */);

  addCmd("vsetcolorbg", VBackground, /* [vsetcolorbg] */ R"(
Alias for 'vbackground -color Color'.
)" /* [vsetcolorbg] */);

  addCmd("vsetdefaultbg", VBackground, /* [vsetdefaultbg] */ R"(
Alias for 'vbackground -default -gradient Color1 Color2 [-gradientMode FillMethod]'
  and for 'vbackground -default -color Color'.
)" /* [vsetdefaultbg] */);

  addCmd("vscale", VScale, /* [vscale] */ R"(
vscale X Y Z
)" /* [vscale] */);

  addCmd("vzbufftrihedron", VZBuffTrihedron, /* [vzbufftrihedron] */ R"(
vzbufftrihedron [{-on|-off}=-on] [-type {wireframe|zbuffer}=zbuffer]
       [-position center|left_lower|left_upper|right_lower|right_upper]
       [-scale value=0.1] [-size value=0.8] [-arrowDiam value=0.05]
       [-colorArrowX color=RED] [-colorArrowY color=GREEN] [-colorArrowZ color=BLUE]
       [-nbfacets value=12] [-colorLabels color=WHITE]
       [-colorLabelX color] [-colorLabelY color] [-colorLabelZ color]
Displays a trihedron.
)" /* [vzbufftrihedron] */);

  addCmd("vrotate", VRotate, /* [vrotate] */ R"(
vrotate [[-mouseStart X Y] [-mouseMove X Y]]|[AX AY AZ [X Y Z]]
 -mouseStart start rotation according to the mouse position;
 -mouseMove  continue rotation with angle computed
             from last and new mouse position.
)" /* [vrotate] */);

  addCmd("vzoom", VZoom, /* [vzoom] */ R"(
vzoom coef
)" /* [vzoom] */);

  addCmd("vpan", VPan, /* [vpan] */ R"(
vpan dx dy
)" /* [vpan] */);

  addCmd("vcolorscale", VColorScale, /* [vcolorscale] */ R"(
vcolorscale name [-noupdate|-update] [-demo]
      [-range RangeMin=0 RangeMax=1 NbIntervals=10]
      [-font HeightFont=20]
      [-logarithmic {on|off}=off] [-reversed {on|off}=off]
      [-smoothTransition {on|off}=off]
      [-hueRange MinAngle=230 MaxAngle=0]
      [-colorRange MinColor=BLUE1 MaxColor=RED]
      [-textPos {left|right|center|none}=right]
      [-labelAtBorder {on|off}=on]
      [-colors Color1 Color2 ...] [-color Index Color]
      [-labels Label1 Label2 ...] [-label Index Label]
      [-freeLabels NbOfLabels Label1 Label2 ...]
      [-xy Left=0 Bottom=0]
      [-uniform lightness hue_from hue_to]
 -demo       display a color scale with demonstration values
 -colors     set colors for all intervals
 -color      set color for specific interval
 -uniform    generate colors with the same lightness
 -textpos    horizontal label position relative to color scale bar
 -labelAtBorder vertical label position relative to color interval;
             at border means the value inbetween neighbor intervals,
             at center means the center value within current interval
 -labels     set labels for all intervals
 -freeLabels same as -labels but does not require
             matching the number of intervals
 -label      set label for specific interval
 -title      set title
 -reversed   setup smooth color transition between intervals
 -smoothTransition swap colorscale direction
 -hueRange   set hue angles corresponding to minimum and maximum values
)" /* [vcolorscale] */);

  addCmd("vgraduatedtrihedron", VGraduatedTrihedron, /* [vgraduatedtrihedron] */ R"(
vgraduatedtrihedron : -on/-off [-xname Name] [-yname Name] [-zname Name] [-arrowlength Value]
    [-namefont Name] [-valuesfont Name]
    [-xdrawname on/off] [-ydrawname on/off] [-zdrawname on/off]
    [-xnameoffset IntVal] [-ynameoffset IntVal] [-znameoffset IntVal]
    [-xnamecolor Color] [-ynamecolor Color] [-znamecolor Color]
    [-xdrawvalues on/off] [-ydrawvalues on/off] [-zdrawvalues on/off]
    [-xvaluesoffset IntVal] [-yvaluesoffset IntVal] [-zvaluesoffset IntVal]
    [-xcolor Color] [-ycolor Color] [-zcolor Color]
    [-xdrawticks on/off] [-ydrawticks on/off] [-zdrawticks on/off]
    [-xticks Number] [-yticks Number] [-zticks Number]
    [-xticklength IntVal] [-yticklength IntVal] [-zticklength IntVal]
    [-drawgrid on/off] [-drawaxes on/off]
Display or erase graduated trihedron
 - xname, yname, zname - names of axes, default: X, Y, Z
 - namefont - font of axes names. Default: Arial
 - xnameoffset, ynameoffset, znameoffset - offset of name
   from values or tickmarks or axis. Default: 30
 - xnamecolor, ynamecolor, znamecolor - colors of axes names
 - xvaluesoffset, yvaluesoffset, zvaluesoffset - offset of values
   from tickmarks or axis. Default: 10
 - valuesfont - font of axes values. Default: Arial
 - xcolor, ycolor, zcolor - color of axis and values
 - xticks, yticks, xzicks - number of tickmark on axes. Default: 5
 - xticklength, yticklength, xzicklength - length of tickmark on axes. Default: 10
)" /* [vgraduatedtrihedron] */);

  addCmd("vtile", VTile, /* [vtile] */ R"(
vtile [-totalSize W H] [-lowerLeft X Y] [-upperLeft X Y] [-tileSize W H]
Setup view to draw a tile (a part of virtual bigger viewport).
 -totalSize the size of virtual bigger viewport
 -tileSize  tile size (the view size will be used if omitted)
 -lowerLeft tile offset as lower left corner
 -upperLeft tile offset as upper left corner
)" /* [vtile] */);

  addCmd("vzlayer", VZLayer, /* [vzlayer] */ R"(
vzlayer [layerId]
        [-add|-delete|-get|-settings] [-insertBefore AnotherLayer] [-insertAfter AnotherLayer]
        [-origin X Y Z] [-cullDist Distance] [-cullSize Size]
        [-enable|-disable {depthTest|depthWrite|depthClear|depthoffset}]
        [-enable|-disable {positiveOffset|negativeOffset|textureenv|rayTracing}]
ZLayer list management
 -add      add new z layer to viewer and print its id
 -insertBefore add new z layer and insert it before existing one
 -insertAfter  add new z layer and insert it after  existing one
 -delete   delete z layer
 -get      print sequence of z layers
 -settings print status of z layer settings
 -disable  disables given setting
 -enable   enables  given setting
)" /* [vzlayer] */);

  addCmd("vlayerline", VLayerLine, /* [vlayerline] */ R"(
vlayerline x1 y1 x2 y2 [linewidth=0.5] [linetype=0] [transparency=1.0]
)" /* [vlayerline] */);

  addCmd("vgrid", VGrid, /* [vgrid] */ R"(
vgrid [off] [-type {rect|circ}] [-mode {line|point}] [-origin X Y] [-rotAngle Angle] [-zoffset DZ]
      [-step X Y] [-size DX DY]
      [-step StepRadius NbDivisions] [-radius Radius]
)" /* [vgrid] */);

  addCmd("vpriviledgedplane", VPriviledgedPlane, /* [vpriviledgedplane] */ R"(
vpriviledgedplane [Ox Oy Oz Nx Ny Nz [Xx Xy Xz]]
Sets or prints viewer's priviledged plane geometry:
  Ox, Oy, Oz - plane origin;
  Nx, Ny, Nz - plane normal direction;
  Xx, Xy, Xz - plane x-reference axis direction.
)" /* [vpriviledgedplane] */);

  addCmd("vconvert", VConvert, /* [vconvert] */ R"(
vconvert v [Mode={window|view}]
vconvert x y [Mode={window|view|grid|ray}]
vconvert x y z [Mode={window|grid}]
Convert the given coordinates to window/view/model space:
 - window - convert to window coordinates, pixels;
 - view   - convert to view projection plane;
 - grid   - convert to model coordinates, given on grid;
 - ray    - convert projection ray to model coordinates.
)" /* [vconvert] */);

  addCmd("vfps", VFps, /* [vfps] */ R"(
vfps [framesNb=100] [-duration seconds] : estimate average frame rate for active view.
)" /* [vfps] */);

  addCmd("vstereo", VStereo, /* [vstereo] */ R"(
vstereo [0|1] [-mode Mode] [-reverse {0|1}]
        [-mirrorComposer] [-hmdfov2d AngleDegrees] [-unitFactor MetersFactor]
        [-anaglyph Filter] [-smoothInterlacing]
Control stereo output mode. Available modes for -mode:
  quadBuffer       OpenGL QuadBuffer stereo;
    requires driver support;
    should be called BEFORE vinit!
  anaglyph         Anaglyph glasses, filters for -anaglyph:
    redCyan, redCyanSimple, yellowBlue, yellowBlueSimple, greenMagentaSimple.
  rowInterlaced    row-interlaced display
    smooth         smooth interlaced output for better text readability
  columnInterlaced column-interlaced display
  chessBoard       chess-board output
  sideBySide       horizontal pair
  overUnder        vertical   pair
  openVR           OpenVR (HMD), extra options:
    -mirrorComposer flag to mirror VR frame in the window (debug);
    -unitFactor     specifies meters scale factor for mapping VR input.
)" /* [vstereo] */);

  addCmd("vmemgpu", VMemGpu, /* [vmemgpu] */ R"(
vmemgpu [f]: print system-dependent GPU memory information if available;
with f option returns free memory in bytes.
)" /* [vmemgpu] */);

  addCmd("vreadpixel", VReadPixel, /* [vreadpixel] */ R"(
vreadpixel xPixel yPixel [{rgb|rgba|sRGB|sRGBa|depth|hls|rgbf|rgbaf}=rgba] [-name|-hex]
Read pixel value for active view.
)" /* [vreadpixel] */);

  addCmd("diffimage", VDiffImage, /* [diffimage] */ R"(
diffimage imageFile1 imageFile2 [diffImageFile]
          [-toleranceOfColor {0..1}=0] [-blackWhite {on|off}=off] [-borderFilter {on|off}=off]
          [-display viewName prsName1 prsName2 prsNameDiff] [-exitOnClose] [-closeOnEscape]
Compare two images by content and generate difference image.
When -exitOnClose is specified, closing the view will exit application.
When -closeOnEscape is specified, view will be closed on pressing Escape.
)" /* [diffimage] */);

  addCmd("vselect", VSelect, /* [vselect] */ R"(
vselect x1 y1 [x2 y2 [x3 y3 ... xn yn]] [-allowoverlap 0|1]
        [-replace|-replaceextra|-xor|-add|-remove]
Emulate different types of selection:
 1) Single click selection.
 2) Selection with rectangle having corners at pixel positions (x1,y1) and (x2,y2).
 3) Selection with polygon having corners in pixel positions (x1,y1), (x2,y2),...,(xn,yn).
 4) -allowoverlap manages overlap and inclusion detection in rectangular and polygonal selection.
    If the flag is set to 1, both sensitives that were included completely
    and overlapped partially by defined rectangle or polygon will be detected,
    otherwise algorithm will chose only fully included sensitives.
    Default behavior is to detect only full inclusion
    (partial inclusion - overlap - is not allowed by default).
 5) Selection scheme replace, replaceextra, xor, add or remove (replace by default).
)" /* [vselect] */);

  addCmd("vselectpriority", VSelectPriority, /* [vselectpriority] */ R"(
vselectpriority name [value]
Prints or sets the selection priority for an object.
)" /* [vselectpriority] */);

  addCmd("vmoveto", VMoveTo, /* [vmoveto] */ R"(
vmoveto [x y] [-reset]
Emulate cursor movement to pixel position (x,y).
 -reset resets current highlighting.
)" /* [vmoveto] */);

  addCmd("vselaxis", VSelectByAxis, /* [vselaxis] */ R"(
vselaxis x y z dx dy dz [-onlyTop 0|1] [-display Name] [-showNormal 0|1]"
Provides intersection by given axis and print result intersection points.
 -onlyTop       switches On/Off mode to find only top point or all;
 -display Name  displays intersecting axis and result intersection points for debug goals;
 -showNormal    adds displaying of normal in intersection point or not.
)" /* [vselaxis] */);

  addCmd("vviewparams", VViewParams, /* [vviewparams] */ R"(
vviewparams [-args] [-scale [s]]
            [-eye [x y z]] [-at [x y z]] [-up [x y z]]
            [-proj [x y z]] [-center x y] [-size sx]
Manage current view parameters (camera orientation) or prints all
current values when called without argument.
 -scale [s]    prints or sets viewport relative scale
 -eye  [x y z] prints or sets eye location
 -at   [x y z] prints or sets center of look
 -up   [x y z] prints or sets direction of up vector
 -proj [x y z] prints or sets direction of look
 -center x y   sets location of center of the screen in pixels
 -size [sx]    prints viewport projection width and height sizes
               or changes the size of its maximum dimension
 -args         prints vviewparams arguments for restoring current view
)" /* [vviewparams] */);

  addCmd("v2dmode", V2DMode, /* [v2dmode] */ R"(
v2dmode [-name viewName] [-mode {-on|-off}=-on]
  name - name of existing view, if not defined, the active view is changed;
  mode - switches On/Off rotation mode.
Set 2D mode of the active viewer manipulating. The following mouse and key actions are disabled:
 - rotation of the view by 3rd mouse button with Ctrl active
 - set view projection using key buttons: A/D/T/B/L/R for AXO, Reset, Top, Bottom, Left, Right
View camera position might be changed only by commands.
)" /* [v2dmode] */);

  addCmd("vanimation", VAnimation, /* [vanimation] */ R"(
Alias for vanim
)" /* [vanimation] */);

  addCmd("vanim", VAnimation, /* [vanim] */ R"(
List existing animations:
  vanim

Animation playback:
  vanim name {-play|-resume|-pause|-stop} [playFrom [playDuration]] [-speed Coeff]
             [-freeLook] [-noPauseOnClick] [-lockLoop] [-elapsedTime]

  -speed          playback speed (1.0 is normal speed)
  -freeLook       skip camera animations
  -noPauseOnClick do not pause animation on mouse click
  -lockLoop       disable any interactions
  -elapsedTime    prints elapsed time in seconds"

Animation definition:
  vanim Name/sub/name [-clear] [-delete]
        [-start TimeSec] [-duration TimeSec] [-end TimeSec]

Animation name defined in path-style (anim/name or anim.name)
specifies nested animations.
There is no syntax to explicitly add new animation,
and all non-existing animations within the name will be
implicitly created on first use (including parents).

Each animation might define the SINGLE action (see below),
like camera transition, object transformation or custom callback.
Child animations can be used for defining concurrent actions.

Camera animation:
  vanim name -view [-eye1 X Y Z] [-eye2 X Y Z]
                   [-at1  X Y Z] [-at2  X Y Z]
                   [-up1  X Y Z] [-up2  X Y Z]
                   [-scale1 Scale] [-scale2 Scale]
  -eyeX   camera Eye positions pair (start and end)
  -atX    camera Center positions pair
  -upX    camera Up directions pair
  -scaleX camera Scale factors pair

Object animation:
  vanim name -object [-loc1 X Y Z] [-loc2 X Y Z]
                     [-rot1 QX QY QZ QW] [-rot2 QX QY QZ QW]
                     [-scale1 Scale] [-scale2 Scale]
 -locX   object Location points pair (translation)
 -rotX   object Orientations pair (quaternions)
 -scaleX object Scale factors pair (quaternions)

  vanim name -object [-axis OX OY OZ DX DY DZ] [-ang1 A] [-ang2 A]
 -axis   rotation axis
 -ang1   start rotation angle in degrees
 -ang2   end   rotation angle in degrees

Custom callback:
  vanim name -invoke "Command Arg1 Arg2 %Pts %LocalPts %Normalized ArgN"

  %Pts        overall animation presentation timestamp
  %LocalPts   local animation timestamp
  %Normalized local animation normalized value in range 0..1

Video recording:
  vanim name -record FileName [Width Height] [-fps FrameRate=24]
        [-format Format] [-vcodec Codec] [-pix_fmt PixelFormat]
        [-crf Value] [-preset Preset]
  -fps     video framerate
  -format  file format, container (matroska, etc.)
  -vcodec  video codec identifier (ffv1, mjpeg, etc.)
  -pix_fmt image pixel format (yuv420p, rgb24, etc.)
  -crf     constant rate factor (specific to codec)
  -preset  codec parameters preset (specific to codec)
)" /* [vanim] */);

  addCmd("vchangeselected", VChangeSelected, /* [vchangeselected] */ R"(
vchangeselected shape : Add shape to selection or remove one from it.
)" /* [vchangeselected] */);

  addCmd("vnbselected", VNbSelected, /* [vnbselected] */ R"(
vnbselected : Returns number of selected objects in the interactive context.
)" /* [vnbselected] */);

  addCmd("vcamera", VCamera, /* [vcamera] */ R"(
vcamera [PrsName] [-ortho] [-projtype]
        [-persp]
        [-fovy   [Angle]] [-distance [Distance]]
        [-stereo] [-leftEye] [-rightEye]
        [-iod [Distance]] [-iodType    [absolute|relative]]
        [-zfocus [Value]] [-zfocusType [absolute|relative]]
        [-fov2d  [Angle]] [-lockZup {0|1}]
        [-rotationMode {active|pick|pickCenter|cameraAt|scene}]
        [-navigationMode {orbit|walk|flight}]
        [-xrPose base|head=base]
Manages camera parameters.
Displays frustum when presentation name PrsName is specified.
Prints current value when option called without argument.

Orthographic camera:
 -ortho      activate orthographic projection.

Perspective camera:
 -persp      activate perspective  projection (mono);
 -fovy       field of view in y axis, in degrees;
 -fov2d      field of view limit for 2d on-screen elements;
 -distance   distance of eye from camera center;
 -lockZup    lock Z up (turntable mode);
 -rotationMode rotation mode (gravity point);
 -navigationMode navigation mode.

Stereoscopic camera:
 -stereo     perspective  projection (stereo);
 -leftEye    perspective  projection (left  eye);
 -rightEye   perspective  projection (right eye);
 -iod        intraocular distance value;
 -iodType    distance type, absolute or relative;
 -zfocus     stereographic focus value;
 -zfocusType focus type, absolute or relative.
)" /* [vcamera] */);

  addCmd("vautozfit", VAutoZFit, /* [vautozfit] */ R"(
vautozfit [on={1|0}] [scale]
Prints or changes parameters of automatic z-fit mode:
 "on" - turns automatic z-fit on or off;
 "scale" - specifies factor to scale computed z range.
)" /* [vautozfit] */);

  addCmd("vzrange", VZRange, /* [vzrange] */ R"(
vzrange [znear] [zfar]
Applies provided znear/zfar to view or prints current values.
)" /* [vzrange] */);

  addCmd("vsetviewsize", VSetViewSize, /* [vsetviewsize] */ R"(
vsetviewsize size
)" /* [vsetviewsize] */);

  addCmd("vmoveview", VMoveView, /* [vmoveview] */ R"(
vmoveview Dx Dy Dz [Start = 1|0]
)" /* [vmoveview] */);

  addCmd("vtranslateview", VTranslateView, /* [vtranslateview] */ R"(
vtranslateview Dx Dy Dz [Start = 1|0)]
)" /* [vtranslateview] */);

  addCmd("vturnview", VTurnView, /* [vturnview] */ R"(
vturnview Ax Ay Az [Start = 1|0]
)" /* [vturnview] */);

  addCmd("vtextureenv", VTextureEnv, /* [vtextureenv] */ R"(
vtextureenv {on|off} {image_file}
            [{clamp|repeat} {decal|modulate} {nearest|bilinear|trilinear} ss st ts tt rot]
Enables or disables environment mapping in the 3D view, loading the texture from the given standard
or user-defined file and optionally applying texture mapping parameters.
 ss, st - scale factors for s and t texture coordinates;
 ts, tt - translation for s and t texture coordinates;
 rot    - texture rotation angle in degrees.
)" /* [vtextureenv] */);

  addCmd("vhlr", VHLR, /* [vhlr] */ R"(
vhlr {on|off} [-showHidden={1|0}] [-algoType={algo|polyAlgo}] [-noupdate]
Hidden Line Removal algorithm.
 -showHidden if set ON, hidden lines are drawn as dotted ones;
 -algoType   type of HLR algorithm:
            'algo' - exact HLR algorithm is applied;
            'polyAlgo' - polygonal HLR algorithm is applied.
)" /* [vhlr] */);

  addCmd("vhlrtype", VHLRType, /* [vhlrtype] */ R"(
vhlrtype {algo|polyAlgo} [shape_1 ... shape_n] [-noupdate]
Changes the type of HLR algorithm using for shapes:
 'algo' - exact HLR algorithm is applied;
 'polyAlgo' - polygonal HLR algorithm is applied.
If shapes are not given - option is applied to all shapes in the view.
)" /* [vhlrtype] */);

  addCmd("vclipplane", VClipPlane, /* [vclipplane] */ R"(
vclipplane planeName [{0|1}]
    [-equation1 A B C D]
    [-equation2 A B C D]
    [-boxInterior MinX MinY MinZ MaxX MaxY MaxZ]
    [-set|-unset|-setOverrideGlobal [objects|views]]
    [-maxPlanes]
    [-capping {0|1}]
      [-color R G B] [-transparency Value] [-hatch {on|off|ID}]
      [-texName Texture] [-texScale SX SY] [-texOrigin TX TY]
        [-texRotate Angle]
      [-useObjMaterial {0|1}] [-useObjTexture {0|1}]
        [-useObjShader {0|1}]

Clipping planes management:
 -maxPlanes   print plane limit for view;
 -delete      delete plane with given name;
 {off|on|0|1} turn clipping on/off;
 -set|-unset  set/unset plane for Object or View list;
              applied to active View when list is omitted;
 -equation A B C D change plane equation;
 -clone SourcePlane NewPlane clone the plane definition.

Capping options:
 -capping {off|on|0|1} turn capping on/off;
 -color R G B          set capping color;
 -transparency Value   set capping transparency 0..1;
 -texName Texture      set capping texture;
 -texScale SX SY       set capping tex scale;
 -texOrigin TX TY      set capping tex origin;
 -texRotate Angle      set capping tex rotation;
 -hatch {on|off|ID}    set capping hatching mask;
 -useObjMaterial {off|on|0|1} use material of clipped object;
 -useObjTexture  {off|on|0|1} use texture of clipped object;
 -useObjShader   {off|on|0|1} use shader program of object.
)" /* [vclipplane] */);

  addCmd("vdefaults", VDefaults, /* [vdefaults] */ R"(
vdefaults [-absDefl value] [-devCoeff value] [-angDefl value]
          [-autoTriang {off/on | 0/1}]
)" /* [vdefaults] */);

  addCmd("vlight", VLight, /* [vlight] */ R"(
vlight [lightName] [-noupdate]
       [-clear|-defaults] [-layer Id] [-local|-global] [-disable|-enable]
       [-type {ambient|directional|spotlight|positional}] [-name value]
       [-position X Y Z] [-direction X Y Z] [-color colorName] [-intensity value]
       [-headlight 0|1] [-castShadows 0|1]
       [-range value] [-constAttenuation value] [-linearAttenuation value]
       [-spotExponent value] [-spotAngle angleDeg]
       [-smoothAngle value] [-smoothRadius value]
       [-display] [-showName 1|0] [-showRange 1|0] [-prsZoomable 1|0] [-prsSize Value]
       [-arcSize Value]

Command manages light sources. Without arguments shows list of lights.
Arguments affecting the list of defined/active lights:
 -clear       remove all light sources;
 -defaults    defines two standard light sources;
 -reset       resets light source parameters to default values;
 -type        sets type of light source;
 -name        sets new name to light source;
 -global      assigns light source to all views (default state);
 -local       assigns light source to active view;
 -zlayer      assigns light source to specified Z-Layer.

Ambient light parameters:
 -color       sets (normalized) light color;
 -intensity   sets intensity of light source, 1.0 by default;
              affects also environment cubemap intensity.

Point light parameters:
 -color       sets (normalized) light color;
 -intensity   sets PBR intensity;
 -range       sets clamping distance;
 -constAtten  (obsolete) sets constant attenuation factor;
 -linearAtten (obsolete) sets linear   attenuation factor;
 -smoothRadius sets PBR smoothing radius.

Directional light parameters:
 -color       sets (normalized) light color;
 -intensity   sets PBR intensity;
 -direction   sets direction;
 -headlight   sets headlight flag;
 -castShadows enables/disables shadow casting;
 -smoothAngle sets PBR smoothing angle (in degrees) within 0..90 range.

Spot light parameters:
 -color       sets (normalized) light color;
 -intensity   sets PBR intensity;
 -range       sets clamping distance;
 -position    sets position;
 -direction   sets direction;
 -spotAngle   sets spotlight angle;
 -spotExp     sets spotlight exponenta;
 -headlight   sets headlight flag;
 -castShadows enables/disables shadow casting;
 -constAtten  (obsolete) sets constant attenuation factor;
 -linearAtten (obsolete) sets linear   attenuation factor.

Light presentation parameters:
 -display     adds light source presentation;
 -showName    shows/hides the name of light source; 1 by default;
 -showRange   shows/hides the range of spot/positional light source; 1 by default;
 -prsZoomable makes light presentation zoomable/non-zoomable;
 -prsDraggable makes light presentation draggable/non-draggable;
 -prsSize     sets light presentation size;
 -arcSize     sets arc presentation size(in pixels)
              for rotation directional light source; 25 by default.

Examples:
 vlight redlight -type POSITIONAL -headlight 1 -pos 0 1 1 -color RED
 vlight redlight -delete
)" /* [vlight] */);

  addCmd("vpbrenv", VPBREnvironment, /* [vpbrenv] */ R"(
vpbrenv -clear|-generate
Clears or generates PBR environment map of active view.
 -clear clears PBR environment (fills by white color);
 -generate generates PBR environment from current background cubemap.
)" /* [vpbrenv] */);

  addCmd("vraytrace", VRenderParams, /* [vraytrace] */ R"(
vraytrace [0|1] : Turns on/off ray-tracing renderer.
 'vraytrace 0' alias for 'vrenderparams -raster'.
 'vraytrace 1' alias for 'vrenderparams -rayTrace'.
)" /* [vraytrace] */);

  addCmd("vrenderparams", VRenderParams, /* [vrenderparams] */ R"(
Manages rendering parameters, affecting visual appearance, quality and performance.
Should be applied taking into account GPU hardware capabilities and performance.

Common parameters:
vrenderparams [-raster] [-shadingModel {unlit|facet|gouraud|phong|pbr|pbr_facet}=gouraud]
              [-msaa 0..8=0] [-rendScale scale=1]
              [-resolution value=72] [-fontHinting {off|normal|light}=off]
              [-fontAutoHinting {auto|force|disallow}=auto]
              [-oit {off|weight|peel}] [-oit weighted [depthFactor=0.0]] [-oit peeling [nbLayers=4]]
              [-shadows {on|off}=on] [-shadowMapResolution value=1024] [-shadowMapBias value=0.005]
              [-depthPrePass {on|off}=off] [-alphaToCoverage {on|off}=on]
              [-frustumCulling {on|off|noupdate}=on] [-lineFeather width=1.0]
              [-sync {default|views}] [-reset]
 -raster          Disables GPU ray-tracing.
 -shadingModel    Controls shading model.
 -msaa            Specifies number of samples for MSAA.
 -rendScale       Rendering resolution scale factor (supersampling, alternative to MSAA).
 -resolution      Sets new pixels density (PPI) used as text scaling factor.
 -fontHinting     Enables/disables font hinting for better readability on low-resolution screens.
 -fontAutoHinting Manages font autohinting.
 -lineFeather     Sets line feather factor while displaying mesh edges.
 -alphaToCoverage Enables/disables alpha to coverage (needs MSAA).
 -oit             Enables/disables order-independent transparency (OIT) rendering;
      off         unordered transparency (but opaque objects implicitly draw first);
      weighted    weight OIT is managed by depth weight factor 0.0..1.0;
      peeling     depth peeling OIT is managed by number of peeling layers.
  -shadows         Enables/disables shadows rendering.
  -shadowMapResolution Shadow texture map resolution.
  -shadowMapBias   Shadow map bias.
  -depthPrePass    Enables/disables depth pre-pass.
  -frustumCulling  Enables/disables objects frustum clipping or
                   sets state to check structures culled previously.
  -sync            Sets active View parameters as Viewer defaults / to other Views.
  -reset           Resets active View parameters to Viewer defaults.

Diagnostic output (on-screen overlay):
vrenderparams [-perfCounters none|fps|cpu|layers|structures|groups|arrays|triangles|points
                                 |gpuMem|frameTime|basic|extended|full|nofps|skipImmediate]
              [-perfUpdateInterval nbSeconds=1] [-perfChart nbFrames=1] [-perfChartMax seconds=0.1]
 -perfCounters       Show/hide performance counters (flags can be combined).
 -perfUpdateInterval Performance counters update interval.
 -perfChart          Show frame timers chart limited by specified number of frames.
 -perfChartMax       Maximum time in seconds with the chart.

Ray-Tracing options:
vrenderparams [-rayTrace] [-rayDepth {0..10}=3] [-reflections {on|off}=off]
              [-fsaa {on|off}=off] [-gleam {on|off}=off] [-env {on|off}=off]
              [-gi {on|off}=off] [-brng {on|off}=off]
              [-iss {on|off}=off] [-tileSize {1..4096}=32] [-nbTiles {64..1024}=256]
              [-ignoreNormalMap {on|off}=off] [-twoSide {on|off}=off]
              [-maxRad {value>0}=30.0]
              [-aperture {value>=0}=0.0] [-focal {value>=0.0}=1.0]
              [-exposure value=0.0] [-whitePoint value=1.0] [-toneMapping {disabled|filmic}=disabled]
 -rayTrace     Enables  GPU ray-tracing.
 -rayDepth     Defines maximum ray-tracing depth.
 -reflections  Enables/disables specular reflections.
 -fsaa         Enables/disables adaptive anti-aliasing.
 -gleam        Enables/disables transparency shadow effects.
 -gi           Enables/disables global illumination effects (Path-Tracing).
 -env          Enables/disables environment map background.
 -ignoreNormalMap Enables/disables normal map ignoring during path tracing.
 -twoSide      Enables/disables two-sided BSDF models (PT mode).
 -iss          Enables/disables adaptive screen sampling (PT mode).
 -maxRad       Value used for clamping radiance estimation (PT mode).
 -tileSize     Specifies   size of screen tiles in ISS mode (32 by default).
 -nbTiles      Specifies number of screen tiles per Redraw in ISS mode (256 by default).
 -aperture     Aperture size  of perspective camera for depth-of-field effect (0 disables DOF).
 -focal        Focal distance of perspective camera for depth-of-field effect.
 -exposure     Exposure value for tone mapping (0.0 value disables the effect).
 -whitePoint   White point value for filmic tone mapping.
 -toneMapping  Tone mapping mode (disabled, filmic).

PBR environment baking parameters (advanced/debug):
vrenderparams [-pbrEnvPow2size {power>0}=9] [-pbrEnvSMLN {levels>1}=6] [-pbrEnvBP {0..1}=0.99]
              [-pbrEnvBDSN {samples>0}=1024] [-pbrEnvBSSN {samples>0}=256]
 -pbrEnvPow2size Controls size of IBL maps (real size can be calculates as 2^pbrenvpow2size).
 -pbrEnvSMLN     Controls number of mipmap levels used in specular IBL map.
 -pbrEnvBDSN     Controls number of samples in Monte-Carlo integration during
                 diffuse IBL map's sherical harmonics calculation.
 -pbrEnvBSSN     Controls maximum number of samples per mipmap level
                 in Monte-Carlo integration during specular IBL maps generation.
 -pbrEnvBP       Controls strength of samples number reducing
                 during specular IBL maps generation (1 disables reducing).

Debug options:
vrenderparams [-issd {on|off}=off] [-rebuildGlsl on|off]
 -issd         Shows screen sampling distribution in ISS mode.
 -rebuildGlsl  Rebuild Ray-Tracing GLSL programs (for debugging).
 -brng         Enables/disables blocked RNG (fast coherent PT).
)" /* [vrenderparams] */);

  addCmd("vstatprofiler", VStatProfiler, /* [vstatprofiler] */ R"(
vstatprofiler [fps|cpu|allLayers|layers|allstructures|structures|groups
                |allArrays|fillArrays|lineArrays|pointArrays|textArrays
                |triangles|points|geomMem|textureMem|frameMem
                |elapsedFrame|cpuFrameAverage|cpuPickingAverage|cpuCullingAverage|cpuDynAverage
                |cpuFrameMax|cpuPickingMax|cpuCullingMax|cpuDynMax]
              [-noredraw]
Prints rendering statistics for specified counters or for all when unspecified.
Set '-noredraw' flag to avoid additional redraw call and use already collected values.
)" /* [vstatprofiler] */);

  addCmd("vplace", VPlace, /* [vplace] */ R"(
vplace dx dy : Places the point (in pixels) at the center of the window
)" /* [vplace] */);

  addCmd("vxrotate", VXRotate, /* [vxrotate] */ R"(
vxrotate
)" /* [vxrotate] */);

  addCmd("vmanipulator", VManipulator, /* [vmanipulator] */ R"(
vmanipulator Name [-attach AISObject | -detach | ...]
Tool to create and manage AIS manipulators.
Options:
 '-attach AISObject'                 attach manipulator to AISObject
 '-adjustPosition {0|center|location|shapeLocation}' adjust position when attaching
 '-adjustSize     {0|1}'             adjust size when attaching
 '-enableModes    {0|1}'             enable modes when attaching
 '-view  {active | [name of view]}'  display manipulator only in defined view,
                                     by default it is displayed in all views of the current viewer
 '-detach'                           detach manipulator
 '-startTransform mouse_x mouse_y' - invoke start of transformation
 '-transform      mouse_x mouse_y' - invoke transformation
 '-stopTransform  [abort]'         - invoke stop of transformation
 '-move x y z'                     - move attached object
 '-rotate x y z dx dy dz angle'    - rotate attached object
 '-scale factor'                   - scale attached object
 '-autoActivate      {0|1}'        - set activation on detection
 '-followTranslation {0|1}'        - set following translation transform
 '-followRotation    {0|1}'        - set following rotation transform
 '-followDragging    {0|1}'        - set following dragging transform
 '-gap value'                      - set gap between sub-parts
 '-part axis mode    {0|1}'        - set visual part
 '-parts axis mode   {0|1}'        - set visual part
 '-pos x y z [nx ny nz [xx xy xz]' - set position of manipulator
 '-size value'                     - set size of manipulator
 '-zoomable {0|1}'                 - set zoom persistence
)" /* [vmanipulator] */);

  addCmd("vselprops", VSelectionProperties, /* [vselprops] */ R"(
vselprops [dynHighlight|localDynHighlight|selHighlight|localSelHighlight] [options]
Customizes selection and dynamic highlight parameters for the whole interactive context:
 -autoActivate {0|1}     disables|enables default computation
                         and activation of global selection mode
 -autoHighlight {0|1}    disables|enables automatic highlighting in 3D Viewer
 -highlightSelected {0|1} disables|enables highlighting of detected object in selected state
 -pickStrategy {first|topmost} : defines picking strategy
               'first'   to pick first acceptable (default)
               'topmost' to pick only topmost (and nothing, if topmost is rejected by filters)
 -pixTol    value        sets up pixel tolerance
 -depthTol {uniform|uniformpx} value : sets tolerance for sorting results by depth
 -depthTol {sensfactor}  use sensitive factor for sorting results by depth
 -preferClosest {0|1}    sets if depth should take precedence over priority while sorting results
 -dispMode  dispMode     sets display mode for highlighting
 -layer     ZLayer       sets ZLayer for highlighting
 -color     {name|r g b} sets highlight color
 -transp    value        sets transparency coefficient for highlight
 -material  material     sets highlight material
 -print                  prints current state of all mentioned parameters
)" /* [vselprops] */);

  addCmd("vhighlightselected", VSelectionProperties, /* [vhighlightselected] */ R"(
vhighlightselected [0|1] : alias for vselprops -highlightSelected.
)" /* [vhighlightselected] */);

  addCmd("vseldump", VDumpSelectionImage, /* [vseldump] */ R"(
vseldump file -type {depth|unnormDepth|object|owner|selMode|entity|entityType|surfNormal}=depth
         -pickedIndex Index=1
         [-xrPose base|head=base]
Generate an image based on detection results:
  depth       normalized depth values
  unnormDepth unnormalized depth values
  object      color of detected object
  owner       color of detected owner
  selMode     color of selection mode
  entity      color of detected entity
  entityType  color of detected entity type
  surfNormal  normal direction values
)" /* [vseldump] */);

  addCmd("vviewcube", VViewCube, /* [vviewcube] */ R"(
vviewcube name
Displays interactive view manipulation object. Options:
 -reset                   reset geometric and visual attributes
 -size Size               adapted size of View Cube
 -boxSize Size            box size
 -axes  {0|1}             show/hide axes (trihedron)
 -edges {0|1}             show/hide edges of View Cube
 -vertices {0|1}          show/hide vertices of View Cube
 -Yup {0|1} -Zup {0|1}    set Y-up or Z-up view orientation
 -color Color             color of View Cube
 -boxColor Color          box color
 -boxSideColor Color      box sides color
 -boxEdgeColor Color      box edges color
 -boxCornerColor Color    box corner color
 -textColor Color         color of side text of view cube
 -innerColor Color        inner box color
 -transparency Value      transparency of object within [0, 1] range
 -boxTransparency Value   transparency of box    within [0, 1] range
 -xAxisTextColor Color    color of X axis label
 -yAxisTextColor Color    color of Y axis label
 -zAxisTextColor Color    color of Z axis label
 -font Name               font name
 -fontHeight Value        font height
 -boxFacetExtension Value box facet extension
 -boxEdgeGap Value        gap between box edges and box sides
 -boxEdgeMinSize Value    minimal box edge size
 -boxCornerMinSize Value  minimal box corner size
 -axesPadding Value       padding between box and arrows
 -roundRadius Value       relative radius of corners of sides within [0.0, 0.5] range
 -axesRadius Value        radius of axes of the trihedron
 -axesConeRadius Value    radius of the cone (arrow) of the trihedron
 -axesSphereRadius Value  radius of the sphere (central point) of trihedron
 -fixedAnimation {0|1}    uninterruptible animation loop
 -duration Seconds        animation duration in seconds
 -orthoPers               force orthographic projection persistence.
)" /* [vviewcube] */);

  addCmd("vcolorconvert", VColorConvert, /* [vcolorconvert] */ R"(
vcolorconvert [-from {sRGB|HLS|Lab|Lch|RGB}]=RGB [-to {sRGB|HLS|Lab|Lch|RGB|hex|name}]=RGB C1 C2 C2
To convert color from specified color space to linear RGB:
  vcolorconvert -from {sRGB|HLS|Lab|Lch|RGB} C1 C2 C2
To convert linear RGB color to specified color space:
  vcolorconvert -to {sRGB|HLS|Lab|Lch|RGB|hex|name} R G B
)" /* [vcolorconvert] */);

  addCmd("vcolordiff", VColorDiff, /* [vcolordiff] */ R"(
vcolordiff R1 G1 B1 R2 G2 B2 : returns CIEDE2000 color difference between two RGB colors.
)" /* [vcolordiff] */);

  addCmd("vselbvhbuild", VSelBvhBuild, /* [vselbvhbuild] */ R"(
vselbvhbuild [{0|1}] [-nbThreads value] [-wait]
Turns on/off prebuilding of BVH within background thread(s).
 -nbThreads   number of threads, 1 by default; if < 1 then used (NbLogicalProcessors - 1);
 -wait        waits for building all of BVH.
)" /* [vselbvhbuild] */);

  addCmd("vchangemousegesture", VChangeMouseGesture, /* [vchangemousegesture] */ R"(
vchangemousegesture -button  {none|left|middle|right}=left
                    -gesture {none|selectRectangle|selectLasso|zoom|zoomWindow|pan|rotateOrbit|rotateView|drag}=rotateOrbit
Changes the gesture for the mouse button.
 -button  the mouse button;
 -gesture the new gesture for the button.
)" /* [vchangemousegesture] */);
}
