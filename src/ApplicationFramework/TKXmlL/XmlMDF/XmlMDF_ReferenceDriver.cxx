// Created on: 2001-09-04
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
#include <TDF_Reference.hxx>
#include <TDF_Tool.hxx>
#include <XmlMDF_ReferenceDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDF_ReferenceDriver, XmlMDF_ADriver)

//=================================================================================================

XmlMDF_ReferenceDriver::XmlMDF_ReferenceDriver(const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, nullptr)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMDF_ReferenceDriver::NewEmpty() const
{
  return (new TDF_Reference());
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool XmlMDF_ReferenceDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                   const occ::handle<TDF_Attribute>& theTarget,
                                   XmlObjMgt_RRelocationTable&) const
{
  XmlObjMgt_DOMString anXPath = XmlObjMgt::GetStringValue(theSource);

  if (anXPath == nullptr)
  {
    myMessageDriver->Send("Cannot retrieve reference string from element", Message_Fail);
    return false;
  }

  TCollection_AsciiString anEntry;
  if (!XmlObjMgt::GetTagEntryString(anXPath, anEntry))
  {
    TCollection_ExtendedString aMessage =
      TCollection_ExtendedString("Cannot retrieve reference from \"") + anXPath + '\"';
    myMessageDriver->Send(aMessage, Message_Fail);
    return false;
  }

  occ::handle<TDF_Reference> aRef = occ::down_cast<TDF_Reference>(theTarget);

  // find label by entry
  TDF_Label tLab; // Null label.
  if (anEntry.Length() > 0)
  {
    TDF_Tool::Label(aRef->Label().Data(), anEntry, tLab, true);
  }

  // set referenced label
  aRef->Set(tLab);

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//           <label tag='1'>     <This is label entry 0:4:1>
//           ...
//           <label tag='8'>     <This is label entry 0:4:1:8>
//
//           <TDF_Reference id="621"> /document/label/label[@tag="4"]/label[@tag="1"]
//           </TDF_Reference>    <This is reference to label 0:4:1>
//=======================================================================
void XmlMDF_ReferenceDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                   XmlObjMgt_Persistent&             theTarget,
                                   XmlObjMgt_SRelocationTable&) const
{
  occ::handle<TDF_Reference> aRef = occ::down_cast<TDF_Reference>(theSource);
  if (!aRef.IsNull())
  {
    const TDF_Label& lab    = aRef->Label();
    const TDF_Label& refLab = aRef->Get();
    if (!lab.IsNull() && !refLab.IsNull())
    {
      if (lab.IsDescendant(refLab.Root()))
      {
        // Internal reference
        TCollection_AsciiString anEntry;
        TDF_Tool::Entry(refLab, anEntry);

        XmlObjMgt_DOMString aDOMString;
        XmlObjMgt::SetTagEntryString(aDOMString, anEntry);
        // No occurrence of '&', '<' and other irregular XML characters
        XmlObjMgt::SetStringValue(theTarget, aDOMString, true);
      }
    }
  }
}
