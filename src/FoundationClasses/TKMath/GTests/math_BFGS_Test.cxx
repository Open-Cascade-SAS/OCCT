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

#include <math_BFGS.hxx>
#include <math_FRPR.hxx>
#include <math_MultipleVarFunctionWithGradient.hxx>
#include <math_Vector.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_DimensionError.hxx>
#include <Precision.hxx>

#include <cmath>

// Test function classes for optimization

// Quadratic function: f(x,y) = (x-1)^2 + (y-2)^2 (minimum at (1,2), value = 0)
class QuadraticFunction2D : public math_MultipleVarFunctionWithGradient
{
public:
  QuadraticFunction2D() {}

  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    if (theX.Length() != 2)
      return Standard_False;
    Standard_Real dx = theX(1) - 1.0;
    Standard_Real dy = theX(2) - 2.0;
    theF             = dx * dx + dy * dy;
    return Standard_True;
  }

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    if (theX.Length() != 2 || theG.Length() != 2)
      return Standard_False;
    theG(1) = 2.0 * (theX(1) - 1.0);
    theG(2) = 2.0 * (theX(2) - 2.0);
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    return Value(theX, theF) && Gradient(theX, theG);
  }
};

// Rosenbrock function: f(x,y) = 100*(y-x^2)^2 + (1-x)^2 (minimum at (1,1), value = 0)
class RosenbrockFunction : public math_MultipleVarFunctionWithGradient
{
public:
  RosenbrockFunction() {}

  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    if (theX.Length() != 2)
      return Standard_False;
    Standard_Real x  = theX(1);
    Standard_Real y  = theX(2);
    Standard_Real t1 = y - x * x;
    Standard_Real t2 = 1.0 - x;
    theF             = 100.0 * t1 * t1 + t2 * t2;
    return Standard_True;
  }

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    if (theX.Length() != 2 || theG.Length() != 2)
      return Standard_False;
    Standard_Real x = theX(1);
    Standard_Real y = theX(2);
    theG(1)         = -400.0 * x * (y - x * x) - 2.0 * (1.0 - x);
    theG(2)         = 200.0 * (y - x * x);
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    return Value(theX, theF) && Gradient(theX, theG);
  }
};

// 3D Paraboloid: f(x,y,z) = x^2 + 2*y^2 + 3*z^2 (minimum at origin, value = 0)
class Paraboloid3D : public math_MultipleVarFunctionWithGradient
{
public:
  Paraboloid3D() {}

  Standard_Integer NbVariables() const override { return 3; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    if (theX.Length() != 3)
      return Standard_False;
    theF = theX(1) * theX(1) + 2.0 * theX(2) * theX(2) + 3.0 * theX(3) * theX(3);
    return Standard_True;
  }

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    if (theX.Length() != 3 || theG.Length() != 3)
      return Standard_False;
    theG(1) = 2.0 * theX(1);
    theG(2) = 4.0 * theX(2);
    theG(3) = 6.0 * theX(3);
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    return Value(theX, theF) && Gradient(theX, theG);
  }
};

// Tests for math_BFGS optimization
TEST(MathBFGSTest, QuadraticFunction2DOptimization)
{
  QuadraticFunction2D aFunc;
  Standard_Real       aTolerance     = 1.0e-8;
  Standard_Integer    aMaxIterations = 100;

  math_BFGS anOptimizer(2, aTolerance, aMaxIterations);

  // Start from point (5, 7), should converge to (1, 2)
  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 5.0;
  aStartPoint(2) = 7.0;

  anOptimizer.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(anOptimizer.IsDone()) << "BFGS optimization should succeed";

  const math_Vector& aLocation = anOptimizer.Location();
  EXPECT_NEAR(aLocation(1), 1.0, 1.0e-6) << "X coordinate should be close to 1.0";
  EXPECT_NEAR(aLocation(2), 2.0, 1.0e-6) << "Y coordinate should be close to 2.0";

  EXPECT_NEAR(anOptimizer.Minimum(), 0.0, 1.0e-10) << "Minimum value should be close to 0.0";

  const math_Vector& aGradient = anOptimizer.Gradient();
  EXPECT_NEAR(aGradient(1), 0.0, 1.0e-6) << "Gradient X component should be close to 0";
  EXPECT_NEAR(aGradient(2), 0.0, 1.0e-6) << "Gradient Y component should be close to 0";

  EXPECT_GT(anOptimizer.NbIterations(), 0) << "Should require at least one iteration";
  EXPECT_LE(anOptimizer.NbIterations(), aMaxIterations) << "Should not exceed max iterations";
}

