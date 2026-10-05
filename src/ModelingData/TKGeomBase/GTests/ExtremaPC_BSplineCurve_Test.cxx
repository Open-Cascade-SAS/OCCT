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

#include <ExtremaPC_BSplineCurve.hxx>
#include <ExtremaPC_GridEvaluator.hxx>
#include <ExtremaPC2d_BSplineCurve.hxx>

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <chrono>
#include <cmath>
#include <iomanip>
#include <vector>

//! Test fixture for ExtremaPC_BSplineCurve tests.
class ExtremaPC_BSplineCurveTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-6;

  //! Create a simple cubic BSpline curve (arc-like).
  occ::handle<Geom_BSplineCurve> createCubicBSpline() const
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 4);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(1, 2, 0);
    aPoles(3) = gp_Pnt(2, 2, 0);
    aPoles(4) = gp_Pnt(3, 0, 0);

    NCollection_Array1<double> aKnots(1, 2);
    aKnots(1) = 0.0;
    aKnots(2) = 1.0;

    NCollection_Array1<int> aMults(1, 2);
    aMults(1) = 4;
    aMults(2) = 4;

    return new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  }

  //! Create a linear BSpline (degree 1).
  occ::handle<Geom_BSplineCurve> createLinearBSpline() const
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 2);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(10, 0, 0);

    NCollection_Array1<double> aKnots(1, 2);
    aKnots(1) = 0.0;
    aKnots(2) = 1.0;

    NCollection_Array1<int> aMults(1, 2);
    aMults(1) = 2;
    aMults(2) = 2;

    return new Geom_BSplineCurve(aPoles, aKnots, aMults, 1);
  }

  //! Create a quadratic BSpline.
  occ::handle<Geom_BSplineCurve> createQuadraticBSpline() const
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 3);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(5, 10, 0);
    aPoles(3) = gp_Pnt(10, 0, 0);

    NCollection_Array1<double> aKnots(1, 2);
    aKnots(1) = 0.0;
    aKnots(2) = 1.0;

    NCollection_Array1<int> aMults(1, 2);
    aMults(1) = 3;
    aMults(2) = 3;

    return new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
  }

  //! Create a BSpline with multiple knots (more spans).
  //! For 5 poles and degree 2: sum of mults = 5 + 2 + 1 = 8
  occ::handle<Geom_BSplineCurve> createMultiSpanBSpline() const
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 5);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(1, 2, 0);
    aPoles(3) = gp_Pnt(2, 0, 0);
    aPoles(4) = gp_Pnt(3, 2, 0);
    aPoles(5) = gp_Pnt(4, 0, 0);

    NCollection_Array1<double> aKnots(1, 4);
    aKnots(1) = 0.0;
    aKnots(2) = 0.33;
    aKnots(3) = 0.67;
    aKnots(4) = 1.0;

    NCollection_Array1<int> aMults(1, 4);
    aMults(1) = 3; // sum = 3 + 1 + 1 + 3 = 8 OK
    aMults(2) = 1;
    aMults(3) = 1;
    aMults(4) = 3;

    return new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
  }

  //! Create a 3D BSpline (not in XY plane).
  occ::handle<Geom_BSplineCurve> create3DBSpline() const
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 4);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(1, 1, 1);
    aPoles(3) = gp_Pnt(2, 1, 2);
    aPoles(4) = gp_Pnt(3, 0, 3);

    NCollection_Array1<double> aKnots(1, 2);
    aKnots(1) = 0.0;
    aKnots(2) = 1.0;

    NCollection_Array1<int> aMults(1, 2);
    aMults(1) = 4;
    aMults(2) = 4;

    return new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  }
};

//==================================================================================================
// Basic tests
//==================================================================================================

