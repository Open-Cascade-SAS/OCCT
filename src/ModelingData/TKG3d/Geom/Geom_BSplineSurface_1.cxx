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

// xab : 30-Mar-95 introduced cache mechanism for surfaces
// xab : 21-Jun-95 in remove knots sync size of weights and poles
// pmn : 28-Jun-96 Distinction entre la continuite en U et V (bug PRO4625)
// pmn : 07-Jan-97 Centralisation des verif rational (PRO6834)
//       et ajout des InvalideCache() dans les SetPole* (PRO6833)
// pmn : 03-Feb-97 Prise en compte de la periode dans Locate(U/V) (PRO6963)
//                 + bon appel a LocateParameter (PRO6973).
// RBD : 15/10/98 ; Le cache est desormais defini sur [-1,1] (pro15537).

#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include "Geom_EvalRepSurfaceDesc.hxx"
#include "Geom_EvalRepUtils.pxx"
#include <Geom_UndefinedDerivative.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_RangeError.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_Array1.hxx>

//=================================================================================================

bool Geom_BSplineSurface::IsCNu(const int N) const
{
  Standard_RangeError_Raise_if(N < 0, " ");
  switch (myUSmooth)
  {
    case GeomAbs_CN:
      return true;
    case GeomAbs_C0:
      return N <= 0;
    case GeomAbs_G1:
      return N <= 0;
    case GeomAbs_C1:
      return N <= 1;
    case GeomAbs_G2:
      return N <= 1;
    case GeomAbs_C2:
      return N <= 2;
    case GeomAbs_C3:
      return N <= 3 ? true
                    : N <= myUDeg
                             - BSplCLib::MaxKnotMult(myUMults,
                                                     myUMults.Lower() + 1,
                                                     myUMults.Upper() - 1);
    default:
      return false;
  }
}

//=================================================================================================

bool Geom_BSplineSurface::IsCNv(const int N) const
{
  Standard_RangeError_Raise_if(N < 0, " ");

  switch (myVSmooth)
  {
    case GeomAbs_CN:
      return true;
    case GeomAbs_C0:
      return N <= 0;
    case GeomAbs_G1:
      return N <= 0;
    case GeomAbs_C1:
      return N <= 1;
    case GeomAbs_G2:
      return N <= 1;
    case GeomAbs_C2:
      return N <= 2;
    case GeomAbs_C3:
      return N <= 3 ? true
                    : N <= myVDeg
                             - BSplCLib::MaxKnotMult(myVMults,
                                                     myVMults.Lower() + 1,
                                                     myVMults.Upper() - 1);
    default:
      return false;
  }
}

//=================================================================================================

std::optional<gp_Pnt> Geom_BSplineSurface::EvalD0(const double U, const double V) const
{
  if (const std::optional<gp_Pnt> aEvalRepResult =
        Geom_EvalRepUtils::TryEvalSurfaceD0(myEvalRep, U, V);
      aEvalRepResult.has_value())
  {
    return aEvalRepResult;
  }

  double aNewU = U;
  double aNewV = V;
  PeriodicNormalization(aNewU, aNewV);

  gp_Pnt P;
  BSplSLib::D0(aNewU,
               aNewV,
               0,
               0,
               myPoles,
               Weights(),
               myUFlatKnots,
               myVFlatKnots,
               BSplCLib::NoMults(),
               BSplCLib::NoMults(),
               myUDeg,
               myVDeg,
               myURational,
               myVRational,
               myUPeriodic,
               myVPeriodic,
               P);
  return P;
}

//=================================================================================================

std::optional<Geom_Surface::ResD1> Geom_BSplineSurface::EvalD1(const double U, const double V) const
{
  if (const std::optional<Geom_Surface::ResD1> aEvalRepResult =
        Geom_EvalRepUtils::TryEvalSurfaceD1(myEvalRep, U, V);
      aEvalRepResult.has_value())
  {
    return aEvalRepResult;
  }

  double aNewU = U;
  double aNewV = V;
  PeriodicNormalization(aNewU, aNewV);

  int uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(myUDeg, myUKnots, &myUMults, U, myUPeriodic, uindex, aNewU);
  uindex = BSplCLib::FlatIndex(myUDeg, uindex, myUMults, myUPeriodic);

  BSplCLib::LocateParameter(myVDeg, myVKnots, &myVMults, V, myVPeriodic, vindex, aNewV);
  vindex = BSplCLib::FlatIndex(myVDeg, vindex, myVMults, myVPeriodic);

  std::optional<Geom_Surface::ResD1> aResult{std::in_place};
  BSplSLib::D1(aNewU,
               aNewV,
               uindex,
               vindex,
               myPoles,
               Weights(),
               myUFlatKnots,
               myVFlatKnots,
               BSplCLib::NoMults(),
               BSplCLib::NoMults(),
               myUDeg,
               myVDeg,
               myURational,
               myVRational,
               myUPeriodic,
               myVPeriodic,
               aResult->Point,
               aResult->D1U,
               aResult->D1V);
  return aResult;
}

//=================================================================================================

std::optional<Geom_Surface::ResD2> Geom_BSplineSurface::EvalD2(const double U, const double V) const
{
  if (const std::optional<Geom_Surface::ResD2> aEvalRepResult =
        Geom_EvalRepUtils::TryEvalSurfaceD2(myEvalRep, U, V);
      aEvalRepResult.has_value())
  {
    return aEvalRepResult;
  }

  double aNewU = U;
  double aNewV = V;
  PeriodicNormalization(aNewU, aNewV);

  int uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(myUDeg, myUKnots, &myUMults, U, myUPeriodic, uindex, aNewU);
  uindex = BSplCLib::FlatIndex(myUDeg, uindex, myUMults, myUPeriodic);

  BSplCLib::LocateParameter(myVDeg, myVKnots, &myVMults, V, myVPeriodic, vindex, aNewV);
  vindex = BSplCLib::FlatIndex(myVDeg, vindex, myVMults, myVPeriodic);

  std::optional<Geom_Surface::ResD2> aResult{std::in_place};
  BSplSLib::D2(aNewU,
               aNewV,
               uindex,
               vindex,
               myPoles,
               Weights(),
               myUFlatKnots,
               myVFlatKnots,
               BSplCLib::NoMults(),
               BSplCLib::NoMults(),
               myUDeg,
               myVDeg,
               myURational,
               myVRational,
               myUPeriodic,
               myVPeriodic,
               aResult->Point,
               aResult->D1U,
               aResult->D1V,
               aResult->D2U,
               aResult->D2V,
               aResult->D2UV);
  return aResult;
}

//=================================================================================================

std::optional<Geom_Surface::ResD3> Geom_BSplineSurface::EvalD3(const double U, const double V) const
{
  if (const std::optional<Geom_Surface::ResD3> aEvalRepResult =
        Geom_EvalRepUtils::TryEvalSurfaceD3(myEvalRep, U, V);
      aEvalRepResult.has_value())
  {
    return aEvalRepResult;
  }

  std::optional<Geom_Surface::ResD3> aResult{std::in_place};
  BSplSLib::D3(U,
               V,
               0,
               0,
               myPoles,
               Weights(),
               myUFlatKnots,
               myVFlatKnots,
               BSplCLib::NoMults(),
               BSplCLib::NoMults(),
               myUDeg,
               myVDeg,
               myURational,
               myVRational,
               myUPeriodic,
               myVPeriodic,
               aResult->Point,
               aResult->D1U,
               aResult->D1V,
               aResult->D2U,
               aResult->D2V,
               aResult->D2UV,
               aResult->D3U,
               aResult->D3V,
               aResult->D3UUV,
               aResult->D3UVV);
  return aResult;
}

