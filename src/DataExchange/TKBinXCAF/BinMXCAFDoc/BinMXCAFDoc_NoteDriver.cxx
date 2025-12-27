// Created on: 2017-02-10
// Created by: Eugeny NIKONOV
// Copyright (c) 2005-2017 OPEN CASCADE SAS
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

#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <BinMXCAFDoc_NoteDriver.hxx>
#include <XCAFDoc_Note.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_NoteDriver, BinMDF_ADriver)

//=================================================================================================

BinMXCAFDoc_NoteDriver::BinMXCAFDoc_NoteDriver(const occ::handle<Message_Messenger>& theMsgDriver,
                                               const char*                 theName)
    : BinMDF_ADriver(theMsgDriver, theName)
{
}

//=================================================================================================

bool BinMXCAFDoc_NoteDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                               const occ::handle<TDF_Attribute>& theTarget,
                                               BinObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  occ::handle<XCAFDoc_Note> aNote = occ::down_cast<XCAFDoc_Note>(theTarget);
  if (aNote.IsNull())
    return false;

  TCollection_ExtendedString aUserName, aTimeStamp;
  if (!(theSource >> aUserName >> aTimeStamp))
    return false;

  aNote->Set(aUserName, aTimeStamp);

  return true;
}

//=================================================================================================

void BinMXCAFDoc_NoteDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                   BinObjMgt_Persistent&        theTarget,
                                   NCollection_IndexedMap<occ::handle<Standard_Transient>>& /*theRelocTable*/) const
{
  occ::handle<XCAFDoc_Note> aNote = occ::down_cast<XCAFDoc_Note>(theSource);
  if (!aNote.IsNull())
    theTarget << aNote->UserName() << aNote->TimeStamp();
}
