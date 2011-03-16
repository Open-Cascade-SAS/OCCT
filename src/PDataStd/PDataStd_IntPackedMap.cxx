// File:	PDataStd_IntPackedMap.cxx
// Created:	Wed Aug 22 17:28:25 2007
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CASCADE SA 2007

#include <PDataStd_IntPackedMap.ixx>
#include <PColStd_HArray1OfInteger.hxx>
//=======================================================================
//function : PDataStd_IntPackedMap
//purpose  : Constructor
//=======================================================================
PDataStd_IntPackedMap::PDataStd_IntPackedMap() {}

//=======================================================================
//function : Init
//purpose  : Initializes the internal container
//=======================================================================
void PDataStd_IntPackedMap::Init(const Standard_Integer theLow, 
const Standard_Integer theUp) {
  myIntValues = new PColStd_HArray1OfInteger(theLow, theUp);
}

//=======================================================================
//function : GetValue
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntPackedMap::GetValue(
				   const Standard_Integer theIndex) const
{ return myIntValues->Value(theIndex); }

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================
void PDataStd_IntPackedMap::SetValue(const Standard_Integer theIndx, 
				     const Standard_Integer theValue) 
{ 
  myIntValues->SetValue(theIndx, theValue);
}

//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_IntPackedMap::IsEmpty() const
{ 
  if(myIntValues.IsNull()) return Standard_True;
  if(!myIntValues->Upper() && !myIntValues->Lower()) return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : Returns an upper bound of the internal container
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntPackedMap::Upper() const
{ 
  if(myIntValues.IsNull()) return 0;
  return myIntValues->Upper();
}

//=======================================================================
//function : Returns a lower bound of the internal container
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntPackedMap::Lower() const
{ 
  if(myIntValues.IsNull()) return 0;
  return myIntValues->Lower();
}
