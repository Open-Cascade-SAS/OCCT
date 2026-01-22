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

//! @file Extrema_Array2OfPOnCurv.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array2<Extrema_POnCurv> directly instead.

#ifndef _Extrema_Array2OfPOnCurv_hxx
#define _Extrema_Array2OfPOnCurv_hxx

#include <Standard_Macro.hxx>
#include <Extrema_POnCurv.hxx>
#include <NCollection_Array2.hxx>

Standard_HEADER_DEPRECATED("Extrema_Array2OfPOnCurv.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection_Array2<Extrema_POnCurv> directly.")

  Standard_DEPRECATED(
    "Extrema_Array2OfPOnCurv is deprecated, use NCollection_Array2<Extrema_POnCurv> directly")
typedef NCollection_Array2<Extrema_POnCurv> Extrema_Array2OfPOnCurv;

#endif // _Extrema_Array2OfPOnCurv_hxx
