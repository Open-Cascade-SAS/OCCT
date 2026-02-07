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

#define POLES (myData.Poles)
#define WEIGHTS (myData.Weights)
#define UKNOTS (myData.UKnots)
#define VKNOTS (myData.VKnots)
#define UFKNOTS (myData.UFlatKnots)
#define VFKNOTS (myData.VFlatKnots)
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
                    : N <= myData.UDegree
                             - BSplCLib::MaxKnotMult(myData.UMults,
                                                     myData.UMults.Lower() + 1,
                                                     myData.UMults.Upper() - 1);
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
                    : N <= myData.VDegree
                             - BSplCLib::MaxKnotMult(myData.VMults,
                                                     myData.VMults.Lower() + 1,
                                                     myData.VMults.Upper() - 1);
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
               myData.UDegree,
               myData.VDegree,
               urational,
               vrational,
               myData.IsUPeriodic,
               myData.IsVPeriodic,
               P);
}

//=================================================================================================

void Geom_BSplineSurface::D1(const double U,
                             const double V,
                             gp_Pnt&      P,
                             gp_Vec&      D1U,
                             gp_Vec&      D1V) const
{
  double aNewU = U;
  double aNewV = V;
  PeriodicNormalization(aNewU, aNewV);

  int uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(myData.UDegree,
                            myData.UKnots,
                            &myData.UMults,
                            U,
                            myData.IsUPeriodic,
                            uindex,
                            aNewU);
  uindex = BSplCLib::FlatIndex(myData.UDegree, uindex, myData.UMults, myData.IsUPeriodic);

  BSplCLib::LocateParameter(myData.VDegree,
                            myData.VKnots,
                            &myData.VMults,
                            V,
                            myData.IsVPeriodic,
                            vindex,
                            aNewV);
  vindex = BSplCLib::FlatIndex(myData.VDegree, vindex, myData.VMults, myData.IsVPeriodic);

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
               myData.UDegree,
               myData.VDegree,
               urational,
               vrational,
               myData.IsUPeriodic,
               myData.IsVPeriodic,
               P,
               D1U,
               D1V);
}

//=================================================================================================

void Geom_BSplineSurface::D2(const double U,
                             const double V,
                             gp_Pnt&      P,
                             gp_Vec&      D1U,
                             gp_Vec&      D1V,
                             gp_Vec&      D2U,
                             gp_Vec&      D2V,
                             gp_Vec&      D2UV) const
{
  double aNewU = U;
  double aNewV = V;
  PeriodicNormalization(aNewU, aNewV);

  int uindex = 0, vindex = 0;

  BSplCLib::LocateParameter(myData.UDegree,
                            myData.UKnots,
                            &myData.UMults,
                            U,
                            myData.IsUPeriodic,
                            uindex,
                            aNewU);
  uindex = BSplCLib::FlatIndex(myData.UDegree, uindex, myData.UMults, myData.IsUPeriodic);

  BSplCLib::LocateParameter(myData.VDegree,
                            myData.VKnots,
                            &myData.VMults,
                            V,
                            myData.IsVPeriodic,
                            vindex,
                            aNewV);
  vindex = BSplCLib::FlatIndex(myData.VDegree, vindex, myData.VMults, myData.IsVPeriodic);

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
               myData.UDegree,
               myData.VDegree,
               urational,
               vrational,
               myData.IsUPeriodic,
               myData.IsVPeriodic,
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
               myData.UDegree,
               myData.VDegree,
               urational,
               vrational,
               myData.IsUPeriodic,
               myData.IsVPeriodic,
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

gp_Vec Geom_BSplineSurface::DN(const double U, const double V, const int Nu, const int Nv) const
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
               myData.UDegree,
               myData.VDegree,
               urational,
               vrational,
               myData.IsUPeriodic,
               myData.IsVPeriodic,
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

  BSplCLib::LocateParameter(myData.UDegree,
                            UFKNOTS,
                            U,
                            myData.IsUPeriodic,
                            FromUK1,
                            ToUK2,
                            uindex,
                            u);
  uindex = BSplCLib::FlatIndex(myData.UDegree, uindex, myData.UMults, myData.IsUPeriodic);

  BSplCLib::LocateParameter(myData.VDegree,
                            VFKNOTS,
                            V,
                            myData.IsVPeriodic,
                            FromVK1,
                            ToVK2,
                            vindex,
                            v);
  vindex = BSplCLib::FlatIndex(myData.VDegree, vindex, myData.VMults, myData.IsVPeriodic);

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
               myData.UDegree,
               myData.VDegree,
               urational,
               vrational,
               myData.IsUPeriodic,
               myData.IsVPeriodic,
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

  BSplCLib::LocateParameter(myData.UDegree,
                            UFKNOTS,
                            U,
                            myData.IsUPeriodic,
                            FromUK1,
                            ToUK2,
                            uindex,
                            u);
  uindex = BSplCLib::FlatIndex(myData.UDegree, uindex, myData.UMults, myData.IsUPeriodic);

  BSplCLib::LocateParameter(myData.VDegree,
                            VFKNOTS,
                            V,
                            myData.IsVPeriodic,
                            FromVK1,
                            ToVK2,
                            vindex,
                            v);
  vindex = BSplCLib::FlatIndex(myData.VDegree, vindex, myData.VMults, myData.IsVPeriodic);

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
               myData.UDegree,
               myData.VDegree,
               urational,
               vrational,
               myData.IsUPeriodic,
               myData.IsVPeriodic,
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

  BSplCLib::LocateParameter(myData.UDegree,
                            UFKNOTS,
                            U,
                            myData.IsUPeriodic,
                            FromUK1,
                            ToUK2,
                            uindex,
                            u);
  uindex = BSplCLib::FlatIndex(myData.UDegree, uindex, myData.UMults, myData.IsUPeriodic);

  BSplCLib::LocateParameter(myData.VDegree,
                            VFKNOTS,
                            V,
                            myData.IsVPeriodic,
                            FromVK1,
                            ToVK2,
                            vindex,
                            v);
  vindex = BSplCLib::FlatIndex(myData.VDegree, vindex, myData.VMults, myData.IsVPeriodic);

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
               myData.UDegree,
               myData.VDegree,
               urational,
               vrational,
               myData.IsUPeriodic,
               myData.IsVPeriodic,
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

  BSplCLib::LocateParameter(myData.UDegree,
                            UFKNOTS,
                            U,
                            myData.IsUPeriodic,
                            FromUK1,
                            ToUK2,
                            uindex,
                            u);
  uindex = BSplCLib::FlatIndex(myData.UDegree, uindex, myData.UMults, myData.IsUPeriodic);

  BSplCLib::LocateParameter(myData.VDegree,
                            VFKNOTS,
                            V,
                            myData.IsVPeriodic,
                            FromVK1,
                            ToVK2,
                            vindex,
                            v);
  vindex = BSplCLib::FlatIndex(myData.VDegree, vindex, myData.VMults, myData.IsVPeriodic);

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
               myData.UDegree,
               myData.VDegree,
               urational,
               vrational,
               myData.IsUPeriodic,
               myData.IsVPeriodic,
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

  BSplCLib::LocateParameter(myData.UDegree,
                            UFKNOTS,
                            U,
                            myData.IsUPeriodic,
                            FromUK1,
                            ToUK2,
                            uindex,
                            u);
  uindex = BSplCLib::FlatIndex(myData.UDegree, uindex, myData.UMults, myData.IsUPeriodic);

  BSplCLib::LocateParameter(myData.VDegree,
                            VFKNOTS,
                            V,
                            myData.IsVPeriodic,
                            FromVK1,
                            ToVK2,
                            vindex,
                            v);
  vindex = BSplCLib::FlatIndex(myData.VDegree, vindex, myData.VMults, myData.IsVPeriodic);

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
               myData.UDegree,
               myData.VDegree,
               urational,
               vrational,
               myData.IsUPeriodic,
               myData.IsVPeriodic,
               Vn);
  return Vn;
}

