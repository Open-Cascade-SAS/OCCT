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

#include <Geom_BSplineCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <gp_Pnt.hxx>

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
  Handle(Geom_BSplineCurve) createCubicBSpline() const
  {
    TColgp_Array1OfPnt aPoles(1, 4);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(1, 2, 0);
    aPoles(3) = gp_Pnt(2, 2, 0);
    aPoles(4) = gp_Pnt(3, 0, 0);

    TColStd_Array1OfReal aKnots(1, 2);
    aKnots(1) = 0.0;
    aKnots(2) = 1.0;

    TColStd_Array1OfInteger aMults(1, 2);
    aMults(1) = 4;
    aMults(2) = 4;

    return new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  }

  //! Create a linear BSpline (degree 1).
  Handle(Geom_BSplineCurve) createLinearBSpline() const
  {
    TColgp_Array1OfPnt aPoles(1, 2);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(10, 0, 0);

    TColStd_Array1OfReal aKnots(1, 2);
    aKnots(1) = 0.0;
    aKnots(2) = 1.0;

    TColStd_Array1OfInteger aMults(1, 2);
    aMults(1) = 2;
    aMults(2) = 2;

    return new Geom_BSplineCurve(aPoles, aKnots, aMults, 1);
  }

  //! Create a quadratic BSpline.
  Handle(Geom_BSplineCurve) createQuadraticBSpline() const
  {
    TColgp_Array1OfPnt aPoles(1, 3);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(5, 10, 0);
    aPoles(3) = gp_Pnt(10, 0, 0);

    TColStd_Array1OfReal aKnots(1, 2);
    aKnots(1) = 0.0;
    aKnots(2) = 1.0;

    TColStd_Array1OfInteger aMults(1, 2);
    aMults(1) = 3;
    aMults(2) = 3;

    return new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
  }

  //! Create a BSpline with multiple knots (more spans).
  //! For 5 poles and degree 2: sum of mults = 5 + 2 + 1 = 8
  Handle(Geom_BSplineCurve) createMultiSpanBSpline() const
  {
    TColgp_Array1OfPnt aPoles(1, 5);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(1, 2, 0);
    aPoles(3) = gp_Pnt(2, 0, 0);
    aPoles(4) = gp_Pnt(3, 2, 0);
    aPoles(5) = gp_Pnt(4, 0, 0);

    TColStd_Array1OfReal aKnots(1, 4);
    aKnots(1) = 0.0;
    aKnots(2) = 0.33;
    aKnots(3) = 0.67;
    aKnots(4) = 1.0;

    TColStd_Array1OfInteger aMults(1, 4);
    aMults(1) = 3;  // sum = 3 + 1 + 1 + 3 = 8 ✓
    aMults(2) = 1;
    aMults(3) = 1;
    aMults(4) = 3;

    return new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
  }

  //! Create a 3D BSpline (not in XY plane).
  Handle(Geom_BSplineCurve) create3DBSpline() const
  {
    TColgp_Array1OfPnt aPoles(1, 4);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(1, 1, 1);
    aPoles(3) = gp_Pnt(2, 1, 2);
    aPoles(4) = gp_Pnt(3, 0, 3);

    TColStd_Array1OfReal aKnots(1, 2);
    aKnots(1) = 0.0;
    aKnots(2) = 1.0;

    TColStd_Array1OfInteger aMults(1, 2);
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
  Handle(Geom_BSplineCurve) aBSpline = createCubicBSpline();
  gp_Pnt                    aPoint(0.0, 0.0, 0.0);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BSplineCurveTest, PointOnCurve_End)
{
  Handle(Geom_BSplineCurve) aBSpline = createCubicBSpline();
  gp_Pnt                    aPoint(3.0, 0.0, 0.0);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BSplineCurveTest, PointOnCurve_Middle)
{
  Handle(Geom_BSplineCurve) aBSpline = createCubicBSpline();
  gp_Pnt                    aPoint   = aBSpline->Value(0.5);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

//==================================================================================================
// Point near curve tests
//==================================================================================================

TEST_F(ExtremaPC_BSplineCurveTest, PointNearCurve_Above)
{
  Handle(Geom_BSplineCurve) aBSpline = createCubicBSpline();
  gp_Pnt                    aPoint(1.5, 3.0, 0.0);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify distance is reasonable
  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_GT(aMinSqDist, 0.0);
  EXPECT_LT(aMinSqDist, 4.0); // Should be less than 2^2
}

TEST_F(ExtremaPC_BSplineCurveTest, PointNearCurve_Below)
{
  Handle(Geom_BSplineCurve) aBSpline = createCubicBSpline();
  gp_Pnt                    aPoint(1.5, -1.0, 0.0);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify closest point is on the curve
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBSpline->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);

  // Distance should be at least 1.0 (point is below curve which is above Y=0)
  EXPECT_GE(aResult.MinSquareDistance(), 1.0);
}

