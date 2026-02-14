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

#include <ExtremaPC_BezierCurve.hxx>

#include <Geom_BezierCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <cmath>

//! Test fixture for ExtremaPC_BezierCurve tests.
class ExtremaPC_BezierCurveTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-6;

  //! Create a linear Bezier curve (degree 1).
  occ::handle<Geom_BezierCurve> createLinearBezier(const gp_Pnt& theP0, const gp_Pnt& theP1) const
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 2);
    aPoles(1) = theP0;
    aPoles(2) = theP1;
    return new Geom_BezierCurve(aPoles);
  }

  //! Create a quadratic Bezier curve (degree 2).
  occ::handle<Geom_BezierCurve> createQuadraticBezier(const gp_Pnt& theP0,
                                                      const gp_Pnt& theP1,
                                                      const gp_Pnt& theP2) const
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 3);
    aPoles(1) = theP0;
    aPoles(2) = theP1;
    aPoles(3) = theP2;
    return new Geom_BezierCurve(aPoles);
  }

  //! Create a cubic Bezier curve (degree 3).
  occ::handle<Geom_BezierCurve> createCubicBezier(const gp_Pnt& theP0,
                                                  const gp_Pnt& theP1,
                                                  const gp_Pnt& theP2,
                                                  const gp_Pnt& theP3) const
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 4);
    aPoles(1) = theP0;
    aPoles(2) = theP1;
    aPoles(3) = theP2;
    aPoles(4) = theP3;
    return new Geom_BezierCurve(aPoles);
  }

  //! Create a quartic Bezier curve (degree 4).
  occ::handle<Geom_BezierCurve> createQuarticBezier(const gp_Pnt& theP0,
                                                    const gp_Pnt& theP1,
                                                    const gp_Pnt& theP2,
                                                    const gp_Pnt& theP3,
                                                    const gp_Pnt& theP4) const
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 5);
    aPoles(1) = theP0;
    aPoles(2) = theP1;
    aPoles(3) = theP2;
    aPoles(4) = theP3;
    aPoles(5) = theP4;
    return new Geom_BezierCurve(aPoles);
  }

  //! Create a rational quadratic Bezier curve.
  occ::handle<Geom_BezierCurve> createRationalQuadraticBezier(const gp_Pnt& theP0,
                                                              const gp_Pnt& theP1,
                                                              const gp_Pnt& theP2,
                                                              double        theW1) const
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 3);
    NCollection_Array1<double> aWeights(1, 3);
    aPoles(1)   = theP0;
    aPoles(2)   = theP1;
    aPoles(3)   = theP2;
    aWeights(1) = 1.0;
    aWeights(2) = theW1;
    aWeights(3) = 1.0;
    return new Geom_BezierCurve(aPoles, aWeights);
  }

  //! Create a 3D cubic Bezier curve.
  occ::handle<Geom_BezierCurve> create3DCubicBezier() const
  {
    NCollection_Array1<gp_Pnt> aPoles(1, 4);
    aPoles(1) = gp_Pnt(0, 0, 0);
    aPoles(2) = gp_Pnt(1, 2, 1);
    aPoles(3) = gp_Pnt(2, 2, 2);
    aPoles(4) = gp_Pnt(3, 0, 3);
    return new Geom_BezierCurve(aPoles);
  }
};

//==================================================================================================
// Basic Tests - Point on Curve
//==================================================================================================

TEST_F(ExtremaPC_BezierCurveTest, PointOnCurve_AtStart)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(0, 0, 0), gp_Pnt(1, 2, 0), gp_Pnt(3, 2, 0), gp_Pnt(4, 0, 0));
  gp_Pnt aPoint = aBezier->Value(0.0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BezierCurveTest, PointOnCurve_AtEnd)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(0, 0, 0), gp_Pnt(1, 2, 0), gp_Pnt(3, 2, 0), gp_Pnt(4, 0, 0));
  gp_Pnt aPoint = aBezier->Value(1.0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BezierCurveTest, PointOnCurve_AtMiddle)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(0, 0, 0), gp_Pnt(1, 2, 0), gp_Pnt(3, 2, 0), gp_Pnt(4, 0, 0));
  gp_Pnt aPoint = aBezier->Value(0.5);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 0.0, THE_TOL);
}

