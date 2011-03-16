// File:	TDataStd_HDataMapOfStringHArray1OfInteger.cxx
// Created:	Fri Aug 17 17:10:36 2007
// Author:	Sergey ZARITCHNY
//		<szy@popox.nnov.matra-dtv.fr>
// Copyright:   Open Cascade SA 2007 

#include <TDataStd_HDataMapOfStringHArray1OfInteger.ixx>

//=======================================================================
//function : TDataStd_HDataMapOfStringHArray1OfInteger
//purpose  : Constructor of empty map
//=======================================================================
TDataStd_HDataMapOfStringHArray1OfInteger::TDataStd_HDataMapOfStringHArray1OfInteger(const Standard_Integer NbBuckets)
{
  myMap.ReSize(NbBuckets);
}

//=======================================================================
//function : TDataStd_HDataMapOfStringHArray1OfInteger
//purpose  : Constructor from already existing map; performs copying
//=======================================================================
TDataStd_HDataMapOfStringHArray1OfInteger::TDataStd_HDataMapOfStringHArray1OfInteger (const TDataStd_DataMapOfStringHArray1OfInteger &theOther)
{ 
  myMap.Assign ( theOther ); 
}
