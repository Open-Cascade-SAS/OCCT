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
#include <ExtremaPC_GridEvaluator.hxx>
#include <ExtremaPC2d_BezierCurve.hxx>

#include <Geom_BezierCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
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
  for (size_t i = 0; i < aResult.NbExt(); ++i)
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
  size_t aMinIdx    = aResult.MinIndex();
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
  size_t aMinIdx    = aResult.MinIndex();
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
  size_t aMinIdx    = aResult.MinIndex();
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
  for (size_t i = 0; i < aResult.NbExt(); ++i)
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
  for (size_t i = 0; i < aResult.NbExt(); ++i)
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
  for (size_t i = 0; i < aResult.NbExt(); ++i)
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

  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 1.0, 0.01);
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
  size_t aMinIdx    = aResult.MinIndex();
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
  size_t aMinIdx    = aResult.MinIndex();
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
  for (size_t i = 0; i < aResult.NbExt(); ++i)
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

  ASSERT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 1.0, THE_TOL);
}

TEST_F(ExtremaPC_BezierCurveTest, SingletonDomain_ReturnsSolePoint)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createQuadraticBezier(gp_Pnt(0, 0, 0), gp_Pnt(1, 2, 0), gp_Pnt(3, 0, 0));
  constexpr double aParameter = 0.4;

  ExtremaPC_BezierCurve    anEval(aBezier, ExtremaPC::Domain1D{aParameter, aParameter});
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(gp_Pnt(5, 5, 0), THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].Parameter, aParameter, THE_TOL);
  EXPECT_TRUE(aResult[0].IsMinimum);
  EXPECT_TRUE(aResult[0].IsMaximum);
}

