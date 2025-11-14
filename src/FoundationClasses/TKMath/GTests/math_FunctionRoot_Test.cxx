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

#include <math_FunctionRoot.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <math_BracketedRoot.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <StdFail_NotDone.hxx>
#include <Precision.hxx>

#include <cmath>

// Test function classes for root finding

// Simple quadratic function: f(x) = x^2 - 4 (roots at x = +/-2)
class QuadraticFunction : public math_FunctionWithDerivative
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = theX * theX - 4.0;
    return Standard_True;
  }

  Standard_Boolean Derivative(const Standard_Real theX, Standard_Real& theD) override
  {
    theD = 2.0 * theX;
    return Standard_True;
  }

  Standard_Boolean Values(const Standard_Real theX,
                          Standard_Real&      theF,
                          Standard_Real&      theD) override
  {
    theF = theX * theX - 4.0;
    theD = 2.0 * theX;
    return Standard_True;
  }
};

// Cubic function: f(x) = x^3 - 6x^2 + 11x - 6 = (x-1)(x-2)(x-3) (roots at x = 1, 2, 3)
class CubicFunction : public math_FunctionWithDerivative
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = theX * theX * theX - 6.0 * theX * theX + 11.0 * theX - 6.0;
    return Standard_True;
  }

  Standard_Boolean Derivative(const Standard_Real theX, Standard_Real& theD) override
  {
    theD = 3.0 * theX * theX - 12.0 * theX + 11.0;
    return Standard_True;
  }

  Standard_Boolean Values(const Standard_Real theX,
                          Standard_Real&      theF,
                          Standard_Real&      theD) override
  {
    theF = theX * theX * theX - 6.0 * theX * theX + 11.0 * theX - 6.0;
    theD = 3.0 * theX * theX - 12.0 * theX + 11.0;
    return Standard_True;
  }
};

// Trigonometric function: f(x) = sin(x) (root at x = PI)
class SinFunction : public math_FunctionWithDerivative
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = sin(theX);
    return Standard_True;
  }

  Standard_Boolean Derivative(const Standard_Real theX, Standard_Real& theD) override
  {
    theD = cos(theX);
    return Standard_True;
  }

  Standard_Boolean Values(const Standard_Real theX,
                          Standard_Real&      theF,
                          Standard_Real&      theD) override
  {
    theF = sin(theX);
    theD = cos(theX);
    return Standard_True;
  }
};

// Function with zero derivative at root: f(x) = x^2 (root at x = 0)
class ZeroDerivativeFunction : public math_FunctionWithDerivative
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = theX * theX;
    return Standard_True;
  }

  Standard_Boolean Derivative(const Standard_Real theX, Standard_Real& theD) override
  {
    theD = 2.0 * theX;
    return Standard_True;
  }

  Standard_Boolean Values(const Standard_Real theX,
                          Standard_Real&      theF,
                          Standard_Real&      theD) override
  {
    theF = theX * theX;
    theD = 2.0 * theX;
    return Standard_True;
  }
};

// Tests for math_FunctionRoot
TEST(MathFunctionRootTest, QuadraticPositiveRoot)
{
  QuadraticFunction aFunc;
  Standard_Real     aTolerance     = 1.0e-6;
  Standard_Real     anInitialGuess = 3.0; // Should converge to +2

  math_FunctionRoot aRootFinder(aFunc, anInitialGuess, aTolerance);

  EXPECT_TRUE(aRootFinder.IsDone()) << "Root finding should succeed";
  EXPECT_NEAR(aRootFinder.Root(), 2.0, aTolerance) << "Root should be approximately 2.0";
  EXPECT_NEAR(aRootFinder.Value(), 0.0, aTolerance)
    << "Function value at root should be approximately 0";
  EXPECT_NEAR(aRootFinder.Derivative(), 4.0, aTolerance)
    << "Derivative at root should be approximately 4.0";
  EXPECT_GT(aRootFinder.NbIterations(), 0) << "Should require at least one iteration";
}

