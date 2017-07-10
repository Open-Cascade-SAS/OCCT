// Created on: 2012-01-26
// Created by: Kirill GAVRILOV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#include <OpenGl_Context.hxx>

#include <OpenGl_ArbTBO.hxx>
#include <OpenGl_ArbIns.hxx>
#include <OpenGl_ArbDbg.hxx>
#include <OpenGl_ArbFBO.hxx>
#include <OpenGl_ExtGS.hxx>
#include <OpenGl_ArbSamplerObject.hxx>
#include <OpenGl_ArbTexBindless.hxx>
#include <OpenGl_GlCore44.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_Sampler.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_AspectFace.hxx>
#include <Graphic3d_TransformUtils.hxx>
#include <Graphic3d_RenderingParams.hxx>

#include <Message_Messenger.hxx>

#include <NCollection_Vector.hxx>

#include <Standard_ProgramError.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Context,Standard_Transient)

#if defined(HAVE_EGL)
  #include <EGL/egl.h>
  #ifdef _MSC_VER
    #pragma comment(lib, "libEGL.lib")
  #endif
#elif defined(_WIN32)
  //
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  #include <dlfcn.h>
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    //
  #else
    #include <OpenGL/OpenGL.h>
    #include <CoreGraphics/CoreGraphics.h>
  #endif
#else
  #include <GL/glx.h> // glXGetProcAddress()
#endif

#ifdef HAVE_GL2PS
  #include <gl2ps.h>
  #ifdef _MSC_VER
    #pragma comment (lib, "gl2ps.lib")
  #endif
#endif


namespace
{
  static const Handle(OpenGl_Resource) NULL_GL_RESOURCE;
  static const OpenGl_Mat4 THE_IDENTITY_MATRIX;

  //! Add key-value pair to the dictionary.
  static void addInfo (TColStd_IndexedDataMapOfStringString& theDict,
                       const TCollection_AsciiString& theKey,
                       const TCollection_AsciiString& theValue)
  {
    theDict.ChangeFromIndex (theDict.Add (theKey, theValue)) = theValue;
  }

  //! Add key-value pair to the dictionary.
  static void addInfo (TColStd_IndexedDataMapOfStringString& theDict,
                       const TCollection_AsciiString& theKey,
                       const char* theValue)
  {
    TCollection_AsciiString aValue (theValue != NULL ? theValue : "");
    theDict.ChangeFromIndex (theDict.Add (theKey, aValue)) = aValue;
  }
}

// =======================================================================
// function : OpenGl_Context
// purpose  :
// =======================================================================
OpenGl_Context::OpenGl_Context (const Handle(OpenGl_Caps)& theCaps)
: core11     (NULL),
  core11fwd  (NULL),
  core15     (NULL),
  core15fwd  (NULL),
  core20     (NULL),
  core20fwd  (NULL),
  core32     (NULL),
  core32back (NULL),
  core33     (NULL),
  core33back (NULL),
  core41     (NULL),
  core41back (NULL),
  core42     (NULL),
  core42back (NULL),
  core43     (NULL),
  core43back (NULL),
  core44     (NULL),
  core44back (NULL),
  caps   (!theCaps.IsNull() ? theCaps : new OpenGl_Caps()),
#if defined(GL_ES_VERSION_2_0)
  hasHighp   (Standard_False),
  hasUintIndex(Standard_False),
  hasTexRGBA8(Standard_False),
#else
  hasHighp   (Standard_True),
  hasUintIndex(Standard_True),
  hasTexRGBA8(Standard_True),
#endif
  hasDrawBuffers     (OpenGl_FeatureNotAvailable),
  hasFloatBuffer     (OpenGl_FeatureNotAvailable),
  hasHalfFloatBuffer (OpenGl_FeatureNotAvailable),
  hasSampleVariables (OpenGl_FeatureNotAvailable),
  arbDrawBuffers (Standard_False),
  arbNPTW  (Standard_False),
  arbTexRG (Standard_False),
  arbTexFloat (Standard_False),
  arbSamplerObject (NULL),
  arbTexBindless (NULL),
  arbTBO (NULL),
  arbTboRGB32 (Standard_False),
  arbIns (NULL),
  arbDbg (NULL),
  arbFBO (NULL),
  arbFBOBlit (NULL),
  arbSampleShading (Standard_False),
  extFragDepth (Standard_False),
  extDrawBuffers (Standard_False),
  extGS  (NULL),
  extBgra(Standard_False),
  extAnis(Standard_False),
  extPDS (Standard_False),
  atiMem (Standard_False),
  nvxMem (Standard_False),
  oesSampleVariables (Standard_False),
  mySharedResources (new OpenGl_ResourcesMap()),
  myDelayed         (new OpenGl_DelayReleaseMap()),
  myUnusedResources (new OpenGl_ResourcesStack()),
  myClippingState (),
  myGlLibHandle (NULL),
  myFuncs (new OpenGl_GlFunctions()),
  myAnisoMax   (1),
  myTexClamp   (GL_CLAMP_TO_EDGE),
  myMaxTexDim  (1024),
  myMaxTexCombined (1),
  myMaxClipPlanes (6),
  myMaxMsaaSamples(0),
  myMaxDrawBuffers (1),
  myMaxColorAttachments (1),
  myGlVerMajor (0),
  myGlVerMinor (0),
  myIsInitialized (Standard_False),
  myIsStereoBuffers (Standard_False),
  myIsGlNormalizeEnabled (Standard_False),
  myHasRayTracing (Standard_False),
  myHasRayTracingTextures (Standard_False),
  myHasRayTracingAdaptiveSampling (Standard_False),
#if !defined(GL_ES_VERSION_2_0)
  myPointSpriteOrig (GL_UPPER_LEFT),
  myRenderMode (GL_RENDER),
  myPolygonMode (GL_FILL),
#else
  myPointSpriteOrig (0),
  myRenderMode (0),
  myPolygonMode (0),
#endif
  myToCullBackFaces (false),
  myReadBuffer (0),
  myDrawBuffers (1),
  myDefaultVao (0),
  myIsGlDebugCtx (Standard_False),
  myResolution (Graphic3d_RenderingParams::THE_DEFAULT_RESOLUTION),
  myResolutionRatio (1.0f),
  myLineWidthScale (1.0f),
  myRenderScale (1.0f),
  myRenderScaleInv (1.0f)
{
  myViewport[0] = 0;
  myViewport[1] = 0;
  myViewport[2] = 0;
  myViewport[3] = 0;
  myViewportVirt[0] = 0;
  myViewportVirt[1] = 0;
  myViewportVirt[2] = 0;
  myViewportVirt[3] = 0;

  // system-dependent fields
#if defined(HAVE_EGL)
  myDisplay  = (Aspect_Display          )EGL_NO_DISPLAY;
  myWindow   = (Aspect_Drawable         )EGL_NO_SURFACE;
  myGContext = (Aspect_RenderingContext )EGL_NO_CONTEXT;
#elif defined(_WIN32)
  myWindow   = NULL;
  myWindowDC = NULL;
  myGContext = NULL;
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  // Vendors can not extend functionality on this system
  // and developers are limited to OpenGL support provided by Mac OS X SDK.
  // We retrieve function pointers from system library
  // to generalize extensions support on all platforms.
  // In this way we also reach binary compatibility benefit between OS releases
  // if some newest functionality is optionally used.
  // Notice that GL version / extension availability checks are required
  // because function pointers may be available but not functionality itself
  // (depends on renderer).
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  myGContext    = NULL;
  myGlLibHandle = dlopen ("/System/Library/Frameworks/OpenGLES.framework/OpenGLES", RTLD_LAZY);
#else
  myGContext    = NULL;
  myGlLibHandle = dlopen ("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", RTLD_LAZY);
#endif
#else
  myDisplay =  NULL;
  myWindow   = 0;
  myGContext = 0;
#endif

  memset (myFuncs.operator->(), 0, sizeof(OpenGl_GlFunctions));
  myShaderManager = new OpenGl_ShaderManager (this);
}

// =======================================================================
// function : ~OpenGl_Context
// purpose  :
// =======================================================================
OpenGl_Context::~OpenGl_Context()
{
  // release clean up queue
  ReleaseDelayed();

#if !defined(GL_ES_VERSION_2_0)
  // release default VAO
  if (myDefaultVao != 0
   && IsValid()
   && core32 != NULL)
  {
    core32->glDeleteVertexArrays (1, &myDefaultVao);
  }
  myDefaultVao = 0;
#endif

  // release default FBO
  if (!myDefaultFbo.IsNull())
  {
    myDefaultFbo->Release (this);
    myDefaultFbo.Nullify();
  }

  // release shared resources if any
  if (mySharedResources->GetRefCount() <= 1)
  {
    myShaderManager.Nullify();
    for (NCollection_DataMap<TCollection_AsciiString, Handle(OpenGl_Resource)>::Iterator anIter (*mySharedResources);
         anIter.More(); anIter.Next())
    {
      anIter.Value()->Release (this);
    }

    // release delayed resources added during deletion of shared resources
    while (!myUnusedResources->IsEmpty())
    {
      myUnusedResources->First()->Release (this);
      myUnusedResources->RemoveFirst();
    }
  }
  else if (myShaderManager->IsSameContext (this))
  {
    myShaderManager->SetContext (NULL);
  }
  mySharedResources.Nullify();
  myDelayed.Nullify();

  if (arbDbg != NULL
   && myIsGlDebugCtx
   && IsValid())
  {
    // reset callback
  #if !defined(GL_ES_VERSION_2_0)
    void* aPtr = NULL;
    glGetPointerv (GL_DEBUG_CALLBACK_USER_PARAM, &aPtr);
    if (aPtr == this)
  #endif
    {
      arbDbg->glDebugMessageCallback (NULL, NULL);
    }
    myIsGlDebugCtx = Standard_False;
  }
}

// =======================================================================
// function : forcedRelease
// purpose  :
// =======================================================================
void OpenGl_Context::forcedRelease()
{
  ReleaseDelayed();
  for (NCollection_DataMap<TCollection_AsciiString, Handle(OpenGl_Resource)>::Iterator anIter (*mySharedResources);
       anIter.More(); anIter.Next())
  {
    anIter.Value()->Release (this);
  }
  mySharedResources->Clear();
  myShaderManager->clear();
  myShaderManager->SetContext (NULL);

  // release delayed resources added during deletion of shared resources
  while (!myUnusedResources->IsEmpty())
  {
    myUnusedResources->First()->Release (this);
    myUnusedResources->RemoveFirst();
  }
}

// =======================================================================
// function : ResizeViewport
// purpose  :
// =======================================================================
void OpenGl_Context::ResizeViewport (const Standard_Integer* theRect)
{
  core11fwd->glViewport (theRect[0], theRect[1], theRect[2], theRect[3]);
  myViewport[0] = theRect[0];
  myViewport[1] = theRect[1];
  myViewport[2] = theRect[2];
  myViewport[3] = theRect[3];
  if (HasRenderScale())
  {
    myViewportVirt[0] = Standard_Integer(theRect[0] * myRenderScaleInv);
    myViewportVirt[1] = Standard_Integer(theRect[1] * myRenderScaleInv);
    myViewportVirt[2] = Standard_Integer(theRect[2] * myRenderScaleInv);
    myViewportVirt[3] = Standard_Integer(theRect[3] * myRenderScaleInv);
  }
  else
  {
    myViewportVirt[0] = theRect[0];
    myViewportVirt[1] = theRect[1];
    myViewportVirt[2] = theRect[2];
    myViewportVirt[3] = theRect[3];
  }
}

#if !defined(GL_ES_VERSION_2_0)
inline Standard_Integer stereoToMonoBuffer (const Standard_Integer theBuffer)
{
  switch (theBuffer)
  {
    case GL_BACK_LEFT:
    case GL_BACK_RIGHT:
      return GL_BACK;
    case GL_FRONT_LEFT:
    case GL_FRONT_RIGHT:
      return GL_FRONT;
    default:
      return theBuffer;
  }
}
#endif

// =======================================================================
// function : SetReadBuffer
// purpose  :
// =======================================================================
void OpenGl_Context::SetReadBuffer (const Standard_Integer theReadBuffer)
{
#if !defined(GL_ES_VERSION_2_0)
  myReadBuffer = !myIsStereoBuffers ? stereoToMonoBuffer (theReadBuffer) : theReadBuffer;
  if (myReadBuffer < GL_COLOR_ATTACHMENT0
   && arbFBO != NULL)
  {
    arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
  }
  ::glReadBuffer (myReadBuffer);
#else
  (void )theReadBuffer;
#endif
}

// =======================================================================
// function : SetDrawBuffer
// purpose  :
// =======================================================================
void OpenGl_Context::SetDrawBuffer (const Standard_Integer theDrawBuffer)
{
#if !defined(GL_ES_VERSION_2_0)
  const Standard_Integer aDrawBuffer = !myIsStereoBuffers ? stereoToMonoBuffer (theDrawBuffer) : theDrawBuffer;
  if (aDrawBuffer < GL_COLOR_ATTACHMENT0
   && arbFBO != NULL)
  {
    arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
  }
  ::glDrawBuffer (aDrawBuffer);

  myDrawBuffers.Clear();

  if (aDrawBuffer != GL_NONE)
  {
    myDrawBuffers.SetValue (0, aDrawBuffer);
  }
#else
  (void )theDrawBuffer;
#endif
}

