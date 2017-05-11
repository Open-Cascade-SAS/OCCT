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

//AGV 150202: Changed prototype XmlObjMgt::SetStringValue()

#include <CDM_MessageDriver.hxx>
#include <NCollection_LocalArray.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd.hxx>
#include <XmlMDataStd_IntegerArrayDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_IntegerArrayDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString,  "last")
IMPLEMENT_DOMSTRING (IsDeltaOn,        "delta")
IMPLEMENT_DOMSTRING (AttributeIDString, "intarrattguid")

//=======================================================================
//function : XmlMDataStd_IntegerArrayDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_IntegerArrayDriver::XmlMDataStd_IntegerArrayDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_IntegerArrayDriver::NewEmpty() const
{
  return (new TDataStd_IntegerArray());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_IntegerArrayDriver::Paste
                                (const XmlObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd, aValue, ind;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex= anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for IntegerArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read the LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for IntegerArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDataStd_IntegerArray) anIntArray =
    Handle(TDataStd_IntegerArray)::DownCast(theTarget);
  anIntArray->Init(aFirstInd, aLastInd);

  if(aFirstInd == aLastInd) {
    Standard_Integer anInteger;
    if(!XmlObjMgt::GetStringValue(anElement).GetInteger( anInteger)) {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve integer member"
                                   " for IntegerArray attribute as \"");
      WriteMessage (aMessageString);
      return Standard_False;
    }
    anIntArray->SetValue(aFirstInd, anInteger);
    
  }
  else {
    // Warning: check implementation of XmlObjMgt_DOMString !! For LDOM this is OK
    Standard_CString aValueStr =
      Standard_CString(XmlObjMgt::GetStringValue(anElement).GetString());
    
    for (ind = aFirstInd; ind <= aLastInd; ind++)
    {
      if (!XmlObjMgt::GetInteger(aValueStr, aValue)) {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve integer member"
                                     " for IntegerArray attribute as \"")
            + aValueStr + "\"";
        WriteMessage (aMessageString);
        return Standard_False;
      }
      anIntArray->SetValue(ind, aValue);
    }
  }
  Standard_Boolean aDelta(Standard_False);
  
  if(XmlMDataStd::DocumentVersion() > 2) {
    Standard_Integer aDeltaValue;
    if (!anElement.getAttribute(::IsDeltaOn()).GetInteger(aDeltaValue)) 
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve the isDelta value"
                                     " for IntegerArray attribute as \"")
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
  anIntArray->SetDelta(aDelta);

  // attribute id
  Standard_GUID aGUID;
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::AttributeIDString());
  if (aGUIDStr.Type() == XmlObjMgt_DOMString::LDOM_NULL)
    aGUID = TDataStd_IntegerArray::GetID(); //default case
  else
    aGUID = Standard_GUID(Standard_CString(aGUIDStr.GetString())); // user defined case

  anIntArray->SetID(aGUID);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_IntegerArrayDriver::Paste
                                (const Handle(TDF_Attribute)& theSource,
                                 XmlObjMgt_Persistent&        theTarget,
                                 XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_IntegerArray) anIntArray =
    Handle(TDataStd_IntegerArray)::DownCast(theSource);
  const Handle(TColStd_HArray1OfInteger)& hIntArray = anIntArray->Array();
  const TColStd_Array1OfInteger& intArray = hIntArray->Array1();
  Standard_Integer aL = intArray.Lower(), anU = intArray.Upper();

  if (aL != 1) 
    theTarget.Element().setAttribute(::FirstIndexString(), aL);
  theTarget.Element().setAttribute(::LastIndexString(), anU);
  theTarget.Element().setAttribute(::IsDeltaOn(), anIntArray->GetDelta() ? 1 : 0);

  // Allocation of 12 chars for each integer including the space.
  // An example: -2 147 483 648
  Standard_Integer iChar = 0;
  NCollection_LocalArray<Standard_Character> str;
  if (intArray.Length())
    str.Allocate(12 * intArray.Length() + 1);

  Standard_Integer i = aL;
  for (;;) 
  {
    iChar += Sprintf(&(str[iChar]), "%d ", intArray.Value(i));
    if (i >= anU)
      break;
    ++i;
  }

  if (intArray.Length())
  {
    // No occurrence of '&', '<' and other irregular XML characters
    str[iChar - 1] = '\0';
    XmlObjMgt::SetStringValue (theTarget, (Standard_Character*)str, Standard_True);
  }
  if(anIntArray->ID() != TDataStd_IntegerArray::GetID()) {
    //convert GUID
    Standard_Character aGuidStr [Standard_GUID_SIZE_ALLOC];
    Standard_PCharacter pGuidStr = aGuidStr;
    anIntArray->ID().ToCString (pGuidStr);
    theTarget.Element().setAttribute (::AttributeIDString(), aGuidStr);
  }
}
