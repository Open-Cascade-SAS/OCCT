// Created on: 2007-07-03
// Created by: Sergey ZARITCHNY
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
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_NamedData.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_NamedDataDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_NamedDataDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(FirstIntegerIndex, "firstI")
IMPLEMENT_DOMSTRING(LastIntegerIndex, "lastI")
IMPLEMENT_DOMSTRING(FirstRealIndex, "firstR")
IMPLEMENT_DOMSTRING(LastRealIndex, "lastR")
IMPLEMENT_DOMSTRING(FirstStringIndex, "firstS")
IMPLEMENT_DOMSTRING(LastStringIndex, "lastS")
IMPLEMENT_DOMSTRING(FirstByteIndex, "firstB")
IMPLEMENT_DOMSTRING(LastByteIndex, "lastB")
IMPLEMENT_DOMSTRING(FirstIntArrIndex, "firstIA")
IMPLEMENT_DOMSTRING(LastIntArrIndex, "lastIA")
IMPLEMENT_DOMSTRING(FirstRealArrIndex, "firstRA")
IMPLEMENT_DOMSTRING(LastRealArrIndex, "lastRA")

IMPLEMENT_DOMSTRING(ExtString, "string")
IMPLEMENT_DOMSTRING(Value, "value")

#ifdef _WIN32
  #define EXCEPTION ...
#else
  #define EXCEPTION Standard_Failure const&
#endif
//=================================================================================================

XmlMDataStd_NamedDataDriver::XmlMDataStd_NamedDataDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, NULL)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMDataStd_NamedDataDriver::NewEmpty() const
{
  return new TDataStd_NamedData();
}

//=======================================================================
static TCollection_ExtendedString SplitItemFromEnd(TCollection_ExtendedString& Key)
{
  TCollection_ExtendedString aValue;
  const int     aPos = Key.SearchFromEnd(" ");
  if (aPos == -1)
    return aValue;
  aValue = Key.Split(aPos - 1);
  aValue.Remove(1, 1);
  return aValue;
}

//=======================================================================
static TCollection_ExtendedString SplitItemFromStart(TCollection_ExtendedString& Key)
{
  TCollection_ExtendedString aValue;
  const int     aPos = Key.Search(" ");
  if (aPos == -1)
    return aValue;
  aValue = Key.Split(aPos);
  Key.Remove(Key.Length(), 1);
  return aValue;
}

//=======================================================================
occ::handle<NCollection_HArray1<int>> BuildIntArray(const TCollection_AsciiString& ValString,
                                               const int         theLen)
{
  occ::handle<NCollection_HArray1<int>> anArr;
  if (ValString.Length() == 0 || !theLen)
    return anArr;
  anArr = new NCollection_HArray1<int>(1, theLen, 0);
  for (int i = 1; i <= theLen; i++)
  {
    const TCollection_AsciiString& aSVal = ValString.Token(" ", i);
    if (aSVal.Length())
      anArr->SetValue(i, aSVal.IntegerValue());
  }
  return anArr;
}

