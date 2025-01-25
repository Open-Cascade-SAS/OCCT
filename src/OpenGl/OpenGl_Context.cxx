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
#elif defined(__APPLE__)
  // macOS 10.4 deprecated OpenGL framework - suppress useless warnings
  #define GL_SILENCE_DEPRECATION
#endif

#include <OpenGl_Context.hxx>

#include <OpenGl_ArbTBO.hxx>
#include <OpenGl_ArbIns.hxx>
#include <OpenGl_ArbDbg.hxx>
#include <OpenGl_ArbFBO.hxx>
#include <OpenGl_ExtGS.hxx>
#include <OpenGl_ArbSamplerObject.hxx>
#include <OpenGl_ArbTexBindless.hxx>
#include <OpenGl_GlCore46.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_FrameStats.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_TextureSetPairIterator.hxx>
#include <OpenGl_Workspace.hxx>
#include <OpenGl_Aspects.hxx>

#include <Aspect_Handle.hxx>
#include <Graphic3d_TransformUtils.hxx>
#include <Graphic3d_RenderingParams.hxx>
#include <Image_SupportedFormats.hxx>
#include <Message_Messenger.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_ProgramError.hxx>
#include <Standard_WarningDisableFunctionCast.hxx>

#if defined(_WIN32) && defined(max)
  #undef max
#endif
#include <limits>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Context, Standard_Transient)

#if defined(HAVE_EGL)
  #include <EGL/egl.h>
  #ifdef _MSC_VER
    #pragma comment(lib, "libEGL.lib")
  #endif
#elif defined(_WIN32)
//
#elif defined(HAVE_XLIB)
  #include <GL/glx.h> // glXGetProcAddress()
#elif defined(__APPLE__)
  #include <dlfcn.h>
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  //
  #else
    #include <OpenGL/OpenGL.h>
    #include <CoreGraphics/CoreGraphics.h>
  #endif
#else
//
#endif

#ifdef __EMSCRIPTEN__
  #include <emscripten.h>
  #include <emscripten/html5.h>
#endif

#if defined(HAVE_GLES2) || defined(OCCT_UWP) || defined(__ANDROID__) || defined(__QNX__)           \
  || defined(__EMSCRIPTEN__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)
  #define OCC_USE_GLES2
#endif

namespace
{
static const Handle(OpenGl_Resource) NULL_GL_RESOURCE;
static const OpenGl_Mat4             THE_IDENTITY_MATRIX;

//! Add key-value pair to the dictionary.
static void addInfo(TColStd_IndexedDataMapOfStringString& theDict,
                    const TCollection_AsciiString&        theKey,
                    const TCollection_AsciiString&        theValue)
{
  theDict.ChangeFromIndex(theDict.Add(theKey, theValue)) = theValue;
}

//! Add key-value pair to the dictionary.
static void addInfo(TColStd_IndexedDataMapOfStringString& theDict,
                    const TCollection_AsciiString&        theKey,
                    const char*                           theValue)
{
  TCollection_AsciiString aValue(theValue != NULL ? theValue : "");
  theDict.ChangeFromIndex(theDict.Add(theKey, aValue)) = aValue;
}
} // namespace

// =======================================================================
// function : OpenGl_Context
// purpose  :
// =======================================================================
OpenGl_Context::OpenGl_Context(const Handle(OpenGl_Caps)& theCaps)
    : core11ffp(NULL),
      core11fwd(NULL),
      core15(NULL),
      core20(NULL),
      core30(NULL),
      core32(NULL),
      core33(NULL),
      core41(NULL),
      core42(NULL),
      core43(NULL),
      core44(NULL),
      core45(NULL),
      core46(NULL),
      core15fwd(NULL),
      core20fwd(NULL),
      caps(!theCaps.IsNull() ? theCaps : new OpenGl_Caps()),
      hasGetBufferData(Standard_False),
#if defined(OCC_USE_GLES2)
      hasPackRowLength(Standard_False),
      hasUnpackRowLength(Standard_False),
      hasHighp(Standard_False),
      hasUintIndex(Standard_False),
      hasTexRGBA8(Standard_False),
#else
      hasPackRowLength(Standard_True),
      hasUnpackRowLength(Standard_True),
      hasHighp(Standard_True),
      hasUintIndex(Standard_True),
      hasTexRGBA8(Standard_True),
#endif
      hasTexFloatLinear(Standard_False),
      hasTexSRGB(Standard_False),
      hasFboSRGB(Standard_False),
      hasSRGBControl(Standard_False),
      hasFboRenderMipmap(Standard_False),
#if defined(OCC_USE_GLES2)
      hasFlatShading(OpenGl_FeatureNotAvailable),
#else
      hasFlatShading(OpenGl_FeatureInCore),
#endif
      hasGlslBitwiseOps(OpenGl_FeatureNotAvailable),
      hasDrawBuffers(OpenGl_FeatureNotAvailable),
      hasFloatBuffer(OpenGl_FeatureNotAvailable),
      hasHalfFloatBuffer(OpenGl_FeatureNotAvailable),
      hasSampleVariables(OpenGl_FeatureNotAvailable),
      hasGeometryStage(OpenGl_FeatureNotAvailable),
      arbDrawBuffers(Standard_False),
      arbNPTW(Standard_False),
      arbTexRG(Standard_False),
      arbTexFloat(Standard_False),
      arbSamplerObject(NULL),
      arbTexBindless(NULL),
      arbTBO(NULL),
      arbTboRGB32(Standard_False),
      arbClipControl(Standard_False),
      arbIns(NULL),
      arbDbg(NULL),
      arbFBO(NULL),
      arbFBOBlit(NULL),
      arbSampleShading(Standard_False),
      arbDepthClamp(Standard_False),
      extFragDepth(Standard_False),
      extDrawBuffers(Standard_False),
      extGS(NULL),
      extBgra(Standard_False),
      extTexR16(Standard_False),
      extAnis(Standard_False),
      extPDS(Standard_False),
      atiMem(Standard_False),
      nvxMem(Standard_False),
      oesSampleVariables(Standard_False),
      oesStdDerivatives(Standard_False),
      myWindow(0),
      myDisplay(0),
      myGContext(0),
      mySharedResources(new OpenGl_ResourcesMap()),
      myDelayed(new OpenGl_DelayReleaseMap()),
      myUnusedResources(new OpenGl_ResourcesStack()),
      myClippingState(),
      myGlLibHandle(NULL),
      myFuncs(new OpenGl_GlFunctions()),
      myGapi(
#if defined(OCC_USE_GLES2)
        Aspect_GraphicsLibrary_OpenGLES
#else
        Aspect_GraphicsLibrary_OpenGL
#endif
        ),
      mySupportedFormats(new Image_SupportedFormats()),
      myAnisoMax(1),
      myTexClamp(GL_CLAMP_TO_EDGE),
      myMaxTexDim(1024),
      myMaxTexCombined(1),
      myMaxTexUnitsFFP(1),
      myMaxDumpSizeX(1024),
      myMaxDumpSizeY(1024),
      myMaxClipPlanes(6),
      myMaxMsaaSamples(0),
      myMaxDrawBuffers(1),
      myMaxColorAttachments(1),
      myGlVerMajor(0),
      myGlVerMinor(0),
      myIsInitialized(Standard_False),
      myIsStereoBuffers(Standard_False),
      myHasMsaaTextures(Standard_False),
      myIsGlNormalizeEnabled(Standard_False),
      mySpriteTexUnit(Graphic3d_TextureUnit_PointSprite),
      myHasRayTracing(Standard_False),
      myHasRayTracingTextures(Standard_False),
      myHasRayTracingAdaptiveSampling(Standard_False),
      myHasRayTracingAdaptiveSamplingAtomic(Standard_False),
      myHasPBR(Standard_False),
      myPBREnvLUTTexUnit(Graphic3d_TextureUnit_PbrEnvironmentLUT),
      myPBRDiffIBLMapSHTexUnit(Graphic3d_TextureUnit_PbrIblDiffuseSH),
      myPBRSpecIBLMapTexUnit(Graphic3d_TextureUnit_PbrIblSpecular),
      myShadowMapTexUnit(Graphic3d_TextureUnit_ShadowMap),
      myDepthPeelingDepthTexUnit(Graphic3d_TextureUnit_DepthPeelingDepth),
      myDepthPeelingFrontColorTexUnit(Graphic3d_TextureUnit_DepthPeelingFrontColor),
      myFrameStats(new OpenGl_FrameStats()),
      myActiveMockTextures(0),
      myActiveHatchType(Aspect_HS_SOLID),
      myHatchIsEnabled(false),
      myPointSpriteOrig(GL_UPPER_LEFT),
      myRenderMode(GL_RENDER),
      myShadeModel(GL_SMOOTH),
      myPolygonMode(GL_FILL),
      myFaceCulling(Graphic3d_TypeOfBackfacingModel_DoubleSided),
      myReadBuffer(0),
      myDrawBuffers(0, 7),
      myDefaultVao(0),
      myColorMask(true),
      myAlphaToCoverage(false),
      myIsGlDebugCtx(false),
      myIsWindowDeepColor(false),
      myIsSRgbWindow(false),
      myIsSRgbActive(false),
      myResolution(Graphic3d_RenderingParams::THE_DEFAULT_RESOLUTION),
      myResolutionRatio(1.0f),
      myLineWidthScale(1.0f),
      myLineFeather(1.0f),
      myRenderScale(1.0f),
      myRenderScaleInv(1.0f)
{
  myViewport[0]     = 0;
  myViewport[1]     = 0;
  myViewport[2]     = 0;
  myViewport[3]     = 0;
  myViewportVirt[0] = 0;
  myViewportVirt[1] = 0;
  myViewportVirt[2] = 0;
  myViewportVirt[3] = 0;

  myPolygonOffset.Mode   = Aspect_POM_Off;
  myPolygonOffset.Factor = 0.0f;
  myPolygonOffset.Units  = 0.0f;

  // system-dependent fields
#if defined(HAVE_EGL)
  myDisplay  = (Aspect_Display)EGL_NO_DISPLAY;
  myWindow   = (Aspect_Drawable)EGL_NO_SURFACE;
  myGContext = (Aspect_RenderingContext)EGL_NO_CONTEXT;
#elif defined(__APPLE__) && !defined(HAVE_XLIB)
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
  myGlLibHandle = dlopen("/System/Library/Frameworks/OpenGLES.framework/OpenGLES", RTLD_LAZY);
  #else
  myGlLibHandle =
    dlopen("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", RTLD_LAZY);
  #endif
#endif

  memset(myFuncs.get(), 0, sizeof(OpenGl_GlFunctions));
  myShaderManager = new OpenGl_ShaderManager(this);
}

