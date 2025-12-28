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
#define No_Standard_OutOfRange

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
    : urational(theOther.urational),
      vrational(theOther.vrational),
      uperiodic(theOther.uperiodic),
      vperiodic(theOther.vperiodic),
      uknotSet(theOther.uknotSet),
      vknotSet(theOther.vknotSet),
      Usmooth(theOther.Usmooth),
      Vsmooth(theOther.Vsmooth),
      udeg(theOther.udeg),
      vdeg(theOther.vdeg),
      umaxderivinv(theOther.umaxderivinv),
      vmaxderivinv(theOther.vmaxderivinv),
      maxderivinvok(false)
{
  // Deep copy all data arrays without validation
  poles                 = new NCollection_HArray2<gp_Pnt>(theOther.poles->LowerRow(),
                                          theOther.poles->UpperRow(),
                                          theOther.poles->LowerCol(),
                                          theOther.poles->UpperCol());
  poles->ChangeArray2() = theOther.poles->Array2();

  uknots = new NCollection_HArray1<double>(theOther.uknots->Lower(), theOther.uknots->Upper());
  uknots->ChangeArray1() = theOther.uknots->Array1();

  vknots = new NCollection_HArray1<double>(theOther.vknots->Lower(), theOther.vknots->Upper());
  vknots->ChangeArray1() = theOther.vknots->Array1();

  umults = new NCollection_HArray1<int>(theOther.umults->Lower(), theOther.umults->Upper());
  umults->ChangeArray1() = theOther.umults->Array1();

  vmults = new NCollection_HArray1<int>(theOther.vmults->Lower(), theOther.vmults->Upper());
  vmults->ChangeArray1() = theOther.vmults->Array1();

  if (!theOther.ufknots.IsNull())
  {
    ufknots = new NCollection_HArray1<double>(theOther.ufknots->Lower(), theOther.ufknots->Upper());
    ufknots->ChangeArray1() = theOther.ufknots->Array1();
  }

  if (!theOther.vfknots.IsNull())
  {
    vfknots = new NCollection_HArray1<double>(theOther.vfknots->Lower(), theOther.vfknots->Upper());
    vfknots->ChangeArray1() = theOther.vfknots->Array1();
  }

  if (!theOther.weights.IsNull())
  {
    weights                 = new NCollection_HArray2<double>(theOther.weights->LowerRow(),
                                              theOther.weights->UpperRow(),
                                              theOther.weights->LowerCol(),
                                              theOther.weights->UpperCol());
    weights->ChangeArray2() = theOther.weights->Array2();
  }
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
      uperiodic(UPeriodic),
      vperiodic(VPeriodic),
      udeg(UDegree),
      vdeg(VDegree),
      maxderivinvok(0)

{

  // check

  CheckSurfaceData(Poles, UKnots, VKnots, UMults, VMults, UDegree, VDegree, UPeriodic, VPeriodic);

  // copy arrays

  poles = new NCollection_HArray2<gp_Pnt>(1, Poles.ColLength(), 1, Poles.RowLength());
  poles->ChangeArray2() = Poles;

  weights = new NCollection_HArray2<double>(1, Poles.ColLength(), 1, Poles.RowLength(), 1.0);

  uknots                 = new NCollection_HArray1<double>(1, UKnots.Length());
  uknots->ChangeArray1() = UKnots;

  umults                 = new NCollection_HArray1<int>(1, UMults.Length());
  umults->ChangeArray1() = UMults;

  vknots                 = new NCollection_HArray1<double>(1, VKnots.Length());
  vknots->ChangeArray1() = VKnots;

  vmults                 = new NCollection_HArray1<int>(1, VMults.Length());
  vmults->ChangeArray1() = VMults;

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
      uperiodic(UPeriodic),
      vperiodic(VPeriodic),
      udeg(UDegree),
      vdeg(VDegree),
      maxderivinvok(0)
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

  // copy arrays

  poles = new NCollection_HArray2<gp_Pnt>(1, Poles.ColLength(), 1, Poles.RowLength());
  poles->ChangeArray2() = Poles;

  weights = new NCollection_HArray2<double>(1, Poles.ColLength(), 1, Poles.RowLength());
  weights->ChangeArray2() = Weights;

  uknots                 = new NCollection_HArray1<double>(1, UKnots.Length());
  uknots->ChangeArray1() = UKnots;

  umults                 = new NCollection_HArray1<int>(1, UMults.Length());
  umults->ChangeArray1() = UMults;

  vknots                 = new NCollection_HArray1<double>(1, VKnots.Length());
  vknots->ChangeArray1() = VKnots;

  vmults                 = new NCollection_HArray1<int>(1, VMults.Length());
  vmults->ChangeArray1() = VMults;

  UpdateUKnots();
  UpdateVKnots();
}

//=================================================================================================

void Geom_BSplineSurface::ExchangeUV()
{
  int LC = poles->LowerCol();
  int UC = poles->UpperCol();
  int LR = poles->LowerRow();
  int UR = poles->UpperRow();

  occ::handle<NCollection_HArray2<gp_Pnt>> npoles = new NCollection_HArray2<gp_Pnt>(LC, UC, LR, UR);
  occ::handle<NCollection_HArray2<double>> nweights;
  if (!weights.IsNull())
  {
    nweights = new NCollection_HArray2<double>(LC, UC, LR, UR);
  }

  const NCollection_Array2<gp_Pnt>& spoles   = poles->Array2();
  const NCollection_Array2<double>* sweights = !weights.IsNull() ? &weights->Array2() : nullptr;

  NCollection_Array2<gp_Pnt>& snpoles   = npoles->ChangeArray2();
  NCollection_Array2<double>* snweights = !nweights.IsNull() ? &nweights->ChangeArray2() : nullptr;
  for (int i = LC; i <= UC; i++)
  {
    for (int j = LR; j <= UR; j++)
    {
      snpoles(i, j) = spoles(j, i);
      if (snweights != nullptr)
      {
        snweights->ChangeValue(i, j) = sweights->Value(j, i);
      }
    }
  }
  poles   = npoles;
  weights = nweights;

  std::swap(urational, vrational);
  std::swap(uperiodic, vperiodic);
  std::swap(udeg, vdeg);
  std::swap(uknots, vknots);
  std::swap(umults, vmults);

  UpdateUKnots();
  UpdateVKnots();
}

//=================================================================================================

void Geom_BSplineSurface::IncreaseDegree(const int UDegree, const int VDegree)
{
  if (UDegree != udeg)
  {
    if (UDegree < udeg || UDegree > Geom_BSplineSurface::MaxDegree())
      throw Standard_ConstructionError("Geom_BSplineSurface::IncreaseDegree: bad U degree value");

    int FromK1 = FirstUKnotIndex();
    int ToK2   = LastUKnotIndex();

    int Step = UDegree - udeg;

    occ::handle<NCollection_HArray2<gp_Pnt>> npoles =
      new NCollection_HArray2<gp_Pnt>(1,
                                      poles->ColLength() + Step * (ToK2 - FromK1),
                                      1,
                                      poles->RowLength());

    int nbknots = BSplCLib::IncreaseDegreeCountKnots(udeg, UDegree, uperiodic, umults->Array1());

    occ::handle<NCollection_HArray1<double>> nknots = new NCollection_HArray1<double>(1, nbknots);

    occ::handle<NCollection_HArray1<int>> nmults = new NCollection_HArray1<int>(1, nbknots);

    occ::handle<NCollection_HArray2<double>> nweights =
      new NCollection_HArray2<double>(1, npoles->ColLength(), 1, npoles->RowLength(), 1.);

    if (urational || vrational)
    {

      BSplSLib::IncreaseDegree(true,
                               udeg,
                               UDegree,
                               uperiodic,
                               poles->Array2(),
                               &weights->Array2(),
                               uknots->Array1(),
                               umults->Array1(),
                               npoles->ChangeArray2(),
                               &nweights->ChangeArray2(),
                               nknots->ChangeArray1(),
                               nmults->ChangeArray1());
    }
    else
    {

      BSplSLib::IncreaseDegree(true,
                               udeg,
                               UDegree,
                               uperiodic,
                               poles->Array2(),
                               BSplSLib::NoWeights(),
                               uknots->Array1(),
                               umults->Array1(),
                               npoles->ChangeArray2(),
                               BSplSLib::NoWeights(),
                               nknots->ChangeArray1(),
                               nmults->ChangeArray1());
    }
    udeg    = UDegree;
    poles   = npoles;
    weights = nweights;
    uknots  = nknots;
    umults  = nmults;
    UpdateUKnots();
  }

  if (VDegree != vdeg)
  {
    if (VDegree < vdeg || VDegree > Geom_BSplineSurface::MaxDegree())
      throw Standard_ConstructionError("Geom_BSplineSurface::IncreaseDegree: bad V degree value");

    int FromK1 = FirstVKnotIndex();
    int ToK2   = LastVKnotIndex();

    int Step = VDegree - vdeg;

    occ::handle<NCollection_HArray2<gp_Pnt>> npoles =
      new NCollection_HArray2<gp_Pnt>(1,
                                      poles->ColLength(),
                                      1,
                                      poles->RowLength() + Step * (ToK2 - FromK1));

    int nbknots = BSplCLib::IncreaseDegreeCountKnots(vdeg, VDegree, vperiodic, vmults->Array1());

    occ::handle<NCollection_HArray1<double>> nknots = new NCollection_HArray1<double>(1, nbknots);

    occ::handle<NCollection_HArray1<int>> nmults = new NCollection_HArray1<int>(1, nbknots);

    occ::handle<NCollection_HArray2<double>> nweights =
      new NCollection_HArray2<double>(1, npoles->ColLength(), 1, npoles->RowLength(), 1.);

    if (urational || vrational)
    {

      BSplSLib::IncreaseDegree(false,
                               vdeg,
                               VDegree,
                               vperiodic,
                               poles->Array2(),
                               &weights->Array2(),
                               vknots->Array1(),
                               vmults->Array1(),
                               npoles->ChangeArray2(),
                               &nweights->ChangeArray2(),
                               nknots->ChangeArray1(),
                               nmults->ChangeArray1());
    }
    else
    {

      BSplSLib::IncreaseDegree(false,
                               vdeg,
                               VDegree,
                               vperiodic,
                               poles->Array2(),
                               BSplSLib::NoWeights(),
                               vknots->Array1(),
                               vmults->Array1(),
                               npoles->ChangeArray2(),
                               BSplSLib::NoWeights(),
                               nknots->ChangeArray1(),
                               nmults->ChangeArray1());
    }
    vdeg    = VDegree;
    poles   = npoles;
    weights = nweights;
    vknots  = nknots;
    vmults  = nmults;
    UpdateVKnots();
  }
}

//=================================================================================================

void Geom_BSplineSurface::IncreaseUMultiplicity(const int UIndex, const int M)
{
  NCollection_Array1<double> k(1, 1);
  k(1) = uknots->Value(UIndex);
  NCollection_Array1<int> m(1, 1);
  m(1) = M - umults->Value(UIndex);
  InsertUKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom_BSplineSurface::IncreaseUMultiplicity(const int FromI1, const int ToI2, const int M)
{
  occ::handle<NCollection_HArray1<double>> tk = uknots;
  NCollection_Array1<double>               k((uknots->Array1())(FromI1), FromI1, ToI2);
  NCollection_Array1<int>                  m(FromI1, ToI2);
  for (int i = FromI1; i <= ToI2; i++)
    m(i) = M - umults->Value(i);
  InsertUKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom_BSplineSurface::IncreaseVMultiplicity(const int VIndex, const int M)
{
  NCollection_Array1<double> k(1, 1);
  k(1) = vknots->Value(VIndex);
  NCollection_Array1<int> m(1, 1);
  m(1) = M - vmults->Value(VIndex);
  InsertVKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom_BSplineSurface::IncreaseVMultiplicity(const int FromI1, const int ToI2, const int M)
{
  occ::handle<NCollection_HArray1<double>> tk = vknots;
  NCollection_Array1<double>               k((vknots->Array1())(FromI1), FromI1, ToI2);
  NCollection_Array1<int>                  m(FromI1, ToI2);
  for (int i = FromI1; i <= ToI2; i++)
    m(i) = M - vmults->Value(i);
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
  if (uperiodic)
  {
    double aUPeriod = uknots->Last() - uknots->First();
    if (deltaU - aUPeriod > Precision::PConfusion())
      throw Standard_DomainError("Geom_BSplineSurface::Segment");
    if (deltaU > aUPeriod)
      deltaU = aUPeriod;
  }

  double deltaV = V2 - V1;
  if (vperiodic)
  {
    double aVPeriod = vknots->Last() - vknots->First();
    if (deltaV - aVPeriod > Precision::PConfusion())
      throw Standard_DomainError("Geom_BSplineSurface::Segment");
    if (deltaV > aVPeriod)
      deltaV = aVPeriod;
  }

  double NewU1, NewU2, NewV1, NewV2;
  double U, V;
  int    indexU, indexV;

  indexU = 0;
  BSplCLib::LocateParameter(udeg,
                            uknots->Array1(),
                            umults->Array1(),
                            U1,
                            uperiodic,
                            uknots->Lower(),
                            uknots->Upper(),
                            indexU,
                            NewU1);
  indexU = 0;
  BSplCLib::LocateParameter(udeg,
                            uknots->Array1(),
                            umults->Array1(),
                            U2,
                            uperiodic,
                            uknots->Lower(),
                            uknots->Upper(),
                            indexU,
                            NewU2);
  if (SegmentInU)
  {
    // inserting the UKnots
    NCollection_Array1<double> UKnots(1, 2);
    NCollection_Array1<int>    UMults(1, 2);
    UKnots(1) = std::min(NewU1, NewU2);
    UKnots(2) = std::max(NewU1, NewU2);
    UMults(1) = UMults(2) = udeg;

    InsertUKnots(UKnots, UMults, EpsU);
  }

  indexV = 0;
  BSplCLib::LocateParameter(vdeg,
                            vknots->Array1(),
                            vmults->Array1(),
                            V1,
                            vperiodic,
                            vknots->Lower(),
                            vknots->Upper(),
                            indexV,
                            NewV1);
  indexV = 0;
  BSplCLib::LocateParameter(vdeg,
                            vknots->Array1(),
                            vmults->Array1(),
                            V2,
                            vperiodic,
                            vknots->Lower(),
                            vknots->Upper(),
                            indexV,
                            NewV2);
  if (SegmentInV)
  {
    // Inserting the VKnots
    NCollection_Array1<double> VKnots(1, 2);
    NCollection_Array1<int>    VMults(1, 2);

    VKnots(1) = std::min(NewV1, NewV2);
    VKnots(2) = std::max(NewV1, NewV2);
    VMults(1) = VMults(2) = vdeg;
    InsertVKnots(VKnots, VMults, EpsV);
  }

  if (uperiodic && SegmentInU)
  { // set the origine at NewU1
    int index = 0;
    BSplCLib::LocateParameter(udeg,
                              uknots->Array1(),
                              umults->Array1(),
                              U1,
                              uperiodic,
                              uknots->Lower(),
                              uknots->Upper(),
                              index,
                              U);
    if (std::abs(uknots->Value(index + 1) - U) <= EpsU)
      index++;
    SetUOrigin(index);
    SetUNotPeriodic();
  }

  // compute index1 and index2 to set the new knots and mults
  int index1U = 0, index2U = 0;
  int FromU1 = uknots->Lower();
  int ToU2   = uknots->Upper();
  BSplCLib::LocateParameter(udeg,
                            uknots->Array1(),
                            umults->Array1(),
                            NewU1,
                            uperiodic,
                            FromU1,
                            ToU2,
                            index1U,
                            U);
  if (std::abs(uknots->Value(index1U + 1) - U) <= EpsU)
    index1U++;
  BSplCLib::LocateParameter(udeg,
                            uknots->Array1(),
                            umults->Array1(),
                            NewU1 + deltaU,
                            uperiodic,
                            FromU1,
                            ToU2,
                            index2U,
                            U);
  if (std::abs(uknots->Value(index2U + 1) - U) <= EpsU || index2U == index1U)
    index2U++;

  int nbuknots = index2U - index1U + 1;

  occ::handle<NCollection_HArray1<double>> nuknots = new NCollection_HArray1<double>(1, nbuknots);
  occ::handle<NCollection_HArray1<int>>    numults = new NCollection_HArray1<int>(1, nbuknots);

  int i, k = 1;
  for (i = index1U; i <= index2U; i++)
  {
    nuknots->SetValue(k, uknots->Value(i));
    numults->SetValue(k, umults->Value(i));
    k++;
  }
  if (SegmentInU)
  {
    numults->SetValue(1, udeg + 1);
    numults->SetValue(nbuknots, udeg + 1);
  }

  if (vperiodic && SegmentInV)
  { // set the origine at NewV1
    int index = 0;
    BSplCLib::LocateParameter(vdeg,
                              vknots->Array1(),
                              vmults->Array1(),
                              V1,
                              vperiodic,
                              vknots->Lower(),
                              vknots->Upper(),
                              index,
                              V);
    if (std::abs(vknots->Value(index + 1) - V) <= EpsV)
      index++;
    SetVOrigin(index);
    SetVNotPeriodic();
  }

  // compute index1 and index2 to set the new knots and mults
  int index1V = 0, index2V = 0;
  int FromV1 = vknots->Lower();
  int ToV2   = vknots->Upper();
  BSplCLib::LocateParameter(vdeg,
                            vknots->Array1(),
                            vmults->Array1(),
                            NewV1,
                            vperiodic,
                            FromV1,
                            ToV2,
                            index1V,
                            V);
  if (std::abs(vknots->Value(index1V + 1) - V) <= EpsV)
    index1V++;
  BSplCLib::LocateParameter(vdeg,
                            vknots->Array1(),
                            vmults->Array1(),
                            NewV1 + deltaV,
                            vperiodic,
                            FromV1,
                            ToV2,
                            index2V,
                            V);
  if (std::abs(vknots->Value(index2V + 1) - V) <= EpsV || index2V == index1V)
    index2V++;

  int nbvknots = index2V - index1V + 1;

  occ::handle<NCollection_HArray1<double>> nvknots = new NCollection_HArray1<double>(1, nbvknots);
  occ::handle<NCollection_HArray1<int>>    nvmults = new NCollection_HArray1<int>(1, nbvknots);

  k = 1;
  for (i = index1V; i <= index2V; i++)
  {
    nvknots->SetValue(k, vknots->Value(i));
    nvmults->SetValue(k, vmults->Value(i));
    k++;
  }
  if (SegmentInV)
  {
    nvmults->SetValue(1, vdeg + 1);
    nvmults->SetValue(nbvknots, vdeg + 1);
  }

  // compute index1 and index2 to set the new poles and weights
  int pindex1U = BSplCLib::PoleIndex(udeg, index1U, uperiodic, umults->Array1());
  int pindex2U = BSplCLib::PoleIndex(udeg, index2U, uperiodic, umults->Array1());

  pindex1U++;
  pindex2U = std::min(pindex2U + 1, poles->ColLength());

  int nbupoles = pindex2U - pindex1U + 1;

  // compute index1 and index2 to set the new poles and weights
  int pindex1V = BSplCLib::PoleIndex(vdeg, index1V, vperiodic, vmults->Array1());
  int pindex2V = BSplCLib::PoleIndex(vdeg, index2V, vperiodic, vmults->Array1());

  pindex1V++;
  pindex2V = std::min(pindex2V + 1, poles->RowLength());

  int nbvpoles = pindex2V - pindex1V + 1;

  occ::handle<NCollection_HArray2<double>> nweights;

  occ::handle<NCollection_HArray2<gp_Pnt>> npoles =
    new NCollection_HArray2<gp_Pnt>(1, nbupoles, 1, nbvpoles);
  k = 1;
  int j, l;
  if (urational || vrational)
  {
    nweights = new NCollection_HArray2<double>(1, nbupoles, 1, nbvpoles);
    for (i = pindex1U; i <= pindex2U; i++)
    {
      l = 1;
      for (j = pindex1V; j <= pindex2V; j++)
      {
        npoles->SetValue(k, l, poles->Value(i, j));
        nweights->SetValue(k, l, weights->Value(i, j));
        l++;
      }
      k++;
    }
  }
  else
  {
    for (i = pindex1U; i <= pindex2U; i++)
    {
      l = 1;
      for (j = pindex1V; j <= pindex2V; j++)
      {
        npoles->SetValue(k, l, poles->Value(i, j));
        l++;
      }
      k++;
    }
  }

  uknots = nuknots;
  umults = numults;
  vknots = nvknots;
  vmults = nvmults;
  poles  = npoles;
  if (urational || vrational)
    weights = nweights;
  else
    weights = new NCollection_HArray2<double>(1, poles->ColLength(), 1, poles->RowLength(), 1.0);

  maxderivinvok = 0;
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
  segment_in_U      = (std::abs(U1 - uknots->Value(uknots->Lower())) > EpsU)
                 || (std::abs(U2 - uknots->Value(uknots->Upper())) > EpsU);
  segment_in_V = (std::abs(V1 - vknots->Value(vknots->Lower())) > EpsV)
                 || (std::abs(V2 - vknots->Value(vknots->Upper())) > EpsV);

  segment(U1, U2, V1, V2, EpsU, EpsV, segment_in_U, segment_in_V);
}

//=================================================================================================

void Geom_BSplineSurface::SetUKnot(const int UIndex, const double K)
{
  if (UIndex < 1 || UIndex > uknots->Length())
    throw Standard_OutOfRange("Geom_BSplineSurface::SetUKnot: Index and #knots mismatch");

  int    NewIndex = UIndex;
  double DU       = std::abs(Epsilon(K));
  if (UIndex == 1)
  {
    if (K >= uknots->Value(2) - DU)
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnot: K out of range");
  }
  else if (UIndex == uknots->Length())
  {
    if (K <= uknots->Value(uknots->Length() - 1) + DU)
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnot: K out of range");
    }
  }
  else
  {
    if (K <= uknots->Value(NewIndex - 1) + DU || K >= uknots->Value(NewIndex + 1) - DU)
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnot: K out of range");
    }
  }

  if (K != uknots->Value(NewIndex))
  {
    uknots->SetValue(NewIndex, K);
    maxderivinvok = 0;
    UpdateUKnots();
  }
}

//=================================================================================================

void Geom_BSplineSurface::SetUKnots(const NCollection_Array1<double>& UK)
{

  int Lower = UK.Lower();
  int Upper = UK.Upper();
  if (Lower < 1 || Lower > uknots->Length() || Upper < 1 || Upper > uknots->Length())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetUKnots: invalid array dimension");
  }
  if (Lower > 1)
  {
    if (std::abs(UK(Lower) - uknots->Value(Lower - 1)) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnots: invalid knot value");
    }
  }
  if (Upper < uknots->Length())
  {
    if (std::abs(UK(Upper) - uknots->Value(Upper + 1)) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnots: invalid knot value");
    }
  }
  double K1 = UK(Lower);
  for (int i = Lower; i <= Upper; i++)
  {
    uknots->SetValue(i, UK(i));
    if (i != Lower)
    {
      if (std::abs(UK(i) - K1) <= gp::Resolution())
      {
        throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnots: invalid knot value");
      }
      K1 = UK(i);
    }
  }

  maxderivinvok = 0;
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
  if (VIndex < 1 || VIndex > vknots->Length())
    throw Standard_OutOfRange("Geom_BSplineSurface::SetVKnot: Index and #knots mismatch");
  int    NewIndex = VIndex + vknots->Lower() - 1;
  double DV       = std::abs(Epsilon(K));
  if (VIndex == 1)
  {
    if (K >= vknots->Value(2) - DV)
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnot: K out of range");
    }
  }
  else if (VIndex == vknots->Length())
  {
    if (K <= vknots->Value(vknots->Length() - 1) + DV)
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnot: K out of range");
    }
  }
  else
  {
    if (K <= vknots->Value(NewIndex - 1) + DV || K >= vknots->Value(NewIndex + 1) - DV)
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnot: K out of range");
    }
  }

  if (K != vknots->Value(NewIndex))
  {
    vknots->SetValue(NewIndex, K);
    maxderivinvok = 0;
    UpdateVKnots();
  }
}

