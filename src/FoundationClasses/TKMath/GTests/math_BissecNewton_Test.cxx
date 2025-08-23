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

#include <math_BissecNewton.hxx>
#include <math_FunctionWithDerivative.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <StdFail_NotDone.hxx>
#include <Precision.hxx>

#include <cmath>

namespace
{

// Quadratic function with derivative: f(x) = (x-2)^2 - 1, f'(x) = 2(x-2)
class QuadraticWithDerivative : public math_FunctionWithDerivative
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = (theX - 2.0) * (theX - 2.0) - 1.0;
    return Standard_True;
  }

  Standard_Boolean Derivative(const Standard_Real theX, Standard_Real& theD) override
  {
    theD = 2.0 * (theX - 2.0);
    return Standard_True;
  }

  Standard_Boolean Values(const Standard_Real theX,
                          Standard_Real&      theF,
                          Standard_Real&      theD) override
  {
    theF = (theX - 2.0) * (theX - 2.0) - 1.0;
    theD = 2.0 * (theX - 2.0);
    return Standard_True;
  }
};

// Cubic function: f(x) = x^3 - x - 2, f'(x) = 3x^2 - 1
// Root at x approximately 1.521 (exact root of x^3 - x - 2 = 0)
class CubicWithDerivative : public math_FunctionWithDerivative
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = theX * theX * theX - theX - 2.0;
    return Standard_True;
  }

  Standard_Boolean Derivative(const Standard_Real theX, Standard_Real& theD) override
  {
    theD = 3.0 * theX * theX - 1.0;
    return Standard_True;
  }

  Standard_Boolean Values(const Standard_Real theX,
                          Standard_Real&      theF,
                          Standard_Real&      theD) override
  {
    theF = theX * theX * theX - theX - 2.0;
    theD = 3.0 * theX * theX - 1.0;
    return Standard_True;
  }
};

// Sine function: f(x) = sin(x), f'(x) = cos(x)
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

// Function with zero derivative: f(x) = x^3, f'(x) = 3x^2 (derivative zero at x=0)
class CubicZeroDerivative : public math_FunctionWithDerivative
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

} // anonymous namespace

TEST(MathBissecNewtonTest, QuadraticRootFinding)
{
  // Test finding roots of quadratic function (x-2)^2 - 1 = 0
  // Roots are at x = 1 and x = 3
  QuadraticWithDerivative aFunc;

  math_BissecNewton aSolver(1.0e-10);
  aSolver.Perform(aFunc, 0.0, 1.5, 100); // Find root near x = 1

  EXPECT_TRUE(aSolver.IsDone()) << "Should find root for quadratic function";
  EXPECT_NEAR(aSolver.Root(), 1.0, 1.0e-8) << "Root should be x = 1";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-10) << "Function value at root should be zero";
  EXPECT_NEAR(aSolver.Derivative(), -2.0, 1.0e-8) << "Derivative at x=1 should be -2";
}

TEST(MathBissecNewtonTest, QuadraticSecondRoot)
{
  // Test finding the second root of the same quadratic function
  QuadraticWithDerivative aFunc;

  math_BissecNewton aSolver(1.0e-10);
  aSolver.Perform(aFunc, 2.5, 4.0, 100); // Find root near x = 3

  EXPECT_TRUE(aSolver.IsDone()) << "Should find second root for quadratic function";
  EXPECT_NEAR(aSolver.Root(), 3.0, 1.0e-8) << "Root should be x = 3";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-10) << "Function value at root should be zero";
  EXPECT_NEAR(aSolver.Derivative(), 2.0, 1.0e-8) << "Derivative at x=3 should be 2";
}

TEST(MathBissecNewtonTest, CubicRootFinding)
{
  // Test finding root of cubic function x^3 - x - 2 = 0
  CubicWithDerivative aFunc;

  math_BissecNewton aSolver(1.0e-10);
  aSolver.Perform(aFunc, 1.0, 2.0, 100); // Root is approximately x = 1.521

  EXPECT_TRUE(aSolver.IsDone()) << "Should find root for cubic function";
  Standard_Real aRoot = aSolver.Root();
  EXPECT_GT(aRoot, 1.52) << "Root should be greater than 1.52";
  EXPECT_LT(aRoot, 1.53) << "Root should be less than 1.53";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-8) << "Function value at root should be near zero";
}

TEST(MathBissecNewtonTest, SineFunctionRoot)
{
  // Test finding root of sin(x) = 0 near PI
  SineWithDerivative aFunc;

  math_BissecNewton aSolver(1.0e-10);
  aSolver.Perform(aFunc, 3.0, 3.5, 100); // Find root near PI approximately 3.14159

  EXPECT_TRUE(aSolver.IsDone()) << "Should find root for sine function";
  EXPECT_NEAR(aSolver.Root(), M_PI, 1.0e-8) << "Root should be PI";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-10) << "Function value at root should be zero";
  EXPECT_NEAR(aSolver.Derivative(), -1.0, 1.0e-8) << "cos(PI) should be -1";
}

