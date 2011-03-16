
#include <Standard_GUID.hxx>
#include <Standard_UUID.hxx>

const Handle(Standard_Type) & TYPE(Standard_UUID)
{ static Handle(Standard_Type) _aType= 
    new Standard_Type("Standard_UUID", sizeof(Standard_UUID));
  return _aType;
}

//void Standard_GUID::Assign(const Standard_UUID& uid) 
void Standard_GUID::Assign(const Standard_UUID& ) 
{
}
