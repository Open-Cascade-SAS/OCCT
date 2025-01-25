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

#ifndef _Aspect_InteriorStyle_HeaderFile
#define _Aspect_InteriorStyle_HeaderFile

//! Interior types for primitive faces.
enum Aspect_InteriorStyle
{
  Aspect_IS_EMPTY = -1, //!< no interior
  Aspect_IS_SOLID = 0,  //!< normally filled surface interior
  Aspect_IS_HATCH,      //!< hatched surface interior
  Aspect_IS_HIDDENLINE, //!< interior is filled with viewer background color
  Aspect_IS_POINT,      //!< display only vertices of surface (obsolete)

  // obsolete aliases
  Aspect_IS_HOLLOW = Aspect_IS_EMPTY, //!< transparent surface interior
};

#endif // _Aspect_InteriorStyle_HeaderFile
