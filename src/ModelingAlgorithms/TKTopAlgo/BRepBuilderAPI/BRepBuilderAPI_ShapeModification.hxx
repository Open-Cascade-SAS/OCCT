// Created on: 1993-07-06
// Created by: Remi LEQUETTE
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

#ifndef _BRepBuilderAPI_ShapeModification_HeaderFile
#define _BRepBuilderAPI_ShapeModification_HeaderFile

//! Lists the possible types of modification to a shape
//! following a topological operation: Preserved, Deleted,
//! Trimmed, Merged or BoundaryModified.
//! This enumeration enables you to assign a "state" to the
//! different shapes that are on the list of operands for
//! each API function. The MakeShape class then uses this
//! to determine what has happened to the shapes which
//! constitute the list of operands.
enum BRepBuilderAPI_ShapeModification
{
  BRepBuilderAPI_Preserved,
  BRepBuilderAPI_Deleted,
  BRepBuilderAPI_Trimmed,
  BRepBuilderAPI_Merged,
  BRepBuilderAPI_BoundaryModified
};

#endif // _BRepBuilderAPI_ShapeModification_HeaderFile
