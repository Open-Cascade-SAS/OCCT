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
  #include <windows.h> // for UWP
#endif

#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Text.hxx>
#include <OpenGl_Window.hxx>

#include <Aspect_GraphicDeviceDefinitionError.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <OSD_Environment.hxx>
#include <Standard_NotImplemented.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_GraphicDriver,Graphic3d_GraphicDriver)

#if defined(_WIN32)
  #include <WNT_Window.hxx>
#elif defined(HAVE_XLIB)
  #include <Xw_Window.hxx>
#elif defined(__APPLE__)
  #include <Cocoa_Window.hxx>
#else
  #include <Aspect_NeutralWindow.hxx>
#endif

#if !defined(_WIN32) && !defined(__ANDROID__) && !defined(__QNX__) && !defined(__EMSCRIPTEN__) && (!defined(__APPLE__) || defined(HAVE_XLIB))
  #include <X11/Xlib.h> // XOpenDisplay()
  #include <GL/glx.h>
#endif

#if !defined(HAVE_EGL)
#if defined(__ANDROID__) || defined(__QNX__) || defined(__EMSCRIPTEN__) || defined(HAVE_GLES2) || defined(OCCT_UWP)
  #if !defined(__APPLE__)
    #define HAVE_EGL // EAGL is used instead of EGL
  #endif
#elif !defined(_WIN32) && !defined(__APPLE__) && !defined(HAVE_XLIB)
  #define HAVE_EGL
#endif
#endif

#if defined(HAVE_EGL)
  #include <EGL/egl.h>
  #ifndef EGL_OPENGL_ES3_BIT
    #define EGL_OPENGL_ES3_BIT 0x00000040
  #endif
#endif

#if defined(HAVE_GLES2) || defined(OCCT_UWP) || defined(__ANDROID__) || defined(__QNX__) || defined(__EMSCRIPTEN__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)
  #define OpenGl_USE_GLES2
#endif

namespace
{
  static const Handle(OpenGl_Context) TheNullGlCtx;

#if defined(HAVE_EGL)
  //! Wrapper over eglChooseConfig() called with preferred defaults.
  static EGLConfig chooseEglSurfConfig (EGLDisplay theDisplay,
                                        const Handle(OpenGl_Caps)& theCaps)
  {
    EGLConfig aCfg = NULL;
    EGLint aNbConfigs = 0;
    for (Standard_Integer aGlesVer = 3; aGlesVer >= 2; --aGlesVer)
    {
      bool isDeepColor = theCaps->buffersDeepColor;
      EGLint aConfigAttribs[] =
      {
        EGL_RED_SIZE,     isDeepColor ? 10 : 8,
        EGL_GREEN_SIZE,   isDeepColor ? 10 : 8,
        EGL_BLUE_SIZE,    isDeepColor ? 10 : 8,
        EGL_ALPHA_SIZE,   0,
        EGL_DEPTH_SIZE,   24,
        EGL_STENCIL_SIZE, 8,
      #if defined(OpenGl_USE_GLES2)
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
      #else
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
      #endif
        EGL_NONE
      };

    #if defined(OpenGl_USE_GLES2)
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

      if (isDeepColor)
      {
        // try config with smaller color buffer
        aConfigAttribs[0 * 2 + 1] = 8;
        aConfigAttribs[1 * 2 + 1] = 8;
        aConfigAttribs[2 * 2 + 1] = 8;
        if (eglChooseConfig (theDisplay, aConfigAttribs, &aCfg, 1, &aNbConfigs) == EGL_TRUE
         && aCfg != NULL)
        {
          return aCfg;
        }
        eglGetError();
      }

      {
        // try config with smaller depth buffer
        aConfigAttribs[4 * 2 + 1] = 16;
        if (eglChooseConfig (theDisplay, aConfigAttribs, &aCfg, 1, &aNbConfigs) == EGL_TRUE
         && aCfg != NULL)
        {
          return aCfg;
        }
        eglGetError();
      }
    }
    return aCfg;
  }
#elif defined(HAVE_XLIB)
  //! Search for RGBA double-buffered visual with stencil buffer.
  static int TheDoubleBuffVisual[] =
  {
    GLX_RGBA,
    GLX_DEPTH_SIZE, 16,
    GLX_STENCIL_SIZE, 1,
    GLX_DOUBLEBUFFER,
    None
  };

