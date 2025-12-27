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

#include <XmlMXCAFDoc_LengthUnitDriver.hxx>

#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <XCAFDoc_LengthUnit.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_LengthUnitDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(UnitScaleValue, "value")

//=================================================================================================

XmlMXCAFDoc_LengthUnitDriver::XmlMXCAFDoc_LengthUnitDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, "xcaf", "LengthUnit")
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMXCAFDoc_LengthUnitDriver::NewEmpty() const
{
  return (new XCAFDoc_LengthUnit());
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool XmlMXCAFDoc_LengthUnitDriver::Paste(const XmlObjMgt_Persistent&  theSource,
                                                     const occ::handle<TDF_Attribute>& theTarget,
                                                     XmlObjMgt_RRelocationTable&) const
{
  XmlObjMgt_DOMString aNameStr = XmlObjMgt::GetStringValue(theSource);

  if (aNameStr == NULL)
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve LengthUnit attribute");
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }
  const XmlObjMgt_Element& anElement       = theSource;
  XmlObjMgt_DOMString      aUnitScaleValue = anElement.getAttribute(::UnitScaleValue());
  if (aUnitScaleValue == NULL)
  {
    TCollection_ExtendedString aMessageString("Cannot retrieve LengthUnit scale factor");
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }
  TCollection_AsciiString aScaleFactor(aUnitScaleValue.GetString());
  TCollection_AsciiString anUnitName(aNameStr.GetString());
  if (!aScaleFactor.IsRealValue(true))
  {
    TCollection_ExtendedString aMessageString("Cannot retrieve LengthUnit scale factor");
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  occ::handle<XCAFDoc_LengthUnit> anInt = occ::down_cast<XCAFDoc_LengthUnit>(theTarget);
  anInt->Set(anUnitName, aScaleFactor.RealValue());
  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMXCAFDoc_LengthUnitDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                         XmlObjMgt_Persistent&        theTarget,
                                         XmlObjMgt_SRelocationTable&) const
{
  occ::handle<XCAFDoc_LengthUnit> anAtt     = occ::down_cast<XCAFDoc_LengthUnit>(theSource);
  XmlObjMgt_DOMString        aNameUnit = anAtt->GetUnitName().ToCString();
  XmlObjMgt_DOMString aValueUnit       = TCollection_AsciiString(anAtt->GetUnitValue()).ToCString();
  XmlObjMgt::SetStringValue(theTarget, aNameUnit);
  theTarget.Element().setAttribute(::UnitScaleValue(), aValueUnit);
}
