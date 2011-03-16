#include <Storage_InternalData.ixx>

Storage_InternalData::Storage_InternalData() : myObjId(1), myTypeId(1)
{
}

void Storage_InternalData::Clear()
{
  myTypeId = 1;
  myObjId  = 1;
  myReadArray.Nullify();
  myPtoA.Clear();
  myTypeBinding.Clear();
}

