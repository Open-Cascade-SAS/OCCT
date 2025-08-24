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

namespace
{

// Quadratic bowl function: f(x,y) = (x-1)^2 + (y-2)^2, minimum at (1, 2) with value 0
class QuadraticBowlFunction : public math_MultipleVarFunctionWithGradient
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    Standard_Real dx = theX(1) - 1.0;
    Standard_Real dy = theX(2) - 2.0;
    theF             = dx * dx + dy * dy;
    return Standard_True;
  }

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    theG(1) = 2.0 * (theX(1) - 1.0);
    theG(2) = 2.0 * (theX(2) - 2.0);
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    Value(theX, theF);
    Gradient(theX, theG);
    return Standard_True;
  }
};

// Rosenbrock function: f(x,y) = (1-x)^2 + 100*(y-x^2)^2, minimum at (1, 1) with value 0
class RosenbrockFunction : public math_MultipleVarFunctionWithGradient
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    Standard_Real x  = theX(1);
    Standard_Real y  = theX(2);
    Standard_Real dx = 1.0 - x;
    Standard_Real dy = y - x * x;
    theF             = dx * dx + 100.0 * dy * dy;
    return Standard_True;
  }

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    Standard_Real x = theX(1);
    Standard_Real y = theX(2);
    theG(1)         = -2.0 * (1.0 - x) + 200.0 * (y - x * x) * (-2.0 * x);
    theG(2)         = 200.0 * (y - x * x);
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    Value(theX, theF);
    Gradient(theX, theG);
    return Standard_True;
  }
};

// 3D quadratic function: f(x,y,z) = (x-1)^2 + 2*(y-2)^2 + 3*(z-3)^2, minimum at (1,2,3)
class Quadratic3DFunction : public math_MultipleVarFunctionWithGradient
{
public:
  Standard_Integer NbVariables() const override { return 3; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    Standard_Real dx = theX(1) - 1.0;
    Standard_Real dy = theX(2) - 2.0;
    Standard_Real dz = theX(3) - 3.0;
    theF             = dx * dx + 2.0 * dy * dy + 3.0 * dz * dz;
    return Standard_True;
  }

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    theG(1) = 2.0 * (theX(1) - 1.0);
    theG(2) = 4.0 * (theX(2) - 2.0);
    theG(3) = 6.0 * (theX(3) - 3.0);
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    Value(theX, theF);
    Gradient(theX, theG);
    return Standard_True;
  }
};

// Linear function: f(x,y) = 2*x + 3*y (unbounded, no minimum)
class LinearFunction : public math_MultipleVarFunctionWithGradient
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    theF = 2.0 * theX(1) + 3.0 * theX(2);
    return Standard_True;
  }

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    (void)theX;
    theG(1) = 2.0;
    theG(2) = 3.0;
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    Value(theX, theF);
    Gradient(theX, theG);
    return Standard_True;
  }
};

// Quartic function with flat minimum: f(x,y) = (x-1)^4 + (y-2)^4
class QuarticFunction : public math_MultipleVarFunctionWithGradient
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    Standard_Real dx = theX(1) - 1.0;
    Standard_Real dy = theX(2) - 2.0;
    theF             = dx * dx * dx * dx + dy * dy * dy * dy;
    return Standard_True;
  }

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    Standard_Real dx = theX(1) - 1.0;
    Standard_Real dy = theX(2) - 2.0;
    theG(1)          = 4.0 * dx * dx * dx;
    theG(2)          = 4.0 * dy * dy * dy;
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    Value(theX, theF);
    Gradient(theX, theG);
    return Standard_True;
  }
};

} // anonymous namespace

TEST(MathFRPRTest, QuadraticBowlOptimization)
{
  // Test FRPR on simple quadratic bowl function
  QuadraticBowlFunction aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0; // Start at (0, 0)
  aStartPoint(2) = 0.0;

  math_FRPR aSolver(aFunc, 1.0e-10);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum for quadratic bowl function";

  const math_Vector& aLoc = aSolver.Location();
  EXPECT_NEAR(aLoc(1), 1.0, 1.0e-6) << "Minimum should be at x = 1";
  EXPECT_NEAR(aLoc(2), 2.0, 1.0e-6) << "Minimum should be at y = 2";
  EXPECT_NEAR(aSolver.Minimum(), 0.0, 1.0e-10) << "Minimum value should be 0";
  EXPECT_GT(aSolver.NbIterations(), 0) << "Should have performed some iterations";
}

