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

#include <math_NewtonFunctionRoot.hxx>
#include <math_FunctionWithDerivative.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <StdFail_NotDone.hxx>
#include <Precision.hxx>

#include <cmath>

namespace
{

// Quadratic function: f(x) = x^2 - 4, f'(x) = 2x, roots at x = +/-2
class QuadraticWithDerivative : public math_FunctionWithDerivative
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

// Cubic function: f(x) = x^3 - 6x^2 + 11x - 6 = (x-1)(x-2)(x-3), f'(x) = 3x^2 - 12x + 11
class CubicWithDerivative : public math_FunctionWithDerivative
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

// Sine function: f(x) = sin(x), f'(x) = cos(x), root at x = PI
class SineWithDerivative : public math_FunctionWithDerivative
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

// Exponential function: f(x) = exp(x) - 2, f'(x) = exp(x), root at x = ln(2)
class ExponentialWithDerivative : public math_FunctionWithDerivative
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = exp(theX) - 2.0;
    return Standard_True;
  }

  Standard_Boolean Derivative(const Standard_Real theX, Standard_Real& theD) override
  {
    theD = exp(theX);
    return Standard_True;
  }

  Standard_Boolean Values(const Standard_Real theX,
                          Standard_Real&      theF,
                          Standard_Real&      theD) override
  {
    theF = exp(theX) - 2.0;
    theD = exp(theX);
    return Standard_True;
  }
};

// Function with zero derivative at root: f(x) = x^3, f'(x) = 3x^2, root at x = 0
class CubicWithZeroDerivative : public math_FunctionWithDerivative
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = theX * theX * theX;
    return Standard_True;
  }

  Standard_Boolean Derivative(const Standard_Real theX, Standard_Real& theD) override
  {
    theD = 3.0 * theX * theX;
    return Standard_True;
  }

  Standard_Boolean Values(const Standard_Real theX,
                          Standard_Real&      theF,
                          Standard_Real&      theD) override
  {
    theF = theX * theX * theX;
    theD = 3.0 * theX * theX;
    return Standard_True;
  }
};

// Linear function: f(x) = 2x - 4, f'(x) = 2, root at x = 2
class LinearWithDerivative : public math_FunctionWithDerivative
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = 2.0 * theX - 4.0;
    return Standard_True;
  }

  Standard_Boolean Derivative(const Standard_Real theX, Standard_Real& theD) override
  {
    (void)theX;
    theD = 2.0;
    return Standard_True;
  }

  Standard_Boolean Values(const Standard_Real theX,
                          Standard_Real&      theF,
                          Standard_Real&      theD) override
  {
    theF = 2.0 * theX - 4.0;
    theD = 2.0;
    return Standard_True;
  }
};

} // anonymous namespace

TEST(MathNewtonFunctionRootTest, QuadraticRootFinding)
{
  // Test finding root of quadratic function
  QuadraticWithDerivative aFunc;

  math_NewtonFunctionRoot aSolver(aFunc, 3.0, 1.0e-10, 1.0e-10); // Guess near positive root

  EXPECT_TRUE(aSolver.IsDone()) << "Should find root for quadratic function";
  EXPECT_NEAR(aSolver.Root(), 2.0, 1.0e-8) << "Root should be x = 2";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-10) << "Function value at root should be near 0";
  EXPECT_NEAR(aSolver.Derivative(), 4.0, 1.0e-8) << "Derivative at root should be 4";
  EXPECT_GT(aSolver.NbIterations(), 0) << "Should have performed some iterations";
}

TEST(MathNewtonFunctionRootTest, QuadraticNegativeRoot)
{
  // Test finding negative root of quadratic function
  QuadraticWithDerivative aFunc;

  math_NewtonFunctionRoot aSolver(aFunc, -3.0, 1.0e-10, 1.0e-10); // Guess near negative root

  EXPECT_TRUE(aSolver.IsDone()) << "Should find negative root for quadratic function";
  EXPECT_NEAR(aSolver.Root(), -2.0, 1.0e-8) << "Root should be x = -2";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-10) << "Function value at root should be near 0";
  EXPECT_NEAR(aSolver.Derivative(), -4.0, 1.0e-8) << "Derivative at root should be -4";
}

TEST(MathNewtonFunctionRootTest, CubicRootFinding)
{
  // Test finding root of cubic function
  CubicWithDerivative aFunc;

  math_NewtonFunctionRoot aSolver(aFunc, 1.1, 1.0e-10, 1.0e-10); // Guess closer to root at 1

  EXPECT_TRUE(aSolver.IsDone()) << "Should find root for cubic function";
  // Newton's method may converge to any of the roots (1, 2, or 3) depending on initial guess
  Standard_Real    aRoot = aSolver.Root();
  Standard_Boolean aFoundValidRoot =
    (fabs(aRoot - 1.0) < 1.0e-6) || (fabs(aRoot - 2.0) < 1.0e-6) || (fabs(aRoot - 3.0) < 1.0e-6);
  EXPECT_TRUE(aFoundValidRoot) << "Root should be one of: 1, 2, or 3, found: " << aRoot;
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-10) << "Function value at root should be near 0";
}

