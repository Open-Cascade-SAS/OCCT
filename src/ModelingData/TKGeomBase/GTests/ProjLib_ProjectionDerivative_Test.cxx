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

#include <Geom2d_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Line.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <ProjLib_CompProjectedCurve.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

#include <gtest/gtest.h>

#include <cmath>

//==================================================================================================

TEST(ProjLib_ProjectionDerivativeTest, RegularProjectionNearSingularSurfaceDerivatives)
{
  // S(u,v)=(u*u,u+v,0), C(t)=(t,0,0): the pcurve is (sqrt(t),-sqrt(t)).
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.ChangeAt(0)                            = gp_Pnt(0.0, 0.0, 0.0);
  aPoles.ChangeAt(1)                            = gp_Pnt(0.0, 0.5, 0.0);
  aPoles.ChangeAt(2)                            = gp_Pnt(1.0, 1.0, 0.0);
  const occ::handle<Geom_BezierCurve>    aBasis = new Geom_BezierCurve(aPoles);
  const occ::handle<GeomAdaptor_Surface> aSurface =
    new GeomAdaptor_Surface(new Geom_SurfaceOfLinearExtrusion(aBasis, gp_Dir(0.0, 1.0, 0.0)),
                            0.0,
                            1.0,
                            -2.0,
                            2.0);
  constexpr double aParameterTolerance = 1.e-13;
  // At u=1e-6, D2 amplifies the relative parameter error by a factor of eight.
  constexpr double aRelativeDerivativeTolerance = 1.e-6;
  for (const double aFirst : {1.e-12, 0.25})
  {
    SCOPED_TRACE(aFirst);
    const occ::handle<GeomAdaptor_Curve> aCurve =
      new GeomAdaptor_Curve(new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0)),
                            aFirst,
                            1.0);
    ProjLib_CompProjectedCurve aProjection(aSurface,
                                           aCurve,
                                           aParameterTolerance,
                                           aParameterTolerance);
    ASSERT_EQ(aProjection.NbCurves(), 1);
    double aStart, anEnd;
    aProjection.Bounds(1, aStart, anEnd);
    ASSERT_TRUE(std::isfinite(aStart));
    ASSERT_TRUE(std::isfinite(anEnd));
    ASSERT_LE(aStart, aFirst);
    ASSERT_GE(anEnd, aFirst);

    const double              anExpectedU  = std::sqrt(aFirst);
    const double              anExpectedD1 = 0.5 / anExpectedU;
    const double              anExpectedD2 = -0.25 / (aFirst * anExpectedU);
    const Geom2d_Curve::ResD1 aD1          = aProjection.EvalD1(aFirst);
    const Geom2d_Curve::ResD2 aD2          = aProjection.EvalD2(aFirst);
    for (const gp_Pnt2d& aPoint : {aD1.Point, aD2.Point})
    {
      EXPECT_NEAR(aPoint.X(), anExpectedU, aParameterTolerance);
      EXPECT_NEAR(aPoint.Y(), -anExpectedU, aParameterTolerance);
      EXPECT_LE(aSurface->EvalD0(aPoint.X(), aPoint.Y()).Distance(aCurve->EvalD0(aFirst)),
                Precision::Confusion());
    }
    // The inverse point is resolved first; the remaining relative error is derivative arithmetic.
    for (const gp_Vec2d& aDerivative : {aD1.D1, aD2.D1})
    {
      EXPECT_NEAR(aDerivative.X(), anExpectedD1, anExpectedD1 * aRelativeDerivativeTolerance);
      EXPECT_NEAR(aDerivative.Y(), -anExpectedD1, anExpectedD1 * aRelativeDerivativeTolerance);
    }
    EXPECT_NEAR(aD2.D2.X(), anExpectedD2, std::abs(anExpectedD2) * aRelativeDerivativeTolerance);
    EXPECT_NEAR(aD2.D2.Y(), -anExpectedD2, std::abs(anExpectedD2) * aRelativeDerivativeTolerance);
  }
}

//==================================================================================================

TEST(ProjLib_ProjectionDerivativeTest, ProjectionDerivativesIncludeNormalOffsetCurvature)
{
  // S(u,v)=(u,u*u,v), C(t)=(0,t,0): the positive branch has u=sqrt(t-0.5).
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.ChangeAt(0)                            = gp_Pnt(0.0, 0.0, 0.0);
  aPoles.ChangeAt(1)                            = gp_Pnt(0.5, 0.0, 0.0);
  aPoles.ChangeAt(2)                            = gp_Pnt(1.0, 1.0, 0.0);
  const occ::handle<Geom_BezierCurve>    aBasis = new Geom_BezierCurve(aPoles);
  const occ::handle<GeomAdaptor_Surface> aSurface =
    new GeomAdaptor_Surface(new Geom_SurfaceOfLinearExtrusion(aBasis, gp_Dir(0.0, 0.0, 1.0)),
                            0.0,
                            1.0,
                            -1.0,
                            1.0);
  const occ::handle<GeomAdaptor_Curve> aCurve =
    new GeomAdaptor_Curve(new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0)), 0.75, 1.0);
  ProjLib_CompProjectedCurve aProjection(aSurface, aCurve, 1.e-13, 1.e-13);
  ASSERT_EQ(aProjection.NbCurves(), 1);
  const Geom2d_Curve::ResD1 aD1 = aProjection.EvalD1(0.75);
  const Geom2d_Curve::ResD2 aD2 = aProjection.EvalD2(0.75);
  EXPECT_NEAR(aD1.Point.X(), 0.5, Precision::PConfusion());
  EXPECT_NEAR(aD1.Point.Y(), 0.0, Precision::PConfusion());
  EXPECT_NEAR(aD2.Point.X(), 0.5, Precision::PConfusion());
  EXPECT_NEAR(aD2.Point.Y(), 0.0, Precision::PConfusion());
  constexpr double aDerivativeTolerance = 1.e-6;
  EXPECT_NEAR(aD1.D1.X(), 1.0, aDerivativeTolerance);
  EXPECT_NEAR(aD1.D1.Y(), 0.0, aDerivativeTolerance);
  EXPECT_NEAR(aD2.D1.X(), 1.0, aDerivativeTolerance);
  EXPECT_NEAR(aD2.D1.Y(), 0.0, aDerivativeTolerance);
  EXPECT_NEAR(aD2.D2.X(), -2.0, 2.0 * aDerivativeTolerance);
  EXPECT_NEAR(aD2.D2.Y(), 0.0, aDerivativeTolerance);
}
