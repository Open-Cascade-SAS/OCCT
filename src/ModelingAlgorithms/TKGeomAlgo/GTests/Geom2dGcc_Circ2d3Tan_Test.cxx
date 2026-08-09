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

#include <Geom2dGcc_Circ2d3Tan.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2dGcc.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <Precision.hxx>
#include <NCollection_Array1.hxx>

#include <gtest/gtest.h>

namespace
{
double circleLength(const gp_Circ2d& theCircle)
{
  const occ::handle<Geom2d_Circle> aCurve = new Geom2d_Circle(theCircle);
  Geom2dAdaptor_Curve              anAdaptor(aCurve);
  return GCPnts_AbscissaPoint::Length(anAdaptor);
}

} // namespace

// Test fixture for Geom2dGcc_Circ2d3Tan testing
class Geom2dGcc_Circ2d3TanTest : public ::testing::Test
{
protected:
  void SetUp() override { myTolerance = Precision::Confusion(); }

  double myTolerance;

  //! Verify that a solution is tangent to all three input circles
  void verifyTangencyConstraints(const gp_Circ2d& theSolution,
                                 const gp_Circ2d& theCircle1,
                                 const gp_Circ2d& theCircle2,
                                 const gp_Circ2d& theCircle3,
                                 const int        theSolutionIndex,
                                 const double     theTolerance = 1e-6) const
  {
    gp_Pnt2d aSolCenter = theSolution.Location();
    double   aSolRadius = theSolution.Radius();

    // Check tangency to each input circle
    for (int i = 1; i <= 3; i++)
    {
      const gp_Circ2d* aCircle     = (i == 1) ? &theCircle1 : (i == 2) ? &theCircle2 : &theCircle3;
      gp_Pnt2d         aCircCenter = aCircle->Location();
      double           aCircRadius = aCircle->Radius();

      double aDistanceCenters = aSolCenter.Distance(aCircCenter);

      // For tangency: distance = |R1 - R2| (internal) or R1 + R2 (external)
      double aExpectedDistExt = aSolRadius + aCircRadius;
      double aExpectedDistInt = std::abs(aSolRadius - aCircRadius);

      bool isTangent = (std::abs(aDistanceCenters - aExpectedDistExt) <= theTolerance)
                       || (std::abs(aDistanceCenters - aExpectedDistInt) <= theTolerance);

      EXPECT_TRUE(isTangent) << "Solution " << theSolutionIndex << " should be tangent to circle "
                             << i << " (distance=" << aDistanceCenters
                             << ", expected external=" << aExpectedDistExt
                             << ", expected internal=" << aExpectedDistInt << ")";
    }
  }

  //! Create a qualified curve from circle parameters
  Geom2dGcc_QualifiedCurve createQualifiedCircle(const double theX,
                                                 const double theY,
                                                 const double theRadius) const
  {
    gp_Pnt2d                   aCenter(theX, theY);
    occ::handle<Geom2d_Circle> aCircle =
      new Geom2d_Circle(gp_Circ2d(gp_Ax2d(aCenter, gp_Dir2d(1, 0)), theRadius));
    Geom2dAdaptor_Curve anAdaptor(aCircle);
    return Geom2dGcc::Unqualified(anAdaptor);
  }

  //! Verify solution validity
  void verifySolutionValidity(const gp_Circ2d& theSolution,
                              const int        theSolutionIndex,
                              const double     theMinX      = -10000.0,
                              const double     theMaxX      = 10000.0,
                              const double     theMinY      = -10000.0,
                              const double     theMaxY      = 10000.0,
                              const double     theMaxRadius = 100000.0) const
  {
    EXPECT_GT(theSolution.Radius(), 0)
      << "Solution " << theSolutionIndex << " should have positive radius";
    EXPECT_LT(theSolution.Radius(), theMaxRadius)
      << "Solution " << theSolutionIndex << " should have reasonable radius";

    gp_Pnt2d aCenter = theSolution.Location();
    EXPECT_GT(aCenter.X(), theMinX)
      << "Solution " << theSolutionIndex << " X coordinate should be reasonable";
    EXPECT_LT(aCenter.X(), theMaxX)
      << "Solution " << theSolutionIndex << " X coordinate should be reasonable";
    EXPECT_GT(aCenter.Y(), theMinY)
      << "Solution " << theSolutionIndex << " Y coordinate should be reasonable";
    EXPECT_LT(aCenter.Y(), theMaxY)
      << "Solution " << theSolutionIndex << " Y coordinate should be reasonable";
  }
};