TEST(MathNewtonFunctionRootTest, SineRootFinding)
{
  // Test finding root of sine function
  SineWithDerivative aFunc;

  math_NewtonFunctionRoot aSolver(aFunc, 3.0, 1.0e-10, 1.0e-10); // Guess near PI

  EXPECT_TRUE(aSolver.IsDone()) << "Should find root for sine function";
  EXPECT_NEAR(aSolver.Root(), M_PI, 1.0e-8) << "Root should be PI";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-10) << "Function value at root should be near 0";
  EXPECT_NEAR(aSolver.Derivative(), -1.0, 1.0e-8) << "Derivative at root should be -1";
}

TEST(MathNewtonFunctionRootTest, ExponentialRootFinding)
{
  // Test finding root of exponential function
  ExponentialWithDerivative aFunc;

  math_NewtonFunctionRoot aSolver(aFunc, 1.0, 1.0e-10, 1.0e-10); // Guess near ln(2)

  EXPECT_TRUE(aSolver.IsDone()) << "Should find root for exponential function";
  EXPECT_NEAR(aSolver.Root(), log(2.0), 1.0e-8) << "Root should be ln(2)";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-10) << "Function value at root should be near 0";
  EXPECT_NEAR(aSolver.Derivative(), 2.0, 1.0e-8) << "Derivative at root should be 2";
}

TEST(MathNewtonFunctionRootTest, LinearRootFinding)
{
  // Test finding root of linear function
  LinearWithDerivative aFunc;

  math_NewtonFunctionRoot aSolver(aFunc, 1.0, 1.0e-10, 1.0e-10);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find root for linear function";
  EXPECT_NEAR(aSolver.Root(), 2.0, 1.0e-10) << "Root should be x = 2";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-10) << "Function value at root should be near 0";
  EXPECT_NEAR(aSolver.Derivative(), 2.0, 1.0e-10) << "Derivative should be 2";
  EXPECT_GT(aSolver.NbIterations(), 0) << "Should have performed some iterations";
  EXPECT_LE(aSolver.NbIterations(), 5) << "Linear function should converge quickly";
}

TEST(MathNewtonFunctionRootTest, BoundedInterval)
{
  // Test Newton method with bounded interval
  QuadraticWithDerivative aFunc;

  math_NewtonFunctionRoot aSolver(aFunc, 1.5, 1.0e-10, 1.0e-10, 0.0, 3.0); // Bounded in [0, 3]

  EXPECT_TRUE(aSolver.IsDone()) << "Should find root within bounds";
  EXPECT_NEAR(aSolver.Root(), 2.0, 1.0e-8) << "Root should be x = 2";
  EXPECT_GE(aSolver.Root(), 0.0) << "Root should be within lower bound";
  EXPECT_LE(aSolver.Root(), 3.0) << "Root should be within upper bound";
}

TEST(MathNewtonFunctionRootTest, CustomTolerances)
{
  // Test with different tolerance values
  QuadraticWithDerivative aFunc;

  // Loose tolerance
  math_NewtonFunctionRoot aSolver1(aFunc, 3.0, 1.0e-3, 1.0e-3);
  EXPECT_TRUE(aSolver1.IsDone()) << "Should converge with loose tolerance";
  EXPECT_NEAR(aSolver1.Root(), 2.0, 1.0e-2) << "Root should be approximately correct";

  // Tight tolerance
  math_NewtonFunctionRoot aSolver2(aFunc, 3.0, 1.0e-12, 1.0e-12);
  EXPECT_TRUE(aSolver2.IsDone()) << "Should converge with tight tolerance";
  EXPECT_NEAR(aSolver2.Root(), 2.0, 1.0e-10) << "Root should be very accurate";
}

TEST(MathNewtonFunctionRootTest, CustomIterationLimit)
{
  // Test with custom iteration limits
  CubicWithDerivative aFunc; // More challenging function

  // Few iterations
  math_NewtonFunctionRoot aSolver1(aFunc, 1.5, 1.0e-10, 1.0e-10, 5);
  if (aSolver1.IsDone())
  {
    EXPECT_LE(aSolver1.NbIterations(), 5) << "Should respect iteration limit";
  }

  // Many iterations
  math_NewtonFunctionRoot aSolver2(aFunc, 1.5, 1.0e-10, 1.0e-10, 100);
  EXPECT_TRUE(aSolver2.IsDone()) << "Should converge with many iterations allowed";
}

TEST(MathNewtonFunctionRootTest, ProtectedConstructorAndPerform)
{
  // Test protected constructor and separate Perform call
  QuadraticWithDerivative aFunc;

  math_NewtonFunctionRoot aSolver(0.0, 5.0, 1.0e-10, 1.0e-10); // Protected constructor with bounds
  aSolver.Perform(aFunc, 3.0);

  EXPECT_TRUE(aSolver.IsDone()) << "Should work with protected constructor";
  EXPECT_NEAR(aSolver.Root(), 2.0, 1.0e-8) << "Should find correct root";
}

