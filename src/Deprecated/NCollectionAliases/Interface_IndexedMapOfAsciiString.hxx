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

//! @file Interface_IndexedMapOfAsciiString.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_IndexedMap<TCollection_AsciiString> directly instead.

#ifndef _Interface_IndexedMapOfAsciiString_hxx
#define _Interface_IndexedMapOfAsciiString_hxx

#include <Standard_Macro.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_IndexedMap.hxx>

Standard_HEADER_DEPRECATED("Interface_IndexedMapOfAsciiString.hxx is deprecated since OCCT 8.0.0. "
                           "Use NCollection_IndexedMap<TCollection_AsciiString> directly.")

  Standard_DEPRECATED("Interface_IndexedMapOfAsciiString is deprecated, use "
                      "NCollection_IndexedMap<TCollection_AsciiString> directly")
typedef NCollection_IndexedMap<TCollection_AsciiString> Interface_IndexedMapOfAsciiString;

#endif // _Interface_IndexedMapOfAsciiString_hxx
