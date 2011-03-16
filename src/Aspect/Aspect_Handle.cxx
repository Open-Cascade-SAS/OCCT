#include <Aspect_Handle.hxx>
#include <Standard_Type.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(Aspect_Handle)
{
  static Handle(Standard_Type) _atype = new Standard_Type ("Aspect_Handle", sizeof (Aspect_Handle));
  return _atype;
}
