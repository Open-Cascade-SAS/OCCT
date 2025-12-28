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

#define POLES (poles->Array2())
#define WEIGHTS (weights->Array2())
#define UKNOTS (uknots->Array1())
#define VKNOTS (vknots->Array1())
#define UFKNOTS (ufknots->Array1())
#define VFKNOTS (vfknots->Array1())
#define FMULTS (BSplCLib::NoMults())

//=================================================================================================

bool Geom_BSplineSurface::IsCNu(const int N) const
{
  Standard_RangeError_Raise_if(N < 0, " ");
  switch (Usmooth)
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
                    : N <= udeg
                             - BSplCLib::MaxKnotMult(umults->Array1(),
                                                     umults->Lower() + 1,
                                                     umults->Upper() - 1);
    default:
      return false;
  }
}

//=================================================================================================

bool Geom_BSplineSurface::IsCNv(const int N) const
{
  Standard_RangeError_Raise_if(N < 0, " ");

  switch (Vsmooth)
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
                    : N <= vdeg
                             - BSplCLib::MaxKnotMult(vmults->Array1(),
                                                     vmults->Lower() + 1,
                                                     vmults->Upper() - 1);
    default:
      return false;
  }
}

//=================================================================================================

void Geom_BSplineSurface::D0(const double U, const double V, gp_Pnt& P) const
{
  double aNewU = U;
  double aNewV = V;
  PeriodicNormalization(aNewU, aNewV);

  BSplSLib::D0(aNewU,
               aNewV,
               0,
               0,
               POLES,
               &WEIGHTS,
               UFKNOTS,
               VFKNOTS,
               FMULTS,
               FMULTS,
               udeg,
               vdeg,
               urational,
               vrational,
               uperiodic,
               vperiodic,
               P);
}

//=================================================================================================

void Geom_BSplineSurface::D1(const double U,
                             const double V,
                             gp_Pnt&             P,
                             gp_Vec&             D1U,
                             gp_Vec&             D1V) const
{
  double aNewU = U;
  double aNewV = V;
  PeriodicNormalization(aNewU, aNewV);

  int uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(udeg, uknots->Array1(), &umults->Array1(), U, uperiodic, uindex, aNewU);
  uindex = BSplCLib::FlatIndex(udeg, uindex, umults->Array1(), uperiodic);

  BSplCLib::LocateParameter(vdeg, vknots->Array1(), &vmults->Array1(), V, vperiodic, vindex, aNewV);
  vindex = BSplCLib::FlatIndex(vdeg, vindex, vmults->Array1(), vperiodic);

  BSplSLib::D1(aNewU,
               aNewV,
               uindex,
               vindex,
               POLES,
               &WEIGHTS,
               UFKNOTS,
               VFKNOTS,
               FMULTS,
               FMULTS,
               udeg,
               vdeg,
               urational,
               vrational,
               uperiodic,
               vperiodic,
               P,
               D1U,
               D1V);
}

//=================================================================================================

void Geom_BSplineSurface::D2(const double U,
                             const double V,
                             gp_Pnt&             P,
                             gp_Vec&             D1U,
                             gp_Vec&             D1V,
                             gp_Vec&             D2U,
                             gp_Vec&             D2V,
                             gp_Vec&             D2UV) const
{
  double aNewU = U;
  double aNewV = V;
  PeriodicNormalization(aNewU, aNewV);

  int uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(udeg, uknots->Array1(), &umults->Array1(), U, uperiodic, uindex, aNewU);
  uindex = BSplCLib::FlatIndex(udeg, uindex, umults->Array1(), uperiodic);

  BSplCLib::LocateParameter(vdeg, vknots->Array1(), &vmults->Array1(), V, vperiodic, vindex, aNewV);
  vindex = BSplCLib::FlatIndex(vdeg, vindex, vmults->Array1(), vperiodic);

  BSplSLib::D2(aNewU,
               aNewV,
               uindex,
               vindex,
               POLES,
               &WEIGHTS,
               UFKNOTS,
               VFKNOTS,
               FMULTS,
               FMULTS,
               udeg,
               vdeg,
               urational,
               vrational,
               uperiodic,
               vperiodic,
               P,
               D1U,
               D1V,
               D2U,
               D2V,
               D2UV);
}

//=================================================================================================

