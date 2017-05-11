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
#include <LDOM_MemManager.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_ExtStringListDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_ExtStringListDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString,  "last")
IMPLEMENT_DOMSTRING (ExtString,        "string")
IMPLEMENT_DOMSTRING (AttributeIDString, "extstrlistattguid")

//=======================================================================
//function : XmlMDataStd_ExtStringListDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_ExtStringListDriver::XmlMDataStd_ExtStringListDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_ExtStringListDriver::NewEmpty() const
{
  return new TDataStd_ExtStringList();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_ExtStringListDriver::Paste(const XmlObjMgt_Persistent&  theSource,
                                                        const Handle(TDF_Attribute)& theTarget,
                                                        XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aFirstInd, aLastInd;
  const XmlObjMgt_Element& anElement = theSource;

  // Read the FirstIndex; if the attribute is absent initialize to 1
  XmlObjMgt_DOMString aFirstIndex= anElement.getAttribute(::FirstIndexString());
  if (aFirstIndex == NULL)
    aFirstInd = 1;
  else if (!aFirstIndex.GetInteger(aFirstInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the first index"
                                 " for ExtStringList attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read the LastIndex; the attribute should present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for ExtStringList attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  const Handle(TDataStd_ExtStringList) anExtStringList = Handle(TDataStd_ExtStringList)::DownCast(theTarget);
  if(aLastInd > 0) {
    if (!anElement.hasChildNodes())
    {
      TCollection_ExtendedString aMessageString = 
        TCollection_ExtendedString("Cannot retrieve a list of extended strings");
      WriteMessage (aMessageString);
      return Standard_False;
    }

    LDOM_Node aCurNode = anElement.getFirstChild();
    LDOM_Element* aCurElement = (LDOM_Element*)&aCurNode;
    TCollection_ExtendedString aValueStr;
    while (*aCurElement != anElement.getLastChild())
    {
      XmlObjMgt::GetExtendedString( *aCurElement, aValueStr );
      anExtStringList->Append(aValueStr);
      aCurNode = aCurElement->getNextSibling();
      aCurElement = (LDOM_Element*)&aCurNode;
    }

    XmlObjMgt::GetExtendedString( *aCurElement, aValueStr );
    anExtStringList->Append(aValueStr);
  }
  // attribute id
  Standard_GUID aGUID;
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::AttributeIDString());
  if (aGUIDStr.Type() == XmlObjMgt_DOMString::LDOM_NULL)
    aGUID = TDataStd_ExtStringList::GetID(); //default case
  else
    aGUID = Standard_GUID(Standard_CString(aGUIDStr.GetString())); // user defined case

  anExtStringList->SetID(aGUID);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_ExtStringListDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                            XmlObjMgt_Persistent&        theTarget,
                                            XmlObjMgt_SRelocationTable&  ) const
{
  const Handle(TDataStd_ExtStringList) anExtStringList = Handle(TDataStd_ExtStringList)::DownCast(theSource);

  Standard_Integer anU = anExtStringList->Extent();
  XmlObjMgt_Element& anElement = theTarget;
  anElement.setAttribute(::LastIndexString(), anU);
  
  XmlObjMgt_Document aDoc (anElement.getOwnerDocument());
  
  TDataStd_ListIteratorOfListOfExtendedString itr(anExtStringList->List());
  for (; itr.More(); itr.Next())
  {
    const TCollection_ExtendedString& aValueStr = itr.Value();
    XmlObjMgt_Element aCurTarget = aDoc.createElement( ::ExtString() );
    XmlObjMgt::SetExtendedString( aCurTarget, aValueStr );
    anElement.appendChild( aCurTarget );
  }

  if(anExtStringList->ID() != TDataStd_ExtStringList::GetID()) {
    //convert GUID
    Standard_Character aGuidStr [Standard_GUID_SIZE_ALLOC];
    Standard_PCharacter pGuidStr = aGuidStr;
    anExtStringList->ID().ToCString (pGuidStr);
    theTarget.Element().setAttribute (::AttributeIDString(), aGuidStr);
  }
}
