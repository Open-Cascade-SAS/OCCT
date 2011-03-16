#include <XCAFDoc_Material.ixx>

#include <TDF_RelocationTable.hxx>


//=======================================================================
//function : XCAFDoc_Material
//purpose  : 
//=======================================================================

XCAFDoc_Material::XCAFDoc_Material()
{
}


//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Material::GetID() 
{
  static Standard_GUID MatID ("efd212f8-6dfd-11d4-b9c8-0060b0ee281b");
  return MatID;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(XCAFDoc_Material) XCAFDoc_Material::Set(const TDF_Label& label,
                                               const Handle(TCollection_HAsciiString)& aName,
                                               const Handle(TCollection_HAsciiString)& aDescription,
                                               const Standard_Real aDensity,
                                               const Handle(TCollection_HAsciiString)& aDensName,
                                               const Handle(TCollection_HAsciiString)& aDensValType)
{
  Handle(XCAFDoc_Material) A;
  if (!label.FindAttribute(XCAFDoc_Material::GetID(), A)) {
    A = new XCAFDoc_Material();
    label.AddAttribute(A);
  }
  A->Set(aName,aDescription,aDensity,aDensName,aDensValType); 
  return A;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void XCAFDoc_Material::Set(const Handle(TCollection_HAsciiString)& aName,
                           const Handle(TCollection_HAsciiString)& aDescription,
                           const Standard_Real aDensity,
                           const Handle(TCollection_HAsciiString)& aDensName,
                           const Handle(TCollection_HAsciiString)& aDensValType)
{
  myName = aName;
  myDescription = aDescription;
  myDensity = aDensity;
  myDensName = aDensName;
  myDensValType = aDensValType;
}


//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) XCAFDoc_Material::GetName() const
{
  return myName;
}


//=======================================================================
//function : GetDescription
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) XCAFDoc_Material::GetDescription() const
{
  return myDescription;
}


//=======================================================================
//function : GetDensity
//purpose  : 
//=======================================================================

Standard_Real XCAFDoc_Material::GetDensity() const
{
  return myDensity;
}


//=======================================================================
//function : GetDensName
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) XCAFDoc_Material::GetDensName() const
{
  return myDensName;
}


//=======================================================================
//function : GetDensValType
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) XCAFDoc_Material::GetDensValType() const
{
  return myDensValType;
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_Material::ID() const
{
  return GetID();
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_Material::Restore(const Handle(TDF_Attribute)& With) 
{
  myName = Handle(XCAFDoc_Material)::DownCast(With)->GetName();
  myDensity = Handle(XCAFDoc_Material)::DownCast(With)->GetDensity();
  myDescription = Handle(XCAFDoc_Material)::DownCast(With)->GetDescription();
  myDensName = Handle(XCAFDoc_Material)::DownCast(With)->GetDensName();
  myDensValType = Handle(XCAFDoc_Material)::DownCast(With)->GetDensValType();
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) XCAFDoc_Material::NewEmpty() const
{
  return new XCAFDoc_Material();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void XCAFDoc_Material::Paste(const Handle(TDF_Attribute)& Into,
                             const Handle(TDF_RelocationTable)& /*RT*/) const
{
  Handle(XCAFDoc_Material)::DownCast(Into)->Set(myName,myDescription,myDensity,
                                                myDensName,myDensValType);
}

