// Created on: 1993-03-09
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// 14-Mar-96 : xab  portage hp
// pmn : 28-Jun-96 Distinction entre la continuite en U et V (bug PRO4625)
// pmn : 07-Jan-97 Centralisation des verif rational (PRO6834)
//       et ajout des InvalideCache() dans les SetWeight*(PRO6833)
// RBD : 15-10-98 ; Le cache est maintenant calcule sur [-1,1] (pro15537).
// jct : 19-01-99 ; permutation de urational et vrational dans Rational.

#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_BSplineSurface, Geom_BoundedSurface)

//=================================================================================================

static void CheckSurfaceData(const NCollection_Array2<gp_Pnt>& SPoles,
                             const NCollection_Array1<double>& SUKnots,
                             const NCollection_Array1<double>& SVKnots,
                             const NCollection_Array1<int>&    SUMults,
                             const NCollection_Array1<int>&    SVMults,
                             const int                         UDegree,
                             const int                         VDegree,
                             const bool                        UPeriodic,
                             const bool                        VPeriodic)
{
  if (UDegree < 1 || UDegree > Geom_BSplineSurface::MaxDegree() || VDegree < 1
      || VDegree > Geom_BSplineSurface::MaxDegree())
  {
    throw Standard_ConstructionError("Geom_BSplineSurface: invalid degree");
  }
  if (SPoles.ColLength() < 2 || SPoles.RowLength() < 2)
  {
    throw Standard_ConstructionError("Geom_BSplineSurface: at least 2 poles required");
  }

  if (SUKnots.Length() != SUMults.Length() || SVKnots.Length() != SVMults.Length())
  {
    throw Standard_ConstructionError("Geom_BSplineSurface: Knot and Mult array size mismatch");
  }

  int i;
  for (i = SUKnots.Lower(); i < SUKnots.Upper(); i++)
  {
    if (SUKnots(i + 1) - SUKnots(i) <= Epsilon(std::abs(SUKnots(i))))
    {
      throw Standard_ConstructionError("Geom_BSplineSurface: UKnots interval values too close");
    }
  }

  for (i = SVKnots.Lower(); i < SVKnots.Upper(); i++)
  {
    if (SVKnots(i + 1) - SVKnots(i) <= Epsilon(std::abs(SVKnots(i))))
    {
      throw Standard_ConstructionError("Geom_BSplineSurface: VKnots interval values too close");
    }
  }

  if (SPoles.ColLength() != BSplCLib::NbPoles(UDegree, UPeriodic, SUMults))
    throw Standard_ConstructionError("Geom_BSplineSurface: # U Poles and degree mismatch");

  if (SPoles.RowLength() != BSplCLib::NbPoles(VDegree, VPeriodic, SVMults))
    throw Standard_ConstructionError("Geom_BSplineSurface: # V Poles and degree mismatch");
}

//=================================================================================================

static void Rational(const NCollection_Array2<double>& Weights, bool& Urational, bool& Vrational)
{
  int I, J;
  J         = Weights.LowerCol();
  Vrational = false;
  while (!Vrational && J <= Weights.UpperCol())
  {
    I = Weights.LowerRow();
    while (!Vrational && I <= Weights.UpperRow() - 1)
    {
      Vrational = (std::abs(Weights(I, J) - Weights(I + 1, J)) > Epsilon(std::abs(Weights(I, J))));
      I++;
    }
    J++;
  }

  I         = Weights.LowerRow();
  Urational = false;
  while (!Urational && I <= Weights.UpperRow())
  {
    J = Weights.LowerCol();
    while (!Urational && J <= Weights.UpperCol() - 1)
    {
      Urational = (std::abs(Weights(I, J) - Weights(I, J + 1)) > Epsilon(std::abs(Weights(I, J))));
      J++;
    }
    I++;
  }
}

//=================================================================================================

occ::handle<Geom_Geometry> Geom_BSplineSurface::Copy() const
{
  return new Geom_BSplineSurface(*this);
}

//=================================================================================================

Geom_BSplineSurface::Geom_BSplineSurface(const Geom_BSplineSurface& theOther)
    : myData(theOther.myData),
      urational(theOther.urational),
      vrational(theOther.vrational),
      uknotSet(theOther.uknotSet),
      vknotSet(theOther.vknotSet),
      Usmooth(theOther.Usmooth),
      Vsmooth(theOther.Vsmooth),
      umaxderivinv(theOther.umaxderivinv),
      vmaxderivinv(theOther.vmaxderivinv),
      maxderivinvok(false)
{
}

//=================================================================================================

Geom_BSplineSurface::Geom_BSplineSurface(const NCollection_Array2<gp_Pnt>& Poles,
                                         const NCollection_Array1<double>& UKnots,
                                         const NCollection_Array1<double>& VKnots,
                                         const NCollection_Array1<int>&    UMults,
                                         const NCollection_Array1<int>&    VMults,
                                         const int                         UDegree,
                                         const int                         VDegree,
                                         const bool                        UPeriodic,
                                         const bool                        VPeriodic)
    : urational(false),
      vrational(false),
      maxderivinvok(false)

