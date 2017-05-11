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
#include <TDataStd_ReferenceArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <XmlMDataStd_ReferenceArrayDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Document.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_ReferenceArrayDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (FirstIndexString, "first")
IMPLEMENT_DOMSTRING (LastIndexString,  "last")
IMPLEMENT_DOMSTRING (ExtString,        "string")
IMPLEMENT_DOMSTRING (AttributeIDString, "refarrattguid")
//=======================================================================
//function : XmlMDataStd_ReferenceArrayDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_ReferenceArrayDriver::XmlMDataStd_ReferenceArrayDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
     : XmlMDF_ADriver (theMsgDriver, NULL)
{

}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_ReferenceArrayDriver::NewEmpty() const
{
  return new TDataStd_ReferenceArray();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataStd_ReferenceArrayDriver::Paste(const XmlObjMgt_Persistent&  theSource,
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
                                 " for ReferenceArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  // Read the LastIndex; the attribute should present
  if (!anElement.getAttribute(::LastIndexString()).GetInteger(aLastInd)) 
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve the last index"
                                 " for ReferenceArray attribute as \"")
        + aFirstIndex + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TDataStd_ReferenceArray) aReferenceArray = Handle(TDataStd_ReferenceArray)::DownCast(theTarget);
  aReferenceArray->Init(aFirstInd, aLastInd);
  
  if (!anElement.hasChildNodes())
  {
    TCollection_ExtendedString aMessageString = 
      TCollection_ExtendedString("Cannot retrieve a Array of reference");
    WriteMessage (aMessageString);
    return Standard_False;
  }

  LDOM_Node aCurNode = anElement.getFirstChild();
  LDOM_Element* aCurElement = (LDOM_Element*)&aCurNode;
  XmlObjMgt_DOMString aValueStr;
  Standard_Integer i = aFirstInd;
  while (*aCurElement != anElement.getLastChild())
  {
    aValueStr = XmlObjMgt::GetStringValue( *aCurElement );
    if (aValueStr == NULL)
    {
      WriteMessage ("Cannot retrieve reference string from element");
      return Standard_False;
    }
    TCollection_AsciiString anEntry;
    if (XmlObjMgt::GetTagEntryString (aValueStr, anEntry) == Standard_False)
    {
      TCollection_ExtendedString aMessage =
        TCollection_ExtendedString ("Cannot retrieve reference from \"")
        + aValueStr + '\"';
      WriteMessage (aMessage);
      return Standard_False;
    }
    // Find label by entry
    TDF_Label tLab; // Null label.
    if (anEntry.Length() > 0)
    {
      TDF_Tool::Label(aReferenceArray->Label().Data(), anEntry, tLab, Standard_True);
    }
    aReferenceArray->SetValue(i++, tLab);
    aCurNode = aCurElement->getNextSibling();
    aCurElement = (LDOM_Element*)&aCurNode;
  }

  // Last reference
  aValueStr = XmlObjMgt::GetStringValue( *aCurElement );
  if (aValueStr == NULL)
  {
    WriteMessage ("Cannot retrieve reference string from element");
    return Standard_False;
  }
  TCollection_AsciiString anEntry;
  if (XmlObjMgt::GetTagEntryString (aValueStr, anEntry) == Standard_False)
  {
    TCollection_ExtendedString aMessage =
      TCollection_ExtendedString ("Cannot retrieve reference from \"")
      + aValueStr + '\"';
    WriteMessage (aMessage);
    return Standard_False;
  }
  // Find label by entry
  TDF_Label tLab; // Null label.
  if (anEntry.Length() > 0)
  {
    TDF_Tool::Label(aReferenceArray->Label().Data(), anEntry, tLab, Standard_True);
  }
  aReferenceArray->SetValue(i, tLab);

  // attribute id
  Standard_GUID aGUID;
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::AttributeIDString());
  if (aGUIDStr.Type() == XmlObjMgt_DOMString::LDOM_NULL)
    aGUID = TDataStd_ReferenceArray::GetID(); //default case
  else
    aGUID = Standard_GUID(Standard_CString(aGUIDStr.GetString())); // user defined case

  aReferenceArray->SetID(aGUID);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataStd_ReferenceArrayDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                             XmlObjMgt_Persistent&        theTarget,
                                             XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_ReferenceArray) aReferenceArray = Handle(TDataStd_ReferenceArray)::DownCast(theSource);
  TDF_Label L = aReferenceArray->Label();
  if (L.IsNull())
  {
    WriteMessage ("Label of a ReferenceArray is Null.");
    return;
  }

  Standard_Integer aL = aReferenceArray->Lower();
  Standard_Integer anU = aReferenceArray->Upper();
  XmlObjMgt_Element& anElement = theTarget;
  anElement.setAttribute(::FirstIndexString(), aL);
  anElement.setAttribute(::LastIndexString(), anU);
  
  XmlObjMgt_Document aDoc (anElement.getOwnerDocument());
  
  for (Standard_Integer i = aL; i <= anU; i++)
  {
    if (L.IsDescendant(aReferenceArray->Value(i).Root()))
    {
      // Internal reference
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(aReferenceArray->Value(i), anEntry);

      XmlObjMgt_DOMString aDOMString;
      XmlObjMgt::SetTagEntryString (aDOMString, anEntry);
      XmlObjMgt_Element aCurTarget = aDoc.createElement( ::ExtString() );
      XmlObjMgt::SetStringValue (aCurTarget, aDOMString, Standard_True);
      anElement.appendChild( aCurTarget );
    }
  }
  if(aReferenceArray->ID() != TDataStd_ReferenceArray::GetID()) {
    //convert GUID
    Standard_Character aGuidStr [Standard_GUID_SIZE_ALLOC];
    Standard_PCharacter pGuidStr = aGuidStr;
    aReferenceArray->ID().ToCString (pGuidStr);
    theTarget.Element().setAttribute (::AttributeIDString(), aGuidStr);
  }
}
