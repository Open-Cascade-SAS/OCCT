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

#include <math_BracketedRoot.hxx>
#include <math_Function.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <StdFail_NotDone.hxx>
#include <Precision.hxx>

#include <cmath>

namespace
{

// Quadratic function: f(x) = (x-2)^2 - 1, roots at x = 1 and x = 3
class QuadraticFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = (theX - 2.0) * (theX - 2.0) - 1.0;
    return Standard_True;
  }
};

// Cubic function: f(x) = x^3 - x - 2, root at x approximately 1.521
class CubicFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = theX * theX * theX - theX - 2.0;
    return Standard_True;
  }
};

// Sine function: f(x) = sin(x), root at x = PI
class SineFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = sin(theX);
    return Standard_True;
  }
};

// Linear function: f(x) = 2x - 4, root at x = 2
class LinearFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = 2.0 * theX - 4.0;
    return Standard_True;
  }
};

} // anonymous namespace

TEST(MathBracketedRootTest, QuadraticRootFinding)
{
  // Test finding root of quadratic function between x = 0 and x = 1.5
  QuadraticFunction aFunc;

  math_BracketedRoot aSolver(aFunc, 0.0, 1.5, 1.0e-10);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find root for quadratic function";
  EXPECT_NEAR(aSolver.Root(), 1.0, 1.0e-8) << "Root should be x = 1";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-9) << "Function value at root should be near zero";
  EXPECT_GT(aSolver.NbIterations(), 0) << "Should have performed some iterations";
}

TEST(MathBracketedRootTest, QuadraticSecondRoot)
{
  // Test finding the second root of quadratic function between x = 2.5 and x = 4
  QuadraticFunction aFunc;

  math_BracketedRoot aSolver(aFunc, 2.5, 4.0, 1.0e-10);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find second root for quadratic function";
  EXPECT_NEAR(aSolver.Root(), 3.0, 1.0e-8) << "Root should be x = 3";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-9) << "Function value at root should be near zero";
}

TEST(MathBracketedRootTest, CubicRootFinding)
{
  // Test finding root of cubic function x^3 - x - 2 = 0
  CubicFunction aFunc;

  math_BracketedRoot aSolver(aFunc, 1.0, 2.0, 1.0e-10);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find root for cubic function";
  Standard_Real aRoot = aSolver.Root();
  EXPECT_GT(aRoot, 1.52) << "Root should be approximately 1.521";
  EXPECT_LT(aRoot, 1.53) << "Root should be approximately 1.521";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-8) << "Function value at root should be near zero";
}

TEST(MathBracketedRootTest, SineFunctionRoot)
{
  // Test finding root of sin(x) = 0 near PI
  SineFunction aFunc;

  math_BracketedRoot aSolver(aFunc, 3.0, 3.5, 1.0e-10);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find root for sine function";
  EXPECT_NEAR(aSolver.Root(), M_PI, 1.0e-8) << "Root should be PI";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-9) << "Function value at root should be near zero";
}

TEST(MathBracketedRootTest, LinearFunctionRoot)
{
  // Test finding root of linear function 2x - 4 = 0
  LinearFunction aFunc;

  math_BracketedRoot aSolver(aFunc, 1.0, 3.0, 1.0e-10);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find root for linear function";
  EXPECT_NEAR(aSolver.Root(), 2.0, 1.0e-12) << "Root should be x = 2 (exact)";
  EXPECT_NEAR(aSolver.Value(), 0.0, 1.0e-12) << "Function value should be exactly zero";
}

TEST(MathBracketedRootTest, CustomTolerance)
{
  // Test with different tolerance values
  QuadraticFunction aFunc;

  // Loose tolerance
  math_BracketedRoot aSolver1(aFunc, 0.5, 1.5, 1.0e-3);
  EXPECT_TRUE(aSolver1.IsDone()) << "Should converge with loose tolerance";
  EXPECT_NEAR(aSolver1.Root(), 1.0, 1.0e-2) << "Root should be approximately correct";

  // Tight tolerance
  math_BracketedRoot aSolver2(aFunc, 0.5, 1.5, 1.0e-12);
  EXPECT_TRUE(aSolver2.IsDone()) << "Should converge with tight tolerance";
  EXPECT_NEAR(aSolver2.Root(), 1.0, 1.0e-10) << "Root should be very accurate";
}

