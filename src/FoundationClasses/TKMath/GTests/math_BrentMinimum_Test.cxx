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

#include <math_BrentMinimum.hxx>
#include <math_Function.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <StdFail_NotDone.hxx>
#include <Precision.hxx>

#include <cmath>

namespace
{

// Quadratic function: f(x) = (x-2)^2 + 1, minimum at x = 2 with value 1
class QuadraticFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = (theX - 2.0) * (theX - 2.0) + 1.0;
    return Standard_True;
  }
};

// Quartic function: f(x) = (x-1)^4 + 2, minimum at x = 1 with value 2
class QuarticFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    Standard_Real dx = theX - 1.0;
    theF             = dx * dx * dx * dx + 2.0;
    return Standard_True;
  }
};

// Cosine function: f(x) = cos(x), minimum at x = PI with value -1
class CosineFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = cos(theX);
    return Standard_True;
  }
};

// Shifted exponential: f(x) = e^(x-3), minimum approaches x = -infinity
class ShiftedExponentialFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = exp(theX - 3.0);
    return Standard_True;
  }
};

// Rosenbrock 1D slice: f(x) = (1-x)^2 + 100*(x-x^2)^2 for fixed y
class RosenbrockSliceFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    Standard_Real dx = 1.0 - theX;
    Standard_Real dy = theX - theX * theX;
    theF             = dx * dx + 100.0 * dy * dy;
    return Standard_True;
  }
};

} // anonymous namespace

TEST(MathBrentMinimumTest, QuadraticMinimumFinding)
{
  // Test finding minimum of quadratic function
  QuadraticFunction aFunc;

  math_BrentMinimum aSolver(1.0e-10);
  aSolver.Perform(aFunc, 0.0, 1.5, 4.0); // Bracketing triplet around minimum at x=2

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum for quadratic function";
  EXPECT_NEAR(aSolver.Location(), 2.0, 1.0e-8) << "Minimum should be at x = 2";
  EXPECT_NEAR(aSolver.Minimum(), 1.0, 1.0e-10) << "Minimum value should be 1";
  EXPECT_GT(aSolver.NbIterations(), 0) << "Should have performed some iterations";
}

TEST(MathBrentMinimumTest, QuarticMinimumFinding)
{
  // Test with quartic function that has flat minimum
  QuarticFunction aFunc;

  math_BrentMinimum aSolver(1.0e-10);
  aSolver.Perform(aFunc, 0.0, 0.8, 2.0); // Bracketing triplet around minimum at x=1

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum for quartic function";
  EXPECT_NEAR(aSolver.Location(), 1.0, 1.0e-4) << "Minimum should be at x = 1";
  EXPECT_NEAR(aSolver.Minimum(), 2.0, 1.0e-6) << "Minimum value should be 2";
}

TEST(MathBrentMinimumTest, CosineMinimumFinding)
{
  // Test with cosine function, minimum at PI
  CosineFunction aFunc;

  math_BrentMinimum aSolver(1.0e-10);
  aSolver.Perform(aFunc, 2.5, 3.1, 4.0); // Bracketing triplet around minimum at PI

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum for cosine function";
  EXPECT_NEAR(aSolver.Location(), M_PI, 1.0e-8) << "Minimum should be at PI";
  EXPECT_NEAR(aSolver.Minimum(), -1.0, 1.0e-10) << "Minimum value should be -1";
}

TEST(MathBrentMinimumTest, ConstructorWithKnownValue)
{
  // Test constructor when F(Bx) is known
  QuadraticFunction aFunc;
  Standard_Real     Bx  = 1.5;
  Standard_Real     Fbx = (Bx - 2.0) * (Bx - 2.0) + 1.0; // F(1.5) = 1.25

  math_BrentMinimum aSolver(1.0e-10, Fbx);
  aSolver.Perform(aFunc, 0.0, Bx, 4.0);

  EXPECT_TRUE(aSolver.IsDone()) << "Should work with precomputed F(Bx)";
  EXPECT_NEAR(aSolver.Location(), 2.0, 1.0e-8) << "Should still find correct minimum";
}

