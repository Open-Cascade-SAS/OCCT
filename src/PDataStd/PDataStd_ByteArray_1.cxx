// Created on: 2008-03-26
// Created by: Sergey ZARITCHNY
// Copyright (c) 2008-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
