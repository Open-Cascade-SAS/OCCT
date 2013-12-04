// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <PDataStd_BooleanArray.ixx>

//=======================================================================
//function : PDataStd_BooleanArray
//purpose  : 
//=======================================================================
PDataStd_BooleanArray::PDataStd_BooleanArray() :
    myLower(0),
    myUpper(0)
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