TEST(MathBrentMinimumTest, CustomTolerance)
{
  // Test with different tolerance values
  QuadraticFunction aFunc;

  // Loose tolerance
  math_BrentMinimum aSolver1(1.0e-3);
  aSolver1.Perform(aFunc, 0.0, 1.5, 4.0);

  EXPECT_TRUE(aSolver1.IsDone()) << "Should converge with loose tolerance";
  EXPECT_NEAR(aSolver1.Location(), 2.0, 1.0e-2) << "Location should be approximately correct";

  // Tight tolerance
  math_BrentMinimum aSolver2(1.0e-12);
  aSolver2.Perform(aFunc, 0.0, 1.5, 4.0);

  EXPECT_TRUE(aSolver2.IsDone()) << "Should converge with tight tolerance";
  EXPECT_NEAR(aSolver2.Location(), 2.0, 1.0e-8) << "Location should be very accurate";
}

TEST(MathBrentMinimumTest, CustomIterationLimit)
{
  // Test with custom iteration limits
  RosenbrockSliceFunction aFunc; // More challenging function

  // Few iterations
  math_BrentMinimum aSolver1(1.0e-10, 5);
  aSolver1.Perform(aFunc, 0.0, 0.5, 2.0);

  if (aSolver1.IsDone())
  {
    EXPECT_LE(aSolver1.NbIterations(), 5) << "Should respect iteration limit";
  }

  // Many iterations
  math_BrentMinimum aSolver2(1.0e-12, 200);
  aSolver2.Perform(aFunc, 0.0, 0.5, 2.0);

  EXPECT_TRUE(aSolver2.IsDone()) << "Should converge with many iterations allowed";
}

TEST(MathBrentMinimumTest, InvalidBracketingTriplet)
{
  // Test with invalid bracketing (Bx not between Ax and Cx)
  QuadraticFunction aFunc;

  math_BrentMinimum aSolver(1.0e-10);
  aSolver.Perform(aFunc, 0.0, 4.0, 1.5); // Bx > Cx, invalid bracketing

  // Implementation may handle this gracefully or fail
  if (aSolver.IsDone())
  {
    // If it succeeds, the result should still be reasonable
    EXPECT_GT(aSolver.Location(), 0.0) << "Result should be reasonable if converged";
    EXPECT_LT(aSolver.Location(), 5.0) << "Result should be reasonable if converged";
  }
}

TEST(MathBrentMinimumTest, FlatFunction)
{
  // Test with a very flat function around minimum
  QuarticFunction aFunc; // Has very flat minimum at x=1

  math_BrentMinimum aSolver(1.0e-12);
  aSolver.Perform(aFunc, 0.99, 1.0, 1.01); // Very narrow bracketing

  EXPECT_TRUE(aSolver.IsDone()) << "Should handle flat function";
  EXPECT_NEAR(aSolver.Location(), 1.0, 1.0e-8) << "Should find flat minimum";
}

TEST(MathBrentMinimumTest, MonotonicFunction)
{
  // Test with monotonic function (no true minimum in interval)
  ShiftedExponentialFunction aFunc;

  math_BrentMinimum aSolver(1.0e-10);
  aSolver.Perform(aFunc, 0.0, 1.0, 2.0);

  if (aSolver.IsDone())
  {
    // If it finds a "minimum", it should be at the left boundary
    EXPECT_LT(aSolver.Location(), 1.0) << "Minimum should be toward left boundary";
  }
}

