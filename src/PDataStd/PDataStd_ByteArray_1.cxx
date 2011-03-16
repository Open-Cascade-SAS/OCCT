// File:	PDataStd_ByteArray_1.cxx
// Created:	Wed Mar 26 18:11:22 2008
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CasCade SA 2008

#include <PDataStd_ByteArray_1.ixx>

//=======================================================================
//function : PDataStd_ByteArray_1
//purpose  : 
//=======================================================================
PDataStd_ByteArray_1::PDataStd_ByteArray_1() {}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
void PDataStd_ByteArray_1::Set(const Handle(PColStd_HArray1OfInteger)& values)
{
  myValues = values;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================
const Handle(PColStd_HArray1OfInteger)& PDataStd_ByteArray_1::Get() const
{
  return myValues;
}

//=======================================================================
//function : SetDelta
//purpose  : 
//=======================================================================
void PDataStd_ByteArray_1::SetDelta(const Standard_Boolean delta)
{
  myDelta = delta;
}

//=======================================================================
//function : GetDelta
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_ByteArray_1::GetDelta() const
{
  return myDelta;
}
