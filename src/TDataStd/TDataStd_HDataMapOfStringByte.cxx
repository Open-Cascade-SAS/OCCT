// File:	TDataStd_HDataMapOfStringByte.cxx
// Created:	Fri Aug 17 16:58:50 2007
// Author:	Sergey ZARITCHNY
//		<szy@popox.nnov.matra-dtv.fr>
// Copyright:   Open Cascade SA 2007 

#include <TDataStd_HDataMapOfStringByte.ixx>

//=======================================================================
//function : TDataStd_HDataMapOfStringByte
//purpose  : Constructor of empty map
//=======================================================================
TDataStd_HDataMapOfStringByte::TDataStd_HDataMapOfStringByte(const Standard_Integer NbBuckets)
{
  myMap.ReSize(NbBuckets);
}

//=======================================================================
//function : TDataStd_HDataMapOfStringByte
//purpose  : Constructor from already existing map; performs copying
//=======================================================================
TDataStd_HDataMapOfStringByte::TDataStd_HDataMapOfStringByte (const TDataStd_DataMapOfStringByte &theOther)
{ 
  myMap.Assign ( theOther ); 
}
