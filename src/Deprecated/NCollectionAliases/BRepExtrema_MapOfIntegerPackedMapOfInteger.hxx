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

//! @file BRepExtrema_MapOfIntegerPackedMapOfInteger.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_DataMap<int, TColStd_PackedMapOfInteger> directly instead.

#ifndef _BRepExtrema_MapOfIntegerPackedMapOfInteger_hxx
#define _BRepExtrema_MapOfIntegerPackedMapOfInteger_hxx

#include <Standard_Macro.hxx>
#include <NCollection_DataMap.hxx>
#include <TColStd_PackedMapOfInteger.hxx>

Standard_HEADER_DEPRECATED("BRepExtrema_MapOfIntegerPackedMapOfInteger.hxx is deprecated since OCCT 8.0.0. Use NCollection_DataMap<int, TColStd_PackedMapOfInteger> directly.")

Standard_DEPRECATED("BRepExtrema_MapOfIntegerPackedMapOfInteger is deprecated, use NCollection_DataMap<int, TColStd_PackedMapOfInteger> directly")
typedef NCollection_DataMap<int, TColStd_PackedMapOfInteger> BRepExtrema_MapOfIntegerPackedMapOfInteger;

#endif // _BRepExtrema_MapOfIntegerPackedMapOfInteger_hxx