// =======================================================================
// function : ~OpenGl_Context
// purpose  :
// =======================================================================
OpenGl_Context::~OpenGl_Context()
{
  // release clean up queue
  ReleaseDelayed();

  // release default VAO
  if (myDefaultVao != 0 && IsValid() && core32 != NULL)
  {
    core32->glDeleteVertexArrays(1, &myDefaultVao);
  }
  myDefaultVao = 0;

  // release mock textures
  if (!myTextureRgbaBlack.IsNull())
  {
    myTextureRgbaBlack->Release(this);
    myTextureRgbaBlack.Nullify();
  }
  if (!myTextureRgbaWhite.IsNull())
  {
    myTextureRgbaWhite->Release(this);
    myTextureRgbaWhite.Nullify();
  }

  // release default FBO
  if (!myDefaultFbo.IsNull())
  {
    myDefaultFbo->Release(this);
    myDefaultFbo.Nullify();
  }

  // release shared resources if any
  if (mySharedResources->GetRefCount() <= 1)
  {
    myShaderManager.Nullify();
    for (NCollection_DataMap<TCollection_AsciiString, Handle(OpenGl_Resource)>::Iterator anIter(
           *mySharedResources);
         anIter.More();
         anIter.Next())
    {
      anIter.Value()->Release(this);
    }

    // release delayed resources added during deletion of shared resources
    while (!myUnusedResources->IsEmpty())
    {
      myUnusedResources->First()->Release(this);
      myUnusedResources->RemoveFirst();
    }
  }
  else if (myShaderManager->IsSameContext(this))
  {
    myShaderManager->SetContext(NULL);
  }
  mySharedResources.Nullify();
  myDelayed.Nullify();

  if (arbDbg != NULL && myIsGlDebugCtx && IsValid())
  {
    // reset callback
    void* aPtr = NULL;
    if (myGapi == Aspect_GraphicsLibrary_OpenGL)
    {
      myFuncs->glGetPointerv(GL_DEBUG_CALLBACK_USER_PARAM, &aPtr);
    }
    if (aPtr == this || myGapi != Aspect_GraphicsLibrary_OpenGL)
    {
      arbDbg->glDebugMessageCallback(NULL, NULL);
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
  for (NCollection_DataMap<TCollection_AsciiString, Handle(OpenGl_Resource)>::Iterator anIter(
         *mySharedResources);
       anIter.More();
       anIter.Next())
  {
    anIter.Value()->Release(this);
  }
  mySharedResources->Clear();
  myShaderManager->clear();
  myShaderManager->SetContext(NULL);

  // release delayed resources added during deletion of shared resources
  while (!myUnusedResources->IsEmpty())
  {
    myUnusedResources->First()->Release(this);
    myUnusedResources->RemoveFirst();
  }
}

// =======================================================================
// function : ResizeViewport
// purpose  :
// =======================================================================
void OpenGl_Context::ResizeViewport(const Standard_Integer* theRect)
{
  core11fwd->glViewport(theRect[0], theRect[1], theRect[2], theRect[3]);
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

static Standard_Integer stereoToMonoBuffer(const Standard_Integer theBuffer)
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

// =======================================================================
// function : SetReadBuffer
// purpose  :
// =======================================================================
void OpenGl_Context::SetReadBuffer(const Standard_Integer theReadBuffer)
{
  if (myGapi == Aspect_GraphicsLibrary_OpenGLES)
  {
    return;
  }

  myReadBuffer = !myIsStereoBuffers ? stereoToMonoBuffer(theReadBuffer) : theReadBuffer;
  if (myReadBuffer < GL_COLOR_ATTACHMENT0 && arbFBO != NULL)
  {
    arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
  }
  core11fwd->glReadBuffer(myReadBuffer);
}

// =======================================================================
// function : SetDrawBuffer
// purpose  :
// =======================================================================
void OpenGl_Context::SetDrawBuffer(const Standard_Integer theDrawBuffer)
{
  if (myGapi == Aspect_GraphicsLibrary_OpenGLES)
  {
    return;
  }

  const Standard_Integer aDrawBuffer =
    !myIsStereoBuffers ? stereoToMonoBuffer(theDrawBuffer) : theDrawBuffer;
  if (aDrawBuffer < GL_COLOR_ATTACHMENT0 && arbFBO != NULL)
  {
    arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
  }
  core11fwd->glDrawBuffer(aDrawBuffer);

  myDrawBuffers.Init(GL_NONE);
  myDrawBuffers.SetValue(0, aDrawBuffer);
}

// =======================================================================
// function : SetDrawBuffers
// purpose  :
// =======================================================================
void OpenGl_Context::SetDrawBuffers(const Standard_Integer  theNb,
                                    const Standard_Integer* theDrawBuffers)
{
  Standard_ASSERT_RETURN(hasDrawBuffers,
                         "Multiple draw buffers feature is not supported by the context",
                         Standard_ASSERT_DO_NOTHING());

  if (myDrawBuffers.Length() < theNb)
  {
    // should actually never happen here
    myDrawBuffers.Resize(0, theNb - 1, false);
  }
  myDrawBuffers.Init(GL_NONE);

  Standard_Boolean useDefaultFbo = Standard_False;
  for (Standard_Integer anI = 0; anI < theNb; ++anI)
  {
    if (theDrawBuffers[anI] < GL_COLOR_ATTACHMENT0 && theDrawBuffers[anI] != GL_NONE)
    {
      useDefaultFbo = Standard_True;
    }
    else
    {
      myDrawBuffers.SetValue(anI, theDrawBuffers[anI]);
    }
  }
  if (arbFBO != NULL && useDefaultFbo)
  {
    arbFBO->glBindFramebuffer(GL_FRAMEBUFFER, OpenGl_FrameBuffer::NO_FRAMEBUFFER);
  }

  myFuncs->glDrawBuffers(theNb, (const GLenum*)theDrawBuffers);
}

// =======================================================================
// function : SetFrameBufferSRGB
// purpose  :
// =======================================================================
void OpenGl_Context::SetFrameBufferSRGB(bool theIsFbo, bool theIsFboSRgb)
{
  if (!hasFboSRGB)
  {
    myIsSRgbActive = false;
    return;
  }
  myIsSRgbActive = ToRenderSRGB() && (theIsFbo || myIsSRgbWindow) && theIsFboSRgb;
  if (!hasSRGBControl)
  {
    return;
  }

  if (myIsSRgbActive)
  {
    core11fwd->glEnable(GL_FRAMEBUFFER_SRGB);
  }
  else
  {
    core11fwd->glDisable(GL_FRAMEBUFFER_SRGB);
  }
}

// =======================================================================
// function : SetFaceCulling
// purpose  :
// =======================================================================
void OpenGl_Context::SetFaceCulling(Graphic3d_TypeOfBackfacingModel theMode)
{
  if (myFaceCulling == theMode)
  {
    return;
  }

  if (theMode == Graphic3d_TypeOfBackfacingModel_BackCulled)
  {
    if (myFaceCulling == Graphic3d_TypeOfBackfacingModel_FrontCulled)
    {
      core11fwd->glCullFace(GL_BACK);
    }
    core11fwd->glEnable(GL_CULL_FACE);
  }
  else if (theMode == Graphic3d_TypeOfBackfacingModel_FrontCulled)
  {
    core11fwd->glCullFace(GL_FRONT);
    core11fwd->glEnable(GL_CULL_FACE);
  }
  else
  {
    core11fwd->glCullFace(GL_BACK);
    core11fwd->glDisable(GL_CULL_FACE);
  }
  myFaceCulling = theMode;
}

// =======================================================================
// function : FetchState
// purpose  :
// =======================================================================
void OpenGl_Context::FetchState()
{
  if (myGapi == Aspect_GraphicsLibrary_OpenGLES)
  {
    return;
  }

  // cache feedback mode state
  if (core11ffp != NULL)
  {
    core11fwd->glGetIntegerv(GL_RENDER_MODE, &myRenderMode);
    core11fwd->glGetIntegerv(GL_SHADE_MODEL, &myShadeModel);
  }

  // cache read buffers state
  core11fwd->glGetIntegerv(GL_READ_BUFFER, &myReadBuffer);

  // cache draw buffers state
  if (myDrawBuffers.Length() < myMaxDrawBuffers)
  {
    myDrawBuffers.Resize(0, myMaxDrawBuffers - 1, false);
  }
  myDrawBuffers.Init(GL_NONE);

  Standard_Integer aDrawBuffer = GL_NONE;
  if (myMaxDrawBuffers == 1)
  {
    core11fwd->glGetIntegerv(GL_DRAW_BUFFER, &aDrawBuffer);
    myDrawBuffers.SetValue(0, aDrawBuffer);
  }
  else
  {
    for (Standard_Integer anI = 0; anI < myMaxDrawBuffers; ++anI)
    {
      core11fwd->glGetIntegerv(GL_DRAW_BUFFER0 + anI, &aDrawBuffer);
      myDrawBuffers.SetValue(anI, aDrawBuffer);
    }
  }
}

// =======================================================================
// function : Share
// purpose  :
// =======================================================================
void OpenGl_Context::Share(const Handle(OpenGl_Context)& theShareCtx)
{
  if (!theShareCtx.IsNull())
  {
    mySharedResources = theShareCtx->mySharedResources;
    myDelayed         = theShareCtx->myDelayed;
    myUnusedResources = theShareCtx->myUnusedResources;
    myShaderManager   = theShareCtx->myShaderManager;
  }
}

#if !defined(__APPLE__) || defined(HAVE_XLIB)

// =======================================================================
// function : IsCurrent
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::IsCurrent() const
{
  #if defined(HAVE_EGL)
  if ((EGLDisplay)myDisplay == EGL_NO_DISPLAY || (EGLContext)myGContext == EGL_NO_CONTEXT)
  {
    return Standard_False;
  }

  return (((EGLDisplay)myDisplay == eglGetCurrentDisplay())
          && ((EGLContext)myGContext == eglGetCurrentContext())
          && ((EGLSurface)myWindow == eglGetCurrentSurface(EGL_DRAW)));
  #elif defined(_WIN32)
  if (myDisplay == NULL || myGContext == NULL)
  {
    return Standard_False;
  }
  return (((HDC)myDisplay == wglGetCurrentDC()) && ((HGLRC)myGContext == wglGetCurrentContext()));
  #elif defined(HAVE_XLIB)
  if (myDisplay == NULL || myWindow == 0 || myGContext == 0)
  {
    return Standard_False;
  }

  return (((Display*)myDisplay == glXGetCurrentDisplay())
          && ((GLXContext)myGContext == glXGetCurrentContext())
          && ((GLXDrawable)myWindow == glXGetCurrentDrawable()));
  #else
  return Standard_False;
  #endif
}

// =======================================================================
// function : MakeCurrent
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::MakeCurrent()
{
  #if defined(HAVE_EGL)
  if ((EGLDisplay)myDisplay == EGL_NO_DISPLAY || (EGLContext)myGContext == EGL_NO_CONTEXT)
  {
    Standard_ProgramError_Raise_if(myIsInitialized,
                                   "OpenGl_Context::Init() should be called before!");
    return Standard_False;
  }

  if (eglMakeCurrent((EGLDisplay)myDisplay,
                     (EGLSurface)myWindow,
                     (EGLSurface)myWindow,
                     (EGLContext)myGContext)
      != EGL_TRUE)
  {
    // if there is no current context it might be impossible to use glGetError() correctly
    PushMessage(GL_DEBUG_SOURCE_WINDOW_SYSTEM,
                GL_DEBUG_TYPE_ERROR,
                0,
                GL_DEBUG_SEVERITY_HIGH,
                "eglMakeCurrent() has failed!");
    myIsInitialized = Standard_False;
    return Standard_False;
  }
  #elif defined(_WIN32)
  if (myDisplay == NULL || myGContext == NULL)
  {
    Standard_ProgramError_Raise_if(myIsInitialized,
                                   "OpenGl_Context::Init() should be called before!");
    return Standard_False;
  }

  // technically it should be safe to activate already bound GL context
  // however some drivers (Intel etc.) may FAIL doing this for unknown reason
  if (IsCurrent())
  {
    myShaderManager->SetContext(this);
    return Standard_True;
  }
  else if (wglMakeCurrent((HDC)myDisplay, (HGLRC)myGContext) != TRUE)
  {
    // notice that glGetError() couldn't be used here!
    wchar_t* aMsgBuff    = NULL;
    DWORD    anErrorCode = GetLastError();
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
                     | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL,
                   anErrorCode,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (wchar_t*)&aMsgBuff,
                   0,
                   NULL);
    TCollection_ExtendedString aMsg("wglMakeCurrent() has failed. ");
    if (aMsgBuff != NULL)
    {
      aMsg += (Standard_ExtString)aMsgBuff;
      LocalFree(aMsgBuff);
    }
    PushMessage(GL_DEBUG_SOURCE_WINDOW_SYSTEM,
                GL_DEBUG_TYPE_ERROR,
                (unsigned int)anErrorCode,
                GL_DEBUG_SEVERITY_HIGH,
                aMsg);
    myIsInitialized = Standard_False;
    return Standard_False;
  }
  #elif defined(HAVE_XLIB)
  if (myDisplay == NULL || myWindow == 0 || myGContext == 0)
  {
    Standard_ProgramError_Raise_if(myIsInitialized,
                                   "OpenGl_Context::Init() should be called before!");
    return Standard_False;
  }

  if (!glXMakeCurrent((Display*)myDisplay, (GLXDrawable)myWindow, (GLXContext)myGContext))
  {
    // if there is no current context it might be impossible to use glGetError() correctly
    PushMessage(GL_DEBUG_SOURCE_WINDOW_SYSTEM,
                GL_DEBUG_TYPE_ERROR,
                0,
                GL_DEBUG_SEVERITY_HIGH,
                "glXMakeCurrent() has failed!");
    myIsInitialized = Standard_False;
    return Standard_False;
  }
  #else
  // not implemented
  if (!myIsInitialized)
  {
    throw Standard_ProgramError("OpenGl_Context::Init() should be called before!");
  }
  #endif
  myShaderManager->SetContext(this);
  return Standard_True;
}

// =======================================================================
// function : SwapBuffers
// purpose  :
// =======================================================================
void OpenGl_Context::SwapBuffers()
{
  #if defined(HAVE_EGL)
  if ((EGLSurface)myWindow != EGL_NO_SURFACE)
  {
    eglSwapBuffers((EGLDisplay)myDisplay, (EGLSurface)myWindow);
  }
  #elif defined(_WIN32)
  if ((HDC)myDisplay != NULL)
  {
    ::SwapBuffers((HDC)myDisplay);
    core11fwd->glFlush();
  }
  #elif defined(HAVE_XLIB)
  if ((Display*)myDisplay != NULL)
  {
    glXSwapBuffers((Display*)myDisplay, (GLXDrawable)myWindow);
  }
  #else
    //
  #endif
}

#endif // __APPLE__

// =======================================================================
// function : SetSwapInterval
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::SetSwapInterval(const Standard_Integer theInterval)
{
#if defined(HAVE_EGL)
  if (::eglSwapInterval((EGLDisplay)myDisplay, theInterval) == EGL_TRUE)
  {
    return Standard_True;
  }
#elif defined(_WIN32)
  if (myFuncs->wglSwapIntervalEXT != NULL)
  {
    myFuncs->wglSwapIntervalEXT(theInterval);
    return Standard_True;
  }
#elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  (void)theInterval; // vsync cannot be turned OFF on iOS
#elif defined(__APPLE__)
  if (::CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &theInterval) == kCGLNoError)
  {
    return Standard_True;
  }
#elif defined(HAVE_XLIB)
  if (theInterval == -1 && myFuncs->glXSwapIntervalEXT != NULL)
  {
    typedef int (
      *glXSwapIntervalEXT_t_x)(Display* theDisplay, GLXDrawable theDrawable, int theInterval);
    glXSwapIntervalEXT_t_x aFuncPtr = (glXSwapIntervalEXT_t_x)myFuncs->glXSwapIntervalEXT;
    aFuncPtr((Display*)myDisplay, (GLXDrawable)myWindow, theInterval);
    return Standard_True;
  }
  else if (myFuncs->glXSwapIntervalSGI != NULL)
  {
    myFuncs->glXSwapIntervalSGI(theInterval);
    return Standard_True;
  }
#else
  //
#endif
  return Standard_False;
}

// =======================================================================
// function : findProc
// purpose  :
// =======================================================================
void* OpenGl_Context::findProc(const char* theFuncName)
{
#if defined(HAVE_EGL)
  return (void*)eglGetProcAddress(theFuncName);
#elif defined(_WIN32)
  return (void*)wglGetProcAddress(theFuncName);
#elif defined(HAVE_XLIB)
  return (void*)glXGetProcAddress((const GLubyte*)theFuncName);
#elif defined(__APPLE__)
  return (myGlLibHandle != NULL) ? dlsym(myGlLibHandle, theFuncName) : NULL;
#else
  (void)theFuncName;
  return NULL;
#endif
}

// =======================================================================
// function : CheckExtension
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::CheckExtension(const char* theExtName) const
{
  if (theExtName == NULL)
  {
#ifdef OCCT_DEBUG
    std::cerr << "CheckExtension called with NULL string!\n";
#endif
    return Standard_False;
  }
  else if (caps->contextNoExtensions)
  {
    return Standard_False;
  }

  // available since OpenGL 3.0
  // and the ONLY way to check extensions with OpenGL 3.1+ core profile
  if (myGapi == Aspect_GraphicsLibrary_OpenGL && IsGlGreaterEqual(3, 0)
      && myFuncs->glGetStringi != NULL)
  {
    GLint anExtNb = 0;
    core11fwd->glGetIntegerv(GL_NUM_EXTENSIONS, &anExtNb);
    const size_t anExtNameLen = strlen(theExtName);
    for (GLint anIter = 0; anIter < anExtNb; ++anIter)
    {
      const char*  anExtension = (const char*)myFuncs->glGetStringi(GL_EXTENSIONS, (GLuint)anIter);
      const size_t aTestExtNameLen = strlen(anExtension);
      if (aTestExtNameLen == anExtNameLen && strncmp(anExtension, theExtName, anExtNameLen) == 0)
      {
        return Standard_True;
      }
    }
    return Standard_False;
  }

  // use old way with huge string for all extensions
  const char* anExtString = (const char*)core11fwd->glGetString(GL_EXTENSIONS);
  if (anExtString == NULL)
  {
    Messenger()->Send("TKOpenGL: glGetString (GL_EXTENSIONS) has returned NULL! No GL context?",
                      Message_Warning);
    return Standard_False;
  }
  if (!CheckExtension(anExtString, theExtName))
  {
    return Standard_False;
  }

#ifdef __EMSCRIPTEN__
  //! Check if WebGL extension is available and activate it
  //! (usage of extension without activation will generate errors).
  if (EMSCRIPTEN_WEBGL_CONTEXT_HANDLE aWebGlCtx = emscripten_webgl_get_current_context())
  {
    if (emscripten_webgl_enable_extension(aWebGlCtx, theExtName))
    {
      return Standard_True;
    }
  }
  return Standard_False;
#else
  return Standard_True;
#endif
}

// =======================================================================
// function : CheckExtension
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::CheckExtension(const char* theExtString, const char* theExtName)
{
  if (theExtString == NULL)
  {
    return Standard_False;
  }

  // Search for theExtName in the extensions string.
  // Use of strstr() is not sufficient because extension names can be prefixes of other extension
  // names.
  char*        aPtrIter     = (char*)theExtString;
  const char*  aPtrEnd      = aPtrIter + strlen(theExtString);
  const size_t anExtNameLen = strlen(theExtName);
  while (aPtrIter < aPtrEnd)
  {
    const size_t n = strcspn(aPtrIter, " ");
    if ((n == anExtNameLen) && (strncmp(aPtrIter, theExtName, anExtNameLen) == 0))
    {
      return Standard_True;
    }
    aPtrIter += (n + 1);
  }
  return Standard_False;
}

#if !defined(__APPLE__) || defined(HAVE_XLIB)

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::Init(const Standard_Boolean theIsCoreProfile)
{
  if (myIsInitialized)
  {
    return Standard_True;
  }

  #if defined(HAVE_EGL)
  myDisplay  = (Aspect_Display)eglGetCurrentDisplay();
  myGContext = (Aspect_RenderingContext)eglGetCurrentContext();
  myWindow   = (Aspect_Drawable)eglGetCurrentSurface(EGL_DRAW);
  #elif defined(_WIN32)
  myDisplay  = (Aspect_Handle)wglGetCurrentDC();
  myGContext = (Aspect_RenderingContext)wglGetCurrentContext();
  #elif defined(HAVE_XLIB)
  myDisplay  = (Aspect_Display)glXGetCurrentDisplay();
  myGContext = (Aspect_RenderingContext)glXGetCurrentContext();
  myWindow   = (Aspect_Drawable)glXGetCurrentDrawable();
  #else
    //
  #endif
  if (myGContext == NULL)
  {
    return Standard_False;
  }

  init(theIsCoreProfile);
  myIsInitialized = Standard_True;
  return Standard_True;
}

#endif // __APPLE__

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::Init(const Aspect_Drawable         theSurface,
                                      const Aspect_Display          theDisplay,
                                      const Aspect_RenderingContext theContext,
                                      const Standard_Boolean        theIsCoreProfile)
{
  Standard_ProgramError_Raise_if(myIsInitialized,
                                 "OpenGl_Context::Init() should be called only once!");
  myWindow   = theSurface;
  myDisplay  = theDisplay;
  myGContext = theContext;
  if (myGContext == NULL || !MakeCurrent())
  {
    return Standard_False;
  }

  init(theIsCoreProfile);
  myIsInitialized = Standard_True;
  return Standard_True;
}

// =======================================================================
// function : FormatGlEnumHex
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_Context::FormatGlEnumHex(int theGlEnum)
{
  char aBuff[16];
  Sprintf(aBuff,
          theGlEnum < (int)std::numeric_limits<uint16_t>::max() ? "0x%04X" : "0x%08X",
          theGlEnum);
  return aBuff;
}

// =======================================================================
// function : FormatSize
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_Context::FormatSize(Standard_Size theSize)
{
  char aBuff[32];
  Sprintf(aBuff, "%" PRIu64, (uint64_t)theSize);
  return aBuff;
}

// =======================================================================
// function : FormatPointer
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_Context::FormatPointer(const void* thePtr)
{
  char aBuff[32];
  Sprintf(aBuff, "0x%" PRIXPTR, (uintptr_t)thePtr);
  return aBuff;
}

// =======================================================================
// function : FormatGlError
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_Context::FormatGlError(int theGlError)
{
  switch (theGlError)
  {
    case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW:
      return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW:
      return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "GL_INVALID_FRAMEBUFFER_OPERATION";
  }
  return FormatGlEnumHex(theGlError);
}

// =======================================================================
// function : ResetErrors
// purpose  :
// =======================================================================
bool OpenGl_Context::ResetErrors(const bool theToPrintErrors)
{
  int        aPrevErr = 0;
  int        anErr    = core11fwd->glGetError();
  const bool hasError = anErr != GL_NO_ERROR;
  if (!theToPrintErrors)
  {
    for (; anErr != GL_NO_ERROR && aPrevErr != anErr;
         aPrevErr = anErr, anErr = core11fwd->glGetError())
    {
      //
    }
    return hasError;
  }

  for (; anErr != GL_NO_ERROR && aPrevErr != anErr;
       aPrevErr = anErr, anErr = core11fwd->glGetError())
  {
    const TCollection_ExtendedString aMsg =
      TCollection_ExtendedString("Unhandled GL error: ") + FormatGlError(anErr);
    PushMessage(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_OTHER, 0, GL_DEBUG_SEVERITY_LOW, aMsg);
  }
  return hasError;
}

// =======================================================================
// function : ReadGlVersion
// purpose  :
// =======================================================================
void OpenGl_Context::ReadGlVersion(Standard_Integer& theGlVerMajor, Standard_Integer& theGlVerMinor)
{
  OpenGl_GlFunctions::readGlVersion(theGlVerMajor, theGlVerMinor);
}

static Standard_CString THE_DBGMSG_UNKNOWN   = "UNKNOWN";
static Standard_CString THE_DBGMSG_SOURCES[] = {
  ".OpenGL",    // GL_DEBUG_SOURCE_API
  ".WinSystem", // GL_DEBUG_SOURCE_WINDOW_SYSTEM
  ".GLSL",      // GL_DEBUG_SOURCE_SHADER_COMPILER
  ".3rdParty",  // GL_DEBUG_SOURCE_THIRD_PARTY
  "",           // GL_DEBUG_SOURCE_APPLICATION
  ".Other"      // GL_DEBUG_SOURCE_OTHER
};

static Standard_CString THE_DBGMSG_TYPES[] = {
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
                                       int /*theLength*/,
                                       const char* theMessage,
                                       const void* theUserParam)
{
  OpenGl_Context* aCtx = (OpenGl_Context*)theUserParam;
  aCtx->PushMessage(theSource, theType, theId, theSeverity, theMessage);
}

// =======================================================================
// function : PushMessage
// purpose  :
// =======================================================================
void OpenGl_Context::PushMessage(const unsigned int                theSource,
                                 const unsigned int                theType,
                                 const unsigned int                theId,
                                 const unsigned int                theSeverity,
                                 const TCollection_ExtendedString& theMessage)
{
  if (caps->suppressExtraMsg && theSource >= GL_DEBUG_SOURCE_API
      && theSource <= GL_DEBUG_SOURCE_OTHER
      && myFilters[theSource - GL_DEBUG_SOURCE_API].Contains(theId))
  {
    return;
  }

  Standard_CString& aSrc  = (theSource >= GL_DEBUG_SOURCE_API && theSource <= GL_DEBUG_SOURCE_OTHER)
                              ? THE_DBGMSG_SOURCES[theSource - GL_DEBUG_SOURCE_API]
                              : THE_DBGMSG_UNKNOWN;
  Standard_CString& aType = (theType >= GL_DEBUG_TYPE_ERROR && theType <= GL_DEBUG_TYPE_OTHER)
                              ? THE_DBGMSG_TYPES[theType - GL_DEBUG_TYPE_ERROR]
                              : THE_DBGMSG_UNKNOWN;
  Standard_CString& aSev =
    theSeverity == GL_DEBUG_SEVERITY_HIGH
      ? THE_DBGMSG_SEV_HIGH
      : (theSeverity == GL_DEBUG_SEVERITY_MEDIUM ? THE_DBGMSG_SEV_MEDIUM : THE_DBGMSG_SEV_LOW);
  Message_Gravity aGrav =
    theSeverity == GL_DEBUG_SEVERITY_HIGH
      ? Message_Alarm
      : (theSeverity == GL_DEBUG_SEVERITY_MEDIUM ? Message_Warning : Message_Info);

  TCollection_ExtendedString aMsg;
  aMsg += "TKOpenGl";
  aMsg += aSrc;
  aMsg += " | Type: ";
  aMsg += aType;
  aMsg += " | ID: ";
  aMsg += (Standard_Integer)theId;
  aMsg += " | Severity: ";
  aMsg += aSev;
  aMsg += " | Message:\n  ";
  aMsg += theMessage;
  Messenger()->Send(aMsg, aGrav);
}

// =======================================================================
// function : ExcludeMessage
// purpose  :
// ======================================================================
Standard_Boolean OpenGl_Context::ExcludeMessage(const unsigned int theSource,
                                                const unsigned int theId)
{
  return theSource >= GL_DEBUG_SOURCE_API && theSource <= GL_DEBUG_SOURCE_OTHER
         && myFilters[theSource - GL_DEBUG_SOURCE_API].Add(theId);
}

// =======================================================================
// function : IncludeMessage
// purpose  :
// ======================================================================
Standard_Boolean OpenGl_Context::IncludeMessage(const unsigned int theSource,
                                                const unsigned int theId)
{
  return theSource >= GL_DEBUG_SOURCE_API && theSource <= GL_DEBUG_SOURCE_OTHER
         && myFilters[theSource - GL_DEBUG_SOURCE_API].Remove(theId);
}

// =======================================================================
// function : checkWrongVersion
// purpose  :
// ======================================================================
void OpenGl_Context::checkWrongVersion(Standard_Integer theGlVerMajor,
                                       Standard_Integer theGlVerMinor,
                                       const char*      theLastFailedProc)
{
  if (!IsGlGreaterEqual(theGlVerMajor, theGlVerMinor))
  {
    return;
  }

  PushMessage(GL_DEBUG_SOURCE_APPLICATION,
              GL_DEBUG_TYPE_ERROR,
              0,
              GL_DEBUG_SEVERITY_HIGH,
              TCollection_AsciiString() + "Error! OpenGL context reports version " + myGlVerMajor
                + "." + myGlVerMinor + " but does not export required functions for "
                + theGlVerMajor + "." + theGlVerMinor + " ("
                + (theLastFailedProc != NULL ? theLastFailedProc : "") + ")\n"
                + "Please report this issue to OpenGL driver vendor '" + myVendor + "'");

  // lower internal version
  if (theGlVerMinor > 0)
  {
    myGlVerMajor = theGlVerMajor;
    myGlVerMinor = theGlVerMinor - 1;
    return;
  }

  if (myGapi == Aspect_GraphicsLibrary_OpenGLES)
  {
    switch (theGlVerMajor)
    {
      case 3:
        myGlVerMajor = 2;
        myGlVerMinor = 0;
        return;
    }
  }
  else
  {
    switch (theGlVerMajor)
    {
      case 2:
        myGlVerMajor = 1;
        myGlVerMinor = 5;
        return;
      case 3:
        myGlVerMajor = 2;
        myGlVerMinor = 1;
        return;
      case 4:
        myGlVerMajor = 3;
        myGlVerMinor = 3;
        return;
    }
  }
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
void OpenGl_Context::init(const Standard_Boolean theIsCoreProfile)
{
  // read version
  myGlVerMajor          = 0;
  myGlVerMinor          = 0;
  myHasMsaaTextures     = false;
  myMaxMsaaSamples      = 0;
  myMaxDrawBuffers      = 1;
  myMaxColorAttachments = 1;
  myDefaultVao          = 0;
  OpenGl_GlFunctions::readGlVersion(myGlVerMajor, myGlVerMinor);
  mySupportedFormats->Clear();

  if (caps->contextMajorVersionUpper != -1)
  {
    // synthetically restrict OpenGL version for testing
    Standard_Integer aCtxVer[2] = {myGlVerMajor, myGlVerMinor};
    bool             isLowered  = false;
    if (myGlVerMajor > caps->contextMajorVersionUpper)
    {
      isLowered    = true;
      myGlVerMajor = caps->contextMajorVersionUpper;
      if (myGapi == Aspect_GraphicsLibrary_OpenGLES)
      {
        switch (myGlVerMajor)
        {
          case 2:
            myGlVerMinor = 0;
            break;
        }
      }
      else
      {
        switch (myGlVerMajor)
        {
          case 1:
            myGlVerMinor = 5;
            break;
          case 2:
            myGlVerMinor = 1;
            break;
          case 3:
            myGlVerMinor = 3;
            break;
        }
      }
    }
    if (caps->contextMinorVersionUpper != -1 && myGlVerMinor > caps->contextMinorVersionUpper)
    {
      isLowered    = true;
      myGlVerMinor = caps->contextMinorVersionUpper;
    }
    if (isLowered)
    {
      PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                  GL_DEBUG_TYPE_PORTABILITY,
                  0,
                  GL_DEBUG_SEVERITY_MEDIUM,
                  TCollection_AsciiString("OpenGL version ") + aCtxVer[0] + "." + aCtxVer[1]
                    + " has been lowered to " + myGlVerMajor + "." + myGlVerMinor);
    }
  }

  myFuncs->load(*this, theIsCoreProfile);

  if (!caps->ffpEnable && !IsGlGreaterEqual(2, 0))
  {
    caps->ffpEnable = true;
    TCollection_ExtendedString aMsg =
      TCollection_ExtendedString("OpenGL driver is too old! Context info:\n")
      + "    Vendor:   " + (const char*)core11fwd->glGetString(GL_VENDOR) + "\n"
      + "    Renderer: " + (const char*)core11fwd->glGetString(GL_RENDERER) + "\n"
      + "    Version:  " + (const char*)core11fwd->glGetString(GL_VERSION) + "\n"
      + "  Fallback using deprecated fixed-function pipeline.\n"
      + "  Visualization might work incorrectly.\n"
        "  Consider upgrading the graphics driver.";
    PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                GL_DEBUG_TYPE_PORTABILITY,
                0,
                GL_DEBUG_SEVERITY_HIGH,
                aMsg);
  }

  myVendor = (const char*)core11fwd->glGetString(GL_VENDOR);
  myVendor.LowerCase();
  if (myVendor.Search("nvidia") != -1)
  {
    // Buffer detailed info: Buffer object 1 (bound to GL_ARRAY_BUFFER_ARB, usage hint is
    // GL_STATIC_DRAW) will use VIDEO memory as the source for buffer object operations.
    ExcludeMessage(GL_DEBUG_SOURCE_API, 131185);
  }

  // setup shader generator
  myShaderManager->SetGapiVersion(myGlVerMajor, myGlVerMinor);
  myShaderManager->SetEmulateDepthClamp(!arbDepthClamp);

  // workaround Adreno driver bug computing reversed normal using dFdx/dFdy
  bool toReverseDFdxSign =
    myGapi == Aspect_GraphicsLibrary_OpenGLES && myVendor.Search("qualcomm") != -1;
  myShaderManager->SetFlatShading(hasFlatShading != OpenGl_FeatureNotAvailable, toReverseDFdxSign);
  myShaderManager->SetUseRedAlpha(myGapi != Aspect_GraphicsLibrary_OpenGLES && core11ffp == NULL);
#define checkGlslExtensionShort(theName)                                                           \
  myShaderManager->EnableGlslExtension(Graphic3d_GlslExtension_##theName, CheckExtension(#theName))
  if (myGapi == Aspect_GraphicsLibrary_OpenGLES)
  {
    checkGlslExtensionShort(GL_OES_standard_derivatives);
    checkGlslExtensionShort(GL_EXT_shader_texture_lod);
    checkGlslExtensionShort(GL_EXT_frag_depth);
  }
  else
  {
    checkGlslExtensionShort(GL_EXT_gpu_shader4);
  }

  // initialize debug context extension
  if (arbDbg != NULL && caps->contextDebug)
  {
    // setup default callback
    myIsGlDebugCtx = Standard_True;
    arbDbg->glDebugMessageCallback(debugCallbackWrap, this);
    if (myGapi == Aspect_GraphicsLibrary_OpenGLES)
    {
      core11fwd->glEnable(GL_DEBUG_OUTPUT);
    }
    else if (core43 != NULL)
    {
      core11fwd->glEnable(GL_DEBUG_OUTPUT);
    }
    if (caps->contextSyncDebug)
    {
      // note that some broken implementations (e.g. simulators) might generate error message on
      // this call
      core11fwd->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }
  }

  if (hasDrawBuffers)
  {
    core11fwd->glGetIntegerv(GL_MAX_DRAW_BUFFERS, &myMaxDrawBuffers);
    core11fwd->glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &myMaxColorAttachments);
    if (myDrawBuffers.Length() < myMaxDrawBuffers)
    {
      myDrawBuffers.Resize(0, myMaxDrawBuffers - 1, false);
    }
  }

  core11fwd->glGetIntegerv(GL_MAX_TEXTURE_SIZE, &myMaxTexDim);
  if (IsGlGreaterEqual(1, 3) && core11ffp != NULL)
  {
    // this is a maximum of texture units for FFP functionality,
    // usually smaller than combined texture units available for GLSL
    core11fwd->glGetIntegerv(GL_MAX_TEXTURE_UNITS, &myMaxTexUnitsFFP);
    myMaxTexCombined = myMaxTexUnitsFFP;
  }
  if (IsGlGreaterEqual(2, 0))
  {
    core11fwd->glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &myMaxTexCombined);
  }
  mySpriteTexUnit =
    myMaxTexCombined >= 2 ? Graphic3d_TextureUnit_PointSprite : Graphic3d_TextureUnit_0;

  GLint aMaxVPortSize[2] = {0, 0};
  core11fwd->glGetIntegerv(GL_MAX_VIEWPORT_DIMS, aMaxVPortSize);
  myMaxDumpSizeX = Min(aMaxVPortSize[0], myMaxTexDim);
  myMaxDumpSizeY = Min(aMaxVPortSize[1], myMaxTexDim);
  if (myVendor == "intel")
  {
    // Intel drivers have known bug with empty dump for images with width>=5462
    myMaxDumpSizeX = Min(myMaxDumpSizeX, 4096);
  }

  if (extAnis)
  {
    core11fwd->glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &myAnisoMax);
  }

  myClippingState.Init();

  if (myGapi == Aspect_GraphicsLibrary_OpenGLES)
  {
    if (IsGlGreaterEqual(3, 0))
    {
      // MSAA RenderBuffers have been defined in OpenGL ES 3.0, but MSAA Textures - only in OpenGL
      // ES 3.1+
      myHasMsaaTextures = IsGlGreaterEqual(3, 1) && myFuncs->glTexStorage2DMultisample != NULL;
      core11fwd->glGetIntegerv(GL_MAX_SAMPLES, &myMaxMsaaSamples);
    }
  }
  else if (core30 != NULL)
  {
    // MSAA RenderBuffers have been defined in OpenGL 3.0, but MSAA Textures - only in OpenGL 3.2+
    if (core32 != NULL)
    {
      myHasMsaaTextures = true;
      core11fwd->glGetIntegerv(GL_MAX_SAMPLES, &myMaxMsaaSamples);
    }
    else if (CheckExtension("GL_ARB_texture_multisample")
             && myFuncs->glTexImage2DMultisample != NULL)
    {
      myHasMsaaTextures     = true;
      GLint aNbColorSamples = 0, aNbDepthSamples = 0;
      core11fwd->glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &aNbColorSamples);
      core11fwd->glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &aNbDepthSamples);
      myMaxMsaaSamples = Min(aNbColorSamples, aNbDepthSamples);
    }
  }
  if (myMaxMsaaSamples <= 1)
  {
    myHasMsaaTextures = false;
  }

  if (myGapi != Aspect_GraphicsLibrary_OpenGLES)
  {
    if (core32 != NULL && core11ffp == NULL)
    {
      core32->glGenVertexArrays(1, &myDefaultVao);
    }

    myTexClamp = IsGlGreaterEqual(1, 2) ? GL_CLAMP_TO_EDGE : GL_CLAMP;

    GLint aStereo = GL_FALSE;
    core11fwd->glGetIntegerv(GL_STEREO, &aStereo);
    myIsStereoBuffers = aStereo == 1;

    // get number of maximum clipping planes
    core11fwd->glGetIntegerv(GL_MAX_CLIP_PLANES, &myMaxClipPlanes);
  }

  if (myGapi == Aspect_GraphicsLibrary_OpenGLES)
  {
    // check whether ray tracing mode is supported
    myHasRayTracing                         = IsGlGreaterEqual(3, 2);
    myHasRayTracingTextures                 = myHasRayTracingAdaptiveSampling =
      myHasRayTracingAdaptiveSamplingAtomic = false;
  }
  else
  {
    // check whether ray tracing mode is supported
    myHasRayTracing = IsGlGreaterEqual(3, 1) && arbTboRGB32 && arbFBOBlit != NULL;

    // check whether textures in ray tracing mode are supported
    myHasRayTracingTextures = myHasRayTracing && arbTexBindless != NULL;

    // check whether adaptive screen sampling in ray tracing mode is supported
    myHasRayTracingAdaptiveSampling = myHasRayTracing && core44 != NULL;
    myHasRayTracingAdaptiveSamplingAtomic =
      myHasRayTracingAdaptiveSampling && CheckExtension("GL_NV_shader_atomic_float");
  }

  if (arbFBO != NULL && hasFboSRGB)
  {
    // Detect if window buffer is considered by OpenGL as sRGB-ready
    // (linear RGB color written by shader is automatically converted into sRGB)
    // or not (offscreen FBO should be blit into window buffer with gamma correction).
    const GLenum aDefWinBuffer = myGapi == Aspect_GraphicsLibrary_OpenGLES ? GL_BACK : GL_BACK_LEFT;
    GLint        aWinColorEncoding = 0; // GL_LINEAR
    bool         toSkipCheck       = false;
    if (myGapi == Aspect_GraphicsLibrary_OpenGLES)
    {
      toSkipCheck = !IsGlGreaterEqual(3, 0);
    }
    if (!toSkipCheck)
    {
      arbFBO->glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER,
                                                    aDefWinBuffer,
                                                    GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING,
                                                    &aWinColorEncoding);
      ResetErrors(true);
    }
    myIsSRgbWindow = aWinColorEncoding == GL_SRGB;

    // On desktop OpenGL, pixel formats are almost always sRGB-ready, even when not requested;
    // it is safe behavior on desktop where GL_FRAMEBUFFER_SRGB is disabled by default
    // (contrary to OpenGL ES, where it is enabled by default).
    // NVIDIA drivers, however, always return GL_LINEAR even for sRGB-ready pixel formats on Windows
    // platform, while AMD and Intel report GL_SRGB as expected. macOS drivers seems to be also
    // report GL_LINEAR even for [NSColorSpace sRGBColorSpace].
    if (myGapi != Aspect_GraphicsLibrary_OpenGLES)
    {
#ifdef __APPLE__
      myIsSRgbWindow = true;
#else
      if (!myIsSRgbWindow && myVendor.Search("nvidia") != -1)
      {
        myIsSRgbWindow = true;
      }
#endif
    }
    if (!myIsSRgbWindow)
    {
      Message::SendTrace("OpenGl_Context, warning: window buffer is not sRGB-ready.\n"
                         "Check OpenGL window creation parameters for optimal performance.");
    }
  }

  Graphic3d_Vec4i aWinBitsRGBA;
  Graphic3d_Vec2i aWinBitsDepthStencil;
  WindowBufferBits(aWinBitsRGBA, aWinBitsDepthStencil);
  myIsWindowDeepColor = aWinBitsRGBA.r() >= 10;

  // standard formats
  mySupportedFormats->Add(Image_Format_Gray);
  mySupportedFormats->Add(Image_Format_Alpha);
  mySupportedFormats->Add(Image_Format_RGB);
  mySupportedFormats->Add(Image_Format_RGB32);
  mySupportedFormats->Add(Image_Format_RGBA);
  if (extBgra)
  {
    if (myGapi != Aspect_GraphicsLibrary_OpenGLES)
    {
      // no BGR on OpenGL ES - only BGRA as extension
      mySupportedFormats->Add(Image_Format_BGR);
    }
    mySupportedFormats->Add(Image_Format_BGR32);
    mySupportedFormats->Add(Image_Format_BGRA);
  }
  if (extTexR16)
  {
    mySupportedFormats->Add(Image_Format_Gray16);
  }
  if (arbTexFloat)
  {
    mySupportedFormats->Add(Image_Format_GrayF);
    mySupportedFormats->Add(Image_Format_AlphaF);
    mySupportedFormats->Add(Image_Format_RGBF);
    mySupportedFormats->Add(Image_Format_RGBAF);
    if (hasHalfFloatBuffer != OpenGl_FeatureNotAvailable)
    {
      mySupportedFormats->Add(Image_Format_RGBAF_half);
    }
    if (arbTexRG)
    {
      mySupportedFormats->Add(Image_Format_RGF);
      if (hasHalfFloatBuffer != OpenGl_FeatureNotAvailable)
      {
        mySupportedFormats->Add(Image_Format_GrayF_half);
        mySupportedFormats->Add(Image_Format_RGF_half);
      }
    }
    if (extBgra)
    {
      if (myGapi != Aspect_GraphicsLibrary_OpenGLES)
      {
        mySupportedFormats->Add(Image_Format_BGRF);
      }
      mySupportedFormats->Add(Image_Format_BGRAF);
    }
  }

