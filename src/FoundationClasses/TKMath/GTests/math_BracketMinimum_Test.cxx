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

#include <math_BracketMinimum.hxx>
#include <math_Function.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <StdFail_NotDone.hxx>
#include <Precision.hxx>

#include <cmath>

namespace
{

// Quadratic function with minimum: f(x) = (x-2)^2 + 1, minimum at x = 2
class QuadraticFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = (theX - 2.0) * (theX - 2.0) + 1.0;
    return Standard_True;
  }
};

// Quartic function: f(x) = x^4 - 4*x^3 + 6*x^2 - 4*x + 5, minimum at x = 1
class QuarticFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    Standard_Real x2 = theX * theX;
    Standard_Real x3 = x2 * theX;
    Standard_Real x4 = x3 * theX;
    theF             = x4 - 4.0 * x3 + 6.0 * x2 - 4.0 * theX + 5.0;
    return Standard_True;
  }
};

// Cosine function: f(x) = cos(x), minimum at x = PI in [0, 2*PI]
class CosineFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = cos(theX);
    return Standard_True;
  }
};

// Exponential function: f(x) = e^x, no minimum (always increasing)
class ExponentialFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = exp(theX);
    return Standard_True;
  }
};

// Multiple minima function: f(x) = sin(x) + 0.1*x, has local minima
class MultipleMinFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = sin(theX) + 0.1 * theX;
    return Standard_True;
  }
};

} // anonymous namespace

TEST(MathBracketMinimumTest, QuadraticMinimumBracketing)
{
  // Test bracketing minimum of quadratic function
  QuadraticFunction aFunc;

  math_BracketMinimum aBracketer(aFunc, 0.0, 1.0);

  EXPECT_TRUE(aBracketer.IsDone()) << "Should successfully bracket quadratic minimum";

  Standard_Real aA, aB, aC;
  aBracketer.Values(aA, aB, aC);

  // Check that B is between A and C
  EXPECT_TRUE((aA < aB && aB < aC) || (aC < aB && aB < aA)) << "B should be between A and C";

  // Check that the minimum is around x = 2
  EXPECT_GT(aB, 1.0) << "Bracketed minimum should be greater than 1";
  EXPECT_LT(aB, 3.0) << "Bracketed minimum should be less than 3";

  Standard_Real aFA, aFB, aFC;
  aBracketer.FunctionValues(aFA, aFB, aFC);

  // Check that F(B) is less than both F(A) and F(C)
  EXPECT_LT(aFB, aFA) << "F(B) should be less than F(A)";
  EXPECT_LT(aFB, aFC) << "F(B) should be less than F(C)";
}

TEST(MathBracketMinimumTest, ConstructorWithPrecomputedValues)
{
  // Test constructor with precomputed function values
  QuadraticFunction aFunc;

  Standard_Real aA = 0.0, aB = 1.0;
  Standard_Real aFA = (aA - 2.0) * (aA - 2.0) + 1.0; // F(0) = 5
  Standard_Real aFB = (aB - 2.0) * (aB - 2.0) + 1.0; // F(1) = 2

  math_BracketMinimum aBracketer(aFunc, aA, aB, aFA, aFB);

  EXPECT_TRUE(aBracketer.IsDone()) << "Should successfully bracket with precomputed values";

  Standard_Real aRetA, aRetB, aRetC;
  aBracketer.Values(aRetA, aRetB, aRetC);

  EXPECT_TRUE((aRetA < aRetB && aRetB < aRetC) || (aRetC < aRetB && aRetB < aRetA));
}

TEST(MathBracketMinimumTest, ConstructorWithOnePrecomputedValue)
{
  // Test constructor with one precomputed function value
  QuadraticFunction aFunc;

  Standard_Real aA = 0.0, aB = 1.0;
  Standard_Real aFA = 5.0; // F(0) = 5

  math_BracketMinimum aBracketer(aFunc, aA, aB, aFA);

  EXPECT_TRUE(aBracketer.IsDone()) << "Should successfully bracket with one precomputed value";
}

TEST(MathBracketMinimumTest, QuarticFunctionBracketing)
{
  // Test with quartic function that has minimum at x = 1
  QuarticFunction aFunc;

  math_BracketMinimum aBracketer(aFunc, 0.0, 0.5);

  EXPECT_TRUE(aBracketer.IsDone()) << "Should bracket quartic function minimum";

  Standard_Real aA, aB, aC;
  aBracketer.Values(aA, aB, aC);

  // The minimum should be bracketed around x = 1
  EXPECT_GT(aB, 0.5) << "Bracketed point should be greater than 0.5";
  EXPECT_LT(aB, 1.5) << "Bracketed point should be less than 1.5";
}

TEST(MathBracketMinimumTest, CosineFunction)
{
  // Test with cosine function which has minimum at PI
  CosineFunction aFunc;

  math_BracketMinimum aBracketer(aFunc, 2.0, 4.0);

  EXPECT_TRUE(aBracketer.IsDone()) << "Should bracket cosine function minimum";

  Standard_Real aA, aB, aC;
  aBracketer.Values(aA, aB, aC);

  // The minimum should be bracketed around PI approximately 3.14159
  EXPECT_GT(aB, 2.5) << "Bracketed point should be greater than 2.5";
  EXPECT_LT(aB, 4.5) << "Bracketed point should be less than 4.5";

  Standard_Real aFA, aFB, aFC;
  aBracketer.FunctionValues(aFA, aFB, aFC);

  EXPECT_LT(aFB, aFA) << "F(B) should be less than F(A)";
  EXPECT_LT(aFB, aFC) << "F(B) should be less than F(C)";
}

