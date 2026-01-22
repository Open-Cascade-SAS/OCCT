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

//! @file BOPDS_DataMapOfPaveBlockCommonBlock.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _BOPDS_DataMapOfPaveBlockCommonBlock_hxx
#define _BOPDS_DataMapOfPaveBlockCommonBlock_hxx

#include <Standard_Macro.hxx>
#include <NCollection_DataMap.hxx>
#include <BOPDS_CommonBlock.hxx>

Standard_HEADER_DEPRECATED("BOPDS_DataMapOfPaveBlockCommonBlock.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("BOPDS_DataMapOfPaveBlockCommonBlock is deprecated, use NCollection_DataMap<opencascade::handle<BOPDS_PaveBlock>, opencascade::handle<BOPDS_CommonBlock>> directly")
typedef NCollection_DataMap<opencascade::handle<BOPDS_PaveBlock>, opencascade::handle<BOPDS_CommonBlock>> BOPDS_DataMapOfPaveBlockCommonBlock;
Standard_DEPRECATED("BOPDS_DataMapIteratorOfDataMapOfPaveBlockCommonBlock is deprecated, use NCollection_DataMap<opencascade::handle<BOPDS_PaveBlock>, opencascade::handle<BOPDS_CommonBlock>>::Iterator directly")
typedef NCollection_DataMap<opencascade::handle<BOPDS_PaveBlock>, opencascade::handle<BOPDS_CommonBlock>>::Iterator BOPDS_DataMapIteratorOfDataMapOfPaveBlockCommonBlock;

#endif // _BOPDS_DataMapOfPaveBlockCommonBlock_hxx
