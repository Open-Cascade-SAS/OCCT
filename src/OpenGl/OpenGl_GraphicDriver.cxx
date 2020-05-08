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

#if defined(_WIN32)
  #include <windows.h>
#endif

#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Flipper.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>
#include <OpenGl_Group.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_StencilTest.hxx>
#include <OpenGl_Text.hxx>
#include <OpenGl_Workspace.hxx>

#include <Aspect_GraphicDeviceDefinitionError.hxx>
#include <Aspect_IdentDefinitionError.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Message_Messenger.hxx>
#include <OSD_Environment.hxx>
#include <Standard_NotImplemented.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_GraphicDriver,Graphic3d_GraphicDriver)

#if defined(_WIN32)
  #include <WNT_Window.hxx>
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  #include <Cocoa_Window.hxx>
#else
  #include <Xw_Window.hxx>
#endif

#if !defined(_WIN32) && !defined(__ANDROID__) && !defined(__QNX__) && !defined(__EMSCRIPTEN__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  #include <X11/Xlib.h> // XOpenDisplay()
#endif

#if defined(HAVE_EGL) || defined(HAVE_GLES2) || defined(OCCT_UWP) || defined(__ANDROID__) || defined(__QNX__) || defined(__EMSCRIPTEN__)
  #include <EGL/egl.h>
  #ifndef EGL_OPENGL_ES3_BIT
    #define EGL_OPENGL_ES3_BIT 0x00000040
  #endif
#endif

namespace
{
  static const Handle(OpenGl_Context) TheNullGlCtx;

#if defined(HAVE_EGL) || defined(HAVE_GLES2) || defined(OCCT_UWP) || defined(__ANDROID__) || defined(__QNX__) || defined(__EMSCRIPTEN__)
  //! Wrapper over eglChooseConfig() called with preferred defaults.
  static EGLConfig chooseEglSurfConfig (EGLDisplay theDisplay)
  {
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

    EGLConfig aCfg = NULL;
    EGLint aNbConfigs = 0;
    for (Standard_Integer aGlesVer = 3; aGlesVer >= 2; --aGlesVer)
    {
    #if defined(GL_ES_VERSION_2_0)
      aConfigAttribs[6 * 2 + 1] = aGlesVer == 3 ? EGL_OPENGL_ES3_BIT : EGL_OPENGL_ES2_BIT;
    #else
      if (aGlesVer == 2)
      {
        break;
      }
    #endif

      if (eglChooseConfig (theDisplay, aConfigAttribs, &aCfg, 1, &aNbConfigs) == EGL_TRUE
       && aCfg != NULL)
      {
        return aCfg;
      }
      eglGetError();

      aConfigAttribs[4 * 2 + 1] = 16; // try config with smaller depth buffer
      if (eglChooseConfig (theDisplay, aConfigAttribs, &aCfg, 1, &aNbConfigs) == EGL_TRUE
       && aCfg != NULL)
      {
        return aCfg;
      }
      eglGetError();
    }
    return aCfg;
  }
#endif
}

// =======================================================================
// function : OpenGl_GraphicDriver
// purpose  :
// =======================================================================
OpenGl_GraphicDriver::OpenGl_GraphicDriver (const Handle(Aspect_DisplayConnection)& theDisp,
                                            const Standard_Boolean                  theToInitialize)
: Graphic3d_GraphicDriver (theDisp),
  myIsOwnContext (Standard_False),
#if defined(HAVE_EGL) || defined(HAVE_GLES2) || defined(OCCT_UWP) || defined(__ANDROID__) || defined(__QNX__) || defined(__EMSCRIPTEN__)
  myEglDisplay ((Aspect_Display )EGL_NO_DISPLAY),
  myEglContext ((Aspect_RenderingContext )EGL_NO_CONTEXT),
  myEglConfig  (NULL),
