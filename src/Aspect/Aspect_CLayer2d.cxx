#include <Aspect_CLayer2d.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(Aspect_CLayer2d)
{
  static Handle(Standard_Type) _atype = new Standard_Type ("Aspect_CLayer2d", sizeof (Aspect_CLayer2d));
  return _atype;
}