//=================================================================================================

std::optional<gp_Vec> Geom_BSplineSurface::EvalDN(const double U,
                                                  const double V,
                                                  const int    Nu,
                                                  const int    Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
    return std::nullopt;

  if (const std::optional<gp_Vec> aEvalRepResult =
        Geom_EvalRepUtils::TryEvalSurfaceDN(myEvalRep, U, V, Nu, Nv);
      aEvalRepResult.has_value())
  {
    return aEvalRepResult;
  }

  gp_Vec Vn;
  BSplSLib::DN(U,
               V,
               Nu,
               Nv,
               0,
               0,
               myPoles,
               Weights(),
               myUFlatKnots,
               myVFlatKnots,
               BSplCLib::NoMults(),
               BSplCLib::NoMults(),
               myUDeg,
               myVDeg,
               myURational,
               myVRational,
               myUPeriodic,
               myVPeriodic,
               Vn);
  return Vn;
}

//=================================================================================================

gp_Pnt Geom_BSplineSurface::LocalValue(const double U,
                                       const double V,
                                       const int    FromUK1,
                                       const int    ToUK2,
                                       const int    FromVK1,
                                       const int    ToVK2) const
{
  gp_Pnt P;
  LocalD0(U, V, FromUK1, ToUK2, FromVK1, ToVK2, P);
  return P;
}

//=================================================================================================

void Geom_BSplineSurface::LocalD0(const double U,
                                  const double V,
                                  const int    FromUK1,
                                  const int    ToUK2,
                                  const int    FromVK1,
                                  const int    ToVK2,
                                  gp_Pnt&      P) const
{
  Standard_DomainError_Raise_if(FromUK1 == ToUK2 || FromVK1 == ToVK2,
                                "Geom_BSplineSurface::LocalD0");

  double u = U, v = V;
  int    uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(myUDeg, myUFlatKnots, U, myUPeriodic, FromUK1, ToUK2, uindex, u);
  uindex = BSplCLib::FlatIndex(myUDeg, uindex, myUMults, myUPeriodic);

  BSplCLib::LocateParameter(myVDeg, myVFlatKnots, V, myVPeriodic, FromVK1, ToVK2, vindex, v);
  vindex = BSplCLib::FlatIndex(myVDeg, vindex, myVMults, myVPeriodic);

  BSplSLib::D0(u,
               v,
               uindex,
               vindex,
               myPoles,
               Weights(),
               myUFlatKnots,
               myVFlatKnots,
               BSplCLib::NoMults(),
               BSplCLib::NoMults(),
               myUDeg,
               myVDeg,
               myURational,
               myVRational,
               myUPeriodic,
               myVPeriodic,
               P);
}

//=================================================================================================

void Geom_BSplineSurface::LocalD1(const double U,
                                  const double V,
                                  const int    FromUK1,
                                  const int    ToUK2,
                                  const int    FromVK1,
                                  const int    ToVK2,
                                  gp_Pnt&      P,
                                  gp_Vec&      D1U,
                                  gp_Vec&      D1V) const
{
  Standard_DomainError_Raise_if(FromUK1 == ToUK2 || FromVK1 == ToVK2,
                                "Geom_BSplineSurface::LocalD1");

  double u = U, v = V;
  int    uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(myUDeg, myUFlatKnots, U, myUPeriodic, FromUK1, ToUK2, uindex, u);
  uindex = BSplCLib::FlatIndex(myUDeg, uindex, myUMults, myUPeriodic);

  BSplCLib::LocateParameter(myVDeg, myVFlatKnots, V, myVPeriodic, FromVK1, ToVK2, vindex, v);
  vindex = BSplCLib::FlatIndex(myVDeg, vindex, myVMults, myVPeriodic);

  BSplSLib::D1(u,
               v,
               uindex,
               vindex,
               myPoles,
               Weights(),
               myUFlatKnots,
               myVFlatKnots,
               BSplCLib::NoMults(),
               BSplCLib::NoMults(),
               myUDeg,
               myVDeg,
               myURational,
               myVRational,
               myUPeriodic,
               myVPeriodic,
               P,
               D1U,
               D1V);
}

//=================================================================================================

void Geom_BSplineSurface::LocalD2(const double U,
                                  const double V,
                                  const int    FromUK1,
                                  const int    ToUK2,
                                  const int    FromVK1,
                                  const int    ToVK2,
                                  gp_Pnt&      P,
                                  gp_Vec&      D1U,
                                  gp_Vec&      D1V,
                                  gp_Vec&      D2U,
                                  gp_Vec&      D2V,
                                  gp_Vec&      D2UV) const
{
  Standard_DomainError_Raise_if(FromUK1 == ToUK2 || FromVK1 == ToVK2,
                                "Geom_BSplineSurface::LocalD2");

  double u = U, v = V;
  int    uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(myUDeg, myUFlatKnots, U, myUPeriodic, FromUK1, ToUK2, uindex, u);
  uindex = BSplCLib::FlatIndex(myUDeg, uindex, myUMults, myUPeriodic);

  BSplCLib::LocateParameter(myVDeg, myVFlatKnots, V, myVPeriodic, FromVK1, ToVK2, vindex, v);
  vindex = BSplCLib::FlatIndex(myVDeg, vindex, myVMults, myVPeriodic);

  BSplSLib::D2(u,
               v,
               uindex,
               vindex,
               myPoles,
               Weights(),
               myUFlatKnots,
               myVFlatKnots,
               BSplCLib::NoMults(),
               BSplCLib::NoMults(),
               myUDeg,
               myVDeg,
               myURational,
               myVRational,
               myUPeriodic,
               myVPeriodic,
               P,
               D1U,
               D1V,
               D2U,
               D2V,
               D2UV);
}

//=================================================================================================

void Geom_BSplineSurface::LocalD3(const double U,
                                  const double V,
                                  const int    FromUK1,
                                  const int    ToUK2,
                                  const int    FromVK1,
                                  const int    ToVK2,
                                  gp_Pnt&      P,
                                  gp_Vec&      D1U,
                                  gp_Vec&      D1V,
                                  gp_Vec&      D2U,
                                  gp_Vec&      D2V,
                                  gp_Vec&      D2UV,
                                  gp_Vec&      D3U,
                                  gp_Vec&      D3V,
                                  gp_Vec&      D3UUV,
                                  gp_Vec&      D3UVV) const
{
  Standard_DomainError_Raise_if(FromUK1 == ToUK2 || FromVK1 == ToVK2,
                                "Geom_BSplineSurface::LocalD3");

  double u = U, v = V;
  int    uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(myUDeg, myUFlatKnots, U, myUPeriodic, FromUK1, ToUK2, uindex, u);
  uindex = BSplCLib::FlatIndex(myUDeg, uindex, myUMults, myUPeriodic);

  BSplCLib::LocateParameter(myVDeg, myVFlatKnots, V, myVPeriodic, FromVK1, ToVK2, vindex, v);
  vindex = BSplCLib::FlatIndex(myVDeg, vindex, myVMults, myVPeriodic);

  BSplSLib::D3(u,
               v,
               uindex,
               vindex,
               myPoles,
               Weights(),
               myUFlatKnots,
               myVFlatKnots,
               BSplCLib::NoMults(),
               BSplCLib::NoMults(),
               myUDeg,
               myVDeg,
               myURational,
               myVRational,
               myUPeriodic,
               myVPeriodic,
               P,
               D1U,
               D1V,
               D2U,
               D2V,
               D2UV,
               D3U,
               D3V,
               D3UUV,
               D3UVV);
}

//=================================================================================================

