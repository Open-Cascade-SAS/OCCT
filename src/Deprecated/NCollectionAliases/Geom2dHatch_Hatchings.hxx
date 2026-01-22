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

//! @file Geom2dHatch_Hatchings.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _Geom2dHatch_Hatchings_hxx
#define _Geom2dHatch_Hatchings_hxx

#include <Standard_Macro.hxx>
#include <Standard_Integer.hxx>
#include <Geom2dHatch_Hatching.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("Geom2dHatch_Hatchings.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("Geom2dHatch_Hatchings is deprecated, use NCollection_DataMap<int, Geom2dHatch_Hatching> directly")
typedef NCollection_DataMap<int, Geom2dHatch_Hatching> Geom2dHatch_Hatchings;
Standard_DEPRECATED("Geom2dHatch_DataMapIteratorOfHatchings is deprecated, use NCollection_DataMap<int, Geom2dHatch_Hatching>::Iterator directly")
typedef NCollection_DataMap<int, Geom2dHatch_Hatching>::Iterator Geom2dHatch_DataMapIteratorOfHatchings;

#endif // _Geom2dHatch_Hatchings_hxx
