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

  #include <Aspect_ScrollDelta.hxx>
  #include <Aspect_WindowDefinitionError.hxx>
  #include <Aspect_WindowInputListener.hxx>
  #include <Message.hxx>
  #include <NCollection_LocalArray.hxx>
  #include <TCollection_ExtendedString.hxx>
  #include <WNT_WClass.hxx>
  #include <WNT_HIDSpaceMouse.hxx>

IMPLEMENT_STANDARD_RTTIEXT(WNT_Window, Aspect_Window)

  #ifndef MOUSEEVENTF_FROMTOUCH
    #define MOUSEEVENTF_FROMTOUCH 0xFF515700
  #endif

//! Auxiliary tool for handling WM_TOUCH events.
//! Dynamically loads functions from User32 available since Win7 and later.
class WNT_Window::TouchInputHelper : public Standard_Transient
{
public:
  typedef BOOL(WINAPI* RegisterTouchWindow_t)(HWND hwnd, ULONG ulFlags);
  typedef BOOL(WINAPI* UnregisterTouchWindow_t)(HWND hwnd);
  typedef BOOL(WINAPI* GetTouchInputInfo_t)(HTOUCHINPUT hTouchInput,
                                            UINT        cInputs,
                                            PTOUCHINPUT pInputs,
                                            int         cbSize);
  typedef BOOL(WINAPI* CloseTouchInputHandle_t)(HTOUCHINPUT hTouchInput);

  typedef NCollection_LocalArray<TOUCHINPUT, 16> InnerTouchArray;

public:
  //! Main constructor.
  TouchInputHelper()
      : myRegisterTouchWindow(NULL),
        myUnregisterTouchWindow(NULL),
        myGetTouchInputInfo(NULL),
        myCloseTouchInputHandle(NULL),
        myIsRegistered(false)
  {
    HMODULE aUser32Module = GetModuleHandleW(L"User32");
    if (aUser32Module != NULL)
    {
      // User32 should be already loaded
      myRegisterTouchWindow =
        (RegisterTouchWindow_t)GetProcAddress(aUser32Module, "RegisterTouchWindow");
      myUnregisterTouchWindow =
        (UnregisterTouchWindow_t)GetProcAddress(aUser32Module, "UnregisterTouchWindow");
      myGetTouchInputInfo = (GetTouchInputInfo_t)GetProcAddress(aUser32Module, "GetTouchInputInfo");
      myCloseTouchInputHandle =
        (CloseTouchInputHandle_t)GetProcAddress(aUser32Module, "CloseTouchInputHandle");
    }
  }

  //! Return TRUE if window has been registered.
  bool IsRegistered() const { return myIsRegistered; }

  //! Register window to receive WM_TOUCH events.
  bool Register(HWND theWin)
  {
    if (myRegisterTouchWindow == NULL)
    {
      return false;
    }

    if (myRegisterTouchWindow(theWin, TWF_FINETOUCH))
    {
      myIsRegistered = true;
      return true;
    }
    // Message::SendTrace() << "RegisterTouchWindow() FAILED";
    return false;
  }

  //! Array of touches retrieved from HTOUCHINPUT.
  class TouchInputInfo : public InnerTouchArray
  {
  public:
    //! Main constructor.
    TouchInputInfo(const TouchInputHelper& theHelper, const MSG& theMsg)
        : InnerTouchArray(0),
          myHelper(&theHelper),
          myHInput((HTOUCHINPUT)theMsg.lParam)
    {
      const int aNbTouches = LOWORD(theMsg.wParam);
      if (aNbTouches > 0 && theHelper.myGetTouchInputInfo != NULL)
      {
        InnerTouchArray::Allocate(aNbTouches);
        TOUCHINPUT* aTouches = InnerTouchArray::operator TOUCHINPUT*();
        if (!theHelper.myGetTouchInputInfo(myHInput, aNbTouches, aTouches, sizeof(TOUCHINPUT)))
        {
          InnerTouchArray::Deallocate();
        }
      }
    }

    //! Destructor.
    ~TouchInputInfo()
    {
      if (myHelper->myCloseTouchInputHandle != NULL)
      {
        myHelper->myCloseTouchInputHandle(myHInput);
      }
    }

