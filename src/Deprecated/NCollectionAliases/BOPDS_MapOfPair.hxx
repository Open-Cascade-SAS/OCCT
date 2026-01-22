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

//! @file BOPDS_MapOfPair.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _BOPDS_MapOfPair_hxx
#define _BOPDS_MapOfPair_hxx

#include <Standard_Macro.hxx>
#include <NCollection_Map.hxx>
#include <BOPDS_Pair.hxx>

Standard_HEADER_DEPRECATED("BOPDS_MapOfPair.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("BOPDS_MapOfPair is deprecated, use NCollection_Map<BOPDS_Pair> directly")
typedef NCollection_Map<BOPDS_Pair> BOPDS_MapOfPair;
Standard_DEPRECATED("BOPDS_MapIteratorOfMapOfPair is deprecated, use NCollection_Map<BOPDS_Pair>::Iterator directly")
typedef NCollection_Map<BOPDS_Pair>::Iterator BOPDS_MapIteratorOfMapOfPair;

#endif // _BOPDS_MapOfPair_hxx