#ifdef __EMSCRIPTEN__
  if (CheckExtension("GL_WEBGL_compressed_texture_s3tc")) // GL_WEBGL_compressed_texture_s3tc_srgb
                                                          // for sRGB formats
  {
    mySupportedFormats->Add(Image_CompressedFormat_RGB_S3TC_DXT1);
    mySupportedFormats->Add(Image_CompressedFormat_RGBA_S3TC_DXT1);
    mySupportedFormats->Add(Image_CompressedFormat_RGBA_S3TC_DXT3);
    mySupportedFormats->Add(Image_CompressedFormat_RGBA_S3TC_DXT5);
  }
  if (!extPDS && CheckExtension("GL_WEBGL_depth_texture"))
  {
    extPDS = true; // WebGL 1.0 extension (in WebGL 2.0 core)
  }
#else
  if (CheckExtension("GL_EXT_texture_compression_s3tc")) // GL_EXT_texture_sRGB for sRGB formats
  {
    mySupportedFormats->Add(Image_CompressedFormat_RGB_S3TC_DXT1);
    mySupportedFormats->Add(Image_CompressedFormat_RGBA_S3TC_DXT1);
    mySupportedFormats->Add(Image_CompressedFormat_RGBA_S3TC_DXT3);
    mySupportedFormats->Add(Image_CompressedFormat_RGBA_S3TC_DXT5);
  }
  else
  {
    if (CheckExtension("GL_EXT_texture_compression_dxt1"))
    {
      mySupportedFormats->Add(Image_CompressedFormat_RGB_S3TC_DXT1);
      mySupportedFormats->Add(Image_CompressedFormat_RGBA_S3TC_DXT1);
    }
    if (CheckExtension("GL_ANGLE_texture_compression_dxt3"))
    {
      mySupportedFormats->Add(Image_CompressedFormat_RGBA_S3TC_DXT3);
    }
    if (CheckExtension("GL_ANGLE_texture_compression_dxt5"))
    {
      mySupportedFormats->Add(Image_CompressedFormat_RGBA_S3TC_DXT5);
    }
  }