// =======================================================================
// function : SetDrawBuffers
// purpose  :
// =======================================================================
void OpenGl_Context::SetDrawBuffers (const Standard_Integer theNb, const Standard_Integer* theDrawBuffers)
{
  Standard_ASSERT_RETURN (hasDrawBuffers, "Multiple draw buffers feature is not supported by the context", Standard_ASSERT_DO_NOTHING());

  myDrawBuffers.Clear();

  Standard_Boolean useDefaultFbo = Standard_False;
  for (Standard_Integer anI = 0; anI < theNb; ++anI)
  {
#if !defined(GL_ES_VERSION_2_0)
    const Standard_Integer aDrawBuffer = !myIsStereoBuffers ? stereoToMonoBuffer (theDrawBuffers[anI]) : theDrawBuffers[anI];
#else
    const Standard_Integer aDrawBuffer = theDrawBuffers[anI];
#endif
    if (aDrawBuffer < GL_COLOR_ATTACHMENT0 && aDrawBuffer != GL_NONE)
    {
      useDefaultFbo = Standard_True;
    }
    else if (aDrawBuffer != GL_NONE)
    {
      myDrawBuffers.SetValue (anI, aDrawBuffer);
    }
  }
  if (arbFBO != NULL && useDefaultFbo)
  {
    arbFBO->glBindFramebuffer (GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
  }

  myFuncs->glDrawBuffers (theNb, (const GLenum*)theDrawBuffers);
}

// =======================================================================
// function : SetCullBackFaces
// purpose  :
// =======================================================================
void OpenGl_Context::SetCullBackFaces (bool theToEnable)
{
  if (myToCullBackFaces == theToEnable)
  {
    return;
  }

  myToCullBackFaces = theToEnable;
  if (theToEnable)
  {
    //glCullFace (GL_BACK); GL_BACK by default
    core11fwd->glEnable (GL_CULL_FACE);
  }
  else
  {
    core11fwd->glDisable (GL_CULL_FACE);
  }
}

// =======================================================================
// function : FetchState
// purpose  :
// =======================================================================
void OpenGl_Context::FetchState()
{
#if !defined(GL_ES_VERSION_2_0)
  // cache feedback mode state
  if (core11 != NULL)
  {
    ::glGetIntegerv (GL_RENDER_MODE, &myRenderMode);
  }

  // cache read buffers state
  ::glGetIntegerv (GL_READ_BUFFER, &myReadBuffer);

  // cache draw buffers state
  myDrawBuffers.Clear();

  if (myMaxDrawBuffers == 1)
  {
    Standard_Integer aDrawBuffer;

    ::glGetIntegerv (GL_DRAW_BUFFER, &aDrawBuffer);

    if (aDrawBuffer != GL_NONE)
    {
      myDrawBuffers.SetValue (0, aDrawBuffer);
    }
  }
  else
  {
    Standard_Integer aDrawBuffer;

    for (Standard_Integer anI = 0; anI < myMaxDrawBuffers; ++anI)
    {
      ::glGetIntegerv (GL_DRAW_BUFFER0 + anI, &aDrawBuffer);

      if (aDrawBuffer != GL_NONE)
      {
        myDrawBuffers.SetValue (anI, aDrawBuffer);
      }
    }
  }
#endif
}

// =======================================================================
// function : Share
// purpose  :
// =======================================================================
void OpenGl_Context::Share (const Handle(OpenGl_Context)& theShareCtx)
{
  if (!theShareCtx.IsNull())
  {
    mySharedResources = theShareCtx->mySharedResources;
    myDelayed         = theShareCtx->myDelayed;
    myUnusedResources = theShareCtx->myUnusedResources;
    myShaderManager   = theShareCtx->myShaderManager;
  }
}

#if !defined(__APPLE__) || defined(MACOSX_USE_GLX)

// =======================================================================
// function : IsCurrent
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::IsCurrent() const
{
#if defined(HAVE_EGL)
  if ((EGLDisplay )myDisplay  == EGL_NO_DISPLAY
   || (EGLSurface )myWindow   == EGL_NO_SURFACE
   || (EGLContext )myGContext == EGL_NO_CONTEXT)
  {
    return Standard_False;
  }

  return (((EGLDisplay )myDisplay  == eglGetCurrentDisplay())
       && ((EGLContext )myGContext == eglGetCurrentContext())
       && ((EGLSurface )myWindow   == eglGetCurrentSurface (EGL_DRAW)));
#elif defined(_WIN32)
  if (myWindowDC == NULL || myGContext == NULL)
  {
    return Standard_False;
  }
  return (( (HDC )myWindowDC == wglGetCurrentDC())
      && ((HGLRC )myGContext == wglGetCurrentContext()));
#else
  if (myDisplay == NULL || myWindow == 0 || myGContext == 0)
  {
    return Standard_False;
  }

  return (   ((Display* )myDisplay  == glXGetCurrentDisplay())
       &&  ((GLXContext )myGContext == glXGetCurrentContext())
       && ((GLXDrawable )myWindow   == glXGetCurrentDrawable()));
#endif
}

// =======================================================================
// function : MakeCurrent
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::MakeCurrent()
{
#if defined(HAVE_EGL)
  if ((EGLDisplay )myDisplay  == EGL_NO_DISPLAY
   || (EGLSurface )myWindow   == EGL_NO_SURFACE
   || (EGLContext )myGContext == EGL_NO_CONTEXT)
  {
    Standard_ProgramError_Raise_if (myIsInitialized, "OpenGl_Context::Init() should be called before!");
    return Standard_False;
  }

  if (eglMakeCurrent ((EGLDisplay )myDisplay, (EGLSurface )myWindow, (EGLSurface )myWindow, (EGLContext )myGContext) != EGL_TRUE)
  {
    // if there is no current context it might be impossible to use glGetError() correctly
    PushMessage (GL_DEBUG_SOURCE_WINDOW_SYSTEM, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                 "eglMakeCurrent() has failed!");
    myIsInitialized = Standard_False;
    return Standard_False;
  }
#elif defined(_WIN32)
  if (myWindowDC == NULL || myGContext == NULL)
  {
    Standard_ProgramError_Raise_if (myIsInitialized, "OpenGl_Context::Init() should be called before!");
    return Standard_False;
  }

  // technically it should be safe to activate already bound GL context
  // however some drivers (Intel etc.) may FAIL doing this for unknown reason
  if (IsCurrent())
  {
    myShaderManager->SetContext (this);
    return Standard_True;
  }
  else if (wglMakeCurrent ((HDC )myWindowDC, (HGLRC )myGContext) != TRUE)
  {
    // notice that glGetError() couldn't be used here!
    wchar_t* aMsgBuff = NULL;
    DWORD anErrorCode = GetLastError();
    FormatMessageW (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, anErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (wchar_t* )&aMsgBuff, 0, NULL);
    TCollection_ExtendedString aMsg ("wglMakeCurrent() has failed. ");
    if (aMsgBuff != NULL)
    {
      aMsg += (Standard_ExtString )aMsgBuff;
      LocalFree (aMsgBuff);
    }
    PushMessage (GL_DEBUG_SOURCE_WINDOW_SYSTEM, GL_DEBUG_TYPE_ERROR, (unsigned int )anErrorCode, GL_DEBUG_SEVERITY_HIGH, aMsg);
    myIsInitialized = Standard_False;
    return Standard_False;
  }
#else
  if (myDisplay == NULL || myWindow == 0 || myGContext == 0)
  {
    Standard_ProgramError_Raise_if (myIsInitialized, "OpenGl_Context::Init() should be called before!");
    return Standard_False;
  }

  if (!glXMakeCurrent ((Display* )myDisplay, (GLXDrawable )myWindow, (GLXContext )myGContext))
  {
    // if there is no current context it might be impossible to use glGetError() correctly
    PushMessage (GL_DEBUG_SOURCE_WINDOW_SYSTEM, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                 "glXMakeCurrent() has failed!");
    myIsInitialized = Standard_False;
    return Standard_False;
  }
#endif
  myShaderManager->SetContext (this);
  return Standard_True;
}

// =======================================================================
// function : SwapBuffers
// purpose  :
// =======================================================================
void OpenGl_Context::SwapBuffers()
{
#if defined(HAVE_EGL)
  if ((EGLSurface )myWindow != EGL_NO_SURFACE)
  {
    eglSwapBuffers ((EGLDisplay )myDisplay, (EGLSurface )myWindow);
  }
#elif defined(_WIN32)
  if ((HDC )myWindowDC != NULL)
  {
    ::SwapBuffers ((HDC )myWindowDC);
    glFlush();
  }
#else
  if ((Display* )myDisplay != NULL)
  {
    glXSwapBuffers ((Display* )myDisplay, (GLXDrawable )myWindow);
  }
#endif
}

#endif // __APPLE__

// =======================================================================
// function : SetSwapInterval
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::SetSwapInterval (const Standard_Integer theInterval)
{
#if defined(HAVE_EGL)
  if (::eglSwapInterval ((EGLDisplay )myDisplay, theInterval) == EGL_TRUE)
  {
    return Standard_True;
  }
#elif defined(_WIN32)
  if (myFuncs->wglSwapIntervalEXT != NULL)
  {
    myFuncs->wglSwapIntervalEXT (theInterval);
    return Standard_True;
  }
#elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  //
#elif defined(__APPLE__)
  if (::CGLSetParameter (CGLGetCurrentContext(), kCGLCPSwapInterval, &theInterval) == kCGLNoError)
  {
    return Standard_True;
  }
#else
  if (theInterval == -1
   && myFuncs->glXSwapIntervalEXT != NULL)
  {
    typedef int (*glXSwapIntervalEXT_t_x)(Display* theDisplay, GLXDrawable theDrawable, int theInterval);
    glXSwapIntervalEXT_t_x aFuncPtr = (glXSwapIntervalEXT_t_x )myFuncs->glXSwapIntervalEXT;
    aFuncPtr ((Display* )myDisplay, (GLXDrawable )myWindow, theInterval);
    return Standard_True;
  }
  else if (myFuncs->glXSwapIntervalSGI != NULL)
  {
    myFuncs->glXSwapIntervalSGI (theInterval);
    return Standard_True;
  }
#endif
  return Standard_False;
}

// =======================================================================
// function : findProc
// purpose  :
// =======================================================================
void* OpenGl_Context::findProc (const char* theFuncName)
{
#if defined(HAVE_EGL)
  return (void* )eglGetProcAddress (theFuncName);
#elif defined(_WIN32)
  return (void* )wglGetProcAddress (theFuncName);
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  return (myGlLibHandle != NULL) ? dlsym (myGlLibHandle, theFuncName) : NULL;
#else
  return (void* )glXGetProcAddress ((const GLubyte* )theFuncName);
#endif
}

// =======================================================================
// function : CheckExtension
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::CheckExtension (const char* theExtName) const
{
  if (theExtName  == NULL)
  {
#ifdef OCCT_DEBUG
    std::cerr << "CheckExtension called with NULL string!\n";
#endif
    return Standard_False;
  }

#if !defined(GL_ES_VERSION_2_0)
  // available since OpenGL 3.0
  // and the ONLY way to check extensions with OpenGL 3.1+ core profile
  if (IsGlGreaterEqual (3, 0)
   && myFuncs->glGetStringi != NULL)
  {
    GLint anExtNb = 0;
    ::glGetIntegerv (GL_NUM_EXTENSIONS, &anExtNb);
    const size_t anExtNameLen = strlen (theExtName);
    for (GLint anIter = 0; anIter < anExtNb; ++anIter)
    {
      const char* anExtension = (const char* )myFuncs->glGetStringi (GL_EXTENSIONS, (GLuint )anIter);
      const size_t aTestExtNameLen = strlen (anExtension);
      if (aTestExtNameLen == anExtNameLen
       && strncmp (anExtension, theExtName, anExtNameLen) == 0)
      {
        return Standard_True;
      }
    }
    return Standard_False;
  }
#endif

  // use old way with huge string for all extensions
  const char* anExtString = (const char* )glGetString (GL_EXTENSIONS);
  if (anExtString == NULL)
  {
    Messenger()->Send ("TKOpenGL: glGetString (GL_EXTENSIONS) has returned NULL! No GL context?", Message_Warning);
    return Standard_False;
  }
  return CheckExtension (anExtString, theExtName);
}

// =======================================================================
// function : CheckExtension
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::CheckExtension (const char* theExtString,
                                                 const char* theExtName)
{
  if (theExtString == NULL)
  {
    return Standard_False;
  }

  // Search for theExtName in the extensions string.
  // Use of strstr() is not sufficient because extension names can be prefixes of other extension names.
  char* aPtrIter = (char* )theExtString;
  const char*  aPtrEnd      = aPtrIter + strlen (theExtString);
  const size_t anExtNameLen = strlen (theExtName);
  while (aPtrIter < aPtrEnd)
  {
    const size_t n = strcspn (aPtrIter, " ");
    if ((n == anExtNameLen) && (strncmp (aPtrIter, theExtName, anExtNameLen) == 0))
    {
      return Standard_True;
    }
    aPtrIter += (n + 1);
  }
  return Standard_False;
}

#if !defined(__APPLE__) || defined(MACOSX_USE_GLX)

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::Init (const Standard_Boolean theIsCoreProfile)
{
  if (myIsInitialized)
  {
    return Standard_True;
  }

#if defined(HAVE_EGL)
  myDisplay  = (Aspect_Display )eglGetCurrentDisplay();
  myGContext = (Aspect_RenderingContext )eglGetCurrentContext();
  myWindow   = (Aspect_Drawable )eglGetCurrentSurface(EGL_DRAW);
#elif defined(_WIN32)
  myWindowDC = (Aspect_Handle )wglGetCurrentDC();
  myGContext = (Aspect_RenderingContext )wglGetCurrentContext();
#else
  myDisplay  = (Aspect_Display )glXGetCurrentDisplay();
  myGContext = (Aspect_RenderingContext )glXGetCurrentContext();
  myWindow   = (Aspect_Drawable )glXGetCurrentDrawable();
#endif
  if (myGContext == NULL)
  {
    return Standard_False;
  }

  init (theIsCoreProfile);
  myIsInitialized = Standard_True;
  return Standard_True;
}

#endif // __APPLE__

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
#if defined(HAVE_EGL)
Standard_Boolean OpenGl_Context::Init (const Aspect_Drawable         theEglSurface,
                                       const Aspect_Display          theEglDisplay,
                                       const Aspect_RenderingContext theEglContext,
                                       const Standard_Boolean        theIsCoreProfile)
#elif defined(_WIN32)
Standard_Boolean OpenGl_Context::Init (const Aspect_Handle           theWindow,
                                       const Aspect_Handle           theWindowDC,
                                       const Aspect_RenderingContext theGContext,
                                       const Standard_Boolean        theIsCoreProfile)
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
Standard_Boolean OpenGl_Context::Init (EAGLContext*                  theGContext,
                                       const Standard_Boolean        theIsCoreProfile)
#else
Standard_Boolean OpenGl_Context::Init (NSOpenGLContext*              theGContext,
                                       const Standard_Boolean        theIsCoreProfile)
#endif

#else
Standard_Boolean OpenGl_Context::Init (const Aspect_Drawable         theWindow,
                                       const Aspect_Display          theDisplay,
                                       const Aspect_RenderingContext theGContext,
                                       const Standard_Boolean        theIsCoreProfile)
#endif
{
  Standard_ProgramError_Raise_if (myIsInitialized, "OpenGl_Context::Init() should be called only once!");
#if defined(HAVE_EGL)
  myWindow   = theEglSurface;
  myGContext = theEglContext;
  myDisplay  = theEglDisplay;
#elif defined(_WIN32)
  myWindow   = theWindow;
  myGContext = theGContext;
  myWindowDC = theWindowDC;
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  myGContext = theGContext;
#else
  myWindow   = theWindow;
  myGContext = theGContext;
  myDisplay  = theDisplay;
#endif
  if (myGContext == NULL || !MakeCurrent())
  {
    return Standard_False;
  }

  init (theIsCoreProfile);
  myIsInitialized = Standard_True;
  return Standard_True;
}

// =======================================================================
// function : ResetErrors
// purpose  :
// =======================================================================
bool OpenGl_Context::ResetErrors (const bool theToPrintErrors)
{
  int aPrevErr = 0;
  int anErr    = ::glGetError();
  const bool hasError = anErr != GL_NO_ERROR;
  if (!theToPrintErrors)
  {
    for (; anErr != GL_NO_ERROR && aPrevErr != anErr; aPrevErr = anErr, anErr = ::glGetError())
    {
      //
    }
    return hasError;
  }

  for (; anErr != GL_NO_ERROR && aPrevErr != anErr; aPrevErr = anErr, anErr = ::glGetError())
  {
    TCollection_ExtendedString anErrId;
    switch (anErr)
    {
      case GL_INVALID_ENUM:      anErrId = "GL_INVALID_ENUM";      break;
      case GL_INVALID_VALUE:     anErrId = "GL_INVALID_VALUE";     break;
      case GL_INVALID_OPERATION: anErrId = "GL_INVALID_OPERATION"; break;
    #ifdef GL_STACK_OVERFLOW
      case GL_STACK_OVERFLOW:    anErrId = "GL_STACK_OVERFLOW";    break;
      case GL_STACK_UNDERFLOW:   anErrId = "GL_STACK_UNDERFLOW";   break;
    #endif
      case GL_OUT_OF_MEMORY:     anErrId = "GL_OUT_OF_MEMORY";     break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        anErrId = "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
      default:
        anErrId = TCollection_ExtendedString("#") + anErr;
        break;
    }

    const TCollection_ExtendedString aMsg = TCollection_ExtendedString ("Unhandled GL error: ") + anErrId;
    PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER, 0, GL_DEBUG_SEVERITY_LOW, aMsg);
  }
  return hasError;
}

// =======================================================================
// function : ReadGlVersion
// purpose  :
// =======================================================================
void OpenGl_Context::ReadGlVersion (Standard_Integer& theGlVerMajor,
                                    Standard_Integer& theGlVerMinor)
{
  // reset values
  theGlVerMajor = 0;
  theGlVerMinor = 0;

#ifdef GL_MAJOR_VERSION
  // available since OpenGL 3.0 and OpenGL 3.0 ES
  GLint aMajor = 0, aMinor = 0;
  glGetIntegerv (GL_MAJOR_VERSION, &aMajor);
  glGetIntegerv (GL_MINOR_VERSION, &aMinor);
  // glGetError() sometimes does not report an error here even if
  // GL does not know GL_MAJOR_VERSION and GL_MINOR_VERSION constants.
  // This happens on some renderers like e.g. Cygwin MESA.
  // Thus checking additionally if GL has put anything to
  // the output variables.
  if (::glGetError() == GL_NO_ERROR && aMajor != 0 && aMinor != 0)
  {
    theGlVerMajor = aMajor;
    theGlVerMinor = aMinor;
    return;
  }
  for (GLenum anErr = ::glGetError(), aPrevErr = GL_NO_ERROR;; aPrevErr = anErr, anErr = ::glGetError())
  {
    if (anErr == GL_NO_ERROR
     || anErr == aPrevErr)
    {
      break;
    }
  }
#endif

  // Read version string.
  // Notice that only first two numbers split by point '2.1 XXXXX' are significant.
  // Following trash (after space) is vendor-specific.
  // New drivers also returns micro version of GL like '3.3.0' which has no meaning
  // and should be considered as vendor-specific too.
  const char* aVerStr = (const char* )glGetString (GL_VERSION);
  if (aVerStr == NULL || *aVerStr == '\0')
  {
    // invalid GL context
    return;
  }

//#if defined(GL_ES_VERSION_2_0)
  // skip "OpenGL ES-** " section
  for (; *aVerStr != '\0'; ++aVerStr)
  {
    if (*aVerStr >= '0' && *aVerStr <= '9')
    {
      break;
    }
  }
//#endif

  // parse string for major number
  char aMajorStr[32];
  char aMinorStr[32];
  size_t aMajIter = 0;
  while (aVerStr[aMajIter] >= '0' && aVerStr[aMajIter] <= '9')
  {
    ++aMajIter;
  }
  if (aMajIter == 0 || aMajIter >= sizeof(aMajorStr))
  {
    return;
  }
  memcpy (aMajorStr, aVerStr, aMajIter);
  aMajorStr[aMajIter] = '\0';

  // parse string for minor number
  aVerStr += aMajIter + 1;
  size_t aMinIter = 0;
  while (aVerStr[aMinIter] >= '0' && aVerStr[aMinIter] <= '9')
  {
    ++aMinIter;
  }
  if (aMinIter == 0 || aMinIter >= sizeof(aMinorStr))
  {
    return;
  }
  memcpy (aMinorStr, aVerStr, aMinIter);
  aMinorStr[aMinIter] = '\0';

  // read numbers
  theGlVerMajor = atoi (aMajorStr);
  theGlVerMinor = atoi (aMinorStr);

  if (theGlVerMajor <= 0)
  {
    theGlVerMajor = 0;
    theGlVerMinor = 0;
  }
}

static Standard_CString THE_DBGMSG_UNKNOWN = "UNKNOWN";
static Standard_CString THE_DBGMSG_SOURCES[] =
{
  ".OpenGL",    // GL_DEBUG_SOURCE_API
  ".WinSystem", // GL_DEBUG_SOURCE_WINDOW_SYSTEM
  ".GLSL",      // GL_DEBUG_SOURCE_SHADER_COMPILER
  ".3rdParty",  // GL_DEBUG_SOURCE_THIRD_PARTY
  "",           // GL_DEBUG_SOURCE_APPLICATION
  ".Other"      // GL_DEBUG_SOURCE_OTHER
};

static Standard_CString THE_DBGMSG_TYPES[] =
{
  "Error",           // GL_DEBUG_TYPE_ERROR
  "Deprecated",      // GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR
  "Undef. behavior", // GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR
  "Portability",     // GL_DEBUG_TYPE_PORTABILITY
  "Performance",     // GL_DEBUG_TYPE_PERFORMANCE
  "Other"            // GL_DEBUG_TYPE_OTHER
};

