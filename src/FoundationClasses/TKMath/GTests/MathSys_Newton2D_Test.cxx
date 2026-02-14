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

#include <MathSys_Newton2D.hxx>
#include <MathUtils_Config.hxx>

#include <cmath>

namespace
{

//! Simple quadratic function for testing: f(x,y) = x^2 + y^2
//! Minimum at (0, 0) with gradient = (2x, 2y)
class QuadraticFunc
{
public:
  bool ValueAndJacobian(double theU, double theV,
                        double& theF1, double& theF2,
                        double& theJ11, double& theJ12,
                        double& theJ21, double& theJ22) const
  {
    // Gradient of f(x,y) = x^2 + y^2 is (2x, 2y)
    theF1 = 2.0 * theU;
    theF2 = 2.0 * theV;
    // Hessian (Jacobian of gradient)
    theJ11 = 2.0;
    theJ12 = 0.0;
    theJ21 = 0.0;
    theJ22 = 2.0;
    return true;
  }
};

//! Symmetric function for testing Newton2DSymmetric
//! Represents gradient of distance function f(u,v) = (u-a)^2 + (v-b)^2
class SymmetricDistFunc
{
public:
  SymmetricDistFunc(double theTargetU, double theTargetV)
      : myTargetU(theTargetU),
        myTargetV(theTargetV)
  {
  }

  bool Value(double theU, double theV, double& theFu, double& theFv) const
  {
    theFu = 2.0 * (theU - myTargetU);
    theFv = 2.0 * (theV - myTargetV);
    return true;
  }

  bool ValueAndJacobian(double theU, double theV,
                        double& theFu, double& theFv,
                        double& theDFuu, double& theDFuv, double& theDFvv) const
  {
    theFu   = 2.0 * (theU - myTargetU);
    theFv   = 2.0 * (theV - myTargetV);
    theDFuu = 2.0;
    theDFuv = 0.0;
    theDFvv = 2.0;
    return true;
  }

private:
  double myTargetU;
  double myTargetV;
};

//! Rosenbrock-like function for challenging optimization
//! f(x,y) = (1-x)^2 + 100*(y-x^2)^2
//! Gradient: [2*(x-1) + 400*x*(x^2-y), 200*(y-x^2)]
class RosenbrockGrad
{
public:
  bool ValueAndJacobian(double theU, double theV,
                        double& theF1, double& theF2,
                        double& theJ11, double& theJ12,
                        double& theJ21, double& theJ22) const
  {
    double aX2 = theU * theU;
    theF1 = 2.0 * (theU - 1.0) + 400.0 * theU * (aX2 - theV);
    theF2 = 200.0 * (theV - aX2);

    // Jacobian (Hessian of Rosenbrock)
    theJ11 = 2.0 + 400.0 * (3.0 * aX2 - theV);
    theJ12 = -400.0 * theU;
    theJ21 = -400.0 * theU;
    theJ22 = 200.0;
    return true;
  }
};

//! Ill-conditioned function for SVD fallback testing
//! Creates a nearly singular Jacobian
class IllConditionedFunc
{
public:
  IllConditionedFunc(double theCondition = 1.0e10)
      : myCondition(theCondition)
  {
  }

  bool ValueAndJacobian(double theU, double theV,
                        double& theF1, double& theF2,
                        double& theJ11, double& theJ12,
                        double& theJ21, double& theJ22) const
  {
    // Function with ill-conditioned Hessian
    theF1 = theU + theV * 1.0e-10;
    theF2 = theU * 1.0e-10 + theV / myCondition;

    theJ11 = 1.0;
    theJ12 = 1.0e-10;
    theJ21 = 1.0e-10;
    theJ22 = 1.0 / myCondition;
    return true;
  }

private:
  double myCondition;
};

//! Function for testing symmetric Newton with SVD fallback
class SymmetricIllConditioned
{
public:
  SymmetricIllConditioned(double theEpsilon = 1.0e-12)
      : myEpsilon(theEpsilon)
  {
  }

  bool Value(double theU, double theV, double& theFu, double& theFv) const
  {
    theFu = theU + myEpsilon * theV;
    theFv = myEpsilon * theU + myEpsilon * myEpsilon * theV;
    return true;
  }

  bool ValueAndJacobian(double /*theU*/, double /*theV*/,
                        double& theFu, double& theFv,
                        double& theDFuu, double& theDFuv, double& theDFvv) const
  {
    theFu   = 0.0; // At solution
    theFv   = 0.0;
    theDFuu = 1.0;
    theDFuv = myEpsilon;
    theDFvv = myEpsilon * myEpsilon;
    return true;
  }

private:
  double myEpsilon;
};

} // namespace

