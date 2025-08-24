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

#include <math_GlobOptMin.hxx>
#include <math_MultipleVarFunction.hxx>
#include <math_Vector.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Precision.hxx>

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

// Multi-modal function with multiple local minima: f(x,y) = sin(x) + sin(y) + 0.1*(x^2 + y^2)
class MultiModalFunction : public math_MultipleVarFunction
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    Standard_Real x = theX(1);
    Standard_Real y = theX(2);
    theF            = sin(x) + sin(y) + 0.1 * (x * x + y * y);
    return Standard_True;
  }
};

// 1D function: f(x) = sin(x) + 0.5*sin(3*x) with multiple local minima
class MultiModal1DFunction : public math_MultipleVarFunction
{
public:
  Standard_Integer NbVariables() const override { return 1; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    Standard_Real x = theX(1);
    theF            = sin(x) + 0.5 * sin(3.0 * x);
    return Standard_True;
  }
};

// Rosenbrock function in 2D: f(x,y) = (1-x)^2 + 100*(y-x^2)^2
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

// Simple linear function: f(x,y) = x + y (minimum at bounds)
class LinearFunction : public math_MultipleVarFunction
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    theF = theX(1) + theX(2);
    return Standard_True;
  }
};

} // anonymous namespace

TEST(MathGlobOptMinTest, QuadraticFunctionOptimization)
{
  // Test global optimization on simple quadratic function
  QuadraticFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -1.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 3.0;
  aUpperBorder(2) = 4.0;

  math_GlobOptMin aSolver(&aFunc, aLowerBorder, aUpperBorder);
  aSolver.Perform(Standard_True); // Find single solution

  EXPECT_TRUE(aSolver.isDone()) << "Should successfully optimize quadratic function";
  EXPECT_GT(aSolver.NbExtrema(), 0) << "Should find at least one extremum";

  math_Vector aSol(1, 2);
  aSolver.Points(1, aSol);

  EXPECT_NEAR(aSol(1), 1.0, 1.0e-1) << "Should find minimum near x = 1";
  EXPECT_NEAR(aSol(2), 2.0, 1.0e-1) << "Should find minimum near y = 2";
  EXPECT_NEAR(aSolver.GetF(), 0.0, 1.0e-2) << "Function value at minimum should be near 0";
}

TEST(MathGlobOptMinTest, MultiModalFunctionOptimization)
{
  // Test global optimization on multi-modal function
  MultiModalFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -5.0;
  aLowerBorder(2) = -5.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 5.0;
  aUpperBorder(2) = 5.0;

  math_GlobOptMin aSolver(&aFunc, aLowerBorder, aUpperBorder);
  aSolver.Perform(Standard_False); // Find all solutions

  EXPECT_TRUE(aSolver.isDone()) << "Should successfully optimize multi-modal function";
  EXPECT_GE(aSolver.NbExtrema(), 1) << "Should find at least one extremum for multi-modal function";

  // Check that at least one solution exists
  math_Vector aSol(1, 2);
  aSolver.Points(1, aSol);
  EXPECT_TRUE(aSol(1) >= -5.0 && aSol(1) <= 5.0) << "Solution should be within bounds";
  EXPECT_TRUE(aSol(2) >= -5.0 && aSol(2) <= 5.0) << "Solution should be within bounds";
}

TEST(MathGlobOptMinTest, OneDimensionalOptimization)
{
  // Test global optimization on 1D function
  MultiModal1DFunction aFunc;

  math_Vector aLowerBorder(1, 1);
  aLowerBorder(1) = 0.0;

  math_Vector aUpperBorder(1, 1);
  aUpperBorder(1) = 2.0 * M_PI;

  math_GlobOptMin aSolver(&aFunc, aLowerBorder, aUpperBorder);
  aSolver.Perform(Standard_False); // Find all solutions

  EXPECT_TRUE(aSolver.isDone()) << "Should successfully optimize 1D function";
  EXPECT_GT(aSolver.NbExtrema(), 0) << "Should find at least one extremum";

  math_Vector aSol(1, 1);
  aSolver.Points(1, aSol);
  EXPECT_TRUE(aSol(1) >= 0.0 && aSol(1) <= 2.0 * M_PI) << "Solution should be within bounds";
}

