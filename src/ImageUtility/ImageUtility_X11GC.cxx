#include <ImageUtility_X11GC.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(ImageUtility_X11GC)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type ("ImageUtility_X11GC", sizeof (ImageUtility_X11GC));
  return _atype;
}
