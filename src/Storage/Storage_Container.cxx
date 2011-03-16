#include <Storage_Container.hxx>
#include <Standard_Type.hxx> 

const Handle_Standard_Type& Storage_Container_Type_() 
{
  static Handle_Standard_Type _aType = new Standard_Type("Storage_Container",sizeof(Storage_Container));
  
  return _aType;
}