{

  // check

  CheckSurfaceData(Poles, UKnots, VKnots, UMults, VMults, UDegree, VDegree, UPeriodic, VPeriodic);

  // set scalar fields
  myData.UDegree     = UDegree;
  myData.VDegree     = VDegree;
  myData.IsUPeriodic = UPeriodic;
  myData.IsVPeriodic = VPeriodic;

  // copy arrays

  myData.Poles.Resize(1, Poles.ColLength(), 1, Poles.RowLength(), false);
  myData.Poles.Assign(Poles);

  myData.Weights.Resize(1, Poles.ColLength(), 1, Poles.RowLength(), false);
  myData.Weights.Init(1.0);

  myData.UKnots.Resize(1, UKnots.Length(), false);
  myData.UKnots.Assign(UKnots);

  myData.UMults.Resize(1, UMults.Length(), false);
  myData.UMults.Assign(UMults);

  myData.VKnots.Resize(1, VKnots.Length(), false);
  myData.VKnots.Assign(VKnots);

  myData.VMults.Resize(1, VMults.Length(), false);
  myData.VMults.Assign(VMults);

  UpdateUKnots();
  UpdateVKnots();
}

//=================================================================================================

Geom_BSplineSurface::Geom_BSplineSurface(const NCollection_Array2<gp_Pnt>& Poles,
                                         const NCollection_Array2<double>& Weights,
                                         const NCollection_Array1<double>& UKnots,
                                         const NCollection_Array1<double>& VKnots,
                                         const NCollection_Array1<int>&    UMults,
                                         const NCollection_Array1<int>&    VMults,
                                         const int                         UDegree,
                                         const int                         VDegree,
                                         const bool                        UPeriodic,
                                         const bool                        VPeriodic)
    : urational(false),
      vrational(false),
      maxderivinvok(false)
{
  // check weights

  if (Weights.ColLength() != Poles.ColLength())
    throw Standard_ConstructionError(
      "Geom_BSplineSurface: U Weights and Poles array size mismatch");

  if (Weights.RowLength() != Poles.RowLength())
    throw Standard_ConstructionError(
      "Geom_BSplineSurface: V Weights and Poles array size mismatch");

  int i, j;
  for (i = Weights.LowerRow(); i <= Weights.UpperRow(); i++)
  {
    for (j = Weights.LowerCol(); j <= Weights.UpperCol(); j++)
    {
      if (Weights(i, j) <= gp::Resolution())
        throw Standard_ConstructionError("Geom_BSplineSurface: Weights values too small");
    }
  }

  // check really rational

  Rational(Weights, urational, vrational);

  // check

  CheckSurfaceData(Poles, UKnots, VKnots, UMults, VMults, UDegree, VDegree, UPeriodic, VPeriodic);

  // set scalar fields
  myData.UDegree     = UDegree;
  myData.VDegree     = VDegree;
  myData.IsUPeriodic = UPeriodic;
  myData.IsVPeriodic = VPeriodic;

  // copy arrays

  myData.Poles.Resize(1, Poles.ColLength(), 1, Poles.RowLength(), false);
  myData.Poles.Assign(Poles);

  myData.Weights.Resize(1, Poles.ColLength(), 1, Poles.RowLength(), false);
  myData.Weights.Assign(Weights);

  myData.UKnots.Resize(1, UKnots.Length(), false);
  myData.UKnots.Assign(UKnots);

  myData.UMults.Resize(1, UMults.Length(), false);
  myData.UMults.Assign(UMults);

  myData.VKnots.Resize(1, VKnots.Length(), false);
  myData.VKnots.Assign(VKnots);

  myData.VMults.Resize(1, VMults.Length(), false);
  myData.VMults.Assign(VMults);

  UpdateUKnots();
  UpdateVKnots();
}

//=================================================================================================

void Geom_BSplineSurface::ExchangeUV()
{
  int LC = myData.Poles.LowerCol();
  int UC = myData.Poles.UpperCol();
  int LR = myData.Poles.LowerRow();
  int UR = myData.Poles.UpperRow();

  NCollection_Array2<gp_Pnt> npoles(LC, UC, LR, UR);
  NCollection_Array2<double> nweights;
  if (urational || vrational)
  {
    nweights = NCollection_Array2<double>(LC, UC, LR, UR);
  }

  for (int i = LC; i <= UC; i++)
  {
    for (int j = LR; j <= UR; j++)
    {
      npoles(i, j) = myData.Poles(j, i);
      if (urational || vrational)
      {
        nweights.ChangeValue(i, j) = myData.Weights.Value(j, i);
      }
    }
  }
  myData.Poles = std::move(npoles);
  if (urational || vrational)
    myData.Weights = std::move(nweights);

  std::swap(urational, vrational);
  std::swap(myData.IsUPeriodic, myData.IsVPeriodic);
  std::swap(myData.UDegree, myData.VDegree);
  std::swap(myData.UKnots, myData.VKnots);
  std::swap(myData.UMults, myData.VMults);

  UpdateUKnots();
  UpdateVKnots();
}

//=================================================================================================