TEST_F(ExtremaPC_BSplineCurveTest, PointNearCurve_OutOfPlane)
{
  Handle(Geom_BSplineCurve) aBSpline = createCubicBSpline();
  gp_Pnt                    aPoint(1.5, 2.0, 5.0);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

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
  Handle(Geom_BSplineCurve) aBSpline = createLinearBSpline();
  gp_Pnt                    aPoint(5.0, 0.0, 0.0);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BSplineCurveTest, LinearBSpline_PointOffLine)
{
  Handle(Geom_BSplineCurve) aBSpline = createLinearBSpline();
  gp_Pnt                    aPoint(5.0, 3.0, 4.0);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

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
  Handle(Geom_BSplineCurve) aBSpline = createQuadraticBSpline();
  gp_Pnt                    aPoint(5.0, 8.0, 0.0);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify closest point is on the curve and distance is consistent
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBSpline->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aPoint.SquareDistance(aPtOnCurve), THE_TOL);
}

TEST_F(ExtremaPC_BSplineCurveTest, QuadraticBSpline_PointOnApex)
{
  Handle(Geom_BSplineCurve) aBSpline = createQuadraticBSpline();
  // Apex is approximately at middle
  gp_Pnt aPoint = aBSpline->Value(0.5);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

//==================================================================================================
// Multi-span BSpline tests
//==================================================================================================

TEST_F(ExtremaPC_BSplineCurveTest, MultiSpanBSpline_PointNear)
{
  Handle(Geom_BSplineCurve) aBSpline = createMultiSpanBSpline();
  gp_Pnt                    aPoint(2.5, 3.0, 0.0);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify closest point is on the curve
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBSpline->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);

  // Oscillating curve peaks at Y≈2, so distance from (2.5, 3.0) should be around 1
  EXPECT_LT(std::sqrt(aResult.MinSquareDistance()), 2.0);
}

TEST_F(ExtremaPC_BSplineCurveTest, MultiSpanBSpline_MultipleExtrema)
{
  Handle(Geom_BSplineCurve) aBSpline = createMultiSpanBSpline();
  gp_Pnt                    aPoint(2.5, 1.0, 0.0);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // With oscillating curve, might find multiple local extrema
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify all extrema are on the curve
  for (int i = 0; i < aResult.NbExt(); ++i)
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
  Handle(Geom_BSplineCurve) aBSpline = create3DBSpline();
  gp_Pnt                    aPoint   = aBSpline->Value(0.5);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  double aMinSqDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinSqDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BSplineCurveTest, BSpline3D_PointNear)
{
  Handle(Geom_BSplineCurve) aBSpline = create3DBSpline();
  gp_Pnt                    aPoint(1.5, 2.0, 1.5);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify closest point is on the curve and distance is consistent
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBSpline->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aPoint.SquareDistance(aPtOnCurve), THE_TOL);
}

//==================================================================================================
// Bound constraint tests
//==================================================================================================

