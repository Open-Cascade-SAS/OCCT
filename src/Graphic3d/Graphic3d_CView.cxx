
#include <Graphic3d_CView.hxx>

const Handle(Standard_Type)& TYPE(Graphic3d_CView)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type ("Graphic3d_CView", sizeof (Graphic3d_CView));
  return _atype;
}
