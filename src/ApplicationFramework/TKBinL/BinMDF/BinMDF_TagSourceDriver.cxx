// Created on: 2002-11-19
// Created by: Edward AGAPOV (eap)
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

#include <BinMDF_TagSourceDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_TagSource.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDF_TagSourceDriver, BinMDF_ADriver)

//=================================================================================================

BinMDF_TagSourceDriver::BinMDF_TagSourceDriver(const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, nullptr)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDF_TagSourceDriver::NewEmpty() const
{
  return (new TDF_TagSource());
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool BinMDF_TagSourceDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                   const occ::handle<TDF_Attribute>& theTarget,
                                   BinObjMgt_RRelocationTable&) const
{
  occ::handle<TDF_TagSource> aTag = occ::down_cast<TDF_TagSource>(theTarget);
  int                        aValue;
  bool                       ok = theSource >> aValue;
  if (ok)
    aTag->Set(aValue);
  return ok;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void BinMDF_TagSourceDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                   BinObjMgt_Persistent&             theTarget,
                                   NCollection_IndexedMap<occ::handle<Standard_Transient>>&) const
{
  occ::handle<TDF_TagSource> aTag = occ::down_cast<TDF_TagSource>(theSource);
  theTarget << aTag->Get();
}
