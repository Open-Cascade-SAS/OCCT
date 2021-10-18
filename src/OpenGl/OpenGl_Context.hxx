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

#ifndef OpenGl_Context_HeaderFile
#define OpenGl_Context_HeaderFile

#include <Aspect_Handle.hxx>
#include <Aspect_HatchStyle.hxx>
#include <Aspect_Drawable.hxx>
#include <Aspect_Display.hxx>
#include <Aspect_RenderingContext.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <NCollection_DataMap.hxx>
#include <Graphic3d_DiagnosticInfo.hxx>
#include <Graphic3d_TextureUnit.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Handle.hxx>
#include <NCollection_List.hxx>
#include <NCollection_SparseArray.hxx>
#include <Message.hxx>
#include <OpenGl_Caps.hxx>
#include <OpenGl_LineAttributes.hxx>
#include <OpenGl_Material.hxx>
#include <OpenGl_MatrixState.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_Resource.hxx>
#include <OpenGl_TextureSet.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_IndexedDataMapOfStringString.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <OpenGl_Clipping.hxx>
#include <OpenGl_GlCore11.hxx>

#include <NCollection_Shared.hxx>

//! Forward declarations
#if defined(__APPLE__)
  #import <TargetConditionals.h>
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    #ifdef __OBJC__
      @class EAGLContext;
    #else
      struct EAGLContext;
    #endif
  #else
    #ifdef __OBJC__
      @class NSOpenGLContext;
    #else
      struct NSOpenGLContext;
    #endif
  #endif
#endif

struct OpenGl_GlFunctions;
struct OpenGl_ArbTBO;
struct OpenGl_ArbIns;
struct OpenGl_ArbDbg;
struct OpenGl_ArbFBO;
struct OpenGl_ArbFBOBlit;
struct OpenGl_ArbSamplerObject;
struct OpenGl_ArbTexBindless;
struct OpenGl_ExtGS;

struct OpenGl_GlCore12;
struct OpenGl_GlCore13;
struct OpenGl_GlCore14;
struct OpenGl_GlCore15;
struct OpenGl_GlCore20;
struct OpenGl_GlCore21;
struct OpenGl_GlCore30;
struct OpenGl_GlCore31;
struct OpenGl_GlCore32;
struct OpenGl_GlCore33;
struct OpenGl_GlCore40;
struct OpenGl_GlCore41;
struct OpenGl_GlCore42;
struct OpenGl_GlCore43;
struct OpenGl_GlCore44;
struct OpenGl_GlCore45;
struct OpenGl_GlCore46;

class Graphic3d_Camera;
class Graphic3d_PresentationAttributes;
class OpenGl_Aspects;
class OpenGl_FrameBuffer;
class OpenGl_Sampler;
class OpenGl_ShaderProgram;
class OpenGl_ShaderManager;
class OpenGl_FrameStats;

enum OpenGl_FeatureFlag
{
  OpenGl_FeatureNotAvailable = 0, //!< Feature is not supported by OpenGl implementation.
  OpenGl_FeatureInExtensions = 1, //!< Feature is supported as extension.
  OpenGl_FeatureInCore       = 2  //!< Feature is supported as part of core profile.
};

DEFINE_STANDARD_HANDLE(OpenGl_Context, Standard_Transient)

//! This class generalize access to the GL context and available extensions.
//!
//! Functions related to specific OpenGL version or extension are grouped into structures which can be accessed as fields of this class.
//! The most simple way to check that required functionality is available - is NULL check for the group:
//! @code
//!   if (myContext->core20 != NULL)
//!   {
//!     myGlProgram = myContext->core20->glCreateProgram();
//!     .. do more stuff ..
//!   }
//!   else
//!   {
//!     .. compatibility with outdated configurations ..
//!   }
//! @endcode
//!
//! Current implementation provide access to OpenGL core functionality up to 4.6 version (core12, core13, core14, etc.)
//! as well as several extensions (arbTBO, arbFBO, etc.).
//!
//! OpenGL context might be initialized in Core Profile. In this case deprecated functionality become unavailable.
//! To select which core** function set should be used in specific case:
//!  - Determine the minimal OpenGL version required for implemented functionality and use it to access all functions.
//!    For example, if algorithm requires OpenGL 2.1+, it is better to write core20fwd->glEnable() rather than core11fwd->glEnable() for uniformity.
//!  - Validate minimal requirements at initialization/creation time and omit checks within code where algorithm should be already initialized.
//!    Properly escape code incompatible with Core Profile. The simplest way to check Core Profile is "if (core11ffp == NULL)".
//!
//! Simplified extensions classification:
//!  - prefixed with NV, AMD, ATI are vendor-specific (however may be provided by other vendors in some cases);
//!  - prefixed with EXT are accepted by 2+ vendors;
//!  - prefixed with ARB are accepted by Architecture Review Board and are candidates
//!    for inclusion into GL core functionality.
//! Some functionality can be represented in several extensions simultaneously.
//! In this case developer should be careful because different specification may differ
//! in aspects (like enumeration values and error-handling).
//!
//! Notice that some systems provide mechanisms to simultaneously incorporate with GL contexts with different capabilities.
//! For this reason OpenGl_Context should be initialized and used for each GL context independently.
//!
//! Matrices of OpenGl transformations:
//! model -> world -> view -> projection
//! These matrices might be changed for local transformation, transform persistent using direct access to
//! current matrix of ModelWorldState, WorldViewState and ProjectionState
//! After, these matrices should be applied using ApplyModelWorldMatrix, ApplyWorldViewMatrix,
//! ApplyModelViewMatrix or ApplyProjectionMatrix.
class OpenGl_Context : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(OpenGl_Context, Standard_Transient)
  friend class OpenGl_Window;
  friend struct OpenGl_GlFunctions;
public:

  typedef NCollection_Shared< NCollection_DataMap<TCollection_AsciiString, Handle(OpenGl_Resource)> > OpenGl_ResourcesMap;

  //! Function for getting power of to number larger or equal to input number.
  //! @param theNumber    number to 'power of two'
  //! @param theThreshold upper threshold
  //! @return power of two number
  inline static Standard_Integer GetPowerOfTwo (const Standard_Integer theNumber,
                                                const Standard_Integer theThreshold)
  {
    for (Standard_Integer p2 = 2; p2 <= theThreshold; p2 <<= 1)
    {
      if (theNumber <= p2)
      {
        return p2;
      }
    }
    return theThreshold;
  }

  //! Format GL constant as hex value 0xABCD.
  Standard_EXPORT static TCollection_AsciiString FormatGlEnumHex (int theGlEnum);

  //! Format pointer as hex value 0xABCD.
  Standard_EXPORT static TCollection_AsciiString FormatPointer (const void* thePtr);

  //! Format size value.
  Standard_EXPORT static TCollection_AsciiString FormatSize (Standard_Size theSize);

  //! Return text description of GL error.
  Standard_EXPORT static TCollection_AsciiString FormatGlError (int theGlError);

