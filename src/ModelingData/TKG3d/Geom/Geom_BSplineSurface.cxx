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
// jct : 19-01-99 ; permutation de myURational et myVRational dans Rational.

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
    : myPoles(theOther.myPoles),
      myWeights((theOther.myURational || theOther.myVRational)
                  ? NCollection_Array2<double>(theOther.myWeights)
                  : BSplSLib::UnitWeights(theOther.myPoles.ColLength(),
                                          theOther.myPoles.RowLength())),
      myUKnots(theOther.myUKnots),
      myVKnots(theOther.myVKnots),
      myUFlatKnots(theOther.myUFlatKnots),
      myVFlatKnots(theOther.myVFlatKnots),
      myUMults(theOther.myUMults),
      myVMults(theOther.myVMults),
      myUDeg(theOther.myUDeg),
      myVDeg(theOther.myVDeg),
      myUPeriodic(theOther.myUPeriodic),
      myVPeriodic(theOther.myVPeriodic),
      myURational(theOther.myURational),
      myVRational(theOther.myVRational),
      myUKnotSet(theOther.myUKnotSet),
      myVKnotSet(theOther.myVKnotSet),
      myUSmooth(theOther.myUSmooth),
      myVSmooth(theOther.myVSmooth),
      myUMaxDerivInv(theOther.myUMaxDerivInv),
      myVMaxDerivInv(theOther.myVMaxDerivInv),
      myMaxDerivInvOk(false)
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
    : myURational(false),
      myVRational(false),
      myMaxDerivInvOk(false)

{

  // check

  CheckSurfaceData(Poles, UKnots, VKnots, UMults, VMults, UDegree, VDegree, UPeriodic, VPeriodic);

  // set scalar fields
  myUDeg      = UDegree;
  myVDeg      = VDegree;
  myUPeriodic = UPeriodic;
  myVPeriodic = VPeriodic;

  // copy arrays

  myPoles.Resize(1, Poles.ColLength(), 1, Poles.RowLength(), false);
  myPoles.Assign(Poles);

  myWeights = BSplSLib::UnitWeights(Poles.ColLength(), Poles.RowLength());

  myUKnots.Resize(1, UKnots.Length(), false);
  myUKnots.Assign(UKnots);

  myUMults.Resize(1, UMults.Length(), false);
  myUMults.Assign(UMults);

  myVKnots.Resize(1, VKnots.Length(), false);
  myVKnots.Assign(VKnots);

  myVMults.Resize(1, VMults.Length(), false);
  myVMults.Assign(VMults);

  updateUKnots();
  updateVKnots();
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
    : myURational(false),
      myVRational(false),
      myMaxDerivInvOk(false)
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

  Rational(Weights, myURational, myVRational);

  // check

  CheckSurfaceData(Poles, UKnots, VKnots, UMults, VMults, UDegree, VDegree, UPeriodic, VPeriodic);

  // set scalar fields
  myUDeg      = UDegree;
  myVDeg      = VDegree;
  myUPeriodic = UPeriodic;
  myVPeriodic = VPeriodic;

  // copy arrays

  myPoles.Resize(1, Poles.ColLength(), 1, Poles.RowLength(), false);
  myPoles.Assign(Poles);

  if (myURational || myVRational)
  {
    myWeights.Resize(1, Poles.ColLength(), 1, Poles.RowLength(), false);
    myWeights.Assign(Weights);
  }
  else
  {
    myWeights = BSplSLib::UnitWeights(Poles.ColLength(), Poles.RowLength());
  }

  myUKnots.Resize(1, UKnots.Length(), false);
  myUKnots.Assign(UKnots);

  myUMults.Resize(1, UMults.Length(), false);
  myUMults.Assign(UMults);

  myVKnots.Resize(1, VKnots.Length(), false);
  myVKnots.Assign(VKnots);

  myVMults.Resize(1, VMults.Length(), false);
  myVMults.Assign(VMults);

  updateUKnots();
  updateVKnots();
}

//=================================================================================================

