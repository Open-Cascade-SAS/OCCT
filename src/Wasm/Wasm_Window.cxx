// Created by: Kirill Gavrilov
// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <Wasm_Window.hxx>

#include <Aspect_ScrollDelta.hxx>
#include <Aspect_WindowInputListener.hxx>

#if defined(__EMSCRIPTEN__)
  #include <emscripten.h>
  #include <emscripten/html5.h>
  #include <emscripten/key_codes.h>
#endif

IMPLEMENT_STANDARD_RTTIEXT(Wasm_Window, Aspect_Window)

// =======================================================================
// function : Wasm_Window
// purpose  :
// =======================================================================
Wasm_Window::Wasm_Window (const TCollection_AsciiString& theCanvasId,
                          const bool theToScaleBacking)
: myCanvasId (theCanvasId),
  mySize (0),
  myDevicePixelRatio (1.0),
  myToScaleBacking (theToScaleBacking),
  myIsMapped (true)
{
#if defined(__EMSCRIPTEN__)
  myDevicePixelRatio = emscripten_get_device_pixel_ratio();
  emscripten_get_canvas_element_size (myCanvasId.ToCString(), &mySize.x(), &mySize.y());
  if (myToScaleBacking)
  {
    myDevicePixelRatio = emscripten_get_device_pixel_ratio();
    Graphic3d_Vec2d aCssSize;
    emscripten_get_element_css_size (myCanvasId.ToCString(), &aCssSize.x(), &aCssSize.y());
    Graphic3d_Vec2i aCanvasSize = Graphic3d_Vec2i (aCssSize * myDevicePixelRatio);
    if (aCanvasSize != mySize)
    {
      mySize = aCanvasSize;
      emscripten_set_canvas_element_size (myCanvasId.ToCString(), aCanvasSize.x(), aCanvasSize.y());
      emscripten_set_element_css_size    (myCanvasId.ToCString(), aCssSize.x(),    aCssSize.y());
    }
  }
#endif
}

// =======================================================================
// function : ~Wasm_Window
// purpose  :
// =======================================================================
Wasm_Window::~Wasm_Window()
{
  //
}

// =======================================================================
// function : DoResize
// purpose  :
// =======================================================================
Aspect_TypeOfResize Wasm_Window::DoResize()
{
  if (IsVirtual())
  {
    return Aspect_TOR_UNKNOWN;
  }

#if defined(__EMSCRIPTEN__)
  emscripten_get_canvas_element_size (myCanvasId.ToCString(), &mySize.x(), &mySize.y());
  if (myToScaleBacking)
  {
    myDevicePixelRatio = emscripten_get_device_pixel_ratio();
    Graphic3d_Vec2d aCssSize;
    emscripten_get_element_css_size (myCanvasId.ToCString(), &aCssSize.x(), &aCssSize.y());
    Graphic3d_Vec2i aCanvasSize = Graphic3d_Vec2i (aCssSize * myDevicePixelRatio);
    if (aCanvasSize != mySize)
    {
      mySize = aCanvasSize;
      emscripten_set_canvas_element_size (myCanvasId.ToCString(), aCanvasSize.x(), aCanvasSize.y());
      emscripten_set_element_css_size    (myCanvasId.ToCString(), aCssSize.x(),    aCssSize.y());
    }
  }
#endif
  return Aspect_TOR_UNKNOWN;
}

// =======================================================================
// function : Ratio
// purpose  :
// =======================================================================
Standard_Real Wasm_Window::Ratio() const
{
  Graphic3d_Vec2i aCanvasSize = mySize;
  if (!IsVirtual())
  {
  #if defined(__EMSCRIPTEN__)
    emscripten_get_canvas_element_size (myCanvasId.ToCString(), &aCanvasSize.x(), &aCanvasSize.y());
  #endif
  }

  return (aCanvasSize.x() != 0 && aCanvasSize.y() != 0)
        ? Standard_Real(aCanvasSize.x()) / Standard_Real(aCanvasSize.y())
        : 1.0;
}

