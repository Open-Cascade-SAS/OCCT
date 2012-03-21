// Created on: 2002-01-16
// Created by: Michael PONIKAROV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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



#include <PDataStd_ExtStringArray.ixx>

//=======================================================================
//function : PDataStd_ExtStringArray
//purpose  : 
//=======================================================================

PDataStd_ExtStringArray::PDataStd_ExtStringArray () { }

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void PDataStd_ExtStringArray::Init(const Standard_Integer lower,
				   const Standard_Integer upper)
{
  myValue = new PColStd_HArray1OfExtendedString(lower, upper);
}


//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================

void PDataStd_ExtStringArray::SetValue(const Standard_Integer index,
				       const Handle(PCollection_HExtendedString)& value)
{
  myValue->SetValue(index, value);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Handle(PCollection_HExtendedString) PDataStd_ExtStringArray::Value( const Standard_Integer index ) const
{
  return myValue->Value(index);
}

//=======================================================================
//function : Lower
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_ExtStringArray::Lower (void) const 
{
  return myValue->Lower();
}


//=======================================================================
//function : Upper
//purpose  : 
//=======================================================================
Standard_Integer PDataStd_ExtStringArray::Upper (void) const 
{
  return myValue->Upper();
}
