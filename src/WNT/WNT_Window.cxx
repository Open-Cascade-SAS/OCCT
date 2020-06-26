// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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
  // include windows.h first to have all definitions available
  #include <windows.h>
#endif

#include <WNT_Window.hxx>

#if defined(_WIN32) && !defined(OCCT_UWP)

#include <Aspect_Convert.hxx>
#include <Aspect_WindowDefinitionError.hxx>
#include <Aspect_WindowError.hxx>
#include <Message.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <WNT_WClass.hxx>

IMPLEMENT_STANDARD_RTTIEXT(WNT_Window, Aspect_Window)

// =======================================================================
// function : WNT_Window
// purpose  :
// =======================================================================
WNT_Window::WNT_Window (const Standard_CString           theTitle,
                        const Handle(WNT_WClass)&        theClass,
                        const WNT_Dword&                 theStyle,
                        const Standard_Integer           thePxLeft,
                        const Standard_Integer           thePxTop,
                        const Standard_Integer           thePxWidth,
                        const Standard_Integer           thePxHeight,
                        const Quantity_NameOfColor       theBackColor,
                        const Aspect_Handle              theParent,
                        const Aspect_Handle              theMenu,
                        const Standard_Address           theClientStruct)
: Aspect_Window(),
  aXLeft (thePxLeft),
  aYTop (thePxTop),
  aXRight (thePxLeft + thePxWidth),
  aYBottom (thePxTop + thePxHeight),
  myWClass (theClass),
  myIsForeign (Standard_False)
{
  if (thePxWidth <= 0 || thePxHeight <= 0)
  {
    throw Aspect_WindowDefinitionError("Coordinate(s) out of range");
  }

  DWORD aStyle = theStyle;
  if (theParent && !(theStyle & WS_CHILD))
  {
    aStyle |= WS_CHILD | WS_CLIPSIBLINGS;
  }
  else if (!theParent && !(theStyle & WS_CLIPCHILDREN))
  {
    aStyle |= WS_CLIPCHILDREN;
  }

  // include decorations in the window dimensions to reproduce same behavior of Xw_Window
  RECT aRect;
  aRect.top    = aYTop;
  aRect.bottom = aYBottom;
  aRect.left   = aXLeft;
  aRect.right  = aXRight;
  AdjustWindowRect (&aRect, aStyle, theMenu != NULL ? TRUE : FALSE);
  aXLeft   = aRect.left;
  aYTop    = aRect.top;
  aXRight  = aRect.right;
  aYBottom = aRect.bottom;

  const TCollection_ExtendedString aTitleW (theTitle);
  const TCollection_ExtendedString aClassNameW (myWClass->Name());
  myHWindow = CreateWindowW (aClassNameW.ToWideString(), aTitleW.ToWideString(),
                             aStyle,
                             aXLeft, aYTop,
                             (aXRight - aXLeft), (aYBottom - aYTop),
                             (HWND )theParent,
                             (HMENU )theMenu,
                             (HINSTANCE )myWClass->Instance(),
                             theClientStruct);
  if (!myHWindow)
  {
    throw Aspect_WindowDefinitionError("Unable to create window");
  }

  myHParentWindow = theParent;
  SetBackground (theBackColor);
}

// =======================================================================
// function : WNT_Window
// purpose  :
// =======================================================================
WNT_Window::WNT_Window (const Aspect_Handle        theHandle,
                        const Quantity_NameOfColor theBackColor)
: myIsForeign (Standard_True)
{
  myHWindow        = theHandle;
  myHParentWindow  = GetParent ((HWND )theHandle);

  SetBackground (theBackColor);

  WINDOWPLACEMENT aPlace;
  aPlace.length = sizeof (WINDOWPLACEMENT);
  ::GetWindowPlacement ((HWND )myHWindow, &aPlace);

  aXLeft   = aPlace.rcNormalPosition.left;
  aYTop    = aPlace.rcNormalPosition.top;
  aXRight  = aPlace.rcNormalPosition.right;
  aYBottom = aPlace.rcNormalPosition.bottom;
}