#endif

  // check whether PBR shading model is supported
  myHasPBR = false;
  if (arbFBO != NULL && myMaxTexCombined >= 4 && arbTexFloat)
  {
    if (myGapi == Aspect_GraphicsLibrary_OpenGLES)
    {
      myHasPBR = IsGlGreaterEqual(3, 0) || hasHighp;
      // || CheckExtension ("GL_EXT_shader_texture_lod") fallback is used when extension is
      // unavailable
    }
    else
    {
      myHasPBR =
        IsGlGreaterEqual(3, 0) || (IsGlGreaterEqual(2, 1) && CheckExtension("GL_EXT_gpu_shader4"));
    }
  }

  // clang-format off
  myDepthPeelingDepthTexUnit      = static_cast<Graphic3d_TextureUnit>(myMaxTexCombined + Graphic3d_TextureUnit_DepthPeelingDepth);      // -6
  myDepthPeelingFrontColorTexUnit = static_cast<Graphic3d_TextureUnit>(myMaxTexCombined + Graphic3d_TextureUnit_DepthPeelingFrontColor); // -5
  myShadowMapTexUnit              = static_cast<Graphic3d_TextureUnit>(myMaxTexCombined + Graphic3d_TextureUnit_ShadowMap);              // -4
  myPBREnvLUTTexUnit              = static_cast<Graphic3d_TextureUnit>(myMaxTexCombined + Graphic3d_TextureUnit_PbrEnvironmentLUT);      // -3
  myPBRDiffIBLMapSHTexUnit        = static_cast<Graphic3d_TextureUnit>(myMaxTexCombined + Graphic3d_TextureUnit_PbrIblDiffuseSH);        // -2
  myPBRSpecIBLMapTexUnit          = static_cast<Graphic3d_TextureUnit>(myMaxTexCombined + Graphic3d_TextureUnit_PbrIblSpecular);         // -1
  // clang-format on
  if (!myHasPBR)
  {
    myDepthPeelingDepthTexUnit = static_cast<Graphic3d_TextureUnit>(myDepthPeelingDepthTexUnit + 3);
    myDepthPeelingFrontColorTexUnit =
      static_cast<Graphic3d_TextureUnit>(myDepthPeelingFrontColorTexUnit + 3);
    myShadowMapTexUnit = static_cast<Graphic3d_TextureUnit>(myShadowMapTexUnit + 3);
  }
}

