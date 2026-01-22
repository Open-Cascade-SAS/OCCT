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

//! @file Storage_MapOfPers.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _Storage_MapOfPers_hxx
#define _Storage_MapOfPers_hxx

#include <Standard_Macro.hxx>
#include <Storage_Root.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED(
  "Storage_MapOfPers.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

  Standard_DEPRECATED(
    "Storage_MapOfPers is deprecated, use NCollection_DataMap<TCollection_AsciiString, "
    "opencascade::handle<Storage_Root>> directly")
typedef NCollection_DataMap<TCollection_AsciiString, opencascade::handle<Storage_Root>>
  Storage_MapOfPers;
Standard_DEPRECATED("Storage_DataMapIteratorOfMapOfPers is deprecated, use "
                    "NCollection_DataMap<TCollection_AsciiString, "
                    "opencascade::handle<Storage_Root>>::Iterator directly")
typedef NCollection_DataMap<TCollection_AsciiString, opencascade::handle<Storage_Root>>::Iterator
  Storage_DataMapIteratorOfMapOfPers;

#endif // _Storage_MapOfPers_hxx
