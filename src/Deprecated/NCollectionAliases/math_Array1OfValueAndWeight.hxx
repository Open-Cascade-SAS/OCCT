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

//! @file math_Array1OfValueAndWeight.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array1<math_ValueAndWeight> directly instead.

#ifndef _math_Array1OfValueAndWeight_hxx
#define _math_Array1OfValueAndWeight_hxx

#include <Standard_Macro.hxx>
#include <math_ValueAndWeight.hxx>
#include <NCollection_Array1.hxx>

Standard_HEADER_DEPRECATED("math_Array1OfValueAndWeight.hxx is deprecated since OCCT 8.0.0. Use NCollection_Array1<math_ValueAndWeight> directly.")

Standard_DEPRECATED("math_Array1OfValueAndWeight is deprecated, use NCollection_Array1<math_ValueAndWeight> directly")
typedef NCollection_Array1<math_ValueAndWeight> math_Array1OfValueAndWeight;

#endif // _math_Array1OfValueAndWeight_hxx