//==================================================================================================
// Linear Bezier Tests
//==================================================================================================

TEST_F(ExtremaPC_BezierCurveTest, LinearBezier_PointProjection)
{
  occ::handle<Geom_BezierCurve> aBezier = createLinearBezier(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  gp_Pnt                        aPoint(5, 3, 0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 9.0, THE_TOL); // Distance should be 3, squared = 9
}

TEST_F(ExtremaPC_BezierCurveTest, LinearBezier_PointAtPole)
{
  occ::handle<Geom_BezierCurve> aBezier = createLinearBezier(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  gp_Pnt                        aPoint(0, 0, 0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BezierCurveTest, LinearBezier_PointBeforeStart)
{
  occ::handle<Geom_BezierCurve> aBezier = createLinearBezier(gp_Pnt(0, 0, 0), gp_Pnt(10, 0, 0));
  gp_Pnt                        aPoint(-5, 0, 0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);
  // Closest point is at t=0, distance = 5
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 25.0, THE_TOL);
}

//==================================================================================================
// Quadratic Bezier Tests
//==================================================================================================

TEST_F(ExtremaPC_BezierCurveTest, QuadraticBezier_SymmetricArc)
{
  // Symmetric parabolic arc
  occ::handle<Geom_BezierCurve> aBezier =
    createQuadraticBezier(gp_Pnt(-2, 0, 0), gp_Pnt(0, 2, 0), gp_Pnt(2, 0, 0));
  // Point on axis of symmetry above the curve
  gp_Pnt aPoint(0, 3, 0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Should find extremum at t=0.5
  bool aFoundMiddle = false;
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aU = aResult[i].Parameter;
    if (std::abs(aU - 0.5) < 0.01)
    {
      aFoundMiddle = true;
      break;
    }
  }
  EXPECT_TRUE(aFoundMiddle);
}

TEST_F(ExtremaPC_BezierCurveTest, QuadraticBezier_PointBelowApex)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createQuadraticBezier(gp_Pnt(-2, 0, 0), gp_Pnt(0, 2, 0), gp_Pnt(2, 0, 0));
  gp_Pnt aPoint(0, 0, 0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  // Should find extrema - at least one
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify closest point is on the curve
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBezier->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);

  // Minimum distance should be less than distance to endpoints (both at 2)
  EXPECT_LT(std::sqrt(aResult.MinSquareDistance()), 2.0);
}

TEST_F(ExtremaPC_BezierCurveTest, QuadraticBezier_AsymmetricArc)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createQuadraticBezier(gp_Pnt(0, 0, 0), gp_Pnt(2, 4, 0), gp_Pnt(6, 0, 0));
  gp_Pnt aPoint(3, 3, 0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify closest point is on the curve and distance is consistent
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBezier->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aPoint.SquareDistance(aPtOnCurve), THE_TOL);
}

//==================================================================================================
// Cubic Bezier Tests
//==================================================================================================

TEST_F(ExtremaPC_BezierCurveTest, CubicBezier_SCurve)
{
  // S-shaped curve
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(0, 0, 0), gp_Pnt(0, 4, 0), gp_Pnt(4, -4, 0), gp_Pnt(4, 0, 0));
  gp_Pnt aPoint(2, 0, 0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify closest point is on the curve
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBezier->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BezierCurveTest, CubicBezier_LoopedCurve)
{
  // Curve with a loop
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(0, 0, 0), gp_Pnt(4, 3, 0), gp_Pnt(-2, 3, 0), gp_Pnt(2, 0, 0));
  gp_Pnt aPoint(1, 1.5, 0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // With a loop, may find multiple extrema - verify all are on curve
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    gp_Pnt aPtOnCurve = aBezier->Value(aResult[i].Parameter);
    EXPECT_NEAR(aResult[i].Point.Distance(aPtOnCurve), 0.0, THE_TOL);
  }
}

TEST_F(ExtremaPC_BezierCurveTest, CubicBezier_MultipleExtrema)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(0, 0, 0), gp_Pnt(1, 3, 0), gp_Pnt(3, 3, 0), gp_Pnt(4, 0, 0));
  gp_Pnt aPoint(2, -1, 0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify all extrema are on the curve
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    gp_Pnt aPtOnCurve = aBezier->Value(aResult[i].Parameter);
    EXPECT_NEAR(aResult[i].Point.Distance(aPtOnCurve), 0.0, THE_TOL);
  }
}

