// Created on: 2002-10-29
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <BinMDF_ADriver.hxx>
#include <BinMDF_ADriverTable.hxx>
#include <Standard_Type.hxx>
#include <NCollection_DataMap.hxx>
#include <BinMDF_DerivedDriver.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TDF_DerivedAttribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDF_ADriverTable, Standard_Transient)

//=================================================================================================

BinMDF_ADriverTable::BinMDF_ADriverTable() = default;

//=======================================================================
// function : AddDriver
// purpose  : Adds a translation driver <theDriver>.
//=======================================================================

void BinMDF_ADriverTable::AddDriver(const occ::handle<BinMDF_ADriver>& theDriver)
{
  const occ::handle<Standard_Type>& aType = theDriver->SourceType();
  myMap.Bind(aType, theDriver);
}

//=================================================================================================

void BinMDF_ADriverTable::AddDerivedDriver(const occ::handle<TDF_Attribute>& theInstance)
{
  const occ::handle<Standard_Type>& anInstanceType = theInstance->DynamicType();
  if (!myMap.IsBound(anInstanceType)) // no direct driver, use a derived one
  {
    for (occ::handle<Standard_Type> aType = anInstanceType->Parent(); !aType.IsNull();
         aType                            = aType->Parent())
    {
      const occ::handle<BinMDF_ADriver>* pDriver = myMap.Seek(aType);
      if (pDriver)
      {
        occ::handle<BinMDF_DerivedDriver> aDriver = new BinMDF_DerivedDriver(theInstance, *pDriver);
        myMap.Bind(anInstanceType, aDriver);
        return;
      }
    }
  }
}

//=================================================================================================

const occ::handle<Standard_Type>& BinMDF_ADriverTable::AddDerivedDriver(const char* theDerivedType)
{
  if (occ::handle<TDF_Attribute> anInstance = TDF_DerivedAttribute::Attribute(theDerivedType))
  {
    AddDerivedDriver(anInstance);
    return anInstance->DynamicType();
  }
  static const occ::handle<Standard_Type> aNullType;
  return aNullType;
}

//=======================================================================
// function : AssignIds
// purpose  : Assigns the IDs to the drivers of the given Types.
//           It uses indices in the map as IDs.
//           Useful in storage procedure.
//=======================================================================

void BinMDF_ADriverTable::AssignIds(
  const NCollection_IndexedMap<occ::handle<Standard_Transient>>& theTypes)
{
  myMapId.Clear();
  int i;
  for (i = 1; i <= theTypes.Extent(); i++)
  {
    occ::handle<Standard_Type> aType(occ::down_cast<Standard_Type>(theTypes(i)));
    if (myMap.IsBound(aType))
    {
      myMapId.Bind(aType, i);
    }
    else
    {
      throw Standard_NoSuchObject((TCollection_AsciiString("BinMDF_ADriverTable::AssignIds : ")
                                   + "the type " + aType->Name() + " has not been registered")
                                    .ToCString());
    }
  }
}

//=======================================================================
// function : AssignIds
// purpose  : Assigns the IDs to the drivers of the given Type Names;
//           It uses indices in the sequence as IDs.
//           Useful in retrieval procedure.
//=======================================================================

void BinMDF_ADriverTable::AssignIds(
  const NCollection_Sequence<TCollection_AsciiString>& theTypeNames)
{
  myMapId.Clear();
  // first prepare the data map (TypeName => TypeID) for input types
  NCollection_DataMap<TCollection_AsciiString, int> aStringIdMap;
  int                                               i;
  for (i = 1; i <= theTypeNames.Length(); i++)
  {
    const TCollection_AsciiString& aTypeName = theTypeNames(i);
    aStringIdMap.Bind(aTypeName, i);
  }
  // and now associate the names with the registered types
  NCollection_DataMap<occ::handle<Standard_Type>, occ::handle<BinMDF_ADriver>>::Iterator it(myMap);
  for (; it.More(); it.Next())
  {
    const occ::handle<Standard_Type>&  aType     = it.Key();
    const occ::handle<BinMDF_ADriver>& aDriver   = it.Value();
    const TCollection_AsciiString&     aTypeName = aDriver->TypeName();
    const int*                         pId       = aStringIdMap.Seek(aTypeName);
    if (pId)
    {
      myMapId.Bind(aType, *pId);
    }
  }

  // try to add derived drivers for attributes not found in myMap
  for (NCollection_DataMap<TCollection_AsciiString, int>::Iterator aStrId(aStringIdMap);
       aStrId.More();
       aStrId.Next())
  {
    if (!myMapId.IsBound2(aStrId.Value()))
    {
      if (occ::handle<Standard_Type> anAdded = AddDerivedDriver(aStrId.Key().ToCString()))
      {
        myMapId.Bind(anAdded, aStrId.Value());
      }
    }
  }
}
