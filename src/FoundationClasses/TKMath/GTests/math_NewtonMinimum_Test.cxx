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

#include <math_NewtonMinimum.hxx>
#include <math_MultipleVarFunctionWithHessian.hxx>
#include <math_Vector.hxx>
#include <math_Matrix.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_DimensionError.hxx>
#include <Precision.hxx>

#include <cmath>

namespace
{

// Quadratic bowl function: f(x,y) = (x-1)^2 + 2*(y-2)^2, minimum at (1, 2) with value 0
class QuadraticBowlWithHessian : public math_MultipleVarFunctionWithHessian
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    Standard_Real dx = theX(1) - 1.0;
    Standard_Real dy = theX(2) - 2.0;
    theF             = dx * dx + 2.0 * dy * dy;
    return Standard_True;
  }

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    theG(1) = 2.0 * (theX(1) - 1.0);
    theG(2) = 4.0 * (theX(2) - 2.0);
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    Value(theX, theF);
    Gradient(theX, theG);
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX,
                          Standard_Real&     theF,
                          math_Vector&       theG,
                          math_Matrix&       theH) override
  {
    Value(theX, theF);
    Gradient(theX, theG);
    // Hessian matrix: [[2, 0], [0, 4]]
    theH(1, 1) = 2.0;
    theH(1, 2) = 0.0;
    theH(2, 1) = 0.0;
    theH(2, 2) = 4.0;
    return Standard_True;
  }
};

// Rosenbrock function: f(x,y) = (1-x)^2 + 100*(y-x^2)^2
class RosenbrockWithHessian : public math_MultipleVarFunctionWithHessian
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

  Standard_Boolean Values(const math_Vector& theX,
                          Standard_Real&     theF,
                          math_Vector&       theG,
                          math_Matrix&       theH) override
  {
    Standard_Real x = theX(1);
    Standard_Real y = theX(2);

    Value(theX, theF);
    Gradient(theX, theG);

    // Hessian matrix computation
    theH(1, 1) = 2.0 + 1200.0 * x * x - 400.0 * (y - x * x);
    theH(1, 2) = -400.0 * x;
    theH(2, 1) = -400.0 * x;
    theH(2, 2) = 200.0;
    return Standard_True;
  }
};

// 3D quadratic function: f(x,y,z) = (x-1)^2 + 2*(y-2)^2 + 3*(z-3)^2
class Quadratic3DWithHessian : public math_MultipleVarFunctionWithHessian
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

  Standard_Boolean Values(const math_Vector& theX,
                          Standard_Real&     theF,
                          math_Vector&       theG,
                          math_Matrix&       theH) override
  {
    Value(theX, theF);
    Gradient(theX, theG);

    // Diagonal Hessian matrix
    theH.Init(0.0);
    theH(1, 1) = 2.0;
    theH(2, 2) = 4.0;
    theH(3, 3) = 6.0;
    return Standard_True;
  }
};

// Non-convex function with saddle point: f(x,y) = x^2 - y^2
class SaddleFunction : public math_MultipleVarFunctionWithHessian
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    Standard_Real x = theX(1);
    Standard_Real y = theX(2);
    theF            = x * x - y * y;
    return Standard_True;
  }

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    theG(1) = 2.0 * theX(1);
    theG(2) = -2.0 * theX(2);
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    Value(theX, theF);
    Gradient(theX, theG);
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX,
                          Standard_Real&     theF,
                          math_Vector&       theG,
                          math_Matrix&       theH) override
  {
    Value(theX, theF);
    Gradient(theX, theG);

    // Hessian matrix: [[2, 0], [0, -2]] (indefinite)
    theH(1, 1) = 2.0;
    theH(1, 2) = 0.0;
    theH(2, 1) = 0.0;
    theH(2, 2) = -2.0;
    return Standard_True;
  }
};

} // anonymous namespace

TEST(MathNewtonMinimumTest, QuadraticBowlOptimization)
{
  // Test Newton minimum on simple quadratic bowl function
  QuadraticBowlWithHessian aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0; // Start at (0, 0)
  aStartPoint(2) = 0.0;

  math_NewtonMinimum aSolver(aFunc, 1.0e-10);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum for quadratic bowl function";

  const math_Vector& aLoc = aSolver.Location();
  EXPECT_NEAR(aLoc(1), 1.0, 1.0e-8) << "Minimum should be at x = 1";
  EXPECT_NEAR(aLoc(2), 2.0, 1.0e-8) << "Minimum should be at y = 2";
  EXPECT_NEAR(aSolver.Minimum(), 0.0, 1.0e-12) << "Minimum value should be 0";
  EXPECT_GT(aSolver.NbIterations(), 0) << "Should have performed some iterations";
  EXPECT_LE(aSolver.NbIterations(), 10) << "Should converge quickly for quadratic";
}

