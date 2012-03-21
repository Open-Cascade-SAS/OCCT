// Created on: 2001-09-26
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
