// Created on: 2011-10-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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

#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Flipper.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>
#include <OpenGl_Group.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_StencilTest.hxx>
#include <OpenGl_Text.hxx>
#include <OpenGl_Trihedron.hxx>
#include <OpenGl_Workspace.hxx>

#include <Aspect_GraphicDeviceDefinitionError.hxx>
#include <Aspect_IdentDefinitionError.hxx>
#include <Message_Messenger.hxx>
#include <OSD_Environment.hxx>
#include <Standard_NotImplemented.hxx>

#if defined(_WIN32)
  #include <WNT_Window.hxx>
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  #include <Cocoa_Window.hxx>
#else
  #include <Xw_Window.hxx>
#endif

#if !defined(_WIN32) && !defined(__ANDROID__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  #include <X11/Xlib.h> // XOpenDisplay()
#endif

#if defined(HAVE_EGL) || defined(__ANDROID__)
  #include <EGL/egl.h>
#endif

namespace
{
  static const Handle(OpenGl_Context) TheNullGlCtx;
}

// =======================================================================
// function : OpenGl_GraphicDriver
// purpose  :
// =======================================================================
OpenGl_GraphicDriver::OpenGl_GraphicDriver (const Handle(Aspect_DisplayConnection)& theDisp,
                                            const Standard_Boolean                  theToInitialize)
: Graphic3d_GraphicDriver (theDisp),
  myIsOwnContext (Standard_False),
#if defined(HAVE_EGL) || defined(__ANDROID__)
  myEglDisplay ((Aspect_Display )EGL_NO_DISPLAY),
  myEglContext ((Aspect_RenderingContext )EGL_NO_CONTEXT),
  myEglConfig  (NULL),
#endif
  myCaps           (new OpenGl_Caps()),
  myMapOfView      (1, NCollection_BaseAllocator::CommonBaseAllocator()),
  myMapOfStructure (1, NCollection_BaseAllocator::CommonBaseAllocator())
{
#if !defined(_WIN32) && !defined(__ANDROID__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  if (myDisplayConnection.IsNull())
  {
    //Aspect_GraphicDeviceDefinitionError::Raise ("OpenGl_GraphicDriver: cannot connect to X server!");
    return;
  }

  Display* aDisplay = myDisplayConnection->GetDisplay();
  Bool toSync = ::getenv ("CSF_GraphicSync") != NULL
             || ::getenv ("CALL_SYNCHRO_X")  != NULL;
  XSynchronize (aDisplay, toSync);

#if !defined(HAVE_EGL)
  // does the server know about OpenGL & GLX?
  int aDummy;
  if (!XQueryExtension (aDisplay, "GLX", &aDummy, &aDummy, &aDummy))
  {
    ::Message::DefaultMessenger()->Send ("OpenGl_GraphicDriver, this system doesn't appear to support OpenGL!", Message_Warning);
  }
#endif
#endif
  if (theToInitialize
  && !InitContext())
  {
    Aspect_GraphicDeviceDefinitionError::Raise ("OpenGl_GraphicDriver: default context can not be initialized!");
  }

  // default layers are always presented in display layer sequence it can not be removed
  Graphic3d_ZLayerSettings anUnderlaySettings;
  anUnderlaySettings.Flags = 0;
  anUnderlaySettings.IsImmediate = false;
  myLayerIds.Add             (Graphic3d_ZLayerId_BotOSD);
  myLayerSeq.Append          (Graphic3d_ZLayerId_BotOSD);
  myMapOfZLayerSettings.Bind (Graphic3d_ZLayerId_BotOSD, anUnderlaySettings);

  Graphic3d_ZLayerSettings aDefSettings;
  aDefSettings.Flags = Graphic3d_ZLayerDepthTest
                     | Graphic3d_ZLayerDepthWrite;
  aDefSettings.IsImmediate = false;
  myLayerIds.Add             (Graphic3d_ZLayerId_Default);
  myLayerSeq.Append          (Graphic3d_ZLayerId_Default);
  myMapOfZLayerSettings.Bind (Graphic3d_ZLayerId_Default, aDefSettings);

  Graphic3d_ZLayerSettings aTopSettings;
  aTopSettings.Flags = Graphic3d_ZLayerDepthTest
                     | Graphic3d_ZLayerDepthWrite;
  aTopSettings.IsImmediate = true;
  myLayerIds.Add             (Graphic3d_ZLayerId_Top);
  myLayerSeq.Append          (Graphic3d_ZLayerId_Top);
  myMapOfZLayerSettings.Bind (Graphic3d_ZLayerId_Top, aTopSettings);

  Graphic3d_ZLayerSettings aTopmostSettings;
  aTopmostSettings.Flags = Graphic3d_ZLayerDepthTest
                         | Graphic3d_ZLayerDepthWrite
                         | Graphic3d_ZLayerDepthClear;
  aTopmostSettings.IsImmediate = true;
  myLayerIds.Add             (Graphic3d_ZLayerId_Topmost);
  myLayerSeq.Append          (Graphic3d_ZLayerId_Topmost);
  myMapOfZLayerSettings.Bind (Graphic3d_ZLayerId_Topmost, aTopmostSettings);

  Graphic3d_ZLayerSettings anOsdSettings;
  anOsdSettings.Flags = 0;
  anOsdSettings.IsImmediate = true;
  myLayerIds.Add             (Graphic3d_ZLayerId_TopOSD);
  myLayerSeq.Append          (Graphic3d_ZLayerId_TopOSD);
  myMapOfZLayerSettings.Bind (Graphic3d_ZLayerId_TopOSD, anOsdSettings);
}

