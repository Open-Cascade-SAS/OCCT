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
#include <BinMDF_DataMapIteratorOfTypeADriverMap.hxx>
#include <BinMDF_StringIdMap.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDF_ADriverTable,Standard_Transient)

//=======================================================================
//function : BinMDF_ADriverTable
//purpose  : Constructor
//=======================================================================
BinMDF_ADriverTable::BinMDF_ADriverTable ()
{
}

//=======================================================================
//function : AddDriver
//purpose  : Adds a translation driver <theDriver>.
//=======================================================================

void BinMDF_ADriverTable::AddDriver
                (const Handle(BinMDF_ADriver)& theDriver)
{
  const Handle(Standard_Type)& aType = theDriver->SourceType();
  myMap.Bind (aType, theDriver);
}

//=======================================================================
//function : AssignIds
//purpose  : Assigns the IDs to the drivers of the given Types.
//           It uses indices in the map as IDs.
//           Useful in storage procedure.
//=======================================================================

void BinMDF_ADriverTable::AssignIds
                (const TColStd_IndexedMapOfTransient& theTypes)
{
  myMapId.Clear();
  Standard_Integer i;
  for (i=1; i <= theTypes.Extent(); i++) {
    Handle(Standard_Type) aType (Handle(Standard_Type)::DownCast (theTypes(i)));
    if (myMap.IsBound (aType)) {
      myMapId.Bind (aType, i);
    }
    else {
      throw Standard_NoSuchObject((TCollection_AsciiString("BinMDF_ADriverTable::AssignIds : ") +
          "the type " + aType->Name() + " has not been registered").ToCString());
    }
  }
}

//=======================================================================
//function : AssignIds
//purpose  : Assigns the IDs to the drivers of the given Type Names;
//           It uses indices in the sequence as IDs.
//           Useful in retrieval procedure.
//=======================================================================

void BinMDF_ADriverTable::AssignIds
                (const TColStd_SequenceOfAsciiString& theTypeNames)
{
  myMapId.Clear();
  // first prepare the data map (TypeName => TypeID) for input types
  BinMDF_StringIdMap aStringIdMap;
  Standard_Integer i;
  for (i=1; i <= theTypeNames.Length(); i++) {
    const TCollection_AsciiString& aTypeName = theTypeNames(i);
    aStringIdMap.Bind (aTypeName, i);
  }
  // and now associate the names with the registered types
  BinMDF_DataMapIteratorOfTypeADriverMap it (myMap);
  for (; it.More(); it.Next()) {
    const Handle(Standard_Type)& aType = it.Key();
    const Handle(BinMDF_ADriver)& aDriver = it.Value();
    const TCollection_AsciiString& aTypeName = aDriver->TypeName();
    if (aStringIdMap.IsBound(aTypeName)) {
      i = aStringIdMap(aTypeName);
      myMapId.Bind (aType, i);
    }
  }
}