static Standard_CString THE_DBGMSG_SEV_HIGH   = "High";   // GL_DEBUG_SEVERITY_HIGH
static Standard_CString THE_DBGMSG_SEV_MEDIUM = "Medium"; // GL_DEBUG_SEVERITY_MEDIUM
static Standard_CString THE_DBGMSG_SEV_LOW    = "Low";    // GL_DEBUG_SEVERITY_LOW

//! Callback for GL_ARB_debug_output extension
static void APIENTRY debugCallbackWrap(unsigned int theSource,
                                       unsigned int theType,
                                       unsigned int theId,
                                       unsigned int theSeverity,
                                       int          /*theLength*/,
                                       const char*  theMessage,
                                       const void*  theUserParam)
{
  OpenGl_Context* aCtx = (OpenGl_Context* )theUserParam;
  aCtx->PushMessage (theSource, theType, theId, theSeverity, theMessage);
}

// =======================================================================
// function : PushMessage
// purpose  :
// =======================================================================
void OpenGl_Context::PushMessage (const unsigned int theSource,
                                  const unsigned int theType,
                                  const unsigned int theId,
                                  const unsigned int theSeverity,
                                  const TCollection_ExtendedString& theMessage)
{
  if (caps->suppressExtraMsg
   && theSource >= GL_DEBUG_SOURCE_API
   && theSource <= GL_DEBUG_SOURCE_OTHER
   && myFilters[theSource - GL_DEBUG_SOURCE_API].Contains (theId))
  {
    return;
  }

  Standard_CString& aSrc = (theSource >= GL_DEBUG_SOURCE_API
                        && theSource <= GL_DEBUG_SOURCE_OTHER)
                         ? THE_DBGMSG_SOURCES[theSource - GL_DEBUG_SOURCE_API]
                         : THE_DBGMSG_UNKNOWN;
  Standard_CString& aType = (theType >= GL_DEBUG_TYPE_ERROR
                         && theType <= GL_DEBUG_TYPE_OTHER)
                          ? THE_DBGMSG_TYPES[theType - GL_DEBUG_TYPE_ERROR]
                          : THE_DBGMSG_UNKNOWN;
  Standard_CString& aSev = theSeverity == GL_DEBUG_SEVERITY_HIGH
                         ? THE_DBGMSG_SEV_HIGH
                         : (theSeverity == GL_DEBUG_SEVERITY_MEDIUM
                          ? THE_DBGMSG_SEV_MEDIUM
                          : THE_DBGMSG_SEV_LOW);
  Message_Gravity aGrav = theSeverity == GL_DEBUG_SEVERITY_HIGH
                        ? Message_Alarm
                        : (theSeverity == GL_DEBUG_SEVERITY_MEDIUM
                         ? Message_Warning
                         : Message_Info);

  TCollection_ExtendedString aMsg;
  aMsg += "TKOpenGl"; aMsg += aSrc;
  aMsg += " | Type: ";        aMsg += aType;
  aMsg += " | ID: ";          aMsg += (Standard_Integer )theId;
  aMsg += " | Severity: ";    aMsg += aSev;
  aMsg += " | Message:\n  ";
  aMsg += theMessage;
  Messenger()->Send (aMsg, aGrav);
}

// =======================================================================
// function : ExcludeMessage
// purpose  :
// ======================================================================
Standard_Boolean OpenGl_Context::ExcludeMessage (const unsigned int theSource,
                                                 const unsigned int theId)
{
  return theSource >= GL_DEBUG_SOURCE_API
      && theSource <= GL_DEBUG_SOURCE_OTHER
      && myFilters[theSource - GL_DEBUG_SOURCE_API].Add (theId);
}

// =======================================================================
// function : IncludeMessage
// purpose  :
// ======================================================================
Standard_Boolean OpenGl_Context::IncludeMessage (const unsigned int theSource,
                                                 const unsigned int theId)
{
  return theSource >= GL_DEBUG_SOURCE_API
      && theSource <= GL_DEBUG_SOURCE_OTHER
      && myFilters[theSource - GL_DEBUG_SOURCE_API].Remove (theId);
}