  //! Search for RGBA double-buffered visual with stencil buffer.
  static int TheDoubleBuffFBConfig[] =
  {
    GLX_X_RENDERABLE,  True,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE,   GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
    GLX_DEPTH_SIZE,    16,
    GLX_STENCIL_SIZE,  1,
    GLX_DOUBLEBUFFER,  True,
    None
  };
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
  myEglDisplay (NULL),
  myEglContext (NULL),
  myEglConfig  (NULL),
  myCaps           (new OpenGl_Caps()),
  myMapOfView      (1, NCollection_BaseAllocator::CommonBaseAllocator()),
  myMapOfStructure (1, NCollection_BaseAllocator::CommonBaseAllocator())
{
#if defined(HAVE_EGL)
  myEglDisplay = (Aspect_Display )EGL_NO_DISPLAY;
  myEglContext = (Aspect_RenderingContext )EGL_NO_CONTEXT;
#endif
#if defined(OpenGl_USE_GLES2)
  myCaps->contextCompatible = false;
#endif

#if defined(HAVE_XLIB)
  if (myDisplayConnection.IsNull())
  {
    //throw Aspect_GraphicDeviceDefinitionError("OpenGl_GraphicDriver: cannot connect to X server!");
    return;
  }

  Display* aDisplay = (Display* )myDisplayConnection->GetDisplayAspect();
  Bool toSync = ::getenv ("CSF_GraphicSync") != NULL
             || ::getenv ("CALL_SYNCHRO_X")  != NULL;
  XSynchronize (aDisplay, toSync);
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

#if defined(HAVE_EGL)
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
#if defined(HAVE_EGL)

#if defined(HAVE_XLIB)
  if (myDisplayConnection.IsNull())
  {
    return Standard_False;
  }
  Display* aDisplay = (Display* )myDisplayConnection->GetDisplayAspect();
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

  myEglConfig = chooseEglSurfConfig ((EGLDisplay )myEglDisplay, myCaps);
  if (myEglConfig == NULL)
  {
    ::Message::SendFail ("Error: EGL does not provide compatible configurations");
    return Standard_False;
  }

#if defined(OpenGl_USE_GLES2)
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
  chooseVisualInfo();
  myIsOwnContext = Standard_True;
  return Standard_True;
}

// =======================================================================
// function : InitEglContext
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_GraphicDriver::InitEglContext (Aspect_Display          theEglDisplay,
                                                       Aspect_RenderingContext theEglContext,
                                                       void*                   theEglConfig)
{
  ReleaseContext();
#if defined(HAVE_EGL)
#if defined(HAVE_XLIB)
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
    myEglConfig = chooseEglSurfConfig ((EGLDisplay )myEglDisplay, myCaps);
    if (myEglConfig == NULL)
    {
      ::Message::SendFail ("Error: EGL does not provide compatible configurations");
      return Standard_False;
    }
  }
  chooseVisualInfo();
  return Standard_True;
#else
  (void )theEglDisplay;
  (void )theEglContext;
  (void )theEglConfig;
  throw Standard_NotImplemented ("OpenGl_GraphicDriver::InitEglContext() is not implemented");
#endif
}