void Geom_BSplineSurface::ExchangeUV()
{
  int LC = myPoles.LowerCol();
  int UC = myPoles.UpperCol();
  int LR = myPoles.LowerRow();
  int UR = myPoles.UpperRow();

  NCollection_Array2<gp_Pnt> npoles(LC, UC, LR, UR);
  NCollection_Array2<double> nweights;
  if (myURational || myVRational)
  {
    nweights = NCollection_Array2<double>(LC, UC, LR, UR);
  }

  for (int i = LC; i <= UC; i++)
  {
    for (int j = LR; j <= UR; j++)
    {
      npoles(i, j) = myPoles(j, i);
      if (myURational || myVRational)
      {
        nweights.ChangeValue(i, j) = myWeights.Value(j, i);
      }
    }
  }
  myPoles = std::move(npoles);
  if (myURational || myVRational)
  {
    myWeights = std::move(nweights);
  }
  else
  {
    myWeights = BSplSLib::UnitWeights(myPoles.ColLength(), myPoles.RowLength());
  }

  std::swap(myURational, myVRational);
  std::swap(myUPeriodic, myVPeriodic);
  std::swap(myUDeg, myVDeg);
  std::swap(myUKnots, myVKnots);
  std::swap(myUMults, myVMults);

  updateUKnots();
  updateVKnots();
}

//=================================================================================================

void Geom_BSplineSurface::IncreaseDegree(const int UDegree, const int VDegree)
{
  if (UDegree != myUDeg)
  {
    if (UDegree < myUDeg || UDegree > Geom_BSplineSurface::MaxDegree())
      throw Standard_ConstructionError("Geom_BSplineSurface::IncreaseDegree: bad U degree value");

    int FromK1 = FirstUKnotIndex();
    int ToK2   = LastUKnotIndex();

    int Step = UDegree - myUDeg;

    NCollection_Array2<gp_Pnt> npoles(1,
                                      myPoles.ColLength() + Step * (ToK2 - FromK1),
                                      1,
                                      myPoles.RowLength());

    int nbknots = BSplCLib::IncreaseDegreeCountKnots(myUDeg, UDegree, myUPeriodic, myUMults);

    NCollection_Array1<double> nknots(1, nbknots);

    NCollection_Array1<int> nmults(1, nbknots);

    if (myURational || myVRational)
    {
      NCollection_Array2<double> nweights(1, npoles.ColLength(), 1, npoles.RowLength());
      BSplSLib::IncreaseDegree(true,
                               myUDeg,
                               UDegree,
                               myUPeriodic,
                               myPoles,
                               &myWeights,
                               myUKnots,
                               myUMults,
                               npoles,
                               &nweights,
                               nknots,
                               nmults);
      myWeights = std::move(nweights);
    }
    else
    {
      BSplSLib::IncreaseDegree(true,
                               myUDeg,
                               UDegree,
                               myUPeriodic,
                               myPoles,
                               BSplSLib::NoWeights(),
                               myUKnots,
                               myUMults,
                               npoles,
                               BSplSLib::NoWeights(),
                               nknots,
                               nmults);
      myWeights = BSplSLib::UnitWeights(npoles.ColLength(), npoles.RowLength());
    }
    myUDeg   = UDegree;
    myPoles  = std::move(npoles);
    myUKnots = std::move(nknots);
    myUMults = std::move(nmults);
    updateUKnots();
  }

  if (VDegree != myVDeg)
  {
    if (VDegree < myVDeg || VDegree > Geom_BSplineSurface::MaxDegree())
      throw Standard_ConstructionError("Geom_BSplineSurface::IncreaseDegree: bad V degree value");

    int FromK1 = FirstVKnotIndex();
    int ToK2   = LastVKnotIndex();

    int Step = VDegree - myVDeg;

    NCollection_Array2<gp_Pnt> npoles(1,
                                      myPoles.ColLength(),
                                      1,
                                      myPoles.RowLength() + Step * (ToK2 - FromK1));

    int nbknots = BSplCLib::IncreaseDegreeCountKnots(myVDeg, VDegree, myVPeriodic, myVMults);

    NCollection_Array1<double> nknots(1, nbknots);

    NCollection_Array1<int> nmults(1, nbknots);

    if (myURational || myVRational)
    {
      NCollection_Array2<double> nweights(1, npoles.ColLength(), 1, npoles.RowLength());
      BSplSLib::IncreaseDegree(false,
                               myVDeg,
                               VDegree,
                               myVPeriodic,
                               myPoles,
                               &myWeights,
                               myVKnots,
                               myVMults,
                               npoles,
                               &nweights,
                               nknots,
                               nmults);
      myWeights = std::move(nweights);
    }
    else
    {
      BSplSLib::IncreaseDegree(false,
                               myVDeg,
                               VDegree,
                               myVPeriodic,
                               myPoles,
                               BSplSLib::NoWeights(),
                               myVKnots,
                               myVMults,
                               npoles,
                               BSplSLib::NoWeights(),
                               nknots,
                               nmults);
      myWeights = BSplSLib::UnitWeights(npoles.ColLength(), npoles.RowLength());
    }
    myVDeg   = VDegree;
    myPoles  = std::move(npoles);
    myVKnots = std::move(nknots);
    myVMults = std::move(nmults);
    updateVKnots();
  }
}

