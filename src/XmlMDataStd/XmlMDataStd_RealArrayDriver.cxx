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
#include <Standard_ErrorHandler.hxx>
#include <Standard_OutOfMemory.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd.hxx>
#include <XmlMDataStd_RealArrayDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_RealArrayDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString, "last")
IMPLEMENT_DOMSTRING (IsDeltaOn,       "delta")
IMPLEMENT_DOMSTRING (AttributeIDString, "realarrattguid")
//=======================================================================
//function : XmlMDataStd_RealArrayDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_RealArrayDriver::XmlMDataStd_RealArrayDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_RealArrayDriver::NewEmpty() const
{
  return (new TDataStd_RealArray());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_RealArrayDriver::Paste
                                        (const XmlObjMgt_Persistent&  theSource,
                                         const Handle(TDF_Attribute)& theTarget,
                                         XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd, ind;
  Standard_Real aValue;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex= anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for RealArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for RealArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDataStd_RealArray) aRealArray =
    Handle(TDataStd_RealArray)::DownCast(theTarget);
  aRealArray->Init(aFirstInd, aLastInd);

  // Check the type of LDOMString
  const XmlObjMgt_DOMString& aString = XmlObjMgt::GetStringValue(anElement);
  if (aString.Type() == LDOMBasicString::LDOM_Integer) {
    if (aFirstInd == aLastInd) {
      Standard_Integer anIntValue;
      if (aString.GetInteger(anIntValue))
        aRealArray -> SetValue (aFirstInd, Standard_Real(anIntValue));
    } else {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve array of real members"
                                   " for RealArray attribute from Integer \"")
        + aString + "\"";
      WriteMessage (aMessageString);
      return Standard_False;
    }
  } else {
    Standard_CString aValueStr = Standard_CString(aString.GetString());
    for (ind = aFirstInd; ind <= aLastInd; ind++)
    {
      if (!XmlObjMgt::GetReal(aValueStr, aValue)) {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve real member"
                                     " for RealArray attribute as \"")
            + aValueStr + "\"";
        WriteMessage (aMessageString);
        return Standard_False;
      }
      aRealArray->SetValue(ind, aValue);
    }
  }
  Standard_Boolean aDelta(Standard_False);
  
  if(XmlMDataStd::DocumentVersion() > 2) {
    Standard_Integer aDeltaValue;
    if (!anElement.getAttribute(::IsDeltaOn()).GetInteger(aDeltaValue)) 
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve the isDelta value"
                                     " for RealArray attribute as \"")
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
  aRealArray->SetDelta(aDelta);

  // attribute id
  Standard_GUID aGUID;
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::AttributeIDString());
  if (aGUIDStr.Type() == XmlObjMgt_DOMString::LDOM_NULL)
    aGUID = TDataStd_RealArray::GetID(); //default case
  else
    aGUID = Standard_GUID(Standard_CString(aGUIDStr.GetString())); // user defined case

  aRealArray->SetID(aGUID);
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_RealArrayDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                         XmlObjMgt_Persistent&        theTarget,
                                         XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_RealArray) aRealArray =
    Handle(TDataStd_RealArray)::DownCast(theSource);
  const Handle(TColStd_HArray1OfReal)& hRealArray = aRealArray->Array();
  const TColStd_Array1OfReal& realArray = hRealArray->Array1();
  Standard_Integer aL = realArray.Lower(), anU = realArray.Upper();

  if (aL != 1) theTarget.Element().setAttribute(::FirstIndexString(), aL);
  theTarget.Element().setAttribute(::LastIndexString(), anU);
  theTarget.Element().setAttribute(::IsDeltaOn(), aRealArray->GetDelta() ? 1 : 0);

  // Allocation of 25 chars for each double value including the space:
  // An example: -3.1512678732195273e+020
  Standard_Integer iChar = 0;
  NCollection_LocalArray<Standard_Character> str;
  if (realArray.Length())
  {
    try
    {
      OCC_CATCH_SIGNALS
      str.Allocate(25 * realArray.Length() + 1);
    }
    catch (Standard_OutOfMemory)
    {
      // If allocation of big space for the string of double array values failed,
      // try to calculate the necessary space more accurate and allocate it.
      // It may take some time... therefore it was not done initially and
      // an attempt to use a simple 25 chars for a double value was used.
      Standard_Character buf[25];
      Standard_Integer i(aL), nbChars(0);
      while (i <= anU)
      {
        nbChars += Sprintf(buf, "%.17g ", realArray.Value(i++)) + 1/*a space*/;
      }
      if (nbChars)
        str.Allocate(nbChars);
    }
  }
  
  Standard_Integer i = aL;
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
    XmlObjMgt::SetStringValue (theTarget, (Standard_Character*)str, Standard_True);
  }
  if(aRealArray->ID() != TDataStd_RealArray::GetID()) {
    //convert GUID
    Standard_Character aGuidStr [Standard_GUID_SIZE_ALLOC];
    Standard_PCharacter pGuidStr = aGuidStr;
    aRealArray->ID().ToCString (pGuidStr);
    theTarget.Element().setAttribute (::AttributeIDString(), aGuidStr);
  }
}