void Geom_BSplineSurface::IncreaseDegree(const int UDegree, const int VDegree)
{
  if (UDegree != myData.UDegree)
  {
    if (UDegree < myData.UDegree || UDegree > Geom_BSplineSurface::MaxDegree())
      throw Standard_ConstructionError("Geom_BSplineSurface::IncreaseDegree: bad U degree value");

    int FromK1 = FirstUKnotIndex();
    int ToK2   = LastUKnotIndex();

    int Step = UDegree - myData.UDegree;

    NCollection_Array2<gp_Pnt> npoles(1,
                                      myData.Poles.ColLength() + Step * (ToK2 - FromK1),
                                      1,
                                      myData.Poles.RowLength());

    int nbknots = BSplCLib::IncreaseDegreeCountKnots(myData.UDegree,
                                                     UDegree,
                                                     myData.IsUPeriodic,
                                                     myData.UMults);

    NCollection_Array1<double> nknots(1, nbknots);

    NCollection_Array1<int> nmults(1, nbknots);

    NCollection_Array2<double> nweights(1, npoles.ColLength(), 1, npoles.RowLength(), 1.);

    if (urational || vrational)
    {

      BSplSLib::IncreaseDegree(true,
                               myData.UDegree,
                               UDegree,
                               myData.IsUPeriodic,
                               myData.Poles,
                               &myData.Weights,
                               myData.UKnots,
                               myData.UMults,
                               npoles,
                               &nweights,
                               nknots,
                               nmults);
    }
    else
    {

      BSplSLib::IncreaseDegree(true,
                               myData.UDegree,
                               UDegree,
                               myData.IsUPeriodic,
                               myData.Poles,
                               BSplSLib::NoWeights(),
                               myData.UKnots,
                               myData.UMults,
                               npoles,
                               BSplSLib::NoWeights(),
                               nknots,
                               nmults);
    }
    myData.UDegree = UDegree;
    myData.Poles   = std::move(npoles);
    myData.Weights = std::move(nweights);
    myData.UKnots  = std::move(nknots);
    myData.UMults  = std::move(nmults);
    UpdateUKnots();
  }

  if (VDegree != myData.VDegree)
  {
    if (VDegree < myData.VDegree || VDegree > Geom_BSplineSurface::MaxDegree())
      throw Standard_ConstructionError("Geom_BSplineSurface::IncreaseDegree: bad V degree value");

    int FromK1 = FirstVKnotIndex();
    int ToK2   = LastVKnotIndex();

    int Step = VDegree - myData.VDegree;

    NCollection_Array2<gp_Pnt> npoles(1,
                                      myData.Poles.ColLength(),
                                      1,
                                      myData.Poles.RowLength() + Step * (ToK2 - FromK1));

    int nbknots = BSplCLib::IncreaseDegreeCountKnots(myData.VDegree,
                                                     VDegree,
                                                     myData.IsVPeriodic,
                                                     myData.VMults);

    NCollection_Array1<double> nknots(1, nbknots);

    NCollection_Array1<int> nmults(1, nbknots);

    NCollection_Array2<double> nweights(1, npoles.ColLength(), 1, npoles.RowLength(), 1.);

    if (urational || vrational)
    {

      BSplSLib::IncreaseDegree(false,
                               myData.VDegree,
                               VDegree,
                               myData.IsVPeriodic,
                               myData.Poles,
                               &myData.Weights,
                               myData.VKnots,
                               myData.VMults,
                               npoles,
                               &nweights,
                               nknots,
                               nmults);
    }
    else
    {

      BSplSLib::IncreaseDegree(false,
                               myData.VDegree,
                               VDegree,
                               myData.IsVPeriodic,
                               myData.Poles,
                               BSplSLib::NoWeights(),
                               myData.VKnots,
                               myData.VMults,
                               npoles,
                               BSplSLib::NoWeights(),
                               nknots,
                               nmults);
    }
    myData.VDegree = VDegree;
    myData.Poles   = std::move(npoles);
    myData.Weights = std::move(nweights);
    myData.VKnots  = std::move(nknots);
    myData.VMults  = std::move(nmults);
    UpdateVKnots();
  }
}

//=================================================================================================

