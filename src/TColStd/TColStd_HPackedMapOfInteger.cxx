// File:	TColStd_HPackedMapOfInteger.cxx
// Created:	Tue Dec 05 15:04:47 2006
// Author:	Sergey  KOCHETKOV
// Copyright:   Open Cascade 2006 


#include <TColStd_HPackedMapOfInteger.ixx>

//! Constructor of empty map
TColStd_HPackedMapOfInteger::TColStd_HPackedMapOfInteger (const Standard_Integer NbBuckets)
{
  myMap.ReSize(NbBuckets);
}

//! Constructor from already existing map; performs copying
TColStd_HPackedMapOfInteger::TColStd_HPackedMapOfInteger (const TColStd_PackedMapOfInteger &theOther)
{ 
  myMap.Assign ( theOther ); 
}