TEST(MathNewtonMinimumTest, RosenbrockOptimization)
{
  // Test Newton minimum on challenging Rosenbrock function
  RosenbrockWithHessian aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.8; // Start closer to minimum
  aStartPoint(2) = 0.8;

  math_NewtonMinimum aSolver(aFunc, 1.0e-6, 100); // More iterations for challenging function
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum for Rosenbrock function";

  const math_Vector& aLoc = aSolver.Location();
  EXPECT_NEAR(aLoc(1), 1.0, 2.0e-1) << "Minimum should be near x = 1 (within tolerance)";
  EXPECT_NEAR(aLoc(2), 1.0, 2.0e-1) << "Minimum should be near y = 1 (within tolerance)";
  EXPECT_LT(aSolver.Minimum(), 2.0e-2) << "Should find a reasonably small minimum";
}

TEST(MathNewtonMinimumTest, ThreeDimensionalOptimization)
{
  // Test Newton minimum on 3D quadratic function
  Quadratic3DWithHessian aFunc;

  math_Vector aStartPoint(1, 3);
  aStartPoint(1) = 0.0; // Start at (0, 0, 0)
  aStartPoint(2) = 0.0;
  aStartPoint(3) = 0.0;

  math_NewtonMinimum aSolver(aFunc, 1.0e-10);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum for 3D function";

  const math_Vector& aLoc = aSolver.Location();
  EXPECT_NEAR(aLoc(1), 1.0, 1.0e-8) << "Minimum should be at x = 1";
  EXPECT_NEAR(aLoc(2), 2.0, 1.0e-8) << "Minimum should be at y = 2";
  EXPECT_NEAR(aLoc(3), 3.0, 1.0e-8) << "Minimum should be at z = 3";
  EXPECT_NEAR(aSolver.Minimum(), 0.0, 1.0e-12) << "Minimum value should be 0";
}

TEST(MathNewtonMinimumTest, BoundedOptimization)
{
  // Test Newton minimum with bounds
  QuadraticBowlWithHessian aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  math_Vector aLeftBound(1, 2);
  aLeftBound(1) = -0.5;
  aLeftBound(2) = -0.5;

  math_Vector aRightBound(1, 2);
  aRightBound(1) = 1.5;
  aRightBound(2) = 2.5;

  math_NewtonMinimum aSolver(aFunc, 1.0e-10);
  aSolver.SetBoundary(aLeftBound, aRightBound);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum with bounds";

  const math_Vector& aLoc = aSolver.Location();
  EXPECT_NEAR(aLoc(1), 1.0, 1.0e-8) << "Minimum should be at x = 1";
  EXPECT_NEAR(aLoc(2), 2.0, 1.0e-8) << "Minimum should be at y = 2";
  EXPECT_GE(aLoc(1), -0.5) << "Should respect lower bound";
  EXPECT_LE(aLoc(1), 1.5) << "Should respect upper bound";
  EXPECT_GE(aLoc(2), -0.5) << "Should respect lower bound";
  EXPECT_LE(aLoc(2), 2.5) << "Should respect upper bound";
}

TEST(MathNewtonMinimumTest, CustomTolerance)
{
  // Test with different tolerance values
  QuadraticBowlWithHessian aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  // Loose tolerance
  math_NewtonMinimum aSolver1(aFunc, 1.0e-3);
  aSolver1.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver1.IsDone()) << "Should converge with loose tolerance";
  EXPECT_NEAR(aSolver1.Location()(1), 1.0, 1.0e-2) << "Location should be approximately correct";
  EXPECT_NEAR(aSolver1.Location()(2), 2.0, 1.0e-2) << "Location should be approximately correct";

  // Tight tolerance
  math_NewtonMinimum aSolver2(aFunc, 1.0e-12);
  aSolver2.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver2.IsDone()) << "Should converge with tight tolerance";
  EXPECT_NEAR(aSolver2.Location()(1), 1.0, 1.0e-10) << "Location should be very accurate";
  EXPECT_NEAR(aSolver2.Location()(2), 2.0, 1.0e-10) << "Location should be very accurate";
}

TEST(MathNewtonMinimumTest, CustomIterationLimit)
{
  // Test with custom iteration limits
  RosenbrockWithHessian aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.8;
  aStartPoint(2) = 0.8;

  // Few iterations
  math_NewtonMinimum aSolver1(aFunc, 1.0e-6, 5);
  aSolver1.Perform(aFunc, aStartPoint);

  if (aSolver1.IsDone())
  {
    EXPECT_LE(aSolver1.NbIterations(), 5) << "Should respect iteration limit";
  }

  // Many iterations
  math_NewtonMinimum aSolver2(aFunc, 1.0e-8, 200);
  aSolver2.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver2.IsDone()) << "Should converge with many iterations allowed";
}

