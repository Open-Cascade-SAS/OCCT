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

//! @file StepElement_Array1OfHSequenceOfCurveElementPurposeMember.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array1<opencascade::handle<StepElement_HSequenceOfCurveElementPurposeMember>> directly instead.

#ifndef _StepElement_Array1OfHSequenceOfCurveElementPurposeMember_hxx
#define _StepElement_Array1OfHSequenceOfCurveElementPurposeMember_hxx

#include <Standard_Macro.hxx>
#include <StepElement_HSequenceOfCurveElementPurposeMember.hxx>
#include <NCollection_Array1.hxx>

Standard_HEADER_DEPRECATED("StepElement_Array1OfHSequenceOfCurveElementPurposeMember.hxx is deprecated since OCCT 8.0.0. Use NCollection_Array1<opencascade::handle<StepElement_HSequenceOfCurveElementPurposeMember>> directly.")

Standard_DEPRECATED("StepElement_Array1OfHSequenceOfCurveElementPurposeMember is deprecated, use NCollection_Array1<opencascade::handle<StepElement_HSequenceOfCurveElementPurposeMember>> directly")
typedef NCollection_Array1<opencascade::handle<StepElement_HSequenceOfCurveElementPurposeMember>> StepElement_Array1OfHSequenceOfCurveElementPurposeMember;

#endif // _StepElement_Array1OfHSequenceOfCurveElementPurposeMember_hxx
