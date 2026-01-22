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

//! @file Interface_HArray1OfHAsciiString.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HArray1<opencascade::handle<TCollection_HAsciiString>> directly
//!             instead.

#ifndef _Interface_HArray1OfHAsciiString_hxx
#define _Interface_HArray1OfHAsciiString_hxx

#include <Standard_Macro.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Interface_Array1OfHAsciiString.hxx>
#include <NCollection_HArray1.hxx>

Standard_HEADER_DEPRECATED(
  "Interface_HArray1OfHAsciiString.hxx is deprecated since OCCT 8.0.0. Use "
  "NCollection_HArray1<opencascade::handle<TCollection_HAsciiString>> directly.")

  Standard_DEPRECATED("Interface_HArray1OfHAsciiString is deprecated, use "
                      "NCollection_HArray1<opencascade::handle<TCollection_HAsciiString>> directly")
typedef NCollection_HArray1<opencascade::handle<TCollection_HAsciiString>>
  Interface_HArray1OfHAsciiString;

#endif // _Interface_HArray1OfHAsciiString_hxx
