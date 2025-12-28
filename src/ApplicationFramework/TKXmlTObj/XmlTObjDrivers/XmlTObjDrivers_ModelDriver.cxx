// Created on: 2004-11-24
// Created by: Edward AGAPOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include "XmlTObjDrivers_ModelDriver.hxx"

#include <XmlObjMgt_Persistent.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt.hxx>
#include <Standard_GUID.hxx>

#include <TObj_TModel.hxx>
#include <TObj_Model.hxx>
#include <TObj_Assistant.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlTObjDrivers_ModelDriver, XmlMDF_ADriver)

//=================================================================================================

XmlTObjDrivers_ModelDriver::XmlTObjDrivers_ModelDriver(
  const occ::handle<Message_Messenger>& theMessageDriver)
    : XmlMDF_ADriver(theMessageDriver, nullptr)
{
}

//=======================================================================
// function : NewEmpty
// purpose  : Creates a new attribute
//=======================================================================

occ::handle<TDF_Attribute> XmlTObjDrivers_ModelDriver::NewEmpty() const
{
  return new TObj_TModel;
}

//=======================================================================
// function : Paste
// purpose  : Translate the contents of <aSource> and put it
//           into <aTarget>, using the relocation table
//           <aRelocTable> to keep the sharings.
//           Set CurrentModel of TObj_Assistant into Target TObj_TModel
//           if its GUID and GUID stored in Source are same
//=======================================================================

bool XmlTObjDrivers_ModelDriver::Paste(const XmlObjMgt_Persistent&       Source,
                                       const occ::handle<TDF_Attribute>& Target,
                                       XmlObjMgt_RRelocationTable& /*RelocTable*/) const
{
  TCollection_ExtendedString aString;
  if (XmlObjMgt::GetExtendedString(Source, aString))
  {
    Standard_GUID           aGUID(aString.ToExtString());
    occ::handle<TObj_Model> aCurrentModel = TObj_Assistant::GetCurrentModel();
    if (aGUID == aCurrentModel->GetGUID())
    {
      occ::handle<TObj_TModel> aTModel = occ::down_cast<TObj_TModel>(Target);
      aCurrentModel->SetLabel(aTModel->Label());
      aTModel->Set(aCurrentModel);
      return true;
    }
    myMessageDriver->Send("TObj_TModel retrieval: wrong model GUID", Message_Fail);
    return false;
  }
  myMessageDriver->Send("error retrieving ExtendedString for type TObj_TModel", Message_Fail);
  return false;
}

//=======================================================================
// function : Paste
// purpose  : Translate the contents of <aSource> and put it
//           into <aTarget>, using the relocation table
//           <aRelocTable> to keep the sharings.
//           a Model is stored as its GUID
//=======================================================================

void XmlTObjDrivers_ModelDriver::Paste(const occ::handle<TDF_Attribute>& Source,
                                       XmlObjMgt_Persistent&             Target,
                                       XmlObjMgt_SRelocationTable& /*RelocTable*/) const
{
  occ::handle<TObj_TModel> aTModel = occ::down_cast<TObj_TModel>(Source);
  occ::handle<TObj_Model>  aModel  = aTModel->Model();

  // Store model GUID.
  Standard_PCharacter aPGuidString = new char[256];
  aModel->GetGUID().ToCString(aPGuidString);
  XmlObjMgt::SetExtendedString(Target, aPGuidString);
  delete[] aPGuidString;
}
