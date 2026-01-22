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

//! @file StepBasic_Array1OfDerivedUnitElement.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array1<opencascade::handle<StepBasic_DerivedUnitElement>> directly instead.

#ifndef _StepBasic_Array1OfDerivedUnitElement_hxx
#define _StepBasic_Array1OfDerivedUnitElement_hxx

#include <Standard_Macro.hxx>
#include <StepBasic_DerivedUnitElement.hxx>
#include <NCollection_Array1.hxx>

Standard_HEADER_DEPRECATED("StepBasic_Array1OfDerivedUnitElement.hxx is deprecated since OCCT 8.0.0. Use NCollection_Array1<opencascade::handle<StepBasic_DerivedUnitElement>> directly.")

Standard_DEPRECATED("StepBasic_Array1OfDerivedUnitElement is deprecated, use NCollection_Array1<opencascade::handle<StepBasic_DerivedUnitElement>> directly")
typedef NCollection_Array1<opencascade::handle<StepBasic_DerivedUnitElement>> StepBasic_Array1OfDerivedUnitElement;

#endif // _StepBasic_Array1OfDerivedUnitElement_hxx
