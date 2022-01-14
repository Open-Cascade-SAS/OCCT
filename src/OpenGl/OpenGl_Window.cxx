// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <OpenGl_GlCore12.hxx>

#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_Window.hxx>

#include <Aspect_GraphicDeviceDefinitionError.hxx>
#include <Aspect_Handle.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#include <memory>

#include <Standard_WarningDisableFunctionCast.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Window,Standard_Transient)

#if defined(HAVE_EGL)
  #include <EGL/egl.h>
#elif defined(HAVE_XLIB)
  #include <GL/glx.h>
#endif

#if !defined(__APPLE__) || defined(HAVE_XLIB)

namespace
{

#if defined(HAVE_EGL)
  //
#elif defined(_WIN32)

  // WGL_ARB_pixel_format
#ifndef WGL_NUMBER_PIXEL_FORMATS_ARB
  #define WGL_NUMBER_PIXEL_FORMATS_ARB            0x2000
  #define WGL_DRAW_TO_WINDOW_ARB                  0x2001
  #define WGL_DRAW_TO_BITMAP_ARB                  0x2002
  #define WGL_ACCELERATION_ARB                    0x2003
  #define WGL_NEED_PALETTE_ARB                    0x2004
  #define WGL_NEED_SYSTEM_PALETTE_ARB             0x2005
  #define WGL_SWAP_LAYER_BUFFERS_ARB              0x2006
  #define WGL_SWAP_METHOD_ARB                     0x2007
  #define WGL_NUMBER_OVERLAYS_ARB                 0x2008
  #define WGL_NUMBER_UNDERLAYS_ARB                0x2009
  #define WGL_TRANSPARENT_ARB                     0x200A
  #define WGL_TRANSPARENT_RED_VALUE_ARB           0x2037
  #define WGL_TRANSPARENT_GREEN_VALUE_ARB         0x2038
  #define WGL_TRANSPARENT_BLUE_VALUE_ARB          0x2039
  #define WGL_TRANSPARENT_ALPHA_VALUE_ARB         0x203A
  #define WGL_TRANSPARENT_INDEX_VALUE_ARB         0x203B
  #define WGL_SHARE_DEPTH_ARB                     0x200C
  #define WGL_SHARE_STENCIL_ARB                   0x200D
  #define WGL_SHARE_ACCUM_ARB                     0x200E
  #define WGL_SUPPORT_GDI_ARB                     0x200F
  #define WGL_SUPPORT_OPENGL_ARB                  0x2010
  #define WGL_DOUBLE_BUFFER_ARB                   0x2011
  #define WGL_STEREO_ARB                          0x2012
  #define WGL_PIXEL_TYPE_ARB                      0x2013
  #define WGL_COLOR_BITS_ARB                      0x2014
  #define WGL_RED_BITS_ARB                        0x2015
  #define WGL_RED_SHIFT_ARB                       0x2016
  #define WGL_GREEN_BITS_ARB                      0x2017
  #define WGL_GREEN_SHIFT_ARB                     0x2018
  #define WGL_BLUE_BITS_ARB                       0x2019
  #define WGL_BLUE_SHIFT_ARB                      0x201A
  #define WGL_ALPHA_BITS_ARB                      0x201B
  #define WGL_ALPHA_SHIFT_ARB                     0x201C
  #define WGL_ACCUM_BITS_ARB                      0x201D
  #define WGL_ACCUM_RED_BITS_ARB                  0x201E
  #define WGL_ACCUM_GREEN_BITS_ARB                0x201F
  #define WGL_ACCUM_BLUE_BITS_ARB                 0x2020
  #define WGL_ACCUM_ALPHA_BITS_ARB                0x2021
  #define WGL_DEPTH_BITS_ARB                      0x2022
  #define WGL_STENCIL_BITS_ARB                    0x2023
  #define WGL_AUX_BUFFERS_ARB                     0x2024

  #define WGL_NO_ACCELERATION_ARB                 0x2025
  #define WGL_GENERIC_ACCELERATION_ARB            0x2026
  #define WGL_FULL_ACCELERATION_ARB               0x2027

  #define WGL_SWAP_EXCHANGE_ARB                   0x2028
  #define WGL_SWAP_COPY_ARB                       0x2029
  #define WGL_SWAP_UNDEFINED_ARB                  0x202A

  #define WGL_TYPE_RGBA_ARB                       0x202B
  #define WGL_TYPE_COLORINDEX_ARB                 0x202C
#endif // WGL_NUMBER_PIXEL_FORMATS_ARB

  // WGL_ARB_create_context_profile
#ifndef WGL_CONTEXT_MAJOR_VERSION_ARB
  #define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
  #define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
  #define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
  #define WGL_CONTEXT_FLAGS_ARB                   0x2094
  #define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

