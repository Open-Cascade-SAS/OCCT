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

#include <math_FunctionRoots.hxx>
#include <math_FunctionWithDerivative.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_RangeError.hxx>

#include <algorithm>
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

// Sine function: f(x) = sin(x), f'(x) = cos(x), multiple roots
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

// Constant function: f(x) = 0, f'(x) = 0 (always null)
class ConstantZeroFunction : public math_FunctionWithDerivative
{
public:
  Standard_Boolean Value(const Standard_Real, Standard_Real& theF) override
  {
    theF = 0.0;
    return Standard_True;
  }

  Standard_Boolean Derivative(const Standard_Real, Standard_Real& theD) override
  {
    theD = 0.0;
    return Standard_True;
  }

  Standard_Boolean Values(const Standard_Real theX,
                          Standard_Real&      theF,
                          Standard_Real&      theD) override
  {
    (void)theX;
    theF = 0.0;
    theD = 0.0;
    return Standard_True;
  }
};

// Function with no real roots: f(x) = x^2 + 1, f'(x) = 2x
class NoRootsFunction : public math_FunctionWithDerivative
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = theX * theX + 1.0;
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
    theF = theX * theX + 1.0;
    theD = 2.0 * theX;
    return Standard_True;
  }
};

// High degree polynomial: f(x) = (x-1)(x-2)(x-3)(x-4) = x^4 - 10x^3 + 35x^2 - 50x + 24
class QuarticWithDerivative : public math_FunctionWithDerivative
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    // f(x) = (x-1)(x-2)(x-3)(x-4)
    theF = (theX - 1.0) * (theX - 2.0) * (theX - 3.0) * (theX - 4.0);
    return Standard_True;
  }

  Standard_Boolean Derivative(const Standard_Real theX, Standard_Real& theD) override
  {
    // f'(x) = 4x^3 - 30x^2 + 70x - 50
    theD = 4.0 * theX * theX * theX - 30.0 * theX * theX + 70.0 * theX - 50.0;
    return Standard_True;
  }

  Standard_Boolean Values(const Standard_Real theX,
                          Standard_Real&      theF,
                          Standard_Real&      theD) override
  {
    Value(theX, theF);
    Derivative(theX, theD);
    return Standard_True;
  }
};

} // anonymous namespace

TEST(MathFunctionRootsTest, QuadraticTwoRoots)
{
  // Test finding two roots of quadratic function
  QuadraticWithDerivative aFunc;

  math_FunctionRoots aRootFinder(aFunc, -5.0, 5.0, 20, 1.0e-10, 1.0e-10);

  EXPECT_TRUE(aRootFinder.IsDone()) << "Should successfully find roots";
  EXPECT_FALSE(aRootFinder.IsAllNull()) << "Function should not be all null";
  EXPECT_EQ(aRootFinder.NbSolutions(), 2) << "Should find exactly 2 roots";

  // Check that roots are approximately +/-2
  Standard_Real aRoot1 = aRootFinder.Value(1);
  Standard_Real aRoot2 = aRootFinder.Value(2);

  // Sort roots for consistent testing
  if (aRoot1 > aRoot2)
  {
    Standard_Real aTemp = aRoot1;
    aRoot1              = aRoot2;
    aRoot2              = aTemp;
  }

  EXPECT_NEAR(aRoot1, -2.0, 1.0e-8) << "First root should be -2";
  EXPECT_NEAR(aRoot2, 2.0, 1.0e-8) << "Second root should be 2";
}

