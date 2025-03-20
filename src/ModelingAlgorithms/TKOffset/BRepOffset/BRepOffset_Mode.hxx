// Created on: 1995-10-12
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepOffset_Mode_HeaderFile
#define _BRepOffset_Mode_HeaderFile

//! Lists the offset modes. These are the following:
//! - BRepOffset_Skin which describes the offset along
//! the surface of a solid, used to obtain a manifold topological space,
//! - BRepOffset_Pipe which describes the offset of a
//! curve, used to obtain a pre-surface,
//! - BRepOffset_RectoVerso which describes the offset
//! of a given surface shell along both sides of the surface.
enum BRepOffset_Mode
{
  BRepOffset_Skin,
  BRepOffset_Pipe,
  BRepOffset_RectoVerso
};

#endif // _BRepOffset_Mode_HeaderFile