  // WGL_CONTEXT_FLAGS bits
  #define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
  #define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

  // WGL_CONTEXT_PROFILE_MASK_ARB bits
  #define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001
  #define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#endif // WGL_CONTEXT_MAJOR_VERSION_ARB

  static LRESULT CALLBACK wndProcDummy (HWND theWin, UINT theMsg, WPARAM theParamW, LPARAM theParamL)
  {
    return DefWindowProcW (theWin, theMsg, theParamW, theParamL);
  }
#elif defined(HAVE_XLIB)

  // GLX_ARB_create_context
#ifndef GLX_CONTEXT_MAJOR_VERSION_ARB
  #define GLX_CONTEXT_DEBUG_BIT_ARB         0x00000001
  #define GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
  #define GLX_CONTEXT_MAJOR_VERSION_ARB     0x2091
  #define GLX_CONTEXT_MINOR_VERSION_ARB     0x2092
  #define GLX_CONTEXT_FLAGS_ARB             0x2094

  // GLX_ARB_create_context_profile
  #define GLX_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
  #define GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
  #define GLX_CONTEXT_PROFILE_MASK_ARB      0x9126
#endif

  //! Dummy XError handler which just skips errors
  static int xErrorDummyHandler (Display*     /*theDisplay*/,
                                 XErrorEvent* /*theErrorEvent*/)
  {
    return 0;
  }

  //! Auxiliary method to format list.
  static void addMsgToList (TCollection_ExtendedString&       theList,
                            const TCollection_ExtendedString& theMsg)
  {
    if (!theList.IsEmpty())
    {
      theList += ", ";
    }
    theList += theMsg;
  }
#endif

}

