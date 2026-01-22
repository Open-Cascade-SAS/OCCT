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

//! @file StepGeom_Array1OfBoundaryCurve.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array1<opencascade::handle<StepGeom_BoundaryCurve>> directly
//!             instead.

#ifndef _StepGeom_Array1OfBoundaryCurve_hxx
#define _StepGeom_Array1OfBoundaryCurve_hxx

#include <Standard_Macro.hxx>
#include <StepGeom_BoundaryCurve.hxx>
#include <NCollection_Array1.hxx>

Standard_HEADER_DEPRECATED(
  "StepGeom_Array1OfBoundaryCurve.hxx is deprecated since OCCT 8.0.0. Use "
  "NCollection_Array1<opencascade::handle<StepGeom_BoundaryCurve>> directly.")

  Standard_DEPRECATED("StepGeom_Array1OfBoundaryCurve is deprecated, use "
                      "NCollection_Array1<opencascade::handle<StepGeom_BoundaryCurve>> directly")
typedef NCollection_Array1<opencascade::handle<StepGeom_BoundaryCurve>>
  StepGeom_Array1OfBoundaryCurve;

#endif // _StepGeom_Array1OfBoundaryCurve_hxx