// Test the exact BUC60622 regression case
TEST_F(Geom2dGcc_Circ2d3TanTest, BUC60622_RegressionCase)
{
  // BUC60622 test case: 3 circles that should produce 3 tangent solutions
  // circle c1 500 1800 500
  // circle c2 500 1900 400
  // circle c3 700 1900 200

  Geom2dGcc_QualifiedCurve aQual1 = createQualifiedCircle(500.0, 1800.0, 500.0);
  Geom2dGcc_QualifiedCurve aQual2 = createQualifiedCircle(500.0, 1900.0, 400.0);
  Geom2dGcc_QualifiedCurve aQual3 = createQualifiedCircle(700.0, 1900.0, 200.0);

  // Input circles for tangency verification
  gp_Circ2d aInputCirc1 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(500.0, 1800.0), gp_Dir2d(1, 0)), 500.0);
  gp_Circ2d aInputCirc2 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(500.0, 1900.0), gp_Dir2d(1, 0)), 400.0);
  gp_Circ2d aInputCirc3 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(700.0, 1900.0), gp_Dir2d(1, 0)), 200.0);

  // Solve for tangent circles with default tolerance
  Geom2dGcc_Circ2d3Tan aTangentSolver(aQual1, aQual2, aQual3, myTolerance, 0, 0, 0);

  EXPECT_TRUE(aTangentSolver.IsDone()) << "Algorithm should succeed";

  int aNbSolutions = aTangentSolver.NbSolutions();

  // The regression test: we should find exactly 3 solutions for BUC60622 case
  EXPECT_EQ(aNbSolutions, 3) << "BUC60622 case should find exactly 3 solutions";

  // Verify each solution is geometrically valid and satisfies tangency constraints
  for (int i = 1; i <= aNbSolutions; i++)
  {
    gp_Circ2d aSol = aTangentSolver.ThisSolution(i);

    // Basic validity checks
    verifySolutionValidity(aSol, i, -1000.0, 2000.0, 1000.0, 3000.0, 10000.0);

    // Tangency constraint verification
    verifyTangencyConstraints(aSol, aInputCirc1, aInputCirc2, aInputCirc3, i);
  }

  // Verify specific known solutions for BUC60622
  if (aNbSolutions >= 3)
  {
    // Solution 1: center(500, 1900), radius=400 - this is actually one of the input circles
    gp_Circ2d aSol1 = aTangentSolver.ThisSolution(1);
    EXPECT_NEAR(aSol1.Location().X(), 500.0, 1.0);
    EXPECT_NEAR(aSol1.Location().Y(), 1900.0, 1.0);
    EXPECT_NEAR(aSol1.Radius(), 400.0, 1.0);
  }
}

// Migrated from tests/lowalgos/2dgcc/buc60622_1.  The original DRAW case
// checked that this degenerate configuration can be passed to cirtang without
// raising an exception.
TEST_F(Geom2dGcc_Circ2d3TanTest, BUC60622_1_DegenerateThreeCircleCase)
{
  const Geom2dGcc_QualifiedCurve aQual1 = createQualifiedCircle(2900.0, 800.0, 500.0);
  const Geom2dGcc_QualifiedCurve aQual2 = createQualifiedCircle(2900.0, 900.0, 400.0);
  const Geom2dGcc_QualifiedCurve aQual3 = createQualifiedCircle(2900.0, 1000.0, 300.0);

  EXPECT_NO_THROW({ Geom2dGcc_Circ2d3Tan aSolver(aQual1, aQual2, aQual3, myTolerance, 0, 0, 0); });
}

