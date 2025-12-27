// Created on: 2002-10-31
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

#include <BinMDataStd.hxx>
#include <BinMDataStd_IntegerArrayDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDocStd_FormatVersion.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_IntegerArrayDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataStd_IntegerArrayDriver::BinMDataStd_IntegerArrayDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(TDataStd_IntegerArray)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDataStd_IntegerArrayDriver::NewEmpty() const
{
  return new TDataStd_IntegerArray();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================

bool BinMDataStd_IntegerArrayDriver::Paste(
  const BinObjMgt_Persistent&  theSource,
  const occ::handle<TDF_Attribute>& theTarget,
  BinObjMgt_RRelocationTable&  theRelocTable) const
{
  int aFirstInd, aLastInd;
  if (!(theSource >> aFirstInd >> aLastInd))
    return false;
  const int aLength = aLastInd - aFirstInd + 1;
  if (aLength <= 0)
    return false;

  const occ::handle<TDataStd_IntegerArray> anAtt = occ::down_cast<TDataStd_IntegerArray>(theTarget);
  anAtt->Init(aFirstInd, aLastInd);
  NCollection_Array1<int>& aTargetArray = anAtt->Array()->ChangeArray1();
  if (!theSource.GetIntArray(&aTargetArray(aFirstInd), aLength))
    return false;
  bool aDelta(false);
  if (theRelocTable.GetHeaderData()->StorageVersion().IntegerValue()
      >= TDocStd_FormatVersion_VERSION_3)
  {
    uint8_t aDeltaValue;
    if (!(theSource >> aDeltaValue))
      return false;
    else
      aDelta = (aDeltaValue != 0);
  }
#ifdef OCCT_DEBUG
  // std::cout << "Current Document Format Version = " <<
  // theRelocTable.GetHeaderData()->StorageVersion().IntegerValue() <<std::endl;
#endif
  anAtt->SetDelta(aDelta);

  BinMDataStd::SetAttributeID(theSource,
                              anAtt,
                              theRelocTable.GetHeaderData()->StorageVersion().IntegerValue());
  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataStd_IntegerArrayDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                           BinObjMgt_Persistent&        theTarget,
                                           NCollection_IndexedMap<occ::handle<Standard_Transient>>&) const
{
  occ::handle<TDataStd_IntegerArray>  anAtt        = occ::down_cast<TDataStd_IntegerArray>(theSource);
  const NCollection_Array1<int>& aSourceArray = anAtt->Array()->Array1();
  const int         aFirstInd    = aSourceArray.Lower();
  const int         aLastInd     = aSourceArray.Upper();
  const int         aLength      = aLastInd - aFirstInd + 1;
  theTarget << aFirstInd << aLastInd;
  int* aPtr = (int*)&aSourceArray(aFirstInd);
  theTarget.PutIntArray(aPtr, aLength);
  theTarget << (uint8_t)(anAtt->GetDelta() ? 1 : 0);

  // process user defined guid
  if (anAtt->ID() != TDataStd_IntegerArray::GetID())
    theTarget << anAtt->ID();
}