//=======================================================================
occ::handle<NCollection_HArray1<double>> BuildRealArray(const TCollection_AsciiString& ValString,
                                             const int         theLen)
{
  occ::handle<NCollection_HArray1<double>> anArr;
  if (ValString.Length() == 0 || !theLen)
    return anArr;
  anArr = new NCollection_HArray1<double>(1, theLen, .0);
  for (int i = 1; i <= theLen; i++)
  {
    const TCollection_AsciiString& aSVal = ValString.Token(" ", i);
    if (aSVal.Length())
      anArr->SetValue(i, aSVal.RealValue());
  }
  return anArr;
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool XmlMDataStd_NamedDataDriver::Paste(const XmlObjMgt_Persistent&  theSource,
                                                    const occ::handle<TDF_Attribute>& theTarget,
                                                    XmlObjMgt_RRelocationTable&) const
{
  int         aFirstInd, aLastInd, ind;
  const XmlObjMgt_Element& anElement = theSource;

  // DataMapOfStringInteger: Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex = anElement.getAttribute(::FirstIntegerIndex());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index for NamedData"
                                 " attribute (DataMapOfStringInteger) as \"")
      + aFirstIndex + "\"";
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  // Read the LastIndex;
  XmlObjMgt_DOMString aLastIndex = anElement.getAttribute(::LastIntegerIndex());
  if (aLastIndex == NULL)
  {
    aFirstInd = 0;
    aLastInd  = 0;
  }
  else if (!aLastIndex.GetInteger(aLastInd))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index for NamedData"
                                 " attribute (DataMapOfStringInteger) as \"")
      + aLastIndex + "\"";
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }
  try
  {
    occ::handle<TDataStd_NamedData> T = occ::down_cast<TDataStd_NamedData>(theTarget);
    LDOM_Node                  aCurNode;

    if ((aFirstInd | aLastInd) && aLastInd >= aFirstInd)
    {
      if (!anElement.hasChildNodes())
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve DataMapOfStringInteger");
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }
      aCurNode                                   = anElement.getFirstChild();
      LDOM_Element*                  aCurElement = (LDOM_Element*)&aCurNode;
      TCollection_ExtendedString     aValueStr, aKey;
      NCollection_DataMap<TCollection_ExtendedString, int> aMap;
      for (ind = aFirstInd; ind < aLastInd; ind++)
      {
        XmlObjMgt::GetExtendedString(*aCurElement, aKey);
        aValueStr = SplitItemFromEnd(aKey);
        if (aValueStr.Length() == 0)
        {
          TCollection_ExtendedString aMessageString =
            TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
          myMessageDriver->Send(aMessageString, Message_Fail);
          return false;
        }
        TCollection_AsciiString aVal(aValueStr, '?');
        int        aValue = aVal.IntegerValue();
        aMap.Bind(aKey, aValue);
        aCurNode    = aCurElement->getNextSibling();
        aCurElement = (LDOM_Element*)&aCurNode;
      }
      XmlObjMgt::GetExtendedString(*aCurElement, aKey);
      aValueStr = SplitItemFromEnd(aKey);
      if (aValueStr.Length() == 0)
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }
      TCollection_AsciiString aVal(aValueStr, '?');
      int        aValue = aVal.IntegerValue();
      aMap.Bind(aKey, aValue);
      T->ChangeIntegers(aMap);
    }

    // DataMapOfStringReal
    aFirstIndex = anElement.getAttribute(::FirstRealIndex());
    if (aFirstIndex == NULL)
      aFirstInd = 1;
    else if (!aFirstIndex.GetInteger(aFirstInd))
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve the first index for NamedData "
                                   "attribute (DataMapOfStringReal) as \"")
        + aFirstIndex + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }

    // Read the LastIndex;
    aLastIndex = anElement.getAttribute(::LastRealIndex());
    if (aLastIndex == NULL)
    {
      aFirstInd = 0;
      aLastInd  = 0;
    }
    else if (!aLastIndex.GetInteger(aLastInd))
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve the last index for NamedData"
                                   " attribute (DataMapOfStringReal) as \"")
        + aLastIndex + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }

    if ((aFirstInd | aLastInd) && aLastInd >= aFirstInd)
    {
      if (!anElement.hasChildNodes())
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve DataMapOfStringReal");
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }

      LDOM_Element* aCurElement;
      if (aCurNode.isNull())
        aCurNode = anElement.getFirstChild();
      else
        aCurNode = ((LDOM_Element*)&aCurNode)->getNextSibling();

      aCurElement = (LDOM_Element*)&aCurNode;
      TCollection_ExtendedString   aValueStr, aKey;
      NCollection_DataMap<TCollection_ExtendedString, double> aMap;
      for (ind = aFirstInd; ind < aLastInd; ind++)
      {
        XmlObjMgt::GetExtendedString(*aCurElement, aKey);
        aValueStr = SplitItemFromEnd(aKey);
        if (aValueStr.Length() == 0)
        {
          TCollection_ExtendedString aMessageString =
            TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
          myMessageDriver->Send(aMessageString, Message_Fail);
          return false;
        }
        TCollection_AsciiString aVal(aValueStr, '?');
        double           aValue = aVal.RealValue();
        aMap.Bind(aKey, aValue);
        aCurNode    = aCurElement->getNextSibling();
        aCurElement = (LDOM_Element*)&aCurNode;
      }
      XmlObjMgt::GetExtendedString(*aCurElement, aKey);
      aValueStr = SplitItemFromEnd(aKey);
      if (aValueStr.Length() == 0)
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }
      TCollection_AsciiString aVal(aValueStr, '?');
      double           aValue = aVal.RealValue();
      aMap.Bind(aKey, aValue);
      T->ChangeReals(aMap);
    }

    // DataMapOfStringString
    aFirstIndex = anElement.getAttribute(::FirstStringIndex());
    if (aFirstIndex == NULL)
      aFirstInd = 1;
    else if (!aFirstIndex.GetInteger(aFirstInd))
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve the first index for NamedData"
                                   " attribute (DataMapOfStringString) as \"")
        + aFirstIndex + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }
    aLastIndex = anElement.getAttribute(::LastStringIndex());
    if (aLastIndex == NULL)
    {
      aFirstInd = 0;
      aLastInd  = 0;
    }
    else if (!aLastIndex.GetInteger(aLastInd))
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve the last index for NamedData"
                                   " attribute (DataMapOfStringString) as \"")
        + aLastIndex + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }

    if ((aFirstInd | aLastInd) && aLastInd >= aFirstInd)
    {
      if (!anElement.hasChildNodes())
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve DataMapOfStringString");
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }
      LDOM_Element* aCurElement;
      if (aCurNode.isNull())
        aCurNode = anElement.getFirstChild();
      else
        aCurNode = ((LDOM_Element*)&aCurNode)->getNextSibling();

      aCurElement = (LDOM_Element*)&aCurNode;
      TCollection_ExtendedString     aValue, aKey;
      NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString> aMap;
      for (ind = aFirstInd; ind < aLastInd; ind++)
      {
        XmlObjMgt::GetExtendedString(*aCurElement, aKey);
        aValue = SplitItemFromStart(aKey); // ==>from start
        if (aValue.Length() == 0)
        {
          TCollection_ExtendedString aMessageString =
            TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
          myMessageDriver->Send(aMessageString, Message_Fail);
          return false;
        }
        aMap.Bind(aKey, aValue);
        aCurNode    = aCurElement->getNextSibling();
        aCurElement = (LDOM_Element*)&aCurNode;
      }
      XmlObjMgt::GetExtendedString(*aCurElement, aKey);
      aValue = SplitItemFromStart(aKey);
      if (aValue.Length() == 0)
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }

      aMap.Bind(aKey, aValue);
      T->ChangeStrings(aMap);
    }

    // DataMapOfStringByte
    aFirstIndex = anElement.getAttribute(::FirstByteIndex());
    if (aFirstIndex == NULL)
      aFirstInd = 1;
    else if (!aFirstIndex.GetInteger(aFirstInd))
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve the first index for NamedData "
                                   "attribute (DataMapOfStringByte) as \"")
        + aFirstIndex + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }

    // Read the LastIndex;
    aLastIndex = anElement.getAttribute(::LastByteIndex());
    if (aLastIndex == NULL)
    {
      aFirstInd = 0;
      aLastInd  = 0;
    }
    else if (!aLastIndex.GetInteger(aLastInd))
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve the last index for NamedData"
                                   " attribute (DataMapOfStringByte) as \"")
        + aLastIndex + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }

    if ((aFirstInd | aLastInd) && aLastInd >= aFirstInd)
    {
      if (!anElement.hasChildNodes())
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve DataMapOfStringByte");
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }

      LDOM_Element* aCurElement;
      if (aCurNode.isNull())
        aCurNode = anElement.getFirstChild();
      else
        aCurNode = ((LDOM_Element*)&aCurNode)->getNextSibling();

      aCurElement = (LDOM_Element*)&aCurNode;
      TCollection_ExtendedString   aValueStr, aKey;
      NCollection_DataMap<TCollection_ExtendedString, uint8_t> aMap;
      for (ind = aFirstInd; ind < aLastInd; ind++)
      {
        XmlObjMgt::GetExtendedString(*aCurElement, aKey);
        aValueStr = SplitItemFromEnd(aKey);
        if (aValueStr.Length() == 0)
        {
          TCollection_ExtendedString aMessageString =
            TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
          myMessageDriver->Send(aMessageString, Message_Fail);
          return false;
        }

        TCollection_AsciiString aVal(aValueStr, '?');
        uint8_t           aValue = (uint8_t)aVal.IntegerValue();

        aMap.Bind(aKey, aValue);
        aCurNode    = aCurElement->getNextSibling();
        aCurElement = (LDOM_Element*)&aCurNode;
      }
      XmlObjMgt::GetExtendedString(*aCurElement, aKey);
      aValueStr = SplitItemFromEnd(aKey);
      if (aValueStr.Length() == 0)
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }

      TCollection_AsciiString aVal(aValueStr, '?');
      uint8_t           aValue = (uint8_t)aVal.IntegerValue();
      aMap.Bind(aKey, aValue);
      T->ChangeBytes(aMap);
    }

    // DataMapOfStringHArray1OfInteger
    aFirstIndex = anElement.getAttribute(::FirstIntArrIndex());
    if (aFirstIndex == NULL)
      aFirstInd = 1;
    else if (!aFirstIndex.GetInteger(aFirstInd))
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve the first index for NamedData "
                                   "attribute (DataMapOfStringHArray1OfInteger) as \"")
        + aFirstIndex + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }

    // Read the LastIndex;
    aLastIndex = anElement.getAttribute(::LastIntArrIndex());
    if (aLastIndex == NULL)
    {
      aFirstInd = 0;
      aLastInd  = 0;
    }
    else if (!aLastIndex.GetInteger(aLastInd))
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve the last index for NamedData"
                                   " attribute (DataMapOfStringHArray1OfInteger) as \"")
        + aLastIndex + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }

    if ((aFirstInd | aLastInd) && aLastInd >= aFirstInd)
    {
      if (!anElement.hasChildNodes())
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve DataMapOfStringHArray1OfInteger");
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }
      LDOM_Element* aCurElement;
      if (aCurNode.isNull())
        aCurNode = anElement.getFirstChild();
      else
        aCurNode = ((LDOM_Element*)&aCurNode)->getNextSibling();

      aCurElement = (LDOM_Element*)&aCurNode;
      TCollection_ExtendedString               aKey, aValueStr;
      NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>> aMap;

      for (ind = aFirstInd; ind < aLastInd; ind++)
      {
        XmlObjMgt::GetExtendedString(*aCurElement, aKey); // Len - at the end
        aValueStr = SplitItemFromEnd(aKey);
        if (aValueStr.Length() == 0)
        {
          TCollection_ExtendedString aMessageString =
            TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
          myMessageDriver->Send(aMessageString, Message_Fail);
          return false;
        }
        TCollection_AsciiString aVal(aValueStr, '?');
        int        aLen = aVal.IntegerValue();

        TCollection_AsciiString          aValueString = aCurElement->getAttribute(::Value());
        occ::handle<NCollection_HArray1<int>> aValue       = BuildIntArray(aValueString, aLen);
        if (aValue.IsNull())
        {
          TCollection_ExtendedString aMessageString =
            TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
          myMessageDriver->Send(aMessageString, Message_Fail);
          return false;
        }

        aMap.Bind(aKey, aValue);
        aCurNode    = aCurElement->getNextSibling();
        aCurElement = (LDOM_Element*)&aCurNode;
      }

      XmlObjMgt::GetExtendedString(*aCurElement, aKey);
      aValueStr = SplitItemFromEnd(aKey);
      if (aValueStr.Length() == 0)
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }
      TCollection_AsciiString          aVal(aValueStr, '?');
      int                 aLen         = aVal.IntegerValue();
      TCollection_AsciiString          aValueString = aCurElement->getAttribute(::Value());
      occ::handle<NCollection_HArray1<int>> aValue       = BuildIntArray(aValueString, aLen);
      if (aValue.IsNull())
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }
      aMap.Bind(aKey, aValue);
      T->ChangeArraysOfIntegers(aMap);
    }

    // DataMapOfStringHArray1OfReal
    aFirstIndex = anElement.getAttribute(::FirstRealArrIndex());
    if (aFirstIndex == NULL)
      aFirstInd = 1;
    else if (!aFirstIndex.GetInteger(aFirstInd))
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve the first index for NamedData "
                                   "attribute (DataMapOfStringHArray1OfReal) as \"")
        + aFirstIndex + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }

    // Read the LastIndex;
    aLastIndex = anElement.getAttribute(::LastRealArrIndex());
    if (aLastIndex == NULL)
    {
      aFirstInd = 0;
      aLastInd  = 0;
    }
    else if (!aLastIndex.GetInteger(aLastInd))
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve the last index for NamedData"
                                   " attribute (DataMapOfStringHArray1OfReal) as \"")
        + aLastIndex + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }

    if ((aFirstInd | aLastInd) && aLastInd >= aFirstInd)
    {
      if (!anElement.hasChildNodes())
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve DataMapOfStringHArray1OfReal");
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }

      LDOM_Element* aCurElement;
      if (aCurNode.isNull())
        aCurNode = anElement.getFirstChild();
      else
        aCurNode = ((LDOM_Element*)&aCurNode)->getNextSibling();

      aCurElement = (LDOM_Element*)&aCurNode;
      TCollection_ExtendedString            aKey, aValueStr;
      NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<double>>> aMap;

      for (ind = aFirstInd; ind < aLastInd; ind++)
      {
        XmlObjMgt::GetExtendedString(*aCurElement, aKey); // Len - at the end
        aValueStr = SplitItemFromEnd(aKey);
        if (aValueStr.Length() == 0)
        {
          TCollection_ExtendedString aMessageString =
            TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
          myMessageDriver->Send(aMessageString, Message_Fail);
          return false;
        }
        TCollection_AsciiString aVal(aValueStr, '?');
        int        aLen = aVal.IntegerValue();

        TCollection_AsciiString       aValueString = aCurElement->getAttribute(::Value());
        occ::handle<NCollection_HArray1<double>> aValue       = BuildRealArray(aValueString, aLen);
        if (aValue.IsNull())
        {
          TCollection_ExtendedString aMessageString =
            TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
          myMessageDriver->Send(aMessageString, Message_Fail);
          return false;
        }

        aMap.Bind(aKey, aValue);
        aCurNode    = aCurElement->getNextSibling();
        aCurElement = (LDOM_Element*)&aCurNode;
      }

      XmlObjMgt::GetExtendedString(*aCurElement, aKey);
      aValueStr = SplitItemFromEnd(aKey);
      if (aValueStr.Length() == 0)
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }
      TCollection_AsciiString aVal(aValueStr, '?');
      int        aLen = aVal.IntegerValue();

      TCollection_AsciiString       aValueString = aCurElement->getAttribute(::Value());
      occ::handle<NCollection_HArray1<double>> aValue       = BuildRealArray(aValueString, aLen);
      if (aValue.IsNull())
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve a value from item = ") + aKey;
        myMessageDriver->Send(aMessageString, Message_Fail);
        return false;
      }

      aMap.Bind(aKey, aValue);
      T->ChangeArraysOfReals(aMap);
    }
  }
  catch (EXCEPTION)
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Unknown exception during data retrieve in NamedDatDriver ");
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_NamedDataDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                        XmlObjMgt_Persistent&        theTarget,
                                        XmlObjMgt_SRelocationTable&) const
{
  occ::handle<TDataStd_NamedData> S = occ::down_cast<TDataStd_NamedData>(theSource);
  if (S.IsNull())
  {
    myMessageDriver->Send("NamedDataDriver:: The source attribute is Null.", Message_Warning);
    return;
  }

  int   i         = 0, up;
  XmlObjMgt_Element& anElement = theTarget;
  XmlObjMgt_Document aDoc(anElement.getOwnerDocument());
  S->LoadDeferredData();
  if (S->HasIntegers() && !S->GetIntegersContainer().IsEmpty())
  {
    // store a set of elements with string in each of them
    up = S->GetIntegersContainer().Extent();
    theTarget.Element().setAttribute(::LastIntegerIndex(), up);

    NCollection_DataMap<TCollection_ExtendedString, int>::Iterator itr(S->GetIntegersContainer());
    for (i = 1; itr.More(); itr.Next(), i++)
    {
      const TCollection_ExtendedString aValueStr =
        itr.Key() + ' ' + TCollection_ExtendedString(itr.Value()); // key - value;
      XmlObjMgt_Element aCurTarget = aDoc.createElement(::ExtString());
      XmlObjMgt::SetExtendedString(aCurTarget, aValueStr);
      anElement.appendChild(aCurTarget);
    }
  }

  if (S->HasReals() && !S->GetRealsContainer().IsEmpty())
  {
    up = S->GetRealsContainer().Extent();
    theTarget.Element().setAttribute(::LastRealIndex(), up);
    NCollection_DataMap<TCollection_ExtendedString, double>::Iterator itr(S->GetRealsContainer());
    for (i = 1; itr.More(); itr.Next(), i++)
    {
      const TCollection_ExtendedString aValueStr =
        itr.Key() + ' ' + TCollection_ExtendedString(itr.Value()); // key - value;
      XmlObjMgt_Element aCurTarget = aDoc.createElement(::ExtString());
      XmlObjMgt::SetExtendedString(aCurTarget, aValueStr);
      anElement.appendChild(aCurTarget);
    }
  }

  if (S->HasStrings() && !S->GetStringsContainer().IsEmpty())
  {
    up = S->GetStringsContainer().Extent();
    theTarget.Element().setAttribute(::LastStringIndex(), up);
    NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString>::Iterator itr(S->GetStringsContainer());
    for (i = 1; itr.More(); itr.Next(), i++)
    {
      const TCollection_ExtendedString aValueStr =
        itr.Key() + ' ' + TCollection_ExtendedString(itr.Value()); // key(without blanks) - value;
      XmlObjMgt_Element aCurTarget = aDoc.createElement(::ExtString());
      XmlObjMgt::SetExtendedString(aCurTarget, aValueStr);
      anElement.appendChild(aCurTarget);
    }
  }

  if (S->HasBytes() && !S->GetBytesContainer().IsEmpty())
  {
    up = S->GetBytesContainer().Extent();
    theTarget.Element().setAttribute(::LastByteIndex(), up);
    NCollection_DataMap<TCollection_ExtendedString, uint8_t>::Iterator itr(S->GetBytesContainer());
    for (i = 1; itr.More(); itr.Next(), i++)
    {
      const TCollection_ExtendedString aValueStr =
        itr.Key() + ' ' + TCollection_ExtendedString(itr.Value()); // key - value;
      XmlObjMgt_Element aCurTarget = aDoc.createElement(::ExtString());
      XmlObjMgt::SetExtendedString(aCurTarget, aValueStr);
      anElement.appendChild(aCurTarget);
    }
  }

  if (S->HasArraysOfIntegers() && !S->GetArraysOfIntegersContainer().IsEmpty())
  {
    up = S->GetArraysOfIntegersContainer().Extent();
    theTarget.Element().setAttribute(::LastIntArrIndex(), up);
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>>::Iterator itr(
      S->GetArraysOfIntegersContainer());
    for (i = 1; itr.More(); itr.Next(), i++)
    {
      const NCollection_Array1<int>& anArr1 = itr.Value()->Array1();
      const int         aLen   = anArr1.Upper() - anArr1.Lower() + 1;

      const TCollection_ExtendedString aValueStr =
        itr.Key() + ' ' + TCollection_ExtendedString(aLen); // key - Num_of_Arr_elements;
      XmlObjMgt_Element aCurTarget = aDoc.createElement(::ExtString());
      XmlObjMgt::SetExtendedString(aCurTarget, aValueStr); // key
      anElement.appendChild(aCurTarget);

      // Value = Array
      TCollection_AsciiString aValueStr2;
      int        j = anArr1.Lower();
      for (;;)
      {
        aValueStr2 += TCollection_AsciiString(anArr1.Value(j));
        if (j >= anArr1.Upper())
          break;
        aValueStr2 += ' ';
        j++;
      }

      aCurTarget.setAttribute(::Value(), aValueStr2.ToCString());
    }
  }

  if (S->HasArraysOfReals() && !S->GetArraysOfRealsContainer().IsEmpty())
  {
    up = S->GetArraysOfRealsContainer().Extent();
    theTarget.Element().setAttribute(::LastRealArrIndex(), up);
    NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<double>>>::Iterator itr(S->GetArraysOfRealsContainer());
    for (i = 1; itr.More(); itr.Next(), i++)
    {
      const NCollection_Array1<double>& anArr1 = itr.Value()->Array1();
      const int      aLen   = anArr1.Upper() - anArr1.Lower() + 1;

      // key
      const TCollection_ExtendedString aValueStr =
        itr.Key() + ' ' + TCollection_ExtendedString(aLen); // key - Num_of_Arr_elements;
      XmlObjMgt_Element aCurTarget = aDoc.createElement(::ExtString());
      XmlObjMgt::SetExtendedString(aCurTarget, aValueStr); // key
      anElement.appendChild(aCurTarget);

      // Value = Array
      TCollection_AsciiString aValueStr2;
      int        j = anArr1.Lower();
      for (;;)
      {
        char aValueChar[32];
        Sprintf(aValueChar, "%.15g", anArr1.Value(j));
        TCollection_AsciiString aValueStr3(aValueChar);
        aValueStr2 += aValueStr3;
        if (j >= anArr1.Upper())
          break;
        aValueStr2 += ' ';
        j++;
      }
      aCurTarget.setAttribute(::Value(), aValueStr2.ToCString());
    }
  }
}
