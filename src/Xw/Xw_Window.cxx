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

#include <Aspect_ScrollDelta.hxx>
#include <Aspect_WindowDefinitionError.hxx>
#include <Aspect_WindowInputListener.hxx>
#include <Message.hxx>

#if defined(HAVE_XLIB)
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
// #include <X11/XF86keysym.h>
#endif

#include <Aspect_DisplayConnection.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Xw_Window, Aspect_Window)

// =======================================================================
// function : Xw_Window
// purpose  :
// =======================================================================
Xw_Window::Xw_Window(const Handle(Aspect_DisplayConnection)& theXDisplay,
                     const Standard_CString                  theTitle,
                     const Standard_Integer                  thePxLeft,
                     const Standard_Integer                  thePxTop,
                     const Standard_Integer                  thePxWidth,
                     const Standard_Integer                  thePxHeight)
    : Aspect_Window(),
      myXWindow(0),
      myFBConfig(NULL),
      myXLeft(thePxLeft),
      myYTop(thePxTop),
      myXRight(thePxLeft + thePxWidth),
      myYBottom(thePxTop + thePxHeight),
      myIsOwnWin(Standard_True)
{
  myDisplay = theXDisplay;
  if (thePxWidth <= 0 || thePxHeight <= 0)
  {
    throw Aspect_WindowDefinitionError("Xw_Window, Coordinate(s) out of range");
  }
  else if (theXDisplay.IsNull())
  {
    throw Aspect_WindowDefinitionError("Xw_Window, X Display connection is undefined");
  }

#if defined(HAVE_XLIB)
  myFBConfig            = theXDisplay->GetDefaultFBConfig();
  XVisualInfo* aVisInfo = theXDisplay->GetDefaultVisualInfoX();

  Display* aDisp   = myDisplay->GetDisplay();
  int      aScreen = DefaultScreen(aDisp);
  Window   aParent = RootWindow(aDisp, aScreen);

  XSetWindowAttributes aWinAttr;
  memset(&aWinAttr, 0, sizeof(aWinAttr));
  aWinAttr.event_mask = ExposureMask | StructureNotifyMask;

  if (aVisInfo != NULL)
  {
    aWinAttr.colormap = XCreateColormap(aDisp, aParent, aVisInfo->visual, AllocNone);
  }
  aWinAttr.border_pixel      = 0;
  aWinAttr.override_redirect = False;

  myXWindow = (Window)XCreateWindow(aDisp,
                                    aParent,
                                    myXLeft,
                                    myYTop,
                                    thePxWidth,
                                    thePxHeight,
                                    0,
                                    aVisInfo != NULL ? aVisInfo->depth : CopyFromParent,
                                    InputOutput,
                                    aVisInfo != NULL ? aVisInfo->visual : CopyFromParent,
                                    CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
                                    &aWinAttr);
  if (myXWindow == 0)
  {
    throw Aspect_WindowDefinitionError("Xw_Window, Unable to create window");
  }

  // if parent - desktop
  XSizeHints aSizeHints;
  aSizeHints.x      = myXLeft;
  aSizeHints.y      = myYTop;
  aSizeHints.flags  = PPosition;
  aSizeHints.width  = thePxWidth;
  aSizeHints.height = thePxHeight;
  aSizeHints.flags |= PSize;
  XSetStandardProperties(aDisp, (Window)myXWindow, theTitle, theTitle, None, NULL, 0, &aSizeHints);

  /*XTextProperty aTitleProperty;
  aTitleProperty.encoding = None;
  char* aTitle = (char* )theTitle;
  Xutf8TextListToTextProperty(aDisp, &aTitle, 1, XUTF8StringStyle, &aTitleProperty);
  XSetWMName      (aDisp, (Window )myXWindow, &aTitleProperty);
  XSetWMProperties(aDisp, (Window )myXWindow, &aTitleProperty, &aTitleProperty, NULL, 0, NULL, NULL,
  NULL);*/

  XFlush(aDisp);
#else
  (void)theTitle;
  if (myXWindow == 0)
  {
    throw Aspect_WindowDefinitionError("Xw_Window, Unable to create window - not implemented");
  }
#endif
}

