// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
