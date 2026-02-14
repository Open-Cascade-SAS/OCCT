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

#include <ExtremaPS_Cylinder.hxx>
#include <ExtremaPS_Surface.hxx>

#include <Geom_CylindricalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

//! Test fixture for ExtremaPS_Cylinder tests.
class ExtremaPS_CylinderTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-6;
};

//==================================================================================================
// Basic projection tests
//==================================================================================================

TEST_F(ExtremaPS_CylinderTest, PointOutside_OnXAxis)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt      aPoint(25.0, 0.0, 5.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 25 - 10 = 15
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPS_CylinderTest, PointOutside_OnYAxis)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt      aPoint(0.0, 25.0, 5.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPS_CylinderTest, PointOutside_Diagonal)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  // Point at 45 degrees, distance from axis = 25*sqrt(2)/sqrt(2) = 25
  gp_Pnt aPoint(25.0, 25.0, 5.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Distance from axis is sqrt(25^2 + 25^2) = 35.35, min dist = 35.35 - 10 = 25.35
  double aDistFromAxis = std::sqrt(25.0 * 25.0 + 25.0 * 25.0);
  double aExpectedDist = aDistFromAxis - 10.0;
  double aMinDist      = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, aExpectedDist, THE_TOL);
}

TEST_F(ExtremaPS_CylinderTest, PointInside)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt      aPoint(3.0, 0.0, 5.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 10 - 3 = 7
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 7.0, THE_TOL);
}

TEST_F(ExtremaPS_CylinderTest, PointOnSurface)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt      aPoint(10.0, 0.0, 5.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 0.0, THE_TOL);
}

//==================================================================================================
// Degenerate case tests
//==================================================================================================

TEST_F(ExtremaPS_CylinderTest, PointOnAxis_Degenerate)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt      aPoint(0.0, 0.0, 5.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  // Should return infinite solutions
  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 10.0, THE_TOL);
}

TEST_F(ExtremaPS_CylinderTest, PointOnAxis_BottomOfCylinder)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt      aPoint(0.0, 0.0, 0.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  EXPECT_TRUE(aResult.IsInfinite());
}

TEST_F(ExtremaPS_CylinderTest, PointOnAxis_TopOfCylinder)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt      aPoint(0.0, 0.0, 20.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  EXPECT_TRUE(aResult.IsInfinite());
}

//==================================================================================================
// Seam tests
//==================================================================================================

TEST_F(ExtremaPS_CylinderTest, PointNearSeam_Positive)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  // Point near seam (U = 0 and U = 2*PI are the same)
  gp_Pnt aPoint(15.0, -0.1, 5.0); // Slightly off the seam

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  int    aMinIdx = aResult.MinIndex();
  double aU      = aResult[aMinIdx].U;
  // U should be near 0 or near 2*PI
  bool aNearSeam = (aU < 0.1) || (aU > 2.0 * M_PI - 0.1);
  EXPECT_TRUE(aNearSeam);
}

TEST_F(ExtremaPS_CylinderTest, PointNearSeam_Negative)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt      aPoint(15.0, 0.1, 5.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

//==================================================================================================
// Boundary tests
//==================================================================================================

TEST_F(ExtremaPS_CylinderTest, PointOnBoundaryV_Bottom)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt      aPoint(10.0, 0.0, -5.0); // Below V=0

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 10.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Closest point should be at V=0 boundary
  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].V, 0.0, THE_TOL);
}

TEST_F(ExtremaPS_CylinderTest, PointOnBoundaryV_Top)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt      aPoint(10.0, 0.0, 25.0); // Above V=20

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Closest point should be at V=20 boundary
  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].V, 20.0, THE_TOL);
}

TEST_F(ExtremaPS_CylinderTest, PartialDomain)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  // Point in the first quadrant
  gp_Pnt aPoint(20.0, 20.0, 5.0);

  // Only first quadrant: U in [0, PI/2]
  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, M_PI / 2.0, 0.0, 10.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum should be at U = PI/4 (45 deg)
  int aMinIdx = aResult.MinIndex();
  EXPECT_NEAR(aResult[aMinIdx].U, M_PI / 4.0, THE_TOL);
}

