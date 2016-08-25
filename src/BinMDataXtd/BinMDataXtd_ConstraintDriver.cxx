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
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Real.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDF_Attribute.hxx>
#include <TNaming_NamedShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMDataXtd_ConstraintDriver,BinMDF_ADriver)

//=======================================================================
//function : BinMDataXtd_ConstraintDriver
//purpose  : Constructor
//=======================================================================
BinMDataXtd_ConstraintDriver::BinMDataXtd_ConstraintDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : BinMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataXtd_ConstraintDriver::NewEmpty() const
{
  return (new TDataXtd_Constraint());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMDataXtd_ConstraintDriver::Paste
                        (const BinObjMgt_Persistent&  theSource,
                         const Handle(TDF_Attribute)& theTarget,
                         BinObjMgt_RRelocationTable&  theRelocTable) const
{
  Handle(TDataXtd_Constraint) aC = 
    Handle(TDataXtd_Constraint)::DownCast(theTarget);

  Standard_Integer aNb;

  // value
  if (! (theSource >> aNb))
    return Standard_False;
  if (aNb > 0)
  {
    Handle(TDataStd_Real) aTValue;
    if (theRelocTable.IsBound(aNb))
      aTValue = Handle(TDataStd_Real)::DownCast(theRelocTable.Find(aNb));
    else
    {
      aTValue = new TDataStd_Real;
      theRelocTable.Bind(aNb, aTValue);
    }
    aC->SetValue(aTValue);
  }

  // geometries
  Standard_Integer NbGeom;
  if (! (theSource >> NbGeom))
    return Standard_False;
  Standard_Integer iG = 1;
  while (iG <= NbGeom)
  {
    if (! (theSource >> aNb))
      return Standard_False;
    if (aNb > 0)
    {
      Handle(TNaming_NamedShape) aG;
      if (theRelocTable.IsBound(aNb))
        aG = Handle(TNaming_NamedShape)::DownCast(theRelocTable.Find(aNb));
      else
      {
        aG = new TNaming_NamedShape;
        theRelocTable.Bind(aNb, aG);
      }
      aC->SetGeometry (iG++, aG);
    }
  }

  // plane
  if (! (theSource >> aNb))
    return Standard_False;
  if (aNb > 0)
  {
    Handle(TNaming_NamedShape) aTPlane;
    if (theRelocTable.IsBound(aNb))
      aTPlane = Handle(TNaming_NamedShape)::DownCast(theRelocTable.Find(aNb));
    else
    {
      aTPlane = new TNaming_NamedShape;
      theRelocTable.Bind(aNb, aTPlane);
    }
    aC->SetPlane(aTPlane);
  }

  // constraint type
  Standard_Integer aType;
  if (! (theSource >> aType))
    return Standard_False;
  aC->SetType( (TDataXtd_ConstraintEnum) aType );

  // flags
  Standard_Integer flags;
  if (! (theSource >> flags))
    return Standard_False;
  aC->Verified ((flags & 1) != 0);
  aC->Inverted ((flags & 2) != 0);
  aC->Reversed ((flags & 4) != 0);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void BinMDataXtd_ConstraintDriver::Paste
                        (const Handle(TDF_Attribute)& theSource,
                         BinObjMgt_Persistent&        theTarget,
                         BinObjMgt_SRelocationTable&  theRelocTable) const
{
  Handle(TDataXtd_Constraint) aC =
    Handle(TDataXtd_Constraint)::DownCast(theSource);

  Standard_Integer aNb;

  // value
  Handle(TDataStd_Real) aValue = aC->GetValue();
  if (!aValue.IsNull())
    aNb = theRelocTable.Add(aValue);    // create and/or get index
  else
    aNb = -1;
  theTarget << aNb;

  // geometries
  Standard_Integer NbGeom = aC->NbGeometries();
  theTarget << NbGeom;
  Standard_Integer iG;
  for (iG = 1; iG <= NbGeom; iG++)
  {
    Handle(TNaming_NamedShape) aG = aC->GetGeometry(iG);
    if (!aG.IsNull())
      aNb = theRelocTable.Add(aG);
    else
      aNb = -1;
    theTarget << aNb;
  }

  // plane
  Handle(TNaming_NamedShape) aTPlane = aC->GetPlane();
  if (!aTPlane.IsNull())
    aNb = theRelocTable.Add(aTPlane);
  else
    aNb = -1;
  theTarget << aNb;

  // constraint type
  theTarget << (Standard_Integer) aC->GetType();

  // flags
  Standard_Integer flags = 0;
  if (aC->Verified()) flags |= 1;
  if (aC->Inverted()) flags |= 2;
  if (aC->Reversed()) flags |= 4;
  theTarget << flags;
}
