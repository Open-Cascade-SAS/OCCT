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

//! @file TopOpeBRepBuild_IndexedDataMapOfShapeVertexInfo.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepBuild_VertexInfo, TopTools_ShapeMapHasher> directly instead.

#ifndef _TopOpeBRepBuild_IndexedDataMapOfShapeVertexInfo_hxx
#define _TopOpeBRepBuild_IndexedDataMapOfShapeVertexInfo_hxx

#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_VertexInfo.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>

Standard_HEADER_DEPRECATED("TopOpeBRepBuild_IndexedDataMapOfShapeVertexInfo.hxx is deprecated since OCCT 8.0.0. Use NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepBuild_VertexInfo, TopTools_ShapeMapHasher> directly.")

Standard_DEPRECATED("TopOpeBRepBuild_IndexedDataMapOfShapeVertexInfo is deprecated, use NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepBuild_VertexInfo, TopTools_ShapeMapHasher> directly")
typedef NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepBuild_VertexInfo, TopTools_ShapeMapHasher> TopOpeBRepBuild_IndexedDataMapOfShapeVertexInfo;

#endif // _TopOpeBRepBuild_IndexedDataMapOfShapeVertexInfo_hxx
