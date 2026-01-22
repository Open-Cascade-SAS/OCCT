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

//! @file LocOpe_DataMapOfShapePnt.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _LocOpe_DataMapOfShapePnt_hxx
#define _LocOpe_DataMapOfShapePnt_hxx

#include <Standard_Macro.hxx>
#include <gp_Pnt.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED(
  "LocOpe_DataMapOfShapePnt.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

  Standard_DEPRECATED("LocOpe_DataMapOfShapePnt is deprecated, use "
                      "NCollection_DataMap<TopoDS_Shape, gp_Pnt, TopTools_ShapeMapHasher> directly")
typedef NCollection_DataMap<TopoDS_Shape, gp_Pnt, TopTools_ShapeMapHasher> LocOpe_DataMapOfShapePnt;
Standard_DEPRECATED(
  "LocOpe_DataMapIteratorOfDataMapOfShapePnt is deprecated, use NCollection_DataMap<TopoDS_Shape, "
  "gp_Pnt, TopTools_ShapeMapHasher>::Iterator directly")
typedef NCollection_DataMap<TopoDS_Shape, gp_Pnt, TopTools_ShapeMapHasher>::Iterator
  LocOpe_DataMapIteratorOfDataMapOfShapePnt;

#endif // _LocOpe_DataMapOfShapePnt_hxx
