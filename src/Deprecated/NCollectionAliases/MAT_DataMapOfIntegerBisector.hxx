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

//! @file MAT_DataMapOfIntegerBisector.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _MAT_DataMapOfIntegerBisector_hxx
#define _MAT_DataMapOfIntegerBisector_hxx

#include <Standard_Macro.hxx>
#include <Standard_Integer.hxx>
#include <MAT_Bisector.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("MAT_DataMapOfIntegerBisector.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection types directly.")

  Standard_DEPRECATED("MAT_DataMapOfIntegerBisector is deprecated, use NCollection_DataMap<int, "
                      "opencascade::handle<MAT_Bisector>> directly")
typedef NCollection_DataMap<int, opencascade::handle<MAT_Bisector>> MAT_DataMapOfIntegerBisector;
Standard_DEPRECATED(
  "MAT_DataMapIteratorOfDataMapOfIntegerBisector is deprecated, use NCollection_DataMap<int, "
  "opencascade::handle<MAT_Bisector>>::Iterator directly")
typedef NCollection_DataMap<int, opencascade::handle<MAT_Bisector>>::Iterator
  MAT_DataMapIteratorOfDataMapOfIntegerBisector;

#endif // _MAT_DataMapOfIntegerBisector_hxx
