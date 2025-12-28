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

#include <BinMDataXtd_ConstraintDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Real.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDF_Attribute.hxx>
#include <TNaming_NamedShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataXtd_ConstraintDriver, BinMDF_ADriver)

//=================================================================================================

BinMDataXtd_ConstraintDriver::BinMDataXtd_ConstraintDriver(
  const occ::handle<Message_Messenger>& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, nullptr)
{
}

//=================================================================================================

occ::handle<TDF_Attribute> BinMDataXtd_ConstraintDriver::NewEmpty() const
{
  return (new TDataXtd_Constraint());
}

//=======================================================================
// function : Paste
// purpose  : persistent -> transient (retrieve)
//=======================================================================

bool BinMDataXtd_ConstraintDriver::Paste(const BinObjMgt_Persistent&       theSource,
                                         const occ::handle<TDF_Attribute>& theTarget,
                                         BinObjMgt_RRelocationTable&       theRelocTable) const
{
  occ::handle<TDataXtd_Constraint> aC = occ::down_cast<TDataXtd_Constraint>(theTarget);

  int aNb;

  // value
  if (!(theSource >> aNb))
    return false;
  if (aNb > 0)
  {
    occ::handle<TDataStd_Real> aTValue;
    if (theRelocTable.IsBound(aNb))
      aTValue = occ::down_cast<TDataStd_Real>(theRelocTable.Find(aNb));
    else
    {
      aTValue = new TDataStd_Real;
      theRelocTable.Bind(aNb, aTValue);
    }
    aC->SetValue(aTValue);
  }

  // geometries
  int NbGeom;
  if (!(theSource >> NbGeom))
    return false;
  int iG = 1;
  while (iG <= NbGeom)
  {
    if (!(theSource >> aNb))
      return false;
    if (aNb > 0)
    {
      occ::handle<TNaming_NamedShape> aG;
      if (theRelocTable.IsBound(aNb))
        aG = occ::down_cast<TNaming_NamedShape>(theRelocTable.Find(aNb));
      else
      {
        aG = new TNaming_NamedShape;
        theRelocTable.Bind(aNb, aG);
      }
      aC->SetGeometry(iG++, aG);
    }
  }

  // plane
  if (!(theSource >> aNb))
    return false;
  if (aNb > 0)
  {
    occ::handle<TNaming_NamedShape> aTPlane;
    if (theRelocTable.IsBound(aNb))
      aTPlane = occ::down_cast<TNaming_NamedShape>(theRelocTable.Find(aNb));
    else
    {
      aTPlane = new TNaming_NamedShape;
      theRelocTable.Bind(aNb, aTPlane);
    }
    aC->SetPlane(aTPlane);
  }

  // constraint type
  int aType;
  if (!(theSource >> aType))
    return false;
  aC->SetType((TDataXtd_ConstraintEnum)aType);

  // flags
  int flags;
  if (!(theSource >> flags))
    return false;
  aC->Verified((flags & 1) != 0);
  aC->Inverted((flags & 2) != 0);
  aC->Reversed((flags & 4) != 0);

  return true;
}

//=======================================================================
// function : Paste
// purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataXtd_ConstraintDriver::Paste(
  const occ::handle<TDF_Attribute>&                        theSource,
  BinObjMgt_Persistent&                                    theTarget,
  NCollection_IndexedMap<occ::handle<Standard_Transient>>& theRelocTable) const
{
  occ::handle<TDataXtd_Constraint> aC = occ::down_cast<TDataXtd_Constraint>(theSource);

  int aNb;

  // value
  occ::handle<TDataStd_Real> aValue = aC->GetValue();
  if (!aValue.IsNull())
    aNb = theRelocTable.Add(aValue); // create and/or get index
  else
    aNb = -1;
  theTarget << aNb;

  // geometries
  int NbGeom = aC->NbGeometries();
  theTarget << NbGeom;
  int iG;
  for (iG = 1; iG <= NbGeom; iG++)
  {
    occ::handle<TNaming_NamedShape> aG = aC->GetGeometry(iG);
    if (!aG.IsNull())
      aNb = theRelocTable.Add(aG);
    else
      aNb = -1;
    theTarget << aNb;
  }

  // plane
  occ::handle<TNaming_NamedShape> aTPlane = aC->GetPlane();
  if (!aTPlane.IsNull())
    aNb = theRelocTable.Add(aTPlane);
  else
    aNb = -1;
  theTarget << aNb;

  // constraint type
  theTarget << (int)aC->GetType();

  // flags
  int flags = 0;
  if (aC->Verified())
    flags |= 1;
  if (aC->Inverted())
    flags |= 2;
  if (aC->Reversed())
    flags |= 4;
  theTarget << flags;
}