// =======================================================================
// function : ~OpenGl_GraphicDriver
// purpose  :
// =======================================================================
OpenGl_GraphicDriver::~OpenGl_GraphicDriver()
{
  ReleaseContext();
}

// =======================================================================
// function : ReleaseContext
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::ReleaseContext()
{
  Handle(OpenGl_Context) aCtxShared;
  for (NCollection_Map<Handle(OpenGl_View)>::Iterator aViewIter (myMapOfView);
       aViewIter.More(); aViewIter.Next())
  {
    const Handle(OpenGl_View)& aView = aViewIter.ChangeValue();
    const Handle(OpenGl_Window)& aWindow = aView->GlWindow();
    if (aWindow.IsNull())
    {
      continue;
    }

    const Handle(OpenGl_Context)& aCtx = aWindow->GetGlContext();
    if (aCtx->MakeCurrent()
     && aCtxShared.IsNull())
    {
      aCtxShared = aCtx;
    }
  }

  if (!aCtxShared.IsNull())
  {
    aCtxShared->MakeCurrent();
  }
  for (NCollection_Map<Handle(OpenGl_View)>::Iterator aViewIter (myMapOfView);
       aViewIter.More(); aViewIter.Next())
  {
    const Handle(OpenGl_View)& aView = aViewIter.ChangeValue();
    aView->ReleaseGlResources (aCtxShared);
  }

  for (NCollection_DataMap<Standard_Integer, OpenGl_Structure*>::Iterator aStructIt (myMapOfStructure);
       aStructIt.More (); aStructIt.Next())
  {
    OpenGl_Structure* aStruct = aStructIt.ChangeValue();
    aStruct->ReleaseGlResources (aCtxShared);
  }
  myDeviceLostFlag = myDeviceLostFlag || !myMapOfStructure.IsEmpty();

  for (NCollection_Map<Handle(OpenGl_View)>::Iterator aViewIter (myMapOfView);
       aViewIter.More(); aViewIter.Next())
  {
    const Handle(OpenGl_View)& aView = aViewIter.ChangeValue();
    const Handle(OpenGl_Window)& aWindow = aView->GlWindow();
    if (aWindow.IsNull())
    {
      continue;
    }

    aWindow->GetGlContext()->forcedRelease();
  }

#if defined(HAVE_EGL) || defined(__ANDROID__)
  if (myIsOwnContext)
  {
    if (myEglContext != (Aspect_RenderingContext )EGL_NO_CONTEXT)
    {
      if (eglMakeCurrent ((EGLDisplay )myEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) != EGL_TRUE)
      {
        ::Message::DefaultMessenger()->Send ("OpenGl_GraphicDriver, FAILED to release OpenGL context!", Message_Warning);
      }
      eglDestroyContext ((EGLDisplay )myEglDisplay, (EGLContext )myEglContext);
    }

    if (myEglDisplay != (Aspect_Display )EGL_NO_DISPLAY)
    {
      if (eglTerminate ((EGLDisplay )myEglDisplay) != EGL_TRUE)
      {
        ::Message::DefaultMessenger()->Send ("OpenGl_GraphicDriver, EGL, eglTerminate FAILED!", Message_Warning);
      }
    }
  }

  myEglDisplay = (Aspect_Display )EGL_NO_DISPLAY;
  myEglContext = (Aspect_RenderingContext )EGL_NO_CONTEXT;
  myEglConfig  = NULL;
#endif
  myIsOwnContext = Standard_False;
}