gp_Vec Geom_BSplineSurface::LocalDN(const double U,
                                    const double V,
                                    const int    FromUK1,
                                    const int    ToUK2,
                                    const int    FromVK1,
                                    const int    ToVK2,
                                    const int    Nu,
                                    const int    Nv) const
{
  Standard_DomainError_Raise_if(FromUK1 == ToUK2 || FromVK1 == ToVK2,
                                "Geom_BSplineSurface::LocalDN");

  double u = U, v = V;
  int    uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(myUDeg, myUFlatKnots, U, myUPeriodic, FromUK1, ToUK2, uindex, u);
  uindex = BSplCLib::FlatIndex(myUDeg, uindex, myUMults, myUPeriodic);

  BSplCLib::LocateParameter(myVDeg, myVFlatKnots, V, myVPeriodic, FromVK1, ToVK2, vindex, v);
  vindex = BSplCLib::FlatIndex(myVDeg, vindex, myVMults, myVPeriodic);

  gp_Vec Vn;
  BSplSLib::DN(u,
               v,
               Nu,
               Nv,
               uindex,
               vindex,
               myPoles,
               Weights(),
               myUFlatKnots,
               myVFlatKnots,
               BSplCLib::NoMults(),
               BSplCLib::NoMults(),
               myUDeg,
               myVDeg,
               myURational,
               myVRational,
               myUPeriodic,
               myVPeriodic,
               Vn);
  return Vn;
}

//=================================================================================================

const gp_Pnt& Geom_BSplineSurface::Pole(const int UIndex, const int VIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > myPoles.ColLength() || VIndex < 1
                                 || VIndex > myPoles.RowLength(),
                               " ");
  return myPoles.Value(UIndex, VIndex);
}

//=================================================================================================

void Geom_BSplineSurface::Poles(NCollection_Array2<gp_Pnt>& P) const
{
  Standard_DimensionError_Raise_if(P.ColLength() != myPoles.ColLength()
                                     || P.RowLength() != myPoles.RowLength(),
                                   " ");
  P = myPoles;
}

const NCollection_Array2<gp_Pnt>& Geom_BSplineSurface::Poles() const
{
  return myPoles;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BSplineSurface::UIso(const double U) const
{
  NCollection_Array1<gp_Pnt> cpoles(1, myPoles.RowLength());
  NCollection_Array1<double> cweights(1, myPoles.RowLength());

  occ::handle<Geom_BSplineCurve> C;

  if (myURational || myVRational)
  {
    BSplSLib::Iso(U,
                  true,
                  myPoles,
                  Weights(),
                  myUFlatKnots,
                  BSplCLib::NoMults(),
                  myUDeg,
                  myUPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles, cweights, myVKnots, myVMults, myVDeg, myVPeriodic);
  }
  else
  {
    BSplSLib::Iso(U,
                  true,
                  myPoles,
                  BSplSLib::NoWeights(),
                  myUFlatKnots,
                  BSplCLib::NoMults(),
                  myUDeg,
                  myUPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles, myVKnots, myVMults, myVDeg, myVPeriodic);
  }

  return C;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BSplineSurface::UIso(const double U, const bool CheckRational) const
{
  NCollection_Array1<gp_Pnt> cpoles(1, myPoles.RowLength());
  NCollection_Array1<double> cweights(1, myPoles.RowLength());

  occ::handle<Geom_BSplineCurve> C;

  if (myURational || myVRational)
  {
    BSplSLib::Iso(U,
                  true,
                  myPoles,
                  Weights(),
                  myUFlatKnots,
                  BSplCLib::NoMults(),
                  myUDeg,
                  myUPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles,
                              cweights,
                              myVKnots,
                              myVMults,
                              myVDeg,
                              myVPeriodic,
                              CheckRational);
  }
  else
  {
    BSplSLib::Iso(U,
                  true,
                  myPoles,
                  BSplSLib::NoWeights(),
                  myUFlatKnots,
                  BSplCLib::NoMults(),
                  myUDeg,
                  myUPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles, myVKnots, myVMults, myVDeg, myVPeriodic);
  }

  return C;
}

//=================================================================================================

double Geom_BSplineSurface::UKnot(const int UIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > myUKnots.Length(), " ");
  return myUKnots.Value(UIndex);
}

//=================================================================================================

double Geom_BSplineSurface::VKnot(const int VIndex) const
{
  Standard_OutOfRange_Raise_if(VIndex < 1 || VIndex > myVKnots.Length(), " ");
  return myVKnots.Value(VIndex);
}

//=================================================================================================

void Geom_BSplineSurface::UKnots(NCollection_Array1<double>& Ku) const
{
  Standard_DimensionError_Raise_if(Ku.Length() != myUKnots.Length(), " ");
  Ku = myUKnots;
}

const NCollection_Array1<double>& Geom_BSplineSurface::UKnots() const
{
  return myUKnots;
}

//=================================================================================================

void Geom_BSplineSurface::VKnots(NCollection_Array1<double>& Kv) const
{
  Standard_DimensionError_Raise_if(Kv.Length() != myVKnots.Length(), " ");
  Kv = myVKnots;
}

const NCollection_Array1<double>& Geom_BSplineSurface::VKnots() const
{
  return myVKnots;
}

//=================================================================================================

void Geom_BSplineSurface::UKnotSequence(NCollection_Array1<double>& Ku) const
{
  Standard_DimensionError_Raise_if(Ku.Length() != myUFlatKnots.Length(), " ");
  Ku = myUFlatKnots;
}

const NCollection_Array1<double>& Geom_BSplineSurface::UKnotSequence() const
{
  return myUFlatKnots;
}

//=================================================================================================

void Geom_BSplineSurface::VKnotSequence(NCollection_Array1<double>& Kv) const
{
  Standard_DimensionError_Raise_if(Kv.Length() != myVFlatKnots.Length(), " ");
  Kv = myVFlatKnots;
}

const NCollection_Array1<double>& Geom_BSplineSurface::VKnotSequence() const
{
  return myVFlatKnots;
}

//=================================================================================================

int Geom_BSplineSurface::UMultiplicity(const int UIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > myUMults.Length(), " ");
  return myUMults.Value(UIndex);
}

//=================================================================================================

void Geom_BSplineSurface::UMultiplicities(NCollection_Array1<int>& Mu) const
{
  Standard_DimensionError_Raise_if(Mu.Length() != myUMults.Length(), " ");
  Mu = myUMults;
}