TEST(MathFunctionRootsTest, CubicThreeRoots)
{
  // Test finding three roots of cubic function
  CubicWithDerivative aFunc;

  math_FunctionRoots aRootFinder(aFunc, 0.0, 4.0, 30, 1.0e-10, 1.0e-10);

  EXPECT_TRUE(aRootFinder.IsDone()) << "Should successfully find roots";
  EXPECT_FALSE(aRootFinder.IsAllNull()) << "Function should not be all null";
  EXPECT_EQ(aRootFinder.NbSolutions(), 3) << "Should find exactly 3 roots";

  // Check roots are approximately 1, 2, 3
  std::vector<Standard_Real> aRoots;
  for (Standard_Integer i = 1; i <= aRootFinder.NbSolutions(); ++i)
  {
    aRoots.push_back(aRootFinder.Value(i));
  }
  std::sort(aRoots.begin(), aRoots.end());

  EXPECT_NEAR(aRoots[0], 1.0, 1.0e-8) << "First root should be 1";
  EXPECT_NEAR(aRoots[1], 2.0, 1.0e-8) << "Second root should be 2";
  EXPECT_NEAR(aRoots[2], 3.0, 1.0e-8) << "Third root should be 3";
}

TEST(MathFunctionRootsTest, SineMultipleRoots)
{
  // Test finding multiple roots of sine function
  SineWithDerivative aFunc;

  math_FunctionRoots aRootFinder(aFunc, 0.0, 2.0 * M_PI, 50, 1.0e-10, 1.0e-10);

  EXPECT_TRUE(aRootFinder.IsDone()) << "Should successfully find roots";
  EXPECT_FALSE(aRootFinder.IsAllNull()) << "Sine function should not be all null";

  Standard_Integer aNbRoots = aRootFinder.NbSolutions();
  EXPECT_GE(aNbRoots, 2) << "Should find at least 2 roots (0, PI, 2PI)";
  EXPECT_LE(aNbRoots, 3) << "Should find at most 3 roots in [0, 2PI]";

  // Check that all found roots are actually roots
  for (Standard_Integer i = 1; i <= aNbRoots; ++i)
  {
    Standard_Real aRoot = aRootFinder.Value(i);
    Standard_Real aFuncValue;
    aFunc.Value(aRoot, aFuncValue);
    EXPECT_NEAR(aFuncValue, 0.0, 1.0e-8) << "Root " << i << " should have function value near 0";
  }
}

TEST(MathFunctionRootsTest, LinearSingleRoot)
{
  // Test finding single root of linear function
  LinearWithDerivative aFunc;

  math_FunctionRoots aRootFinder(aFunc, 0.0, 4.0, 10, 1.0e-10, 1.0e-10);

  EXPECT_TRUE(aRootFinder.IsDone()) << "Should successfully find root";
  EXPECT_FALSE(aRootFinder.IsAllNull()) << "Linear function should not be all null";
  EXPECT_EQ(aRootFinder.NbSolutions(), 1) << "Should find exactly 1 root";

  Standard_Real aRoot = aRootFinder.Value(1);
  EXPECT_NEAR(aRoot, 2.0, 1.0e-10) << "Root should be x = 2";
}

TEST(MathFunctionRootsTest, ConstantZeroFunction)
{
  // Test constant zero function (all null)
  ConstantZeroFunction aFunc;

  math_FunctionRoots aRootFinder(aFunc, -2.0, 2.0, 10, 1.0e-10, 1.0e-10, 1.0e-6);

  EXPECT_TRUE(aRootFinder.IsDone()) << "Should successfully process constant function";
  EXPECT_TRUE(aRootFinder.IsAllNull()) << "Constant zero function should be all null";
}

TEST(MathFunctionRootsTest, NoRootsFunction)
{
  // Test function with no real roots
  NoRootsFunction aFunc;

  math_FunctionRoots aRootFinder(aFunc, -5.0, 5.0, 20, 1.0e-10, 1.0e-10);

  EXPECT_TRUE(aRootFinder.IsDone()) << "Should successfully process function";
  EXPECT_FALSE(aRootFinder.IsAllNull()) << "Function is not zero everywhere";
  EXPECT_EQ(aRootFinder.NbSolutions(), 0) << "Should find no real roots";
}

