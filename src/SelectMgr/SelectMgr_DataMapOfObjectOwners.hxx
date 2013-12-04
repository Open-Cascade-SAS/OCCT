// Created on: 2003-10-16
// Created by: Alexander Solovyov
// Copyright (c) 2003-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _SelectMgr_DataMapOfObjectOwners_HeaderFile
#define _SelectMgr_DataMapOfObjectOwners_HeaderFile

#include <NCollection_DefineDataMap.hxx>
#include <Handle_SelectMgr_SelectableObject.hxx>
#include <SelectMgr_SequenceOfOwner.hxx>

inline Standard_Boolean IsEqual (const Handle_SelectMgr_SelectableObject& theH1,
				 const Handle_SelectMgr_SelectableObject& theH2)
{
  return (theH1 == theH2);
}

DEFINE_BASECOLLECTION(SelectMgr_CollectionOfSequenceOfOwner, SelectMgr_SequenceOfOwner)
DEFINE_DATAMAP(SelectMgr_DataMapOfObjectOwners, SelectMgr_CollectionOfSequenceOfOwner,
               Handle(SelectMgr_SelectableObject), SelectMgr_SequenceOfOwner)

typedef SelectMgr_DataMapOfObjectOwners::Iterator
  SelectMgr_DataMapIteratorOfMapOfObjectOwners;


#endif
