// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#include <OpenGl_GlCore11.hxx>

#include <InterfaceGraphic.hxx>

#include <OpenGl_Window.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_Display.hxx>

#include <Aspect_GraphicDeviceDefinitionError.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_HANDLE(OpenGl_Window,MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Window,MMgt_TShared)

#if !defined(__APPLE__) || defined(MACOSX_USE_GLX)

namespace
{
  static const TEL_COLOUR THE_DEFAULT_BG_COLOR = { { 0.F, 0.F, 0.F, 1.F } };

#if defined(_WIN32)

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
#else
  static Bool WaitForNotify (Display* theDisp, XEvent* theEv, char* theArg)
  {
    return (theEv->type == MapNotify) && (theEv->xmap.window == (Window )theArg);
  }
#endif

}

// =======================================================================
// function : OpenGl_Window
// purpose  :
// =======================================================================
OpenGl_Window::OpenGl_Window (const Handle(OpenGl_Display)& theDisplay,
                              const CALL_DEF_WINDOW&        theCWindow,
                              Aspect_RenderingContext       theGContext,
                              const Handle(OpenGl_Caps)&    theCaps,
                              const Handle(OpenGl_Context)& theShareCtx)
: myDisplay (theDisplay),
  myGlContext (new OpenGl_Context (theCaps)),
  myOwnGContext (theGContext == 0),
#if defined(_WIN32)
  mySysPalInUse (FALSE),
#endif
  myWidth ((Standard_Integer )theCWindow.dx),
  myHeight ((Standard_Integer )theCWindow.dy),
  myBgColor (THE_DEFAULT_BG_COLOR),
  myDither (theDisplay->Dither()),
  myBackDither (theDisplay->BackDither())
{
  myBgColor.rgb[0] = theCWindow.Background.r;
  myBgColor.rgb[1] = theCWindow.Background.g;
  myBgColor.rgb[2] = theCWindow.Background.b;

#if defined(_WIN32)
  HWND  aWindow   = (HWND )theCWindow.XWindow;
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
  if (aPixelFrmtId == 0)
  {
    ReleaseDC (aWindow, aWindowDC);

    TCollection_AsciiString aMsg ("OpenGl_Window::CreateWindow: ChoosePixelFormat failed. Error code: ");
    aMsg += (int )GetLastError();
    Aspect_GraphicDeviceDefinitionError::Raise (aMsg.ToCString());
    return;
  }

  DescribePixelFormat (aWindowDC, aPixelFrmtId, sizeof(aPixelFrmt), &aPixelFrmt);
  if (aPixelFrmt.dwFlags & PFD_NEED_PALETTE)
  {
    WINDOW_DATA* aWndData = (WINDOW_DATA* )GetWindowLongPtr (aWindow, GWLP_USERDATA);

    mySysPalInUse = (aPixelFrmt.dwFlags & PFD_NEED_SYSTEM_PALETTE) ? TRUE : FALSE;
    InterfaceGraphic_RealizePalette (aWindowDC, aWndData->hPal, FALSE, mySysPalInUse);
  }

  if (myDither)
  {
    myDither = (aPixelFrmt.cColorBits <= 8);
  }

  if (myBackDither)
  {
    myBackDither = (aPixelFrmt.cColorBits <= 8);
  }

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
    if ((!theCaps->contextDebug && !theCaps->contextNoAccel)
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
        WGL_STEREO_ARB,         theCaps->contextStereo ? GL_TRUE : GL_FALSE,
        WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
        //WGL_SAMPLE_BUFFERS_ARB, 1,
        //WGL_SAMPLES_ARB,        8,
        WGL_COLOR_BITS_ARB,     24,
        WGL_DEPTH_BITS_ARB,     24,
        WGL_STENCIL_BITS_ARB,   8,
        WGL_ACCELERATION_ARB,   theCaps->contextNoAccel ? WGL_NO_ACCELERATION_ARB : WGL_FULL_ACCELERATION_ARB,
        0, 0,
      };
      unsigned int aFrmtsNb = 0;
      aChoosePixProc (aWindowDC, aPixAttribs, NULL, 1, &aPixelFrmtId, &aFrmtsNb);
    }

    // setup pixel format - may be set only once per window
    if (!SetPixelFormat (aWindowDC, aPixelFrmtId, &aPixelFrmt))
    {
      ReleaseDC (aWindow, aWindowDC);

      TCollection_AsciiString aMsg("OpenGl_Window::CreateWindow: SetPixelFormat failed. Error code: ");
      aMsg += (int )GetLastError();
      Aspect_GraphicDeviceDefinitionError::Raise (aMsg.ToCString());
      return;
    }

    // create GL context with extra options
    if (aCreateCtxProc != NULL)
    {
      // Beware! NVIDIA drivers reject context creation when WGL_CONTEXT_PROFILE_MASK_ARB are specified
      // but not WGL_CONTEXT_MAJOR_VERSION_ARB/WGL_CONTEXT_MINOR_VERSION_ARB.
      int aCtxAttribs[] =
      {
        //WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        //WGL_CONTEXT_MINOR_VERSION_ARB, 2,
        //WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB, //WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB,         theCaps->contextDebug ? WGL_CONTEXT_DEBUG_BIT_ARB : 0,
        0, 0
      };

      aGContext = aCreateCtxProc (aWindowDC, aSlaveCtx, aCtxAttribs);
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
      Aspect_GraphicDeviceDefinitionError::Raise (aMsg.ToCString());
      return;
    }
  }

  // all GL context within one OpenGl_GraphicDriver should be shared!
  if (aSlaveCtx != NULL && wglShareLists (aSlaveCtx, aGContext) != TRUE)
  {
    TCollection_AsciiString aMsg ("OpenGl_Window::CreateWindow: wglShareLists failed. Error code: ");
    aMsg += (int )GetLastError();
    Aspect_GraphicDeviceDefinitionError::Raise (aMsg.ToCString());
    return;
  }

  myGlContext->Init ((Aspect_Handle )aWindow, (Aspect_Handle )aWindowDC, (Aspect_RenderingContext )aGContext);
