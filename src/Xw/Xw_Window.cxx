// Created on: 2013-04-06
// Created by: Kirill Gavrilov
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <Xw_Window.hxx>

#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)) && !defined(__ANDROID__) && !defined(__QNX__) && !defined(__EMSCRIPTEN__)

#include <Aspect_Convert.hxx>
#include <Aspect_WindowDefinitionError.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>

//#include <X11/XF86keysym.h>

#if defined(HAVE_EGL) || defined(HAVE_GLES2)
  #include <EGL/egl.h>
  #ifndef EGL_OPENGL_ES3_BIT
    #define EGL_OPENGL_ES3_BIT 0x00000040
  #endif
#else
  #include <GL/glx.h>

namespace
{

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

}

#endif

IMPLEMENT_STANDARD_RTTIEXT(Xw_Window, Aspect_Window)

// =======================================================================
// function : Xw_Window
// purpose  :
// =======================================================================
Xw_Window::Xw_Window (const Handle(Aspect_DisplayConnection)& theXDisplay,
                      const Standard_CString theTitle,
                      const Standard_Integer thePxLeft,
                      const Standard_Integer thePxTop,
                      const Standard_Integer thePxWidth,
                      const Standard_Integer thePxHeight,
                      const Aspect_FBConfig  theFBConfig)
: Aspect_Window(),
  myDisplay  (theXDisplay),
  myXWindow  (0),
  myFBConfig (theFBConfig),
  myXLeft    (thePxLeft),
  myYTop     (thePxTop),
  myXRight   (thePxLeft + thePxWidth),
  myYBottom  (thePxTop + thePxHeight),
  myIsOwnWin (Standard_True)
{
  if (thePxWidth <= 0 || thePxHeight <= 0)
  {
    throw Aspect_WindowDefinitionError("Xw_Window, Coordinate(s) out of range");
  }
  else if (theXDisplay.IsNull())
  {
    throw Aspect_WindowDefinitionError("Xw_Window, X Display connection is undefined");
    return;
  }

  Display* aDisp   = myDisplay->GetDisplay();
  int      aScreen = DefaultScreen(aDisp);
  Window   aParent = RootWindow   (aDisp, aScreen);
  XVisualInfo* aVisInfo = NULL;

#if defined(HAVE_EGL) || defined(HAVE_GLES2)
  EGLDisplay anEglDisplay = eglGetDisplay (aDisp);
  EGLint aVerMajor = 0; EGLint aVerMinor = 0;
  XVisualInfo aVisInfoTmp; memset (&aVisInfoTmp, 0, sizeof(aVisInfoTmp));
  if (anEglDisplay != EGL_NO_DISPLAY
   && eglInitialize (anEglDisplay, &aVerMajor, &aVerMinor) == EGL_TRUE)
  {
    EGLint aConfigAttribs[] =
    {
      EGL_RED_SIZE,     8,
      EGL_GREEN_SIZE,   8,
      EGL_BLUE_SIZE,    8,
      EGL_ALPHA_SIZE,   0,
      EGL_DEPTH_SIZE,   24,
      EGL_STENCIL_SIZE, 8,
    #if defined(HAVE_GLES2)
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    #else
      EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
    #endif
      EGL_NONE
    };

    EGLint aNbConfigs = 0;
    void* anEglConfig = NULL;
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

      if (eglChooseConfig (anEglDisplay, aConfigAttribs, &anEglConfig, 1, &aNbConfigs) == EGL_TRUE
       && anEglConfig != NULL)
      {
        break;
      }
      eglGetError();

      aConfigAttribs[4 * 2 + 1] = 16; // try config with smaller depth buffer
      if (eglChooseConfig (anEglDisplay, aConfigAttribs, &anEglConfig, 1, &aNbConfigs) == EGL_TRUE
       && anEglConfig != NULL)
      {
        break;
      }
      eglGetError();
    }

    if (anEglConfig != NULL
     && eglGetConfigAttrib (anEglDisplay, anEglConfig, EGL_NATIVE_VISUAL_ID, (EGLint* )&aVisInfoTmp.visualid) == EGL_TRUE)
    {
      int aNbVisuals = 0;
      aVisInfoTmp.screen = DefaultScreen (aDisp);
      aVisInfo = XGetVisualInfo (aDisp, VisualIDMask | VisualScreenMask, &aVisInfoTmp, &aNbVisuals);
    }
  }
  if (aVisInfo == NULL)
  {
    Message::SendWarning ("Warning: cannot choose Visual using EGL while creating Xw_Window");
  }