TEST(MathGlobOptMinTest, SingleSolutionSearch)
{
  // Test single solution search mode
  QuadraticFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -1.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 3.0;
  aUpperBorder(2) = 4.0;

  math_GlobOptMin aSolver(&aFunc, aLowerBorder, aUpperBorder);
  aSolver.Perform(Standard_True); // Find single solution

  EXPECT_TRUE(aSolver.isDone()) << "Should find single solution";
  EXPECT_EQ(aSolver.NbExtrema(), 1) << "Should find exactly one extremum";
}

TEST(MathGlobOptMinTest, AllSolutionsSearch)
{
  // Test all solutions search mode
  MultiModalFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -3.0;
  aLowerBorder(2) = -3.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 3.0;
  aUpperBorder(2) = 3.0;

  math_GlobOptMin aSolver(&aFunc, aLowerBorder, aUpperBorder);
  aSolver.Perform(Standard_False); // Find all solutions

  EXPECT_TRUE(aSolver.isDone()) << "Should find all solutions";
  EXPECT_GE(aSolver.NbExtrema(), 1) << "Should find at least one extremum";
}

TEST(MathGlobOptMinTest, CustomTolerances)
{
  // Test with custom tolerances
  QuadraticFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -1.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 3.0;
  aUpperBorder(2) = 4.0;

  math_GlobOptMin aSolver(&aFunc, aLowerBorder, aUpperBorder, 9, 1.0e-3, 1.0e-8);

  // Test setting and getting tolerances
  Standard_Real aDiscTol, aSameTol;
  aSolver.GetTol(aDiscTol, aSameTol);
  EXPECT_NEAR(aDiscTol, 1.0e-3, 1.0e-12) << "Discretization tolerance should match";
  EXPECT_NEAR(aSameTol, 1.0e-8, 1.0e-12) << "Same tolerance should match";

  // Update tolerances
  aSolver.SetTol(1.0e-2, 1.0e-6);
  aSolver.GetTol(aDiscTol, aSameTol);
  EXPECT_NEAR(aDiscTol, 1.0e-2, 1.0e-12) << "Updated discretization tolerance should match";
  EXPECT_NEAR(aSameTol, 1.0e-6, 1.0e-12) << "Updated same tolerance should match";

  aSolver.Perform(Standard_True);
  EXPECT_TRUE(aSolver.isDone()) << "Should work with custom tolerances";
}

TEST(MathGlobOptMinTest, LocalParamsReduction)
{
  // Test with local parameters (bounding box reduction)
  QuadraticFunction aFunc;

  math_Vector aGlobalLower(1, 2);
  aGlobalLower(1) = -5.0;
  aGlobalLower(2) = -5.0;

  math_Vector aGlobalUpper(1, 2);
  aGlobalUpper(1) = 5.0;
  aGlobalUpper(2) = 5.0;

  math_GlobOptMin aSolver(&aFunc, aGlobalLower, aGlobalUpper);

  // Set local parameters (smaller search box)
  math_Vector aLocalLower(1, 2);
  aLocalLower(1) = 0.0;
  aLocalLower(2) = 1.0;

  math_Vector aLocalUpper(1, 2);
  aLocalUpper(1) = 2.0;
  aLocalUpper(2) = 3.0;

  aSolver.SetLocalParams(aLocalLower, aLocalUpper);
  aSolver.Perform(Standard_True);

  EXPECT_TRUE(aSolver.isDone()) << "Should work with local parameters";

  math_Vector aSol(1, 2);
  aSolver.Points(1, aSol);
  EXPECT_TRUE(aSol(1) >= 0.0 && aSol(1) <= 2.0) << "Solution should be within local bounds";
  EXPECT_TRUE(aSol(2) >= 1.0 && aSol(2) <= 3.0) << "Solution should be within local bounds";
}

