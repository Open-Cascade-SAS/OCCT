#include <MFT_FileRecord.hxx>

const Handle_Standard_Type& STANDARD_TYPE(MFT_FileRecord)
{
  static Handle_Standard_Type _aType = new Standard_Type("MFT_FileRecord",sizeof(MFT_FileRecord));

  return _aType;
}