// Migrated from tests/lowalgos/2dgcc/buc60622_2.  Keep the three explicit
// solution-length checks in addition to the general regression validation.
TEST_F(Geom2dGcc_Circ2d3TanTest, BUC60622_2_AllKnownSolutionLengths)
{
  const Geom2dGcc_QualifiedCurve aQual1 = createQualifiedCircle(500.0, 1800.0, 500.0);
  const Geom2dGcc_QualifiedCurve aQual2 = createQualifiedCircle(500.0, 1900.0, 400.0);
  const Geom2dGcc_QualifiedCurve aQual3 = createQualifiedCircle(700.0, 1900.0, 200.0);
  Geom2dGcc_Circ2d3Tan           aSolver(aQual1, aQual2, aQual3, myTolerance, 0, 0, 0);

  ASSERT_TRUE(aSolver.IsDone());
  ASSERT_EQ(aSolver.NbSolutions(), 3);

  const double anExpectedLengths[] = {2513.2741228703289, 837.75804095727631, 279.2526803190928};
  for (int anIndex = 1; anIndex <= aSolver.NbSolutions(); ++anIndex)
  {
    EXPECT_NEAR(circleLength(aSolver.ThisSolution(anIndex)),
                anExpectedLengths[anIndex - 1],
                anExpectedLengths[anIndex - 1] * 1.e-8);
  }
}

// Migrated from tests/lowalgos/2dgcc/buc60622_3.  The original DRAW test
// checks the lengths of all five returned tangent circles.
TEST_F(Geom2dGcc_Circ2d3TanTest, BUC60622_3_AllTangentCircleLengths)
{
  const Geom2dGcc_QualifiedCurve aQual1 = createQualifiedCircle(625.0, 3000.0, 125.0);
  const Geom2dGcc_QualifiedCurve aQual2 = createQualifiedCircle(375.0, 3000.0, 125.0);
  const Geom2dGcc_QualifiedCurve aQual3 = createQualifiedCircle(500.0, 3000.0, 250.0);
  Geom2dGcc_Circ2d3Tan           aSolver(aQual1, aQual2, aQual3, myTolerance, 0, 0, 0);

  ASSERT_TRUE(aSolver.IsDone());
  ASSERT_EQ(aSolver.NbSolutions(), 5);

  const double anExpectedLengths[] = {1570.7963267948965,
                                      785.39816339744868,
                                      785.39816339744823,
                                      523.59877559829806,
                                      523.59877559829806};
  for (int anIndex = 1; anIndex <= aSolver.NbSolutions(); ++anIndex)
  {
    const gp_Circ2d aSolution = aSolver.ThisSolution(anIndex);
    EXPECT_NEAR(circleLength(aSolution),
                anExpectedLengths[anIndex - 1],
                anExpectedLengths[anIndex - 1] * 1.e-8);
  }
}

// Migrated from tests/lowalgos/2dgcc/buc60618.  This preserves the eight
// solution-length checks that guard the solver's solution filtering.
TEST_F(Geom2dGcc_Circ2d3TanTest, BUC60618_FilteringKeepsAllSolutions)
{
  const Geom2dGcc_QualifiedCurve aQual1 = createQualifiedCircle(0.0, 0.0, 100.0);
  const Geom2dGcc_QualifiedCurve aQual2 = createQualifiedCircle(500.0, -400.0, 300.0);
  const Geom2dGcc_QualifiedCurve aQual3 = createQualifiedCircle(300.0, 200.0, 200.0);
  Geom2dGcc_Circ2d3Tan           aSolver(aQual1, aQual2, aQual3, myTolerance, 0, 0, 0);

  ASSERT_TRUE(aSolver.IsDone());
  ASSERT_EQ(aSolver.NbSolutions(), 8);

  const double anExpectedLengths[] = {3568.9587306060739,
                                      9964.7111736227271,
                                      2082.9371204466097,
                                      32696.069757517998,
                                      1681.9298517645602,
                                      3097.3731840080009,
                                      1545.2428620020787,
                                      772.26646380656518};
  for (int anIndex = 1; anIndex <= aSolver.NbSolutions(); ++anIndex)
  {
    const gp_Circ2d aSolution = aSolver.ThisSolution(anIndex);
    EXPECT_NEAR(circleLength(aSolution),
                anExpectedLengths[anIndex - 1],
                anExpectedLengths[anIndex - 1] * 1.e-8);
  }
}

