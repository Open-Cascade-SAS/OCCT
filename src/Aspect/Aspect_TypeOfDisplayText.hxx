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

#ifndef _Aspect_TypeOfDisplayText_HeaderFile
#define _Aspect_TypeOfDisplayText_HeaderFile

//! Define the display type of the text.
//!
//! TODT_NORMAL         Default display. Text only.
//! TODT_SUBTITLE       There is a subtitle under the text.
//! TODT_DEKALE         The text is displayed with a 3D style.
//! TODT_BLEND          The text is displayed in XOR.
//! TODT_DIMENSION      Dimension line under text will be invisible.
enum Aspect_TypeOfDisplayText
{
Aspect_TODT_NORMAL,
Aspect_TODT_SUBTITLE,
Aspect_TODT_DEKALE,
Aspect_TODT_BLEND,
Aspect_TODT_DIMENSION
};

#endif // _Aspect_TypeOfDisplayText_HeaderFile
