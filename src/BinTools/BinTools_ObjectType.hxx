// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _BinTools_ObjectType_HeaderFile
#define _BinTools_ObjectType_HeaderFile

//! Enumeration defining objects identifiers in the shape read/write format.
enum BinTools_ObjectType
{
  BinTools_ObjectType_Unknown = 0,
  BinTools_ObjectType_Reference8,   //!< 8-bits reference
  BinTools_ObjectType_Reference16,  //!< 16-bits reference
  BinTools_ObjectType_Reference32,  //!< 32-bits reference
  BinTools_ObjectType_Reference64,  //!< 64-bits reference
  BinTools_ObjectType_Location,
  BinTools_ObjectType_SimpleLocation,
  BinTools_ObjectType_EmptyLocation,
  BinTools_ObjectType_LocationEnd,
  BinTools_ObjectType_Curve,
  BinTools_ObjectType_EmptyCurve,
  BinTools_ObjectType_Curve2d,
  BinTools_ObjectType_EmptyCurve2d,
  BinTools_ObjectType_Surface,
  BinTools_ObjectType_EmptySurface,
  BinTools_ObjectType_Polygon3d,
  BinTools_ObjectType_EmptyPolygon3d,
  BinTools_ObjectType_PolygonOnTriangulation,
  BinTools_ObjectType_EmptyPolygonOnTriangulation,
  BinTools_ObjectType_Triangulation,
  BinTools_ObjectType_EmptyTriangulation,
  BinTools_ObjectType_EmptyShape = 198, //!< identifier of the null shape
  BinTools_ObjectType_EndShape = 199, //!< identifier of the shape record end
  // here is the space for TopAbs_ShapeEnum+Orientation types
};

#endif // _BinTools_ObjectType_HeaderFile
