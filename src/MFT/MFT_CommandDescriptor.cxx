#include <MFT_CommandDescriptor.hxx>

const Handle_Standard_Type& STANDARD_TYPE(MFT_CommandDescriptor)
{
  static Handle_Standard_Type _aType = new Standard_Type("MFT_CommandDescriptor",sizeof(MFT_CommandDescriptor));

  return _aType;
}
