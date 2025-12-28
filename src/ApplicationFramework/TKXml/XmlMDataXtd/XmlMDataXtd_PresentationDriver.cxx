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

#include <XmlMDataXtd_PresentationDriver.hxx>

#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <TDataXtd_Presentation.hxx>
#include <Quantity_Color.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataXtd_PresentationDriver, XmlMDF_ADriver)

IMPLEMENT_DOMSTRING(GuidString, "guid")
IMPLEMENT_DOMSTRING(IsDisplayedString, "isdisplayed")
IMPLEMENT_DOMSTRING(ColorString, "color")
IMPLEMENT_DOMSTRING(MaterialString, "material")
IMPLEMENT_DOMSTRING(TransparencyString, "transparency")
IMPLEMENT_DOMSTRING(WidthString, "width")
IMPLEMENT_DOMSTRING(ModeString, "mode")

IMPLEMENT_DOMSTRING(DisplayedString, "true")

//=================================================================================================

XmlMDataXtd_PresentationDriver::XmlMDataXtd_PresentationDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, nullptr)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMDataXtd_PresentationDriver::NewEmpty() const
{
  return (new TDataXtd_Presentation());
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool XmlMDataXtd_PresentationDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                           const occ::handle<TDF_Attribute>& theTarget,
                                           XmlObjMgt_RRelocationTable&) const
{
  TCollection_ExtendedString aMessageString;
  XmlObjMgt_DOMString        aDOMStr;

  occ::handle<TDataXtd_Presentation> aTPrs  = occ::down_cast<TDataXtd_Presentation>(theTarget);
  const XmlObjMgt_Element&           anElem = theSource;

  // convert attribute value into GUID
  aDOMStr = anElem.getAttribute(::GuidString());
  if (aDOMStr == nullptr)
  {
    myMessageDriver->Send("Cannot retrieve guid string from attribute", Message_Fail);
    return false;
  }
  const char* aGuidStr = (const char*)aDOMStr.GetString();
  aTPrs->SetDriverGUID(aGuidStr);

  // is displayed
  aDOMStr = anElem.getAttribute(::IsDisplayedString());
  aTPrs->SetDisplayed(aDOMStr != nullptr);

  int anIValue;

  // color
  aDOMStr = anElem.getAttribute(::ColorString());
  if (aDOMStr != nullptr)
  {
    if (!aDOMStr.GetInteger(anIValue))
    {
      aMessageString =
        TCollection_ExtendedString("Cannot retrieve Integer value from \"") + aDOMStr + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }

    const Quantity_NameOfColor aNameOfColor =
      TDataXtd_Presentation::getColorNameFromOldEnum(anIValue);
    aTPrs->SetColor(aNameOfColor);
  }
  else
  {
    aTPrs->UnsetColor();
  }

  // material
  aDOMStr = anElem.getAttribute(::MaterialString());
  if (aDOMStr != nullptr)
  {
    if (!aDOMStr.GetInteger(anIValue))
    {
      aMessageString =
        TCollection_ExtendedString("Cannot retrieve Integer value from \"") + aDOMStr + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }
    aTPrs->SetMaterialIndex(anIValue);
  }
  else
  {
    aTPrs->UnsetMaterial();
  }

  double aValue;

  // transparency
  aDOMStr = anElem.getAttribute(::TransparencyString());
  if (aDOMStr != nullptr)
  {
    if (!XmlObjMgt::GetReal(aDOMStr, aValue))
    {
      aMessageString =
        TCollection_ExtendedString("Cannot retrieve Real value from \"") + aDOMStr + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }
    aTPrs->SetTransparency(aValue);
  }
  else
  {
    aTPrs->UnsetTransparency();
  }

  // width
  aDOMStr = anElem.getAttribute(::WidthString());
  if (aDOMStr != nullptr)
  {
    if (!XmlObjMgt::GetReal(aDOMStr, aValue))
    {
      aMessageString =
        TCollection_ExtendedString("Cannot retrieve Real value from \"") + aDOMStr + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }
    aTPrs->SetWidth(aValue);
  }
  else
  {
    aTPrs->UnsetWidth();
  }

  // mode
  aDOMStr = anElem.getAttribute(::ModeString());
  if (aDOMStr != nullptr)
  {
    if (!aDOMStr.GetInteger(anIValue))
    {
      aMessageString =
        TCollection_ExtendedString("Cannot retrieve Integer value from \"") + aDOMStr + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }
    aTPrs->SetMode(anIValue);
  }
  else
  {
    aTPrs->UnsetMode();
  }

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataXtd_PresentationDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                           XmlObjMgt_Persistent&             theTarget,
                                           XmlObjMgt_SRelocationTable&) const
{
  occ::handle<TDataXtd_Presentation> aTPrs = occ::down_cast<TDataXtd_Presentation>(theSource);
  if (aTPrs.IsNull())
    return;

  // convert GUID into attribute value
  char                aGuidStr[40];
  Standard_PCharacter pGuidStr;
  pGuidStr = aGuidStr;
  aTPrs->GetDriverGUID().ToCString(pGuidStr);
  theTarget.Element().setAttribute(::GuidString(), aGuidStr);

  // is displayed
  if (aTPrs->IsDisplayed())
    theTarget.Element().setAttribute(::IsDisplayedString(), ::DisplayedString());

  int aNb;

  // color
  if (aTPrs->HasOwnColor())
  {
    aNb = TDataXtd_Presentation::getOldColorNameFromNewEnum(aTPrs->Color());
    theTarget.Element().setAttribute(::ColorString(), aNb);
  }

  // material
  if (aTPrs->HasOwnMaterial())
  {
    aNb = aTPrs->MaterialIndex();
    theTarget.Element().setAttribute(::MaterialString(), aNb);
  }

  // transparency
  if (aTPrs->HasOwnTransparency())
  {
    TCollection_AsciiString aRNbStr(aTPrs->Transparency());
    theTarget.Element().setAttribute(::TransparencyString(), aRNbStr.ToCString());
  }

  // width
  if (aTPrs->HasOwnWidth())
  {
    TCollection_AsciiString aRNbStr(aTPrs->Width());
    theTarget.Element().setAttribute(::WidthString(), aRNbStr.ToCString());
  }

  // mode
  if (aTPrs->HasOwnMode())
  {
    aNb = aTPrs->Mode();
    theTarget.Element().setAttribute(::ModeString(), aNb);
  }
}
