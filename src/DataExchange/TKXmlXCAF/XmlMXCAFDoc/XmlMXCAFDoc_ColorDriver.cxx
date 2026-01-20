// Created on: 2001-09-11
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <XCAFDoc_Color.hxx>
#include <XmlMXCAFDoc_ColorDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_ColorDriver, XmlMDF_ADriver)

//=================================================================================================

XmlMXCAFDoc_ColorDriver::XmlMXCAFDoc_ColorDriver(const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, "xcaf", "Color")
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMXCAFDoc_ColorDriver::NewEmpty() const
{
  return (new XCAFDoc_Color());
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool XmlMXCAFDoc_ColorDriver::Paste(const XmlObjMgt_Persistent&  theSource,
                                                const occ::handle<TDF_Attribute>& theTarget,
                                                XmlObjMgt_RRelocationTable&) const
{
  int    aValue;
  XmlObjMgt_DOMString anIntStr = XmlObjMgt::GetStringValue(theSource);

  if (anIntStr.GetInteger(aValue) == false)
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve Color attribute from \"") + anIntStr + "\"";
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  occ::handle<XCAFDoc_Color> anInt = occ::down_cast<XCAFDoc_Color>(theTarget);
  anInt->Set((Quantity_NameOfColor)aValue);

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMXCAFDoc_ColorDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                    XmlObjMgt_Persistent&        theTarget,
                                    XmlObjMgt_SRelocationTable&) const
{
  occ::handle<XCAFDoc_Color> anInt = occ::down_cast<XCAFDoc_Color>(theSource);
  XmlObjMgt::SetStringValue(theTarget, (int)anInt->GetNOC());
}