  private:
    const TouchInputHelper* myHelper;
    HTOUCHINPUT             myHInput;
  };

private:
  RegisterTouchWindow_t   myRegisterTouchWindow;
  UnregisterTouchWindow_t myUnregisterTouchWindow;
  GetTouchInputInfo_t     myGetTouchInputInfo;
  CloseTouchInputHandle_t myCloseTouchInputHandle;
  bool                    myIsRegistered;
};

// =======================================================================
// function : WNT_Window
// purpose  :
// =======================================================================
WNT_Window::WNT_Window(const Standard_CString     theTitle,
                       const Handle(WNT_WClass)&  theClass,
                       const WNT_Dword&           theStyle,
                       const Standard_Integer     thePxLeft,
                       const Standard_Integer     thePxTop,
                       const Standard_Integer     thePxWidth,
                       const Standard_Integer     thePxHeight,
                       const Quantity_NameOfColor theBackColor,
                       const Aspect_Handle        theParent,
                       const Aspect_Handle        theMenu,
                       const Standard_Address     theClientStruct)
    : Aspect_Window(),
      myWClass(theClass),
      myHWindow(NULL),
      myHParentWindow(NULL),
      myXLeft(thePxLeft),
      myYTop(thePxTop),
      myXRight(thePxLeft + thePxWidth),
      myYBottom(thePxTop + thePxHeight),
      myIsForeign(Standard_False)
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
  aRect.top    = myYTop;
  aRect.bottom = myYBottom;
  aRect.left   = myXLeft;
  aRect.right  = myXRight;
  AdjustWindowRect(&aRect, aStyle, theMenu != NULL ? TRUE : FALSE);
  myXLeft   = aRect.left;
  myYTop    = aRect.top;
  myXRight  = aRect.right;
  myYBottom = aRect.bottom;

  const TCollection_ExtendedString aTitleW(theTitle);
  const TCollection_ExtendedString aClassNameW(myWClass->Name());
  myHWindow = CreateWindowW(aClassNameW.ToWideString(),
                            aTitleW.ToWideString(),
                            aStyle,
                            myXLeft,
                            myYTop,
                            (myXRight - myXLeft),
                            (myYBottom - myYTop),
                            (HWND)theParent,
                            (HMENU)theMenu,
                            (HINSTANCE)myWClass->Instance(),
                            theClientStruct);
  if (!myHWindow)
  {
    throw Aspect_WindowDefinitionError("Unable to create window");
  }

  myHParentWindow = theParent;
  SetBackground(theBackColor);
}

// =======================================================================
// function : WNT_Window
// purpose  :
// =======================================================================
WNT_Window::WNT_Window(const Aspect_Handle theHandle, const Quantity_NameOfColor theBackColor)
    : myHWindow(theHandle),
      myHParentWindow(GetParent((HWND)theHandle)),
      myXLeft(0),
      myYTop(0),
      myXRight(0),
      myYBottom(0),
      myIsForeign(Standard_True)
{
  SetBackground(theBackColor);

  WINDOWPLACEMENT aPlace = {};
  aPlace.length          = sizeof(WINDOWPLACEMENT);
  ::GetWindowPlacement((HWND)myHWindow, &aPlace);

  myXLeft   = aPlace.rcNormalPosition.left;
  myYTop    = aPlace.rcNormalPosition.top;
  myXRight  = aPlace.rcNormalPosition.right;
  myYBottom = aPlace.rcNormalPosition.bottom;
}

// =======================================================================
// function : ~WNT_Window
// purpose  :
// =======================================================================
WNT_Window::~WNT_Window()
{
  if (myHWindow == NULL || myIsForeign)
  {
    return;
  }

  DestroyWindow((HWND)myHWindow);
  myIsForeign = Standard_False;
}

// =======================================================================
// function : SetCursor
// purpose  :
// =======================================================================
void WNT_Window::SetCursor(const Aspect_Handle theCursor) const
{
  ::SetClassLongPtrW((HWND)myHWindow, GCLP_HCURSOR, (LONG_PTR)theCursor);
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

  WINDOWPLACEMENT aPlace = {};
  aPlace.length          = sizeof(WINDOWPLACEMENT);
  ::GetWindowPlacement((HWND)myHWindow, &aPlace);
  return !(aPlace.showCmd == SW_HIDE || aPlace.showCmd == SW_MINIMIZE);
}