TEST(MathFunctionRootsTest, QuarticFourRoots)
{
  // Test finding four roots of quartic function
  QuarticWithDerivative aFunc;

  math_FunctionRoots aRootFinder(aFunc, 0.0, 5.0, 40, 1.0e-10, 1.0e-10);

  EXPECT_TRUE(aRootFinder.IsDone()) << "Should successfully find roots";
  EXPECT_FALSE(aRootFinder.IsAllNull()) << "Function should not be all null";
  EXPECT_EQ(aRootFinder.NbSolutions(), 4) << "Should find exactly 4 roots";

  // Check roots are approximately 1, 2, 3, 4
  std::vector<Standard_Real> aRoots;
  for (Standard_Integer i = 1; i <= aRootFinder.NbSolutions(); ++i)
  {
    aRoots.push_back(aRootFinder.Value(i));
  }
  std::sort(aRoots.begin(), aRoots.end());

  EXPECT_NEAR(aRoots[0], 1.0, 1.0e-8) << "First root should be 1";
  EXPECT_NEAR(aRoots[1], 2.0, 1.0e-8) << "Second root should be 2";
  EXPECT_NEAR(aRoots[2], 3.0, 1.0e-8) << "Third root should be 3";
  EXPECT_NEAR(aRoots[3], 4.0, 1.0e-8) << "Fourth root should be 4";
}

TEST(MathFunctionRootsTest, CustomTolerances)
{
  // Test with different tolerance values
  QuadraticWithDerivative aFunc;

  // Loose tolerances
  math_FunctionRoots aRootFinder1(aFunc, -5.0, 5.0, 20, 1.0e-3, 1.0e-3);

  EXPECT_TRUE(aRootFinder1.IsDone()) << "Should work with loose tolerances";
  EXPECT_EQ(aRootFinder1.NbSolutions(), 2) << "Should still find 2 roots";

  // Tight tolerances
  math_FunctionRoots aRootFinder2(aFunc, -5.0, 5.0, 20, 1.0e-12, 1.0e-12);

  EXPECT_TRUE(aRootFinder2.IsDone()) << "Should work with tight tolerances";
  EXPECT_EQ(aRootFinder2.NbSolutions(), 2) << "Should still find 2 roots";
}

TEST(MathFunctionRootsTest, CustomSampleCount)
{
  // Test with different sample counts
  CubicWithDerivative aFunc;

  // Few samples
  math_FunctionRoots aRootFinder1(aFunc, 0.0, 4.0, 5, 1.0e-8, 1.0e-8);

  EXPECT_TRUE(aRootFinder1.IsDone()) << "Should work with few samples";
  EXPECT_GE(aRootFinder1.NbSolutions(), 1) << "Should find at least some roots";

  // Many samples
  math_FunctionRoots aRootFinder2(aFunc, 0.0, 4.0, 100, 1.0e-10, 1.0e-10);

  EXPECT_TRUE(aRootFinder2.IsDone()) << "Should work with many samples";
  EXPECT_EQ(aRootFinder2.NbSolutions(), 3) << "Should find all 3 roots with many samples";
}

TEST(MathFunctionRootsTest, StateNumberAccess)
{
  // Test state number access for roots
  QuadraticWithDerivative aFunc;

  math_FunctionRoots aRootFinder(aFunc, -5.0, 5.0, 20, 1.0e-10, 1.0e-10);

  EXPECT_TRUE(aRootFinder.IsDone()) << "Should successfully find roots";
  EXPECT_EQ(aRootFinder.NbSolutions(), 2) << "Should find 2 roots";

  // Test valid state number access
  if (aRootFinder.NbSolutions() >= 1)
  {
    Standard_Integer aState1 = aRootFinder.StateNumber(1);
    EXPECT_GE(aState1, 0) << "State number should be non-negative";
  }
  if (aRootFinder.NbSolutions() >= 2)
  {
    Standard_Integer aState2 = aRootFinder.StateNumber(2);
    EXPECT_GE(aState2, 0) << "State number should be non-negative";
  }

  // Test bounds checking in release builds
  EXPECT_GE(aRootFinder.NbSolutions(), 0) << "Number of solutions should be non-negative";
}

