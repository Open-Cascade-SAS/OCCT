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

//! @file Draft_IndexedDataMapOfEdgeEdgeInfo.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_IndexedDataMap<TopoDS_Edge, Draft_EdgeInfo, TopTools_ShapeMapHasher> directly instead.

#ifndef _Draft_IndexedDataMapOfEdgeEdgeInfo_hxx
#define _Draft_IndexedDataMapOfEdgeEdgeInfo_hxx

#include <Standard_Macro.hxx>
#include <TopoDS_Edge.hxx>
#include <Draft_EdgeInfo.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>

Standard_HEADER_DEPRECATED("Draft_IndexedDataMapOfEdgeEdgeInfo.hxx is deprecated since OCCT 8.0.0. Use NCollection_IndexedDataMap<TopoDS_Edge, Draft_EdgeInfo, TopTools_ShapeMapHasher> directly.")

Standard_DEPRECATED("Draft_IndexedDataMapOfEdgeEdgeInfo is deprecated, use NCollection_IndexedDataMap<TopoDS_Edge, Draft_EdgeInfo, TopTools_ShapeMapHasher> directly")
typedef NCollection_IndexedDataMap<TopoDS_Edge, Draft_EdgeInfo, TopTools_ShapeMapHasher> Draft_IndexedDataMapOfEdgeEdgeInfo;

#endif // _Draft_IndexedDataMapOfEdgeEdgeInfo_hxx
