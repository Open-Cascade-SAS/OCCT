// Created by: Peter KURNEV
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef BOPDS_DataMapOfShapeCoupleOfPaveBlocks_HeaderFile
#define BOPDS_DataMapOfShapeCoupleOfPaveBlocks_HeaderFile

#include <BOPDS_CoupleOfPaveBlocks.hxx>

#include <NCollection_DataMap.hxx>
#include <TopTools_ShapeMapHasher.hxx>

typedef NCollection_DataMap<TopoDS_Shape, BOPDS_CoupleOfPaveBlocks, TopTools_ShapeMapHasher>
  BOPDS_DataMapOfShapeCoupleOfPaveBlocks;
typedef BOPDS_DataMapOfShapeCoupleOfPaveBlocks::Iterator
  BOPDS_DataMapIteratorOfDataMapOfShapeCoupleOfPaveBlocks;

#endif
