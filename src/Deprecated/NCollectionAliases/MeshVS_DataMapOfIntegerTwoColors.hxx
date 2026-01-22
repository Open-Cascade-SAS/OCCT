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

//! @file MeshVS_DataMapOfIntegerTwoColors.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _MeshVS_DataMapOfIntegerTwoColors_hxx
#define _MeshVS_DataMapOfIntegerTwoColors_hxx

#include <Standard_Macro.hxx>
#include <Standard_Integer.hxx>
#include <MeshVS_TwoColors.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("MeshVS_DataMapOfIntegerTwoColors.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("MeshVS_DataMapOfIntegerTwoColors is deprecated, use NCollection_DataMap<int, MeshVS_TwoColors> directly")
typedef NCollection_DataMap<int, MeshVS_TwoColors> MeshVS_DataMapOfIntegerTwoColors;
Standard_DEPRECATED("MeshVS_DataMapIteratorOfDataMapOfIntegerTwoColors is deprecated, use NCollection_DataMap<int, MeshVS_TwoColors>::Iterator directly")
typedef NCollection_DataMap<int, MeshVS_TwoColors>::Iterator MeshVS_DataMapIteratorOfDataMapOfIntegerTwoColors;

#endif // _MeshVS_DataMapOfIntegerTwoColors_hxx
