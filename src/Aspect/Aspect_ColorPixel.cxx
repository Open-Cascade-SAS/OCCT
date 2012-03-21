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

#include <Aspect_ColorPixel.ixx>

Aspect_ColorPixel::Aspect_ColorPixel () {

  myColor.SetValues( 0.,0.,0. , Quantity_TOC_RGB ) ;

}

Aspect_ColorPixel::Aspect_ColorPixel (const Quantity_Color& aColor) {

  myColor = aColor;

}

const Quantity_Color& Aspect_ColorPixel::Value() const { 

return myColor;

}


void Aspect_ColorPixel::SetValue(const Quantity_Color& aColor) {

  myColor = aColor;

}

void Aspect_ColorPixel::Print(Standard_OStream& s) const

{ Standard_Real r,g,b ;

  myColor.Values( r,g,b, Quantity_TOC_RGB ) ;

  s << "( " << r << ", " << g << ", " << b << " )" << flush;
}

// ------------------------------------------------------------------
// Hascode : Computes a hascoding value for a given Aspect_ColorPixel
// ------------------------------------------------------------------
Standard_Integer Aspect_ColorPixel::HashCode(const Standard_Integer Upper) const
{ Standard_Real r,g,b ;
  Standard_Integer ret ;

  myColor.Values( r,g,b, Quantity_TOC_RGB ) ;

  ret = ( Standard_Integer ) ( ( r + g + b ) * Upper  ) ;

  return ( ret % Upper ) + 1 ;
}


Standard_Boolean Aspect_ColorPixel::IsEqual(const Aspect_ColorPixel& Other) const
{
  return (myColor == Other.myColor);
}

Standard_Boolean Aspect_ColorPixel::IsNotEqual(const Aspect_ColorPixel& Other) const
{
  return !IsEqual(Other);
}