// =======================================================================
// function : InitContext
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_GraphicDriver::InitContext()
{
  ReleaseContext();
#if defined(HAVE_EGL) || defined(__ANDROID__)

#if !defined(_WIN32) && !defined(__ANDROID__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  if (myDisplayConnection.IsNull())
  {
    return Standard_False;
  }
  Display* aDisplay = myDisplayConnection->GetDisplay();
  myEglDisplay = (Aspect_Display )eglGetDisplay (aDisplay);
#else
  myEglDisplay = (Aspect_Display )eglGetDisplay (EGL_DEFAULT_DISPLAY);
#endif
  if ((EGLDisplay )myEglDisplay == EGL_NO_DISPLAY)
  {
    ::Message::DefaultMessenger()->Send ("Error: no EGL display!", Message_Fail);
    return Standard_False;
  }

  EGLint aVerMajor = 0; EGLint aVerMinor = 0;
  if (eglInitialize ((EGLDisplay )myEglDisplay, &aVerMajor, &aVerMinor) != EGL_TRUE)
  {
    ::Message::DefaultMessenger()->Send ("Error: EGL display is unavailable!", Message_Fail);
    return Standard_False;
  }

  EGLint aConfigAttribs[] =
  {
    EGL_RED_SIZE,     8,
    EGL_GREEN_SIZE,   8,
    EGL_BLUE_SIZE,    8,
    EGL_ALPHA_SIZE,   0,
    EGL_DEPTH_SIZE,   24,
    EGL_STENCIL_SIZE, 8,
  #if defined(GL_ES_VERSION_2_0)
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
  #else
    EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
  #endif
    EGL_NONE
  };

  EGLint aNbConfigs = 0;
  if (eglChooseConfig ((EGLDisplay )myEglDisplay, aConfigAttribs, &myEglConfig, 1, &aNbConfigs) != EGL_TRUE
   || myEglConfig == NULL)
  {
    eglGetError();
    aConfigAttribs[4 * 2 + 1] = 16; // try config with smaller depth buffer
    if (eglChooseConfig ((EGLDisplay )myEglDisplay, aConfigAttribs, &myEglConfig, 1, &aNbConfigs) != EGL_TRUE
     || myEglConfig == NULL)
    {
      ::Message::DefaultMessenger()->Send ("Error: EGL does not provide compatible configurations!", Message_Fail);
      return Standard_False;
    }
  }

#if defined(GL_ES_VERSION_2_0)
  EGLint anEglCtxAttribs[] =
  {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };
  if (eglBindAPI (EGL_OPENGL_ES_API) != EGL_TRUE)
  {
    ::Message::DefaultMessenger()->Send ("Error: EGL does not provide OpenGL ES client!", Message_Fail);
    return Standard_False;
  }
#else
  EGLint* anEglCtxAttribs = NULL;
  if (eglBindAPI (EGL_OPENGL_API) != EGL_TRUE)
  {
    ::Message::DefaultMessenger()->Send ("Error: EGL does not provide OpenGL client!", Message_Fail);
    return Standard_False;
  }
#endif

  myEglContext = (Aspect_RenderingContext )eglCreateContext ((EGLDisplay )myEglDisplay, myEglConfig, EGL_NO_CONTEXT, anEglCtxAttribs);
  if ((EGLContext )myEglContext == EGL_NO_CONTEXT)
  {
    ::Message::DefaultMessenger()->Send ("Error: EGL is unable to create OpenGL context!", Message_Fail);
    return Standard_False;
  }
  if (eglMakeCurrent ((EGLDisplay )myEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, (EGLContext )myEglContext) != EGL_TRUE)
  {
    ::Message::DefaultMessenger()->Send ("Error: EGL is unable bind OpenGL context!", Message_Fail);
    return Standard_False;
  }
#endif
  myIsOwnContext = Standard_True;
  return Standard_True;
}

