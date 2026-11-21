// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Plane.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

//==================================================================================================

TEST(Adaptor3d_CurveOnSurfaceTest, D3UsesThirdDerivativeOfPlanarPCurve)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles.Init(gp_Pnt2d(0.0, 0.0));
  aPoles(4)                                     = gp_Pnt2d(1.0, 2.0);
  const occ::handle<Geom2d_BezierCurve> aPCurve = new Geom2d_BezierCurve(aPoles);
  const occ::handle<Geom_Plane>         aPlane  = new Geom_Plane(gp_Pln());
  const Adaptor3d_CurveOnSurface        aCurve(new Geom2dAdaptor_Curve(aPCurve),
                                        new GeomAdaptor_Surface(aPlane));

  const gp_Vec aD3 = aCurve.EvalD3(0.5).D3;
  EXPECT_DOUBLE_EQ(aD3.X(), 6.0);
  EXPECT_DOUBLE_EQ(aD3.Y(), 12.0);
  EXPECT_DOUBLE_EQ(aD3.Z(), 0.0);
}

//==================================================================================================

TEST(Adaptor3d_CurveOnSurfaceTest, D3CountsMixedSurfaceTermOnce)
{
  // Surface (u, v, u*v*v) and pcurve (t, t) give z = t*t*t.
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 3);
  for (int aU = 0; aU <= 1; ++aU)
  {
    for (int aV = 0; aV <= 2; ++aV)
    {
      aPoles(aU + 1, aV + 1) = gp_Pnt(aU, 0.5 * aV, aV == 2 ? aU : 0.0);
    }
  }
  const occ::handle<Geom_BezierSurface> aSurface = new Geom_BezierSurface(aPoles);
  NCollection_Array1<gp_Pnt2d>          aPCurvePoles(1, 2);
  aPCurvePoles(1)                               = gp_Pnt2d(0.0, 0.0);
  aPCurvePoles(2)                               = gp_Pnt2d(1.0, 1.0);
  const occ::handle<Geom2d_BezierCurve> aPCurve = new Geom2d_BezierCurve(aPCurvePoles);
  const Adaptor3d_CurveOnSurface        aCurve(new Geom2dAdaptor_Curve(aPCurve),
                                        new GeomAdaptor_Surface(aSurface));

  const gp_Vec aD3 = aCurve.EvalD3(0.4).D3;
  EXPECT_NEAR(aD3.Z(), 6.0, 1.e-12);
}

//==================================================================================================

TEST(Adaptor3d_CurveOnSurfaceTest, ReloadCurveRebuildsIntervalBounds)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles(1)                                     = gp_Pnt2d(0.0, 0.0);
  aPoles(2)                                     = gp_Pnt2d(1.0, 1.0);
  const occ::handle<Geom2d_BezierCurve> aPCurve = new Geom2d_BezierCurve(aPoles);
  const occ::handle<Geom_Plane>         aPlane  = new Geom_Plane(gp_Pln());
  Adaptor3d_CurveOnSurface              aCurve(new Geom2dAdaptor_Curve(aPCurve),
                                  new GeomAdaptor_Surface(aPlane));

  NCollection_Array1<double> anIntervals(1, 2);
  ASSERT_EQ(aCurve.NbIntervals(GeomAbs_C1), 1);
  aCurve.Intervals(anIntervals, GeomAbs_C1);
  EXPECT_DOUBLE_EQ(anIntervals(1), 0.0);
  EXPECT_DOUBLE_EQ(anIntervals(2), 1.0);

  aCurve.Load(new Geom2dAdaptor_Curve(aPCurve, 0.2, 0.8));
  ASSERT_EQ(aCurve.NbIntervals(GeomAbs_C1), 1);
  aCurve.Intervals(anIntervals, GeomAbs_C1);
  EXPECT_DOUBLE_EQ(anIntervals(1), 0.2);
  EXPECT_DOUBLE_EQ(anIntervals(2), 0.8);
}

//==================================================================================================

TEST(Adaptor3d_CurveOnSurfaceTest, ReloadSurfaceRebuildsEndpointEvaluation)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  for (int aU = 0; aU <= 1; ++aU)
  {
    for (int aV = 0; aV <= 1; ++aV)
    {
      aPoles(aU + 1, aV + 1) = gp_Pnt(aU, aV, 10.0);
    }
  }
  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;
  NCollection_Array1<int> aMults(1, 2);
  aMults.Init(2);
  const occ::handle<Geom_BSplineSurface> aBSpline =
    new Geom_BSplineSurface(aPoles, aKnots, aKnots, aMults, aMults, 1, 1);
  NCollection_Array1<gp_Pnt2d> aPCurvePoles(1, 2);
  aPCurvePoles(1)                               = gp_Pnt2d(0.2, 0.2);
  aPCurvePoles(2)                               = gp_Pnt2d(0.8, 0.8);
  const occ::handle<Geom2d_BezierCurve> aPCurve = new Geom2d_BezierCurve(aPCurvePoles);
  Adaptor3d_CurveOnSurface              aCurve(new Geom2dAdaptor_Curve(aPCurve),
                                  new GeomAdaptor_Surface(aBSpline));
  const occ::handle<Adaptor3d_Curve>    aCopy = aCurve.ShallowCopy();
  ASSERT_FALSE(aCopy.IsNull());
  EXPECT_DOUBLE_EQ(aCurve.EvalD1(0.0).Point.Z(), 10.0);
  EXPECT_DOUBLE_EQ(aCurve.EvalD1(1.0).Point.Z(), 10.0);

  const occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln());
  aCurve.Load(new GeomAdaptor_Surface(aPlane));
  EXPECT_DOUBLE_EQ(aCurve.EvalD1(0.0).Point.Z(), 0.0);
  EXPECT_DOUBLE_EQ(aCurve.EvalD1(1.0).Point.Z(), 0.0);
  EXPECT_DOUBLE_EQ(aCopy->EvalD1(0.0).Point.Z(), 10.0);
}
