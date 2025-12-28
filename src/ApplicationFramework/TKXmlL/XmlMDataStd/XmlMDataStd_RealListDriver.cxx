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
#include <NCollection_LocalArray.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_RealList.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_RealListDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_RealListDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(FirstIndexString, "first")
IMPLEMENT_DOMSTRING(LastIndexString, "last")
IMPLEMENT_DOMSTRING(AttributeIDString, "reallistattguid")

//=================================================================================================

XmlMDataStd_RealListDriver::XmlMDataStd_RealListDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, NULL)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMDataStd_RealListDriver::NewEmpty() const
{
  return new TDataStd_RealList();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool XmlMDataStd_RealListDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                       const occ::handle<TDF_Attribute>& theTarget,
                                       XmlObjMgt_RRelocationTable&) const
{
  const occ::handle<TDataStd_RealList> aRealList = occ::down_cast<TDataStd_RealList>(theTarget);
  const XmlObjMgt_Element&             anElement = theSource;

  // attribute id
  Standard_GUID       aGUID;
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::AttributeIDString());
  if (aGUIDStr.Type() == XmlObjMgt_DOMString::LDOM_NULL)
    aGUID = TDataStd_RealList::GetID(); // default case
  else
    aGUID = Standard_GUID(static_cast<const char*>(aGUIDStr.GetString())); // user defined case
  aRealList->SetID(aGUID);

  // Read the FirstIndex; if the attribute is absent initialize to 1
  int                 aFirstInd, aLastInd, ind;
  XmlObjMgt_DOMString aFirstIndex = anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for RealList attribute as \"")
      + aFirstIndex + "\"";
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  // Read the LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for RealList attribute as \"")
      + aFirstIndex + "\"";
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  // Check the type of LDOMString
  const XmlObjMgt_DOMString& aString = XmlObjMgt::GetStringValue(anElement);
  if (aLastInd == 0)
    aFirstInd = 0;
  if (aString.Type() == LDOMBasicString::LDOM_Integer)
  {
    if (aFirstInd == aLastInd && aLastInd > 0)
    {
      int anIntValue;
      if (aString.GetInteger(anIntValue))
        aRealList->Append(double(anIntValue));
    }
    else
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve array of real members"
                                   " for RealList attribute from Integer \"")
        + aString + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }
  }
  else if (aLastInd >= 1)
  {
    const char* aValueStr = static_cast<const char*>(aString.GetString());
    for (ind = aFirstInd; ind <= aLastInd; ind++)
    {
      double aValue;
      if (!XmlObjMgt::GetReal(aValueStr, aValue))
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve real member"
                                     " for RealList attribute as \"")
          + aValueStr + "\"";
        myMessageDriver->Send(aMessageString, Message_Warning);
        // skip the first space, if exists
        while (*aValueStr != 0 && IsSpace(*aValueStr))
          ++aValueStr;
        // skip to the next space separator
        while (*aValueStr != 0 && !IsSpace(*aValueStr))
          ++aValueStr;
      }
      aRealList->Append(aValue);
    }
  }

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_RealListDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                       XmlObjMgt_Persistent&             theTarget,
                                       XmlObjMgt_SRelocationTable&) const
{
  const occ::handle<TDataStd_RealList> aRealList = occ::down_cast<TDataStd_RealList>(theSource);

  int anU = aRealList->Extent();
  theTarget.Element().setAttribute(::LastIndexString(), anU);
  // Allocation of 25 chars for each double value including the space:
  // An example: -3.1512678732195273e+020
  NCollection_LocalArray<char> str(25 * anU + 1);
  if (anU == 0)
    str[0] = 0;
  else if (anU >= 1)
  {
    int                                iChar = 0;
    NCollection_List<double>::Iterator itr(aRealList->List());
    for (; itr.More(); itr.Next())
    {
      const double& realValue = itr.Value();
      iChar += Sprintf(&(str[iChar]), "%.17g ", realValue);
    }
  }
  XmlObjMgt::SetStringValue(theTarget, (char*)str, true);

  if (aRealList->ID() != TDataStd_RealList::GetID())
  {
    // convert GUID
    char                aGuidStr[Standard_GUID_SIZE_ALLOC];
    Standard_PCharacter pGuidStr = aGuidStr;
    aRealList->ID().ToCString(pGuidStr);
    theTarget.Element().setAttribute(::AttributeIDString(), aGuidStr);
  }
}
