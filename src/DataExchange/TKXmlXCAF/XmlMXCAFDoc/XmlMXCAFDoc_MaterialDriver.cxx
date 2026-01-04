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

#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <XCAFDoc_Material.hxx>
#include <XmlMXCAFDoc_MaterialDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_MaterialDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(NameIndexString, "name")
IMPLEMENT_DOMSTRING(DescrIndexString, "descr")
IMPLEMENT_DOMSTRING(DensNameIndexString, "dens_name")
IMPLEMENT_DOMSTRING(DensTypeIndexString, "dens_type")

//=================================================================================================

XmlMXCAFDoc_MaterialDriver::XmlMXCAFDoc_MaterialDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, "xcaf", "Material")
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMXCAFDoc_MaterialDriver::NewEmpty() const
{
  return (new XCAFDoc_Material());
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool XmlMXCAFDoc_MaterialDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                       const occ::handle<TDF_Attribute>& theTarget,
                                       XmlObjMgt_RRelocationTable&) const
{
  double              aDensity;
  XmlObjMgt_DOMString aRealStr = XmlObjMgt::GetStringValue(theSource);

  if (!XmlObjMgt::GetReal(aRealStr, aDensity))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve Material attribute density from \"") + aRealStr
      + "\"";
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  const XmlObjMgt_Element& anElement    = theSource;
  XmlObjMgt_DOMString      aNameStr     = anElement.getAttribute(::NameIndexString());
  XmlObjMgt_DOMString      aDescrStr    = anElement.getAttribute(::DescrIndexString());
  XmlObjMgt_DOMString      aDensNameStr = anElement.getAttribute(::DensNameIndexString());
  XmlObjMgt_DOMString      aDensTypeStr = anElement.getAttribute(::DensTypeIndexString());
  if (aNameStr == nullptr || aDescrStr == nullptr || aDensNameStr == nullptr
      || aDensTypeStr == nullptr)
  {
    TCollection_ExtendedString aMessageString(
      "Cannot retrieve Material attribute name or description");
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString(aNameStr.GetString());
  occ::handle<TCollection_HAsciiString> aDescr =
    new TCollection_HAsciiString(aDescrStr.GetString());
  occ::handle<TCollection_HAsciiString> aDensName =
    new TCollection_HAsciiString(aDensNameStr.GetString());
  occ::handle<TCollection_HAsciiString> aDensType =
    new TCollection_HAsciiString(aDensTypeStr.GetString());

  occ::handle<XCAFDoc_Material> anAtt = occ::down_cast<XCAFDoc_Material>(theTarget);
  anAtt->Set(aName, aDescr, aDensity, aDensName, aDensType);

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMXCAFDoc_MaterialDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                       XmlObjMgt_Persistent&             theTarget,
                                       XmlObjMgt_SRelocationTable&) const
{
  occ::handle<XCAFDoc_Material> anAtt = occ::down_cast<XCAFDoc_Material>(theSource);

  XmlObjMgt_DOMString aNameString, aDescrString, aDensNameStr, aDensTypeStr;
  if (!anAtt->GetName().IsNull())
    aNameString = anAtt->GetName()->String().ToCString();
  if (!anAtt->GetDescription().IsNull())
    aDescrString = anAtt->GetDescription()->String().ToCString();
  if (!anAtt->GetDensName().IsNull())
    aDensNameStr = anAtt->GetDensName()->String().ToCString();
  if (!anAtt->GetDensValType().IsNull())
    aDensTypeStr = anAtt->GetDensValType()->String().ToCString();

  TCollection_AsciiString aDensityStr(anAtt->GetDensity());
  XmlObjMgt::SetStringValue(theTarget, aDensityStr.ToCString());
  theTarget.Element().setAttribute(::NameIndexString(), aNameString);
  theTarget.Element().setAttribute(::DescrIndexString(), aDescrString);
  theTarget.Element().setAttribute(::DensNameIndexString(), aDensNameStr);
  theTarget.Element().setAttribute(::DensTypeIndexString(), aDensTypeStr);
}
