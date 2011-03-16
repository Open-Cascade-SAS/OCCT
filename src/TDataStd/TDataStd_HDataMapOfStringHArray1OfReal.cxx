// File:	TDataStd_HDataMapOfStringHArray1OfReal.cxx
// Created:	Fri Aug 17 17:27:26 2007
// Author:	Sergey ZARITCHNY
//		<szy@popox.nnov.matra-dtv.fr>
// Copyright:   Open Cascade SA 2007 

#include <TDataStd_HDataMapOfStringHArray1OfReal.ixx>

//=======================================================================
//function : TDataStd_HDataMapOfStringHArray1OfReal
//purpose  : Constructor of empty map
//=======================================================================
TDataStd_HDataMapOfStringHArray1OfReal::TDataStd_HDataMapOfStringHArray1OfReal(const Standard_Integer NbBuckets)
{
  myMap.ReSize(NbBuckets);
}

//=======================================================================
//function : TDataStd_HDataMapOfStringHArray1OfReal
//purpose  : Constructor from already existing map; performs copying
//=======================================================================
TDataStd_HDataMapOfStringHArray1OfReal::TDataStd_HDataMapOfStringHArray1OfReal (const TDataStd_DataMapOfStringHArray1OfReal &theOther)
{ 
  myMap.Assign ( theOther ); 
}
