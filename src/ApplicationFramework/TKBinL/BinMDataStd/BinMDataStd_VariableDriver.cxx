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

#include <BinMDataStd_VariableDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDataStd_Variable.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataStd_VariableDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataStd_VariableDriver::BinMDataStd_VariableDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, nullptr)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDataStd_VariableDriver::NewEmpty() const
{
  return (new TDataStd_Variable());
}

//=================================================================================================

bool BinMDataStd_VariableDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                       const occ::handle<TDF_Attribute>& theTarget,
                                       BinObjMgt_RRelocationTable&) const
{
  occ::handle<TDataStd_Variable> aV = occ::down_cast<TDataStd_Variable>(theTarget);

  bool isConstant;
  if (!(theSource >> isConstant))
    return false;
  aV->Constant(isConstant);

  TCollection_AsciiString anStr;
  if (!(theSource >> anStr))
    return false;
  aV->Unit(anStr);

  return true;
}

//=================================================================================================

void BinMDataStd_VariableDriver::Paste(
  const occ::handle<TDF_Attribute>& theSource,
  BinObjMgt_Persistent&             theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>&) const
{
  occ::handle<TDataStd_Variable> aV = occ::down_cast<TDataStd_Variable>(theSource);
  theTarget << aV->IsConstant() << aV->Unit();
}