#if defined(HAVE_EGL) || defined(__ANDROID__)
// =======================================================================
// function : InitEglContext
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_GraphicDriver::InitEglContext (Aspect_Display          theEglDisplay,
                                                       Aspect_RenderingContext theEglContext,
                                                       void*                   theEglConfig)
{
  ReleaseContext();
#if !defined(_WIN32) && !defined(__ANDROID__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  if (myDisplayConnection.IsNull())
  {
    return Standard_False;
  }
#endif

  if ((EGLDisplay )theEglDisplay == EGL_NO_DISPLAY
   || (EGLContext )theEglContext == EGL_NO_CONTEXT
   || theEglConfig == NULL)
  {
    return Standard_False;
  }
  myEglDisplay = theEglDisplay;
  myEglContext = theEglContext;
  myEglConfig  = theEglConfig;
  return Standard_True;
}
#endif

// =======================================================================
// function : InquireLightLimit
// purpose  :
// =======================================================================
Standard_Integer OpenGl_GraphicDriver::InquireLightLimit()
{
  return OpenGLMaxLights;
}

// =======================================================================
// function : InquireViewLimit
// purpose  :
// =======================================================================
Standard_Integer OpenGl_GraphicDriver::InquireViewLimit()
{
  return 10000;
}

// =======================================================================
// function : InquirePlaneLimit
// purpose  :
// =======================================================================
Standard_Integer OpenGl_GraphicDriver::InquirePlaneLimit()
{
  // NOTE the 2 first planes are reserved for ZClipping
  const Handle(OpenGl_Context)& aCtx = GetSharedContext();
  return aCtx.IsNull() ? 0 : Max (aCtx->MaxClipPlanes() - 2, 0);
}

// =======================================================================
// function : DefaultTextHeight
// purpose  :
// =======================================================================
Standard_ShortReal OpenGl_GraphicDriver::DefaultTextHeight() const
{
  return 16.;
}

// =======================================================================
// function : EnableVBO
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::EnableVBO (const Standard_Boolean theToTurnOn)
{
  myCaps->vboDisable = !theToTurnOn;
}

// =======================================================================
// function : GetSharedContext
// purpose  :
// =======================================================================
const Handle(OpenGl_Context)& OpenGl_GraphicDriver::GetSharedContext() const
{
  if (myMapOfView.IsEmpty())
  {
    return TheNullGlCtx;
  }

  NCollection_Map<Handle(OpenGl_View)>::Iterator anIter (myMapOfView);
  for (; anIter.More(); anIter.Next())
  {
    Handle(OpenGl_Window) aWindow = anIter.Value()->GlWindow();
    if (aWindow.IsNull())
    {
      continue;
    }

    return aWindow->GetGlContext();
  }

  return TheNullGlCtx;
}

// =======================================================================
// function : MemoryInfo
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_GraphicDriver::MemoryInfo (Standard_Size&           theFreeBytes,
                                                   TCollection_AsciiString& theInfo) const
{
  // this is extra work (for OpenGl_Context initialization)...
  OpenGl_Context aGlCtx;
  if (!aGlCtx.Init())
  {
    return Standard_False;
  }
  theFreeBytes = aGlCtx.AvailableMemory();
  theInfo      = aGlCtx.MemoryInfo();
  return !theInfo.IsEmpty();
}

