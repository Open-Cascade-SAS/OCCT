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

#ifndef _Aspect_TypeOfMarker_HeaderFile
#define _Aspect_TypeOfMarker_HeaderFile

//! Definition of types of markers
//!
//! TOM_POINT   point   .
//! TOM_PLUS    plus    +
//! TOM_STAR    star    *
//! TOM_X       cross   x
//! TOM_O       circle  O
//! TOM_O_POINT a point in a circle
//! TOM_O_PLUS  a plus in a circle
//! TOM_O_STAR  a star in a circle
//! TOM_O_X     a cross in a circle
//! TOM_RING1   a large ring
//! TOM_RING2   a medium ring
//! TOM_RING3   a small ring
//! TOM_BALL    a ball with 1 color and different saturations
//! TOM_USERDEFINED     defined by Users
enum Aspect_TypeOfMarker
{
Aspect_TOM_POINT,
Aspect_TOM_PLUS,
Aspect_TOM_STAR,
Aspect_TOM_X,
Aspect_TOM_O,
Aspect_TOM_O_POINT,
Aspect_TOM_O_PLUS,
Aspect_TOM_O_STAR,
Aspect_TOM_O_X,
Aspect_TOM_RING1,
Aspect_TOM_RING2,
Aspect_TOM_RING3,
Aspect_TOM_BALL,
Aspect_TOM_USERDEFINED
};

#endif // _Aspect_TypeOfMarker_HeaderFile