public:

  //! Empty constructor. You should call Init() to perform initialization with bound GL context.
  Standard_EXPORT OpenGl_Context (const Handle(OpenGl_Caps)& theCaps = NULL);

  //! Destructor.
  Standard_EXPORT virtual ~OpenGl_Context();

  //! Release all resources, including shared ones
  Standard_EXPORT void forcedRelease();

  //! Share GL context resources.
  //! theShareCtx - handle to context to retrieve handles to shared resources.
  Standard_EXPORT void Share (const Handle(OpenGl_Context)& theShareCtx);

  //! Initialize class from currently bound OpenGL context. Method should be called only once.
  //! @return false if no GL context is bound to the current thread
  Standard_EXPORT Standard_Boolean Init (const Standard_Boolean theIsCoreProfile = Standard_False);

  //! @return true if this context is valid (has been initialized)
  inline Standard_Boolean IsValid() const
  {
    return myIsInitialized;
  }

  //! Initialize class from specified surface and rendering context. Method should be called only once.
  //! The meaning of parameters is platform-specific.
  //!
  //! EGL:
  //! @code
  //!   Handle(Aspect_Window) theAspWin;
  //!   EGLSurface theEglSurf = eglCreateWindowSurface (theEglDisp, anEglConfig, (EGLNativeWindowType )theAspWin->NativeHandle(), NULL);
  //!   EGLDisplay theEglDisp = eglGetDisplay (EGL_DEFAULT_DISPLAY);
  //!   EGLContext theEglCtx  = eglCreateContext ((EGLDisplay )theEglDisp, anEglConfig, EGL_NO_CONTEXT, anEglCtxAttribs);
  //!   Handle(OpenGl_Context) aGlCtx = new OpenGl_Context();
  //!   aGlCtx->Init ((Aspect_Drawable )theEglSurf, (Aspect_Display )theEglDisp,  (Aspect_RenderingContext )theEglCtx);
  //! @endcode
  //!
  //! Windows (Win32):
  //! @code
  //!   Handle(WNT_Window) theAspWin;
  //!   HWND  theWindow   = (HWND )theAspWin->NativeHandle();
  //!   HDC   theDevCtx   = GetDC(theWindow);
  //!   HGLRC theGContext = wglCreateContext (theDevCtx);
  //!   Handle(OpenGl_Context) aGlCtx = new OpenGl_Context();
  //!   aGlCtx->Init ((Aspect_Drawable )theWindow, (Aspect_Display )theDevCtx, (Aspect_RenderingContext )theGContext);
  //! @endcode
  //!
  //! Linux (Xlib):
  //! @code
  //!   Handle(Xw_Window) theAspWin;
  //!   Window     theXWindow = (Window )theAspWin->NativeHandle();
  //!   Display*   theXDisp   = (Display* )theAspWin->DisplayConnection()->GetDisplayAspect();
  //!   GLXContext theGlxCtx  = glXCreateContext (theXDisp, aVis.get(), NULL, GL_TRUE);
  //!   Handle(OpenGl_Context) aGlCtx = new OpenGl_Context();
  //!   aGlCtx->Init ((Aspect_Drawable )theXWindow, (Aspect_Display )theXDisp,  (Aspect_RenderingContext )theGlxCtx);
  //! @endcode
  //!
  //! @param theSurface [in] surface / window          (EGLSurface | HWND  | GLXDrawable/Window)
  //! @param theDisplay [in] display or device context (EGLDisplay | HDC   | Display*)
  //! @param theContext [in] rendering context         (EGLContext | HGLRC | GLXContext | EAGLContext* | NSOpenGLContext*)
  //! @param theIsCoreProfile [in] flag indicating that passed GL rendering context has been created with Core Profile
  //! @return false if OpenGL context can not be bound to specified surface
  Standard_EXPORT Standard_Boolean Init (const Aspect_Drawable         theSurface,
                                         const Aspect_Display          theDisplay,
                                         const Aspect_RenderingContext theContext,
                                         const Standard_Boolean        theIsCoreProfile = Standard_False);

  //! Return window handle currently bound to this OpenGL context (EGLSurface | HWND | GLXDrawable).
  Aspect_Drawable Window() const { return myWindow; }

  //! Return display / window device context (EGLDisplay | HDC | Display*).
  Aspect_Display GetDisplay() const { return myDisplay; }

  //! Return rendering context (EGLContext | HGLRC | GLXContext | EAGLContext* | NSOpenGLContext*).
  Aspect_RenderingContext RenderingContext() const { return myGContext; }

#if defined(__APPLE__) && !defined(HAVE_XLIB)
  #if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE

  //! Initialize class from specified OpenGL ES context (EAGLContext). Method should be called only once.
  Standard_Boolean Init (EAGLContext*           theGContext,
                         const Standard_Boolean theIsCoreProfile = Standard_False)
  {
    return Init ((Aspect_Drawable )0, (Aspect_Display )0, (Aspect_RenderingContext )theGContext, theIsCoreProfile);
  }
  #else
  //! Initialize class from specified OpenGL context (NSOpenGLContext). Method should be called only once.
  Standard_Boolean Init (NSOpenGLContext*       theGContext,
                         const Standard_Boolean theIsCoreProfile = Standard_False)
  {
    return Init ((Aspect_Drawable )0, (Aspect_Display )0, (Aspect_RenderingContext )theGContext, theIsCoreProfile);
  }
  #endif
#endif

  //! Read OpenGL version information from active context.
  Standard_EXPORT static void ReadGlVersion (Standard_Integer& theGlVerMajor,
                                             Standard_Integer& theGlVerMinor);

  //! Check if theExtName extension is supported by active GL context.
  Standard_EXPORT Standard_Boolean CheckExtension (const char* theExtName) const;

  //! Check if theExtName extension is in extensions string.
  Standard_EXPORT static Standard_Boolean CheckExtension (const char* theExtString,
                                                          const char* theExtName);

  //! Auxiliary template to retrieve GL function pointer.
  //! Pointer to function retrieved from library is statically casted
  //! to requested type - there no way to check real signature of exported function.
  //! The context should be bound before call.
  //! @param theLastFailFuncName [out] set to theFuncName in case of failure, unmodified on success
  //! @param theFuncName [in] function name to find
  //! @param theFuncPtr [out] retrieved function pointer
  //! @return TRUE on success
  template <typename FuncType_t>
  Standard_Boolean FindProcVerbose (const char*& theLastFailFuncName,
                                    const char* theFuncName,
                                    FuncType_t& theFuncPtr)
  {
    theFuncPtr = (FuncType_t )findProc (theFuncName);
    if (theFuncPtr == NULL)
    {
      theLastFailFuncName = theFuncName;
      return Standard_False;
    }
    return Standard_True;
  }

  //! Auxiliary template to retrieve GL function pointer.
  //! Same as FindProcVerbose() but without auxiliary last function name argument.
  template <typename FuncType_t>
  Standard_Boolean FindProc (const char* theFuncName,
                             FuncType_t& theFuncPtr)
  {
    theFuncPtr = (FuncType_t )findProc (theFuncName);
    return (theFuncPtr != NULL);
  }

  //! @return true if detected GL version is greater or equal to requested one.
  inline Standard_Boolean IsGlGreaterEqual (const Standard_Integer theVerMajor,
                                            const Standard_Integer theVerMinor) const
  {
    return (myGlVerMajor >  theVerMajor)
        || (myGlVerMajor == theVerMajor && myGlVerMinor >= theVerMinor);
  }

  //! Return cached GL version major number.
  Standard_Integer VersionMajor() const { return myGlVerMajor; }

  //! Return cached GL version minor number.
  Standard_Integer VersionMinor() const { return myGlVerMinor; }

  //! Access entire map of loaded OpenGL functions.
  const OpenGl_GlFunctions* Functions() const { return myFuncs.operator->(); }

  //! Clean up errors stack for this GL context (glGetError() in loop).
  //! @return true if some error has been cleared
  Standard_EXPORT bool ResetErrors (const bool theToPrintErrors = false);

  //! This method uses system-dependent API to retrieve information
  //! about GL context bound to the current thread.
  //! @return true if current thread is bound to this GL context
  Standard_EXPORT Standard_Boolean IsCurrent() const;

  //! Activates current context.
  //! Class should be initialized with appropriate info.
  Standard_EXPORT Standard_Boolean MakeCurrent();

  //! Swap front/back buffers for this GL context (should be activated before!).
  Standard_EXPORT void SwapBuffers();

  //! Setup swap interval (VSync).
  Standard_EXPORT Standard_Boolean SetSwapInterval (const Standard_Integer theInterval);

  //! Return true if active mode is GL_RENDER (cached state)
  Standard_Boolean IsRender() const
  {
  #if !defined(GL_ES_VERSION_2_0)
    return myRenderMode == GL_RENDER;
  #else
    return Standard_True;
  #endif
  }

  //! Return true if active mode is GL_FEEDBACK (cached state)
  Standard_Boolean IsFeedback() const
  {
  #if !defined(GL_ES_VERSION_2_0)
    return myRenderMode == GL_FEEDBACK;
  #else
    return Standard_False;
  #endif
  }

  //! This function retrieves information from GL about free GPU memory that is:
  //!  - OS-dependent. On some OS it is per-process and on others - for entire system.
  //!  - Vendor-dependent. Currently available only on NVIDIA and AMD/ATi drivers only.
  //!  - Numbers meaning may vary.
  //! You should use this info only for diagnostics purposes.
  //! @return free GPU dedicated memory in bytes.
  Standard_EXPORT Standard_Size AvailableMemory() const;

  //! This function retrieves information from GL about GPU memory
  //! and contains more vendor-specific values than AvailableMemory().
  Standard_EXPORT TCollection_AsciiString MemoryInfo() const;

  //! This function retrieves information from GL about GPU memory.
  Standard_EXPORT void MemoryInfo (TColStd_IndexedDataMapOfStringString& theDict) const;

  //! Fill in the dictionary with OpenGL info.
  //! Should be called with bound context.
  Standard_EXPORT void DiagnosticInformation (TColStd_IndexedDataMapOfStringString& theDict,
                                              Graphic3d_DiagnosticInfo theFlags) const;

  //! Access shared resource by its name.
  //! @param  theKey - unique identifier;
  //! @return handle to shared resource or NULL.
  Standard_EXPORT const Handle(OpenGl_Resource)& GetResource (const TCollection_AsciiString& theKey) const;

  //! Access shared resource by its name.
  //! @param  theKey   - unique identifier;
  //! @param  theValue - handle to fill;
  //! @return true if resource was shared.
  template<typename TheHandleType>
  Standard_Boolean GetResource (const TCollection_AsciiString& theKey,
                                TheHandleType&                 theValue) const
  {
    const Handle(OpenGl_Resource)& aResource = GetResource (theKey);
    if (aResource.IsNull())
    {
      return Standard_False;
    }

    theValue = TheHandleType::DownCast (aResource);
    return !theValue.IsNull();
  }

  //! Register shared resource.
  //! Notice that after registration caller shouldn't release it by himself -
  //! it will be automatically released on context destruction.
  //! @param theKey      - unique identifier, shouldn't be empty;
  //! @param theResource - new resource to register, shouldn't be NULL.
  Standard_EXPORT Standard_Boolean ShareResource (const TCollection_AsciiString& theKey,
                                                  const Handle(OpenGl_Resource)& theResource);

  //! Release shared resource.
  //! If there are more than one reference to this resource
  //! (also used by some other existing object) then call will be ignored.
  //! This means that current object itself should nullify handle before this call.
  //! Notice that this is unrecommended operation at all and should be used
  //! only in case of fat resources to release memory for other needs.
  //! @param theKey     unique identifier
  //! @param theToDelay postpone release until next redraw call
  Standard_EXPORT void ReleaseResource (const TCollection_AsciiString& theKey,
                                        const Standard_Boolean         theToDelay = Standard_False);

  //! Append resource to queue for delayed clean up.
  //! Resources in this queue will be released at next redraw call.
  template <class T>
  void DelayedRelease (Handle(T)& theResource)
  {
    myUnusedResources->Prepend (theResource);
    theResource.Nullify();
  }

  //! Clean up the delayed release queue.
  Standard_EXPORT void ReleaseDelayed();

  //! Return map of shared resources.
  const OpenGl_ResourcesMap& SharedResources() const { return *mySharedResources; }

  //! @return tool for management of clippings within this context.
  inline OpenGl_Clipping& ChangeClipping() { return myClippingState; }

  //! @return tool for management of clippings within this context.
  inline const OpenGl_Clipping& Clipping() const { return myClippingState; }

  //! @return tool for management of shader programs within this context.
  inline const Handle(OpenGl_ShaderManager)& ShaderManager() const { return myShaderManager; }