// =======================================================================
// function : MemoryInfo
// purpose  :
// =======================================================================
Standard_Size OpenGl_Context::AvailableMemory() const
{
  if (atiMem)
  {
    // this is actually information for VBO pool
    // however because pools are mostly shared
    // it can be used for total GPU memory estimations
    GLint aMemInfo[4];
    aMemInfo[0] = 0;

    core11fwd->glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, aMemInfo);
    // returned value is in KiB, however this maybe changed in future
    return Standard_Size(aMemInfo[0]) * 1024;
  }
  else if (nvxMem)
  {
    // current available dedicated video memory (in KiB), currently unused GPU memory
    GLint aMemInfo = 0;
    core11fwd->glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &aMemInfo);
    return Standard_Size(aMemInfo) * 1024;
  }
  return 0;
}

// =======================================================================
// function : MemoryInfo
// purpose  :
// =======================================================================
TCollection_AsciiString OpenGl_Context::MemoryInfo() const
{
  TColStd_IndexedDataMapOfStringString aDict;
  MemoryInfo(aDict);

  TCollection_AsciiString aText;
  for (TColStd_IndexedDataMapOfStringString::Iterator anIter(aDict); anIter.More(); anIter.Next())
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
void OpenGl_Context::MemoryInfo(TColStd_IndexedDataMapOfStringString& theDict) const
{
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
  (void)theDict;
#elif defined(__APPLE__) && !defined(HAVE_XLIB)
  GLint aGlRendId = 0;
  CGLGetParameter(CGLGetCurrentContext(), kCGLCPCurrentRendererID, &aGlRendId);

  CGLRendererInfoObj  aRendObj  = NULL;
  CGOpenGLDisplayMask aDispMask = CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay);
  GLint               aRendNb   = 0;
  CGLQueryRendererInfo(aDispMask, &aRendObj, &aRendNb);
  for (GLint aRendIter = 0; aRendIter < aRendNb; ++aRendIter)
  {
    GLint aRendId = 0;
    if (CGLDescribeRenderer(aRendObj, aRendIter, kCGLRPRendererID, &aRendId) != kCGLNoError
        || aRendId != aGlRendId)
    {
      continue;
    }

    // kCGLRPVideoMemoryMegabytes   = 131;
    // kCGLRPTextureMemoryMegabytes = 132;
    GLint aVMem = 0;
  #if MAC_OS_X_VERSION_MIN_REQUIRED >= 1070
    if (CGLDescribeRenderer(aRendObj, aRendIter, kCGLRPVideoMemoryMegabytes, &aVMem) == kCGLNoError)
    {
      addInfo(theDict, "GPU memory", TCollection_AsciiString() + aVMem + " MiB");
    }
    if (CGLDescribeRenderer(aRendObj, aRendIter, kCGLRPTextureMemoryMegabytes, &aVMem)
        == kCGLNoError)
    {
      addInfo(theDict, "GPU Texture memory", TCollection_AsciiString() + aVMem + " MiB");
    }
  #else
    if (CGLDescribeRenderer(aRendObj, aRendIter, kCGLRPVideoMemory, &aVMem) == kCGLNoError)
    {
      addInfo(theDict, "GPU memory", TCollection_AsciiString() + (aVMem / (1024 * 1024)) + " MiB");
    }
    if (CGLDescribeRenderer(aRendObj, aRendIter, kCGLRPTextureMemory, &aVMem) == kCGLNoError)
    {
      addInfo(theDict,
              "GPU Texture memory",
              TCollection_AsciiString() + (aVMem / (1024 * 1024)) + " MiB");
    }
  #endif
  }
#endif

  if (atiMem)
  {
    GLint aValues[4];
    memset(aValues, 0, sizeof(aValues));
    core11fwd->glGetIntegerv(GL_VBO_FREE_MEMORY_ATI, aValues);

    // total memory free in the pool
    addInfo(theDict, "GPU free memory", TCollection_AsciiString() + (aValues[0] / 1024) + " MiB");

    if (aValues[1] != aValues[0])
    {
      // largest available free block in the pool
      addInfo(theDict,
              "Largest free block",
              TCollection_AsciiString() + (aValues[1] / 1024) + " MiB");
    }
    if (aValues[2] != aValues[0])
    {
      // total auxiliary memory free
      addInfo(theDict,
              "Free auxiliary memory",
              TCollection_AsciiString() + (aValues[2] / 1024) + " MiB");
    }
  }
  else if (nvxMem)
  {
    // current available dedicated video memory (in KiB), currently unused GPU memory
    GLint aValue = 0;
    core11fwd->glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &aValue);
    addInfo(theDict, "GPU free memory", TCollection_AsciiString() + (aValue / 1024) + " MiB");

    // dedicated video memory, total size (in KiB) of the GPU memory
    GLint aDedicated = 0;
    core11fwd->glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &aDedicated);
    addInfo(theDict, "GPU memory", TCollection_AsciiString() + (aDedicated / 1024) + " MiB");

    // total available memory, total size (in KiB) of the memory available for allocations
    core11fwd->glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &aValue);
    if (aValue != aDedicated)
    {
      // different only for special configurations
      addInfo(theDict, "Total memory", TCollection_AsciiString() + (aValue / 1024) + " MiB");
    }
  }
#if defined(_WIN32)
  else if (myFuncs->wglGetGPUInfoAMD != NULL && myFuncs->wglGetContextGPUIDAMD != NULL)
  {
    GLuint aTotalMemMiB = 0;
    UINT   anAmdId      = myFuncs->wglGetContextGPUIDAMD((HGLRC)myGContext);
    if (anAmdId != 0)
    {
      if (myFuncs->wglGetGPUInfoAMD(anAmdId,
                                    WGL_GPU_RAM_AMD,
                                    GL_UNSIGNED_INT,
                                    sizeof(aTotalMemMiB),
                                    &aTotalMemMiB)
          > 0)
      {
        addInfo(theDict, "GPU memory", TCollection_AsciiString() + (int)aTotalMemMiB + " MiB");
      }
    }
  }
#endif

#if defined(HAVE_XLIB) && !defined(__APPLE__) && !defined(_WIN32)
  // GLX_RENDERER_VENDOR_ID_MESA
  if (myFuncs->glXQueryCurrentRendererIntegerMESA != NULL)
  {
    unsigned int aVMemMiB = 0;
    if (myFuncs->glXQueryCurrentRendererIntegerMESA(GLX_RENDERER_VIDEO_MEMORY_MESA, &aVMemMiB) != 0)
    {
      addInfo(theDict, "GPU memory", TCollection_AsciiString() + int(aVMemMiB) + " MiB");
    }
  }
#endif
}

// =======================================================================
// function : WindowBufferBits
// purpose  :
// =======================================================================
void OpenGl_Context::WindowBufferBits(Graphic3d_Vec4i& theColorBits,
                                      Graphic3d_Vec2i& theDepthStencilBits) const
{
  if (core11ffp != NULL || myGapi == Aspect_GraphicsLibrary_OpenGLES)
  {
    // removed from core with no working alternative
    core11fwd->glGetIntegerv(GL_RED_BITS, &theColorBits.r());
    core11fwd->glGetIntegerv(GL_GREEN_BITS, &theColorBits.g());
    core11fwd->glGetIntegerv(GL_BLUE_BITS, &theColorBits.b());
    core11fwd->glGetIntegerv(GL_ALPHA_BITS, &theColorBits.a());
    core11fwd->glGetIntegerv(GL_DEPTH_BITS, &theDepthStencilBits[0]);
    core11fwd->glGetIntegerv(GL_STENCIL_BITS, &theDepthStencilBits[1]);
  }
  else
  {
#if defined(HAVE_EGL)
    //
#elif defined(_WIN32)
    const int             aPixFrmtIndex = GetPixelFormat((HDC)myDisplay);
    PIXELFORMATDESCRIPTOR aFormat;
    memset(&aFormat, 0, sizeof(aFormat));
    aFormat.nSize = sizeof(aFormat);
    DescribePixelFormat((HDC)myDisplay, aPixFrmtIndex, sizeof(PIXELFORMATDESCRIPTOR), &aFormat);
    theColorBits.SetValues(aFormat.cRedBits,
                           aFormat.cGreenBits,
                           aFormat.cBlueBits,
                           aFormat.cAlphaBits);
    theDepthStencilBits.SetValues(aFormat.cDepthBits, aFormat.cStencilBits);
#elif defined(HAVE_XLIB)
    Display*          aDisplay = (Display*)myDisplay;
    XWindowAttributes aWinAttribs;
    XGetWindowAttributes(aDisplay, (::Window)myWindow, &aWinAttribs);
    XVisualInfo aVisInfo;
    aVisInfo.visualid                                     = aWinAttribs.visual->visualid;
    aVisInfo.screen                                       = DefaultScreen(aDisplay);
    int                                          aNbItems = 0;
    std::unique_ptr<XVisualInfo, int (*)(void*)> aVis(
      XGetVisualInfo(aDisplay, VisualIDMask | VisualScreenMask, &aVisInfo, &aNbItems),
      &XFree);
    if (aVis.get() != NULL)
    {
      glXGetConfig(aDisplay, aVis.get(), GLX_RED_SIZE, &theColorBits.r());
      glXGetConfig(aDisplay, aVis.get(), GLX_GREEN_SIZE, &theColorBits.g());
      glXGetConfig(aDisplay, aVis.get(), GLX_BLUE_SIZE, &theColorBits.b());
      glXGetConfig(aDisplay, aVis.get(), GLX_ALPHA_SIZE, &theColorBits.a());
      glXGetConfig(aDisplay, aVis.get(), GLX_DEPTH_SIZE, &theDepthStencilBits[0]);
      glXGetConfig(aDisplay, aVis.get(), GLX_STENCIL_SIZE, &theDepthStencilBits[1]);
    }
#endif
  }
}