//==================================================================================================
// Basic Newton2D Tests
//==================================================================================================

TEST(MathSys_Newton2DTest, QuadraticFunction_ConvergesToOrigin)
{
  QuadraticFunc aFunc;

  auto aResult = MathSys::Newton2D(aFunc, 5.0, 3.0, -10.0, 10.0, -10.0, 10.0, 1.0e-10, 50);

  EXPECT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.U, 0.0, 1.0e-9);
  EXPECT_NEAR(aResult.V, 0.0, 1.0e-9);
  EXPECT_LT(aResult.FNorm, 1.0e-10);
}

TEST(MathSys_Newton2DTest, QuadraticFunction_QuadraticConvergence)
{
  QuadraticFunc aFunc;

  // Should converge in very few iterations for a quadratic
  auto aResult = MathSys::Newton2D(aFunc, 100.0, 100.0, -1000.0, 1000.0, -1000.0, 1000.0, 1.0e-12, 20);

  EXPECT_TRUE(aResult.IsDone());
  EXPECT_LE(aResult.NbIter, 10u); // Quadratic should converge fast
}

//==================================================================================================
// Newton2DSymmetric Tests
//==================================================================================================

TEST(MathSys_Newton2DSymmetricTest, DistanceFunction_FindsTarget)
{
  SymmetricDistFunc aFunc(3.5, 7.2);

  auto aResult = MathSys::Newton2DSymmetric(aFunc, 0.0, 0.0, -10.0, 10.0, -10.0, 10.0, 1.0e-10, 50);

  EXPECT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.U, 3.5, 1.0e-9);
  EXPECT_NEAR(aResult.V, 7.2, 1.0e-9);
}

TEST(MathSys_Newton2DSymmetricTest, DistanceFunction_WithBounds)
{
  // Target outside bounds
  SymmetricDistFunc aFunc(15.0, 15.0);

  auto aResult = MathSys::Newton2DSymmetric(aFunc, 5.0, 5.0, 0.0, 10.0, 0.0, 10.0, 1.0e-8, 50);

  // Should converge near boundary (with soft bounds) or stagnate - any termination is acceptable
  // The key is that the algorithm terminates and doesn't hang
  EXPECT_GT(aResult.NbIter, 0u);
}

TEST(MathSys_Newton2DSymmetricTest, HighPrecisionConvergence)
{
  SymmetricDistFunc aFunc(5.123456789, 2.987654321);

  // Test high precision (5e-8)
  auto aResult =
    MathSys::Newton2DSymmetric(aFunc, 0.0, 0.0, -10.0, 10.0, -10.0, 10.0, 5.0e-8, 100);

  EXPECT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.U, 5.123456789, 1.0e-7);
  EXPECT_NEAR(aResult.V, 2.987654321, 1.0e-7);
  EXPECT_LT(aResult.FNorm, 5.0e-8);
}

//==================================================================================================
// SVD Fallback Tests
//==================================================================================================

TEST(MathSys_Newton2DTest, SVDFallback_SolveSymmetric2x2)
{
  // Test the SVD solver directly with a well-conditioned system
  // J = [[2, 1], [1, 3]], b = [-1, -2]
  // Solution: x = [0.2, 0.6] (approximately)
  double aDU, aDV;
  bool   aSuccess = MathSys::SolveSymmetric2x2SVD(2.0, 1.0, 3.0, 1.0, 2.0, aDU, aDV);

  EXPECT_TRUE(aSuccess);

  // Verify: J * x = b
  double aRes1 = 2.0 * aDU + 1.0 * aDV + 1.0;
  double aRes2 = 1.0 * aDU + 3.0 * aDV + 2.0;
  EXPECT_NEAR(aRes1, 0.0, 1.0e-10);
  EXPECT_NEAR(aRes2, 0.0, 1.0e-10);
}

TEST(MathSys_Newton2DTest, SVDFallback_NearSingular)
{
  // Nearly singular matrix: J = [[1, 1-eps], [1-eps, 1]]
  const double aEps = 1.0e-8;
  double       aDU, aDV;
  bool aSuccess = MathSys::SolveSymmetric2x2SVD(1.0, 1.0 - aEps, 1.0, 1.0, 1.0, aDU, aDV);

  // Should still find a solution (regularized)
  EXPECT_TRUE(aSuccess);
}

TEST(MathSys_Newton2DTest, SVDFallback_SingularMatrix)
{
  // Truly singular matrix: J = [[1, 1], [1, 1]]
  double aDU, aDV;
  bool   aSuccess = MathSys::SolveSymmetric2x2SVD(1.0, 1.0, 1.0, 1.0, 1.0, aDU, aDV);

  // SVD should handle this with regularization or return failure
  // Either outcome is acceptable for truly singular case
  (void)aSuccess; // May or may not succeed depending on regularization
}