// =======================================================================
// function : checkWrongVersion
// purpose  :
// ======================================================================
void OpenGl_Context::checkWrongVersion (const Standard_Integer theGlVerMajor,
                                        const Standard_Integer theGlVerMinor)
{
  if (!IsGlGreaterEqual (theGlVerMajor, theGlVerMinor))
  {
    return;
  }

  TCollection_ExtendedString aMsg = TCollection_ExtendedString()
    + "Error! OpenGL context reports version "
    + myGlVerMajor  + "." + myGlVerMinor
    + " but does not export required functions for "
    + theGlVerMajor + "." + theGlVerMinor;
  PushMessage (GL_DEBUG_SOURCE_APPLICATION,
               GL_DEBUG_TYPE_ERROR,
               0,
               GL_DEBUG_SEVERITY_HIGH,
               aMsg);
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
void OpenGl_Context::init (const Standard_Boolean theIsCoreProfile)
{
  // read version
  myGlVerMajor = 0;
  myGlVerMinor = 0;
  myMaxMsaaSamples = 0;
  myMaxDrawBuffers = 1;
  myMaxColorAttachments = 1;
  ReadGlVersion (myGlVerMajor, myGlVerMinor);
  myVendor = (const char* )::glGetString (GL_VENDOR);
  if (!caps->ffpEnable
   && !IsGlGreaterEqual (2, 0))
  {
    caps->ffpEnable = true;
    TCollection_ExtendedString aMsg =
      TCollection_ExtendedString("OpenGL driver is too old! Context info:\n")
                               + "    Vendor:   " + (const char* )::glGetString (GL_VENDOR)   + "\n"
                               + "    Renderer: " + (const char* )::glGetString (GL_RENDERER) + "\n"
                               + "    Version:  " + (const char* )::glGetString (GL_VERSION)  + "\n"
                               + "  Fallback using deprecated fixed-function pipeline.\n"
                               + "  Visualization might work incorrectly.\n"
                                 "  Consider upgrading the graphics driver.";
    PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_HIGH, aMsg);
  }

#if defined(GL_ES_VERSION_2_0)
  (void )theIsCoreProfile;
  const bool isCoreProfile = false;
#else

  if (myVendor.Search ("NVIDIA") != -1)
  {
    // Buffer detailed info: Buffer object 1 (bound to GL_ARRAY_BUFFER_ARB, usage hint is GL_STATIC_DRAW)
    // will use VIDEO memory as the source for buffer object operations.
    ExcludeMessage (GL_DEBUG_SOURCE_API, 131185);
  }
  if (IsGlGreaterEqual (3, 0))
  {
    // retrieve auxiliary function in advance
    FindProc ("glGetStringi", myFuncs->glGetStringi);
  }

  bool isCoreProfile = false;
  if (IsGlGreaterEqual (3, 2))
  {
    isCoreProfile = (theIsCoreProfile == Standard_True);

    // detect Core profile
    if (!isCoreProfile)
    {
      GLint aProfile = 0;
      ::glGetIntegerv (GL_CONTEXT_PROFILE_MASK, &aProfile);
      isCoreProfile = (aProfile & GL_CONTEXT_CORE_PROFILE_BIT) != 0;
    }
  }
#endif

  core11     = NULL;
  if (!isCoreProfile)
  {
    core11 = (OpenGl_GlCore11* )(&(*myFuncs));
  }
  core11fwd  = (OpenGl_GlCore11Fwd* )(&(*myFuncs));
  core15     = NULL;
  core15fwd  = NULL;
  core20     = NULL;
  core20fwd  = NULL;
  core32     = NULL;
  core32back = NULL;
  core33     = NULL;
  core33back = NULL;
  core41     = NULL;
  core41back = NULL;
  core42     = NULL;
  core42back = NULL;
  core43     = NULL;
  core43back = NULL;
  core44     = NULL;
  core44back = NULL;
  arbTBO     = NULL;
  arbTboRGB32 = Standard_False;
  arbIns     = NULL;
  arbDbg     = NULL;
  arbFBO     = NULL;
  arbFBOBlit = NULL;
  extGS      = NULL;
  myDefaultVao = 0;

  //! Make record shorter to retrieve function pointer using variable with same name
  #define FindProcShort(theFunc) FindProc(#theFunc, myFuncs->theFunc)

#if defined(GL_ES_VERSION_2_0)

  hasTexRGBA8 = IsGlGreaterEqual (3, 0)
             || CheckExtension ("GL_OES_rgb8_rgba8");
  // NPOT textures has limited support within OpenGL ES 2.0
  // which are relaxed by OpenGL ES 3.0 or some extensions
  //arbNPTW     = IsGlGreaterEqual (3, 0)
  //           || CheckExtension ("GL_OES_texture_npot")
  //           || CheckExtension ("GL_NV_texture_npot_2D_mipmap");
  arbNPTW     = Standard_True;
  arbTexRG    = IsGlGreaterEqual (3, 0)
             || CheckExtension ("GL_EXT_texture_rg");
  extBgra     = CheckExtension ("GL_EXT_texture_format_BGRA8888");
  extAnis = CheckExtension ("GL_EXT_texture_filter_anisotropic");
  extPDS  = IsGlGreaterEqual (3, 0)
         || CheckExtension ("GL_OES_packed_depth_stencil");

  core11fwd = (OpenGl_GlCore11Fwd* )(&(*myFuncs));
  if (IsGlGreaterEqual (2, 0))
  {
    // enable compatible functions
    core20    = (OpenGl_GlCore20*    )(&(*myFuncs));
    core20fwd = (OpenGl_GlCore20Fwd* )(&(*myFuncs));
    core15fwd = (OpenGl_GlCore15Fwd* )(&(*myFuncs));
    arbFBO    = (OpenGl_ArbFBO*      )(&(*myFuncs));
  }
  if (IsGlGreaterEqual (3, 0)
   && FindProcShort (glBlitFramebuffer))
  {
    arbFBOBlit = (OpenGl_ArbFBOBlit* )(&(*myFuncs));
  }
  if (IsGlGreaterEqual (3, 0)
   && FindProcShort (glGenSamplers)
   && FindProcShort (glDeleteSamplers)
   && FindProcShort (glIsSampler)
   && FindProcShort (glBindSampler)
   && FindProcShort (glSamplerParameteri)
   && FindProcShort (glSamplerParameteriv)
   && FindProcShort (glSamplerParameterf)
   && FindProcShort (glSamplerParameterfv)
   && FindProcShort (glGetSamplerParameteriv)
   && FindProcShort (glGetSamplerParameterfv))
   //&& FindProcShort (glSamplerParameterIiv) // only on Desktop or with extensions GL_OES_texture_border_clamp/GL_EXT_texture_border_clamp
   //&& FindProcShort (glSamplerParameterIuiv)
   //&& FindProcShort (glGetSamplerParameterIiv)
   //&& FindProcShort (glGetSamplerParameterIuiv))
  {
    arbSamplerObject = (OpenGl_ArbSamplerObject* )(&(*myFuncs));
  }
  extFragDepth = !IsGlGreaterEqual(3, 0)
               && CheckExtension ("GL_EXT_frag_depth");
  if (IsGlGreaterEqual (3, 1)
   && FindProc ("glTexStorage2DMultisample", myFuncs->glTexStorage2DMultisample))
  {
    // MSAA RenderBuffers have been defined in OpenGL ES 3.0,
    // but MSAA Textures - only in OpenGL ES 3.1+
    ::glGetIntegerv (GL_MAX_SAMPLES, &myMaxMsaaSamples);
  }

  hasUintIndex = IsGlGreaterEqual (3, 0)
              || CheckExtension ("GL_OES_element_index_uint");
  hasHighp     = CheckExtension ("GL_OES_fragment_precision_high");
  GLint aRange[2] = {0, 0};
  GLint aPrec     = 0;
  ::glGetShaderPrecisionFormat (GL_FRAGMENT_SHADER, GL_HIGH_FLOAT, aRange, &aPrec);
  if (aPrec != 0)
  {
    hasHighp = Standard_True;
  }

  arbTexFloat = IsGlGreaterEqual (3, 0)
             && FindProc ("glTexImage3D", myFuncs->glTexImage3D);

  const Standard_Boolean hasTexBuffer32  = IsGlGreaterEqual (3, 2) && FindProc ("glTexBuffer", myFuncs->glTexBuffer);
  const Standard_Boolean hasExtTexBuffer = CheckExtension ("GL_EXT_texture_buffer") && FindProc ("glTexBufferEXT", myFuncs->glTexBuffer);

  if (hasTexBuffer32 || hasExtTexBuffer)
  {
    arbTBO = reinterpret_cast<OpenGl_ArbTBO*> (myFuncs.get());
  }

  // initialize debug context extension
  if (CheckExtension ("GL_KHR_debug"))
  {
    // this functionality become a part of OpenGL ES 3.2
    arbDbg = NULL;
    // According to GL_KHR_debug spec, all functions should have KHR suffix.
    // However, some implementations can export these functions without suffix.
    if (FindProc ("glDebugMessageControlKHR",  myFuncs->glDebugMessageControl)
     && FindProc ("glDebugMessageInsertKHR",   myFuncs->glDebugMessageInsert)
     && FindProc ("glDebugMessageCallbackKHR", myFuncs->glDebugMessageCallback)
     && FindProc ("glGetDebugMessageLogKHR",   myFuncs->glGetDebugMessageLog))
    {
      arbDbg = (OpenGl_ArbDbg* )(&(*myFuncs));
    }

    if (arbDbg != NULL
     && caps->contextDebug)
    {
      // setup default callback
      myIsGlDebugCtx = Standard_True;
      arbDbg->glDebugMessageCallback (debugCallbackWrap, this);
      ::glEnable (GL_DEBUG_OUTPUT);
      if (caps->contextSyncDebug)
      {
        // note that some broken implementations (e.g. simulators) might generate error message on this call
        ::glEnable (GL_DEBUG_OUTPUT_SYNCHRONOUS);
      }
    }
  }

  extDrawBuffers = CheckExtension ("GL_EXT_draw_buffers") && FindProc ("glDrawBuffersEXT", myFuncs->glDrawBuffers);
  arbDrawBuffers = CheckExtension ("GL_ARB_draw_buffers") && FindProc ("glDrawBuffersARB", myFuncs->glDrawBuffers);

  if (IsGlGreaterEqual (3, 0) && FindProc ("glDrawBuffers", myFuncs->glDrawBuffers))
  {
    hasDrawBuffers = OpenGl_FeatureInCore;
  }
  else if (extDrawBuffers || arbDrawBuffers)
  {
    hasDrawBuffers = OpenGl_FeatureInExtensions;
  }

  hasFloatBuffer     = IsGlGreaterEqual (3, 2) ? OpenGl_FeatureInCore :
                       CheckExtension ("GL_EXT_color_buffer_float") ? OpenGl_FeatureInExtensions 
                                                                    : OpenGl_FeatureNotAvailable;
  hasHalfFloatBuffer = IsGlGreaterEqual (3, 2) ? OpenGl_FeatureInCore :
                       CheckExtension ("GL_EXT_color_buffer_half_float") ? OpenGl_FeatureInExtensions 
                                                                         : OpenGl_FeatureNotAvailable;

  oesSampleVariables = CheckExtension ("GL_OES_sample_variables");
  hasSampleVariables = IsGlGreaterEqual (3, 2) ? OpenGl_FeatureInCore :
                       oesSampleVariables ? OpenGl_FeatureInExtensions
                                          : OpenGl_FeatureNotAvailable;
#else

  myTexClamp = IsGlGreaterEqual (1, 2) ? GL_CLAMP_TO_EDGE : GL_CLAMP;

  hasTexRGBA8 = Standard_True;
  arbDrawBuffers   = CheckExtension ("GL_ARB_draw_buffers");
  arbNPTW          = CheckExtension ("GL_ARB_texture_non_power_of_two");
  arbTexFloat      = IsGlGreaterEqual (3, 0)
                  || CheckExtension ("GL_ARB_texture_float");
  arbSampleShading = CheckExtension ("GL_ARB_sample_shading");
  extBgra          = CheckExtension ("GL_EXT_bgra");
  extAnis          = CheckExtension ("GL_EXT_texture_filter_anisotropic");
  extPDS           = CheckExtension ("GL_EXT_packed_depth_stencil");
  atiMem           = CheckExtension ("GL_ATI_meminfo");
  nvxMem           = CheckExtension ("GL_NVX_gpu_memory_info");

  hasDrawBuffers = IsGlGreaterEqual (2, 0) ? OpenGl_FeatureInCore :
                   arbDrawBuffers ? OpenGl_FeatureInExtensions 
                                  : OpenGl_FeatureNotAvailable;

  hasFloatBuffer = hasHalfFloatBuffer =  IsGlGreaterEqual (3, 0) ? OpenGl_FeatureInCore :
                                         CheckExtension ("GL_ARB_color_buffer_float") ? OpenGl_FeatureInExtensions
                                                                                      : OpenGl_FeatureNotAvailable;

  hasSampleVariables = IsGlGreaterEqual (4, 0) ? OpenGl_FeatureInCore :
                        arbSampleShading ? OpenGl_FeatureInExtensions
                                         : OpenGl_FeatureNotAvailable;

  GLint aStereo = GL_FALSE;
  glGetIntegerv (GL_STEREO, &aStereo);
  myIsStereoBuffers = aStereo == 1;

  // get number of maximum clipping planes
  glGetIntegerv (GL_MAX_CLIP_PLANES,  &myMaxClipPlanes);
#endif

  if (hasDrawBuffers)
  {
    glGetIntegerv (GL_MAX_DRAW_BUFFERS,      &myMaxDrawBuffers);
    glGetIntegerv (GL_MAX_COLOR_ATTACHMENTS, &myMaxColorAttachments);
  }

  glGetIntegerv (GL_MAX_TEXTURE_SIZE, &myMaxTexDim);
  if (IsGlGreaterEqual (1, 5))
  {
    glGetIntegerv (GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &myMaxTexCombined);
  }

  if (extAnis)
  {
    glGetIntegerv (GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &myAnisoMax);
  }

  myClippingState.Init (myMaxClipPlanes);

#if !defined(GL_ES_VERSION_2_0)

  bool has12 = false;
  bool has13 = false;
  bool has14 = false;
  bool has15 = false;
  bool has20 = false;
  bool has21 = false;
  bool has30 = false;
  bool has31 = false;
  bool has32 = false;
  bool has33 = false;
  bool has40 = false;
  bool has41 = false;
  bool has42 = false;
  bool has43 = false;
  bool has44 = false;

  // retrieve platform-dependent extensions
#if defined(HAVE_EGL)
  //
#elif defined(_WIN32)
  if (FindProcShort (wglGetExtensionsStringARB))
  {
    const char* aWglExts = myFuncs->wglGetExtensionsStringARB (wglGetCurrentDC());
    if (CheckExtension (aWglExts, "WGL_EXT_swap_control"))
    {
      FindProcShort (wglSwapIntervalEXT);
    }
    if (CheckExtension (aWglExts, "WGL_ARB_pixel_format"))
    {
      FindProcShort (wglChoosePixelFormatARB);
    }
    if (CheckExtension (aWglExts, "WGL_ARB_create_context_profile"))
    {
      FindProcShort (wglCreateContextAttribsARB);
    }
    if (CheckExtension (aWglExts, "WGL_NV_DX_interop"))
    {
      FindProcShort (wglDXSetResourceShareHandleNV);
      FindProcShort (wglDXOpenDeviceNV);
      FindProcShort (wglDXCloseDeviceNV);
      FindProcShort (wglDXRegisterObjectNV);
      FindProcShort (wglDXUnregisterObjectNV);
      FindProcShort (wglDXObjectAccessNV);
      FindProcShort (wglDXLockObjectsNV);
      FindProcShort (wglDXUnlockObjectsNV);
    }
    if (CheckExtension (aWglExts, "WGL_AMD_gpu_association"))
    {
      FindProcShort (wglGetGPUIDsAMD);
      FindProcShort (wglGetGPUInfoAMD);
      FindProcShort (wglGetContextGPUIDAMD);
    }
  }
#elif defined(__APPLE__)
    //
#else
    const char* aGlxExts = ::glXQueryExtensionsString ((Display* )myDisplay, DefaultScreen ((Display* )myDisplay));
    if (CheckExtension (aGlxExts, "GLX_EXT_swap_control"))
    {
      FindProcShort (glXSwapIntervalEXT);
    }
    if (CheckExtension (aGlxExts, "GLX_SGI_swap_control"))
    {
      FindProcShort (glXSwapIntervalSGI);
    }
    if (CheckExtension (aGlxExts, "GLX_MESA_query_renderer"))
    {
      FindProcShort (glXQueryRendererIntegerMESA);
      FindProcShort (glXQueryCurrentRendererIntegerMESA);
      FindProcShort (glXQueryRendererStringMESA);
      FindProcShort (glXQueryCurrentRendererStringMESA);
    }
    //extSwapTear = CheckExtension (aGlxExts, "GLX_EXT_swap_control_tear");
#endif

  // load OpenGL 1.2 new functions
  has12 = IsGlGreaterEqual (1, 2)
       && FindProcShort (glBlendColor)
       && FindProcShort (glBlendEquation)
       && FindProcShort (glDrawRangeElements)
       && FindProcShort (glTexImage3D)
       && FindProcShort (glTexSubImage3D)
       && FindProcShort (glCopyTexSubImage3D);

  // load OpenGL 1.3 new functions
  has13 = IsGlGreaterEqual (1, 3)
       && FindProcShort (glActiveTexture)
       && FindProcShort (glSampleCoverage)
       && FindProcShort (glCompressedTexImage3D)
       && FindProcShort (glCompressedTexImage2D)
       && FindProcShort (glCompressedTexImage1D)
       && FindProcShort (glCompressedTexSubImage3D)
       && FindProcShort (glCompressedTexSubImage2D)
       && FindProcShort (glCompressedTexSubImage1D)
       && FindProcShort (glGetCompressedTexImage);

  if (!isCoreProfile)
  {
    has13 = has13
       && FindProcShort (glClientActiveTexture)
       && FindProcShort (glMultiTexCoord1d)
       && FindProcShort (glMultiTexCoord1dv)
       && FindProcShort (glMultiTexCoord1f)
       && FindProcShort (glMultiTexCoord1fv)
       && FindProcShort (glMultiTexCoord1i)
       && FindProcShort (glMultiTexCoord1iv)
       && FindProcShort (glMultiTexCoord1s)
       && FindProcShort (glMultiTexCoord1sv)
       && FindProcShort (glMultiTexCoord2d)
       && FindProcShort (glMultiTexCoord2dv)
       && FindProcShort (glMultiTexCoord2f)
       && FindProcShort (glMultiTexCoord2fv)
       && FindProcShort (glMultiTexCoord2i)
       && FindProcShort (glMultiTexCoord2iv)
       && FindProcShort (glMultiTexCoord2s)
       && FindProcShort (glMultiTexCoord2sv)
       && FindProcShort (glMultiTexCoord3d)
       && FindProcShort (glMultiTexCoord3dv)
       && FindProcShort (glMultiTexCoord3f)
       && FindProcShort (glMultiTexCoord3fv)
       && FindProcShort (glMultiTexCoord3i)
       && FindProcShort (glMultiTexCoord3iv)
       && FindProcShort (glMultiTexCoord3s)
       && FindProcShort (glMultiTexCoord3sv)
       && FindProcShort (glMultiTexCoord4d)
       && FindProcShort (glMultiTexCoord4dv)
       && FindProcShort (glMultiTexCoord4f)
       && FindProcShort (glMultiTexCoord4fv)
       && FindProcShort (glMultiTexCoord4i)
       && FindProcShort (glMultiTexCoord4iv)
       && FindProcShort (glMultiTexCoord4s)
       && FindProcShort (glMultiTexCoord4sv)
       && FindProcShort (glLoadTransposeMatrixf)
       && FindProcShort (glLoadTransposeMatrixd)
       && FindProcShort (glMultTransposeMatrixf)
       && FindProcShort (glMultTransposeMatrixd);
  }

  // load OpenGL 1.4 new functions
  has14 = IsGlGreaterEqual (1, 4)
       && FindProcShort (glBlendFuncSeparate)
       && FindProcShort (glMultiDrawArrays)
       && FindProcShort (glMultiDrawElements)
       && FindProcShort (glPointParameterf)
       && FindProcShort (glPointParameterfv)
       && FindProcShort (glPointParameteri)
       && FindProcShort (glPointParameteriv);

  // load OpenGL 1.5 new functions
  has15 = IsGlGreaterEqual (1, 5)
       && FindProcShort (glGenQueries)
       && FindProcShort (glDeleteQueries)
       && FindProcShort (glIsQuery)
       && FindProcShort (glBeginQuery)
       && FindProcShort (glEndQuery)
       && FindProcShort (glGetQueryiv)
       && FindProcShort (glGetQueryObjectiv)
       && FindProcShort (glGetQueryObjectuiv)
       && FindProcShort (glBindBuffer)
       && FindProcShort (glDeleteBuffers)
       && FindProcShort (glGenBuffers)
       && FindProcShort (glIsBuffer)
       && FindProcShort (glBufferData)
       && FindProcShort (glBufferSubData)
       && FindProcShort (glGetBufferSubData)
       && FindProcShort (glMapBuffer)
       && FindProcShort (glUnmapBuffer)
       && FindProcShort (glGetBufferParameteriv)
       && FindProcShort (glGetBufferPointerv);

  // load OpenGL 2.0 new functions
  has20 = IsGlGreaterEqual (2, 0)
       && FindProcShort (glBlendEquationSeparate)
       && FindProcShort (glDrawBuffers)
       && FindProcShort (glStencilOpSeparate)
       && FindProcShort (glStencilFuncSeparate)
       && FindProcShort (glStencilMaskSeparate)
       && FindProcShort (glAttachShader)
       && FindProcShort (glBindAttribLocation)
       && FindProcShort (glCompileShader)
       && FindProcShort (glCreateProgram)
       && FindProcShort (glCreateShader)
       && FindProcShort (glDeleteProgram)
       && FindProcShort (glDeleteShader)
       && FindProcShort (glDetachShader)
       && FindProcShort (glDisableVertexAttribArray)
       && FindProcShort (glEnableVertexAttribArray)
       && FindProcShort (glGetActiveAttrib)
       && FindProcShort (glGetActiveUniform)
       && FindProcShort (glGetAttachedShaders)
       && FindProcShort (glGetAttribLocation)
       && FindProcShort (glGetProgramiv)
       && FindProcShort (glGetProgramInfoLog)
       && FindProcShort (glGetShaderiv)
       && FindProcShort (glGetShaderInfoLog)
       && FindProcShort (glGetShaderSource)
       && FindProcShort (glGetUniformLocation)
       && FindProcShort (glGetUniformfv)
       && FindProcShort (glGetUniformiv)
       && FindProcShort (glGetVertexAttribdv)
       && FindProcShort (glGetVertexAttribfv)
       && FindProcShort (glGetVertexAttribiv)
       && FindProcShort (glGetVertexAttribPointerv)
       && FindProcShort (glIsProgram)
       && FindProcShort (glIsShader)
       && FindProcShort (glLinkProgram)
       && FindProcShort (glShaderSource)
       && FindProcShort (glUseProgram)
       && FindProcShort (glUniform1f)
       && FindProcShort (glUniform2f)
       && FindProcShort (glUniform3f)
       && FindProcShort (glUniform4f)
       && FindProcShort (glUniform1i)
       && FindProcShort (glUniform2i)
       && FindProcShort (glUniform3i)
       && FindProcShort (glUniform4i)
       && FindProcShort (glUniform1fv)
       && FindProcShort (glUniform2fv)
       && FindProcShort (glUniform3fv)
       && FindProcShort (glUniform4fv)
       && FindProcShort (glUniform1iv)
       && FindProcShort (glUniform2iv)
       && FindProcShort (glUniform3iv)
       && FindProcShort (glUniform4iv)
       && FindProcShort (glUniformMatrix2fv)
       && FindProcShort (glUniformMatrix3fv)
       && FindProcShort (glUniformMatrix4fv)
       && FindProcShort (glValidateProgram)
       && FindProcShort (glVertexAttrib1d)
       && FindProcShort (glVertexAttrib1dv)
       && FindProcShort (glVertexAttrib1f)
       && FindProcShort (glVertexAttrib1fv)
       && FindProcShort (glVertexAttrib1s)
       && FindProcShort (glVertexAttrib1sv)
       && FindProcShort (glVertexAttrib2d)
       && FindProcShort (glVertexAttrib2dv)
       && FindProcShort (glVertexAttrib2f)
       && FindProcShort (glVertexAttrib2fv)
       && FindProcShort (glVertexAttrib2s)
       && FindProcShort (glVertexAttrib2sv)
       && FindProcShort (glVertexAttrib3d)
       && FindProcShort (glVertexAttrib3dv)
       && FindProcShort (glVertexAttrib3f)
       && FindProcShort (glVertexAttrib3fv)
       && FindProcShort (glVertexAttrib3s)
       && FindProcShort (glVertexAttrib3sv)
       && FindProcShort (glVertexAttrib4Nbv)
       && FindProcShort (glVertexAttrib4Niv)
       && FindProcShort (glVertexAttrib4Nsv)
       && FindProcShort (glVertexAttrib4Nub)
       && FindProcShort (glVertexAttrib4Nubv)
       && FindProcShort (glVertexAttrib4Nuiv)
       && FindProcShort (glVertexAttrib4Nusv)
       && FindProcShort (glVertexAttrib4bv)
       && FindProcShort (glVertexAttrib4d)
       && FindProcShort (glVertexAttrib4dv)
       && FindProcShort (glVertexAttrib4f)
       && FindProcShort (glVertexAttrib4fv)
       && FindProcShort (glVertexAttrib4iv)
       && FindProcShort (glVertexAttrib4s)
       && FindProcShort (glVertexAttrib4sv)
       && FindProcShort (glVertexAttrib4ubv)
       && FindProcShort (glVertexAttrib4uiv)
       && FindProcShort (glVertexAttrib4usv)
       && FindProcShort (glVertexAttribPointer);

  // load OpenGL 2.1 new functions
  has21 = IsGlGreaterEqual (2, 1)
       && FindProcShort (glUniformMatrix2x3fv)
       && FindProcShort (glUniformMatrix3x2fv)
       && FindProcShort (glUniformMatrix2x4fv)
       && FindProcShort (glUniformMatrix4x2fv)
       && FindProcShort (glUniformMatrix3x4fv)
       && FindProcShort (glUniformMatrix4x3fv);

  // load GL_ARB_framebuffer_object (added to OpenGL 3.0 core)
  const bool hasFBO = (IsGlGreaterEqual (3, 0) || CheckExtension ("GL_ARB_framebuffer_object"))
       && FindProcShort (glIsRenderbuffer)
       && FindProcShort (glBindRenderbuffer)
       && FindProcShort (glDeleteRenderbuffers)
       && FindProcShort (glGenRenderbuffers)
       && FindProcShort (glRenderbufferStorage)
       && FindProcShort (glGetRenderbufferParameteriv)
       && FindProcShort (glIsFramebuffer)
       && FindProcShort (glBindFramebuffer)
       && FindProcShort (glDeleteFramebuffers)
       && FindProcShort (glGenFramebuffers)
       && FindProcShort (glCheckFramebufferStatus)
       && FindProcShort (glFramebufferTexture1D)
       && FindProcShort (glFramebufferTexture2D)
       && FindProcShort (glFramebufferTexture3D)
       && FindProcShort (glFramebufferRenderbuffer)
       && FindProcShort (glGetFramebufferAttachmentParameteriv)
       && FindProcShort (glGenerateMipmap)
       && FindProcShort (glBlitFramebuffer)
       && FindProcShort (glRenderbufferStorageMultisample)
       && FindProcShort (glFramebufferTextureLayer);

  // load GL_ARB_vertex_array_object (added to OpenGL 3.0 core)
  const bool hasVAO = (IsGlGreaterEqual (3, 0) || CheckExtension ("GL_ARB_vertex_array_object"))
       && FindProcShort (glBindVertexArray)
       && FindProcShort (glDeleteVertexArrays)
       && FindProcShort (glGenVertexArrays)
       && FindProcShort (glIsVertexArray);

  // load GL_ARB_map_buffer_range (added to OpenGL 3.0 core)
  const bool hasMapBufferRange = (IsGlGreaterEqual (3, 0) || CheckExtension ("GL_ARB_map_buffer_range"))
       && FindProcShort (glMapBufferRange)
       && FindProcShort (glFlushMappedBufferRange);

  // load OpenGL 3.0 new functions
  has30 = IsGlGreaterEqual (3, 0)
       && hasFBO
       && hasVAO
       && hasMapBufferRange
       && FindProcShort (glColorMaski)
       && FindProcShort (glGetBooleani_v)
       && FindProcShort (glGetIntegeri_v)
       && FindProcShort (glEnablei)
       && FindProcShort (glDisablei)
       && FindProcShort (glIsEnabledi)
       && FindProcShort (glBeginTransformFeedback)
       && FindProcShort (glEndTransformFeedback)
       && FindProcShort (glBindBufferRange)
       && FindProcShort (glBindBufferBase)
       && FindProcShort (glTransformFeedbackVaryings)
       && FindProcShort (glGetTransformFeedbackVarying)
       && FindProcShort (glClampColor)
       && FindProcShort (glBeginConditionalRender)
       && FindProcShort (glEndConditionalRender)
       && FindProcShort (glVertexAttribIPointer)
       && FindProcShort (glGetVertexAttribIiv)
       && FindProcShort (glGetVertexAttribIuiv)
       && FindProcShort (glVertexAttribI1i)
       && FindProcShort (glVertexAttribI2i)
       && FindProcShort (glVertexAttribI3i)
       && FindProcShort (glVertexAttribI4i)
       && FindProcShort (glVertexAttribI1ui)
       && FindProcShort (glVertexAttribI2ui)
       && FindProcShort (glVertexAttribI3ui)
       && FindProcShort (glVertexAttribI4ui)
       && FindProcShort (glVertexAttribI1iv)
       && FindProcShort (glVertexAttribI2iv)
       && FindProcShort (glVertexAttribI3iv)
       && FindProcShort (glVertexAttribI4iv)
       && FindProcShort (glVertexAttribI1uiv)
       && FindProcShort (glVertexAttribI2uiv)
       && FindProcShort (glVertexAttribI3uiv)
       && FindProcShort (glVertexAttribI4uiv)
       && FindProcShort (glVertexAttribI4bv)
       && FindProcShort (glVertexAttribI4sv)
       && FindProcShort (glVertexAttribI4ubv)
       && FindProcShort (glVertexAttribI4usv)
       && FindProcShort (glGetUniformuiv)
       && FindProcShort (glBindFragDataLocation)
       && FindProcShort (glGetFragDataLocation)
       && FindProcShort (glUniform1ui)
       && FindProcShort (glUniform2ui)
       && FindProcShort (glUniform3ui)
       && FindProcShort (glUniform4ui)
       && FindProcShort (glUniform1uiv)
       && FindProcShort (glUniform2uiv)
       && FindProcShort (glUniform3uiv)
       && FindProcShort (glUniform4uiv)
       && FindProcShort (glTexParameterIiv)
       && FindProcShort (glTexParameterIuiv)
       && FindProcShort (glGetTexParameterIiv)
       && FindProcShort (glGetTexParameterIuiv)
       && FindProcShort (glClearBufferiv)
       && FindProcShort (glClearBufferuiv)
       && FindProcShort (glClearBufferfv)
       && FindProcShort (glClearBufferfi)
       && FindProcShort (glGetStringi);

  // load GL_ARB_uniform_buffer_object (added to OpenGL 3.1 core)
  const bool hasUBO = (IsGlGreaterEqual (3, 1) || CheckExtension ("GL_ARB_uniform_buffer_object"))
       && FindProcShort (glGetUniformIndices)
       && FindProcShort (glGetActiveUniformsiv)
       && FindProcShort (glGetActiveUniformName)
       && FindProcShort (glGetUniformBlockIndex)
       && FindProcShort (glGetActiveUniformBlockiv)
       && FindProcShort (glGetActiveUniformBlockName)
       && FindProcShort (glUniformBlockBinding);

  // load GL_ARB_copy_buffer (added to OpenGL 3.1 core)
  const bool hasCopyBufSubData = (IsGlGreaterEqual (3, 1) || CheckExtension ("GL_ARB_copy_buffer"))
       && FindProcShort (glCopyBufferSubData);

  if (has30)
  {
    // NPOT textures are required by OpenGL 2.0 specifications
    // but doesn't hardware accelerated by some ancient OpenGL 2.1 hardware (GeForce FX, RadeOn 9700 etc.)
    arbNPTW  = Standard_True;
    arbTexRG = Standard_True;
  }

  // load OpenGL 3.1 new functions
  has31 = IsGlGreaterEqual (3, 1)
       && hasUBO
       && hasCopyBufSubData
       && FindProcShort (glDrawArraysInstanced)
       && FindProcShort (glDrawElementsInstanced)
       && FindProcShort (glTexBuffer)
       && FindProcShort (glPrimitiveRestartIndex);

  // load GL_ARB_draw_elements_base_vertex (added to OpenGL 3.2 core)
  const bool hasDrawElemsBaseVert = (IsGlGreaterEqual (3, 2) || CheckExtension ("GL_ARB_draw_elements_base_vertex"))
       && FindProcShort (glDrawElementsBaseVertex)
       && FindProcShort (glDrawRangeElementsBaseVertex)
       && FindProcShort (glDrawElementsInstancedBaseVertex)
       && FindProcShort (glMultiDrawElementsBaseVertex);

  // load GL_ARB_provoking_vertex (added to OpenGL 3.2 core)
  const bool hasProvokingVert = (IsGlGreaterEqual (3, 2) || CheckExtension ("GL_ARB_provoking_vertex"))
       && FindProcShort (glProvokingVertex);

  // load GL_ARB_sync (added to OpenGL 3.2 core)
  const bool hasSync = (IsGlGreaterEqual (3, 2) || CheckExtension ("GL_ARB_sync"))
       && FindProcShort (glFenceSync)
       && FindProcShort (glIsSync)
       && FindProcShort (glDeleteSync)
       && FindProcShort (glClientWaitSync)
       && FindProcShort (glWaitSync)
       && FindProcShort (glGetInteger64v)
       && FindProcShort (glGetSynciv);

  // load GL_ARB_texture_multisample (added to OpenGL 3.2 core)
  const bool hasTextureMultisample = (IsGlGreaterEqual (3, 2) || CheckExtension ("GL_ARB_texture_multisample"))
       && FindProcShort (glTexImage2DMultisample)
       && FindProcShort (glTexImage3DMultisample)
       && FindProcShort (glGetMultisamplefv)
       && FindProcShort (glSampleMaski);

  // load OpenGL 3.2 new functions
  has32 = IsGlGreaterEqual (3, 2)
       && hasDrawElemsBaseVert
       && hasProvokingVert
       && hasSync
       && hasTextureMultisample
       && FindProcShort (glGetInteger64i_v)
       && FindProcShort (glGetBufferParameteri64v)
       && FindProcShort (glFramebufferTexture);

  // load GL_ARB_blend_func_extended (added to OpenGL 3.3 core)
  const bool hasBlendFuncExtended = (IsGlGreaterEqual (3, 3) || CheckExtension ("GL_ARB_blend_func_extended"))
       && FindProcShort (glBindFragDataLocationIndexed)
       && FindProcShort (glGetFragDataIndex);

  // load GL_ARB_sampler_objects (added to OpenGL 3.3 core)
  const bool hasSamplerObjects = (IsGlGreaterEqual (3, 3) || CheckExtension ("GL_ARB_sampler_objects"))
       && FindProcShort (glGenSamplers)
       && FindProcShort (glDeleteSamplers)
       && FindProcShort (glIsSampler)
       && FindProcShort (glBindSampler)
       && FindProcShort (glSamplerParameteri)
       && FindProcShort (glSamplerParameteriv)
       && FindProcShort (glSamplerParameterf)
       && FindProcShort (glSamplerParameterfv)
       && FindProcShort (glSamplerParameterIiv)
       && FindProcShort (glSamplerParameterIuiv)
       && FindProcShort (glGetSamplerParameteriv)
       && FindProcShort (glGetSamplerParameterIiv)
       && FindProcShort (glGetSamplerParameterfv)
       && FindProcShort (glGetSamplerParameterIuiv);
  if (hasSamplerObjects)
  {
    arbSamplerObject = (OpenGl_ArbSamplerObject* )(&(*myFuncs));
  }

  // load GL_ARB_timer_query (added to OpenGL 3.3 core)
  const bool hasTimerQuery = (IsGlGreaterEqual (3, 3) || CheckExtension ("GL_ARB_timer_query"))
       && FindProcShort (glQueryCounter)
       && FindProcShort (glGetQueryObjecti64v)
       && FindProcShort (glGetQueryObjectui64v);

  // load GL_ARB_vertex_type_2_10_10_10_rev (added to OpenGL 3.3 core)
  const bool hasVertType21010101rev = (IsGlGreaterEqual (3, 3) || CheckExtension ("GL_ARB_vertex_type_2_10_10_10_rev"))
       && FindProcShort (glVertexAttribP1ui)
       && FindProcShort (glVertexAttribP1uiv)
       && FindProcShort (glVertexAttribP2ui)
       && FindProcShort (glVertexAttribP2uiv)
       && FindProcShort (glVertexAttribP3ui)
       && FindProcShort (glVertexAttribP3uiv)
       && FindProcShort (glVertexAttribP4ui)
       && FindProcShort (glVertexAttribP4uiv);

  if ( hasVertType21010101rev
   && !isCoreProfile)
  {
    // load deprecated functions
    const bool hasVertType21010101revExt =
          FindProcShort (glVertexP2ui)
       && FindProcShort (glVertexP2uiv)
       && FindProcShort (glVertexP3ui)
       && FindProcShort (glVertexP3uiv)
       && FindProcShort (glVertexP4ui)
       && FindProcShort (glVertexP4uiv)
       && FindProcShort (glTexCoordP1ui)
       && FindProcShort (glTexCoordP1uiv)
       && FindProcShort (glTexCoordP2ui)
       && FindProcShort (glTexCoordP2uiv)
       && FindProcShort (glTexCoordP3ui)
       && FindProcShort (glTexCoordP3uiv)
       && FindProcShort (glTexCoordP4ui)
       && FindProcShort (glTexCoordP4uiv)
       && FindProcShort (glMultiTexCoordP1ui)
       && FindProcShort (glMultiTexCoordP1uiv)
       && FindProcShort (glMultiTexCoordP2ui)
       && FindProcShort (glMultiTexCoordP2uiv)
       && FindProcShort (glMultiTexCoordP3ui)
       && FindProcShort (glMultiTexCoordP3uiv)
       && FindProcShort (glMultiTexCoordP4ui)
       && FindProcShort (glMultiTexCoordP4uiv)
       && FindProcShort (glNormalP3ui)
       && FindProcShort (glNormalP3uiv)
       && FindProcShort (glColorP3ui)
       && FindProcShort (glColorP3uiv)
       && FindProcShort (glColorP4ui)
       && FindProcShort (glColorP4uiv)
       && FindProcShort (glSecondaryColorP3ui)
       && FindProcShort (glSecondaryColorP3uiv);
    (void )hasVertType21010101revExt;
  }

  // load OpenGL 3.3 extra functions
  has33 = IsGlGreaterEqual (3, 3)
       && hasBlendFuncExtended
       && hasSamplerObjects
       && hasTimerQuery
       && hasVertType21010101rev
       && FindProcShort (glVertexAttribDivisor);

  // load GL_ARB_draw_indirect (added to OpenGL 4.0 core)
  const bool hasDrawIndirect = (IsGlGreaterEqual (4, 0) || CheckExtension ("GL_ARB_draw_indirect"))
       && FindProcShort (glDrawArraysIndirect)
       && FindProcShort (glDrawElementsIndirect);

  // load GL_ARB_gpu_shader_fp64 (added to OpenGL 4.0 core)
  const bool hasShaderFP64 = (IsGlGreaterEqual (4, 0) || CheckExtension ("GL_ARB_gpu_shader_fp64"))
       && FindProcShort (glUniform1d)
       && FindProcShort (glUniform2d)
       && FindProcShort (glUniform3d)
       && FindProcShort (glUniform4d)
       && FindProcShort (glUniform1dv)
       && FindProcShort (glUniform2dv)
       && FindProcShort (glUniform3dv)
       && FindProcShort (glUniform4dv)
       && FindProcShort (glUniformMatrix2dv)
       && FindProcShort (glUniformMatrix3dv)
       && FindProcShort (glUniformMatrix4dv)
       && FindProcShort (glUniformMatrix2x3dv)
       && FindProcShort (glUniformMatrix2x4dv)
       && FindProcShort (glUniformMatrix3x2dv)
       && FindProcShort (glUniformMatrix3x4dv)
       && FindProcShort (glUniformMatrix4x2dv)
       && FindProcShort (glUniformMatrix4x3dv)
       && FindProcShort (glGetUniformdv);

  // load GL_ARB_shader_subroutine (added to OpenGL 4.0 core)
  const bool hasShaderSubroutine = (IsGlGreaterEqual (4, 0) || CheckExtension ("GL_ARB_shader_subroutine"))
       && FindProcShort (glGetSubroutineUniformLocation)
       && FindProcShort (glGetSubroutineIndex)
       && FindProcShort (glGetActiveSubroutineUniformiv)
       && FindProcShort (glGetActiveSubroutineUniformName)
       && FindProcShort (glGetActiveSubroutineName)
       && FindProcShort (glUniformSubroutinesuiv)
       && FindProcShort (glGetUniformSubroutineuiv)
       && FindProcShort (glGetProgramStageiv);

  // load GL_ARB_tessellation_shader (added to OpenGL 4.0 core)
  const bool hasTessellationShader = (IsGlGreaterEqual (4, 0) || CheckExtension ("GL_ARB_tessellation_shader"))
       && FindProcShort (glPatchParameteri)
       && FindProcShort (glPatchParameterfv);

  // load GL_ARB_transform_feedback2 (added to OpenGL 4.0 core)
  const bool hasTrsfFeedback2 = (IsGlGreaterEqual (4, 0) || CheckExtension ("GL_ARB_transform_feedback2"))
       && FindProcShort (glBindTransformFeedback)
       && FindProcShort (glDeleteTransformFeedbacks)
       && FindProcShort (glGenTransformFeedbacks)
       && FindProcShort (glIsTransformFeedback)
       && FindProcShort (glPauseTransformFeedback)
       && FindProcShort (glResumeTransformFeedback)
       && FindProcShort (glDrawTransformFeedback);

  // load GL_ARB_transform_feedback3 (added to OpenGL 4.0 core)
  const bool hasTrsfFeedback3 = (IsGlGreaterEqual (4, 0) || CheckExtension ("GL_ARB_transform_feedback3"))
       && FindProcShort (glDrawTransformFeedbackStream)
       && FindProcShort (glBeginQueryIndexed)
       && FindProcShort (glEndQueryIndexed)
       && FindProcShort (glGetQueryIndexediv);

  // load OpenGL 4.0 new functions
  has40 = IsGlGreaterEqual (4, 0)
      && hasDrawIndirect
      && hasShaderFP64
      && hasShaderSubroutine
      && hasTessellationShader
      && hasTrsfFeedback2
      && hasTrsfFeedback3
      && FindProcShort (glMinSampleShading)
      && FindProcShort (glBlendEquationi)
      && FindProcShort (glBlendEquationSeparatei)
      && FindProcShort (glBlendFunci)
      && FindProcShort (glBlendFuncSeparatei);

  // load GL_ARB_ES2_compatibility (added to OpenGL 4.1 core)
  const bool hasES2Compatibility = (IsGlGreaterEqual (4, 1) || CheckExtension ("GL_ARB_ES2_compatibility"))
       && FindProcShort (glReleaseShaderCompiler)
       && FindProcShort (glShaderBinary)
       && FindProcShort (glGetShaderPrecisionFormat)
       && FindProcShort (glDepthRangef)
       && FindProcShort (glClearDepthf);

  // load GL_ARB_get_program_binary (added to OpenGL 4.1 core)
  const bool hasGetProgramBinary = (IsGlGreaterEqual (4, 1) || CheckExtension ("GL_ARB_get_program_binary"))
       && FindProcShort (glGetProgramBinary)
       && FindProcShort (glProgramBinary)
       && FindProcShort (glProgramParameteri);


  // load GL_ARB_separate_shader_objects (added to OpenGL 4.1 core)
  const bool hasSeparateShaderObjects = (IsGlGreaterEqual (4, 1) || CheckExtension ("GL_ARB_separate_shader_objects"))
       && FindProcShort (glUseProgramStages)
       && FindProcShort (glActiveShaderProgram)
       && FindProcShort (glCreateShaderProgramv)
       && FindProcShort (glBindProgramPipeline)
       && FindProcShort (glDeleteProgramPipelines)
       && FindProcShort (glGenProgramPipelines)
       && FindProcShort (glIsProgramPipeline)
       && FindProcShort (glGetProgramPipelineiv)
       && FindProcShort (glProgramUniform1i)
       && FindProcShort (glProgramUniform1iv)
       && FindProcShort (glProgramUniform1f)
       && FindProcShort (glProgramUniform1fv)
       && FindProcShort (glProgramUniform1d)
       && FindProcShort (glProgramUniform1dv)
       && FindProcShort (glProgramUniform1ui)
       && FindProcShort (glProgramUniform1uiv)
       && FindProcShort (glProgramUniform2i)
       && FindProcShort (glProgramUniform2iv)
       && FindProcShort (glProgramUniform2f)
       && FindProcShort (glProgramUniform2fv)
       && FindProcShort (glProgramUniform2d)
       && FindProcShort (glProgramUniform2dv)
       && FindProcShort (glProgramUniform2ui)
       && FindProcShort (glProgramUniform2uiv)
       && FindProcShort (glProgramUniform3i)
       && FindProcShort (glProgramUniform3iv)
       && FindProcShort (glProgramUniform3f)
       && FindProcShort (glProgramUniform3fv)
       && FindProcShort (glProgramUniform3d)
       && FindProcShort (glProgramUniform3dv)
       && FindProcShort (glProgramUniform3ui)
       && FindProcShort (glProgramUniform3uiv)
       && FindProcShort (glProgramUniform4i)
       && FindProcShort (glProgramUniform4iv)
       && FindProcShort (glProgramUniform4f)
       && FindProcShort (glProgramUniform4fv)
       && FindProcShort (glProgramUniform4d)
       && FindProcShort (glProgramUniform4dv)
       && FindProcShort (glProgramUniform4ui)
       && FindProcShort (glProgramUniform4uiv)
       && FindProcShort (glProgramUniformMatrix2fv)
       && FindProcShort (glProgramUniformMatrix3fv)
       && FindProcShort (glProgramUniformMatrix4fv)
       && FindProcShort (glProgramUniformMatrix2dv)
       && FindProcShort (glProgramUniformMatrix3dv)
       && FindProcShort (glProgramUniformMatrix4dv)
       && FindProcShort (glProgramUniformMatrix2x3fv)
       && FindProcShort (glProgramUniformMatrix3x2fv)
       && FindProcShort (glProgramUniformMatrix2x4fv)
       && FindProcShort (glProgramUniformMatrix4x2fv)
       && FindProcShort (glProgramUniformMatrix3x4fv)
       && FindProcShort (glProgramUniformMatrix4x3fv)
       && FindProcShort (glProgramUniformMatrix2x3dv)
       && FindProcShort (glProgramUniformMatrix3x2dv)
       && FindProcShort (glProgramUniformMatrix2x4dv)
       && FindProcShort (glProgramUniformMatrix4x2dv)
       && FindProcShort (glProgramUniformMatrix3x4dv)
       && FindProcShort (glProgramUniformMatrix4x3dv)
       && FindProcShort (glValidateProgramPipeline)
       && FindProcShort (glGetProgramPipelineInfoLog);

  // load GL_ARB_vertex_attrib_64bit (added to OpenGL 4.1 core)
  const bool hasVertAttrib64bit = (IsGlGreaterEqual (4, 1) || CheckExtension ("GL_ARB_vertex_attrib_64bit"))
       && FindProcShort (glVertexAttribL1d)
       && FindProcShort (glVertexAttribL2d)
       && FindProcShort (glVertexAttribL3d)
       && FindProcShort (glVertexAttribL4d)
       && FindProcShort (glVertexAttribL1dv)
       && FindProcShort (glVertexAttribL2dv)
       && FindProcShort (glVertexAttribL3dv)
       && FindProcShort (glVertexAttribL4dv)
       && FindProcShort (glVertexAttribLPointer)
       && FindProcShort (glGetVertexAttribLdv);

  // load GL_ARB_viewport_array (added to OpenGL 4.1 core)
  const bool hasViewportArray = (IsGlGreaterEqual (4, 1) || CheckExtension ("GL_ARB_viewport_array"))
       && FindProcShort (glViewportArrayv)
       && FindProcShort (glViewportIndexedf)
       && FindProcShort (glViewportIndexedfv)
       && FindProcShort (glScissorArrayv)
       && FindProcShort (glScissorIndexed)
       && FindProcShort (glScissorIndexedv)
       && FindProcShort (glDepthRangeArrayv)
       && FindProcShort (glDepthRangeIndexed)
       && FindProcShort (glGetFloati_v)
       && FindProcShort (glGetDoublei_v);

  has41 = IsGlGreaterEqual (4, 1)
       && hasES2Compatibility
       && hasGetProgramBinary
       && hasSeparateShaderObjects
       && hasVertAttrib64bit
       && hasViewportArray;

  // load GL_ARB_base_instance (added to OpenGL 4.2 core)
  const bool hasBaseInstance = (IsGlGreaterEqual (4, 2) || CheckExtension ("GL_ARB_base_instance"))
       && FindProcShort (glDrawArraysInstancedBaseInstance)
       && FindProcShort (glDrawElementsInstancedBaseInstance)
       && FindProcShort (glDrawElementsInstancedBaseVertexBaseInstance);

  // load GL_ARB_transform_feedback_instanced (added to OpenGL 4.2 core)
  const bool hasTrsfFeedbackInstanced = (IsGlGreaterEqual (4, 2) || CheckExtension ("GL_ARB_transform_feedback_instanced"))
       && FindProcShort (glDrawTransformFeedbackInstanced)
       && FindProcShort (glDrawTransformFeedbackStreamInstanced);

  // load GL_ARB_internalformat_query (added to OpenGL 4.2 core)
  const bool hasInternalFormatQuery = (IsGlGreaterEqual (4, 2) || CheckExtension ("GL_ARB_internalformat_query"))
       && FindProcShort (glGetInternalformativ);

  // load GL_ARB_shader_atomic_counters (added to OpenGL 4.2 core)
  const bool hasShaderAtomicCounters = (IsGlGreaterEqual (4, 2) || CheckExtension ("GL_ARB_shader_atomic_counters"))
       && FindProcShort (glGetActiveAtomicCounterBufferiv);

  // load GL_ARB_shader_image_load_store (added to OpenGL 4.2 core)
  const bool hasShaderImgLoadStore = (IsGlGreaterEqual (4, 2) || CheckExtension ("GL_ARB_shader_image_load_store"))
       && FindProcShort (glBindImageTexture)
       && FindProcShort (glMemoryBarrier);

  // load GL_ARB_texture_storage (added to OpenGL 4.2 core)
  const bool hasTextureStorage = (IsGlGreaterEqual (4, 2) || CheckExtension ("GL_ARB_texture_storage"))
       && FindProcShort (glTexStorage1D)
       && FindProcShort (glTexStorage2D)
       && FindProcShort (glTexStorage3D);

  has42 = IsGlGreaterEqual (4, 2)
       && hasBaseInstance
       && hasTrsfFeedbackInstanced
       && hasInternalFormatQuery
       && hasShaderAtomicCounters
       && hasShaderImgLoadStore
       && hasTextureStorage;

  has43 = IsGlGreaterEqual (4, 3)
       && FindProcShort (glClearBufferData)
       && FindProcShort (glClearBufferSubData)
       && FindProcShort (glDispatchCompute)
       && FindProcShort (glDispatchComputeIndirect)
       && FindProcShort (glCopyImageSubData)
       && FindProcShort (glFramebufferParameteri)
       && FindProcShort (glGetFramebufferParameteriv)
       && FindProcShort (glGetInternalformati64v)
       && FindProcShort (glInvalidateTexSubImage)
       && FindProcShort (glInvalidateTexImage)
       && FindProcShort (glInvalidateBufferSubData)
       && FindProcShort (glInvalidateBufferData)
       && FindProcShort (glInvalidateFramebuffer)
       && FindProcShort (glInvalidateSubFramebuffer)
       && FindProcShort (glMultiDrawArraysIndirect)
       && FindProcShort (glMultiDrawElementsIndirect)
       && FindProcShort (glGetProgramInterfaceiv)
       && FindProcShort (glGetProgramResourceIndex)
       && FindProcShort (glGetProgramResourceName)
       && FindProcShort (glGetProgramResourceiv)
       && FindProcShort (glGetProgramResourceLocation)
       && FindProcShort (glGetProgramResourceLocationIndex)
       && FindProcShort (glShaderStorageBlockBinding)
       && FindProcShort (glTexBufferRange)
       && FindProcShort (glTexStorage2DMultisample)
       && FindProcShort (glTexStorage3DMultisample)
       && FindProcShort (glTextureView)
       && FindProcShort (glBindVertexBuffer)
       && FindProcShort (glVertexAttribFormat)
       && FindProcShort (glVertexAttribIFormat)
       && FindProcShort (glVertexAttribLFormat)
       && FindProcShort (glVertexAttribBinding)
       && FindProcShort (glVertexBindingDivisor)
       && FindProcShort (glDebugMessageControl)
       && FindProcShort (glDebugMessageInsert)
       && FindProcShort (glDebugMessageCallback)
       && FindProcShort (glGetDebugMessageLog)
       && FindProcShort (glPushDebugGroup)
       && FindProcShort (glPopDebugGroup)
       && FindProcShort (glObjectLabel)
       && FindProcShort (glGetObjectLabel)
       && FindProcShort (glObjectPtrLabel)
       && FindProcShort (glGetObjectPtrLabel);

  // load GL_ARB_clear_texture (added to OpenGL 4.4 core)
  bool arbTexClear = (IsGlGreaterEqual (4, 4) || CheckExtension ("GL_ARB_clear_texture"))
       && FindProcShort (glClearTexImage)
       && FindProcShort (glClearTexSubImage);

  has44 = IsGlGreaterEqual (4, 4)
       && arbTexClear
       && FindProcShort (glBufferStorage)
       && FindProcShort (glBindBuffersBase)
       && FindProcShort (glBindBuffersRange)
       && FindProcShort (glBindTextures)
       && FindProcShort (glBindSamplers)
       && FindProcShort (glBindImageTextures)
       && FindProcShort (glBindVertexBuffers);

  // initialize debug context extension
  if (CheckExtension ("GL_ARB_debug_output"))
  {
    arbDbg = NULL;
    if (has43)
    {
      arbDbg = (OpenGl_ArbDbg* )(&(*myFuncs));
    }
    else if (FindProc ("glDebugMessageControlARB",  myFuncs->glDebugMessageControl)
          && FindProc ("glDebugMessageInsertARB",   myFuncs->glDebugMessageInsert)
          && FindProc ("glDebugMessageCallbackARB", myFuncs->glDebugMessageCallback)
          && FindProc ("glGetDebugMessageLogARB",   myFuncs->glGetDebugMessageLog))
    {
      arbDbg = (OpenGl_ArbDbg* )(&(*myFuncs));
    }

    if (arbDbg != NULL
     && caps->contextDebug)
    {
      // setup default callback
      myIsGlDebugCtx = Standard_True;
      arbDbg->glDebugMessageCallback (debugCallbackWrap, this);
      if (has43)
      {
        ::glEnable (GL_DEBUG_OUTPUT);
      }
      if (caps->contextSyncDebug)
      {
        ::glEnable (GL_DEBUG_OUTPUT_SYNCHRONOUS);
      }
    }
  }

  // initialize TBO extension (ARB)
  if (!has31
   && CheckExtension ("GL_ARB_texture_buffer_object")
   && FindProc ("glTexBufferARB", myFuncs->glTexBuffer))
  {
    arbTBO = (OpenGl_ArbTBO* )(&(*myFuncs));
  }
  arbTboRGB32 = CheckExtension ("GL_ARB_texture_buffer_object_rgb32");

  // initialize hardware instancing extension (ARB)
  if (!has31
   && CheckExtension ("GL_ARB_draw_instanced")
   && FindProc ("glDrawArraysInstancedARB",   myFuncs->glDrawArraysInstanced)
   && FindProc ("glDrawElementsInstancedARB", myFuncs->glDrawElementsInstanced))
  {
    arbIns = (OpenGl_ArbIns* )(&(*myFuncs));
  }

  // initialize FBO extension (ARB)
  if (hasFBO)
  {
    arbFBO     = (OpenGl_ArbFBO*     )(&(*myFuncs));
    arbFBOBlit = (OpenGl_ArbFBOBlit* )(&(*myFuncs));
    extPDS = Standard_True; // extension for EXT, but part of ARB
  }

  // initialize GS extension (EXT)
  if (CheckExtension ("GL_EXT_geometry_shader4")
   && FindProcShort (glProgramParameteriEXT))
  {
    extGS = (OpenGl_ExtGS* )(&(*myFuncs));
  }

  // initialize bindless texture extension (ARB)
  if (CheckExtension ("GL_ARB_bindless_texture")
   && FindProcShort (glGetTextureHandleARB)
   && FindProcShort (glGetTextureSamplerHandleARB)
   && FindProcShort (glMakeTextureHandleResidentARB)
   && FindProcShort (glMakeTextureHandleNonResidentARB)
   && FindProcShort (glGetImageHandleARB)
   && FindProcShort (glMakeImageHandleResidentARB)
   && FindProcShort (glMakeImageHandleNonResidentARB)
   && FindProcShort (glUniformHandleui64ARB)
   && FindProcShort (glUniformHandleui64vARB)
   && FindProcShort (glProgramUniformHandleui64ARB)
   && FindProcShort (glProgramUniformHandleui64vARB)
   && FindProcShort (glIsTextureHandleResidentARB)
   && FindProcShort (glIsImageHandleResidentARB)
   && FindProcShort (glVertexAttribL1ui64ARB)
   && FindProcShort (glVertexAttribL1ui64vARB)
   && FindProcShort (glGetVertexAttribLui64vARB))
  {
    arbTexBindless = (OpenGl_ArbTexBindless* )(&(*myFuncs));
  }

  if (!has12)
  {
    checkWrongVersion (1, 2);
    myGlVerMajor = 1;
    myGlVerMinor = 1;
    return;
  }
  else if (!has13)
  {
    checkWrongVersion (1, 3);
    myGlVerMajor = 1;
    myGlVerMinor = 2;
    return;
  }
  else if (!has14)
  {
    checkWrongVersion (1, 4);
    myGlVerMajor = 1;
    myGlVerMinor = 3;
    return;
  }
  else if (!has15)
  {
    checkWrongVersion (1, 5);
    myGlVerMajor = 1;
    myGlVerMinor = 4;
    return;
  }
  if (!isCoreProfile)
  {
    core15 = (OpenGl_GlCore15* )(&(*myFuncs));
  }
  core15fwd = (OpenGl_GlCore15Fwd* )(&(*myFuncs));

  if (!has20)
  {
    checkWrongVersion (2, 0);
    myGlVerMajor = 1;
    myGlVerMinor = 5;
    return;
  }

  const char* aGlslVer = (const char* )::glGetString (GL_SHADING_LANGUAGE_VERSION);
  if (aGlslVer == NULL
  || *aGlslVer == '\0')
  {
    // broken context has been detected
    TCollection_ExtendedString aMsg = TCollection_ExtendedString()
      + "Error! OpenGL context reports version "
      + myGlVerMajor  + "." + myGlVerMinor
      + " but reports wrong GLSL version";
    PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                 GL_DEBUG_TYPE_ERROR,
                 0,
                 GL_DEBUG_SEVERITY_HIGH,
                 aMsg);
    myGlVerMajor = 1;
    myGlVerMinor = 5;
    return;
  }

  if (!isCoreProfile)
  {
    core20  = (OpenGl_GlCore20*    )(&(*myFuncs));
  }
  core20fwd = (OpenGl_GlCore20Fwd* )(&(*myFuncs));

  if (!has21)
  {
    checkWrongVersion (2, 1);
    myGlVerMajor = 2;
    myGlVerMinor = 0;
    return;
  }

  if (!has30)
  {
    checkWrongVersion (3, 0);
    myGlVerMajor = 2;
    myGlVerMinor = 1;
    return;
  }

  // MSAA RenderBuffers have been defined in OpenGL 3.0,
  // but MSAA Textures - only in OpenGL 3.2+
  if (!has32
   && CheckExtension ("GL_ARB_texture_multisample")
   && FindProcShort (glTexImage2DMultisample))
  {
    GLint aNbColorSamples = 0, aNbDepthSamples = 0;
    ::glGetIntegerv (GL_MAX_COLOR_TEXTURE_SAMPLES, &aNbColorSamples);
    ::glGetIntegerv (GL_MAX_DEPTH_TEXTURE_SAMPLES, &aNbDepthSamples);
    myMaxMsaaSamples = Min (aNbColorSamples, aNbDepthSamples);
  }
  if (!has43
   && CheckExtension ("GL_ARB_texture_storage_multisample")
   && FindProcShort (glTexStorage2DMultisample))
  {
    //
  }

  if (!has31)
  {
    checkWrongVersion (3, 1);
    myGlVerMajor = 3;
    myGlVerMinor = 0;
    return;
  }
  arbTBO = (OpenGl_ArbTBO* )(&(*myFuncs));
  arbIns = (OpenGl_ArbIns* )(&(*myFuncs));

  // check whether ray tracing mode is supported
  myHasRayTracing = has31
                 && arbTboRGB32
                 && arbFBOBlit  != NULL;

  // check whether textures in ray tracing mode are supported
  myHasRayTracingTextures = myHasRayTracing
                         && arbTexBindless != NULL;

  // check whether adaptive screen sampling in ray tracing mode is supported
  myHasRayTracingAdaptiveSampling = myHasRayTracing
                                 && has44
                                 && CheckExtension ("GL_NV_shader_atomic_float");

  if (!has32)
  {
    checkWrongVersion (3, 2);
    myGlVerMajor = 3;
    myGlVerMinor = 1;
    return;
  }
  core32 = (OpenGl_GlCore32* )(&(*myFuncs));
  if (isCoreProfile)
  {
    core32->glGenVertexArrays (1, &myDefaultVao);
  }
  else
  {
    core32back = (OpenGl_GlCore32Back* )(&(*myFuncs));
  }
  ::glGetIntegerv (GL_MAX_SAMPLES, &myMaxMsaaSamples);

  if (!has33)
  {
    checkWrongVersion (3, 3);
    myGlVerMajor = 3;
    myGlVerMinor = 2;
    return;
  }
  core33 = (OpenGl_GlCore33* )(&(*myFuncs));
  if (!isCoreProfile)
  {
    core33back = (OpenGl_GlCore33Back* )(&(*myFuncs));
  }

  if (!has40)
  {
    checkWrongVersion (4, 0);
    myGlVerMajor = 3;
    myGlVerMinor = 3;
    return;
  }
  arbTboRGB32 = Standard_True; // in core since OpenGL 4.0

  if (!has41)
  {
    checkWrongVersion (4, 1);
    myGlVerMajor = 4;
    myGlVerMinor = 0;
    return;
  }
  core41 = (OpenGl_GlCore41* )(&(*myFuncs));
  if (!isCoreProfile)
  {
    core41back = (OpenGl_GlCore41Back* )(&(*myFuncs));
  }

  if(!has42)
  {
    checkWrongVersion (4, 2);
    myGlVerMajor = 4;
    myGlVerMinor = 1;
    return;
  }
  core42 = (OpenGl_GlCore42* )(&(*myFuncs));
  if (!isCoreProfile)
  {
    core42back = (OpenGl_GlCore42Back* )(&(*myFuncs));
  }

  if (!has43)
  {
    checkWrongVersion (4, 3);
    myGlVerMajor = 4;
    myGlVerMinor = 2;
    return;
  }
  core43 = (OpenGl_GlCore43* )(&(*myFuncs));
  if (!isCoreProfile)
  {
    core43back = (OpenGl_GlCore43Back* )(&(*myFuncs));
  }

  if (!has44)
  {
    checkWrongVersion (4, 4);
    myGlVerMajor = 4;
    myGlVerMinor = 3;
    return;
  }
  core44 = (OpenGl_GlCore44* )(&(*myFuncs));
  if (!isCoreProfile)
  {
    core44back = (OpenGl_GlCore44Back* )(&(*myFuncs));
  }
