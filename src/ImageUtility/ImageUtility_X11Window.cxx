#include <ImageUtility_X11Window.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(ImageUtility_X11Window)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type ("ImageUtility_X11Window", sizeof (ImageUtility_X11Window));
  return _atype;
}