#else
  int aDummy = 0;
  if (!glXQueryExtension (myDisplay->GetDisplay(), &aDummy, &aDummy))
  {
    throw Aspect_WindowDefinitionError("Xw_Window, GLX extension is unavailable");
    return;
  }
  if (myFBConfig == NULL)
  {
    // FBConfigs were added in GLX version 1.3
    int aGlxMajor = 0;
    int aGlxMinor = 0;
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
        myFBConfig = aFBCfgList[0];
        aVisInfo   = glXGetVisualFromFBConfig (aDisp, myFBConfig);
      }
      XFree (aFBCfgList);
    }
  }

  if (aVisInfo == NULL)
  {
    aVisInfo = glXChooseVisual (aDisp, aScreen, TheDoubleBuffVisual);
  }
  if (aVisInfo == NULL)
  {
    throw Aspect_WindowDefinitionError("Xw_Window, couldn't find compatible Visual (RGBA, double-buffered)");
    return;
  }
#endif

  unsigned long aMask = 0;
  XSetWindowAttributes aWinAttr;
  memset(&aWinAttr, 0, sizeof(XSetWindowAttributes));
  aWinAttr.event_mask = ExposureMask | StructureNotifyMask;
  aMask |= CWEventMask;
  if (aVisInfo != NULL)
  {
    aWinAttr.colormap = XCreateColormap(aDisp, aParent, aVisInfo->visual, AllocNone);
  }
  aWinAttr.border_pixel = 0;
  aWinAttr.override_redirect = False;

  myXWindow = XCreateWindow(aDisp, aParent,
                            myXLeft, myYTop, thePxWidth, thePxHeight,
                            0, aVisInfo != NULL ? aVisInfo->depth : CopyFromParent,
                            InputOutput,
                            aVisInfo != NULL ? aVisInfo->visual : CopyFromParent,
                            CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect, &aWinAttr);
  if (aVisInfo != NULL)
  {
    XFree (aVisInfo);
    aVisInfo = NULL;
  }
  if (myXWindow == 0)
  {
    throw Aspect_WindowDefinitionError("Xw_Window, Unable to create window");
    return;
  }

  // if parent - desktop
  XSizeHints aSizeHints;
  aSizeHints.x      = myXLeft;
  aSizeHints.y      = myYTop;
  aSizeHints.flags  = PPosition;
  aSizeHints.width  = thePxWidth;
  aSizeHints.height = thePxHeight;
  aSizeHints.flags |= PSize;
  XSetStandardProperties (aDisp, myXWindow, theTitle, theTitle, None,
                          NULL, 0, &aSizeHints);

  /*XTextProperty aTitleProperty;
  aTitleProperty.encoding = None;
  char* aTitle = (char* )theTitle;
  Xutf8TextListToTextProperty(aDisp, &aTitle, 1, XUTF8StringStyle, &aTitleProperty);
  XSetWMName      (aDisp, myXWindow, &aTitleProperty);
  XSetWMProperties(aDisp, myXWindow, &aTitleProperty, &aTitleProperty, NULL, 0, NULL, NULL, NULL);*/

  XFlush (aDisp);
}

// =======================================================================
// function : Xw_Window
// purpose  :
// =======================================================================
Xw_Window::Xw_Window (const Handle(Aspect_DisplayConnection)& theXDisplay,
                      const Window theXWin,
                      const Aspect_FBConfig theFBConfig)
