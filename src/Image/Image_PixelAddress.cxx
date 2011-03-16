
#include <Image_PixelAddress.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(Image_PixelAddress)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type ("Image_PixelAddress", sizeof (Image_PixelAddress));
  return _atype;
}
