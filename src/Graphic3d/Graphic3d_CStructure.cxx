
#include <Graphic3d_CStructure.hxx>

const Handle(Standard_Type)& TYPE(Graphic3d_CStructure)
{
  static Handle(Standard_Type) _atype =
    new Standard_Type ("Graphic3d_CStructure", sizeof (Graphic3d_CStructure));
  return _atype;
}