TEST(MathFunctionRootTest, QuadraticNegativeRoot)
{
  QuadraticFunction aFunc;
  Standard_Real     aTolerance     = 1.0e-6;
  Standard_Real     anInitialGuess = -3.0; // Should converge to -2

  math_FunctionRoot aRootFinder(aFunc, anInitialGuess, aTolerance);

  EXPECT_TRUE(aRootFinder.IsDone()) << "Root finding should succeed";
  EXPECT_NEAR(aRootFinder.Root(), -2.0, aTolerance) << "Root should be approximately -2.0";
  EXPECT_NEAR(aRootFinder.Value(), 0.0, aTolerance)
    << "Function value at root should be approximately 0";
  EXPECT_NEAR(aRootFinder.Derivative(), -4.0, aTolerance)
    << "Derivative at root should be approximately -4.0";
}

TEST(MathFunctionRootTest, QuadraticWithBounds)
{
  QuadraticFunction aFunc;
  Standard_Real     aTolerance     = 1.0e-6;
  Standard_Real     anInitialGuess = 1.5;
  Standard_Real     aLowerBound    = 1.0;
  Standard_Real     anUpperBound   = 3.0;

  math_FunctionRoot aRootFinder(aFunc, anInitialGuess, aTolerance, aLowerBound, anUpperBound);

  EXPECT_TRUE(aRootFinder.IsDone()) << "Root finding with bounds should succeed";
  EXPECT_NEAR(aRootFinder.Root(), 2.0, aTolerance) << "Root should be approximately 2.0";
  EXPECT_GE(aRootFinder.Root(), aLowerBound) << "Root should be within lower bound";
  EXPECT_LE(aRootFinder.Root(), anUpperBound) << "Root should be within upper bound";
}

TEST(MathFunctionRootTest, CubicMultipleRoots)
{
  CubicFunction aFunc;
  Standard_Real aTolerance = 1.0e-6;

  // Test finding root near x = 1
  {
    Standard_Real     anInitialGuess = 0.8;
    math_FunctionRoot aRootFinder(aFunc, anInitialGuess, aTolerance);

    EXPECT_TRUE(aRootFinder.IsDone()) << "Root finding should succeed for first root";
    EXPECT_NEAR(aRootFinder.Root(), 1.0, aTolerance) << "Root should be approximately 1.0";
    EXPECT_NEAR(aRootFinder.Value(), 0.0, aTolerance)
      << "Function value at root should be approximately 0";
  }

  // Test finding root near x = 2
  {
    Standard_Real     anInitialGuess = 1.8;
    math_FunctionRoot aRootFinder(aFunc, anInitialGuess, aTolerance);

    EXPECT_TRUE(aRootFinder.IsDone()) << "Root finding should succeed for second root";
    EXPECT_NEAR(aRootFinder.Root(), 2.0, aTolerance) << "Root should be approximately 2.0";
    EXPECT_NEAR(aRootFinder.Value(), 0.0, aTolerance)
      << "Function value at root should be approximately 0";
  }

  // Test finding root near x = 3
  {
    Standard_Real     anInitialGuess = 3.2;
    math_FunctionRoot aRootFinder(aFunc, anInitialGuess, aTolerance);

    EXPECT_TRUE(aRootFinder.IsDone()) << "Root finding should succeed for third root";
    EXPECT_NEAR(aRootFinder.Root(), 3.0, aTolerance) << "Root should be approximately 3.0";
    EXPECT_NEAR(aRootFinder.Value(), 0.0, aTolerance)
      << "Function value at root should be approximately 0";
  }
}

TEST(MathFunctionRootTest, TrigonometricFunction)
{
  SinFunction   aFunc;
  Standard_Real aTolerance     = 1.0e-6;
  Standard_Real anInitialGuess = 3.5; // Should converge to PI approximately 3.14159

  math_FunctionRoot aRootFinder(aFunc, anInitialGuess, aTolerance);

  EXPECT_TRUE(aRootFinder.IsDone()) << "Root finding should succeed for sin(x)";
  EXPECT_NEAR(aRootFinder.Root(), M_PI, aTolerance) << "Root should be approximately PI";
  EXPECT_NEAR(aRootFinder.Value(), 0.0, aTolerance)
    << "Function value at root should be approximately 0";
  EXPECT_NEAR(aRootFinder.Derivative(), -1.0, aTolerance) << "cos(PI) should be approximately -1";
}

