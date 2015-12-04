// Created on: 2008-12-10
// Created by: Pavel TELKOV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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
#include <TCollection_HAsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_Material.hxx>
#include <XmlMXCAFDoc_MaterialDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_MaterialDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (NameIndexString,     "name")
IMPLEMENT_DOMSTRING (DescrIndexString,    "descr")
IMPLEMENT_DOMSTRING (DensNameIndexString, "dens_name")
IMPLEMENT_DOMSTRING (DensTypeIndexString, "dens_type")

//=======================================================================
//function : XmlMXCAFDoc_MaterialDriver
//purpose  : Constructor
//=======================================================================
XmlMXCAFDoc_MaterialDriver::XmlMXCAFDoc_MaterialDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver)
: XmlMDF_ADriver (theMsgDriver, "xcaf", "Material")
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_MaterialDriver::NewEmpty() const
{
  return (new XCAFDoc_Material());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMXCAFDoc_MaterialDriver::Paste
                                (const XmlObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Real aDensity;
  XmlObjMgt_DOMString aRealStr = XmlObjMgt::GetStringValue(theSource);

  if (XmlObjMgt::GetReal(aRealStr, aDensity) == Standard_False) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve Material attribute density from \"")
        + aRealStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }
  
  const XmlObjMgt_Element& anElement = theSource;
  XmlObjMgt_DOMString aNameStr = anElement.getAttribute(::NameIndexString());
  XmlObjMgt_DOMString aDescrStr = anElement.getAttribute(::DescrIndexString());
  XmlObjMgt_DOMString aDensNameStr = anElement.getAttribute(::DensNameIndexString());
  XmlObjMgt_DOMString aDensTypeStr = anElement.getAttribute(::DensTypeIndexString());
  if ( aNameStr == NULL || aDescrStr == NULL ||
       aDensNameStr == NULL ||aDensTypeStr == NULL ) {
    TCollection_ExtendedString aMessageString
      ("Cannot retrieve Material attribute name or description");
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TCollection_HAsciiString) aName =
    new TCollection_HAsciiString( aNameStr.GetString() );
  Handle(TCollection_HAsciiString) aDescr =
    new TCollection_HAsciiString( aDescrStr.GetString() );
  Handle(TCollection_HAsciiString) aDensName =
    new TCollection_HAsciiString( aDensNameStr.GetString() );
  Handle(TCollection_HAsciiString) aDensType =
    new TCollection_HAsciiString( aDensTypeStr.GetString() );
  
  Handle(XCAFDoc_Material) anAtt = Handle(XCAFDoc_Material)::DownCast(theTarget);
  anAtt->Set(aName, aDescr, aDensity, aDensName, aDensType);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMXCAFDoc_MaterialDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                       XmlObjMgt_Persistent&        theTarget,
                                       XmlObjMgt_SRelocationTable&  ) const
{
  Handle(XCAFDoc_Material) anAtt = Handle(XCAFDoc_Material)::DownCast(theSource);
  
  XmlObjMgt_DOMString aNameString, aDescrString, aDensNameStr, aDensTypeStr;
  if ( !anAtt->GetName().IsNull() )
    aNameString = anAtt->GetName()->String().ToCString();
  if ( !anAtt->GetDescription().IsNull() )
    aDescrString = anAtt->GetDescription()->String().ToCString();
  if ( !anAtt->GetDensName().IsNull() )
    aDensNameStr = anAtt->GetDensName()->String().ToCString();
  if ( !anAtt->GetDensValType().IsNull() )
    aDensTypeStr = anAtt->GetDensValType()->String().ToCString();

  TCollection_AsciiString aDensityStr (anAtt->GetDensity());
  XmlObjMgt::SetStringValue (theTarget, aDensityStr.ToCString());
  theTarget.Element().setAttribute(::NameIndexString(), aNameString);
  theTarget.Element().setAttribute(::DescrIndexString(),aDescrString);
  theTarget.Element().setAttribute(::DensNameIndexString(),aDensNameStr);
  theTarget.Element().setAttribute(::DensTypeIndexString(),aDensTypeStr);
}
