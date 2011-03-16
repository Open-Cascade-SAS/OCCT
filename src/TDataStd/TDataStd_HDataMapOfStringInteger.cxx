// File:	TDataStd_HDataMapOfStringInteger.cxx
// Created:	Fri Aug 17 16:20:53 2007
// Author:	Sergey ZARITCHNY
//		<szy@popox.nnov.matra-dtv.fr>
// Copyright:   Open Cascade SA 2007 

#include <TDataStd_HDataMapOfStringInteger.ixx>

//=======================================================================
//function : TDataStd_HDataMapOfStringInteger
//purpose  : Constructor of empty map
//=======================================================================
TDataStd_HDataMapOfStringInteger::TDataStd_HDataMapOfStringInteger(const Standard_Integer NbBuckets)
{
  myMap.ReSize(NbBuckets);
}

//=======================================================================
//function : TDataStd_HDataMapOfStringInteger
//purpose  : Constructor from already existing map; performs copying
//=======================================================================
TDataStd_HDataMapOfStringInteger::TDataStd_HDataMapOfStringInteger (const TColStd_DataMapOfStringInteger &theOther)
{ 
  myMap.Assign ( theOther ); 
}
