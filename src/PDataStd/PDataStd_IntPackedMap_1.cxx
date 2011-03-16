// File:	PDataStd_IntPackedMap_1.cxx
// Created:	Thu Mar 27 17:22:25 2008
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CasCade SA 2008


#include <PDataStd_IntPackedMap_1.ixx>
#include <PColStd_HArray1OfInteger.hxx>

//=======================================================================
//function : PDataStd_IntPackedMap_1
//purpose  : Constructor
//=======================================================================
PDataStd_IntPackedMap_1::PDataStd_IntPackedMap_1() {}

//=======================================================================
//function : Init
//purpose  : Initializes the internal container
//=======================================================================
void PDataStd_IntPackedMap_1::Init(const Standard_Integer theLow, 
const Standard_Integer theUp) {
  myIntValues = new PColStd_HArray1OfInteger(theLow, theUp);
}

//=======================================================================
//function : GetValue
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntPackedMap_1::GetValue(
				   const Standard_Integer theIndex) const
{ return myIntValues->Value(theIndex); }

//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================
void PDataStd_IntPackedMap_1::SetValue(const Standard_Integer theIndx, 
				     const Standard_Integer theValue) 
{ 
  myIntValues->SetValue(theIndx, theValue);
}

//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_IntPackedMap_1::IsEmpty() const
{ 
  if(myIntValues.IsNull()) return Standard_True;
  if(!myIntValues->Upper() && !myIntValues->Lower()) return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : Returns an upper bound of the internal container
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntPackedMap_1::Upper() const
{ 
  if(myIntValues.IsNull()) return 0;
  return myIntValues->Upper();
}

//=======================================================================
//function : Returns a lower bound of the internal container
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_IntPackedMap_1::Lower() const
{ 
  if(myIntValues.IsNull()) return 0;
  return myIntValues->Lower();
}

//=======================================================================
//function : SetDelta
//purpose  : 
//=======================================================================
void PDataStd_IntPackedMap_1::SetDelta(const Standard_Boolean delta)
{
  myDelta = delta;
}

//=======================================================================
//function : GetDelta
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_IntPackedMap_1::GetDelta() const
{
  return myDelta;
}
