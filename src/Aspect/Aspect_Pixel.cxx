#include <Aspect_Pixel.ixx>
#include <Standard_OStream.hxx>

Aspect_Pixel::Aspect_Pixel() {}

Standard_OStream& operator << (Standard_OStream& s, const Aspect_Pixel& aPixel)
{ 
  aPixel.Print( s ) ;
  return s ;
}

