// File:	AIS_NDataMapOfTransientIteratorOfListTransient.hxx
// Created:	Sun May 04 17:51:26 2003
// Author:      Alexander Grigoriev (a-grigoriev@opencascade.com)

#ifndef _AIS_NDataMapOfTransientIteratorOfListTransient_HeaderFile
#define _AIS_NDataMapOfTransientIteratorOfListTransient_HeaderFile

#include <AIS_NListIteratorOfListTransient.hxx>
#include <NCollection_DefineDataMap.hxx>

inline Standard_Boolean IsEqual (const Handle_Standard_Transient& theH1,
				 const Handle_Standard_Transient& theH2)
{
  return (theH1 == theH2);
}

DEFINE_BASECOLLECTION(AIS_BaseCollItListTransient,
                      AIS_NListIteratorOfListTransient)
DEFINE_DATAMAP       (AIS_NDataMapOfTransientIteratorOfListTransient,
                      AIS_BaseCollItListTransient,
                      Handle(Standard_Transient),
                      AIS_NListIteratorOfListTransient)

#endif
