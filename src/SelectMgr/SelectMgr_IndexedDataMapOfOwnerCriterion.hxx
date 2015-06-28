// Created on: 1995-02-06
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef SelectMgr_IndexedDataMapOfOwnerCriterion_HeaderFile
#define SelectMgr_IndexedDataMapOfOwnerCriterion_HeaderFile

#include <SelectBasics_EntityOwner.hxx>
#include <SelectMgr_SortCriterion.hxx>
#include <TColStd_MapTransientHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>

typedef NCollection_IndexedDataMap<Handle(SelectBasics_EntityOwner),SelectMgr_SortCriterion,TColStd_MapTransientHasher> SelectMgr_IndexedDataMapOfOwnerCriterion;


#endif