//=================================================================================================

void Geom_BSplineSurface::IncreaseUMultiplicity(const int UIndex, const int M)
{
  NCollection_Array1<double> k(1, 1);
  k(1) = myUKnots.Value(UIndex);
  NCollection_Array1<int> m(1, 1);
  m(1) = M - myUMults.Value(UIndex);
  InsertUKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom_BSplineSurface::IncreaseUMultiplicity(const int FromI1, const int ToI2, const int M)
{
  NCollection_Array1<double> k(myUKnots(FromI1), FromI1, ToI2);
  NCollection_Array1<int>    m(FromI1, ToI2);
  for (int i = FromI1; i <= ToI2; i++)
    m(i) = M - myUMults.Value(i);
  InsertUKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom_BSplineSurface::IncreaseVMultiplicity(const int VIndex, const int M)
{
  NCollection_Array1<double> k(1, 1);
  k(1) = myVKnots.Value(VIndex);
  NCollection_Array1<int> m(1, 1);
  m(1) = M - myVMults.Value(VIndex);
  InsertVKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom_BSplineSurface::IncreaseVMultiplicity(const int FromI1, const int ToI2, const int M)
{
  NCollection_Array1<double> k(myVKnots(FromI1), FromI1, ToI2);
  NCollection_Array1<int>    m(FromI1, ToI2);
  for (int i = FromI1; i <= ToI2; i++)
    m(i) = M - myVMults.Value(i);
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
  if (myUPeriodic)
  {
    double aUPeriod = myUKnots.Last() - myUKnots.First();
    if (deltaU - aUPeriod > Precision::PConfusion())
      throw Standard_DomainError("Geom_BSplineSurface::Segment");
    if (deltaU > aUPeriod)
      deltaU = aUPeriod;
  }

  double deltaV = V2 - V1;
  if (myVPeriodic)
  {
    double aVPeriod = myVKnots.Last() - myVKnots.First();
    if (deltaV - aVPeriod > Precision::PConfusion())
      throw Standard_DomainError("Geom_BSplineSurface::Segment");
    if (deltaV > aVPeriod)
      deltaV = aVPeriod;
  }

  double NewU1, NewU2, NewV1, NewV2;
  double U, V;
  int    indexU, indexV;

  indexU = 0;
  BSplCLib::LocateParameter(myUDeg,
                            myUKnots,
                            myUMults,
                            U1,
                            myUPeriodic,
                            myUKnots.Lower(),
                            myUKnots.Upper(),
                            indexU,
                            NewU1);
  indexU = 0;
  BSplCLib::LocateParameter(myUDeg,
                            myUKnots,
                            myUMults,
                            U2,
                            myUPeriodic,
                            myUKnots.Lower(),
                            myUKnots.Upper(),
                            indexU,
                            NewU2);
  if (SegmentInU)
  {
    // inserting the UKnots
    NCollection_Array1<double> UKnots(1, 2);
    NCollection_Array1<int>    UMults(1, 2);
    UKnots(1) = std::min(NewU1, NewU2);
    UKnots(2) = std::max(NewU1, NewU2);
    UMults(1) = UMults(2) = myUDeg;

    InsertUKnots(UKnots, UMults, EpsU);
  }

  indexV = 0;
  BSplCLib::LocateParameter(myVDeg,
                            myVKnots,
                            myVMults,
                            V1,
                            myVPeriodic,
                            myVKnots.Lower(),
                            myVKnots.Upper(),
                            indexV,
                            NewV1);
  indexV = 0;
  BSplCLib::LocateParameter(myVDeg,
                            myVKnots,
                            myVMults,
                            V2,
                            myVPeriodic,
                            myVKnots.Lower(),
                            myVKnots.Upper(),
                            indexV,
                            NewV2);
  if (SegmentInV)
  {
    // Inserting the VKnots
    NCollection_Array1<double> VKnots(1, 2);
    NCollection_Array1<int>    VMults(1, 2);

    VKnots(1) = std::min(NewV1, NewV2);
    VKnots(2) = std::max(NewV1, NewV2);
    VMults(1) = VMults(2) = myVDeg;
    InsertVKnots(VKnots, VMults, EpsV);
  }

  if (myUPeriodic && SegmentInU)
  { // set the origine at NewU1
    int index = 0;
    BSplCLib::LocateParameter(myUDeg,
                              myUKnots,
                              myUMults,
                              U1,
                              myUPeriodic,
                              myUKnots.Lower(),
                              myUKnots.Upper(),
                              index,
                              U);
    if (std::abs(myUKnots.Value(index + 1) - U) <= EpsU)
      index++;
    SetUOrigin(index);
    SetUNotPeriodic();
  }

  // compute index1 and index2 to set the new knots and mults
  int index1U = 0, index2U = 0;
  int FromU1 = myUKnots.Lower();
  int ToU2   = myUKnots.Upper();
  BSplCLib::LocateParameter(myUDeg,
                            myUKnots,
                            myUMults,
                            NewU1,
                            myUPeriodic,
                            FromU1,
                            ToU2,
                            index1U,
                            U);
  if (std::abs(myUKnots.Value(index1U + 1) - U) <= EpsU)
    index1U++;
  BSplCLib::LocateParameter(myUDeg,
                            myUKnots,
                            myUMults,
                            NewU1 + deltaU,
                            myUPeriodic,
                            FromU1,
                            ToU2,
                            index2U,
                            U);
  if (std::abs(myUKnots.Value(index2U + 1) - U) <= EpsU || index2U == index1U)
    index2U++;

  int nbuknots = index2U - index1U + 1;

  NCollection_Array1<double> nuknots(1, nbuknots);
  NCollection_Array1<int>    numults(1, nbuknots);

  int i, k = 1;
  for (i = index1U; i <= index2U; i++)
  {
    nuknots.SetValue(k, myUKnots.Value(i));
    numults.SetValue(k, myUMults.Value(i));
    k++;
  }
  if (SegmentInU)
  {
    numults.SetValue(1, myUDeg + 1);
    numults.SetValue(nbuknots, myUDeg + 1);
  }

  if (myVPeriodic && SegmentInV)
  { // set the origine at NewV1
    int index = 0;
    BSplCLib::LocateParameter(myVDeg,
                              myVKnots,
                              myVMults,
                              V1,
                              myVPeriodic,
                              myVKnots.Lower(),
                              myVKnots.Upper(),
                              index,
                              V);
    if (std::abs(myVKnots.Value(index + 1) - V) <= EpsV)
      index++;
    SetVOrigin(index);
    SetVNotPeriodic();
  }

  // compute index1 and index2 to set the new knots and mults
  int index1V = 0, index2V = 0;
  int FromV1 = myVKnots.Lower();
  int ToV2   = myVKnots.Upper();
  BSplCLib::LocateParameter(myVDeg,
                            myVKnots,
                            myVMults,
                            NewV1,
                            myVPeriodic,
                            FromV1,
                            ToV2,
                            index1V,
                            V);
  if (std::abs(myVKnots.Value(index1V + 1) - V) <= EpsV)
    index1V++;
  BSplCLib::LocateParameter(myVDeg,
                            myVKnots,
                            myVMults,
                            NewV1 + deltaV,
                            myVPeriodic,
                            FromV1,
                            ToV2,
                            index2V,
                            V);
  if (std::abs(myVKnots.Value(index2V + 1) - V) <= EpsV || index2V == index1V)
    index2V++;

  int nbvknots = index2V - index1V + 1;

  NCollection_Array1<double> nvknots(1, nbvknots);
  NCollection_Array1<int>    nvmults(1, nbvknots);

  k = 1;
  for (i = index1V; i <= index2V; i++)
  {
    nvknots.SetValue(k, myVKnots.Value(i));
    nvmults.SetValue(k, myVMults.Value(i));
    k++;
  }
  if (SegmentInV)
  {
    nvmults.SetValue(1, myVDeg + 1);
    nvmults.SetValue(nbvknots, myVDeg + 1);
  }

  // compute index1 and index2 to set the new poles and weights
  int pindex1U = BSplCLib::PoleIndex(myUDeg, index1U, myUPeriodic, myUMults);
  int pindex2U = BSplCLib::PoleIndex(myUDeg, index2U, myUPeriodic, myUMults);

  pindex1U++;
  pindex2U = std::min(pindex2U + 1, myPoles.ColLength());

  int nbupoles = pindex2U - pindex1U + 1;

  // compute index1 and index2 to set the new poles and weights
  int pindex1V = BSplCLib::PoleIndex(myVDeg, index1V, myVPeriodic, myVMults);
  int pindex2V = BSplCLib::PoleIndex(myVDeg, index2V, myVPeriodic, myVMults);

  pindex1V++;
  pindex2V = std::min(pindex2V + 1, myPoles.RowLength());

  int nbvpoles = pindex2V - pindex1V + 1;

  NCollection_Array2<gp_Pnt> npoles(1, nbupoles, 1, nbvpoles);
  k = 1;
  int j, l;
  if (myURational || myVRational)
  {
    NCollection_Array2<double> nweights(1, nbupoles, 1, nbvpoles);
    for (i = pindex1U; i <= pindex2U; i++)
    {
      l = 1;
      for (j = pindex1V; j <= pindex2V; j++)
      {
        npoles.SetValue(k, l, myPoles.Value(i, j));
        nweights.SetValue(k, l, myWeights.Value(i, j));
        l++;
      }
      k++;
    }
    myWeights = std::move(nweights);
  }
  else
  {
    for (i = pindex1U; i <= pindex2U; i++)
    {
      l = 1;
      for (j = pindex1V; j <= pindex2V; j++)
      {
        npoles.SetValue(k, l, myPoles.Value(i, j));
        l++;
      }
      k++;
    }
    myWeights = BSplSLib::UnitWeights(nbupoles, nbvpoles);
  }

  myUKnots = std::move(nuknots);
  myUMults = std::move(numults);
  myVKnots = std::move(nvknots);
  myVMults = std::move(nvmults);
  myPoles  = std::move(npoles);

  myMaxDerivInvOk = false;
  updateUKnots();
  updateVKnots();
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
  segment_in_U      = (std::abs(U1 - myUKnots.Value(myUKnots.Lower())) > EpsU)
                 || (std::abs(U2 - myUKnots.Value(myUKnots.Upper())) > EpsU);
  segment_in_V = (std::abs(V1 - myVKnots.Value(myVKnots.Lower())) > EpsV)
                 || (std::abs(V2 - myVKnots.Value(myVKnots.Upper())) > EpsV);

  segment(U1, U2, V1, V2, EpsU, EpsV, segment_in_U, segment_in_V);
}

//=================================================================================================

void Geom_BSplineSurface::SetUKnot(const int UIndex, const double K)
{
  if (UIndex < 1 || UIndex > myUKnots.Length())
    throw Standard_OutOfRange("Geom_BSplineSurface::SetUKnot: Index and #knots mismatch");

  int    NewIndex = UIndex;
  double DU       = std::abs(Epsilon(K));
  if (UIndex == 1)
  {
    if (K >= myUKnots.Value(2) - DU)
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnot: K out of range");
  }
  else if (UIndex == myUKnots.Length())
  {
    if (K <= myUKnots.Value(myUKnots.Length() - 1) + DU)
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnot: K out of range");
    }
  }
  else
  {
    if (K <= myUKnots.Value(NewIndex - 1) + DU || K >= myUKnots.Value(NewIndex + 1) - DU)
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnot: K out of range");
    }
  }

  if (K != myUKnots.Value(NewIndex))
  {
    myUKnots.SetValue(NewIndex, K);
    myMaxDerivInvOk = false;
    updateUKnots();
  }
}

//=================================================================================================

void Geom_BSplineSurface::SetUKnots(const NCollection_Array1<double>& UK)
{

  int Lower = UK.Lower();
  int Upper = UK.Upper();
  if (Lower < 1 || Lower > myUKnots.Length() || Upper < 1 || Upper > myUKnots.Length())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetUKnots: invalid array dimension");
  }
  if (Lower > 1)
  {
    if (std::abs(UK(Lower) - myUKnots.Value(Lower - 1)) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnots: invalid knot value");
    }
  }
  if (Upper < myUKnots.Length())
  {
    if (std::abs(UK(Upper) - myUKnots.Value(Upper + 1)) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnots: invalid knot value");
    }
  }
  double K1 = UK(Lower);
  for (int i = Lower; i <= Upper; i++)
  {
    myUKnots.SetValue(i, UK(i));
    if (i != Lower)
    {
      if (std::abs(UK(i) - K1) <= gp::Resolution())
      {
        throw Standard_ConstructionError("Geom_BSplineSurface::SetUKnots: invalid knot value");
      }
      K1 = UK(i);
    }
  }

  myMaxDerivInvOk = false;
  updateUKnots();
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
  if (VIndex < 1 || VIndex > myVKnots.Length())
    throw Standard_OutOfRange("Geom_BSplineSurface::SetVKnot: Index and #knots mismatch");
  int    NewIndex = VIndex + myVKnots.Lower() - 1;
  double DV       = std::abs(Epsilon(K));
  if (VIndex == 1)
  {
    if (K >= myVKnots.Value(2) - DV)
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnot: K out of range");
    }
  }
  else if (VIndex == myVKnots.Length())
  {
    if (K <= myVKnots.Value(myVKnots.Length() - 1) + DV)
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnot: K out of range");
    }
  }
  else
  {
    if (K <= myVKnots.Value(NewIndex - 1) + DV || K >= myVKnots.Value(NewIndex + 1) - DV)
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnot: K out of range");
    }
  }

  if (K != myVKnots.Value(NewIndex))
  {
    myVKnots.SetValue(NewIndex, K);
    myMaxDerivInvOk = false;
    updateVKnots();
  }
}

