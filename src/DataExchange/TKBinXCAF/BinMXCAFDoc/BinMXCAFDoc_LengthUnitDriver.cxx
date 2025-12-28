// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <BinMXCAFDoc_LengthUnitDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_LengthUnit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_LengthUnitDriver, BinMDF_ADriver)

//=================================================================================================

BinMXCAFDoc_LengthUnitDriver::BinMXCAFDoc_LengthUnitDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_LengthUnit)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMXCAFDoc_LengthUnitDriver::NewEmpty() const
{
  return new XCAFDoc_LengthUnit();
}

//=================================================================================================

bool BinMXCAFDoc_LengthUnitDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                         const occ::handle<TDF_Attribute>& theTarget,
                                         BinObjMgt_RRelocationTable&       theRelocTable) const
{
  (void)theRelocTable;
  occ::handle<XCAFDoc_LengthUnit> anAtt = occ::down_cast<XCAFDoc_LengthUnit>(theTarget);
  TCollection_AsciiString         aName;
  double                          aScaleFactor = 1.;
  bool                            isOk         = theSource >> aName >> aScaleFactor;
  if (isOk)
  {
    anAtt->Set(aName, aScaleFactor);
  }
  return isOk;
}

//=================================================================================================

void BinMXCAFDoc_LengthUnitDriver::Paste(
  const occ::handle<TDF_Attribute>&                        theSource,
  BinObjMgt_Persistent&                                    theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>& theRelocTable) const
{
  (void)theRelocTable;
  occ::handle<XCAFDoc_LengthUnit> anAtt = occ::down_cast<XCAFDoc_LengthUnit>(theSource);
  theTarget << anAtt->GetUnitName() << anAtt->GetUnitValue();
}