// =======================================================================
// function : SetBuffersNoSwap
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::SetBuffersNoSwap (const Standard_Boolean theIsNoSwap)
{
  myCaps->buffersNoSwap = theIsNoSwap;
}

// =======================================================================
// function : TextSize
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::TextSize (const Handle(Graphic3d_CView)& theView,
                                     const Standard_CString         theText,
                                     const Standard_ShortReal       theHeight,
                                     Standard_ShortReal&            theWidth,
                                     Standard_ShortReal&            theAscent,
                                     Standard_ShortReal&            theDescent) const
{
  const Handle(OpenGl_Context)& aCtx = GetSharedContext();
  if (aCtx.IsNull())
  {
    return;
  }

  const Standard_ShortReal aHeight = (theHeight < 2.0f) ? DefaultTextHeight() : theHeight;
  OpenGl_TextParam aTextParam;
  aTextParam.Height = (int )aHeight;
  OpenGl_AspectText aTextAspect;
  CALL_DEF_CONTEXTTEXT aDefaultContextText =
  {
    1, //IsDef
    1, //IsSet
    "Courier", //Font
    0.3F, //Space
    1.F, //Expan
    { 1.F, 1.F, 1.F }, //Color
    (int)Aspect_TOST_NORMAL, //Style
    (int)Aspect_TODT_NORMAL, //DisplayType
    { 1.F, 1.F, 1.F }, //ColorSubTitle
    0, //TextZoomable
    0.F, //TextAngle
    (int)Font_FA_Regular //TextFontAspect
  };
  aTextAspect.SetAspect(aDefaultContextText);
  TCollection_ExtendedString anExtText = theText;
  NCollection_String aText = (Standard_Utf16Char* )anExtText.ToExtString();
  OpenGl_Text::StringSize(aCtx, aText, aTextAspect, aTextParam, theView->RenderingParams().Resolution, theWidth, theAscent, theDescent);
}

//=======================================================================
//function : AddZLayer
//purpose  :
//=======================================================================
void OpenGl_GraphicDriver::AddZLayer (const Graphic3d_ZLayerId theLayerId)
{
  if (theLayerId < 1)
  {
    Standard_ASSERT_RAISE (theLayerId > 0,
                           "OpenGl_GraphicDriver::AddZLayer, "
                           "negative and zero IDs are reserved");
  }

  myLayerIds.Add    (theLayerId);
  myLayerSeq.Append (theLayerId);

  // Default z-layer settings
  myMapOfZLayerSettings.Bind (theLayerId, Graphic3d_ZLayerSettings());

  // Add layer to all views
  NCollection_Map<Handle(OpenGl_View)>::Iterator aViewIt (myMapOfView);
  for (; aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->AddZLayer (theLayerId);
  }
}

//=======================================================================
//function : RemoveZLayer
//purpose  :
//=======================================================================
void OpenGl_GraphicDriver::RemoveZLayer (const Graphic3d_ZLayerId theLayerId)
{
  Standard_ASSERT_RAISE (theLayerId > 0,
                         "OpenGl_GraphicDriver::AddZLayer, "
                         "negative and zero IDs are reserved"
                         "and can not be removed");

  Standard_ASSERT_RAISE (myLayerIds.Contains (theLayerId),
                         "OpenGl_GraphicDriver::RemoveZLayer, "
                         "Layer with theLayerId does not exist");

  // Remove layer from all of the views
  NCollection_Map<Handle(OpenGl_View)>::Iterator aViewIt (myMapOfView);
  for (; aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->RemoveZLayer (theLayerId);
  }

  // Unset Z layer for all of the structures.
  NCollection_DataMap<Standard_Integer, OpenGl_Structure*>::Iterator aStructIt (myMapOfStructure);
  for( ; aStructIt.More (); aStructIt.Next ())
  {
    OpenGl_Structure* aStruct = aStructIt.ChangeValue ();
    if (aStruct->ZLayer() == theLayerId)
      aStruct->SetZLayer (Graphic3d_ZLayerId_Default);
  }

  // Remove index
  for (int aIdx = 1; aIdx <= myLayerSeq.Length (); aIdx++)
  {
    if (myLayerSeq (aIdx) == theLayerId)
    {
      myLayerSeq.Remove (aIdx);
      break;
    }
  }

  myMapOfZLayerSettings.UnBind (theLayerId);
  myLayerIds.Remove  (theLayerId);
}

