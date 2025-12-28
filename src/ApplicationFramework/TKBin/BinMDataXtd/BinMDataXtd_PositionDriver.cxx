// Created on: 2004-05-17
// Created by: Sergey ZARITCHNY
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

// modified     13.04.2009 Sergey ZARITCHNY

#include <BinMDataXtd_PositionDriver.hxx>

#include <Standard_Type.hxx>
#include <TDataXtd_Position.hxx>
#include <TDF_Attribute.hxx>
#include <BinObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataXtd_PositionDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataXtd_PositionDriver::BinMDataXtd_PositionDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(TDataXtd_Position)->Name())
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDataXtd_PositionDriver::NewEmpty() const
{
  return new TDataXtd_Position();
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================

bool BinMDataXtd_PositionDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                       const occ::handle<TDF_Attribute>& theTarget,
                                       BinObjMgt_RRelocationTable&) const
{
  occ::handle<TDataXtd_Position> anAtt = occ::down_cast<TDataXtd_Position>(theTarget);
  double                         aValue;
  bool                           ok = theSource >> aValue;
  if (!ok)
    return ok;
  gp_Pnt aPosition(0., 0., 0.);
  aPosition.SetX(aValue);

  ok = theSource >> aValue;
  if (!ok)
    return ok;
  aPosition.SetY(aValue);

  ok = theSource >> aValue;
  if (!ok)
    return ok;
  aPosition.SetZ(aValue);

  anAtt->SetPosition(aPosition);

  return ok;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================

void BinMDataXtd_PositionDriver::Paste(
  const occ::handle<TDF_Attribute>& theSource,
  BinObjMgt_Persistent&             theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>&) const
{
  occ::handle<TDataXtd_Position> anAtt = occ::down_cast<TDataXtd_Position>(theSource);

  theTarget << anAtt->GetPosition().X();
  theTarget << anAtt->GetPosition().Y();
  theTarget << anAtt->GetPosition().Z();
}
