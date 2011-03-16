
#include <Graphic3d_CUserDraw.hxx>

const Handle(Standard_Type)& TYPE(Graphic3d_CUserDraw)
{
  static Handle(Standard_Type) _atype =
    new Standard_Type ("Graphic3d_CUserDraw", sizeof (Graphic3d_CUserDraw));
  return _atype;
}