// =======================================================================
// function : Map
// purpose  :
// =======================================================================
void WNT_Window::Map() const
{
  if (!IsVirtual())
  {
    Map(SW_SHOW);
  }
}

// =======================================================================
// function : Map
// purpose  :
// =======================================================================
void WNT_Window::Map(const Standard_Integer theMapMode) const
{
  if (IsVirtual())
  {
    return;
  }

  ::ShowWindow((HWND)myHWindow, theMapMode);
  ::UpdateWindow((HWND)myHWindow);
}

// =======================================================================
// function : Unmap
// purpose  :
// =======================================================================
void WNT_Window::Unmap() const
{
  Map(SW_HIDE);
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

  WINDOWPLACEMENT aPlace = {};
  aPlace.length          = sizeof(WINDOWPLACEMENT);
  GetWindowPlacement((HWND)myHWindow, &aPlace);
  if (aPlace.showCmd == SW_SHOWMINIMIZED)
  {
    return Aspect_TOR_UNKNOWN;
  }

  int aMask = 0;
  if (Abs((int)aPlace.rcNormalPosition.left - myXLeft) > 2)
  {
    aMask |= 1;
  }
  if (Abs((int)aPlace.rcNormalPosition.right - myXRight) > 2)
  {
    aMask |= 2;
  }
  if (Abs((int)aPlace.rcNormalPosition.top - myYTop) > 2)
  {
    aMask |= 4;
  }
  if (Abs((int)aPlace.rcNormalPosition.bottom - myYBottom) > 2)
  {
    aMask |= 8;
  }

  myXLeft   = aPlace.rcNormalPosition.left;
  myXRight  = aPlace.rcNormalPosition.right;
  myYTop    = aPlace.rcNormalPosition.top;
  myYBottom = aPlace.rcNormalPosition.bottom;
  switch (aMask)
  {
    case 0:
      return Aspect_TOR_NO_BORDER;
    case 1:
      return Aspect_TOR_LEFT_BORDER;
    case 2:
      return Aspect_TOR_RIGHT_BORDER;
    case 4:
      return Aspect_TOR_TOP_BORDER;
    case 5:
      return Aspect_TOR_LEFT_AND_TOP_BORDER;
    case 6:
      return Aspect_TOR_TOP_AND_RIGHT_BORDER;
    case 8:
      return Aspect_TOR_BOTTOM_BORDER;
    case 9:
      return Aspect_TOR_BOTTOM_AND_LEFT_BORDER;
    case 10:
      return Aspect_TOR_RIGHT_AND_BOTTOM_BORDER;
  }
  return Aspect_TOR_UNKNOWN;
}

