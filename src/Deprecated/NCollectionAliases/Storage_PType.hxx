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

//! @file Storage_PType.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_IndexedDataMap<TCollection_AsciiString, int> directly instead.

#ifndef _Storage_PType_hxx
#define _Storage_PType_hxx

#include <Standard_Macro.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_IndexedDataMap.hxx>

Standard_HEADER_DEPRECATED("Storage_PType.hxx is deprecated since OCCT 8.0.0. Use NCollection_IndexedDataMap<TCollection_AsciiString, int> directly.")

Standard_DEPRECATED("Storage_PType is deprecated, use NCollection_IndexedDataMap<TCollection_AsciiString, int> directly")
typedef NCollection_IndexedDataMap<TCollection_AsciiString, int> Storage_PType;

#endif // _Storage_PType_hxx
