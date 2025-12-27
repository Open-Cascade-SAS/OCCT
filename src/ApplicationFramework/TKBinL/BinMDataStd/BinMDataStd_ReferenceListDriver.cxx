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

#include <BinMDataStd_ReferenceListDriver.hxx>
#include <BinMDataStd.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_Label.hxx>
#include <NCollection_List.hxx>
#include <TDF_Tool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_ReferenceListDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataStd_ReferenceListDriver::BinMDataStd_ReferenceListDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(TDataStd_ReferenceList)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDataStd_ReferenceListDriver::NewEmpty() const
{
  return new TDataStd_ReferenceList();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool BinMDataStd_ReferenceListDriver::Paste(
  const BinObjMgt_Persistent&  theSource,
  const occ::handle<TDF_Attribute>& theTarget,
  BinObjMgt_RRelocationTable&  theRelocTable) const
{
  int aFirstInd, aLastInd;
  if (!(theSource >> aFirstInd >> aLastInd))
    return false;

  const occ::handle<TDataStd_ReferenceList> anAtt = occ::down_cast<TDataStd_ReferenceList>(theTarget);
  if (aLastInd > 0)
  {

    const int aLength = aLastInd - aFirstInd + 1;
    if (aLength <= 0)
      return false;
    for (int i = aFirstInd; i <= aLastInd; i++)
    {
      TCollection_AsciiString entry;
      if (!(theSource >> entry))
        return false;
      TDF_Label L;
      TDF_Tool::Label(anAtt->Label().Data(), entry, L, true);
      if (!L.IsNull())
        anAtt->Append(L);
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
void BinMDataStd_ReferenceListDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                            BinObjMgt_Persistent&        theTarget,
                                            NCollection_IndexedMap<occ::handle<Standard_Transient>>&) const
{
  const occ::handle<TDataStd_ReferenceList> anAtt = occ::down_cast<TDataStd_ReferenceList>(theSource);
  if (anAtt.IsNull())
    return;
  const int aFirstInd = (anAtt->Extent() > 0) ? 1 : 0;
  const int aLastInd(anAtt->Extent());
  theTarget << aFirstInd << aLastInd;
  if (aLastInd == 0)
    return;
  NCollection_List<TDF_Label>::Iterator itr(anAtt->List());
  for (int i = aFirstInd; itr.More(); itr.Next(), i++)
  {
    TDF_Label L = itr.Value();
    if (!L.IsNull())
    {
      TCollection_AsciiString entry;
      TDF_Tool::Entry(L, entry);
      theTarget << entry;
    }
  }

  // process user defined guid
  if (anAtt->ID() != TDataStd_ReferenceList::GetID())
    theTarget << anAtt->ID();
}
