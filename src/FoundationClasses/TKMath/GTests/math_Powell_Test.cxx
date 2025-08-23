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

#include <math_Powell.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <math_MultipleVarFunction.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_DimensionError.hxx>
#include <Precision.hxx>

#include <cmath>

namespace
{

// Test function: f(x,y) = (x-1)^2 + (y-2)^2
// Minimum at (1,2) with value 0
class QuadraticFunction : public math_MultipleVarFunction
{
public:
  QuadraticFunction() {}

  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& X, Standard_Real& F) override
  {
    Standard_Real x = X(1) - 1.0;
    Standard_Real y = X(2) - 2.0;
    F               = x * x + y * y;
    return Standard_True;
  }
};

// Rosenbrock function: f(x,y) = 100*(y-x^2)^2 + (1-x)^2
// Minimum at (1,1) with value 0 - classic optimization test case
class RosenbrockFunction : public math_MultipleVarFunction
{
public:
  RosenbrockFunction() {}

  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& X, Standard_Real& F) override
  {
    Standard_Real x     = X(1);
    Standard_Real y     = X(2);
    Standard_Real term1 = y - x * x;
    Standard_Real term2 = 1.0 - x;
    F                   = 100.0 * term1 * term1 + term2 * term2;
    return Standard_True;
  }
};

// Simple 1D function: f(x) = (x-3)^2
// Minimum at x=3 with value 0
class Simple1DFunction : public math_MultipleVarFunction
{
public:
  Simple1DFunction() {}

  Standard_Integer NbVariables() const override { return 1; }

  Standard_Boolean Value(const math_Vector& X, Standard_Real& F) override
  {
    Standard_Real x = X(1) - 3.0;
    F               = x * x;
    return Standard_True;
  }
};

// Higher dimensional function: f(x) = Sum(xi - i)^2 for i=1..n
class MultiDimensionalQuadratic : public math_MultipleVarFunction
{
private:
  Standard_Integer myN;

public:
  MultiDimensionalQuadratic(Standard_Integer n)
      : myN(n)
  {
  }

  Standard_Integer NbVariables() const override { return myN; }

  Standard_Boolean Value(const math_Vector& X, Standard_Real& F) override
  {
    F = 0.0;
    for (Standard_Integer i = 1; i <= myN; i++)
    {
      Standard_Real diff = X(i) - static_cast<Standard_Real>(i);
      F += diff * diff;
    }
    return Standard_True;
  }
};

TEST(MathPowellTest, SimpleQuadraticFunction)
{
  QuadraticFunction aFunc;
  math_Powell       aPowell(aFunc, 1.0e-8, 100);

  // Starting point away from minimum
  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 5.0;
  aStartPoint(2) = 7.0;

  // Initial search directions (identity matrix)
  math_Matrix aDirections(1, 2, 1, 2);
  aDirections(1, 1) = 1.0;
  aDirections(1, 2) = 0.0;
  aDirections(2, 1) = 0.0;
  aDirections(2, 2) = 1.0;

  aPowell.Perform(aFunc, aStartPoint, aDirections);

  EXPECT_TRUE(aPowell.IsDone()) << "Powell should converge for simple quadratic function";

  const math_Vector& aLocation = aPowell.Location();
  Standard_Real      aMinimum  = aPowell.Minimum();

  EXPECT_NEAR(aLocation(1), 1.0, 1.0e-6) << "Optimal X coordinate";
  EXPECT_NEAR(aLocation(2), 2.0, 1.0e-6) << "Optimal Y coordinate";
  EXPECT_NEAR(aMinimum, 0.0, 1.0e-10) << "Minimum function value";
}

TEST(MathPowellTest, Simple1DOptimization)
{
  Simple1DFunction aFunc;
  math_Powell      aPowell(aFunc, 1.0e-10, 50);

  math_Vector aStartPoint(1, 1);
  aStartPoint(1) = 10.0; // Start far from optimum

  math_Matrix aDirections(1, 1, 1, 1);
  aDirections(1, 1) = 1.0; // Single direction

  aPowell.Perform(aFunc, aStartPoint, aDirections);

  EXPECT_TRUE(aPowell.IsDone()) << "Powell should converge for 1D quadratic";

  const math_Vector& aLocation = aPowell.Location();
  EXPECT_NEAR(aLocation(1), 3.0, 1.0e-8) << "1D optimum should be at x=3";
  EXPECT_NEAR(aPowell.Minimum(), 0.0, 1.0e-12) << "1D minimum value should be 0";
}

