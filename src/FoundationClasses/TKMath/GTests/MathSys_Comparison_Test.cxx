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

#include <MathSys_Newton.hxx>
#include <math_NewtonFunctionSetRoot.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Vector.hxx>
#include <math_Matrix.hxx>

#include <cmath>

namespace
{
  constexpr double THE_TOLERANCE = 1.0e-8;

  //! Simple 2x2 nonlinear system: F1 = x^2 + y^2 - 1, F2 = x - y
  //! Solution: x = y = sqrt(0.5) ~ 0.7071
  class SimpleCircleLineSystem : public math_FunctionSetWithDerivatives
  {
  public:
    int NbVariables() const override { return 2; }
    int NbEquations() const override { return 2; }

    bool Value(const math_Vector& X, math_Vector& F) override
    {
      F(1) = X(1) * X(1) + X(2) * X(2) - 1.0; // x^2 + y^2 = 1
      F(2) = X(1) - X(2);                     // x = y
      return true;
    }

    bool Derivatives(const math_Vector& X, math_Matrix& D) override
    {
      D(1, 1) = 2.0 * X(1); // dF1/dx
      D(1, 2) = 2.0 * X(2); // dF1/dy
      D(2, 1) = 1.0;        // dF2/dx
      D(2, 2) = -1.0;       // dF2/dy
      return true;
    }

    bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override
    {
      return Value(X, F) && Derivatives(X, D);
    }
  };

  //! Rosenbrock-like 2D system: F1 = 10*(y - x^2), F2 = 1 - x
  //! Solution: x = 1, y = 1
  class RosenbrockSystem : public math_FunctionSetWithDerivatives
  {
  public:
    int NbVariables() const override { return 2; }
    int NbEquations() const override { return 2; }

    bool Value(const math_Vector& X, math_Vector& F) override
    {
      F(1) = 10.0 * (X(2) - X(1) * X(1));
      F(2) = 1.0 - X(1);
      return true;
    }

    bool Derivatives(const math_Vector& X, math_Matrix& D) override
    {
      D(1, 1) = -20.0 * X(1);
      D(1, 2) = 10.0;
      D(2, 1) = -1.0;
      D(2, 2) = 0.0;
      return true;
    }

    bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override
    {
      return Value(X, F) && Derivatives(X, D);
    }
  };

  //! 3x3 linear-like system: F1 = 2x + y - 2, F2 = x + 3y - 5, F3 = z - 1
  //! Solution: x = 0.2, y = 1.6, z = 1
  class Linear3System : public math_FunctionSetWithDerivatives
  {
  public:
    int NbVariables() const override { return 3; }
    int NbEquations() const override { return 3; }

    bool Value(const math_Vector& X, math_Vector& F) override
    {
      F(1) = 2.0 * X(1) + X(2) - 2.0;
      F(2) = X(1) + 3.0 * X(2) - 5.0;
      F(3) = X(3) - 1.0;
      return true;
    }

    bool Derivatives(const math_Vector&, math_Matrix& D) override
    {
      D(1, 1) = 2.0;
      D(1, 2) = 1.0;
      D(1, 3) = 0.0;
      D(2, 1) = 1.0;
      D(2, 2) = 3.0;
      D(2, 3) = 0.0;
      D(3, 1) = 0.0;
      D(3, 2) = 0.0;
      D(3, 3) = 1.0;
      return true;
    }

    bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override
    {
      return Value(X, F) && Derivatives(X, D);
    }
  };

  //! Polynomial system: x^3 - y = 0, y^3 - x = 0
  //! Solutions: (0,0), (1,1), (-1,-1)
  class PolynomialSystem : public math_FunctionSetWithDerivatives
  {
  public:
    int NbVariables() const override { return 2; }
    int NbEquations() const override { return 2; }

    bool Value(const math_Vector& X, math_Vector& F) override
    {
      F(1) = X(1) * X(1) * X(1) - X(2);
      F(2) = X(2) * X(2) * X(2) - X(1);
      return true;
    }

