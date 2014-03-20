// Created by: Eugeny MALTCHIKOV
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

#ifndef BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock_HeaderFile
#define BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock_HeaderFile

#include <NCollection_IndexedDataMap.hxx>
#include <TColStd_MapTransientHasher.hxx>
#include <BOPDS_PaveBlock.hxx>   
#include <BOPDS_ListOfPaveBlock.hxx>

typedef NCollection_IndexedDataMap<Handle(BOPDS_PaveBlock), BOPDS_ListOfPaveBlock, TColStd_MapTransientHasher> BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock; 

#endif
