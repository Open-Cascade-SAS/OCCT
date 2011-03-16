#include <Storage_Root.ixx>

Storage_Root::Storage_Root() : myRef(0)
{
}

Storage_Root::Storage_Root(const TCollection_AsciiString& aName,const Handle(Standard_Persistent)& anObject) : myRef(0)
{
  myName   = aName;
  myObject = anObject;
  
  if (!anObject.IsNull()) {
    myType   = anObject->DynamicType()->Name();
  }
}

void Storage_Root::SetName(const TCollection_AsciiString& aName) 
{
  myName   = aName;
}

TCollection_AsciiString Storage_Root::Name() const
{
  return myName;
}

void Storage_Root::SetObject(const Handle(Standard_Persistent)& anObject) 
{
  myObject = anObject;

  if (!anObject.IsNull()) {
    myType   = anObject->DynamicType()->Name();
  }
}

Handle(Standard_Persistent) Storage_Root::Object() const
{
  return myObject;
}

TCollection_AsciiString Storage_Root::Type() const
{
  return myType;
}

void Storage_Root::SetReference(const Standard_Integer aRef) 
{
  myRef = aRef;
}

Standard_Integer Storage_Root::Reference() const
{
  return myRef;
}

void Storage_Root::SetType(const TCollection_AsciiString& aType) 
{
  myType = aType;
}