public:

  //! Either GL_CLAMP_TO_EDGE (1.2+) or GL_CLAMP (1.1).
  Standard_Integer TextureWrapClamp() const { return myTexClamp; }

  //! @return true if texture parameters GL_TEXTURE_BASE_LEVEL/GL_TEXTURE_MAX_LEVEL are supported.
  Standard_Boolean HasTextureBaseLevel() const
  {
  #if !defined(GL_ES_VERSION_2_0)
    return IsGlGreaterEqual (1, 2);
  #else
    return IsGlGreaterEqual (3, 0);
  #endif
  }

  //! Return map of supported texture formats.
  const Handle(Image_SupportedFormats)& SupportedTextureFormats() const { return mySupportedFormats; }

  //! @return maximum degree of anisotropy texture filter
  Standard_Integer MaxDegreeOfAnisotropy() const { return myAnisoMax; }

  //! @return value for GL_MAX_TEXTURE_SIZE
  Standard_Integer MaxTextureSize() const { return myMaxTexDim; }

  //! @return value for GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
  Standard_Integer MaxCombinedTextureUnits() const { return myMaxTexCombined; }

  //! This method returns the multi-texture limit for obsolete fixed-function pipeline.
  //! Use MaxCombinedTextureUnits() instead for limits for using programmable pipeline.
  //! @return value for GL_MAX_TEXTURE_UNITS
  Standard_Integer MaxTextureUnitsFFP() const { return myMaxTexUnitsFFP; }

  //! Return texture unit to be used for sprites (Graphic3d_TextureUnit_PointSprite by default).
  Graphic3d_TextureUnit SpriteTextureUnit() const { return mySpriteTexUnit; }

  //! @return value for GL_MAX_SAMPLES
  Standard_Integer MaxMsaaSamples() const { return myMaxMsaaSamples; }

  //! @return maximum FBO width for image dump
  Standard_Integer MaxDumpSizeX() const { return myMaxDumpSizeX; }

  //! @return maximum FBO height for image dump
  Standard_Integer MaxDumpSizeY() const { return myMaxDumpSizeY; }

  //! @return value for GL_MAX_DRAW_BUFFERS
  Standard_Integer MaxDrawBuffers() const { return myMaxDrawBuffers; }

  //! @return value for GL_MAX_COLOR_ATTACHMENTS
  Standard_Integer MaxColorAttachments() const { return myMaxColorAttachments; }

  //! Get maximum number of clip planes supported by OpenGl.
  //! This value is implementation dependent. At least 6
  //! planes should be supported by OpenGl (see specs).
  //! @return value for GL_MAX_CLIP_PLANES
  Standard_Integer MaxClipPlanes() const { return myMaxClipPlanes; }

  //! @return TRUE if ray tracing mode is supported
  Standard_Boolean HasRayTracing() const { return myHasRayTracing; }

  //! @return TRUE if textures in ray tracing mode are supported
  Standard_Boolean HasRayTracingTextures() const { return myHasRayTracingTextures; }

  //! @return TRUE if adaptive screen sampling in ray tracing mode is supported
  Standard_Boolean HasRayTracingAdaptiveSampling() const { return myHasRayTracingAdaptiveSampling; }

  //! @return TRUE if atomic adaptive screen sampling in ray tracing mode is supported
  Standard_Boolean HasRayTracingAdaptiveSamplingAtomic() const { return myHasRayTracingAdaptiveSamplingAtomic; }

  //! Returns TRUE if sRGB rendering is supported.
  bool HasSRGB() const
  {
    return hasTexSRGB
       &&  hasFboSRGB;
  }

  //! Returns TRUE if sRGB rendering is supported and permitted.
  bool ToRenderSRGB() const
  {
    return HasSRGB()
       && !caps->sRGBDisable
       && !caps->ffpEnable;
  }

  //! Returns TRUE if window/surface buffer is sRGB-ready.
  //!
  //! When offscreen FBOs are created in sRGB, but window is not sRGB-ready,
  //! blitting into window should be done with manual gamma correction.
  //!
  //! In desktop OpenGL, window buffer can be considered as sRGB-ready by default,
  //! even when application has NOT requested sRGB-ready pixel format,
  //! and rendering is managed via GL_FRAMEBUFFER_SRGB state.
  //!
  //! In OpenGL ES, sRGB-ready window surface should be explicitly requested on construction,
  //! and cannot be disabled/enabled without GL_EXT_sRGB_write_control extension afterwards
  //! (GL_FRAMEBUFFER_SRGB can be considered as always tuned ON).
  bool IsWindowSRGB() const { return myIsSRgbWindow; }

  //! Overrides if window/surface buffer is sRGB-ready or not (initialized with the context).
  void SetWindowSRGB (bool theIsSRgb) { myIsSRgbWindow = theIsSRgb; }

  //! Convert Quantity_ColorRGBA into vec4
  //! with conversion or no conversion into non-linear sRGB
  //! basing on ToRenderSRGB() flag.
  OpenGl_Vec4 Vec4FromQuantityColor (const OpenGl_Vec4& theColor) const
  {
    return myIsSRgbActive
         ? Vec4LinearFromQuantityColor(theColor)
         : Vec4sRGBFromQuantityColor  (theColor);
  }

  //! Convert Quantity_ColorRGBA into vec4.
  //! Quantity_Color is expected to be linear RGB, hence conversion is NOT required
  const OpenGl_Vec4& Vec4LinearFromQuantityColor (const OpenGl_Vec4& theColor) const { return theColor; }

  //! Convert Quantity_ColorRGBA (linear RGB) into non-linear sRGB vec4.
  OpenGl_Vec4 Vec4sRGBFromQuantityColor (const OpenGl_Vec4& theColor) const
  {
    return Quantity_ColorRGBA::Convert_LinearRGB_To_sRGB (theColor);
  }

  //! Returns TRUE if PBR shading model is supported.
  //! Basically, feature requires OpenGL 3.0+ / OpenGL ES 3.0+ hardware; more precisely:
  //! - Graphics hardware with moderate capabilities for compiling long enough GLSL program.
  //! - FBO (e.g. for baking environment).
  //! - Multi-texturing with >= 4 units (LUT and IBL textures).
  //! - GL_RG32F texture format (arbTexRG + arbTexFloat)
  //! - Cubemap texture lookup textureCubeLod()/textureLod() with LOD index within Fragment Shader,
  //!   which requires GLSL OpenGL 3.0+ / OpenGL ES 3.0+ or OpenGL 2.1 + GL_EXT_gpu_shader4 extension.
  Standard_Boolean HasPBR() const { return myHasPBR; }

  //! Returns texture unit where Environment Lookup Table is expected to be bound, or 0 if PBR is unavailable.
  Graphic3d_TextureUnit PBREnvLUTTexUnit() const { return myPBREnvLUTTexUnit; }

  //! Returns texture unit where Diffuse (irradiance) IBL map's spherical harmonics coefficients is expected to be bound, or 0 if PBR is unavailable.
  Graphic3d_TextureUnit PBRDiffIBLMapSHTexUnit() const { return myPBRDiffIBLMapSHTexUnit; }

  //! Returns texture unit where Specular IBL map is expected to be bound, or 0 if PBR is unavailable.
  Graphic3d_TextureUnit PBRSpecIBLMapTexUnit() const { return myPBRSpecIBLMapTexUnit; }

  //! Returns texture unit where shadow map is expected to be bound, or 0 if unavailable.
  Graphic3d_TextureUnit ShadowMapTexUnit() const { return myShadowMapTexUnit; }

  //! Returns texture unit for occDepthPeelingDepth within enabled Depth Peeling.
  Graphic3d_TextureUnit DepthPeelingDepthTexUnit() const { return myDepthPeelingDepthTexUnit; }

  //! Returns texture unit for occDepthPeelingFrontColor within enabled Depth Peeling.
  Graphic3d_TextureUnit DepthPeelingFrontColorTexUnit() const { return myDepthPeelingFrontColorTexUnit; }

  //! Returns true if VBO is supported and permitted.
  inline bool ToUseVbo() const
  {
    return core15fwd != NULL
       && !caps->vboDisable;
  }

  //! @return cached state of GL_NORMALIZE.
  Standard_Boolean IsGlNormalizeEnabled() const { return myIsGlNormalizeEnabled; }

  //! Sets GL_NORMALIZE enabled or disabled.
  //! @return old value of the flag
  Standard_EXPORT Standard_Boolean SetGlNormalizeEnabled (Standard_Boolean isEnabled);

  //! @return cached state of polygon rasterization mode (glPolygonMode()).
  Standard_Integer PolygonMode() const { return myPolygonMode; }

  //! Sets polygon rasterization mode (glPolygonMode() function).
  //! @return old value of the rasterization mode.
  Standard_EXPORT Standard_Integer SetPolygonMode (const Standard_Integer theMode);

  //! @return cached enabled state of polygon hatching rasterization.
  bool IsPolygonHatchEnabled() const { return myHatchIsEnabled; }

  //! Sets enabled state of polygon hatching rasterization
  //! without affecting currently selected hatching pattern.
  //! @return previous state of polygon hatching mode.
  Standard_EXPORT bool SetPolygonHatchEnabled (const bool theIsEnabled);

  //! @return cached state of polygon hatch type.
  Standard_Integer PolygonHatchStyle() const { return myActiveHatchType; }

  //! Sets polygon hatch pattern.
  //! Zero-index value is a default alias for solid filling.
  //! @param the type of hatch supported by base implementation of
  //! OpenGl_LineAttributes (Aspect_HatchStyle) or the type supported by custom
  //! implementation derived from OpenGl_LineAttributes class.
  //! @return old type of hatch.
  Standard_EXPORT Standard_Integer SetPolygonHatchStyle (const Handle(Graphic3d_HatchStyle)& theStyle);

  //! Sets and applies current polygon offset.
  Standard_EXPORT void SetPolygonOffset (const Graphic3d_PolygonOffset& theOffset);

  //! Returns currently applied polygon offset parameters.
  const Graphic3d_PolygonOffset& PolygonOffset() const { return myPolygonOffset; }

  //! Returns camera object.
  const Handle(Graphic3d_Camera)& Camera() const { return myCamera; }

  //! Sets camera object to the context and update matrices.
  Standard_EXPORT void SetCamera (const Handle(Graphic3d_Camera)& theCamera);

  //! Applies matrix into shader manager stored in ModelWorldState to OpenGl.
  //! In "model -> world -> view -> projection" it performs:
  //!     model -> world
  Standard_EXPORT void ApplyModelWorldMatrix();

  //! Applies matrix stored in WorldViewState to OpenGl.
  //! In "model -> world -> view -> projection" it performs:
  //!     model -> world -> view,
  //! where model -> world is identical matrix
  Standard_EXPORT void ApplyWorldViewMatrix();

  //! Applies combination of matrices stored in ModelWorldState and WorldViewState to OpenGl.
  //! In "model -> world -> view -> projection" it performs:
  //!     model -> world -> view
  Standard_EXPORT void ApplyModelViewMatrix();

  //! Applies matrix stored in ProjectionState to OpenGl.
  //! In "model -> world -> view -> projection" it performs:
  //!                       view -> projection
  Standard_EXPORT void ApplyProjectionMatrix();