//=================================================================================================

void Geom_BSplineSurface::SetVKnots(const NCollection_Array1<double>& VK)
{

  int Lower = VK.Lower();
  int Upper = VK.Upper();
  if (Lower < 1 || Lower > vknots->Length() || Upper < 1 || Upper > vknots->Length())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetVKnots: invalid array dimension");
  }
  if (Lower > 1)
  {
    if (std::abs(VK(Lower) - vknots->Value(Lower - 1)) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnots: invalid knot value");
    }
  }
  if (Upper < vknots->Length())
  {
    if (std::abs(VK(Upper) - vknots->Value(Upper + 1)) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnots: invalid knot value");
    }
  }
  double K1 = VK(Lower);
  for (int i = Lower; i <= Upper; i++)
  {
    vknots->SetValue(i, VK(i));
    if (i != Lower)
    {
      if (std::abs(VK(i) - K1) <= gp::Resolution())
      {
        throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnots: invalid knot value");
      }
      K1 = VK(i);
    }
  }

  maxderivinvok = 0;
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
  occ::handle<NCollection_HArray1<double>> tk = uknots;
  NCollection_Array1<double>               k((uknots->Array1())(FromI1), FromI1, ToI2);
  NCollection_Array1<int>                  m(FromI1, ToI2);
  m.Init(Step);
  InsertUKnots(k, m, Epsilon(1.));
}

