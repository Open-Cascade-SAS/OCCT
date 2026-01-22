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

//! @file MoniTool_IndexedDataMapOfShapeTransient.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_IndexedDataMap<TopoDS_Shape,
//!             opencascade::handle<Standard_Transient>, TopTools_ShapeMapHasher> directly instead.

#ifndef _MoniTool_IndexedDataMapOfShapeTransient_hxx
#define _MoniTool_IndexedDataMapOfShapeTransient_hxx

#include <Standard_Macro.hxx>
#include <Standard_Transient.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>

Standard_HEADER_DEPRECATED(
  "MoniTool_IndexedDataMapOfShapeTransient.hxx is deprecated since OCCT 8.0.0. Use "
  "NCollection_IndexedDataMap<TopoDS_Shape, opencascade::handle<Standard_Transient>, "
  "TopTools_ShapeMapHasher> directly.")

  Standard_DEPRECATED("MoniTool_IndexedDataMapOfShapeTransient is deprecated, use "
                      "NCollection_IndexedDataMap<TopoDS_Shape, "
                      "opencascade::handle<Standard_Transient>, TopTools_ShapeMapHasher> directly")
typedef NCollection_IndexedDataMap<TopoDS_Shape,
                                   opencascade::handle<Standard_Transient>,
                                   TopTools_ShapeMapHasher>
  MoniTool_IndexedDataMapOfShapeTransient;

#endif // _MoniTool_IndexedDataMapOfShapeTransient_hxx
