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

//! Definition of the Triedron position in the views
//!
//! TOTP_CENTER         at the center of the view
//! TOTP_LEFT_LOWER     at the left lower corner
//! TOTP_LEFT_UPPER     at the left upper corner
//! TOTP_RIGHT_LOWER    at the right lower corner
//! TOTP_RIGHT_UPPER    at the right upper corner
//! TOTP_01 to TOTP_10  not yet implemented.
enum Aspect_TypeOfTriedronPosition
{
Aspect_TOTP_CENTER,
Aspect_TOTP_LEFT_LOWER,
Aspect_TOTP_LEFT_UPPER,
Aspect_TOTP_RIGHT_LOWER,
Aspect_TOTP_RIGHT_UPPER,
Aspect_TOTP_01,
Aspect_TOTP_02,
Aspect_TOTP_03,
Aspect_TOTP_04,
Aspect_TOTP_05,
Aspect_TOTP_06,
Aspect_TOTP_07,
Aspect_TOTP_08,
Aspect_TOTP_09,
Aspect_TOTP_10
};

#endif // _Aspect_TypeOfTriedronPosition_HeaderFile
