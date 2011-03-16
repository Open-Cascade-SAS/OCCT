
#include <Graphic3d_CGroup.hxx>

const Handle(Standard_Type)& TYPE(Graphic3d_CGroup)
{
  static Handle(Standard_Type) _atype =
    new Standard_Type ("Graphic3d_CGroup", sizeof (Graphic3d_CGroup));
  return _atype;
}
