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

//! @file BinMDF_TypeADriverMap.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _BinMDF_TypeADriverMap_hxx
#define _BinMDF_TypeADriverMap_hxx

#include <Standard_Macro.hxx>
#include <Standard_Type.hxx>
#include <BinMDF_ADriver.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("BinMDF_TypeADriverMap.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("BinMDF_TypeADriverMap is deprecated, use NCollection_DataMap<opencascade::handle<Standard_Type>, opencascade::handle<BinMDF_ADriver>> directly")
typedef NCollection_DataMap<opencascade::handle<Standard_Type>, opencascade::handle<BinMDF_ADriver>> BinMDF_TypeADriverMap;
Standard_DEPRECATED("BinMDF_DataMapIteratorOfTypeADriverMap is deprecated, use NCollection_DataMap<opencascade::handle<Standard_Type>, opencascade::handle<BinMDF_ADriver>>::Iterator directly")
typedef NCollection_DataMap<opencascade::handle<Standard_Type>, opencascade::handle<BinMDF_ADriver>>::Iterator BinMDF_DataMapIteratorOfTypeADriverMap;

#endif // _BinMDF_TypeADriverMap_hxx
