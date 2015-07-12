// Created on: 1993-12-15
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

#ifndef _BRepLib_ShapeModification_HeaderFile
#define _BRepLib_ShapeModification_HeaderFile

//! Modification type after a topologic operation.
enum BRepLib_ShapeModification
{
BRepLib_Preserved,
BRepLib_Deleted,
BRepLib_Trimmed,
BRepLib_Merged,
BRepLib_BoundaryModified
};

#endif // _BRepLib_ShapeModification_HeaderFile
