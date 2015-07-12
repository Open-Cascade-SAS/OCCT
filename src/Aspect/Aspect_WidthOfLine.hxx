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

#ifndef _Aspect_WidthOfLine_HeaderFile
#define _Aspect_WidthOfLine_HeaderFile

//! Definition of line types
//!
//! WOL_THIN            thin line (1 pixel width)
//! WOL_MEDIUM          medium width of 0.5 MM
//! WOL_THICK           thick width of 0.7 MM
//! WOL_VERYTHICK       very thick width of 1.5 MM
//! WOL_USERDEFINED     defined by Users
enum Aspect_WidthOfLine
{
Aspect_WOL_THIN,
Aspect_WOL_MEDIUM,
Aspect_WOL_THICK,
Aspect_WOL_VERYTHICK,
Aspect_WOL_USERDEFINED
};

#endif // _Aspect_WidthOfLine_HeaderFile
