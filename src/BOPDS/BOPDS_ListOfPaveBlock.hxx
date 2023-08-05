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

#ifndef BOPDS_ListOfPaveBlock_HeaderFile
#define BOPDS_ListOfPaveBlock_HeaderFile

#include <NCollection_List.hxx>
#include <Standard_Handle.hxx>

class BOPDS_PaveBlock;

typedef NCollection_List<Handle(BOPDS_PaveBlock)> BOPDS_ListOfPaveBlock; 
typedef BOPDS_ListOfPaveBlock::Iterator BOPDS_ListIteratorOfListOfPaveBlock;


#endif