TEST(MathBissecNewtonTest, CustomTolerance)
{
  // Test with different tolerance values
  QuadraticWithDerivative aFunc;

  // Loose tolerance
  math_BissecNewton aSolver1(1.0e-3);
  aSolver1.Perform(aFunc, 0.5, 1.5, 100);

  EXPECT_TRUE(aSolver1.IsDone()) << "Should converge with loose tolerance";
  EXPECT_NEAR(aSolver1.Root(), 1.0, 1.0e-2) << "Root should be approximately correct";

  // Tight tolerance
  math_BissecNewton aSolver2(1.0e-12);
  aSolver2.Perform(aFunc, 0.5, 1.5, 100);

  EXPECT_TRUE(aSolver2.IsDone()) << "Should converge with tight tolerance";
  EXPECT_NEAR(aSolver2.Root(), 1.0, 1.0e-10) << "Root should be very accurate";
}

TEST(MathBissecNewtonTest, IterationLimit)
{
  // Test behavior with limited iterations
  CubicWithDerivative aFunc;

  math_BissecNewton aSolver(1.0e-12);
  aSolver.Perform(aFunc, 1.0, 2.0, 5); // Very few iterations

  // May or may not converge with so few iterations
  if (aSolver.IsDone())
  {
    EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-3) << "If converged, should be reasonably accurate";
  }
}

TEST(MathBissecNewtonTest, InvalidBounds)
{
  // Test with bounds that don't bracket a root
  QuadraticWithDerivative aFunc;

  math_BissecNewton aSolver(1.0e-10);
  aSolver.Perform(aFunc, 1.5, 2.5, 100); // Both bounds give positive function values

  // Function may fail to find root or find the nearby root at x=3
  if (aSolver.IsDone())
  {
    Standard_Real aRoot = aSolver.Root();
    EXPECT_GT(aRoot, 1.4) << "If found, root should be reasonable";
    EXPECT_LT(aRoot, 3.1) << "If found, root should be reasonable";
  }
}

TEST(MathBissecNewtonTest, ZeroDerivativeHandling)
{
  // Test function where derivative can be zero (f(x) = x^3 has f'(0) = 0)
  CubicZeroDerivative aFunc;

  math_BissecNewton aSolver(1.0e-10);
  aSolver.Perform(aFunc, -0.5, 0.5, 100); // Root at x = 0

  EXPECT_TRUE(aSolver.IsDone()) << "Should handle zero derivative case";
  EXPECT_NEAR(aSolver.Root(), 0.0, 1.0e-8) << "Root should be x = 0";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-10) << "Function value at root should be zero";
  EXPECT_NEAR(aSolver.Derivative(), 0.0, 1.0e-8) << "Derivative at x=0 should be zero";
}

TEST(MathBissecNewtonTest, NotDoneState)
{
  // Test state handling for incomplete calculations
  math_BissecNewton aSolver(1.0e-10);

  EXPECT_FALSE(aSolver.IsDone()) << "Solver should not be done before Perform()";
}

TEST(MathBissecNewtonTest, MultipleCalls)
{
  // Test multiple calls to Perform with the same solver instance
  QuadraticWithDerivative aFunc;
  math_BissecNewton       aSolver(1.0e-10);

  // First call
  aSolver.Perform(aFunc, 0.0, 1.5, 100);
  EXPECT_TRUE(aSolver.IsDone()) << "First call should succeed";
  EXPECT_NEAR(aSolver.Root(), 1.0, 1.0e-8) << "First root should be x = 1";

  // Second call with different bounds
  aSolver.Perform(aFunc, 2.5, 4.0, 100);
  EXPECT_TRUE(aSolver.IsDone()) << "Second call should succeed";
  EXPECT_NEAR(aSolver.Root(), 3.0, 1.0e-8) << "Second root should be x = 3";
}

TEST(MathBissecNewtonTest, HighPrecisionRequirement)
{
  // Test with extremely tight tolerance
  SineWithDerivative aFunc;
  math_BissecNewton  aSolver(1.0e-15);

  aSolver.Perform(aFunc, 3.0, 3.5, 200);

  EXPECT_TRUE(aSolver.IsDone()) << "Should converge with high precision requirement";
  EXPECT_NEAR(aSolver.Root(), M_PI, 1.0e-12) << "Root should be very accurate";
}

TEST(MathBissecNewtonTest, EdgeCaseBounds)
{
  // Test with bounds very close to the root
  QuadraticWithDerivative aFunc;
  math_BissecNewton       aSolver(1.0e-10);

  aSolver.Perform(aFunc, 0.99, 1.01, 100); // Very narrow bounds around x = 1

  EXPECT_TRUE(aSolver.IsDone()) << "Should find root with narrow bounds";
  EXPECT_NEAR(aSolver.Root(), 1.0, 1.0e-8) << "Root should be accurate";
}