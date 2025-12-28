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

// AGV 150202: Changed prototype XmlObjMgt::SetStringValue()

#include <Message_Messenger.hxx>
#include <NCollection_LocalArray.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDocStd_FormatVersion.hxx>
#include <XmlMDataStd.hxx>
#include <XmlMDataStd_RealArrayDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_RealArrayDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(FirstIndexString, "first")
IMPLEMENT_DOMSTRING(LastIndexString, "last")
IMPLEMENT_DOMSTRING(IsDeltaOn, "delta")
IMPLEMENT_DOMSTRING(AttributeIDString, "realarrattguid")

//=================================================================================================

XmlMDataStd_RealArrayDriver::XmlMDataStd_RealArrayDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, NULL)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMDataStd_RealArrayDriver::NewEmpty() const
{
  return (new TDataStd_RealArray());
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool XmlMDataStd_RealArrayDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                        const occ::handle<TDF_Attribute>& theTarget,
                                        XmlObjMgt_RRelocationTable&       theRelocTable) const
{

  occ::handle<TDataStd_RealArray> aRealArray = occ::down_cast<TDataStd_RealArray>(theTarget);
  const XmlObjMgt_Element&        anElement  = theSource;

  // attribute id
  Standard_GUID       aGUID;
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::AttributeIDString());
  if (aGUIDStr.Type() == XmlObjMgt_DOMString::LDOM_NULL)
    aGUID = TDataStd_RealArray::GetID(); // default case
  else
    aGUID = Standard_GUID(static_cast<const char*>(aGUIDStr.GetString())); // user defined case
  aRealArray->SetID(aGUID);

  int aFirstInd, aLastInd, ind;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex = anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for RealArray attribute as \"")
      + aFirstIndex + "\"";
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  // Read LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for RealArray attribute as \"")
      + aFirstIndex + "\"";
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  aRealArray->Init(aFirstInd, aLastInd);

  // Check the type of LDOMString
  const XmlObjMgt_DOMString& aString = XmlObjMgt::GetStringValue(anElement);
  if (aString.Type() == LDOMBasicString::LDOM_Integer)
  {
    if (aFirstInd == aLastInd)
    {
      int anIntValue;
      if (aString.GetInteger(anIntValue))
        aRealArray->SetValue(aFirstInd, double(anIntValue));
    }
    else
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve array of real members"
                                   " for RealArray attribute from Integer \"")
        + aString + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }
  }
  else
  {
    double      aValue;
    const char* aValueStr = static_cast<const char*>(aString.GetString());
    for (ind = aFirstInd; ind <= aLastInd; ind++)
    {
      if (!XmlObjMgt::GetReal(aValueStr, aValue))
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve real member"
                                     " for RealArray attribute as \"")
          + aValueStr + "\"";
        myMessageDriver->Send(aMessageString, Message_Warning);
        // skip the first space, if exists
        while (*aValueStr != 0 && IsSpace(*aValueStr))
          ++aValueStr;
        // skip to the next space separator
        while (*aValueStr != 0 && !IsSpace(*aValueStr))
          ++aValueStr;
      }
      aRealArray->SetValue(ind, aValue);
    }
  }
  bool aDelta(false);

  if (theRelocTable.GetHeaderData()->StorageVersion().IntegerValue()
      >= TDocStd_FormatVersion_VERSION_3)
  {
    int aDeltaValue;
    if (!anElement.getAttribute(::IsDeltaOn()).GetInteger(aDeltaValue))
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve the isDelta value"
                                   " for RealArray attribute as \"")
        + aDeltaValue + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }
    else
      aDelta = aDeltaValue != 0;
  }
  aRealArray->SetDelta(aDelta);

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_RealArrayDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                        XmlObjMgt_Persistent&             theTarget,
                                        XmlObjMgt_SRelocationTable&) const
{
  occ::handle<TDataStd_RealArray> aRealArray = occ::down_cast<TDataStd_RealArray>(theSource);
  const occ::handle<NCollection_HArray1<double>>& hRealArray = aRealArray->Array();
  const NCollection_Array1<double>&               realArray  = hRealArray->Array1();
  int                                             aL = realArray.Lower(), anU = realArray.Upper();

  if (aL != 1)
    theTarget.Element().setAttribute(::FirstIndexString(), aL);
  theTarget.Element().setAttribute(::LastIndexString(), anU);
  theTarget.Element().setAttribute(::IsDeltaOn(), aRealArray->GetDelta() ? 1 : 0);

  // Allocation of 25 chars for each double value including the space:
  // An example: -3.1512678732195273e+020
  int                          iChar = 0;
  NCollection_LocalArray<char> str;
  if (realArray.Length())
  {
    try
    {
      OCC_CATCH_SIGNALS
      str.Allocate(25 * realArray.Length() + 1);
    }
    catch (Standard_OutOfMemory const&)
    {
      // If allocation of big space for the string of double array values failed,
      // try to calculate the necessary space more accurate and allocate it.
      // It may take some time... therefore it was not done initially and
      // an attempt to use a simple 25 chars for a double value was used.
      char buf[25];
      int  i(aL), nbChars(0);
      while (i <= anU)
      {
        nbChars += Sprintf(buf, "%.17g ", realArray.Value(i++)) + 1 /*a space*/;
      }
      if (nbChars)
        str.Allocate(nbChars);
    }
  }

  int i = aL;
  for (;;)
  {
    iChar += Sprintf(&(str[iChar]), "%.17g ", realArray.Value(i));
    if (i >= anU)
      break;
    ++i;
  }

  // No occurrence of '&', '<' and other irregular XML characters
  if (realArray.Length())
  {
    str[iChar - 1] = '\0';
    XmlObjMgt::SetStringValue(theTarget, (char*)str, true);
  }
  if (aRealArray->ID() != TDataStd_RealArray::GetID())
  {
    // convert GUID
    char                aGuidStr[Standard_GUID_SIZE_ALLOC];
    Standard_PCharacter pGuidStr = aGuidStr;
    aRealArray->ID().ToCString(pGuidStr);
    theTarget.Element().setAttribute(::AttributeIDString(), aGuidStr);
  }
}
