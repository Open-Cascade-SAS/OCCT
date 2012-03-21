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



#include <PDataStd_ExtStringArray_1.ixx>

//=======================================================================
//function : PDataStd_ExtStringArray_1
//purpose  : 
//=======================================================================

PDataStd_ExtStringArray_1::PDataStd_ExtStringArray_1 () { }

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void PDataStd_ExtStringArray_1::Init(const Standard_Integer lower,
				   const Standard_Integer upper)
{
  myValue = new PColStd_HArray1OfExtendedString(lower, upper);
}


//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void PDataStd_ExtStringArray_1::SetValue(const Standard_Integer index,
				       const Handle(PCollection_HExtendedString)& value)
{
  myValue->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(PCollection_HExtendedString) PDataStd_ExtStringArray_1::Value( const Standard_Integer index ) const
{
  return myValue->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_ExtStringArray_1::Lower (void) const 
{
  return myValue->Lower();
}


//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_ExtStringArray_1::Upper (void) const 
{
  return myValue->Upper();
}

//=======================================================================
//function : SetDelta
//purpose  : 
//=======================================================================
void PDataStd_ExtStringArray_1::SetDelta(const Standard_Boolean delta)
{
  myDelta = delta;
}

//=======================================================================
//function : GetDelta
//purpose  : 
//=======================================================================
Standard_Boolean PDataStd_ExtStringArray_1::GetDelta() const
{
  return myDelta;
}