    bool Derivatives(const math_Vector& X, math_Matrix& D) override
    {
      D(1, 1) = 3.0 * X(1) * X(1);
      D(1, 2) = -1.0;
      D(2, 1) = -1.0;
      D(2, 2) = 3.0 * X(2) * X(2);
      return true;
    }

    bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override
    {
      return Value(X, F) && Derivatives(X, D);
    }
  };
}

//==================================================================================================
// Test: Simple circle-line system - compare new API with legacy
//==================================================================================================

TEST(MathSys_ComparisonTest, Newton_CircleLineSystem)
{
  SimpleCircleLineSystem aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.5;
  aStart(2) = 0.5;

  math_Vector aTolX(1, 2, 1.0e-10);
  double      aTolF = 1.0e-10;

  // Solve using new API
  auto aNewResult = MathSys::Newton(aFunc, aStart, aTolX, aTolF);
  ASSERT_TRUE(aNewResult.IsDone());

  // Solve using legacy API
  math_NewtonFunctionSetRoot aOldSolver(aFunc, aTolX, aTolF);
  aOldSolver.Perform(aFunc, aStart);
  ASSERT_TRUE(aOldSolver.IsDone());

  const math_Vector& aOldRoot = aOldSolver.Root();

  // Expected solution: x = y = sqrt(0.5)
  const double aExpected = std::sqrt(0.5);

  // Compare solutions
  EXPECT_NEAR((*aNewResult.Solution)(1), aExpected, THE_TOLERANCE);
  EXPECT_NEAR((*aNewResult.Solution)(2), aExpected, THE_TOLERANCE);
  EXPECT_NEAR(aOldRoot(1), aExpected, THE_TOLERANCE);
  EXPECT_NEAR(aOldRoot(2), aExpected, THE_TOLERANCE);

  // Compare new and old results
  EXPECT_NEAR((*aNewResult.Solution)(1), aOldRoot(1), THE_TOLERANCE);
  EXPECT_NEAR((*aNewResult.Solution)(2), aOldRoot(2), THE_TOLERANCE);
}

//==================================================================================================
// Test: Rosenbrock system
//==================================================================================================

