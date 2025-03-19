// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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
#include <TDataStd_ReferenceList.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_Tool.hxx>
#include <XmlMDataStd_ReferenceListDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_ReferenceListDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(FirstIndexString, "first")
IMPLEMENT_DOMSTRING(LastIndexString, "last")
IMPLEMENT_DOMSTRING(ExtString, "string")
IMPLEMENT_DOMSTRING(AttributeIDString, "reflistattguid")

//=================================================================================================

XmlMDataStd_ReferenceListDriver::XmlMDataStd_ReferenceListDriver(
  const Handle(Message_Messenger)& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, NULL)
{
}

//=================================================================================================

Handle(TDF_Attribute) XmlMDataStd_ReferenceListDriver::NewEmpty() const
{
  return new TDataStd_ReferenceList();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_ReferenceListDriver::Paste(const XmlObjMgt_Persistent&  theSource,
                                                        const Handle(TDF_Attribute)& theTarget,
                                                        XmlObjMgt_RRelocationTable&) const
{
  Standard_Integer         aFirstInd, aLastInd;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex = anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for ReferenceList attribute as \"")
      + aFirstIndex + "\"";
    myMessageDriver->Send(aMessageString, Message_Fail);
    return Standard_False;
  }

  // Read the LastIndex; the attribute should present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for ReferenceList attribute as \"")
      + aFirstIndex + "\"";
    myMessageDriver->Send(aMessageString, Message_Fail);
    return Standard_False;
  }

  const Handle(TDataStd_ReferenceList) aReferenceList =
    Handle(TDataStd_ReferenceList)::DownCast(theTarget);
  // attribute id
  Standard_GUID       aGUID;
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::AttributeIDString());
  if (aGUIDStr.Type() == XmlObjMgt_DOMString::LDOM_NULL)
    aGUID = TDataStd_ReferenceList::GetID(); // default case
  else
    aGUID = Standard_GUID(Standard_CString(aGUIDStr.GetString())); // user defined case
  aReferenceList->SetID(aGUID);

  if (aLastInd > 0)
  {
    if (!anElement.hasChildNodes())
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve a list of reference");
      myMessageDriver->Send(aMessageString, Message_Fail);
      return Standard_False;
    }

    LDOM_Node           aCurNode    = anElement.getFirstChild();
    LDOM_Element*       aCurElement = (LDOM_Element*)&aCurNode;
    XmlObjMgt_DOMString aValueStr;
    while (*aCurElement != anElement.getLastChild())
    {
      aValueStr = XmlObjMgt::GetStringValue(*aCurElement);
      if (aValueStr == NULL)
      {
        myMessageDriver->Send("Cannot retrieve reference string from element", Message_Fail);
        return Standard_False;
      }
      TCollection_AsciiString anEntry;
      if (XmlObjMgt::GetTagEntryString(aValueStr, anEntry) == Standard_False)
      {
        TCollection_ExtendedString aMessage =
          TCollection_ExtendedString("Cannot retrieve reference from \"") + aValueStr + '\"';
        myMessageDriver->Send(aMessage, Message_Fail);
        return Standard_False;
      }
      // Find label by entry
      TDF_Label tLab; // Null label.
      if (anEntry.Length() > 0)
        TDF_Tool::Label(aReferenceList->Label().Data(), anEntry, tLab, Standard_True);

      aReferenceList->Append(tLab);
      aCurNode    = aCurElement->getNextSibling();
      aCurElement = (LDOM_Element*)&aCurNode;
    }

    // Last reference
    aValueStr = XmlObjMgt::GetStringValue(*aCurElement);
    if (aValueStr == NULL)
    {
      myMessageDriver->Send("Cannot retrieve reference string from element", Message_Fail);
      return Standard_False;
    }
    TCollection_AsciiString anEntry;
    if (XmlObjMgt::GetTagEntryString(aValueStr, anEntry) == Standard_False)
    {
      TCollection_ExtendedString aMessage =
        TCollection_ExtendedString("Cannot retrieve reference from \"") + aValueStr + '\"';
      myMessageDriver->Send(aMessage, Message_Fail);
      return Standard_False;
    }
    // Find label by entry
    TDF_Label tLab; // Null label.
    if (anEntry.Length() > 0)
    {
      TDF_Tool::Label(aReferenceList->Label().Data(), anEntry, tLab, Standard_True);
    }
    aReferenceList->Append(tLab);
  }

  return Standard_True;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_ReferenceListDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                            XmlObjMgt_Persistent&        theTarget,
                                            XmlObjMgt_SRelocationTable&) const
{
  const Handle(TDataStd_ReferenceList) aReferenceList =
    Handle(TDataStd_ReferenceList)::DownCast(theSource);
  TDF_Label L = aReferenceList->Label();
  if (L.IsNull())
  {
    myMessageDriver->Send("Label of a ReferenceList is Null.", Message_Fail);
    return;
  }

  Standard_Integer   anU       = aReferenceList->Extent();
  XmlObjMgt_Element& anElement = theTarget;
  anElement.setAttribute(::LastIndexString(), anU);
  if (anU == 0)
    return;
  XmlObjMgt_Document aDoc(anElement.getOwnerDocument());

  TDF_ListIteratorOfLabelList itr(aReferenceList->List());
  for (; itr.More(); itr.Next())
  {
    if (L.IsDescendant(itr.Value().Root()))
    {
      // Internal reference
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(itr.Value(), anEntry);

      XmlObjMgt_DOMString aDOMString;
      XmlObjMgt::SetTagEntryString(aDOMString, anEntry);
      XmlObjMgt_Element aCurTarget = aDoc.createElement(::ExtString());
      XmlObjMgt::SetStringValue(aCurTarget, aDOMString, Standard_True);
      anElement.appendChild(aCurTarget);
    }
  }

  if (aReferenceList->ID() != TDataStd_ReferenceList::GetID())
  {
    // convert GUID
    Standard_Character  aGuidStr[Standard_GUID_SIZE_ALLOC];
    Standard_PCharacter pGuidStr = aGuidStr;
    aReferenceList->ID().ToCString(pGuidStr);
    theTarget.Element().setAttribute(::AttributeIDString(), aGuidStr);
  }
}