// Test with different tolerance values to demonstrate the filtering issue
TEST_F(Geom2dGcc_Circ2d3TanTest, ToleranceImpact_Analysis)
{
  // Same BUC60622 configuration
  Geom2dGcc_QualifiedCurve aQual1 = createQualifiedCircle(500.0, 1800.0, 500.0);
  Geom2dGcc_QualifiedCurve aQual2 = createQualifiedCircle(500.0, 1900.0, 400.0);
  Geom2dGcc_QualifiedCurve aQual3 = createQualifiedCircle(700.0, 1900.0, 200.0);

  // Test with different tolerance values to ensure stability
  NCollection_Array1<double> aTestTolerances(1, 4);
  aTestTolerances(1) = Precision::Confusion(); // Default ~1e-15
  aTestTolerances(2) = 1e-12;                  // Slightly larger
  aTestTolerances(3) = 1e-10;                  // Moderate
  aTestTolerances(4) = 1e-8;                   // Large

  int aDefaultSolCount = 0;

  for (int anIndex = aTestTolerances.Lower(); anIndex <= aTestTolerances.Upper(); ++anIndex)
  {
    const double         aTol = aTestTolerances(anIndex);
    Geom2dGcc_Circ2d3Tan aSolver(aQual1, aQual2, aQual3, aTol, 0, 0, 0);

    EXPECT_TRUE(aSolver.IsDone()) << "Algorithm should succeed with tolerance " << aTol;

    int aNbSol = aSolver.NbSolutions();

    if (aTol == Precision::Confusion())
    {
      aDefaultSolCount = aNbSol;
      EXPECT_GE(aNbSol, 1) << "Should find at least 1 solution with default tolerance";
    }
    else
    {
      // Solution count may vary with tolerance due to numerical precision effects
      // We allow some variation but expect reasonable bounds
      EXPECT_GE(aNbSol, 1) << "Should find at least 1 solution with tolerance " << aTol;
      EXPECT_LE(aNbSol, aDefaultSolCount + 2)
        << "Solution count shouldn't increase dramatically with tolerance " << aTol;
    }

    // Verify all solutions are valid regardless of tolerance
    for (int i = 1; i <= aNbSol; i++)
    {
      gp_Circ2d aSol = aSolver.ThisSolution(i);
      verifySolutionValidity(aSol, i, -1000.0, 2000.0, 1000.0, 3000.0, 10000.0);
    }
  }
}

// Test a simpler case that should definitely work
TEST_F(Geom2dGcc_Circ2d3TanTest, Simple_ThreeCircle_Case)
{
  // Simple case: three well-separated circles
  Geom2dGcc_QualifiedCurve aQual1 = createQualifiedCircle(0.0, 0.0, 2.0);
  Geom2dGcc_QualifiedCurve aQual2 = createQualifiedCircle(10.0, 0.0, 2.0);
  Geom2dGcc_QualifiedCurve aQual3 = createQualifiedCircle(5.0, 8.0, 2.0);

  // Input circles for verification
  gp_Circ2d aInputCirc1 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1, 0)), 2.0);
  gp_Circ2d aInputCirc2 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(10.0, 0.0), gp_Dir2d(1, 0)), 2.0);
  gp_Circ2d aInputCirc3 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(5.0, 8.0), gp_Dir2d(1, 0)), 2.0);

  Geom2dGcc_Circ2d3Tan aSolver(aQual1, aQual2, aQual3, myTolerance, 0, 0, 0);

  EXPECT_TRUE(aSolver.IsDone()) << "Simple case should always work";

  int aNbSol = aSolver.NbSolutions();
  EXPECT_GE(aNbSol, 1) << "Should find at least one solution for simple case";

  // Verify all solutions are valid and satisfy tangency constraints
  for (int i = 1; i <= aNbSol; i++)
  {
    gp_Circ2d aSol = aSolver.ThisSolution(i);
    verifySolutionValidity(aSol, i);
    verifyTangencyConstraints(aSol, aInputCirc1, aInputCirc2, aInputCirc3, i);
  }
}

