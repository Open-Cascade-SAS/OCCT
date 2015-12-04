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
#include <TDataStd_Variable.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_VariableDriver.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_VariableDriver,XmlMDF_ADriver)
IMPLEMENT_DOMSTRING (IsConstString, "isconst")
IMPLEMENT_DOMSTRING (UnitString,    "unit")
IMPLEMENT_DOMSTRING (ConstString,   "true")

//=======================================================================
//function : XmlMDataStd_VariableDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_VariableDriver::XmlMDataStd_VariableDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_VariableDriver::NewEmpty() const
{
  return (new TDataStd_Variable());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean XmlMDataStd_VariableDriver::Paste
                                (const XmlObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 XmlObjMgt_RRelocationTable&  ) const
{
  Handle(TDataStd_Variable) aV = Handle(TDataStd_Variable)::DownCast(theTarget);
  
  XmlObjMgt_DOMString aStr =
    theSource.Element().getAttribute(::IsConstString());
  aV->Constant (aStr != NULL);

  aStr = theSource.Element().getAttribute(::UnitString());
  aV->Unit(aStr);
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XmlMDataStd_VariableDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                        XmlObjMgt_Persistent&        theTarget,
                                        XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_Variable) aV = Handle(TDataStd_Variable)::DownCast(theSource);
  if (aV->IsConstant())
    theTarget.Element().setAttribute (::IsConstString(), ::ConstString());
  theTarget.Element().setAttribute(::UnitString(), aV->Unit().ToCString());
}
