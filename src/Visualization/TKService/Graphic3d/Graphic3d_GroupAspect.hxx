// Created on: 1993-03-31
// Created by: NW,JPB,CAL
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Graphic3d_GroupAspect_HeaderFile
#define _Graphic3d_GroupAspect_HeaderFile

//! Identifies primitives aspects defined per group.
//! - ASPECT_LINE: aspect for line primitives;
//! - ASPECT_TEXT: aspect for text primitives;
//! - ASPECT_MARKER: aspect for marker primitives;
//! - ASPECT_FILL_AREA: aspect for face primitives.
enum Graphic3d_GroupAspect
{
  Graphic3d_ASPECT_LINE,
  Graphic3d_ASPECT_TEXT,
  Graphic3d_ASPECT_MARKER,
  Graphic3d_ASPECT_FILL_AREA
};

#endif // _Graphic3d_GroupAspect_HeaderFile