// =======================================================================
// function : Xw_Window
// purpose  :
// =======================================================================
Xw_Window::Xw_Window(const Handle(Aspect_DisplayConnection)& theXDisplay,
                     const Aspect_Drawable                   theXWin,
                     const Aspect_FBConfig                   theFBConfig)
    : Aspect_Window(),
      myXWindow(theXWin),
      myFBConfig(theFBConfig),
      myXLeft(0),
      myYTop(0),
      myXRight(512),
      myYBottom(512),
      myIsOwnWin(Standard_False)
{
  myDisplay = theXDisplay;
  if (theXWin == 0)
  {
    throw Aspect_WindowDefinitionError("Xw_Window, given invalid X window");
  }
  else if (theXDisplay.IsNull())
  {
    throw Aspect_WindowDefinitionError("Xw_Window, X Display connection is undefined");
  }

#if defined(HAVE_XLIB)
  Display* aDisp = myDisplay->GetDisplay();

  XWindowAttributes aWinAttr;
  XGetWindowAttributes(aDisp, (Window)myXWindow, &aWinAttr);
  XVisualInfo aVisInfoTmp;
  aVisInfoTmp.visualid  = aWinAttr.visual->visualid;
  aVisInfoTmp.screen    = DefaultScreen(aDisp);
  int          aNbItems = 0;
  XVisualInfo* aVisInfo =
    XGetVisualInfo(aDisp, VisualIDMask | VisualScreenMask, &aVisInfoTmp, &aNbItems);
  if (aVisInfo == NULL)
  {
    throw Aspect_WindowDefinitionError("Xw_Window, Visual is unavailable");
  }
  XFree(aVisInfo);

  DoResize();
#else
  // throw Standard_NotImplemented("Xw_Window, not implemented");
#endif
}

