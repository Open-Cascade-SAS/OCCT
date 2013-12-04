// Created on: 2008-03-26
// Created by: Sergey ZARITCHNY
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <PDataStd_ByteArray_1.ixx>

//=======================================================================
//function : PDataStd_ByteArray_1
//purpose  : 
//=======================================================================
PDataStd_ByteArray_1::PDataStd_ByteArray_1() : myDelta(Standard_False) {}

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
