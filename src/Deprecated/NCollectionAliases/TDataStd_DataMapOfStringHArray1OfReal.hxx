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

//! @file TDataStd_DataMapOfStringHArray1OfReal.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _TDataStd_DataMapOfStringHArray1OfReal_hxx
#define _TDataStd_DataMapOfStringHArray1OfReal_hxx

#include <Standard_Macro.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("TDataStd_DataMapOfStringHArray1OfReal.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("TDataStd_DataMapOfStringHArray1OfReal is deprecated, use NCollection_DataMap<TCollection_ExtendedString, opencascade::handle<TColStd_HArray1OfReal>> directly")
typedef NCollection_DataMap<TCollection_ExtendedString, opencascade::handle<TColStd_HArray1OfReal>> TDataStd_DataMapOfStringHArray1OfReal;
Standard_DEPRECATED("TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfReal is deprecated, use NCollection_DataMap<TCollection_ExtendedString, opencascade::handle<TColStd_HArray1OfReal>>::Iterator directly")
typedef NCollection_DataMap<TCollection_ExtendedString, opencascade::handle<TColStd_HArray1OfReal>>::Iterator TDataStd_DataMapIteratorOfDataMapOfStringHArray1OfReal;

#endif // _TDataStd_DataMapOfStringHArray1OfReal_hxx