// =======================================================================
// function : chooseVisualInfo
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::chooseVisualInfo()
{
  if (myDisplayConnection.IsNull())
  {
    return;
  }

#if defined(HAVE_XLIB)
  Display* aDisp = (Display* )myDisplayConnection->GetDisplayAspect();

  XVisualInfo* aVisInfo = NULL;
  Aspect_FBConfig anFBConfig = NULL;
#if defined(HAVE_EGL)
  XVisualInfo aVisInfoTmp;
  memset (&aVisInfoTmp, 0, sizeof(aVisInfoTmp));
  aVisInfoTmp.screen = DefaultScreen (aDisp);
  if (myEglDisplay != EGL_NO_DISPLAY
   && myEglConfig != NULL
   && eglGetConfigAttrib ((EGLDisplay )myEglDisplay, myEglConfig, EGL_NATIVE_VISUAL_ID, (EGLint* )&aVisInfoTmp.visualid) == EGL_TRUE)
  {
    int aNbVisuals = 0;
    aVisInfo = XGetVisualInfo (aDisp, VisualIDMask | VisualScreenMask, &aVisInfoTmp, &aNbVisuals);
  }
#else
  int aScreen = DefaultScreen(aDisp);
  int aDummy = 0;
  if (!XQueryExtension (aDisp, "GLX", &aDummy, &aDummy, &aDummy)
   || !glXQueryExtension (aDisp, &aDummy, &aDummy))
  {
    Message::SendFail ("Error: OpenGl_GraphicDriver, GLX extension is unavailable");
  }

  // FBConfigs were added in GLX version 1.3
  int aGlxMajor = 0, aGlxMinor = 0;
  const bool hasFBCfg = glXQueryVersion (aDisp, &aGlxMajor, &aGlxMinor)
                     && ((aGlxMajor == 1 && aGlxMinor >= 3) || (aGlxMajor > 1));
  if (hasFBCfg)
  {
    int aFBCount = 0;
    GLXFBConfig* aFBCfgList = NULL;
    if (hasFBCfg)
    {
      aFBCfgList = glXChooseFBConfig (aDisp, aScreen, TheDoubleBuffFBConfig, &aFBCount);
    }
    if(aFBCfgList != NULL
    && aFBCount >= 1)
    {
      anFBConfig = aFBCfgList[0];
      aVisInfo   = glXGetVisualFromFBConfig (aDisp, anFBConfig);

      /*int aDepthSize = 0, aStencilSize = 0;
      glXGetFBConfigAttrib (aDisp, anFBConfig, GLX_DEPTH_SIZE, &aDepthSize);
      glXGetFBConfigAttrib (aDisp, anFBConfig, GLX_STENCIL_SIZE, &aStencilSize);
      Message::SendInfo() << "GLX FBConfig:"
                          << "\n  DepthSize= " << aDepthSize
                          << "\n  StencilSize= " << aStencilSize;*/
    }
    XFree (aFBCfgList);
  }

  if (aVisInfo == NULL)
  {
    aVisInfo = glXChooseVisual (aDisp, aScreen, TheDoubleBuffVisual);
  }
#endif
  if (aVisInfo != NULL)
  {
    myDisplayConnection->SetDefaultVisualInfo ((Aspect_XVisualInfo* )aVisInfo, anFBConfig);
  }
  else
  {
    Message::SendWarning ("OpenGl_GraphicDriver, couldn't find compatible Visual (RGBA, double-buffered)");
  }
#endif
}

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
// function : IsVerticalSync
// purpose  :
// =======================================================================
bool OpenGl_GraphicDriver::IsVerticalSync() const
{
  return myCaps->swapInterval == 1;
}

// =======================================================================
// function : SetVerticalSync
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::SetVerticalSync (bool theToEnable)
{
  myCaps->swapInterval = theToEnable ? 1 : 0;
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
  OpenGl_Text::StringSize (aCtx, aText, aTextAspect, aHeight,
                           theView->RenderingParams().Resolution, theView->RenderingParams().FontHinting,
                           theWidth, theAscent, theDescent);
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
  myMapOfStructure.Bind (aStructure->Identification(), aStructure.operator->());
  return aStructure;
}

// =======================================================================
// function : Structure
// purpose  :
// =======================================================================
void OpenGl_GraphicDriver::RemoveStructure (Handle(Graphic3d_CStructure)& theCStructure)
{
  OpenGl_Structure* aStructure = NULL;
  if (!myMapOfStructure.Find (theCStructure->Identification(), aStructure))
  {
    return;
  }

  myMapOfStructure.UnBind (theCStructure->Identification());
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
// function : CreateRenderWindow
// purpose  :
// =======================================================================
Handle(OpenGl_Window) OpenGl_GraphicDriver::CreateRenderWindow (const Handle(Aspect_Window)& theNativeWindow,
                                                                const Handle(Aspect_Window)& theSizeWindow,
                                                                const Aspect_RenderingContext theContext)
{
  Handle(OpenGl_Context) aShareCtx = GetSharedContext();
  Handle(OpenGl_Window) aWindow = new OpenGl_Window();
  aWindow->Init (this, theNativeWindow, theSizeWindow, theContext, myCaps, aShareCtx);
  return aWindow;
}

//=======================================================================
//function : ViewExists
//purpose  :
//=======================================================================
Standard_Boolean OpenGl_GraphicDriver::ViewExists (const Handle(Aspect_Window)& theWindow,
                                                   Handle(Graphic3d_CView)& theView)
{
  // Parse the list of views to find a view with the specified window
  const Aspect_Drawable aNativeHandle = theWindow->NativeHandle();
  for (NCollection_Map<Handle(OpenGl_View)>::Iterator aViewIt (myMapOfView); aViewIt.More(); aViewIt.Next())
  {
    const Handle(OpenGl_View)& aView = aViewIt.Value();
    if (!aView->IsDefined()
     || !aView->IsActive())
    {
      continue;
    }

    const Handle(Aspect_Window) anAspectWindow = aView->Window();
    const Aspect_Drawable aViewNativeHandle = anAspectWindow->NativeHandle();
    if (aViewNativeHandle == aNativeHandle)
    {
      theView = aView;
      return true;
    }
  }

  return false;
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