TEST_F(ExtremaPC_BSplineCurveTest, PointOnCurve_Start)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createCubicBSpline();
  gp_Pnt                         aPoint(0.0, 0.0, 0.0);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BSplineCurveTest, PointOnCurve_End)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createCubicBSpline();
  gp_Pnt                         aPoint(3.0, 0.0, 0.0);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BSplineCurveTest, PointOnCurve_Middle)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createCubicBSpline();
  gp_Pnt                         aPoint   = aBSpline->Value(0.5);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BSplineCurveTest, SingletonDomain_ReturnsSolePoint)
{
  occ::handle<Geom_BSplineCurve> aBSpline   = createCubicBSpline();
  constexpr double               aParameter = 0.35;

  ExtremaPC_BSplineCurve   anEval(aBSpline, ExtremaPC::Domain1D{aParameter, aParameter});
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(gp_Pnt(5, 5, 0), THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].Parameter, aParameter, THE_TOL);
  EXPECT_TRUE(aResult[0].IsMinimum);
  EXPECT_TRUE(aResult[0].IsMaximum);
}

//==================================================================================================
// Point near curve tests
//==================================================================================================

TEST_F(ExtremaPC_BSplineCurveTest, PointNearCurve_Above)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createCubicBSpline();
  gp_Pnt                         aPoint(1.5, 3.0, 0.0);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify distance is reasonable
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_GT(aMinSqDist, 0.0);
  EXPECT_LT(aMinSqDist, 4.0); // Should be less than 2^2
}

TEST_F(ExtremaPC_BSplineCurveTest, PointNearCurve_Below)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createCubicBSpline();
  gp_Pnt                         aPoint(1.5, -1.0, 0.0);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify closest point is on the curve
  size_t aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBSpline->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);

  // Distance should be at least 1.0 (point is below curve which is above Y=0)
  EXPECT_GE(aResult.MinSquareDistance(), 1.0);
}

TEST_F(ExtremaPC_BSplineCurveTest, PointNearCurve_OutOfPlane)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createCubicBSpline();
  gp_Pnt                         aPoint(1.5, 2.0, 5.0);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Distance should include Z offset
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_GE(aMinSqDist, 25.0); // At least 5^2
}

//==================================================================================================
// Linear BSpline tests
//==================================================================================================

TEST_F(ExtremaPC_BSplineCurveTest, LinearBSpline_PointOnLine)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createLinearBSpline();
  gp_Pnt                         aPoint(5.0, 0.0, 0.0);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BSplineCurveTest, LinearBSpline_PointOffLine)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createLinearBSpline();
  gp_Pnt                         aPoint(5.0, 3.0, 4.0);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 25.0, THE_TOL); // 3^2 + 4^2 = 25
}

//==================================================================================================
// Quadratic BSpline tests
//==================================================================================================

TEST_F(ExtremaPC_BSplineCurveTest, QuadraticBSpline_PointNear)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createQuadraticBSpline();
  gp_Pnt                         aPoint(5.0, 8.0, 0.0);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify closest point is on the curve and distance is consistent
  size_t aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBSpline->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aPoint.SquareDistance(aPtOnCurve), THE_TOL);
}

TEST_F(ExtremaPC_BSplineCurveTest, QuadraticBSpline_PointOnApex)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createQuadraticBSpline();
  // Apex is approximately at middle
  gp_Pnt aPoint = aBSpline->Value(0.5);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

//==================================================================================================
// Multi-span BSpline tests
//==================================================================================================

TEST_F(ExtremaPC_BSplineCurveTest, MultiSpanBSpline_PointNear)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createMultiSpanBSpline();
  gp_Pnt                         aPoint(2.5, 3.0, 0.0);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify closest point is on the curve
  size_t aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBSpline->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);

  // Oscillating curve peaks at Y~=2, so distance from (2.5, 3.0) should be around 1
  EXPECT_LT(std::sqrt(aResult.MinSquareDistance()), 2.0);
}

TEST_F(ExtremaPC_BSplineCurveTest, MultiSpanBSpline_MultipleExtrema)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createMultiSpanBSpline();
  gp_Pnt                         aPoint(2.5, 1.0, 0.0);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // With oscillating curve, might find multiple local extrema
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify all extrema are on the curve
  for (size_t i = 0; i < aResult.NbExt(); ++i)
  {
    gp_Pnt aPtOnCurve = aBSpline->Value(aResult[i].Parameter);
    EXPECT_NEAR(aResult[i].Point.Distance(aPtOnCurve), 0.0, THE_TOL);
  }
}

