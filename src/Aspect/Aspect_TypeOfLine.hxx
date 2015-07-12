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

#ifndef _Aspect_TypeOfLine_HeaderFile
#define _Aspect_TypeOfLine_HeaderFile

//! Definition of line types
//!
//! TOL_SOLID           continuous
//! TOL_DASH            dashed 2.0,1.0 (MM)
//! TOL_DOT             dotted 0.2,0.5 (MM)
//! TOL_DOTDASH         mixed  10.0,1.0,2.0,1.0 (MM)
//! TOL_USERDEFINED     defined by Users
enum Aspect_TypeOfLine
{
Aspect_TOL_SOLID,
Aspect_TOL_DASH,
Aspect_TOL_DOT,
Aspect_TOL_DOTDASH,
Aspect_TOL_USERDEFINED
};

#endif // _Aspect_TypeOfLine_HeaderFile