const NCollection_Array1<int>& Geom_BSplineSurface::UMultiplicities() const
{
  return myUMults;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BSplineSurface::VIso(const double V) const
{
  NCollection_Array1<gp_Pnt> cpoles(1, myPoles.ColLength());
  NCollection_Array1<double> cweights(1, myPoles.ColLength());

  occ::handle<Geom_BSplineCurve> C;

  if (myURational || myVRational)
  {
    BSplSLib::Iso(V,
                  false,
                  myPoles,
                  Weights(),
                  myVFlatKnots,
                  BSplCLib::NoMults(),
                  myVDeg,
                  myVPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles, cweights, myUKnots, myUMults, myUDeg, myUPeriodic);
  }
  else
  {
    BSplSLib::Iso(V,
                  false,
                  myPoles,
                  BSplSLib::NoWeights(),
                  myVFlatKnots,
                  BSplCLib::NoMults(),
                  myVDeg,
                  myVPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles, myUKnots, myUMults, myUDeg, myUPeriodic);
  }

  return C;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BSplineSurface::VIso(const double V, const bool CheckRational) const
{
  NCollection_Array1<gp_Pnt> cpoles(1, myPoles.ColLength());
  NCollection_Array1<double> cweights(1, myPoles.ColLength());

  occ::handle<Geom_BSplineCurve> C;

  if (myURational || myVRational)
  {
    BSplSLib::Iso(V,
                  false,
                  myPoles,
                  Weights(),
                  myVFlatKnots,
                  BSplCLib::NoMults(),
                  myVDeg,
                  myVPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles,
                              cweights,
                              myUKnots,
                              myUMults,
                              myUDeg,
                              myUPeriodic,
                              CheckRational);
  }
  else
  {
    BSplSLib::Iso(V,
                  false,
                  myPoles,
                  BSplSLib::NoWeights(),
                  myVFlatKnots,
                  BSplCLib::NoMults(),
                  myVDeg,
                  myVPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles, myUKnots, myUMults, myUDeg, myUPeriodic);
  }

  return C;
}

//=================================================================================================

int Geom_BSplineSurface::VMultiplicity(const int VIndex) const
{
  Standard_OutOfRange_Raise_if(VIndex < 1 || VIndex > myVMults.Length(), " ");
  return myVMults.Value(VIndex);
}

//=================================================================================================

void Geom_BSplineSurface::VMultiplicities(NCollection_Array1<int>& Mv) const
{
  Standard_DimensionError_Raise_if(Mv.Length() != myVMults.Length(), " ");
  Mv = myVMults;
}

const NCollection_Array1<int>& Geom_BSplineSurface::VMultiplicities() const
{
  return myVMults;
}

//=================================================================================================

double Geom_BSplineSurface::Weight(const int UIndex, const int VIndex) const
{
  if (!(myURational || myVRational))
    return 1.0;
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > myWeights.ColLength() || VIndex < 1
                                 || VIndex > myWeights.RowLength(),
                               " ");
  return myWeights.Value(UIndex, VIndex);
}

//=================================================================================================

void Geom_BSplineSurface::Weights(NCollection_Array2<double>& W) const
{
  if (!(myURational || myVRational))
  {
    Standard_DimensionError_Raise_if(W.ColLength() != myPoles.ColLength()
                                       || W.RowLength() != myPoles.RowLength(),
                                     " ");
    W.Init(1.0);
    return;
  }
  Standard_DimensionError_Raise_if(W.ColLength() != myWeights.ColLength()
                                     || W.RowLength() != myWeights.RowLength(),
                                   " ");
  W = myWeights;
}

const NCollection_Array2<double>* Geom_BSplineSurface::Weights() const
{
  if (myURational || myVRational)
    return &myWeights;
  return BSplSLib::NoWeights();
}

//=================================================================================================

void Geom_BSplineSurface::Transform(const gp_Trsf& T)
{
  ClearEvalRepresentation();
  for (int j = myPoles.LowerCol(); j <= myPoles.UpperCol(); j++)
  {
    for (int i = myPoles.LowerRow(); i <= myPoles.UpperRow(); i++)
    {
      myPoles(i, j).Transform(T);
    }
  }
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BSplineSurface::SetUPeriodic()
{
  ClearEvalRepresentation();
  int first = FirstUKnotIndex();
  int last  = LastUKnotIndex();

  NCollection_Array1<double> cknots(1, last - first + 1);
  for (int k = first; k <= last; k++)
    cknots(k - first + 1) = myUKnots(k);
  myUKnots = std::move(cknots);

  NCollection_Array1<int> cmults(1, last - first + 1);
  for (int k = first; k <= last; k++)
    cmults(k - first + 1) = myUMults(k);
  cmults(1) = cmults(cmults.Upper()) =
    std::min(myUDeg, std::max(cmults(1), cmults(cmults.Upper())));
  myUMults = std::move(cmults);

  // compute new number of poles;
  int nbp = BSplCLib::NbPoles(myUDeg, true, myUMults);

  myPoles.ResizeWithTrim(1, nbp, myPoles.LowerCol(), myPoles.UpperCol(), true);
  if (myURational || myVRational)
  {
    myWeights.ResizeWithTrim(1, nbp, myWeights.LowerCol(), myWeights.UpperCol(), true);
  }
  else
  {
    myWeights = BSplSLib::UnitWeights(nbp, myPoles.RowLength());
  }

  myUPeriodic = true;

  myMaxDerivInvOk = false;
  updateUKnots();
}

//=================================================================================================

void Geom_BSplineSurface::SetVPeriodic()
{
  ClearEvalRepresentation();
  int first = FirstVKnotIndex();
  int last  = LastVKnotIndex();

  NCollection_Array1<double> cknots(1, last - first + 1);
  for (int k = first; k <= last; k++)
    cknots(k - first + 1) = myVKnots(k);
  myVKnots = std::move(cknots);

  NCollection_Array1<int> cmults(1, last - first + 1);
  for (int k = first; k <= last; k++)
    cmults(k - first + 1) = myVMults(k);
  cmults(1) = cmults(cmults.Upper()) =
    std::min(myVDeg, std::max(cmults(1), cmults(cmults.Upper())));
  myVMults = std::move(cmults);

  // compute new number of poles;
  int nbp = BSplCLib::NbPoles(myVDeg, true, myVMults);

  myPoles.ResizeWithTrim(myPoles.LowerRow(), myPoles.UpperRow(), 1, nbp, true);
  if (myURational || myVRational)
  {
    myWeights.ResizeWithTrim(myWeights.LowerRow(), myWeights.UpperRow(), 1, nbp, true);
  }
  else
  {
    myWeights = BSplSLib::UnitWeights(myPoles.ColLength(), nbp);
  }

  myVPeriodic = true;

  myMaxDerivInvOk = false;
  updateVKnots();
}

//=================================================================================================

void Geom_BSplineSurface::SetUOrigin(const int Index)
{
  if (!myUPeriodic)
    throw Standard_NoSuchObject("Geom_BSplineSurface::SetUOrigin: surface is not U periodic");

  ClearEvalRepresentation();

  int i, j, k;
  int first = FirstUKnotIndex();
  int last  = LastUKnotIndex();

  if ((Index < first) || (Index > last))
    throw Standard_DomainError("Geom_BSplineCurve::SetUOrigin: Index out of range");

  int nbknots = myUKnots.Length();
  int nbpoles = myPoles.ColLength();

  NCollection_Array1<double> newknots(1, nbknots);

  NCollection_Array1<int> newmults(1, nbknots);

  // set the knots and mults
  double period = myUKnots.Value(last) - myUKnots.Value(first);
  k             = 1;
  for (i = Index; i <= last; i++)
  {
    newknots(k) = myUKnots.Value(i);
    newmults(k) = myUMults.Value(i);
    k++;
  }
  for (i = first + 1; i <= Index; i++)
  {
    newknots(k) = myUKnots.Value(i) + period;
    newmults(k) = myUMults.Value(i);
    k++;
  }

  int index = 1;
  for (i = first + 1; i <= Index; i++)
    index += myUMults.Value(i);

  // set the poles and weights
  int                        nbvp = myPoles.RowLength();
  NCollection_Array2<gp_Pnt> newpoles(1, nbpoles, 1, nbvp);
  first = myPoles.LowerRow();
  last  = myPoles.UpperRow();
  if (myURational || myVRational)
  {
    NCollection_Array2<double> newweights(1, nbpoles, 1, nbvp);
    k = 1;
    for (i = index; i <= last; i++)
    {
      for (j = 1; j <= nbvp; j++)
      {
        newpoles(k, j)   = myPoles.Value(i, j);
        newweights(k, j) = myWeights.Value(i, j);
      }
      k++;
    }
    for (i = first; i < index; i++)
    {
      for (j = 1; j <= nbvp; j++)
      {
        newpoles(k, j)   = myPoles.Value(i, j);
        newweights(k, j) = myWeights.Value(i, j);
      }
      k++;
    }
    myWeights = std::move(newweights);
  }
  else
  {
    k = 1;
    for (i = index; i <= last; i++)
    {
      for (j = 1; j <= nbvp; j++)
      {
        newpoles(k, j) = myPoles.Value(i, j);
      }
      k++;
    }
    for (i = first; i < index; i++)
    {
      for (j = 1; j <= nbvp; j++)
      {
        newpoles(k, j) = myPoles.Value(i, j);
      }
      k++;
    }
    myWeights = BSplSLib::UnitWeights(nbpoles, nbvp);
  }

  myPoles  = std::move(newpoles);
  myUKnots = std::move(newknots);
  myUMults = std::move(newmults);
  updateUKnots();
}

//=================================================================================================

void Geom_BSplineSurface::SetVOrigin(const int Index)
{
  if (!myVPeriodic)
    throw Standard_NoSuchObject("Geom_BSplineSurface::SetVOrigin: surface is not V periodic");

  ClearEvalRepresentation();

  int i, j, k;
  int first = FirstVKnotIndex();
  int last  = LastVKnotIndex();

  if ((Index < first) || (Index > last))
    throw Standard_DomainError("Geom_BSplineCurve::SetVOrigin: Index out of range");

  int nbknots = myVKnots.Length();
  int nbpoles = myPoles.RowLength();

  NCollection_Array1<double> newknots(1, nbknots);

  NCollection_Array1<int> newmults(1, nbknots);

  // set the knots and mults
  double period = myVKnots.Value(last) - myVKnots.Value(first);
  k             = 1;
  for (i = Index; i <= last; i++)
  {
    newknots(k) = myVKnots.Value(i);
    newmults(k) = myVMults.Value(i);
    k++;
  }
  for (i = first + 1; i <= Index; i++)
  {
    newknots(k) = myVKnots.Value(i) + period;
    newmults(k) = myVMults.Value(i);
    k++;
  }

  int index = 1;
  for (i = first + 1; i <= Index; i++)
    index += myVMults.Value(i);

  // set the poles and weights
  int                        nbup = myPoles.ColLength();
  NCollection_Array2<gp_Pnt> newpoles(1, nbup, 1, nbpoles);
  first = myPoles.LowerCol();
  last  = myPoles.UpperCol();
  if (myURational || myVRational)
  {
    NCollection_Array2<double> newweights(1, nbup, 1, nbpoles);
    k = 1;
    for (j = index; j <= last; j++)
    {
      for (i = 1; i <= nbup; i++)
      {
        newpoles(i, k)   = myPoles.Value(i, j);
        newweights(i, k) = myWeights.Value(i, j);
      }
      k++;
    }
    for (j = first; j < index; j++)
    {
      for (i = 1; i <= nbup; i++)
      {
        newpoles(i, k)   = myPoles.Value(i, j);
        newweights(i, k) = myWeights.Value(i, j);
      }
      k++;
    }
    myWeights = std::move(newweights);
  }
  else
  {
    k = 1;
    for (j = index; j <= last; j++)
    {
      for (i = 1; i <= nbup; i++)
      {
        newpoles(i, k) = myPoles.Value(i, j);
      }
      k++;
    }
    for (j = first; j < index; j++)
    {
      for (i = 1; i <= nbup; i++)
      {
        newpoles(i, k) = myPoles.Value(i, j);
      }
      k++;
    }
    myWeights = BSplSLib::UnitWeights(nbup, nbpoles);
  }

  myPoles  = std::move(newpoles);
  myVKnots = std::move(newknots);
  myVMults = std::move(newmults);
  updateVKnots();
}

//=================================================================================================

void Geom_BSplineSurface::SetUNotPeriodic()
{
  if (myUPeriodic)
  {
    ClearEvalRepresentation();
    int NbKnots, NbPoles;
    BSplCLib::PrepareUnperiodize(myUDeg, myUMults, NbKnots, NbPoles);

    NCollection_Array2<gp_Pnt> npoles(1, NbPoles, 1, myPoles.RowLength());

    NCollection_Array1<double> nknots(1, NbKnots);

    NCollection_Array1<int> nmults(1, NbKnots);

    if (myURational || myVRational)
    {
      NCollection_Array2<double> nweights(1, NbPoles, 1, myPoles.RowLength());
      nweights.Init(0.0);
      BSplSLib::Unperiodize(true,
                            myUDeg,
                            myUMults,
                            myUKnots,
                            myPoles,
                            Weights(),
                            nmults,
                            nknots,
                            npoles,
                            &nweights);
      myWeights = std::move(nweights);
    }
    else
    {
      BSplSLib::Unperiodize(true,
                            myUDeg,
                            myUMults,
                            myUKnots,
                            myPoles,
                            BSplSLib::NoWeights(),
                            nmults,
                            nknots,
                            npoles,
                            BSplSLib::NoWeights());
      myWeights = BSplSLib::UnitWeights(npoles.ColLength(), npoles.RowLength());
    }
    myPoles     = std::move(npoles);
    myUMults    = std::move(nmults);
    myUKnots    = std::move(nknots);
    myUPeriodic = false;

    myMaxDerivInvOk = false;
    updateUKnots();
  }
}

//=================================================================================================

void Geom_BSplineSurface::SetVNotPeriodic()
{
  if (myVPeriodic)
  {
    ClearEvalRepresentation();
    int NbKnots, NbPoles;
    BSplCLib::PrepareUnperiodize(myVDeg, myVMults, NbKnots, NbPoles);

    NCollection_Array2<gp_Pnt> npoles(1, myPoles.ColLength(), 1, NbPoles);

    NCollection_Array1<double> nknots(1, NbKnots);

    NCollection_Array1<int> nmults(1, NbKnots);

    if (myURational || myVRational)
    {
      NCollection_Array2<double> nweights(1, myPoles.ColLength(), 1, NbPoles);
      nweights.Init(0.0);
      BSplSLib::Unperiodize(false,
                            myVDeg,
                            myVMults,
                            myVKnots,
                            myPoles,
                            Weights(),
                            nmults,
                            nknots,
                            npoles,
                            &nweights);
      myWeights = std::move(nweights);
    }
    else
    {
      BSplSLib::Unperiodize(false,
                            myVDeg,
                            myVMults,
                            myVKnots,
                            myPoles,
                            BSplSLib::NoWeights(),
                            nmults,
                            nknots,
                            npoles,
                            BSplSLib::NoWeights());
      myWeights = BSplSLib::UnitWeights(npoles.ColLength(), npoles.RowLength());
    }
    myPoles     = std::move(npoles);
    myVMults    = std::move(nmults);
    myVKnots    = std::move(nknots);
    myVPeriodic = false;

    myMaxDerivInvOk = false;
    updateVKnots();
  }
}

//=================================================================================================

bool Geom_BSplineSurface::IsUClosed() const
{
  if (myUPeriodic)
    return true;

  double aU1, aU2, aV1, aV2;
  Bounds(aU1, aU2, aV1, aV2);
  occ::handle<Geom_Curve> aCUF = UIso(aU1);
  occ::handle<Geom_Curve> aCUL = UIso(aU2);
  if (aCUF.IsNull() || aCUL.IsNull())
    return false;
  occ::handle<Geom_BSplineCurve> aBsF = occ::down_cast<Geom_BSplineCurve>(aCUF);
  occ::handle<Geom_BSplineCurve> aBsL = occ::down_cast<Geom_BSplineCurve>(aCUL);
  return (!aBsF.IsNull() && !aBsL.IsNull() && aBsF->IsEqual(aBsL, Precision::Confusion()));
}

//=================================================================================================

bool Geom_BSplineSurface::IsVClosed() const
{
  if (myVPeriodic)
    return true;

  double aU1, aU2, aV1, aV2;
  Bounds(aU1, aU2, aV1, aV2);
  occ::handle<Geom_Curve> aCVF = VIso(aV1);
  occ::handle<Geom_Curve> aCVL = VIso(aV2);
  if (aCVF.IsNull() || aCVL.IsNull())
    return false;
  occ::handle<Geom_BSplineCurve> aBsF = occ::down_cast<Geom_BSplineCurve>(aCVF);
  occ::handle<Geom_BSplineCurve> aBsL = occ::down_cast<Geom_BSplineCurve>(aCVL);
  return (!aBsF.IsNull() && !aBsL.IsNull() && aBsF->IsEqual(aBsL, Precision::Confusion()));
}

//=================================================================================================

bool Geom_BSplineSurface::IsUPeriodic() const
{
  return myUPeriodic;
}

//=================================================================================================

bool Geom_BSplineSurface::IsVPeriodic() const
{
  return myVPeriodic;
}

//=================================================================================================

int Geom_BSplineSurface::FirstUKnotIndex() const
{
  if (myUPeriodic)
    return 1;
  else
    return BSplCLib::FirstUKnotIndex(myUDeg, myUMults);
}

//=================================================================================================

int Geom_BSplineSurface::FirstVKnotIndex() const
{
  if (myVPeriodic)
    return 1;
  else
    return BSplCLib::FirstUKnotIndex(myVDeg, myVMults);
}

//=================================================================================================

int Geom_BSplineSurface::LastUKnotIndex() const
{
  if (myUPeriodic)
    return myUKnots.Length();
  else
    return BSplCLib::LastUKnotIndex(myUDeg, myUMults);
}

//=================================================================================================

int Geom_BSplineSurface::LastVKnotIndex() const
{
  if (myVPeriodic)
    return myVKnots.Length();
  else
    return BSplCLib::LastUKnotIndex(myVDeg, myVMults);
}

//=================================================================================================

void Geom_BSplineSurface::LocateU(const double U,
                                  const double ParametricTolerance,
                                  int&         I1,
                                  int&         I2,
                                  const bool   WithKnotRepetition) const
{
  double                            NewU = U, vbid = myVKnots.Value(1);
  const NCollection_Array1<double>& TheKnots = WithKnotRepetition ? myUFlatKnots : myUKnots;

  PeriodicNormalization(NewU, vbid); // Attention a la periode

  double UFirst               = TheKnots(1);
  double ULast                = TheKnots(TheKnots.Length());
  double PParametricTolerance = std::abs(ParametricTolerance);
  if (std::abs(NewU - UFirst) <= PParametricTolerance)
  {
    I1 = I2 = 1;
  }
  else if (std::abs(NewU - ULast) <= PParametricTolerance)
  {
    I1 = I2 = TheKnots.Length();
  }
  else if (NewU < UFirst)
  {
    I2 = 1;
    I1 = 0;
  }
  else if (NewU > ULast)
  {
    I1 = TheKnots.Length();
    I2 = I1 + 1;
  }
  else
  {
    I1 = 1;
    BSplCLib::Hunt(TheKnots, NewU, I1);
    I1 = std::max(std::min(I1, TheKnots.Upper()), TheKnots.Lower());
    while (I1 + 1 <= TheKnots.Upper() && std::abs(TheKnots(I1 + 1) - NewU) <= PParametricTolerance)
    {
      I1++;
    }
    if (std::abs(TheKnots(I1) - NewU) <= PParametricTolerance)
    {
      I2 = I1;
    }
    else
    {
      I2 = I1 + 1;
    }
  }
}

//=================================================================================================

void Geom_BSplineSurface::LocateV(const double V,
                                  const double ParametricTolerance,
                                  int&         I1,
                                  int&         I2,
                                  const bool   WithKnotRepetition) const
{
  double                            NewV = V, ubid = myUKnots.Value(1);
  const NCollection_Array1<double>& TheKnots = WithKnotRepetition ? myVFlatKnots : myVKnots;

  PeriodicNormalization(ubid, NewV); // Attention a la periode

  double VFirst               = TheKnots(1);
  double VLast                = TheKnots(TheKnots.Length());
  double PParametricTolerance = std::abs(ParametricTolerance);
  if (std::abs(NewV - VFirst) <= PParametricTolerance)
  {
    I1 = I2 = 1;
  }
  else if (std::abs(NewV - VLast) <= PParametricTolerance)
  {
    I1 = I2 = TheKnots.Length();
  }
  else if (NewV < VFirst - PParametricTolerance)
  {
    I2 = 1;
    I1 = 0;
  }
  else if (NewV > VLast + PParametricTolerance)
  {
    I1 = TheKnots.Length();
    I2 = I1 + 1;
  }
  else
  {
    I1 = 1;
    BSplCLib::Hunt(TheKnots, NewV, I1);
    I1 = std::max(std::min(I1, TheKnots.Upper()), TheKnots.Lower());
    while (I1 + 1 <= TheKnots.Upper() && std::abs(TheKnots(I1 + 1) - NewV) <= PParametricTolerance)
    {
      I1++;
    }
    if (std::abs(TheKnots(I1) - NewV) <= PParametricTolerance)
    {
      I2 = I1;
    }
    else
    {
      I2 = I1 + 1;
    }
  }
}

//=================================================================================================

void Geom_BSplineSurface::UReverse()
{
  ClearEvalRepresentation();
  BSplCLib::Reverse(myUMults);
  BSplCLib::Reverse(myUKnots);
  int last;
  if (myUPeriodic)
    last = myUFlatKnots.Upper() - myUDeg - 1;
  else
    last = myPoles.UpperRow();
  BSplSLib::Reverse(myPoles, last, true);
  if (myURational || myVRational)
    BSplSLib::Reverse(myWeights, last, true);
  updateUKnots();
}

//=================================================================================================

double Geom_BSplineSurface::UReversedParameter(const double U) const
{
  return (myUKnots.Value(1) + myUKnots.Value(myUKnots.Length()) - U);
}

//=================================================================================================

void Geom_BSplineSurface::VReverse()
{
  ClearEvalRepresentation();
  BSplCLib::Reverse(myVMults);
  BSplCLib::Reverse(myVKnots);
  int last;
  if (myVPeriodic)
    last = myVFlatKnots.Upper() - myVDeg - 1;
  else
    last = myPoles.UpperCol();
  BSplSLib::Reverse(myPoles, last, false);
  if (myURational || myVRational)
    BSplSLib::Reverse(myWeights, last, false);
  updateVKnots();
}

//=================================================================================================

double Geom_BSplineSurface::VReversedParameter(const double V) const
{
  return (myVKnots.Value(1) + myVKnots.Value(myVKnots.Length()) - V);
}

//=================================================================================================

void Geom_BSplineSurface::SetPoleCol(const int VIndex, const NCollection_Array1<gp_Pnt>& CPoles)
{
  if (VIndex < 1 || VIndex > myPoles.RowLength())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetPoleCol: VIndex out of range");
  }
  if (CPoles.Lower() < 1 || CPoles.Lower() > myPoles.ColLength() || CPoles.Upper() < 1
      || CPoles.Upper() > myPoles.ColLength())
  {
    throw Standard_ConstructionError("Geom_BSplineSurface::SetPoleCol: invalid array dimension");
  }

  ClearEvalRepresentation();
  for (int I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    myPoles(I + myPoles.LowerRow() - 1, VIndex + myPoles.LowerCol() - 1) = CPoles(I);
  }
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BSplineSurface::SetPoleCol(const int                         VIndex,
                                     const NCollection_Array1<gp_Pnt>& CPoles,
                                     const NCollection_Array1<double>& CPoleWeights)
{
  SetPoleCol(VIndex, CPoles);
  SetWeightCol(VIndex, CPoleWeights);
}

//=================================================================================================

void Geom_BSplineSurface::SetPoleRow(const int UIndex, const NCollection_Array1<gp_Pnt>& CPoles)
{
  if (UIndex < 1 || UIndex > myPoles.ColLength())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetPoleRow: UIndex out of range");
  }
  if (CPoles.Lower() < 1 || CPoles.Lower() > myPoles.RowLength() || CPoles.Upper() < 1
      || CPoles.Upper() > myPoles.RowLength())
  {
    throw Standard_ConstructionError("Geom_BSplineSurface::SetPoleRow: invalid array dimension");
  }

  ClearEvalRepresentation();
  for (int I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    myPoles(UIndex + myPoles.LowerRow() - 1, I + myPoles.LowerCol() - 1) = CPoles(I);
  }
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BSplineSurface::SetPoleRow(const int                         UIndex,
                                     const NCollection_Array1<gp_Pnt>& CPoles,
                                     const NCollection_Array1<double>& CPoleWeights)
{
  SetPoleRow(UIndex, CPoles);
  SetWeightRow(UIndex, CPoleWeights);
}

//=================================================================================================

void Geom_BSplineSurface::SetPole(const int UIndex, const int VIndex, const gp_Pnt& P)
{
  ClearEvalRepresentation();
  myPoles.SetValue(UIndex + myPoles.LowerRow() - 1, VIndex + myPoles.LowerCol() - 1, P);
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BSplineSurface::SetPole(const int     UIndex,
                                  const int     VIndex,
                                  const gp_Pnt& P,
                                  const double  Weight)
{
  SetWeight(UIndex, VIndex, Weight);
  SetPole(UIndex, VIndex, P);
}

//=================================================================================================

void Geom_BSplineSurface::MovePoint(const double  U,
                                    const double  V,
                                    const gp_Pnt& P,
                                    const int     UIndex1,
                                    const int     UIndex2,
                                    const int     VIndex1,
                                    const int     VIndex2,
                                    int&          UFirstModifiedPole,
                                    int&          ULastmodifiedPole,
                                    int&          VFirstModifiedPole,
                                    int&          VLastmodifiedPole)
{
  if (UIndex1 < 1 || UIndex1 > myPoles.UpperRow() || UIndex2 < 1 || UIndex2 > myPoles.UpperRow()
      || UIndex1 > UIndex2 || VIndex1 < 1 || VIndex1 > myPoles.UpperCol() || VIndex2 < 1
      || VIndex2 > myPoles.UpperCol() || VIndex1 > VIndex2)
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::MovePoint: Index and #pole mismatch");
  }

  NCollection_Array2<gp_Pnt> npoles(1, myPoles.UpperRow(), 1, myPoles.UpperCol());
  gp_Pnt                     P0;
  Geom_Surface::D0(U, V, P0);
  gp_Vec Displ(P0, P);
  bool   rational = (myURational || myVRational);
  BSplSLib::MovePoint(U,
                      V,
                      Displ,
                      UIndex1,
                      UIndex2,
                      VIndex1,
                      VIndex2,
                      myUDeg,
                      myVDeg,
                      rational,
                      myPoles,
                      myWeights,
                      myUFlatKnots,
                      myVFlatKnots,
                      UFirstModifiedPole,
                      ULastmodifiedPole,
                      VFirstModifiedPole,
                      VLastmodifiedPole,
                      npoles);
  if (UFirstModifiedPole)
  {
    ClearEvalRepresentation();
    myPoles = std::move(npoles);
  }
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BSplineSurface::Bounds(double& U1, double& U2, double& V1, double& V2) const
{
  U1 = myUFlatKnots.Value(myUDeg + 1);
  U2 = myUFlatKnots.Value(myUFlatKnots.Upper() - myUDeg);
  V1 = myVFlatKnots.Value(myVDeg + 1);
  V2 = myVFlatKnots.Value(myVFlatKnots.Upper() - myVDeg);
}

//=================================================================================================

int Geom_BSplineSurface::MaxDegree()
{
  return BSplCLib::MaxDegree();
}

//=================================================================================================

bool Geom_BSplineSurface::IsURational() const
{
  return myURational;
}

//=================================================================================================

bool Geom_BSplineSurface::IsVRational() const
{
  return myVRational;
}

//=================================================================================================

GeomAbs_Shape Geom_BSplineSurface::Continuity() const
{
  return ((myUSmooth < myVSmooth) ? myUSmooth : myVSmooth);
}

//=================================================================================================

int Geom_BSplineSurface::NbUKnots() const
{
  return myUKnots.Length();
}

//=================================================================================================

int Geom_BSplineSurface::NbUPoles() const
{
  return myPoles.ColLength();
}

//=================================================================================================

int Geom_BSplineSurface::NbVKnots() const
{
  return myVKnots.Length();
}

//=================================================================================================

int Geom_BSplineSurface::NbVPoles() const
{
  return myPoles.RowLength();
}

//=================================================================================================

int Geom_BSplineSurface::UDegree() const
{
  return myUDeg;
}

//=================================================================================================

int Geom_BSplineSurface::VDegree() const
{
  return myVDeg;
}

//=================================================================================================

GeomAbs_BSplKnotDistribution Geom_BSplineSurface::UKnotDistribution() const
{
  return myUKnotSet;
}

//=================================================================================================

GeomAbs_BSplKnotDistribution Geom_BSplineSurface::VKnotDistribution() const
{
  return myVKnotSet;
}

//=================================================================================================

void Geom_BSplineSurface::InsertUKnots(const NCollection_Array1<double>& Knots,
                                       const NCollection_Array1<int>&    Mults,
                                       const double                      ParametricTolerance,
                                       const bool                        Add)
{
  // Check and compute new sizes
  int nbpoles, nbknots;

  if (!BSplCLib::PrepareInsertKnots(myUDeg,
                                    myUPeriodic,
                                    myUKnots,
                                    myUMults,
                                    Knots,
                                    &Mults,
                                    nbpoles,
                                    nbknots,
                                    ParametricTolerance,
                                    Add))
    throw Standard_ConstructionError("Geom_BSplineSurface::InsertUKnots");

  if (nbpoles == myPoles.ColLength())
    return;

  ClearEvalRepresentation();

  NCollection_Array2<gp_Pnt> npoles(1, nbpoles, 1, myPoles.RowLength());
  NCollection_Array1<double> nknots(1, nbknots);
  NCollection_Array1<int>    nmults(1, nbknots);

  if (myURational || myVRational)
  {
    NCollection_Array2<double> nweights(1, nbpoles, 1, myPoles.RowLength());
    BSplSLib::InsertKnots(true,
                          myUDeg,
                          myUPeriodic,
                          myPoles,
                          Weights(),
                          myUKnots,
                          myUMults,
                          Knots,
                          &Mults,
                          npoles,
                          &nweights,
                          nknots,
                          nmults,
                          ParametricTolerance,
                          Add);
    myWeights = std::move(nweights);
  }
  else
  {
    BSplSLib::InsertKnots(true,
                          myUDeg,
                          myUPeriodic,
                          myPoles,
                          BSplSLib::NoWeights(),
                          myUKnots,
                          myUMults,
                          Knots,
                          &Mults,
                          npoles,
                          BSplSLib::NoWeights(),
                          nknots,
                          nmults,
                          ParametricTolerance,
                          Add);
    myWeights = BSplSLib::UnitWeights(npoles.ColLength(), npoles.RowLength());
  }

  myPoles  = std::move(npoles);
  myUKnots = std::move(nknots);
  myUMults = std::move(nmults);
  updateUKnots();
}

//=================================================================================================

void Geom_BSplineSurface::InsertVKnots(const NCollection_Array1<double>& Knots,
                                       const NCollection_Array1<int>&    Mults,
                                       const double                      ParametricTolerance,
                                       const bool                        Add)
{
  // Check and compute new sizes
  int nbpoles, nbknots;

  if (!BSplCLib::PrepareInsertKnots(myVDeg,
                                    myVPeriodic,
                                    myVKnots,
                                    myVMults,
                                    Knots,
                                    &Mults,
                                    nbpoles,
                                    nbknots,
                                    ParametricTolerance,
                                    Add))
    throw Standard_ConstructionError("Geom_BSplineSurface::InsertVKnots");

  if (nbpoles == myPoles.RowLength())
    return;

  ClearEvalRepresentation();

  NCollection_Array2<gp_Pnt> npoles(1, myPoles.ColLength(), 1, nbpoles);
  NCollection_Array1<double> nknots(1, nbknots);
  NCollection_Array1<int>    nmults(1, nbknots);

  if (myURational || myVRational)
  {
    NCollection_Array2<double> nweights(1, myPoles.ColLength(), 1, nbpoles);
    BSplSLib::InsertKnots(false,
                          myVDeg,
                          myVPeriodic,
                          myPoles,
                          Weights(),
                          myVKnots,
                          myVMults,
                          Knots,
                          &Mults,
                          npoles,
                          &nweights,
                          nknots,
                          nmults,
                          ParametricTolerance,
                          Add);
    myWeights = std::move(nweights);
  }
  else
  {
    BSplSLib::InsertKnots(false,
                          myVDeg,
                          myVPeriodic,
                          myPoles,
                          BSplSLib::NoWeights(),
                          myVKnots,
                          myVMults,
                          Knots,
                          &Mults,
                          npoles,
                          BSplSLib::NoWeights(),
                          nknots,
                          nmults,
                          ParametricTolerance,
                          Add);
    myWeights = BSplSLib::UnitWeights(npoles.ColLength(), npoles.RowLength());
  }

  myPoles  = std::move(npoles);
  myVKnots = std::move(nknots);
  myVMults = std::move(nmults);
  updateVKnots();
}

//=================================================================================================

bool Geom_BSplineSurface::RemoveUKnot(const int Index, const int M, const double Tolerance)
{
  if (M < 0)
    return true;

  int I1 = FirstUKnotIndex();
  int I2 = LastUKnotIndex();

  if (!myUPeriodic && (Index <= I1 || Index >= I2))
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::RemoveUKnot: invalid Index");
  }
  else if (myUPeriodic && (Index < I1 || Index > I2))
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::RemoveUKnot: invalid Index for periodic case");
  }

  int step = myUMults.Value(Index) - M;
  if (step <= 0)
    return true;

  NCollection_Array2<gp_Pnt> npoles(1, myPoles.ColLength() - step, 1, myPoles.RowLength());
  const int                  aNewUKnotLen = (M == 0) ? myUKnots.Length() - 1 : myUKnots.Length();
  NCollection_Array1<double> nknots(1, aNewUKnotLen);
  NCollection_Array1<int>    nmults(1, aNewUKnotLen);
  if (myURational || myVRational)
  {
    NCollection_Array2<double> nweights(1, npoles.ColLength(), 1, npoles.RowLength());
    if (!BSplSLib::RemoveKnot(true,
                              Index,
                              M,
                              myUDeg,
                              myUPeriodic,
                              myPoles,
                              Weights(),
                              myUKnots,
                              myUMults,
                              npoles,
                              &nweights,
                              nknots,
                              nmults,
                              Tolerance))
      return false;
    myWeights = std::move(nweights);
  }
  else
  {
    if (!BSplSLib::RemoveKnot(true,
                              Index,
                              M,
                              myUDeg,
                              myUPeriodic,
                              myPoles,
                              BSplSLib::NoWeights(),
                              myUKnots,
                              myUMults,
                              npoles,
                              BSplSLib::NoWeights(),
                              nknots,
                              nmults,
                              Tolerance))
      return false;
    myWeights = BSplSLib::UnitWeights(npoles.ColLength(), npoles.RowLength());
  }

  ClearEvalRepresentation();
  myPoles  = std::move(npoles);
  myUKnots = std::move(nknots);
  myUMults = std::move(nmults);

  myMaxDerivInvOk = false;
  updateUKnots();
  return true;
}

//=================================================================================================

bool Geom_BSplineSurface::RemoveVKnot(const int Index, const int M, const double Tolerance)
{
  if (M < 0)
    return true;

  int I1 = FirstVKnotIndex();
  int I2 = LastVKnotIndex();

  if (!myVPeriodic && (Index <= I1 || Index >= I2))
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::RemoveVKnot: invalid Index");
  }
  else if (myVPeriodic && (Index < I1 || Index > I2))
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::RemoveVKnot: invalid Index for periodic case");
  }

  int step = myVMults.Value(Index) - M;
  if (step <= 0)
    return true;

  NCollection_Array2<gp_Pnt> npoles(1, myPoles.ColLength(), 1, myPoles.RowLength() - step);
  const int                  aNewVKnotLen = (M == 0) ? myVKnots.Length() - 1 : myVKnots.Length();
  NCollection_Array1<double> nknots(1, aNewVKnotLen);
  NCollection_Array1<int>    nmults(1, aNewVKnotLen);
  if (myURational || myVRational)
  {
    NCollection_Array2<double> nweights(1, npoles.ColLength(), 1, npoles.RowLength());
    if (!BSplSLib::RemoveKnot(false,
                              Index,
                              M,
                              myVDeg,
                              myVPeriodic,
                              myPoles,
                              Weights(),
                              myVKnots,
                              myVMults,
                              npoles,
                              &nweights,
                              nknots,
                              nmults,
                              Tolerance))
      return false;
    myWeights = std::move(nweights);
  }
  else
  {
    if (!BSplSLib::RemoveKnot(false,
                              Index,
                              M,
                              myVDeg,
                              myVPeriodic,
                              myPoles,
                              BSplSLib::NoWeights(),
                              myVKnots,
                              myVMults,
                              npoles,
                              BSplSLib::NoWeights(),
                              nknots,
                              nmults,
                              Tolerance))
      return false;
    myWeights = BSplSLib::UnitWeights(npoles.ColLength(), npoles.RowLength());
  }

  ClearEvalRepresentation();
  myPoles  = std::move(npoles);
  myVKnots = std::move(nknots);
  myVMults = std::move(nmults);

  myMaxDerivInvOk = false;
  updateVKnots();
  return true;
}

//=================================================================================================

void Geom_BSplineSurface::Resolution(const double Tolerance3D,
                                     double&      UTolerance,
                                     double&      VTolerance)
{
  if (!myMaxDerivInvOk)
  {
    BSplSLib::Resolution(myPoles,
                         Weights(),
                         myUKnots,
                         myVKnots,
                         myUMults,
                         myVMults,
                         myUDeg,
                         myVDeg,
                         myURational,
                         myVRational,
                         myUPeriodic,
                         myVPeriodic,
                         1.,
                         myUMaxDerivInv,
                         myVMaxDerivInv);
    myMaxDerivInvOk = true;
  }
  UTolerance = Tolerance3D * myUMaxDerivInv;
  VTolerance = Tolerance3D * myVMaxDerivInv;
}