// Test with concentric circles to verify edge cases
TEST_F(Geom2dGcc_Circ2d3TanTest, Concentric_Circles_EdgeCase)
{
  // Concentric circles: geometrically, this specific configuration with
  // concentric circles at (0,0) with radii 1,3 and separate circle at (10,0) radius 2
  // has no solutions - this is mathematically correct
  Geom2dGcc_QualifiedCurve aQual1 = createQualifiedCircle(0.0, 0.0, 1.0);
  Geom2dGcc_QualifiedCurve aQual2 = createQualifiedCircle(0.0, 0.0, 3.0);
  Geom2dGcc_QualifiedCurve aQual3 = createQualifiedCircle(10.0, 0.0, 2.0);

  Geom2dGcc_Circ2d3Tan aSolver(aQual1, aQual2, aQual3, myTolerance, 0, 0, 0);

  // For this specific concentric configuration, the algorithm succeeds but finds no solutions
  EXPECT_TRUE(aSolver.IsDone())
    << "Algorithm should complete successfully even when no solutions exist";

  int aNbSol = aSolver.NbSolutions();
  // This specific geometric configuration has no solutions - this is correct
  EXPECT_EQ(aNbSol, 0) << "This concentric configuration geometrically has no solutions";
}

// Test with very small circles
TEST_F(Geom2dGcc_Circ2d3TanTest, SmallCircles_PrecisionTest)
{
  // Very small circles to test precision handling
  Geom2dGcc_QualifiedCurve aQual1 = createQualifiedCircle(0.0, 0.0, 0.01);
  Geom2dGcc_QualifiedCurve aQual2 = createQualifiedCircle(0.1, 0.0, 0.01);
  Geom2dGcc_QualifiedCurve aQual3 = createQualifiedCircle(0.05, 0.08, 0.01);

  // Input circles for verification
  gp_Circ2d aInputCirc1 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1, 0)), 0.01);
  gp_Circ2d aInputCirc2 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(0.1, 0.0), gp_Dir2d(1, 0)), 0.01);
  gp_Circ2d aInputCirc3 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(0.05, 0.08), gp_Dir2d(1, 0)), 0.01);

  Geom2dGcc_Circ2d3Tan aSolver(aQual1, aQual2, aQual3, myTolerance, 0, 0, 0);

  EXPECT_TRUE(aSolver.IsDone()) << "Small circles should be handled correctly";

  int aNbSol = aSolver.NbSolutions();
  EXPECT_GE(aNbSol, 1) << "Should find at least one solution for small circles";

  for (int i = 1; i <= aNbSol; i++)
  {
    gp_Circ2d aSol = aSolver.ThisSolution(i);
    verifySolutionValidity(aSol, i, -10.0, 10.0, -10.0, 10.0, 10.0);
    verifyTangencyConstraints(aSol,
                              aInputCirc1,
                              aInputCirc2,
                              aInputCirc3,
                              i,
                              1e-3); // Relaxed tolerance for small circles
  }
}

// Test with large circles
TEST_F(Geom2dGcc_Circ2d3TanTest, LargeCircles_ScalingTest)
{
  // Large circles to test numerical scaling
  Geom2dGcc_QualifiedCurve aQual1 = createQualifiedCircle(0.0, 0.0, 1000.0);
  Geom2dGcc_QualifiedCurve aQual2 = createQualifiedCircle(5000.0, 0.0, 1500.0);
  Geom2dGcc_QualifiedCurve aQual3 = createQualifiedCircle(2500.0, 4000.0, 800.0);

  // Input circles for verification
  gp_Circ2d aInputCirc1 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1, 0)), 1000.0);
  gp_Circ2d aInputCirc2 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(5000.0, 0.0), gp_Dir2d(1, 0)), 1500.0);
  gp_Circ2d aInputCirc3 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(2500.0, 4000.0), gp_Dir2d(1, 0)), 800.0);

  Geom2dGcc_Circ2d3Tan aSolver(aQual1, aQual2, aQual3, myTolerance, 0, 0, 0);

  EXPECT_TRUE(aSolver.IsDone()) << "Large circles should be handled correctly";

  int aNbSol = aSolver.NbSolutions();
  EXPECT_GE(aNbSol, 1) << "Should find at least one solution for large circles";

  for (int i = 1; i <= aNbSol; i++)
  {
    gp_Circ2d aSol = aSolver.ThisSolution(i);
    verifySolutionValidity(aSol, i, -10000.0, 10000.0, -10000.0, 10000.0, 50000.0);
    verifyTangencyConstraints(aSol,
                              aInputCirc1,
                              aInputCirc2,
                              aInputCirc3,
                              i,
                              1.0); // Relaxed tolerance for large circles
  }
}

