// Created on: 2007-07-31
// Created by: Sergey ZARITCHNY
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <TDataStd_IntPackedMap.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TDataStd_DeltaOnModificationOfIntPackedMap.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DefaultDeltaOnModification.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_IntPackedMap, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDataStd_IntPackedMap::GetID()
{
  static Standard_GUID theGUID("7031faff-161e-44df-8239-7c264a81f5a1");
  return theGUID;
}

//=================================================================================================

Handle(TDataStd_IntPackedMap) TDataStd_IntPackedMap::Set(const TDF_Label&       theLabel,
                                                         const Standard_Boolean isDelta)
{
  Handle(TDataStd_IntPackedMap) anAtt;
  if (!theLabel.FindAttribute(TDataStd_IntPackedMap::GetID(), anAtt))
  {
    anAtt = new TDataStd_IntPackedMap;
    anAtt->Clear();
    anAtt->SetDelta(isDelta);
    theLabel.AddAttribute(anAtt);
  }
  return anAtt;
}

//=================================================================================================

TDataStd_IntPackedMap::TDataStd_IntPackedMap()
    : myIsDelta(Standard_False)
{
  myMap = new TColStd_HPackedMapOfInteger();
}

//=================================================================================================

Standard_Boolean TDataStd_IntPackedMap::ChangeMap(const Handle(TColStd_HPackedMapOfInteger)& theMap)
{
  if (theMap.IsNull())
    return Standard_False;
  if (myMap != theMap)
  {
    if (!myMap->Map().IsEqual(theMap->Map()))
    {
      Backup();
      myMap->ChangeMap().Assign(theMap->Map());
      return Standard_True;
    }
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean TDataStd_IntPackedMap::ChangeMap(const TColStd_PackedMapOfInteger& theMap)
{
  if (!myMap->Map().IsEqual(theMap))
  {
    Backup();
    myMap->ChangeMap().Assign(theMap);
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean TDataStd_IntPackedMap::Clear()
{
  if (!myMap->Map().IsEmpty())
  {
    Backup();
    myMap = new TColStd_HPackedMapOfInteger;
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Standard_Boolean TDataStd_IntPackedMap::Contains(const Standard_Integer theKey) const
{
  return myMap->Map().Contains(theKey);
}

//=================================================================================================

Standard_Boolean TDataStd_IntPackedMap::Add(const Standard_Integer theKey)
{
  Standard_Boolean aResult = !myMap->Map().Contains(theKey);
  if (aResult)
  {
    Backup();
    aResult = myMap->ChangeMap().Add(theKey);
  }
  return aResult;
}

//=================================================================================================

Standard_Boolean TDataStd_IntPackedMap::Remove(const Standard_Integer theKey)
{
  Standard_Boolean aResult = myMap->Map().Contains(theKey);
  if (aResult)
  {
    Backup();
    aResult = myMap->ChangeMap().Remove(theKey);
  }
  return aResult;
}

//=================================================================================================

Handle(TDF_Attribute) TDataStd_IntPackedMap::NewEmpty() const
{
  return new TDataStd_IntPackedMap;
}

//=================================================================================================

void TDataStd_IntPackedMap::Restore(const Handle(TDF_Attribute)& theWith)
{
  Handle(TDataStd_IntPackedMap) aWith = Handle(TDataStd_IntPackedMap)::DownCast(theWith);
  if (aWith->myMap.IsNull())
    myMap.Nullify();
  else
  {
    myMap = new TColStd_HPackedMapOfInteger;
    myMap->ChangeMap().Assign(aWith->myMap->Map());
    myIsDelta = aWith->myIsDelta;
  }
}

//=================================================================================================

void TDataStd_IntPackedMap::Paste(const Handle(TDF_Attribute)& theInto,
                                  const Handle(TDF_RelocationTable)&) const
{
  Handle(TDataStd_IntPackedMap) anInto = Handle(TDataStd_IntPackedMap)::DownCast(theInto);
  if (!anInto.IsNull())
  {
    anInto->ChangeMap(myMap);
    anInto->SetDelta(myIsDelta);
  }
}

//=================================================================================================

const Standard_GUID& TDataStd_IntPackedMap::ID() const
{
  return GetID();
}

//=================================================================================================

Standard_OStream& TDataStd_IntPackedMap::Dump(Standard_OStream& theOS) const
{
  Standard_OStream& anOS = TDF_Attribute::Dump(theOS);
  anOS << "IntPackedMap size = " << Extent();
  anOS << " Delta is " << (myIsDelta ? "ON" : "OFF");
  anOS << std::endl;
  return anOS;
}

//=================================================================================================

Handle(TDF_DeltaOnModification) TDataStd_IntPackedMap::DeltaOnModification(
  const Handle(TDF_Attribute)& OldAttribute) const
{
  if (myIsDelta)
    return new TDataStd_DeltaOnModificationOfIntPackedMap(
      Handle(TDataStd_IntPackedMap)::DownCast(OldAttribute));
  else
    return new TDF_DefaultDeltaOnModification(OldAttribute);
}

//=================================================================================================

void TDataStd_IntPackedMap::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  for (TColStd_PackedMapOfInteger::Iterator aMapIt(myMap->Map()); aMapIt.More(); aMapIt.Next())
  {
    Standard_Integer aKey = aMapIt.Key();
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aKey)
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsDelta)
}