//==================================================================================================
// 3D BSpline tests
//==================================================================================================

TEST_F(ExtremaPC_BSplineCurveTest, BSpline3D_PointOn)
{
  occ::handle<Geom_BSplineCurve> aBSpline = create3DBSpline();
  gp_Pnt                         aPoint   = aBSpline->Value(0.5);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BSplineCurveTest, BSpline3D_PointNear)
{
  occ::handle<Geom_BSplineCurve> aBSpline = create3DBSpline();
  gp_Pnt                         aPoint(1.5, 2.0, 1.5);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify closest point is on the curve and distance is consistent
  size_t aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBSpline->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aPoint.SquareDistance(aPtOnCurve), THE_TOL);
}

//==================================================================================================
// Bound constraint tests
//==================================================================================================

TEST_F(ExtremaPC_BSplineCurveTest, PartialRange_FirstHalf)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createCubicBSpline();
  gp_Pnt                         aPoint(2.5, 1.0, 0.0);

  // Create evaluator with domain restricted to first half [0, 0.5]
  ExtremaPC_BSplineCurve   anEval(aBSpline, ExtremaPC::Domain1D{0.0, 0.5});
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // All parameters should be in first half
  for (size_t i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_GE(aResult[i].Parameter, 0.0 - THE_TOL);
    EXPECT_LE(aResult[i].Parameter, 0.5 + THE_TOL);
  }
}

TEST_F(ExtremaPC_BSplineCurveTest, PartialRange_SecondHalf)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createCubicBSpline();
  gp_Pnt                         aPoint(0.5, 1.0, 0.0);

  // Create evaluator with domain restricted to second half [0.5, 1.0]
  ExtremaPC_BSplineCurve   anEval(aBSpline, ExtremaPC::Domain1D{0.5, 1.0});
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // All parameters should be in second half
  for (size_t i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_GE(aResult[i].Parameter, 0.5 - THE_TOL);
    EXPECT_LE(aResult[i].Parameter, 1.0 + THE_TOL);
  }
}

//==================================================================================================
// Verification tests
//==================================================================================================

TEST_F(ExtremaPC_BSplineCurveTest, VerifyProjectedPoint)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createCubicBSpline();
  gp_Pnt                         aPoint(1.5, 3.0, 0.0);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify the projected point is on the curve
  size_t aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBSpline->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.X(), aPtOnCurve.X(), THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].Point.Y(), aPtOnCurve.Y(), THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].Point.Z(), aPtOnCurve.Z(), THE_TOL);
}

TEST_F(ExtremaPC_BSplineCurveTest, VerifyDistanceConsistency)
{
  occ::handle<Geom_BSplineCurve> aBSpline = createCubicBSpline();
  gp_Pnt                         aPoint(2.0, 2.5, 1.0);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  GeomAdaptor_Curve        anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify distance matches point distance
  size_t aMinIdx     = aResult.MinIndex();
  double aComputedSq = aPoint.SquareDistance(aResult[aMinIdx].Point);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aComputedSq, THE_TOL);
}

//==================================================================================================
// Iterative grid refinement tests
//==================================================================================================

