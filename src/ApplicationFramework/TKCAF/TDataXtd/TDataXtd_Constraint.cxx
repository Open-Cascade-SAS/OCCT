// Created on: 2009-04-06
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd.hxx>
#include <TDataStd_Real.hxx>
#include <TDataXtd.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDataXtd_ConstraintEnum.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataXtd_Constraint, TDF_Attribute)

// for symmetry midpoint the third argument is the axis or the point
//=================================================================================================

const Standard_GUID& TDataXtd_Constraint::GetID()
{
  static Standard_GUID TDataXtd_ConstraintID("2a96b602-ec8b-11d0-bee7-080009dc3333");
  return TDataXtd_ConstraintID;
}

//=================================================================================================

occ::handle<TDataXtd_Constraint> TDataXtd_Constraint::Set(const TDF_Label& L)
{
  occ::handle<TDataXtd_Constraint> A;
  if (!L.FindAttribute(TDataXtd_Constraint::GetID(), A))
  {
    A = new TDataXtd_Constraint();
    L.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

TDataXtd_Constraint::TDataXtd_Constraint()
    : myType(TDataXtd_RADIUS),
      myIsReversed(false),
      myIsInverted(false),
      myIsVerified(true)
{
}

//=================================================================================================

void TDataXtd_Constraint::Set(const TDataXtd_ConstraintEnum          type,
                              const occ::handle<TNaming_NamedShape>& G1)
{
  // OCC2932 correction
  if (myType == type)
  {
    occ::handle<TNaming_NamedShape> aShape = occ::down_cast<TNaming_NamedShape>(myGeometries[0]);
    if (!aShape.IsNull() && !G1.IsNull())
      if (aShape->Get() == G1->Get())
        return;
  }

  Backup();
  myType          = type;
  myGeometries[0] = G1;
}

//=================================================================================================

void TDataXtd_Constraint::Set(const TDataXtd_ConstraintEnum          type,
                              const occ::handle<TNaming_NamedShape>& G1,
                              const occ::handle<TNaming_NamedShape>& G2)
{
  // OCC2932 correction
  if (myType == type)
  {
    occ::handle<TNaming_NamedShape> aShape1 = occ::down_cast<TNaming_NamedShape>(myGeometries[0]);
    occ::handle<TNaming_NamedShape> aShape2 = occ::down_cast<TNaming_NamedShape>(myGeometries[1]);
    if (!aShape1.IsNull() && !G1.IsNull() && !aShape2.IsNull()
        && !G2.IsNull())
      if (aShape1->Get() == G1->Get() && aShape2->Get() == G2->Get())
        return;
  }

  Backup();
  myType          = type;
  myGeometries[0] = G1;
  myGeometries[1] = G2;
}

//=================================================================================================

void TDataXtd_Constraint::Set(const TDataXtd_ConstraintEnum          type,
                              const occ::handle<TNaming_NamedShape>& G1,
                              const occ::handle<TNaming_NamedShape>& G2,
                              const occ::handle<TNaming_NamedShape>& G3)
{
  // OCC2932 correction
  if (myType == type)
  {
    occ::handle<TNaming_NamedShape> aShape1 = occ::down_cast<TNaming_NamedShape>(myGeometries[0]);
    occ::handle<TNaming_NamedShape> aShape2 = occ::down_cast<TNaming_NamedShape>(myGeometries[1]);
    occ::handle<TNaming_NamedShape> aShape3 = occ::down_cast<TNaming_NamedShape>(myGeometries[2]);
    if (!aShape1.IsNull() && !G1.IsNull() && !aShape2.IsNull()
        && !G2.IsNull() && !aShape3.IsNull() && !G3.IsNull())
      if (aShape1->Get() == G1->Get() && aShape2->Get() == G2->Get() && aShape3->Get() == G3->Get())
        return;
  }

  Backup();
  myType          = type;
  myGeometries[0] = G1;
  myGeometries[1] = G2;
  myGeometries[2] = G3;
}

//=================================================================================================

void TDataXtd_Constraint::Set(const TDataXtd_ConstraintEnum          type,
                              const occ::handle<TNaming_NamedShape>& G1,
                              const occ::handle<TNaming_NamedShape>& G2,
                              const occ::handle<TNaming_NamedShape>& G3,
                              const occ::handle<TNaming_NamedShape>& G4)
{
  // OCC2932 correction
  if (myType == type)
  {
    occ::handle<TNaming_NamedShape> aShape1 = occ::down_cast<TNaming_NamedShape>(myGeometries[0]);
    occ::handle<TNaming_NamedShape> aShape2 = occ::down_cast<TNaming_NamedShape>(myGeometries[1]);
    occ::handle<TNaming_NamedShape> aShape3 = occ::down_cast<TNaming_NamedShape>(myGeometries[2]);
    occ::handle<TNaming_NamedShape> aShape4 = occ::down_cast<TNaming_NamedShape>(myGeometries[3]);
    if (!aShape1.IsNull() && !G1.IsNull() && !aShape2.IsNull()
        && !G2.IsNull() && !aShape3.IsNull() && !G3.IsNull()
        && !aShape4.IsNull() && !G4.IsNull())
      if (aShape1->Get() == G1->Get() && aShape2->Get() == G2->Get() && aShape3->Get() == G3->Get()
          && aShape3->Get() == G4->Get())
        return;
  }

  Backup();
  myType          = type;
  myGeometries[0] = G1;
  myGeometries[1] = G2;
  myGeometries[2] = G3;
  myGeometries[3] = G4;
}

//=================================================================================================

void TDataXtd_Constraint::SetPlane(const occ::handle<TNaming_NamedShape>& plane)
{
  // OCC2932 correction
  if (!myPlane.IsNull() && !plane.IsNull() && myPlane->Get() == plane->Get())
    return;

  Backup();
  myPlane = plane;
}

//=================================================================================================

const occ::handle<TNaming_NamedShape>& TDataXtd_Constraint::GetPlane() const
{
  return myPlane;
}

//=================================================================================================

void TDataXtd_Constraint::SetType(const TDataXtd_ConstraintEnum CTR)
{
  // OCC2932 correction
  if (myType == CTR)
    return;

  Backup();
  myType = CTR;
}

//=================================================================================================

TDataXtd_ConstraintEnum TDataXtd_Constraint::GetType() const
{
  return myType;
}

//=================================================================================================

void TDataXtd_Constraint::ClearGeometries()
{
  // OCC2932 correction
  if (myGeometries[0].IsNull() && myGeometries[1].IsNull() && myGeometries[2].IsNull()
      && myGeometries[3].IsNull())
    return;

  Backup();
  (myGeometries[0]).Nullify();
  (myGeometries[1]).Nullify();
  (myGeometries[2]).Nullify();
  (myGeometries[3]).Nullify();
}

//=================================================================================================

void TDataXtd_Constraint::SetGeometry(const int Index, const occ::handle<TNaming_NamedShape>& G)
{
  // OCC2932 correction
  occ::handle<TNaming_NamedShape> aGeom =
    occ::down_cast<TNaming_NamedShape>(myGeometries[Index - 1]);
  if (!aGeom.IsNull() && !G.IsNull())
    if (aGeom->Get() == G->Get())
      return;

  Backup();
  myGeometries[Index - 1] = G;
}

//=================================================================================================

occ::handle<TNaming_NamedShape> TDataXtd_Constraint::GetGeometry(const int Index) const
{
  return occ::down_cast<TNaming_NamedShape>(myGeometries[Index - 1]);
}

//=================================================================================================

int TDataXtd_Constraint::NbGeometries() const
{
  int num_geom = 0;
  while (num_geom < 4 && !myGeometries[num_geom].IsNull())
  {
    num_geom += 1;
  }
  return num_geom;
}

//=================================================================================================

bool TDataXtd_Constraint::IsDimension() const
{
  return !myValue.IsNull();
}

//=================================================================================================

bool TDataXtd_Constraint::IsPlanar() const
{
  return !myPlane.IsNull();
}

//=================================================================================================

void TDataXtd_Constraint::SetValue(const occ::handle<TDataStd_Real>& V)
{
  // OCC2932 correction
  if (!myValue.IsNull() && !V.IsNull())
    if (myValue->Get() == V->Get())
      return;

  Backup();
  myValue = V;
}

//=================================================================================================

const occ::handle<TDataStd_Real>& TDataXtd_Constraint::GetValue() const
{
  return myValue;
}

//=================================================================================================

const Standard_GUID& TDataXtd_Constraint::ID() const
{
  return GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> TDataXtd_Constraint::NewEmpty() const
{
  return new TDataXtd_Constraint();
}

//=================================================================================================

void TDataXtd_Constraint::Restore(const occ::handle<TDF_Attribute>& with)
{
  occ::handle<TDataXtd_Constraint> CTR = occ::down_cast<TDataXtd_Constraint>(with);
  myGeometries[0]                      = CTR->GetGeometry(1);
  myGeometries[1]                      = CTR->GetGeometry(2);
  myGeometries[2]                      = CTR->GetGeometry(3);
  myGeometries[3]                      = CTR->GetGeometry(4);
  myType                               = CTR->GetType();
  myValue                              = CTR->GetValue();
  myIsVerified                         = CTR->Verified();
  myIsInverted                         = CTR->Inverted();
  myIsReversed                         = CTR->Reversed();
  myPlane                              = CTR->GetPlane();
}

//=================================================================================================

void TDataXtd_Constraint::Paste(const occ::handle<TDF_Attribute>&       into,
                                const occ::handle<TDF_RelocationTable>& RT) const
{
  occ::handle<TDataXtd_Constraint> CTR = occ::down_cast<TDataXtd_Constraint>(into);
  occ::handle<TNaming_NamedShape>  G1, G2, G3, G4, plane;
  RT->HasRelocation(myGeometries[0], G1);
  CTR->SetGeometry(1, G1);
  RT->HasRelocation(myGeometries[1], G2);
  CTR->SetGeometry(2, G2);
  RT->HasRelocation(myGeometries[2], G3);
  CTR->SetGeometry(3, G3);
  RT->HasRelocation(myGeometries[3], G4);
  CTR->SetGeometry(4, G4);

  RT->HasRelocation(myPlane, plane);
  CTR->SetPlane(plane);

  occ::handle<TDataStd_Real> Value;
  RT->HasRelocation(myValue, Value);
  CTR->SetValue(Value);

  CTR->SetType(myType);
  CTR->Verified(Verified());
  CTR->Inverted(Inverted());
  CTR->Reversed(Reversed());
}

//=================================================================================================

void TDataXtd_Constraint::References(const occ::handle<TDF_DataSet>& DS) const

{
  // bidouille en attendant traitement des contraintes d assemblage en dehors de la part
  //  l attribut placement devrait oriente vers les contraintes de placement en dehors
  int Lim;
  if (myType >= TDataXtd_MATE && myType <= TDataXtd_FACES_ANGLE)
    Lim = 1;
  else
    Lim = 3;

  for (int i = 0; i <= Lim; i++)
  {
    if (!myGeometries[i].IsNull())
      DS->AddAttribute(myGeometries[i]);
  }

  if (!myValue.IsNull())
    DS->AddAttribute(myValue);
  if (!myPlane.IsNull())
    DS->AddAttribute(myPlane);
}

//=================================================================================================

void TDataXtd_Constraint::Verified(const bool status)
{
  // OCC2932 correction
  if (myIsVerified == status)
    return;

  Backup();
  myIsVerified = status;
}

//=================================================================================================

bool TDataXtd_Constraint::Verified() const
{
  return myIsVerified;
}

//=================================================================================================

void TDataXtd_Constraint::Reversed(const bool status)
{
  // OCC2932 correction
  if (myIsReversed == status)
    return;

  Backup();
  myIsReversed = status;
}

//=================================================================================================

bool TDataXtd_Constraint::Reversed() const
{
  return myIsReversed;
}

//=================================================================================================

void TDataXtd_Constraint::Inverted(const bool status)
{
  // OCC2932 correction
  if (myIsInverted == status)
    return;

  Backup();
  myIsInverted = status;
}

//=================================================================================================

bool TDataXtd_Constraint::Inverted() const
{
  return myIsInverted;
}

//=================================================================================================

void TDataXtd_Constraint::CollectChildConstraints(const TDF_Label&             aLabel,
                                                  NCollection_List<TDF_Label>& LL)
{
  TDF_ChildIterator                it(aLabel, true);
  occ::handle<TDataXtd_Constraint> aConstraint;
  for (; it.More(); it.Next())
  {
    if (it.Value().FindAttribute(TDataXtd_Constraint::GetID(), aConstraint))
    {
      LL.Append(it.Value());
    }
  }
}

//=================================================================================================

Standard_OStream& TDataXtd_Constraint::Dump(Standard_OStream& anOS) const
{
  anOS << "Constraint ";
  TDataXtd::Print(GetType(), anOS);
  return anOS;
}
