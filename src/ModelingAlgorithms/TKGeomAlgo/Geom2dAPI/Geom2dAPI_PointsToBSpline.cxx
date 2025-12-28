// Created on: 1994-03-23
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
#include <AppParCurves_ConstraintCouple.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <BSplCLib.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dAPI_PointsToBSpline.hxx>
#include <math_Vector.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>

//=================================================================================================

Geom2dAPI_PointsToBSpline::Geom2dAPI_PointsToBSpline()
{
  myIsDone = false;
}

//=================================================================================================

Geom2dAPI_PointsToBSpline::Geom2dAPI_PointsToBSpline(const NCollection_Array1<gp_Pnt2d>& Points,
                                                     const int      DegMin,
                                                     const int      DegMax,
                                                     const GeomAbs_Shape         Continuity,
                                                     const double         Tol2D)
{
  Init(Points, DegMin, DegMax, Continuity, Tol2D);
}

//=================================================================================================

Geom2dAPI_PointsToBSpline::Geom2dAPI_PointsToBSpline(const NCollection_Array1<double>& YValues,
                                                     const double         X0,
                                                     const double         DX,
                                                     const int      DegMin,
                                                     const int      DegMax,
                                                     const GeomAbs_Shape         Continuity,
                                                     const double         Tol2D)
{
  Init(YValues, X0, DX, DegMin, DegMax, Continuity, Tol2D);
}

//=================================================================================================

Geom2dAPI_PointsToBSpline::Geom2dAPI_PointsToBSpline(const NCollection_Array1<gp_Pnt2d>&      Points,
                                                     const Approx_ParametrizationType ParType,
                                                     const int           DegMin,
                                                     const int           DegMax,
                                                     const GeomAbs_Shape              Continuity,
                                                     const double              Tol2D)
{
  myIsDone = false;
  Init(Points, ParType, DegMin, DegMax, Continuity, Tol2D);
}

//=================================================================================================

Geom2dAPI_PointsToBSpline::Geom2dAPI_PointsToBSpline(const NCollection_Array1<gp_Pnt2d>& Points,
                                                     const NCollection_Array1<double>& Params,
                                                     const int      DegMin,
                                                     const int      DegMax,
                                                     const GeomAbs_Shape         Continuity,
                                                     const double         Tol2D)
{
  myIsDone = false;
  Init(Points, Params, DegMin, DegMax, Continuity, Tol2D);
}

//=================================================================================================

Geom2dAPI_PointsToBSpline::Geom2dAPI_PointsToBSpline(const NCollection_Array1<gp_Pnt2d>& Points,
                                                     const double         W1,
                                                     const double         W2,
                                                     const double         W3,
                                                     const int      DegMax,
                                                     const GeomAbs_Shape         Continuity,
                                                     const double         Tol2D)
{
  myIsDone = false;
  Init(Points, W1, W2, W3, DegMax, Continuity, Tol2D);
}

//=================================================================================================

void Geom2dAPI_PointsToBSpline::Init(const NCollection_Array1<gp_Pnt2d>&      Points,
                                     const Approx_ParametrizationType ParType,
                                     const int           DegMin,
                                     const int           DegMax,
                                     const GeomAbs_Shape              Continuity,
                                     const double              Tol2D)
{
  double Tol3D = 0.; // dummy argument for BSplineCompute.

  int nbit       = 2;
  bool UseSquares = false;
  if (Tol2D <= 1.e-3)
    UseSquares = true;

  AppDef_BSplineCompute
    TheComputer(DegMin, DegMax, Tol3D, Tol2D, nbit, true, ParType, UseSquares);

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

  NCollection_Array1<gp_Pnt2d> Poles(1, TheCurve.NbPoles());

  TheCurve.Curve(1, Poles);

  myCurve =
    new Geom2d_BSplineCurve(Poles, TheCurve.Knots(), TheCurve.Multiplicities(), TheCurve.Degree());
  myIsDone = true;
}

//=================================================================================================