TEST_F(ExtremaPC_BSplineCurveTest, RefinementConfig_DisableRefinement)
{
  // Test that disabling refinement works
  occ::handle<Geom_BSplineCurve> aBSpline = createCubicBSpline();
  GeomAdaptor_Curve              anAdaptor(aBSpline);
  gp_Pnt                         aPoint(1.5, 2.0, 0.0);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  // Should work normally even with default refinement enabled
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaPC_BSplineCurveTest, HighDegreeSpline_RefinementHelps)
{
  // Create a high-degree spline where Newton might have more difficulty
  NCollection_Array1<gp_Pnt> aPoles(1, 8);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(0.5, 1.0, 0.0);
  aPoles(3) = gp_Pnt(1.0, -0.5, 0.0);
  aPoles(4) = gp_Pnt(1.5, 1.5, 0.0);
  aPoles(5) = gp_Pnt(2.0, 0.0, 0.0);
  aPoles(6) = gp_Pnt(2.5, 1.0, 0.0);
  aPoles(7) = gp_Pnt(3.0, -0.5, 0.0);
  aPoles(8) = gp_Pnt(3.5, 0.5, 0.0);

  NCollection_Array1<double> aKnots(1, 2);
  NCollection_Array1<int>    aMults(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;
  aMults(1) = 8;
  aMults(2) = 8;

  occ::handle<Geom_BSplineCurve> aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 7);
  GeomAdaptor_Curve              anAdaptor(aBSpline);

  // Point that requires accurate projection
  gp_Pnt aPoint(1.75, 0.8, 0.0);

  ExtremaPC_BSplineCurve   anEval(aBSpline);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify the closest point is on the curve
  size_t aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBSpline->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);

  // Verify the distance is reasonable (point is within expected range of curve)
  double aDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_LT(aDist, 2.0);
}

TEST_F(ExtremaPC_BSplineCurveTest, C1Junction_PointOnCurve_FindsMinimum)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles(3) = gp_Pnt(2.0, 1.0, 0.0);
  aPoles(4) = gp_Pnt(3.0, 0.0, 0.0);

  NCollection_Array1<double> aKnots(1, 3);
  aKnots(1) = 0.0;
  aKnots(2) = 0.5;
  aKnots(3) = 1.0;

  NCollection_Array1<int> aMultiplicities(1, 3);
  aMultiplicities(1) = 3;
  aMultiplicities(2) = 1;
  aMultiplicities(3) = 3;

  occ::handle<Geom_BSplineCurve> aBSpline =
    new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 2);
  const gp_Pnt aPoint = aBSpline->Value(0.5);

  ExtremaPC_BSplineCurve   anEvaluator(aBSpline);
  const ExtremaPC::Result& aResult = anEvaluator.Perform(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GT(aResult.NbExt(), 0);
  EXPECT_NEAR(aResult[aResult.MinIndex()].Parameter, 0.5, THE_TOL);
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BSplineCurveTest, ClusteredC0Junctions_FindsBothExtrema)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles(3) = gp_Pnt(2.0, 0.0, 0.0);
  aPoles(4) = gp_Pnt(3.0, 1.0, 0.0);

  NCollection_Array1<double> aKnots(1, 4);
  aKnots(1) = 0.0;
  aKnots(2) = 0.499;
  aKnots(3) = 0.501;
  aKnots(4) = 1.0;

  NCollection_Array1<int> aMultiplicities(1, 4);
  aMultiplicities(1) = 2;
  aMultiplicities(2) = 1;
  aMultiplicities(3) = 1;
  aMultiplicities(4) = 2;

  occ::handle<Geom_BSplineCurve> aBSpline =
    new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 1);
  ExtremaPC_BSplineCurve   anEvaluator(aBSpline);
  const ExtremaPC::Result& aResult = anEvaluator.Perform(gp_Pnt(1.5, 1.5, 0.0), THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  bool hasFirstJunction  = false;
  bool hasSecondJunction = false;
  for (size_t anIndex = 0; anIndex < aResult.NbExt(); ++anIndex)
  {
    if (std::abs(aResult[anIndex].Parameter - 0.499) <= THE_TOL)
    {
      hasFirstJunction = true;
      EXPECT_TRUE(aResult[anIndex].IsMinimum);
    }
    if (std::abs(aResult[anIndex].Parameter - 0.501) <= THE_TOL)
    {
      hasSecondJunction = true;
      EXPECT_TRUE(aResult[anIndex].IsMaximum);
    }
  }
  EXPECT_TRUE(hasFirstJunction);
  EXPECT_TRUE(hasSecondJunction);

  const ExtremaPC::Result& aMinResult =
    anEvaluator.Perform(gp_Pnt(1.5, 1.5, 0.0), THE_TOL, ExtremaPC::SearchMode::Min);
  hasFirstJunction  = false;
  hasSecondJunction = false;
  for (size_t anIndex = 0; anIndex < aMinResult.NbExt(); ++anIndex)
  {
    hasFirstJunction =
      hasFirstJunction || std::abs(aMinResult[anIndex].Parameter - 0.499) <= THE_TOL;
    hasSecondJunction =
      hasSecondJunction || std::abs(aMinResult[anIndex].Parameter - 0.501) <= THE_TOL;
  }
  EXPECT_TRUE(hasFirstJunction);
  EXPECT_FALSE(hasSecondJunction);

  const ExtremaPC::Result& aMaxResult =
    anEvaluator.Perform(gp_Pnt(1.5, 1.5, 0.0), THE_TOL, ExtremaPC::SearchMode::Max);
  hasFirstJunction  = false;
  hasSecondJunction = false;
  for (size_t anIndex = 0; anIndex < aMaxResult.NbExt(); ++anIndex)
  {
    hasFirstJunction =
      hasFirstJunction || std::abs(aMaxResult[anIndex].Parameter - 0.499) <= THE_TOL;
    hasSecondJunction =
      hasSecondJunction || std::abs(aMaxResult[anIndex].Parameter - 0.501) <= THE_TOL;
  }
  EXPECT_FALSE(hasFirstJunction);
  EXPECT_TRUE(hasSecondJunction);
}

