// Created on: 1994-03-21
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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

#include <AppDef_BSplineCompute.hxx>
#include <AppDef_MultiPointConstraint.hxx>
#include <AppDef_Variational.hxx>
#include <AppParCurves_Constraint.hxx>
#include <AppParCurves_ConstraintCouple.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <BSplCLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <math_Vector.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

//=================================================================================================

GeomAPI_PointsToBSpline::GeomAPI_PointsToBSpline()
{
  myIsDone = false;
}

//=================================================================================================

GeomAPI_PointsToBSpline::GeomAPI_PointsToBSpline(const NCollection_Array1<gp_Pnt>& Points,
                                                 const int                         DegMin,
                                                 const int                         DegMax,
                                                 const GeomAbs_Shape               Continuity,
                                                 const double                      Tol3D)
{
  myIsDone = false;
  Init(Points, DegMin, DegMax, Continuity, Tol3D);
}

//=================================================================================================

GeomAPI_PointsToBSpline::GeomAPI_PointsToBSpline(const NCollection_Array1<gp_Pnt>& Points,
                                                 const Approx_ParametrizationType  ParType,
                                                 const int                         DegMin,
                                                 const int                         DegMax,
                                                 const GeomAbs_Shape               Continuity,
                                                 const double                      Tol3D)
{
  myIsDone = false;
  Init(Points, ParType, DegMin, DegMax, Continuity, Tol3D);
}

//=================================================================================================

GeomAPI_PointsToBSpline::GeomAPI_PointsToBSpline(const NCollection_Array1<gp_Pnt>& Points,
                                                 const NCollection_Array1<double>& Params,
                                                 const int                         DegMin,
                                                 const int                         DegMax,
                                                 const GeomAbs_Shape               Continuity,
                                                 const double                      Tol3D)
{
  myIsDone = false;
  Init(Points, Params, DegMin, DegMax, Continuity, Tol3D);
}

//=================================================================================================

GeomAPI_PointsToBSpline::GeomAPI_PointsToBSpline(const NCollection_Array1<gp_Pnt>& Points,
                                                 const double                      W1,
                                                 const double                      W2,
                                                 const double                      W3,
                                                 const int                         DegMax,
                                                 const GeomAbs_Shape               Continuity,
                                                 const double                      Tol3D)
{
  myIsDone = false;
  Init(Points, W1, W2, W3, DegMax, Continuity, Tol3D);
}

//=================================================================================================

void GeomAPI_PointsToBSpline::Init(const NCollection_Array1<gp_Pnt>& Points,
                                   const int                         DegMin,
                                   const int                         DegMax,
                                   const GeomAbs_Shape               Continuity,
                                   const double                      Tol3D)
{
  myIsDone = false;
  Init(Points, Approx_ChordLength, DegMin, DegMax, Continuity, Tol3D);
}

//=================================================================================================

void GeomAPI_PointsToBSpline::Init(const NCollection_Array1<gp_Pnt>& Points,
                                   const Approx_ParametrizationType  ParType,
                                   const int                         DegMin,
                                   const int                         DegMax,
                                   const GeomAbs_Shape               Continuity,
                                   const double                      Tol3D)
{
  double Tol2D = 0.; // dummy argument for BSplineCompute.

  int  nbit       = 2;
  bool UseSquares = false;
  if (Tol3D <= 1.e-3)
    UseSquares = true;

  AppDef_BSplineCompute TheComputer(DegMin, DegMax, Tol3D, Tol2D, nbit, true, ParType, UseSquares);

  switch (Continuity)
  {
    case GeomAbs_C0:
      TheComputer.SetContinuity(0);
      break;

    case GeomAbs_G1:
    case GeomAbs_C1:
      TheComputer.SetContinuity(1);
      break;

    case GeomAbs_G2:
    case GeomAbs_C2:
      TheComputer.SetContinuity(2);
      break;

    default:
      TheComputer.SetContinuity(3);
  }

  TheComputer.Perform(Points);

  AppParCurves_MultiBSpCurve TheCurve = TheComputer.Value();

  NCollection_Array1<gp_Pnt> Poles(1, TheCurve.NbPoles());

  TheCurve.Curve(1, Poles);

  myCurve =
    new Geom_BSplineCurve(Poles, TheCurve.Knots(), TheCurve.Multiplicities(), TheCurve.Degree());
  myIsDone = true;
}

//=================================================================================================