// =======================================================================
// function : Position
// purpose  :
// =======================================================================
void Wasm_Window::Position (Standard_Integer& theX1, Standard_Integer& theY1,
                            Standard_Integer& theX2, Standard_Integer& theY2) const
{
  theX1 = 0;
  theY1 = 0;
  if (IsVirtual())
  {
    theX2 = mySize.x();
    theY2 = mySize.y();
    return;
  }

#if defined(__EMSCRIPTEN__)
  emscripten_get_canvas_element_size (myCanvasId.ToCString(), &theX2, &theY2);
#endif
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
void Wasm_Window::Size (Standard_Integer& theWidth,
                        Standard_Integer& theHeight) const
{
  if (IsVirtual())
  {
    theWidth  = mySize.x();
    theHeight = mySize.y();
    return;
  }

#if defined(__EMSCRIPTEN__)
  emscripten_get_canvas_element_size (myCanvasId.ToCString(), &theWidth, &theHeight);
#endif
}

// =======================================================================
// function : SetSizeLogical
// purpose  :
// =======================================================================
void Wasm_Window::SetSizeLogical (const Graphic3d_Vec2d& theSize)
{
  mySize = Graphic3d_Vec2i (theSize * myDevicePixelRatio);
  if (IsVirtual())
  {
    return;
  }

#if defined(__EMSCRIPTEN__)
  emscripten_set_canvas_element_size (myCanvasId.ToCString(), mySize.x(),  mySize.y());
  emscripten_set_element_css_size    (myCanvasId.ToCString(), theSize.x(), theSize.y());
#endif
}

// =======================================================================
// function : SetSizeBacking
// purpose  :
// =======================================================================
void Wasm_Window::SetSizeBacking (const Graphic3d_Vec2i& theSize)
{
  mySize = theSize;
  if (IsVirtual())
  {
    return;
  }

#if defined(__EMSCRIPTEN__)
  Graphic3d_Vec2i aCanvasSize = mySize;
  Graphic3d_Vec2d aCssSize = Graphic3d_Vec2d (mySize) / myDevicePixelRatio;
  emscripten_set_canvas_element_size (myCanvasId.ToCString(), aCanvasSize.x(), aCanvasSize.y());
  emscripten_set_element_css_size    (myCanvasId.ToCString(), aCssSize.x(),    aCssSize.y());
#endif
}

// =======================================================================
// function : InvalidateContent
// purpose  :
// =======================================================================
void Wasm_Window::InvalidateContent (const Handle(Aspect_DisplayConnection)& )
{
  //
}

// =======================================================================
// function : ProcessMessage
// purpose  :
// =======================================================================
bool Wasm_Window::ProcessMessage (Aspect_WindowInputListener& theListener,
                                  int theEventType, const void* theEvent)
{
#if defined(__EMSCRIPTEN__)
  switch (theEventType)
  {
    case EMSCRIPTEN_EVENT_MOUSEMOVE:
    case EMSCRIPTEN_EVENT_MOUSEDOWN:
    case EMSCRIPTEN_EVENT_MOUSEUP:
    case EMSCRIPTEN_EVENT_CLICK:
    case EMSCRIPTEN_EVENT_DBLCLICK:
    case EMSCRIPTEN_EVENT_MOUSEENTER:
    case EMSCRIPTEN_EVENT_MOUSELEAVE:
    {
      return ProcessMouseEvent (theListener, theEventType, (const EmscriptenMouseEvent* )theEvent);
    }
    case EMSCRIPTEN_EVENT_TOUCHSTART:
    case EMSCRIPTEN_EVENT_TOUCHMOVE:
    case EMSCRIPTEN_EVENT_TOUCHEND:
    case EMSCRIPTEN_EVENT_TOUCHCANCEL:
    {
      return ProcessTouchEvent (theListener, theEventType, (const EmscriptenTouchEvent* )theEvent);
    }
    case EMSCRIPTEN_EVENT_WHEEL:
    {
      return ProcessWheelEvent (theListener, theEventType, (const EmscriptenWheelEvent* )theEvent);
    }
    case EMSCRIPTEN_EVENT_KEYDOWN:
    case EMSCRIPTEN_EVENT_KEYUP:
    case EMSCRIPTEN_EVENT_KEYPRESS:
    {
      return ProcessKeyEvent (theListener, theEventType, (const EmscriptenKeyboardEvent* )theEvent);
    }
    case EMSCRIPTEN_EVENT_RESIZE:
    case EMSCRIPTEN_EVENT_CANVASRESIZED:
    {
      return ProcessUiEvent (theListener, theEventType, (const EmscriptenUiEvent* )theEvent);
    }
    case EMSCRIPTEN_EVENT_FOCUS:
    case EMSCRIPTEN_EVENT_FOCUSIN:
    case EMSCRIPTEN_EVENT_FOCUSOUT:
    {
      return ProcessFocusEvent (theListener, theEventType, (const EmscriptenFocusEvent* )theEvent);
    }
  }
  return false;
#else
  (void )theListener;
  (void )theEventType;
  (void )theEvent;
  return false;
#endif
}

// =======================================================================
// function : ProcessMouseEvent
// purpose  :
// =======================================================================
bool Wasm_Window::ProcessMouseEvent (Aspect_WindowInputListener& theListener,
                                     int theEventType, const EmscriptenMouseEvent* theEvent)
{
#if defined(__EMSCRIPTEN__)
  const Graphic3d_Vec2d aNewPos2d = ConvertPointToBacking (Graphic3d_Vec2d (theEvent->targetX, theEvent->targetY));
  const Graphic3d_Vec2i aNewPos2i = Graphic3d_Vec2i (aNewPos2d + Graphic3d_Vec2d (0.5));
  Aspect_VKeyFlags aFlags = 0;
  if (theEvent->ctrlKey  == EM_TRUE) { aFlags |= Aspect_VKeyFlags_CTRL;  }
  if (theEvent->shiftKey == EM_TRUE) { aFlags |= Aspect_VKeyFlags_SHIFT; }
  if (theEvent->altKey   == EM_TRUE) { aFlags |= Aspect_VKeyFlags_ALT;   }
  if (theEvent->metaKey  == EM_TRUE) { aFlags |= Aspect_VKeyFlags_META;  }

  const bool isEmulated = false;
  const Aspect_VKeyMouse aButtonsOld = theListener.PressedMouseButtons();
  Aspect_VKeyMouse aButtons = Wasm_Window::MouseButtonsFromNative (theEvent->buttons);
  if (theEventType != EMSCRIPTEN_EVENT_MOUSEDOWN)
  {
    aButtons &= aButtonsOld; // filter out unexpected buttons
  }
  switch (theEventType)
  {
    case EMSCRIPTEN_EVENT_MOUSEMOVE:
    {
      if ((aNewPos2i.x() < 0 || aNewPos2i.x() > mySize.x()
        || aNewPos2i.y() < 0 || aNewPos2i.y() > mySize.y())
        && aButtonsOld == Aspect_VKeyMouse_NONE)
      {
        return false;
      }
      if (theListener.UpdateMousePosition (aNewPos2i, aButtons, aFlags, isEmulated))
      {
        theListener.ProcessInput();
      }
      break;
    }
    case EMSCRIPTEN_EVENT_MOUSEDOWN:
    case EMSCRIPTEN_EVENT_MOUSEUP:
    {
      if (theEventType == EMSCRIPTEN_EVENT_MOUSEDOWN)
      {
        if (aNewPos2i.x() < 0 || aNewPos2i.x() > mySize.x()
         || aNewPos2i.y() < 0 || aNewPos2i.y() > mySize.y())
        {
          return false;
        }
      }
      if (theListener.UpdateMouseButtons (aNewPos2i, aButtons, aFlags, isEmulated))
      {
        theListener.ProcessInput();
      }
      break;
    }
    case EMSCRIPTEN_EVENT_CLICK:
    case EMSCRIPTEN_EVENT_DBLCLICK:
    {
      if (aNewPos2i.x() < 0 || aNewPos2i.x() > mySize.x()
       || aNewPos2i.y() < 0 || aNewPos2i.y() > mySize.y())
      {
        return false;
      }
      break;
    }
    case EMSCRIPTEN_EVENT_MOUSEENTER:
    {
      break;
    }
    case EMSCRIPTEN_EVENT_MOUSELEAVE:
    {
      // there is no SetCapture() support, so that mouse unclick events outside canvas will not arrive,
      // so we have to forget current state...
      if (theListener.UpdateMouseButtons (aNewPos2i, Aspect_VKeyMouse_NONE, aFlags, isEmulated))
      {
        theListener.ProcessInput();
      }
      break;
    }
  }
  return true;
#else
  (void )theListener;
  (void )theEventType;
  (void )theEvent;
  return false;
#endif
}

// =======================================================================
// function : ProcessWheelEvent
// purpose  :
// =======================================================================
bool Wasm_Window::ProcessWheelEvent (Aspect_WindowInputListener& theListener,
                                     int theEventType, const EmscriptenWheelEvent* theEvent)
{
#if defined(__EMSCRIPTEN__)
  if (theEventType != EMSCRIPTEN_EVENT_WHEEL)
  {
    return false;
  }

  const Graphic3d_Vec2d aNewPos2d = ConvertPointToBacking (Graphic3d_Vec2d (theEvent->mouse.targetX, theEvent->mouse.targetY));
  const Graphic3d_Vec2i aNewPos2i = Graphic3d_Vec2i (aNewPos2d + Graphic3d_Vec2d (0.5));
  if (aNewPos2i.x() < 0 || aNewPos2i.x() > mySize.x()
   || aNewPos2i.y() < 0 || aNewPos2i.y() > mySize.y())
  {
    return false;
  }

  double aDelta = 0.0;
  switch (theEvent->deltaMode)
  {
    case DOM_DELTA_PIXEL:
    {
      aDelta = theEvent->deltaY / (5.0 * DevicePixelRatio());
      break;
    }
    case DOM_DELTA_LINE:
    {
      aDelta = theEvent->deltaY * 8.0;
      break;
    }
    case DOM_DELTA_PAGE:
    {
      aDelta = theEvent->deltaY >= 0.0 ? 24.0 : -24.0;
      break;
    }
  }
  aDelta /= 15.0;

  if (theListener.UpdateMouseScroll (Aspect_ScrollDelta (aNewPos2i, -aDelta)))
  {
    theListener.ProcessInput();
  }
  return true;
#else
  (void )theListener;
  (void )theEventType;
  (void )theEvent;
  return false;
#endif
}

// =======================================================================
// function : ProcessTouchEvent
// purpose  :
// =======================================================================
bool Wasm_Window::ProcessTouchEvent (Aspect_WindowInputListener& theListener,
                                     int theEventType, const EmscriptenTouchEvent* theEvent)
{
  bool hasUpdates = false;
#if defined(__EMSCRIPTEN__)
  if (theEventType != EMSCRIPTEN_EVENT_TOUCHSTART
   && theEventType != EMSCRIPTEN_EVENT_TOUCHMOVE
   && theEventType != EMSCRIPTEN_EVENT_TOUCHEND
   && theEventType != EMSCRIPTEN_EVENT_TOUCHCANCEL)
  {
    return false;
  }

  for (int aTouchIter = 0; aTouchIter < theEvent->numTouches; ++aTouchIter)
  {
    const EmscriptenTouchPoint& aTouch = theEvent->touches[aTouchIter];
    if (!aTouch.isChanged)
    {
      continue;
    }

    const Standard_Size aTouchId = (Standard_Size )aTouch.identifier;

    const Graphic3d_Vec2d aNewPos2d = ConvertPointToBacking (Graphic3d_Vec2d (aTouch.targetX, aTouch.targetY));
    const Graphic3d_Vec2i aNewPos2i = Graphic3d_Vec2i (aNewPos2d + Graphic3d_Vec2d (0.5));
    switch (theEventType)
    {
      case EMSCRIPTEN_EVENT_TOUCHSTART:
      {
        if (aNewPos2i.x() >= 0 && aNewPos2i.x() < mySize.x()
         && aNewPos2i.y() >= 0 && aNewPos2i.y() < mySize.y())
        {
          hasUpdates = true;
          theListener.AddTouchPoint (aTouchId, aNewPos2d);
        }
        break;
      }
      case EMSCRIPTEN_EVENT_TOUCHMOVE:
      {
        const int anOldIndex = theListener.TouchPoints().FindIndex (aTouchId);
        if (anOldIndex != 0)
        {
          hasUpdates = true;
          theListener.UpdateTouchPoint (aTouchId, aNewPos2d);
        }
        break;
      }
      case EMSCRIPTEN_EVENT_TOUCHEND:
      case EMSCRIPTEN_EVENT_TOUCHCANCEL:
      {
        if (theListener.RemoveTouchPoint (aTouchId))
        {
          hasUpdates = true;
        }
        break;
      }
    }
  }
  if (hasUpdates)
  {
    theListener.ProcessInput();
  }
#else
  (void )theEventType;
  (void )theEvent;
#endif
  return hasUpdates || theListener.HasTouchPoints();
}

// =======================================================================
// function : ProcessKeyEvent
// purpose  :
// =======================================================================
bool Wasm_Window::ProcessKeyEvent (Aspect_WindowInputListener& theListener,
                                   int theEventType, const EmscriptenKeyboardEvent* theEvent)
{
#if defined(__EMSCRIPTEN__)
  if (theEventType != EMSCRIPTEN_EVENT_KEYDOWN
   && theEventType != EMSCRIPTEN_EVENT_KEYUP
   && theEventType != EMSCRIPTEN_EVENT_KEYPRESS)
  {
    return false;
  }

  const double aTimeStamp = theListener.EventTime();
  const Aspect_VKey aVKey = Wasm_Window::VirtualKeyFromNative (theEvent->keyCode);
  if (aVKey == Aspect_VKey_UNKNOWN)
  {
    return false;
  }

  switch (theEventType)
  {
    case EMSCRIPTEN_EVENT_KEYDOWN:
    {
      if (theEvent->repeat == EM_TRUE)
      {
        return false;
      }

      theListener.KeyDown (aVKey, aTimeStamp);
      theListener.ProcessInput();
      return false;
    }
    case EMSCRIPTEN_EVENT_KEYUP:
    {
      theListener.KeyUp (aVKey, aTimeStamp);
      theListener.ProcessInput();
      return false;
    }
  }
#else
  (void )theListener;
  (void )theEventType;
  (void )theEvent;
#endif
  return false;
}

// =======================================================================
// function : ProcessUiEvent
// purpose  :
// =======================================================================
bool Wasm_Window::ProcessUiEvent (Aspect_WindowInputListener& theListener,
                                  int theEventType, const EmscriptenUiEvent* )
{
#if defined(__EMSCRIPTEN__)
  if (theEventType != EMSCRIPTEN_EVENT_RESIZE
   && theEventType != EMSCRIPTEN_EVENT_CANVASRESIZED)
  {
    return false;
  }
#else
  (void )theEventType;
#endif
  theListener.ProcessConfigure (true);
  return true;
}

// =======================================================================
// function : ProcessFocusEvent
// purpose  :
// =======================================================================
bool Wasm_Window::ProcessFocusEvent (Aspect_WindowInputListener& theListener,
                                     int theEventType, const EmscriptenFocusEvent* )
{
  bool isActivated = false;
#if defined(__EMSCRIPTEN__)
  if (theEventType != EMSCRIPTEN_EVENT_FOCUS
   && theEventType != EMSCRIPTEN_EVENT_FOCUSIN // about to receive focus
   && theEventType != EMSCRIPTEN_EVENT_FOCUSOUT)
  {
    return false;
  }
  isActivated = theEventType == EMSCRIPTEN_EVENT_FOCUS;
#else
  (void )theEventType;
#endif
  theListener.ProcessFocus (isActivated);
  return true;
}

// =======================================================================
// function : MouseButtonsFromNative
// purpose  :
// =======================================================================
Aspect_VKeyMouse Wasm_Window::MouseButtonsFromNative (unsigned short theButtons)
{
  Aspect_VKeyMouse aButtons = Aspect_VKeyMouse_NONE;
  if ((theButtons & 0x1) != 0)
  {
    aButtons |= Aspect_VKeyMouse_LeftButton;
  }
  if ((theButtons & 0x2) != 0)
  {
    aButtons |= Aspect_VKeyMouse_RightButton;
  }
  if ((theButtons & 0x4) != 0)
  {
    aButtons |= Aspect_VKeyMouse_MiddleButton;
  }
  return aButtons;
}

// =======================================================================
// function : VirtualKeyFromNative
// purpose  :
// =======================================================================
Aspect_VKey Wasm_Window::VirtualKeyFromNative (Standard_Integer theKey)
{
#if defined(__EMSCRIPTEN__)
  if (theKey >= DOM_VK_0
   && theKey <= DOM_VK_9)
  {
    // numpad keys
    return Aspect_VKey((theKey - DOM_VK_0) + Aspect_VKey_0);
  }
  if (theKey >= DOM_VK_A
   && theKey <= DOM_VK_Z)
  {
    // main latin alphabet keys
    return Aspect_VKey((theKey - DOM_VK_A) + Aspect_VKey_A);
  }
  if (theKey >= DOM_VK_F1
   && theKey <= DOM_VK_F24)
  {
    // special keys
    if (theKey <= DOM_VK_F12)
    {
      return Aspect_VKey((theKey - DOM_VK_F1) + Aspect_VKey_F1);
    }
    return Aspect_VKey_UNKNOWN;
  }
  if (theKey >= DOM_VK_NUMPAD0
   && theKey <= DOM_VK_NUMPAD9)
  {
    // numpad keys
    return Aspect_VKey((theKey - DOM_VK_NUMPAD0) + Aspect_VKey_Numpad0);
  }

  switch (theKey)
  {
    case DOM_VK_CANCEL:
    case DOM_VK_HELP:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_BACK_SPACE:
      return Aspect_VKey_Backspace;
    case DOM_VK_TAB:
      return Aspect_VKey_Tab;
    case DOM_VK_CLEAR:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_RETURN:
    case DOM_VK_ENTER:
      return Aspect_VKey_Enter;
    case DOM_VK_SHIFT:
      return Aspect_VKey_Shift;
    case DOM_VK_CONTROL:
      return Aspect_VKey_Control;
    case DOM_VK_ALT:
      return Aspect_VKey_Alt;
    case DOM_VK_PAUSE:
    case DOM_VK_CAPS_LOCK:
    case DOM_VK_KANA:
    //case DOM_VK_HANGUL:
    case DOM_VK_EISU:
    case DOM_VK_JUNJA:
    case DOM_VK_FINAL:
    case DOM_VK_HANJA:
    //case DOM_VK_KANJI:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_ESCAPE:
      return Aspect_VKey_Escape;
    case DOM_VK_CONVERT:
    case DOM_VK_NONCONVERT:
    case DOM_VK_ACCEPT:
    case DOM_VK_MODECHANGE:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_SPACE:
      return Aspect_VKey_Space;
    case DOM_VK_PAGE_UP:
      return Aspect_VKey_PageUp;
    case DOM_VK_PAGE_DOWN:
      return Aspect_VKey_PageDown;
    case DOM_VK_END:
      return Aspect_VKey_End;
    case DOM_VK_HOME:
      return Aspect_VKey_Home;
    case DOM_VK_LEFT:
      return Aspect_VKey_Left;
    case DOM_VK_UP:
      return Aspect_VKey_Up;
    case DOM_VK_RIGHT:
      return Aspect_VKey_Right;
    case DOM_VK_DOWN:
      return Aspect_VKey_Down;
    case DOM_VK_SELECT:
    case DOM_VK_PRINT:
    case DOM_VK_EXECUTE:
    case DOM_VK_PRINTSCREEN:
    case DOM_VK_INSERT:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_DELETE:
      return Aspect_VKey_Delete;
    case DOM_VK_COLON:
      return Aspect_VKey_Comma;
    case DOM_VK_SEMICOLON:
      return Aspect_VKey_Semicolon;
    case DOM_VK_LESS_THAN:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_EQUALS:
      return Aspect_VKey_Equal;
    case DOM_VK_GREATER_THAN:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_QUESTION_MARK:
      return Aspect_VKey_Slash;
    case DOM_VK_AT: // @ key
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_WIN:
      return Aspect_VKey_Meta;
    case DOM_VK_CONTEXT_MENU:
    case DOM_VK_SLEEP:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_MULTIPLY:
      return Aspect_VKey_NumpadMultiply;
    case DOM_VK_ADD:
      return Aspect_VKey_NumpadAdd;
    case DOM_VK_SEPARATOR:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_SUBTRACT:
      return Aspect_VKey_NumpadSubtract;
    case DOM_VK_DECIMAL:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_DIVIDE:
      return Aspect_VKey_NumpadDivide;
    case DOM_VK_NUM_LOCK:
      return Aspect_VKey_Numlock;
    case DOM_VK_SCROLL_LOCK:
      return Aspect_VKey_Scroll;
    case DOM_VK_WIN_OEM_FJ_JISHO:
    case DOM_VK_WIN_OEM_FJ_MASSHOU:
    case DOM_VK_WIN_OEM_FJ_TOUROKU:
    case DOM_VK_WIN_OEM_FJ_LOYA:
    case DOM_VK_WIN_OEM_FJ_ROYA:
    case DOM_VK_CIRCUMFLEX:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_EXCLAMATION:
    case DOM_VK_DOUBLE_QUOTE:
    //case DOM_VK_HASH:
    case DOM_VK_DOLLAR:
    case DOM_VK_PERCENT:
    case DOM_VK_AMPERSAND:
    case DOM_VK_UNDERSCORE:
    case DOM_VK_OPEN_PAREN:
    case DOM_VK_CLOSE_PAREN:
    case DOM_VK_ASTERISK:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_PLUS:
      return Aspect_VKey_Plus;
    case DOM_VK_PIPE:
    case DOM_VK_HYPHEN_MINUS:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_OPEN_CURLY_BRACKET:
      return Aspect_VKey_BracketLeft;
    case DOM_VK_CLOSE_CURLY_BRACKET:
      return Aspect_VKey_BracketRight;
    case DOM_VK_TILDE:
      return Aspect_VKey_Tilde;
    case DOM_VK_VOLUME_MUTE:
      return Aspect_VKey_VolumeMute;
    case DOM_VK_VOLUME_DOWN:
      return Aspect_VKey_VolumeDown;
    case DOM_VK_VOLUME_UP:
      return Aspect_VKey_VolumeUp;
    case DOM_VK_COMMA:
      return Aspect_VKey_Comma;
    case DOM_VK_PERIOD:
      return Aspect_VKey_Period;
    case DOM_VK_SLASH:
      return Aspect_VKey_Slash;
    case DOM_VK_BACK_QUOTE:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_OPEN_BRACKET:
      return Aspect_VKey_BracketLeft;
    case DOM_VK_BACK_SLASH:
      return Aspect_VKey_Backslash;
    case DOM_VK_CLOSE_BRACKET:
      return Aspect_VKey_BracketRight;
    case DOM_VK_QUOTE:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_META:
      return Aspect_VKey_Meta;
    case DOM_VK_ALTGR:
      return Aspect_VKey_Alt;
    case DOM_VK_WIN_ICO_HELP:
    case DOM_VK_WIN_ICO_00:
    case DOM_VK_WIN_ICO_CLEAR:
    case DOM_VK_WIN_OEM_RESET:
    case DOM_VK_WIN_OEM_JUMP:
    case DOM_VK_WIN_OEM_PA1:
    case DOM_VK_WIN_OEM_PA2:
    case DOM_VK_WIN_OEM_PA3:
    case DOM_VK_WIN_OEM_WSCTRL:
    case DOM_VK_WIN_OEM_CUSEL:
    case DOM_VK_WIN_OEM_ATTN:
    case DOM_VK_WIN_OEM_FINISH:
    case DOM_VK_WIN_OEM_COPY:
    case DOM_VK_WIN_OEM_AUTO:
    case DOM_VK_WIN_OEM_ENLW:
    case DOM_VK_WIN_OEM_BACKTAB:
    case DOM_VK_ATTN:
    case DOM_VK_CRSEL:
    case DOM_VK_EXSEL:
    case DOM_VK_EREOF:
      return Aspect_VKey_UNKNOWN;
    case DOM_VK_PLAY:
      return Aspect_VKey_MediaPlayPause;
    case DOM_VK_ZOOM:
    case DOM_VK_PA1:
    case DOM_VK_WIN_OEM_CLEAR:
      return Aspect_VKey_UNKNOWN;
  }
#else
  (void )theKey;
#endif
  return Aspect_VKey_UNKNOWN;
}
