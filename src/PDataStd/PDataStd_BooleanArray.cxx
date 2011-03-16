// File:        PDataStd_BooleanArray.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <PDataStd_BooleanArray.ixx>

//=======================================================================
//function : PDataStd_BooleanArray
//purpose  : 
//=======================================================================
PDataStd_BooleanArray::PDataStd_BooleanArray() 
{ 

}

//=======================================================================
//function : SetLower
//purpose  : 
//=======================================================================
void PDataStd_BooleanArray::SetLower(const Standard_Integer lower)
{
  myLower = lower;
}

//=======================================================================
//function : SetUpper
//purpose  : 
//=======================================================================
void PDataStd_BooleanArray::SetUpper(const Standard_Integer upper)
{
  myUpper = upper;
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_BooleanArray::Lower (void) const 
{ 
  return myLower;
}

//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_BooleanArray::Upper (void) const 
{ 
  return myUpper;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
void PDataStd_BooleanArray::Set(const Handle(PColStd_HArray1OfInteger)& values)
{
  myValues = values;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================
const Handle(PColStd_HArray1OfInteger)& PDataStd_BooleanArray::Get() const
{
  return myValues;
}
