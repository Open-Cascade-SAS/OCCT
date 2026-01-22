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

//! @file BOPDS_DataMapOfIntegerListOfPaveBlock.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_DataMap<int, BOPDS_ListOfPaveBlock> directly instead.

#ifndef _BOPDS_DataMapOfIntegerListOfPaveBlock_hxx
#define _BOPDS_DataMapOfIntegerListOfPaveBlock_hxx

#include <Standard_Macro.hxx>
#include <NCollection_DataMap.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>

Standard_HEADER_DEPRECATED("BOPDS_DataMapOfIntegerListOfPaveBlock.hxx is deprecated since OCCT "
                           "8.0.0. Use NCollection_DataMap<int, BOPDS_ListOfPaveBlock> directly.")

  Standard_DEPRECATED("BOPDS_DataMapOfIntegerListOfPaveBlock is deprecated, use "
                      "NCollection_DataMap<int, BOPDS_ListOfPaveBlock> directly")
typedef NCollection_DataMap<int, BOPDS_ListOfPaveBlock> BOPDS_DataMapOfIntegerListOfPaveBlock;

#endif // _BOPDS_DataMapOfIntegerListOfPaveBlock_hxx