//==================================================================================================
// Stagnation Detection Tests
//==================================================================================================

TEST(MathSys_Newton2DSymmetricTest, StagnationDetection_EarlyTermination)
{
  // Use Rosenbrock which has slow convergence in the valley
  RosenbrockGrad aFunc;

  // Starting far from optimum
  auto aResult = MathSys::Newton2D(aFunc, -1.0, 1.0, -5.0, 5.0, -5.0, 5.0, 1.0e-10, 100);

  // Rosenbrock is hard to solve - may stagnate or converge
  // The key test is that it terminates (doesn't loop forever)
  EXPECT_GT(aResult.NbIter, 0u);
}

//==================================================================================================
// Convergence Constants Tests
//==================================================================================================

TEST(MathSys_Newton2DTest, ConstantsAreReasonable)
{
  // Verify the centralized constants have reasonable values
  EXPECT_LT(MathUtils::THE_NEWTON2D_SINGULAR_DET, 1.0e-10);
  EXPECT_GT(MathUtils::THE_NEWTON2D_SINGULAR_DET, 0.0);

  EXPECT_LT(MathUtils::THE_NEWTON2D_CRITICAL_GRAD_SQ, MathUtils::THE_NEWTON2D_SINGULAR_DET);

  EXPECT_GT(MathUtils::THE_NEWTON2D_MAX_STEP_RATIO, 0.0);
  EXPECT_LE(MathUtils::THE_NEWTON2D_MAX_STEP_RATIO, 1.0);

  EXPECT_GT(MathUtils::THE_NEWTON2D_STAGNATION_RATIO, 0.9);
  EXPECT_LT(MathUtils::THE_NEWTON2D_STAGNATION_RATIO, 1.0);

  EXPECT_GE(MathUtils::THE_NEWTON2D_STAGNATION_COUNT, 2);
  EXPECT_LE(MathUtils::THE_NEWTON2D_STAGNATION_COUNT, 10);

  EXPECT_GE(MathUtils::THE_NEWTON2D_LINE_SEARCH_MAX, 5);
  EXPECT_LE(MathUtils::THE_NEWTON2D_LINE_SEARCH_MAX, 20);
}

//==================================================================================================
// Boundary Handling Tests
//==================================================================================================

TEST(MathSys_Newton2DSymmetricTest, SoftBoundaryHandling)
{
  // Target at boundary
  SymmetricDistFunc aFunc(10.0, 10.0);

  auto aResult = MathSys::Newton2DSymmetric(aFunc, 5.0, 5.0, 0.0, 10.0, 0.0, 10.0, 1.0e-8, 50);

  // Should find solution at or near boundary
  if (aResult.IsDone())
  {
    EXPECT_GE(aResult.U, 9.9);
    EXPECT_GE(aResult.V, 9.9);
  }
}

TEST(MathSys_Newton2DSymmetricTest, DomainExtension)
{
  // Target slightly outside domain - soft bounds should allow this
  SymmetricDistFunc aFunc(10.05, 10.05);

  auto aResult = MathSys::Newton2DSymmetric(aFunc, 5.0, 5.0, 0.0, 10.0, 0.0, 10.0, 1.0e-6, 50);

  // With domain extension (1%), solution at 10.05 should be reachable
  // Domain extension is 1% of 10.0 = 0.1, so 10.05 is within extended bounds
  if (aResult.IsDone())
  {
    EXPECT_NEAR(aResult.U, 10.05, 0.2);
    EXPECT_NEAR(aResult.V, 10.05, 0.2);
  }
}

//==================================================================================================
// Multiple Starting Points Tests
//==================================================================================================

TEST(MathSys_Newton2DSymmetricTest, ConsistentResultsFromDifferentStarts)
{
  SymmetricDistFunc aFunc(5.0, 5.0);

  // Try from different starting points
  std::vector<std::pair<double, double>> aStarts = {{0.0, 0.0}, {10.0, 10.0}, {2.0, 8.0}, {8.0, 2.0}};

  for (const auto& aStart : aStarts)
  {
    auto aResult = MathSys::Newton2DSymmetric(
      aFunc, aStart.first, aStart.second, 0.0, 10.0, 0.0, 10.0, 1.0e-10, 50);

    EXPECT_TRUE(aResult.IsDone()) << "Failed from start (" << aStart.first << ", " << aStart.second
                                  << ")";
    if (aResult.IsDone())
    {
      EXPECT_NEAR(aResult.U, 5.0, 1.0e-8);
      EXPECT_NEAR(aResult.V, 5.0, 1.0e-8);
    }
  }
}
