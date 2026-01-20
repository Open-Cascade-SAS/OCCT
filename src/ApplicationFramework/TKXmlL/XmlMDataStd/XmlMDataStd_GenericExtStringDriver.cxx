// Copyright (c) 2020 OPEN CASCADE SAS
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
#include <TDataStd_GenericExtString.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_GenericExtStringDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_GenericExtStringDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(AttributeIDString, "nameguid")

//=================================================================================================

XmlMDataStd_GenericExtStringDriver::XmlMDataStd_GenericExtStringDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, NULL)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMDataStd_GenericExtStringDriver::NewEmpty() const
{
  return occ::handle<TDF_Attribute>(); // this attribute can not be created
}

//=================================================================================================

occ::handle<Standard_Type> XmlMDataStd_GenericExtStringDriver::SourceType() const
{
  return Standard_Type::Instance<TDataStd_GenericExtString>();
}

//=================================================================================================

bool XmlMDataStd_GenericExtStringDriver::Paste(const XmlObjMgt_Persistent&  theSource,
                                                           const occ::handle<TDF_Attribute>& theTarget,
                                                           XmlObjMgt_RRelocationTable&) const
{
  if (!theTarget.IsNull())
  {
    TCollection_ExtendedString aString;
    if (XmlObjMgt::GetExtendedString(theSource, aString))
    {
      occ::down_cast<TDataStd_GenericExtString>(theTarget)->Set(aString);
      const XmlObjMgt_Element& anElement = theSource;
      XmlObjMgt_DOMString      aGUIDStr  = anElement.getAttribute(::AttributeIDString());
      if (aGUIDStr.Type() != XmlObjMgt_DOMString::LDOM_NULL)
      { // user defined GUID case
        Standard_GUID aGUID = Standard_GUID(static_cast<const char*>(aGUIDStr.GetString()));
        occ::down_cast<TDataStd_GenericExtString>(theTarget)->SetID(aGUID);
      }
      return true;
    }
  }
  myMessageDriver->Send("error retrieving ExtendedString for type TDataStd_GenericExtString",
                        Message_Fail);
  return false;
}

//=================================================================================================

void XmlMDataStd_GenericExtStringDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                               XmlObjMgt_Persistent&        theTarget,
                                               XmlObjMgt_SRelocationTable&) const
{
  occ::handle<TDataStd_GenericExtString> aStr = occ::down_cast<TDataStd_GenericExtString>(theSource);
  if (aStr.IsNull())
    return;
  XmlObjMgt::SetExtendedString(theTarget, aStr->Get());
  // convert GUID
  char  aGuidStr[Standard_GUID_SIZE_ALLOC];
  Standard_PCharacter pGuidStr = aGuidStr;
  aStr->ID().ToCString(pGuidStr);
  theTarget.Element().setAttribute(::AttributeIDString(), aGuidStr);
}