//=================================================================================================

void Geom_BSplineSurface::SetVKnots(const NCollection_Array1<double>& VK)
{

  int Lower = VK.Lower();
  int Upper = VK.Upper();
  if (Lower < 1 || Lower > myVKnots.Length() || Upper < 1 || Upper > myVKnots.Length())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetVKnots: invalid array dimension");
  }
  if (Lower > 1)
  {
    if (std::abs(VK(Lower) - myVKnots.Value(Lower - 1)) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnots: invalid knot value");
    }
  }
  if (Upper < myVKnots.Length())
  {
    if (std::abs(VK(Upper) - myVKnots.Value(Upper + 1)) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnots: invalid knot value");
    }
  }
  double K1 = VK(Lower);
  for (int i = Lower; i <= Upper; i++)
  {
    myVKnots.SetValue(i, VK(i));
    if (i != Lower)
    {
      if (std::abs(VK(i) - K1) <= gp::Resolution())
      {
        throw Standard_ConstructionError("Geom_BSplineSurface::SetVKnots: invalid knot value");
      }
      K1 = VK(i);
    }
  }

  myMaxDerivInvOk = false;
  updateVKnots();
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
  NCollection_Array1<double> k(myUKnots(FromI1), FromI1, ToI2);
  NCollection_Array1<int>    m(FromI1, ToI2);
  m.Init(Step);
  InsertUKnots(k, m, Epsilon(1.));
}

