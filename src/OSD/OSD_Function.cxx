
#include <OSD_Function.hxx>

const Handle_Standard_Type& OSD_Function_Type_()
{
  static Handle_Standard_Type _aType = 
    new Standard_Type("OSD_Function",sizeof(OSD_Function));

  return _aType;
}
