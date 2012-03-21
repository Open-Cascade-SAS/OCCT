// Created on: 2008-03-27
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