#endif
}

// =======================================================================
// function : MemoryInfo
// purpose  :
// =======================================================================
Standard_Size OpenGl_Context::AvailableMemory() const
{
#if !defined(GL_ES_VERSION_2_0)
  if (atiMem)
  {
    // this is actually information for VBO pool
    // however because pools are mostly shared
    // it can be used for total GPU memory estimations
    GLint aMemInfo[4];
    aMemInfo[0] = 0;
    glGetIntegerv (GL_VBO_FREE_MEMORY_ATI, aMemInfo);
    // returned value is in KiB, however this maybe changed in future
    return Standard_Size(aMemInfo[0]) * 1024;
  }
  else if (nvxMem)
  {
    // current available dedicated video memory (in KiB), currently unused GPU memory
    GLint aMemInfo = 0;
    glGetIntegerv (GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &aMemInfo);
    return Standard_Size(aMemInfo) * 1024;
  }
#endif
  return 0;
}

// =======================================================================
// function : MemoryInfo
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_Context::MemoryInfo() const
{
  TColStd_IndexedDataMapOfStringString aDict;
  MemoryInfo (aDict);

  TCollection_AsciiString aText;
  for (TColStd_IndexedDataMapOfStringString::Iterator anIter (aDict); anIter.More(); anIter.Next())
  {
    if (!aText.IsEmpty())
    {
      aText += "\n";
    }
    aText += TCollection_AsciiString("  ") + anIter.Key() + ": " + anIter.Value();
  }
  return aText;
}