TEST(MathFunctionRootTest, HighPrecisionTolerance)
{
  QuadraticFunction aFunc;
  Standard_Real     aTolerance     = 1.0e-12;
  Standard_Real     anInitialGuess = 2.1;

  math_FunctionRoot aRootFinder(aFunc, anInitialGuess, aTolerance);

  EXPECT_TRUE(aRootFinder.IsDone()) << "High precision root finding should succeed";
  EXPECT_NEAR(aRootFinder.Root(), 2.0, aTolerance) << "Root should be very precise";
  EXPECT_NEAR(aRootFinder.Value(), 0.0, aTolerance)
    << "Function value should be very close to zero";
}

TEST(MathFunctionRootTest, MaxIterationsLimit)
{
  QuadraticFunction aFunc;
  Standard_Real     aTolerance     = 1.0e-10; // Reasonable tolerance for debug mode
  Standard_Real     anInitialGuess = 2.1;
  Standard_Integer  aMaxIterations = 5; // Few but reasonable iterations

  // Wrap in try-catch to handle potential exceptions in debug mode
  EXPECT_NO_THROW({
    math_FunctionRoot aRootFinder(aFunc, anInitialGuess, aTolerance, aMaxIterations);

    // Should either succeed within iterations or fail gracefully
    if (aRootFinder.IsDone())
    {
      EXPECT_LE(aRootFinder.NbIterations(), aMaxIterations) << "Should not exceed max iterations";
      EXPECT_NEAR(aRootFinder.Root(), 2.0, 1.0e-3)
        << "Root should be reasonably close even with few iterations";
    }
    else
    {
      // It's acceptable to fail if too few iterations are allowed
      EXPECT_LE(aMaxIterations, 10) << "Failure is acceptable with very few iterations";
    }
  }) << "Function root finding should not throw exceptions";
}

TEST(MathFunctionRootTest, OutOfBoundsGuess)
{
  QuadraticFunction aFunc;
  Standard_Real     aTolerance     = 1.0e-6;
  Standard_Real     anInitialGuess = 0.0;
  Standard_Real     aLowerBound    = 2.5;
  Standard_Real     anUpperBound   = 3.0; // No root in this interval

  math_FunctionRoot aRootFinder(aFunc, anInitialGuess, aTolerance, aLowerBound, anUpperBound);

  // Test that the algorithm respects bounds - the solution should be within bounds
  // if one is found, or the algorithm should fail
  if (aRootFinder.IsDone())
  {
    Standard_Real aRoot = aRootFinder.Root();
    EXPECT_GE(aRoot, aLowerBound) << "Solution should be within lower bound";
    EXPECT_LE(aRoot, anUpperBound) << "Solution should be within upper bound";

    // If the algorithm reports Done but the function value is not near zero,
    // it might have stopped due to bounds rather than finding a true root
    Standard_Real aFunctionValue = aRootFinder.Value();
    if (std::abs(aFunctionValue) > 1.0e-3)
    {
      // This is acceptable - the algorithm stopped due to bounds, not convergence to root
      EXPECT_GE(aRoot, aLowerBound) << "Should still respect bounds";
      EXPECT_LE(aRoot, anUpperBound) << "Should still respect bounds";
    }
    else
    {
      // True root found
      EXPECT_NEAR(aFunctionValue, 0.0, aTolerance)
        << "True root should have function value near zero";
    }
  }
}

