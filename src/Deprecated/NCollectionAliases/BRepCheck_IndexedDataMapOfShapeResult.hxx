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

//! @file BRepCheck_IndexedDataMapOfShapeResult.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_IndexedDataMap<TopoDS_Shape, opencascade::handle<BRepCheck_Result>>
//!             directly instead.

#ifndef _BRepCheck_IndexedDataMapOfShapeResult_hxx
#define _BRepCheck_IndexedDataMapOfShapeResult_hxx

#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepCheck_Result.hxx>
#include <NCollection_IndexedDataMap.hxx>

Standard_HEADER_DEPRECATED(
  "BRepCheck_IndexedDataMapOfShapeResult.hxx is deprecated since OCCT 8.0.0. Use "
  "NCollection_IndexedDataMap<TopoDS_Shape, opencascade::handle<BRepCheck_Result>> directly.")

  Standard_DEPRECATED(
    "BRepCheck_IndexedDataMapOfShapeResult is deprecated, use "
    "NCollection_IndexedDataMap<TopoDS_Shape, opencascade::handle<BRepCheck_Result>> directly")
typedef NCollection_IndexedDataMap<TopoDS_Shape, opencascade::handle<BRepCheck_Result>>
  BRepCheck_IndexedDataMapOfShapeResult;

#endif // _BRepCheck_IndexedDataMapOfShapeResult_hxx