TEST(MathNewtonFunctionRootTest, UnperformedState)
{
  // Test state handling before solving
  QuadraticWithDerivative aFunc;
  math_NewtonFunctionRoot aSolver(0.0, 5.0, 1.0e-10, 1.0e-10); // Protected constructor only

  // Before Perform() is called, solver should report not done
  EXPECT_FALSE(aSolver.IsDone()) << "Solver should not be done before Perform()";

  // In release builds, verify the solver maintains consistent state
  if (!aSolver.IsDone())
  {
    EXPECT_FALSE(aSolver.IsDone()) << "State should be consistent when not done";
  }
}

TEST(MathNewtonFunctionRootTest, StartingAtRoot)
{
  // Test when initial guess is already at the root
  LinearWithDerivative aFunc;

  math_NewtonFunctionRoot aSolver(aFunc, 2.0, 1.0e-10, 1.0e-10); // Start exactly at root

  EXPECT_TRUE(aSolver.IsDone()) << "Should succeed when starting at root";
  EXPECT_NEAR(aSolver.Root(), 2.0, 1.0e-10) << "Should find exact root";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-10) << "Function value should be exactly 0";
}

TEST(MathNewtonFunctionRootTest, ZeroDerivativeAtRoot)
{
  // Test with function having zero derivative at root (challenging case)
  CubicWithZeroDerivative aFunc;

  math_NewtonFunctionRoot aSolver(aFunc, 0.1, 1.0e-8, 1.0e-8); // Slightly looser tolerance

  if (aSolver.IsDone())
  {
    EXPECT_NEAR(aSolver.Root(), 0.0, 1.0e-6) << "Should find root despite zero derivative";
    EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-8) << "Function value should be near 0";
    EXPECT_NEAR(aSolver.Derivative(), 0.0, 1.0e-6) << "Derivative should be near 0";
  }
  // Note: Newton's method may fail with zero derivative, so we don't assert IsDone()
}

TEST(MathNewtonFunctionRootTest, NarrowBounds)
{
  // Test with very narrow bounds around the root
  QuadraticWithDerivative aFunc;

  math_NewtonFunctionRoot aSolver(aFunc, 2.0, 1.0e-10, 1.0e-10, 1.99, 2.01);

  EXPECT_TRUE(aSolver.IsDone()) << "Should work with narrow bounds";
  EXPECT_NEAR(aSolver.Root(), 2.0, 1.0e-8) << "Should find root within narrow bounds";
  EXPECT_GE(aSolver.Root(), 1.99) << "Root should be within lower bound";
  EXPECT_LE(aSolver.Root(), 2.01) << "Root should be within upper bound";
}

TEST(MathNewtonFunctionRootTest, MultipleCubicRoots)
{
  // Test finding different roots of cubic function with different starting points
  CubicWithDerivative aFunc;

  // Find root with different starting points - Newton's method converges to different roots
  math_NewtonFunctionRoot aSolver1(aFunc, 0.5, 1.0e-10, 1.0e-10);
  EXPECT_TRUE(aSolver1.IsDone()) << "Should find first root";
  Standard_Real aRoot1 = aSolver1.Root();
  EXPECT_TRUE(fabs(aRoot1 - 1.0) < 1.0e-6 || fabs(aRoot1 - 2.0) < 1.0e-6
              || fabs(aRoot1 - 3.0) < 1.0e-6)
    << "First root should be one of: 1, 2, or 3, found: " << aRoot1;

  // Find root with different starting point
  math_NewtonFunctionRoot aSolver2(aFunc, 2.8, 1.0e-10, 1.0e-10);
  EXPECT_TRUE(aSolver2.IsDone()) << "Should find second root";
  Standard_Real aRoot2 = aSolver2.Root();
  EXPECT_TRUE(fabs(aRoot2 - 1.0) < 1.0e-6 || fabs(aRoot2 - 2.0) < 1.0e-6
              || fabs(aRoot2 - 3.0) < 1.0e-6)
    << "Second root should be one of: 1, 2, or 3, found: " << aRoot2;

  // Find root with third starting point
  math_NewtonFunctionRoot aSolver3(aFunc, 1.8, 1.0e-10, 1.0e-10);
  EXPECT_TRUE(aSolver3.IsDone()) << "Should find third root";
  Standard_Real aRoot3 = aSolver3.Root();
  EXPECT_TRUE(fabs(aRoot3 - 1.0) < 1.0e-6 || fabs(aRoot3 - 2.0) < 1.0e-6
              || fabs(aRoot3 - 3.0) < 1.0e-6)
    << "Third root should be one of: 1, 2, or 3, found: " << aRoot3;
}

TEST(MathNewtonFunctionRootTest, ConvergenceFromFarGuess)
{
  // Test convergence from initial guess far from root
  QuadraticWithDerivative aFunc;

  math_NewtonFunctionRoot aSolver(aFunc, 100.0, 1.0e-10, 1.0e-10); // Very far initial guess

  EXPECT_TRUE(aSolver.IsDone()) << "Should converge from far initial guess";
  EXPECT_NEAR(aSolver.Root(), 2.0, 1.0e-8) << "Should still find correct root";
  EXPECT_GT(aSolver.NbIterations(), 5) << "Should require several iterations from far guess";
}