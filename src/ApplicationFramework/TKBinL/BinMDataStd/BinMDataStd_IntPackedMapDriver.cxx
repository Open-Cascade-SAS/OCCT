// Created on: 2007-08-01
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

#include <BinMDataStd.hxx>
#include <BinMDataStd_IntPackedMapDriver.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <TDF_Attribute.hxx>
#include <TDocStd_FormatVersion.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_IntPackedMapDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataStd_IntPackedMapDriver::BinMDataStd_IntPackedMapDriver(
  const occ::handle<Message_Messenger>& theMessageDriver)
    : BinMDF_ADriver(theMessageDriver, STANDARD_TYPE(TDataStd_IntPackedMap)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDataStd_IntPackedMapDriver::NewEmpty() const
{
  return new TDataStd_IntPackedMap;
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================

bool BinMDataStd_IntPackedMapDriver::Paste(const BinObjMgt_Persistent&       Source,
                                           const occ::handle<TDF_Attribute>& Target,
                                           BinObjMgt_RRelocationTable&       RelocTable) const
{
  occ::handle<TDataStd_IntPackedMap> aTagAtt = occ::down_cast<TDataStd_IntPackedMap>(Target);
  if (aTagAtt.IsNull())
  {
    myMessageDriver->Send("IntPackedMapDriver:: The target attribute is Null.", Message_Fail);
    return false;
  }

  int aSize = 0;
  if (!(Source >> aSize))
  {
    myMessageDriver->Send("Cannot retrieve size for IntPackedMap attribute.", Message_Fail);
    return false;
  }
  if (aSize)
  {
    occ::handle<TColStd_HPackedMapOfInteger> aHMap = new TColStd_HPackedMapOfInteger();
    int                                      aKey;
    for (int i = 0; i < aSize; i++)
    {
      bool ok = Source >> aKey;
      if (!ok)
      {
        myMessageDriver->Send("Cannot retrieve integer member for IntPackedMap attribute.",
                              Message_Fail);
        return false;
      }
      if (!aHMap->ChangeMap().Add(aKey))
        return false;
    }
    aTagAtt->ChangeMap(aHMap);
  }

  bool aDelta(false);
  if (RelocTable.GetHeaderData()->StorageVersion().IntegerValue()
      >= TDocStd_FormatVersion_VERSION_3)
  {
    uint8_t aDeltaValue;
    if (!(Source >> aDeltaValue))
      return false;
    else
      aDelta = (aDeltaValue != 0);
  }
  aTagAtt->SetDelta(aDelta);
  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_IntPackedMapDriver::Paste(
  const occ::handle<TDF_Attribute>& Source,
  BinObjMgt_Persistent&             Target,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>& /*RelocTable*/) const
{
  occ::handle<TDataStd_IntPackedMap> anAtt = occ::down_cast<TDataStd_IntPackedMap>(Source);
  if (anAtt.IsNull())
  {
    myMessageDriver->Send("IntPackedMapDriver:: The source attribute is Null.", Message_Fail);
    return;
  }
  int aSize = (anAtt->IsEmpty()) ? 0 : anAtt->Extent();
  Target << aSize;
  if (aSize)
  {
    TColStd_PackedMapOfInteger::Iterator anIt(anAtt->GetMap());
    for (; anIt.More(); anIt.Next())
      Target << anIt.Key();
  }
  Target << (uint8_t)(anAtt->GetDelta() ? 1 : 0);
}
