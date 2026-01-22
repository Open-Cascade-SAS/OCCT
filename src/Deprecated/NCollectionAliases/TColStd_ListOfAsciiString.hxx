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

//! @file TColStd_ListOfAsciiString.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _TColStd_ListOfAsciiString_hxx
#define _TColStd_ListOfAsciiString_hxx

#include <Standard_Macro.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_List.hxx>

Standard_HEADER_DEPRECATED(
  "TColStd_ListOfAsciiString.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

  Standard_DEPRECATED("TColStd_ListOfAsciiString is deprecated, use "
                      "NCollection_List<TCollection_AsciiString> directly")
typedef NCollection_List<TCollection_AsciiString> TColStd_ListOfAsciiString;
Standard_DEPRECATED("TColStd_ListIteratorOfListOfAsciiString is deprecated, use "
                    "NCollection_List<TCollection_AsciiString>::Iterator directly")
typedef NCollection_List<TCollection_AsciiString>::Iterator TColStd_ListIteratorOfListOfAsciiString;

#endif // _TColStd_ListOfAsciiString_hxx