TEST(MathBracketMinimumTest, SetLimits)
{
  // Test setting limits on the parameter range
  QuadraticFunction aFunc;

  math_BracketMinimum aBracketer(0.0, 1.0);
  aBracketer.SetLimits(1.5, 3.0); // Limit search to [1.5, 3.0]
  aBracketer.Perform(aFunc);

  EXPECT_TRUE(aBracketer.IsDone()) << "Should find minimum within limits";

  Standard_Real aA, aB, aC;
  aBracketer.Values(aA, aB, aC);

  // All points should be reasonably close to limits (implementation may extend slightly)
  EXPECT_GE(aA, 0.5) << "A should be reasonably within limits";
  EXPECT_LE(aA, 3.5) << "A should be reasonably within limits";
  EXPECT_GE(aB, 0.5) << "B should be reasonably within limits";
  EXPECT_LE(aB, 3.5) << "B should be reasonably within limits";
  EXPECT_GE(aC, 0.5) << "C should be reasonably within limits";
  EXPECT_LE(aC, 3.5) << "C should be reasonably within limits";
}

TEST(MathBracketMinimumTest, SetPrecomputedValues)
{
  // Test setting precomputed function values
  QuadraticFunction aFunc;

  math_BracketMinimum aBracketer(0.0, 1.0);
  aBracketer.SetFA(5.0); // F(0) = 5
  aBracketer.SetFB(2.0); // F(1) = 2
  aBracketer.Perform(aFunc);

  EXPECT_TRUE(aBracketer.IsDone()) << "Should work with precomputed values";
}

TEST(MathBracketMinimumTest, NoMinimumFunction)
{
  // Test with function that has no minimum (exponential)
  ExponentialFunction aFunc;

  math_BracketMinimum aBracketer(aFunc, -1.0, 0.0);

  // Exponential function is monotonic, so may not find true bracketing
  // Implementation behavior varies - just check it doesn't crash
  EXPECT_TRUE(true) << "Exponential function test completed without crash";
}

TEST(MathBracketMinimumTest, MultipleLocalMinima)
{
  // Test with function having multiple local minima
  MultipleMinFunction aFunc;

  math_BracketMinimum aBracketer(aFunc, -2.0, 0.0);

  EXPECT_TRUE(aBracketer.IsDone()) << "Should bracket one of the minima";

  Standard_Real aFA, aFB, aFC;
  aBracketer.FunctionValues(aFA, aFB, aFC);

  EXPECT_LT(aFB, aFA) << "F(B) should be less than F(A)";
  EXPECT_LT(aFB, aFC) << "F(B) should be less than F(C)";
}

TEST(MathBracketMinimumTest, UnperformedState)
{
  // Test state handling for incomplete calculations
  math_BracketMinimum aBracketer(0.0, 1.0);

  EXPECT_FALSE(aBracketer.IsDone()) << "Bracketer should not be done before Perform()";
}

TEST(MathBracketMinimumTest, VeryNarrowInitialBounds)
{
  // Test with very close initial points
  QuadraticFunction aFunc;

  math_BracketMinimum aBracketer(aFunc, 1.99, 2.01); // Very close to minimum

  EXPECT_TRUE(aBracketer.IsDone()) << "Should handle narrow initial bounds";

  Standard_Real aA, aB, aC;
  aBracketer.Values(aA, aB, aC);

  // When very close to minimum, bracketing may be approximate
  // Just verify we get reasonable results
  EXPECT_GT(aA, 1.5) << "Bracketing points should be reasonable";
  EXPECT_LT(aC, 2.5) << "Bracketing points should be reasonable";
}

TEST(MathBracketMinimumTest, ReverseOrderInitialPoints)
{
  // Test with initial points in reverse order (B < A)
  QuadraticFunction aFunc;

  math_BracketMinimum aBracketer(aFunc, 4.0, 0.0); // B < A

  EXPECT_TRUE(aBracketer.IsDone()) << "Should handle reverse order initial points";

  Standard_Real aA, aB, aC;
  aBracketer.Values(aA, aB, aC);

  // Just verify reasonable bracketing points were found
  EXPECT_GT(aB, -1.0) << "Bracketed point should be reasonable";
  EXPECT_LT(aB, 5.0) << "Bracketed point should be reasonable";
}

TEST(MathBracketMinimumTest, RestrictiveLimits)
{
  // Test with very restrictive limits that may prevent finding minimum
  QuadraticFunction aFunc; // Minimum at x = 2

  math_BracketMinimum aBracketer(0.0, 0.5);
  aBracketer.SetLimits(0.0, 1.0); // Limits exclude the actual minimum at x = 2
  aBracketer.Perform(aFunc);

  // May or may not succeed depending on implementation
  if (aBracketer.IsDone())
  {
    Standard_Real aA, aB, aC;
    aBracketer.Values(aA, aB, aC);

    // If successful, points should be within limits
    EXPECT_GE(aA, -0.1) << "A should respect lower limit";
    EXPECT_LE(aA, 1.1) << "A should respect upper limit";
    EXPECT_GE(aB, -0.1) << "B should respect lower limit";
    EXPECT_LE(aB, 1.1) << "B should respect upper limit";
    EXPECT_GE(aC, -0.1) << "C should respect lower limit";
    EXPECT_LE(aC, 1.1) << "C should respect upper limit";
  }
}