public:

  //! @return messenger instance
  inline const Handle(Message_Messenger)& Messenger() const
  {
    return ::Message::DefaultMessenger();
  }

  //! Callback for GL_ARB_debug_output extension
  //! @param theSource   message source   within GL_DEBUG_SOURCE_   enumeration
  //! @param theType     message type     within GL_DEBUG_TYPE_     enumeration
  //! @param theId       message ID       within source
  //! @param theSeverity message severity within GL_DEBUG_SEVERITY_ enumeration
  //! @param theMessage  the message itself
  Standard_EXPORT void PushMessage (const unsigned int theSource,
                                    const unsigned int theType,
                                    const unsigned int theId,
                                    const unsigned int theSeverity,
                                    const TCollection_ExtendedString& theMessage);

  //! Adds a filter for messages with theId and theSource (GL_DEBUG_SOURCE_)
  Standard_EXPORT Standard_Boolean ExcludeMessage (const unsigned int theSource,
                                                   const unsigned int theId);

  //! Removes a filter for messages with theId and theSource (GL_DEBUG_SOURCE_)
  Standard_EXPORT Standard_Boolean IncludeMessage (const unsigned int theSource,
                                                   const unsigned int theId);

  //! @return true if OpenGl context supports left and
  //! right rendering buffers.
  Standard_Boolean HasStereoBuffers() const
  {
  #if !defined(GL_ES_VERSION_2_0)
    return myIsStereoBuffers;
  #else
    return Standard_False;
  #endif
  }

