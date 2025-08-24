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

#include <math_PSO.hxx>
#include <math_PSOParticlesPool.hxx>
#include <math_MultipleVarFunction.hxx>
#include <math_Vector.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>

#include <cmath>

namespace
{

// Simple quadratic function: f(x,y) = (x-1)^2 + (y-2)^2, minimum at (1, 2)
class QuadraticFunction : public math_MultipleVarFunction
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
};

// 1D quadratic function: f(x) = (x-3)^2, minimum at x = 3
class Quadratic1DFunction : public math_MultipleVarFunction
{
public:
  Standard_Integer NbVariables() const override { return 1; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    Standard_Real dx = theX(1) - 3.0;
    theF             = dx * dx;
    return Standard_True;
  }
};

// Rosenbrock function: f(x,y) = (1-x)^2 + 100*(y-x^2)^2
class RosenbrockFunction : public math_MultipleVarFunction
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
};

// Multi-modal function: f(x,y) = -cos(x)*cos(y)*exp(-((x-PI)^2+(y-PI)^2))
class MultiModalFunction : public math_MultipleVarFunction
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    Standard_Real x  = theX(1);
    Standard_Real y  = theX(2);
    Standard_Real dx = x - M_PI;
    Standard_Real dy = y - M_PI;
    theF             = -cos(x) * cos(y) * exp(-(dx * dx + dy * dy));
    return Standard_True;
  }
};

// 3D function: f(x,y,z) = x^2 + 2*y^2 + 3*z^2, minimum at (0,0,0)
class Quadratic3DFunction : public math_MultipleVarFunction
{
public:
  Standard_Integer NbVariables() const override { return 3; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    Standard_Real x = theX(1);
    Standard_Real y = theX(2);
    Standard_Real z = theX(3);
    theF            = x * x + 2.0 * y * y + 3.0 * z * z;
    return Standard_True;
  }
};

} // anonymous namespace

TEST(MathPSOTest, QuadraticFunctionOptimization)
{
  // Test PSO on simple quadratic function
  QuadraticFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -1.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 4.0;
  aUpperBorder(2) = 5.0;

  math_Vector aSteps(1, 2);
  aSteps(1) = 0.1;
  aSteps(2) = 0.1;

  math_PSO aSolver(&aFunc, aLowerBorder, aUpperBorder, aSteps, 20, 50);

  Standard_Real aValue;
  math_Vector   aSolution(1, 2);

  aSolver.Perform(aSteps, aValue, aSolution);

  EXPECT_NEAR(aSolution(1), 1.0, 0.5) << "PSO should find solution near x = 1";
  EXPECT_NEAR(aSolution(2), 2.0, 0.5) << "PSO should find solution near y = 2";
  EXPECT_LT(aValue, 1.0) << "Function value should be small near minimum";
}

TEST(MathPSOTest, OneDimensionalOptimization)
{
  // Test PSO on 1D function
  Quadratic1DFunction aFunc;

  math_Vector aLowerBorder(1, 1);
  aLowerBorder(1) = 0.0;

  math_Vector aUpperBorder(1, 1);
  aUpperBorder(1) = 6.0;

  math_Vector aSteps(1, 1);
  aSteps(1) = 0.1;

  math_PSO aSolver(&aFunc, aLowerBorder, aUpperBorder, aSteps, 15, 30);

  Standard_Real aValue;
  math_Vector   aSolution(1, 1);

  aSolver.Perform(aSteps, aValue, aSolution);

  EXPECT_NEAR(aSolution(1), 3.0, 0.5) << "PSO should find solution near x = 3";
  EXPECT_LT(aValue, 0.5) << "Function value should be small near minimum";
}

