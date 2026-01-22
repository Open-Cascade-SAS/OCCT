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

//! @file StepAP203_Array1OfSpecifiedItem.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array1<StepAP203_SpecifiedItem> directly instead.

#ifndef _StepAP203_Array1OfSpecifiedItem_hxx
#define _StepAP203_Array1OfSpecifiedItem_hxx

#include <Standard_Macro.hxx>
#include <StepAP203_SpecifiedItem.hxx>
#include <NCollection_Array1.hxx>

Standard_HEADER_DEPRECATED("StepAP203_Array1OfSpecifiedItem.hxx is deprecated since OCCT 8.0.0. "
                           "Use NCollection_Array1<StepAP203_SpecifiedItem> directly.")

  Standard_DEPRECATED("StepAP203_Array1OfSpecifiedItem is deprecated, use "
                      "NCollection_Array1<StepAP203_SpecifiedItem> directly")
typedef NCollection_Array1<StepAP203_SpecifiedItem> StepAP203_Array1OfSpecifiedItem;

#endif // _StepAP203_Array1OfSpecifiedItem_hxx
