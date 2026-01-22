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

//! @file TColStd_DataMapOfAsciiStringInteger.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _TColStd_DataMapOfAsciiStringInteger_hxx
#define _TColStd_DataMapOfAsciiStringInteger_hxx

#include <Standard_Macro.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("TColStd_DataMapOfAsciiStringInteger.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("TColStd_DataMapOfAsciiStringInteger is deprecated, use NCollection_DataMap<TCollection_AsciiString, int> directly")
typedef NCollection_DataMap<TCollection_AsciiString, int> TColStd_DataMapOfAsciiStringInteger;
Standard_DEPRECATED("TColStd_DataMapIteratorOfDataMapOfAsciiStringInteger is deprecated, use NCollection_DataMap<TCollection_AsciiString, int>::Iterator directly")
typedef NCollection_DataMap<TCollection_AsciiString, int>::Iterator TColStd_DataMapIteratorOfDataMapOfAsciiStringInteger;

#endif // _TColStd_DataMapOfAsciiStringInteger_hxx
