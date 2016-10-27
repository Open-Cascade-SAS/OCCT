// Created on: 2001-08-01
// Created by: Alexander GRIGORIEV
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
#include <TDataStd_Name.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_NameDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_NameDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (AttributeIDString, "nameguid")
//=======================================================================
//function : XmlMDataStd_NameDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_NameDriver::XmlMDataStd_NameDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty()
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_NameDriver::NewEmpty () const
{
  return (new TDataStd_Name());
}

//=======================================================================
//function : Paste()
//purpose  : 
//=======================================================================
Standard_Boolean XmlMDataStd_NameDriver::Paste
                               (const XmlObjMgt_Persistent&  theSource,
                                const Handle(TDF_Attribute)& theTarget,
                                XmlObjMgt_RRelocationTable&  ) const
{
  if(!theTarget.IsNull()) {	
    TCollection_ExtendedString aString;
    if (XmlObjMgt::GetExtendedString (theSource, aString))
    {
      Handle(TDataStd_Name)::DownCast(theTarget) -> Set (aString);    
      // attribute id
      Standard_GUID aGUID;
      const XmlObjMgt_Element& anElement = theSource;
      XmlObjMgt_DOMString aGUIDStr = anElement.getAttribute(::AttributeIDString());
      if (aGUIDStr.Type() == XmlObjMgt_DOMString::LDOM_NULL)
        aGUID = TDataStd_Name::GetID(); //default case
      else
        aGUID = Standard_GUID(Standard_CString(aGUIDStr.GetString())); // user defined case

      Handle(TDataStd_Name)::DownCast(theTarget)->SetID(aGUID);
	  return Standard_True;
    }
  }
  WriteMessage("error retrieving ExtendedString for type TDataStd_Name");
  return Standard_False;
}

//=======================================================================
//function : Paste()
//purpose  : store
//=======================================================================
void XmlMDataStd_NameDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    XmlObjMgt_Persistent&        theTarget,
                                    XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_Name) aName = Handle(TDataStd_Name)::DownCast(theSource);
  if (aName.IsNull()) return;
  XmlObjMgt::SetExtendedString (theTarget, aName -> Get());
  if(aName->ID() != TDataStd_Name::GetID()) {
    //convert GUID
    Standard_Character aGuidStr [Standard_GUID_SIZE_ALLOC];
    Standard_PCharacter pGuidStr = aGuidStr;
    aName->ID().ToCString (pGuidStr);
    theTarget.Element().setAttribute (::AttributeIDString(), aGuidStr);
  }
}
