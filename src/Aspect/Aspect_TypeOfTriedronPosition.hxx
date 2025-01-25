// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Aspect_TypeOfTriedronPosition_HeaderFile
#define _Aspect_TypeOfTriedronPosition_HeaderFile

//! Definition of the Trihedron position in the views.
//! It is defined as a bitmask to simplify handling vertical and horizontal alignment independently.
enum Aspect_TypeOfTriedronPosition
{
  Aspect_TOTP_CENTER      = 0x0000, //!< at the center of the view
  Aspect_TOTP_TOP         = 0x0001, //!< at the middle of the top    side
  Aspect_TOTP_BOTTOM      = 0x0002, //!< at the middle of the bottom side
  Aspect_TOTP_LEFT        = 0x0004, //!< at the middle of the left   side
  Aspect_TOTP_RIGHT       = 0x0008, //!< at the middle of the right  side
  Aspect_TOTP_LEFT_LOWER  = Aspect_TOTP_BOTTOM | Aspect_TOTP_LEFT,  //!< at the left lower corner
  Aspect_TOTP_LEFT_UPPER  = Aspect_TOTP_TOP | Aspect_TOTP_LEFT,     //!< at the left upper corner
  Aspect_TOTP_RIGHT_LOWER = Aspect_TOTP_BOTTOM | Aspect_TOTP_RIGHT, //!< at the right lower corner
  Aspect_TOTP_RIGHT_UPPER = Aspect_TOTP_TOP | Aspect_TOTP_RIGHT,    //!< at the right upper corner
};

#endif // _Aspect_TypeOfTriedronPosition_HeaderFile
