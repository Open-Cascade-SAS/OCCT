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

#include <XmlMDF_ADriverTable.hxx>

#include <Message_Messenger.hxx>
#include <TDF_DerivedAttribute.hxx>
#include <TDF_AttributeList.hxx>
#include <XmlMDF_ADriver.hxx>
#include <XmlMDF_DerivedDriver.hxx>

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
//function : AddDerivedDriver
//purpose  :
//=======================================================================
void XmlMDF_ADriverTable::AddDerivedDriver (const Handle(TDF_Attribute)& theInstance)
{
  const Handle(Standard_Type)& anInstanceType = theInstance->DynamicType();
  if (!myMap.IsBound (anInstanceType)) // no direct driver, use a derived one
  {
    for (Handle(Standard_Type) aType = anInstanceType->Parent(); !aType.IsNull(); aType = aType->Parent())
    {
      if (myMap.IsBound (aType))
      {
        Handle(XmlMDF_ADriver) aDriver = new XmlMDF_DerivedDriver (theInstance, myMap (aType));
        myMap.Bind (anInstanceType, aDriver);
        return;
      }
    }
  }
}

//=======================================================================
//function : AddDerivedDriver
//purpose  :
//=======================================================================
const Handle(Standard_Type)& XmlMDF_ADriverTable::AddDerivedDriver (Standard_CString theDerivedType)
{
  if (Handle(TDF_Attribute) anInstance = TDF_DerivedAttribute::Attribute (theDerivedType))
  {
    AddDerivedDriver (anInstance);
    return anInstance->DynamicType();
  }
  static const Handle(Standard_Type) aNullType;
  return aNullType;
}

//=======================================================================
//function : GetDriver
//purpose  : 
//=======================================================================
Standard_Boolean XmlMDF_ADriverTable::GetDriver
  (const Handle(Standard_Type)& aType,
   Handle(XmlMDF_ADriver)&      anHDriver)
{
  if (!myMap.IsBound (aType)) // try to assign driver for derived type
  {
    AddDerivedDriver (aType->Name());
  }
  if (myMap.IsBound(aType))
  {
    anHDriver = myMap.Find(aType);
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : CreateDrvMap
//purpose  :
//=======================================================================
void XmlMDF_ADriverTable::CreateDrvMap (XmlMDF_MapOfDriver& theDriverMap)
{
  // add derived drivers not yet registered in the map
  TDF_AttributeList aDerived;
  TDF_DerivedAttribute::Attributes (aDerived);
  for (TDF_AttributeList::Iterator aDerIter (aDerived); aDerIter.More(); aDerIter.Next())
  {
    if (!myMap.IsBound (aDerIter.Value()->DynamicType()))
    {
      AddDerivedDriver (aDerIter.Value());
    }
  }

  // put everything to the map
  for (XmlMDF_DataMapIteratorOfTypeADriverMap anIter (myMap); anIter.More(); anIter.Next())
  {
    const Handle(XmlMDF_ADriver)& aDriver = anIter.Value();
    const TCollection_AsciiString aTypeName = aDriver->TypeName();
    if (!theDriverMap.IsBound (aTypeName))
    {
      theDriverMap.Bind (aTypeName, aDriver);
    }
    else
    {
      aDriver->MessageDriver()->Send (TCollection_AsciiString ("Warning: skipped driver name: \"")
                                    + aTypeName + "\"", Message_Warning);
    }
  }
}