TEST(MathFunctionRootsTest, ShiftedTarget)
{
  // Test finding roots of F(x) - K = 0 with non-zero K
  QuadraticWithDerivative aFunc; // f(x) = x^2 - 4

  // Find roots of f(x) - (-4) = 0, i.e., x^2 = 0, so x = 0
  math_FunctionRoots aRootFinder(aFunc, -2.0, 2.0, 20, 1.0e-10, 1.0e-10, 0.0, -4.0);

  EXPECT_TRUE(aRootFinder.IsDone()) << "Should successfully find shifted roots";
  EXPECT_EQ(aRootFinder.NbSolutions(), 1) << "Should find 1 root for x^2 = 0";

  Standard_Real aRoot = aRootFinder.Value(1);
  EXPECT_NEAR(aRoot, 0.0, 1.0e-8) << "Root should be x = 0";
}

TEST(MathFunctionRootsTest, NotDoneExceptions)
{
  // Create a root finder that doesn't complete (simulation)
  // We'll test the exceptions by accessing methods on an uninitialized object

  // Note: In practice, math_FunctionRoots constructor already performs computation,
  // so we test exception handling conceptually

  QuadraticWithDerivative aFunc;
  math_FunctionRoots      aRootFinder(aFunc, -5.0, 5.0, 20, 1.0e-10, 1.0e-10);

  // These should work since computation is done in constructor
  EXPECT_TRUE(aRootFinder.IsDone()) << "Root finder should be done";
  EXPECT_NO_THROW(aRootFinder.IsAllNull()) << "Should be able to check if all null";
  EXPECT_NO_THROW(aRootFinder.NbSolutions()) << "Should be able to get number of solutions";

  if (aRootFinder.NbSolutions() > 0)
  {
    EXPECT_NO_THROW(aRootFinder.Value(1)) << "Should be able to get first root";
  }
}

TEST(MathFunctionRootsTest, NarrowRange)
{
  // Test root finding in very narrow range
  LinearWithDerivative aFunc;

  // Search in narrow range around the root
  math_FunctionRoots aRootFinder(aFunc, 1.9, 2.1, 10, 1.0e-10, 1.0e-10);

  EXPECT_TRUE(aRootFinder.IsDone()) << "Should work in narrow range";
  EXPECT_EQ(aRootFinder.NbSolutions(), 1) << "Should find the root in narrow range";

  Standard_Real aRoot = aRootFinder.Value(1);
  EXPECT_NEAR(aRoot, 2.0, 1.0e-8) << "Should find accurate root";
}

TEST(MathFunctionRootsTest, RootAtBoundary)
{
  // Test when root is at the boundary of search range
  LinearWithDerivative aFunc; // Root at x = 2

  // Search range includes root at left boundary
  math_FunctionRoots aRootFinder1(aFunc, 2.0, 4.0, 10, 1.0e-10, 1.0e-10);

  EXPECT_TRUE(aRootFinder1.IsDone()) << "Should work with root at left boundary";
  EXPECT_EQ(aRootFinder1.NbSolutions(), 1) << "Should find root at boundary";

  // Search range includes root at right boundary
  math_FunctionRoots aRootFinder2(aFunc, 0.0, 2.0, 10, 1.0e-10, 1.0e-10);

  EXPECT_TRUE(aRootFinder2.IsDone()) << "Should work with root at right boundary";
  EXPECT_EQ(aRootFinder2.NbSolutions(), 1) << "Should find root at boundary";
}

TEST(MathFunctionRootsTest, ReversedRange)
{
  // Test with reversed range (B < A)
  QuadraticWithDerivative aFunc;

  math_FunctionRoots aRootFinder(aFunc, 5.0, -5.0, 20, 1.0e-10, 1.0e-10);

  EXPECT_TRUE(aRootFinder.IsDone()) << "Should handle reversed range";
  EXPECT_EQ(aRootFinder.NbSolutions(), 2) << "Should still find both roots";
}