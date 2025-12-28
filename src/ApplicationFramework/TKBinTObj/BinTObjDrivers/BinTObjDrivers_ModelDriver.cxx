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

#include <BinTObjDrivers_ModelDriver.hxx>

#include <BinObjMgt_Persistent.hxx>
#include <Standard_GUID.hxx>
#include <TDF_Attribute.hxx>

#include <TObj_TModel.hxx>
#include <TObj_Model.hxx>
#include <TObj_Assistant.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinTObjDrivers_ModelDriver, BinMDF_ADriver)

//=================================================================================================

BinTObjDrivers_ModelDriver::BinTObjDrivers_ModelDriver(
  const occ::handle<Message_Messenger>& theMessageDriver)
    : BinMDF_ADriver(theMessageDriver, NULL)
{
}

//=======================================================================
// function : NewEmpty
// purpose  : Creates a new attribute
//=======================================================================

occ::handle<TDF_Attribute> BinTObjDrivers_ModelDriver::NewEmpty() const
{
  return new TObj_TModel;
}

//=======================================================================
// function : Paste
// purpose  : Translate the contents of <theSource> and put it
//           into <theTarget>.
//           Set CurrentModel of TObj_Assistant into theTarget TObj_TModel
//           if its GUID and GUID stored in theSource are same
//=======================================================================

bool BinTObjDrivers_ModelDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                       const occ::handle<TDF_Attribute>& theTarget,
                                       BinObjMgt_RRelocationTable&) const
{
  Standard_GUID aGUID;
  if (!(theSource >> aGUID))
    return false;

  occ::handle<TObj_Model> aCurrentModel = TObj_Assistant::GetCurrentModel();
  if (aCurrentModel.IsNull())
    return false;

  if (aGUID != aCurrentModel->GetGUID())
  {
    myMessageDriver->Send("TObj_TModel retrieval: wrong model GUID", Message_Fail);
    return false;
  }

  occ::handle<TObj_TModel> aTModel = occ::down_cast<TObj_TModel>(theTarget);
  aCurrentModel->SetLabel(aTModel->Label());
  aTModel->Set(aCurrentModel);
  return true;
}

//=======================================================================
// function : Paste
// purpose  : Translate the contents of <theSource> and put it
//           into <theTarget>.
//           a Model is stored as its GUID
//=======================================================================

void BinTObjDrivers_ModelDriver::Paste(
  const occ::handle<TDF_Attribute>& theSource,
  BinObjMgt_Persistent&             theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>&) const
{
  occ::handle<TObj_TModel> aTModel = occ::down_cast<TObj_TModel>(theSource);
  occ::handle<TObj_Model>  aModel  = aTModel->Model();
  if (!aModel.IsNull())
  {
    // Store model GUID.
    Standard_GUID aGUID = aModel->GetGUID();
    theTarget << aGUID;
  }
}
