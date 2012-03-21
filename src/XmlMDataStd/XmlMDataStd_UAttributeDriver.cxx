// Created on: 2001-08-24
// Created by: Alexnder GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <XmlMDataStd_UAttributeDriver.ixx>
#include <TDataStd_UAttribute.hxx>

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