TEST(MathPSOTest, ThreeDimensionalOptimization)
{
  // Test PSO on 3D function
  Quadratic3DFunction aFunc;

  math_Vector aLowerBorder(1, 3);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -2.0;
  aLowerBorder(3) = -2.0;

  math_Vector aUpperBorder(1, 3);
  aUpperBorder(1) = 2.0;
  aUpperBorder(2) = 2.0;
  aUpperBorder(3) = 2.0;

  math_Vector aSteps(1, 3);
  aSteps(1) = 0.1;
  aSteps(2) = 0.1;
  aSteps(3) = 0.1;

  math_PSO aSolver(&aFunc, aLowerBorder, aUpperBorder, aSteps, 25, 40);

  Standard_Real aValue;
  math_Vector   aSolution(1, 3);

  aSolver.Perform(aSteps, aValue, aSolution);

  EXPECT_NEAR(aSolution(1), 0.0, 0.5) << "PSO should find solution near x = 0";
  EXPECT_NEAR(aSolution(2), 0.0, 0.5) << "PSO should find solution near y = 0";
  EXPECT_NEAR(aSolution(3), 0.0, 0.5) << "PSO should find solution near z = 0";
  EXPECT_LT(aValue, 1.0) << "Function value should be small near minimum";
}

TEST(MathPSOTest, CustomParticleCount)
{
  // Test PSO with different particle counts
  QuadraticFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -1.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 4.0;
  aUpperBorder(2) = 5.0;

  math_Vector aSteps(1, 2);
  aSteps(1) = 0.2;
  aSteps(2) = 0.2;

  // Few particles
  math_PSO aSolver1(&aFunc, aLowerBorder, aUpperBorder, aSteps, 5, 20);

  Standard_Real aValue1;
  math_Vector   aSolution1(1, 2);

  aSolver1.Perform(aSteps, aValue1, aSolution1);

  EXPECT_TRUE(aSolution1(1) >= -2.0 && aSolution1(1) <= 4.0) << "Solution should be within bounds";
  EXPECT_TRUE(aSolution1(2) >= -1.0 && aSolution1(2) <= 5.0) << "Solution should be within bounds";

  // Many particles
  math_PSO aSolver2(&aFunc, aLowerBorder, aUpperBorder, aSteps, 50, 30);

  Standard_Real aValue2;
  math_Vector   aSolution2(1, 2);

  aSolver2.Perform(aSteps, aValue2, aSolution2);

  EXPECT_TRUE(aSolution2(1) >= -2.0 && aSolution2(1) <= 4.0) << "Solution should be within bounds";
  EXPECT_TRUE(aSolution2(2) >= -1.0 && aSolution2(2) <= 5.0) << "Solution should be within bounds";
}

TEST(MathPSOTest, CustomIterationCount)
{
  // Test PSO with different iteration counts
  QuadraticFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -1.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 4.0;
  aUpperBorder(2) = 5.0;

  math_Vector aSteps(1, 2);
  aSteps(1) = 0.1;
  aSteps(2) = 0.1;

  math_PSO aSolver(&aFunc, aLowerBorder, aUpperBorder, aSteps, 20, 10);

  Standard_Real aValue;
  math_Vector   aSolution(1, 2);

  // Test with fewer iterations
  aSolver.Perform(aSteps, aValue, aSolution, 5);

  EXPECT_TRUE(aSolution(1) >= -2.0 && aSolution(1) <= 4.0) << "Solution should be within bounds";
  EXPECT_TRUE(aSolution(2) >= -1.0 && aSolution(2) <= 5.0) << "Solution should be within bounds";

  // Test with more iterations
  aSolver.Perform(aSteps, aValue, aSolution, 100);

  EXPECT_TRUE(aSolution(1) >= -2.0 && aSolution(1) <= 4.0) << "Solution should be within bounds";
  EXPECT_TRUE(aSolution(2) >= -1.0 && aSolution(2) <= 5.0) << "Solution should be within bounds";
}

TEST(MathPSOTest, RosenbrockOptimization)
{
  // Test PSO on challenging Rosenbrock function
  RosenbrockFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -1.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 2.0;
  aUpperBorder(2) = 3.0;

  math_Vector aSteps(1, 2);
  aSteps(1) = 0.1;
  aSteps(2) = 0.1;

  math_PSO aSolver(&aFunc, aLowerBorder, aUpperBorder, aSteps, 40, 100);

  Standard_Real aValue;
  math_Vector   aSolution(1, 2);

  aSolver.Perform(aSteps, aValue, aSolution);

  // PSO may not find exact minimum due to stochastic nature, but should be reasonably close
  EXPECT_TRUE(aSolution(1) >= -2.0 && aSolution(1) <= 2.0) << "Solution should be within bounds";
  EXPECT_TRUE(aSolution(2) >= -1.0 && aSolution(2) <= 3.0) << "Solution should be within bounds";
  EXPECT_LT(aValue, 100.0) << "Function value should improve from random start";
}