// =======================================================================
// function : OpenGl_Window
// purpose  :
// =======================================================================
OpenGl_Window::OpenGl_Window()
: myOwnGContext (false),
  mySwapInterval (0)
{
  //
}

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Window::Init (const Handle(OpenGl_GraphicDriver)& theDriver,
                          const Handle(Aspect_Window)&  thePlatformWindow,
                          const Handle(Aspect_Window)&  theSizeWindow,
                          Aspect_RenderingContext       theGContext,
                          const Handle(OpenGl_Caps)&    theCaps,
                          const Handle(OpenGl_Context)& theShareCtx)
{
  myGlContext = new OpenGl_Context (theCaps);
  myOwnGContext = (theGContext == 0);
  myPlatformWindow = thePlatformWindow;
  mySizeWindow = theSizeWindow;
  mySwapInterval = theCaps->swapInterval;

  mySizeWindow->Size (mySize.x(), mySize.y());

  Standard_Boolean isCoreProfile = Standard_False;

#if defined(HAVE_EGL)
  EGLDisplay anEglDisplay = (EGLDisplay )theDriver->getRawGlDisplay();
  EGLContext anEglContext = (EGLContext )theDriver->getRawGlContext();
  EGLConfig  anEglConfig  = (EGLConfig  )theDriver->getRawGlConfig();
  if (anEglDisplay == EGL_NO_DISPLAY
   || anEglContext == EGL_NO_CONTEXT
   || (anEglConfig == NULL
    && (EGLContext )theGContext == EGL_NO_CONTEXT))
  {
    throw Aspect_GraphicDeviceDefinitionError("OpenGl_Window, EGL does not provide compatible configurations!");
  }

  EGLSurface anEglSurf = EGL_NO_SURFACE;
  if ((EGLContext )theGContext == EGL_NO_CONTEXT)
  {
    // EGL_KHR_gl_colorspace extension specifies if OpenGL should write into window buffer as into sRGB or RGB framebuffer
    //const int aSurfAttribs[] =
    //{
    //  EGL_GL_COLORSPACE_KHR, !theCaps->sRGBDisable ? EGL_GL_COLORSPACE_SRGB_KHR : EGL_GL_COLORSPACE_LINEAR_KHR,
    //  EGL_NONE,
    //};

    // create new surface
    anEglSurf = eglCreateWindowSurface (anEglDisplay,
                                        anEglConfig,
                                        (EGLNativeWindowType )myPlatformWindow->NativeHandle(),
                                        NULL);
    if (anEglSurf == EGL_NO_SURFACE
     && myPlatformWindow->NativeHandle() != 0)
    {
      throw Aspect_GraphicDeviceDefinitionError("OpenGl_Window, EGL is unable to create surface for window!");
    }
    else if (anEglSurf == EGL_NO_SURFACE)
    {
      // window-less EGL context (off-screen)
      //throw Aspect_GraphicDeviceDefinitionError("OpenGl_Window, EGL is unable to retrieve current surface!");
      if (anEglConfig != NULL)
      {
      #if !defined(__EMSCRIPTEN__) // eglCreatePbufferSurface() is not implemented by Emscripten EGL
        const int aSurfAttribs[] =
        {
          EGL_WIDTH,  mySize.x(),
          EGL_HEIGHT, mySize.y(),
          // EGL_KHR_gl_colorspace extension specifies if OpenGL should write into window buffer as into sRGB or RGB framebuffer
          //EGL_GL_COLORSPACE_KHR, !theCaps->sRGBDisable ? EGL_GL_COLORSPACE_SRGB_KHR : EGL_GL_COLORSPACE_LINEAR_KHR,
          EGL_NONE
        };
        anEglSurf = eglCreatePbufferSurface (anEglDisplay, anEglConfig, aSurfAttribs);
        if (anEglSurf == EGL_NO_SURFACE)
        {
          throw Aspect_GraphicDeviceDefinitionError("OpenGl_Window, EGL is unable to create off-screen surface!");
        }
      #endif
      }
      myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_LOW,
                                "OpenGl_Window::CreateWindow: WARNING, a Window is created without a EGL Surface!");
    }
  }
  else if (theGContext != anEglContext)
  {
    throw Aspect_GraphicDeviceDefinitionError("OpenGl_Window, EGL is used in unsupported combination!");
  }
  else
  {
    anEglSurf = eglGetCurrentSurface(EGL_DRAW);
    if (anEglSurf == EGL_NO_SURFACE)
    {
      // window-less EGL context (off-screen)
      //throw Aspect_GraphicDeviceDefinitionError("OpenGl_Window, EGL is unable to retrieve current surface!");
      if (anEglConfig != NULL)
      {
      #if !defined(__EMSCRIPTEN__) // eglCreatePbufferSurface() is not implemented by Emscripten EGL
        const int aSurfAttribs[] =
        {
          EGL_WIDTH,  mySize.x(),
          EGL_HEIGHT, mySize.y(),
          // EGL_KHR_gl_colorspace extension specifies if OpenGL should write into window buffer as into sRGB or RGB framebuffer
          //EGL_GL_COLORSPACE_KHR, !theCaps->sRGBDisable ? EGL_GL_COLORSPACE_SRGB_KHR : EGL_GL_COLORSPACE_LINEAR_KHR,
          EGL_NONE
        };
        anEglSurf = eglCreatePbufferSurface (anEglDisplay, anEglConfig, aSurfAttribs);
        if (anEglSurf == EGL_NO_SURFACE)
        {
          throw Aspect_GraphicDeviceDefinitionError("OpenGl_Window, EGL is unable to create off-screen surface!");
        }
      #endif
      }
      myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_LOW,
                                "OpenGl_Window::CreateWindow: WARNING, a Window is created without a EGL Surface!");
    }
  }

  myGlContext->Init ((Aspect_Drawable )anEglSurf, (Aspect_Display )anEglDisplay, (Aspect_RenderingContext )anEglContext, isCoreProfile);
