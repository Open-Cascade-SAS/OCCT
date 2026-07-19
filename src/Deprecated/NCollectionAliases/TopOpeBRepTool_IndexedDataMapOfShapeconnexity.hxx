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

//! @file TopOpeBRepTool_IndexedDataMapOfShapeconnexity.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepTool_connexity,
//!             TopTools_ShapeMapHasher> directly instead.

#ifndef _TopOpeBRepTool_IndexedDataMapOfShapeconnexity_hxx
#define _TopOpeBRepTool_IndexedDataMapOfShapeconnexity_hxx

#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_connexity.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>

Standard_HEADER_DEPRECATED("TopOpeBRepTool_IndexedDataMapOfShapeconnexity.hxx is deprecated since "
                           "OCCT 8.0.0. Use NCollection_IndexedDataMap<TopoDS_Shape, "
                           "TopOpeBRepTool_connexity, TopTools_ShapeMapHasher> directly.")

  Standard_DEPRECATED("TopOpeBRepTool_IndexedDataMapOfShapeconnexity is deprecated, use "
                      "NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepTool_connexity, "
                      "TopTools_ShapeMapHasher> directly")
typedef NCollection_IndexedDataMap<TopoDS_Shape, TopOpeBRepTool_connexity, TopTools_ShapeMapHasher>
  TopOpeBRepTool_IndexedDataMapOfShapeconnexity;

#endif // _TopOpeBRepTool_IndexedDataMapOfShapeconnexity_hxx
