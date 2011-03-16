// File:	TDataStd_HDataMapOfStringReal.cxx
// Created:	Fri Aug 17 16:41:04 2007
// Author:	Sergey ZARITCHNY
//		<szy@popox.nnov.matra-dtv.fr>
// Copyright:   Open Cascade SA 2007 

#include <TDataStd_HDataMapOfStringReal.ixx>
//=======================================================================
//function : TDataStd_HDataMapOfStringReal
//purpose  : Constructor of empty map
//=======================================================================
TDataStd_HDataMapOfStringReal::TDataStd_HDataMapOfStringReal(const Standard_Integer NbBuckets)
{
  myMap.ReSize(NbBuckets);
}

//=======================================================================
//function : TDataStd_HDataMapOfStringReal
//purpose  : Constructor from already existing map; performs copying
//=======================================================================
TDataStd_HDataMapOfStringReal::TDataStd_HDataMapOfStringReal (const TDataStd_DataMapOfStringReal &theOther)
{ 
  myMap.Assign ( theOther ); 
}
