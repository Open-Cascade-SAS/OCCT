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
