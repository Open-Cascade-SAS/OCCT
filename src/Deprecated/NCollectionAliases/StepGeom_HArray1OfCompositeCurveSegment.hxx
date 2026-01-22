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

//! @file StepGeom_HArray1OfCompositeCurveSegment.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HArray1<opencascade::handle<StepGeom_CompositeCurveSegment>>
//!             directly instead.

#ifndef _StepGeom_HArray1OfCompositeCurveSegment_hxx
#define _StepGeom_HArray1OfCompositeCurveSegment_hxx

#include <Standard_Macro.hxx>
#include <StepGeom_Array1OfCompositeCurveSegment.hxx>
#include <NCollection_HArray1.hxx>

Standard_HEADER_DEPRECATED(
  "StepGeom_HArray1OfCompositeCurveSegment.hxx is deprecated since OCCT 8.0.0. Use "
  "NCollection_HArray1<opencascade::handle<StepGeom_CompositeCurveSegment>> directly.")

  Standard_DEPRECATED(
    "StepGeom_HArray1OfCompositeCurveSegment is deprecated, use "
    "NCollection_HArray1<opencascade::handle<StepGeom_CompositeCurveSegment>> directly")
typedef NCollection_HArray1<opencascade::handle<StepGeom_CompositeCurveSegment>>
  StepGeom_HArray1OfCompositeCurveSegment;

#endif // _StepGeom_HArray1OfCompositeCurveSegment_hxx
