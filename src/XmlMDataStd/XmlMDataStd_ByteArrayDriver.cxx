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


#include <CDM_MessageDriver.hxx>
#include <NCollection_LocalArray.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfByte.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd.hxx>
#include <XmlMDataStd_ByteArrayDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>
IMPLEMENT_DOMSTRING (AttributeIDString, "bytearrattguid")

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_ByteArrayDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString,  "last")
IMPLEMENT_DOMSTRING (IsDeltaOn,        "delta")
//=======================================================================
//function : XmlMDataStd_ByteArrayDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_ByteArrayDriver::XmlMDataStd_ByteArrayDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_ByteArrayDriver::NewEmpty() const
{
  return new TDataStd_ByteArray();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_ByteArrayDriver::Paste(const XmlObjMgt_Persistent&  theSource,
                                                    const Handle(TDF_Attribute)& theTarget,
                                                    XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd, aValue;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex= anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for ByteArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read the LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for ByteArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  if (aFirstInd > aLastInd)
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("The last index is greater than the first index"
                                 " for ByteArray attribute \"");
    WriteMessage (aMessageString);
    return Standard_False;
  }


  Handle(TDataStd_ByteArray) aByteArray = Handle(TDataStd_ByteArray)::DownCast(theTarget);
  Handle(TColStd_HArray1OfByte) hArr = new TColStd_HArray1OfByte(aFirstInd, aLastInd);
  TColStd_Array1OfByte& arr = hArr->ChangeArray1();

  Standard_CString aValueStr = Standard_CString(XmlObjMgt::GetStringValue(anElement).GetString());
  Standard_Integer i = arr.Lower(), upper = arr.Upper();
  for (; i <= upper; i++)
  {
    if (!XmlObjMgt::GetInteger(aValueStr, aValue)) 
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve integer member"
                                   " for ByteArray attribute as \"")
                                   + aValueStr + "\"";
      WriteMessage (aMessageString);
      return Standard_False;
    }
    arr.SetValue(i, (Standard_Byte) aValue);
  }
  aByteArray->ChangeArray(hArr);
  
  Standard_Boolean aDelta(Standard_False);
  
  if(XmlMDataStd::DocumentVersion() > 2) {
    Standard_Integer aDeltaValue;
    if (!anElement.getAttribute(::IsDeltaOn()).GetInteger(aDeltaValue)) 
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve the isDelta value"
                                   " for ByteArray attribute as \"")
                                   + aDeltaValue + "\"";
      WriteMessage (aMessageString);
      return Standard_False;
    } 
    else
      aDelta = aDeltaValue != 0;
  }
#ifdef OCCT_DEBUG
  else if(XmlMDataStd::DocumentVersion() == -1)
    cout << "Current DocVersion field is not initialized. "  <<endl;
#endif
  aByteArray->SetDelta(aDelta);

  // attribute id
  Standard_GUID aGUID;
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::AttributeIDString());
  if (aGUIDStr.Type() == XmlObjMgt_DOMString::LDOM_NULL)
    aGUID = TDataStd_ByteArray::GetID(); //default case
  else
    aGUID = Standard_GUID(Standard_CString(aGUIDStr.GetString())); // user defined case

  aByteArray->SetID(aGUID);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_ByteArrayDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                        XmlObjMgt_Persistent&        theTarget,
                                        XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_ByteArray) aByteArray = Handle(TDataStd_ByteArray)::DownCast(theSource);

  Standard_Integer aL = aByteArray->Lower();
  Standard_Integer anU = aByteArray->Upper();

  theTarget.Element().setAttribute(::FirstIndexString(), aL);
  theTarget.Element().setAttribute(::LastIndexString(), anU);
  theTarget.Element().setAttribute(::IsDeltaOn(),aByteArray->GetDelta() ? 1 : 0);

  const Handle(TColStd_HArray1OfByte)& hArr = aByteArray->InternalArray();
  if (!hArr.IsNull() && hArr->Length())
  {
    // Access to data through an internal reprsentation of the array is faster.
    const TColStd_Array1OfByte& arr = hArr->Array1();

    // Allocate 4 characters (including a space ' ') for each byte (unsigned char) from the array.
    NCollection_LocalArray<Standard_Character> str(4 * arr.Length() + 1);

    // Char counter in the array of chars.
    Standard_Integer iChar = 0;

    // Iterate on the array of bytes and fill-in the array of chars inserting spacing between the chars.
    Standard_Integer iByte = arr.Lower(); // position inside the byte array
    for (; iByte <= arr.Upper(); ++iByte)
    {
      const Standard_Byte& byte = arr.Value(iByte);
      iChar += Sprintf(&(str[iChar]), "%d ", byte);
    }

    // Transfer the string (array of chars) to XML.
    XmlObjMgt::SetStringValue (theTarget, (Standard_Character*)str, Standard_True);
  }
  if(aByteArray->ID() != TDataStd_ByteArray::GetID()) {
    //convert GUID
    Standard_Character aGuidStr [Standard_GUID_SIZE_ALLOC];
    Standard_PCharacter pGuidStr = aGuidStr;
    aByteArray->ID().ToCString (pGuidStr);
    theTarget.Element().setAttribute (::AttributeIDString(), aGuidStr);
  }
}