public: //! @name methods to alter or retrieve current state

  //! Return structure holding frame statistics.
  const Handle(OpenGl_FrameStats)& FrameStats() const { return myFrameStats; }

  //! Set structure holding frame statistics.
  //! This call makes sense only if application defines OpenGl_FrameStats sub-class.
  void SetFrameStats (const Handle(OpenGl_FrameStats)& theStats) { myFrameStats = theStats; }

  //! Return cached viewport definition (x, y, width, height).
  const Standard_Integer* Viewport() const { return myViewport; }

  //! Resize the viewport (alias for glViewport).
  //! @param theRect viewport definition (x, y, width, height)
  Standard_EXPORT void ResizeViewport (const Standard_Integer theRect[4]);

  //! Return virtual viewport definition (x, y, width, height).
  const Standard_Integer* VirtualViewport() const { return myViewportVirt; }

  //! Return active read buffer.
  Standard_Integer ReadBuffer() { return myReadBuffer; }

  //! Switch read buffer, wrapper for ::glReadBuffer().
  Standard_EXPORT void SetReadBuffer (const Standard_Integer theReadBuffer);

  //! Return active draw buffer attached to a render target referred by index (layout location).
  Standard_Integer DrawBuffer (Standard_Integer theIndex = 0) const
  {
    return theIndex >= myDrawBuffers.Lower()
        && theIndex <= myDrawBuffers.Upper()
         ? myDrawBuffers.Value (theIndex)
         : GL_NONE;
  }

  //! Switch draw buffer, wrapper for ::glDrawBuffer().
  Standard_EXPORT void SetDrawBuffer (const Standard_Integer theDrawBuffer);

  //! Switch draw buffer, wrapper for ::glDrawBuffers (GLsizei, const GLenum*).
  Standard_EXPORT void SetDrawBuffers (const Standard_Integer theNb, const Standard_Integer* theDrawBuffers);

  //! Switch read/draw buffers.
  void SetReadDrawBuffer (const Standard_Integer theBuffer)
  {
    SetReadBuffer (theBuffer);
    SetDrawBuffer (theBuffer);
  }

  //! Returns cached GL_FRAMEBUFFER_SRGB state.
  //! If TRUE, GLSL program is expected to write linear RGB color.
  //! Otherwise, GLSL program might need manually converting result color into sRGB color space.
  bool IsFrameBufferSRGB() const { return myIsSRgbActive; }

  //! Enables/disables GL_FRAMEBUFFER_SRGB flag.
  //! This flag can be set to:
  //! - TRUE when writing into offscreen FBO (always expected to be in sRGB or RGBF formats).
  //! - TRUE when writing into sRGB-ready window buffer (might require choosing proper pixel format on window creation).
  //! - FALSE if sRGB rendering is not supported or sRGB-not-ready window buffer is used for drawing.
  //! @param theIsFbo [in] flag indicating writing into offscreen FBO (always expected sRGB-ready when sRGB FBO is supported)
  //!                      or into window buffer (FALSE, sRGB-readiness might vary).
  //! @param theIsSRgb [in] flag indicating off-screen FBO is sRGB-ready
  Standard_EXPORT void SetFrameBufferSRGB (bool theIsFbo, bool theIsFboSRgb = true);

  //! Return cached flag indicating writing into color buffer is enabled or disabled (glColorMask).
  const NCollection_Vec4<bool>& ColorMaskRGBA() const { return myColorMask; }

  //! Enable/disable writing into color buffer (wrapper for glColorMask).
  Standard_EXPORT void SetColorMaskRGBA (const NCollection_Vec4<bool>& theToWriteColor);

  //! Return cached flag indicating writing into color buffer is enabled or disabled (glColorMask).
  bool ColorMask() const { return myColorMask.r(); }

  //! Enable/disable writing into color buffer (wrapper for glColorMask).
  //! Alpha component writes will be disabled unconditionally in case of caps->buffersOpaqueAlpha.
  Standard_EXPORT bool SetColorMask (bool theToWriteColor);

  //! Return TRUE if GL_SAMPLE_ALPHA_TO_COVERAGE usage is allowed.
  bool AllowSampleAlphaToCoverage() const { return myAllowAlphaToCov; }

  //! Allow GL_SAMPLE_ALPHA_TO_COVERAGE usage.
  void SetAllowSampleAlphaToCoverage (bool theToEnable) { myAllowAlphaToCov = theToEnable; }

  //! Return GL_SAMPLE_ALPHA_TO_COVERAGE state.
  bool SampleAlphaToCoverage() const { return myAlphaToCoverage; }

  //! Enable/disable GL_SAMPLE_ALPHA_TO_COVERAGE.
  Standard_EXPORT bool SetSampleAlphaToCoverage (bool theToEnable);

  //! Return back face culling state.
  bool ToCullBackFaces() const { return myToCullBackFaces; }

  //! Enable or disable back face culling (glEnable (GL_CULL_FACE)).
  Standard_EXPORT void SetCullBackFaces (bool theToEnable);

  //! Fetch OpenGl context state. This class tracks value of several OpenGl
  //! state variables. Consulting the cached values is quicker than
  //! doing the same via OpenGl API. Call this method if any of the controlled
  //! OpenGl state variables has a possibility of being out-of-date.
  Standard_EXPORT void FetchState();

  //! @return active textures
  const Handle(OpenGl_TextureSet)& ActiveTextures() const { return myActiveTextures; }

  //! Bind specified texture set to current context taking into account active GLSL program.
  Standard_DEPRECATED("BindTextures() with explicit GLSL program should be used instead")
  Handle(OpenGl_TextureSet) BindTextures (const Handle(OpenGl_TextureSet)& theTextures)
  {
    return BindTextures (theTextures, myActiveProgram);
  }

  //! Bind specified texture set to current context, or unbind previous one when NULL specified.
  //! @param theTextures [in] texture set to bind
  //! @param theProgram  [in] program attributes; when not NULL,
  //!                         mock textures will be bound to texture units expected by GLSL program, but undefined by texture set
  //! @return previous texture set
  Standard_EXPORT Handle(OpenGl_TextureSet) BindTextures (const Handle(OpenGl_TextureSet)& theTextures,
                                                          const Handle(OpenGl_ShaderProgram)& theProgram);

  //! @return active GLSL program
  const Handle(OpenGl_ShaderProgram)& ActiveProgram() const
  {
    return myActiveProgram;
  }

  //! Bind specified program to current context,
  //! or unbind previous one when NULL specified.
  //! @return true if some program is bound to context
  Standard_EXPORT Standard_Boolean BindProgram (const Handle(OpenGl_ShaderProgram)& theProgram);

  //! Setup current shading material.
  Standard_EXPORT void SetShadingMaterial (const OpenGl_Aspects* theAspect,
                                           const Handle(Graphic3d_PresentationAttributes)& theHighlight);

  //! Checks if transparency is required for the given aspect and highlight style.
  Standard_EXPORT static Standard_Boolean CheckIsTransparent (const OpenGl_Aspects* theAspect,
                                                              const Handle(Graphic3d_PresentationAttributes)& theHighlight,
                                                              Standard_ShortReal& theAlphaFront,
                                                              Standard_ShortReal& theAlphaBack);

  //! Checks if transparency is required for the given aspect and highlight style.
  static Standard_Boolean CheckIsTransparent (const OpenGl_Aspects* theAspect,
                                              const Handle(Graphic3d_PresentationAttributes)& theHighlight)
  {
    Standard_ShortReal anAlphaFront = 1.0f, anAlphaBack = 1.0f;
    return CheckIsTransparent (theAspect, theHighlight, anAlphaFront, anAlphaBack);
  }

  //! Setup current color.
  Standard_EXPORT void SetColor4fv (const OpenGl_Vec4& theColor);

  //! Setup type of line.
  Standard_EXPORT void SetTypeOfLine (const Aspect_TypeOfLine  theType,
                                      const Standard_ShortReal theFactor = 1.0f);

  //! Setup stipple line pattern with 1.0f factor; wrapper for glLineStipple().
  void SetLineStipple (const uint16_t thePattern) { SetLineStipple (1.0f, thePattern); }

  //! Setup type of line; wrapper for glLineStipple().
  Standard_EXPORT void SetLineStipple (const Standard_ShortReal theFactor,
                                       const uint16_t thePattern);

  //! Setup width of line.
  Standard_EXPORT void SetLineWidth (const Standard_ShortReal theWidth);

  //! Setup point size.
  Standard_EXPORT void SetPointSize (const Standard_ShortReal theSize);

  //! Setup point sprite origin using GL_POINT_SPRITE_COORD_ORIGIN state:
  //! - GL_UPPER_LEFT when GLSL program is active;
  //!   flipping should be handled in GLSL program for compatibility with OpenGL ES
  //! - GL_LOWER_LEFT for FFP
  Standard_EXPORT void SetPointSpriteOrigin();

  //! Setup texture matrix to active GLSL program or to FFP global state using glMatrixMode (GL_TEXTURE).
  //! @param theParams    [in] texture parameters
  //! @param theIsTopDown [in] texture top-down flag
  Standard_EXPORT void SetTextureMatrix (const Handle(Graphic3d_TextureParams)& theParams,
                                         const Standard_Boolean theIsTopDown);

  //! Bind default Vertex Array Object
  Standard_EXPORT void BindDefaultVao();

  //! Default Frame Buffer Object.
  const Handle(OpenGl_FrameBuffer)& DefaultFrameBuffer() const
  {
    return myDefaultFbo;
  }

  //! Setup new Default Frame Buffer Object and return previously set.
  //! This call doesn't change Active FBO!
  Standard_EXPORT Handle(OpenGl_FrameBuffer) SetDefaultFrameBuffer (const Handle(OpenGl_FrameBuffer)& theFbo);

  //! Return debug context initialization state.
  Standard_Boolean IsDebugContext() const
  {
    return myIsGlDebugCtx;
  }

  Standard_EXPORT void EnableFeatures() const;

  Standard_EXPORT void DisableFeatures() const;

  //! Return resolution for rendering text.
  unsigned int Resolution() const { return myResolution; }

  //! Resolution scale factor (rendered resolution to standard resolution).
  //! This scaling factor for parameters like text size to be properly displayed on device (screen / printer).
  Standard_ShortReal ResolutionRatio() const { return myResolutionRatio; }

  //! Rendering scale factor (rendering viewport height to real window buffer height).
  Standard_ShortReal RenderScale() const { return myRenderScale; }

  //! Return TRUE if rendering scale factor is not 1.
  Standard_Boolean HasRenderScale() const { return Abs (myRenderScale - 1.0f) > 0.0001f; }

  //! Rendering scale factor (inverted value).
  Standard_ShortReal RenderScaleInv() const { return myRenderScaleInv; }

  //! Return scale factor for line width.
  Standard_ShortReal LineWidthScale() const { return myLineWidthScale; }

  //! Set resolution ratio.
  //! Note that this method rounds @theRatio to nearest integer.
  void SetResolution (unsigned int theResolution,
                      Standard_ShortReal theRatio,
                      Standard_ShortReal theScale)
  {
    myResolution      = (unsigned int )(theScale * theResolution + 0.5f);
    myRenderScale     = theScale;
    myRenderScaleInv  = 1.0f / theScale;
    SetResolutionRatio (theRatio * theScale);
  }

  //! Set resolution ratio.
  //! Note that this method rounds @theRatio to nearest integer.
  void SetResolutionRatio (const Standard_ShortReal theRatio)
  {
    myResolutionRatio = theRatio;
    myLineWidthScale  = Max (1.0f, std::floor (theRatio + 0.5f));
  }

  //! Return line feater width in pixels.
  Standard_ShortReal LineFeather() const { return myLineFeather; }

  //! Set line feater width.
  void SetLineFeather(Standard_ShortReal theValue) { myLineFeather = theValue; }

  //! Wrapper over glGetBufferSubData(), implemented as:
  //! - OpenGL 1.5+ (desktop) via glGetBufferSubData();
  //! - OpenGL ES 3.0+ via glMapBufferRange();
  //! - WebGL 2.0+ via gl.getBufferSubData().
  //! @param theTarget [in] target buffer to map
  //! @param theOffset [in] offset to the beginning of sub-data
  //! @param theSize   [in] number of bytes to read
  //! @param theData  [out] destination pointer to fill
  //! @return FALSE if functionality is unavailable
  Standard_EXPORT bool GetBufferSubData (GLenum theTarget, GLintptr theOffset, GLsizeiptr theSize, void* theData);

  //! Return Graphics Driver's vendor.
  const TCollection_AsciiString& Vendor() const { return myVendor; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;
    
  //! Dumps the content of openGL state into the stream
  Standard_EXPORT static void DumpJsonOpenGlState (Standard_OStream& theOStream, Standard_Integer theDepth = -1);

  //! Set GL_SHADE_MODEL value.
  Standard_EXPORT void SetShadeModel (Graphic3d_TypeOfShadingModel theModel);

private:

  //! Wrapper to system function to retrieve GL function pointer by name.
  Standard_EXPORT void* findProc (const char* theFuncName);

  //! Print error if not all functions have been exported by context for reported version.
  //! Note that this will never happen when using GLX, since returned functions can not be validated.
  //! @param theGlVerMajor the OpenGL major version with missing functions
  //! @param theGlVerMinor the OpenGL minor version with missing functions
  //! @param theLastFailedProc function name which cannot be found
  Standard_EXPORT void checkWrongVersion (Standard_Integer theGlVerMajor, Standard_Integer theGlVerMinor,
                                          const char* theLastFailedProc);

  //! Private initialization function that should be called only once.
  Standard_EXPORT void init (const Standard_Boolean theIsCoreProfile);

public: //! @name core profiles

  OpenGl_GlCore11*     core11ffp;  //!< OpenGL 1.1 core functionality
  OpenGl_GlCore11Fwd*  core11fwd;  //!< OpenGL 1.1 without deprecated entry points
  OpenGl_GlCore15*     core15;     //!< OpenGL 1.5 without deprecated entry points
  OpenGl_GlCore20*     core20;     //!< OpenGL 2.0 without deprecated entry points
  OpenGl_GlCore30*     core30;     //!< OpenGL 3.0 without deprecated entry points
  OpenGl_GlCore32*     core32;     //!< OpenGL 3.2 core profile
  OpenGl_GlCore33*     core33;     //!< OpenGL 3.3 core profile
  OpenGl_GlCore41*     core41;     //!< OpenGL 4.1 core profile
  OpenGl_GlCore42*     core42;     //!< OpenGL 4.2 core profile
  OpenGl_GlCore43*     core43;     //!< OpenGL 4.3 core profile
  OpenGl_GlCore44*     core44;     //!< OpenGL 4.4 core profile
  OpenGl_GlCore45*     core45;     //!< OpenGL 4.5 core profile
  OpenGl_GlCore46*     core46;     //!< OpenGL 4.6 core profile

  OpenGl_GlCore15*     core15fwd;  //!< obsolete entry left for code portability; core15 should be used instead
  OpenGl_GlCore20*     core20fwd;  //!< obsolete entry left for code portability; core20 should be used instead

  Handle(OpenGl_Caps) caps; //!< context options

public: //! @name extensions

  Standard_Boolean       hasGetBufferData;   //!< flag indicating if GetBufferSubData() is supported
  Standard_Boolean       hasPackRowLength;   //!< supporting of GL_PACK_ROW_LENGTH   parameters (any desktop OpenGL; OpenGL ES 3.0)
  Standard_Boolean       hasUnpackRowLength; //!< supporting of GL_UNPACK_ROW_LENGTH parameters (any desktop OpenGL; OpenGL ES 3.0)
  Standard_Boolean       hasHighp;           //!< highp in GLSL ES fragment shader is supported
  Standard_Boolean       hasUintIndex;       //!< GLuint for index buffer is supported (always available on desktop; on OpenGL ES - since 3.0 or as extension GL_OES_element_index_uint)
  Standard_Boolean       hasTexRGBA8;        //!< always available on desktop; on OpenGL ES - since 3.0 or as extension GL_OES_rgb8_rgba8
  Standard_Boolean       hasTexFloatLinear;  //!< texture-filterable state for 32-bit floating texture formats (always on desktop, GL_OES_texture_float_linear within OpenGL ES)
  Standard_Boolean       hasTexSRGB;         //!< sRGB texture    formats (desktop OpenGL 2.1, OpenGL ES 3.0 or OpenGL ES 2.0 + GL_EXT_sRGB)
  Standard_Boolean       hasFboSRGB;         //!< sRGB FBO render targets (desktop OpenGL 2.1, OpenGL ES 3.0)
  Standard_Boolean       hasSRGBControl;     //!< sRGB write control (any desktop OpenGL, OpenGL ES + GL_EXT_sRGB_write_control extension)
  Standard_Boolean       hasFboRenderMipmap; //!< FBO render target could be non-zero mipmap level of texture
  OpenGl_FeatureFlag     hasFlatShading;     //!< Complex flag indicating support of Flat shading (Graphic3d_TypeOfShadingModel_Phong) (always available on desktop; on OpenGL ES - since 3.0 or as extension GL_OES_standard_derivatives)
  OpenGl_FeatureFlag     hasGlslBitwiseOps;  //!< GLSL supports bitwise operations; OpenGL 3.0 / OpenGL ES 3.0 (GLSL 130 / GLSL ES 300) or OpenGL 2.1 + GL_EXT_gpu_shader4
  OpenGl_FeatureFlag     hasDrawBuffers;     //!< Complex flag indicating support of multiple draw buffers (desktop OpenGL 2.0, OpenGL ES 3.0, GL_ARB_draw_buffers, GL_EXT_draw_buffers)
  OpenGl_FeatureFlag     hasFloatBuffer;     //!< Complex flag indicating support of float color buffer format (desktop OpenGL 3.0, GL_ARB_color_buffer_float, GL_EXT_color_buffer_float)
  OpenGl_FeatureFlag     hasHalfFloatBuffer; //!< Complex flag indicating support of half-float color buffer format (desktop OpenGL 3.0, GL_ARB_color_buffer_float, GL_EXT_color_buffer_half_float)
  OpenGl_FeatureFlag     hasSampleVariables; //!< Complex flag indicating support of MSAA variables in GLSL shader (desktop OpenGL 4.0, GL_ARB_sample_shading)
  OpenGl_FeatureFlag     hasGeometryStage;   //!< Complex flag indicating support of Geometry shader (desktop OpenGL 3.2, OpenGL ES 3.2, GL_EXT_geometry_shader)
  Standard_Boolean       arbDrawBuffers;     //!< GL_ARB_draw_buffers
  Standard_Boolean       arbNPTW;            //!< GL_ARB_texture_non_power_of_two
  Standard_Boolean       arbTexRG;           //!< GL_ARB_texture_rg
  Standard_Boolean       arbTexFloat;        //!< GL_ARB_texture_float (on desktop OpenGL - since 3.0 or as extension GL_ARB_texture_float; on OpenGL ES - since 3.0); @sa hasTexFloatLinear for linear filtering support
  OpenGl_ArbSamplerObject* arbSamplerObject; //!< GL_ARB_sampler_objects (on desktop OpenGL - since 3.3 or as extension GL_ARB_sampler_objects; on OpenGL ES - since 3.0)
  OpenGl_ArbTexBindless* arbTexBindless;     //!< GL_ARB_bindless_texture
  OpenGl_ArbTBO*         arbTBO;             //!< GL_ARB_texture_buffer_object (on desktop OpenGL - since 3.1 or as extension GL_ARB_texture_buffer_object; on OpenGL ES - since 3.2)
  Standard_Boolean       arbTboRGB32;        //!< GL_ARB_texture_buffer_object_rgb32 (3-component TBO), in core since 4.0 (on OpenGL ES - since 3.2)
  Standard_Boolean       arbClipControl;     //!< GL_ARB_clip_control, in core since 4.5
  OpenGl_ArbIns*         arbIns;             //!< GL_ARB_draw_instanced (on desktop OpenGL - since 3.1 or as extension GL_ARB_draw_instanced; on OpenGL ES - since 3.0 or as extension GL_ANGLE_instanced_arrays to WebGL 1.0)
  OpenGl_ArbDbg*         arbDbg;             //!< GL_ARB_debug_output (on desktop OpenGL - since 4.3 or as extension GL_ARB_debug_output; on OpenGL ES - since 3.2 or as extension GL_KHR_debug)
  OpenGl_ArbFBO*         arbFBO;             //!< GL_ARB_framebuffer_object
  OpenGl_ArbFBOBlit*     arbFBOBlit;         //!< glBlitFramebuffer function, moved out from OpenGl_ArbFBO structure for compatibility with OpenGL ES 2.0
  Standard_Boolean       arbSampleShading;   //!< GL_ARB_sample_shading
  Standard_Boolean       arbDepthClamp;      //!< GL_ARB_depth_clamp (on desktop OpenGL - since 3.2 or as extensions GL_ARB_depth_clamp,NV_depth_clamp; unavailable on OpenGL ES)
  Standard_Boolean       extFragDepth;       //!< GL_EXT_frag_depth on OpenGL ES 2.0 (gl_FragDepthEXT built-in variable, before OpenGL ES 3.0)
  Standard_Boolean       extDrawBuffers;     //!< GL_EXT_draw_buffers
  OpenGl_ExtGS*          extGS;              //!< GL_EXT_geometry_shader4
  Standard_Boolean       extBgra;            //!< GL_EXT_bgra or GL_EXT_texture_format_BGRA8888 on OpenGL ES
  Standard_Boolean       extAnis;            //!< GL_EXT_texture_filter_anisotropic
  Standard_Boolean       extPDS;             //!< GL_EXT_packed_depth_stencil
  Standard_Boolean       atiMem;             //!< GL_ATI_meminfo
  Standard_Boolean       nvxMem;             //!< GL_NVX_gpu_memory_info
  Standard_Boolean       oesSampleVariables; //!< GL_OES_sample_variables
  Standard_Boolean       oesStdDerivatives;  //!< GL_OES_standard_derivatives

public: //! @name public properties tracking current state

  OpenGl_MatrixState<Standard_ShortReal> ModelWorldState; //!< state of orientation matrix
  OpenGl_MatrixState<Standard_ShortReal> WorldViewState;  //!< state of orientation matrix
  OpenGl_MatrixState<Standard_ShortReal> ProjectionState; //!< state of projection  matrix

private: // system-dependent fields

  Aspect_Drawable         myWindow;   //!< surface           EGLSurface | HWND  | GLXDrawable
  Aspect_Display          myDisplay;  //!< display           EGLDisplay | HDC   | Display*
  Aspect_RenderingContext myGContext; //!< rendering context EGLContext | HGLRC | GLXContext | EAGLContext* | NSOpenGLContext*

private: // context info

  typedef NCollection_Shared< NCollection_DataMap<TCollection_AsciiString, Standard_Integer> > OpenGl_DelayReleaseMap;
  typedef NCollection_Shared< NCollection_List<Handle(OpenGl_Resource)> > OpenGl_ResourcesStack;

  Handle(OpenGl_ResourcesMap)    mySharedResources; //!< shared resources with unique identification key
  Handle(OpenGl_DelayReleaseMap) myDelayed;         //!< shared resources for delayed release
  Handle(OpenGl_ResourcesStack)  myUnusedResources; //!< stack of resources for delayed clean up

  OpenGl_Clipping myClippingState; //!< state of clip planes

  void*            myGlLibHandle;          //!< optional handle to GL library
  NCollection_Handle<OpenGl_GlFunctions>
                   myFuncs;                //!< mega structure for all GL functions
  Handle(Image_SupportedFormats)
                   mySupportedFormats;     //!< map of supported texture formats
  Standard_Integer myAnisoMax;             //!< maximum level of anisotropy texture filter
  Standard_Integer myTexClamp;             //!< either GL_CLAMP_TO_EDGE (1.2+) or GL_CLAMP (1.1)
  Standard_Integer myMaxTexDim;            //!< value for GL_MAX_TEXTURE_SIZE
  Standard_Integer myMaxTexCombined;       //!< value for GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
  Standard_Integer myMaxTexUnitsFFP;       //!< value for GL_MAX_TEXTURE_UNITS (fixed-function pipeline only)
  Standard_Integer myMaxDumpSizeX;         //!< maximum FBO width  for image dump
  Standard_Integer myMaxDumpSizeY;         //!< maximum FBO height for image dump
  Standard_Integer myMaxClipPlanes;        //!< value for GL_MAX_CLIP_PLANES
  Standard_Integer myMaxMsaaSamples;       //!< value for GL_MAX_SAMPLES
  Standard_Integer myMaxDrawBuffers;       //!< value for GL_MAX_DRAW_BUFFERS
  Standard_Integer myMaxColorAttachments;  //!< value for GL_MAX_COLOR_ATTACHMENTS
  Standard_Integer myGlVerMajor;           //!< cached GL version major number
  Standard_Integer myGlVerMinor;           //!< cached GL version minor number
  Standard_Boolean myIsInitialized;        //!< flag indicates initialization state
  Standard_Boolean myIsStereoBuffers;      //!< context supports stereo buffering
  Standard_Boolean myIsGlNormalizeEnabled; //!< GL_NORMALIZE flag
                                           //!< Used to tell OpenGl that normals should be normalized
  Graphic3d_TextureUnit mySpriteTexUnit;   //!< sampler2D occSamplerPointSprite, texture unit for point sprite texture

  Standard_Boolean myHasRayTracing;                 //! indicates whether ray tracing mode is supported
  Standard_Boolean myHasRayTracingTextures;         //! indicates whether textures in ray tracing mode are supported
  Standard_Boolean myHasRayTracingAdaptiveSampling; //! indicates whether adaptive screen sampling in ray tracing mode is supported
  Standard_Boolean myHasRayTracingAdaptiveSamplingAtomic; //! indicates whether atomic adaptive screen sampling in ray tracing mode is supported

  Standard_Boolean myHasPBR;                      //!< indicates whether PBR shading model is supported
  Graphic3d_TextureUnit myPBREnvLUTTexUnit;       //!< sampler2D occEnvLUT, texture unit where environment lookup table is expected to be binded (0 if PBR is not supported)
  Graphic3d_TextureUnit myPBRDiffIBLMapSHTexUnit; //!< sampler2D occDiffIBLMapSHCoeffs, texture unit where diffuse (irradiance) IBL map's spherical harmonics coefficients is expected to  be binded
                                                  //!  (0 if PBR is not supported)
  Graphic3d_TextureUnit myPBRSpecIBLMapTexUnit;   //!< samplerCube occSpecIBLMap, texture unit where specular IBL map is expected to  be binded (0 if PBR is not supported)
  Graphic3d_TextureUnit myShadowMapTexUnit;       //!< sampler2D occShadowMapSampler

  Graphic3d_TextureUnit myDepthPeelingDepthTexUnit;      //!< sampler2D occDepthPeelingDepth, texture unit for Depth Peeling lookups
  Graphic3d_TextureUnit myDepthPeelingFrontColorTexUnit; //!< sampler2D occDepthPeelingFrontColor, texture unit for Depth Peeling lookups

  Handle(OpenGl_ShaderManager) myShaderManager; //! support object for managing shader programs

private: //! @name fields tracking current state

  Handle(Graphic3d_Camera)      myCamera;          //!< active camera object
  Handle(OpenGl_FrameStats)     myFrameStats;      //!< structure accumulating frame statistics
  Handle(OpenGl_ShaderProgram)  myActiveProgram;   //!< currently active GLSL program
  Handle(OpenGl_TextureSet)     myActiveTextures;  //!< currently bound textures
                                                   //!< currently active sampler objects
  Standard_Integer              myActiveMockTextures; //!< currently active mock sampler objects
  Handle(OpenGl_FrameBuffer)    myDefaultFbo;      //!< default Frame Buffer Object
  Handle(OpenGl_LineAttributes) myHatchStyles;     //!< resource holding predefined hatch styles patterns
  Standard_Integer              myActiveHatchType; //!< currently activated type of polygon hatch
  Standard_Boolean              myHatchIsEnabled;  //!< current enabled state of polygon hatching rasterization
  Handle(OpenGl_Texture)        myTextureRgbaBlack;//!< mock black texture returning (0, 0, 0, 0)
  Handle(OpenGl_Texture)        myTextureRgbaWhite;//!< mock white texture returning (1, 1, 1, 1)
  Standard_Integer              myViewport[4];     //!< current viewport
  Standard_Integer              myViewportVirt[4]; //!< virtual viewport
  Standard_Integer              myPointSpriteOrig; //!< GL_POINT_SPRITE_COORD_ORIGIN state (GL_UPPER_LEFT by default)
  Standard_Integer              myRenderMode;      //!< value for active rendering mode
  Standard_Integer              myShadeModel;      //!< currently used shade model (glShadeModel)
  Standard_Integer              myPolygonMode;     //!< currently used polygon rasterization mode (glPolygonMode)
  Graphic3d_PolygonOffset       myPolygonOffset;   //!< currently applied polygon offset
  bool                          myToCullBackFaces; //!< back face culling mode enabled state (glIsEnabled (GL_CULL_FACE))
  Standard_Integer              myReadBuffer;      //!< current read buffer
  NCollection_Array1<Standard_Integer>
                                myDrawBuffers;     //!< current draw buffers
  unsigned int                  myDefaultVao;      //!< default Vertex Array Object
  NCollection_Vec4<bool>        myColorMask;       //!< flag indicating writing into color buffer is enabled or disabled (glColorMask)
  Standard_Boolean              myAllowAlphaToCov; //!< flag allowing   GL_SAMPLE_ALPHA_TO_COVERAGE usage
  Standard_Boolean              myAlphaToCoverage; //!< flag indicating GL_SAMPLE_ALPHA_TO_COVERAGE state
  Standard_Boolean              myIsGlDebugCtx;    //!< debug context initialization state
  Standard_Boolean              myIsSRgbWindow;    //!< indicates that window buffer is sRGB-ready
  Standard_Boolean              myIsSRgbActive;    //!< flag indicating GL_FRAMEBUFFER_SRGB state
  TCollection_AsciiString       myVendor;          //!< Graphics Driver's vendor
  TColStd_PackedMapOfInteger    myFilters[6];      //!< messages suppressing filter (for sources from GL_DEBUG_SOURCE_API_ARB to GL_DEBUG_SOURCE_OTHER_ARB)
  unsigned int                  myResolution;      //!< Pixels density (PPI), defines scaling factor for parameters like text size
  Standard_ShortReal            myResolutionRatio; //!< scaling factor for parameters like text size
                                                   //!  to be properly displayed on device (screen / printer)
  Standard_ShortReal            myLineWidthScale;  //!< scaling factor for line width
  Standard_ShortReal            myLineFeather;     //!< line feater width in pixels
  Standard_ShortReal            myRenderScale;     //!< scaling factor for rendering resolution
  Standard_ShortReal            myRenderScaleInv;  //!< scaling factor for rendering resolution (inverted value)
  OpenGl_Material               myMaterial;        //!< current front/back material state (cached to reduce GL context updates)

private:

  //! Copying allowed only within Handles
  OpenGl_Context            (const OpenGl_Context& );
  OpenGl_Context& operator= (const OpenGl_Context& );

};

#endif // _OpenGl_Context_H__
