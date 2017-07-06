// Created on: 2001-09-26
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Standard_Type.hxx>
#include <XmlMDF_ADriver.hxx>
#include <XmlMDF_ADriverTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDF_ADriverTable,Standard_Transient)

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