TEST(MathPSOTest, MultiModalOptimization)
{
  // Test PSO on multi-modal function
  MultiModalFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = 0.0;
  aLowerBorder(2) = 0.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 2.0 * M_PI;
  aUpperBorder(2) = 2.0 * M_PI;

  math_Vector aSteps(1, 2);
  aSteps(1) = 0.2;
  aSteps(2) = 0.2;

  math_PSO aSolver(&aFunc, aLowerBorder, aUpperBorder, aSteps, 30, 50);

  Standard_Real aValue;
  math_Vector   aSolution(1, 2);

  aSolver.Perform(aSteps, aValue, aSolution);

  EXPECT_TRUE(aSolution(1) >= 0.0 && aSolution(1) <= 2.0 * M_PI)
    << "Solution should be within bounds";
  EXPECT_TRUE(aSolution(2) >= 0.0 && aSolution(2) <= 2.0 * M_PI)
    << "Solution should be within bounds";
  EXPECT_LT(aValue, 0.0) << "Should find negative value (local/global minimum)";
}

TEST(MathPSOTest, DifferentStepSizes)
{
  // Test PSO with different step sizes
  QuadraticFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -1.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 4.0;
  aUpperBorder(2) = 5.0;

  // Large steps
  math_Vector aLargeSteps(1, 2);
  aLargeSteps(1) = 0.5;
  aLargeSteps(2) = 0.5;

  math_PSO aSolver1(&aFunc, aLowerBorder, aUpperBorder, aLargeSteps, 15, 20);

  Standard_Real aValue1;
  math_Vector   aSolution1(1, 2);

  aSolver1.Perform(aLargeSteps, aValue1, aSolution1);

  EXPECT_TRUE(aSolution1(1) >= -2.0 && aSolution1(1) <= 4.0) << "Solution should be within bounds";
  EXPECT_TRUE(aSolution1(2) >= -1.0 && aSolution1(2) <= 5.0) << "Solution should be within bounds";

  // Small steps
  math_Vector aSmallSteps(1, 2);
  aSmallSteps(1) = 0.05;
  aSmallSteps(2) = 0.05;

  math_PSO aSolver2(&aFunc, aLowerBorder, aUpperBorder, aSmallSteps, 15, 20);

  Standard_Real aValue2;
  math_Vector   aSolution2(1, 2);

  aSolver2.Perform(aSmallSteps, aValue2, aSolution2);

  EXPECT_TRUE(aSolution2(1) >= -2.0 && aSolution2(1) <= 4.0) << "Solution should be within bounds";
  EXPECT_TRUE(aSolution2(2) >= -1.0 && aSolution2(2) <= 5.0) << "Solution should be within bounds";
}

TEST(MathPSOTest, PSOParticlesPoolIntegration)
{
  // Test PSO with explicit particles pool
  QuadraticFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -1.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 4.0;
  aUpperBorder(2) = 5.0;

  math_Vector aSteps(1, 2);
  aSteps(1) = 0.1;
  aSteps(2) = 0.1;

  math_PSO aSolver(&aFunc, aLowerBorder, aUpperBorder, aSteps, 20, 30);

  // Create particles pool
  Standard_Integer      aNbParticles = 20;
  math_PSOParticlesPool aParticlesPool(aNbParticles, 2);

  Standard_Real aValue;
  math_Vector   aSolution(1, 2);

  aSolver.Perform(aParticlesPool, aNbParticles, aValue, aSolution);

  EXPECT_TRUE(aSolution(1) >= -2.0 && aSolution(1) <= 4.0) << "Solution should be within bounds";
  EXPECT_TRUE(aSolution(2) >= -1.0 && aSolution(2) <= 5.0) << "Solution should be within bounds";
}

