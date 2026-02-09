// Created on: 1998-08-17
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomConvert.hxx>
#include <GeomFill_EvolvedSection.hxx>
#include <gp_Pnt.hxx>
#include <Law_Function.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

#include <cstdio>
IMPLEMENT_STANDARD_RTTIEXT(GeomFill_EvolvedSection, GeomFill_SectionLaw)

GeomFill_EvolvedSection::GeomFill_EvolvedSection(const occ::handle<Geom_Curve>&   C,
                                                 const occ::handle<Law_Function>& L)
{
  L->Bounds(First, Last);
  mySection = occ::down_cast<Geom_Curve>(C->Copy());
  myLaw     = L->Trim(First, Last, 1.e-20);
  TLaw      = myLaw;
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
bool GeomFill_EvolvedSection::D0(const double                U,
                                 NCollection_Array1<gp_Pnt>& Poles,
                                 NCollection_Array1<double>& Weights)
{
  double val;
  int    ii, L = Poles.Length();
  val = TLaw->Value(U);
  Poles = myCurve->Poles();
  for (ii = 1; ii <= L; ii++)
  {
    Poles(ii).ChangeCoord() *= val;
  }
  Weights = myCurve->WeightsArray();

  return true;
}

//=======================================================
// Purpose :D1
//=======================================================
bool GeomFill_EvolvedSection::D1(const double                U,
                                 NCollection_Array1<gp_Pnt>& Poles,
                                 NCollection_Array1<gp_Vec>& DPoles,
                                 NCollection_Array1<double>& Weights,
                                 NCollection_Array1<double>& DWeights)
{
  double val, dval;
  int    ii, L = Poles.Length();
  TLaw->D1(U, val, dval);

  Poles = myCurve->Poles();
  Weights = myCurve->WeightsArray();
  for (ii = 1; ii <= L; ii++)
  {
    DPoles(ii).SetXYZ(Poles(ii).XYZ());
    DPoles(ii) *= dval;
    Poles(ii).ChangeCoord() *= val;
  }
  DWeights.Init(0);

  return true;
}

//=======================================================
// Purpose :D2
//=======================================================
bool GeomFill_EvolvedSection::D2(const double                U,
                                 NCollection_Array1<gp_Pnt>& Poles,
                                 NCollection_Array1<gp_Vec>& DPoles,
                                 NCollection_Array1<gp_Vec>& D2Poles,
                                 NCollection_Array1<double>& Weights,
                                 NCollection_Array1<double>& DWeights,
                                 NCollection_Array1<double>& D2Weights)
{
  double val, dval, d2val;
  int    ii, L = Poles.Length();
  TLaw->D2(U, val, dval, d2val);
  Poles = myCurve->Poles();
  Weights = myCurve->WeightsArray();

  for (ii = 1; ii <= L; ii++)
  {
    DPoles(ii).SetXYZ(Poles(ii).XYZ());
    D2Poles(ii) = DPoles(ii);
    D2Poles(ii) *= d2val;
    DPoles(ii) *= dval;
    Poles(ii).ChangeCoord() *= val;
  }

  DWeights.Init(0);
  D2Weights.Init(0);

  return true;
}

//=======================================================
// Purpose :BSplineSurface()
//=======================================================
occ::handle<Geom_BSplineSurface> GeomFill_EvolvedSection::BSplineSurface() const
{
  /*  int ii, NbPoles = myCurve->NbPoles();
    NCollection_Array2<gp_Pnt> Poles( 1, NbPoles, 1, 2);
    NCollection_Array1<double> UKnots(1,myCurve->NbKnots()), VKnots(1,2);
    NCollection_Array1<int> UMults(1,myCurve->NbKnots()), VMults(1,2);

    for (ii=1; ii <= NbPoles; ii++) {
      Poles(ii, 1) =  Poles(ii, 2) = myCurve->Pole(ii);
    }

    myCurve->Knots(UKnots);
    VKnots(1) = First;
    VKnots(2) = Last;

    myCurve->Multiplicities(UMults);
    VMults.Init(2);

    occ::handle<Geom_BSplineSurface> BS =
      new (Geom_BSplineSurface) ( Poles,
                     UKnots, VKnots,
                     UMults, VMults,
                     myCurve->Degree(), 1,
                     myCurve->IsPeriodic());*/
  occ::handle<Geom_BSplineSurface> BS;
  BS.Nullify();
  return BS;
}

//=======================================================
// Purpose :SectionShape
//=======================================================
void GeomFill_EvolvedSection::SectionShape(int& NbPoles, int& NbKnots, int& Degree) const
{
  NbPoles = myCurve->NbPoles();
  NbKnots = myCurve->NbKnots();
  Degree  = myCurve->Degree();
}

void GeomFill_EvolvedSection::Knots(NCollection_Array1<double>& TKnots) const
{
  TKnots = myCurve->Knots();
}

//=======================================================
// Purpose :Mults
//=======================================================
void GeomFill_EvolvedSection::Mults(NCollection_Array1<int>& TMults) const
{
  TMults = myCurve->Multiplicities();
}

//=======================================================
// Purpose :IsRational
//=======================================================
bool GeomFill_EvolvedSection::IsRational() const
{
  return myCurve->IsRational();
}

//=======================================================
// Purpose :IsUPeriodic
//=======================================================
bool GeomFill_EvolvedSection::IsUPeriodic() const
{
  return myCurve->IsPeriodic();
}

//=======================================================
// Purpose :IsVPeriodic
//=======================================================
bool GeomFill_EvolvedSection::IsVPeriodic() const
{
  return (std::abs(myLaw->Value(First) - myLaw->Value(Last)) < Precision::Confusion());
}

//=======================================================
// Purpose :NbIntervals
//=======================================================
int GeomFill_EvolvedSection::NbIntervals(const GeomAbs_Shape S) const
{
  return myLaw->NbIntervals(S);
}

//=======================================================
// Purpose :Intervals
//=======================================================
void GeomFill_EvolvedSection::Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
{
  myLaw->Intervals(T, S);
}

//=======================================================
// Purpose : SetInterval
//=======================================================
void GeomFill_EvolvedSection::SetInterval(const double F, const double L)
{
  TLaw = myLaw->Trim(F, L, Precision::PConfusion());
}

//=======================================================
// Purpose : GetInterval
//=======================================================
void GeomFill_EvolvedSection::GetInterval(double& F, double& L) const
{
  TLaw->Bounds(F, L);
}

//=======================================================
// Purpose : GetDomain
//=======================================================
void GeomFill_EvolvedSection::GetDomain(double& F, double& L) const
{
  F = First;
  L = Last;
}

//=======================================================
// Purpose : GetTolerance
//=======================================================
void GeomFill_EvolvedSection::GetTolerance(const double BoundTol,
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
gp_Pnt GeomFill_EvolvedSection::BarycentreOfSurf() const
{
  double U = mySection->FirstParameter(), Delta, b;
  int    ii;
  gp_Pnt P, Bary;

  Delta = (myCurve->LastParameter() - U) / 20;
  Bary.SetCoord(0., 0., 0.);
  for (ii = 0; ii <= 20; ii++, U += Delta)
  {
    P = myCurve->Value(U);
    Bary.ChangeCoord() += P.XYZ();
  }

  U     = First;
  Delta = (Last - First) / 20;
  for (ii = 0, b = 0.0; ii <= 20; ii++, U += Delta)
  {
    b += myLaw->Value(U);
  }
  Bary.ChangeCoord() *= b / (21 * 21);
  return Bary;
}

double GeomFill_EvolvedSection::MaximalSection() const
{
  double            L, val, max, U, Delta;
  int               ii;
  GeomAdaptor_Curve AC(mySection);
  L = GCPnts_AbscissaPoint::Length(AC);

  Delta = (Last - First) / 20;
  for (ii = 0, max = 0.0, U = First; ii <= 20; ii++, U += Delta)
  {
    val = myLaw->Value(U);
    if (val > max)
      max = val;
  }
  return L * max;
}

void GeomFill_EvolvedSection::GetMinimalWeight(NCollection_Array1<double>& Weights) const
{
  Weights = myCurve->WeightsArray();
}

bool GeomFill_EvolvedSection::IsConstant(double& Error) const
{
  //  double isconst = false;
  bool isconst = false;
  Error        = 0.;
  return isconst;
}

occ::handle<Geom_Curve> GeomFill_EvolvedSection::ConstantSection() const
{
  double Err, scale;
  if (!IsConstant(Err))
    throw StdFail_NotDone("The Law is not Constant!");
  gp_Trsf T;
  gp_Pnt  P(0, 0, 0);
  scale = myLaw->Value(First) + myLaw->Value((First + Last) / 2) + myLaw->Value(Last);
  T.SetScale(P, scale / 3);

  occ::handle<Geom_Curve> C;
  C = occ::down_cast<Geom_Curve>(mySection->Copy());
  C->Transform(T);
  return C;
}