//=================================================================================================

TEST_F(ExtremaPC_BSplineCurveTest, PlanarDelegation_RationalCurvePreservesDomainAndEndpoints)
{
  NCollection_Array1<gp_Pnt>   aPoles3d(1, 4);
  NCollection_Array1<gp_Pnt2d> aPoles2d(1, 4);
  NCollection_Array1<double>   aWeights(1, 4);
  for (size_t anIndex = 0; anIndex < aPoles3d.Size(); ++anIndex)
  {
    const double anOrdinal     = static_cast<double>(anIndex + 1);
    const double anX           = 10.0 + anOrdinal;
    const double anY           = (anIndex + 1) % 2 == 0 ? 2.0 : -1.0;
    aPoles3d.ChangeAt(anIndex) = gp_Pnt(anX, -4.0, anY);
    aPoles2d.ChangeAt(anIndex) = gp_Pnt2d(anX, anY);
    aWeights.ChangeAt(anIndex) = 0.5 + 0.25 * anOrdinal;
  }
  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 2.0;
  aKnots(2) = 6.0;
  NCollection_Array1<int> aMultiplicities(1, 2);
  aMultiplicities(1) = 4;
  aMultiplicities(2) = 4;
  occ::handle<Geom_BSplineCurve> aCurve3d =
    new Geom_BSplineCurve(aPoles3d, aWeights, aKnots, aMultiplicities, 3);
  occ::handle<Geom2d_BSplineCurve> aCurve2d =
    new Geom2d_BSplineCurve(aPoles2d, aWeights, aKnots, aMultiplicities, 3);
  ExtremaPC_BSplineCurve   anEvaluator3d(aCurve3d, ExtremaPC::Domain1D{2.5, 5.5});
  ExtremaPC2d_BSplineCurve anEvaluator2d(aCurve2d, ExtremaPC2d::Domain1D{2.5, 5.5});
  const ExtremaPC::Result& aResult3d =
    anEvaluator3d.PerformWithEndpoints(gp_Pnt(12.0, 3.0, 0.5), THE_TOL);
  const ExtremaPC2d::Result& aResult2d =
    anEvaluator2d.PerformWithEndpoints(gp_Pnt2d(12.0, 0.5), THE_TOL);
  ASSERT_EQ(aResult3d.NbExt(), aResult2d.NbExt());
  for (size_t anIndex = 0; anIndex < aResult3d.NbExt(); ++anIndex)
  {
    EXPECT_NEAR(aResult3d[anIndex].Parameter, aResult2d[anIndex].Parameter, THE_TOL);
    EXPECT_NEAR(aResult3d[anIndex].SquareDistance,
                aResult2d[anIndex].SquareDistance + 49.0,
                THE_TOL);
  }
}

