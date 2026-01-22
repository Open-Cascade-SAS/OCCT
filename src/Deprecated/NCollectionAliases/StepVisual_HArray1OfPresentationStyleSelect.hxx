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

//! @file StepVisual_HArray1OfPresentationStyleSelect.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HArray1<StepVisual_PresentationStyleSelect> directly instead.

#ifndef _StepVisual_HArray1OfPresentationStyleSelect_hxx
#define _StepVisual_HArray1OfPresentationStyleSelect_hxx

#include <Standard_Macro.hxx>
#include <StepVisual_Array1OfPresentationStyleSelect.hxx>
#include <NCollection_HArray1.hxx>

Standard_HEADER_DEPRECATED("StepVisual_HArray1OfPresentationStyleSelect.hxx is deprecated since OCCT 8.0.0. Use NCollection_HArray1<StepVisual_PresentationStyleSelect> directly.")

Standard_DEPRECATED("StepVisual_HArray1OfPresentationStyleSelect is deprecated, use NCollection_HArray1<StepVisual_PresentationStyleSelect> directly")
typedef NCollection_HArray1<StepVisual_PresentationStyleSelect> StepVisual_HArray1OfPresentationStyleSelect;

#endif // _StepVisual_HArray1OfPresentationStyleSelect_hxx