#elif defined(_WIN32)
  (void )theDriver;
  HWND  aWindow   = (HWND )myPlatformWindow->NativeHandle();
  HDC   aWindowDC = GetDC (aWindow);
  HGLRC aGContext = (HGLRC )theGContext;

  PIXELFORMATDESCRIPTOR aPixelFrmt;
  memset (&aPixelFrmt, 0, sizeof(aPixelFrmt));
  aPixelFrmt.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
  aPixelFrmt.nVersion     = 1;
  aPixelFrmt.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  aPixelFrmt.iPixelType   = PFD_TYPE_RGBA;
  aPixelFrmt.cColorBits   = 24;
  aPixelFrmt.cDepthBits   = 24;
  aPixelFrmt.cStencilBits = 8;
  aPixelFrmt.iLayerType   = PFD_MAIN_PLANE;
  if (theCaps->contextStereo)
  {
    aPixelFrmt.dwFlags |= PFD_STEREO;
  }

  int aPixelFrmtId = ChoosePixelFormat (aWindowDC, &aPixelFrmt);

  // in case of failure try without stereo if any
  const Standard_Boolean hasStereo = aPixelFrmtId != 0 && theCaps->contextStereo;
  if (aPixelFrmtId == 0 && theCaps->contextStereo)
  {
    TCollection_ExtendedString aMsg ("OpenGl_Window::CreateWindow: "
                                     "ChoosePixelFormat is unable to find stereo supported pixel format. "
                                     "Choosing similar non stereo format.");
    myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                              GL_DEBUG_TYPE_OTHER,
                              0, GL_DEBUG_SEVERITY_HIGH, aMsg);

    aPixelFrmt.dwFlags &= ~PFD_STEREO;
    aPixelFrmtId = ChoosePixelFormat (aWindowDC, &aPixelFrmt);
  }

  if (aPixelFrmtId == 0)
  {
    ReleaseDC (aWindow, aWindowDC);

    TCollection_AsciiString aMsg ("OpenGl_Window::CreateWindow: ChoosePixelFormat failed. Error code: ");
    aMsg += (int )GetLastError();
    throw Aspect_GraphicDeviceDefinitionError(aMsg.ToCString());
  }

  DescribePixelFormat (aWindowDC, aPixelFrmtId, sizeof(aPixelFrmt), &aPixelFrmt);

  HGLRC aSlaveCtx = !theShareCtx.IsNull() ? (HGLRC )theShareCtx->myGContext : NULL;
  if (aGContext == NULL)
  {
    // create temporary context to retrieve advanced context creation procedures
    HMODULE aModule = GetModuleHandleW(NULL);
    WNDCLASSW aClass; memset (&aClass, 0, sizeof(aClass));
    aClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    aClass.lpfnWndProc   = wndProcDummy;
    aClass.hInstance     = aModule;
    aClass.lpszClassName = L"OpenGl_WindowTmp";
    HWND  aWinTmp     = NULL;
    HDC   aDevCtxTmp  = NULL;
    HGLRC aRendCtxTmp = NULL;
    if ((!theCaps->contextDebug && !theCaps->contextNoAccel && theCaps->contextCompatible && !theCaps->buffersDeepColor)
     || RegisterClassW (&aClass) == 0)
    {
      aClass.lpszClassName = NULL;
    }
    if (aClass.lpszClassName != NULL)
    {
      DWORD anExStyle = WS_EX_TOOLWINDOW | WS_EX_WINDOWEDGE;
    #if (_WIN32_WINNT >= 0x0500)
      anExStyle |= WS_EX_NOACTIVATE;
    #endif
      aWinTmp = CreateWindowExW(anExStyle,
                                aClass.lpszClassName, L"OpenGl_WindowTmp",
                                WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_DISABLED,
                                2, 2, 4, 4,
                                NULL, NULL, aModule, NULL);
    }
    if (aWinTmp != NULL)
    {
      aDevCtxTmp = GetDC (aWinTmp);
      SetPixelFormat (aDevCtxTmp, aPixelFrmtId, &aPixelFrmt);
      aRendCtxTmp = wglCreateContext (aDevCtxTmp);
    }

    typedef BOOL (WINAPI *wglChoosePixelFormatARB_t)(HDC           theDevCtx,
                                                     const int*    theIntAttribs,
                                                     const float*  theFloatAttribs,
                                                     unsigned int  theMaxFormats,
                                                     int*          theFormatsOut,
                                                     unsigned int* theNumFormatsOut);
    typedef HGLRC (WINAPI *wglCreateContextAttribsARB_t)(HDC        theDevCtx,
                                                         HGLRC      theShareContext,
                                                         const int* theAttribs);
    wglChoosePixelFormatARB_t    aChoosePixProc = NULL;
    wglCreateContextAttribsARB_t aCreateCtxProc = NULL;
    if (aRendCtxTmp != NULL)
    {
      wglMakeCurrent (aDevCtxTmp, aRendCtxTmp);

      typedef const char* (WINAPI *wglGetExtensionsStringARB_t)(HDC theDeviceContext);
      wglGetExtensionsStringARB_t aGetExtensions = (wglGetExtensionsStringARB_t  )wglGetProcAddress ("wglGetExtensionsStringARB");
      const char* aWglExts = (aGetExtensions != NULL) ? aGetExtensions (wglGetCurrentDC()) : NULL;
      if (OpenGl_Context::CheckExtension (aWglExts, "WGL_ARB_pixel_format"))
      {
        aChoosePixProc = (wglChoosePixelFormatARB_t    )wglGetProcAddress ("wglChoosePixelFormatARB");
      }
      if (OpenGl_Context::CheckExtension (aWglExts, "WGL_ARB_create_context_profile"))
      {
        aCreateCtxProc = (wglCreateContextAttribsARB_t )wglGetProcAddress ("wglCreateContextAttribsARB");
      }
    }

    // choose extended pixel format
    if (aChoosePixProc != NULL)
    {
      const int aPixAttribs[] =
      {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
        WGL_STEREO_ARB,         hasStereo ? GL_TRUE : GL_FALSE,
        WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
        //WGL_SAMPLE_BUFFERS_ARB, 1,
        //WGL_SAMPLES_ARB,        8,
        //WGL_COLOR_BITS_ARB,     24,
        WGL_RED_BITS_ARB,   theCaps->buffersDeepColor ? 10 : 8,
        WGL_GREEN_BITS_ARB, theCaps->buffersDeepColor ? 10 : 8,
        WGL_BLUE_BITS_ARB,  theCaps->buffersDeepColor ? 10 : 8,
        WGL_ALPHA_BITS_ARB, theCaps->buffersDeepColor ? 2  : 8,
        WGL_DEPTH_BITS_ARB,     24,
        WGL_STENCIL_BITS_ARB,   8,
        // WGL_EXT_colorspace extension specifies if OpenGL should write into window buffer as into sRGB or RGB framebuffer
        //WGL_COLORSPACE_EXT, !theCaps->sRGBDisable ? WGL_COLORSPACE_SRGB_EXT : WGL_COLORSPACE_LINEAR_EXT,
        // requires WGL_ARB_framebuffer_sRGB or WGL_EXT_framebuffer_sRGB extensions
        //WGL_FRAMEBUFFER_SRGB_CAPABLE_EXT, !theCaps->sRGBDisable ? GL_TRUE : GL_FALSE,
        WGL_ACCELERATION_ARB,   theCaps->contextNoAccel ? WGL_NO_ACCELERATION_ARB : WGL_FULL_ACCELERATION_ARB,
        0, 0,
      };
      unsigned int aFrmtsNb = 0;
      if (!aChoosePixProc (aWindowDC, aPixAttribs, NULL, 1, &aPixelFrmtId, &aFrmtsNb)
        && theCaps->buffersDeepColor)
      {
        /// TODO
        Message::SendFail() << "Error: unable to find RGB10_A2 window buffer format!";
      }
    }

    // setup pixel format - may be set only once per window
    if (!SetPixelFormat (aWindowDC, aPixelFrmtId, &aPixelFrmt))
    {
      ReleaseDC (aWindow, aWindowDC);

      TCollection_AsciiString aMsg("OpenGl_Window::CreateWindow: SetPixelFormat failed. Error code: ");
      aMsg += (int )GetLastError();
      throw Aspect_GraphicDeviceDefinitionError(aMsg.ToCString());
    }

    // create GL context with extra options
    if (aCreateCtxProc != NULL)
    {
      if (!theCaps->contextCompatible)
      {
        int aCoreCtxAttribs[] =
        {
          WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
          WGL_CONTEXT_MINOR_VERSION_ARB, 2,
          WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
          WGL_CONTEXT_FLAGS_ARB,         theCaps->contextDebug ? WGL_CONTEXT_DEBUG_BIT_ARB : 0,
          0, 0
        };

        // Try to create the core profile of highest OpenGL version supported by OCCT
        // (this will be done automatically by some drivers when requesting 3.2,
        //  but some will not (e.g. AMD Catalyst) since WGL_ARB_create_context_profile specification allows both implementations).
        for (int aLowVer4 = 6; aLowVer4 >= 0 && aGContext == NULL; --aLowVer4)
        {
          aCoreCtxAttribs[1] = 4;
          aCoreCtxAttribs[3] = aLowVer4;
          aGContext = aCreateCtxProc (aWindowDC, aSlaveCtx, aCoreCtxAttribs);
        }
        for (int aLowVer3 = 3; aLowVer3 >= 2 && aGContext == NULL; --aLowVer3)
        {
          aCoreCtxAttribs[1] = 3;
          aCoreCtxAttribs[3] = aLowVer3;
          aGContext = aCreateCtxProc (aWindowDC, aSlaveCtx, aCoreCtxAttribs);
        }
        isCoreProfile = aGContext != NULL;
      }

      if (aGContext == NULL)
      {
        int aCtxAttribs[] =
        {
          // Beware! NVIDIA drivers reject context creation when WGL_CONTEXT_PROFILE_MASK_ARB are specified
          // but not WGL_CONTEXT_MAJOR_VERSION_ARB/WGL_CONTEXT_MINOR_VERSION_ARB.
          //WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
          WGL_CONTEXT_FLAGS_ARB,         theCaps->contextDebug ? WGL_CONTEXT_DEBUG_BIT_ARB : 0,
          0, 0
        };
        isCoreProfile = Standard_False;
        aGContext = aCreateCtxProc (aWindowDC, aSlaveCtx, aCtxAttribs);

        if (aGContext != NULL
        && !theCaps->contextCompatible)
        {
          TCollection_ExtendedString aMsg("OpenGl_Window::CreateWindow: core profile creation failed.");
          myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_LOW, aMsg);
        }
      }

      if (aGContext != NULL)
      {
        aSlaveCtx = NULL;
      }
    }

    if (aRendCtxTmp != NULL)
    {
      wglDeleteContext (aRendCtxTmp);
    }
    if (aDevCtxTmp != NULL)
    {
      ReleaseDC (aWinTmp, aDevCtxTmp);
    }
    if (aWinTmp != NULL)
    {
      DestroyWindow (aWinTmp);
    }
    if (aClass.lpszClassName != NULL)
    {
      UnregisterClassW (aClass.lpszClassName, aModule);
    }

    if (aGContext == NULL)
    {
      // create context using obsolete functionality
      aGContext = wglCreateContext (aWindowDC);
    }
    if (aGContext == NULL)
    {
      ReleaseDC (aWindow, aWindowDC);

      TCollection_AsciiString aMsg ("OpenGl_Window::CreateWindow: wglCreateContext failed. Error code: ");
      aMsg += (int )GetLastError();
      throw Aspect_GraphicDeviceDefinitionError(aMsg.ToCString());
    }
  }

  // all GL context within one OpenGl_GraphicDriver should be shared!
  if (aSlaveCtx != NULL && wglShareLists (aSlaveCtx, aGContext) != TRUE)
  {
    TCollection_AsciiString aMsg ("OpenGl_Window::CreateWindow: wglShareLists failed. Error code: ");
    aMsg += (int )GetLastError();
    throw Aspect_GraphicDeviceDefinitionError(aMsg.ToCString());
  }

  myGlContext->Init ((Aspect_Handle )aWindow, (Aspect_Handle )aWindowDC, (Aspect_RenderingContext )aGContext, isCoreProfile);
