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

//! @file StepDimTol_HArray1OfGeometricToleranceModifier.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HArray1<StepDimTol_GeometricToleranceModifier> directly instead.

#ifndef _StepDimTol_HArray1OfGeometricToleranceModifier_hxx
#define _StepDimTol_HArray1OfGeometricToleranceModifier_hxx

#include <Standard_Macro.hxx>
#include <StepDimTol_Array1OfGeometricToleranceModifier.hxx>
#include <NCollection_HArray1.hxx>

Standard_HEADER_DEPRECATED(
  "StepDimTol_HArray1OfGeometricToleranceModifier.hxx is deprecated since OCCT 8.0.0. Use "
  "NCollection_HArray1<StepDimTol_GeometricToleranceModifier> directly.")

  Standard_DEPRECATED("StepDimTol_HArray1OfGeometricToleranceModifier is deprecated, use "
                      "NCollection_HArray1<StepDimTol_GeometricToleranceModifier> directly")
typedef NCollection_HArray1<StepDimTol_GeometricToleranceModifier>
  StepDimTol_HArray1OfGeometricToleranceModifier;

#endif // _StepDimTol_HArray1OfGeometricToleranceModifier_hxx
