// Copyright (c) 2016-2019 OPEN CASCADE SAS
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

#ifndef _Aspect_VKey_HeaderFile
#define _Aspect_VKey_HeaderFile

#include <Aspect_VKeyFlags.hxx>

//! Define virtual key as integer number to allow extensions.
typedef unsigned int Aspect_VKey;

//! Enumeration defining virtual keys irrelevant to current keyboard layout for simplified hot-keys management logic.
enum Aspect_VKeyBasic
{
  Aspect_VKey_UNKNOWN = 0,

  // main latin alphabet keys
  Aspect_VKey_A = 1,
  Aspect_VKey_B,
  Aspect_VKey_C,
  Aspect_VKey_D,
  Aspect_VKey_E,
  Aspect_VKey_F,
  Aspect_VKey_G,
  Aspect_VKey_H,
  Aspect_VKey_I,
  Aspect_VKey_J,
  Aspect_VKey_K,
  Aspect_VKey_L,
  Aspect_VKey_M,
  Aspect_VKey_N,
  Aspect_VKey_O,
  Aspect_VKey_P,
  Aspect_VKey_Q,
  Aspect_VKey_R,
  Aspect_VKey_S,
  Aspect_VKey_T,
  Aspect_VKey_U,
  Aspect_VKey_V,
  Aspect_VKey_W,
  Aspect_VKey_X,
  Aspect_VKey_Y,
  Aspect_VKey_Z,

  Aspect_VKey_0,
  Aspect_VKey_1,
  Aspect_VKey_2,
  Aspect_VKey_3,
  Aspect_VKey_4,
  Aspect_VKey_5,
  Aspect_VKey_6,
  Aspect_VKey_7,
  Aspect_VKey_8,
  Aspect_VKey_9,

  Aspect_VKey_F1,
  Aspect_VKey_F2,
  Aspect_VKey_F3,
  Aspect_VKey_F4,
  Aspect_VKey_F5,
  Aspect_VKey_F6,
  Aspect_VKey_F7,
  Aspect_VKey_F8,
  Aspect_VKey_F9,
  Aspect_VKey_F10,
  Aspect_VKey_F11,
  Aspect_VKey_F12,

  // standard keys
  Aspect_VKey_Up,
  Aspect_VKey_Down,
  Aspect_VKey_Left,
  Aspect_VKey_Right,
  Aspect_VKey_Plus,         //!< '+'
  Aspect_VKey_Minus,        //!< '-'
  Aspect_VKey_Equal,        //!< '=+'
  Aspect_VKey_PageUp,
  Aspect_VKey_PageDown,
  Aspect_VKey_Home,
  Aspect_VKey_End,
  Aspect_VKey_Escape,
  Aspect_VKey_Back,
  Aspect_VKey_Enter,
  Aspect_VKey_Backspace,
  Aspect_VKey_Space,
  Aspect_VKey_Delete,
  Aspect_VKey_Tilde,
  Aspect_VKey_Tab,
  Aspect_VKey_Comma,        //!< ','
  Aspect_VKey_Period,       //!< '.'
  Aspect_VKey_Semicolon,    //!< ';:'
  Aspect_VKey_Slash,        //!< '/?'
  Aspect_VKey_BracketLeft,  //!< '[{'
  Aspect_VKey_Backslash,    //!< '\|'
  Aspect_VKey_BracketRight, //!< ']}'
  Aspect_VKey_Apostrophe,   //!< ''"'
  Aspect_VKey_Numlock,      //!< Num Lock key
  Aspect_VKey_Scroll,       //!< Scroll Lock key

  // numpad keys
  Aspect_VKey_Numpad0,
  Aspect_VKey_Numpad1,
  Aspect_VKey_Numpad2,
  Aspect_VKey_Numpad3,
  Aspect_VKey_Numpad4,
  Aspect_VKey_Numpad5,
  Aspect_VKey_Numpad6,
  Aspect_VKey_Numpad7,
  Aspect_VKey_Numpad8,
  Aspect_VKey_Numpad9,
  Aspect_VKey_NumpadMultiply, //!< numpad '*'
  Aspect_VKey_NumpadAdd,      //!< numpad '+'
  Aspect_VKey_NumpadSubtract, //!< numpad '-'
  Aspect_VKey_NumpadDivide,   //!< numpad '/'