void Geom_BSplineSurface::IncreaseUMultiplicity(const int UIndex, const int M)
{
  NCollection_Array1<double> k(1, 1);
  k(1) = myData.UKnots.Value(UIndex);
  NCollection_Array1<int> m(1, 1);
  m(1) = M - myData.UMults.Value(UIndex);
  InsertUKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom_BSplineSurface::IncreaseUMultiplicity(const int FromI1, const int ToI2, const int M)
{
  NCollection_Array1<double> k(myData.UKnots(FromI1), FromI1, ToI2);
  NCollection_Array1<int>    m(FromI1, ToI2);
  for (int i = FromI1; i <= ToI2; i++)
    m(i) = M - myData.UMults.Value(i);
  InsertUKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom_BSplineSurface::IncreaseVMultiplicity(const int VIndex, const int M)
{
  NCollection_Array1<double> k(1, 1);
  k(1) = myData.VKnots.Value(VIndex);
  NCollection_Array1<int> m(1, 1);
  m(1) = M - myData.VMults.Value(VIndex);
  InsertVKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom_BSplineSurface::IncreaseVMultiplicity(const int FromI1, const int ToI2, const int M)
{
  NCollection_Array1<double> k(myData.VKnots(FromI1), FromI1, ToI2);
  NCollection_Array1<int>    m(FromI1, ToI2);
  for (int i = FromI1; i <= ToI2; i++)
    m(i) = M - myData.VMults.Value(i);
  InsertVKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom_BSplineSurface::segment(const double U1,
                                  const double U2,
                                  const double V1,
                                  const double V2,
                                  const double EpsU,
                                  const double EpsV,
                                  const bool   SegmentInU,
                                  const bool   SegmentInV)
{
  double deltaU = U2 - U1;
  if (myData.IsUPeriodic)
  {
    double aUPeriod = myData.UKnots.Last() - myData.UKnots.First();
    if (deltaU - aUPeriod > Precision::PConfusion())
      throw Standard_DomainError("Geom_BSplineSurface::Segment");
    if (deltaU > aUPeriod)
      deltaU = aUPeriod;
  }

  double deltaV = V2 - V1;
  if (myData.IsVPeriodic)
  {
    double aVPeriod = myData.VKnots.Last() - myData.VKnots.First();
    if (deltaV - aVPeriod > Precision::PConfusion())
      throw Standard_DomainError("Geom_BSplineSurface::Segment");
    if (deltaV > aVPeriod)
      deltaV = aVPeriod;
  }

  double NewU1, NewU2, NewV1, NewV2;
  double U, V;
  int    indexU, indexV;

  indexU = 0;
  BSplCLib::LocateParameter(myData.UDegree,
                            myData.UKnots,
                            myData.UMults,
                            U1,
                            myData.IsUPeriodic,
                            myData.UKnots.Lower(),
                            myData.UKnots.Upper(),
                            indexU,
                            NewU1);
  indexU = 0;
  BSplCLib::LocateParameter(myData.UDegree,
                            myData.UKnots,
                            myData.UMults,
                            U2,
                            myData.IsUPeriodic,
                            myData.UKnots.Lower(),
                            myData.UKnots.Upper(),
                            indexU,
                            NewU2);
  if (SegmentInU)
  {
    // inserting the UKnots
    NCollection_Array1<double> UKnots(1, 2);
    NCollection_Array1<int>    UMults(1, 2);
    UKnots(1) = std::min(NewU1, NewU2);
    UKnots(2) = std::max(NewU1, NewU2);
    UMults(1) = UMults(2) = myData.UDegree;

    InsertUKnots(UKnots, UMults, EpsU);
  }

  indexV = 0;
  BSplCLib::LocateParameter(myData.VDegree,
                            myData.VKnots,
                            myData.VMults,
                            V1,
                            myData.IsVPeriodic,
                            myData.VKnots.Lower(),
                            myData.VKnots.Upper(),
                            indexV,
                            NewV1);
  indexV = 0;
  BSplCLib::LocateParameter(myData.VDegree,
                            myData.VKnots,
                            myData.VMults,
                            V2,
                            myData.IsVPeriodic,
                            myData.VKnots.Lower(),
                            myData.VKnots.Upper(),
                            indexV,
                            NewV2);
  if (SegmentInV)
  {
    // Inserting the VKnots
    NCollection_Array1<double> VKnots(1, 2);
    NCollection_Array1<int>    VMults(1, 2);

    VKnots(1) = std::min(NewV1, NewV2);
    VKnots(2) = std::max(NewV1, NewV2);
    VMults(1) = VMults(2) = myData.VDegree;
    InsertVKnots(VKnots, VMults, EpsV);
  }

  if (myData.IsUPeriodic && SegmentInU)
  { // set the origine at NewU1
    int index = 0;
    BSplCLib::LocateParameter(myData.UDegree,
                              myData.UKnots,
                              myData.UMults,
                              U1,
                              myData.IsUPeriodic,
                              myData.UKnots.Lower(),
                              myData.UKnots.Upper(),
                              index,
                              U);
    if (std::abs(myData.UKnots.Value(index + 1) - U) <= EpsU)
      index++;
    SetUOrigin(index);
    SetUNotPeriodic();
  }

  // compute index1 and index2 to set the new knots and mults
  int index1U = 0, index2U = 0;
  int FromU1 = myData.UKnots.Lower();
  int ToU2   = myData.UKnots.Upper();
  BSplCLib::LocateParameter(myData.UDegree,
                            myData.UKnots,
                            myData.UMults,
                            NewU1,
                            myData.IsUPeriodic,
                            FromU1,
                            ToU2,
                            index1U,
                            U);
  if (std::abs(myData.UKnots.Value(index1U + 1) - U) <= EpsU)
    index1U++;
  BSplCLib::LocateParameter(myData.UDegree,
                            myData.UKnots,
                            myData.UMults,
                            NewU1 + deltaU,
                            myData.IsUPeriodic,
                            FromU1,
                            ToU2,
                            index2U,
                            U);
  if (std::abs(myData.UKnots.Value(index2U + 1) - U) <= EpsU || index2U == index1U)
    index2U++;

  int nbuknots = index2U - index1U + 1;

  NCollection_Array1<double> nuknots(1, nbuknots);
  NCollection_Array1<int>    numults(1, nbuknots);

  int i, k = 1;
  for (i = index1U; i <= index2U; i++)
  {
    nuknots.SetValue(k, myData.UKnots.Value(i));
    numults.SetValue(k, myData.UMults.Value(i));
    k++;
  }
  if (SegmentInU)
  {
    numults.SetValue(1, myData.UDegree + 1);
    numults.SetValue(nbuknots, myData.UDegree + 1);
  }

  if (myData.IsVPeriodic && SegmentInV)
  { // set the origine at NewV1
    int index = 0;
    BSplCLib::LocateParameter(myData.VDegree,
                              myData.VKnots,
                              myData.VMults,
                              V1,
                              myData.IsVPeriodic,
                              myData.VKnots.Lower(),
                              myData.VKnots.Upper(),
                              index,
                              V);
    if (std::abs(myData.VKnots.Value(index + 1) - V) <= EpsV)
      index++;
    SetVOrigin(index);
    SetVNotPeriodic();
  }

  // compute index1 and index2 to set the new knots and mults
  int index1V = 0, index2V = 0;
  int FromV1 = myData.VKnots.Lower();
  int ToV2   = myData.VKnots.Upper();
  BSplCLib::LocateParameter(myData.VDegree,
                            myData.VKnots,
                            myData.VMults,
                            NewV1,
                            myData.IsVPeriodic,
                            FromV1,
                            ToV2,
                            index1V,
                            V);
  if (std::abs(myData.VKnots.Value(index1V + 1) - V) <= EpsV)
    index1V++;
  BSplCLib::LocateParameter(myData.VDegree,
                            myData.VKnots,
                            myData.VMults,
                            NewV1 + deltaV,
                            myData.IsVPeriodic,
                            FromV1,
                            ToV2,
                            index2V,
                            V);
  if (std::abs(myData.VKnots.Value(index2V + 1) - V) <= EpsV || index2V == index1V)
    index2V++;

  int nbvknots = index2V - index1V + 1;

  NCollection_Array1<double> nvknots(1, nbvknots);
  NCollection_Array1<int>    nvmults(1, nbvknots);

  k = 1;
  for (i = index1V; i <= index2V; i++)
  {
    nvknots.SetValue(k, myData.VKnots.Value(i));
    nvmults.SetValue(k, myData.VMults.Value(i));
    k++;
  }
  if (SegmentInV)
  {
    nvmults.SetValue(1, myData.VDegree + 1);
    nvmults.SetValue(nbvknots, myData.VDegree + 1);
  }

  // compute index1 and index2 to set the new poles and weights
  int pindex1U = BSplCLib::PoleIndex(myData.UDegree, index1U, myData.IsUPeriodic, myData.UMults);
  int pindex2U = BSplCLib::PoleIndex(myData.UDegree, index2U, myData.IsUPeriodic, myData.UMults);

  pindex1U++;
  pindex2U = std::min(pindex2U + 1, myData.Poles.ColLength());

  int nbupoles = pindex2U - pindex1U + 1;

  // compute index1 and index2 to set the new poles and weights
  int pindex1V = BSplCLib::PoleIndex(myData.VDegree, index1V, myData.IsVPeriodic, myData.VMults);
  int pindex2V = BSplCLib::PoleIndex(myData.VDegree, index2V, myData.IsVPeriodic, myData.VMults);

  pindex1V++;
  pindex2V = std::min(pindex2V + 1, myData.Poles.RowLength());

  int nbvpoles = pindex2V - pindex1V + 1;

  NCollection_Array2<gp_Pnt> npoles(1, nbupoles, 1, nbvpoles);
  k = 1;
  int j, l;
  if (urational || vrational)
  {
    NCollection_Array2<double> nweights(1, nbupoles, 1, nbvpoles);
    for (i = pindex1U; i <= pindex2U; i++)
    {
      l = 1;
      for (j = pindex1V; j <= pindex2V; j++)
      {
        npoles.SetValue(k, l, myData.Poles.Value(i, j));
        nweights.SetValue(k, l, myData.Weights.Value(i, j));
        l++;
      }
      k++;
    }
    myData.Weights = std::move(nweights);
  }
  else
  {
    for (i = pindex1U; i <= pindex2U; i++)
    {
      l = 1;
      for (j = pindex1V; j <= pindex2V; j++)
      {
        npoles.SetValue(k, l, myData.Poles.Value(i, j));
        l++;
      }
      k++;
    }
    myData.Weights =
      NCollection_Array2<double>(1, npoles.ColLength(), 1, npoles.RowLength(), 1.0);
  }

  myData.UKnots = std::move(nuknots);
  myData.UMults = std::move(numults);
  myData.VKnots = std::move(nvknots);
  myData.VMults = std::move(nvmults);
  myData.Poles  = std::move(npoles);

  maxderivinvok = false;
  UpdateUKnots();
  UpdateVKnots();
}

//=================================================================================================

void Geom_BSplineSurface::Segment(const double U1,
                                  const double U2,
                                  const double V1,
                                  const double V2,
                                  const double theUTolerance,
                                  const double theVTolerance)
{
  if ((U2 < U1) || (V2 < V1))
    throw Standard_DomainError("Geom_BSplineSurface::Segment");

  double aMaxU = std::max(std::abs(U2), std::abs(U1));
  double EpsU  = std::max(Epsilon(aMaxU), theUTolerance);

  double aMaxV = std::max(std::abs(V2), std::abs(V1));
  double EpsV  = std::max(Epsilon(aMaxV), theVTolerance);

  segment(U1, U2, V1, V2, EpsU, EpsV, true, true);
}

//=================================================================================================

void Geom_BSplineSurface::CheckAndSegment(const double U1,
                                          const double U2,
                                          const double V1,
                                          const double V2,
                                          const double theUTolerance,
                                          const double theVTolerance)
{

  if ((U2 < U1) || (V2 < V1))
    throw Standard_DomainError("Geom_BSplineSurface::CheckAndSegment");

  double aMaxU = std::max(std::abs(U2), std::abs(U1));
  double EpsU  = std::max(Epsilon(aMaxU), theUTolerance);

  double aMaxV = std::max(std::abs(V2), std::abs(V1));
  double EpsV  = std::max(Epsilon(aMaxV), theVTolerance);

  bool segment_in_U = true;
  bool segment_in_V = true;
  segment_in_U      = (std::abs(U1 - myData.UKnots.Value(myData.UKnots.Lower())) > EpsU)
                 || (std::abs(U2 - myData.UKnots.Value(myData.UKnots.Upper())) > EpsU);
  segment_in_V = (std::abs(V1 - myData.VKnots.Value(myData.VKnots.Lower())) > EpsV)
                 || (std::abs(V2 - myData.VKnots.Value(myData.VKnots.Upper())) > EpsV);

  segment(U1, U2, V1, V2, EpsU, EpsV, segment_in_U, segment_in_V);
}

//=================================================================================================

void Geom_BSplineSurface::SetUKnot(const int UIndex, const double K)
{
  if (UIndex < 1 || UIndex > myData.UKnots.Length())
    throw Standard_OutOfRange("Geom_BSplineSurface::SetUKnot: Index and #knots mismatch");

  int    NewIndex = UIndex;
  double DU       = std::abs(Epsilon(K));
  if (UIndex == 1)
  {
    if (K >= myData.UKnots.Value(2) - DU)
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnot: K out of range");
  }
  else if (UIndex == myData.UKnots.Length())
  {
    if (K <= myData.UKnots.Value(myData.UKnots.Length() - 1) + DU)
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnot: K out of range");
    }
  }
  else
  {
    if (K <= myData.UKnots.Value(NewIndex - 1) + DU
        || K >= myData.UKnots.Value(NewIndex + 1) - DU)
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnot: K out of range");
    }
  }

  if (K != myData.UKnots.Value(NewIndex))
  {
    myData.UKnots.SetValue(NewIndex, K);
    maxderivinvok = false;
    UpdateUKnots();
  }
}

//=================================================================================================

void Geom_BSplineSurface::SetUKnots(const NCollection_Array1<double>& UK)
{

  int Lower = UK.Lower();
  int Upper = UK.Upper();
  if (Lower < 1 || Lower > myData.UKnots.Length() || Upper < 1
      || Upper > myData.UKnots.Length())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetUKnots: invalid array dimension");
  }
  if (Lower > 1)
  {
    if (std::abs(UK(Lower) - myData.UKnots.Value(Lower - 1)) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnots: invalid knot value");
    }
  }
  if (Upper < myData.UKnots.Length())
  {
    if (std::abs(UK(Upper) - myData.UKnots.Value(Upper + 1)) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnots: invalid knot value");
    }
  }
  double K1 = UK(Lower);
  for (int i = Lower; i <= Upper; i++)
  {
    myData.UKnots.SetValue(i, UK(i));
    if (i != Lower)
    {
      if (std::abs(UK(i) - K1) <= gp::Resolution())
      {
        throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnots: invalid knot value");
      }
      K1 = UK(i);
    }
  }

  maxderivinvok = false;
  UpdateUKnots();
}

//=================================================================================================

void Geom_BSplineSurface::SetUKnot(const int UIndex, const double K, const int M)
{
  IncreaseUMultiplicity(UIndex, M);
  SetUKnot(UIndex, K);
}

//=================================================================================================

void Geom_BSplineSurface::SetVKnot(const int VIndex, const double K)
{
  if (VIndex < 1 || VIndex > myData.VKnots.Length())
    throw Standard_OutOfRange("Geom_BSplineSurface::SetVKnot: Index and #knots mismatch");
  int    NewIndex = VIndex + myData.VKnots.Lower() - 1;
  double DV       = std::abs(Epsilon(K));
  if (VIndex == 1)
  {
    if (K >= myData.VKnots.Value(2) - DV)
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnot: K out of range");
    }
  }
  else if (VIndex == myData.VKnots.Length())
  {
    if (K <= myData.VKnots.Value(myData.VKnots.Length() - 1) + DV)
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnot: K out of range");
    }
  }
  else
  {
    if (K <= myData.VKnots.Value(NewIndex - 1) + DV
        || K >= myData.VKnots.Value(NewIndex + 1) - DV)
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnot: K out of range");
    }
  }

  if (K != myData.VKnots.Value(NewIndex))
  {
    myData.VKnots.SetValue(NewIndex, K);
    maxderivinvok = false;
    UpdateVKnots();
  }
}