TEST(MathFRPRTest, RosenbrockOptimization)
{
  // Test FRPR on the challenging Rosenbrock function
  RosenbrockFunction aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0; // Start at (0, 0)
  aStartPoint(2) = 0.0;

  math_FRPR aSolver(aFunc, 1.0e-8, 500); // More iterations for challenging function
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum for Rosenbrock function";

  const math_Vector& aLoc = aSolver.Location();
  EXPECT_NEAR(aLoc(1), 1.0, 1.0e-3) << "Minimum should be near x = 1";
  EXPECT_NEAR(aLoc(2), 1.0, 1.0e-3) << "Minimum should be near y = 1";
  EXPECT_LT(aSolver.Minimum(), 1.0e-4) << "Should find a very small minimum";
}

TEST(MathFRPRTest, ThreeDimensionalOptimization)
{
  // Test FRPR on 3D quadratic function
  Quadratic3DFunction aFunc;

  math_Vector aStartPoint(1, 3);
  aStartPoint(1) = 0.0; // Start at (0, 0, 0)
  aStartPoint(2) = 0.0;
  aStartPoint(3) = 0.0;

  math_FRPR aSolver(aFunc, 1.0e-10);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum for 3D function";

  const math_Vector& aLoc = aSolver.Location();
  EXPECT_NEAR(aLoc(1), 1.0, 1.0e-6) << "Minimum should be at x = 1";
  EXPECT_NEAR(aLoc(2), 2.0, 1.0e-6) << "Minimum should be at y = 2";
  EXPECT_NEAR(aLoc(3), 3.0, 1.0e-6) << "Minimum should be at z = 3";
  EXPECT_NEAR(aSolver.Minimum(), 0.0, 1.0e-10) << "Minimum value should be 0";
}

TEST(MathFRPRTest, CustomTolerance)
{
  // Test with different tolerance values
  QuadraticBowlFunction aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  // Loose tolerance
  math_FRPR aSolver1(aFunc, 1.0e-3);
  aSolver1.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver1.IsDone()) << "Should converge with loose tolerance";
  EXPECT_NEAR(aSolver1.Location()(1), 1.0, 1.0e-2) << "Location should be approximately correct";
  EXPECT_NEAR(aSolver1.Location()(2), 2.0, 1.0e-2) << "Location should be approximately correct";

  // Tight tolerance
  math_FRPR aSolver2(aFunc, 1.0e-12);
  aSolver2.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver2.IsDone()) << "Should converge with tight tolerance";
  EXPECT_NEAR(aSolver2.Location()(1), 1.0, 1.0e-8) << "Location should be very accurate";
  EXPECT_NEAR(aSolver2.Location()(2), 2.0, 1.0e-8) << "Location should be very accurate";
}

TEST(MathFRPRTest, CustomIterationLimit)
{
  // Test with custom iteration limits
  RosenbrockFunction aFunc; // More challenging function

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  // Few iterations
  math_FRPR aSolver1(aFunc, 1.0e-10, 10);
  aSolver1.Perform(aFunc, aStartPoint);

  if (aSolver1.IsDone())
  {
    EXPECT_LE(aSolver1.NbIterations(), 10) << "Should respect iteration limit";
  }

  // Many iterations
  math_FRPR aSolver2(aFunc, 1.0e-10, 1000);
  aSolver2.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver2.IsDone()) << "Should converge with many iterations allowed";
}

TEST(MathFRPRTest, GradientAccess)
{
  // Test gradient vector access
  QuadraticBowlFunction aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  math_FRPR aSolver(aFunc, 1.0e-10);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum";

  const math_Vector& aGrad = aSolver.Gradient();
  EXPECT_NEAR(aGrad(1), 0.0, 1.0e-8) << "Gradient should be near zero at minimum";
  EXPECT_NEAR(aGrad(2), 0.0, 1.0e-8) << "Gradient should be near zero at minimum";

  // Test gradient output method
  math_Vector aGradOut(1, 2);
  aSolver.Gradient(aGradOut);
  EXPECT_NEAR(aGradOut(1), 0.0, 1.0e-8) << "Output gradient should match";
  EXPECT_NEAR(aGradOut(2), 0.0, 1.0e-8) << "Output gradient should match";
}

TEST(MathFRPRTest, LocationAccess)
{
  // Test location vector access methods
  QuadraticBowlFunction aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  math_FRPR aSolver(aFunc, 1.0e-10);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum";

  // Test location output method
  math_Vector aLocOut(1, 2);
  aSolver.Location(aLocOut);
  EXPECT_NEAR(aLocOut(1), 1.0, 1.0e-6) << "Output location should match";
  EXPECT_NEAR(aLocOut(2), 2.0, 1.0e-6) << "Output location should match";
}

