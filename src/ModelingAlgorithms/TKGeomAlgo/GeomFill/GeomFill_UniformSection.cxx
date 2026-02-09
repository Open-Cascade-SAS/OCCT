// Created on: 1997-12-05
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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

#include <GCPnts_AbscissaPoint.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomConvert.hxx>
#include <GeomFill_UniformSection.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

#include <cstdio>
IMPLEMENT_STANDARD_RTTIEXT(GeomFill_UniformSection, GeomFill_SectionLaw)

GeomFill_UniformSection::GeomFill_UniformSection(const occ::handle<Geom_Curve>& C,
                                                 const double                   FirstParameter,
                                                 const double                   LastParameter)
    : First(FirstParameter),
      Last(LastParameter)
{
  mySection = occ::down_cast<Geom_Curve>(C->Copy());
  myCurve   = occ::down_cast<Geom_BSplineCurve>(C);
  if (myCurve.IsNull())
  {
    myCurve = GeomConvert::CurveToBSplineCurve(C, Convert_QuasiAngular);
    if (myCurve->IsPeriodic())
    {
      int M = myCurve->Degree() / 2 + 1;
      myCurve->RemoveKnot(1, M, Precision::Confusion());
    }
  }
}

//=======================================================
// Purpose :D0
//=======================================================
bool GeomFill_UniformSection::D0(const double,
                                 NCollection_Array1<gp_Pnt>& Poles,
                                 NCollection_Array1<double>& Weights)
{
  Poles   = myCurve->Poles();
  Weights = myCurve->WeightsArray();

  return true;
}

//=======================================================
// Purpose :D1
//=======================================================
bool GeomFill_UniformSection::D1(const double,
                                 NCollection_Array1<gp_Pnt>& Poles,
                                 NCollection_Array1<gp_Vec>& DPoles,
                                 NCollection_Array1<double>& Weights,
                                 NCollection_Array1<double>& DWeights)
{
  Poles   = myCurve->Poles();
  Weights = myCurve->WeightsArray();
  gp_Vec V0(0, 0, 0);
  DPoles.Init(V0);
  DWeights.Init(0);

  return true;
}

//=======================================================
// Purpose :D2
//=======================================================
bool GeomFill_UniformSection::D2(const double,
                                 NCollection_Array1<gp_Pnt>& Poles,
                                 NCollection_Array1<gp_Vec>& DPoles,
                                 NCollection_Array1<gp_Vec>& D2Poles,
                                 NCollection_Array1<double>& Weights,
                                 NCollection_Array1<double>& DWeights,
                                 NCollection_Array1<double>& D2Weights)
{
  Poles   = myCurve->Poles();
  Weights = myCurve->WeightsArray();
  gp_Vec V0(0, 0, 0);
  DPoles.Init(V0);
  DWeights.Init(0);
  D2Poles.Init(V0);
  D2Weights.Init(0);

  return true;
}

//=======================================================
// Purpose :BSplineSurface()
//=======================================================
occ::handle<Geom_BSplineSurface> GeomFill_UniformSection::BSplineSurface() const
{
  int                              ii, NbPoles = myCurve->NbPoles();
  NCollection_Array2<gp_Pnt>       Poles(1, NbPoles, 1, 2);
  const NCollection_Array1<double>& CurKnots = myCurve->Knots();
  NCollection_Array1<double>       UKnots(CurKnots);
  NCollection_Array1<double>       VKnots(1, 2);
  const NCollection_Array1<int>&   CurMults = myCurve->Multiplicities();
  NCollection_Array1<int>          UMults(CurMults);
  NCollection_Array1<int>          VMults(1, 2);

  for (ii = 1; ii <= NbPoles; ii++)
  {
    Poles(ii, 1) = Poles(ii, 2) = myCurve->Pole(ii);
  }

  VKnots(1) = First;
  VKnots(2) = Last;

  VMults.Init(2);

  occ::handle<Geom_BSplineSurface> BS = new (Geom_BSplineSurface)(Poles,
                                                                  UKnots,
                                                                  VKnots,
                                                                  UMults,
                                                                  VMults,
                                                                  myCurve->Degree(),
                                                                  1,
                                                                  myCurve->IsPeriodic());

  return BS;
}

