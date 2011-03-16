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