// =======================================================================
// function : DiagnosticInfo
// purpose  :
// =======================================================================
void OpenGl_Context::DiagnosticInformation(TColStd_IndexedDataMapOfStringString& theDict,
                                           Graphic3d_DiagnosticInfo              theFlags) const
{
  if ((theFlags & Graphic3d_DiagnosticInfo_NativePlatform) != 0)
  {
#if defined(HAVE_EGL)
    addInfo(theDict, "EGLVersion", ::eglQueryString((EGLDisplay)myDisplay, EGL_VERSION));
    addInfo(theDict, "EGLVendor", ::eglQueryString((EGLDisplay)myDisplay, EGL_VENDOR));
    addInfo(theDict, "EGLClientAPIs", ::eglQueryString((EGLDisplay)myDisplay, EGL_CLIENT_APIS));
    if ((theFlags & Graphic3d_DiagnosticInfo_Extensions) != 0)
    {
      addInfo(theDict, "EGLExtensions", ::eglQueryString((EGLDisplay)myDisplay, EGL_EXTENSIONS));
    }
#elif defined(_WIN32)
    if ((theFlags & Graphic3d_DiagnosticInfo_Extensions) != 0
        && myFuncs->wglGetExtensionsStringARB != NULL)
    {
      const char* aWglExts = myFuncs->wglGetExtensionsStringARB((HDC)myDisplay);
      addInfo(theDict, "WGLExtensions", aWglExts);
    }
#elif defined(HAVE_XLIB)
    Display*  aDisplay = (Display*)myDisplay;
    const int aScreen  = DefaultScreen(aDisplay);
    addInfo(theDict,
            "GLXDirectRendering",
            ::glXIsDirect(aDisplay, (GLXContext)myGContext) ? "Yes" : "No");
    addInfo(theDict, "GLXVendor", ::glXQueryServerString(aDisplay, aScreen, GLX_VENDOR));
    addInfo(theDict, "GLXVersion", ::glXQueryServerString(aDisplay, aScreen, GLX_VERSION));
    if ((theFlags & Graphic3d_DiagnosticInfo_Extensions) != 0)
    {
      const char* aGlxExts = ::glXQueryExtensionsString(aDisplay, aScreen);
      addInfo(theDict, "GLXExtensions", aGlxExts);
    }

    addInfo(theDict, "GLXClientVendor", ::glXGetClientString(aDisplay, GLX_VENDOR));
    addInfo(theDict, "GLXClientVersion", ::glXGetClientString(aDisplay, GLX_VERSION));
    if ((theFlags & Graphic3d_DiagnosticInfo_Extensions) != 0)
    {
      addInfo(theDict, "GLXClientExtensions", ::glXGetClientString(aDisplay, GLX_EXTENSIONS));
    }
#else
    //
#endif
  }

  if ((theFlags & Graphic3d_DiagnosticInfo_Device) != 0)
  {
    Standard_Integer aDriverVer[2] = {};
    OpenGl_GlFunctions::readGlVersion(aDriverVer[0], aDriverVer[1]);
    addInfo(theDict, "GLvendor", (const char*)core11fwd->glGetString(GL_VENDOR));
    addInfo(theDict, "GLdevice", (const char*)core11fwd->glGetString(GL_RENDERER));
#ifdef __EMSCRIPTEN__
    if (CheckExtension("GL_WEBGL_debug_renderer_info"))
    {
      if (const char* aVendor = (const char*)core11fwd->glGetString(0x9245))
      {
        addInfo(theDict, "GLunmaskedVendor", aVendor);
      }
      if (const char* aDevice = (const char*)core11fwd->glGetString(0x9246))
      {
        addInfo(theDict, "GLunmaskedDevice", aDevice);
      }
    }
#endif

    addInfo(theDict, "GLversion", (const char*)core11fwd->glGetString(GL_VERSION));
    if (myGlVerMajor != aDriverVer[0] || myGlVerMinor != aDriverVer[1])
    {
      addInfo(theDict,
              "GLversionOcct",
              TCollection_AsciiString(myGlVerMajor) + "." + TCollection_AsciiString(myGlVerMinor));
    }
    if (IsGlGreaterEqual(2, 0))
    {
      addInfo(theDict,
              "GLSLversion",
              (const char*)core11fwd->glGetString(GL_SHADING_LANGUAGE_VERSION));
    }
    if (myIsGlDebugCtx)
    {
      addInfo(theDict, "GLdebug", "ON");
    }
  }

  if ((theFlags & Graphic3d_DiagnosticInfo_Limits) != 0)
  {
    addInfo(theDict, "Max texture size", TCollection_AsciiString(myMaxTexDim));
    addInfo(theDict,
            "Max FBO dump size",
            TCollection_AsciiString() + myMaxDumpSizeX + "x" + myMaxDumpSizeY);
    addInfo(theDict, "Max combined texture units", TCollection_AsciiString(myMaxTexCombined));
    addInfo(theDict, "Max MSAA samples", TCollection_AsciiString(myMaxMsaaSamples));
  }

  if ((theFlags & Graphic3d_DiagnosticInfo_FrameBuffer) != 0)
  {
    GLint aViewport[4] = {};
    core11fwd->glGetIntegerv(GL_VIEWPORT, aViewport);
    addInfo(theDict, "Viewport", TCollection_AsciiString() + aViewport[2] + "x" + aViewport[3]);

    Graphic3d_Vec4i aWinBitsRGBA;
    Graphic3d_Vec2i aWinBitsDepthStencil;
    WindowBufferBits(aWinBitsRGBA, aWinBitsDepthStencil);
    addInfo(theDict,
            "Window buffer",
            TCollection_AsciiString() + "RGB" + aWinBitsRGBA.r() + " ALPHA" + aWinBitsRGBA.a()
              + " DEPTH" + aWinBitsDepthStencil[0] + " STENCIL" + aWinBitsDepthStencil[1]);
  }

  if ((theFlags & Graphic3d_DiagnosticInfo_Memory) != 0)
  {
    MemoryInfo(theDict);
  }

  if ((theFlags & Graphic3d_DiagnosticInfo_Extensions) != 0)
  {
    if (myGapi != Aspect_GraphicsLibrary_OpenGLES && IsGlGreaterEqual(3, 0)
        && myFuncs->glGetStringi != NULL)
    {
      TCollection_AsciiString anExtList;
      GLint                   anExtNb = 0;
      core11fwd->glGetIntegerv(GL_NUM_EXTENSIONS, &anExtNb);
      for (GLint anIter = 0; anIter < anExtNb; ++anIter)
      {
        const char* anExtension = (const char*)myFuncs->glGetStringi(GL_EXTENSIONS, (GLuint)anIter);
        if (!anExtList.IsEmpty())
        {
          anExtList += " ";
        }
        anExtList += anExtension;
      }
      addInfo(theDict, "GLextensions", anExtList);
    }
    else
    {
      addInfo(theDict, "GLextensions", (const char*)core11fwd->glGetString(GL_EXTENSIONS));
    }
  }
}

// =======================================================================
// function : GetResource
// purpose  :
// =======================================================================
const Handle(OpenGl_Resource)& OpenGl_Context::GetResource(
  const TCollection_AsciiString& theKey) const
{
  return mySharedResources->IsBound(theKey) ? mySharedResources->Find(theKey) : NULL_GL_RESOURCE;
}

// =======================================================================
// function : ShareResource
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::ShareResource(const TCollection_AsciiString& theKey,
                                               const Handle(OpenGl_Resource)& theResource)
{
  if (theKey.IsEmpty() || theResource.IsNull())
  {
    return Standard_False;
  }
  return mySharedResources->Bind(theKey, theResource);
}

// =======================================================================
// function : ReleaseResource
// purpose  :
// =======================================================================
void OpenGl_Context::ReleaseResource(const TCollection_AsciiString& theKey,
                                     const Standard_Boolean         theToDelay)
{
  if (!mySharedResources->IsBound(theKey))
  {
    return;
  }
  const Handle(OpenGl_Resource)& aRes = mySharedResources->Find(theKey);
  if (aRes->GetRefCount() > 1)
  {
    return;
  }

  if (theToDelay)
  {
    myDelayed->Bind(theKey, 1);
  }
  else
  {
    aRes->Release(this);
    mySharedResources->UnBind(theKey);
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
    myUnusedResources->First()->Release(this);
    myUnusedResources->RemoveFirst();
  }

  // release delayed shared resources
  NCollection_Vector<TCollection_AsciiString> aDeadList;
  for (NCollection_DataMap<TCollection_AsciiString, Standard_Integer>::Iterator anIter(*myDelayed);
       anIter.More();
       anIter.Next())
  {
    if (++anIter.ChangeValue() <= 2)
    {
      continue; // postpone release one more frame to ensure no one uses it periodically
    }

    const TCollection_AsciiString& aKey = anIter.Key();
    if (!mySharedResources->IsBound(aKey))
    {
      // mixed unshared strategy delayed/undelayed was used!
      aDeadList.Append(aKey);
      continue;
    }

    const Handle(OpenGl_Resource)& aRes = mySharedResources->ChangeFind(aKey);
    if (aRes->GetRefCount() > 1)
    {
      // should be only 1 instance in mySharedResources
      // if not - resource was reused again
      aDeadList.Append(aKey);
      continue;
    }

    // release resource if no one requested it more than 2 redraw calls
    aRes->Release(this);
    mySharedResources->UnBind(aKey);
    aDeadList.Append(aKey);
  }

  for (Standard_Integer anIter = 0; anIter < aDeadList.Length(); ++anIter)
  {
    myDelayed->UnBind(aDeadList.Value(anIter));
  }
}

// =======================================================================
// function : BindTextures
// purpose  :
// =======================================================================
Handle(OpenGl_TextureSet) OpenGl_Context::BindTextures(
  const Handle(OpenGl_TextureSet)&    theTextures,
  const Handle(OpenGl_ShaderProgram)& theProgram)
{
  const Standard_Integer aTextureSetBits =
    !theTextures.IsNull() ? theTextures->TextureSetBits() : 0;
  const Standard_Integer aProgramBits = !theProgram.IsNull() ? theProgram->TextureSetBits() : 0;
  Standard_Integer       aMissingBits = aProgramBits & ~aTextureSetBits;
  if (aMissingBits != 0 && myTextureRgbaBlack.IsNull())
  {
    // allocate mock textures
    myTextureRgbaBlack = new OpenGl_Texture();
    myTextureRgbaWhite = new OpenGl_Texture();
    Image_PixMap anImage;
    anImage.InitZero(Image_Format_RGBA, 2, 2, 0, (Standard_Byte)0);
    if (!myTextureRgbaBlack->Init(this,
                                  OpenGl_TextureFormat::Create<GLubyte, 4>(),
                                  Graphic3d_Vec2i(2, 2),
                                  Graphic3d_TypeOfTexture_2D,
                                  &anImage))
    {
      PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                  GL_DEBUG_TYPE_PORTABILITY,
                  0,
                  GL_DEBUG_SEVERITY_HIGH,
                  "Error: unable to create unit mock PBR texture map.");
    }
    anImage.InitZero(Image_Format_RGBA, 2, 2, 0, (Standard_Byte)255);
    if (!myTextureRgbaWhite->Init(this,
                                  OpenGl_TextureFormat::Create<GLubyte, 4>(),
                                  Graphic3d_Vec2i(2, 2),
                                  Graphic3d_TypeOfTexture_2D,
                                  &anImage))
    {
      PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                  GL_DEBUG_TYPE_PORTABILITY,
                  0,
                  GL_DEBUG_SEVERITY_HIGH,
                  "Error: unable to create normal mock PBR texture map.");
    }
  }

  Handle(OpenGl_TextureSet) anOldTextures = myActiveTextures;
  if (myActiveTextures != theTextures)
  {
    Handle(OpenGl_Context) aThisCtx(this);
    for (OpenGl_TextureSetPairIterator aSlotIter(myActiveTextures, theTextures); aSlotIter.More();
         aSlotIter.Next())
    {
      const Graphic3d_TextureUnit aTexUnit    = aSlotIter.Unit();
      const OpenGl_Texture*       aTextureOld = aSlotIter.Texture1();
      const OpenGl_Texture*       aTextureNew = aSlotIter.Texture2();
      if (aTextureNew == aTextureOld)
      {
        continue;
      }

      if (aTextureNew != NULL && aTextureNew->IsValid())
      {
        if (aTexUnit >= myMaxTexCombined)
        {
          PushMessage(GL_DEBUG_SOURCE_APPLICATION,
                      GL_DEBUG_TYPE_ERROR,
                      0,
                      GL_DEBUG_SEVERITY_HIGH,
                      TCollection_AsciiString("Texture unit ") + aTexUnit + " for "
                        + aTextureNew->ResourceId() + " exceeds hardware limit "
                        + myMaxTexCombined);
          continue;
        }

        aTextureNew->Bind(aThisCtx, aTexUnit);
        if (aTextureNew->Sampler()->ToUpdateParameters())
        {
          if (aTextureNew->Sampler()->IsImmutable())
          {
            aTextureNew->Sampler()->Init(aThisCtx, *aTextureNew);
          }
          else
          {
            OpenGl_Sampler::applySamplerParams(aThisCtx,
                                               aTextureNew->Sampler()->Parameters(),
                                               aTextureNew->Sampler().get(),
                                               aTextureNew->GetTarget(),
                                               aTextureNew->MaxMipmapLevel());
          }
        }
        if (core11ffp != NULL)
        {
          OpenGl_Sampler::applyGlobalTextureParams(aThisCtx,
                                                   *aTextureNew,
                                                   aTextureNew->Sampler()->Parameters());
        }
      }
      else if (aTextureOld != NULL && aTextureOld->IsValid())
      {
        aTextureOld->Unbind(aThisCtx, aTexUnit);
        if (core11ffp != NULL)
        {
          OpenGl_Sampler::resetGlobalTextureParams(aThisCtx,
                                                   *aTextureOld,
                                                   aTextureOld->Sampler()->Parameters());
        }
      }
    }
    myActiveTextures = theTextures;
  }

  if (myActiveMockTextures != aMissingBits)
  {
    myActiveMockTextures = aMissingBits;
    for (Standard_Integer aBitIter = 0; aMissingBits != 0; ++aBitIter)
    {
      Standard_Integer aUnitMask = 1 << aBitIter;
      if ((aUnitMask & aMissingBits) != 0)
      {
        aMissingBits = aMissingBits & ~aUnitMask;
        if (aBitIter == Graphic3d_TextureUnit_Normal)
        {
          myTextureRgbaBlack->Bind(this, static_cast<Graphic3d_TextureUnit>(aBitIter));
        }
        else
        {
          myTextureRgbaWhite->Bind(this, static_cast<Graphic3d_TextureUnit>(aBitIter));
        }
      }
    }
  }

  return anOldTextures;
}

// =======================================================================
// function : BindProgram
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::BindProgram(const Handle(OpenGl_ShaderProgram)& theProgram)
{
  if (core20fwd == NULL)
  {
    return Standard_False;
  }
  else if (myActiveProgram == theProgram)
  {
    return Standard_True;
  }

  if (theProgram.IsNull() || !theProgram->IsValid())
  {
    if (!myActiveProgram.IsNull())
    {
      core20fwd->glUseProgram(OpenGl_ShaderProgram::NO_PROGRAM);
      myActiveProgram.Nullify();
    }
    return Standard_False;
  }

  myActiveProgram = theProgram;
  core20fwd->glUseProgram(theProgram->ProgramId());
  return Standard_True;
}

