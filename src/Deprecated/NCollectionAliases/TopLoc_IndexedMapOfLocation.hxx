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

//! @file TopLoc_IndexedMapOfLocation.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_IndexedMap<TopLoc_Location> directly instead.

#ifndef _TopLoc_IndexedMapOfLocation_hxx
#define _TopLoc_IndexedMapOfLocation_hxx

#include <Standard_Macro.hxx>
#include <TopLoc_Location.hxx>
#include <NCollection_IndexedMap.hxx>

Standard_HEADER_DEPRECATED("TopLoc_IndexedMapOfLocation.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection_IndexedMap<TopLoc_Location> directly.")

  Standard_DEPRECATED("TopLoc_IndexedMapOfLocation is deprecated, use "
                      "NCollection_IndexedMap<TopLoc_Location> directly")
typedef NCollection_IndexedMap<TopLoc_Location> TopLoc_IndexedMapOfLocation;

#endif // _TopLoc_IndexedMapOfLocation_hxx