//=================================================================================================

const gp_Pnt& Geom_BSplineSurface::Pole(const int UIndex, const int VIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > myData.Poles.ColLength() || VIndex < 1
                                 || VIndex > myData.Poles.RowLength(),
                               " ");
  return myData.Poles.Value(UIndex, VIndex);
}

//=================================================================================================

void Geom_BSplineSurface::Poles(NCollection_Array2<gp_Pnt>& P) const
{
  Standard_DimensionError_Raise_if(P.ColLength() != myData.Poles.ColLength()
                                     || P.RowLength() != myData.Poles.RowLength(),
                                   " ");
  P = myData.Poles;
}

const NCollection_Array2<gp_Pnt>& Geom_BSplineSurface::Poles() const
{
  return myData.Poles;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BSplineSurface::UIso(const double U) const
{
  NCollection_Array1<gp_Pnt> cpoles(1, myData.Poles.RowLength());
  NCollection_Array1<double> cweights(1, myData.Poles.RowLength());

  occ::handle<Geom_BSplineCurve> C;

  if (urational || vrational)
  {
    BSplSLib::Iso(U,
                  true,
                  POLES,
                  &WEIGHTS,
                  UFKNOTS,
                  FMULTS,
                  myData.UDegree,
                  myData.IsUPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles,
                              cweights,
                              myData.VKnots,
                              myData.VMults,
                              myData.VDegree,
                              myData.IsVPeriodic);
  }
  else
  {
    BSplSLib::Iso(U,
                  true,
                  POLES,
                  BSplSLib::NoWeights(),
                  UFKNOTS,
                  FMULTS,
                  myData.UDegree,
                  myData.IsUPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles,
                              myData.VKnots,
                              myData.VMults,
                              myData.VDegree,
                              myData.IsVPeriodic);
  }

  return C;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BSplineSurface::UIso(const double U, const bool CheckRational) const
{
  NCollection_Array1<gp_Pnt> cpoles(1, myData.Poles.RowLength());
  NCollection_Array1<double> cweights(1, myData.Poles.RowLength());

  occ::handle<Geom_BSplineCurve> C;

  if (urational || vrational)
  {
    BSplSLib::Iso(U,
                  true,
                  POLES,
                  &WEIGHTS,
                  UFKNOTS,
                  FMULTS,
                  myData.UDegree,
                  myData.IsUPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles,
                              cweights,
                              myData.VKnots,
                              myData.VMults,
                              myData.VDegree,
                              myData.IsVPeriodic,
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
                  myData.UDegree,
                  myData.IsUPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles,
                              myData.VKnots,
                              myData.VMults,
                              myData.VDegree,
                              myData.IsVPeriodic);
  }

  return C;
}

//=================================================================================================

double Geom_BSplineSurface::UKnot(const int UIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > myData.UKnots.Length(), " ");
  return myData.UKnots.Value(UIndex);
}

//=================================================================================================

double Geom_BSplineSurface::VKnot(const int VIndex) const
{
  Standard_OutOfRange_Raise_if(VIndex < 1 || VIndex > myData.VKnots.Length(), " ");
  return myData.VKnots.Value(VIndex);
}

//=================================================================================================

void Geom_BSplineSurface::UKnots(NCollection_Array1<double>& Ku) const
{
  Standard_DimensionError_Raise_if(Ku.Length() != myData.UKnots.Length(), " ");
  Ku = myData.UKnots;
}

const NCollection_Array1<double>& Geom_BSplineSurface::UKnots() const
{
  return myData.UKnots;
}

//=================================================================================================

void Geom_BSplineSurface::VKnots(NCollection_Array1<double>& Kv) const
{
  Standard_DimensionError_Raise_if(Kv.Length() != myData.VKnots.Length(), " ");
  Kv = myData.VKnots;
}

const NCollection_Array1<double>& Geom_BSplineSurface::VKnots() const
{
  return myData.VKnots;
}

//=================================================================================================

void Geom_BSplineSurface::UKnotSequence(NCollection_Array1<double>& Ku) const
{
  Standard_DimensionError_Raise_if(Ku.Length() != myData.UFlatKnots.Length(), " ");
  Ku = myData.UFlatKnots;
}

const NCollection_Array1<double>& Geom_BSplineSurface::UKnotSequence() const
{
  return myData.UFlatKnots;
}

//=================================================================================================

void Geom_BSplineSurface::VKnotSequence(NCollection_Array1<double>& Kv) const
{
  Standard_DimensionError_Raise_if(Kv.Length() != myData.VFlatKnots.Length(), " ");
  Kv = myData.VFlatKnots;
}

const NCollection_Array1<double>& Geom_BSplineSurface::VKnotSequence() const
{
  return myData.VFlatKnots;
}

//=================================================================================================

int Geom_BSplineSurface::UMultiplicity(const int UIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > myData.UMults.Length(), " ");
  return myData.UMults.Value(UIndex);
}