//=================================================================================================

TEST_F(ExtremaPC_BSplineCurveTest, PlanarDelegation_NonPlanarCurveUses3dFallback)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 2.0, 1.0);
  aPoles(3) = gp_Pnt(2.0, -1.0, 3.0);
  aPoles(4) = gp_Pnt(4.0, 0.0, -2.0);
  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;
  NCollection_Array1<int> aMultiplicities(1, 2);
  aMultiplicities(1)                    = 4;
  aMultiplicities(2)                    = 4;
  occ::handle<Geom_BSplineCurve> aCurve = new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 3);
  ExtremaPC_BSplineCurve         anEvaluator(aCurve);
  const ExtremaPC::Result&       aResult = anEvaluator.Perform(gp_Pnt(1.0, 0.5, 2.0), THE_TOL);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaPC_BSplineCurveTest, ShiftedMultiPeriodNonPlanarCurveFindsEveryMinimum)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 5);
  aPoles(1) = gp_Pnt(1.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(0.3, 1.0, 0.4);
  aPoles(3) = gp_Pnt(-0.8, 0.6, -0.2);
  aPoles(4) = gp_Pnt(-0.7, -0.7, 0.8);
  aPoles(5) = gp_Pnt(0.4, -0.9, -0.5);
  NCollection_Array1<double> aKnots(1, 6);
  for (size_t anIndex = 0; anIndex < aKnots.Size(); ++anIndex)
  {
    aKnots.ChangeAt(anIndex) = 0.2 * static_cast<double>(anIndex);
  }
  NCollection_Array1<int> aMultiplicities(1, 6);
  aMultiplicities.Init(1);
  occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 3, true);

  const double              aPeriod = aCurve->Period();
  const double              aTarget = aCurve->FirstParameter() + 0.17 * aPeriod;
  const ExtremaPC::Domain1D aDomain{aTarget - 0.1 * aPeriod, aTarget + 3.1 * aPeriod};
  ExtremaPC_BSplineCurve    anEvaluator(aCurve, aDomain);
  const ExtremaPC::Result&  aResult =
    anEvaluator.Perform(aCurve->Value(aTarget), 1.0e-9, ExtremaPC::SearchMode::Min);

  ASSERT_TRUE(aResult.IsDone());
  int aNbZeroDistanceMinima = 0;
  for (size_t anIndex = 0; anIndex < aResult.NbExt(); ++anIndex)
  {
    if (aResult[anIndex].SquareDistance <= 1.0e-16)
    {
      EXPECT_NEAR(aResult[anIndex].Parameter, aTarget + aNbZeroDistanceMinima * aPeriod, 1.0e-7);
      ++aNbZeroDistanceMinima;
    }
  }
  EXPECT_EQ(aNbZeroDistanceMinima, 4);
}

TEST_F(ExtremaPC_BSplineCurveTest, GridUtilitiesRejectInvalidSamplesAndShortOpenClosure)
{
#if !defined(No_Exception) && !defined(No_Standard_RangeError)
  EXPECT_THROW(ExtremaPC_GridEvaluator::BuildUniformParams(0.0, 1.0, 1), Standard_RangeError);
#endif

  NCollection_Array1<gp_Pnt> aPoles(1, 2);
  aPoles(1)                            = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2)                            = gp_Pnt(0.5 * Precision::Confusion(), 0.0, 0.0);
  occ::handle<Geom_BezierCurve> aCurve = new Geom_BezierCurve(aPoles);
  GeomAdaptor_Curve             anAdaptor(aCurve);
  EXPECT_FALSE(ExtremaPC_GridEvaluator::IsClosedDomain(anAdaptor, {0.0, 1.0}));
}