//=================================================================================================

void Geom_BSplineSurface::IncrementVMultiplicity(const int FromI1, const int ToI2, const int Step)
{
  NCollection_Array1<double> k(myVKnots(FromI1), FromI1, ToI2);

  NCollection_Array1<int> m(FromI1, ToI2);
  m.Init(Step);

  InsertVKnots(k, m, Epsilon(1.));
}

//=================================================================================================

void Geom_BSplineSurface::updateUKnots()
{
  myMaxDerivInvOk = false;

  int MaxKnotMult = 0;
  BSplCLib::KnotAnalysis(myUDeg, myUPeriodic, myUKnots, myUMults, myUKnotSet, MaxKnotMult);

  if (myUKnotSet == GeomAbs_Uniform && !myUPeriodic)
  {
    myUFlatKnots.Resize(myUKnots.Lower(), myUKnots.Upper(), false);
    myUFlatKnots.Assign(myUKnots);
  }
  else
  {
    myUFlatKnots.Resize(1, BSplCLib::KnotSequenceLength(myUMults, myUDeg, myUPeriodic), false);

    BSplCLib::KnotSequence(myUKnots, myUMults, myUDeg, myUPeriodic, myUFlatKnots);
  }

  if (MaxKnotMult == 0)
    myUSmooth = GeomAbs_CN;
  else
  {
    switch (myUDeg - MaxKnotMult)
    {
      case 0:
        myUSmooth = GeomAbs_C0;
        break;
      case 1:
        myUSmooth = GeomAbs_C1;
        break;
      case 2:
        myUSmooth = GeomAbs_C2;
        break;
      case 3:
        myUSmooth = GeomAbs_C3;
        break;
      default:
        myUSmooth = GeomAbs_C3;
        break;
    }
  }
}

