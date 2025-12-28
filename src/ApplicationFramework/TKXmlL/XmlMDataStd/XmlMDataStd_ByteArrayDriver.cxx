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
#include <TDataStd_ByteArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDocStd_FormatVersion.hxx>
#include <XmlMDataStd.hxx>
#include <XmlMDataStd_ByteArrayDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_DOMSTRING(AttributeIDString, "bytearrattguid")

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_ByteArrayDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(FirstIndexString, "first")
IMPLEMENT_DOMSTRING(LastIndexString, "last")
IMPLEMENT_DOMSTRING(IsDeltaOn, "delta")

//=================================================================================================

XmlMDataStd_ByteArrayDriver::XmlMDataStd_ByteArrayDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, NULL)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMDataStd_ByteArrayDriver::NewEmpty() const
{
  return new TDataStd_ByteArray();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================
bool XmlMDataStd_ByteArrayDriver::Paste(const XmlObjMgt_Persistent&  theSource,
                                                    const occ::handle<TDF_Attribute>& theTarget,
                                                    XmlObjMgt_RRelocationTable& theRelocTable) const
{
  int         aFirstInd, aLastInd, aValue;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex = anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for ByteArray attribute as \"")
      + aFirstIndex + "\"";
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  // Read the LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for ByteArray attribute as \"")
      + aFirstIndex + "\"";
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  if (aFirstInd > aLastInd)
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("The last index is greater than the first index"
                                 " for ByteArray attribute \"");
    myMessageDriver->Send(aMessageString, Message_Fail);
    return false;
  }

  occ::handle<TDataStd_ByteArray> aByteArray = occ::down_cast<TDataStd_ByteArray>(theTarget);

  // attribute id
  Standard_GUID       aGUID;
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::AttributeIDString());
  if (aGUIDStr.Type() == XmlObjMgt_DOMString::LDOM_NULL)
    aGUID = TDataStd_ByteArray::GetID(); // default case
  else
    aGUID = Standard_GUID(static_cast<const char*>(aGUIDStr.GetString())); // user defined case

  aByteArray->SetID(aGUID);

  occ::handle<NCollection_HArray1<uint8_t>> hArr = new NCollection_HArray1<uint8_t>(aFirstInd, aLastInd);
  NCollection_Array1<uint8_t>&         arr  = hArr->ChangeArray1();

  const char* aValueStr = static_cast<const char*>(XmlObjMgt::GetStringValue(anElement).GetString());
  int i = arr.Lower(), upper = arr.Upper();
  for (; i <= upper; i++)
  {
    if (!XmlObjMgt::GetInteger(aValueStr, aValue))
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve integer member"
                                   " for ByteArray attribute as \"")
        + aValueStr + "\"";
      myMessageDriver->Send(aMessageString, Message_Warning);
      aValue = 0;
    }
    arr.SetValue(i, (uint8_t)aValue);
  }
  aByteArray->ChangeArray(hArr);

  bool aDelta(false);

  if (theRelocTable.GetHeaderData()->StorageVersion().IntegerValue()
      >= TDocStd_FormatVersion_VERSION_3)
  {
    int aDeltaValue;
    if (!anElement.getAttribute(::IsDeltaOn()).GetInteger(aDeltaValue))
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve the isDelta value"
                                   " for ByteArray attribute as \"")
        + aDeltaValue + "\"";
      myMessageDriver->Send(aMessageString, Message_Fail);
      return false;
    }
    else
      aDelta = aDeltaValue != 0;
  }
#ifdef OCCT_DEBUG
  std::cout << "Current Document Format Version = "
            << theRelocTable.GetHeaderData()->StorageVersion().IntegerValue() << std::endl;
#endif
  aByteArray->SetDelta(aDelta);

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_ByteArrayDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                        XmlObjMgt_Persistent&        theTarget,
                                        XmlObjMgt_SRelocationTable&) const
{
  occ::handle<TDataStd_ByteArray> aByteArray = occ::down_cast<TDataStd_ByteArray>(theSource);

  int aL  = aByteArray->Lower();
  int anU = aByteArray->Upper();

  theTarget.Element().setAttribute(::FirstIndexString(), aL);
  theTarget.Element().setAttribute(::LastIndexString(), anU);
  theTarget.Element().setAttribute(::IsDeltaOn(), aByteArray->GetDelta() ? 1 : 0);

  const occ::handle<NCollection_HArray1<uint8_t>>& hArr = aByteArray->InternalArray();
  if (!hArr.IsNull() && hArr->Length())
  {
    // Access to data through an internal representation of the array is faster.
    const NCollection_Array1<uint8_t>& arr = hArr->Array1();

    // Allocate 4 characters (including a space ' ') for each byte (unsigned char) from the array.
    NCollection_LocalArray<char> str(4 * arr.Length() + 1);

    // Char counter in the array of chars.
    int iChar = 0;

    // Iterate on the array of bytes and fill-in the array of chars inserting spacing between the
    // chars.
    int iByte = arr.Lower(); // position inside the byte array
    for (; iByte <= arr.Upper(); ++iByte)
    {
      const uint8_t& byte = arr.Value(iByte);
      iChar += Sprintf(&(str[iChar]), "%d ", byte);
    }

    // Transfer the string (array of chars) to XML.
    XmlObjMgt::SetStringValue(theTarget, (char*)str, true);
  }
  if (aByteArray->ID() != TDataStd_ByteArray::GetID())
  {
    // convert GUID
    char  aGuidStr[Standard_GUID_SIZE_ALLOC];
    Standard_PCharacter pGuidStr = aGuidStr;
    aByteArray->ID().ToCString(pGuidStr);
    theTarget.Element().setAttribute(::AttributeIDString(), aGuidStr);
  }
}
