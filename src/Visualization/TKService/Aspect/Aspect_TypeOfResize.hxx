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

#ifndef _Aspect_TypeOfResize_HeaderFile
#define _Aspect_TypeOfResize_HeaderFile

//! Defines the type of Resize Window method applied
//! by the user.
enum Aspect_TypeOfResize
{
  Aspect_TOR_UNKNOWN,
  Aspect_TOR_NO_BORDER,
  Aspect_TOR_TOP_BORDER,
  Aspect_TOR_RIGHT_BORDER,
  Aspect_TOR_BOTTOM_BORDER,
  Aspect_TOR_LEFT_BORDER,
  Aspect_TOR_TOP_AND_RIGHT_BORDER,
  Aspect_TOR_RIGHT_AND_BOTTOM_BORDER,
  Aspect_TOR_BOTTOM_AND_LEFT_BORDER,
  Aspect_TOR_LEFT_AND_TOP_BORDER
};

#endif // _Aspect_TypeOfResize_HeaderFile