// =======================================================================
// function : Ratio
// purpose  :
// =======================================================================
Standard_Real WNT_Window::Ratio() const
{
  if (IsVirtual())
  {
    return Standard_Real(myXRight - myXLeft) / Standard_Real(myYBottom - myYTop);
  }

  RECT aRect = {};
  GetClientRect((HWND)myHWindow, &aRect);
  return Standard_Real(aRect.right - aRect.left) / Standard_Real(aRect.bottom - aRect.top);
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void WNT_Window::Position(Standard_Integer& theX1,
                          Standard_Integer& theY1,
                          Standard_Integer& theX2,
                          Standard_Integer& theY2) const
{
  if (IsVirtual())
  {
    theX1 = myXLeft;
    theX2 = myXRight;
    theY1 = myYTop;
    theY2 = myYBottom;
    return;
  }

  RECT aRect = {};
  ::GetClientRect((HWND)myHWindow, &aRect);

  POINT aPntLeft, aPntRight;
  aPntLeft.x = aPntLeft.y = 0;
  ::ClientToScreen((HWND)myHWindow, &aPntLeft);
  aPntRight.x = aRect.right;
  aPntRight.y = aRect.bottom;
  ::ClientToScreen((HWND)myHWindow, &aPntRight);

  if (myHParentWindow != NULL)
  {
    ::ScreenToClient((HWND)myHParentWindow, &aPntLeft);
    ::ScreenToClient((HWND)myHParentWindow, &aPntRight);
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
void WNT_Window::Size(Standard_Integer& theWidth, Standard_Integer& theHeight) const
{
  if (IsVirtual())
  {
    theWidth  = myXRight - myXLeft;
    theHeight = myYBottom - myYTop;
    return;
  }

  RECT aRect = {};
  ::GetClientRect((HWND)myHWindow, &aRect);
  theWidth  = aRect.right;
  theHeight = aRect.bottom;
}

// =======================================================================
// function : SetPos
// purpose  :
// =======================================================================
void WNT_Window::SetPos(const Standard_Integer theX,
                        const Standard_Integer theY,
                        const Standard_Integer theX1,
                        const Standard_Integer theY1)
{
  myXLeft   = theX;
  myYTop    = theY;
  myXRight  = theX1;
  myYBottom = theY1;
}

// =======================================================================
// function : SetTitle
// purpose  :
// =======================================================================
void WNT_Window::SetTitle(const TCollection_AsciiString& theTitle)
{
  const TCollection_ExtendedString aTitleW(theTitle);
  SetWindowTextW((HWND)myHWindow, aTitleW.ToWideString());
}

// =======================================================================
// function : InvalidateContent
// purpose  :
// =======================================================================
void WNT_Window::InvalidateContent(const Handle(Aspect_DisplayConnection)&)
{
  if (myHWindow != NULL)
  {
    ::InvalidateRect((HWND)myHWindow, NULL, TRUE);
  }
}

// =======================================================================
// function : VirtualKeyFromNative
// purpose  :
// =======================================================================
Aspect_VKey WNT_Window::VirtualKeyFromNative(Standard_Integer theKey)
{
  if (theKey >= Standard_Integer('0') && theKey <= Standard_Integer('9'))
  {
    return Aspect_VKey((theKey - Standard_Integer('0')) + Aspect_VKey_0);
  }
  if (theKey >= Standard_Integer('A') && theKey <= Standard_Integer('Z'))
  {
    // main latin alphabet keys
    return Aspect_VKey((theKey - Standard_Integer('A')) + Aspect_VKey_A);
  }
  if (theKey >= VK_F1 && theKey <= VK_F24)
  {
    // special keys
    if (theKey <= VK_F12)
    {
      return Aspect_VKey((theKey - VK_F1) + Aspect_VKey_F1);
    }
    return Aspect_VKey_UNKNOWN;
  }
  if (theKey >= VK_NUMPAD0 && theKey <= VK_NUMPAD9)
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
      return Aspect_VKey_Alt; // Aspect_VKey_Menu;
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
Aspect_VKeyFlags WNT_Window::MouseKeyFlagsFromEvent(WPARAM theKeys)
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
  if (GetKeyState(VK_MENU) < 0)
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
  if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)
  {
    aFlags |= Aspect_VKeyFlags_CTRL;
  }
  if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0)
  {
    aFlags |= Aspect_VKeyFlags_SHIFT;
  }
  if ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0)
  {
    aFlags |= Aspect_VKeyFlags_ALT;
  }
  return aFlags;
}

// =======================================================================
// function : MouseButtonsFromEvent
// purpose  :
// =======================================================================
Aspect_VKeyMouse WNT_Window::MouseButtonsFromEvent(WPARAM theKeys)
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
  Aspect_VKeyMouse aButtons  = Aspect_VKeyMouse_NONE;
  const bool       isSwapped = GetSystemMetrics(SM_SWAPBUTTON) != 0;

  if ((GetAsyncKeyState(!isSwapped ? VK_LBUTTON : VK_RBUTTON) & 0x8000) != 0)
  {
    aButtons |= Aspect_VKeyMouse_LeftButton;
  }
  if ((GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0)
  {
    aButtons |= Aspect_VKeyMouse_MiddleButton;
  }
  if ((GetAsyncKeyState(!isSwapped ? VK_RBUTTON : VK_LBUTTON) & 0x8000) != 0)
  {
    aButtons |= Aspect_VKeyMouse_RightButton;
  }
  return aButtons;
}