//=================================================================================================

void Geom_BSplineSurface::SetVKnots(const NCollection_Array1<double>& VK)
{

  int Lower = VK.Lower();
  int Upper = VK.Upper();
  if (Lower < 1 || Lower > myData.VKnots.Length() || Upper < 1
      || Upper > myData.VKnots.Length())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetVKnots: invalid array dimension");
  }
  if (Lower > 1)
  {
    if (std::abs(VK(Lower) - myData.VKnots.Value(Lower - 1)) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnots: invalid knot value");
    }
  }
  if (Upper < myData.VKnots.Length())
  {
    if (std::abs(VK(Upper) - myData.VKnots.Value(Upper + 1)) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnots: invalid knot value");
    }
  }
  double K1 = VK(Lower);
  for (int i = Lower; i <= Upper; i++)
  {
    myData.VKnots.SetValue(i, VK(i));
    if (i != Lower)
    {
      if (std::abs(VK(i) - K1) <= gp::Resolution())
      {
        throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnots: invalid knot value");
      }
      K1 = VK(i);
    }
  }

  maxderivinvok = false;
  UpdateVKnots();
}

//=================================================================================================

void Geom_BSplineSurface::SetVKnot(const int VIndex, const double K, const int M)
{
  IncreaseVMultiplicity(VIndex, M);
  SetVKnot(VIndex, K);
}

