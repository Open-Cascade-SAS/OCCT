// Created on: 2008-03-05
// Created by: Vlad ROMASHKO
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
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TFunction_Scope.hxx>
#include <XmlMFunction_ScopeDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMFunction_ScopeDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(LastIDIndex, "lastid")
IMPLEMENT_DOMSTRING(LastLabelIndex, "lastlabel")

IMPLEMENT_DOMSTRING(ExtString, "string")

//=================================================================================================

XmlMFunction_ScopeDriver::XmlMFunction_ScopeDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, nullptr)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMFunction_ScopeDriver::NewEmpty() const
{
  return (new TFunction_Scope());
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool XmlMFunction_ScopeDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                     const occ::handle<TDF_Attribute>& theTarget,
                                     XmlObjMgt_RRelocationTable&) const
{
  occ::handle<TFunction_Scope> S = occ::down_cast<TFunction_Scope>(theTarget);
  NCollection_List<int>        IDs;
  NCollection_List<TDF_Label>  Labels;

  int                      aFirstInd, aLastInd, aValue, ind, nbIDs = 0, nbLabels = 0;
  const XmlObjMgt_Element& anElement = theSource;

  // IDs
  // ===

  // Read the FirstIndex; if the attribute is absent initialize to 1
  aFirstInd = 1;

  // Read the LastIndex; the attribute should present
  if (!anElement.getAttribute(::LastIDIndex()).GetInteger(aLastInd))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for Scope attribute");
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }
  nbIDs = aLastInd - aFirstInd + 1;

  if (aFirstInd == aLastInd)
  {
    int anInteger;
    if (!XmlObjMgt::GetStringValue(anElement).GetInteger(anInteger))
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve integer member"
                                   " for Scope attribute as \"");
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }
    IDs.Append(anInteger);
  }
  else
  {
    const char* aValueStr =
      static_cast<const char*>(XmlObjMgt::GetStringValue(anElement).GetString());

    for (ind = aFirstInd; ind <= aLastInd; ind++)
    {
      if (!XmlObjMgt::GetInteger(aValueStr, aValue))
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve integer member"
                                     " for Scope attribute as \"")
          + aValueStr + "\"";
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }
      IDs.Append(aValue);
    }
  }

  // Labels
  // ======

  aFirstInd = 1;

  // Read the LastIndex; the attribute should present
  if (!anElement.getAttribute(::LastLabelIndex()).GetInteger(aLastInd))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for Scope attribute");
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }
  nbLabels = aLastInd - aFirstInd + 1;

  if (!anElement.hasChildNodes())
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve an array of labels");
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  LDOM_Node           aCurNode = anElement.getFirstChild() /*.getNextSibling().getNextSibling()*/;
  LDOM_Element*       aCurElement = (LDOM_Element*)&aCurNode;
  XmlObjMgt_DOMString aValueStr;
  while (*aCurElement != anElement.getLastChild())
  {
    aValueStr = XmlObjMgt::GetStringValue(*aCurElement);
    if (aValueStr == nullptr)
    {
      aCurNode    = aCurElement->getNextSibling();
      aCurElement = (LDOM_Element*)&aCurNode;
      continue;
    }
    TCollection_AsciiString anEntry;
    if (!XmlObjMgt::GetTagEntryString(aValueStr, anEntry))
    {
      TCollection_ExtendedString aMessage =
        TCollection_ExtendedString("Cannot retrieve reference from \"") + aValueStr + '\"';
      myMessageDriver->Send(aMessage, Message_Fail);
      return false;
    }
    // Find label by entry
    TDF_Label tLab; // Null label.
    if (anEntry.Length() > 0)
    {
      TDF_Tool::Label(S->Label().Data(), anEntry, tLab, true);
    }
    Labels.Append(tLab);
    aCurNode    = aCurElement->getNextSibling();
    aCurElement = (LDOM_Element*)&aCurNode;
  }

  // Last reference
  aValueStr = XmlObjMgt::GetStringValue(*aCurElement);
  if (aValueStr == nullptr)
  {
    myMessageDriver->Send("Cannot retrieve reference string from element", Message_Fail);
    return false;
  }
  TCollection_AsciiString anEntry;
  if (!XmlObjMgt::GetTagEntryString(aValueStr, anEntry))
  {
    TCollection_ExtendedString aMessage =
      TCollection_ExtendedString("Cannot retrieve reference from \"") + aValueStr + '\"';
    myMessageDriver->Send(aMessage, Message_Fail);
    return false;
  }
  // Find label by entry
  TDF_Label tLab; // Null label.
  if (anEntry.Length() > 0)
  {
    TDF_Tool::Label(S->Label().Data(), anEntry, tLab, true);
  }
  Labels.Append(tLab);

  // Check equality of lengths of the list of IDs & Labels.
  if (nbIDs != nbLabels)
  {
    TCollection_ExtendedString aMessage =
      TCollection_ExtendedString("Numbers of IDs & Labels are different");
    myMessageDriver->Send(aMessage, Message_Fail);
    return false;
  }

  // Set IDs & Labels into the Scope attribute
  int                                   freeID = 0;
  NCollection_List<int>::Iterator       itri(IDs);
  NCollection_List<TDF_Label>::Iterator itrl(Labels);
  for (; itri.More(); itri.Next(), itrl.Next())
  {
    int ID = itri.Value();
    if (ID > freeID)
      freeID = ID;
    S->ChangeFunctions().Bind(ID, itrl.Value());
  }
  freeID++;
  S->SetFreeID(freeID);

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMFunction_ScopeDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                     XmlObjMgt_Persistent&             theTarget,
                                     XmlObjMgt_SRelocationTable&) const
{
  occ::handle<TFunction_Scope> S = occ::down_cast<TFunction_Scope>(theSource);

  // IDs
  // ===

  theTarget.Element().setAttribute(::LastIDIndex(), S->GetFunctions().Extent());

  TCollection_AsciiString                         aValueStr;
  NCollection_DoubleMap<int, TDF_Label>::Iterator itrd(S->GetFunctions());
  for (; itrd.More(); itrd.Next())
  {
    const int ID = itrd.Key1();
    aValueStr += TCollection_AsciiString(ID);
    aValueStr += ' ';
  }
  aValueStr += "\n";

  XmlObjMgt::SetStringValue(theTarget, aValueStr.ToCString(), true);

  // Labels
  // ======

  XmlObjMgt_Element& anElement = theTarget;
  anElement.setAttribute(::LastLabelIndex(), S->GetFunctions().Extent());

  XmlObjMgt_Document aDoc(anElement.getOwnerDocument());

  for (itrd.Initialize(S->GetFunctions()); itrd.More(); itrd.Next())
  {
    TDF_Label L = itrd.Key2();

    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(L, anEntry);

    XmlObjMgt_DOMString aDOMString;
    XmlObjMgt::SetTagEntryString(aDOMString, anEntry);
    XmlObjMgt_Element aCurTarget = aDoc.createElement(::ExtString());
    XmlObjMgt::SetStringValue(aCurTarget, aDOMString, true);
    anElement.appendChild(aCurTarget);
  }
}
