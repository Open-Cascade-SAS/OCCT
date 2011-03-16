// File:	SelectMgr_DataMapOfObjectOwners.hxx
// Created:	16.10.2003
// Author:      Alexander Solovyov

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
