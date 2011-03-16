
#include <Graphic3d_CLight.hxx>

const Handle(Standard_Type)& TYPE(Graphic3d_CLight)
{
  static Handle(Standard_Type) _atype =
    new Standard_Type ("Graphic3d_CLight", sizeof (Graphic3d_CLight));
  return _atype;
}