TEST(MathBrentMinimumTest, CustomZEPS)
{
  // Test with custom ZEPS (machine epsilon) parameter
  QuadraticFunction aFunc;

  math_BrentMinimum aSolver(1.0e-10, 100, 1.0e-15);
  aSolver.Perform(aFunc, 0.0, 1.5, 4.0);

  EXPECT_TRUE(aSolver.IsDone()) << "Should work with custom ZEPS";
  EXPECT_NEAR(aSolver.Location(), 2.0, 1.0e-8) << "Result should be accurate";
}

TEST(MathBrentMinimumTest, UnperformedState)
{
  // Test state handling before Perform() is called
  math_BrentMinimum aSolver(1.0e-10);

  // Before Perform() is called, solver should report not done
  EXPECT_FALSE(aSolver.IsDone()) << "Solver should not be done before Perform()";

  // In release builds, verify the solver maintains consistent state
  if (!aSolver.IsDone())
  {
    EXPECT_FALSE(aSolver.IsDone()) << "State should be consistent when not done";
  }
}

TEST(MathBrentMinimumTest, ReversedBracketOrder)
{
  // Test with brackets in reverse order (Cx < Ax)
  QuadraticFunction aFunc;

  math_BrentMinimum aSolver(1.0e-10);
  aSolver.Perform(aFunc, 4.0, 1.5, 0.0); // Reversed order

  EXPECT_TRUE(aSolver.IsDone()) << "Should handle reversed bracket order";
  EXPECT_NEAR(aSolver.Location(), 2.0, 1.0e-8) << "Should still find correct minimum";
}

TEST(MathBrentMinimumTest, HighPrecisionRequirement)
{
  // Test with extremely tight tolerance
  CosineFunction    aFunc;
  math_BrentMinimum aSolver(1.0e-15, 200);

  aSolver.Perform(aFunc, 2.5, 3.1, 4.0);

  EXPECT_TRUE(aSolver.IsDone()) << "Should converge with high precision requirement";
  EXPECT_NEAR(aSolver.Location(), M_PI, 1.0e-8) << "Location should be very accurate";
  EXPECT_GT(aSolver.NbIterations(), 10) << "Should require several iterations for high precision";
}

TEST(MathBrentMinimumTest, VeryNarrowBracket)
{
  // Test with very narrow initial bracket
  QuadraticFunction aFunc;

  math_BrentMinimum aSolver(1.0e-10);
  aSolver.Perform(aFunc, 1.99, 2.0, 2.01); // Very narrow around minimum

  EXPECT_TRUE(aSolver.IsDone()) << "Should handle narrow bracket";
  EXPECT_NEAR(aSolver.Location(), 2.0, 1.0e-8) << "Should find accurate minimum";
}

TEST(MathBrentMinimumTest, MultipleCalls)
{
  // Test multiple calls to Perform with same instance
  QuadraticFunction aFunc1;
  CosineFunction    aFunc2;

  math_BrentMinimum aSolver(1.0e-10);

  // First call
  aSolver.Perform(aFunc1, 0.0, 1.5, 4.0);
  EXPECT_TRUE(aSolver.IsDone()) << "First call should succeed";
  EXPECT_NEAR(aSolver.Location(), 2.0, 1.0e-8) << "First minimum should be x = 2";

  // Second call with different function
  aSolver.Perform(aFunc2, 2.5, 3.1, 4.0);
  EXPECT_TRUE(aSolver.IsDone()) << "Second call should succeed";
  EXPECT_NEAR(aSolver.Location(), M_PI, 1.0e-8) << "Second minimum should be PI";
}

TEST(MathBrentMinimumTest, EdgeCaseAtBoundary)
{
  // Test when minimum is very close to one of the boundaries
  QuadraticFunction aFunc; // Minimum at x = 2

  math_BrentMinimum aSolver(1.0e-10);
  aSolver.Perform(aFunc, 2.0, 2.1, 3.0); // Minimum at left boundary

  EXPECT_TRUE(aSolver.IsDone()) << "Should handle minimum near boundary";
  EXPECT_NEAR(aSolver.Location(), 2.0, 1.0e-8) << "Should find minimum at boundary";
}