//=================================================================================================

void Geom_BSplineSurface::InsertUKnot(const double U,
                                      const int    M,
                                      const double ParametricTolerance,
                                      const bool   Add)
{
  NCollection_Array1<double> k(1, 1);
  k(1) = U;
  NCollection_Array1<int> m(1, 1);
  m(1) = M;
  InsertUKnots(k, m, ParametricTolerance, Add);
}

//=================================================================================================

void Geom_BSplineSurface::InsertVKnot(const double V,
                                      const int    M,
                                      const double ParametricTolerance,
                                      const bool   Add)
{
  NCollection_Array1<double> k(1, 1);
  k(1) = V;
  NCollection_Array1<int> m(1, 1);
  m(1) = M;
  InsertVKnots(k, m, ParametricTolerance, Add);
}

//=================================================================================================

void Geom_BSplineSurface::IncrementUMultiplicity(const int FromI1, const int ToI2, const int Step)
{
  NCollection_Array1<double> k(myData.UKnots(FromI1), FromI1, ToI2);
  NCollection_Array1<int>    m(FromI1, ToI2);
  m.Init(Step);
  InsertUKnots(k, m, Epsilon(1.));
}

//=================================================================================================

void Geom_BSplineSurface::IncrementVMultiplicity(const int FromI1, const int ToI2, const int Step)
{
  NCollection_Array1<double> k(myData.VKnots(FromI1), FromI1, ToI2);

  NCollection_Array1<int> m(FromI1, ToI2);
  m.Init(Step);

  InsertVKnots(k, m, Epsilon(1.));
}

