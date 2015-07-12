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

//! Definition of interior types for primitive
//! faces.
//!
//! IS_EMPTY    no interior.
//! IS_HOLLOW   display the boundaries of the surface.
//! IS_HATCH    display hatched with a hatch style.
//! IS_SOLID    display the interior entirely filled.
//! IS_HIDDENLINE       display in hidden lines removed.
//! IS_POINT            display only vertices.
enum Aspect_InteriorStyle
{
Aspect_IS_EMPTY,
Aspect_IS_HOLLOW,
Aspect_IS_HATCH,
Aspect_IS_SOLID,
Aspect_IS_HIDDENLINE,
Aspect_IS_POINT
};

#endif // _Aspect_InteriorStyle_HeaderFile