void GeomAPI_PointsToBSpline::Init(const NCollection_Array1<gp_Pnt>& Points,
                                   const NCollection_Array1<double>& Params,
                                   const int                         DegMin,
                                   const int                         DegMax,
                                   const GeomAbs_Shape               Continuity,
                                   const double                      Tol3D)
{
  if (Params.Length() != Points.Length())
    throw Standard_OutOfRange("GeomAPI_PointsToBSpline::Init() - invalid input");

  double      Tol2D = 0.; // dummy argument for BSplineCompute.
  int         Nbp   = Params.Length();
  math_Vector theParams(1, Nbp);
  theParams(1)   = 0.;
  theParams(Nbp) = 1.;

  double Uf = Params(Params.Lower());
  double Ul = Params(Params.Upper()) - Uf;
  for (int i = 2; i < Nbp; i++)
  {
    theParams(i) = (Params(i) - Uf) / Ul;
  }

  AppDef_BSplineCompute
    TheComputer(DegMin, DegMax, Tol3D, Tol2D, 0, true, Approx_IsoParametric, true);

  TheComputer.SetParameters(theParams);

  switch (Continuity)
  {
    case GeomAbs_C0:
      TheComputer.SetContinuity(0);
      break;

    case GeomAbs_G1:
    case GeomAbs_C1:
      TheComputer.SetContinuity(1);
      break;

    case GeomAbs_G2:
    case GeomAbs_C2:
      TheComputer.SetContinuity(2);
      break;

    default:
      TheComputer.SetContinuity(3);
  }

  TheComputer.Perform(Points);

  AppParCurves_MultiBSpCurve TheCurve = TheComputer.Value();

  NCollection_Array1<gp_Pnt> Poles(1, TheCurve.NbPoles());
  NCollection_Array1<double> Knots(TheCurve.Knots().Lower(), TheCurve.Knots().Upper());

  TheCurve.Curve(1, Poles);
  Knots = TheCurve.Knots();
  BSplCLib::Reparametrize(Params(Params.Lower()), Params(Params.Upper()), Knots);

  myCurve  = new Geom_BSplineCurve(Poles, Knots, TheCurve.Multiplicities(), TheCurve.Degree());
  myIsDone = true;
}

//=================================================================================================

void GeomAPI_PointsToBSpline::Init(const NCollection_Array1<gp_Pnt>& Points,
                                   const double                      W1,
                                   const double                      W2,
                                   const double                      W3,
                                   const int                         DegMax,
                                   const GeomAbs_Shape               Continuity,
                                   const double                      Tol3D)
{
  int NbPoint = Points.Length(), i;

  int nbit = 2;
  if (Tol3D <= 1.e-3)
    nbit = 0;

  // Variational algo

  AppDef_MultiLine multL(NbPoint);
  for (i = 1; i <= NbPoint; ++i)
  {
    AppDef_MultiPointConstraint mpc(1, 0);
    mpc.SetPoint(1, Points.Value(Points.Lower() + i - 1));
    multL.SetValue(i, mpc);
  }

  occ::handle<NCollection_HArray1<AppParCurves_ConstraintCouple>> TABofCC =
    new NCollection_HArray1<AppParCurves_ConstraintCouple>(1, NbPoint);
  AppParCurves_Constraint Constraint = AppParCurves_NoConstraint;

  for (i = 1; i <= NbPoint; ++i)
  {
    AppParCurves_ConstraintCouple ACC(i, Constraint);
    TABofCC->SetValue(i, ACC);
  }

  AppDef_Variational Variation(multL, 1, NbPoint, TABofCC);

  //===================================
  int  theMaxSegments = 1000;
  bool theWithMinMax  = false;
  //===================================

  Variation.SetMaxDegree(DegMax);
  Variation.SetContinuity(Continuity);
  Variation.SetMaxSegment(theMaxSegments);

  Variation.SetTolerance(Tol3D);
  Variation.SetWithMinMax(theWithMinMax);
  Variation.SetNbIterations(nbit);

  Variation.SetCriteriumWeight(W1, W2, W3);

  if (!Variation.IsCreated())
  {
    return;
  }

  if (Variation.IsOverConstrained())
  {
    return;
  }

  try
  {
    Variation.Approximate();
  }
  catch (Standard_Failure const&)
  {
    return;
  }

  if (!Variation.IsDone())
  {
    return;
  }

  AppParCurves_MultiBSpCurve TheCurve = Variation.Value();

  NCollection_Array1<gp_Pnt> Poles(1, TheCurve.NbPoles());

  TheCurve.Curve(1, Poles);

  myCurve =
    new Geom_BSplineCurve(Poles, TheCurve.Knots(), TheCurve.Multiplicities(), TheCurve.Degree());
  myIsDone = true;
}

//=======================================================================
// function : occ::handle<Geom_BSplineCurve>&
// purpose  :
//=======================================================================

const occ::handle<Geom_BSplineCurve>& GeomAPI_PointsToBSpline::Curve() const
{
  if (!myIsDone)
    throw StdFail_NotDone("GeomAPI_PointsToBSpline::Curve ");
  return myCurve;
}

//=================================================================================================

GeomAPI_PointsToBSpline::operator occ::handle<Geom_BSplineCurve>() const
{
  return myCurve;
}

//=================================================================================================

bool GeomAPI_PointsToBSpline::IsDone() const
{
  return myIsDone;
}