void Geom_BSplineSurface::D3(const double U,
                             const double V,
                             gp_Pnt&             P,
                             gp_Vec&             D1U,
                             gp_Vec&             D1V,
                             gp_Vec&             D2U,
                             gp_Vec&             D2V,
                             gp_Vec&             D2UV,
                             gp_Vec&             D3U,
                             gp_Vec&             D3V,
                             gp_Vec&             D3UUV,
                             gp_Vec&             D3UVV) const
{
  BSplSLib::D3(U,
               V,
               0,
               0,
               POLES,
               &WEIGHTS,
               UFKNOTS,
               VFKNOTS,
               FMULTS,
               FMULTS,
               udeg,
               vdeg,
               urational,
               vrational,
               uperiodic,
               vperiodic,
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

gp_Vec Geom_BSplineSurface::DN(const double    U,
                               const double    V,
                               const int Nu,
                               const int Nv) const
{
  gp_Vec Vn;
  BSplSLib::DN(U,
               V,
               Nu,
               Nv,
               0,
               0,
               POLES,
               &WEIGHTS,
               UFKNOTS,
               VFKNOTS,
               FMULTS,
               FMULTS,
               udeg,
               vdeg,
               urational,
               vrational,
               uperiodic,
               vperiodic,
               Vn);
  return Vn;
}

//=================================================================================================

gp_Pnt Geom_BSplineSurface::LocalValue(const double    U,
                                       const double    V,
                                       const int FromUK1,
                                       const int ToUK2,
                                       const int FromVK1,
                                       const int ToVK2) const
{
  gp_Pnt P;
  LocalD0(U, V, FromUK1, ToUK2, FromVK1, ToVK2, P);
  return P;
}

//=================================================================================================

void Geom_BSplineSurface::LocalD0(const double    U,
                                  const double    V,
                                  const int FromUK1,
                                  const int ToUK2,
                                  const int FromVK1,
                                  const int ToVK2,
                                  gp_Pnt&                P) const
{
  Standard_DomainError_Raise_if(FromUK1 == ToUK2 || FromVK1 == ToVK2,
                                "Geom_BSplineSurface::LocalD0");

  double    u = U, v = V;
  int uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(udeg, UFKNOTS, U, uperiodic, FromUK1, ToUK2, uindex, u);
  uindex = BSplCLib::FlatIndex(udeg, uindex, umults->Array1(), uperiodic);

  BSplCLib::LocateParameter(vdeg, VFKNOTS, V, vperiodic, FromVK1, ToVK2, vindex, v);
  vindex = BSplCLib::FlatIndex(vdeg, vindex, vmults->Array1(), vperiodic);

  //  BSplSLib::D0(U,V,uindex,vindex,POLES,WEIGHTS,UFKNOTS,VFKNOTS,FMULTS,FMULTS,
  BSplSLib::D0(u,
               v,
               uindex,
               vindex,
               POLES,
               &WEIGHTS,
               UFKNOTS,
               VFKNOTS,
               FMULTS,
               FMULTS,
               udeg,
               vdeg,
               urational,
               vrational,
               uperiodic,
               vperiodic,
               P);
}

//=================================================================================================

void Geom_BSplineSurface::LocalD1(const double    U,
                                  const double    V,
                                  const int FromUK1,
                                  const int ToUK2,
                                  const int FromVK1,
                                  const int ToVK2,
                                  gp_Pnt&                P,
                                  gp_Vec&                D1U,
                                  gp_Vec&                D1V) const
{
  Standard_DomainError_Raise_if(FromUK1 == ToUK2 || FromVK1 == ToVK2,
                                "Geom_BSplineSurface::LocalD1");

  double    u = U, v = V;
  int uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(udeg, UFKNOTS, U, uperiodic, FromUK1, ToUK2, uindex, u);
  uindex = BSplCLib::FlatIndex(udeg, uindex, umults->Array1(), uperiodic);

  BSplCLib::LocateParameter(vdeg, VFKNOTS, V, vperiodic, FromVK1, ToVK2, vindex, v);
  vindex = BSplCLib::FlatIndex(vdeg, vindex, vmults->Array1(), vperiodic);

  BSplSLib::D1(u,
               v,
               uindex,
               vindex,
               POLES,
               &WEIGHTS,
               UFKNOTS,
               VFKNOTS,
               FMULTS,
               FMULTS,
               udeg,
               vdeg,
               urational,
               vrational,
               uperiodic,
               vperiodic,
               P,
               D1U,
               D1V);
}

//=================================================================================================

void Geom_BSplineSurface::LocalD2(const double    U,
                                  const double    V,
                                  const int FromUK1,
                                  const int ToUK2,
                                  const int FromVK1,
                                  const int ToVK2,
                                  gp_Pnt&                P,
                                  gp_Vec&                D1U,
                                  gp_Vec&                D1V,
                                  gp_Vec&                D2U,
                                  gp_Vec&                D2V,
                                  gp_Vec&                D2UV) const
{
  Standard_DomainError_Raise_if(FromUK1 == ToUK2 || FromVK1 == ToVK2,
                                "Geom_BSplineSurface::LocalD2");

  double    u = U, v = V;
  int uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(udeg, UFKNOTS, U, uperiodic, FromUK1, ToUK2, uindex, u);
  uindex = BSplCLib::FlatIndex(udeg, uindex, umults->Array1(), uperiodic);

  BSplCLib::LocateParameter(vdeg, VFKNOTS, V, vperiodic, FromVK1, ToVK2, vindex, v);
  vindex = BSplCLib::FlatIndex(vdeg, vindex, vmults->Array1(), vperiodic);

  BSplSLib::D2(u,
               v,
               uindex,
               vindex,
               POLES,
               &WEIGHTS,
               UFKNOTS,
               VFKNOTS,
               FMULTS,
               FMULTS,
               udeg,
               vdeg,
               urational,
               vrational,
               uperiodic,
               vperiodic,
               P,
               D1U,
               D1V,
               D2U,
               D2V,
               D2UV);
}

//=================================================================================================

void Geom_BSplineSurface::LocalD3(const double    U,
                                  const double    V,
                                  const int FromUK1,
                                  const int ToUK2,
                                  const int FromVK1,
                                  const int ToVK2,
                                  gp_Pnt&                P,
                                  gp_Vec&                D1U,
                                  gp_Vec&                D1V,
                                  gp_Vec&                D2U,
                                  gp_Vec&                D2V,
                                  gp_Vec&                D2UV,
                                  gp_Vec&                D3U,
                                  gp_Vec&                D3V,
                                  gp_Vec&                D3UUV,
                                  gp_Vec&                D3UVV) const
{
  Standard_DomainError_Raise_if(FromUK1 == ToUK2 || FromVK1 == ToVK2,
                                "Geom_BSplineSurface::LocalD3");

  double    u = U, v = V;
  int uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(udeg, UFKNOTS, U, uperiodic, FromUK1, ToUK2, uindex, u);
  uindex = BSplCLib::FlatIndex(udeg, uindex, umults->Array1(), uperiodic);

  BSplCLib::LocateParameter(vdeg, VFKNOTS, V, vperiodic, FromVK1, ToVK2, vindex, v);
  vindex = BSplCLib::FlatIndex(vdeg, vindex, vmults->Array1(), vperiodic);

  BSplSLib::D3(u,
               v,
               uindex,
               vindex,
               POLES,
               &WEIGHTS,
               UFKNOTS,
               VFKNOTS,
               FMULTS,
               FMULTS,
               udeg,
               vdeg,
               urational,
               vrational,
               uperiodic,
               vperiodic,
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

gp_Vec Geom_BSplineSurface::LocalDN(const double    U,
                                    const double    V,
                                    const int FromUK1,
                                    const int ToUK2,
                                    const int FromVK1,
                                    const int ToVK2,
                                    const int Nu,
                                    const int Nv) const
{
  Standard_DomainError_Raise_if(FromUK1 == ToUK2 || FromVK1 == ToVK2,
                                "Geom_BSplineSurface::LocalDN");

  double    u = U, v = V;
  int uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(udeg, UFKNOTS, U, uperiodic, FromUK1, ToUK2, uindex, u);
  uindex = BSplCLib::FlatIndex(udeg, uindex, umults->Array1(), uperiodic);

  BSplCLib::LocateParameter(vdeg, VFKNOTS, V, vperiodic, FromVK1, ToVK2, vindex, v);
  vindex = BSplCLib::FlatIndex(vdeg, vindex, vmults->Array1(), vperiodic);

  gp_Vec Vn;
  BSplSLib::DN(u,
               v,
               Nu,
               Nv,
               uindex,
               vindex,
               POLES,
               &WEIGHTS,
               UFKNOTS,
               VFKNOTS,
               FMULTS,
               FMULTS,
               udeg,
               vdeg,
               urational,
               vrational,
               uperiodic,
               vperiodic,
               Vn);
  return Vn;
}

//=================================================================================================

const gp_Pnt& Geom_BSplineSurface::Pole(const int UIndex,
                                        const int VIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > poles->ColLength() || VIndex < 1
                                 || VIndex > poles->RowLength(),
                               " ");
  return poles->Value(UIndex, VIndex);
}

//=================================================================================================

void Geom_BSplineSurface::Poles(NCollection_Array2<gp_Pnt>& P) const
{
  Standard_DimensionError_Raise_if(P.ColLength() != poles->ColLength()
                                     || P.RowLength() != poles->RowLength(),
                                   " ");
  P = poles->Array2();
}

const NCollection_Array2<gp_Pnt>& Geom_BSplineSurface::Poles() const
{
  return poles->Array2();
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BSplineSurface::UIso(const double U) const
{
  NCollection_Array1<gp_Pnt>   cpoles(1, poles->RowLength());
  NCollection_Array1<double> cweights(1, poles->RowLength());

  occ::handle<Geom_BSplineCurve> C;

  if (urational || vrational)
  {
    BSplSLib::Iso(U,
                  true,
                  POLES,
                  &WEIGHTS,
                  UFKNOTS,
                  FMULTS,
                  udeg,
                  uperiodic,
                  cpoles,
                  &cweights);
    C =
      new Geom_BSplineCurve(cpoles, cweights, vknots->Array1(), vmults->Array1(), vdeg, vperiodic);
  }
  else
  {
    BSplSLib::Iso(U,
                  true,
                  POLES,
                  BSplSLib::NoWeights(),
                  UFKNOTS,
                  FMULTS,
                  udeg,
                  uperiodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles, vknots->Array1(), vmults->Array1(), vdeg, vperiodic);
  }

  return C;
}

//=======================================================================
// function : UIso
// purpose  : If CheckRational=False, no try to make it non-rational
//=======================================================================

occ::handle<Geom_Curve> Geom_BSplineSurface::UIso(const double    U,
                                             const bool CheckRational) const
{
  NCollection_Array1<gp_Pnt>   cpoles(1, poles->RowLength());
  NCollection_Array1<double> cweights(1, poles->RowLength());

  occ::handle<Geom_BSplineCurve> C;

  if (urational || vrational)
  {
    BSplSLib::Iso(U,
                  true,
                  POLES,
                  &WEIGHTS,
                  UFKNOTS,
                  FMULTS,
                  udeg,
                  uperiodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles,
                              cweights,
                              vknots->Array1(),
                              vmults->Array1(),
                              vdeg,
                              vperiodic,
                              CheckRational);
  }
  else
  {
    BSplSLib::Iso(U,
                  true,
                  POLES,
                  BSplSLib::NoWeights(),
                  UFKNOTS,
                  FMULTS,
                  udeg,
                  uperiodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles, vknots->Array1(), vmults->Array1(), vdeg, vperiodic);
  }

  return C;
}

//=================================================================================================

double Geom_BSplineSurface::UKnot(const int UIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > uknots->Length(), " ");
  return uknots->Value(UIndex);
}

//=================================================================================================

double Geom_BSplineSurface::VKnot(const int VIndex) const
{
  Standard_OutOfRange_Raise_if(VIndex < 1 || VIndex > vknots->Length(), " ");
  return vknots->Value(VIndex);
}

//=================================================================================================

void Geom_BSplineSurface::UKnots(NCollection_Array1<double>& Ku) const
{
  Standard_DimensionError_Raise_if(Ku.Length() != uknots->Length(), " ");
  Ku = uknots->Array1();
}

const NCollection_Array1<double>& Geom_BSplineSurface::UKnots() const
{
  return uknots->Array1();
}

//=================================================================================================

void Geom_BSplineSurface::VKnots(NCollection_Array1<double>& Kv) const
{
  Standard_DimensionError_Raise_if(Kv.Length() != vknots->Length(), " ");
  Kv = vknots->Array1();
}

const NCollection_Array1<double>& Geom_BSplineSurface::VKnots() const
{
  return vknots->Array1();
}

//=================================================================================================

void Geom_BSplineSurface::UKnotSequence(NCollection_Array1<double>& Ku) const
{
  Standard_DimensionError_Raise_if(Ku.Length() != ufknots->Length(), " ");
  Ku = ufknots->Array1();
}

const NCollection_Array1<double>& Geom_BSplineSurface::UKnotSequence() const
{
  return ufknots->Array1();
}

//=================================================================================================

void Geom_BSplineSurface::VKnotSequence(NCollection_Array1<double>& Kv) const
{
  Standard_DimensionError_Raise_if(Kv.Length() != vfknots->Length(), " ");
  Kv = vfknots->Array1();
}

const NCollection_Array1<double>& Geom_BSplineSurface::VKnotSequence() const
{
  return vfknots->Array1();
}

//=================================================================================================

int Geom_BSplineSurface::UMultiplicity(const int UIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > umults->Length(), " ");
  return umults->Value(UIndex);
}

//=================================================================================================

void Geom_BSplineSurface::UMultiplicities(NCollection_Array1<int>& Mu) const
{
  Standard_DimensionError_Raise_if(Mu.Length() != umults->Length(), " ");
  Mu = umults->Array1();
}

const NCollection_Array1<int>& Geom_BSplineSurface::UMultiplicities() const
{
  return umults->Array1();
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BSplineSurface::VIso(const double V) const
{
  NCollection_Array1<gp_Pnt>   cpoles(1, poles->ColLength());
  NCollection_Array1<double> cweights(1, poles->ColLength());

  occ::handle<Geom_BSplineCurve> C;

  if (urational || vrational)
  {
    BSplSLib::Iso(V,
                  false,
                  POLES,
                  &WEIGHTS,
                  VFKNOTS,
                  FMULTS,
                  vdeg,
                  vperiodic,
                  cpoles,
                  &cweights);
    C =
      new Geom_BSplineCurve(cpoles, cweights, uknots->Array1(), umults->Array1(), udeg, uperiodic);
  }
  else
  {
    BSplSLib::Iso(V,
                  false,
                  POLES,
                  BSplSLib::NoWeights(),
                  VFKNOTS,
                  FMULTS,
                  vdeg,
                  vperiodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles, uknots->Array1(), umults->Array1(), udeg, uperiodic);
  }

  return C;
}

//=======================================================================
// function : VIso
// purpose  : If CheckRational=False, no try to make it non-rational
//=======================================================================

occ::handle<Geom_Curve> Geom_BSplineSurface::VIso(const double    V,
                                             const bool CheckRational) const
{
  NCollection_Array1<gp_Pnt>   cpoles(1, poles->ColLength());
  NCollection_Array1<double> cweights(1, poles->ColLength());

  occ::handle<Geom_BSplineCurve> C;

  if (urational || vrational)
  {
    BSplSLib::Iso(V,
                  false,
                  POLES,
                  &WEIGHTS,
                  VFKNOTS,
                  FMULTS,
                  vdeg,
                  vperiodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles,
                              cweights,
                              uknots->Array1(),
                              umults->Array1(),
                              udeg,
                              uperiodic,
                              CheckRational);
  }
  else
  {
    BSplSLib::Iso(V,
                  false,
                  POLES,
                  BSplSLib::NoWeights(),
                  VFKNOTS,
                  FMULTS,
                  vdeg,
                  vperiodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles, uknots->Array1(), umults->Array1(), udeg, uperiodic);
  }

  return C;
}

//=================================================================================================

int Geom_BSplineSurface::VMultiplicity(const int VIndex) const
{
  Standard_OutOfRange_Raise_if(VIndex < 1 || VIndex > vmults->Length(), " ");
  return vmults->Value(VIndex);
}

//=================================================================================================

void Geom_BSplineSurface::VMultiplicities(NCollection_Array1<int>& Mv) const
{
  Standard_DimensionError_Raise_if(Mv.Length() != vmults->Length(), " ");
  Mv = vmults->Array1();
}

const NCollection_Array1<int>& Geom_BSplineSurface::VMultiplicities() const
{
  return vmults->Array1();
}

//=================================================================================================

double Geom_BSplineSurface::Weight(const int UIndex,
                                          const int VIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > weights->ColLength() || VIndex < 1
                                 || VIndex > weights->RowLength(),
                               " ");
  return weights->Value(UIndex, VIndex);
}

//=================================================================================================

void Geom_BSplineSurface::Weights(NCollection_Array2<double>& W) const
{
  Standard_DimensionError_Raise_if(W.ColLength() != weights->ColLength()
                                     || W.RowLength() != weights->RowLength(),
                                   " ");
  W = weights->Array2();
}

const NCollection_Array2<double>* Geom_BSplineSurface::Weights() const
{
  if (urational || vrational)
    return &weights->Array2();
  return BSplSLib::NoWeights();
}

//=================================================================================================

void Geom_BSplineSurface::Transform(const gp_Trsf& T)
{
  NCollection_Array2<gp_Pnt>& VPoles = poles->ChangeArray2();
  for (int j = VPoles.LowerCol(); j <= VPoles.UpperCol(); j++)
  {
    for (int i = VPoles.LowerRow(); i <= VPoles.UpperRow(); i++)
    {
      VPoles(i, j).Transform(T);
    }
  }
}

//=================================================================================================

void Geom_BSplineSurface::SetUPeriodic()
{
  int i, j;

  int first = FirstUKnotIndex();
  int last  = LastUKnotIndex();

  occ::handle<NCollection_HArray1<double>> tk = uknots;
  NCollection_Array1<double>          cknots((uknots->Array1())(first), first, last);
  uknots                 = new NCollection_HArray1<double>(1, cknots.Length());
  uknots->ChangeArray1() = cknots;

  occ::handle<NCollection_HArray1<int>> tm = umults;
  NCollection_Array1<int>          cmults((umults->Array1())(first), first, last);
  //  Modified by Sergey KHROMOV - Mon Feb 10 10:59:00 2003 Begin
  //   cmults(first) = cmults(last) = std::max( cmults(first), cmults(last));
  cmults(first) = cmults(last) = std::min(udeg, std::max(cmults(first), cmults(last)));
  //  Modified by Sergey KHROMOV - Mon Feb 10 10:59:00 2003 End
  umults                 = new NCollection_HArray1<int>(1, cmults.Length());
  umults->ChangeArray1() = cmults;

  // compute new number of poles;
  int nbp = BSplCLib::NbPoles(udeg, true, cmults);

  NCollection_Array2<gp_Pnt> cpoles(1, nbp, poles->LowerCol(), poles->UpperCol());
  for (i = 1; i <= nbp; i++)
  {
    for (j = poles->LowerCol(); j <= poles->UpperCol(); j++)
    {
      cpoles(i, j) = poles->Value(i, j);
    }
  }
  poles                 = new NCollection_HArray2<gp_Pnt>(1, nbp, cpoles.LowerCol(), cpoles.UpperCol());
  poles->ChangeArray2() = cpoles;

  NCollection_Array2<double> cweights(1, nbp, weights->LowerCol(), weights->UpperCol());
  if (urational || vrational)
  {
    for (i = 1; i <= nbp; i++)
    {
      for (j = weights->LowerCol(); j <= weights->UpperCol(); j++)
      {
        cweights(i, j) = weights->Value(i, j);
      }
    }
  }
  else
  {
    for (i = 1; i <= nbp; i++)
    {
      for (j = weights->LowerCol(); j <= weights->UpperCol(); j++)
      {
        cweights(i, j) = 1;
      }
    }
  }
  weights = new NCollection_HArray2<double>(1, nbp, cweights.LowerCol(), cweights.UpperCol());
  weights->ChangeArray2() = cweights;

  uperiodic = true;

  maxderivinvok = 0;
  UpdateUKnots();
}

//=================================================================================================

void Geom_BSplineSurface::SetVPeriodic()
{
  int i, j;

  int first = FirstVKnotIndex();
  int last  = LastVKnotIndex();

  occ::handle<NCollection_HArray1<double>> tk = vknots;
  NCollection_Array1<double>          cknots((vknots->Array1())(first), first, last);
  vknots                 = new NCollection_HArray1<double>(1, cknots.Length());
  vknots->ChangeArray1() = cknots;

  occ::handle<NCollection_HArray1<int>> tm = vmults;
  NCollection_Array1<int>          cmults((vmults->Array1())(first), first, last);
  //  Modified by Sergey KHROMOV - Mon Feb 10 11:00:33 2003 Begin
  //   cmults(first) = cmults(last) = std::max( cmults(first), cmults(last));
  cmults(first) = cmults(last) = std::min(vdeg, std::max(cmults(first), cmults(last)));
  //  Modified by Sergey KHROMOV - Mon Feb 10 11:00:34 2003 End
  vmults                 = new NCollection_HArray1<int>(1, cmults.Length());
  vmults->ChangeArray1() = cmults;

  // compute new number of poles;
  int nbp = BSplCLib::NbPoles(vdeg, true, cmults);

  NCollection_Array2<gp_Pnt> cpoles(poles->LowerRow(), poles->UpperRow(), 1, nbp);
  for (i = poles->LowerRow(); i <= poles->UpperRow(); i++)
  {
    for (j = 1; j <= nbp; j++)
    {
      cpoles(i, j) = poles->Value(i, j);
    }
  }
  poles                 = new NCollection_HArray2<gp_Pnt>(cpoles.LowerRow(), cpoles.UpperRow(), 1, nbp);
  poles->ChangeArray2() = cpoles;

  if (urational || vrational)
  {
    NCollection_Array2<double> cweights(weights->LowerRow(), weights->UpperRow(), 1, nbp);
    for (i = weights->LowerRow(); i <= weights->UpperRow(); i++)
    {
      for (j = 1; j <= nbp; j++)
      {
        cweights(i, j) = weights->Value(i, j);
      }
    }
    weights = new NCollection_HArray2<double>(cweights.LowerRow(), cweights.UpperRow(), 1, nbp);
    weights->ChangeArray2() = cweights;
  }

  vperiodic = true;

  maxderivinvok = 0;
  UpdateVKnots();
}

//=================================================================================================

void Geom_BSplineSurface::SetUOrigin(const int Index)
{
  if (!uperiodic)
    throw Standard_NoSuchObject("Geom_BSplineSurface::SetUOrigin: surface is not U periodic");

  int i, j, k;
  int first = FirstUKnotIndex();
  int last  = LastUKnotIndex();

  if ((Index < first) || (Index > last))
    throw Standard_DomainError("Geom_BSplineCurve::SetUOrigin: Index out of range");

  int nbknots = uknots->Length();
  int nbpoles = poles->ColLength();

  occ::handle<NCollection_HArray1<double>> nknots   = new NCollection_HArray1<double>(1, nbknots);
  NCollection_Array1<double>&         newknots = nknots->ChangeArray1();

  occ::handle<NCollection_HArray1<int>> nmults   = new NCollection_HArray1<int>(1, nbknots);
  NCollection_Array1<int>&         newmults = nmults->ChangeArray1();

  // set the knots and mults
  double period = uknots->Value(last) - uknots->Value(first);
  k                    = 1;
  for (i = Index; i <= last; i++)
  {
    newknots(k) = uknots->Value(i);
    newmults(k) = umults->Value(i);
    k++;
  }
  for (i = first + 1; i <= Index; i++)
  {
    newknots(k) = uknots->Value(i) + period;
    newmults(k) = umults->Value(i);
    k++;
  }

  int index = 1;
  for (i = first + 1; i <= Index; i++)
    index += umults->Value(i);

  // set the poles and weights
  int              nbvp       = poles->RowLength();
  occ::handle<NCollection_HArray2<gp_Pnt>>   npoles     = new NCollection_HArray2<gp_Pnt>(1, nbpoles, 1, nbvp);
  occ::handle<NCollection_HArray2<double>> nweights   = new NCollection_HArray2<double>(1, nbpoles, 1, nbvp);
  NCollection_Array2<gp_Pnt>&           newpoles   = npoles->ChangeArray2();
  NCollection_Array2<double>&         newweights = nweights->ChangeArray2();
  first                                    = poles->LowerRow();
  last                                     = poles->UpperRow();
  if (urational || vrational)
  {
    k = 1;
    for (i = index; i <= last; i++)
    {
      for (j = 1; j <= nbvp; j++)
      {
        newpoles(k, j)   = poles->Value(i, j);
        newweights(k, j) = weights->Value(i, j);
      }
      k++;
    }
    for (i = first; i < index; i++)
    {
      for (j = 1; j <= nbvp; j++)
      {
        newpoles(k, j)   = poles->Value(i, j);
        newweights(k, j) = weights->Value(i, j);
      }
      k++;
    }
  }
  else
  {
    k = 1;
    for (i = index; i <= last; i++)
    {
      for (j = 1; j <= nbvp; j++)
      {
        newpoles(k, j) = poles->Value(i, j);
      }
      k++;
    }
    for (i = first; i < index; i++)
    {
      for (j = 1; j <= nbvp; j++)
      {
        newpoles(k, j) = poles->Value(i, j);
      }
      k++;
    }
  }

  poles  = npoles;
  uknots = nknots;
  umults = nmults;
  if (urational || vrational)
    weights = nweights;
  UpdateUKnots();
}

//=================================================================================================

void Geom_BSplineSurface::SetVOrigin(const int Index)
{
  if (!vperiodic)
    throw Standard_NoSuchObject("Geom_BSplineSurface::SetVOrigin: surface is not V periodic");

  int i, j, k;
  int first = FirstVKnotIndex();
  int last  = LastVKnotIndex();

  if ((Index < first) || (Index > last))
    throw Standard_DomainError("Geom_BSplineCurve::SetVOrigin: Index out of range");

  int nbknots = vknots->Length();
  int nbpoles = poles->RowLength();

  occ::handle<NCollection_HArray1<double>> nknots   = new NCollection_HArray1<double>(1, nbknots);
  NCollection_Array1<double>&         newknots = nknots->ChangeArray1();

  occ::handle<NCollection_HArray1<int>> nmults   = new NCollection_HArray1<int>(1, nbknots);
  NCollection_Array1<int>&         newmults = nmults->ChangeArray1();

  // set the knots and mults
  double period = vknots->Value(last) - vknots->Value(first);
  k                    = 1;
  for (i = Index; i <= last; i++)
  {
    newknots(k) = vknots->Value(i);
    newmults(k) = vmults->Value(i);
    k++;
  }
  for (i = first + 1; i <= Index; i++)
  {
    newknots(k) = vknots->Value(i) + period;
    newmults(k) = vmults->Value(i);
    k++;
  }

  int index = 1;
  for (i = first + 1; i <= Index; i++)
    index += vmults->Value(i);

  // set the poles and weights
  int              nbup       = poles->ColLength();
  occ::handle<NCollection_HArray2<gp_Pnt>>   npoles     = new NCollection_HArray2<gp_Pnt>(1, nbup, 1, nbpoles);
  occ::handle<NCollection_HArray2<double>> nweights   = new NCollection_HArray2<double>(1, nbup, 1, nbpoles);
  NCollection_Array2<gp_Pnt>&           newpoles   = npoles->ChangeArray2();
  NCollection_Array2<double>&         newweights = nweights->ChangeArray2();
  first                                    = poles->LowerCol();
  last                                     = poles->UpperCol();
  if (urational || vrational)
  {
    k = 1;
    for (j = index; j <= last; j++)
    {
      for (i = 1; i <= nbup; i++)
      {
        newpoles(i, k)   = poles->Value(i, j);
        newweights(i, k) = weights->Value(i, j);
      }
      k++;
    }
    for (j = first; j < index; j++)
    {
      for (i = 1; i <= nbup; i++)
      {
        newpoles(i, k)   = poles->Value(i, j);
        newweights(i, k) = weights->Value(i, j);
      }
      k++;
    }
  }
  else
  {
    k = 1;
    for (j = index; j <= last; j++)
    {
      for (i = 1; i <= nbup; i++)
      {
        newpoles(i, k) = poles->Value(i, j);
      }
      k++;
    }
    for (j = first; j < index; j++)
    {
      for (i = 1; i <= nbup; i++)
      {
        newpoles(i, k) = poles->Value(i, j);
      }
      k++;
    }
  }

  poles  = npoles;
  vknots = nknots;
  vmults = nmults;
  if (urational || vrational)
    weights = nweights;
  UpdateVKnots();
}

//=================================================================================================

void Geom_BSplineSurface::SetUNotPeriodic()
{
  if (uperiodic)
  {
    int NbKnots, NbPoles;
    BSplCLib::PrepareUnperiodize(udeg, umults->Array1(), NbKnots, NbPoles);

    occ::handle<NCollection_HArray2<gp_Pnt>> npoles = new NCollection_HArray2<gp_Pnt>(1, NbPoles, 1, poles->RowLength());

    occ::handle<NCollection_HArray1<double>> nknots = new NCollection_HArray1<double>(1, NbKnots);

    occ::handle<NCollection_HArray1<int>> nmults = new NCollection_HArray1<int>(1, NbKnots);

    occ::handle<NCollection_HArray2<double>> nweights =
      new NCollection_HArray2<double>(1, NbPoles, 1, poles->RowLength(), 0);

    if (urational || vrational)
    {

      BSplSLib::Unperiodize(true,
                            udeg,
                            umults->Array1(),
                            uknots->Array1(),
                            poles->Array2(),
                            &weights->Array2(),
                            nmults->ChangeArray1(),
                            nknots->ChangeArray1(),
                            npoles->ChangeArray2(),
                            &nweights->ChangeArray2());
    }
    else
    {

      BSplSLib::Unperiodize(true,
                            udeg,
                            umults->Array1(),
                            uknots->Array1(),
                            poles->Array2(),
                            BSplSLib::NoWeights(),
                            nmults->ChangeArray1(),
                            nknots->ChangeArray1(),
                            npoles->ChangeArray2(),
                            BSplSLib::NoWeights());
    }
    poles     = npoles;
    weights   = nweights;
    umults    = nmults;
    uknots    = nknots;
    uperiodic = false;

    maxderivinvok = 0;
    UpdateUKnots();
  }
}

//=================================================================================================

void Geom_BSplineSurface::SetVNotPeriodic()
{
  if (vperiodic)
  {
    int NbKnots, NbPoles;
    BSplCLib::PrepareUnperiodize(vdeg, vmults->Array1(), NbKnots, NbPoles);

    occ::handle<NCollection_HArray2<gp_Pnt>> npoles = new NCollection_HArray2<gp_Pnt>(1, poles->ColLength(), 1, NbPoles);

    occ::handle<NCollection_HArray1<double>> nknots = new NCollection_HArray1<double>(1, NbKnots);

    occ::handle<NCollection_HArray1<int>> nmults = new NCollection_HArray1<int>(1, NbKnots);

    occ::handle<NCollection_HArray2<double>> nweights =
      new NCollection_HArray2<double>(1, poles->ColLength(), 1, NbPoles, 0);

    if (urational || vrational)
    {

      BSplSLib::Unperiodize(false,
                            vdeg,
                            vmults->Array1(),
                            vknots->Array1(),
                            poles->Array2(),
                            &weights->Array2(),
                            nmults->ChangeArray1(),
                            nknots->ChangeArray1(),
                            npoles->ChangeArray2(),
                            &nweights->ChangeArray2());
    }
    else
    {

      BSplSLib::Unperiodize(false,
                            vdeg,
                            vmults->Array1(),
                            vknots->Array1(),
                            poles->Array2(),
                            BSplSLib::NoWeights(),
                            nmults->ChangeArray1(),
                            nknots->ChangeArray1(),
                            npoles->ChangeArray2(),
                            BSplSLib::NoWeights());
    }
    poles     = npoles;
    weights   = nweights;
    vmults    = nmults;
    vknots    = nknots;
    vperiodic = false;

    maxderivinvok = 0;
    UpdateVKnots();
  }
}

//=================================================================================================

bool Geom_BSplineSurface::IsUClosed() const
{
  if (uperiodic)
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
  if (vperiodic)
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
  return uperiodic;
}

//=================================================================================================

bool Geom_BSplineSurface::IsVPeriodic() const
{
  return vperiodic;
}

//=================================================================================================

int Geom_BSplineSurface::FirstUKnotIndex() const
{
  if (uperiodic)
    return 1;
  else
    return BSplCLib::FirstUKnotIndex(udeg, umults->Array1());
}

//=================================================================================================

int Geom_BSplineSurface::FirstVKnotIndex() const
{
  if (vperiodic)
    return 1;
  else
    return BSplCLib::FirstUKnotIndex(vdeg, vmults->Array1());
}

//=================================================================================================

int Geom_BSplineSurface::LastUKnotIndex() const
{
  if (uperiodic)
    return uknots->Length();
  else
    return BSplCLib::LastUKnotIndex(udeg, umults->Array1());
}

//=================================================================================================

int Geom_BSplineSurface::LastVKnotIndex() const
{
  if (vperiodic)
    return vknots->Length();
  else
    return BSplCLib::LastUKnotIndex(vdeg, vmults->Array1());
}

//=================================================================================================

void Geom_BSplineSurface::LocateU(const double    U,
                                  const double    ParametricTolerance,
                                  int&      I1,
                                  int&      I2,
                                  const bool WithKnotRepetition) const
{
  double                 NewU = U, vbid = vknots->Value(1);
  occ::handle<NCollection_HArray1<double>> TheKnots;
  if (WithKnotRepetition)
    TheKnots = ufknots;
  else
    TheKnots = uknots;

  PeriodicNormalization(NewU, vbid); // Attention a la periode

  const NCollection_Array1<double>& Knots                = TheKnots->Array1();
  double               UFirst               = Knots(1);
  double               ULast                = Knots(Knots.Length());
  double               PParametricTolerance = std::abs(ParametricTolerance);
  if (std::abs(NewU - UFirst) <= PParametricTolerance)
  {
    I1 = I2 = 1;
  }
  else if (std::abs(NewU - ULast) <= PParametricTolerance)
  {
    I1 = I2 = Knots.Length();
  }
  else if (NewU < UFirst)
  {
    I2 = 1;
    I1 = 0;
  }
  else if (NewU > ULast)
  {
    I1 = Knots.Length();
    I2 = I1 + 1;
  }
  else
  {
    I1 = 1;
    BSplCLib::Hunt(Knots, NewU, I1);
    I1 = std::max(std::min(I1, Knots.Upper()), Knots.Lower());
    while (I1 + 1 <= Knots.Upper() && std::abs(Knots(I1 + 1) - NewU) <= PParametricTolerance)
    {
      I1++;
    }
    if (std::abs(Knots(I1) - NewU) <= PParametricTolerance)
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

void Geom_BSplineSurface::LocateV(const double    V,
                                  const double    ParametricTolerance,
                                  int&      I1,
                                  int&      I2,
                                  const bool WithKnotRepetition) const
{
  double                 NewV = V, ubid = uknots->Value(1);
  occ::handle<NCollection_HArray1<double>> TheKnots;
  if (WithKnotRepetition)
    TheKnots = vfknots;
  else
    TheKnots = vknots;

  PeriodicNormalization(ubid, NewV); // Attention a la periode

  const NCollection_Array1<double>& Knots                = TheKnots->Array1();
  double               VFirst               = Knots(1);
  double               VLast                = Knots(Knots.Length());
  double               PParametricTolerance = std::abs(ParametricTolerance);
  if (std::abs(NewV - VFirst) <= PParametricTolerance)
  {
    I1 = I2 = 1;
  }
  else if (std::abs(NewV - VLast) <= PParametricTolerance)
  {
    I1 = I2 = Knots.Length();
  }
  else if (NewV < VFirst - PParametricTolerance)
  {
    I2 = 1;
    I1 = 0;
  }
  else if (NewV > VLast + PParametricTolerance)
  {
    I1 = Knots.Length();
    I2 = I1 + 1;
  }
  else
  {
    I1 = 1;
    BSplCLib::Hunt(Knots, NewV, I1);
    I1 = std::max(std::min(I1, Knots.Upper()), Knots.Lower());
    while (I1 + 1 <= Knots.Upper() && std::abs(Knots(I1 + 1) - NewV) <= PParametricTolerance)
    {
      I1++;
    }
    if (std::abs(Knots(I1) - NewV) <= PParametricTolerance)
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
  BSplCLib::Reverse(umults->ChangeArray1());
  BSplCLib::Reverse(uknots->ChangeArray1());
  int last;
  if (uperiodic)
    last = ufknots->Upper() - udeg - 1;
  else
    last = poles->UpperRow();
  BSplSLib::Reverse(poles->ChangeArray2(), last, true);
  if (urational || vrational)
    BSplSLib::Reverse(weights->ChangeArray2(), last, true);
  UpdateUKnots();
}

//=================================================================================================

double Geom_BSplineSurface::UReversedParameter(const double U) const
{
  return (uknots->Value(1) + uknots->Value(uknots->Length()) - U);
}

//=================================================================================================

void Geom_BSplineSurface::VReverse()
{
  BSplCLib::Reverse(vmults->ChangeArray1());
  BSplCLib::Reverse(vknots->ChangeArray1());
  int last;
  if (vperiodic)
    last = vfknots->Upper() - vdeg - 1;
  else
    last = poles->UpperCol();
  BSplSLib::Reverse(poles->ChangeArray2(), last, false);
  if (urational || vrational)
    BSplSLib::Reverse(weights->ChangeArray2(), last, false);
  UpdateVKnots();
}

//=================================================================================================

double Geom_BSplineSurface::VReversedParameter(const double V) const
{
  return (vknots->Value(1) + vknots->Value(vknots->Length()) - V);
}

//=================================================================================================

void Geom_BSplineSurface::SetPoleCol(const int    VIndex,
                                     const NCollection_Array1<gp_Pnt>& CPoles)
{
  if (VIndex < 1 || VIndex > poles->RowLength())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetPoleCol: VIndex out of range");
  }
  if (CPoles.Lower() < 1 || CPoles.Lower() > poles->ColLength() || CPoles.Upper() < 1
      || CPoles.Upper() > poles->ColLength())
  {
    throw Standard_ConstructionError("Geom_BSplineSurface::SetPoleCol: invalid array dimension");
  }

  NCollection_Array2<gp_Pnt>& Poles = poles->ChangeArray2();

  for (int I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    Poles(I + Poles.LowerRow() - 1, VIndex + Poles.LowerCol() - 1) = CPoles(I);
  }
}

//=================================================================================================

void Geom_BSplineSurface::SetPoleCol(const int      VIndex,
                                     const NCollection_Array1<gp_Pnt>&   CPoles,
                                     const NCollection_Array1<double>& CPoleWeights)
{
  SetPoleCol(VIndex, CPoles);
  SetWeightCol(VIndex, CPoleWeights);
}

//=================================================================================================

void Geom_BSplineSurface::SetPoleRow(const int    UIndex,
                                     const NCollection_Array1<gp_Pnt>& CPoles)
{
  if (UIndex < 1 || UIndex > poles->ColLength())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetPoleRow: UIndex out of range");
  }
  if (CPoles.Lower() < 1 || CPoles.Lower() > poles->RowLength() || CPoles.Upper() < 1
      || CPoles.Upper() > poles->RowLength())
  {
    throw Standard_ConstructionError("Geom_BSplineSurface::SetPoleRow: invalid array dimension");
  }

  NCollection_Array2<gp_Pnt>& Poles = poles->ChangeArray2();

  for (int I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    Poles(UIndex + Poles.LowerRow() - 1, I + Poles.LowerCol() - 1) = CPoles(I);
  }
}

//=================================================================================================

void Geom_BSplineSurface::SetPoleRow(const int      UIndex,
                                     const NCollection_Array1<gp_Pnt>&   CPoles,
                                     const NCollection_Array1<double>& CPoleWeights)
{
  SetPoleRow(UIndex, CPoles);
  SetWeightRow(UIndex, CPoleWeights);
}

//=================================================================================================

void Geom_BSplineSurface::SetPole(const int UIndex,
                                  const int VIndex,
                                  const gp_Pnt&          P)
{
  poles->SetValue(UIndex + poles->LowerRow() - 1, VIndex + poles->LowerCol() - 1, P);
}

//=================================================================================================

void Geom_BSplineSurface::SetPole(const int UIndex,
                                  const int VIndex,
                                  const gp_Pnt&          P,
                                  const double    Weight)
{
  SetWeight(UIndex, VIndex, Weight);
  SetPole(UIndex, VIndex, P);
}

//=================================================================================================

void Geom_BSplineSurface::MovePoint(const double    U,
                                    const double    V,
                                    const gp_Pnt&          P,
                                    const int UIndex1,
                                    const int UIndex2,
                                    const int VIndex1,
                                    const int VIndex2,
                                    int&      UFirstModifiedPole,
                                    int&      ULastmodifiedPole,
                                    int&      VFirstModifiedPole,
                                    int&      VLastmodifiedPole)
{
  if (UIndex1 < 1 || UIndex1 > poles->UpperRow() || UIndex2 < 1 || UIndex2 > poles->UpperRow()
      || UIndex1 > UIndex2 || VIndex1 < 1 || VIndex1 > poles->UpperCol() || VIndex2 < 1
      || VIndex2 > poles->UpperCol() || VIndex1 > VIndex2)
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::MovePoint: Index and #pole mismatch");
  }

  NCollection_Array2<gp_Pnt> npoles(1, poles->UpperRow(), 1, poles->UpperCol());
  gp_Pnt             P0;
  D0(U, V, P0);
  gp_Vec           Displ(P0, P);
  bool rational = (urational || vrational);
  BSplSLib::MovePoint(U,
                      V,
                      Displ,
                      UIndex1,
                      UIndex2,
                      VIndex1,
                      VIndex2,
                      udeg,
                      vdeg,
                      rational,
                      poles->Array2(),
                      weights->Array2(),
                      ufknots->Array1(),
                      vfknots->Array1(),
                      UFirstModifiedPole,
                      ULastmodifiedPole,
                      VFirstModifiedPole,
                      VLastmodifiedPole,
                      npoles);
  if (UFirstModifiedPole)
  {
    poles->ChangeArray2() = npoles;
  }
  maxderivinvok = 0;
}

//=================================================================================================

void Geom_BSplineSurface::Bounds(double& U1,
                                 double& U2,
                                 double& V1,
                                 double& V2) const
{
  U1 = ufknots->Value(udeg + 1);
  U2 = ufknots->Value(ufknots->Upper() - udeg);
  V1 = vfknots->Value(vdeg + 1);
  V2 = vfknots->Value(vfknots->Upper() - vdeg);
}

//=================================================================================================

int Geom_BSplineSurface::MaxDegree()
{
  return BSplCLib::MaxDegree();
}

//=================================================================================================

bool Geom_BSplineSurface::IsURational() const
{
  return urational;
}

//=================================================================================================

bool Geom_BSplineSurface::IsVRational() const
{
  return vrational;
}

//=================================================================================================

GeomAbs_Shape Geom_BSplineSurface::Continuity() const
{
  return ((Usmooth < Vsmooth) ? Usmooth : Vsmooth);
}

//=================================================================================================

int Geom_BSplineSurface::NbUKnots() const
{
  return uknots->Length();
}

//=================================================================================================

int Geom_BSplineSurface::NbUPoles() const
{
  return poles->ColLength();
}

//=================================================================================================

int Geom_BSplineSurface::NbVKnots() const
{
  return vknots->Length();
}

//=================================================================================================

int Geom_BSplineSurface::NbVPoles() const
{
  return poles->RowLength();
}

//=================================================================================================

int Geom_BSplineSurface::UDegree() const
{
  return udeg;
}

//=================================================================================================

int Geom_BSplineSurface::VDegree() const
{
  return vdeg;
}

//=================================================================================================

GeomAbs_BSplKnotDistribution Geom_BSplineSurface::UKnotDistribution() const
{
  return uknotSet;
}

//=================================================================================================

GeomAbs_BSplKnotDistribution Geom_BSplineSurface::VKnotDistribution() const
{
  return vknotSet;
}

//=================================================================================================

void Geom_BSplineSurface::InsertUKnots(const NCollection_Array1<double>&    Knots,
                                       const NCollection_Array1<int>& Mults,
                                       const double            ParametricTolerance,
                                       const bool         Add)
{
  // Check and compute new sizes
  int nbpoles, nbknots;

  if (!BSplCLib::PrepareInsertKnots(udeg,
                                    uperiodic,
                                    uknots->Array1(),
                                    umults->Array1(),
                                    Knots,
                                    &Mults,
                                    nbpoles,
                                    nbknots,
                                    ParametricTolerance,
                                    Add))
    throw Standard_ConstructionError("Geom_BSplineSurface::InsertUKnots");

  if (nbpoles == poles->ColLength())
    return;

  occ::handle<NCollection_HArray2<gp_Pnt>>   npoles = new NCollection_HArray2<gp_Pnt>(1, nbpoles, 1, poles->RowLength());
  occ::handle<NCollection_HArray2<double>> nweights =
    new NCollection_HArray2<double>(1, nbpoles, 1, poles->RowLength(), 1.0);
  occ::handle<NCollection_HArray1<double>>    nknots = uknots;
  occ::handle<NCollection_HArray1<int>> nmults = umults;

  if (nbknots != uknots->Length())
  {
    nknots = new NCollection_HArray1<double>(1, nbknots);
    nmults = new NCollection_HArray1<int>(1, nbknots);
  }

  if (urational || vrational)
  {
    BSplSLib::InsertKnots(true,
                          udeg,
                          uperiodic,
                          poles->Array2(),
                          &weights->Array2(),
                          uknots->Array1(),
                          umults->Array1(),
                          Knots,
                          &Mults,
                          npoles->ChangeArray2(),
                          &nweights->ChangeArray2(),
                          nknots->ChangeArray1(),
                          nmults->ChangeArray1(),
                          ParametricTolerance,
                          Add);
  }
  else
  {
    BSplSLib::InsertKnots(true,
                          udeg,
                          uperiodic,
                          poles->Array2(),
                          BSplSLib::NoWeights(),
                          uknots->Array1(),
                          umults->Array1(),
                          Knots,
                          &Mults,
                          npoles->ChangeArray2(),
                          BSplSLib::NoWeights(),
                          nknots->ChangeArray1(),
                          nmults->ChangeArray1(),
                          ParametricTolerance,
                          Add);
  }

  poles   = npoles;
  weights = nweights;
  uknots  = nknots;
  umults  = nmults;
  UpdateUKnots();
}

//=================================================================================================

void Geom_BSplineSurface::InsertVKnots(const NCollection_Array1<double>&    Knots,
                                       const NCollection_Array1<int>& Mults,
                                       const double            ParametricTolerance,
                                       const bool         Add)
{
  // Check and compute new sizes
  int nbpoles, nbknots;

  if (!BSplCLib::PrepareInsertKnots(vdeg,
                                    vperiodic,
                                    vknots->Array1(),
                                    vmults->Array1(),
                                    Knots,
                                    &Mults,
                                    nbpoles,
                                    nbknots,
                                    ParametricTolerance,
                                    Add))
    throw Standard_ConstructionError("Geom_BSplineSurface::InsertVKnots");

  if (nbpoles == poles->RowLength())
    return;

  occ::handle<NCollection_HArray2<gp_Pnt>>   npoles = new NCollection_HArray2<gp_Pnt>(1, poles->ColLength(), 1, nbpoles);
  occ::handle<NCollection_HArray2<double>> nweights =
    new NCollection_HArray2<double>(1, poles->ColLength(), 1, nbpoles, 1.0);
  occ::handle<NCollection_HArray1<double>>    nknots = vknots;
  occ::handle<NCollection_HArray1<int>> nmults = vmults;

  if (nbknots != vknots->Length())
  {
    nknots = new NCollection_HArray1<double>(1, nbknots);
    nmults = new NCollection_HArray1<int>(1, nbknots);
  }

  if (urational || vrational)
  {
    BSplSLib::InsertKnots(false,
                          vdeg,
                          vperiodic,
                          poles->Array2(),
                          &weights->Array2(),
                          vknots->Array1(),
                          vmults->Array1(),
                          Knots,
                          &Mults,
                          npoles->ChangeArray2(),
                          &nweights->ChangeArray2(),
                          nknots->ChangeArray1(),
                          nmults->ChangeArray1(),
                          ParametricTolerance,
                          Add);
  }
  else
  {
    BSplSLib::InsertKnots(false,
                          vdeg,
                          vperiodic,
                          poles->Array2(),
                          BSplSLib::NoWeights(),
                          vknots->Array1(),
                          vmults->Array1(),
                          Knots,
                          &Mults,
                          npoles->ChangeArray2(),
                          BSplSLib::NoWeights(),
                          nknots->ChangeArray1(),
                          nmults->ChangeArray1(),
                          ParametricTolerance,
                          Add);
  }

  poles   = npoles;
  weights = nweights;
  vknots  = nknots;
  vmults  = nmults;
  UpdateVKnots();
}

//=================================================================================================

bool Geom_BSplineSurface::RemoveUKnot(const int Index,
                                                  const int M,
                                                  const double    Tolerance)
{
  if (M < 0)
    return true;

  int I1 = FirstUKnotIndex();
  int I2 = LastUKnotIndex();

  if (!uperiodic && (Index <= I1 || Index >= I2))
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::RemoveUKnot: invalid Index");
  }
  else if (uperiodic && (Index < I1 || Index > I2))
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::RemoveUKnot: invalid Index for periodic case");
  }

  const NCollection_Array2<gp_Pnt>& oldpoles = poles->Array2();

  int step = umults->Value(Index) - M;
  if (step <= 0)
    return true;

  occ::handle<NCollection_HArray2<gp_Pnt>> npoles =
    new NCollection_HArray2<gp_Pnt>(1, oldpoles.ColLength() - step, 1, oldpoles.RowLength());
  occ::handle<NCollection_HArray1<double>>    nknots = uknots;
  occ::handle<NCollection_HArray1<int>> nmults = umults;

  if (M == 0)
  {
    nknots = new NCollection_HArray1<double>(1, uknots->Length() - 1);
    nmults = new NCollection_HArray1<int>(1, uknots->Length() - 1);
  }
  occ::handle<NCollection_HArray2<double>> nweights;
  if (urational || vrational)
  {
    nweights = new NCollection_HArray2<double>(1, npoles->ColLength(), 1, npoles->RowLength());
    if (!BSplSLib::RemoveKnot(true,
                              Index,
                              M,
                              udeg,
                              uperiodic,
                              poles->Array2(),
                              &weights->Array2(),
                              uknots->Array1(),
                              umults->Array1(),
                              npoles->ChangeArray2(),
                              &nweights->ChangeArray2(),
                              nknots->ChangeArray1(),
                              nmults->ChangeArray1(),
                              Tolerance))
      return false;
  }
  else
  {
    //
    // sync the size of the weights
    //
    nweights = new NCollection_HArray2<double>(1, npoles->ColLength(), 1, npoles->RowLength(), 1.0e0);
    if (!BSplSLib::RemoveKnot(true,
                              Index,
                              M,
                              udeg,
                              uperiodic,
                              poles->Array2(),
                              BSplSLib::NoWeights(),
                              uknots->Array1(),
                              umults->Array1(),
                              npoles->ChangeArray2(),
                              BSplSLib::NoWeights(),
                              nknots->ChangeArray1(),
                              nmults->ChangeArray1(),
                              Tolerance))
      return false;
  }

  poles   = npoles;
  weights = nweights;
  uknots  = nknots;
  umults  = nmults;

  maxderivinvok = 0;
  UpdateUKnots();
  return true;
}

