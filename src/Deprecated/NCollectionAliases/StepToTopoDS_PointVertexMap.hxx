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

//! @file StepToTopoDS_PointVertexMap.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _StepToTopoDS_PointVertexMap_hxx
#define _StepToTopoDS_PointVertexMap_hxx

#include <Standard_Macro.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <TopoDS_Vertex.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("StepToTopoDS_PointVertexMap.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("StepToTopoDS_PointVertexMap is deprecated, use NCollection_DataMap<opencascade::handle<StepGeom_CartesianPoint>, TopoDS_Vertex> directly")
typedef NCollection_DataMap<opencascade::handle<StepGeom_CartesianPoint>, TopoDS_Vertex> StepToTopoDS_PointVertexMap;
Standard_DEPRECATED("StepToTopoDS_DataMapIteratorOfPointVertexMap is deprecated, use NCollection_DataMap<opencascade::handle<StepGeom_CartesianPoint>, TopoDS_Vertex>::Iterator directly")
typedef NCollection_DataMap<opencascade::handle<StepGeom_CartesianPoint>, TopoDS_Vertex>::Iterator StepToTopoDS_DataMapIteratorOfPointVertexMap;

#endif // _StepToTopoDS_PointVertexMap_hxx
