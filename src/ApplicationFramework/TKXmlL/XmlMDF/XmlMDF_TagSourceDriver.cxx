// Created on: 2001-08-29
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

// AGV 150202: Changed prototype XmlObjMgt::SetStringValue()

#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDF_TagSource.hxx>
#include <XmlMDF_TagSourceDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDF_TagSourceDriver, XmlMDF_ADriver)

//=================================================================================================

XmlMDF_TagSourceDriver::XmlMDF_TagSourceDriver(const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, nullptr)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMDF_TagSourceDriver::NewEmpty() const
{
  return (new TDF_TagSource());
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool XmlMDF_TagSourceDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                   const occ::handle<TDF_Attribute>& theTarget,
                                   XmlObjMgt_RRelocationTable&) const
{
  int                 aTag;
  XmlObjMgt_DOMString aTagStr = XmlObjMgt::GetStringValue(theSource.Element());

  if (!aTagStr.GetInteger(aTag))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve TagSource attribute from \"") + aTagStr + "\"";
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  if (aTag < 0)
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Invalid value of TagSource retrieved: ") + aTag;
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  occ::handle<TDF_TagSource> aT = occ::down_cast<TDF_TagSource>(theTarget);
  aT->Set(aTag);

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDF_TagSourceDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                   XmlObjMgt_Persistent&             theTarget,
                                   XmlObjMgt_SRelocationTable&) const
{
  occ::handle<TDF_TagSource> aTag = occ::down_cast<TDF_TagSource>(theSource);
  // No occurrence of '&', '<' and other irregular XML characters
  XmlObjMgt::SetStringValue(theTarget.Element(), aTag->Get(), true);
}
