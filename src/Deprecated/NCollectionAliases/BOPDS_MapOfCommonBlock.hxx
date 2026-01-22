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

//! @file BOPDS_MapOfCommonBlock.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _BOPDS_MapOfCommonBlock_hxx
#define _BOPDS_MapOfCommonBlock_hxx

#include <Standard_Macro.hxx>
#include <NCollection_Map.hxx>
#include <BOPDS_CommonBlock.hxx>

Standard_HEADER_DEPRECATED("BOPDS_MapOfCommonBlock.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("BOPDS_MapOfCommonBlock is deprecated, use NCollection_Map<opencascade::handle<BOPDS_CommonBlock>> directly")
typedef NCollection_Map<opencascade::handle<BOPDS_CommonBlock>> BOPDS_MapOfCommonBlock;
Standard_DEPRECATED("BOPDS_MapIteratorOfMapOfCommonBlock is deprecated, use NCollection_Map<opencascade::handle<BOPDS_CommonBlock>>::Iterator directly")
typedef NCollection_Map<opencascade::handle<BOPDS_CommonBlock>>::Iterator BOPDS_MapIteratorOfMapOfCommonBlock;

#endif // _BOPDS_MapOfCommonBlock_hxx