void Geom2dAPI_PointsToBSpline::Init(const NCollection_Array1<double>& YValues,
                                     const double         X0,
                                     const double         DX,
                                     const int      DegMin,
                                     const int      DegMax,
                                     const GeomAbs_Shape         Continuity,
                                     const double         Tol2D)
{
  // first approximate the Y values (with dummy 0 as X values)

  double        Tol3D = 0.; // dummy argument for BSplineCompute.
  NCollection_Array1<gp_Pnt2d> Points(YValues.Lower(), YValues.Upper());
  math_Vector          Param(YValues.Lower(), YValues.Upper());
  double        length = DX * (YValues.Upper() - YValues.Lower());
  int     i;

  for (i = YValues.Lower(); i <= YValues.Upper(); i++)
  {
    Param(i) = (X0 + (i - 1) * DX) / (X0 + length);
    Points(i).SetCoord(0.0, YValues(i));
  }

  AppDef_BSplineCompute
    TheComputer(Param, DegMin, DegMax, Tol3D, Tol2D, 0, true, true);

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

  const AppParCurves_MultiBSpCurve& TheCurve = TheComputer.Value();

  int        Degree = TheCurve.Degree();
  NCollection_Array1<gp_Pnt2d>    Poles(1, TheCurve.NbPoles());
  int        nk = TheCurve.Knots().Length();
  NCollection_Array1<double>    Knots(1, nk);
  NCollection_Array1<int> Mults(1, nk);

  TheCurve.Curve(1, Poles);

  // compute X values for the poles
  NCollection_Array1<double> XPoles(1, Poles.Upper());

  // start with a line
  NCollection_Array1<double>    TempPoles(1, 2);
  NCollection_Array1<double>    TempKnots(1, 2);
  NCollection_Array1<int> TempMults(1, 2);
  TempMults.Init(2);
  TempPoles(1) = X0;
  TempPoles(2) = X0 + length;
  TempKnots(1) = 0.;
  TempKnots(2) = 1.;

  // increase the Degree
  NCollection_Array1<double>    NewTempPoles(1, Degree + 1);
  NCollection_Array1<double>    NewTempKnots(1, 2);
  NCollection_Array1<int> NewTempMults(1, 2);
  BSplCLib::IncreaseDegree(1,
                           Degree,
                           false,
                           1,
                           TempPoles,
                           TempKnots,
                           TempMults,
                           NewTempPoles,
                           NewTempKnots,
                           NewTempMults);

  // insert the Knots
  BSplCLib::InsertKnots(Degree,
                        false,
                        1,
                        NewTempPoles,
                        NewTempKnots,
                        NewTempMults,
                        TheCurve.Knots(),
                        &TheCurve.Multiplicities(),
                        XPoles,
                        Knots,
                        Mults,
                        Epsilon(1));

  // scale the knots
  for (i = 1; i <= nk; i++)
  {
    Knots(i) = X0 + length * Knots(i);
  }

  // set the Poles
  for (i = 1; i <= Poles.Upper(); i++)
  {
    Poles(i).SetX(XPoles(i));
  }

  myCurve  = new Geom2d_BSplineCurve(Poles, Knots, Mults, Degree);
  myIsDone = true;
}

//=================================================================================================

void Geom2dAPI_PointsToBSpline::Init(const NCollection_Array1<gp_Pnt2d>& Points,
                                     const int      DegMin,
                                     const int      DegMax,
                                     const GeomAbs_Shape         Continuity,
                                     const double         Tol2D)
{
  myIsDone = false;
  Init(Points, Approx_ChordLength, DegMin, DegMax, Continuity, Tol2D);
}

//=================================================================================================

void Geom2dAPI_PointsToBSpline::Init(const NCollection_Array1<gp_Pnt2d>& Points,
                                     const NCollection_Array1<double>& Params,
                                     const int      DegMin,
                                     const int      DegMax,
                                     const GeomAbs_Shape         Continuity,
                                     const double         Tol2D)
{
  if (Params.Length() != Points.Length())
    throw Standard_OutOfRange("Geom2dAPI_PointsToBSpline::Init() - invalid input");

  double    Tol3D = 0.; // dummy argument for BSplineCompute.
  int Nbp   = Params.Length();
  math_Vector      theParams(1, Nbp);
  theParams(1)   = 0.;
  theParams(Nbp) = 1.;

  double Uf = Params(Params.Lower());
  double Ul = Params(Params.Upper()) - Uf;
  for (int i = 2; i < Nbp; i++)
  {
    theParams(i) = (Params(i) - Uf) / Ul;
  }

  AppDef_BSplineCompute TheComputer(DegMin,
                                    DegMax,
                                    Tol3D,
                                    Tol2D,
                                    0,
                                    true,
                                    Approx_IsoParametric,
                                    true);

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

  NCollection_Array1<gp_Pnt2d> Poles(1, TheCurve.NbPoles());

  TheCurve.Curve(1, Poles);

  myCurve =
    new Geom2d_BSplineCurve(Poles, TheCurve.Knots(), TheCurve.Multiplicities(), TheCurve.Degree());
  myIsDone = true;
}

//=================================================================================================

void Geom2dAPI_PointsToBSpline::Init(const NCollection_Array1<gp_Pnt2d>& Points,
                                     const double         W1,
                                     const double         W2,
                                     const double         W3,
                                     const int      DegMax,
                                     const GeomAbs_Shape         Continuity,
                                     const double         Tol2D)
{
  int NbPoint = Points.Length(), i;

  int nbit = 2;
  if (Tol2D <= 1.e-3)
    nbit = 0;

  // Variational algo

  AppDef_MultiLine multL(NbPoint);
  for (i = 1; i <= NbPoint; ++i)
  {
    AppDef_MultiPointConstraint mpc(0, 1);
    mpc.SetPoint2d(1, Points.Value(Points.Lower() + i - 1));
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
  int theMaxSegments = 1000;
  bool theWithMinMax  = false;
  //===================================

  Variation.SetMaxDegree(DegMax);
  Variation.SetContinuity(Continuity);
  Variation.SetMaxSegment(theMaxSegments);

  Variation.SetTolerance(Tol2D);
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

  NCollection_Array1<gp_Pnt2d> Poles(1, TheCurve.NbPoles());

  TheCurve.Curve(1, Poles);

  myCurve =
    new Geom2d_BSplineCurve(Poles, TheCurve.Knots(), TheCurve.Multiplicities(), TheCurve.Degree());

  myIsDone = true;
}

//=======================================================================
// function : occ::handle<Geom2d_BSplineCurve>&
// purpose  :
//=======================================================================

const occ::handle<Geom2d_BSplineCurve>& Geom2dAPI_PointsToBSpline::Curve() const
{
  if (!myIsDone)
    throw StdFail_NotDone(" ");
  return myCurve;
}

//=================================================================================================

Geom2dAPI_PointsToBSpline::operator occ::handle<Geom2d_BSplineCurve>() const
{
  return myCurve;
}

//=================================================================================================

bool Geom2dAPI_PointsToBSpline::IsDone() const
{
  return myIsDone;
}
