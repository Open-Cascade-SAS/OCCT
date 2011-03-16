
#include <Graphic3d_CTransPersStruct.hxx>

const Handle(Standard_Type)& TYPE(Graphic3d_CTransPersStruct)
{
  static Handle(Standard_Type) _atype =
    new Standard_Type ("Graphic3d_CTransPersStruct", sizeof (Graphic3d_CTransPersStruct));
  return _atype;
}