// =======================================================================
// function : MemoryInfo
// purpose  :
// =======================================================================
void OpenGl_Context::MemoryInfo (TColStd_IndexedDataMapOfStringString& theDict) const
{
#if defined(GL_ES_VERSION_2_0)
  (void )theDict;
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  GLint aGlRendId = 0;
  CGLGetParameter (CGLGetCurrentContext(), kCGLCPCurrentRendererID, &aGlRendId);

  CGLRendererInfoObj  aRendObj = NULL;
  CGOpenGLDisplayMask aDispMask = CGDisplayIDToOpenGLDisplayMask (kCGDirectMainDisplay);
  GLint aRendNb = 0;
  CGLQueryRendererInfo (aDispMask, &aRendObj, &aRendNb);
  for (GLint aRendIter = 0; aRendIter < aRendNb; ++aRendIter)
  {
    GLint aRendId = 0;
    if (CGLDescribeRenderer (aRendObj, aRendIter, kCGLRPRendererID, &aRendId) != kCGLNoError
     || aRendId != aGlRendId)
    {
      continue;
    }

    //kCGLRPVideoMemoryMegabytes   = 131;
    //kCGLRPTextureMemoryMegabytes = 132;
    GLint aVMem = 0;
  #if MAC_OS_X_VERSION_MIN_REQUIRED >= 1070
    if (CGLDescribeRenderer(aRendObj, aRendIter, kCGLRPVideoMemoryMegabytes, &aVMem) == kCGLNoError)
    {
      addInfo (theDict, "GPU memory",         TCollection_AsciiString() + aVMem + " MiB");
    }
    if (CGLDescribeRenderer(aRendObj, aRendIter, kCGLRPTextureMemoryMegabytes, &aVMem) == kCGLNoError)
    {
      addInfo (theDict, "GPU Texture memory", TCollection_AsciiString() + aVMem + " MiB");
    }
  #else
    if (CGLDescribeRenderer(aRendObj, aRendIter, kCGLRPVideoMemory, &aVMem) == kCGLNoError)
    {
      addInfo (theDict, "GPU memory",         TCollection_AsciiString() + (aVMem / (1024 * 1024)) + " MiB");
    }
    if (CGLDescribeRenderer(aRendObj, aRendIter, kCGLRPTextureMemory, &aVMem) == kCGLNoError)
    {
      addInfo (theDict, "GPU Texture memory", TCollection_AsciiString() + (aVMem / (1024 * 1024)) + " MiB");
    }
  #endif
  }
#endif

#if !defined(GL_ES_VERSION_2_0)
  if (atiMem)
  {
    GLint aValues[4];
    memset (aValues, 0, sizeof(aValues));
    glGetIntegerv (GL_VBO_FREE_MEMORY_ATI, aValues);

    // total memory free in the pool
    addInfo (theDict, "GPU free memory",    TCollection_AsciiString() + (aValues[0] / 1024) + " MiB");

    if (aValues[1] != aValues[0])
    {
      // largest available free block in the pool
      addInfo (theDict, "Largest free block", TCollection_AsciiString() + (aValues[1] / 1024) + " MiB");
    }
    if (aValues[2] != aValues[0])
    {
      // total auxiliary memory free
      addInfo (theDict, "Free auxiliary memory", TCollection_AsciiString() + (aValues[2] / 1024) + " MiB");
    }
  }
  else if (nvxMem)
  {
    //current available dedicated video memory (in KiB), currently unused GPU memory
    GLint aValue = 0;
    glGetIntegerv (GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &aValue);
    addInfo (theDict, "GPU free memory", TCollection_AsciiString() + (aValue / 1024) + " MiB");

    // dedicated video memory, total size (in KiB) of the GPU memory
    GLint aDedicated = 0;
    glGetIntegerv (GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &aDedicated);
    addInfo (theDict, "GPU memory", TCollection_AsciiString() + (aDedicated / 1024) + " MiB");

    // total available memory, total size (in KiB) of the memory available for allocations
    glGetIntegerv (GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &aValue);
    if (aValue != aDedicated)
    {
      // different only for special configurations
      addInfo (theDict, "Total memory", TCollection_AsciiString() + (aValue / 1024) + " MiB");
    }
  }
#if defined(_WIN32)
  else if (myFuncs->wglGetGPUInfoAMD != NULL
        && myFuncs->wglGetContextGPUIDAMD != NULL)
  {
    GLuint aTotalMemMiB = 0;
    UINT anAmdId = myFuncs->wglGetContextGPUIDAMD ((HGLRC )myGContext);
    if (anAmdId != 0)
    {
      if (myFuncs->wglGetGPUInfoAMD (anAmdId, WGL_GPU_RAM_AMD, GL_UNSIGNED_INT, sizeof(aTotalMemMiB), &aTotalMemMiB) > 0)
      {
        addInfo (theDict, "GPU memory", TCollection_AsciiString() + (int )aTotalMemMiB + " MiB");
      }
    }
  }
#endif
#endif

#if !defined(GL_ES_VERSION_2_0) && !defined(__APPLE__) && !defined(_WIN32)
  // GLX_RENDERER_VENDOR_ID_MESA
  if (myFuncs->glXQueryCurrentRendererIntegerMESA != NULL)
  {
    unsigned int aVMemMiB = 0;
    if (myFuncs->glXQueryCurrentRendererIntegerMESA (GLX_RENDERER_VIDEO_MEMORY_MESA, &aVMemMiB) != False)
    {
      addInfo (theDict, "GPU memory", TCollection_AsciiString() + int(aVMemMiB) + " MiB");
    }
  }
#endif
}