//=================================================================================================

void Geom_BSplineSurface::updateVKnots()
{
  myMaxDerivInvOk = false;

  int MaxKnotMult = 0;
  BSplCLib::KnotAnalysis(myVDeg, myVPeriodic, myVKnots, myVMults, myVKnotSet, MaxKnotMult);

  if (myVKnotSet == GeomAbs_Uniform && !myVPeriodic)
  {
    myVFlatKnots.Resize(myVKnots.Lower(), myVKnots.Upper(), false);
    myVFlatKnots.Assign(myVKnots);
  }
  else
  {
    myVFlatKnots.Resize(1, BSplCLib::KnotSequenceLength(myVMults, myVDeg, myVPeriodic), false);

    BSplCLib::KnotSequence(myVKnots, myVMults, myVDeg, myVPeriodic, myVFlatKnots);
  }

  if (MaxKnotMult == 0)
    myVSmooth = GeomAbs_CN;
  else
  {
    switch (myVDeg - MaxKnotMult)
    {
      case 0:
        myVSmooth = GeomAbs_C0;
        break;
      case 1:
        myVSmooth = GeomAbs_C1;
        break;
      case 2:
        myVSmooth = GeomAbs_C2;
        break;
      case 3:
        myVSmooth = GeomAbs_C3;
        break;
      default:
        myVSmooth = GeomAbs_C3;
        break;
    }
  }
}