TEST(MathPSOTest, SmallSearchSpace)
{
  // Test PSO with very small search space
  QuadraticFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = 0.8;
  aLowerBorder(2) = 1.8;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 1.2;
  aUpperBorder(2) = 2.2;

  math_Vector aSteps(1, 2);
  aSteps(1) = 0.05;
  aSteps(2) = 0.05;

  math_PSO aSolver(&aFunc, aLowerBorder, aUpperBorder, aSteps, 10, 20);

  Standard_Real aValue;
  math_Vector   aSolution(1, 2);

  aSolver.Perform(aSteps, aValue, aSolution);

  EXPECT_NEAR(aSolution(1), 1.0, 0.3) << "Should find solution close to minimum in small space";
  EXPECT_NEAR(aSolution(2), 2.0, 0.3) << "Should find solution close to minimum in small space";
  EXPECT_LT(aValue, 0.5) << "Should find small function value";
}

TEST(MathPSOTest, AsymmetricBounds)
{
  // Test PSO with asymmetric bounds
  QuadraticFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -5.0; // Far from minimum
  aLowerBorder(2) = 1.5;  // Close to minimum

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 2.0;  // Includes minimum
  aUpperBorder(2) = 10.0; // Far from minimum

  math_Vector aSteps(1, 2);
  aSteps(1) = 0.2;
  aSteps(2) = 0.2;

  math_PSO aSolver(&aFunc, aLowerBorder, aUpperBorder, aSteps, 25, 40);

  Standard_Real aValue;
  math_Vector   aSolution(1, 2);

  aSolver.Perform(aSteps, aValue, aSolution);

  EXPECT_TRUE(aSolution(1) >= -5.0 && aSolution(1) <= 2.0) << "Solution should be within x bounds";
  EXPECT_TRUE(aSolution(2) >= 1.5 && aSolution(2) <= 10.0) << "Solution should be within y bounds";
  EXPECT_NEAR(aSolution(1), 1.0, 1.5) << "Should find solution reasonably close to minimum";
  EXPECT_NEAR(aSolution(2), 2.0, 2.0) << "Should find solution reasonably close to minimum";
}

TEST(MathPSOTest, MinimalConfiguration)
{
  // Test PSO with minimal configuration (few particles, few iterations)
  Quadratic1DFunction aFunc;

  math_Vector aLowerBorder(1, 1);
  aLowerBorder(1) = 0.0;

  math_Vector aUpperBorder(1, 1);
  aUpperBorder(1) = 6.0;

  math_Vector aSteps(1, 1);
  aSteps(1) = 0.5;

  math_PSO aSolver(&aFunc, aLowerBorder, aUpperBorder, aSteps, 3, 5); // Minimal config

  Standard_Real aValue;
  math_Vector   aSolution(1, 1);

  aSolver.Perform(aSteps, aValue, aSolution, 3);

  EXPECT_TRUE(aSolution(1) >= 0.0 && aSolution(1) <= 6.0) << "Solution should be within bounds";
  // With minimal configuration, we just check it doesn't crash and produces valid output
}

TEST(MathPSOTest, RepeatedPerformCalls)
{
  // Test multiple calls to Perform method
  QuadraticFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -1.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 4.0;
  aUpperBorder(2) = 5.0;

  math_Vector aSteps(1, 2);
  aSteps(1) = 0.1;
  aSteps(2) = 0.1;

  math_PSO aSolver(&aFunc, aLowerBorder, aUpperBorder, aSteps, 15, 20);

  Standard_Real aValue1, aValue2;
  math_Vector   aSolution1(1, 2), aSolution2(1, 2);

  // First call
  aSolver.Perform(aSteps, aValue1, aSolution1);

  EXPECT_TRUE(aSolution1(1) >= -2.0 && aSolution1(1) <= 4.0)
    << "First solution should be within bounds";
  EXPECT_TRUE(aSolution1(2) >= -1.0 && aSolution1(2) <= 5.0)
    << "First solution should be within bounds";

  // Second call
  aSolver.Perform(aSteps, aValue2, aSolution2);

  EXPECT_TRUE(aSolution2(1) >= -2.0 && aSolution2(1) <= 4.0)
    << "Second solution should be within bounds";
  EXPECT_TRUE(aSolution2(2) >= -1.0 && aSolution2(2) <= 5.0)
    << "Second solution should be within bounds";

  // Results may vary due to stochastic nature, but both should be valid
}