//=================================================================================================

bool Geom_BSplineSurface::RemoveVKnot(const int Index,
                                                  const int M,
                                                  const double    Tolerance)
{
  if (M < 0)
    return true;

  int I1 = FirstVKnotIndex();
  int I2 = LastVKnotIndex();

  if (!vperiodic && (Index <= I1 || Index >= I2))
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::RemoveVKnot: invalid Index");
  }
  else if (vperiodic && (Index < I1 || Index > I2))
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::RemoveVKnot: invalid Index for periodic case");
  }

  const NCollection_Array2<gp_Pnt>& oldpoles = poles->Array2();

  int step = vmults->Value(Index) - M;
  if (step <= 0)
    return true;

  occ::handle<NCollection_HArray2<gp_Pnt>> npoles =
    new NCollection_HArray2<gp_Pnt>(1, oldpoles.ColLength(), 1, oldpoles.RowLength() - step);
  occ::handle<NCollection_HArray1<double>>    nknots = vknots;
  occ::handle<NCollection_HArray1<int>> nmults = vmults;

  if (M == 0)
  {
    nknots = new NCollection_HArray1<double>(1, vknots->Length() - 1);
    nmults = new NCollection_HArray1<int>(1, vknots->Length() - 1);
  }
  occ::handle<NCollection_HArray2<double>> nweights;
  if (urational || vrational)
  {
    nweights = new NCollection_HArray2<double>(1, npoles->ColLength(), 1, npoles->RowLength());

    if (!BSplSLib::RemoveKnot(false,
                              Index,
                              M,
                              vdeg,
                              vperiodic,
                              poles->Array2(),
                              &weights->Array2(),
                              vknots->Array1(),
                              vmults->Array1(),
                              npoles->ChangeArray2(),
                              &nweights->ChangeArray2(),
                              nknots->ChangeArray1(),
                              nmults->ChangeArray1(),
                              Tolerance))
      return false;
  }
  else
  {
    //
    // sync the size of the weights array
    //
    nweights = new NCollection_HArray2<double>(1, npoles->ColLength(), 1, npoles->RowLength(), 1.0e0);
    if (!BSplSLib::RemoveKnot(false,
                              Index,
                              M,
                              vdeg,
                              vperiodic,
                              poles->Array2(),
                              BSplSLib::NoWeights(),
                              vknots->Array1(),
                              vmults->Array1(),
                              npoles->ChangeArray2(),
                              BSplSLib::NoWeights(),
                              nknots->ChangeArray1(),
                              nmults->ChangeArray1(),
                              Tolerance))
      return false;
  }

  poles         = npoles;
  vknots        = nknots;
  vmults        = nmults;
  weights       = nweights;
  maxderivinvok = 0;
  UpdateVKnots();
  return true;
}

//=================================================================================================

void Geom_BSplineSurface::Resolution(const double Tolerance3D,
                                     double&      UTolerance,
                                     double&      VTolerance)
{
  if (!maxderivinvok)
  {
    BSplSLib::Resolution(poles->Array2(),
                         &weights->Array2(),
                         uknots->Array1(),
                         vknots->Array1(),
                         umults->Array1(),
                         vmults->Array1(),
                         udeg,
                         vdeg,
                         urational,
                         vrational,
                         uperiodic,
                         vperiodic,
                         1.,
                         umaxderivinv,
                         vmaxderivinv);
    maxderivinvok = 1;
  }
  UTolerance = Tolerance3D * umaxderivinv;
  VTolerance = Tolerance3D * vmaxderivinv;
}
