
// created: 1/07/97 ; PCT : Ajout texture mapping

#include <Graphic3d_CTexture.hxx>

const Handle(Standard_Type)& TYPE(Graphic3d_CTexture)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type ("Graphic3d_CTexture", sizeof (Graphic3d_CTexture));
  return _atype;
}
