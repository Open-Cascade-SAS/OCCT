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

#include <gtest/gtest.h>

#include <cmath>

#include <GeomAPI_ExtremaCurveSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>

namespace
{
static occ::handle<Geom_Circle> makeCircle(const gp_Pnt& theCenter,
                                           const gp_Dir& theNormal,
                                           const double  theRadius)
{
  return new Geom_Circle(gp_Ax2(theCenter, theNormal), theRadius);
}

static occ::handle<Geom_SphericalSurface> makeSphere(const double theRadius)
{
  return new Geom_SphericalSurface(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), theRadius);
}

static void expectCircleSphereExtrema(const gp_Pnt& theCenter,
                                      const gp_Dir& theNormal,
                                      const double  theCircleRadius,
                                      const double  theExpectedDistance,
                                      const bool    theIsParallel)
{
  const occ::handle<Geom_Circle> aCircle = makeCircle(theCenter, theNormal, theCircleRadius);
  const occ::handle<Geom_SphericalSurface> aSphere = makeSphere(10.0);

  double aCurveFirst = 0.0, aCurveLast = 0.0;
  double aSurfaceUFirst = 0.0, aSurfaceULast = 0.0;
  double aSurfaceVFirst = 0.0, aSurfaceVLast = 0.0;
  aCurveFirst = aCircle->FirstParameter();
  aCurveLast  = aCircle->LastParameter();
  aSphere->Bounds(aSurfaceUFirst, aSurfaceULast, aSurfaceVFirst, aSurfaceVLast);

  GeomAPI_ExtremaCurveSurface anExtrema(aCircle,
                                        aSphere,
                                        aCurveFirst,
                                        aCurveLast,
                                        aSurfaceUFirst,
                                        aSurfaceULast,
                                        aSurfaceVFirst,
                                        aSurfaceVLast);
  EXPECT_EQ(anExtrema.IsParallel(), theIsParallel);
  if (!theIsParallel)
  {
    ASSERT_GT(anExtrema.NbExtrema(), 0);
  }
  EXPECT_NEAR(anExtrema.LowerDistance(), theExpectedDistance, 1.e-7);
}
} // namespace

// Migrated from tests/lowalgos/extcs/circ_sph_parallel.  A concentric circle
// and sphere form a parallel extrema configuration with a known minimum.
TEST(GeomAPI_ExtremaCurveSurfaceTest, CircSph_ParallelConcentric)
{
  expectCircleSphereExtrema(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0), 4.0, 6.0, true);
}

// Migrated from tests/lowalgos/extcs/circ_sph_inter.  A circle on the sphere
// is an intersection case and its minimum extrema distance is zero.
TEST(GeomAPI_ExtremaCurveSurfaceTest, CircSph_Intersection)
{
  expectCircleSphereExtrema(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0), 10.0, 0.0, true);
}

// Migrated from tests/lowalgos/extcs/circ_sph_touch.  The internally tangent
// circle must report a zero lower distance even though the circle is bounded.
TEST(GeomAPI_ExtremaCurveSurfaceTest, CircSph_Touch)
{
  expectCircleSphereExtrema(gp_Pnt(8.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0), 2.0, 0.0, false);
}

// Migrated from tests/lowalgos/extcs/circ_sph_nointer.  A circle outside the
// sphere has a positive minimum distance.
TEST(GeomAPI_ExtremaCurveSurfaceTest, CircSph_NoIntersection)
{
  expectCircleSphereExtrema(gp_Pnt(0.0, 0.0, 12.0),
                            gp_Dir(0.0, 0.0, 1.0),
                            2.0,
                            std::sqrt(148.0) - 10.0,
                            true);
}