#else
  WINDOW aParent = (WINDOW )theCWindow.XWindow;
  WINDOW aWindow = 0;
  DISPLAY* aDisp = (DISPLAY* )myDisplay->GetDisplay();
  GLXContext aGContext = (GLXContext )theGContext;

  XWindowAttributes wattr;
  XGetWindowAttributes (aDisp, aParent, &wattr);
  const int scr = DefaultScreen (aDisp);

  XVisualInfo* aVis = NULL;
  {
    unsigned long aVisInfoMask = VisualIDMask | VisualScreenMask;
    XVisualInfo aVisInfo;
    aVisInfo.visualid = wattr.visual->visualid;
    aVisInfo.screen   = scr;
    int aNbItems;
    aVis = XGetVisualInfo (aDisp, aVisInfoMask, &aVisInfo, &aNbItems);
  }

  if (!myOwnGContext)
  {
    if (aVis != NULL)
    {
      Aspect_GraphicDeviceDefinitionError::Raise ("OpenGl_Window::CreateWindow: XGetVisualInfo failed.");
      return;
    }

    aWindow = aParent;
  }
  else
  {
  #if defined(__linux) || defined(Linux) || defined(__APPLE__)
    if (aVis != NULL)
    {
      // check Visual for OpenGl context's parameters compability
      int isGl = 0, isDoubleBuffer = 0, isRGBA = 0, aDepthSize = 0, aStencilSize = 0;

      if (glXGetConfig (aDisp, aVis, GLX_USE_GL, &isGl) != 0)
        isGl = 0;

      if (glXGetConfig (aDisp, aVis, GLX_RGBA, &isRGBA) != 0)
        isRGBA = 0;

      if (glXGetConfig (aDisp, aVis, GLX_DOUBLEBUFFER, &isDoubleBuffer) != 0)
        isDoubleBuffer = 0;

      if (glXGetConfig (aDisp, aVis, GLX_DEPTH_SIZE, &aDepthSize) != 0)
        aDepthSize = 0;

      if (glXGetConfig (aDisp, aVis, GLX_STENCIL_SIZE, &aStencilSize) != 0)
        aStencilSize = 0;

      if (!isGl || !aDepthSize || !aStencilSize || !isRGBA  || (isDoubleBuffer ? 1 : 0) != (myDisplay->DBuffer()? 1 : 0))
      {
        XFree (aVis);
        aVis = NULL;
      }
    }
  #endif

    if (aVis == NULL)
    {
      int anIter = 0;
      int anAttribs[13];
      anAttribs[anIter++] = GLX_RGBA;

      anAttribs[anIter++] = GLX_DEPTH_SIZE;
      anAttribs[anIter++] = 1;

      anAttribs[anIter++] = GLX_STENCIL_SIZE;
      anAttribs[anIter++] = 1;

      anAttribs[anIter++] = GLX_RED_SIZE;
      anAttribs[anIter++] = (wattr.depth <= 8) ? 0 : 1;

      anAttribs[anIter++] = GLX_GREEN_SIZE;
      anAttribs[anIter++] = (wattr.depth <= 8) ? 0 : 1;

      anAttribs[anIter++] = GLX_BLUE_SIZE;
      anAttribs[anIter++] = (wattr.depth <= 8) ? 0 : 1;

      if (myDisplay->DBuffer())
        anAttribs[anIter++] = GLX_DOUBLEBUFFER;

      anAttribs[anIter++] = None;

      aVis = glXChooseVisual (aDisp, scr, anAttribs);
      if (aVis == NULL)
      {
        Aspect_GraphicDeviceDefinitionError::Raise ("OpenGl_Window::CreateWindow: glXChooseVisual failed.");
        return;
      }
    }

    if (!theShareCtx.IsNull())
    {
      // ctx est une copie du previous
      aGContext = glXCreateContext (aDisp, aVis, (GLXContext )theShareCtx->myGContext, GL_TRUE);
    }
    else
    {
      aGContext = glXCreateContext (aDisp, aVis, NULL, GL_TRUE);
    }

    if (!aGContext)
    {
      Aspect_GraphicDeviceDefinitionError::Raise ("OpenGl_Window::CreateWindow: glXCreateContext failed.");
      return;
    }

    Colormap cmap = XCreateColormap (aDisp, aParent, aVis->visual, AllocNone);

    XColor color;
    color.red   = (unsigned short) (myBgColor.rgb[0] * 0xFFFF);
    color.green = (unsigned short) (myBgColor.rgb[1] * 0xFFFF);
    color.blue  = (unsigned short) (myBgColor.rgb[2] * 0xFFFF);
    color.flags = DoRed | DoGreen | DoBlue;
    XAllocColor (aDisp, cmap, &color);

    XSetWindowAttributes cwa;
    cwa.colormap         = cmap;
    cwa.event_mask       = StructureNotifyMask;
    cwa.border_pixel     = color.pixel;
    cwa.background_pixel = color.pixel;

    if (aVis->visualid == wattr.visual->visualid)
    {
      aWindow = aParent;
    }
    else
    {
      unsigned long mask = CWBackPixel | CWColormap | CWBorderPixel | CWEventMask;
      aWindow = XCreateWindow (aDisp, aParent, 0, 0, myWidth, myHeight, 0/*bw*/, aVis->depth, InputOutput, aVis->visual, mask, &cwa);
    }

    XSetWindowBackground (aDisp, aWindow, cwa.background_pixel);
    XClearWindow (aDisp, aWindow);

    if (aWindow != aParent)
    {
      XEvent anEvent;
      XMapWindow (aDisp, aWindow);
      XIfEvent (aDisp, &anEvent, WaitForNotify, (char* )aWindow);
    }
  }

  /*
  * Le BackDitherProp est utilise pour le clear du background
  * Pour eviter une difference de couleurs avec la couleur choisie
  * par l'application (XWindow) il faut desactiver le dithering
  * au dessus de 8 plans.
  *
  * Pour le DitherProp:
  * On cherchera a activer le Dithering que si le Visual a au moins
  * 8 plans pour le GLX_RED_SIZE. Le test est plus sur car on peut
  * avoir une profondeur superieure a 12 mais avoir besoin du dithering.
  * (Carte Impact avec GLX_RED_SIZE a 5 par exemple)
  */

  int aValue;
  glXGetConfig (aDisp, aVis, GLX_RED_SIZE, &aValue);

  if (myDither)
    myDither = (aValue < 8);

  if (myBackDither)
    myBackDither = (aVis->depth <= 8);

  XFree ((char* )aVis);

  myGlContext->Init ((Aspect_Drawable )aWindow, (Aspect_Display )myDisplay->GetDisplay(), (Aspect_RenderingContext )aGContext);