TEST(MathGlobOptMinTest, ContinuitySettings)
{
  // Test continuity settings
  QuadraticFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -1.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 3.0;
  aUpperBorder(2) = 4.0;

  math_GlobOptMin aSolver(&aFunc, aLowerBorder, aUpperBorder);

  // Test default continuity
  Standard_Integer aDefaultCont = aSolver.GetContinuity();
  EXPECT_GE(aDefaultCont, 0) << "Default continuity should be non-negative";

  // Set and test different continuity values
  aSolver.SetContinuity(1);
  EXPECT_EQ(aSolver.GetContinuity(), 1) << "Continuity should be set to 1";

  aSolver.SetContinuity(2);
  EXPECT_EQ(aSolver.GetContinuity(), 2) << "Continuity should be set to 2";

  aSolver.Perform(Standard_True);
  EXPECT_TRUE(aSolver.isDone()) << "Should work with different continuity settings";
}

TEST(MathGlobOptMinTest, FunctionalMinimalValue)
{
  // Test functional minimal value setting
  QuadraticFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -1.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 3.0;
  aUpperBorder(2) = 4.0;

  math_GlobOptMin aSolver(&aFunc, aLowerBorder, aUpperBorder);

  // Test default minimal value
  Standard_Real aDefaultMin = aSolver.GetFunctionalMinimalValue();
  EXPECT_EQ(aDefaultMin, -Precision::Infinite()) << "Default should be negative infinity";

  // Set functional minimal value
  aSolver.SetFunctionalMinimalValue(-1.0);
  EXPECT_NEAR(aSolver.GetFunctionalMinimalValue(), -1.0, 1.0e-12)
    << "Functional minimal value should be set";

  aSolver.Perform(Standard_True);
  EXPECT_TRUE(aSolver.isDone()) << "Should work with functional minimal value";
}

TEST(MathGlobOptMinTest, LipschitzConstantState)
{
  // Test Lipschitz constant state management
  QuadraticFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -1.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 3.0;
  aUpperBorder(2) = 4.0;

  math_GlobOptMin aSolver(&aFunc, aLowerBorder, aUpperBorder);

  // Test default state
  Standard_Boolean aDefaultState = aSolver.GetLipConstState();
  EXPECT_FALSE(aDefaultState) << "Default Lipschitz constant should be unlocked";

  // Lock Lipschitz constant
  aSolver.SetLipConstState(Standard_True);
  EXPECT_TRUE(aSolver.GetLipConstState()) << "Lipschitz constant should be locked";

  // Unlock Lipschitz constant
  aSolver.SetLipConstState(Standard_False);
  EXPECT_FALSE(aSolver.GetLipConstState()) << "Lipschitz constant should be unlocked";

  aSolver.Perform(Standard_True);
  EXPECT_TRUE(aSolver.isDone()) << "Should work with Lipschitz constant state management";
}

TEST(MathGlobOptMinTest, RosenbrockOptimization)
{
  // Test on challenging Rosenbrock function
  RosenbrockFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -1.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 2.0;
  aUpperBorder(2) = 3.0;

  math_GlobOptMin aSolver(&aFunc, aLowerBorder, aUpperBorder, 50, 1.0e-2, 1.0e-6);
  aSolver.Perform(Standard_True);

  EXPECT_TRUE(aSolver.isDone()) << "Should handle Rosenbrock function";
  EXPECT_GT(aSolver.NbExtrema(), 0) << "Should find at least one extremum";

  math_Vector aSol(1, 2);
  aSolver.Points(1, aSol);
  // Rosenbrock minimum is at (1,1) but may not be found exactly due to discretization
  EXPECT_TRUE(aSol(1) >= -2.0 && aSol(1) <= 2.0) << "Solution should be within bounds";
  EXPECT_TRUE(aSol(2) >= -1.0 && aSol(2) <= 3.0) << "Solution should be within bounds";
}

