// Created on: 2005-05-17
// Created by: Eugeny NAPALKOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <BinMXCAFDoc_ColorDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_Color.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_ColorDriver, BinMDF_ADriver)

//=================================================================================================

BinMXCAFDoc_ColorDriver::BinMXCAFDoc_ColorDriver(const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_Color)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMXCAFDoc_ColorDriver::NewEmpty() const
{
  return new XCAFDoc_Color();
}

//=================================================================================================

bool BinMXCAFDoc_ColorDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                    const occ::handle<TDF_Attribute>& theTarget,
                                    BinObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  occ::handle<XCAFDoc_Color> anAtt = occ::down_cast<XCAFDoc_Color>(theTarget);
  double                     R, G, B;
  float                      alpha;
  bool                       isOk = theSource >> R >> G >> B;
  if (isOk)
  {
    bool isRGBA = theSource >> alpha;
    if (!isRGBA)
      alpha = 1.0;
    anAtt->Set(R, G, B, alpha);
  }
  return isOk;
}

//=================================================================================================

void BinMXCAFDoc_ColorDriver::Paste(
  const occ::handle<TDF_Attribute>& theSource,
  BinObjMgt_Persistent&             theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>& /*theRelocTable*/) const
{
  occ::handle<XCAFDoc_Color> anAtt = occ::down_cast<XCAFDoc_Color>(theSource);
  double                     R, G, B;
  float                      alpha;
  anAtt->GetRGB(R, G, B);
  alpha = anAtt->GetAlpha();
  theTarget << R << G << B << alpha;
}