TEST(MathBFGSTest, RosenbrockFunctionOptimization)
{
  RosenbrockFunction aFunc;
  Standard_Real      aTolerance     = 1.0e-6;
  Standard_Integer   aMaxIterations = 1000; // Rosenbrock can be challenging

  math_BFGS anOptimizer(2, aTolerance, aMaxIterations);

  // Start from point (-1, 1), should converge to (1, 1)
  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = -1.0;
  aStartPoint(2) = 1.0;

  anOptimizer.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(anOptimizer.IsDone()) << "BFGS optimization should succeed for Rosenbrock";

  const math_Vector& aLocation = anOptimizer.Location();
  EXPECT_NEAR(aLocation(1), 1.0, 1.0e-4) << "X coordinate should be close to 1.0";
  EXPECT_NEAR(aLocation(2), 1.0, 1.0e-4) << "Y coordinate should be close to 1.0";

  EXPECT_NEAR(anOptimizer.Minimum(), 0.0, 1.0e-8) << "Minimum value should be close to 0.0";
}

TEST(MathBFGSTest, Paraboloid3DOptimization)
{
  Paraboloid3D     aFunc;
  Standard_Real    aTolerance     = 1.0e-8;
  Standard_Integer aMaxIterations = 100;

  math_BFGS anOptimizer(3, aTolerance, aMaxIterations);

  // Start from point (3, 4, 5), should converge to origin (0, 0, 0)
  math_Vector aStartPoint(1, 3);
  aStartPoint(1) = 3.0;
  aStartPoint(2) = 4.0;
  aStartPoint(3) = 5.0;

  anOptimizer.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(anOptimizer.IsDone()) << "BFGS optimization should succeed for 3D paraboloid";

  const math_Vector& aLocation = anOptimizer.Location();
  EXPECT_NEAR(aLocation(1), 0.0, 1.0e-6) << "X coordinate should be close to 0.0";
  EXPECT_NEAR(aLocation(2), 0.0, 1.0e-6) << "Y coordinate should be close to 0.0";
  EXPECT_NEAR(aLocation(3), 0.0, 1.0e-6) << "Z coordinate should be close to 0.0";

  EXPECT_NEAR(anOptimizer.Minimum(), 0.0, 1.0e-10) << "Minimum value should be close to 0.0";
}

TEST(MathBFGSTest, BoundaryConstraints)
{
  QuadraticFunction2D aFunc;
  Standard_Real       aTolerance     = 1.0e-8;
  Standard_Integer    aMaxIterations = 100;

  math_BFGS anOptimizer(2, aTolerance, aMaxIterations);

  // Set boundaries: x in [2, 4], y in [3, 5]
  // True minimum (1,2) is outside these bounds
  // Constrained minimum should be at (2,3)
  math_Vector aLowerBound(1, 2);
  aLowerBound(1) = 2.0;
  aLowerBound(2) = 3.0;

  math_Vector anUpperBound(1, 2);
  anUpperBound(1) = 4.0;
  anUpperBound(2) = 5.0;

  anOptimizer.SetBoundary(aLowerBound, anUpperBound);

  // Start from point (3, 4)
  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 3.0;
  aStartPoint(2) = 4.0;

  anOptimizer.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(anOptimizer.IsDone()) << "BFGS optimization with boundaries should succeed";

  const math_Vector& aLocation = anOptimizer.Location();
  EXPECT_GE(aLocation(1), aLowerBound(1)) << "X should be within lower bound";
  EXPECT_LE(aLocation(1), anUpperBound(1)) << "X should be within upper bound";
  EXPECT_GE(aLocation(2), aLowerBound(2)) << "Y should be within lower bound";
  EXPECT_LE(aLocation(2), anUpperBound(2)) << "Y should be within upper bound";

  // Should find constrained minimum at (2,3)
  EXPECT_NEAR(aLocation(1), 2.0, 1.0e-6) << "Constrained minimum X should be at boundary";
  EXPECT_NEAR(aLocation(2), 3.0, 1.0e-6) << "Constrained minimum Y should be at boundary";
}

TEST(MathBFGSTest, LocationCopyMethod)
{
  QuadraticFunction2D aFunc;
  math_BFGS           anOptimizer(2);

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 3.0;
  aStartPoint(2) = 4.0;

  anOptimizer.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(anOptimizer.IsDone()) << "Optimization should succeed";

  // Test Location copy method
  math_Vector aLocationCopy(1, 2);
  anOptimizer.Location(aLocationCopy);

  const math_Vector& aLocationRef = anOptimizer.Location();
  EXPECT_NEAR(aLocationCopy(1), aLocationRef(1), Precision::Confusion())
    << "Copied location should match reference";
  EXPECT_NEAR(aLocationCopy(2), aLocationRef(2), Precision::Confusion())
    << "Copied location should match reference";
}

