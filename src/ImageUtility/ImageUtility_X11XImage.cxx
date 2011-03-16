#include <ImageUtility_X11XImage.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(ImageUtility_X11XImage)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type ("ImageUtility_X11XImage", sizeof (ImageUtility_X11XImage));
  return _atype;
}
