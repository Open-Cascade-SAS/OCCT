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
#include <XCAFDoc_DimTol.hxx>
#include <XmlMXCAFDoc_DimTolDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_DimTolDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(NameIndexString, "name")
IMPLEMENT_DOMSTRING(DescrIndexString, "descr")
IMPLEMENT_DOMSTRING(FirstIndexString, "first")
IMPLEMENT_DOMSTRING(LastIndexString, "last")
IMPLEMENT_DOMSTRING(ValueIndexString, "values")

//=================================================================================================

XmlMXCAFDoc_DimTolDriver::XmlMXCAFDoc_DimTolDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, "xcaf", "DimTol")
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMXCAFDoc_DimTolDriver::NewEmpty() const
{
  return (new XCAFDoc_DimTol());
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool XmlMXCAFDoc_DimTolDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                     const occ::handle<TDF_Attribute>& theTarget,
                                     XmlObjMgt_RRelocationTable&) const
{
  int                 aKind;
  XmlObjMgt_DOMString anIntStr = XmlObjMgt::GetStringValue(theSource);

  if (anIntStr.GetInteger(aKind) == false)
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve DimTol attribute kind from \"") + anIntStr + "\"";
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  const XmlObjMgt_Element& anElement = theSource;
  XmlObjMgt_DOMString      aNameStr  = anElement.getAttribute(::NameIndexString());
  XmlObjMgt_DOMString      aDescrStr = anElement.getAttribute(::DescrIndexString());
  if (aNameStr == NULL || aDescrStr == NULL)
  {
    TCollection_ExtendedString aMessageString(
      "Cannot retrieve DimTol attribute name or description");
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString(aNameStr.GetString());
  occ::handle<TCollection_HAsciiString> aDescr =
    new TCollection_HAsciiString(aDescrStr.GetString());

  int                 aFirstInd, aLastInd;
  XmlObjMgt_DOMString aFirstIndex = anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd))
  {
    TCollection_ExtendedString aMessageString(
      "Cannot retrieve the DimTol first index for real array ");
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }
  XmlObjMgt_DOMString aLastIndex = anElement.getAttribute(::LastIndexString());
  if (aLastIndex == NULL)
    aLastInd = 0;
  else if (!aLastIndex.GetInteger(aLastInd))
  {
    TCollection_ExtendedString aMessageString(
      "Cannot retrieve the DimTol last index for real array ");
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  const int                                aLength = aLastInd - aFirstInd + 1;
  occ::handle<NCollection_HArray1<double>> aHArr;
  if (aLength > 0)
  {
    // read real array
    double                     aValue  = 0.;
    const XmlObjMgt_DOMString& aString = anElement.getAttribute(::ValueIndexString());
    aHArr                              = new NCollection_HArray1<double>(aFirstInd, aLastInd);
    const char* aValueStr              = static_cast<const char*>(aString.GetString());
    for (int ind = aFirstInd; ind <= aLastInd; ind++)
    {
      if (!XmlObjMgt::GetReal(aValueStr, aValue))
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve real member"
                                     " for real array \"")
          + aValueStr + "\"";
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }
      aHArr->SetValue(ind, aValue);
    }
  }

  occ::handle<XCAFDoc_DimTol> anAtt = occ::down_cast<XCAFDoc_DimTol>(theTarget);
  anAtt->Set(aKind, aHArr, aName, aDescr);

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMXCAFDoc_DimTolDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                     XmlObjMgt_Persistent&             theTarget,
                                     XmlObjMgt_SRelocationTable&) const
{
  occ::handle<XCAFDoc_DimTol> anAtt = occ::down_cast<XCAFDoc_DimTol>(theSource);

  XmlObjMgt_DOMString aNameString, aDescrString;
  if (!anAtt->GetName().IsNull())
    aNameString = anAtt->GetName()->String().ToCString();
  if (!anAtt->GetDescription().IsNull())
    aDescrString = anAtt->GetDescription()->String().ToCString();

  XmlObjMgt::SetStringValue(theTarget, anAtt->GetKind());
  theTarget.Element().setAttribute(::NameIndexString(), aNameString);
  theTarget.Element().setAttribute(::DescrIndexString(), aDescrString);

  occ::handle<NCollection_HArray1<double>> aHArr     = anAtt->GetVal();
  int                                      aFirstInd = 1, aLastInd = 0;
  if (!aHArr.IsNull())
  {
    aFirstInd = aHArr->Lower();
    aLastInd  = aHArr->Upper();
  }
  theTarget.Element().setAttribute(::FirstIndexString(), aFirstInd);
  theTarget.Element().setAttribute(::LastIndexString(), aLastInd);
  if (aLastInd >= aFirstInd)
  {
    TCollection_AsciiString aValueStr;
    for (int i = aFirstInd; i <= aLastInd; i++)
    {
      char aValueChar[256];
      Sprintf(aValueChar, "%.15g", aHArr->Value(i));
      aValueStr += aValueChar;
      if (i < aLastInd)
        aValueStr += ' ';
    }
    theTarget.Element().setAttribute(::ValueIndexString(), aValueStr.ToCString());
  }
}