//=================================================================================================

void Geom_BSplineSurface::UpdateUKnots()
{

  int MaxKnotMult = 0;
  BSplCLib::KnotAnalysis(myData.UDegree,
                         myData.IsUPeriodic,
                         myData.UKnots,
                         myData.UMults,
                         uknotSet,
                         MaxKnotMult);

  if (uknotSet == GeomAbs_Uniform && !myData.IsUPeriodic)
  {
    myData.UFlatKnots.Resize(myData.UKnots.Lower(), myData.UKnots.Upper(), false);
    myData.UFlatKnots.Assign(myData.UKnots);
  }
  else
  {
    myData.UFlatKnots.Resize(
      1,
      BSplCLib::KnotSequenceLength(myData.UMults, myData.UDegree, myData.IsUPeriodic),
      false);

    BSplCLib::KnotSequence(myData.UKnots,
                           myData.UMults,
                           myData.UDegree,
                           myData.IsUPeriodic,
                           myData.UFlatKnots);
  }

  if (MaxKnotMult == 0)
    Usmooth = GeomAbs_CN;
  else
  {
    switch (myData.UDegree - MaxKnotMult)
    {
      case 0:
        Usmooth = GeomAbs_C0;
        break;
      case 1:
        Usmooth = GeomAbs_C1;
        break;
      case 2:
        Usmooth = GeomAbs_C2;
        break;
      case 3:
        Usmooth = GeomAbs_C3;
        break;
      default:
        Usmooth = GeomAbs_C3;
        break;
    }
  }
}

//=================================================================================================

void Geom_BSplineSurface::UpdateVKnots()
{
  int MaxKnotMult = 0;
  BSplCLib::KnotAnalysis(myData.VDegree,
                         myData.IsVPeriodic,
                         myData.VKnots,
                         myData.VMults,
                         vknotSet,
                         MaxKnotMult);

  if (vknotSet == GeomAbs_Uniform && !myData.IsVPeriodic)
  {
    myData.VFlatKnots.Resize(myData.VKnots.Lower(), myData.VKnots.Upper(), false);
    myData.VFlatKnots.Assign(myData.VKnots);
  }
  else
  {
    myData.VFlatKnots.Resize(
      1,
      BSplCLib::KnotSequenceLength(myData.VMults, myData.VDegree, myData.IsVPeriodic),
      false);

    BSplCLib::KnotSequence(myData.VKnots,
                           myData.VMults,
                           myData.VDegree,
                           myData.IsVPeriodic,
                           myData.VFlatKnots);
  }

  if (MaxKnotMult == 0)
    Vsmooth = GeomAbs_CN;
  else
  {
    switch (myData.VDegree - MaxKnotMult)
    {
      case 0:
        Vsmooth = GeomAbs_C0;
        break;
      case 1:
        Vsmooth = GeomAbs_C1;
        break;
      case 2:
        Vsmooth = GeomAbs_C2;
        break;
      case 3:
        Vsmooth = GeomAbs_C3;
        break;
      default:
        Vsmooth = GeomAbs_C3;
        break;
    }
  }
}

//=================================================================================================

