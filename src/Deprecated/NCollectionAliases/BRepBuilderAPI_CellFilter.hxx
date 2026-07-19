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

//! @file BRepBuilderAPI_CellFilter.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_CellFilter<BRepBuilderAPI_VertexInspector> directly instead.

#ifndef _BRepBuilderAPI_CellFilter_hxx
#define _BRepBuilderAPI_CellFilter_hxx

#include <Standard_Macro.hxx>
#include <BRepBuilderAPI_VertexInspector.hxx>

Standard_HEADER_DEPRECATED("BRepBuilderAPI_CellFilter.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection_CellFilter<BRepBuilderAPI_VertexInspector> directly.")

  Standard_DEPRECATED("BRepBuilderAPI_CellFilter is deprecated, use "
                      "NCollection_CellFilter<BRepBuilderAPI_VertexInspector> directly")
typedef NCollection_CellFilter<BRepBuilderAPI_VertexInspector> BRepBuilderAPI_CellFilter;

#endif // _BRepBuilderAPI_CellFilter_hxx
