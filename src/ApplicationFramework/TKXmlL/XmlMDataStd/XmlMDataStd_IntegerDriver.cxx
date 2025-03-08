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

XmlMDataStd_IntegerDriver::XmlMDataStd_IntegerDriver(const Handle(Message_Messenger)& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, NULL)
{
}

//=================================================================================================

Handle(TDF_Attribute) XmlMDataStd_IntegerDriver::NewEmpty() const
{
  return (new TDataStd_Integer());
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_IntegerDriver::Paste(const XmlObjMgt_Persistent&  theSource,
                                                  const Handle(TDF_Attribute)& theTarget,
                                                  XmlObjMgt_RRelocationTable&) const
{
  Standard_Integer    aValue;
  XmlObjMgt_DOMString anIntStr = XmlObjMgt::GetStringValue(theSource);

  if (anIntStr.GetInteger(aValue) == Standard_False)
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve Integer attribute from \"") + anIntStr + "\"";
    myMessageDriver->Send(aMessageString, Message_Warning);
    aValue = 0;
  }

  Handle(TDataStd_Integer) anInt = Handle(TDataStd_Integer)::DownCast(theTarget);
  anInt->Set(aValue);

  // attribute id
  const XmlObjMgt_Element& anElement = theSource;
  XmlObjMgt_DOMString      aGUIDStr  = anElement.getAttribute(::AttributeIDString());
  if (aGUIDStr.Type() != XmlObjMgt_DOMString::LDOM_NULL)
  {
    const Standard_GUID aGUID(aGUIDStr.GetString()); // user defined case
    Handle(TDataStd_Integer)::DownCast(theTarget)->SetID(aGUID);
  }

  return Standard_True;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_IntegerDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                      XmlObjMgt_Persistent&        theTarget,
                                      XmlObjMgt_SRelocationTable&) const
{
  Handle(TDataStd_Integer) anInt = Handle(TDataStd_Integer)::DownCast(theSource);
  XmlObjMgt::SetStringValue(theTarget, anInt->Get());
  if (anInt->ID() != TDataStd_Integer::GetID())
  {
    // convert GUID
    Standard_Character  aGuidStr[Standard_GUID_SIZE_ALLOC];
    Standard_PCharacter pGuidStr = aGuidStr;
    anInt->ID().ToCString(pGuidStr);
    theTarget.Element().setAttribute(::AttributeIDString(), aGuidStr);
  }
}
