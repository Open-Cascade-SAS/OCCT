// Created on: 2007-05-29
// Created by: Vlad Romashko
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

#include <BinMDataStd_BooleanListDriver.hxx>
#include <BinMDataStd.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array1.hxx>
#include <TDataStd_BooleanList.hxx>
#include <NCollection_List.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_BooleanListDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataStd_BooleanListDriver::BinMDataStd_BooleanListDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(TDataStd_BooleanList)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDataStd_BooleanListDriver::NewEmpty() const
{
  return new TDataStd_BooleanList();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool BinMDataStd_BooleanListDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                          const occ::handle<TDF_Attribute>& theTarget,
                                          BinObjMgt_RRelocationTable&       theRelocTable) const
{
  int aIndex, aFirstInd, aLastInd;
  if (!(theSource >> aFirstInd >> aLastInd))
    return false;

  const occ::handle<TDataStd_BooleanList> anAtt = occ::down_cast<TDataStd_BooleanList>(theTarget);
  if (aLastInd > 0)
  {

    const int aLength = aLastInd - aFirstInd + 1;
    if (aLength > 0)
    {
      NCollection_Array1<uint8_t> aTargetArray(aFirstInd, aLastInd);
      theSource.GetByteArray(&aTargetArray(aFirstInd), aLength);
      for (aIndex = aFirstInd; aIndex <= aLastInd; aIndex++)
      {
        anAtt->Append(aTargetArray.Value(aIndex) != 0);
      }
    }
  }

  BinMDataStd::SetAttributeID(theSource,
                              anAtt,
                              theRelocTable.GetHeaderData()->StorageVersion().IntegerValue());
  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_BooleanListDriver::Paste(
  const occ::handle<TDF_Attribute>& theSource,
  BinObjMgt_Persistent&             theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>&) const
{
  const occ::handle<TDataStd_BooleanList> anAtt = occ::down_cast<TDataStd_BooleanList>(theSource);
  const int                               aFirstInd = (anAtt->Extent() > 0) ? 1 : 0;
  const int                               aLastInd(anAtt->Extent());
  const int                               aLength = aLastInd - aFirstInd + 1;
  if (aLength <= 0)
    return;
  theTarget << aFirstInd << aLastInd;
  if (aLastInd == 0)
    return;
  NCollection_Array1<uint8_t>         aSourceArray(aFirstInd, aLastInd);
  NCollection_List<uint8_t>::Iterator itr(anAtt->List());
  for (int i = 1; itr.More(); itr.Next(), i++)
  {
    aSourceArray.SetValue(i, itr.Value());
  }
  uint8_t* aPtr = (uint8_t*)&aSourceArray(aFirstInd);
  theTarget.PutByteArray(aPtr, aLength);

  // process user defined guid
  if (anAtt->ID() != TDataStd_BooleanList::GetID())
    theTarget << anAtt->ID();
}
