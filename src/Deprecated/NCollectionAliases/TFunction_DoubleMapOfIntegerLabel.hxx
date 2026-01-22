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

//! @file TFunction_DoubleMapOfIntegerLabel.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _TFunction_DoubleMapOfIntegerLabel_hxx
#define _TFunction_DoubleMapOfIntegerLabel_hxx

#include <Standard_Macro.hxx>
#include <Standard_Integer.hxx>
#include <TDF_Label.hxx>
#include <NCollection_DoubleMap.hxx>

Standard_HEADER_DEPRECATED("TFunction_DoubleMapOfIntegerLabel.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("TFunction_DoubleMapOfIntegerLabel is deprecated, use NCollection_DoubleMap<int, TDF_Label> directly")
typedef NCollection_DoubleMap<int, TDF_Label> TFunction_DoubleMapOfIntegerLabel;
Standard_DEPRECATED("TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel is deprecated, use NCollection_DoubleMap<int, TDF_Label>::Iterator directly")
typedef NCollection_DoubleMap<int, TDF_Label>::Iterator TFunction_DoubleMapIteratorOfDoubleMapOfIntegerLabel;

#endif // _TFunction_DoubleMapOfIntegerLabel_hxx