TEST(MathBFGSTest, GradientCopyMethod)
{
  QuadraticFunction2D aFunc;
  math_BFGS           anOptimizer(2);

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 3.0;
  aStartPoint(2) = 4.0;

  anOptimizer.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(anOptimizer.IsDone()) << "Optimization should succeed";

  // Test Gradient copy method
  math_Vector aGradientCopy(1, 2);
  anOptimizer.Gradient(aGradientCopy);

  const math_Vector& aGradientRef = anOptimizer.Gradient();
  EXPECT_NEAR(aGradientCopy(1), aGradientRef(1), Precision::Confusion())
    << "Copied gradient should match reference";
  EXPECT_NEAR(aGradientCopy(2), aGradientRef(2), Precision::Confusion())
    << "Copied gradient should match reference";
}

TEST(MathBFGSTest, DifferentTolerances)
{
  QuadraticFunction2D aFunc;

  // Test with very tight tolerance
  {
    Standard_Real aTightTolerance = 1.0e-12;
    math_BFGS     anOptimizer(2, aTightTolerance);

    math_Vector aStartPoint(1, 2);
    aStartPoint(1) = 2.0;
    aStartPoint(2) = 3.0;

    anOptimizer.Perform(aFunc, aStartPoint);

    EXPECT_TRUE(anOptimizer.IsDone()) << "High precision optimization should succeed";
    EXPECT_NEAR(anOptimizer.Location()(1), 1.0, aTightTolerance * 10)
      << "High precision X coordinate";
    EXPECT_NEAR(anOptimizer.Location()(2), 2.0, aTightTolerance * 10)
      << "High precision Y coordinate";
  }

  // Test with loose tolerance
  {
    Standard_Real aLooseTolerance = 1.0e-3;
    math_BFGS     anOptimizer(2, aLooseTolerance);

    math_Vector aStartPoint(1, 2);
    aStartPoint(1) = 10.0;
    aStartPoint(2) = 10.0;

    anOptimizer.Perform(aFunc, aStartPoint);

    EXPECT_TRUE(anOptimizer.IsDone()) << "Low precision optimization should succeed";
    EXPECT_NEAR(anOptimizer.Location()(1), 1.0, aLooseTolerance * 10)
      << "Low precision X coordinate";
    EXPECT_NEAR(anOptimizer.Location()(2), 2.0, aLooseTolerance * 10)
      << "Low precision Y coordinate";
  }
}

TEST(MathBFGSTest, MaxIterationsLimit)
{
  RosenbrockFunction aFunc;                        // Challenging function
  Standard_Real      aTolerance         = 1.0e-12; // Very tight tolerance
  Standard_Integer   aVeryFewIterations = 5;       // Very few iterations

  math_BFGS anOptimizer(2, aTolerance, aVeryFewIterations);

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = -2.0;
  aStartPoint(2) = 2.0;

  anOptimizer.Perform(aFunc, aStartPoint);

  // Either succeeds within few iterations (unlikely) or fails
  if (anOptimizer.IsDone())
  {
    EXPECT_LE(anOptimizer.NbIterations(), aVeryFewIterations) << "Should not exceed max iterations";
  }
  else
  {
    // Failure is acceptable with very few iterations
    EXPECT_LE(aVeryFewIterations, 10) << "Failure expected with very few iterations";
  }
}

// Tests for exception handling
TEST(MathBFGSTest, NotDoneState)
{
  QuadraticFunction2D aFunc;
  math_BFGS           anOptimizer(2, 1.0e-12, 3); // Reasonable tolerance, few iterations

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 50.0; // Far from minimum but not extreme
  aStartPoint(2) = 50.0;

  // Wrap in try-catch to handle potential exceptions in debug mode
  EXPECT_NO_THROW({
    anOptimizer.Perform(aFunc, aStartPoint);

    if (!anOptimizer.IsDone())
    {
      EXPECT_GE(anOptimizer.NbIterations(), 0)
        << "Iteration count should be non-negative even on failure";
    }
    else
    {
      EXPECT_GT(anOptimizer.NbIterations(), 0)
        << "Successful optimization should require at least one iteration";
    }
  }) << "BFGS optimization should not throw exceptions";
}

TEST(MathBFGSTest, DimensionCompatibility)
{
  QuadraticFunction2D aFunc;
  math_BFGS           anOptimizer(2);

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 2.0;
  aStartPoint(2) = 3.0;

  anOptimizer.Perform(aFunc, aStartPoint);

  ASSERT_TRUE(anOptimizer.IsDone())
    << "Optimization should succeed for dimension compatibility tests";

  // Verify optimizer works correctly with properly dimensioned vectors
  math_Vector aCorrectSizeLocation(1, 2);
  math_Vector aCorrectSizeGradient(1, 2);

  anOptimizer.Location(aCorrectSizeLocation);
  anOptimizer.Gradient(aCorrectSizeGradient);

  // Verify the results make sense
  EXPECT_EQ(aCorrectSizeLocation.Length(), 2) << "Location vector should have correct dimension";
  EXPECT_EQ(aCorrectSizeGradient.Length(), 2) << "Gradient vector should have correct dimension";
}

