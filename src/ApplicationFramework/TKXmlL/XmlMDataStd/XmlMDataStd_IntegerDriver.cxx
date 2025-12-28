// Created on: 2001-08-24
// Created by: Alexnder GRIGORIEV
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
#include <TDataStd_Integer.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_IntegerDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_IntegerDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(AttributeIDString, "intattguid")

//=================================================================================================

XmlMDataStd_IntegerDriver::XmlMDataStd_IntegerDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, NULL)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMDataStd_IntegerDriver::NewEmpty() const
{
  return (new TDataStd_Integer());
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool XmlMDataStd_IntegerDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                      const occ::handle<TDF_Attribute>& theTarget,
                                      XmlObjMgt_RRelocationTable&) const
{
  int                 aValue;
  XmlObjMgt_DOMString anIntStr = XmlObjMgt::GetStringValue(theSource);

  if (anIntStr.GetInteger(aValue) == false)
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve Integer attribute from \"") + anIntStr + "\"";
    myMessageDriver->Send(aMessageString, Message_Warning);
    aValue = 0;
  }

  occ::handle<TDataStd_Integer> anInt = occ::down_cast<TDataStd_Integer>(theTarget);
  anInt->Set(aValue);

  // attribute id
  const XmlObjMgt_Element& anElement = theSource;
  XmlObjMgt_DOMString      aGUIDStr  = anElement.getAttribute(::AttributeIDString());
  if (aGUIDStr.Type() != XmlObjMgt_DOMString::LDOM_NULL)
  {
    const Standard_GUID aGUID(aGUIDStr.GetString()); // user defined case
    occ::down_cast<TDataStd_Integer>(theTarget)->SetID(aGUID);
  }

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_IntegerDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                      XmlObjMgt_Persistent&             theTarget,
                                      XmlObjMgt_SRelocationTable&) const
{
  occ::handle<TDataStd_Integer> anInt = occ::down_cast<TDataStd_Integer>(theSource);
  XmlObjMgt::SetStringValue(theTarget, anInt->Get());
  if (anInt->ID() != TDataStd_Integer::GetID())
  {
    // convert GUID
    char                aGuidStr[Standard_GUID_SIZE_ALLOC];
    Standard_PCharacter pGuidStr = aGuidStr;
    anInt->ID().ToCString(pGuidStr);
    theTarget.Element().setAttribute(::AttributeIDString(), aGuidStr);
  }
}
