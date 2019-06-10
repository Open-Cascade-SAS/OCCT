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

#ifndef _Aspect_VKeyFlags_HeaderFile
#define _Aspect_VKeyFlags_HeaderFile

//! Key modifier, for combining with general key from Aspect_VKey.
typedef unsigned int Aspect_VKeyFlags;

//! Key modifier, for combining with general key from Aspect_VKey.
enum
{
  Aspect_VKeyFlags_NONE  = 0,
  // reserve first 8 bits to combine value with Aspect_VKey
  Aspect_VKeyFlags_SHIFT = 1 <<  8, //!< Aspect_VKey_Shift
  Aspect_VKeyFlags_CTRL  = 1 <<  9, //!< Aspect_VKey_Control
  Aspect_VKeyFlags_ALT   = 1 << 10, //!< Aspect_VKey_Alt
  Aspect_VKeyFlags_MENU  = 1 << 11, //!< Aspect_VKey_Menu
  Aspect_VKeyFlags_META  = 1 << 12, //!< Aspect_VKey_Meta

  Aspect_VKeyFlags_ALL = Aspect_VKeyFlags_SHIFT | Aspect_VKeyFlags_CTRL | Aspect_VKeyFlags_ALT | Aspect_VKeyFlags_MENU | Aspect_VKeyFlags_META
};

//! Mouse buttons, for combining with Aspect_VKey and Aspect_VKeyFlags.
typedef unsigned int Aspect_VKeyMouse;

//! Mouse button bitmask
enum
{
  Aspect_VKeyMouse_NONE         = 0,       //!< no buttons

  Aspect_VKeyMouse_LeftButton   = 1 << 13, //!< mouse left   button
  Aspect_VKeyMouse_MiddleButton = 1 << 14, //!< mouse middle button (scroll)
  Aspect_VKeyMouse_RightButton  = 1 << 15, //!< mouse right  button

  Aspect_VKeyMouse_MainButtons = Aspect_VKeyMouse_LeftButton | Aspect_VKeyMouse_MiddleButton | Aspect_VKeyMouse_RightButton
};

#endif // _Aspect_VKeyFlags_HeaderFile
