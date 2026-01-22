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

//! @file XmlMDF_MapOfDriver.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _XmlMDF_MapOfDriver_hxx
#define _XmlMDF_MapOfDriver_hxx

#include <Standard_Macro.hxx>
#include <TCollection_AsciiString.hxx>
#include <XmlMDF_ADriver.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("XmlMDF_MapOfDriver.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("XmlMDF_MapOfDriver is deprecated, use NCollection_DataMap<TCollection_AsciiString, opencascade::handle<XmlMDF_ADriver>> directly")
typedef NCollection_DataMap<TCollection_AsciiString, opencascade::handle<XmlMDF_ADriver>> XmlMDF_MapOfDriver;
Standard_DEPRECATED("XmlMDF_DataMapIteratorOfMapOfDriver is deprecated, use NCollection_DataMap<TCollection_AsciiString, opencascade::handle<XmlMDF_ADriver>>::Iterator directly")
typedef NCollection_DataMap<TCollection_AsciiString, opencascade::handle<XmlMDF_ADriver>>::Iterator XmlMDF_DataMapIteratorOfMapOfDriver;

#endif // _XmlMDF_MapOfDriver_hxx
