
#include <Graphic3d_CPick.hxx>

const Handle(Standard_Type)& TYPE(Graphic3d_CPick)
{
  static Handle(Standard_Type) _atype =
    new Standard_Type ("Graphic3d_CPick", sizeof (Graphic3d_CPick));
  return _atype;
}