//=================================================================================================

void Geom_BSplineSurface::UMultiplicities(NCollection_Array1<int>& Mu) const
{
  Standard_DimensionError_Raise_if(Mu.Length() != myData.UMults.Length(), " ");
  Mu = myData.UMults;
}

const NCollection_Array1<int>& Geom_BSplineSurface::UMultiplicities() const
{
  return myData.UMults;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BSplineSurface::VIso(const double V) const
{
  NCollection_Array1<gp_Pnt> cpoles(1, myData.Poles.ColLength());
  NCollection_Array1<double> cweights(1, myData.Poles.ColLength());

  occ::handle<Geom_BSplineCurve> C;

  if (urational || vrational)
  {
    BSplSLib::Iso(V,
                  false,
                  POLES,
                  &WEIGHTS,
                  VFKNOTS,
                  FMULTS,
                  myData.VDegree,
                  myData.IsVPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles,
                              cweights,
                              myData.UKnots,
                              myData.UMults,
                              myData.UDegree,
                              myData.IsUPeriodic);
  }
  else
  {
    BSplSLib::Iso(V,
                  false,
                  POLES,
                  BSplSLib::NoWeights(),
                  VFKNOTS,
                  FMULTS,
                  myData.VDegree,
                  myData.IsVPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles,
                              myData.UKnots,
                              myData.UMults,
                              myData.UDegree,
                              myData.IsUPeriodic);
  }

  return C;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_BSplineSurface::VIso(const double V, const bool CheckRational) const
{
  NCollection_Array1<gp_Pnt> cpoles(1, myData.Poles.ColLength());
  NCollection_Array1<double> cweights(1, myData.Poles.ColLength());

  occ::handle<Geom_BSplineCurve> C;

  if (urational || vrational)
  {
    BSplSLib::Iso(V,
                  false,
                  POLES,
                  &WEIGHTS,
                  VFKNOTS,
                  FMULTS,
                  myData.VDegree,
                  myData.IsVPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles,
                              cweights,
                              myData.UKnots,
                              myData.UMults,
                              myData.UDegree,
                              myData.IsUPeriodic,
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
                  myData.VDegree,
                  myData.IsVPeriodic,
                  cpoles,
                  &cweights);
    C = new Geom_BSplineCurve(cpoles,
                              myData.UKnots,
                              myData.UMults,
                              myData.UDegree,
                              myData.IsUPeriodic);
  }

  return C;
}

//=================================================================================================

int Geom_BSplineSurface::VMultiplicity(const int VIndex) const
{
  Standard_OutOfRange_Raise_if(VIndex < 1 || VIndex > myData.VMults.Length(), " ");
  return myData.VMults.Value(VIndex);
}

//=================================================================================================

void Geom_BSplineSurface::VMultiplicities(NCollection_Array1<int>& Mv) const
{
  Standard_DimensionError_Raise_if(Mv.Length() != myData.VMults.Length(), " ");
  Mv = myData.VMults;
}

const NCollection_Array1<int>& Geom_BSplineSurface::VMultiplicities() const
{
  return myData.VMults;
}

//=================================================================================================

double Geom_BSplineSurface::Weight(const int UIndex, const int VIndex) const
{
  Standard_OutOfRange_Raise_if(UIndex < 1 || UIndex > myData.Weights.ColLength() || VIndex < 1
                                 || VIndex > myData.Weights.RowLength(),
                               " ");
  return myData.Weights.Value(UIndex, VIndex);
}

//=================================================================================================

void Geom_BSplineSurface::Weights(NCollection_Array2<double>& W) const
{
  Standard_DimensionError_Raise_if(W.ColLength() != myData.Weights.ColLength()
                                     || W.RowLength() != myData.Weights.RowLength(),
                                   " ");
  W = myData.Weights;
}

const NCollection_Array2<double>* Geom_BSplineSurface::Weights() const
{
  if (urational || vrational)
    return &myData.Weights;
  return BSplSLib::NoWeights();
}

//=================================================================================================

const NCollection_Array2<double>* Geom_BSplineSurface::InternalWeights() const
{
  if (urational || vrational)
    return &myData.Weights;
  return nullptr;
}

//=================================================================================================

void Geom_BSplineSurface::Transform(const gp_Trsf& T)
{
  for (int j = myData.Poles.LowerCol(); j <= myData.Poles.UpperCol(); j++)
  {
    for (int i = myData.Poles.LowerRow(); i <= myData.Poles.UpperRow(); i++)
    {
      myData.Poles(i, j).Transform(T);
    }
  }
}

//=================================================================================================

void Geom_BSplineSurface::SetUPeriodic()
{
  int first = FirstUKnotIndex();
  int last  = LastUKnotIndex();

  NCollection_Array1<double> cknots(1, last - first + 1);
  for (int k = first; k <= last; k++)
    cknots(k - first + 1) = myData.UKnots(k);
  myData.UKnots = std::move(cknots);

  NCollection_Array1<int> cmults(1, last - first + 1);
  for (int k = first; k <= last; k++)
    cmults(k - first + 1) = myData.UMults(k);
  cmults(1) = cmults(cmults.Upper()) = std::min(myData.UDegree, std::max(cmults(1), cmults(cmults.Upper())));
  myData.UMults = std::move(cmults);

  // compute new number of poles;
  int nbp = BSplCLib::NbPoles(myData.UDegree, true, myData.UMults);

  myData.Poles.ResizeWithTrim(1, nbp, myData.Poles.LowerCol(), myData.Poles.UpperCol(), true);
  myData.Weights.ResizeWithTrim(1, nbp, myData.Weights.LowerCol(), myData.Weights.UpperCol(), true);

  myData.IsUPeriodic = true;

  maxderivinvok = false;
  UpdateUKnots();
}

//=================================================================================================

void Geom_BSplineSurface::SetVPeriodic()
{
  int first = FirstVKnotIndex();
  int last  = LastVKnotIndex();

  NCollection_Array1<double> cknots(1, last - first + 1);
  for (int k = first; k <= last; k++)
    cknots(k - first + 1) = myData.VKnots(k);
  myData.VKnots = std::move(cknots);

  NCollection_Array1<int> cmults(1, last - first + 1);
  for (int k = first; k <= last; k++)
    cmults(k - first + 1) = myData.VMults(k);
  cmults(1) = cmults(cmults.Upper()) = std::min(myData.VDegree, std::max(cmults(1), cmults(cmults.Upper())));
  myData.VMults = std::move(cmults);

  // compute new number of poles;
  int nbp = BSplCLib::NbPoles(myData.VDegree, true, myData.VMults);

  myData.Poles.ResizeWithTrim(myData.Poles.LowerRow(), myData.Poles.UpperRow(), 1, nbp, true);
  if (urational || vrational)
  {
    myData.Weights.ResizeWithTrim(myData.Weights.LowerRow(), myData.Weights.UpperRow(), 1, nbp, true);
  }

  myData.IsVPeriodic = true;

  maxderivinvok = false;
  UpdateVKnots();
}

//=================================================================================================

void Geom_BSplineSurface::SetUOrigin(const int Index)
{
  if (!myData.IsUPeriodic)
    throw Standard_NoSuchObject("Geom_BSplineSurface::SetUOrigin: surface is not U periodic");

  int i, j, k;
  int first = FirstUKnotIndex();
  int last  = LastUKnotIndex();

  if ((Index < first) || (Index > last))
    throw Standard_DomainError("Geom_BSplineCurve::SetUOrigin: Index out of range");

  int nbknots = myData.UKnots.Length();
  int nbpoles = myData.Poles.ColLength();

  NCollection_Array1<double> newknots(1, nbknots);

  NCollection_Array1<int> newmults(1, nbknots);

  // set the knots and mults
  double period = myData.UKnots.Value(last) - myData.UKnots.Value(first);
  k             = 1;
  for (i = Index; i <= last; i++)
  {
    newknots(k) = myData.UKnots.Value(i);
    newmults(k) = myData.UMults.Value(i);
    k++;
  }
  for (i = first + 1; i <= Index; i++)
  {
    newknots(k) = myData.UKnots.Value(i) + period;
    newmults(k) = myData.UMults.Value(i);
    k++;
  }

  int index = 1;
  for (i = first + 1; i <= Index; i++)
    index += myData.UMults.Value(i);

  // set the poles and weights
  int                        nbvp = myData.Poles.RowLength();
  NCollection_Array2<gp_Pnt> newpoles(1, nbpoles, 1, nbvp);
  NCollection_Array2<double> newweights(1, nbpoles, 1, nbvp);
  first = myData.Poles.LowerRow();
  last  = myData.Poles.UpperRow();
  if (urational || vrational)
  {
    k = 1;
    for (i = index; i <= last; i++)
    {
      for (j = 1; j <= nbvp; j++)
      {
        newpoles(k, j)   = myData.Poles.Value(i, j);
        newweights(k, j) = myData.Weights.Value(i, j);
      }
      k++;
    }
    for (i = first; i < index; i++)
    {
      for (j = 1; j <= nbvp; j++)
      {
        newpoles(k, j)   = myData.Poles.Value(i, j);
        newweights(k, j) = myData.Weights.Value(i, j);
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
        newpoles(k, j) = myData.Poles.Value(i, j);
      }
      k++;
    }
    for (i = first; i < index; i++)
    {
      for (j = 1; j <= nbvp; j++)
      {
        newpoles(k, j) = myData.Poles.Value(i, j);
      }
      k++;
    }
  }

  myData.Poles  = std::move(newpoles);
  myData.UKnots = std::move(newknots);
  myData.UMults = std::move(newmults);
  if (urational || vrational)
    myData.Weights = std::move(newweights);
  UpdateUKnots();
}

//=================================================================================================

void Geom_BSplineSurface::SetVOrigin(const int Index)
{
  if (!myData.IsVPeriodic)
    throw Standard_NoSuchObject("Geom_BSplineSurface::SetVOrigin: surface is not V periodic");

  int i, j, k;
  int first = FirstVKnotIndex();
  int last  = LastVKnotIndex();

  if ((Index < first) || (Index > last))
    throw Standard_DomainError("Geom_BSplineCurve::SetVOrigin: Index out of range");

  int nbknots = myData.VKnots.Length();
  int nbpoles = myData.Poles.RowLength();

  NCollection_Array1<double> newknots(1, nbknots);

  NCollection_Array1<int> newmults(1, nbknots);

  // set the knots and mults
  double period = myData.VKnots.Value(last) - myData.VKnots.Value(first);
  k             = 1;
  for (i = Index; i <= last; i++)
  {
    newknots(k) = myData.VKnots.Value(i);
    newmults(k) = myData.VMults.Value(i);
    k++;
  }
  for (i = first + 1; i <= Index; i++)
  {
    newknots(k) = myData.VKnots.Value(i) + period;
    newmults(k) = myData.VMults.Value(i);
    k++;
  }

  int index = 1;
  for (i = first + 1; i <= Index; i++)
    index += myData.VMults.Value(i);

  // set the poles and weights
  int                        nbup = myData.Poles.ColLength();
  NCollection_Array2<gp_Pnt> newpoles(1, nbup, 1, nbpoles);
  NCollection_Array2<double> newweights(1, nbup, 1, nbpoles);
  first = myData.Poles.LowerCol();
  last  = myData.Poles.UpperCol();
  if (urational || vrational)
  {
    k = 1;
    for (j = index; j <= last; j++)
    {
      for (i = 1; i <= nbup; i++)
      {
        newpoles(i, k)   = myData.Poles.Value(i, j);
        newweights(i, k) = myData.Weights.Value(i, j);
      }
      k++;
    }
    for (j = first; j < index; j++)
    {
      for (i = 1; i <= nbup; i++)
      {
        newpoles(i, k)   = myData.Poles.Value(i, j);
        newweights(i, k) = myData.Weights.Value(i, j);
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
        newpoles(i, k) = myData.Poles.Value(i, j);
      }
      k++;
    }
    for (j = first; j < index; j++)
    {
      for (i = 1; i <= nbup; i++)
      {
        newpoles(i, k) = myData.Poles.Value(i, j);
      }
      k++;
    }
  }

  myData.Poles  = std::move(newpoles);
  myData.VKnots = std::move(newknots);
  myData.VMults = std::move(newmults);
  if (urational || vrational)
    myData.Weights = std::move(newweights);
  UpdateVKnots();
}

//=================================================================================================

void Geom_BSplineSurface::SetUNotPeriodic()
{
  if (myData.IsUPeriodic)
  {
    int NbKnots, NbPoles;
    BSplCLib::PrepareUnperiodize(myData.UDegree, myData.UMults, NbKnots, NbPoles);

    NCollection_Array2<gp_Pnt> npoles(1, NbPoles, 1, myData.Poles.RowLength());

    NCollection_Array1<double> nknots(1, NbKnots);

    NCollection_Array1<int> nmults(1, NbKnots);

    NCollection_Array2<double> nweights(1, NbPoles, 1, myData.Poles.RowLength(), 0);

    if (urational || vrational)
    {

      BSplSLib::Unperiodize(true,
                            myData.UDegree,
                            myData.UMults,
                            myData.UKnots,
                            myData.Poles,
                            &myData.Weights,
                            nmults,
                            nknots,
                            npoles,
                            &nweights);
    }
    else
    {

      BSplSLib::Unperiodize(true,
                            myData.UDegree,
                            myData.UMults,
                            myData.UKnots,
                            myData.Poles,
                            BSplSLib::NoWeights(),
                            nmults,
                            nknots,
                            npoles,
                            BSplSLib::NoWeights());
    }
    myData.Poles       = std::move(npoles);
    myData.Weights     = std::move(nweights);
    myData.UMults      = std::move(nmults);
    myData.UKnots      = std::move(nknots);
    myData.IsUPeriodic = false;

    maxderivinvok = false;
    UpdateUKnots();
  }
}

//=================================================================================================

void Geom_BSplineSurface::SetVNotPeriodic()
{
  if (myData.IsVPeriodic)
  {
    int NbKnots, NbPoles;
    BSplCLib::PrepareUnperiodize(myData.VDegree, myData.VMults, NbKnots, NbPoles);

    NCollection_Array2<gp_Pnt> npoles(1, myData.Poles.ColLength(), 1, NbPoles);

    NCollection_Array1<double> nknots(1, NbKnots);

    NCollection_Array1<int> nmults(1, NbKnots);

    NCollection_Array2<double> nweights(1, myData.Poles.ColLength(), 1, NbPoles, 0);

    if (urational || vrational)
    {

      BSplSLib::Unperiodize(false,
                            myData.VDegree,
                            myData.VMults,
                            myData.VKnots,
                            myData.Poles,
                            &myData.Weights,
                            nmults,
                            nknots,
                            npoles,
                            &nweights);
    }
    else
    {

      BSplSLib::Unperiodize(false,
                            myData.VDegree,
                            myData.VMults,
                            myData.VKnots,
                            myData.Poles,
                            BSplSLib::NoWeights(),
                            nmults,
                            nknots,
                            npoles,
                            BSplSLib::NoWeights());
    }
    myData.Poles       = std::move(npoles);
    myData.Weights     = std::move(nweights);
    myData.VMults      = std::move(nmults);
    myData.VKnots      = std::move(nknots);
    myData.IsVPeriodic = false;

    maxderivinvok = false;
    UpdateVKnots();
  }
}

//=================================================================================================

bool Geom_BSplineSurface::IsUClosed() const
{
  if (myData.IsUPeriodic)
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
  if (myData.IsVPeriodic)
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
  return myData.IsUPeriodic;
}

//=================================================================================================

bool Geom_BSplineSurface::IsVPeriodic() const
{
  return myData.IsVPeriodic;
}

//=================================================================================================

int Geom_BSplineSurface::FirstUKnotIndex() const
{
  if (myData.IsUPeriodic)
    return 1;
  else
    return BSplCLib::FirstUKnotIndex(myData.UDegree, myData.UMults);
}

//=================================================================================================

int Geom_BSplineSurface::FirstVKnotIndex() const
{
  if (myData.IsVPeriodic)
    return 1;
  else
    return BSplCLib::FirstUKnotIndex(myData.VDegree, myData.VMults);
}

//=================================================================================================

int Geom_BSplineSurface::LastUKnotIndex() const
{
  if (myData.IsUPeriodic)
    return myData.UKnots.Length();
  else
    return BSplCLib::LastUKnotIndex(myData.UDegree, myData.UMults);
}

//=================================================================================================

int Geom_BSplineSurface::LastVKnotIndex() const
{
  if (myData.IsVPeriodic)
    return myData.VKnots.Length();
  else
    return BSplCLib::LastUKnotIndex(myData.VDegree, myData.VMults);
}

//=================================================================================================

void Geom_BSplineSurface::LocateU(const double U,
                                  const double ParametricTolerance,
                                  int&         I1,
                                  int&         I2,
                                  const bool   WithKnotRepetition) const
{
  double NewU = U, vbid = myData.VKnots.Value(1);
  const NCollection_Array1<double>& TheKnots =
    WithKnotRepetition ? myData.UFlatKnots : myData.UKnots;

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
  double NewV = V, ubid = myData.UKnots.Value(1);
  const NCollection_Array1<double>& TheKnots =
    WithKnotRepetition ? myData.VFlatKnots : myData.VKnots;

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
  BSplCLib::Reverse(myData.UMults);
  BSplCLib::Reverse(myData.UKnots);
  int last;
  if (myData.IsUPeriodic)
    last = myData.UFlatKnots.Upper() - myData.UDegree - 1;
  else
    last = myData.Poles.UpperRow();
  BSplSLib::Reverse(myData.Poles, last, true);
  if (urational || vrational)
    BSplSLib::Reverse(myData.Weights, last, true);
  UpdateUKnots();
}

//=================================================================================================

double Geom_BSplineSurface::UReversedParameter(const double U) const
{
  return (myData.UKnots.Value(1) + myData.UKnots.Value(myData.UKnots.Length()) - U);
}

//=================================================================================================

void Geom_BSplineSurface::VReverse()
{
  BSplCLib::Reverse(myData.VMults);
  BSplCLib::Reverse(myData.VKnots);
  int last;
  if (myData.IsVPeriodic)
    last = myData.VFlatKnots.Upper() - myData.VDegree - 1;
  else
    last = myData.Poles.UpperCol();
  BSplSLib::Reverse(myData.Poles, last, false);
  if (urational || vrational)
    BSplSLib::Reverse(myData.Weights, last, false);
  UpdateVKnots();
}

//=================================================================================================

double Geom_BSplineSurface::VReversedParameter(const double V) const
{
  return (myData.VKnots.Value(1) + myData.VKnots.Value(myData.VKnots.Length()) - V);
}

//=================================================================================================

void Geom_BSplineSurface::SetPoleCol(const int VIndex, const NCollection_Array1<gp_Pnt>& CPoles)
{
  if (VIndex < 1 || VIndex > myData.Poles.RowLength())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetPoleCol: VIndex out of range");
  }
  if (CPoles.Lower() < 1 || CPoles.Lower() > myData.Poles.ColLength() || CPoles.Upper() < 1
      || CPoles.Upper() > myData.Poles.ColLength())
  {
    throw Standard_ConstructionError("Geom_BSplineSurface::SetPoleCol: invalid array dimension");
  }

  for (int I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    myData.Poles(I + myData.Poles.LowerRow() - 1, VIndex + myData.Poles.LowerCol() - 1) =
      CPoles(I);
  }
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
  if (UIndex < 1 || UIndex > myData.Poles.ColLength())
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::SetPoleRow: UIndex out of range");
  }
  if (CPoles.Lower() < 1 || CPoles.Lower() > myData.Poles.RowLength() || CPoles.Upper() < 1
      || CPoles.Upper() > myData.Poles.RowLength())
  {
    throw Standard_ConstructionError("Geom_BSplineSurface::SetPoleRow: invalid array dimension");
  }

  for (int I = CPoles.Lower(); I <= CPoles.Upper(); I++)
  {
    myData.Poles(UIndex + myData.Poles.LowerRow() - 1, I + myData.Poles.LowerCol() - 1) =
      CPoles(I);
  }
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
  myData.Poles.SetValue(UIndex + myData.Poles.LowerRow() - 1,
                        VIndex + myData.Poles.LowerCol() - 1,
                        P);
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
  if (UIndex1 < 1 || UIndex1 > myData.Poles.UpperRow() || UIndex2 < 1
      || UIndex2 > myData.Poles.UpperRow() || UIndex1 > UIndex2 || VIndex1 < 1
      || VIndex1 > myData.Poles.UpperCol() || VIndex2 < 1
      || VIndex2 > myData.Poles.UpperCol() || VIndex1 > VIndex2)
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::MovePoint: Index and #pole mismatch");
  }

  NCollection_Array2<gp_Pnt> npoles(1, myData.Poles.UpperRow(), 1, myData.Poles.UpperCol());
  gp_Pnt                     P0;
  D0(U, V, P0);
  gp_Vec Displ(P0, P);
  bool   rational = (urational || vrational);
  BSplSLib::MovePoint(U,
                      V,
                      Displ,
                      UIndex1,
                      UIndex2,
                      VIndex1,
                      VIndex2,
                      myData.UDegree,
                      myData.VDegree,
                      rational,
                      myData.Poles,
                      myData.Weights,
                      myData.UFlatKnots,
                      myData.VFlatKnots,
                      UFirstModifiedPole,
                      ULastmodifiedPole,
                      VFirstModifiedPole,
                      VLastmodifiedPole,
                      npoles);
  if (UFirstModifiedPole)
  {
    myData.Poles = std::move(npoles);
  }
  maxderivinvok = false;
}