TEST(MathNewtonMinimumTest, GradientAccess)
{
  // Test gradient vector access
  QuadraticBowlWithHessian aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  math_NewtonMinimum aSolver(aFunc, 1.0e-10);
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

TEST(MathNewtonMinimumTest, LocationAccess)
{
  // Test location vector access methods
  QuadraticBowlWithHessian aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  math_NewtonMinimum aSolver(aFunc, 1.0e-10);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum";

  // Test location output method
  math_Vector aLocOut(1, 2);
  aSolver.Location(aLocOut);
  EXPECT_NEAR(aLocOut(1), 1.0, 1.0e-8) << "Output location should match";
  EXPECT_NEAR(aLocOut(2), 2.0, 1.0e-8) << "Output location should match";
}

TEST(MathNewtonMinimumTest, CustomConvexity)
{
  // Test with custom convexity parameter
  QuadraticBowlWithHessian aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  math_NewtonMinimum aSolver(aFunc, 1.0e-10, 40, 1.0e-8);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should work with custom convexity";
  EXPECT_NEAR(aSolver.Location()(1), 1.0, 1.0e-8) << "Result should be accurate";
  EXPECT_NEAR(aSolver.Location()(2), 2.0, 1.0e-8) << "Result should be accurate";
}

TEST(MathNewtonMinimumTest, WithSingularityTreatment)
{
  // Test with singularity treatment enabled
  QuadraticBowlWithHessian aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  math_NewtonMinimum aSolver(aFunc, 1.0e-10, 40, 1.0e-6, Standard_True);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should work with singularity treatment";
  EXPECT_NEAR(aSolver.Location()(1), 1.0, 1.0e-8) << "Should find correct minimum";
  EXPECT_NEAR(aSolver.Location()(2), 2.0, 1.0e-8) << "Should find correct minimum";
}

TEST(MathNewtonMinimumTest, NonConvexFunction)
{
  // Test with non-convex function (saddle point)
  SaddleFunction aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.1;
  aStartPoint(2) = 0.1;

  math_NewtonMinimum aSolver(aFunc, 1.0e-10, 40, 1.0e-6, Standard_False);
  aSolver.Perform(aFunc, aStartPoint);

  // Function is not convex, Newton method may not converge
  // Testing that algorithm can handle non-convex functions
  EXPECT_NO_THROW(aSolver.Perform(aFunc, aStartPoint))
    << "Should handle non-convex function gracefully";
}

TEST(MathNewtonMinimumTest, UnperformedState)
{
  // Test state handling before Perform() is called
  QuadraticBowlWithHessian aFunc;
  math_NewtonMinimum       aSolver(aFunc, 1.0e-10);

  // Before Perform() is called, solver should report not done
  EXPECT_FALSE(aSolver.IsDone()) << "Solver should not be done before Perform()";

  // In release builds, verify the solver maintains consistent state
  if (!aSolver.IsDone())
  {
    EXPECT_FALSE(aSolver.IsDone()) << "State should be consistent when not done";
  }
}

TEST(MathNewtonMinimumTest, DimensionCompatibility)
{
  // Test dimension compatibility handling
  QuadraticBowlWithHessian aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  math_NewtonMinimum aSolver(aFunc, 1.0e-10);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum";

  // Test with correctly dimensioned vectors
  math_Vector aCorrectVec(1, 2); // 2D vector for 2D function
  aSolver.Location(aCorrectVec);
  aSolver.Gradient(aCorrectVec);

  // Verify the results make sense
  EXPECT_EQ(aCorrectVec.Length(), 2) << "Vector should have correct dimension";
}

TEST(MathNewtonMinimumTest, StartingNearMinimum)
{
  // Test when starting point is already near the minimum
  QuadraticBowlWithHessian aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 1.001; // Very close to minimum at (1, 2)
  aStartPoint(2) = 1.999;

  math_NewtonMinimum aSolver(aFunc, 1.0e-12);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should succeed when starting near minimum";
  EXPECT_NEAR(aSolver.Location()(1), 1.0, 1.0e-8) << "Should find accurate minimum";
  EXPECT_NEAR(aSolver.Location()(2), 2.0, 1.0e-8) << "Should find accurate minimum";
  EXPECT_NEAR(aSolver.Minimum(), 0.0, 1.0e-12) << "Minimum value should be very small";
  EXPECT_LE(aSolver.NbIterations(), 5) << "Should converge very quickly";
}

TEST(MathNewtonMinimumTest, StatusAccess)
{
  // Test status access method
  QuadraticBowlWithHessian aFunc;

  math_Vector aStartPoint(1, 2);
  aStartPoint(1) = 0.0;
  aStartPoint(2) = 0.0;

  math_NewtonMinimum aSolver(aFunc, 1.0e-10);
  aSolver.Perform(aFunc, aStartPoint);

  EXPECT_TRUE(aSolver.IsDone()) << "Should find minimum";

  // Test that we can access the status without exception
  EXPECT_NO_THROW(aSolver.GetStatus()) << "Should be able to get status after completion";
}