//=======================================================================
//function : ZLayers
//purpose  :
//=======================================================================
void OpenGl_GraphicDriver::ZLayers (TColStd_SequenceOfInteger& theLayerSeq) const
{
  theLayerSeq.Assign (myLayerSeq);
}

//=======================================================================
//function : SetZLayerSettings
//purpose  :
//=======================================================================
void OpenGl_GraphicDriver::SetZLayerSettings (const Graphic3d_ZLayerId theLayerId,
                                              const Graphic3d_ZLayerSettings& theSettings)
{
  // Change Z layer settings in all managed views
  NCollection_Map<Handle(OpenGl_View)>::Iterator aViewIt (myMapOfView);
  for (; aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->SetZLayerSettings (theLayerId, theSettings);
  }

  if (myMapOfZLayerSettings.IsBound (theLayerId))
  {
    myMapOfZLayerSettings.ChangeFind (theLayerId) = theSettings;
  }
  else
  {
    myMapOfZLayerSettings.Bind (theLayerId, theSettings);
  }
}

//=======================================================================
//function : ZLayerSettings
//purpose  :
//=======================================================================
Graphic3d_ZLayerSettings OpenGl_GraphicDriver::ZLayerSettings (const Graphic3d_ZLayerId theLayerId)
{
  Standard_ASSERT_RAISE (myLayerIds.Contains (theLayerId),
                         "OpenGl_GraphicDriver::ZLayerSettings, "
                         "Layer with theLayerId does not exist");

  return myMapOfZLayerSettings.Find (theLayerId);
}

// =======================================================================
// function : Structure
// purpose  :
// =======================================================================
Handle(Graphic3d_CStructure) OpenGl_GraphicDriver::CreateStructure (const Handle(Graphic3d_StructureManager)& theManager)
{
  Handle(OpenGl_Structure) aStructure = new OpenGl_Structure (theManager);
  myMapOfStructure.Bind (aStructure->Id, aStructure.operator->());
  return aStructure;
}

// =======================================================================
// function : Structure
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::RemoveStructure (Handle(Graphic3d_CStructure)& theCStructure)
{
  OpenGl_Structure* aStructure = NULL;
  if (!myMapOfStructure.Find (theCStructure->Id, aStructure))
  {
    return;
  }

  myMapOfStructure.UnBind (theCStructure->Id);
  aStructure->Release (GetSharedContext());
  theCStructure.Nullify();
}