// Test with circles in a line configuration
TEST_F(Geom2dGcc_Circ2d3TanTest, LinearConfiguration_GeometricTest)
{
  // Three circles aligned on X-axis
  Geom2dGcc_QualifiedCurve aQual1 = createQualifiedCircle(0.0, 0.0, 1.0);
  Geom2dGcc_QualifiedCurve aQual2 = createQualifiedCircle(5.0, 0.0, 1.5);
  Geom2dGcc_QualifiedCurve aQual3 = createQualifiedCircle(10.0, 0.0, 1.2);

  // Input circles for verification
  gp_Circ2d aInputCirc1 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1, 0)), 1.0);
  gp_Circ2d aInputCirc2 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(5.0, 0.0), gp_Dir2d(1, 0)), 1.5);
  gp_Circ2d aInputCirc3 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(10.0, 0.0), gp_Dir2d(1, 0)), 1.2);

  Geom2dGcc_Circ2d3Tan aSolver(aQual1, aQual2, aQual3, myTolerance, 0, 0, 0);

  EXPECT_TRUE(aSolver.IsDone()) << "Linear configuration should be solvable";

  int aNbSol = aSolver.NbSolutions();
  EXPECT_GE(aNbSol, 1) << "Should find at least one solution for linear configuration";

  for (int i = 1; i <= aNbSol; i++)
  {
    gp_Circ2d aSol = aSolver.ThisSolution(i);
    verifySolutionValidity(aSol, i);
    verifyTangencyConstraints(aSol, aInputCirc1, aInputCirc2, aInputCirc3, i);
  }
}

// Test with touching circles
TEST_F(Geom2dGcc_Circ2d3TanTest, TouchingCircles_DegenerateCase)
{
  // Two circles that touch each other externally plus a third separate circle
  // This configuration should have solutions geometrically
  Geom2dGcc_QualifiedCurve aQual1 = createQualifiedCircle(0.0, 0.0, 2.0);
  Geom2dGcc_QualifiedCurve aQual2 = createQualifiedCircle(4.0, 0.0, 2.0); // Touch at (2,0)
  Geom2dGcc_QualifiedCurve aQual3 = createQualifiedCircle(2.0, 5.0, 1.5);

  // Input circles for verification
  gp_Circ2d aInputCirc1 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1, 0)), 2.0);
  gp_Circ2d aInputCirc2 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(4.0, 0.0), gp_Dir2d(1, 0)), 2.0);
  gp_Circ2d aInputCirc3 = gp_Circ2d(gp_Ax2d(gp_Pnt2d(2.0, 5.0), gp_Dir2d(1, 0)), 1.5);

  Geom2dGcc_Circ2d3Tan aSolver(aQual1, aQual2, aQual3, myTolerance, 0, 0, 0);

  // For this specific touching configuration, the algorithm should succeed
  EXPECT_TRUE(aSolver.IsDone()) << "Touching circles configuration should be solvable";

  int aNbSol = aSolver.NbSolutions();
  EXPECT_GE(aNbSol, 1) << "Should find at least one solution for touching circles case";

  // Verify all solutions are valid and satisfy tangency
  for (int i = 1; i <= aNbSol; i++)
  {
    gp_Circ2d aSol = aSolver.ThisSolution(i);
    verifySolutionValidity(aSol, i);
    verifyTangencyConstraints(aSol, aInputCirc1, aInputCirc2, aInputCirc3, i);
  }
}
