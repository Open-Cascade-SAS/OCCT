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

#ifndef _Aspect_TypeOfTriedronEcho_HeaderFile
#define _Aspect_TypeOfTriedronEcho_HeaderFile

//! Definition of the Triedron echo zone to highlight
//!
//! TOTE_NONE           no echo zone
//! TOTE_ORIGIN         a box on origin of the Triedron
//! TOTE_AXIS_X         highlights the X axis
//! TOTE_AXIS_Y         highlights the Y axis
//! TOTE_AXIS_Z         highlights the Z axis
//! TOTE_TEXT_X         highlights the X character
//! TOTE_TEXT_Y         highlights the Y character
//! TOTE_TEXT_Z         highlights the Z character
//! TOTE_01 to TOTE_10  not yet implemented.
enum Aspect_TypeOfTriedronEcho
{
Aspect_TOTE_NONE,
Aspect_TOTE_ORIGIN,
Aspect_TOTE_AXIS_X,
Aspect_TOTE_AXIS_Y,
Aspect_TOTE_AXIS_Z,
Aspect_TOTE_TEXT_X,
Aspect_TOTE_TEXT_Y,
Aspect_TOTE_TEXT_Z,
Aspect_TOTE_01,
Aspect_TOTE_02,
Aspect_TOTE_03,
Aspect_TOTE_04,
Aspect_TOTE_05,
Aspect_TOTE_06,
Aspect_TOTE_07,
Aspect_TOTE_08,
Aspect_TOTE_09,
Aspect_TOTE_10
};

#endif // _Aspect_TypeOfTriedronEcho_HeaderFile
