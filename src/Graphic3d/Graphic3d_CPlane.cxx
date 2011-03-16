
#include <Graphic3d_CPlane.hxx>

const Handle(Standard_Type)& TYPE(Graphic3d_CPlane)
{
  static Handle(Standard_Type) _atype =
    new Standard_Type ("Graphic3d_CPlane", sizeof (Graphic3d_CPlane));
  return _atype;
}