//=================================================================================================

void Geom_BSplineSurface::PeriodicNormalization(double& Uparameter, double& Vparameter) const
{
  double Period, aMaxVal, aMinVal;

  if (myUPeriodic)
  {
    aMaxVal    = myUFlatKnots.Value(myUFlatKnots.Upper() - myUDeg);
    aMinVal    = myUFlatKnots.Value(myUDeg + 1);
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
  if (myVPeriodic)
  {
    aMaxVal    = myVFlatKnots.Value(myVFlatKnots.Upper() - myVDeg);
    aMinVal    = myVFlatKnots.Value(myVDeg + 1);
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
  if (UIndex < 1 || UIndex > myPoles.ColLength() || VIndex < 1 || VIndex > myPoles.RowLength())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetWeight: Index and #pole mismatch");
  }
  if (!myURational && !myVRational)
  {
    // Make an owned copy of the unit weights view before modifying.
    myWeights = NCollection_Array2<double>(myWeights);
  }
  myWeights(UIndex + myWeights.LowerRow() - 1, VIndex + myWeights.LowerCol() - 1) = Weight;
  Rational(myWeights, myURational, myVRational);
  if (!myURational && !myVRational)
    myWeights = BSplSLib::UnitWeights(myPoles.ColLength(), myPoles.RowLength());
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BSplineSurface::SetWeightCol(const int                         VIndex,
                                       const NCollection_Array1<double>& CPoleWeights)
{
  if (VIndex < 1 || VIndex > myPoles.RowLength())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetWeightCol: Index and #pole mismatch");
  }
  if (CPoleWeights.Lower() < 1 || CPoleWeights.Lower() > myPoles.ColLength()
      || CPoleWeights.Upper() < 1 || CPoleWeights.Upper() > myPoles.ColLength())
  {
    throw Standard_ConstructionError("Geom_BSplineSurface::SetWeightCol: invalid array dimension");
  }
  if (!myURational && !myVRational)
  {
    // Make an owned copy of the unit weights view before modifying.
    myWeights = NCollection_Array2<double>(myWeights);
  }
  int I = CPoleWeights.Lower();
  while (I <= CPoleWeights.Upper())
  {
    if (CPoleWeights(I) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetWeightCol: Weight too small");
    }
    myWeights(I + myWeights.LowerRow() - 1, VIndex + myWeights.LowerCol() - 1) = CPoleWeights(I);
    I++;
  }
  // Verifie si c'est rationnel
  Rational(myWeights, myURational, myVRational);
  if (!myURational && !myVRational)
    myWeights = BSplSLib::UnitWeights(myPoles.ColLength(), myPoles.RowLength());
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BSplineSurface::SetWeightRow(const int                         UIndex,
                                       const NCollection_Array1<double>& CPoleWeights)
{
  if (UIndex < 1 || UIndex > myPoles.ColLength())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetWeightRow: Index and #pole mismatch");
  }
  if (CPoleWeights.Lower() < 1 || CPoleWeights.Lower() > myPoles.RowLength()
      || CPoleWeights.Upper() < 1 || CPoleWeights.Upper() > myPoles.RowLength())
  {

    throw Standard_ConstructionError("Geom_BSplineSurface::SetWeightRow: invalid array dimension");
  }
  if (!myURational && !myVRational)
  {
    // Make an owned copy of the unit weights view before modifying.
    myWeights = NCollection_Array2<double>(myWeights);
  }
  int I = CPoleWeights.Lower();

  while (I <= CPoleWeights.Upper())
  {
    if (CPoleWeights(I) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom_BSplineSurface::SetWeightRow: Weight too small");
    }
    myWeights(UIndex + myWeights.LowerRow() - 1, I + myWeights.LowerCol() - 1) = CPoleWeights(I);
    I++;
  }
  // Verifie si c'est rationnel
  Rational(myWeights, myURational, myVRational);
  if (!myURational && !myVRational)
    myWeights = BSplSLib::UnitWeights(myPoles.ColLength(), myPoles.RowLength());
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BSplineSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedSurface)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myURational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVRational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myUPeriodic)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVPeriodic)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myUKnotSet)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVKnotSet)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myUSmooth)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVSmooth)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myUDeg)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVDeg)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPoles.Size())
  if (myURational || myVRational)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myWeights.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myUFlatKnots.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVFlatKnots.Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myUKnots.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVKnots.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myUMults.Size())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVMults.Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myUMaxDerivInv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVMaxDerivInv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMaxDerivInvOk)
}
