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

//=======================================================================
//function : XmlMXCAFDoc_LengthUnitDriver
//purpose  : Constructor
//=======================================================================
XmlMXCAFDoc_LengthUnitDriver::XmlMXCAFDoc_LengthUnitDriver
(const Handle(Message_Messenger)& theMsgDriver)
  : XmlMDF_ADriver(theMsgDriver, "xcaf", "LengthUnit")
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_LengthUnitDriver::NewEmpty() const
{
  return (new XCAFDoc_LengthUnit());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMXCAFDoc_LengthUnitDriver::Paste(const XmlObjMgt_Persistent& theSource,
                                                     const Handle(TDF_Attribute)& theTarget,
                                                     XmlObjMgt_RRelocationTable&) const
{
  XmlObjMgt_DOMString aNameStr = XmlObjMgt::GetStringValue(theSource);

  if (aNameStr == NULL)
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve LengthUnit attribute");
    myMessageDriver->Send(aMessageString, Message_Fail);
    return Standard_False;
  }
  const XmlObjMgt_Element& anElement = theSource;
  XmlObjMgt_DOMString aUnitScaleValue = anElement.getAttribute(::UnitScaleValue());
  if (aUnitScaleValue == NULL)
  {
    TCollection_ExtendedString aMessageString
    ("Cannot retrieve LengthUnit scale factor");
    myMessageDriver->Send(aMessageString, Message_Fail);
    return Standard_False;
  }
  TCollection_AsciiString aScaleFactor(aUnitScaleValue.GetString());
  TCollection_AsciiString anUnitName(aNameStr.GetString());
  if (!aScaleFactor.IsRealValue(true))
  {
    TCollection_ExtendedString aMessageString
    ("Cannot retrieve LengthUnit scale factor");
    myMessageDriver->Send(aMessageString, Message_Fail);
    return Standard_False;
  }

  Handle(XCAFDoc_LengthUnit) anInt = Handle(XCAFDoc_LengthUnit)::DownCast(theTarget);
  anInt->Set(anUnitName, aScaleFactor.RealValue());
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMXCAFDoc_LengthUnitDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                          XmlObjMgt_Persistent&        theTarget,
                                          XmlObjMgt_SRelocationTable&  ) const
{
  Handle(XCAFDoc_LengthUnit) anAtt = Handle(XCAFDoc_LengthUnit)::DownCast(theSource);
  XmlObjMgt_DOMString aNameUnit = anAtt->GetUnitName().ToCString(); 
  XmlObjMgt_DOMString aValueUnit = TCollection_AsciiString(anAtt->GetUnitValue()).ToCString();
  XmlObjMgt::SetStringValue (theTarget, aNameUnit);
  theTarget.Element().setAttribute(::UnitScaleValue(), aValueUnit);
}