: Aspect_Window(),
  myDisplay  (theXDisplay),
  myXWindow  (theXWin),
  myFBConfig (theFBConfig),
  myXLeft    (0),
  myYTop     (0),
  myXRight   (512),
  myYBottom  (512),
  myIsOwnWin (Standard_False)
{
  if (theXWin == 0)
  {
    throw Aspect_WindowDefinitionError("Xw_Window, given invalid X window");
    return;
  }
  else if (theXDisplay.IsNull())
  {
    throw Aspect_WindowDefinitionError("Xw_Window, X Display connection is undefined");
    return;
  }
#if !defined(HAVE_EGL) && !defined(HAVE_GLES2)
  int aDummy = 0;
  if (!glXQueryExtension (myDisplay->GetDisplay(), &aDummy, &aDummy))
  {
    myXWindow = 0;
    throw Aspect_WindowDefinitionError("Xw_Window, GLX extension is unavailable");
    return;
  }
#endif

  Display* aDisp = myDisplay->GetDisplay();

  XWindowAttributes aWinAttr;
  XGetWindowAttributes (aDisp, myXWindow, &aWinAttr);
  XVisualInfo aVisInfoTmp;
  aVisInfoTmp.visualid = aWinAttr.visual->visualid;
  aVisInfoTmp.screen   = DefaultScreen (aDisp);
  int aNbItems = 0;
  XVisualInfo* aVisInfo = XGetVisualInfo (aDisp, VisualIDMask | VisualScreenMask, &aVisInfoTmp, &aNbItems);
  if (aVisInfo == NULL)
  {
    throw Aspect_WindowDefinitionError("Xw_Window, Visual is unavailable");
    return;
  }
  XFree (aVisInfo);

  DoResize();
}

// =======================================================================
// function : ~Xw_Window
// purpose  :
// =======================================================================
Xw_Window::~Xw_Window()
{
  if (myIsOwnWin && myXWindow != 0 && !myDisplay.IsNull())
  {
    XDestroyWindow (myDisplay->GetDisplay(), myXWindow);
  }
}

// =======================================================================
// function : XWindow
// purpose  :
// =======================================================================
Window Xw_Window::XWindow() const
{
  return myXWindow;
}

