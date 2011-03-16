// File:	TDataStd_HDataMapOfStringString.cxx
// Created:	Fri Aug 17 16:53:03 2007
// Author:	Sergey ZARITCHNY
//		<szy@popox.nnov.matra-dtv.fr>
// Copyright:   Open Cascade SA 2007 

#include <TDataStd_HDataMapOfStringString.ixx>

//=======================================================================
//function : TDataStd_HDataMapOfStringString
//purpose  : Constructor of empty map
//=======================================================================
TDataStd_HDataMapOfStringString::TDataStd_HDataMapOfStringString(const Standard_Integer NbBuckets)
{
  myMap.ReSize(NbBuckets);
}

//=======================================================================
//function : TDataStd_HDataMapOfStringString
//purpose  : Constructor from already existing map; performs copying
//=======================================================================
TDataStd_HDataMapOfStringString::TDataStd_HDataMapOfStringString (const TDataStd_DataMapOfStringString &theOther)
{ 
  myMap.Assign ( theOther ); 
}