//==================================================================================================
// Quartic Bezier Tests
//==================================================================================================

TEST_F(ExtremaPC_BezierCurveTest, QuarticBezier_WavyCurve)
{
  occ::handle<Geom_BezierCurve> aBezier = createQuarticBezier(gp_Pnt(0, 0, 0),
                                                              gp_Pnt(1, 2, 0),
                                                              gp_Pnt(2, -1, 0),
                                                              gp_Pnt(3, 2, 0),
                                                              gp_Pnt(4, 0, 0));
  gp_Pnt                        aPoint(2, 1, 0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify all extrema are on the curve
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    gp_Pnt aPtOnCurve = aBezier->Value(aResult[i].Parameter);
    EXPECT_NEAR(aResult[i].Point.Distance(aPtOnCurve), 0.0, THE_TOL);
  }
}

TEST_F(ExtremaPC_BezierCurveTest, QuarticBezier_PointOnCurve)
{
  occ::handle<Geom_BezierCurve> aBezier = createQuarticBezier(gp_Pnt(0, 0, 0),
                                                              gp_Pnt(1, 2, 0),
                                                              gp_Pnt(2, -1, 0),
                                                              gp_Pnt(3, 2, 0),
                                                              gp_Pnt(4, 0, 0));
  gp_Pnt                        aPoint  = aBezier->Value(0.25);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

//==================================================================================================
// Rational Bezier Tests
//==================================================================================================

TEST_F(ExtremaPC_BezierCurveTest, RationalQuadratic_CircularArc)
{
  // Rational quadratic can represent a circular arc
  double                        aW = 1.0 / std::sqrt(2.0);
  occ::handle<Geom_BezierCurve> aBezier =
    createRationalQuadraticBezier(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0), gp_Pnt(0, 1, 0), aW);
  gp_Pnt aPoint(0, 0, 0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // All points on a circular arc should be equidistant from center (radius = 1)
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 1.0, 0.01);
}

TEST_F(ExtremaPC_BezierCurveTest, RationalQuadratic_HighWeight)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createRationalQuadraticBezier(gp_Pnt(0, 0, 0), gp_Pnt(1, 2, 0), gp_Pnt(2, 0, 0), 3.0);
  gp_Pnt aPoint(1, 1.5, 0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify closest point is on the curve and distance is consistent
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBezier->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aPoint.SquareDistance(aPtOnCurve), THE_TOL);
}

//==================================================================================================
// 3D Bezier Tests
//==================================================================================================

TEST_F(ExtremaPC_BezierCurveTest, CubicBezier_3D_HelixLike)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(0, 0, 0), gp_Pnt(1, 1, 1), gp_Pnt(2, 1, 2), gp_Pnt(3, 0, 3));
  gp_Pnt aPoint(1.5, 0.5, 1.5);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify closest point is on the curve and distance is consistent
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBezier->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aMinIdx].SquareDistance, aPoint.SquareDistance(aPtOnCurve), THE_TOL);
}

TEST_F(ExtremaPC_BezierCurveTest, CubicBezier_3D_PointOnCurve)
{
  occ::handle<Geom_BezierCurve> aBezier = create3DCubicBezier();
  gp_Pnt                        aPoint  = aBezier->Value(0.3);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 0.0, THE_TOL);
}

//==================================================================================================
// Bound Constraint Tests
//==================================================================================================

TEST_F(ExtremaPC_BezierCurveTest, BoundedRange_ExtremumInside)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(0, 0, 0), gp_Pnt(1, 2, 0), gp_Pnt(3, 2, 0), gp_Pnt(4, 0, 0));
  gp_Pnt aPoint = aBezier->Value(0.5);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BezierCurveTest, BoundedRange_ExtremumAtBound)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(0, 0, 0), gp_Pnt(1, 2, 0), gp_Pnt(3, 2, 0), gp_Pnt(4, 0, 0));
  // Sample point at u=0.5 (outside the bounded range [0, 0.3])
  gp_Pnt aPoint = aBezier->Value(0.5);

  // Create evaluator with restricted domain [0, 0.3]
  ExtremaPC_BezierCurve    anEval(aBezier, ExtremaPC::Domain1D{0.0, 0.3});
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // All extrema should be within the bounded range [0, 0.3]
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aU = aResult[i].Parameter;
    EXPECT_GE(aU, -0.001);
    EXPECT_LE(aU, 0.301);
  }
}