// =======================================================================
// function : ~Xw_Window
// purpose  :
// =======================================================================
Xw_Window::~Xw_Window()
{
  if (myIsOwnWin && myXWindow != 0 && !myDisplay.IsNull())
  {
#if defined(HAVE_XLIB)
    XDestroyWindow(myDisplay->GetDisplay(), (Window)myXWindow);
#endif
  }
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

#if defined(HAVE_XLIB)
  XFlush(myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  XGetWindowAttributes(myDisplay->GetDisplay(), (Window)myXWindow, &aWinAttr);
  return aWinAttr.map_state == IsUnviewable || aWinAttr.map_state == IsViewable;
#else
  return Standard_False;
#endif
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

#if defined(HAVE_XLIB)
  XMapWindow(myDisplay->GetDisplay(), (Window)myXWindow);
  XFlush(myDisplay->GetDisplay());
#endif
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

#if defined(HAVE_XLIB)
  XIconifyWindow(myDisplay->GetDisplay(),
                 (Window)myXWindow,
                 DefaultScreen(myDisplay->GetDisplay()));
#endif
}

// =======================================================================
// function : DoResize
// purpose  :
// =======================================================================
Aspect_TypeOfResize Xw_Window::DoResize()
{
  if (IsVirtual() || myXWindow == 0)
  {
    return Aspect_TOR_UNKNOWN;
  }

#if defined(HAVE_XLIB)
  XFlush(myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  memset(&aWinAttr, 0, sizeof(aWinAttr));
  XGetWindowAttributes(myDisplay->GetDisplay(), (Window)myXWindow, &aWinAttr);
  if (aWinAttr.map_state == IsUnmapped)
  {
    return Aspect_TOR_UNKNOWN;
  }

  Standard_Integer    aMask = 0;
  Aspect_TypeOfResize aMode = Aspect_TOR_UNKNOWN;

  if (Abs(aWinAttr.x - myXLeft) > 2)
    aMask |= 1;
  if (Abs((aWinAttr.x + aWinAttr.width) - myXRight) > 2)
    aMask |= 2;
  if (Abs(aWinAttr.y - myYTop) > 2)
    aMask |= 4;
  if (Abs((aWinAttr.y + aWinAttr.height) - myYBottom) > 2)
    aMask |= 8;
  switch (aMask)
  {
    case 0:
      aMode = Aspect_TOR_NO_BORDER;
      break;
    case 1:
      aMode = Aspect_TOR_LEFT_BORDER;
      break;
    case 2:
      aMode = Aspect_TOR_RIGHT_BORDER;
      break;
    case 4:
      aMode = Aspect_TOR_TOP_BORDER;
      break;
    case 5:
      aMode = Aspect_TOR_LEFT_AND_TOP_BORDER;
      break;
    case 6:
      aMode = Aspect_TOR_TOP_AND_RIGHT_BORDER;
      break;
    case 8:
      aMode = Aspect_TOR_BOTTOM_BORDER;
      break;
    case 9:
      aMode = Aspect_TOR_BOTTOM_AND_LEFT_BORDER;
      break;
    case 10:
      aMode = Aspect_TOR_RIGHT_AND_BOTTOM_BORDER;
      break;
    default:
      break;
  }

  myXLeft   = aWinAttr.x;
  myXRight  = aWinAttr.x + aWinAttr.width;
  myYTop    = aWinAttr.y;
  myYBottom = aWinAttr.y + aWinAttr.height;
  return aMode;
#else
  return Aspect_TOR_UNKNOWN;
#endif
}

// =======================================================================
// function : Ratio
// purpose  :
// =======================================================================
Standard_Real Xw_Window::Ratio() const
{
  if (IsVirtual() || myXWindow == 0)
  {
    return Standard_Real(myXRight - myXLeft) / Standard_Real(myYBottom - myYTop);
  }

#if defined(HAVE_XLIB)
  XFlush(myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  memset(&aWinAttr, 0, sizeof(aWinAttr));
  XGetWindowAttributes(myDisplay->GetDisplay(), (Window)myXWindow, &aWinAttr);
  return Standard_Real(aWinAttr.width) / Standard_Real(aWinAttr.height);
#else
  return 1.0;
#endif
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void Xw_Window::Position(Standard_Integer& theX1,
                         Standard_Integer& theY1,
                         Standard_Integer& theX2,
                         Standard_Integer& theY2) const
{
  if (IsVirtual() || myXWindow == 0)
  {
    theX1 = myXLeft;
    theX2 = myXRight;
    theY1 = myYTop;
    theY2 = myYBottom;
    return;
  }

#if defined(HAVE_XLIB)
  XFlush(myDisplay->GetDisplay());
  XWindowAttributes anAttributes;
  memset(&anAttributes, 0, sizeof(anAttributes));
  XGetWindowAttributes(myDisplay->GetDisplay(), (Window)myXWindow, &anAttributes);
  Window aChild;
  XTranslateCoordinates(myDisplay->GetDisplay(),
                        anAttributes.root,
                        (Window)myXWindow,
                        0,
                        0,
                        &anAttributes.x,
                        &anAttributes.y,
                        &aChild);

  theX1 = -anAttributes.x;
  theX2 = theX1 + anAttributes.width;
  theY1 = -anAttributes.y;
  theY2 = theY1 + anAttributes.height;
#endif
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
void Xw_Window::Size(Standard_Integer& theWidth, Standard_Integer& theHeight) const
{
  if (IsVirtual() || myXWindow == 0)
  {
    theWidth  = myXRight - myXLeft;
    theHeight = myYBottom - myYTop;
    return;
  }

#if defined(HAVE_XLIB)
  XFlush(myDisplay->GetDisplay());
  XWindowAttributes aWinAttr;
  memset(&aWinAttr, 0, sizeof(aWinAttr));
  XGetWindowAttributes(myDisplay->GetDisplay(), (Window)myXWindow, &aWinAttr);
  theWidth  = aWinAttr.width;
  theHeight = aWinAttr.height;
#endif
}

// =======================================================================
// function : SetTitle
// purpose  :
// =======================================================================
void Xw_Window::SetTitle(const TCollection_AsciiString& theTitle)
{
  if (myXWindow != 0)
  {
#if defined(HAVE_XLIB)
    XStoreName(myDisplay->GetDisplay(), (Window)myXWindow, theTitle.ToCString());
#else
    (void)theTitle;
#endif
  }
}

// =======================================================================
// function : InvalidateContent
// purpose  :
// =======================================================================
void Xw_Window::InvalidateContent(const Handle(Aspect_DisplayConnection)& theDisp)
{
  if (myXWindow == 0)
  {
    return;
  }

#if defined(HAVE_XLIB)
  const Handle(Aspect_DisplayConnection)& aDisp  = !theDisp.IsNull() ? theDisp : myDisplay;
  Display*                                aDispX = aDisp->GetDisplay();

  XEvent anEvent;
  memset(&anEvent, 0, sizeof(anEvent));
  anEvent.type           = Expose;
  anEvent.xexpose.window = (Window)myXWindow;
  XSendEvent(aDispX, (Window)myXWindow, False, ExposureMask, &anEvent);
  XFlush(aDispX);
#else
  (void)theDisp;
#endif
}

// =======================================================================
// function : VirtualKeyFromNative
// purpose  :
// =======================================================================
Aspect_VKey Xw_Window::VirtualKeyFromNative(unsigned long theKey)
{
#if defined(HAVE_XLIB)
  if (theKey >= XK_0 && theKey <= XK_9)
  {
    return Aspect_VKey(theKey - XK_0 + Aspect_VKey_0);
  }

  if (theKey >= XK_A && theKey <= XK_Z)
  {
    return Aspect_VKey(theKey - XK_A + Aspect_VKey_A);
  }

  if (theKey >= XK_a && theKey <= XK_z)
  {
    return Aspect_VKey(theKey - XK_a + Aspect_VKey_A);
  }

  if (theKey >= XK_F1 && theKey <= XK_F24)
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
    // case XK_Linefeed:
    case XK_Return:
    case XK_KP_Enter:
      return Aspect_VKey_Enter;
    // case XK_Pause:
    //   return Aspect_VKey_Pause;
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
    // case XK_Insert:
    //   return Aspect_VKey_Insert;
    case XK_Menu:
      return Aspect_VKey_Menu;
    case XK_Num_Lock:
      return Aspect_VKey_Numlock;
    // case XK_KP_Delete:
    //   return Aspect_VKey_NumDelete;
    case XK_KP_Multiply:
      return Aspect_VKey_NumpadMultiply;
    case XK_KP_Add:
      return Aspect_VKey_NumpadAdd;
    // case XK_KP_Separator:
    //   return Aspect_VKey_Separator;
    case XK_KP_Subtract:
      return Aspect_VKey_NumpadSubtract;
    // case XK_KP_Decimal:
    //   return Aspect_VKey_Decimal;
    case XK_KP_Divide:
      return Aspect_VKey_NumpadDivide;
    case XK_Shift_L:
    case XK_Shift_R:
      return Aspect_VKey_Shift;
    case XK_Control_L:
    case XK_Control_R:
      return Aspect_VKey_Control;
    // case XK_Caps_Lock:
    //   return Aspect_VKey_CapsLock;
    case XK_Alt_L:
    case XK_Alt_R:
      return Aspect_VKey_Alt;
    // case XK_Super_L:
    // case XK_Super_R:
    //   return Aspect_VKey_Super;
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
#else
  (void)theKey;
#endif
  return Aspect_VKey_UNKNOWN;
}

// =======================================================================
// function : ProcessMessage
// purpose  :
// =======================================================================
bool Xw_Window::ProcessMessage(Aspect_WindowInputListener& theListener,
                               XEvent&
#if defined(HAVE_XLIB) // msvc before VS2015 had problems with (void )theMsg
                                 theMsg
#endif
)
{
#if defined(HAVE_XLIB)
  Display* aDisplay = myDisplay->GetDisplay();

  // Handle event for the chosen display connection
  switch (theMsg.type)
  {
    case ClientMessage: {
      if ((Atom)theMsg.xclient.data.l[0] == myDisplay->GetAtom(Aspect_XA_DELETE_WINDOW)
          && theMsg.xclient.window == (Window)myXWindow)
      {
        theListener.ProcessClose();
        return true;
      }
      return false;
    }
    case FocusIn:
    case FocusOut: {
      if (theMsg.xfocus.window == (Window)myXWindow)
      {
        theListener.ProcessFocus(theMsg.type == FocusIn);
      }
      return true;
    }
    case Expose: {
      if (theMsg.xexpose.window == (Window)myXWindow)
      {
        theListener.ProcessExpose();
      }

      // remove all the ExposureMask and process them at once
      for (int aNbMaxEvents = XPending(aDisplay); aNbMaxEvents > 0; --aNbMaxEvents)
      {
        if (!XCheckWindowEvent(aDisplay, (Window)myXWindow, ExposureMask, &theMsg))
        {
          break;
        }
      }

      return true;
    }
    case ConfigureNotify: {
      // remove all the StructureNotifyMask and process them at once
      for (int aNbMaxEvents = XPending(aDisplay); aNbMaxEvents > 0; --aNbMaxEvents)
      {
        if (!XCheckWindowEvent(aDisplay, (Window)myXWindow, StructureNotifyMask, &theMsg))
        {
          break;
        }
      }

      if (theMsg.xconfigure.window == (Window)myXWindow)
      {
        theListener.ProcessConfigure(true);
      }
      return true;
    }
    case KeyPress:
    case KeyRelease: {
      XKeyEvent*        aKeyEvent = (XKeyEvent*)&theMsg;
      const KeySym      aKeySym   = XLookupKeysym(aKeyEvent, 0);
      const Aspect_VKey aVKey     = Xw_Window::VirtualKeyFromNative(aKeySym);
      if (aVKey != Aspect_VKey_UNKNOWN)
      {
        const double aTimeStamp = theListener.EventTime();
        if (theMsg.type == KeyPress)
        {
          theListener.KeyDown(aVKey, aTimeStamp);
        }
        else
        {
          theListener.KeyUp(aVKey, aTimeStamp);
        }
        theListener.ProcessInput();
      }
      return true;
    }
    case ButtonPress:
    case ButtonRelease: {
      const Graphic3d_Vec2i aPos(theMsg.xbutton.x, theMsg.xbutton.y);
      Aspect_VKeyFlags      aFlags  = Aspect_VKeyFlags_NONE;
      Aspect_VKeyMouse      aButton = Aspect_VKeyMouse_NONE;
      if (theMsg.xbutton.button == Button1)
      {
        aButton = Aspect_VKeyMouse_LeftButton;
      }
      if (theMsg.xbutton.button == Button2)
      {
        aButton = Aspect_VKeyMouse_MiddleButton;
      }
      if (theMsg.xbutton.button == Button3)
      {
        aButton = Aspect_VKeyMouse_RightButton;
      }

      if ((theMsg.xbutton.state & ControlMask) != 0)
      {
        aFlags |= Aspect_VKeyFlags_CTRL;
      }
      if ((theMsg.xbutton.state & ShiftMask) != 0)
      {
        aFlags |= Aspect_VKeyFlags_SHIFT;
      }
      if (theListener.Keys().IsKeyDown(Aspect_VKey_Alt))
      {
        aFlags |= Aspect_VKeyFlags_ALT;
      }

      if (theMsg.xbutton.button == Button4 || theMsg.xbutton.button == Button5)
      {
        if (theMsg.type != ButtonPress)
        {
          return true;
        }

        const double aDeltaF = (theMsg.xbutton.button == Button4 ? 1.0 : -1.0);
        theListener.UpdateMouseScroll(Aspect_ScrollDelta(aPos, aDeltaF, aFlags));
      }
      else if (theMsg.type == ButtonPress)
      {
        theListener.PressMouseButton(aPos, aButton, aFlags, false);
      }
      else
      {
        theListener.ReleaseMouseButton(aPos, aButton, aFlags, false);
      }
      theListener.ProcessInput();
      return true;
    }
    case MotionNotify: {
      if (theMsg.xmotion.window != (Window)myXWindow)
      {
        return false;
      }

      // remove all the ButtonMotionMask and process them at once
      for (int aNbMaxEvents = XPending(aDisplay); aNbMaxEvents > 0; --aNbMaxEvents)
      {
        if (!XCheckWindowEvent(aDisplay,
                               (Window)myXWindow,
                               ButtonMotionMask | PointerMotionMask,
                               &theMsg))
        {
          break;
        }
      }

      Graphic3d_Vec2i  aPos(theMsg.xmotion.x, theMsg.xmotion.y);
      Aspect_VKeyMouse aButtons = Aspect_VKeyMouse_NONE;
      Aspect_VKeyFlags aFlags   = Aspect_VKeyFlags_NONE;
      if ((theMsg.xmotion.state & Button1Mask) != 0)
      {
        aButtons |= Aspect_VKeyMouse_LeftButton;
      }
      if ((theMsg.xmotion.state & Button2Mask) != 0)
      {
        aButtons |= Aspect_VKeyMouse_MiddleButton;
      }
      if ((theMsg.xmotion.state & Button3Mask) != 0)
      {
        aButtons |= Aspect_VKeyMouse_RightButton;
      }

      if ((theMsg.xmotion.state & ControlMask) != 0)
      {
        aFlags |= Aspect_VKeyFlags_CTRL;
      }
      if ((theMsg.xmotion.state & ShiftMask) != 0)
      {
        aFlags |= Aspect_VKeyFlags_SHIFT;
      }
      if (theListener.Keys().IsKeyDown(Aspect_VKey_Alt))
      {
        aFlags |= Aspect_VKeyFlags_ALT;
      }

      theListener.UpdateMousePosition(aPos, aButtons, aFlags, false);
      theListener.ProcessInput();
      return true;
    }
  }
#else
  (void)theListener;
#endif
  return false;
}
