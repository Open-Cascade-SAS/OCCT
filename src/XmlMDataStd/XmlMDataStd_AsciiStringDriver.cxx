// Created on: 2007-08-21
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


#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_AsciiStringDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_AsciiStringDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (AttributeIDString, "asciiguid")
//=======================================================================
//function : XmlMDataStd_AsciiStringDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_AsciiStringDriver::XmlMDataStd_AsciiStringDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty()
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_AsciiStringDriver::NewEmpty () const
{
  return (new TDataStd_AsciiString());
}

//=======================================================================
//function : Paste()
//purpose  : retrieve
//=======================================================================
Standard_Boolean XmlMDataStd_AsciiStringDriver::Paste
                               (const XmlObjMgt_Persistent&  theSource,
                                const Handle(TDF_Attribute)& theTarget,
                                XmlObjMgt_RRelocationTable&  ) const
{
  if(!theTarget.IsNull()) {
  const TCollection_AsciiString aString = XmlObjMgt::GetStringValue (theSource);
  Handle(TDataStd_AsciiString)::DownCast(theTarget) -> Set (aString);
  // attribute id
  Standard_GUID aGUID;
  const XmlObjMgt_Element& anElement = theSource;
  XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::AttributeIDString());
  if (aGUIDStr.Type() == XmlObjMgt_DOMString::LDOM_NULL)
    aGUID = TDataStd_AsciiString::GetID(); //default case
  else
    aGUID = Standard_GUID(Standard_CString(aGUIDStr.GetString())); // user defined case

  Handle(TDataStd_AsciiString)::DownCast(theTarget)->SetID(aGUID);
  return Standard_True;
  }
  WriteMessage("error retrieving AsciiString for type TDataStd_AsciiString");
  return Standard_False;
}

//=======================================================================
//function : Paste()
//purpose  : store
//=======================================================================
void XmlMDataStd_AsciiStringDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    XmlObjMgt_Persistent&        theTarget,
                                    XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_AsciiString) aS = Handle(TDataStd_AsciiString)::DownCast(theSource);
  if (aS.IsNull()) return;
  XmlObjMgt_DOMString aString = aS->Get().ToCString();
  XmlObjMgt::SetStringValue (theTarget, aString);
  if(aS->ID() != TDataStd_AsciiString::GetID()) {
    //convert GUID
    Standard_Character aGuidStr [Standard_GUID_SIZE_ALLOC];
    Standard_PCharacter pGuidStr = aGuidStr;
    aS->ID().ToCString (pGuidStr);
    theTarget.Element().setAttribute (::AttributeIDString(), aGuidStr);
  }
}