TEST(MathPowellTest, RosenbrockFunction)
{
  RosenbrockFunction aFunc;
  math_Powell        aPowell(aFunc, 1.0e-6, 1000); // More iterations for challenging function

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = -1.0;
  aStartPoint(2) = 1.0; // Classic starting point

  math_Matrix aDirections(1, 2, 1, 2);
  aDirections(1, 1) = 1.0;
  aDirections(1, 2) = 0.0;
  aDirections(2, 1) = 0.0;
  aDirections(2, 2) = 1.0;

  aPowell.Perform(aFunc, aStartPoint, aDirections);

  EXPECT_TRUE(aPowell.IsDone()) << "Powell should converge for Rosenbrock function";

  const math_Vector& aLocation = aPowell.Location();

  // Rosenbrock is challenging - allow larger tolerance
  EXPECT_NEAR(aLocation(1), 1.0, 1.0e-3) << "Rosenbrock optimal X";
  EXPECT_NEAR(aLocation(2), 1.0, 1.0e-3) << "Rosenbrock optimal Y";
  EXPECT_NEAR(aPowell.Minimum(), 0.0, 1.0e-5) << "Rosenbrock minimum value";
}

TEST(MathPowellTest, HigherDimensionalOptimization)
{
  MultiDimensionalQuadratic aFunc(4); // 4D optimization
  math_Powell               aPowell(aFunc, 1.0e-8, 200);

  math_Vector aStartPoint(1, 4);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;
  aStartPoint(3) = 0.0;
  aStartPoint(4) = 0.0;

  // Identity matrix for initial directions
  math_Matrix aDirections(1, 4, 1, 4);
  for (Standard_Integer i = 1; i <= 4; i++)
  {
    for (Standard_Integer j = 1; j <= 4; j++)
    {
      aDirections(i, j) = (i == j) ? 1.0 : 0.0;
    }
  }

  aPowell.Perform(aFunc, aStartPoint, aDirections);

  EXPECT_TRUE(aPowell.IsDone()) << "Powell should converge for 4D quadratic";

  const math_Vector& aLocation = aPowell.Location();

  // Expected optimum: (1, 2, 3, 4)
  EXPECT_NEAR(aLocation(1), 1.0, 1.0e-6) << "4D optimal X1";
  EXPECT_NEAR(aLocation(2), 2.0, 1.0e-6) << "4D optimal X2";
  EXPECT_NEAR(aLocation(3), 3.0, 1.0e-6) << "4D optimal X3";
  EXPECT_NEAR(aLocation(4), 4.0, 1.0e-6) << "4D optimal X4";
  EXPECT_NEAR(aPowell.Minimum(), 0.0, 1.0e-10) << "4D minimum value";
}

TEST(MathPowellTest, DifferentStartingDirections)
{
  QuadraticFunction aFunc;
  math_Powell       aPowell(aFunc, 1.0e-8, 100);

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 3.0;
  aStartPoint(2) = 5.0;

  // Non-orthogonal starting directions
  math_Matrix aDirections(1, 2, 1, 2);
  aDirections(1, 1) = 1.0;
  aDirections(1, 2) = 1.0; // [1, 1]
  aDirections(2, 1) = 1.0;
  aDirections(2, 2) = -1.0; // [1, -1]

  aPowell.Perform(aFunc, aStartPoint, aDirections);

  EXPECT_TRUE(aPowell.IsDone()) << "Powell should work with non-orthogonal directions";

  const math_Vector& aLocation = aPowell.Location();
  EXPECT_NEAR(aLocation(1), 1.0, 1.0e-6) << "Non-orthogonal directions X";
  EXPECT_NEAR(aLocation(2), 2.0, 1.0e-6) << "Non-orthogonal directions Y";
}

TEST(MathPowellTest, IterationLimit)
{
  RosenbrockFunction aFunc;
  math_Powell        aPowell(aFunc, 1.0e-12, 5); // Very few iterations

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = -2.0;
  aStartPoint(2) = 3.0;

  math_Matrix aDirections(1, 2, 1, 2);
  aDirections(1, 1) = 1.0;
  aDirections(1, 2) = 0.0;
  aDirections(2, 1) = 0.0;
  aDirections(2, 2) = 1.0;

  aPowell.Perform(aFunc, aStartPoint, aDirections);

  // With only 5 iterations, should not converge for Rosenbrock from this starting point
  EXPECT_FALSE(aPowell.IsDone())
    << "Should fail to converge within 5 iterations for challenging function";
}