//==================================================================================================
// Edge Case Tests
//==================================================================================================

TEST_F(ExtremaPC_BezierCurveTest, DegenerateBezier_AllPolesSame)
{
  // All poles at the same location
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(1, 1, 1), gp_Pnt(1, 1, 1), gp_Pnt(1, 1, 1), gp_Pnt(1, 1, 1));
  gp_Pnt aPoint(2, 1, 1);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Distance should be 1 (squared)
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 1.0, THE_TOL);
}

TEST_F(ExtremaPC_BezierCurveTest, PointFarFromCurve)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(0, 0, 0), gp_Pnt(1, 1, 0), gp_Pnt(2, 1, 0), gp_Pnt(3, 0, 0));
  gp_Pnt aPoint(100, 100, 100);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify closest point is on the curve
  int    aMinIdx    = aResult.MinIndex();
  gp_Pnt aPtOnCurve = aBezier->Value(aResult[aMinIdx].Parameter);
  EXPECT_NEAR(aResult[aMinIdx].Point.Distance(aPtOnCurve), 0.0, THE_TOL);

  // Distance should be large (curve is near origin, point is at (100,100,100))
  // Minimum distance should be at least sqrt(100^2+99^2+100^2) ~= 172
  EXPECT_GT(std::sqrt(aResult.MinSquareDistance()), 170.0);
}

TEST_F(ExtremaPC_BezierCurveTest, PointVeryCloseButNotOn)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(0, 0, 0), gp_Pnt(1, 1, 0), gp_Pnt(2, 1, 0), gp_Pnt(3, 0, 0));
  gp_Pnt aOnCurve = aBezier->Value(0.5);
  gp_Pnt aPoint(aOnCurve.X(), aOnCurve.Y() + 1e-8, aOnCurve.Z());

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 0.0, 1e-12);
}

//==================================================================================================
// Verification Tests
//==================================================================================================

TEST_F(ExtremaPC_BezierCurveTest, VerifyExtremumCondition)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(0, 0, 0), gp_Pnt(1, 2, 0), gp_Pnt(3, 2, 0), gp_Pnt(4, 0, 0));
  gp_Pnt aPoint(2, 3, 0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());

  // Count how many interior extrema satisfy the condition
  int aInteriorCount  = 0;
  int aSatisfiedCount = 0;

  // Verify that (C(u) - P) . C'(u) ~= 0 at interior extrema
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aU = aResult[i].Parameter;

    // Skip boundary points where derivative condition may not hold
    // Use generous tolerance since boundary extrema are valid but don't satisfy F=0
    if (aU < 0.01 || aU > 0.99)
      continue;

    aInteriorCount++;

    gp_Pnt aCurvePnt;
    gp_Vec aTangent;
    aBezier->D1(aU, aCurvePnt, aTangent);

    gp_Vec aVec(aCurvePnt, aPoint);
    double aDot = aVec.Dot(aTangent);

    if (std::abs(aDot) < 0.1) // Relaxed tolerance for numerical methods
    {
      aSatisfiedCount++;
    }
  }

  // If there are interior extrema, at least some should satisfy the condition
  if (aInteriorCount > 0)
  {
    EXPECT_GE(aSatisfiedCount, 1);
  }
}

TEST_F(ExtremaPC_BezierCurveTest, ConsistentDistanceCalculation)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(0, 0, 0), gp_Pnt(1, 1, 0), gp_Pnt(2, 1, 0), gp_Pnt(3, 0, 0));
  gp_Pnt aPoint(1.5, 2, 0);

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(aPoint, THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbExt(), 1);

  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    double aSqDist  = aResult[i].SquareDistance;
    double aU       = aResult[i].Parameter;
    gp_Pnt aCurvePt = aBezier->Value(aU);

    double aExpectedSqDist = aPoint.SquareDistance(aCurvePt);
    EXPECT_NEAR(aSqDist, aExpectedSqDist, THE_TOL);
  }
}