// =======================================================================
// function : DiagnosticInfo
// purpose  :
// =======================================================================
void OpenGl_Context::DiagnosticInformation (TColStd_IndexedDataMapOfStringString& theDict,
                                            Graphic3d_DiagnosticInfo theFlags) const
{
  if ((theFlags & Graphic3d_DiagnosticInfo_NativePlatform) != 0)
  {
  #if defined(HAVE_EGL)
    addInfo (theDict, "EGLVersion",    ::eglQueryString ((Aspect_Display)myDisplay, EGL_VERSION));
    addInfo (theDict, "EGLVendor",     ::eglQueryString ((Aspect_Display)myDisplay, EGL_VENDOR));
    addInfo (theDict, "EGLClientAPIs", ::eglQueryString ((Aspect_Display)myDisplay, EGL_CLIENT_APIS));
    if ((theFlags & Graphic3d_DiagnosticInfo_Extensions) != 0)
    {
      addInfo (theDict, "EGLExtensions", ::eglQueryString ((Aspect_Display)myDisplay, EGL_EXTENSIONS));
    }
  #elif defined(_WIN32)
    if ((theFlags & Graphic3d_DiagnosticInfo_Extensions) != 0
     && myFuncs->wglGetExtensionsStringARB != NULL)
    {
      const char* aWglExts = myFuncs->wglGetExtensionsStringARB ((HDC )myWindowDC);
      addInfo (theDict, "WGLExtensions", aWglExts);
    }
  #elif defined(__APPLE__)
    //
  #else
    Display* aDisplay = (Display*)myDisplay;
    const int aScreen = DefaultScreen(aDisplay);
    addInfo (theDict, "GLXDirectRendering", ::glXIsDirect (aDisplay, (GLXContext )myGContext) ? "Yes" : "No");
    addInfo (theDict, "GLXVendor",  ::glXQueryServerString (aDisplay, aScreen, GLX_VENDOR));
    addInfo (theDict, "GLXVersion", ::glXQueryServerString (aDisplay, aScreen, GLX_VERSION));
    if ((theFlags & Graphic3d_DiagnosticInfo_Extensions) != 0)
    {
      const char* aGlxExts = ::glXQueryExtensionsString (aDisplay, aScreen);
      addInfo(theDict, "GLXExtensions", aGlxExts);
    }

    addInfo (theDict, "GLXClientVendor",  ::glXGetClientString (aDisplay, GLX_VENDOR));
    addInfo (theDict, "GLXClientVersion", ::glXGetClientString (aDisplay, GLX_VERSION));
    if ((theFlags & Graphic3d_DiagnosticInfo_Extensions) != 0)
    {
      addInfo (theDict, "GLXClientExtensions", ::glXGetClientString (aDisplay, GLX_EXTENSIONS));
    }
  #endif
  }

  if ((theFlags & Graphic3d_DiagnosticInfo_Device) != 0)
  {
    addInfo (theDict, "GLvendor",    (const char*)::glGetString (GL_VENDOR));
    addInfo (theDict, "GLdevice",    (const char*)::glGetString (GL_RENDERER));
    addInfo (theDict, "GLversion",   (const char*)::glGetString (GL_VERSION));
    addInfo (theDict, "GLSLversion", (const char*)::glGetString (GL_SHADING_LANGUAGE_VERSION));
    if (myIsGlDebugCtx)
    {
      addInfo (theDict, "GLdebug", "ON");
    }
  }

  if ((theFlags & Graphic3d_DiagnosticInfo_Limits) != 0)
  {
    addInfo (theDict, "Max texture size", TCollection_AsciiString(myMaxTexDim));
    addInfo (theDict, "Max combined texture units", TCollection_AsciiString(myMaxTexCombined));
    addInfo (theDict, "Max MSAA samples", TCollection_AsciiString(myMaxMsaaSamples));
  }

  if ((theFlags & Graphic3d_DiagnosticInfo_FrameBuffer) != 0)
  {
    GLint aViewport[4] = {};
    ::glGetIntegerv (GL_VIEWPORT, aViewport);
    addInfo (theDict, "Viewport", TCollection_AsciiString() + aViewport[2] + "x" + aViewport[3]);
  }

  if ((theFlags & Graphic3d_DiagnosticInfo_Memory) != 0)
  {
    MemoryInfo (theDict);
  }

  if ((theFlags & Graphic3d_DiagnosticInfo_Extensions) != 0)
  {
  #if !defined(GL_ES_VERSION_2_0)
    if (IsGlGreaterEqual (3, 0)
     && myFuncs->glGetStringi != NULL)
    {
      TCollection_AsciiString anExtList;
      GLint anExtNb = 0;
      ::glGetIntegerv (GL_NUM_EXTENSIONS, &anExtNb);
      for (GLint anIter = 0; anIter < anExtNb; ++anIter)
      {
        const char* anExtension = (const char*)myFuncs->glGetStringi (GL_EXTENSIONS, (GLuint)anIter);
        if (!anExtList.IsEmpty())
        {
          anExtList += " ";
        }
        anExtList += anExtension;
      }
      addInfo(theDict, "GLextensions", anExtList);
    }
    else
  #endif
    {
      addInfo (theDict, "GLextensions", (const char*)::glGetString (GL_EXTENSIONS));
    }
  }
}

// =======================================================================
// function : GetResource
// purpose  :
// =======================================================================
const Handle(OpenGl_Resource)& OpenGl_Context::GetResource (const TCollection_AsciiString& theKey) const
{
  return mySharedResources->IsBound (theKey) ? mySharedResources->Find (theKey) : NULL_GL_RESOURCE;
}

// =======================================================================
// function : ShareResource
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::ShareResource (const TCollection_AsciiString& theKey,
                                                const Handle(OpenGl_Resource)& theResource)
{
  if (theKey.IsEmpty() || theResource.IsNull())
  {
    return Standard_False;
  }
  return mySharedResources->Bind (theKey, theResource);
}

// =======================================================================
// function : ReleaseResource
// purpose  :
// =======================================================================
void OpenGl_Context::ReleaseResource (const TCollection_AsciiString& theKey,
                                      const Standard_Boolean         theToDelay)
{
  if (!mySharedResources->IsBound (theKey))
  {
    return;
  }
  auto& aRes = mySharedResources->Find (theKey);
  if (aRes->GetRefCount() > 1)
  {
    return;
  }

  if (theToDelay)
  {
    myDelayed->Bind (theKey, 1);
  }
  else
  {
    aRes->Release (this);
    mySharedResources->UnBind (theKey);
  }
}

// =======================================================================
// function : ReleaseDelayed
// purpose  :
// =======================================================================
void OpenGl_Context::ReleaseDelayed()
{
  // release queued elements
  while (!myUnusedResources->IsEmpty())
  {
    myUnusedResources->First()->Release (this);
    myUnusedResources->RemoveFirst();
  }

  // release delayed shared resources
  NCollection_Vector<TCollection_AsciiString> aDeadList;
  for (NCollection_DataMap<TCollection_AsciiString, Standard_Integer>::Iterator anIter (*myDelayed);
       anIter.More(); anIter.Next())
  {
    if (++anIter.ChangeValue() <= 2)
    {
      continue; // postpone release one more frame to ensure noone use it periodically
    }

    const TCollection_AsciiString& aKey = anIter.Key();
    if (!mySharedResources->IsBound (aKey))
    {
      // mixed unshared strategy delayed/undelayed was used!
      aDeadList.Append (aKey);
      continue;
    }

    auto& aRes = mySharedResources->ChangeFind (aKey);
    if (aRes->GetRefCount() > 1)
    {
      // should be only 1 instance in mySharedResources
      // if not - resource was reused again
      aDeadList.Append (aKey);
      continue;
    }

    // release resource if no one requiested it more than 2 redraw calls
    aRes->Release (this);
    mySharedResources->UnBind (aKey);
    aDeadList.Append (aKey);
  }

  for (Standard_Integer anIter = 0; anIter < aDeadList.Length(); ++anIter)
  {
    myDelayed->UnBind (aDeadList.Value (anIter));
  }
}

// =======================================================================
// function : BindTextures
// purpose  :
// =======================================================================
Handle(OpenGl_TextureSet) OpenGl_Context::BindTextures (const Handle(OpenGl_TextureSet)& theTextures)
{
  if (myActiveTextures == theTextures)
  {
    return myActiveTextures;
  }

  Handle(OpenGl_Context) aThisCtx (this);
  OpenGl_TextureSet::Iterator aTextureIterOld (myActiveTextures), aTextureIterNew (theTextures);
  for (;;)
  {
    if (!aTextureIterNew.More())
    {
      for (; aTextureIterOld.More(); aTextureIterOld.Next())
      {
        if (const Handle(OpenGl_Texture)& aTextureOld = aTextureIterOld.Value())
        {
        #if !defined(GL_ES_VERSION_2_0)
          if (core11 != NULL)
          {
            OpenGl_Sampler::resetGlobalTextureParams (aThisCtx, *aTextureOld, aTextureOld->Sampler()->Parameters());
          }
        #endif
          aTextureOld->Unbind (aThisCtx);
        }
      }
      break;
    }

    const Handle(OpenGl_Texture)& aTextureNew = aTextureIterNew.Value();
    if (aTextureIterOld.More())
    {
      const Handle(OpenGl_Texture)& aTextureOld = aTextureIterOld.Value();
      if (aTextureNew == aTextureOld)
      {
        aTextureIterNew.Next();
        aTextureIterOld.Next();
        continue;
      }
      else if (aTextureNew.IsNull()
           || !aTextureNew->IsValid())
      {
        if (!aTextureOld.IsNull())
        {
        #if !defined(GL_ES_VERSION_2_0)
          if (core11 != NULL)
          {
            OpenGl_Sampler::resetGlobalTextureParams (aThisCtx, *aTextureOld, aTextureOld->Sampler()->Parameters());
          }
        #endif
          aTextureOld->Unbind (aThisCtx);
        }

        aTextureIterNew.Next();
        aTextureIterOld.Next();
        continue;
      }

      aTextureIterOld.Next();
    }
    if (aTextureNew.IsNull())
    {
      aTextureIterNew.Next();
      continue;
    }

    const Graphic3d_TextureUnit aTexUnit = aTextureNew->Sampler()->Parameters()->TextureUnit();
    if (aTexUnit >= myMaxTexCombined)
    {
      PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0, GL_DEBUG_SEVERITY_HIGH,
                   TCollection_AsciiString("Texture unit ") + aTexUnit + " for " + aTextureNew->ResourceId() + " exceeds hardware limit " + myMaxTexCombined);
      aTextureIterNew.Next();
      continue;
    }

    aTextureNew->Bind (aThisCtx);
    if (aTextureNew->Sampler()->ToUpdateParameters())
    {
      if (aTextureNew->Sampler()->IsImmutable())
      {
        aTextureNew->Sampler()->Init (aThisCtx, *aTextureNew);
      }
      else
      {
        OpenGl_Sampler::applySamplerParams (aThisCtx, aTextureNew->Sampler()->Parameters(), aTextureNew->Sampler().get(), aTextureNew->GetTarget(), aTextureNew->HasMipmaps());
      }
    }
  #if !defined(GL_ES_VERSION_2_0)
    if (core11 != NULL)
    {
      OpenGl_Sampler::applyGlobalTextureParams (aThisCtx, *aTextureNew, aTextureNew->Sampler()->Parameters());
    }
  #endif
    aTextureIterNew.Next();
  }

  Handle(OpenGl_TextureSet) anOldTextures = myActiveTextures;
  myActiveTextures = theTextures;
  return anOldTextures;
}

// =======================================================================
// function : BindProgram
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::BindProgram (const Handle(OpenGl_ShaderProgram)& theProgram)
{
  if (core20fwd == NULL)
  {
    return Standard_False;
  }
  else if (myActiveProgram == theProgram)
  {
    return Standard_True;
  }

  if (theProgram.IsNull()
  || !theProgram->IsValid())
  {
    if (!myActiveProgram.IsNull())
    {
      core20fwd->glUseProgram (OpenGl_ShaderProgram::NO_PROGRAM);
      myActiveProgram.Nullify();
    }
    return Standard_False;
  }

  myActiveProgram = theProgram;
  core20fwd->glUseProgram (theProgram->ProgramId());
  return Standard_True;
}