// =======================================================================
// function : ~WNT_Window
// purpose  :
// =======================================================================
WNT_Window::~WNT_Window()
{
  if (myHWindow == NULL
   || myIsForeign)
  {
    return;
  }

  DestroyWindow ((HWND )myHWindow);
  myIsForeign = Standard_False;
}

// =======================================================================
// function : SetCursor
// purpose  :
// =======================================================================
void WNT_Window::SetCursor (const Aspect_Handle theCursor) const
{
  ::SetClassLongPtrW ((HWND )myHWindow, GCLP_HCURSOR, (LONG_PTR )theCursor);
}

// =======================================================================
// function : IsMapped
// purpose  :
// =======================================================================
Standard_Boolean WNT_Window::IsMapped() const
{
  if (IsVirtual())
  {
    return Standard_True;
  }

  WINDOWPLACEMENT aPlace;
  aPlace.length = sizeof (WINDOWPLACEMENT);
  ::GetWindowPlacement ((HWND )myHWindow, &aPlace);
  return !(aPlace.showCmd == SW_HIDE
        || aPlace.showCmd == SW_MINIMIZE);
}

// =======================================================================
// function : Map
// purpose  :
// =======================================================================
void WNT_Window::Map() const
{
  if (!IsVirtual())
  {
    Map (SW_SHOW);
  }
}

// =======================================================================
// function : Map
// purpose  :
// =======================================================================
void WNT_Window::Map (const Standard_Integer theMapMode) const
{
  if (IsVirtual())
  {
    return;
  }

  ::ShowWindow   ((HWND )myHWindow, theMapMode);
  ::UpdateWindow ((HWND )myHWindow);
}

// =======================================================================
// function : Unmap
// purpose  :
// =======================================================================
void WNT_Window::Unmap() const
{
  Map (SW_HIDE);
}

// =======================================================================
// function : DoResize
// purpose  :
// =======================================================================
Aspect_TypeOfResize WNT_Window::DoResize()
{
  if (IsVirtual())
  {
    return Aspect_TOR_UNKNOWN;
  }

  int                 mask = 0;
  Aspect_TypeOfResize mode = Aspect_TOR_UNKNOWN;
  WINDOWPLACEMENT     wp;

  wp.length = sizeof ( WINDOWPLACEMENT );
  GetWindowPlacement (  ( HWND )myHWindow, &wp  );

  if (wp.showCmd != SW_SHOWMINIMIZED)
  {
    if (Abs ((int )wp.rcNormalPosition.left   - aXLeft  ) > 2) mask |= 1;
    if (Abs ((int )wp.rcNormalPosition.right  - aXRight ) > 2) mask |= 2;
    if (Abs ((int )wp.rcNormalPosition.top    - aYTop   ) > 2) mask |= 4;
    if (Abs ((int )wp.rcNormalPosition.bottom - aYBottom) > 2) mask |= 8;

    switch (mask)
    {
      case 0:
        mode = Aspect_TOR_NO_BORDER;
        break;
      case 1:
        mode = Aspect_TOR_LEFT_BORDER;
        break;
      case 2:
        mode = Aspect_TOR_RIGHT_BORDER;
        break;
      case 4:
        mode = Aspect_TOR_TOP_BORDER;
        break;
      case 5:
        mode = Aspect_TOR_LEFT_AND_TOP_BORDER;
        break;
      case 6:
        mode = Aspect_TOR_TOP_AND_RIGHT_BORDER;
        break;
      case 8:
        mode = Aspect_TOR_BOTTOM_BORDER;
        break;
      case 9:
        mode = Aspect_TOR_BOTTOM_AND_LEFT_BORDER;
        break;
      case 10:
        mode = Aspect_TOR_RIGHT_AND_BOTTOM_BORDER;
        break;
      default:
        break;
    }  // end switch

    aXLeft   = wp.rcNormalPosition.left;
    aXRight  = wp.rcNormalPosition.right;
    aYTop    = wp.rcNormalPosition.top;
    aYBottom = wp.rcNormalPosition.bottom;
  }

  return mode;
}