#elif defined(HAVE_XLIB)
  Window     aWindow   = (Window )myPlatformWindow->NativeHandle();
  Display*   aDisp     = (Display* )theDriver->GetDisplayConnection()->GetDisplayAspect();
  GLXContext aGContext = (GLXContext )theGContext;
  GLXContext aSlaveCtx = !theShareCtx.IsNull() ? (GLXContext )theShareCtx->myGContext : NULL;

  XWindowAttributes aWinAttribs;
  XGetWindowAttributes (aDisp, aWindow, &aWinAttribs);
  XVisualInfo aVisInfo;
  aVisInfo.visualid = aWinAttribs.visual->visualid;
  aVisInfo.screen   = DefaultScreen (aDisp);
  int aNbItems;
  std::unique_ptr<XVisualInfo, int(*)(void*)> aVis (XGetVisualInfo (aDisp, VisualIDMask | VisualScreenMask, &aVisInfo, &aNbItems), &XFree);
  int isGl = 0;
  if (aVis.get() == NULL)
  {
    throw Aspect_GraphicDeviceDefinitionError("OpenGl_Window::CreateWindow: XGetVisualInfo is unable to choose needed configuration in existing OpenGL context. ");
  }
  else if (glXGetConfig (aDisp, aVis.get(), GLX_USE_GL, &isGl) != 0 || !isGl)
  {
    throw Aspect_GraphicDeviceDefinitionError("OpenGl_Window::CreateWindow: window Visual does not support GL rendering!");
  }

  // create new context
  GLXFBConfig anFBConfig = myPlatformWindow->NativeFBConfig();
  const char* aGlxExts   = glXQueryExtensionsString (aDisp, aVisInfo.screen);
  if (myOwnGContext
   && anFBConfig != NULL
   && OpenGl_Context::CheckExtension (aGlxExts, "GLX_ARB_create_context_profile"))
  {
    // Replace default XError handler to ignore errors.
    // Warning - this is global for all threads!
    typedef int (*xerrorhandler_t)(Display* , XErrorEvent* );
    xerrorhandler_t anOldHandler = XSetErrorHandler(xErrorDummyHandler);

    typedef GLXContext (*glXCreateContextAttribsARB_t)(Display* dpy, GLXFBConfig config,
                                                       GLXContext share_context, Bool direct,
                                                       const int* attrib_list);
    glXCreateContextAttribsARB_t aCreateCtxProc = (glXCreateContextAttribsARB_t )glXGetProcAddress((const GLubyte* )"glXCreateContextAttribsARB");
    if (!theCaps->contextCompatible)
    {
      int aCoreCtxAttribs[] =
      {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 2,
        GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        GLX_CONTEXT_FLAGS_ARB,         theCaps->contextDebug ? GLX_CONTEXT_DEBUG_BIT_ARB : 0,
        0, 0
      };

      // try to create the core profile of highest OpenGL version supported by OCCT
      for (int aLowVer4 = 6; aLowVer4 >= 0 && aGContext == NULL; --aLowVer4)
      {
        aCoreCtxAttribs[1] = 4;
        aCoreCtxAttribs[3] = aLowVer4;
        aGContext = aCreateCtxProc (aDisp, anFBConfig, aSlaveCtx, True, aCoreCtxAttribs);
      }
      for (int aLowVer3 = 3; aLowVer3 >= 2 && aGContext == NULL; --aLowVer3)
      {
        aCoreCtxAttribs[1] = 3;
        aCoreCtxAttribs[3] = aLowVer3;
        aGContext = aCreateCtxProc (aDisp, anFBConfig, aSlaveCtx, True, aCoreCtxAttribs);
      }
      isCoreProfile = aGContext != NULL;
    }

    if (aGContext == NULL)
    {
      int aCtxAttribs[] =
      {
        GLX_CONTEXT_FLAGS_ARB, theCaps->contextDebug ? GLX_CONTEXT_DEBUG_BIT_ARB : 0,
        0, 0
      };
      isCoreProfile = Standard_False;
      aGContext = aCreateCtxProc (aDisp, anFBConfig, aSlaveCtx, True, aCtxAttribs);

      if (aGContext != NULL
      && !theCaps->contextCompatible)
      {
        TCollection_ExtendedString aMsg("OpenGl_Window::CreateWindow: core profile creation failed.");
        myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_LOW, aMsg);
      }
    }
    XSetErrorHandler(anOldHandler);
  }

  if (myOwnGContext
   && aGContext == NULL)
  {
    aGContext = glXCreateContext (aDisp, aVis.get(), aSlaveCtx, GL_TRUE);
    if (aGContext == NULL)
    {
      throw Aspect_GraphicDeviceDefinitionError("OpenGl_Window::CreateWindow: glXCreateContext failed.");
    }
  }

  // check Visual for OpenGl context's parameters compatibility
  TCollection_ExtendedString aList;
  int isDoubleBuffer = 0, isRGBA = 0, isStereo = 0;
  int aDepthSize = 0, aStencilSize = 0;
  glXGetConfig (aDisp, aVis.get(), GLX_RGBA,         &isRGBA);
  glXGetConfig (aDisp, aVis.get(), GLX_DOUBLEBUFFER, &isDoubleBuffer);
  glXGetConfig (aDisp, aVis.get(), GLX_STEREO,       &isStereo);
  glXGetConfig (aDisp, aVis.get(), GLX_DEPTH_SIZE,   &aDepthSize);
  glXGetConfig (aDisp, aVis.get(), GLX_STENCIL_SIZE, &aStencilSize);
  if (aDepthSize < 1)      addMsgToList (aList, "no depth buffer");
  if (aStencilSize < 1)    addMsgToList (aList, "no stencil buffer");
  if (isRGBA == 0)         addMsgToList (aList, "no RGBA color buffer");
  if (isDoubleBuffer == 0) addMsgToList (aList, "no Double Buffer");
  if (theCaps->contextStereo && isStereo == 0)
  {
    addMsgToList (aList, "no Quad Buffer");
  }
  else if (!theCaps->contextStereo && isStereo == 1)
  {
    addMsgToList (aList, "extra Quad Buffer");
  }
  if (!aList.IsEmpty())
  {
    TCollection_ExtendedString aMsg = TCollection_ExtendedString ("OpenGl_Window::CreateWindow: window Visual is incomplete: ") + aList;
    myGlContext->PushMessage (GL_DEBUG_SOURCE_APPLICATION,
                              GL_DEBUG_TYPE_OTHER,
                              0, GL_DEBUG_SEVERITY_MEDIUM, aMsg);
  }

  myGlContext->Init ((Aspect_Drawable )aWindow, (Aspect_Display )aDisp, (Aspect_RenderingContext )aGContext, isCoreProfile);