// =======================================================================
// function : View
// purpose  :
// =======================================================================
Handle(Graphic3d_CView) OpenGl_GraphicDriver::CreateView (const Handle(Graphic3d_StructureManager)& theMgr)
{
  Handle(OpenGl_View) aView = new OpenGl_View (theMgr, this, myCaps, myDeviceLostFlag, &myStateCounter);

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

// =======================================================================
// function : RemoveView
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::RemoveView (const Handle(Graphic3d_CView)& theView)
{
  Handle(OpenGl_Context) aCtx = GetSharedContext();
  Handle(OpenGl_View) aView   = Handle(OpenGl_View)::DownCast (theView);
  if (aView.IsNull())
  {
    return;
  }

  if (!myMapOfView.Remove (aView))
  {
    return;
  }

  Handle(OpenGl_Window) aWindow = aView->GlWindow();
  if (!aWindow.IsNull()
    && aWindow->GetGlContext()->MakeCurrent())
  {
    aCtx = aWindow->GetGlContext();
  }
  else
  {
    // try to hijack another context if any
    const Handle(OpenGl_Context)& anOtherCtx = GetSharedContext();
    if (!anOtherCtx.IsNull()
      && anOtherCtx != aWindow->GetGlContext())
    {
      aCtx = anOtherCtx;
      aCtx->MakeCurrent();
    }
  }

  aView->ReleaseGlResources (aCtx);
  if (myMapOfView.IsEmpty())
  {
    // The last view removed but some objects still present.
    // Release GL resources now without object destruction.
    for (NCollection_DataMap<Standard_Integer, OpenGl_Structure*>::Iterator aStructIt (myMapOfStructure);
         aStructIt.More (); aStructIt.Next())
    {
      OpenGl_Structure* aStruct = aStructIt.ChangeValue();
      aStruct->ReleaseGlResources (aCtx);
    }
    myDeviceLostFlag = !myMapOfStructure.IsEmpty();
  }
}

// =======================================================================
// function : Window
// purpose  :
// =======================================================================
Handle(OpenGl_Window) OpenGl_GraphicDriver::CreateRenderWindow (const Handle(Aspect_Window)&  theWindow,
                                                                const Aspect_RenderingContext theContext)
{
  Handle(OpenGl_Context) aShareCtx = GetSharedContext();
  Handle(OpenGl_Window) aWindow = new OpenGl_Window (this, theWindow, theContext, myCaps, aShareCtx);
  return aWindow;
}

//=======================================================================
//function : ViewExists
//purpose  :
//=======================================================================
Standard_Boolean OpenGl_GraphicDriver::ViewExists (const Handle(Aspect_Window)& AWindow, Handle(Graphic3d_CView)& theView)
{
  Standard_Boolean isExist = Standard_False;

  // Parse the list of views to find
  // a view with the specified window

#if defined(_WIN32)
  const Handle(WNT_Window) THEWindow = Handle(WNT_Window)::DownCast (AWindow);
  Aspect_Handle TheSpecifiedWindowId = THEWindow->HWindow ();
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  const Handle(Cocoa_Window) THEWindow = Handle(Cocoa_Window)::DownCast (AWindow);
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    UIView* TheSpecifiedWindowId = THEWindow->HView();
  #else
    NSView* TheSpecifiedWindowId = THEWindow->HView();
  #endif
#elif defined(__ANDROID__)
  int TheSpecifiedWindowId = -1;
#else
  const Handle(Xw_Window) THEWindow = Handle(Xw_Window)::DownCast (AWindow);
  int TheSpecifiedWindowId = int (THEWindow->XWindow ());
#endif

  NCollection_Map<Handle(OpenGl_View)>::Iterator aViewIt (myMapOfView);
  for(; aViewIt.More(); aViewIt.Next())
  {
    const Handle(OpenGl_View)& aView = aViewIt.Value();
    if (aView->IsDefined() && aView->IsActive())
    {
      const Handle(Aspect_Window) AspectWindow = aView->Window();

#if defined(_WIN32)
      const Handle(WNT_Window) theWindow = Handle(WNT_Window)::DownCast (AspectWindow);
      Aspect_Handle TheWindowIdOfView = theWindow->HWindow ();
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
      const Handle(Cocoa_Window) theWindow = Handle(Cocoa_Window)::DownCast (AspectWindow);
      #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
        UIView* TheWindowIdOfView = theWindow->HView();
      #else
        NSView* TheWindowIdOfView = theWindow->HView();
      #endif
#elif defined(__ANDROID__)
      int TheWindowIdOfView = 0;
#else
      const Handle(Xw_Window) theWindow = Handle(Xw_Window)::DownCast (AspectWindow);
      int TheWindowIdOfView = int (theWindow->XWindow ());
#endif  // WNT
      // Comparaison on window IDs
      if (TheWindowIdOfView == TheSpecifiedWindowId)
      {
        isExist = Standard_True;
        theView = aView;
      }
    }
  }

  return isExist;
}
