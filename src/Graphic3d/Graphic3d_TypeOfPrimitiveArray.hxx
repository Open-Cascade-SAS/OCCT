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

#ifndef _Graphic3d_TypeOfPrimitiveArray_HeaderFile
#define _Graphic3d_TypeOfPrimitiveArray_HeaderFile

//! The type of primitive array in a group in a structure.
enum Graphic3d_TypeOfPrimitiveArray
{
Graphic3d_TOPA_UNDEFINED,
Graphic3d_TOPA_POINTS,
Graphic3d_TOPA_POLYLINES,
Graphic3d_TOPA_SEGMENTS,
Graphic3d_TOPA_POLYGONS,
Graphic3d_TOPA_TRIANGLES,
Graphic3d_TOPA_QUADRANGLES,
Graphic3d_TOPA_TRIANGLESTRIPS,
Graphic3d_TOPA_QUADRANGLESTRIPS,
Graphic3d_TOPA_TRIANGLEFANS
};

#endif // _Graphic3d_TypeOfPrimitiveArray_HeaderFile