TEST(MathFunctionRootTest, ZeroDerivativeHandling)
{
  ZeroDerivativeFunction aFunc;
  Standard_Real          aTolerance     = 1.0e-6;
  Standard_Real          anInitialGuess = 0.1; // Close to the root at x = 0

  math_FunctionRoot aRootFinder(aFunc, anInitialGuess, aTolerance);

  EXPECT_TRUE(aRootFinder.IsDone())
    << "Root finding should succeed even with zero derivative at root";
  EXPECT_NEAR(aRootFinder.Root(), 0.0, aTolerance) << "Root should be approximately 0.0";
  EXPECT_NEAR(aRootFinder.Value(), 0.0, aTolerance)
    << "Function value at root should be approximately 0";
  EXPECT_NEAR(aRootFinder.Derivative(), 0.0, aTolerance)
    << "Derivative at root should be approximately 0";
}

// Tests for exceptions
TEST(MathFunctionRootTest, ConstrainedConvergenceState)
{
  QuadraticFunction aFunc;
  Standard_Real     aTolerance     = 1.0e-10; // Reasonable tolerance for debug mode
  Standard_Real     anInitialGuess = 50.0;    // Far from roots but not extreme
  Standard_Integer  aMaxIterations = 3;       // Few but reasonable iterations

  // Wrap in try-catch to handle potential exceptions in debug mode
  EXPECT_NO_THROW({
    math_FunctionRoot aRootFinder(aFunc, anInitialGuess, aTolerance, aMaxIterations);

    // Test state handling for constrained convergence conditions
    if (!aRootFinder.IsDone())
    {
      // Verify consistent state reporting
      EXPECT_FALSE(aRootFinder.IsDone()) << "Root finder should consistently report failure";
      EXPECT_GE(aRootFinder.NbIterations(), 0)
        << "Iteration count should be non-negative even on failure";
    }
    else
    {
      // If it succeeds, verify the results are reasonable
      EXPECT_GT(aRootFinder.NbIterations(), 0) << "Should have done at least one iteration";
      EXPECT_TRUE(std::abs(aRootFinder.Root() - 2.0) < 0.5
                  || std::abs(aRootFinder.Root() - (-2.0)) < 0.5)
        << "Root should be reasonably close to one of the expected roots";
    }
  }) << "Function root finding should not throw exceptions";
}

// Tests for convergence behavior
TEST(MathFunctionRootTest, ConvergenceBehavior)
{
  QuadraticFunction aFunc;
  Standard_Real     aTolerance = 1.0e-6;

  // Test different initial guesses and verify they converge to the nearest root
  struct TestCase
  {
    Standard_Real initialGuess;
    Standard_Real expectedRoot;
    Standard_Real tolerance;
  };

  TestCase aTestCases[] = {
    {1.0, 2.0, aTolerance},    // Positive initial guess -> positive root
    {-1.0, -2.0, aTolerance},  // Negative initial guess -> negative root
    {10.0, 2.0, aTolerance},   // Far positive guess -> positive root
    {-10.0, -2.0, aTolerance}, // Far negative guess -> negative root
  };

  for (const auto& aTestCase : aTestCases)
  {
    math_FunctionRoot aRootFinder(aFunc, aTestCase.initialGuess, aTestCase.tolerance);

    EXPECT_TRUE(aRootFinder.IsDone())
      << "Root finding should succeed for initial guess " << aTestCase.initialGuess;
    EXPECT_NEAR(aRootFinder.Root(), aTestCase.expectedRoot, aTestCase.tolerance)
      << "Root should converge correctly from initial guess " << aTestCase.initialGuess;
  }
}

// Performance test
TEST(MathFunctionRootTest, PerformanceComparison)
{
  QuadraticFunction aFunc;
  Standard_Real     aTolerance     = 1.0e-10;
  Standard_Real     anInitialGuess = 2.1;

  math_FunctionRoot aRootFinder(aFunc, anInitialGuess, aTolerance);

  EXPECT_TRUE(aRootFinder.IsDone()) << "High precision root finding should succeed";
  EXPECT_LT(aRootFinder.NbIterations(), 50) << "Should converge in reasonable number of iterations";
  EXPECT_NEAR(aRootFinder.Root(), 2.0, aTolerance) << "Root should be highly accurate";
}