// Created on: 2013-05-23 
// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef BOPDS_DataMapOfPaveBlockCommonBlock_HeaderFile
#define BOPDS_DataMapOfPaveBlockCommonBlock_HeaderFile

#include <NCollection_DataMap.hxx>
#include <BOPDS_CommonBlock.hxx>

typedef NCollection_DataMap<Handle(BOPDS_PaveBlock), Handle(BOPDS_CommonBlock)> BOPDS_DataMapOfPaveBlockCommonBlock; 
typedef BOPDS_DataMapOfPaveBlockCommonBlock::Iterator BOPDS_DataMapIteratorOfDataMapOfPaveBlockCommonBlock;
 
#endif