TEST(MathBracketedRootTest, CustomIterationLimit)
{
  // Test with custom iteration limits
  CubicFunction aFunc;

  // Very few iterations
  math_BracketedRoot aSolver1(aFunc, 1.0, 2.0, 1.0e-12, 5);
  if (aSolver1.IsDone())
  {
    EXPECT_LE(aSolver1.NbIterations(), 5) << "Should respect iteration limit";
  }

  // Many iterations
  math_BracketedRoot aSolver2(aFunc, 1.0, 2.0, 1.0e-15, 200);
  EXPECT_TRUE(aSolver2.IsDone()) << "Should converge with many iterations allowed";
}

TEST(MathBracketedRootTest, InvalidBounds)
{
  // Test with bounds that don't bracket a root (same sign function values)
  QuadraticFunction aFunc; // f(x) = (x-2)^2 - 1

  // Both bounds give positive function values: f(3.5) > 0, f(4) > 0
  math_BracketedRoot aSolver(aFunc, 3.5, 4.0, 1.0e-10);

  EXPECT_FALSE(aSolver.IsDone()) << "Should fail when bounds don't bracket root";
}

TEST(MathBracketedRootTest, ZeroAtBoundary)
{
  // Test when the root is exactly at one of the boundaries
  LinearFunction aFunc; // f(x) = 2x - 4, root at x = 2

  math_BracketedRoot aSolver(aFunc, 2.0, 3.0, 1.0e-10);

  EXPECT_TRUE(aSolver.IsDone()) << "Should handle root at boundary";
  EXPECT_NEAR(aSolver.Root(), 2.0, 1.0e-12) << "Should find root at boundary";
}

TEST(MathBracketedRootTest, VeryNarrowBounds)
{
  // Test with very narrow bracketing interval
  LinearFunction aFunc;

  math_BracketedRoot aSolver(aFunc, 1.999, 2.001, 1.0e-10);

  EXPECT_TRUE(aSolver.IsDone()) << "Should handle narrow bounds";
  EXPECT_NEAR(aSolver.Root(), 2.0, 1.0e-8) << "Root should be accurate";
}

TEST(MathBracketedRootTest, ReverseBounds)
{
  // Test with bounds in reverse order (Bound2 < Bound1)
  QuadraticFunction aFunc;

  math_BracketedRoot aSolver(aFunc, 1.5, 0.0, 1.0e-10); // Reversed bounds

  EXPECT_TRUE(aSolver.IsDone()) << "Should handle reverse bounds";
  EXPECT_NEAR(aSolver.Root(), 1.0, 1.0e-8) << "Should still find correct root";
}

TEST(MathBracketedRootTest, NotDoneState)
{
  // Test state handling for incomplete calculations
  // Create solver with invalid bounds to force failure
  QuadraticFunction  aFunc;
  math_BracketedRoot aSolver(aFunc, 3.5, 4.0, 1.0e-10); // No root in interval

  EXPECT_FALSE(aSolver.IsDone()) << "Should not be done for invalid bounds";
}

TEST(MathBracketedRootTest, HighPrecisionRequirement)
{
  // Test with extremely tight tolerance
  SineFunction       aFunc;
  math_BracketedRoot aSolver(aFunc, 3.0, 3.5, 1.0e-15, 200);

  EXPECT_TRUE(aSolver.IsDone()) << "Should converge with high precision requirement";
  EXPECT_NEAR(aSolver.Root(), M_PI, 1.0e-10) << "Root should be very accurate";
  EXPECT_GT(aSolver.NbIterations(), 5) << "Should require several iterations for high precision";
}

TEST(MathBracketedRootTest, CustomZEPS)
{
  // Test with custom ZEPS parameter (machine epsilon)
  QuadraticFunction aFunc;

  math_BracketedRoot aSolver(aFunc, 0.5, 1.5, 1.0e-10, 100, 1.0e-15);

  EXPECT_TRUE(aSolver.IsDone()) << "Should work with custom ZEPS";
  EXPECT_NEAR(aSolver.Root(), 1.0, 1.0e-8) << "Root should be accurate";
}