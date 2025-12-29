// Created on: 2017-02-14
// Created by: Sergey NIKONOV
// Copyright (c) 2008-2017 OPEN CASCADE SAS
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
#include <XCAFDoc_Note.hxx>
#include <XmlMXCAFDoc_NoteDriver.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_NoteDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(UserName, "user_name")
IMPLEMENT_DOMSTRING(TimeStamp, "time_stamp")

//=================================================================================================

XmlMXCAFDoc_NoteDriver::XmlMXCAFDoc_NoteDriver(const occ::handle<Message_Messenger>& theMsgDriver,
                                               const char*                           theName)
    : XmlMDF_ADriver(theMsgDriver, theName)
{
}

//=================================================================================================

bool XmlMXCAFDoc_NoteDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                   const occ::handle<TDF_Attribute>& theTarget,
                                   XmlObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  const XmlObjMgt_Element& anElement = theSource;

  XmlObjMgt_DOMString aUserName  = anElement.getAttribute(::UserName());
  XmlObjMgt_DOMString aTimeStamp = anElement.getAttribute(::TimeStamp());
  if (aUserName == nullptr || aTimeStamp == nullptr)
    return false;

  occ::handle<XCAFDoc_Note> aNote = occ::down_cast<XCAFDoc_Note>(theTarget);
  if (aNote.IsNull())
    return false;

  aNote->Set(aUserName.GetString(), aTimeStamp.GetString());

  return true;
}

//=================================================================================================

void XmlMXCAFDoc_NoteDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                   XmlObjMgt_Persistent&             theTarget,
                                   XmlObjMgt_SRelocationTable& /*theRelocTable*/) const
{
  occ::handle<XCAFDoc_Note> aNote = occ::down_cast<XCAFDoc_Note>(theSource);
  if (aNote.IsNull())
    return;

  XmlObjMgt_DOMString aUserName(TCollection_AsciiString(aNote->UserName()).ToCString());
  XmlObjMgt_DOMString aTimeStamp(TCollection_AsciiString(aNote->TimeStamp()).ToCString());

  theTarget.Element().setAttribute(::UserName(), aUserName);
  theTarget.Element().setAttribute(::TimeStamp(), aTimeStamp);
}