//=================================================================================================

void Geom_BSplineSurface::IncrementVMultiplicity(const int FromI1, const int ToI2, const int Step)
{
  occ::handle<NCollection_HArray1<double>> tk = vknots;
  NCollection_Array1<double>               k((vknots->Array1())(FromI1), FromI1, ToI2);

  NCollection_Array1<int> m(FromI1, ToI2);
  m.Init(Step);

  InsertVKnots(k, m, Epsilon(1.));
}

//=================================================================================================

void Geom_BSplineSurface::UpdateUKnots()
{

  int MaxKnotMult = 0;
  BSplCLib::KnotAnalysis(udeg,
                         uperiodic,
                         uknots->Array1(),
                         umults->Array1(),
                         uknotSet,
                         MaxKnotMult);

  if (uknotSet == GeomAbs_Uniform && !uperiodic)
  {
    ufknots = uknots;
  }
  else
  {
    ufknots = new NCollection_HArray1<double>(
      1,
      BSplCLib::KnotSequenceLength(umults->Array1(), udeg, uperiodic));

    BSplCLib::KnotSequence(uknots->Array1(),
                           umults->Array1(),
                           udeg,
                           uperiodic,
                           ufknots->ChangeArray1());
  }

  if (MaxKnotMult == 0)
    Usmooth = GeomAbs_CN;
  else
  {
    switch (udeg - MaxKnotMult)
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
  BSplCLib::KnotAnalysis(vdeg,
                         vperiodic,
                         vknots->Array1(),
                         vmults->Array1(),
                         vknotSet,
                         MaxKnotMult);

  if (vknotSet == GeomAbs_Uniform && !vperiodic)
  {
    vfknots = vknots;
  }
  else
  {
    vfknots = new NCollection_HArray1<double>(
      1,
      BSplCLib::KnotSequenceLength(vmults->Array1(), vdeg, vperiodic));

    BSplCLib::KnotSequence(vknots->Array1(),
                           vmults->Array1(),
                           vdeg,
                           vperiodic,
                           vfknots->ChangeArray1());
  }

  if (MaxKnotMult == 0)
    Vsmooth = GeomAbs_CN;
  else
  {
    switch (vdeg - MaxKnotMult)
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

//=======================================================================
// function : Normalizes the parameters if the curve is periodic
// purpose  : that is compute the cache so that it is valid
//=======================================================================

void Geom_BSplineSurface::PeriodicNormalization(double& Uparameter, double& Vparameter) const
{
  double Period, aMaxVal, aMinVal;

  if (uperiodic)
  {
    aMaxVal    = ufknots->Value(ufknots->Upper() - udeg);
    aMinVal    = ufknots->Value(udeg + 1);
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
  if (vperiodic)
  {
    aMaxVal    = vfknots->Value(vfknots->Upper() - vdeg);
    aMinVal    = vfknots->Value(vdeg + 1);
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
  NCollection_Array2<double>& Weights = weights->ChangeArray2();
  if (UIndex < 1 || UIndex > Weights.ColLength() || VIndex < 1 || VIndex > Weights.RowLength())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetWeight: Index and #pole mismatch");
  }
  Weights(UIndex + Weights.LowerRow() - 1, VIndex + Weights.LowerCol() - 1) = Weight;
  Rational(Weights, urational, vrational);
}

//=================================================================================================

void Geom_BSplineSurface::SetWeightCol(const int                         VIndex,
                                       const NCollection_Array1<double>& CPoleWeights)
{
  NCollection_Array2<double>& Weights = weights->ChangeArray2();
  if (VIndex < 1 || VIndex > Weights.RowLength())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetWeightCol: Index and #pole mismatch");
  }
  if (CPoleWeights.Lower() < 1 || CPoleWeights.Lower() > Weights.ColLength()
      || CPoleWeights.Upper() < 1 || CPoleWeights.Upper() > Weights.ColLength())
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
    Weights(I + Weights.LowerRow() - 1, VIndex + Weights.LowerCol() - 1) = CPoleWeights(I);
    I++;
  }
  // Verifie si c'est rationnel
  Rational(Weights, urational, vrational);
}

//=================================================================================================

void Geom_BSplineSurface::SetWeightRow(const int                         UIndex,
                                       const NCollection_Array1<double>& CPoleWeights)
{
  NCollection_Array2<double>& Weights = weights->ChangeArray2();
  if (UIndex < 1 || UIndex > Weights.ColLength())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetWeightRow: Index and #pole mismatch");
  }
  if (CPoleWeights.Lower() < 1 || CPoleWeights.Lower() > Weights.RowLength()
      || CPoleWeights.Upper() < 1 || CPoleWeights.Upper() > Weights.RowLength())
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
    Weights(UIndex + Weights.LowerRow() - 1, I + Weights.LowerCol() - 1) = CPoleWeights(I);
    I++;
  }
  // Verifie si c'est rationnel
  Rational(Weights, urational, vrational);
}

//=================================================================================================

void Geom_BSplineSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedSurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, urational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vrational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, uperiodic)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vperiodic)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, uknotSet)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vknotSet)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, Usmooth)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, Vsmooth)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, udeg)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vdeg)

  if (!poles.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, poles->Size())
  if (!weights.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, weights->Size())
  if (!ufknots.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, ufknots->Size())
  if (!vfknots.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vfknots->Size())

  if (!uknots.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, uknots->Size())
  if (!vknots.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vknots->Size())
  if (!umults.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, umults->Size())
  if (!vmults.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vmults->Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, umaxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vmaxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinvok)
}