TEST(MathSys_ComparisonTest, Newton_RosenbrockSystem)
{
  RosenbrockSystem aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  math_Vector aTolX(1, 2, 1.0e-10);
  double      aTolF = 1.0e-10;

  // Solve using new API
  auto aNewResult = MathSys::Newton(aFunc, aStart, aTolX, aTolF);
  ASSERT_TRUE(aNewResult.IsDone());

  // Solve using legacy API
  math_NewtonFunctionSetRoot aOldSolver(aFunc, aTolX, aTolF);
  aOldSolver.Perform(aFunc, aStart);
  ASSERT_TRUE(aOldSolver.IsDone());

  const math_Vector& aOldRoot = aOldSolver.Root();

  // Expected solution: x = 1, y = 1
  EXPECT_NEAR((*aNewResult.Solution)(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR((*aNewResult.Solution)(2), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aOldRoot(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aOldRoot(2), 1.0, THE_TOLERANCE);

  // Compare new and old
  EXPECT_NEAR((*aNewResult.Solution)(1), aOldRoot(1), THE_TOLERANCE);
  EXPECT_NEAR((*aNewResult.Solution)(2), aOldRoot(2), THE_TOLERANCE);
}

//==================================================================================================
// Test: 3D linear system
//==================================================================================================

TEST(MathSys_ComparisonTest, Newton_Linear3System)
{
  Linear3System aFunc;

  math_Vector aStart(1, 3);
  aStart(1) = 0.0;
  aStart(2) = 0.0;
  aStart(3) = 0.0;

  math_Vector aTolX(1, 3, 1.0e-10);
  double      aTolF = 1.0e-10;

  // Solve using new API
  auto aNewResult = MathSys::Newton(aFunc, aStart, aTolX, aTolF);
  ASSERT_TRUE(aNewResult.IsDone());

  // Solve using legacy API
  math_NewtonFunctionSetRoot aOldSolver(aFunc, aTolX, aTolF);
  aOldSolver.Perform(aFunc, aStart);
  ASSERT_TRUE(aOldSolver.IsDone());

  const math_Vector& aOldRoot = aOldSolver.Root();

  // Expected solution: x = 0.2, y = 1.6, z = 1.0
  EXPECT_NEAR((*aNewResult.Solution)(1), 0.2, THE_TOLERANCE);
  EXPECT_NEAR((*aNewResult.Solution)(2), 1.6, THE_TOLERANCE);
  EXPECT_NEAR((*aNewResult.Solution)(3), 1.0, THE_TOLERANCE);

  // Compare new and old
  for (int i = 1; i <= 3; ++i)
  {
    EXPECT_NEAR((*aNewResult.Solution)(i), aOldRoot(i), THE_TOLERANCE);
  }
}

//==================================================================================================
// Test: Polynomial system finding (1,1) solution
//==================================================================================================

TEST(MathSys_ComparisonTest, Newton_PolynomialSystem)
{
  PolynomialSystem aFunc;

  // Start near (1,1)
  math_Vector aStart(1, 2);
  aStart(1) = 0.9;
  aStart(2) = 0.9;

  math_Vector aTolX(1, 2, 1.0e-10);
  double      aTolF = 1.0e-10;

  // Solve using new API
  auto aNewResult = MathSys::Newton(aFunc, aStart, aTolX, aTolF);
  ASSERT_TRUE(aNewResult.IsDone());

  // Solve using legacy API
  math_NewtonFunctionSetRoot aOldSolver(aFunc, aTolX, aTolF);
  aOldSolver.Perform(aFunc, aStart);
  ASSERT_TRUE(aOldSolver.IsDone());

  const math_Vector& aOldRoot = aOldSolver.Root();

  // Expected solution: x = 1, y = 1
  EXPECT_NEAR((*aNewResult.Solution)(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR((*aNewResult.Solution)(2), 1.0, THE_TOLERANCE);

  // Compare new and old
  EXPECT_NEAR((*aNewResult.Solution)(1), aOldRoot(1), THE_TOLERANCE);
  EXPECT_NEAR((*aNewResult.Solution)(2), aOldRoot(2), THE_TOLERANCE);
}

//==================================================================================================
// Test: Bounded Newton with simple system
//==================================================================================================

TEST(MathSys_ComparisonTest, NewtonBounded_CircleLineSystem)
{
  SimpleCircleLineSystem aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.5;
  aStart(2) = 0.5;

  math_Vector aInfBound(1, 2);
  aInfBound(1) = 0.0;
  aInfBound(2) = 0.0;

  math_Vector aSupBound(1, 2);
  aSupBound(1) = 2.0;
  aSupBound(2) = 2.0;

  math_Vector aTolX(1, 2, 1.0e-10);
  double      aTolF = 1.0e-10;

  // Solve using new bounded API
  auto aNewResult = MathSys::NewtonBounded(aFunc, aStart, aInfBound, aSupBound, aTolX, aTolF);
  ASSERT_TRUE(aNewResult.IsDone());

  // Solve using legacy bounded API
  math_NewtonFunctionSetRoot aOldSolver(aFunc, aTolX, aTolF);
  aOldSolver.Perform(aFunc, aStart, aInfBound, aSupBound);
  ASSERT_TRUE(aOldSolver.IsDone());

  const math_Vector& aOldRoot = aOldSolver.Root();

  // Expected solution: x = y = sqrt(0.5)
  const double aExpected = std::sqrt(0.5);

  EXPECT_NEAR((*aNewResult.Solution)(1), aExpected, THE_TOLERANCE);
  EXPECT_NEAR((*aNewResult.Solution)(2), aExpected, THE_TOLERANCE);

  // Compare new and old
  EXPECT_NEAR((*aNewResult.Solution)(1), aOldRoot(1), THE_TOLERANCE);
  EXPECT_NEAR((*aNewResult.Solution)(2), aOldRoot(2), THE_TOLERANCE);
}

//==================================================================================================
// Test: Verify function values at solution are near zero
//==================================================================================================

TEST(MathSys_ComparisonTest, Newton_VerifyFunctionValues)
{
  SimpleCircleLineSystem aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.5;
  aStart(2) = 0.5;

  auto aResult = MathSys::Newton(aFunc, aStart, 1.0e-10, 1.0e-10);
  ASSERT_TRUE(aResult.IsDone());

  // Evaluate function at solution
  math_Vector aF(1, 2);
  ASSERT_TRUE(aFunc.Value(*aResult.Solution, aF));

  // Function values should be near zero
  EXPECT_NEAR(aF(1), 0.0, 1.0e-8);
  EXPECT_NEAR(aF(2), 0.0, 1.0e-8);
}

//==================================================================================================
// Test: Iteration count comparison
//==================================================================================================

TEST(MathSys_ComparisonTest, IterationCount_NewVsOld)
{
  RosenbrockSystem aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  math_Vector aTolX(1, 2, 1.0e-10);
  double      aTolF = 1.0e-10;

  // Solve using new API
  auto aNewResult = MathSys::Newton(aFunc, aStart, aTolX, aTolF);
  ASSERT_TRUE(aNewResult.IsDone());

  // Solve using legacy API
  math_NewtonFunctionSetRoot aOldSolver(aFunc, aTolX, aTolF);
  aOldSolver.Perform(aFunc, aStart);
  ASSERT_TRUE(aOldSolver.IsDone());

  // Both should converge in similar number of iterations
  // (exact count may differ slightly due to implementation details)
  EXPECT_GT(aNewResult.NbIterations, 0u);
  EXPECT_LE(aNewResult.NbIterations, 20u); // Reasonable upper bound

  // The iteration counts should be reasonably close
  int aOldIter = aOldSolver.NbIterations();
  EXPECT_NEAR(static_cast<int>(aNewResult.NbIterations), aOldIter, 5);
}

//==================================================================================================
// Test: Simplified Newton interface with uniform tolerance
//==================================================================================================

TEST(MathSys_ComparisonTest, Newton_UniformTolerance)
{
  SimpleCircleLineSystem aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.5;
  aStart(2) = 0.5;

  // Use simplified interface with uniform tolerance
  auto aResult = MathSys::Newton(aFunc, aStart, 1.0e-10, 1.0e-10);
  ASSERT_TRUE(aResult.IsDone());

  // Expected solution
  const double aExpected = std::sqrt(0.5);
  EXPECT_NEAR((*aResult.Solution)(1), aExpected, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), aExpected, THE_TOLERANCE);
}

//==================================================================================================
// Test: Different starting points
//==================================================================================================

TEST(MathSys_ComparisonTest, Newton_DifferentStartingPoints)
{
  SimpleCircleLineSystem aFunc;

  math_Vector aTolX(1, 2, 1.0e-10);
  double       aTolF = 1.0e-10;

  // Test several starting points
  double aStartPoints[][2] = {
      {0.1, 0.1},
      {0.9, 0.9},
      {0.3, 0.7},
      {1.0, 0.0},
  };

  for (const auto& aStartPt : aStartPoints)
  {
    math_Vector aStart(1, 2);
    aStart(1) = aStartPt[0];
    aStart(2) = aStartPt[1];

    auto aNewResult = MathSys::Newton(aFunc, aStart, aTolX, aTolF);
    ASSERT_TRUE(aNewResult.IsDone()) << "Failed for start (" << aStartPt[0] << ", " << aStartPt[1]
                                     << ")";

    math_NewtonFunctionSetRoot aOldSolver(aFunc, aTolX, aTolF);
    aOldSolver.Perform(aFunc, aStart);
    ASSERT_TRUE(aOldSolver.IsDone());

    const math_Vector& aOldRoot = aOldSolver.Root();

    // Both should find the same solution
    EXPECT_NEAR((*aNewResult.Solution)(1), aOldRoot(1), THE_TOLERANCE);
    EXPECT_NEAR((*aNewResult.Solution)(2), aOldRoot(2), THE_TOLERANCE);
  }
}
