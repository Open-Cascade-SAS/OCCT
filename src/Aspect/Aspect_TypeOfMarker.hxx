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
enum Aspect_TypeOfMarker
{
  Aspect_TOM_EMPTY = -1,  //!< hidden
  Aspect_TOM_POINT =  0,  //!< point   .
  Aspect_TOM_PLUS,        //!< plus    +
  Aspect_TOM_STAR,        //!< star    *
  Aspect_TOM_X,           //!< cross   x
  Aspect_TOM_O,           //!< circle  O
  Aspect_TOM_O_POINT,     //!< a point in a circle
  Aspect_TOM_O_PLUS,      //!< a plus  in a circle
  Aspect_TOM_O_STAR,      //!< a star  in a circle
  Aspect_TOM_O_X,         //!< a cross in a circle
  Aspect_TOM_RING1,       //!< a large  ring
  Aspect_TOM_RING2,       //!< a medium ring
  Aspect_TOM_RING3,       //!< a small  ring
  Aspect_TOM_BALL,        //!< a ball with 1 color and different saturations
  Aspect_TOM_USERDEFINED  //!< defined by Users (custom image)
};

#endif // _Aspect_TypeOfMarker_HeaderFile