// =======================================================================
// function : RegisterRawInputDevices
// purpose  :
// =======================================================================
int WNT_Window::RegisterRawInputDevices(unsigned int theRawDeviceMask)
{
  if (IsVirtual() || myHWindow == NULL)
  {
    return 0;
  }

  // hidusage.h
  enum HidUsagePage
  {
    THE_HID_USAGE_PAGE_GENERIC = 0x01
  }; // HID_USAGE_PAGE_GENERIC

  enum HidUsage
  {
    THE_HID_USAGE_GENERIC_MOUSE                 = 0x02, // HID_USAGE_GENERIC_MOUSE
    THE_HID_USAGE_GENERIC_MULTI_AXIS_CONTROLLER = 0x08, // HID_USAGE_GENERIC_MULTI_AXIS_CONTROLLER
  };

  int            aNbDevices = 0;
  RAWINPUTDEVICE aRawInDevList[2];
  if ((theRawDeviceMask & RawInputMask_Mouse) != 0)
  {
    // mouse
    RAWINPUTDEVICE& aRawMouse = aRawInDevList[aNbDevices++];
    aRawMouse.usUsagePage     = THE_HID_USAGE_PAGE_GENERIC;
    aRawMouse.usUsage         = THE_HID_USAGE_GENERIC_MOUSE;
    aRawMouse.dwFlags         = RIDEV_INPUTSINK;
    aRawMouse.hwndTarget      = (HWND)myHWindow;
  }
  if ((theRawDeviceMask & RawInputMask_SpaceMouse) != 0)
  {
    // space mouse
    RAWINPUTDEVICE& aRawSpace = aRawInDevList[aNbDevices++];
    aRawSpace.usUsagePage     = THE_HID_USAGE_PAGE_GENERIC;
    aRawSpace.usUsage         = THE_HID_USAGE_GENERIC_MULTI_AXIS_CONTROLLER;
    aRawSpace.dwFlags         = 0; // RIDEV_DEVNOTIFY
    aRawSpace.hwndTarget      = (HWND)myHWindow;
  }

  for (int aTryIter = aNbDevices; aTryIter > 0; --aTryIter)
  {
    if (::RegisterRawInputDevices(aRawInDevList, aTryIter, sizeof(aRawInDevList[0])))
    {
      return aTryIter;
    }

    Message::SendTrace(
      aRawInDevList[aTryIter - 1].usUsage == THE_HID_USAGE_GENERIC_MULTI_AXIS_CONTROLLER
        ? "Warning: RegisterRawInputDevices() failed to register RAW multi-axis controller input"
        : "Warning: RegisterRawInputDevices() failed to register RAW mouse input");
  }
  return 0;
}