// =======================================================================
// function : BindDefaultVao
// purpose  :
// =======================================================================
void OpenGl_Context::BindDefaultVao()
{
#if !defined(GL_ES_VERSION_2_0)
  if (myDefaultVao == 0
   || core32 == NULL)
  {
    return;
  }

  core32->glBindVertexArray (myDefaultVao);
#endif
}

// =======================================================================
// function : SetDefaultFrameBuffer
// purpose  :
// =======================================================================
Handle(OpenGl_FrameBuffer) OpenGl_Context::SetDefaultFrameBuffer (const Handle(OpenGl_FrameBuffer)& theFbo)
{
  Handle(OpenGl_FrameBuffer) aFbo = myDefaultFbo;
  myDefaultFbo = theFbo;
  return aFbo;
}

// =======================================================================
// function : SetShadingMaterial
// purpose  :
// =======================================================================
void OpenGl_Context::SetShadingMaterial (const OpenGl_AspectFace* theAspect,
                                         const Handle(Graphic3d_PresentationAttributes)& theHighlight)
{
  const Handle(Graphic3d_AspectFillArea3d)& anAspect = (!theHighlight.IsNull() && !theHighlight->BasicFillAreaAspect().IsNull())
                                                      ?  theHighlight->BasicFillAreaAspect()
                                                      :  theAspect->Aspect();

  const bool toDistinguish = anAspect->Distinguish();
  const bool toMapTexture  = anAspect->ToMapTexture();
  const Graphic3d_MaterialAspect& aMatFrontSrc = anAspect->FrontMaterial();
  const Graphic3d_MaterialAspect& aMatBackSrc  = toDistinguish
                                               ? anAspect->BackMaterial()
                                               : aMatFrontSrc;
  const Quantity_Color& aFrontIntColor = anAspect->InteriorColor();
  const Quantity_Color& aBackIntColor  = toDistinguish
                                       ? anAspect->BackInteriorColor()
                                       : aFrontIntColor;

  myMatFront.Init (aMatFrontSrc, aFrontIntColor);
  if (toDistinguish)
  {
    myMatBack.Init (aMatBackSrc, aBackIntColor);
  }
  else
  {
    myMatBack = myMatFront;
  }

  if (!theHighlight.IsNull()
    && theHighlight->BasicFillAreaAspect().IsNull())
  {
    myMatFront.SetColor (theHighlight->ColorRGBA());
    myMatBack .SetColor (theHighlight->ColorRGBA());
  }

  Standard_ShortReal anAlphaFront = 1.0f;
  Standard_ShortReal anAlphaBack  = 1.0f;
  if (CheckIsTransparent (theAspect, theHighlight, anAlphaFront, anAlphaBack))
  {
    myMatFront.Diffuse.a() = anAlphaFront;
    myMatBack .Diffuse.a() = anAlphaBack;
  }

  // do not update material properties in case of zero reflection mode,
  // because GL lighting will be disabled by OpenGl_PrimitiveArray::DrawArray() anyway.
  if (theAspect->IsNoLighting())
  {
    return;
  }

  if (myMatFront    == myShaderManager->MaterialState().FrontMaterial()
   && myMatBack     == myShaderManager->MaterialState().BackMaterial()
   && toDistinguish == myShaderManager->MaterialState().ToDistinguish()
   && toMapTexture  == myShaderManager->MaterialState().ToMapTexture())
  {
    return;
  }

  myShaderManager->UpdateMaterialStateTo (myMatFront, myMatBack, toDistinguish, toMapTexture);
}

// =======================================================================
// function : CheckIsTransparent
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::CheckIsTransparent (const OpenGl_AspectFace* theAspect,
                                                     const Handle(Graphic3d_PresentationAttributes)& theHighlight,
                                                     Standard_ShortReal& theAlphaFront,
                                                     Standard_ShortReal& theAlphaBack)
{
  const Handle(Graphic3d_AspectFillArea3d)& anAspect = (!theHighlight.IsNull() && !theHighlight->BasicFillAreaAspect().IsNull())
                                                      ?  theHighlight->BasicFillAreaAspect()
                                                      :  theAspect->Aspect();

  const bool toDistinguish = anAspect->Distinguish();
  const Graphic3d_MaterialAspect& aMatFrontSrc = anAspect->FrontMaterial();
  const Graphic3d_MaterialAspect& aMatBackSrc  = toDistinguish
                                               ? anAspect->BackMaterial()
                                               : aMatFrontSrc;

  // handling transparency
  if (!theHighlight.IsNull()
    && theHighlight->BasicFillAreaAspect().IsNull())
  {
    theAlphaFront = theHighlight->ColorRGBA().Alpha();
    theAlphaBack  = theHighlight->ColorRGBA().Alpha();
  }
  else
  {
    theAlphaFront = aMatFrontSrc.Alpha();
    theAlphaBack  = aMatBackSrc .Alpha();
  }

  const bool isTransparent = theAlphaFront < 1.0f
                          || theAlphaBack  < 1.0f;
  return isTransparent;
}

// =======================================================================
// function : SetColor4fv
// purpose  :
// =======================================================================
void OpenGl_Context::SetColor4fv (const OpenGl_Vec4& theColor)
{
  if (!myActiveProgram.IsNull())
  {
    myActiveProgram->SetUniform (this, myActiveProgram->GetStateLocation (OpenGl_OCCT_COLOR), theColor);
  }
#if !defined(GL_ES_VERSION_2_0)
  else if (core11 != NULL)
  {
    core11->glColor4fv (theColor.GetData());
  }
#endif
}

// =======================================================================
// function : SetTypeOfLine
// purpose  :
// =======================================================================
void OpenGl_Context::SetTypeOfLine (const Aspect_TypeOfLine  theType,
                                    const Standard_ShortReal theFactor)
{
  Standard_Integer aPattern = 0xFFFF;
  switch (theType)
  {
    case Aspect_TOL_DASH:
    {
      aPattern = 0xFFC0;
      break;
    }
    case Aspect_TOL_DOT:
    {
      aPattern = 0xCCCC;
      break;
    }
    case Aspect_TOL_DOTDASH:
    {
      aPattern = 0xFF18;
      break;
    }
    case Aspect_TOL_EMPTY:
    case Aspect_TOL_SOLID:
    {
      aPattern = 0xFFFF;
      break;
    }
    case Aspect_TOL_USERDEFINED:
    {
      aPattern = 0xFF24;
      break;
    }
  }

  if (!myActiveProgram.IsNull())
  {
    myActiveProgram->SetUniform (this, "uPattern", aPattern);
    myActiveProgram->SetUniform (this, "uFactor",  theFactor);
    return;
  }

#if !defined(GL_ES_VERSION_2_0)
  if (aPattern != 0xFFFF)
  {
  #ifdef HAVE_GL2PS
    if (IsFeedback())
    {
      gl2psEnable (GL2PS_LINE_STIPPLE);
    }
  #endif

    if (core11 != NULL)
    {
      core11fwd->glEnable (GL_LINE_STIPPLE);

      core11->glLineStipple (static_cast<GLint>    (theFactor),
                             static_cast<GLushort> (aPattern));
    }
  }
  else
  {
    if (core11 != NULL)
    {
      core11fwd->glDisable (GL_LINE_STIPPLE);
    }

  #ifdef HAVE_GL2PS
    if (IsFeedback())
    {
      gl2psDisable (GL2PS_LINE_STIPPLE);
    }
  #endif
  }
#endif
}

// =======================================================================
// function : SetLineWidth
// purpose  :
// =======================================================================
void OpenGl_Context::SetLineWidth (const Standard_ShortReal theWidth)
{
  if (core11 != NULL)
  {
    // glLineWidth() is still defined within Core Profile, but has no effect with values != 1.0f
    core11fwd->glLineWidth (theWidth * myLineWidthScale);
  }
#ifdef HAVE_GL2PS
  if (IsFeedback())
  {
    gl2psLineWidth (theWidth);
  }
#endif
}

// =======================================================================
// function : SetTextureMatrix
// purpose  :
// =======================================================================
void OpenGl_Context::SetTextureMatrix (const Handle(Graphic3d_TextureParams)& theParams)
{
  if (theParams.IsNull())
  {
    return;
  }
  else if (!myActiveProgram.IsNull())
  {
    const GLint aUniLoc = myActiveProgram->GetStateLocation (OpenGl_OCCT_TEXTURE_TRSF2D);
    if (aUniLoc == OpenGl_ShaderProgram::INVALID_LOCATION)
    {
      return;
    }

    // pack transformation parameters
    OpenGl_Vec4 aTrsf[2];
    aTrsf[0].x()  = -theParams->Translation().x();
    aTrsf[0].y()  = -theParams->Translation().y();
    aTrsf[0].zw() = theParams->Scale();
    aTrsf[1].x()  = std::sin (-theParams->Rotation() * static_cast<float> (M_PI / 180.0));
    aTrsf[1].y()  = std::cos (-theParams->Rotation() * static_cast<float> (M_PI / 180.0));
    myActiveProgram->SetUniform (this, aUniLoc, 2, aTrsf);
    return;
  }

#if !defined(GL_ES_VERSION_2_0)
  if (core11 != NULL)
  {
    GLint aMatrixMode = GL_TEXTURE;
    ::glGetIntegerv (GL_MATRIX_MODE, &aMatrixMode);

    core11->glMatrixMode (GL_TEXTURE);
    OpenGl_Mat4 aTextureMat;
    const Graphic3d_Vec2& aScale = theParams->Scale();
    const Graphic3d_Vec2& aTrans = theParams->Translation();
    Graphic3d_TransformUtils::Scale     (aTextureMat,  aScale.x(),  aScale.y(), 1.0f);
    Graphic3d_TransformUtils::Translate (aTextureMat, -aTrans.x(), -aTrans.y(), 0.0f);
    Graphic3d_TransformUtils::Rotate    (aTextureMat, -theParams->Rotation(), 0.0f, 0.0f, 1.0f);
    core11->glLoadMatrixf (aTextureMat);
    core11->glMatrixMode (aMatrixMode);
  }
#endif
}

// =======================================================================
// function : SetPointSize
// purpose  :
// =======================================================================
void OpenGl_Context::SetPointSize (const Standard_ShortReal theSize)
{
  if (!myActiveProgram.IsNull())
  {
    myActiveProgram->SetUniform (this, myActiveProgram->GetStateLocation (OpenGl_OCCT_POINT_SIZE), theSize);
  #if !defined(GL_ES_VERSION_2_0)
    //myContext->core11fwd->glEnable (GL_VERTEX_PROGRAM_POINT_SIZE);
  #endif
  }
#if !defined(GL_ES_VERSION_2_0)
  //else
  {
    core11fwd->glPointSize (theSize);
    if (core20fwd != NULL)
    {
      //myContext->core11fwd->glDisable (GL_VERTEX_PROGRAM_POINT_SIZE);
    }
  }
#endif
}

// =======================================================================
// function : SetPointSpriteOrigin
// purpose  :
// =======================================================================
void OpenGl_Context::SetPointSpriteOrigin()
{
#if !defined(GL_ES_VERSION_2_0)
  if (core15fwd == NULL)
  {
    return;
  }

  const int aNewState = !myActiveProgram.IsNull() ? GL_UPPER_LEFT : GL_LOWER_LEFT;
  if (myPointSpriteOrig != aNewState)
  {
    myPointSpriteOrig = aNewState;
    core15fwd->glPointParameteri (GL_POINT_SPRITE_COORD_ORIGIN, aNewState);
  }
#endif
}

// =======================================================================
// function : SetGlNormalizeEnabled
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::SetGlNormalizeEnabled (Standard_Boolean isEnabled)
{
  if (isEnabled == myIsGlNormalizeEnabled)
  {
    return myIsGlNormalizeEnabled;
  }

  Standard_Boolean anOldGlNormalize = myIsGlNormalizeEnabled;

  myIsGlNormalizeEnabled = isEnabled;

#if !defined(GL_ES_VERSION_2_0)
  if (core11 != NULL)
  {
    if (isEnabled)
    {
      ::glEnable  (GL_NORMALIZE);
    }
    else
    {
      ::glDisable (GL_NORMALIZE);
    }
  }
#endif

  return anOldGlNormalize;
}

// =======================================================================
// function : SetPolygonMode
// purpose  :
// =======================================================================
Standard_Integer OpenGl_Context::SetPolygonMode (const Standard_Integer theMode)
{
  if (myPolygonMode == theMode)
  {
    return myPolygonMode;
  }

  const Standard_Integer anOldPolygonMode = myPolygonMode;

  myPolygonMode = theMode;

#if !defined(GL_ES_VERSION_2_0)
  ::glPolygonMode (GL_FRONT_AND_BACK, (GLenum)theMode);
#endif

  return anOldPolygonMode;
}

// =======================================================================
// function : SetPolygonHatchEnabled
// purpose  :
// =======================================================================
bool OpenGl_Context::SetPolygonHatchEnabled (const bool theIsEnabled)
{
  if (myHatchStyles.IsNull())
  {
    return false;
  }
  else if (myHatchStyles->IsEnabled() == theIsEnabled)
  {
    return theIsEnabled;
  }

  return myHatchStyles->SetEnabled (this, theIsEnabled);
}

// =======================================================================
// function : SetPolygonHatchStyle
// purpose  :
// =======================================================================
Standard_Integer OpenGl_Context::SetPolygonHatchStyle (const Handle(Graphic3d_HatchStyle)& theStyle)
{
  if (theStyle.IsNull())
  {
    return 0;
  }

  if (myHatchStyles.IsNull())
  {
    if (!GetResource ("OpenGl_LineAttributes", myHatchStyles))
    {
      // share and register for release once the resource is no longer used
      myHatchStyles = new OpenGl_LineAttributes();
      ShareResource ("OpenGl_LineAttributes", myHatchStyles);
    }
  }
  if (myHatchStyles->TypeOfHatch() == theStyle->HatchType())
  {
    return theStyle->HatchType();
  }

  return myHatchStyles->SetTypeOfHatch (this, theStyle);
}

// =======================================================================
// function : ApplyModelWorldMatrix
// purpose  :
// =======================================================================
void OpenGl_Context::ApplyModelWorldMatrix()
{
  if (myShaderManager->ModelWorldState().ModelWorldMatrix() != ModelWorldState.Current())
  {
    myShaderManager->UpdateModelWorldStateTo (ModelWorldState.Current());
  }
}

// =======================================================================
// function : ApplyWorldViewMatrix
// purpose  :
// =======================================================================
void OpenGl_Context::ApplyWorldViewMatrix()
{
  if (myShaderManager->ModelWorldState().ModelWorldMatrix() != THE_IDENTITY_MATRIX)
  {
    myShaderManager->UpdateModelWorldStateTo (THE_IDENTITY_MATRIX);
  }
  if (myShaderManager->WorldViewState().WorldViewMatrix() != WorldViewState.Current())
  {
    myShaderManager->UpdateWorldViewStateTo (WorldViewState.Current());
  }
}

// =======================================================================
// function : ApplyModelViewMatrix
// purpose  :
// =======================================================================
void OpenGl_Context::ApplyModelViewMatrix()
{
  if (myShaderManager->ModelWorldState().ModelWorldMatrix() != ModelWorldState.Current())
  {
    myShaderManager->UpdateModelWorldStateTo (ModelWorldState.Current());
  }
  if (myShaderManager->WorldViewState().WorldViewMatrix() != WorldViewState.Current())
  {
    myShaderManager->UpdateWorldViewStateTo  (WorldViewState.Current());
  }
}

// =======================================================================
// function : ApplyProjectionMatrix
// purpose  :
// =======================================================================
void OpenGl_Context::ApplyProjectionMatrix()
{
  if (myShaderManager->ProjectionState().ProjectionMatrix() != ProjectionState.Current())
  {
    myShaderManager->UpdateProjectionStateTo (ProjectionState.Current());
  }
}

// =======================================================================
// function : EnableFeatures
// purpose  :
// =======================================================================
void OpenGl_Context::EnableFeatures() const
{
  //
}

// =======================================================================
// function : DisableFeatures
// purpose  :
// =======================================================================
void OpenGl_Context::DisableFeatures() const
{
  // Disable stuff that's likely to slow down glDrawPixels.
  glDisable(GL_DITHER);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_STENCIL_TEST);

#if !defined(GL_ES_VERSION_2_0)
  if (core11 == NULL)
  {
    return;
  }

  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);

  glDisable(GL_LIGHTING);
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_FOG);
  glDisable(GL_LOGIC_OP);

  glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
  glPixelTransferi(GL_RED_SCALE, 1);
  glPixelTransferi(GL_RED_BIAS, 0);
  glPixelTransferi(GL_GREEN_SCALE, 1);
  glPixelTransferi(GL_GREEN_BIAS, 0);
  glPixelTransferi(GL_BLUE_SCALE, 1);
  glPixelTransferi(GL_BLUE_BIAS, 0);
  glPixelTransferi(GL_ALPHA_SCALE, 1);
  glPixelTransferi(GL_ALPHA_BIAS, 0);

  if ((myGlVerMajor >= 1) && (myGlVerMinor >= 2))
  {
    if (CheckExtension ("GL_CONVOLUTION_1D_EXT"))
      glDisable(GL_CONVOLUTION_1D_EXT);

    if (CheckExtension ("GL_CONVOLUTION_2D_EXT"))
      glDisable(GL_CONVOLUTION_2D_EXT);

    if (CheckExtension ("GL_SEPARABLE_2D_EXT"))
      glDisable(GL_SEPARABLE_2D_EXT);

    if (CheckExtension ("GL_SEPARABLE_2D_EXT"))
      glDisable(GL_HISTOGRAM_EXT);

    if (CheckExtension ("GL_MINMAX_EXT"))
      glDisable(GL_MINMAX_EXT);

    if (CheckExtension ("GL_TEXTURE_3D_EXT"))
      glDisable(GL_TEXTURE_3D_EXT);
  }
#endif
}
