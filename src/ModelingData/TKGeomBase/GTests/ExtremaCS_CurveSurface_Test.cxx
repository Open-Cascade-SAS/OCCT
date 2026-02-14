// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <ExtremaCS_CurveSurface.hxx>

#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for ExtremaCS_CurveSurface tests.
class ExtremaCS_CurveSurfaceTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-7;
};

//==================================================================================================
// Line-Plane tests (analytical dispatch)
//==================================================================================================

TEST_F(ExtremaCS_CurveSurfaceTest, LinePlane_Intersection)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(0, 0, -5), gp_Dir(0, 0, 1));
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  GeomAdaptor_Curve aCurveAdaptor(aLine);
  GeomAdaptor_Surface aSurfAdaptor(aPlane);

  ExtremaCS_CurveSurface anExtrema(aCurveAdaptor, aSurfAdaptor);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].SquareDistance, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[0].ParameterOnCurve, 5.0, THE_TOL);
}

TEST_F(ExtremaCS_CurveSurfaceTest, LinePlane_Parallel)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(0, 0, 5), gp_Dir(1, 0, 0));
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  GeomAdaptor_Curve aCurveAdaptor(aLine);
  GeomAdaptor_Surface aSurfAdaptor(aPlane);

  ExtremaCS_CurveSurface anExtrema(aCurveAdaptor, aSurfAdaptor);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 25.0, THE_TOL);
}

//==================================================================================================
// Line-Sphere tests (analytical dispatch)
//==================================================================================================

TEST_F(ExtremaCS_CurveSurfaceTest, LineSphere_ThroughCenter)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(-10, 0, 0), gp_Dir(1, 0, 0));
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(
    gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  GeomAdaptor_Curve aCurveAdaptor(aLine);
  GeomAdaptor_Surface aSurfAdaptor(aSphere);

  ExtremaCS_CurveSurface anExtrema(aCurveAdaptor, aSurfAdaptor);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  // Line through sphere center: at least one extremum found (may be 1 or 2)
  ASSERT_GE(aResult.NbExt(), 1);
  // Minimum distance should be 0 (line passes through sphere)
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaCS_CurveSurfaceTest, LineSphere_TangentLine)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(-10, 5, 0), gp_Dir(1, 0, 0));
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(
    gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  GeomAdaptor_Curve aCurveAdaptor(aLine);
  GeomAdaptor_Surface aSurfAdaptor(aSphere);

  ExtremaCS_CurveSurface anExtrema(aCurveAdaptor, aSurfAdaptor);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // Tangent line: min distance = 0
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaCS_CurveSurfaceTest, LineSphere_External)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(-10, 8, 0), gp_Dir(1, 0, 0));
  Handle(Geom_SphericalSurface) aSphere = new Geom_SphericalSurface(
    gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  GeomAdaptor_Curve aCurveAdaptor(aLine);
  GeomAdaptor_Surface aSurfAdaptor(aSphere);

  ExtremaCS_CurveSurface anExtrema(aCurveAdaptor, aSurfAdaptor);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // Line at Y=8, sphere radius 5 => min distance = 8-5 = 3
  EXPECT_NEAR(aResult.MinSquareDistance(), 9.0, THE_TOL);
}

//==================================================================================================
// Circle-Plane tests (analytical dispatch)
//==================================================================================================

TEST_F(ExtremaCS_CurveSurfaceTest, CirclePlane_Parallel)
{
  gp_Ax2 aCircleAx(gp_Pnt(0, 0, 5), gp_Dir(0, 0, 1));
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCircleAx, 3.0);
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  GeomAdaptor_Curve aCurveAdaptor(aCircle);
  GeomAdaptor_Surface aSurfAdaptor(aPlane);

  ExtremaCS_CurveSurface anExtrema(aCurveAdaptor, aSurfAdaptor);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 25.0, THE_TOL);
}

TEST_F(ExtremaCS_CurveSurfaceTest, CirclePlane_Tilted)
{
  gp_Dir aTiltedNormal(1, 0, 1); // gp_Dir auto-normalizes
  gp_Ax2 aCircleAx(gp_Pnt(0, 0, 0), aTiltedNormal);
  Handle(Geom_Circle) aCircle = new Geom_Circle(aCircleAx, 2.0);
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  GeomAdaptor_Curve aCurveAdaptor(aCircle);
  GeomAdaptor_Surface aSurfAdaptor(aPlane);

  ExtremaCS_CurveSurface anExtrema(aCurveAdaptor, aSurfAdaptor);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

//==================================================================================================
// Ellipse-Plane tests (analytical dispatch)
//==================================================================================================

TEST_F(ExtremaCS_CurveSurfaceTest, EllipsePlane_Tilted)
{
  gp_Dir aTiltedNormal(1, 0, 1); // gp_Dir auto-normalizes
  gp_Ax2 anEllipseAx(gp_Pnt(0, 0, 0), aTiltedNormal);
  Handle(Geom_Ellipse) anEllipse = new Geom_Ellipse(anEllipseAx, 4.0, 2.0);
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  GeomAdaptor_Curve aCurveAdaptor(anEllipse);
  GeomAdaptor_Surface aSurfAdaptor(aPlane);

  ExtremaCS_CurveSurface anExtrema(aCurveAdaptor, aSurfAdaptor);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::MinMax);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
  // Minimum should be 0 (ellipse passes through plane)
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

//==================================================================================================
// Line-Cylinder tests (analytical dispatch)
//==================================================================================================

TEST_F(ExtremaCS_CurveSurfaceTest, LineCylinder_Parallel)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(8, 0, 0), gp_Dir(0, 0, 1));
  Handle(Geom_CylindricalSurface) aCylinder = new Geom_CylindricalSurface(
    gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  GeomAdaptor_Curve aCurveAdaptor(aLine);
  GeomAdaptor_Surface aSurfAdaptor(aCylinder);

  ExtremaCS_CurveSurface anExtrema(aCurveAdaptor, aSurfAdaptor);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone() || aResult.IsInfinite());
  // Line parallel to cylinder axis at distance 8, cylinder radius 5
  // Min distance should be 8-5 = 3
  if (aResult.IsInfinite())
  {
    EXPECT_NEAR(aResult.InfiniteSquareDistance, 9.0, THE_TOL);
  }
  else
  {
    EXPECT_NEAR(aResult.MinSquareDistance(), 9.0, THE_TOL);
  }
}

//==================================================================================================
// Domain constraint tests
//==================================================================================================

TEST_F(ExtremaCS_CurveSurfaceTest, Domain_LinePlane)
{
  Handle(Geom_Line) aLine = new Geom_Line(gp_Pnt(0, 0, -5), gp_Dir(0, 0, 1));
  Handle(Geom_Plane) aPlane = new Geom_Plane(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

  GeomAdaptor_Curve aCurveAdaptor(aLine, -10.0, 10.0);
  GeomAdaptor_Surface aSurfAdaptor(aPlane, -100.0, 100.0, -100.0, 100.0);

  ExtremaCS::Domain3D aDomain;
  aDomain.Curve = {0.0, 10.0};
  aDomain.Surface = {-10.0, 10.0, -10.0, 10.0};

  ExtremaCS_CurveSurface anExtrema(aCurveAdaptor, aSurfAdaptor, aDomain);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].SquareDistance, 0.0, THE_TOL);
}
