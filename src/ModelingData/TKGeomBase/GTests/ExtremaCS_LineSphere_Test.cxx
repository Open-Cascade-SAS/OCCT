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

#include <ExtremaCS_LineSphere.hxx>

#include <gp_Ax3.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>

#include <cmath>

//! Test fixture for ExtremaCS_LineSphere tests.
class ExtremaCS_LineSphereTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-9;
};

//==================================================================================================
// Line through sphere center tests
//==================================================================================================

TEST_F(ExtremaCS_LineSphereTest, ThroughCenter_XAxis)
{
  // Line passing through sphere center along X axis
  gp_Lin    aLine(gp_Pnt(-10, 0, 0), gp_Dir(1, 0, 0));
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_LineSphere     anExtrema(aLine, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Line passes through sphere - minimum distance is 0 (intersection)
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaCS_LineSphereTest, ThroughCenter_Oblique)
{
  // Line passing through sphere center at 45 degrees
  gp_Lin    aLine(gp_Pnt(-10, -10, 0), gp_Dir(1, 1, 0));
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_LineSphere     anExtrema(aLine, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Line passes through sphere - minimum distance is 0
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

//==================================================================================================
// Tangent line tests
//==================================================================================================

TEST_F(ExtremaCS_LineSphereTest, Tangent_AtEquator)
{
  // Line tangent to sphere at (5, 0, 0)
  gp_Lin    aLine(gp_Pnt(5, -10, 0), gp_Dir(0, 1, 0));
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_LineSphere     anExtrema(aLine, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Tangent line - minimum distance is 0
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaCS_LineSphereTest, Tangent_AtPole)
{
  // Line tangent to sphere at north pole (0, 0, 5)
  gp_Lin    aLine(gp_Pnt(-10, 0, 5), gp_Dir(1, 0, 0));
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_LineSphere     anExtrema(aLine, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Tangent line - minimum distance is 0
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

//==================================================================================================
// External line tests (line misses sphere)
//==================================================================================================

TEST_F(ExtremaCS_LineSphereTest, External_ParallelToAxis)
{
  // Line at Y=8, parallel to X axis, sphere radius 5
  gp_Lin    aLine(gp_Pnt(-10, 8, 0), gp_Dir(1, 0, 0));
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_LineSphere     anExtrema(aLine, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Min distance = 8 - 5 = 3
  EXPECT_NEAR(aResult.MinSquareDistance(), 9.0, THE_TOL);
}

TEST_F(ExtremaCS_LineSphereTest, External_AboveSphere)
{
  // Line at Z=10, parallel to X axis, sphere radius 5
  gp_Lin    aLine(gp_Pnt(-10, 0, 10), gp_Dir(1, 0, 0));
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_LineSphere     anExtrema(aLine, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Min distance = 10 - 5 = 5
  EXPECT_NEAR(aResult.MinSquareDistance(), 25.0, THE_TOL);
}

TEST_F(ExtremaCS_LineSphereTest, External_Oblique)
{
  // Line at distance from sphere
  gp_Lin    aLine(gp_Pnt(10, 10, 0), gp_Dir(0, 0, 1));
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_LineSphere     anExtrema(aLine, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Distance from line to center = sqrt(10^2 + 10^2) = 10*sqrt(2)
  // Min distance to sphere = 10*sqrt(2) - 5
  double aExpectedDist = 10.0 * std::sqrt(2.0) - 5.0;
  EXPECT_NEAR(aResult.MinSquareDistance(), aExpectedDist * aExpectedDist, 0.01);
}

//==================================================================================================
// Intersection verification tests
//==================================================================================================

TEST_F(ExtremaCS_LineSphereTest, Intersection_TwoPoints)
{
  // Line passing through sphere, not through center
  gp_Lin    aLine(gp_Pnt(-10, 3, 0), gp_Dir(1, 0, 0));
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_LineSphere     anExtrema(aLine, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // Should have 2 intersection points
  ASSERT_GE(aResult.NbExt(), 2);

  // Both should be at distance 0
  EXPECT_NEAR(aResult[0].SquareDistance, 0.0, THE_TOL);
  EXPECT_NEAR(aResult[1].SquareDistance, 0.0, THE_TOL);

  // Intersection at y=3, x = +/- sqrt(25-9) = +/- 4
  // t values should be 10-4=6 and 10+4=14 (line starts at x=-10)
  double aT1 = aResult[0].ParameterOnCurve;
  double aT2 = aResult[1].ParameterOnCurve;
  EXPECT_TRUE((std::abs(aT1 - 6.0) < THE_TOL && std::abs(aT2 - 14.0) < THE_TOL)
              || (std::abs(aT1 - 14.0) < THE_TOL && std::abs(aT2 - 6.0) < THE_TOL));
}

//==================================================================================================
// Domain constraint tests
//==================================================================================================

TEST_F(ExtremaCS_LineSphereTest, Domain_IntersectionInside)
{
  gp_Lin    aLine(gp_Pnt(-10, 0, 0), gp_Dir(1, 0, 0));
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS::Domain3D aDomain;
  aDomain.Curve   = {0.0, 20.0};
  aDomain.Surface = {0.0, ExtremaCS::THE_TWO_PI, -M_PI / 2.0, M_PI / 2.0};

  ExtremaCS_LineSphere     anExtrema(aLine, aSphere, aDomain);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaCS_LineSphereTest, Domain_IntersectionOutside)
{
  gp_Lin    aLine(gp_Pnt(-10, 0, 0), gp_Dir(1, 0, 0));
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  // Curve domain excludes intersection (t=5 and t=15 are outside [0, 3])
  ExtremaCS::Domain3D aDomain;
  aDomain.Curve   = {0.0, 3.0};
  aDomain.Surface = {0.0, ExtremaCS::THE_TWO_PI, -M_PI / 2.0, M_PI / 2.0};

  ExtremaCS_LineSphere     anExtrema(aLine, aSphere, aDomain);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL);

  // Intersection at t=5 is outside [0,3], so minimum is not at intersection
  // The result should either be NoSolution or the minimum found should be > 0
  if (aResult.IsDone())
  {
    // If found, minimum should be > 0 (not an intersection)
    EXPECT_GT(aResult.MinSquareDistance(), 0.0);
    // Parameter should be within domain
    EXPECT_GE(aResult[0].ParameterOnCurve, 0.0 - THE_TOL);
    EXPECT_LE(aResult[0].ParameterOnCurve, 3.0 + THE_TOL);
  }
}

//==================================================================================================
// Search mode tests
//==================================================================================================

TEST_F(ExtremaCS_LineSphereTest, SearchMode_MinOnly)
{
  gp_Lin    aLine(gp_Pnt(-10, 8, 0), gp_Dir(1, 0, 0));
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  ExtremaCS_LineSphere     anExtrema(aLine, aSphere);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
  EXPECT_TRUE(aResult[0].IsMinimum);
}

TEST_F(ExtremaCS_LineSphereTest, SearchMode_MaxOnly_Bounded)
{
  gp_Lin    aLine(gp_Pnt(-10, 0, 0), gp_Dir(1, 0, 0));
  gp_Sphere aSphere(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);

  // Need bounded domain for max on line-sphere
  ExtremaCS::Domain3D aDomain;
  aDomain.Curve   = {0.0, 20.0};
  aDomain.Surface = {0.0, ExtremaCS::THE_TWO_PI, -M_PI / 2.0, M_PI / 2.0};

  ExtremaCS_LineSphere     anExtrema(aLine, aSphere, aDomain);
  const ExtremaCS::Result& aResult = anExtrema.Perform(THE_TOL, ExtremaCS::SearchMode::Max);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
  EXPECT_FALSE(aResult[0].IsMinimum);
}
