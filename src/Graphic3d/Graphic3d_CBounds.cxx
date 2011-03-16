
#include <Graphic3d_CBounds.hxx>

const Handle(Standard_Type)& TYPE(Graphic3d_CBounds)
{
  static Handle(Standard_Type) _atype =
    new Standard_Type ("Graphic3d_CBounds", sizeof (Graphic3d_CBounds));
  return _atype;
}