void Geom_BSplineSurface::PeriodicNormalization(double& Uparameter, double& Vparameter) const
{
  double Period, aMaxVal, aMinVal;

  if (myData.IsUPeriodic)
  {
    aMaxVal    = myData.UFlatKnots.Value(myData.UFlatKnots.Upper() - myData.UDegree);
    aMinVal    = myData.UFlatKnots.Value(myData.UDegree + 1);
    double eps = std::abs(Epsilon(Uparameter));
    Period     = aMaxVal - aMinVal;

    if (Period <= eps)
      throw Standard_OutOfRange(
        "Geom_BSplineSurface::PeriodicNormalization: Uparameter is too great number");

    bool isLess, isGreater;
    isLess    = aMinVal - Uparameter > 0;
    isGreater = Uparameter - aMaxVal > 0;
    if (isLess || isGreater)
    {
      double aDPar, aNbPer;
      aDPar = (isLess) ? (aMaxVal - Uparameter) : (aMinVal - Uparameter);
      modf(aDPar / Period, &aNbPer);
      Uparameter += aNbPer * Period;
    }
  }
  if (myData.IsVPeriodic)
  {
    aMaxVal    = myData.VFlatKnots.Value(myData.VFlatKnots.Upper() - myData.VDegree);
    aMinVal    = myData.VFlatKnots.Value(myData.VDegree + 1);
    double eps = std::abs(Epsilon(Vparameter));
    Period     = aMaxVal - aMinVal;

    if (Period <= eps)
      throw Standard_OutOfRange(
        "Geom_BSplineSurface::PeriodicNormalization: Vparameter is too great number");

    bool isLess, isGreater;
    isLess    = aMinVal - Vparameter > 0;
    isGreater = Vparameter - aMaxVal > 0;
    if (isLess || isGreater)
    {
      double aDPar, aNbPer;
      aDPar = (isLess) ? (aMaxVal - Vparameter) : (aMinVal - Vparameter);
      modf(aDPar / Period, &aNbPer);
      Vparameter += aNbPer * Period;
    }
  }
}

//=================================================================================================

void Geom_BSplineSurface::SetWeight(const int UIndex, const int VIndex, const double Weight)
{
  if (Weight <= gp::Resolution())
    throw Standard_ConstructionError("Geom_BSplineSurface::SetWeight: Weight too small");
  if (UIndex < 1 || UIndex > myData.Weights.ColLength() || VIndex < 1
      || VIndex > myData.Weights.RowLength())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetWeight: Index and #pole mismatch");
  }
  myData.Weights(UIndex + myData.Weights.LowerRow() - 1,
                 VIndex + myData.Weights.LowerCol() - 1) = Weight;
  Rational(myData.Weights, urational, vrational);
}

//=================================================================================================

void Geom_BSplineSurface::SetWeightCol(const int                         VIndex,
                                       const NCollection_Array1<double>& CPoleWeights)
{
  if (VIndex < 1 || VIndex > myData.Weights.RowLength())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetWeightCol: Index and #pole mismatch");
  }
  if (CPoleWeights.Lower() < 1 || CPoleWeights.Lower() > myData.Weights.ColLength()
      || CPoleWeights.Upper() < 1 || CPoleWeights.Upper() > myData.Weights.ColLength())
  {
    throw Standard_ConstructionError("Geom_BSplineSurface::SetWeightCol: invalid array dimension");
  }
  int I = CPoleWeights.Lower();
  while (I <= CPoleWeights.Upper())
  {
    if (CPoleWeights(I) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetWeightCol: Weight too small");
    }
    myData.Weights(I + myData.Weights.LowerRow() - 1,
                   VIndex + myData.Weights.LowerCol() - 1) = CPoleWeights(I);
    I++;
  }
  // Verifie si c'est rationnel
  Rational(myData.Weights, urational, vrational);
}

//=================================================================================================

void Geom_BSplineSurface::SetWeightRow(const int                         UIndex,
                                       const NCollection_Array1<double>& CPoleWeights)
{
  if (UIndex < 1 || UIndex > myData.Weights.ColLength())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetWeightRow: Index and #pole mismatch");
  }
  if (CPoleWeights.Lower() < 1 || CPoleWeights.Lower() > myData.Weights.RowLength()
      || CPoleWeights.Upper() < 1 || CPoleWeights.Upper() > myData.Weights.RowLength())
  {

    throw Standard_ConstructionError("Geom_BSplineSurface::SetWeightRow: invalid array dimension");
  }
  int I = CPoleWeights.Lower();

  while (I <= CPoleWeights.Upper())
  {
    if (CPoleWeights(I) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetWeightRow: Weight too small");
    }
    myData.Weights(UIndex + myData.Weights.LowerRow() - 1,
                   I + myData.Weights.LowerCol() - 1) = CPoleWeights(I);
    I++;
  }
  // Verifie si c'est rationnel
  Rational(myData.Weights, urational, vrational);
}

//=================================================================================================

void Geom_BSplineSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedSurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, urational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vrational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.IsUPeriodic)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.IsVPeriodic)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, uknotSet)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vknotSet)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, Usmooth)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, Vsmooth)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.UDegree)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.VDegree)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.Poles.Size())
  if (urational || vrational)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.Weights.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.UFlatKnots.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.VFlatKnots.Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.UKnots.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.VKnots.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.UMults.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.VMults.Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, umaxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vmaxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinvok)
}