//=================================================================================================

void Geom_BSplineSurface::Bounds(double& U1, double& U2, double& V1, double& V2) const
{
  U1 = myData.UFlatKnots.Value(myData.UDegree + 1);
  U2 = myData.UFlatKnots.Value(myData.UFlatKnots.Upper() - myData.UDegree);
  V1 = myData.VFlatKnots.Value(myData.VDegree + 1);
  V2 = myData.VFlatKnots.Value(myData.VFlatKnots.Upper() - myData.VDegree);
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
  return myData.UKnots.Length();
}

//=================================================================================================

int Geom_BSplineSurface::NbUPoles() const
{
  return myData.Poles.ColLength();
}

//=================================================================================================

int Geom_BSplineSurface::NbVKnots() const
{
  return myData.VKnots.Length();
}

//=================================================================================================

int Geom_BSplineSurface::NbVPoles() const
{
  return myData.Poles.RowLength();
}

//=================================================================================================

int Geom_BSplineSurface::UDegree() const
{
  return myData.UDegree;
}

//=================================================================================================

int Geom_BSplineSurface::VDegree() const
{
  return myData.VDegree;
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

void Geom_BSplineSurface::InsertUKnots(const NCollection_Array1<double>& Knots,
                                       const NCollection_Array1<int>&    Mults,
                                       const double                      ParametricTolerance,
                                       const bool                        Add)
{
  // Check and compute new sizes
  int nbpoles, nbknots;

  if (!BSplCLib::PrepareInsertKnots(myData.UDegree,
                                    myData.IsUPeriodic,
                                    myData.UKnots,
                                    myData.UMults,
                                    Knots,
                                    &Mults,
                                    nbpoles,
                                    nbknots,
                                    ParametricTolerance,
                                    Add))
    throw Standard_ConstructionError("Geom_BSplineSurface::InsertUKnots");

  if (nbpoles == myData.Poles.ColLength())
    return;

  NCollection_Array2<gp_Pnt> npoles(1, nbpoles, 1, myData.Poles.RowLength());
  NCollection_Array2<double> nweights(1, nbpoles, 1, myData.Poles.RowLength(), 1.0);
  NCollection_Array1<double> nknots(1, nbknots);
  NCollection_Array1<int>    nmults(1, nbknots);

  if (urational || vrational)
  {
    BSplSLib::InsertKnots(true,
                          myData.UDegree,
                          myData.IsUPeriodic,
                          myData.Poles,
                          &myData.Weights,
                          myData.UKnots,
                          myData.UMults,
                          Knots,
                          &Mults,
                          npoles,
                          &nweights,
                          nknots,
                          nmults,
                          ParametricTolerance,
                          Add);
  }
  else
  {
    BSplSLib::InsertKnots(true,
                          myData.UDegree,
                          myData.IsUPeriodic,
                          myData.Poles,
                          BSplSLib::NoWeights(),
                          myData.UKnots,
                          myData.UMults,
                          Knots,
                          &Mults,
                          npoles,
                          BSplSLib::NoWeights(),
                          nknots,
                          nmults,
                          ParametricTolerance,
                          Add);
  }

  myData.Poles   = std::move(npoles);
  myData.Weights = std::move(nweights);
  myData.UKnots  = std::move(nknots);
  myData.UMults  = std::move(nmults);
  UpdateUKnots();
}

//=================================================================================================

void Geom_BSplineSurface::InsertVKnots(const NCollection_Array1<double>& Knots,
                                       const NCollection_Array1<int>&    Mults,
                                       const double                      ParametricTolerance,
                                       const bool                        Add)
{
  // Check and compute new sizes
  int nbpoles, nbknots;

  if (!BSplCLib::PrepareInsertKnots(myData.VDegree,
                                    myData.IsVPeriodic,
                                    myData.VKnots,
                                    myData.VMults,
                                    Knots,
                                    &Mults,
                                    nbpoles,
                                    nbknots,
                                    ParametricTolerance,
                                    Add))
    throw Standard_ConstructionError("Geom_BSplineSurface::InsertVKnots");

  if (nbpoles == myData.Poles.RowLength())
    return;

  NCollection_Array2<gp_Pnt> npoles(1, myData.Poles.ColLength(), 1, nbpoles);
  NCollection_Array2<double> nweights(1, myData.Poles.ColLength(), 1, nbpoles, 1.0);
  NCollection_Array1<double> nknots(1, nbknots);
  NCollection_Array1<int>    nmults(1, nbknots);

  if (urational || vrational)
  {
    BSplSLib::InsertKnots(false,
                          myData.VDegree,
                          myData.IsVPeriodic,
                          myData.Poles,
                          &myData.Weights,
                          myData.VKnots,
                          myData.VMults,
                          Knots,
                          &Mults,
                          npoles,
                          &nweights,
                          nknots,
                          nmults,
                          ParametricTolerance,
                          Add);
  }
  else
  {
    BSplSLib::InsertKnots(false,
                          myData.VDegree,
                          myData.IsVPeriodic,
                          myData.Poles,
                          BSplSLib::NoWeights(),
                          myData.VKnots,
                          myData.VMults,
                          Knots,
                          &Mults,
                          npoles,
                          BSplSLib::NoWeights(),
                          nknots,
                          nmults,
                          ParametricTolerance,
                          Add);
  }

  myData.Poles   = std::move(npoles);
  myData.Weights = std::move(nweights);
  myData.VKnots  = std::move(nknots);
  myData.VMults  = std::move(nmults);
  UpdateVKnots();
}

//=================================================================================================

bool Geom_BSplineSurface::RemoveUKnot(const int Index, const int M, const double Tolerance)
{
  if (M < 0)
    return true;

  int I1 = FirstUKnotIndex();
  int I2 = LastUKnotIndex();

  if (!myData.IsUPeriodic && (Index <= I1 || Index >= I2))
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::RemoveUKnot: invalid Index");
  }
  else if (myData.IsUPeriodic && (Index < I1 || Index > I2))
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::RemoveUKnot: invalid Index for periodic case");
  }

  int step = myData.UMults.Value(Index) - M;
  if (step <= 0)
    return true;

  NCollection_Array2<gp_Pnt> npoles(1,
                                    myData.Poles.ColLength() - step,
                                    1,
                                    myData.Poles.RowLength());
  const int aNewUKnotLen = (M == 0) ? myData.UKnots.Length() - 1 : myData.UKnots.Length();
  NCollection_Array1<double> nknots(1, aNewUKnotLen);
  NCollection_Array1<int>    nmults(1, aNewUKnotLen);
  NCollection_Array2<double> nweights;
  if (urational || vrational)
  {
    nweights = NCollection_Array2<double>(1, npoles.ColLength(), 1, npoles.RowLength());
    if (!BSplSLib::RemoveKnot(true,
                              Index,
                              M,
                              myData.UDegree,
                              myData.IsUPeriodic,
                              myData.Poles,
                              &myData.Weights,
                              myData.UKnots,
                              myData.UMults,
                              npoles,
                              &nweights,
                              nknots,
                              nmults,
                              Tolerance))
      return false;
  }
  else
  {
    //
    // sync the size of the weights
    //
    nweights = NCollection_Array2<double>(1, npoles.ColLength(), 1, npoles.RowLength(), 1.0e0);
    if (!BSplSLib::RemoveKnot(true,
                              Index,
                              M,
                              myData.UDegree,
                              myData.IsUPeriodic,
                              myData.Poles,
                              BSplSLib::NoWeights(),
                              myData.UKnots,
                              myData.UMults,
                              npoles,
                              BSplSLib::NoWeights(),
                              nknots,
                              nmults,
                              Tolerance))
      return false;
  }

  myData.Poles   = std::move(npoles);
  myData.Weights = std::move(nweights);
  myData.UKnots  = std::move(nknots);
  myData.UMults  = std::move(nmults);

  maxderivinvok = false;
  UpdateUKnots();
  return true;
}

