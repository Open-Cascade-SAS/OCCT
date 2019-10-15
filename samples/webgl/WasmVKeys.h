// Copyright (c) 2019 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#ifndef _WasmVKeys_HeaderFile
#define _WasmVKeys_HeaderFile

#include <Aspect_VKey.hxx>

#include <emscripten/key_codes.h>

//! Convert Emscripten mouse buttons into Aspect_VKeyMouse.
inline Aspect_VKeyMouse WasmVKeys_MouseButtonsFromNative (unsigned short theButtons)
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

//! Convert DOM virtual key into Aspect_VKey.
inline Aspect_VKey WasmVKeys_VirtualKeyFromNative (Standard_Integer theKey)
{
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
  return Aspect_VKey_UNKNOWN;
}

#endif // _WasmVKeys_HeaderFile