  // Multimedia keys
  Aspect_VKey_MediaNextTrack,
  Aspect_VKey_MediaPreviousTrack,
  Aspect_VKey_MediaStop,
  Aspect_VKey_MediaPlayPause,
  Aspect_VKey_VolumeMute,
  Aspect_VKey_VolumeDown,
  Aspect_VKey_VolumeUp,
  Aspect_VKey_BrowserBack,
  Aspect_VKey_BrowserForward,
  Aspect_VKey_BrowserRefresh,
  Aspect_VKey_BrowserStop,
  Aspect_VKey_BrowserSearch,
  Aspect_VKey_BrowserFavorites,
  Aspect_VKey_BrowserHome,

  // 3d view keys
  Aspect_VKey_ViewTop,
  Aspect_VKey_ViewBottom,
  Aspect_VKey_ViewLeft,
  Aspect_VKey_ViewRight,
  Aspect_VKey_ViewFront,
  Aspect_VKey_ViewBack,
  Aspect_VKey_ViewAxoLeftProj,
  Aspect_VKey_ViewAxoRightProj,
  Aspect_VKey_ViewFitAll,
  Aspect_VKey_ViewRoll90CW,
  Aspect_VKey_ViewRoll90CCW,
  Aspect_VKey_ViewSwitchRotate,

  // modifier keys, @sa Aspect_VKey_ModifiersLower and Aspect_VKey_ModifiersUpper below
  Aspect_VKey_Shift,
  Aspect_VKey_Control,
  Aspect_VKey_Alt,
  Aspect_VKey_Menu,
  Aspect_VKey_Meta,

  // virtual navigation keys, @sa Aspect_VKey_NavigationKeysLower and Aspect_VKey_NavigationKeysUpper below
  Aspect_VKey_NavInteract,         //!< interact
  Aspect_VKey_NavForward,          //!< go forward
  Aspect_VKey_NavBackward,         //!< go backward
  Aspect_VKey_NavSlideLeft,        //!< sidewalk, left
  Aspect_VKey_NavSlideRight,       //!< sidewalk, right
  Aspect_VKey_NavSlideUp,          //!< lift up
  Aspect_VKey_NavSlideDown,        //!< fall down
  Aspect_VKey_NavRollCCW,          //!< bank left  (roll counter-clockwise)
  Aspect_VKey_NavRollCW,           //!< bank right (roll clockwise)
  Aspect_VKey_NavLookLeft,         //!< look left  (yaw counter-clockwise)
  Aspect_VKey_NavLookRight,        //!< look right (yaw clockwise)
  Aspect_VKey_NavLookUp,           //!< look up    (pitch clockwise)
  Aspect_VKey_NavLookDown,         //!< look down  (pitch counter-clockwise)
  Aspect_VKey_NavCrouch,           //!< crouch walking
  Aspect_VKey_NavJump,             //!< jump
  Aspect_VKey_NavThrustForward,    //!< increase continuous velocity in forward  direction
  Aspect_VKey_NavThrustBackward,   //!< increase continuous velocity in reversed direction
  Aspect_VKey_NavThrustStop,       //!< reset continuous velocity
  Aspect_VKey_NavSpeedIncrease,    //!< increase navigation speed
  Aspect_VKey_NavSpeedDecrease,    //!< decrease navigation speed
};

//! Auxiliary ranges.
enum
{
  Aspect_VKey_Lower = 0,
  Aspect_VKey_ModifiersLower      = Aspect_VKey_Shift,
  Aspect_VKey_ModifiersUpper      = Aspect_VKey_Meta,
  Aspect_VKey_NavigationKeysLower = Aspect_VKey_NavInteract,
  Aspect_VKey_NavigationKeysUpper = Aspect_VKey_NavSpeedDecrease,
  Aspect_VKey_Upper = Aspect_VKey_NavSpeedDecrease,
  Aspect_VKey_NB  = Aspect_VKey_Upper - Aspect_VKey_Lower + 1,
  Aspect_VKey_MAX = 255
};

//! Return modifier flags for specified modifier key.
inline Aspect_VKeyFlags Aspect_VKey2Modifier (Aspect_VKey theKey)
{
  switch (theKey)
  {
    case Aspect_VKey_Shift:   return Aspect_VKeyFlags_SHIFT;
    case Aspect_VKey_Control: return Aspect_VKeyFlags_CTRL;
    case Aspect_VKey_Alt:     return Aspect_VKeyFlags_ALT;
    case Aspect_VKey_Menu:    return Aspect_VKeyFlags_MENU;
    case Aspect_VKey_Meta:    return Aspect_VKeyFlags_META;
    default:                  return 0;
  }
}

#endif // _Aspect_VKey_HeaderFile