#else
  // not implemented
  (void )isCoreProfile;
#endif
  myGlContext->Share (theShareCtx);
  myGlContext->SetSwapInterval (mySwapInterval);
  init();
}

// =======================================================================
// function : ~OpenGl_Window
// purpose  :
// =======================================================================
OpenGl_Window::~OpenGl_Window()
{
  if (!myOwnGContext
   ||  myGlContext.IsNull())
  {
    myGlContext.Nullify();
    return;
  }

  // release "GL" context if it is owned by window
  // Mesa implementation can fail to destroy GL context if it set for current thread.
  // It should be safer to unset thread GL context before its destruction.
#if defined(HAVE_EGL)
  if ((EGLSurface )myGlContext->myWindow != EGL_NO_SURFACE)
  {
    eglDestroySurface ((EGLDisplay )myGlContext->myDisplay,
                       (EGLSurface )myGlContext->myWindow);
  }
#elif defined(_WIN32)
  HWND  aWindow          = (HWND  )myGlContext->myWindow;
  HDC   aWindowDC        = (HDC   )myGlContext->myDisplay;
  HGLRC aWindowGContext  = (HGLRC )myGlContext->myGContext;
  HGLRC aThreadGContext  = wglGetCurrentContext();
  myGlContext.Nullify();

  if (aThreadGContext != NULL)
  {
    if (aThreadGContext == aWindowGContext)
    {
      wglMakeCurrent (NULL, NULL);
    }

    wglDeleteContext (aWindowGContext);
  }
  ReleaseDC (aWindow, aWindowDC);
#elif defined(HAVE_XLIB)
  Display*    aDisplay        = (Display*    )myGlContext->myDisplay;
  GLXContext  aWindowGContext = (GLXContext  )myGlContext->myGContext;
  GLXContext  aThreadGContext = glXGetCurrentContext();
  myGlContext.Nullify();

  if (aDisplay != NULL)
  {
    if (aThreadGContext == aWindowGContext)
    {
      glXMakeCurrent (aDisplay, None, NULL);
    }

    // FSXXX sync necessary if non-direct rendering
    glXWaitGL();
    glXDestroyContext (aDisplay, aWindowGContext);
  }
#else
  // not implemented
#endif
}