// Migrated from tests/bugs/modalg_5/bug25368_1.  Extrema between a trimmed
// line and a sphere must retain the intersection point on the curve.
TEST(GeomAPI_ExtremaCurveSurfaceTest, ModalgBug_25368_TrimmedLineSphere)
{
  const occ::handle<Geom_TrimmedCurve> aLine =
    new Geom_TrimmedCurve(new Geom_Line(gp_Pnt(0.0, 0.0, 0.2), gp_Dir(1.0, 1.0, 0.0)), 0.0, 10.0);
  const occ::handle<Geom_SphericalSurface> aSphere = makeSphere(5.0);

  double aUFirst = 0.0, aULast = 0.0, aVFirst = 0.0, aVLast = 0.0;
  aSphere->Bounds(aUFirst, aULast, aVFirst, aVLast);
  GeomAPI_ExtremaCurveSurface anExtrema(aLine,
                                        aSphere,
                                        aLine->FirstParameter(),
                                        aLine->LastParameter(),
                                        aUFirst,
                                        aULast,
                                        aVFirst,
                                        aVLast);

  ASSERT_EQ(anExtrema.NbExtrema(), 3);
  gp_Pnt aCurvePoint, aSurfacePoint;
  anExtrema.Points(1, aCurvePoint, aSurfacePoint);
  const gp_Pnt anExpectedPoint(3.5327043465311383, 3.5327043465311383, 0.2);
  EXPECT_TRUE(aCurvePoint.IsEqual(anExpectedPoint, 0.01));
  EXPECT_TRUE(aSurfacePoint.IsEqual(anExpectedPoint, 0.01));
  EXPECT_GT(anExtrema.Distance(2), 0.01);
  EXPECT_GT(anExtrema.Distance(3), 0.01);
}

// Migrated from tests/bugs/modalg_5/bug25368_2.  The rotated sphere and
// infinite line have two extrema, both of which are intersection points.
TEST(GeomAPI_ExtremaCurveSurfaceTest, ModalgBug_25368_RotatedSphereLine)
{
  const occ::handle<Geom_SphericalSurface> aSphere = makeSphere(4.5);
  gp_Trsf                                  aRotation;
  aRotation.SetRotation(gp_Ax1(gp_Pnt(0.0, 0.5, 1.0), gp_Dir(1.0, 1.0, 0.5)), 34.9 * M_PI / 180.0);
  aSphere->Transform(aRotation);

  const occ::handle<Geom_Line> aLine =
    new Geom_Line(gp_Pnt(-0.79, 0.88, 1.22), gp_Dir(0.579, 1.765, 0.576));
  double aUFirst = 0.0, aULast = 0.0, aVFirst = 0.0, aVLast = 0.0;
  aSphere->Bounds(aUFirst, aULast, aVFirst, aVLast);
  GeomAPI_ExtremaCurveSurface anExtrema(aLine,
                                        aSphere,
                                        aLine->FirstParameter(),
                                        aLine->LastParameter(),
                                        aUFirst,
                                        aULast,
                                        aVFirst,
                                        aVLast);

  ASSERT_EQ(anExtrema.NbExtrema(), 4);
  const gp_Pnt anExpectedPoint1(-2.2838350838816694, -3.6737459810900646, -0.2660950057268425);
  const gp_Pnt anExpectedPoint2(0.29086178559218934, 4.1748550113475211, 2.2952614654595873);
  gp_Pnt       aCurvePoint, aSurfacePoint;
  anExtrema.Points(1, aCurvePoint, aSurfacePoint);
  EXPECT_TRUE(aCurvePoint.IsEqual(anExpectedPoint1, 0.01));
  EXPECT_TRUE(aSurfacePoint.IsEqual(anExpectedPoint1, 0.01));
  anExtrema.Points(2, aCurvePoint, aSurfacePoint);
  EXPECT_TRUE(aCurvePoint.IsEqual(anExpectedPoint2, 0.01));
  EXPECT_TRUE(aSurfacePoint.IsEqual(anExpectedPoint2, 0.01));
  EXPECT_GT(anExtrema.Distance(3), 0.01);
  EXPECT_GT(anExtrema.Distance(4), 0.01);
}
