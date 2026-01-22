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

//! @file StepAP214_Array1OfDateAndTimeItem.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array1<StepAP214_DateAndTimeItem> directly instead.

#ifndef _StepAP214_Array1OfDateAndTimeItem_hxx
#define _StepAP214_Array1OfDateAndTimeItem_hxx

#include <Standard_Macro.hxx>
#include <StepAP214_DateAndTimeItem.hxx>
#include <NCollection_Array1.hxx>

Standard_HEADER_DEPRECATED("StepAP214_Array1OfDateAndTimeItem.hxx is deprecated since OCCT 8.0.0. Use NCollection_Array1<StepAP214_DateAndTimeItem> directly.")

Standard_DEPRECATED("StepAP214_Array1OfDateAndTimeItem is deprecated, use NCollection_Array1<StepAP214_DateAndTimeItem> directly")
typedef NCollection_Array1<StepAP214_DateAndTimeItem> StepAP214_Array1OfDateAndTimeItem;

#endif // _StepAP214_Array1OfDateAndTimeItem_hxx