TEST(MathGlobOptMinTest, LinearFunctionOptimization)
{
  // Test on linear function (minimum at boundary)
  LinearFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = 0.0;
  aLowerBorder(2) = 0.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 2.0;
  aUpperBorder(2) = 2.0;

  math_GlobOptMin aSolver(&aFunc, aLowerBorder, aUpperBorder);
  aSolver.Perform(Standard_True);

  EXPECT_TRUE(aSolver.isDone()) << "Should handle linear function";
  EXPECT_GT(aSolver.NbExtrema(), 0) << "Should find at least one extremum";

  math_Vector aSol(1, 2);
  aSolver.Points(1, aSol);
  // For linear function f(x,y) = x + y, minimum should be at (0, 0)
  EXPECT_NEAR(aSol(1), 0.0, 1.0e-1) << "Linear function minimum should be near lower bound";
  EXPECT_NEAR(aSol(2), 0.0, 1.0e-1) << "Linear function minimum should be near lower bound";
}

TEST(MathGlobOptMinTest, SetGlobalParamsMethod)
{
  // Test SetGlobalParams method
  QuadraticFunction aFunc1;
  LinearFunction    aFunc2;

  math_Vector aLowerBorder1(1, 2);
  aLowerBorder1(1) = -2.0;
  aLowerBorder1(2) = -1.0;

  math_Vector aUpperBorder1(1, 2);
  aUpperBorder1(1) = 3.0;
  aUpperBorder1(2) = 4.0;

  // Create solver with first function
  math_GlobOptMin aSolver(&aFunc1, aLowerBorder1, aUpperBorder1);
  aSolver.Perform(Standard_True);

  EXPECT_TRUE(aSolver.isDone()) << "Should work with first function";

  // Change to second function and different bounds
  math_Vector aLowerBorder2(1, 2);
  aLowerBorder2(1) = 0.0;
  aLowerBorder2(2) = 0.0;

  math_Vector aUpperBorder2(1, 2);
  aUpperBorder2(1) = 1.0;
  aUpperBorder2(2) = 1.0;

  aSolver.SetGlobalParams(&aFunc2, aLowerBorder2, aUpperBorder2);
  aSolver.Perform(Standard_True);

  EXPECT_TRUE(aSolver.isDone()) << "Should work after changing global params";
}

TEST(MathGlobOptMinTest, MultipleExtremaAccess)
{
  // Test accessing multiple extrema
  MultiModalFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = -2.0;
  aLowerBorder(2) = -2.0;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 2.0;
  aUpperBorder(2) = 2.0;

  math_GlobOptMin aSolver(&aFunc, aLowerBorder, aUpperBorder);
  aSolver.Perform(Standard_False); // Find all solutions

  EXPECT_TRUE(aSolver.isDone()) << "Should find multiple solutions";
  Standard_Integer aNbSol = aSolver.NbExtrema();
  EXPECT_GT(aNbSol, 0) << "Should have at least one solution";

  // Test accessing all solutions
  for (Standard_Integer i = 1; i <= aNbSol; ++i)
  {
    math_Vector aSol(1, 2);
    EXPECT_NO_THROW(aSolver.Points(i, aSol)) << "Should be able to access solution " << i;
    EXPECT_TRUE(aSol(1) >= -2.0 && aSol(1) <= 2.0)
      << "Solution " << i << " should be within bounds";
    EXPECT_TRUE(aSol(2) >= -2.0 && aSol(2) <= 2.0)
      << "Solution " << i << " should be within bounds";
  }
}

TEST(MathGlobOptMinTest, SmallSearchSpace)
{
  // Test with very small search space
  QuadraticFunction aFunc;

  math_Vector aLowerBorder(1, 2);
  aLowerBorder(1) = 0.99;
  aLowerBorder(2) = 1.99;

  math_Vector aUpperBorder(1, 2);
  aUpperBorder(1) = 1.01;
  aUpperBorder(2) = 2.01;

  math_GlobOptMin aSolver(&aFunc, aLowerBorder, aUpperBorder, 9, 1.0e-3, 1.0e-8);
  aSolver.Perform(Standard_True);

  EXPECT_TRUE(aSolver.isDone()) << "Should handle small search space";

  math_Vector aSol(1, 2);
  aSolver.Points(1, aSol);
  EXPECT_NEAR(aSol(1), 1.0, 0.02) << "Should find solution close to global minimum";
  EXPECT_NEAR(aSol(2), 2.0, 0.02) << "Should find solution close to global minimum";
}