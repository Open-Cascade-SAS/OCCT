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


#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_UAttributeDriver.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_UAttributeDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (GuidString, "guid")

//=======================================================================
//function : XmlMDataStd_UAttributeDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_UAttributeDriver::XmlMDataStd_UAttributeDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_UAttributeDriver::NewEmpty() const
{
  return (new TDataStd_UAttribute());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean XmlMDataStd_UAttributeDriver::Paste
                                (const XmlObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 XmlObjMgt_RRelocationTable&  ) const
{
  XmlObjMgt_DOMString aGuidDomStr =
    theSource.Element().getAttribute (::GuidString());
  Standard_CString aGuidStr = (Standard_CString)aGuidDomStr.GetString();
  if (aGuidStr[0] == '\0') {
    WriteMessage ("error retrieving GUID for type TDataStd_UAttribute");
    return Standard_False;
  }

  Handle(TDataStd_UAttribute)::DownCast (theTarget) -> SetID (aGuidStr);
  return Standard_True;
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XmlMDataStd_UAttributeDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                         XmlObjMgt_Persistent&        theTarget,
                                         XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_UAttribute) aName =
    Handle(TDataStd_UAttribute)::DownCast(theSource);

  //convert GUID into attribute value
  Standard_Character aGuidStr [40];
  Standard_PCharacter pGuidStr;
  pGuidStr=aGuidStr;
  aName->ID().ToCString (pGuidStr);

  theTarget.Element().setAttribute (::GuidString(), aGuidStr);
}
