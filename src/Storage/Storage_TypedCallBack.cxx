#include <Storage_TypedCallBack.ixx>

Storage_TypedCallBack::Storage_TypedCallBack() : myIndex(0)
{
}

Storage_TypedCallBack::Storage_TypedCallBack(const TCollection_AsciiString& aTypeName,const Handle(Storage_CallBack)& aCallBack) : myIndex(0)
{
  myType = aTypeName;
  myCallBack = aCallBack;
}

void Storage_TypedCallBack::SetType(const TCollection_AsciiString& aType) 
{
  myType = aType;
}

TCollection_AsciiString Storage_TypedCallBack::Type() const
{
  return myType;
}

void Storage_TypedCallBack::SetCallBack(const Handle(Storage_CallBack)& aCallBack) 
{
  myCallBack = aCallBack;
}

Handle(Storage_CallBack) Storage_TypedCallBack::CallBack() const
{
  return myCallBack;
}

void Storage_TypedCallBack::SetIndex(const Standard_Integer anIndex)
{
  myIndex  = anIndex;
}

Standard_Integer Storage_TypedCallBack::Index() const
{
  return myIndex;
}
