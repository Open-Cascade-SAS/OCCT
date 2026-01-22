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

//! @file BOPDS_DataMapOfShapeCoupleOfPaveBlocks.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _BOPDS_DataMapOfShapeCoupleOfPaveBlocks_hxx
#define _BOPDS_DataMapOfShapeCoupleOfPaveBlocks_hxx

#include <Standard_Macro.hxx>
#include <BOPDS_CoupleOfPaveBlocks.hxx>
#include <NCollection_DataMap.hxx>
#include <TopTools_ShapeMapHasher.hxx>

Standard_HEADER_DEPRECATED("BOPDS_DataMapOfShapeCoupleOfPaveBlocks.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("BOPDS_DataMapOfShapeCoupleOfPaveBlocks is deprecated, use NCollection_DataMap<TopoDS_Shape, BOPDS_CoupleOfPaveBlocks, TopTools_ShapeMapHasher> directly")
typedef NCollection_DataMap<TopoDS_Shape, BOPDS_CoupleOfPaveBlocks, TopTools_ShapeMapHasher> BOPDS_DataMapOfShapeCoupleOfPaveBlocks;
Standard_DEPRECATED("BOPDS_DataMapIteratorOfDataMapOfShapeCoupleOfPaveBlocks is deprecated, use NCollection_DataMap<TopoDS_Shape, BOPDS_CoupleOfPaveBlocks, TopTools_ShapeMapHasher>::Iterator directly")
typedef NCollection_DataMap<TopoDS_Shape, BOPDS_CoupleOfPaveBlocks, TopTools_ShapeMapHasher>::Iterator BOPDS_DataMapIteratorOfDataMapOfShapeCoupleOfPaveBlocks;

#endif // _BOPDS_DataMapOfShapeCoupleOfPaveBlocks_hxx
