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

#include <ExtremaPS_Cone.hxx>
#include <ExtremaPS_Surface.hxx>

#include <Geom_ConicalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for ExtremaPS_Cone tests.
class ExtremaPS_ConeTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-6;
};

//==================================================================================================
// Basic projection tests
//==================================================================================================

TEST_F(ExtremaPS_ConeTest, PointOutside_OnXAxis)
{
  // Cone with apex at origin, axis along Z, semi-angle 45 degrees
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);
  gp_Pnt aPoint(20.0, 0.0, 5.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify that stored distance matches computed distance
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aStoredSqDist = aResult[i].SquareDistance;
    double aComputedSqDist = aPoint.SquareDistance(aResult[i].Point);
    EXPECT_NEAR(aStoredSqDist, aComputedSqDist, THE_TOL);
  }
}

TEST_F(ExtremaPS_ConeTest, PointOutside_OnYAxis)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);
  gp_Pnt aPoint(0.0, 20.0, 5.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaPS_ConeTest, PointOutside_Diagonal)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);
  gp_Pnt aPoint(15.0, 15.0, 5.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, M_PI / 4.0, 0.1);
}

TEST_F(ExtremaPS_ConeTest, PointInside)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);
  // Point inside cone (distance from axis < height * tan(angle))
  gp_Pnt aPoint(2.0, 0.0, 10.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

//==================================================================================================
// Degenerate case tests (apex, axis)
//==================================================================================================

TEST_F(ExtremaPS_ConeTest, PointAtApex_Degenerate)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);
  gp_Pnt aPoint(0.0, 0.0, 0.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 0.0, THE_TOL);
}

TEST_F(ExtremaPS_ConeTest, PointNearApex)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);
  gp_Pnt aPoint(0.0, 0.0, 0.001);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone() || aResult.IsInfinite());
}

TEST_F(ExtremaPS_ConeTest, PointOnAxis_Degenerate)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);
  gp_Pnt aPoint(0.0, 0.0, 10.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  EXPECT_TRUE(aResult.IsInfinite());
}

TEST_F(ExtremaPS_ConeTest, PointOnAxisBelowApex)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);
  gp_Pnt aPoint(0.0, 0.0, -5.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  // Below apex, closest to apex at V=0
  ASSERT_TRUE(aResult.IsDone() || aResult.IsInfinite());
}

//==================================================================================================
// Different semi-angles
//==================================================================================================

TEST_F(ExtremaPS_ConeTest, SmallSemiAngle_30Deg)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 6.0, 0.0);
  gp_Pnt aPoint(10.0, 0.0, 15.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 30.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaPS_ConeTest, LargeSemiAngle_60Deg)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 3.0, 0.0);
  gp_Pnt aPoint(10.0, 0.0, 5.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaPS_ConeTest, VerySmallSemiAngle)
{
  // Almost a cylinder
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.01, 10.0);
  gp_Pnt aPoint(15.0, 0.0, 5.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

//==================================================================================================
// Cone with non-zero reference radius
//==================================================================================================

TEST_F(ExtremaPS_ConeTest, NonZeroRefRadius)
{
  // Cone with apex not at origin (reference radius = 5)
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 5.0);
  gp_Pnt aPoint(15.0, 0.0, 5.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaPS_ConeTest, LargeRefRadius)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 100.0);
  gp_Pnt aPoint(150.0, 0.0, 10.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 50.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

//==================================================================================================
// Boundary tests
//==================================================================================================

TEST_F(ExtremaPS_ConeTest, PartialDomain_U)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);
  gp_Pnt aPoint(15.0, 15.0, 5.0);

  // Only first quadrant
  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, M_PI / 2.0, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, M_PI / 4.0, 0.1);
}

TEST_F(ExtremaPS_ConeTest, PartialDomain_V)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);
  gp_Pnt aPoint(10.0, 0.0, 25.0);

  // Limited V range
  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 5.0, 15.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

//==================================================================================================
// Rotated cone tests
//==================================================================================================

TEST_F(ExtremaPS_ConeTest, ConeAlongX)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), M_PI / 4.0, 0.0);
  gp_Pnt aPoint(10.0, 20.0, 0.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 30.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaPS_ConeTest, ConeAlongY)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), M_PI / 4.0, 0.0);
  gp_Pnt aPoint(20.0, 10.0, 0.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 30.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

//==================================================================================================
// Translated cone tests
//==================================================================================================

TEST_F(ExtremaPS_ConeTest, TranslatedCone)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(100, 50, 25), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);
  gp_Pnt aPoint(120.0, 50.0, 30.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

//==================================================================================================
// SearchMode tests
//==================================================================================================

TEST_F(ExtremaPS_ConeTest, SearchMode_Min)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);
  gp_Pnt aPoint(20.0, 0.0, 5.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);
  // All returned extrema should be minima
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_TRUE(aResult[i].IsMinimum);
  }
}

TEST_F(ExtremaPS_ConeTest, SearchMode_Max)
{
  gp_Cone aCone(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);
  gp_Pnt aPoint(20.0, 0.0, 5.0);

  ExtremaPS_Cone anEval(aCone, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Max);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

//==================================================================================================
// Aggregator tests
//==================================================================================================

TEST_F(ExtremaPS_ConeTest, Aggregator_Basic)
{
  Handle(Geom_ConicalSurface) aGeomCone = new Geom_ConicalSurface(
    gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 0.0);
  GeomAdaptor_Surface anAdaptor(aGeomCone, 0.0, 2.0 * M_PI, 0.0, 20.0);

  ExtremaPS_Surface anExtPS(anAdaptor);
  EXPECT_TRUE(anExtPS.IsInitialized());

  gp_Pnt aPoint(20.0, 0.0, 5.0);
  const ExtremaPS::Result& aResult = anExtPS.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaPS_ConeTest, Aggregator_NonZeroRefRadius)
{
  Handle(Geom_ConicalSurface) aGeomCone = new Geom_ConicalSurface(
    gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4.0, 5.0);
  GeomAdaptor_Surface anAdaptor(aGeomCone, 0.0, 2.0 * M_PI, 0.0, 20.0);

  ExtremaPS_Surface anExtPS(anAdaptor);

  gp_Pnt aPoint(15.0, 0.0, 5.0);
  const ExtremaPS::Result& aResult = anExtPS.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}
