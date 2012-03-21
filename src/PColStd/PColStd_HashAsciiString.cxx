// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


//-Version: 

//  Version	Date         Purpose
//              01/04/93     Creation   

//-Language     C++

//-Declarations
#include <Standard_Integer.hxx>
#include <PColStd_HashAsciiString.ixx>
#include <Standard_RangeError.hxx>
#include <PCollection_HAsciiString.hxx>

//=======================================================================
// Function : Create 
//=======================================================================

PColStd_HashAsciiString::PColStd_HashAsciiString()
{}

//=======================================================================
// Function : HashCode
//=======================================================================

Standard_Integer PColStd_HashAsciiString::HashCode
  (const Handle(PCollection_HAsciiString) &MyKey,
   const Standard_Integer Upper) const
{
  Standard_Integer total=0;

  if ( Upper < 1 ){
    Standard_RangeError::
      Raise("Try to apply HasCode method with negative or null argument.");
  }
  for (Standard_Integer I = 1 ; I <= MyKey->Length() ; I++ ) {
    total = total + (Standard_Integer) (MyKey->Value(I));
  }
  total = ::HashCode( total , Upper ) ;

  return total;
}

//=======================================================================
// Function : Compare
//=======================================================================

Standard_Boolean PColStd_HashAsciiString::Compare
  (const Handle(PCollection_HAsciiString) &One,
   const Handle(PCollection_HAsciiString) &Two) const
{
  return (One->IsSameString(Two));
}
