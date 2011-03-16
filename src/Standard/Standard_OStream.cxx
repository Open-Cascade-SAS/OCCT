
#include <Standard_OStream.hxx>
#include <Standard_Type.hxx>

Handle_Standard_Type& Standard_OStream_Type_() 
{
  static Handle_Standard_Type _aType = 
    new Standard_Type("Standard_OStream", sizeof(Standard_OStream));
  
  return _aType;
}