#endif
  myCaps           (new OpenGl_Caps()),
  myMapOfView      (1, NCollection_BaseAllocator::CommonBaseAllocator()),
  myMapOfStructure (1, NCollection_BaseAllocator::CommonBaseAllocator())
{
#if !defined(_WIN32) && !defined(__ANDROID__) && !defined(__QNX__) && !defined(__EMSCRIPTEN__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  if (myDisplayConnection.IsNull())
  {
    //throw Aspect_GraphicDeviceDefinitionError("OpenGl_GraphicDriver: cannot connect to X server!");
    return;
  }

  Display* aDisplay = myDisplayConnection->GetDisplay();
  Bool toSync = ::getenv ("CSF_GraphicSync") != NULL
             || ::getenv ("CALL_SYNCHRO_X")  != NULL;
  XSynchronize (aDisplay, toSync);

#if !defined(HAVE_EGL) && !defined(HAVE_GLES2)
  // does the server know about OpenGL & GLX?
  int aDummy;
  if (!XQueryExtension (aDisplay, "GLX", &aDummy, &aDummy, &aDummy))
  {
    ::Message::SendWarning ("OpenGl_GraphicDriver, this system doesn't appear to support OpenGL");
  }
#endif
#endif
  if (theToInitialize
  && !InitContext())
  {
    throw Aspect_GraphicDeviceDefinitionError("OpenGl_GraphicDriver: default context can not be initialized!");
  }
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
    const Handle(OpenGl_View)& aView = aViewIter.Value();
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
    const Handle(OpenGl_View)& aView = aViewIter.Value();
    aView->ReleaseGlResources (aCtxShared);
  }

  for (NCollection_DataMap<Standard_Integer, OpenGl_Structure*>::Iterator aStructIt (myMapOfStructure);
       aStructIt.More (); aStructIt.Next())
  {
    OpenGl_Structure* aStruct = aStructIt.ChangeValue();
    aStruct->ReleaseGlResources (aCtxShared);
  }

  const bool isDeviceLost = !myMapOfStructure.IsEmpty();
  for (NCollection_Map<Handle(OpenGl_View)>::Iterator aViewIter (myMapOfView);
       aViewIter.More(); aViewIter.Next())
  {
    const Handle(OpenGl_View)& aView = aViewIter.Value();
    if (isDeviceLost)
    {
      aView->StructureManager()->SetDeviceLost();
    }

    const Handle(OpenGl_Window)& aWindow = aView->GlWindow();
    if (aWindow.IsNull())
    {
      continue;
    }

    aWindow->GetGlContext()->forcedRelease();
  }

