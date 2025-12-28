// Created on: 2017-02-13
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
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <BinMXCAFDoc_NoteBinDataDriver.hxx>
#include <XCAFDoc_NoteBinData.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_NoteBinDataDriver, BinMXCAFDoc_NoteDriver)

//=================================================================================================

BinMXCAFDoc_NoteBinDataDriver::BinMXCAFDoc_NoteBinDataDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMXCAFDoc_NoteDriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_NoteBinData)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMXCAFDoc_NoteBinDataDriver::NewEmpty() const
{
  return new XCAFDoc_NoteBinData();
}

//=================================================================================================

bool BinMXCAFDoc_NoteBinDataDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                          const occ::handle<TDF_Attribute>& theTarget,
                                          BinObjMgt_RRelocationTable&       theRelocTable) const
{
  if (!BinMXCAFDoc_NoteDriver::Paste(theSource, theTarget, theRelocTable))
    return false;

  occ::handle<XCAFDoc_NoteBinData> aNote = occ::down_cast<XCAFDoc_NoteBinData>(theTarget);
  if (aNote.IsNull())
    return false;

  TCollection_ExtendedString aTitle;
  TCollection_AsciiString    aMIMEtype;
  int                        nbSize;
  if (!(theSource >> aTitle >> aMIMEtype >> nbSize))
    return false;

  occ::handle<NCollection_HArray1<uint8_t>> aData;
  if (nbSize > 0)
  {
    aData.reset(new NCollection_HArray1<uint8_t>(1, nbSize));
    theSource.GetByteArray(&aData->ChangeFirst(), nbSize);
  }

  aNote->Set(aTitle, aMIMEtype, aData);

  return true;
}

//=================================================================================================

void BinMXCAFDoc_NoteBinDataDriver::Paste(
  const occ::handle<TDF_Attribute>&                        theSource,
  BinObjMgt_Persistent&                                    theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>& theRelocTable) const
{
  BinMXCAFDoc_NoteDriver::Paste(theSource, theTarget, theRelocTable);

  occ::handle<XCAFDoc_NoteBinData> aNote = occ::down_cast<XCAFDoc_NoteBinData>(theSource);
  if (!aNote.IsNull())
  {
    theTarget << aNote->Title() << aNote->MIMEtype() << aNote->Size();
    if (aNote->Size() > 0)
      theTarget.PutByteArray(&aNote->Data()->ChangeFirst(), aNote->Size());
  }
}
