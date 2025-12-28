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

// modified     13.04.2009 Sergey Zaritchny

#include <BinMDataXtd_GeometryDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDF_Attribute.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataXtd_GeometryDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataXtd_GeometryDriver::BinMDataXtd_GeometryDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, NULL)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDataXtd_GeometryDriver::NewEmpty() const
{
  return (new TDataXtd_Geometry());
}

//=======================================================================
// function : Paste
// purpose  : P -> T
//=======================================================================
bool BinMDataXtd_GeometryDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                                   const occ::handle<TDF_Attribute>& theTarget,
                                                   BinObjMgt_RRelocationTable&) const
{
  occ::handle<TDataXtd_Geometry> aT = occ::down_cast<TDataXtd_Geometry>(theTarget);

  int aType;
  bool ok = theSource >> aType;
  if (ok)
    aT->SetType((TDataXtd_GeometryEnum)aType);

  return ok;
}

//=======================================================================
// function : Paste
// purpose  : T -> P
//=======================================================================
void BinMDataXtd_GeometryDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                       BinObjMgt_Persistent&        theTarget,
                                       NCollection_IndexedMap<occ::handle<Standard_Transient>>&) const
{
  occ::handle<TDataXtd_Geometry> aG = occ::down_cast<TDataXtd_Geometry>(theSource);
  theTarget << (int)aG->GetType();
}
