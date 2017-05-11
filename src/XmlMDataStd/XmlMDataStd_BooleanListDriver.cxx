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
#include <TDataStd_BooleanList.hxx>
#include <TDataStd_ListIteratorOfListOfByte.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_BooleanListDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_BooleanListDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString,  "last")
IMPLEMENT_DOMSTRING (AttributeIDString, "boollistattguid")

//=======================================================================
//function : XmlMDataStd_BooleanListDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_BooleanListDriver::XmlMDataStd_BooleanListDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_BooleanListDriver::NewEmpty() const
{
  return new TDataStd_BooleanList();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_BooleanListDriver::Paste(const XmlObjMgt_Persistent&  theSource,
                                                      const Handle(TDF_Attribute)& theTarget,
                                                      XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd, aValue, ind;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex= anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for BooleanList attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read the LastIndex; the attribute should be present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for BooleanList attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  const Handle(TDataStd_BooleanList) aBooleanList = Handle(TDataStd_BooleanList)::DownCast(theTarget);
  if(aLastInd == 0) aFirstInd = 0;
  if (aFirstInd == aLastInd && aLastInd > 0) 
  {
    Standard_Integer anInteger;
    if (!XmlObjMgt::GetStringValue(anElement).GetInteger(anInteger)) 
    {
      TCollection_ExtendedString aMessageString =
        TCollection_ExtendedString("Cannot retrieve integer member"
                                   " for BooleanList attribute as \"");
      WriteMessage (aMessageString);
      return Standard_False;
    }
    aBooleanList->Append(anInteger ? Standard_True : Standard_False);
  }
  else if(aLastInd >= 1)
  {
    Standard_CString aValueStr = Standard_CString(XmlObjMgt::GetStringValue(anElement).GetString());
    for (ind = aFirstInd; ind <= aLastInd; ind++)
    {
      if (!XmlObjMgt::GetInteger(aValueStr, aValue)) 
      {
        TCollection_ExtendedString aMessageString =
          TCollection_ExtendedString("Cannot retrieve integer member"
                                     " for BooleanList attribute as \"")
            + aValueStr + "\"";
        WriteMessage (aMessageString);
        return Standard_False;
      }
      aBooleanList->Append(aValue ? Standard_True : Standard_False);
    }
  }

  // attribute id
  Standard_GUID aGUID;
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::AttributeIDString());
  if (aGUIDStr.Type() == XmlObjMgt_DOMString::LDOM_NULL)
    aGUID = TDataStd_BooleanList::GetID(); //default case
  else
    aGUID = Standard_GUID(Standard_CString(aGUIDStr.GetString())); // user defined case

  aBooleanList->SetID(aGUID);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_BooleanListDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                          XmlObjMgt_Persistent&        theTarget,
                                          XmlObjMgt_SRelocationTable&  ) const
{
  const Handle(TDataStd_BooleanList) aBooleanList = Handle(TDataStd_BooleanList)::DownCast(theSource);

  Standard_Integer anU = aBooleanList->Extent();
  theTarget.Element().setAttribute(::LastIndexString(), anU);
  // Allocation of 1 char for each boolean value + a space. 
  NCollection_LocalArray<Standard_Character> str(2 * anU + 1);
  if(anU == 0) str[0] = 0;
  else if (anU >= 1)
  {
    Standard_Integer iChar(0);
    TDataStd_ListIteratorOfListOfByte itr(aBooleanList->List());
    for (; itr.More(); itr.Next())
    {
      const Standard_Byte& byte = itr.Value();
      iChar += Sprintf(&(str[iChar]), "%d ", byte);
    }
  }
  XmlObjMgt::SetStringValue (theTarget, (Standard_Character*)str, Standard_True);

  if(aBooleanList->ID() != TDataStd_BooleanList::GetID()) {
    //convert GUID
    Standard_Character aGuidStr [Standard_GUID_SIZE_ALLOC];
    Standard_PCharacter pGuidStr = aGuidStr;
    aBooleanList->ID().ToCString (pGuidStr);
    theTarget.Element().setAttribute (::AttributeIDString(), aGuidStr);
  }
}
