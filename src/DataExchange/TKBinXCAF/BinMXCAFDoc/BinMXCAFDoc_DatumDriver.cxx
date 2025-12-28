// Created on: 2008-12-10
// Created by: Pavel TELKOV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <BinMXCAFDoc_DatumDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_Datum.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_DatumDriver, BinMDF_ADriver)

//=================================================================================================

BinMXCAFDoc_DatumDriver::BinMXCAFDoc_DatumDriver(const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_Datum)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMXCAFDoc_DatumDriver::NewEmpty() const
{
  return new XCAFDoc_Datum();
}

//=================================================================================================

bool BinMXCAFDoc_DatumDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                    const occ::handle<TDF_Attribute>& theTarget,
                                    BinObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  occ::handle<XCAFDoc_Datum> anAtt = occ::down_cast<XCAFDoc_Datum>(theTarget);
  TCollection_AsciiString    aName, aDescr, anId;
  if (!(theSource >> aName >> aDescr >> anId))
    return false;

  anAtt->Set(new TCollection_HAsciiString(aName),
             new TCollection_HAsciiString(aDescr),
             new TCollection_HAsciiString(anId));
  return true;
}

//=================================================================================================

void BinMXCAFDoc_DatumDriver::Paste(
  const occ::handle<TDF_Attribute>& theSource,
  BinObjMgt_Persistent&             theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>& /*theRelocTable*/) const
{
  occ::handle<XCAFDoc_Datum> anAtt = occ::down_cast<XCAFDoc_Datum>(theSource);
  if (!anAtt->GetName().IsNull())
    theTarget << anAtt->GetName()->String();
  else
    theTarget << TCollection_AsciiString("");

  if (!anAtt->GetDescription().IsNull())
    theTarget << anAtt->GetDescription()->String();
  else
    theTarget << TCollection_AsciiString("");

  if (!anAtt->GetIdentification().IsNull())
    theTarget << anAtt->GetIdentification()->String();
  else
    theTarget << TCollection_AsciiString("");
}