// =======================================================================
// function : BindDefaultVao
// purpose  :
// =======================================================================
void OpenGl_Context::BindDefaultVao()
{
  if (myDefaultVao == 0 || core32 == NULL)
  {
    return;
  }

  core32->glBindVertexArray(myDefaultVao);
}

// =======================================================================
// function : SetDefaultFrameBuffer
// purpose  :
// =======================================================================
Handle(OpenGl_FrameBuffer) OpenGl_Context::SetDefaultFrameBuffer(
  const Handle(OpenGl_FrameBuffer)& theFbo)
{
  Handle(OpenGl_FrameBuffer) aFbo = myDefaultFbo;
  myDefaultFbo                    = theFbo;
  return aFbo;
}

// =======================================================================
// function : IsRender
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::IsRender() const
{
  return myRenderMode == GL_RENDER;
}

// =======================================================================
// function : IsFeedback
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::IsFeedback() const
{
  return myRenderMode == GL_FEEDBACK;
}

// =======================================================================
// function : SetShadingMaterial
// purpose  :
// =======================================================================
void OpenGl_Context::SetShadingMaterial(
  const OpenGl_Aspects*                           theAspect,
  const Handle(Graphic3d_PresentationAttributes)& theHighlight)
{
  const Handle(Graphic3d_Aspects)& anAspect =
    (!theHighlight.IsNull() && !theHighlight->BasicFillAreaAspect().IsNull())
      ? (const Handle(Graphic3d_Aspects)&)theHighlight->BasicFillAreaAspect()
      : theAspect->Aspect();

  const bool                      toDistinguish = anAspect->Distinguish();
  const bool                      toMapTexture  = anAspect->ToMapTexture();
  const Graphic3d_MaterialAspect& aMatFrontSrc  = anAspect->FrontMaterial();
  const Graphic3d_MaterialAspect& aMatBackSrc =
    toDistinguish ? anAspect->BackMaterial() : aMatFrontSrc;
  const Quantity_Color& aFrontIntColor = anAspect->InteriorColor();
  const Quantity_Color& aBackIntColor =
    toDistinguish ? anAspect->BackInteriorColor() : aFrontIntColor;

  myMaterial.Init(*this, aMatFrontSrc, aFrontIntColor, aMatBackSrc, aBackIntColor);
  if (!theHighlight.IsNull() && theHighlight->BasicFillAreaAspect().IsNull())
  {
    myMaterial.SetColor(theHighlight->ColorRGBA().GetRGB());
    myMaterial.SetColor(theHighlight->ColorRGBA().GetRGB());
  }

  float anAlphaFront = 1.0f, anAlphaBack = 1.0f;
  if (CheckIsTransparent(theAspect, theHighlight, anAlphaFront, anAlphaBack))
  {
    myMaterial.Common[0].Diffuse.a() = anAlphaFront;
    myMaterial.Common[1].Diffuse.a() = anAlphaBack;

    myMaterial.Pbr[0].BaseColor.a() = anAlphaFront;
    myMaterial.Pbr[1].BaseColor.a() = anAlphaBack;
  }

  // do not update material properties in case of zero reflection mode,
  // because GL lighting will be disabled by OpenGl_PrimitiveArray::DrawArray() anyway.
  const OpenGl_MaterialState& aMatState     = myShaderManager->MaterialState();
  float                       anAlphaCutoff = (anAspect->AlphaMode() == Graphic3d_AlphaMode_Mask
                         || anAspect->AlphaMode() == Graphic3d_AlphaMode_MaskBlend)
                                                ? anAspect->AlphaCutoff()
                                                : ShortRealLast();
  if (anAspect->ToDrawEdges())
  {
    if (anAspect->InteriorStyle() == Aspect_IS_EMPTY
        || (anAspect->InteriorStyle() == Aspect_IS_SOLID
            && anAspect->EdgeColorRGBA().Alpha() < 1.0f))
    {
      anAlphaCutoff = 0.285f;
    }
  }
  if (theAspect->ShadingModel() == Graphic3d_TypeOfShadingModel_Unlit)
  {
    if (anAlphaCutoff == aMatState.AlphaCutoff())
    {
      return;
    }
  }
  else if (myMaterial.IsEqual(aMatState.Material()) && toDistinguish == aMatState.ToDistinguish()
           && toMapTexture == aMatState.ToMapTexture() && anAlphaCutoff == aMatState.AlphaCutoff())
  {
    return;
  }

  myShaderManager->UpdateMaterialStateTo(myMaterial, anAlphaCutoff, toDistinguish, toMapTexture);
}

// =======================================================================
// function : CheckIsTransparent
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::CheckIsTransparent(
  const OpenGl_Aspects*                           theAspect,
  const Handle(Graphic3d_PresentationAttributes)& theHighlight,
  Standard_ShortReal&                             theAlphaFront,
  Standard_ShortReal&                             theAlphaBack)
{
  const Handle(Graphic3d_Aspects)& anAspect =
    (!theHighlight.IsNull() && !theHighlight->BasicFillAreaAspect().IsNull())
      ? (const Handle(Graphic3d_Aspects)&)theHighlight->BasicFillAreaAspect()
      : theAspect->Aspect();

  const bool                      toDistinguish = anAspect->Distinguish();
  const Graphic3d_MaterialAspect& aMatFrontSrc  = anAspect->FrontMaterial();
  const Graphic3d_MaterialAspect& aMatBackSrc =
    toDistinguish ? anAspect->BackMaterial() : aMatFrontSrc;

  // handling transparency
  if (!theHighlight.IsNull() && theHighlight->BasicFillAreaAspect().IsNull())
  {
    theAlphaFront = theHighlight->ColorRGBA().Alpha();
    theAlphaBack  = theHighlight->ColorRGBA().Alpha();
  }
  else
  {
    theAlphaFront = aMatFrontSrc.Alpha();
    theAlphaBack  = aMatBackSrc.Alpha();
  }

  if (anAspect->AlphaMode() == Graphic3d_AlphaMode_BlendAuto)
  {
    return theAlphaFront < 1.0f || theAlphaBack < 1.0f;
  }
  // Graphic3d_AlphaMode_Mask and Graphic3d_AlphaMode_MaskBlend are not considered transparent here
  return anAspect->AlphaMode() == Graphic3d_AlphaMode_Blend;
}

// =======================================================================
// function : SetColor4fv
// purpose  :
// =======================================================================
void OpenGl_Context::SetColor4fv(const OpenGl_Vec4& theColor)
{
  if (!myActiveProgram.IsNull())
  {
    if (const OpenGl_ShaderUniformLocation& aLoc =
          myActiveProgram->GetStateLocation(OpenGl_OCCT_COLOR))
    {
      myActiveProgram->SetUniform(this, aLoc, Vec4FromQuantityColor(theColor));
    }
  }
  else if (core11ffp != NULL)
  {
    core11ffp->glColor4fv(theColor.GetData());
  }
}

// =======================================================================
// function : SetTypeOfLine
// purpose  :
// =======================================================================
void OpenGl_Context::SetTypeOfLine(const Aspect_TypeOfLine  theType,
                                   const Standard_ShortReal theFactor)
{
  SetLineStipple(theFactor, Graphic3d_Aspects::DefaultLinePatternForType(theType));
}

// =======================================================================
// function : SetLineStipple
// purpose  :
// =======================================================================
void OpenGl_Context::SetLineStipple(const Standard_ShortReal theFactor, const uint16_t thePattern)
{
  if (!myActiveProgram.IsNull())
  {
    if (const OpenGl_ShaderUniformLocation aPatternLoc =
          myActiveProgram->GetStateLocation(OpenGl_OCCT_LINE_STIPPLE_PATTERN))
    {
      if (hasGlslBitwiseOps != OpenGl_FeatureNotAvailable)
      {
        myActiveProgram->SetUniform(this, aPatternLoc, (Standard_Integer)thePattern);
      }
      else
      {
        Standard_Integer aPatArr[16] = {};
        for (unsigned int aBit = 0; aBit < 16; ++aBit)
        {
          aPatArr[aBit] = ((unsigned int)(thePattern) & (1U << aBit)) != 0 ? 1 : 0;
        }
        myActiveProgram->SetUniform(this, aPatternLoc, 16, aPatArr);
      }
      myActiveProgram->SetUniform(
        this,
        myActiveProgram->GetStateLocation(OpenGl_OCCT_LINE_STIPPLE_FACTOR),
        theFactor);
    }
    return;
  }

  if (core11ffp != NULL)
  {
    if (thePattern != 0xFFFF)
    {
      core11fwd->glEnable(GL_LINE_STIPPLE);
      core11ffp->glLineStipple(static_cast<GLint>(theFactor), static_cast<GLushort>(thePattern));
    }
    else
    {
      core11fwd->glDisable(GL_LINE_STIPPLE);
    }
  }
}

// =======================================================================
// function : SetLineWidth
// purpose  :
// =======================================================================
void OpenGl_Context::SetLineWidth(const Standard_ShortReal theWidth)
{
  if (myGapi == Aspect_GraphicsLibrary_OpenGLES || core11ffp != NULL)
  {
    // glLineWidth() is still defined within Core Profile, but has no effect with values != 1.0f
    core11fwd->glLineWidth(theWidth * myLineWidthScale);
  }
}

// =======================================================================
// function : SetTextureMatrix
// purpose  :
// =======================================================================
void OpenGl_Context::SetTextureMatrix(const Handle(Graphic3d_TextureParams)& theParams,
                                      const Standard_Boolean                 theIsTopDown)
{
  if (theParams.IsNull())
  {
    return;
  }

  const Graphic3d_Vec2& aScale = theParams->Scale();
  const Graphic3d_Vec2& aTrans = theParams->Translation();
  if (!myActiveProgram.IsNull())
  {
    const GLint aUniLoc = myActiveProgram->GetStateLocation(OpenGl_OCCT_TEXTURE_TRSF2D);
    if (aUniLoc == OpenGl_ShaderProgram::INVALID_LOCATION)
    {
      return;
    }

    // pack transformation parameters
    OpenGl_Vec4 aTrsf[2] = {
      OpenGl_Vec4(-aTrans.x(), -aTrans.y(), aScale.x(), aScale.y()),
      OpenGl_Vec4(static_cast<float>(std::sin(-theParams->Rotation() * M_PI / 180.0)),
                  static_cast<float>(std::cos(-theParams->Rotation() * M_PI / 180.0)),
                  0.0f,
                  0.0f)};
    if (caps->isTopDownTextureUV != theIsTopDown)
    {
      // flip V
      aTrsf[0].y() = -aTrans.y() + 1.0f / aScale.y();
      aTrsf[0].w() = -aScale.y();
    }
    myActiveProgram->SetUniform(this, aUniLoc, 2, aTrsf);
    return;
  }

  if (core11ffp != NULL)
  {
    GLint aMatrixMode = GL_TEXTURE;
    core11fwd->glGetIntegerv(GL_MATRIX_MODE, &aMatrixMode);

    core11ffp->glMatrixMode(GL_TEXTURE);
    OpenGl_Mat4 aTextureMat;
    if (caps->isTopDownTextureUV != theIsTopDown)
    {
      // flip V
      Graphic3d_TransformUtils::Scale(aTextureMat, aScale.x(), -aScale.y(), 1.0f);
      Graphic3d_TransformUtils::Translate(aTextureMat,
                                          -aTrans.x(),
                                          -aTrans.y() + 1.0f / aScale.y(),
                                          0.0f);
    }
    else
    {
      Graphic3d_TransformUtils::Scale(aTextureMat, aScale.x(), aScale.y(), 1.0f);
      Graphic3d_TransformUtils::Translate(aTextureMat, -aTrans.x(), -aTrans.y(), 0.0f);
    }
    Graphic3d_TransformUtils::Rotate(aTextureMat, -theParams->Rotation(), 0.0f, 0.0f, 1.0f);
    core11ffp->glLoadMatrixf(aTextureMat.GetData());
    core11ffp->glMatrixMode(aMatrixMode);
  }
}

// =======================================================================
// function : SetPointSize
// purpose  :
// =======================================================================
void OpenGl_Context::SetPointSize(const Standard_ShortReal theSize)
{
  if (!myActiveProgram.IsNull())
  {
    myActiveProgram->SetUniform(this,
                                myActiveProgram->GetStateLocation(OpenGl_OCCT_POINT_SIZE),
                                theSize);
    // if (myGapi == Aspect_GraphicsLibrary_OpenGL)
    // core11fwd->glEnable (GL_VERTEX_PROGRAM_POINT_SIZE);
  }
  // else

  if (myGapi != Aspect_GraphicsLibrary_OpenGLES)
  {
    core11fwd->glPointSize(theSize);
    if (core20fwd != NULL)
    {
      // core11fwd->glDisable (GL_VERTEX_PROGRAM_POINT_SIZE);
    }
  }
}

// =======================================================================
// function : SetPointSpriteOrigin
// purpose  :
// =======================================================================
void OpenGl_Context::SetPointSpriteOrigin()
{
  if (myGapi == Aspect_GraphicsLibrary_OpenGLES || core15fwd == NULL)
  {
    return;
  }

  const int aNewState = !myActiveProgram.IsNull() ? GL_UPPER_LEFT : GL_LOWER_LEFT;
  if (myPointSpriteOrig != aNewState)
  {
    myPointSpriteOrig = aNewState;
    core15fwd->glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, aNewState);
  }
}

// =======================================================================
// function : SetGlNormalizeEnabled
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Context::SetGlNormalizeEnabled(Standard_Boolean isEnabled)
{
  if (isEnabled == myIsGlNormalizeEnabled)
  {
    return myIsGlNormalizeEnabled;
  }

  Standard_Boolean anOldGlNormalize = myIsGlNormalizeEnabled;
  myIsGlNormalizeEnabled            = isEnabled;

  if (core11ffp != NULL)
  {
    if (isEnabled)
    {
      core11fwd->glEnable(GL_NORMALIZE);
    }
    else
    {
      core11fwd->glDisable(GL_NORMALIZE);
    }
  }

  return anOldGlNormalize;
}