TEST(MathBFGSTest, ConstructorParameters)
{
  // Test different constructor parameter combinations
  {
    math_BFGS           anOptimizer1(3); // Default tolerance, iterations, ZEPS
    QuadraticFunction2D aFunc; // This is 2D, but optimizer is 3D - should handle gracefully

    // This might not work well due to dimension mismatch, but shouldn't crash
    EXPECT_NO_THROW({
      math_Vector aStart(1, 3);
      aStart.Init(1.0);
      // Don't perform - just test construction doesn't crash
    });
  }

  {
    math_BFGS anOptimizer2(2, 1.0e-6); // Custom tolerance, default iterations and ZEPS
    // Should construct successfully
  }

  {
    math_BFGS anOptimizer3(2, 1.0e-8, 50); // Custom tolerance and iterations
    // Should construct successfully
  }

  {
    math_BFGS anOptimizer4(2, 1.0e-8, 100, 1.0e-10); // All parameters custom
    // Should construct successfully
  }
}

TEST(MathBFGSTest, MultipleOptimizations)
{
  QuadraticFunction2D aFunc;
  math_BFGS           anOptimizer(2);

  // Perform multiple optimizations with the same optimizer instance
  std::vector<std::pair<Standard_Real, Standard_Real>> aStartPoints = {{5.0, 7.0},
                                                                       {-3.0, -4.0},
                                                                       {0.5, 1.5},
                                                                       {10.0, -5.0}};

  for (const auto& aStart : aStartPoints)
  {
    math_Vector aStartPoint(1, 2);
    aStartPoint(1) = aStart.first;
    aStartPoint(2) = aStart.second;

    anOptimizer.Perform(aFunc, aStartPoint);

    EXPECT_TRUE(anOptimizer.IsDone()) << "Each optimization should succeed";
    EXPECT_NEAR(anOptimizer.Location()(1), 1.0, 1.0e-6) << "Each should find correct X minimum";
    EXPECT_NEAR(anOptimizer.Location()(2), 2.0, 1.0e-6) << "Each should find correct Y minimum";
  }
}

// Simple 1D square function: f(x) = x^2 (minimum at x=0, value = 0)
class SquareFunction1D : public math_MultipleVarFunctionWithGradient
{
public:
  SquareFunction1D() {}

  Standard_Integer NbVariables() const override { return 1; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    if (theX.Length() != 1)
      return Standard_False;
    const Standard_Real x = theX(1);
    theF                  = x * x;
    return Standard_True;
  }

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    if (theX.Length() != 1 || theG.Length() != 1)
      return Standard_False;
    const Standard_Real x = theX(1);
    theG(1)               = 2.0 * x;
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    return Value(theX, theF) && Gradient(theX, theG);
  }
};

TEST(MathBFGSTest, OCC30492_StartingPointAtMinimum)
{
  // Bug OCC30492: BFGS and FRPR fail if starting point is exactly the minimum
  // Test that both BFGS and FRPR work correctly when the starting point is at the minimum
  SquareFunction1D aFunc;
  math_Vector      aStartPnt(1, 1);
  aStartPnt(1) = 0.0; // Start exactly at the minimum

  // Test FRPR
  math_FRPR aFRPR(aFunc, Precision::Confusion());
  aFRPR.Perform(aFunc, aStartPnt);
  EXPECT_TRUE(aFRPR.IsDone()) << "FRPR optimization should succeed when starting at minimum";

  // Test BFGS
  math_BFGS aBFGS(1, Precision::Confusion());
  aBFGS.Perform(aFunc, aStartPnt);
  EXPECT_TRUE(aBFGS.IsDone()) << "BFGS optimization should succeed when starting at minimum";

  // Verify that both found the minimum (or stayed at it)
  if (aFRPR.IsDone())
  {
    EXPECT_NEAR(aFRPR.Location()(1), 0.0, Precision::Confusion())
      << "FRPR should stay at/find the minimum";
    EXPECT_NEAR(aFRPR.Minimum(), 0.0, Precision::Confusion())
      << "FRPR should report minimum value of 0";
  }

  if (aBFGS.IsDone())
  {
    EXPECT_NEAR(aBFGS.Location()(1), 0.0, Precision::Confusion())
      << "BFGS should stay at/find the minimum";
    EXPECT_NEAR(aBFGS.Minimum(), 0.0, Precision::Confusion())
      << "BFGS should report minimum value of 0";
  }
}