// =======================================================================
// function : IsMapped
// purpose  :
// =======================================================================
Standard_Boolean Xw_Window::IsMapped() const
{
  if (myXWindow == 0)
  {
    return false;
  }
  else if (IsVirtual())
  {
    return Standard_True;
  }

  XFlush (myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  XGetWindowAttributes (myDisplay->GetDisplay(), myXWindow, &aWinAttr);
  return aWinAttr.map_state == IsUnviewable
      || aWinAttr.map_state == IsViewable;
}

// =======================================================================
// function : Map
// purpose  :
// =======================================================================
void Xw_Window::Map() const
{
  if (IsVirtual() || myXWindow == 0)
  {
    return;
  }

  XMapWindow (myDisplay->GetDisplay(), myXWindow);
  XFlush (myDisplay->GetDisplay());
}

// =======================================================================
// function : Unmap
// purpose  :
// =======================================================================
void Xw_Window::Unmap() const
{
  if (IsVirtual() || myXWindow == 0)
  {
    return;
  }

  XIconifyWindow (myDisplay->GetDisplay(), myXWindow, DefaultScreen(myDisplay->GetDisplay()));
}

// =======================================================================
// function : DoResize
// purpose  :
// =======================================================================
Aspect_TypeOfResize Xw_Window::DoResize()
{
  if (myXWindow == 0)
  {
    return Aspect_TOR_UNKNOWN;
  }

  XFlush (myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  XGetWindowAttributes (myDisplay->GetDisplay(), myXWindow, &aWinAttr);
  if (aWinAttr.map_state == IsUnmapped)
  {
    return Aspect_TOR_UNKNOWN;
  }

  Standard_Integer aMask = 0;
  Aspect_TypeOfResize aMode = Aspect_TOR_UNKNOWN;

  if (Abs (aWinAttr.x                     - myXLeft  ) > 2) aMask |= 1;
  if (Abs ((aWinAttr.x + aWinAttr.width)  - myXRight ) > 2) aMask |= 2;
  if (Abs (aWinAttr.y                     - myYTop   ) > 2) aMask |= 4;
  if (Abs ((aWinAttr.y + aWinAttr.height) - myYBottom) > 2) aMask |= 8;
  switch (aMask)
  {
    case 0:  aMode = Aspect_TOR_NO_BORDER;               break;
    case 1:  aMode = Aspect_TOR_LEFT_BORDER;             break;
    case 2:  aMode = Aspect_TOR_RIGHT_BORDER;            break;
    case 4:  aMode = Aspect_TOR_TOP_BORDER;              break;
    case 5:  aMode = Aspect_TOR_LEFT_AND_TOP_BORDER;     break;
    case 6:  aMode = Aspect_TOR_TOP_AND_RIGHT_BORDER;    break;
    case 8:  aMode = Aspect_TOR_BOTTOM_BORDER;           break;
    case 9:  aMode = Aspect_TOR_BOTTOM_AND_LEFT_BORDER;  break;
    case 10: aMode = Aspect_TOR_RIGHT_AND_BOTTOM_BORDER; break;
    default: break;
  }

  myXLeft   = aWinAttr.x;
  myXRight  = aWinAttr.x + aWinAttr.width;
  myYTop    = aWinAttr.y;
  myYBottom = aWinAttr.y + aWinAttr.height;
  return aMode;
}

// =======================================================================
// function : DoMapping
// purpose  :
// =======================================================================
Standard_Boolean Xw_Window::DoMapping() const
{
  return Standard_True; // IsMapped()
}

// =======================================================================
// function : Ratio
// purpose  :
// =======================================================================
Standard_Real Xw_Window::Ratio() const
{
  if (myXWindow == 0)
  {
    return 1.0;
  }

  XFlush (myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  XGetWindowAttributes (myDisplay->GetDisplay(), myXWindow, &aWinAttr);
  return Standard_Real(aWinAttr.width) / Standard_Real(aWinAttr.height);
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void Xw_Window::Position (Standard_Integer& X1, Standard_Integer& Y1,
                          Standard_Integer& X2, Standard_Integer& Y2) const
{
  if (myXWindow == 0)
  {
    return;
  }

  XFlush (myDisplay->GetDisplay());
  XWindowAttributes anAttributes;
  XGetWindowAttributes (myDisplay->GetDisplay(), myXWindow, &anAttributes);
  Window aChild;
  XTranslateCoordinates (myDisplay->GetDisplay(), anAttributes.root, myXWindow,
                         0, 0, &anAttributes.x, &anAttributes.y, &aChild);

  X1 = -anAttributes.x;
  X2 = X1 + anAttributes.width;
  Y1 = -anAttributes.y;
  Y2 = Y1 + anAttributes.height;
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
void Xw_Window::Size (Standard_Integer& theWidth,
                      Standard_Integer& theHeight) const
{
  if (myXWindow == 0)
  {
    return;
  }

  XFlush (myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  XGetWindowAttributes (myDisplay->GetDisplay(), myXWindow, &aWinAttr);
  theWidth  = aWinAttr.width;
  theHeight = aWinAttr.height;
}

// =======================================================================
// function : SetTitle
// purpose  :
// =======================================================================
void Xw_Window::SetTitle (const TCollection_AsciiString& theTitle)
{
  if (myXWindow != 0)
  {
    XStoreName (myDisplay->GetDisplay(), myXWindow, theTitle.ToCString());
  }
}

// =======================================================================
// function : InvalidateContent
// purpose  :
// =======================================================================
void Xw_Window::InvalidateContent (const Handle(Aspect_DisplayConnection)& theDisp)
{
  if (myXWindow == 0)
  {
    return;
  }

  const Handle(Aspect_DisplayConnection)& aDisp = !theDisp.IsNull() ? theDisp : myDisplay;
  Display* aDispX = aDisp->GetDisplay();

  XEvent anEvent;
  memset (&anEvent, 0, sizeof(anEvent));
  anEvent.type = Expose;
  anEvent.xexpose.window = myXWindow;
  XSendEvent (aDispX, myXWindow, False, ExposureMask, &anEvent);
  XFlush (aDispX);
}

// =======================================================================
// function : VirtualKeyFromNative
// purpose  :
// =======================================================================
Aspect_VKey Xw_Window::VirtualKeyFromNative (unsigned long theKey)
{
  if (theKey >= XK_0
   && theKey <= XK_9)
  {
    return Aspect_VKey(theKey - XK_0 + Aspect_VKey_0);
  }

  if (theKey >= XK_A
   && theKey <= XK_Z)
  {
    return Aspect_VKey(theKey - XK_A + Aspect_VKey_A);
  }

  if (theKey >= XK_a
   && theKey <= XK_z)
  {
    return Aspect_VKey(theKey - XK_a + Aspect_VKey_A);
  }

  if (theKey >= XK_F1
   && theKey <= XK_F24)
  {
    if (theKey <= XK_F12)
    {
      return Aspect_VKey(theKey - XK_F1 + Aspect_VKey_F1);
    }
    return Aspect_VKey_UNKNOWN;
  }

  switch (theKey)
  {
    case XK_space:
      return Aspect_VKey_Space;
    case XK_apostrophe:
      return Aspect_VKey_Apostrophe;
    case XK_comma:
      return Aspect_VKey_Comma;
    case XK_minus:
      return Aspect_VKey_Minus;
    case XK_period:
      return Aspect_VKey_Period;
    case XK_semicolon:
      return Aspect_VKey_Semicolon;
    case XK_equal:
      return Aspect_VKey_Equal;
    case XK_bracketleft:
      return Aspect_VKey_BracketLeft;
    case XK_backslash:
      return Aspect_VKey_Backslash;
    case XK_bracketright:
      return Aspect_VKey_BracketRight;
    case XK_BackSpace:
      return Aspect_VKey_Backspace;
    case XK_Tab:
      return Aspect_VKey_Tab;
    //case XK_Linefeed:
    case XK_Return:
    case XK_KP_Enter:
      return Aspect_VKey_Enter;
    //case XK_Pause:
    //  return Aspect_VKey_Pause;
    case XK_Escape:
      return Aspect_VKey_Escape;
    case XK_Home:
      return Aspect_VKey_Home;
    case XK_Left:
      return Aspect_VKey_Left;
    case XK_Up:
      return Aspect_VKey_Up;
    case XK_Right:
      return Aspect_VKey_Right;
    case XK_Down:
      return Aspect_VKey_Down;
    case XK_Prior:
      return Aspect_VKey_PageUp;
    case XK_Next:
      return Aspect_VKey_PageDown;
    case XK_End:
      return Aspect_VKey_End;
    //case XK_Insert:
    //  return Aspect_VKey_Insert;
    case XK_Menu:
      return Aspect_VKey_Menu;
    case XK_Num_Lock:
      return Aspect_VKey_Numlock;
    //case XK_KP_Delete:
    //  return Aspect_VKey_NumDelete;
    case XK_KP_Multiply:
      return Aspect_VKey_NumpadMultiply;
    case XK_KP_Add:
      return Aspect_VKey_NumpadAdd;
    //case XK_KP_Separator:
    //  return Aspect_VKey_Separator;
    case XK_KP_Subtract:
      return Aspect_VKey_NumpadSubtract;
    //case XK_KP_Decimal:
    //  return Aspect_VKey_Decimal;
    case XK_KP_Divide:
      return Aspect_VKey_NumpadDivide;
    case XK_Shift_L:
    case XK_Shift_R:
      return Aspect_VKey_Shift;
    case XK_Control_L:
    case XK_Control_R:
      return Aspect_VKey_Control;
    //case XK_Caps_Lock:
    //  return Aspect_VKey_CapsLock;
    case XK_Alt_L:
    case XK_Alt_R:
      return Aspect_VKey_Alt;
    //case XK_Super_L:
    //case XK_Super_R:
    //  return Aspect_VKey_Super;
    case XK_Delete:
      return Aspect_VKey_Delete;

    case 0x1008FF11: // XF86AudioLowerVolume
      return Aspect_VKey_VolumeDown;
    case 0x1008FF12: // XF86AudioMute
      return Aspect_VKey_VolumeMute;
    case 0x1008FF13: // XF86AudioRaiseVolume
      return Aspect_VKey_VolumeUp;

    case 0x1008FF14: // XF86AudioPlay
      return Aspect_VKey_MediaPlayPause;
    case 0x1008FF15: // XF86AudioStop
      return Aspect_VKey_MediaStop;
    case 0x1008FF16: // XF86AudioPrev
      return Aspect_VKey_MediaPreviousTrack;
    case 0x1008FF17: // XF86AudioNext
      return Aspect_VKey_MediaNextTrack;

    case 0x1008FF18: // XF86HomePage
      return Aspect_VKey_BrowserHome;
    case 0x1008FF26: // XF86Back
      return Aspect_VKey_BrowserBack;
    case 0x1008FF27: // XF86Forward
      return Aspect_VKey_BrowserForward;
    case 0x1008FF28: // XF86Stop
      return Aspect_VKey_BrowserStop;
    case 0x1008FF29: // XF86Refresh
      return Aspect_VKey_BrowserRefresh;
  }
  return Aspect_VKey_UNKNOWN;
}

#endif //  Win32 or Mac OS X
