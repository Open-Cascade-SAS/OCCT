// Copyright (c) 1995-1999 Matra Datavision
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


#include <stdio.h>

#include <Standard_Stream.hxx>
#include <Aspect_IndexPixel.ixx>

Aspect_IndexPixel::Aspect_IndexPixel () {

  myIndex = 0;

}

Aspect_IndexPixel::Aspect_IndexPixel (const Standard_Integer anIndex) {

  myIndex = anIndex;

}

Standard_Integer Aspect_IndexPixel::Value() const { 

return myIndex;

}


void Aspect_IndexPixel::SetValue(const Standard_Integer anIndex) {

  myIndex = anIndex;

}

void Aspect_IndexPixel::Print(Standard_OStream& s) const

{
	s << dec << setw(4) << myIndex;
}

// ------------------------------------------------------------------
// Hascode : Computes a hascoding value for a given Aspect_IndexPixel
// ------------------------------------------------------------------
Standard_Integer Aspect_IndexPixel::HashCode(const Standard_Integer Upper) const
{
   return ( myIndex % Upper ) + 1 ;
}


Standard_Boolean Aspect_IndexPixel::IsEqual(const Aspect_IndexPixel& Other) const
{
  return (myIndex == Other.myIndex);
}

Standard_Boolean Aspect_IndexPixel::IsNotEqual(const Aspect_IndexPixel& Other) const
{
  return !IsEqual(Other);
}
