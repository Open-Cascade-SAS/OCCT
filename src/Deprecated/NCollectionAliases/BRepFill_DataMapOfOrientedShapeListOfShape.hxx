// Copyright (c) 2025 OPEN CASCADE SAS
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

//! @file BRepFill_DataMapOfOrientedShapeListOfShape.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _BRepFill_DataMapOfOrientedShapeListOfShape_hxx
#define _BRepFill_DataMapOfOrientedShapeListOfShape_hxx

#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("BRepFill_DataMapOfOrientedShapeListOfShape.hxx is deprecated since "
                           "OCCT 8.0.0. Use NCollection types directly.")

  Standard_DEPRECATED("BRepFill_DataMapOfOrientedShapeListOfShape is deprecated, use "
                      "NCollection_DataMap<TopoDS_Shape, TopTools_ListOfShape> directly")
typedef NCollection_DataMap<TopoDS_Shape, TopTools_ListOfShape>
  BRepFill_DataMapOfOrientedShapeListOfShape;
Standard_DEPRECATED(
  "BRepFill_DataMapIteratorOfDataMapOfOrientedShapeListOfShape is deprecated, use "
  "NCollection_DataMap<TopoDS_Shape, TopTools_ListOfShape>::Iterator directly")
typedef NCollection_DataMap<TopoDS_Shape, TopTools_ListOfShape>::Iterator
  BRepFill_DataMapIteratorOfDataMapOfOrientedShapeListOfShape;

#endif // _BRepFill_DataMapOfOrientedShapeListOfShape_hxx