#endif
  myGlContext->Share (theShareCtx);

  Init();
}

// =======================================================================
// function : ~OpenGl_Window
// purpose  :
// =======================================================================
OpenGl_Window::~OpenGl_Window()
{
  if (!myOwnGContext)
  {
    myGlContext.Nullify();
    return;
  }

  // release "GL" context if it is owned by window
#if defined(_WIN32)
  HWND  aWindow          = (HWND  )myGlContext->myWindow;
  HDC   aWindowDC        = (HDC   )myGlContext->myWindowDC;
  HGLRC aWindowGContext  = (HGLRC )myGlContext->myGContext;
  HGLRC aThreadGlContext = wglGetCurrentContext();
  myGlContext.Nullify();

  if (aThreadGlContext != NULL)
  {
    // Mesa implementation can fail to reset this thread's context if wglDeleteContext()
    // called without this step. This might lead to crash when using newly created
    // context if wglMakeCurrent() is not forced right after the wglCreateContext().
    if (aThreadGlContext == aWindowGContext)
    {
      wglMakeCurrent (NULL, NULL);
    }

    wglDeleteContext (aWindowGContext);
  }
  ReleaseDC (aWindow, aWindowDC);
#else
  Display*    aDisplay  = (Display*    )myGlContext->myDisplay;
  GLXContext  aGContext = (GLXContext  )myGlContext->myGContext;
  myGlContext.Nullify();

  if (aDisplay != NULL)
  {
    // FSXXX sync necessary if non-direct rendering
    glXWaitGL();
    glXDestroyContext (aDisplay, aGContext);
  }
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

#if !defined(__APPLE__) || defined(MACOSX_USE_GLX)

// =======================================================================
// function : Resize
// purpose  : call_subr_resize
// =======================================================================
void OpenGl_Window::Resize (const CALL_DEF_WINDOW& theCWindow)
{
  DISPLAY* aDisp = (DISPLAY* )myDisplay->GetDisplay();
  if (aDisp == NULL)
    return;

  // If the size is not changed - do nothing
  if ((myWidth == theCWindow.dx) && (myHeight == theCWindow.dy))
    return;

  myWidth  = (Standard_Integer )theCWindow.dx;
  myHeight = (Standard_Integer )theCWindow.dy;

#if !defined(_WIN32)
  XResizeWindow (aDisp, myGlContext->myWindow, (unsigned int )myWidth, (unsigned int )myHeight);
  XSync (aDisp, False);
#endif

  Init();
}

#endif // !__APPLE__

// =======================================================================
// function : ReadDepths
// purpose  : TelReadDepths
// =======================================================================
void OpenGl_Window::ReadDepths (const Standard_Integer theX,     const Standard_Integer theY,
                                const Standard_Integer theWidth, const Standard_Integer theHeight,
                                float* theDepths)
{
  if (theDepths == NULL || !Activate())
    return;

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D (0.0, (GLdouble )myWidth, 0.0, (GLdouble )myHeight);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();

  glRasterPos2i (theX, theY);
  DisableFeatures();
  glReadPixels (theX, theY, theWidth, theHeight, GL_DEPTH_COMPONENT, GL_FLOAT, theDepths);
  EnableFeatures();
}

// =======================================================================
// function : SetBackgroundColor
// purpose  : call_subr_set_background
// =======================================================================
void OpenGl_Window::SetBackgroundColor (const Standard_ShortReal theR,
                                        const Standard_ShortReal theG,
                                        const Standard_ShortReal theB)
{
  myBgColor.rgb[0] = theR;
  myBgColor.rgb[1] = theG;
  myBgColor.rgb[2] = theB;
}

#if !defined(__APPLE__) || defined(MACOSX_USE_GLX)

// =======================================================================
// function : Init
// purpose  :
// =======================================================================
void OpenGl_Window::Init()
{
  if (!Activate())
    return;

#if defined(_WIN32)
  RECT cr;
  GetClientRect ((HWND )myGlContext->myWindow, &cr);
  myWidth  = cr.right - cr.left;
  myHeight = cr.bottom - cr.top;
#else
  Window aRootWin;
  int aDummy;
  unsigned int aDummyU;
  unsigned int aNewWidth  = 0;
  unsigned int aNewHeight = 0;
  DISPLAY* aDisp = (DISPLAY* )myDisplay->GetDisplay();
  XGetGeometry (aDisp, myGlContext->myWindow, &aRootWin, &aDummy, &aDummy, &aNewWidth, &aNewHeight, &aDummyU, &aDummyU);
  myWidth  = aNewWidth;
  myHeight = aNewHeight;
#endif

  glMatrixMode (GL_MODELVIEW);
  glViewport (0, 0, myWidth, myHeight);

  glDisable (GL_SCISSOR_TEST);
  glDrawBuffer (GL_BACK);
}

#endif // !__APPLE__

// =======================================================================
// function : EnablePolygonOffset
// purpose  : call_subr_enable_polygon_offset
// =======================================================================
void OpenGl_Window::EnablePolygonOffset() const
{
  Standard_ShortReal aFactor, aUnits;
  myDisplay->PolygonOffset (aFactor, aUnits);
  glPolygonOffset (aFactor, aUnits);
  glEnable (GL_POLYGON_OFFSET_FILL);
}

// =======================================================================
// function : DisablePolygonOffset
// purpose  : call_subr_disable_polygon_offset
// =======================================================================
void OpenGl_Window::DisablePolygonOffset() const
{
  glDisable (GL_POLYGON_OFFSET_FILL);
}

// =======================================================================
// function : EnableFeatures
// purpose  :
// =======================================================================
void OpenGl_Window::EnableFeatures() const
{
  /*glPixelTransferi (GL_MAP_COLOR, GL_TRUE);*/

  if (myDither)
    glEnable (GL_DITHER);
  else
    glDisable (GL_DITHER);
}

// =======================================================================
// function : DisableFeatures
// purpose  :
// =======================================================================
void OpenGl_Window::DisableFeatures() const
{
  glDisable (GL_DITHER);
  glPixelTransferi (GL_MAP_COLOR, GL_FALSE);

  /*
  * Disable stuff that's likely to slow down glDrawPixels.
  * (Omit as much of this as possible, when you know in advance
  * that the OpenGL state will already be set correctly.)
  */
  glDisable(GL_ALPHA_TEST);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_FOG);
  glDisable(GL_LIGHTING);

  glDisable(GL_LOGIC_OP);
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_TEXTURE_1D);
  glDisable(GL_TEXTURE_2D);
  glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
  glPixelTransferi(GL_RED_SCALE, 1);
  glPixelTransferi(GL_RED_BIAS, 0);
  glPixelTransferi(GL_GREEN_SCALE, 1);
  glPixelTransferi(GL_GREEN_BIAS, 0);
  glPixelTransferi(GL_BLUE_SCALE, 1);
  glPixelTransferi(GL_BLUE_BIAS, 0);
  glPixelTransferi(GL_ALPHA_SCALE, 1);
  glPixelTransferi(GL_ALPHA_BIAS, 0);

  /*
  * Disable extensions that could slow down glDrawPixels.
  * (Actually, you should check for the presence of the proper
  * extension before making these calls.  I've omitted that
  * code for simplicity.)
  */

