
#include <Standard_IStream.hxx>
#include <Standard_Type.hxx>

Handle_Standard_Type& Standard_IStream_Type_() 
{
  static Handle_Standard_Type _aType = 
    new Standard_Type("Standard_IStream", sizeof(Standard_IStream));
  
  return _aType;
}