#endif // !__APPLE__

// =======================================================================
// function : Activate
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Window::Activate()
{
  return myGlContext->MakeCurrent();
}

#if !defined(__APPLE__) || defined(HAVE_XLIB)

// =======================================================================
// function : Resize
// purpose  : call_subr_resize
// =======================================================================
void OpenGl_Window::Resize()
{
  Graphic3d_Vec2i aWinSize;
  mySizeWindow->Size (aWinSize.x(), aWinSize.y());
  if (mySize == aWinSize)
  {
    // if the size is not changed - do nothing
    return;
  }

  mySize = aWinSize;

  init();
}

// =======================================================================
// function : init
// purpose  :
// =======================================================================
void OpenGl_Window::init()
{
  if (!Activate())
  {
    return;
  }

#if defined(HAVE_EGL)
  if ((EGLSurface )myGlContext->myWindow == EGL_NO_SURFACE)
  {
    // define an offscreen default FBO to avoid rendering into EGL_NO_SURFACE;
    // note that this code is currently never called, since eglCreatePbufferSurface() is used instead as more robust solution
    // for offscreen rendering on bugged OpenGL ES drivers
    Handle(OpenGl_FrameBuffer) aDefFbo = myGlContext->SetDefaultFrameBuffer (Handle(OpenGl_FrameBuffer)());
    if (!aDefFbo.IsNull())
    {
      aDefFbo->Release (myGlContext.operator->());
    }
    else
    {
      aDefFbo = new OpenGl_FrameBuffer();
    }

    OpenGl_ColorFormats aColorFormats;
    aColorFormats.Append (GL_RGBA8);
    if (!aDefFbo->InitRenderBuffer (myGlContext, mySize, aColorFormats, GL_DEPTH24_STENCIL8))
    {
      TCollection_AsciiString aMsg ("OpenGl_Window::CreateWindow: default FBO creation failed");
      throw Aspect_GraphicDeviceDefinitionError(aMsg.ToCString());
    }
    myGlContext->SetDefaultFrameBuffer (aDefFbo);
    aDefFbo->BindBuffer (myGlContext);
  }
  else if (!myPlatformWindow->IsVirtual()
         && mySizeWindow == myPlatformWindow)
  {
    eglQuerySurface ((EGLDisplay )myGlContext->myDisplay, (EGLSurface )myGlContext->myWindow, EGL_WIDTH,  &mySize.x());
    eglQuerySurface ((EGLDisplay )myGlContext->myDisplay, (EGLSurface )myGlContext->myWindow, EGL_HEIGHT, &mySize.y());
  }
#else
  //
#endif

  myGlContext->core11fwd->glDisable (GL_DITHER);
  myGlContext->core11fwd->glDisable (GL_SCISSOR_TEST);
  const Standard_Integer aViewport[4] = { 0, 0, mySize.x(), mySize.y() };
  myGlContext->ResizeViewport (aViewport);
  myGlContext->SetDrawBuffer (GL_BACK);
  if (myGlContext->core11ffp != NULL)
  {
    myGlContext->core11ffp->glMatrixMode (GL_MODELVIEW);
  }
}

// =======================================================================
// function : SetSwapInterval
// purpose  :
// =======================================================================
void OpenGl_Window::SetSwapInterval (Standard_Boolean theToForceNoSync)
{
  const Standard_Integer aSwapInterval = theToForceNoSync ? 0 : myGlContext->caps->swapInterval;
  if (mySwapInterval != aSwapInterval)
  {
    mySwapInterval = aSwapInterval;
    myGlContext->SetSwapInterval (mySwapInterval);
  }
}

#endif // !__APPLE__
