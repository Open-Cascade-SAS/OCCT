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

#include <BinMDataStd_ReferenceArrayDriver.hxx>
#include <BinMDataStd.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_ReferenceArrayDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataStd_ReferenceArrayDriver::BinMDataStd_ReferenceArrayDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(TDataStd_ReferenceArray)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDataStd_ReferenceArrayDriver::NewEmpty() const
{
  return new TDataStd_ReferenceArray();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool BinMDataStd_ReferenceArrayDriver::Paste(
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

  const occ::handle<TDataStd_ReferenceArray> anAtt =
    occ::down_cast<TDataStd_ReferenceArray>(theTarget);
  anAtt->Init(aFirstInd, aLastInd);
  for (int i = aFirstInd; i <= aLastInd; i++)
  {
    TCollection_AsciiString entry;
    if (!(theSource >> entry))
      return false;
    TDF_Label L;
    TDF_Tool::Label(anAtt->Label().Data(), entry, L, true);
    if (!L.IsNull())
      anAtt->SetValue(i, L);
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
void BinMDataStd_ReferenceArrayDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                             BinObjMgt_Persistent&        theTarget,
                                             NCollection_IndexedMap<occ::handle<Standard_Transient>>&) const
{
  occ::handle<TDataStd_ReferenceArray> anAtt = occ::down_cast<TDataStd_ReferenceArray>(theSource);
  int aFirstInd = anAtt->Lower(), aLastInd = anAtt->Upper(), i = aFirstInd;
  if (aFirstInd > aLastInd)
    return;
  theTarget << aFirstInd << aLastInd;
  for (; i <= aLastInd; i++)
  {
    TDF_Label L = anAtt->Value(i);
    if (!L.IsNull())
    {
      TCollection_AsciiString entry;
      TDF_Tool::Entry(L, entry);
      theTarget << entry;
    }
  }

  // process user defined guid
  if (anAtt->ID() != TDataStd_ReferenceArray::GetID())
    theTarget << anAtt->ID();
}
