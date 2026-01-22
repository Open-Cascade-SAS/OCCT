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

//! @file TopOpeBRepDS_IndexedDataMapOfVertexPoint.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_Point,
//!             TopTools_ShapeMapHasher> directly instead.

#ifndef _TopOpeBRepDS_IndexedDataMapOfVertexPoint_hxx
#define _TopOpeBRepDS_IndexedDataMapOfVertexPoint_hxx

#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepDS_Point.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>

Standard_HEADER_DEPRECATED(
  "TopOpeBRepDS_IndexedDataMapOfVertexPoint.hxx is deprecated since OCCT 8.0.0. Use "
  "NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_Point, TopTools_ShapeMapHasher> directly.")

  Standard_DEPRECATED("TopOpeBRepDS_IndexedDataMapOfVertexPoint is deprecated, use "
                      "NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_Point, "
                      "TopTools_ShapeMapHasher> directly")
typedef NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepDS_Point, TopTools_ShapeMapHasher>
  TopOpeBRepDS_IndexedDataMapOfVertexPoint;

#endif // _TopOpeBRepDS_IndexedDataMapOfVertexPoint_hxx