// =======================================================================
// function : Ratio
// purpose  :
// =======================================================================
Standard_Real WNT_Window::Ratio() const
{
  if (IsVirtual())
  {
    return Standard_Real(aXRight - aXLeft)/ Standard_Real(aYBottom - aYTop);
  }

  RECT aRect;
  GetClientRect ((HWND )myHWindow, &aRect);
  return Standard_Real(aRect.right - aRect.left) / Standard_Real(aRect.bottom - aRect.top);
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void WNT_Window::Position (Standard_Integer& theX1, Standard_Integer& theY1,
                           Standard_Integer& theX2, Standard_Integer& theY2) const
{
  if (IsVirtual())
  {
    theX1  = aXLeft;
    theX2  = aXRight;
    theY1  = aYTop;
    theY2  = aYBottom;
    return;
  }

  RECT  aRect;
  ::GetClientRect ((HWND )myHWindow, &aRect);

  POINT aPntLeft, aPntRight;
  aPntLeft.x = aPntLeft.y = 0;
  ::ClientToScreen ((HWND )myHWindow, &aPntLeft);
  aPntRight.x = aRect.right;
  aPntRight.y = aRect.bottom;
  ::ClientToScreen ((HWND )myHWindow, &aPntRight);

  if (myHParentWindow != NULL)
  {
    ::ScreenToClient ((HWND )myHParentWindow, &aPntLeft);
    ::ScreenToClient ((HWND )myHParentWindow, &aPntRight);
  }

  theX1 = aPntLeft.x;
  theX2 = aPntRight.x;
  theY1 = aPntLeft.y;
  theY2 = aPntRight.y;
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
void WNT_Window::Size (Standard_Integer& theWidth,
                       Standard_Integer& theHeight) const
{
  if (IsVirtual())
  {
    theWidth  = aXRight - aXLeft;
    theHeight = aYBottom - aYTop;
    return;
  }

  RECT aRect;
  ::GetClientRect ((HWND )myHWindow, &aRect);
  theWidth  = aRect.right;
  theHeight = aRect.bottom;
}

// =======================================================================
// function : SetPos
// purpose  :
// =======================================================================
void WNT_Window::SetPos (const Standard_Integer theX,  const Standard_Integer theY,
                         const Standard_Integer theX1, const Standard_Integer theY1)
{
  aXLeft   = theX;
  aYTop    = theY;
  aXRight  = theX1;
  aYBottom = theY1;
}

// =======================================================================
// function : SetTitle
// purpose  :
// =======================================================================
void WNT_Window::SetTitle (const TCollection_AsciiString& theTitle)
{
  const TCollection_ExtendedString aTitleW (theTitle);
  SetWindowTextW ((HWND )myHWindow, aTitleW.ToWideString());
}

// =======================================================================
// function : InvalidateContent
// purpose  :
// =======================================================================
void WNT_Window::InvalidateContent (const Handle(Aspect_DisplayConnection)& )
{
  if (myHWindow != NULL)
  {
    ::InvalidateRect ((HWND )myHWindow, NULL, TRUE);
  }
}

// =======================================================================
// function : VirtualKeyFromNative
// purpose  :
// =======================================================================
Aspect_VKey WNT_Window::VirtualKeyFromNative (Standard_Integer theKey)
{
  if (theKey >= Standard_Integer('0')
   && theKey <= Standard_Integer('9'))
  {
    return Aspect_VKey((theKey - Standard_Integer('0')) + Aspect_VKey_0);
  }
  if (theKey >= Standard_Integer('A')
   && theKey <= Standard_Integer('Z'))
  {
    // main latin alphabet keys
    return Aspect_VKey((theKey - Standard_Integer('A')) + Aspect_VKey_A);
  }
  if (theKey >= VK_F1
   && theKey <= VK_F24)
  {
    // special keys
    if (theKey <= VK_F12)
    {
      return Aspect_VKey((theKey - VK_F1) + Aspect_VKey_F1);
    }
    return Aspect_VKey_UNKNOWN;
  }
  if (theKey >= VK_NUMPAD0
   && theKey <= VK_NUMPAD9)
  {
    // numpad keys
    return Aspect_VKey((theKey - VK_NUMPAD0) + Aspect_VKey_Numpad0);
  }

  switch (theKey)
  {
    case VK_LBUTTON:
    case VK_RBUTTON:
    case VK_CANCEL:
    case VK_MBUTTON:
    case VK_XBUTTON1:
    case VK_XBUTTON2:
      return Aspect_VKey_UNKNOWN;
    case VK_BACK:
      return Aspect_VKey_Backspace;
    case VK_TAB:
      return Aspect_VKey_Tab;
    case VK_CLEAR:
      return Aspect_VKey_UNKNOWN;
    case VK_RETURN:
      return Aspect_VKey_Enter;
    case VK_SHIFT:
      return Aspect_VKey_Shift;
    case VK_CONTROL:
      return Aspect_VKey_Control;
    case VK_MENU:
      return Aspect_VKey_Alt; //Aspect_VKey_Menu;
    case VK_PAUSE:
    case VK_CAPITAL:
      return Aspect_VKey_UNKNOWN;
    case VK_ESCAPE:
      return Aspect_VKey_Escape;
    case VK_CONVERT:
    case VK_NONCONVERT:
    case VK_ACCEPT:
    case VK_MODECHANGE:
      return Aspect_VKey_UNKNOWN;
    case VK_SPACE:
      return Aspect_VKey_Space;
    case VK_PRIOR:
      return Aspect_VKey_PageUp;
    case VK_NEXT:
      return Aspect_VKey_PageDown;
    case VK_END:
      return Aspect_VKey_End;
    case VK_HOME:
      return Aspect_VKey_Home;
    case VK_LEFT:
      return Aspect_VKey_Left;
    case VK_UP:
      return Aspect_VKey_Up;
    case VK_DOWN:
      return Aspect_VKey_Down;
    case VK_RIGHT:
      return Aspect_VKey_Right;
    case VK_SELECT:
    case VK_PRINT:
    case VK_EXECUTE:
    case VK_SNAPSHOT:
      return Aspect_VKey_UNKNOWN;
    case VK_INSERT:
      return Aspect_VKey_UNKNOWN; // Aspect_VKey_Insert
    case VK_DELETE:
      return Aspect_VKey_Delete;
    case VK_HELP:
    case VK_LWIN:
    case VK_RWIN:
    case VK_APPS:
    case VK_SLEEP:
      return Aspect_VKey_UNKNOWN;
    case VK_MULTIPLY:
      return Aspect_VKey_NumpadMultiply;
    case VK_ADD:
      return Aspect_VKey_NumpadAdd;
    case VK_SEPARATOR:
    case VK_DECIMAL:
      return Aspect_VKey_UNKNOWN;
    case VK_SUBTRACT:
      return Aspect_VKey_NumpadSubtract;
    case VK_DIVIDE:
      return Aspect_VKey_NumpadDivide;
    case VK_NUMLOCK:
      return Aspect_VKey_Numlock;
    case VK_SCROLL:
      return Aspect_VKey_Scroll;
    case VK_LSHIFT:
    case VK_RSHIFT:
    case VK_LCONTROL:
    case VK_RCONTROL:
    case VK_LMENU:
    case VK_RMENU:
      return Aspect_VKey_UNKNOWN;
    case VK_BROWSER_BACK:
      return Aspect_VKey_BrowserBack;
    case VK_BROWSER_FORWARD:
      return Aspect_VKey_BrowserForward;
    case VK_BROWSER_REFRESH:
      return Aspect_VKey_BrowserRefresh;
    case VK_BROWSER_STOP:
      return Aspect_VKey_BrowserStop;
    case VK_BROWSER_SEARCH:
      return Aspect_VKey_BrowserSearch;
    case VK_BROWSER_FAVORITES:
      return Aspect_VKey_BrowserFavorites;
    case VK_BROWSER_HOME:
      return Aspect_VKey_BrowserHome;
    case VK_VOLUME_MUTE:
      return Aspect_VKey_VolumeMute;
    case VK_VOLUME_DOWN:
      return Aspect_VKey_VolumeDown;
    case VK_VOLUME_UP:
      return Aspect_VKey_VolumeUp;
    case VK_MEDIA_NEXT_TRACK:
      return Aspect_VKey_MediaNextTrack;
    case VK_MEDIA_PREV_TRACK:
      return Aspect_VKey_MediaPreviousTrack;
    case VK_MEDIA_STOP:
      return Aspect_VKey_MediaStop;
    case VK_MEDIA_PLAY_PAUSE:
      return Aspect_VKey_MediaPlayPause;
    case VK_OEM_1:
      return Aspect_VKey_Semicolon;
    case VK_OEM_PLUS:
      return Aspect_VKey_Plus;
    case VK_OEM_COMMA:
      return Aspect_VKey_Comma;
    case VK_OEM_MINUS:
      return Aspect_VKey_Minus;
    case VK_OEM_PERIOD:
      return Aspect_VKey_Period;
    case VK_OEM_2:
      return Aspect_VKey_Slash;
    case VK_OEM_3:
      return Aspect_VKey_Tilde;
    case VK_OEM_4:
      return Aspect_VKey_BracketLeft;
    case VK_OEM_5:
      return Aspect_VKey_Backslash;
    case VK_OEM_6:
      return Aspect_VKey_BracketRight;
    case VK_OEM_7:
      return Aspect_VKey_Apostrophe;
  }
  return Aspect_VKey_UNKNOWN;
}

// =======================================================================
// function : MouseKeyFlagsFromEvent
// purpose  :
// =======================================================================
Aspect_VKeyFlags WNT_Window::MouseKeyFlagsFromEvent (WPARAM theKeys)
{
  Aspect_VKeyFlags aFlags = Aspect_VKeyFlags_NONE;
  if ((theKeys & MK_CONTROL) != 0)
  {
    aFlags |= Aspect_VKeyFlags_CTRL;
  }
  if ((theKeys & MK_SHIFT) != 0)
  {
    aFlags |= Aspect_VKeyFlags_SHIFT;
  }
  if (GetKeyState (VK_MENU) < 0)
  {
    aFlags |= Aspect_VKeyFlags_ALT;
  }
  return aFlags;
}

// =======================================================================
// function : MouseKeyFlagsAsync
// purpose  :
// =======================================================================
Aspect_VKeyFlags WNT_Window::MouseKeyFlagsAsync()
{
  Aspect_VKeyFlags aFlags = Aspect_VKeyFlags_NONE;
  if ((GetAsyncKeyState (VK_CONTROL) & 0x8000) != 0)
  {
    aFlags |= Aspect_VKeyFlags_CTRL;
  }
  if ((GetAsyncKeyState (VK_SHIFT) & 0x8000) != 0)
  {
    aFlags |= Aspect_VKeyFlags_SHIFT;
  }
  if ((GetAsyncKeyState (VK_MENU) & 0x8000) != 0)
  {
    aFlags |= Aspect_VKeyFlags_ALT;
  }
  return aFlags;
}

// =======================================================================
// function : MouseButtonsFromEvent
// purpose  :
// =======================================================================
Aspect_VKeyMouse WNT_Window::MouseButtonsFromEvent (WPARAM theKeys)
{
  Aspect_VKeyMouse aButtons = Aspect_VKeyMouse_NONE;
  if ((theKeys & MK_LBUTTON) != 0)
  {
    aButtons |= Aspect_VKeyMouse_LeftButton;
  }
  if ((theKeys & MK_MBUTTON) != 0)
  {
    aButtons |= Aspect_VKeyMouse_MiddleButton;
  }
  if ((theKeys & MK_RBUTTON) != 0)
  {
    aButtons |= Aspect_VKeyMouse_RightButton;
  }
  return aButtons;
}

// =======================================================================
// function : MouseButtonsAsync
// purpose  :
// =======================================================================
Aspect_VKeyMouse WNT_Window::MouseButtonsAsync()
{
  Aspect_VKeyMouse aButtons = Aspect_VKeyMouse_NONE;
  const bool isSwapped = GetSystemMetrics (SM_SWAPBUTTON) != 0;

  if ((GetAsyncKeyState (!isSwapped ? VK_LBUTTON : VK_RBUTTON) & 0x8000) != 0)
  {
    aButtons |= Aspect_VKeyMouse_LeftButton;
  }
  if ((GetAsyncKeyState (VK_MBUTTON) & 0x8000) != 0)
  {
    aButtons |= Aspect_VKeyMouse_MiddleButton;
  }
  if ((GetAsyncKeyState (!isSwapped ? VK_RBUTTON : VK_LBUTTON) & 0x8000) != 0)
  {
    aButtons |= Aspect_VKeyMouse_RightButton;
  }
  return aButtons;
}

// =======================================================================
// function : RegisterRawInputDevices
// purpose  :
// =======================================================================
int WNT_Window::RegisterRawInputDevices (unsigned int theRawDeviceMask)
{
  if (IsVirtual()
   || myHWindow == NULL)
  {
    return 0;
  }

  // hidusage.h
  enum HidUsagePage { THE_HID_USAGE_PAGE_GENERIC = 0x01 }; // HID_USAGE_PAGE_GENERIC
  enum HidUsage
  {
    THE_HID_USAGE_GENERIC_MOUSE                 = 0x02, // HID_USAGE_GENERIC_MOUSE
    THE_HID_USAGE_GENERIC_MULTI_AXIS_CONTROLLER = 0x08, // HID_USAGE_GENERIC_MULTI_AXIS_CONTROLLER
  };

  int aNbDevices = 0;
  RAWINPUTDEVICE aRawInDevList[2];
  if ((theRawDeviceMask & RawInputMask_Mouse) != 0)
  {
    // mouse
    RAWINPUTDEVICE& aRawMouse = aRawInDevList[aNbDevices++];
    aRawMouse.usUsagePage = THE_HID_USAGE_PAGE_GENERIC;
    aRawMouse.usUsage     = THE_HID_USAGE_GENERIC_MOUSE;
    aRawMouse.dwFlags     = RIDEV_INPUTSINK;
    aRawMouse.hwndTarget  = (HWND )myHWindow;
  }
  if ((theRawDeviceMask & RawInputMask_SpaceMouse) != 0)
  {
    // space mouse
    RAWINPUTDEVICE& aRawSpace = aRawInDevList[aNbDevices++];
    aRawSpace.usUsagePage = THE_HID_USAGE_PAGE_GENERIC;
    aRawSpace.usUsage     = THE_HID_USAGE_GENERIC_MULTI_AXIS_CONTROLLER;
    aRawSpace.dwFlags     = 0; // RIDEV_DEVNOTIFY
    aRawSpace.hwndTarget  = (HWND )myHWindow;
  }

  for (int aTryIter = aNbDevices; aTryIter > 0; --aTryIter)
  {
    if (::RegisterRawInputDevices (aRawInDevList, aTryIter, sizeof(aRawInDevList[0])))
    {
      return aTryIter;
    }

    Message::SendTrace (aRawInDevList[aTryIter - 1].usUsage == THE_HID_USAGE_GENERIC_MULTI_AXIS_CONTROLLER
                      ? "Warning: RegisterRawInputDevices() failed to register RAW multi-axis controller input"
                      : "Warning: RegisterRawInputDevices() failed to register RAW mouse input");
  }
  return 0;
}

#endif // _WIN32
