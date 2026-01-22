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

//! @file AppDef_HArray1OfMultiPointConstraint.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HArray1<AppDef_MultiPointConstraint> directly instead.

#ifndef _AppDef_HArray1OfMultiPointConstraint_hxx
#define _AppDef_HArray1OfMultiPointConstraint_hxx

#include <Standard_Macro.hxx>
#include <AppDef_MultiPointConstraint.hxx>
#include <AppDef_Array1OfMultiPointConstraint.hxx>
#include <NCollection_HArray1.hxx>

Standard_HEADER_DEPRECATED("AppDef_HArray1OfMultiPointConstraint.hxx is deprecated since OCCT 8.0.0. Use NCollection_HArray1<AppDef_MultiPointConstraint> directly.")

Standard_DEPRECATED("AppDef_HArray1OfMultiPointConstraint is deprecated, use NCollection_HArray1<AppDef_MultiPointConstraint> directly")
typedef NCollection_HArray1<AppDef_MultiPointConstraint> AppDef_HArray1OfMultiPointConstraint;

#endif // _AppDef_HArray1OfMultiPointConstraint_hxx