// =======================================================================
// function : SetShadeModel
// purpose  :
// =======================================================================
void OpenGl_Context::SetShadeModel(Graphic3d_TypeOfShadingModel theModel)
{
  if (core11ffp != NULL)
  {
    const Standard_Integer aModel = theModel == Graphic3d_TypeOfShadingModel_PhongFacet
                                        || theModel == Graphic3d_TypeOfShadingModel_PbrFacet
                                      ? GL_FLAT
                                      : GL_SMOOTH;
    if (myShadeModel == aModel)
    {
      return;
    }
    myShadeModel = aModel;
    core11ffp->glShadeModel(aModel);
  }
}

// =======================================================================
// function : SetPolygonMode
// purpose  :
// =======================================================================
Standard_Integer OpenGl_Context::SetPolygonMode(const Standard_Integer theMode)
{
  if (myPolygonMode == theMode)
  {
    return myPolygonMode;
  }

  const Standard_Integer anOldPolygonMode = myPolygonMode;
  myPolygonMode                           = theMode;
  if (myGapi != Aspect_GraphicsLibrary_OpenGLES)
  {
    core11fwd->glPolygonMode(GL_FRONT_AND_BACK, (GLenum)theMode);
  }
  return anOldPolygonMode;
}

// =======================================================================
// function : SetPolygonHatchEnabled
// purpose  :
// =======================================================================
bool OpenGl_Context::SetPolygonHatchEnabled(const bool theIsEnabled)
{
  if (core11ffp == NULL)
  {
    return false;
  }
  else if (myHatchIsEnabled == theIsEnabled)
  {
    return theIsEnabled;
  }

  const bool anOldIsEnabled = myHatchIsEnabled;
  if (theIsEnabled && myActiveHatchType != Aspect_HS_SOLID)
  {
    core11fwd->glEnable(GL_POLYGON_STIPPLE);
  }
  else
  {
    core11fwd->glDisable(GL_POLYGON_STIPPLE);
  }

  myHatchIsEnabled = theIsEnabled;
  return anOldIsEnabled;
}

// =======================================================================
// function : SetPolygonHatchStyle
// purpose  :
// =======================================================================
Standard_Integer OpenGl_Context::SetPolygonHatchStyle(const Handle(Graphic3d_HatchStyle)& theStyle)
{
  const Standard_Integer aNewStyle = !theStyle.IsNull() ? theStyle->HatchType() : Aspect_HS_SOLID;
  if (myActiveHatchType == aNewStyle || core11ffp == NULL)
  {
    return myActiveHatchType;
  }

  if (aNewStyle == Aspect_HS_SOLID)
  {
    if (myHatchIsEnabled)
    {
      core11fwd->glDisable(GL_POLYGON_STIPPLE);
    }
    return myActiveHatchType;
  }

  if (myHatchStyles.IsNull() && !GetResource("OpenGl_LineAttributes", myHatchStyles))
  {
    // share and register for release once the resource is no longer used
    myHatchStyles = new OpenGl_LineAttributes();
    ShareResource("OpenGl_LineAttributes", myHatchStyles);
  }

  const Standard_Integer anOldType = myActiveHatchType;
  myActiveHatchType                = aNewStyle;
  myHatchStyles->SetTypeOfHatch(this, theStyle);
  if (myHatchIsEnabled && anOldType == Aspect_HS_SOLID)
  {
    core11fwd->glEnable(GL_POLYGON_STIPPLE);
  }
  return anOldType;
}

// =======================================================================
// function : SetPolygonOffset
// purpose  :
// =======================================================================
void OpenGl_Context::SetPolygonOffset(const Graphic3d_PolygonOffset& theOffset)
{
  const bool toFillOld = (myPolygonOffset.Mode & Aspect_POM_Fill) == Aspect_POM_Fill;
  const bool toFillNew = (theOffset.Mode & Aspect_POM_Fill) == Aspect_POM_Fill;
  if (toFillNew != toFillOld)
  {
    if (toFillNew)
    {
      core11fwd->glEnable(GL_POLYGON_OFFSET_FILL);
    }
    else
    {
      core11fwd->glDisable(GL_POLYGON_OFFSET_FILL);
    }
  }

  if (myGapi != Aspect_GraphicsLibrary_OpenGLES)
  {
    const bool toLineOld = (myPolygonOffset.Mode & Aspect_POM_Line) == Aspect_POM_Line;
    const bool toLineNew = (theOffset.Mode & Aspect_POM_Line) == Aspect_POM_Line;
    if (toLineNew != toLineOld)
    {
      if (toLineNew)
      {
        core11fwd->glEnable(GL_POLYGON_OFFSET_LINE);
      }
      else
      {
        core11fwd->glDisable(GL_POLYGON_OFFSET_LINE);
      }
    }

    const bool toPointOld = (myPolygonOffset.Mode & Aspect_POM_Point) == Aspect_POM_Point;
    const bool toPointNew = (theOffset.Mode & Aspect_POM_Point) == Aspect_POM_Point;
    if (toPointNew != toPointOld)
    {
      if (toPointNew)
      {
        core11fwd->glEnable(GL_POLYGON_OFFSET_POINT);
      }
      else
      {
        core11fwd->glDisable(GL_POLYGON_OFFSET_POINT);
      }
    }
  }

  if (myPolygonOffset.Factor != theOffset.Factor || myPolygonOffset.Units != theOffset.Units)
  {
    core11fwd->glPolygonOffset(theOffset.Factor, theOffset.Units);
  }
  myPolygonOffset = theOffset;
}

// =======================================================================
// function : SetCamera
// purpose  :
// =======================================================================
void OpenGl_Context::SetCamera(const Handle(Graphic3d_Camera)& theCamera)
{
  myCamera = theCamera;
  if (!theCamera.IsNull())
  {
    ProjectionState.SetCurrent(theCamera->ProjectionMatrixF());
    WorldViewState.SetCurrent(theCamera->OrientationMatrixF());
    ApplyProjectionMatrix();
    ApplyWorldViewMatrix();
  }
}

// =======================================================================
// function : ApplyModelWorldMatrix
// purpose  :
// =======================================================================
void OpenGl_Context::ApplyModelWorldMatrix()
{
  if (myShaderManager->ModelWorldState().ModelWorldMatrix() != ModelWorldState.Current())
  {
    myShaderManager->UpdateModelWorldStateTo(ModelWorldState.Current());
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
    myShaderManager->UpdateModelWorldStateTo(THE_IDENTITY_MATRIX);
  }
  if (myShaderManager->WorldViewState().WorldViewMatrix() != WorldViewState.Current())
  {
    myShaderManager->UpdateWorldViewStateTo(WorldViewState.Current());
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
    myShaderManager->UpdateModelWorldStateTo(ModelWorldState.Current());
  }
  if (myShaderManager->WorldViewState().WorldViewMatrix() != WorldViewState.Current())
  {
    myShaderManager->UpdateWorldViewStateTo(WorldViewState.Current());
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
    myShaderManager->UpdateProjectionStateTo(ProjectionState.Current());
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
  core11fwd->glDisable(GL_DITHER);
  core11fwd->glDisable(GL_BLEND);
  core11fwd->glDisable(GL_DEPTH_TEST);
  core11fwd->glDisable(GL_STENCIL_TEST);

  if (core11ffp == NULL)
  {
    return;
  }

  core11fwd->glDisable(GL_TEXTURE_1D);
  core11fwd->glDisable(GL_TEXTURE_2D);

  core11fwd->glDisable(GL_LIGHTING);
  core11fwd->glDisable(GL_ALPHA_TEST);
  core11fwd->glDisable(GL_FOG);
  core11fwd->glDisable(GL_LOGIC_OP);

  core11ffp->glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
  core11ffp->glPixelTransferi(GL_RED_SCALE, 1);
  core11ffp->glPixelTransferi(GL_RED_BIAS, 0);
  core11ffp->glPixelTransferi(GL_GREEN_SCALE, 1);
  core11ffp->glPixelTransferi(GL_GREEN_BIAS, 0);
  core11ffp->glPixelTransferi(GL_BLUE_SCALE, 1);
  core11ffp->glPixelTransferi(GL_BLUE_BIAS, 0);
  core11ffp->glPixelTransferi(GL_ALPHA_SCALE, 1);
  core11ffp->glPixelTransferi(GL_ALPHA_BIAS, 0);

  if (IsGlGreaterEqual(1, 2))
  {
    if (CheckExtension("GL_CONVOLUTION_1D_EXT"))
    {
      core11fwd->glDisable(GL_CONVOLUTION_1D_EXT);
    }
    if (CheckExtension("GL_CONVOLUTION_2D_EXT"))
    {
      core11fwd->glDisable(GL_CONVOLUTION_2D_EXT);
    }
    if (CheckExtension("GL_SEPARABLE_2D_EXT"))
    {
      core11fwd->glDisable(GL_SEPARABLE_2D_EXT);
    }
    if (CheckExtension("GL_SEPARABLE_2D_EXT"))
    {
      core11fwd->glDisable(GL_HISTOGRAM_EXT);
    }
    if (CheckExtension("GL_MINMAX_EXT"))
    {
      core11fwd->glDisable(GL_MINMAX_EXT);
    }
    if (CheckExtension("GL_TEXTURE_3D_EXT"))
    {
      core11fwd->glDisable(GL_TEXTURE_3D_EXT);
    }
  }
}

// =======================================================================
// function : SetColorMaskRGBA
// purpose  :
// =======================================================================
void OpenGl_Context::SetColorMaskRGBA(const NCollection_Vec4<bool>& theVal)
{
  core11fwd->glColorMask(theVal.r() ? GL_TRUE : GL_FALSE,
                         theVal.g() ? GL_TRUE : GL_FALSE,
                         theVal.b() ? GL_TRUE : GL_FALSE,
                         theVal.a() ? GL_TRUE : GL_FALSE);
  myColorMask = theVal;
}

// =======================================================================
// function : SetColorMask
// purpose  :
// =======================================================================
bool OpenGl_Context::SetColorMask(bool theToWriteColor)
{
  const bool anOldValue = myColorMask.r();
  myColorMask.SetValues(theToWriteColor,
                        theToWriteColor,
                        theToWriteColor,
                        caps->buffersOpaqueAlpha ? false : theToWriteColor);
  const GLboolean toWrite = theToWriteColor ? GL_TRUE : GL_FALSE;
  core11fwd->glColorMask(toWrite, toWrite, toWrite, myColorMask.a() ? GL_TRUE : GL_FALSE);
  return anOldValue;
}

// =======================================================================
// function : SetSampleAlphaToCoverage
// purpose  :
// =======================================================================
bool OpenGl_Context::SetSampleAlphaToCoverage(bool theToEnable)
{
  bool toEnable = myAllowAlphaToCov && theToEnable;
  if (myAlphaToCoverage == toEnable)
  {
    return myAlphaToCoverage;
  }

  if (core15fwd != NULL)
  {
    if (toEnable)
    {
      // core15fwd->core15fwd->glSampleCoverage (1.0f, GL_FALSE);
      core15fwd->glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }
    else
    {
      core15fwd->glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }
  }

  const bool anOldValue = myAlphaToCoverage;
  myAlphaToCoverage     = toEnable;
  return anOldValue;
}

// =======================================================================
// function : GetBufferSubData
// purpose  :
// =======================================================================
bool OpenGl_Context::GetBufferSubData(unsigned int theTarget,
                                      intptr_t     theOffset,
                                      intptr_t     theSize,
                                      void*        theData)
{
  if (!hasGetBufferData)
  {
    return false;
  }
#ifdef __EMSCRIPTEN__
  EM_ASM_(
    { Module.ctx.getBufferSubData($0, $1, HEAPU8.subarray($2, $2 + $3)); },
    theTarget,
    theOffset,
    theData,
    theSize);
  return true;
#elif defined(OCC_USE_GLES2)
  if (void* aData = core30->glMapBufferRange(theTarget, theOffset, theSize, GL_MAP_READ_BIT))
  {
    memcpy(theData, aData, theSize);
    core30->glUnmapBuffer(theTarget);
    return true;
  }
  return false;
#else
  core15fwd->glGetBufferSubData(theTarget, theOffset, theSize, theData);
  return true;
#endif
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void OpenGl_Context::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAnisoMax)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myTexClamp)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMaxTexDim)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMaxTexCombined)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMaxDumpSizeX)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMaxDumpSizeY)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMaxClipPlanes)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMaxMsaaSamples)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMaxDrawBuffers)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMaxColorAttachments)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myGlVerMajor)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myGlVerMinor)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsInitialized)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsStereoBuffers)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsGlNormalizeEnabled)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasRayTracing)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasRayTracingTextures)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasRayTracingAdaptiveSampling)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasRayTracingAdaptiveSamplingAtomic)

  for (int i = 0; i < 4; i++)
  {
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myViewport[i])
  }

  for (int i = 0; i < 4; i++)
  {
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myViewportVirt[i])
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPointSpriteOrig)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRenderMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPolygonMode)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myPolygonOffset)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myFaceCulling)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myReadBuffer)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDefaultVao)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myColorMask)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAllowAlphaToCov)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myAlphaToCoverage)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsGlDebugCtx)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myResolution)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myResolutionRatio)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myLineWidthScale)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myLineFeather)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRenderScale)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRenderScaleInv)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &ModelWorldState)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &WorldViewState)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &ProjectionState)
}

// =======================================================================
// function : DumpJsonOpenGlState
// purpose  :
// =======================================================================
void OpenGl_Context::DumpJsonOpenGlState(Standard_OStream& theOStream, Standard_Integer)
{
  GLboolean isEnableBlend     = core11fwd->glIsEnabled(GL_BLEND);
  GLboolean isEnableCullFace  = core11fwd->glIsEnabled(GL_CULL_FACE);
  GLboolean isEnableDepthTest = core11fwd->glIsEnabled(GL_DEPTH_TEST);

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, isEnableBlend)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, isEnableCullFace)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, isEnableDepthTest)
}
