// created: 1/07/97 ; PCT : texture mapping


#include <Graphic3d_CInitTexture.hxx>

const Handle(Standard_Type)& TYPE(Graphic3d_CInitTexture)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type ("Graphic3d_CInitTexture", sizeof (Graphic3d_CInitTexture));
  return _atype;
}