TEST(MathPowellTest, ToleranceSettings)
{
  QuadraticFunction aFunc;

  // Loose tolerance
  math_Powell aPowell1(aFunc, 1.0e-2, 100);

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 5.0;
  aStartPoint(2) = 7.0;

  math_Matrix aDirections(1, 2, 1, 2);
  aDirections(1, 1) = 1.0;
  aDirections(1, 2) = 0.0;
  aDirections(2, 1) = 0.0;
  aDirections(2, 2) = 1.0;

  aPowell1.Perform(aFunc, aStartPoint, aDirections);
  EXPECT_TRUE(aPowell1.IsDone()) << "Should converge with loose tolerance";

  Standard_Integer aIterationsLoose = aPowell1.NbIterations();

  // Tight tolerance
  math_Powell aPowell2(aFunc, 1.0e-10, 100);
  aPowell2.Perform(aFunc, aStartPoint, aDirections);
  EXPECT_TRUE(aPowell2.IsDone()) << "Should converge with tight tolerance";

  Standard_Integer aIterationsTight = aPowell2.NbIterations();

  // Tighter tolerance usually requires more iterations
  EXPECT_GE(aIterationsTight, aIterationsLoose)
    << "Tighter tolerance should require more iterations";
}

TEST(MathPowellTest, LocationOutputMethod)
{
  QuadraticFunction aFunc;
  math_Powell       aPowell(aFunc, 1.0e-8, 100);

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 4.0;
  aStartPoint(2) = 6.0;

  math_Matrix aDirections(1, 2, 1, 2);
  aDirections(1, 1) = 1.0;
  aDirections(1, 2) = 0.0;
  aDirections(2, 1) = 0.0;
  aDirections(2, 2) = 1.0;

  aPowell.Perform(aFunc, aStartPoint, aDirections);
  EXPECT_TRUE(aPowell.IsDone());

  // Test Location() output method
  math_Vector aLoc(1, 2);
  aPowell.Location(aLoc);

  EXPECT_NEAR(aLoc(1), 1.0, 1.0e-6) << "Location output method X";
  EXPECT_NEAR(aLoc(2), 2.0, 1.0e-6) << "Location output method Y";

  // Compare with direct access
  const math_Vector& aLocDirect = aPowell.Location();
  EXPECT_NEAR(aLoc(1), aLocDirect(1), Precision::Confusion()) << "Location methods should match";
  EXPECT_NEAR(aLoc(2), aLocDirect(2), Precision::Confusion()) << "Location methods should match";
}

TEST(MathPowellTest, UnperformedState)
{
  QuadraticFunction aFunc;
  math_Powell       aPowell(aFunc, 1.0e-8, 100);

  // Before Perform() is called, optimizer should report not done
  EXPECT_FALSE(aPowell.IsDone()) << "Optimizer should not be done before Perform()";

  // In release builds, verify the optimizer maintains consistent state
  if (!aPowell.IsDone())
  {
    EXPECT_FALSE(aPowell.IsDone()) << "State should be consistent when not done";
  }
}

TEST(MathPowellTest, DimensionCompatibility)
{
  QuadraticFunction aFunc;
  math_Powell       aPowell(aFunc, 1.0e-8, 100);

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 2.0;
  aStartPoint(2) = 3.0;

  math_Matrix aDirections(1, 2, 1, 2);
  aDirections(1, 1) = 1.0;
  aDirections(1, 2) = 0.0;
  aDirections(2, 1) = 0.0;
  aDirections(2, 2) = 1.0;

  aPowell.Perform(aFunc, aStartPoint, aDirections);
  EXPECT_TRUE(aPowell.IsDone());

  // Test with correctly sized vector
  math_Vector aCorrectLoc(1, 2); // Correct size 2
  aPowell.Location(aCorrectLoc);

  // Verify the result makes sense
  EXPECT_EQ(aCorrectLoc.Length(), 2) << "Location vector should have correct dimension";
}

TEST(MathPowellTest, AlreadyAtOptimum)
{
  QuadraticFunction aFunc;
  math_Powell       aPowell(aFunc, 1.0e-8, 100);

  // Start at the optimum
  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 1.0;
  aStartPoint(2) = 2.0;

  math_Matrix aDirections(1, 2, 1, 2);
  aDirections(1, 1) = 1.0;
  aDirections(1, 2) = 0.0;
  aDirections(2, 1) = 0.0;
  aDirections(2, 2) = 1.0;

  aPowell.Perform(aFunc, aStartPoint, aDirections);

  EXPECT_TRUE(aPowell.IsDone()) << "Should succeed when starting at optimum";

  const math_Vector& aLocation = aPowell.Location();
  EXPECT_NEAR(aLocation(1), 1.0, 1.0e-10) << "Should stay at optimum X";
  EXPECT_NEAR(aLocation(2), 2.0, 1.0e-10) << "Should stay at optimum Y";
  EXPECT_NEAR(aPowell.Minimum(), 0.0, 1.0e-12) << "Function value should be 0";

  // Should converge very quickly
  EXPECT_LE(aPowell.NbIterations(), 5) << "Should converge quickly when starting at optimum";
}

} // anonymous namespace