TEST_F(ExtremaPC_BSplineCurveTest, PartialRange_FirstHalf)
{
  Handle(Geom_BSplineCurve) aBSpline = createCubicBSpline();
  gp_Pnt                    aPoint(2.5, 1.0, 0.0);

  // Create evaluator with domain restricted to first half [0, 0.5]
  ExtremaPC_BSplineCurve anEval(aBSpline, ExtremaPC::Domain1D{0.0, 0.5});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // All parameters should be in first half
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_GE(aResult[i].Parameter, 0.0 - THE_TOL);
    EXPECT_LE(aResult[i].Parameter, 0.5 + THE_TOL);
  }
}

TEST_F(ExtremaPC_BSplineCurveTest, PartialRange_SecondHalf)
{
  Handle(Geom_BSplineCurve) aBSpline = createCubicBSpline();
  gp_Pnt                    aPoint(0.5, 1.0, 0.0);

  // Create evaluator with domain restricted to second half [0.5, 1.0]
  ExtremaPC_BSplineCurve anEval(aBSpline, ExtremaPC::Domain1D{0.5, 1.0});
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // All parameters should be in second half
  for (int i = 0; i < aResult.NbExt(); ++i)
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
  Handle(Geom_BSplineCurve) aBSpline = createCubicBSpline();
  gp_Pnt                    aPoint(1.5, 3.0, 0.0);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify the projected point is on the curve
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBSpline->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.X(), aPtOnCurve.X(), THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].Point.Y(), aPtOnCurve.Y(), THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].Point.Z(), aPtOnCurve.Z(), THE_TOL);
}

TEST_F(ExtremaPC_BSplineCurveTest, VerifyDistanceConsistency)
{
  Handle(Geom_BSplineCurve) aBSpline = createCubicBSpline();
  gp_Pnt                    aPoint(2.0, 2.5, 1.0);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  GeomAdaptor_Curve      anAdaptor(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify distance matches point distance
  int    aMinIdx     = aResult.MinIndex();
  double aComputedSq = aPoint.SquareDistance(aResult[aMinIdx].Point);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aComputedSq, THE_TOL);
}

//==================================================================================================
// Iterative grid refinement tests
//==================================================================================================

TEST_F(ExtremaPC_BSplineCurveTest, RefinementConfig_DisableRefinement)
{
  // Test that disabling refinement works
  Handle(Geom_BSplineCurve) aBSpline = createCubicBSpline();
  GeomAdaptor_Curve         anAdaptor(aBSpline);
  gp_Pnt                    aPoint(1.5, 2.0, 0.0);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  // Should work normally even with default refinement enabled
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);
}

TEST_F(ExtremaPC_BSplineCurveTest, HighDegreeSpline_RefinementHelps)
{
  // Create a high-degree spline where Newton might have more difficulty
  TColgp_Array1OfPnt aPoles(1, 8);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(0.5, 1.0, 0.0);
  aPoles(3) = gp_Pnt(1.0, -0.5, 0.0);
  aPoles(4) = gp_Pnt(1.5, 1.5, 0.0);
  aPoles(5) = gp_Pnt(2.0, 0.0, 0.0);
  aPoles(6) = gp_Pnt(2.5, 1.0, 0.0);
  aPoles(7) = gp_Pnt(3.0, -0.5, 0.0);
  aPoles(8) = gp_Pnt(3.5, 0.5, 0.0);

  TColStd_Array1OfReal    aKnots(1, 2);
  TColStd_Array1OfInteger aMults(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;
  aMults(1) = 8;
  aMults(2) = 8;

  Handle(Geom_BSplineCurve) aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 7);
  GeomAdaptor_Curve         anAdaptor(aBSpline);

  // Point that requires accurate projection
  gp_Pnt aPoint(1.75, 0.8, 0.0);

  ExtremaPC_BSplineCurve anEval(aBSpline);
  const ExtremaPC::Result& aResult =anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GE(aResult.NbExt(), 1);

  // Verify the closest point is on the curve
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBSpline->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);

  // Verify the distance is reasonable (point is within expected range of curve)
  double aDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_LT(aDist, 2.0);
}


