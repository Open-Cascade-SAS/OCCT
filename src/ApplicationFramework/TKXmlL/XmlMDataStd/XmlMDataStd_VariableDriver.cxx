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

#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Variable.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataStd_VariableDriver.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataStd_VariableDriver, XmlMDF_ADriver)
IMPLEMENT_DOMSTRING(IsConstString, "isconst")
IMPLEMENT_DOMSTRING(UnitString, "unit")
IMPLEMENT_DOMSTRING(ConstString, "true")

//=================================================================================================

XmlMDataStd_VariableDriver::XmlMDataStd_VariableDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : XmlMDF_ADriver(theMsgDriver, nullptr)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> XmlMDataStd_VariableDriver::NewEmpty() const
{
  return (new TDataStd_Variable());
}

//=================================================================================================

bool XmlMDataStd_VariableDriver::Paste(const XmlObjMgt_Persistent&       theSource,
                                       const occ::handle<TDF_Attribute>& theTarget,
                                       XmlObjMgt_RRelocationTable&) const
{
  occ::handle<TDataStd_Variable> aV = occ::down_cast<TDataStd_Variable>(theTarget);

  XmlObjMgt_DOMString aStr = theSource.Element().getAttribute(::IsConstString());
  aV->Constant(aStr != nullptr);

  aStr = theSource.Element().getAttribute(::UnitString());
  aV->Unit(aStr);
  return true;
}

//=================================================================================================

void XmlMDataStd_VariableDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                       XmlObjMgt_Persistent&             theTarget,
                                       XmlObjMgt_SRelocationTable&) const
{
  occ::handle<TDataStd_Variable> aV = occ::down_cast<TDataStd_Variable>(theSource);
  if (aV->IsConstant())
    theTarget.Element().setAttribute(::IsConstString(), ::ConstString());
  theTarget.Element().setAttribute(::UnitString(), aV->Unit().ToCString());
}
