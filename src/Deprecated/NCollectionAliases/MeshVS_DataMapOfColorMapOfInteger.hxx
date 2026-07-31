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

//! @file MeshVS_DataMapOfColorMapOfInteger.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _MeshVS_DataMapOfColorMapOfInteger_hxx
#define _MeshVS_DataMapOfColorMapOfInteger_hxx

#include <Standard_Macro.hxx>
#include <Quantity_Color.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("MeshVS_DataMapOfColorMapOfInteger.hxx is deprecated since OCCT 8.0.0. "
                           "Use NCollection types directly.")

  Standard_DEPRECATED("MeshVS_DataMapOfColorMapOfInteger is deprecated, use "
                      "NCollection_DataMap<Quantity_Color, TColStd_MapOfInteger> directly")
typedef NCollection_DataMap<Quantity_Color, TColStd_MapOfInteger> MeshVS_DataMapOfColorMapOfInteger;
Standard_DEPRECATED("MeshVS_DataMapIteratorOfDataMapOfColorMapOfInteger is deprecated, use "
                    "NCollection_DataMap<Quantity_Color, TColStd_MapOfInteger>::Iterator directly")
typedef NCollection_DataMap<Quantity_Color, TColStd_MapOfInteger>::Iterator
  MeshVS_DataMapIteratorOfDataMapOfColorMapOfInteger;

#endif // _MeshVS_DataMapOfColorMapOfInteger_hxx
