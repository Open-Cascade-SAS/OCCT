// File:      XmlMDF_ADriverTable.cxx
// Created:   26.09.01 16:24:36
// Author:    Julia DOROVSKIKH
// Copyright: Open Cascade 2001
// History:

#include <XmlMDF_ADriverTable.ixx>

//=======================================================================
//function : MDF_ADriverTable
//purpose  : 
//=======================================================================
XmlMDF_ADriverTable::XmlMDF_ADriverTable()
{}

//=======================================================================
//function : AddDriver
//purpose  : 
//=======================================================================
void XmlMDF_ADriverTable::AddDriver (const Handle(XmlMDF_ADriver)& anHDriver)
{
  const Handle(Standard_Type)& type = anHDriver->SourceType();

  // to make possible for applications to redefine standard attribute drivers
  myMap.UnBind(type);

  myMap.Bind(type,anHDriver);
}

//=======================================================================
//function : GetDrivers
//purpose  : 
//=======================================================================
const XmlMDF_TypeADriverMap& XmlMDF_ADriverTable::GetDrivers() const
{
  return myMap;
}

//=======================================================================
//function : GetDriver
//purpose  : 
//=======================================================================
Standard_Boolean XmlMDF_ADriverTable::GetDriver
  (const Handle(Standard_Type)& aType,
   Handle(XmlMDF_ADriver)&      anHDriver) const
{
  if (myMap.IsBound(aType))
  {
    anHDriver = myMap.Find(aType);
    return Standard_True;
  }
  return Standard_False;
}