//=======================================================
// Purpose :SectionShape
//=======================================================
void GeomFill_UniformSection::SectionShape(int& NbPoles, int& NbKnots, int& Degree) const
{
  NbPoles = myCurve->NbPoles();
  NbKnots = myCurve->NbKnots();
  Degree  = myCurve->Degree();
}

void GeomFill_UniformSection::Knots(NCollection_Array1<double>& TKnots) const
{
  TKnots = myCurve->Knots();
}

//=======================================================
// Purpose :Mults
//=======================================================
void GeomFill_UniformSection::Mults(NCollection_Array1<int>& TMults) const
{
  TMults = myCurve->Multiplicities();
}

//=======================================================
// Purpose :IsRational
//=======================================================
bool GeomFill_UniformSection::IsRational() const
{
  return myCurve->IsRational();
}

//=======================================================
// Purpose :IsUPeriodic
//=======================================================
bool GeomFill_UniformSection::IsUPeriodic() const
{
  return myCurve->IsPeriodic();
}

//=======================================================
// Purpose :IsVPeriodic
//=======================================================
bool GeomFill_UniformSection::IsVPeriodic() const
{
  return true;
}

//=======================================================
// Purpose :NbIntervals
//=======================================================
// int GeomFill_UniformSection::NbIntervals(const GeomAbs_Shape S) const
int GeomFill_UniformSection::NbIntervals(const GeomAbs_Shape) const
{
  return 1;
}

//=======================================================
// Purpose :Intervals
//=======================================================
void GeomFill_UniformSection::Intervals(NCollection_Array1<double>& T,
                                        //					 const GeomAbs_Shape S) const
                                        const GeomAbs_Shape) const
{
  T(T.Lower()) = First;
  T(T.Upper()) = Last;
}

//=======================================================
// Purpose : SetInterval
//=======================================================
void GeomFill_UniformSection::SetInterval(const double, const double)
{
  // Ne fait Rien
}

//=======================================================
// Purpose : GetInterval
//=======================================================
void GeomFill_UniformSection::GetInterval(double& F, double& L) const
{
  F = First;
  L = Last;
}

//=======================================================
// Purpose : GetDomain
//=======================================================
void GeomFill_UniformSection::GetDomain(double& F, double& L) const
{
  F = First;
  L = Last;
}

//=======================================================
// Purpose : GetTolerance
//=======================================================
void GeomFill_UniformSection::GetTolerance(const double BoundTol,
                                           const double SurfTol,
                                           //					    const double AngleTol,
                                           const double,
                                           NCollection_Array1<double>& Tol3d) const
{
  Tol3d.Init(SurfTol);
  if (BoundTol < SurfTol)
  {
    Tol3d(Tol3d.Lower()) = BoundTol;
    Tol3d(Tol3d.Upper()) = BoundTol;
  }
}

//=======================================================
// Purpose :
//=======================================================
gp_Pnt GeomFill_UniformSection::BarycentreOfSurf() const
{
  double U = mySection->FirstParameter(), Delta;
  gp_Pnt P, Bary;

  Delta = (myCurve->LastParameter() - U) / 20;
  Bary.SetCoord(0., 0., 0.);
  for (int ii = 0; ii <= 20; ii++, U += Delta)
  {
    P = myCurve->Value(U);
    Bary.ChangeCoord() += P.XYZ();
  }
  Bary.ChangeCoord() /= 21.;

  return Bary;
}

double GeomFill_UniformSection::MaximalSection() const
{
  GeomAdaptor_Curve AC(mySection);
  return GCPnts_AbscissaPoint::Length(AC);
}

void GeomFill_UniformSection::GetMinimalWeight(NCollection_Array1<double>& Weights) const
{
  Weights = myCurve->WeightsArray();
}

bool GeomFill_UniformSection::IsConstant(double& Error) const
{
  Error = 0.;
  return true;
}

occ::handle<Geom_Curve> GeomFill_UniformSection::ConstantSection() const
{
  occ::handle<Geom_Curve> C;
  C = occ::down_cast<Geom_Curve>(mySection->Copy());
  return C;
}
