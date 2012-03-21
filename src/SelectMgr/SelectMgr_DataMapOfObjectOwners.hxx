// Created on: 2003-10-16
// Created by: Alexander Solovyov
// Copyright (c) 2003-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
