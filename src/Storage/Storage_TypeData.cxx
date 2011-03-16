#include <Storage_TypeData.ixx>

Storage_TypeData::Storage_TypeData() : myErrorStatus(Storage_VSOk)
{
}

Standard_Integer Storage_TypeData::NumberOfTypes() const
{
  return myPt.Extent();
}

Standard_Boolean Storage_TypeData::IsType(const TCollection_AsciiString& aName) const
{
  return myPt.Contains(aName);
}

Handle(TColStd_HSequenceOfAsciiString) Storage_TypeData::Types() const
{
  Handle(TColStd_HSequenceOfAsciiString) r = new TColStd_HSequenceOfAsciiString;
  Standard_Integer                       i;

  for (i = 1; i <= myPt.Extent(); i++) {
    r->Append(myPt.FindKey(i));
  }

  return r;
}

void Storage_TypeData::AddType(const TCollection_AsciiString& aName,const Standard_Integer aTypeNum) 
{
  myPt.Add(aName,aTypeNum);
}

TCollection_AsciiString Storage_TypeData::Type(const Standard_Integer aTypeNum) const
{
  TCollection_AsciiString r;

  if (aTypeNum <= myPt.Extent() && aTypeNum > 0) {
    r = myPt.FindKey(aTypeNum);
  }
  else {
    Standard_NoSuchObject::Raise("Storage_TypeData::Type - aTypeNum not in range");
  }

  return r;
}

Standard_Integer Storage_TypeData::Type(const TCollection_AsciiString& aTypeName) const
{
  Standard_Integer r = 0;

  if (myPt.Contains(aTypeName)) {
    r = myPt.FindFromKey(aTypeName);
  }
  else {
    Standard_NoSuchObject::Raise("Storage_TypeData::Type - aTypeName not found");
  }

  return r;
}

void Storage_TypeData::Clear()
{
  myPt.Clear();
}

Storage_Error  Storage_TypeData::ErrorStatus() const
{
  return myErrorStatus;
}

void Storage_TypeData::SetErrorStatus(const Storage_Error anError)
{
  myErrorStatus = anError;
}

void Storage_TypeData::ClearErrorStatus()
{
  myErrorStatus = Storage_VSOk;
  myErrorStatusExt.Clear();
}

TCollection_AsciiString Storage_TypeData::ErrorStatusExtension() const
{
  return myErrorStatusExt;
}

void Storage_TypeData::SetErrorStatusExtension(const TCollection_AsciiString& anErrorExt)
{
  myErrorStatusExt = anErrorExt;
}