#if defined(HAVE_EGL) || defined(HAVE_GLES2) || defined(OCCT_UWP) || defined(__ANDROID__) || defined(__QNX__) || defined(__EMSCRIPTEN__)
  if (myIsOwnContext)
  {
    if (myEglContext != (Aspect_RenderingContext )EGL_NO_CONTEXT)
    {
      if (eglMakeCurrent ((EGLDisplay )myEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) != EGL_TRUE)
      {
        ::Message::SendWarning ("OpenGl_GraphicDriver, FAILED to release OpenGL context");
      }
      eglDestroyContext ((EGLDisplay )myEglDisplay, (EGLContext )myEglContext);
    }

    if (myEglDisplay != (Aspect_Display )EGL_NO_DISPLAY)
    {
      if (eglTerminate ((EGLDisplay )myEglDisplay) != EGL_TRUE)
      {
        ::Message::SendWarning ("OpenGl_GraphicDriver, EGL, eglTerminate FAILED");
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
#if defined(HAVE_EGL) || defined(HAVE_GLES2) || defined(OCCT_UWP) || defined(__ANDROID__) || defined(__QNX__) || defined(__EMSCRIPTEN__)

#if !defined(_WIN32) && !defined(__ANDROID__) && !defined(__QNX__) && !defined(__EMSCRIPTEN__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
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
    ::Message::SendFail ("Error: no EGL display");
    return Standard_False;
  }

  EGLint aVerMajor = 0; EGLint aVerMinor = 0;
  if (eglInitialize ((EGLDisplay )myEglDisplay, &aVerMajor, &aVerMinor) != EGL_TRUE)
  {
    ::Message::SendFail ("Error: EGL display is unavailable");
    return Standard_False;
  }

  myEglConfig = chooseEglSurfConfig ((EGLDisplay )myEglDisplay);
  if (myEglConfig == NULL)
  {
    ::Message::SendFail ("Error: EGL does not provide compatible configurations");
    return Standard_False;
  }

#if defined(GL_ES_VERSION_2_0)
  EGLint anEglCtxAttribs3[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE, EGL_NONE };
  EGLint anEglCtxAttribs2[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
  if (eglBindAPI (EGL_OPENGL_ES_API) != EGL_TRUE)
  {
    ::Message::SendFail ("Error: EGL does not provide OpenGL ES client");
    return Standard_False;
  }
  if (myCaps->contextMajorVersionUpper != 2)
  {
    myEglContext = (Aspect_RenderingContext )eglCreateContext ((EGLDisplay )myEglDisplay, myEglConfig, EGL_NO_CONTEXT, anEglCtxAttribs3);
  }
  if ((EGLContext )myEglContext == EGL_NO_CONTEXT)
  {
    myEglContext = (Aspect_RenderingContext )eglCreateContext ((EGLDisplay )myEglDisplay, myEglConfig, EGL_NO_CONTEXT, anEglCtxAttribs2);
  }
#else
  EGLint* anEglCtxAttribs = NULL;
  if (eglBindAPI (EGL_OPENGL_API) != EGL_TRUE)
  {
    ::Message::SendFail ("Error: EGL does not provide OpenGL client");
    return Standard_False;
  }
  myEglContext = (Aspect_RenderingContext )eglCreateContext ((EGLDisplay )myEglDisplay, myEglConfig, EGL_NO_CONTEXT, anEglCtxAttribs);
#endif

  if ((EGLContext )myEglContext == EGL_NO_CONTEXT)
  {
    ::Message::SendFail ("Error: EGL is unable to create OpenGL context");
    return Standard_False;
  }
  // eglMakeCurrent() fails or even crash with EGL_NO_SURFACE on some implementations
  //if (eglMakeCurrent ((EGLDisplay )myEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, (EGLContext )myEglContext) != EGL_TRUE)
  //{
  //  ::Message::SendFail ("Error: EGL is unable bind OpenGL context");
  //  return Standard_False;
  //}
#endif
  myIsOwnContext = Standard_True;
  return Standard_True;
}

#if defined(HAVE_EGL) || defined(HAVE_GLES2) || defined(OCCT_UWP) || defined(__ANDROID__) || defined(__QNX__) || defined(__EMSCRIPTEN__)
// =======================================================================
// function : InitEglContext
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_GraphicDriver::InitEglContext (Aspect_Display          theEglDisplay,
                                                       Aspect_RenderingContext theEglContext,
                                                       void*                   theEglConfig)
{
  ReleaseContext();
#if !defined(_WIN32) && !defined(__ANDROID__) && !defined(__QNX__) && !defined(__EMSCRIPTEN__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
  if (myDisplayConnection.IsNull())
  {
    return Standard_False;
  }
#endif

  if ((EGLDisplay )theEglDisplay == EGL_NO_DISPLAY
   || (EGLContext )theEglContext == EGL_NO_CONTEXT)
  {
    return Standard_False;
  }
  myEglDisplay = theEglDisplay;
  myEglContext = theEglContext;
  myEglConfig  = theEglConfig;
  if (theEglConfig == NULL)
  {
    myEglConfig = chooseEglSurfConfig ((EGLDisplay )myEglDisplay);
    if (myEglConfig == NULL)
    {
      ::Message::SendFail ("Error: EGL does not provide compatible configurations");
      return Standard_False;
    }
  }
  return Standard_True;
}
#endif

// =======================================================================
// function : InquireLimit
// purpose  :
// =======================================================================
Standard_Integer OpenGl_GraphicDriver::InquireLimit (const Graphic3d_TypeOfLimit theType) const
{
  const Handle(OpenGl_Context)& aCtx = GetSharedContext();
  switch (theType)
  {
    case Graphic3d_TypeOfLimit_MaxNbLights:
      return Graphic3d_ShaderProgram::THE_MAX_LIGHTS_DEFAULT;
    case Graphic3d_TypeOfLimit_MaxNbClipPlanes:
      return !aCtx.IsNull() ? aCtx->MaxClipPlanes() : 0;
    case Graphic3d_TypeOfLimit_MaxNbViews:
      return 10000;
    case Graphic3d_TypeOfLimit_MaxTextureSize:
      return !aCtx.IsNull() ? aCtx->MaxTextureSize() : 1024;
    case Graphic3d_TypeOfLimit_MaxCombinedTextureUnits:
      return !aCtx.IsNull() ? aCtx->MaxCombinedTextureUnits() : 1;
    case Graphic3d_TypeOfLimit_MaxMsaa:
      return !aCtx.IsNull() ? aCtx->MaxMsaaSamples() : 0;
    case Graphic3d_TypeOfLimit_MaxViewDumpSizeX:
      return !aCtx.IsNull() ? aCtx->MaxDumpSizeX() : 1024;
    case Graphic3d_TypeOfLimit_MaxViewDumpSizeY:
      return !aCtx.IsNull() ? aCtx->MaxDumpSizeY() : 1024;
    case Graphic3d_TypeOfLimit_HasPBR:
      return (!aCtx.IsNull() && aCtx->HasPBR()) ? 1 : 0;
    case Graphic3d_TypeOfLimit_HasRayTracing:
      return (!aCtx.IsNull() && aCtx->HasRayTracing()) ? 1 : 0;
    case Graphic3d_TypeOfLimit_HasRayTracingTextures:
      return (!aCtx.IsNull() && aCtx->HasRayTracingTextures()) ? 1 : 0;
    case Graphic3d_TypeOfLimit_HasRayTracingAdaptiveSampling:
      return (!aCtx.IsNull() && aCtx->HasRayTracingAdaptiveSampling()) ? 1 : 0;
    case Graphic3d_TypeOfLimit_HasRayTracingAdaptiveSamplingAtomic:
      return (!aCtx.IsNull() && aCtx->HasRayTracingAdaptiveSamplingAtomic()) ? 1 : 0;
    case Graphic3d_TypeOfLimit_HasSRGB:
      return (!aCtx.IsNull() && aCtx->HasSRGB()) ? 1 : 0;
    case Graphic3d_TypeOfLimit_HasBlendedOit:
      return (!aCtx.IsNull()
            && aCtx->hasDrawBuffers != OpenGl_FeatureNotAvailable
            && (aCtx->hasFloatBuffer != OpenGl_FeatureNotAvailable || aCtx->hasHalfFloatBuffer != OpenGl_FeatureNotAvailable)) ? 1 : 0;
    case Graphic3d_TypeOfLimit_HasBlendedOitMsaa:
      return (!aCtx.IsNull()
            && aCtx->hasSampleVariables != OpenGl_FeatureNotAvailable
            && (InquireLimit (Graphic3d_TypeOfLimit_HasBlendedOit) == 1)) ? 1 : 0;
    case Graphic3d_TypeOfLimit_HasFlatShading:
      return !aCtx.IsNull() && aCtx->hasFlatShading != OpenGl_FeatureNotAvailable ? 1 : 0;
    case Graphic3d_TypeOfLimit_IsWorkaroundFBO:
      return !aCtx.IsNull() && aCtx->MaxTextureSize() != aCtx->MaxDumpSizeX() ? 1 : 0;
    case Graphic3d_TypeOfLimit_HasMeshEdges:
      return !aCtx.IsNull() && aCtx->hasGeometryStage != OpenGl_FeatureNotAvailable ? 1 : 0;
    case Graphic3d_TypeOfLimit_NB:
      return 0;
  }
  return 0;
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
const Handle(OpenGl_Context)& OpenGl_GraphicDriver::GetSharedContext (bool theBound) const
{
  if (myMapOfView.IsEmpty())
  {
    return TheNullGlCtx;
  }

  for (NCollection_Map<Handle(OpenGl_View)>::Iterator aViewIter (myMapOfView); aViewIter.More(); aViewIter.Next())
  {
    if (const Handle(OpenGl_Window)& aWindow = aViewIter.Value()->GlWindow())
    {
      if (!theBound)
      {
        return aWindow->GetGlContext();
      }
      else if (aWindow->GetGlContext()->IsCurrent())
      {
        return aWindow->GetGlContext();
      }
    }
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
  OpenGl_Aspects aTextAspect;
  TCollection_ExtendedString anExtText = theText;
  NCollection_String aText (anExtText.ToExtString());
  OpenGl_Text::StringSize(aCtx, aText, aTextAspect, aHeight, theView->RenderingParams().Resolution, theWidth, theAscent, theDescent);
}

//=======================================================================
//function : InsertLayerBefore
//purpose  :
//=======================================================================
void OpenGl_GraphicDriver::InsertLayerBefore (const Graphic3d_ZLayerId theLayerId,
                                              const Graphic3d_ZLayerSettings& theSettings,
                                              const Graphic3d_ZLayerId theLayerAfter)
{
  base_type::InsertLayerBefore (theLayerId, theSettings, theLayerAfter);

  // Add layer to all views
  for (NCollection_Map<Handle(OpenGl_View)>::Iterator aViewIt (myMapOfView); aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->InsertLayerBefore (theLayerId, theSettings, theLayerAfter);
  }
}

//=======================================================================
//function : InsertLayerAfter
//purpose  :
//=======================================================================
void OpenGl_GraphicDriver::InsertLayerAfter (const Graphic3d_ZLayerId theNewLayerId,
                                             const Graphic3d_ZLayerSettings& theSettings,
                                             const Graphic3d_ZLayerId theLayerBefore)
{
  base_type::InsertLayerAfter (theNewLayerId, theSettings, theLayerBefore);

  // Add layer to all views
  for (NCollection_Map<Handle(OpenGl_View)>::Iterator aViewIt (myMapOfView); aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->InsertLayerAfter (theNewLayerId, theSettings, theLayerBefore);
  }
}

//=======================================================================
//function : RemoveZLayer
//purpose  :
//=======================================================================
void OpenGl_GraphicDriver::RemoveZLayer (const Graphic3d_ZLayerId theLayerId)
{
  base_type::RemoveZLayer (theLayerId);

  // Remove layer from all of the views
  for (NCollection_Map<Handle(OpenGl_View)>::Iterator aViewIt (myMapOfView); aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->RemoveZLayer (theLayerId);
  }

  // Unset Z layer for all of the structures.
  for (NCollection_DataMap<Standard_Integer, OpenGl_Structure*>::Iterator aStructIt (myMapOfStructure); aStructIt.More(); aStructIt.Next())
  {
    OpenGl_Structure* aStruct = aStructIt.ChangeValue ();
    if (aStruct->ZLayer() == theLayerId)
      aStruct->SetZLayer (Graphic3d_ZLayerId_Default);
  }
}

//=======================================================================
//function : SetZLayerSettings
//purpose  :
//=======================================================================
void OpenGl_GraphicDriver::SetZLayerSettings (const Graphic3d_ZLayerId theLayerId,
                                              const Graphic3d_ZLayerSettings& theSettings)
{
  base_type::SetZLayerSettings (theLayerId, theSettings);

  // Change Z layer settings in all managed views
  for (NCollection_Map<Handle(OpenGl_View)>::Iterator aViewIt (myMapOfView); aViewIt.More(); aViewIt.Next())
  {
    aViewIt.Value()->SetZLayerSettings (theLayerId, theSettings);
  }
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
// function : CreateView
// purpose  :
// =======================================================================
Handle(Graphic3d_CView) OpenGl_GraphicDriver::CreateView (const Handle(Graphic3d_StructureManager)& theMgr)
{
  Handle(OpenGl_View) aView = new OpenGl_View (theMgr, this, myCaps, &myStateCounter);
  myMapOfView.Add (aView);
  for (NCollection_List<Handle(Graphic3d_Layer)>::Iterator aLayerIter (myLayers); aLayerIter.More(); aLayerIter.Next())
  {
    const Handle(Graphic3d_Layer)& aLayer = aLayerIter.Value();
    aView->InsertLayerAfter (aLayer->LayerId(), aLayer->LayerSettings(), Graphic3d_ZLayerId_UNKNOWN);
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

    if (!myMapOfStructure.IsEmpty())
    {
      aView->StructureManager()->SetDeviceLost();
    }
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

#if defined(_WIN32) && !defined(OCCT_UWP)
  const Handle(WNT_Window) THEWindow = Handle(WNT_Window)::DownCast (AWindow);
  Aspect_Handle TheSpecifiedWindowId = THEWindow->HWindow ();
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  const Handle(Cocoa_Window) THEWindow = Handle(Cocoa_Window)::DownCast (AWindow);
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    UIView* TheSpecifiedWindowId = THEWindow->HView();
  #else
    NSView* TheSpecifiedWindowId = THEWindow->HView();
  #endif
#elif defined(__ANDROID__) || defined(__QNX__) || defined(__EMSCRIPTEN__) || defined(OCCT_UWP)
  (void )AWindow;
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

#if defined(_WIN32) && !defined(OCCT_UWP)
      const Handle(WNT_Window) theWindow = Handle(WNT_Window)::DownCast (AspectWindow);
      Aspect_Handle TheWindowIdOfView = theWindow->HWindow ();
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
      const Handle(Cocoa_Window) theWindow = Handle(Cocoa_Window)::DownCast (AspectWindow);
      #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
        UIView* TheWindowIdOfView = theWindow->HView();
      #else
        NSView* TheWindowIdOfView = theWindow->HView();
      #endif
#elif defined(__ANDROID__) || defined(__QNX__) || defined(__EMSCRIPTEN__) || defined(OCCT_UWP)
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

//=======================================================================
//function : setDeviceLost
//purpose  :
//=======================================================================
void OpenGl_GraphicDriver::setDeviceLost()
{
  if (myMapOfStructure.IsEmpty())
  {
    return;
  }

  for (NCollection_Map<Handle(OpenGl_View)>::Iterator aViewIter (myMapOfView); aViewIter.More(); aViewIter.Next())
  {
    const Handle(OpenGl_View)& aView = aViewIter.Value();
    if (aView->myWasRedrawnGL)
    {
      aView->StructureManager()->SetDeviceLost();
    }
  }
}