#ifdef GL_EXT_convolution
  glDisable(GL_CONVOLUTION_1D_EXT);
  glDisable(GL_CONVOLUTION_2D_EXT);
  glDisable(GL_SEPARABLE_2D_EXT);
#endif

#ifdef GL_EXT_histogram
  glDisable(GL_HISTOGRAM_EXT);
  glDisable(GL_MINMAX_EXT);
#endif

#ifdef GL_EXT_texture3D
  glDisable(GL_TEXTURE_3D_EXT);
#endif
}

// =======================================================================
// function : MakeFrontBufCurrent
// purpose  : TelMakeFrontBufCurrent
// =======================================================================
void OpenGl_Window::MakeFrontBufCurrent() const
{
  glDrawBuffer (GL_FRONT);
}

// =======================================================================
// function : MakeBackBufCurrent
// purpose  : TelMakeBackBufCurrent
// =======================================================================
void OpenGl_Window::MakeBackBufCurrent() const
{
  glDrawBuffer (GL_BACK);
}

// =======================================================================
// function : MakeFrontAndBackBufCurrent
// purpose  : TelMakeFrontAndBackBufCurrent
// =======================================================================
void OpenGl_Window::MakeFrontAndBackBufCurrent() const
{
  glDrawBuffer (GL_FRONT_AND_BACK);
}

// =======================================================================
// function : GetGContext
// purpose  :
// =======================================================================
GLCONTEXT OpenGl_Window::GetGContext() const
{
  return (GLCONTEXT )myGlContext->myGContext;
}