// =======================================================================
// function : ProcessMessage
// purpose  :
// =======================================================================
bool WNT_Window::ProcessMessage(Aspect_WindowInputListener& theListener, MSG& theMsg)
{
  if (myTouchInputHelper.IsNull())
  {
    myTouchInputHelper = new TouchInputHelper();
    myTouchInputHelper->Register((HWND)myHWindow);
  }

  switch (theMsg.message)
  {
    case WM_CLOSE: {
      if (theMsg.hwnd == (HWND)myHWindow)
      {
        theListener.ProcessClose();
        return true;
      }
      return false;
    }
    case WM_SETFOCUS:
    case WM_KILLFOCUS: {
      if (theMsg.hwnd == (HWND)myHWindow)
      {
        theListener.ProcessFocus(theMsg.message == WM_SETFOCUS);
        return true;
      }
      return false;
    }
    case WM_PAINT: {
      PAINTSTRUCT aPaint;
      BeginPaint(theMsg.hwnd, &aPaint);
      EndPaint(theMsg.hwnd, &aPaint);
      theListener.ProcessExpose();
      return true;
    }
    case WM_SIZE:
    case WM_MOVE:
    case WM_MOVING:
    case WM_SIZING: {
      theListener.ProcessConfigure(theMsg.message == WM_SIZE);
      return true;
    }
    case WM_KEYUP:
    case WM_KEYDOWN: {
      const Aspect_VKey aVKey = WNT_Window::VirtualKeyFromNative((Standard_Integer)theMsg.wParam);
      if (aVKey != Aspect_VKey_UNKNOWN)
      {
        const double aTimeStamp = theListener.EventTime();
        if (theMsg.message == WM_KEYDOWN)
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
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN: {
      const LPARAM anExtraInfo = GetMessageExtraInfo();
      bool         isEmulated  = false;
      if ((anExtraInfo & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH)
      {
        isEmulated = true;
        if (!myTouchInputHelper.IsNull() && myTouchInputHelper->IsRegistered())
        {
          // Message::SendTrace ("Skipping mouse message emulated from touches...");
          break;
        }
      }

      const Graphic3d_Vec2i  aPos(LOWORD(theMsg.lParam), HIWORD(theMsg.lParam));
      const Aspect_VKeyFlags aFlags  = WNT_Window::MouseKeyFlagsFromEvent(theMsg.wParam);
      Aspect_VKeyMouse       aButton = Aspect_VKeyMouse_NONE;
      switch (theMsg.message)
      {
        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN:
          aButton = Aspect_VKeyMouse_LeftButton;
          break;
        case WM_MBUTTONUP:
        case WM_MBUTTONDOWN:
          aButton = Aspect_VKeyMouse_MiddleButton;
          break;
        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
          aButton = Aspect_VKeyMouse_RightButton;
          break;
      }
      if (theMsg.message == WM_LBUTTONDOWN || theMsg.message == WM_MBUTTONDOWN
          || theMsg.message == WM_RBUTTONDOWN)
      {
        SetFocus(theMsg.hwnd);
        SetCapture(theMsg.hwnd);
        theListener.PressMouseButton(aPos, aButton, aFlags, isEmulated);
      }
      else
      {
        ReleaseCapture();
        theListener.ReleaseMouseButton(aPos, aButton, aFlags, isEmulated);
      }
      theListener.ProcessInput();
      return true;
    }
    case WM_MOUSEWHEEL: {
      const int              aDelta  = GET_WHEEL_DELTA_WPARAM(theMsg.wParam);
      const Standard_Real    aDeltaF = Standard_Real(aDelta) / Standard_Real(WHEEL_DELTA);
      const Aspect_VKeyFlags aFlags  = WNT_Window::MouseKeyFlagsFromEvent(theMsg.wParam);
      Graphic3d_Vec2i aPos(int(short(LOWORD(theMsg.lParam))), int(short(HIWORD(theMsg.lParam))));
      POINT           aCursorPnt = {aPos.x(), aPos.y()};
      if (ScreenToClient(theMsg.hwnd, &aCursorPnt))
      {
        aPos.SetValues(aCursorPnt.x, aCursorPnt.y);
      }

      if (theMsg.hwnd != (HWND)myHWindow)
      {
        return false;
      }

      theListener.UpdateMouseScroll(Aspect_ScrollDelta(aPos, aDeltaF, aFlags));
      theListener.ProcessInput();
      return true;
    }
    case WM_MOUSEMOVE: {
      Graphic3d_Vec2i  aPos(LOWORD(theMsg.lParam), HIWORD(theMsg.lParam));
      Aspect_VKeyMouse aButtons = WNT_Window::MouseButtonsFromEvent(theMsg.wParam);
      Aspect_VKeyFlags aFlags   = WNT_Window::MouseKeyFlagsFromEvent(theMsg.wParam);

      // don't make a slide-show from input events - fetch the actual mouse cursor position
      CURSORINFO aCursor;
      aCursor.cbSize = sizeof(aCursor);
      if (::GetCursorInfo(&aCursor) != FALSE)
      {
        POINT aCursorPnt = {aCursor.ptScreenPos.x, aCursor.ptScreenPos.y};
        if (ScreenToClient(theMsg.hwnd, &aCursorPnt))
        {
          // as we override mouse position, we need overriding also mouse state
          aPos.SetValues(aCursorPnt.x, aCursorPnt.y);
          aButtons = WNT_Window::MouseButtonsAsync();
          aFlags   = WNT_Window::MouseKeyFlagsAsync();
        }
      }

      if (theMsg.hwnd != (HWND)myHWindow)
      {
        // mouse move events come also for inactive windows
        return false;
      }

      theListener.UpdateMousePosition(aPos, aButtons, aFlags, false);
      theListener.ProcessInput();
      return true;
    }
    case WM_INPUT: {
      UINT aSize = 0;
      ::GetRawInputData((HRAWINPUT)theMsg.lParam, RID_INPUT, NULL, &aSize, sizeof(RAWINPUTHEADER));
      NCollection_LocalArray<BYTE> aRawData(aSize);
      if (aSize == 0
          || ::GetRawInputData((HRAWINPUT)theMsg.lParam,
                               RID_INPUT,
                               aRawData,
                               &aSize,
                               sizeof(RAWINPUTHEADER))
               != aSize)
      {
        return true;
      }

      const RAWINPUT* aRawInput = (RAWINPUT*)(BYTE*)aRawData;
      if (aRawInput->header.dwType != RIM_TYPEHID)
      {
        return true;
      }

      RID_DEVICE_INFO aDevInfo;
      aDevInfo.cbSize   = sizeof(RID_DEVICE_INFO);
      UINT aDevInfoSize = sizeof(RID_DEVICE_INFO);
      if (::GetRawInputDeviceInfoW(aRawInput->header.hDevice,
                                   RIDI_DEVICEINFO,
                                   &aDevInfo,
                                   &aDevInfoSize)
            != sizeof(RID_DEVICE_INFO)
          || (aDevInfo.hid.dwVendorId != WNT_HIDSpaceMouse::VENDOR_ID_LOGITECH
              && aDevInfo.hid.dwVendorId != WNT_HIDSpaceMouse::VENDOR_ID_3DCONNEXION))
      {
        return true;
      }

      WNT_HIDSpaceMouse aSpaceData(aDevInfo.hid.dwProductId,
                                   aRawInput->data.hid.bRawData,
                                   aRawInput->data.hid.dwSizeHid);
      if (theListener.Update3dMouse(aSpaceData))
      {
        InvalidateContent(Handle(Aspect_DisplayConnection)());
      }
      return true;
    }
    case WM_TOUCH: {
      if (theMsg.hwnd != (HWND)myHWindow || myTouchInputHelper.IsNull())
      {
        return false;
      }

      TouchInputHelper::TouchInputInfo aSrcTouches(*myTouchInputHelper, theMsg);
      if (aSrcTouches.Size() < 1)
      {
        break;
      }

      Graphic3d_Vec2i aWinTopLeft, aWinBotRight;
      Position(aWinTopLeft.x(), aWinTopLeft.y(), aWinBotRight.x(), aWinBotRight.y());

      bool hasUpdates = false;
      for (size_t aTouchIter = 0; aTouchIter < aSrcTouches.Size(); ++aTouchIter)
      {
        const TOUCHINPUT&   aTouchSrc = aSrcTouches[aTouchIter];
        const Standard_Size aTouchId  = (Standard_Size)aTouchSrc.dwID;
        // const Standard_Size aDeviceId = (Standard_Size )aTouchSrc.hSource;

        const Graphic3d_Vec2i aSize = aWinBotRight - aWinTopLeft;
        const Graphic3d_Vec2d aNewPos2d =
          Graphic3d_Vec2d(double(aTouchSrc.x), double(aTouchSrc.y)) * 0.01
          - Graphic3d_Vec2d(aWinTopLeft);
        const Graphic3d_Vec2i aNewPos2i = Graphic3d_Vec2i(aNewPos2d + Graphic3d_Vec2d(0.5));
        if ((aTouchSrc.dwFlags & TOUCHEVENTF_DOWN) == TOUCHEVENTF_DOWN)
        {
          if (aNewPos2i.x() >= 0 && aNewPos2i.x() < aSize.x() && aNewPos2i.y() >= 0
              && aNewPos2i.y() < aSize.y())
          {
            hasUpdates = true;
            theListener.AddTouchPoint(aTouchId, aNewPos2d);
          }
        }
        else if ((aTouchSrc.dwFlags & TOUCHEVENTF_MOVE) == TOUCHEVENTF_MOVE)
        {
          const int anOldIndex = theListener.TouchPoints().FindIndex(aTouchId);
          if (anOldIndex != 0)
          {
            hasUpdates = true;
            theListener.UpdateTouchPoint(aTouchId, aNewPos2d);
          }
        }
        else if ((aTouchSrc.dwFlags & TOUCHEVENTF_UP) == TOUCHEVENTF_UP)
        {
          if (theListener.RemoveTouchPoint(aTouchId))
          {
            hasUpdates = true;
          }
        }
      }

      if (hasUpdates)
      {
        InvalidateContent(Handle(Aspect_DisplayConnection)());
      }
      return true;
    }
  }
  return false;
}

#endif // _WIN32
