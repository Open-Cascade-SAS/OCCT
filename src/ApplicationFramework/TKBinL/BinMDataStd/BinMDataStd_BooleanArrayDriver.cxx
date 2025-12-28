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

#include <BinMDataStd_BooleanArrayDriver.hxx>
#include <BinMDataStd.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_BooleanArrayDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataStd_BooleanArrayDriver::BinMDataStd_BooleanArrayDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(TDataStd_BooleanArray)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDataStd_BooleanArrayDriver::NewEmpty() const
{
  return new TDataStd_BooleanArray();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool BinMDataStd_BooleanArrayDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                           const occ::handle<TDF_Attribute>& theTarget,
                                           BinObjMgt_RRelocationTable&       theRelocTable) const
{
  int aFirstInd, aLastInd;
  if (!(theSource >> aFirstInd >> aLastInd))
    return false;
  if (aLastInd < aFirstInd)
    return false;

  NCollection_Array1<uint8_t> aTargetArray(0, (aLastInd - aFirstInd + 1) >> 3);
  theSource.GetByteArray(&aTargetArray(0), aTargetArray.Length());

  const occ::handle<TDataStd_BooleanArray> anAtt = occ::down_cast<TDataStd_BooleanArray>(theTarget);
  anAtt->Init(aFirstInd, aLastInd);
  occ::handle<NCollection_HArray1<uint8_t>> bytes =
    new NCollection_HArray1<uint8_t>(aTargetArray.Lower(), aTargetArray.Upper());
  int lower = bytes->Lower(), i = lower, upper = bytes->Upper();
  for (; i <= upper; i++)
  {
    bytes->SetValue(i, aTargetArray.Value(i));
  }
  anAtt->SetInternalArray(bytes);
  BinMDataStd::SetAttributeID(theSource,
                              anAtt,
                              theRelocTable.GetHeaderData()->StorageVersion().IntegerValue());
  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataStd_BooleanArrayDriver::Paste(
  const occ::handle<TDF_Attribute>& theSource,
  BinObjMgt_Persistent&             theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>&) const
{
  occ::handle<TDataStd_BooleanArray> anAtt     = occ::down_cast<TDataStd_BooleanArray>(theSource);
  const int                          aFirstInd = anAtt->Lower();
  const int                          aLastInd  = anAtt->Upper();
  if (aLastInd < aFirstInd)
    return;
  theTarget << aFirstInd << aLastInd;

  const occ::handle<NCollection_HArray1<uint8_t>>& bytes = anAtt->InternalArray();
  NCollection_Array1<uint8_t>                      aSourceArray(bytes->Lower(), bytes->Upper());
  int lower = bytes->Lower(), i = lower, upper = bytes->Upper();
  for (; i <= upper; i++)
  {
    aSourceArray.SetValue(i, bytes->Value(i));
  }
  uint8_t* aPtr = (uint8_t*)&aSourceArray(lower);
  theTarget.PutByteArray(aPtr, upper - lower + 1);

  // process user defined guid
  if (anAtt->ID() != TDataStd_BooleanArray::GetID())
    theTarget << anAtt->ID();
}