TEST_F(ExtremaPC_BezierCurveTest, RepeatedEndpointPole_DoesNotFailNumerically)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 0), gp_Pnt(2, 1, 0), gp_Pnt(3, 0, 0));

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(gp_Pnt(0, 0, 0), THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_GT(aResult.NbExt(), 0);
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST_F(ExtremaPC_BezierCurveTest, ClosedCurve_SeamHasSingleRepresentative)
{
  occ::handle<Geom_BezierCurve> aBezier =
    createCubicBezier(gp_Pnt(1, 0, 0), gp_Pnt(1, 1, 0), gp_Pnt(1, -1, 0), gp_Pnt(1, 0, 0));

  ExtremaPC_BezierCurve    anEval(aBezier);
  const ExtremaPC::Result& aResult = anEval.PerformWithEndpoints(gp_Pnt(2, 0, 0), THE_TOL);

  ASSERT_TRUE(aResult.IsDone());
  int aNbSeamRepresentatives = 0;
  for (size_t anIndex = 0; anIndex < aResult.NbExt(); ++anIndex)
  {
    if (std::abs(aResult[anIndex].Parameter - aBezier->FirstParameter()) <= THE_TOL
        || std::abs(aResult[anIndex].Parameter - aBezier->LastParameter()) <= THE_TOL)
    {
      ++aNbSeamRepresentatives;
    }
  }
  EXPECT_EQ(aNbSeamRepresentatives, 1);
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
  size_t aMinIdx    = aResult.MinIndex();
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
  for (size_t i = 0; i < aResult.NbExt(); ++i)
  {
    double aU = aResult[i].Parameter;

    // Skip boundary points where derivative condition may not hold
    // Use generous tolerance since boundary extrema are valid but don't satisfy F=0
    if (aU < 0.01 || aU > 0.99)
    {
      continue;
    }

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

  for (size_t i = 0; i < aResult.NbExt(); ++i)
  {
    double aSqDist  = aResult[i].SquareDistance;
    double aU       = aResult[i].Parameter;
    gp_Pnt aCurvePt = aBezier->Value(aU);

    double aExpectedSqDist = aPoint.SquareDistance(aCurvePt);
    EXPECT_NEAR(aSqDist, aExpectedSqDist, THE_TOL);
  }
}

//=================================================================================================

TEST_F(ExtremaPC_BezierCurveTest, PlanarDelegation_PreservesOffPlaneExtrema)
{
  NCollection_Array1<gp_Pnt>   aPoles3d(1, 4);
  NCollection_Array1<gp_Pnt2d> aPoles2d(1, 4);
  for (size_t anIndex = 0; anIndex < aPoles3d.Size(); ++anIndex)
  {
    const size_t anOrdinal     = anIndex + 1;
    const double anX           = static_cast<double>(anIndex);
    const double anY           = anOrdinal == 2 ? 3.0 : (anOrdinal == 3 ? -1.0 : 0.0);
    aPoles3d.ChangeAt(anIndex) = gp_Pnt(anX, anY, 2.0);
    aPoles2d.ChangeAt(anIndex) = gp_Pnt2d(anX, anY);
  }
  occ::handle<Geom_BezierCurve>   aCurve3d = new Geom_BezierCurve(aPoles3d);
  occ::handle<Geom2d_BezierCurve> aCurve2d = new Geom2d_BezierCurve(aPoles2d);
  ExtremaPC_BezierCurve           anEvaluator3d(aCurve3d);
  ExtremaPC2d_BezierCurve         anEvaluator2d(aCurve2d);
  const ExtremaPC::Result&        aResult3d = anEvaluator3d.Perform(gp_Pnt(1.3, 1.0, 7.0), THE_TOL);
  const ExtremaPC2d::Result&      aResult2d = anEvaluator2d.Perform(gp_Pnt2d(1.3, 1.0), THE_TOL);
  ASSERT_EQ(aResult3d.NbExt(), aResult2d.NbExt());
  for (size_t anIndex = 0; anIndex < aResult3d.NbExt(); ++anIndex)
  {
    EXPECT_NEAR(aResult3d[anIndex].Parameter, aResult2d[anIndex].Parameter, THE_TOL);
    EXPECT_NEAR(aResult3d[anIndex].SquareDistance,
                aResult2d[anIndex].SquareDistance + 25.0,
                THE_TOL);
  }
}

//=================================================================================================

TEST_F(ExtremaPC_BezierCurveTest, PlanarDelegation_ReprojectsMutationPerCall)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1)                            = gp_Pnt(0.0, 0.0, 1.0);
  aPoles(2)                            = gp_Pnt(2.0, 3.0, 1.0);
  aPoles(3)                            = gp_Pnt(4.0, 0.0, 1.0);
  occ::handle<Geom_BezierCurve> aCurve = new Geom_BezierCurve(aPoles);
  ExtremaPC_BezierCurve         anEvaluator(aCurve);
  const ExtremaPC::Result&      aBefore = anEvaluator.Perform(gp_Pnt(2.0, 2.0, 4.0), THE_TOL);
  ASSERT_TRUE(aBefore.IsDone());
  const double aBeforeParameter = aBefore[aBefore.MinIndex()].Parameter;

  aCurve->SetPole(2, gp_Pnt(3.5, -4.0, 1.0));
  const ExtremaPC::Result& anAfter = anEvaluator.Perform(gp_Pnt(2.0, 2.0, 4.0), THE_TOL);
  ASSERT_TRUE(anAfter.IsDone());
  ASSERT_GE(anAfter.NbExt(), 1);
  EXPECT_GT(std::abs(anAfter[anAfter.MinIndex()].Parameter - aBeforeParameter), 1.0e-3);
  EXPECT_NEAR(anAfter[anAfter.MinIndex()].Point.Distance(
                aCurve->Value(anAfter[anAfter.MinIndex()].Parameter)),
              0.0,
              1.0e-12);
}

TEST_F(ExtremaPC_BezierCurveTest, SmallNonConstantCurveIsNotPromotedToInfinite)
{
  constexpr int              THE_NB_POLES = 12;
  constexpr double           aLength      = 1.0e-3;
  NCollection_Array1<gp_Pnt> aPoles(1, THE_NB_POLES);
  for (size_t anIndex = 0; anIndex < aPoles.Size(); ++anIndex)
  {
    const double aRatio      = static_cast<double>(anIndex) / (aPoles.Size() - 1);
    aPoles.ChangeAt(anIndex) = gp_Pnt(aRatio * aLength, 0.0, 0.0);
  }
  occ::handle<Geom_BezierCurve> aCurve = new Geom_BezierCurve(aPoles);
  GeomAdaptor_Curve             anAdaptor(aCurve);
  ExtremaPC_GridEvaluator       anEvaluator;
  anEvaluator.SetParams(ExtremaPC_GridEvaluator::BuildUniformParams(0.0, 1.0, 8));

  const ExtremaPC::Result& aResult = anEvaluator.Perform(anAdaptor,
                                                         gp_Pnt(0.5 * aLength, 1.0, 0.0),
                                                         {0.0, 1.0},
                                                         1.0e-10,
                                                         ExtremaPC::SearchMode::MinMax);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_FALSE(aResult.IsInfinite());
  ASSERT_GE(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[aResult.MinIndex()].Parameter, 0.5, 1.0e-7);
}
