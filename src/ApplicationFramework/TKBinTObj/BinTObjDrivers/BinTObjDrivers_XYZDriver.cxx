// Created on: 2004-11-24
// Created by: Michael SAZONOV
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

#include <BinTObjDrivers_XYZDriver.hxx>
#include <Message_Messenger.hxx>
#include <TDF_Attribute.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <TObj_TXYZ.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinTObjDrivers_XYZDriver, BinMDF_ADriver)

//=================================================================================================

BinTObjDrivers_XYZDriver::BinTObjDrivers_XYZDriver(
  const occ::handle<Message_Messenger>& theMessageDriver)
    : BinMDF_ADriver(theMessageDriver, NULL)
{
}

//=======================================================================
// function : NewEmpty
// purpose  : Creates a new attribute
//=======================================================================

occ::handle<TDF_Attribute> BinTObjDrivers_XYZDriver::NewEmpty() const
{
  return new TObj_TXYZ;
}

//=======================================================================
// function : Paste
// purpose  : Retrieve. Translate the contents of <theSource> and put it
//           into <theTarget>.
//=======================================================================

bool BinTObjDrivers_XYZDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                     const occ::handle<TDF_Attribute>& theTarget,
                                     BinObjMgt_RRelocationTable&) const
{
  occ::handle<TObj_TXYZ> aTarget = occ::down_cast<TObj_TXYZ>(theTarget);
  double                 aX, aY, aZ;
  if (!(theSource >> aX >> aY >> aZ))
    return false;
  aTarget->Set(gp_XYZ(aX, aY, aZ));
  return true;
}

//=======================================================================
// function : Paste
// purpose  : Store. Translate the contents of <theSource> and put it
//           into <theTarget>
//=======================================================================

void BinTObjDrivers_XYZDriver::Paste(const occ::handle<TDF_Attribute>& theSource,
                                     BinObjMgt_Persistent&             theTarget,
                                     NCollection_IndexedMap<occ::handle<Standard_Transient>>&) const
{
  occ::handle<TObj_TXYZ> aSource = occ::down_cast<TObj_TXYZ>(theSource);
  gp_XYZ                 aXYZ    = aSource->Get();
  theTarget << aXYZ.X() << aXYZ.Y() << aXYZ.Z();
}