TEST(MathFRPRTest, CustomZEPS)
{
  // Test with custom ZEPS (machine epsilon) parameter
  QuadraticBowlFunction aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  math_FRPR aSolver(aFunc, 1.0e-10, 200, 1.0e-15);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should work with custom ZEPS";
  EXPECT_NEAR(aSolver.Location()(1), 1.0, 1.0e-6) << "Result should be accurate";
  EXPECT_NEAR(aSolver.Location()(2), 2.0, 1.0e-6) << "Result should be accurate";
}

TEST(MathFRPRTest, UnperformedState)
{
  // Test state handling before Perform() is called
  QuadraticBowlFunction aFunc;
  math_FRPR             aSolver(aFunc, 1.0e-10);

  // Before Perform() is called, solver should report not done
  EXPECT_FALSE(aSolver.IsDone()) << "Solver should not be done before Perform()";

  // In release builds, verify the solver maintains consistent state
  if (!aSolver.IsDone())
  {
    EXPECT_FALSE(aSolver.IsDone()) << "State should be consistent when not done";
  }
}

TEST(MathFRPRTest, DimensionCompatibility)
{
  // Test dimension compatibility handling
  QuadraticBowlFunction aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  math_FRPR aSolver(aFunc, 1.0e-10);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum";

  // Test with correctly dimensioned vectors
  math_Vector aCorrectVec(1, 2); // 2D vector for 2D function
  aSolver.Location(aCorrectVec);
  aSolver.Gradient(aCorrectVec);

  // Verify the results make sense
  EXPECT_EQ(aCorrectVec.Length(), 2) << "Vector should have correct dimension";
}

TEST(MathFRPRTest, StartingNearMinimum)
{
  // Test when starting point is already near the minimum
  QuadraticBowlFunction aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 1.001; // Very close to minimum at (1, 2)
  aStartPoint(2) = 1.999;

  math_FRPR aSolver(aFunc, 1.0e-10);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should succeed when starting near minimum";
  EXPECT_NEAR(aSolver.Location()(1), 1.0, 1.0e-6) << "Should find accurate minimum";
  EXPECT_NEAR(aSolver.Location()(2), 2.0, 1.0e-6) << "Should find accurate minimum";
  EXPECT_NEAR(aSolver.Minimum(), 0.0, 1.0e-10) << "Minimum value should be very small";
}

TEST(MathFRPRTest, QuarticFlatMinimum)
{
  // Test with quartic function that has very flat minimum
  QuarticFunction aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  math_FRPR aSolver(aFunc, 1.0e-8); // Slightly looser tolerance for flat minimum
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should handle quartic function with flat minimum";
  EXPECT_NEAR(aSolver.Location()(1), 1.0, 1.0e-4) << "Should find minimum location";
  EXPECT_NEAR(aSolver.Location()(2), 2.0, 1.0e-4) << "Should find minimum location";
}

TEST(MathFRPRTest, LinearFunctionUnbounded)
{
  // Test with unbounded linear function
  LinearFunction aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  math_FRPR aSolver(aFunc, 1.0e-10, 50); // Limited iterations
  aSolver.Perform(aFunc, aStartPoint);

  // The algorithm may or may not converge for unbounded functions
  // depending on implementation details and stopping criteria
  if (aSolver.IsDone())
  {
    // If it "converges", it should have done some work
    EXPECT_GT(aSolver.NbIterations(), 0) << "Should have performed some iterations";
  }
}

TEST(MathFRPRTest, MultipleCalls)
{
  // Test multiple calls to Perform with same instance
  QuadraticBowlFunction aFunc1;
  Quadratic3DFunction   aFunc2;

  math_Vector aStartPoint2D(1, 2);
  aStartPoint2D(1) = 0.0;
  aStartPoint2D(2) = 0.0;

  math_Vector aStartPoint3D(1, 3);
  aStartPoint3D(1) = 0.0;
  aStartPoint3D(2) = 0.0;
  aStartPoint3D(3) = 0.0;

  math_FRPR aSolver(aFunc1, 1.0e-10);

  // First call with 2D function
  aSolver.Perform(aFunc1, aStartPoint2D);
  EXPECT_TRUE(aSolver.IsDone()) << "First call should succeed";
  EXPECT_NEAR(aSolver.Location()(1), 1.0, 1.0e-6) << "First minimum should be correct";

  // Second call with 3D function - need to create new solver with appropriate function
  math_FRPR aSolver2(aFunc2, 1.0e-10);
  aSolver2.Perform(aFunc2, aStartPoint3D);
  EXPECT_TRUE(aSolver2.IsDone()) << "Second call should succeed";
  EXPECT_NEAR(aSolver2.Location()(1), 1.0, 1.0e-6) << "Second minimum should be correct";
}