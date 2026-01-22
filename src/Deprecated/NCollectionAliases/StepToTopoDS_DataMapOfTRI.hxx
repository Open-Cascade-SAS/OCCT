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

//! @file StepToTopoDS_DataMapOfTRI.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _StepToTopoDS_DataMapOfTRI_hxx
#define _StepToTopoDS_DataMapOfTRI_hxx

#include <Standard_Macro.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED(
  "StepToTopoDS_DataMapOfTRI.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

  Standard_DEPRECATED("StepToTopoDS_DataMapOfTRI is deprecated, use "
                      "NCollection_DataMap<opencascade::handle<StepShape_"
                      "TopologicalRepresentationItem>, TopoDS_Shape> directly")
typedef NCollection_DataMap<opencascade::handle<StepShape_TopologicalRepresentationItem>,
                            TopoDS_Shape>
  StepToTopoDS_DataMapOfTRI;
Standard_DEPRECATED("StepToTopoDS_DataMapIteratorOfDataMapOfTRI is deprecated, use "
                    "NCollection_DataMap<opencascade::handle<StepShape_"
                    "TopologicalRepresentationItem>, TopoDS_Shape>::Iterator directly")
typedef NCollection_DataMap<opencascade::handle<StepShape_TopologicalRepresentationItem>,
                            TopoDS_Shape>::Iterator StepToTopoDS_DataMapIteratorOfDataMapOfTRI;

#endif // _StepToTopoDS_DataMapOfTRI_hxx
