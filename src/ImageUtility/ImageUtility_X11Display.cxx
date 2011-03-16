#include <ImageUtility_X11Display.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(ImageUtility_X11Display)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type ("ImageUtility_X11Display", sizeof (ImageUtility_X11Display));
  return _atype;
}
