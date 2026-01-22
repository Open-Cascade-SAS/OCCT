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

//! @file StepFEA_HSequenceOfCurve3dElementProperty.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HSequence<opencascade::handle<StepFEA_Curve3dElementProperty>>
//!             directly instead.

#ifndef _StepFEA_HSequenceOfCurve3dElementProperty_hxx
#define _StepFEA_HSequenceOfCurve3dElementProperty_hxx

#include <Standard_Macro.hxx>
#include <StepFEA_Curve3dElementProperty.hxx>
#include <StepFEA_SequenceOfCurve3dElementProperty.hxx>
#include <NCollection_HSequence.hxx>

Standard_HEADER_DEPRECATED(
  "StepFEA_HSequenceOfCurve3dElementProperty.hxx is deprecated since OCCT 8.0.0. Use "
  "NCollection_HSequence<opencascade::handle<StepFEA_Curve3dElementProperty>> directly.")

  Standard_DEPRECATED(
    "StepFEA_HSequenceOfCurve3dElementProperty is deprecated, use "
    "NCollection_HSequence<opencascade::handle<StepFEA_Curve3dElementProperty>> directly")
typedef NCollection_HSequence<opencascade::handle<StepFEA_Curve3dElementProperty>>
  StepFEA_HSequenceOfCurve3dElementProperty;

#endif // _StepFEA_HSequenceOfCurve3dElementProperty_hxx