//=================================================================================================

bool Geom_BSplineSurface::RemoveVKnot(const int Index, const int M, const double Tolerance)
{
  if (M < 0)
    return true;

  int I1 = FirstVKnotIndex();
  int I2 = LastVKnotIndex();

  if (!myData.IsVPeriodic && (Index <= I1 || Index >= I2))
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::RemoveVKnot: invalid Index");
  }
  else if (myData.IsVPeriodic && (Index < I1 || Index > I2))
  {
    throw Standard_OutOfRange("Geom_BSplineSurface::RemoveVKnot: invalid Index for periodic case");
  }

  int step = myData.VMults.Value(Index) - M;
  if (step <= 0)
    return true;

  NCollection_Array2<gp_Pnt> npoles(1,
                                    myData.Poles.ColLength(),
                                    1,
                                    myData.Poles.RowLength() - step);
  const int aNewVKnotLen = (M == 0) ? myData.VKnots.Length() - 1 : myData.VKnots.Length();
  NCollection_Array1<double> nknots(1, aNewVKnotLen);
  NCollection_Array1<int>    nmults(1, aNewVKnotLen);
  NCollection_Array2<double> nweights;
  if (urational || vrational)
  {
    nweights = NCollection_Array2<double>(1, npoles.ColLength(), 1, npoles.RowLength());

    if (!BSplSLib::RemoveKnot(false,
                              Index,
                              M,
                              myData.VDegree,
                              myData.IsVPeriodic,
                              myData.Poles,
                              &myData.Weights,
                              myData.VKnots,
                              myData.VMults,
                              npoles,
                              &nweights,
                              nknots,
                              nmults,
                              Tolerance))
      return false;
  }
  else
  {
    //
    // sync the size of the weights array
    //
    nweights = NCollection_Array2<double>(1, npoles.ColLength(), 1, npoles.RowLength(), 1.0e0);
    if (!BSplSLib::RemoveKnot(false,
                              Index,
                              M,
                              myData.VDegree,
                              myData.IsVPeriodic,
                              myData.Poles,
                              BSplSLib::NoWeights(),
                              myData.VKnots,
                              myData.VMults,
                              npoles,
                              BSplSLib::NoWeights(),
                              nknots,
                              nmults,
                              Tolerance))
      return false;
  }

  myData.Poles    = std::move(npoles);
  myData.VKnots   = std::move(nknots);
  myData.VMults   = std::move(nmults);
  myData.Weights  = std::move(nweights);
  maxderivinvok   = false;
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
    BSplSLib::Resolution(myData.Poles,
                         &myData.Weights,
                         myData.UKnots,
                         myData.VKnots,
                         myData.UMults,
                         myData.VMults,
                         myData.UDegree,
                         myData.VDegree,
                         urational,
                         vrational,
                         myData.IsUPeriodic,
                         myData.IsVPeriodic,
                         1.,
                         umaxderivinv,
                         vmaxderivinv);
    maxderivinvok = true;
  }
  UTolerance = Tolerance3D * umaxderivinv;
  VTolerance = Tolerance3D * vmaxderivinv;
}