//==================================================================================================
// Different radii tests
//==================================================================================================

TEST_F(ExtremaPS_CylinderTest, SmallRadius)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.1);
  gp_Pnt      aPoint(1.0, 0.0, 0.5);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 0.9, THE_TOL);
}

TEST_F(ExtremaPS_CylinderTest, LargeRadius)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 1000.0);
  gp_Pnt      aPoint(2000.0, 0.0, 500.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 1000.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 1000.0, THE_TOL);
}

//==================================================================================================
// Rotated cylinder tests
//==================================================================================================

TEST_F(ExtremaPS_CylinderTest, CylinderAlongX)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 10.0);
  gp_Pnt      aPoint(5.0, 25.0, 0.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -20.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPS_CylinderTest, CylinderAlongY)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0)), 10.0);
  gp_Pnt      aPoint(25.0, 5.0, 0.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -20.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

TEST_F(ExtremaPS_CylinderTest, CylinderDiagonal)
{
  gp_Dir      aDiag(1, 1, 1);
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), aDiag), 10.0);

  // Point perpendicular to diagonal at distance ~25
  gp_Pnt aPoint(25.0, 0.0, 0.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, -30.0, 30.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

//==================================================================================================
// Translated cylinder tests
//==================================================================================================

TEST_F(ExtremaPS_CylinderTest, TranslatedCylinder)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(100, 50, 25), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt      aPoint(125.0, 50.0, 30.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Distance from axis is 25, min dist = 25 - 10 = 15
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}

//==================================================================================================
// SearchMode tests
//==================================================================================================

TEST_F(ExtremaPS_CylinderTest, SearchMode_Min)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt      aPoint(25.0, 0.0, 5.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult =
    anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbExt(), 1);
  EXPECT_TRUE(aResult[0].IsMinimum);
}

TEST_F(ExtremaPS_CylinderTest, SearchMode_Max)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt      aPoint(25.0, 0.0, 5.0);

  ExtremaPS_Cylinder       anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));
  const ExtremaPS::Result& aResult =
    anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Max);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
  EXPECT_FALSE(aResult[0].IsMinimum);
}

TEST_F(ExtremaPS_CylinderTest, ModesConsistency)
{
  gp_Cylinder aCylinder(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  gp_Pnt      aPoint(25.0, 0.0, 5.0);

  ExtremaPS_Cylinder anEval(aCylinder, ExtremaPS::Domain2D(0.0, 2.0 * M_PI, 0.0, 20.0));

  const ExtremaPS::Result& aResultAll =
    anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::MinMax);
  const ExtremaPS::Result& aResultMin =
    anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Min);
  const ExtremaPS::Result& aResultMax =
    anEval.PerformWithBoundary(aPoint, THE_TOL, ExtremaPS::SearchMode::Max);

  ASSERT_TRUE(aResultAll.IsDone());
  ASSERT_TRUE(aResultMin.IsDone());
  ASSERT_TRUE(aResultMax.IsDone());

  EXPECT_NEAR(aResultAll.MinSquareDistance(), aResultMin.MinSquareDistance(), THE_TOL);
  EXPECT_NEAR(aResultAll.MaxSquareDistance(), aResultMax.MaxSquareDistance(), THE_TOL);
}

//==================================================================================================
// Aggregator tests
//==================================================================================================

TEST_F(ExtremaPS_CylinderTest, Aggregator_Basic)
{
  occ::handle<Geom_CylindricalSurface> aGeomCylinder =
    new Geom_CylindricalSurface(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Surface anAdaptor(aGeomCylinder, 0.0, 2.0 * M_PI, 0.0, 20.0);

  ExtremaPS_Surface anExtPS(anAdaptor);
  EXPECT_TRUE(anExtPS.IsInitialized());

  gp_Pnt                   aPoint(25.0, 0.0, 5.0);
  const ExtremaPS::Result& aResult = anExtPS.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 15.0, THE_TOL);
}
