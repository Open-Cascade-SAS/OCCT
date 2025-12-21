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

#include <gtest/gtest.h>

#include <MathSys_LevenbergMarquardt.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Vector.hxx>
#include <math_Matrix.hxx>

#include <cmath>

namespace
{
constexpr double THE_TOLERANCE = 1.0e-6;
constexpr double THE_LOOSE_TOL = 1.0e-3;

// ============================================================================
// Test function classes for new API
// ============================================================================

//! Simple linear system: F(x) = A*x - b
//! System: x1 + x2 = 3, x1 - x2 = 1
//! Solution: x1 = 2, x2 = 1
struct LinearSystem2D
{
  int NbVariables() const { return 2; }

  int NbEquations() const { return 2; }

  bool Value(const math_Vector& theX, math_Vector& theF)
  {
    theF(1) = theX(1) + theX(2) - 3.0;
    theF(2) = theX(1) - theX(2) - 1.0;
    return true;
  }

  bool Derivatives(const math_Vector& /*theX*/, math_Matrix& theD)
  {
    theD(1, 1) = 1.0;
    theD(1, 2) = 1.0;
    theD(2, 1) = 1.0;
    theD(2, 2) = -1.0;
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD)
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

//! Nonlinear system:
//! F1(x,y) = x^2 + y^2 - 4
//! F2(x,y) = x*y - 1
//! Solution near (1.93, 0.52) for starting point (1,1)
struct CircleHyperbola
{
  int NbVariables() const { return 2; }

  int NbEquations() const { return 2; }

  bool Value(const math_Vector& theX, math_Vector& theF)
  {
    theF(1) = theX(1) * theX(1) + theX(2) * theX(2) - 4.0;
    theF(2) = theX(1) * theX(2) - 1.0;
    return true;
  }

  bool Derivatives(const math_Vector& theX, math_Matrix& theD)
  {
    theD(1, 1) = 2.0 * theX(1);
    theD(1, 2) = 2.0 * theX(2);
    theD(2, 1) = theX(2);
    theD(2, 2) = theX(1);
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD)
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

//! Rosenbrock function in residual form for least squares
//! F1(x,y) = 10*(y - x^2)
//! F2(x,y) = 1 - x
//! Minimum at (1,1) with F = (0,0)
struct RosenbrockResidual
{
  int NbVariables() const { return 2; }

  int NbEquations() const { return 2; }

  bool Value(const math_Vector& theX, math_Vector& theF)
  {
    theF(1) = 10.0 * (theX(2) - theX(1) * theX(1));
    theF(2) = 1.0 - theX(1);
    return true;
  }

  bool Derivatives(const math_Vector& theX, math_Matrix& theD)
  {
    theD(1, 1) = -20.0 * theX(1);
    theD(1, 2) = 10.0;
    theD(2, 1) = -1.0;
    theD(2, 2) = 0.0;
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD)
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

//! Powell singular function (4D)
//! F1 = x1 + 10*x2
//! F2 = sqrt(5)*(x3 - x4)
//! F3 = (x2 - 2*x3)^2
//! F4 = sqrt(10)*(x1 - x4)^2
//! Minimum at origin with F = 0
struct PowellSingular
{
  int NbVariables() const { return 4; }

  int NbEquations() const { return 4; }

  bool Value(const math_Vector& theX, math_Vector& theF)
  {
    theF(1) = theX(1) + 10.0 * theX(2);
    theF(2) = std::sqrt(5.0) * (theX(3) - theX(4));
    theF(3) = (theX(2) - 2.0 * theX(3)) * (theX(2) - 2.0 * theX(3));
    theF(4) = std::sqrt(10.0) * (theX(1) - theX(4)) * (theX(1) - theX(4));
    return true;
  }

  bool Derivatives(const math_Vector& theX, math_Matrix& theD)
  {
    theD.Init(0.0);
    // F1 = x1 + 10*x2
    theD(1, 1) = 1.0;
    theD(1, 2) = 10.0;
    // F2 = sqrt(5)*(x3 - x4)
    theD(2, 3) = std::sqrt(5.0);
    theD(2, 4) = -std::sqrt(5.0);
    // F3 = (x2 - 2*x3)^2
    const double aT1 = theX(2) - 2.0 * theX(3);
    theD(3, 2)       = 2.0 * aT1;
    theD(3, 3)       = -4.0 * aT1;
    // F4 = sqrt(10)*(x1 - x4)^2
    const double aT2 = theX(1) - theX(4);
    theD(4, 1)       = 2.0 * std::sqrt(10.0) * aT2;
    theD(4, 4)       = -2.0 * std::sqrt(10.0) * aT2;
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD)
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

//! Exponential fitting problem
//! Data: (t_i, y_i), Model: f(t) = x1 * exp(x2 * t)
//! Residuals: F_i = y_i - x1 * exp(x2 * t_i)
struct ExponentialFit
{
  double myT[5] = {0.0, 1.0, 2.0, 3.0, 4.0};
  double myY[5] = {1.0, 2.7, 7.4, 20.1, 54.6}; // Approx e^t

  int NbVariables() const { return 2; }

  int NbEquations() const { return 5; }

  bool Value(const math_Vector& theX, math_Vector& theF)
  {
    for (int i = 0; i < 5; ++i)
    {
      theF(i + 1) = myY[i] - theX(1) * std::exp(theX(2) * myT[i]);
    }
    return true;
  }

  bool Derivatives(const math_Vector& theX, math_Matrix& theD)
  {
    for (int i = 0; i < 5; ++i)
    {
      const double aExp = std::exp(theX(2) * myT[i]);
      theD(i + 1, 1)    = -aExp;
      theD(i + 1, 2)    = -theX(1) * myT[i] * aExp;
    }
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD)
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

//! Overdetermined system (more equations than variables)
//! F1 = x1 + x2 - 2
//! F2 = x1 - x2 - 0
//! F3 = 2*x1 + x2 - 3
//! Least squares solution: x1 = 1, x2 = 1
struct OverdeterminedSystem
{
  int NbVariables() const { return 2; }

  int NbEquations() const { return 3; }

  bool Value(const math_Vector& theX, math_Vector& theF)
  {
    theF(1) = theX(1) + theX(2) - 2.0;
    theF(2) = theX(1) - theX(2);
    theF(3) = 2.0 * theX(1) + theX(2) - 3.0;
    return true;
  }

  bool Derivatives(const math_Vector& /*theX*/, math_Matrix& theD)
  {
    theD(1, 1) = 1.0;
    theD(1, 2) = 1.0;
    theD(2, 1) = 1.0;
    theD(2, 2) = -1.0;
    theD(3, 1) = 2.0;
    theD(3, 2) = 1.0;
    return true;
  }

  bool Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD)
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

// ============================================================================
// Old API adapters for comparison
// ============================================================================

class CircleHyperbolaOld : public math_FunctionSetWithDerivatives
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Integer NbEquations() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, math_Vector& theF) override
  {
    theF(1) = theX(1) * theX(1) + theX(2) * theX(2) - 4.0;
    theF(2) = theX(1) * theX(2) - 1.0;
    return Standard_True;
  }

  Standard_Boolean Derivatives(const math_Vector& theX, math_Matrix& theD) override
  {
    theD(1, 1) = 2.0 * theX(1);
    theD(1, 2) = 2.0 * theX(2);
    theD(2, 1) = theX(2);
    theD(2, 2) = theX(1);
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD) override
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

class RosenbrockResidualOld : public math_FunctionSetWithDerivatives
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Integer NbEquations() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, math_Vector& theF) override
  {
    theF(1) = 10.0 * (theX(2) - theX(1) * theX(1));
    theF(2) = 1.0 - theX(1);
    return Standard_True;
  }

  Standard_Boolean Derivatives(const math_Vector& theX, math_Matrix& theD) override
  {
    theD(1, 1) = -20.0 * theX(1);
    theD(1, 2) = 10.0;
    theD(2, 1) = -1.0;
    theD(2, 2) = 0.0;
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD) override
  {
    return Value(theX, theF) && Derivatives(theX, theD);
  }
};

} // namespace

// ============================================================================
// Basic Levenberg-Marquardt tests
// ============================================================================

TEST(MathSys_LM_Test, LinearSystem2D)
{
  LinearSystem2D aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  MathSys::LMConfig aConfig;
  aConfig.Tolerance     = 1.0e-10;
  aConfig.FTolerance    = 1.0e-10;
  aConfig.MaxIterations = 100;

  auto aResult = MathSys::LevenbergMarquardt(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 2.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathSys_LM_Test, CircleHyperbola)
{
  CircleHyperbola aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 1.5;
  aStart(2) = 0.5;

  MathSys::LMConfig aConfig;
  aConfig.Tolerance     = 1.0e-10;
  aConfig.FTolerance    = 1.0e-10;
  aConfig.MaxIterations = 100;

  auto aResult = MathSys::LevenbergMarquardt(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());

  // Verify solution satisfies both equations
  const double aX = (*aResult.Solution)(1);
  const double aY = (*aResult.Solution)(2);

  EXPECT_NEAR(aX * aX + aY * aY, 4.0, THE_TOLERANCE);
  EXPECT_NEAR(aX * aY, 1.0, THE_TOLERANCE);
}

TEST(MathSys_LM_Test, RosenbrockResidual)
{
  RosenbrockResidual aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = -1.0;
  aStart(2) = 1.0;

  MathSys::LMConfig aConfig;
  aConfig.Tolerance     = 1.0e-10;
  aConfig.FTolerance    = 1.0e-10;
  aConfig.MaxIterations = 200;

  auto aResult = MathSys::LevenbergMarquardt(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, THE_LOOSE_TOL);
  EXPECT_NEAR((*aResult.Solution)(2), 1.0, THE_LOOSE_TOL);
  EXPECT_LT(*aResult.Value, 0.01);
}

TEST(MathSys_LM_Test, PowellSingular)
{
  PowellSingular aFunc;

  math_Vector aStart(1, 4);
  aStart(1) = 3.0;
  aStart(2) = -1.0;
  aStart(3) = 0.0;
  aStart(4) = 1.0;

  MathSys::LMConfig aConfig;
  aConfig.Tolerance     = 1.0e-10;
  aConfig.FTolerance    = 1.0e-10;
  aConfig.MaxIterations = 500;

  auto aResult = MathSys::LevenbergMarquardt(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());

  // Powell singular function converges to origin
  for (int i = 1; i <= 4; ++i)
  {
    EXPECT_NEAR((*aResult.Solution)(i), 0.0, 0.1);
  }
  EXPECT_LT(*aResult.Value, 0.01);
}

// ============================================================================
// Overdetermined system (nonlinear least squares)
// ============================================================================

TEST(MathSys_LM_Test, OverdeterminedSystem)
{
  OverdeterminedSystem aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  MathSys::LMConfig aConfig;
  aConfig.Tolerance     = 1.0e-10;
  aConfig.MaxIterations = 100;

  auto aResult = MathSys::LevenbergMarquardt(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 1.0, THE_TOLERANCE);
}

TEST(MathSys_LM_Test, ExponentialFit)
{
  ExponentialFit aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.5; // Initial guess for amplitude
  aStart(2) = 0.5; // Initial guess for rate

  MathSys::LMConfig aConfig;
  aConfig.Tolerance     = 1.0e-8;
  aConfig.MaxIterations = 100;

  auto aResult = MathSys::LevenbergMarquardt(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());

  // The fit should be approximately f(t) = 1 * exp(1 * t)
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 0.1);
  EXPECT_NEAR((*aResult.Solution)(2), 1.0, 0.1);
}

// ============================================================================
// Bounded Levenberg-Marquardt tests
// ============================================================================

TEST(MathSys_LM_Test, BoundedCircleHyperbola)
{
  CircleHyperbola aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 1.0;
  aStart(2) = 1.0;

  math_Vector aLower(1, 2);
  aLower(1) = 0.0;
  aLower(2) = 0.0;

  math_Vector aUpper(1, 2);
  aUpper(1) = 5.0;
  aUpper(2) = 5.0;

  MathSys::LMConfig aConfig;
  aConfig.Tolerance     = 1.0e-10;
  aConfig.FTolerance    = 1.0e-10;
  aConfig.MaxIterations = 100;

  auto aResult = MathSys::LevenbergMarquardtBounded(aFunc, aStart, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());

  // Verify solution is within bounds
  const double aX = (*aResult.Solution)(1);
  const double aY = (*aResult.Solution)(2);

  EXPECT_GE(aX, 0.0);
  EXPECT_LE(aX, 5.0);
  EXPECT_GE(aY, 0.0);
  EXPECT_LE(aY, 5.0);

  // Verify solution satisfies both equations
  EXPECT_NEAR(aX * aX + aY * aY, 4.0, THE_TOLERANCE);
  EXPECT_NEAR(aX * aY, 1.0, THE_TOLERANCE);
}

TEST(MathSys_LM_Test, BoundedRosenbrock)
{
  RosenbrockResidual aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = -1.0;
  aStart(2) = 1.0;

  math_Vector aLower(1, 2);
  aLower(1) = -5.0;
  aLower(2) = -5.0;

  math_Vector aUpper(1, 2);
  aUpper(1) = 5.0;
  aUpper(2) = 5.0;

  MathSys::LMConfig aConfig;
  aConfig.Tolerance     = 1.0e-10;
  aConfig.FTolerance    = 1.0e-10;
  aConfig.MaxIterations = 200;

  auto aResult = MathSys::LevenbergMarquardtBounded(aFunc, aStart, aLower, aUpper, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, THE_LOOSE_TOL);
  EXPECT_NEAR((*aResult.Solution)(2), 1.0, THE_LOOSE_TOL);
}

TEST(MathSys_LM_Test, BoundedActiveConstraint)
{
  // System where optimal solution would be outside bounds
  LinearSystem2D aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 1.0;
  aStart(2) = 1.0;

  // Constrain x1 <= 1.5, so solution (2, 1) can't be reached exactly
  math_Vector aLower(1, 2);
  aLower(1) = -10.0;
  aLower(2) = -10.0;

  math_Vector aUpper(1, 2);
  aUpper(1) = 1.5;
  aUpper(2) = 10.0;

  MathSys::LMConfig aConfig;
  aConfig.Tolerance     = 1.0e-10;
  aConfig.MaxIterations = 100;

  auto aResult = MathSys::LevenbergMarquardtBounded(aFunc, aStart, aLower, aUpper, aConfig);

  // Solution should be at or near the bound
  EXPECT_LE((*aResult.Solution)(1), 1.5 + THE_TOLERANCE);
}

// ============================================================================
// Damping parameter tests
// ============================================================================

TEST(MathSys_LM_Test, DampingParameters)
{
  CircleHyperbola aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 1.5;
  aStart(2) = 0.5;

  // Test with different initial lambda values
  double aLambdas[] = {1.0e-6, 1.0e-3, 1.0, 100.0};

  for (double aLambda : aLambdas)
  {
    MathSys::LMConfig aConfig;
    aConfig.Tolerance     = 1.0e-10;
    aConfig.FTolerance    = 1.0e-10;
    aConfig.MaxIterations = 200;
    aConfig.LambdaInit    = aLambda;

    auto aResult = MathSys::LevenbergMarquardt(aFunc, aStart, aConfig);

    ASSERT_TRUE(aResult.IsDone()) << "Failed with lambda = " << aLambda;

    const double aX = (*aResult.Solution)(1);
    const double aY = (*aResult.Solution)(2);

    EXPECT_NEAR(aX * aX + aY * aY, 4.0, THE_TOLERANCE)
      << "Failed equation 1 with lambda = " << aLambda;
    EXPECT_NEAR(aX * aY, 1.0, THE_TOLERANCE) << "Failed equation 2 with lambda = " << aLambda;
  }
}

TEST(MathSys_LM_Test, LambdaUpdateFactors)
{
  RosenbrockResidual aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = -1.0;
  aStart(2) = 1.0;

  // Test with different lambda update factors
  struct
  {
    double myIncrease;
    double myDecrease;
  } aFactors[] = {{2.0, 0.5}, {5.0, 0.2}, {10.0, 0.1}, {20.0, 0.05}};

  for (const auto& aFactor : aFactors)
  {
    MathSys::LMConfig aConfig;
    aConfig.Tolerance      = 1.0e-10;
    aConfig.FTolerance     = 1.0e-10;
    aConfig.MaxIterations  = 300;
    aConfig.LambdaIncrease = aFactor.myIncrease;
    aConfig.LambdaDecrease = aFactor.myDecrease;

    auto aResult = MathSys::LevenbergMarquardt(aFunc, aStart, aConfig);

    ASSERT_TRUE(aResult.IsDone()) << "Failed with increase=" << aFactor.myIncrease
                                  << ", decrease=" << aFactor.myDecrease;
    EXPECT_LT(*aResult.Value, 0.1);
  }
}

// ============================================================================
// Comparison with old API
// ============================================================================

TEST(MathSys_LM_Test, CompareWithOldAPI_CircleHyperbola)
{
  CircleHyperbolaOld anOldFunc;
  CircleHyperbola    aNewFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 1.5;
  aStart(2) = 0.5;

  math_Vector aTol(1, 2, 1.0e-10);

  // Old API
  math_FunctionSetRoot anOldSolver(anOldFunc, aTol, 100);
  anOldSolver.Perform(anOldFunc, aStart);

  // New API
  MathSys::LMConfig aConfig;
  aConfig.Tolerance     = 1.0e-10;
  aConfig.FTolerance    = 1.0e-10;
  aConfig.MaxIterations = 100;
  auto aNewResult       = MathSys::LevenbergMarquardt(aNewFunc, aStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find a valid solution
  const math_Vector& anOldSol = anOldSolver.Root();
  EXPECT_NEAR(anOldSol(1) * anOldSol(1) + anOldSol(2) * anOldSol(2), 4.0, THE_LOOSE_TOL);
  EXPECT_NEAR(anOldSol(1) * anOldSol(2), 1.0, THE_LOOSE_TOL);

  const double aNewX = (*aNewResult.Solution)(1);
  const double aNewY = (*aNewResult.Solution)(2);
  EXPECT_NEAR(aNewX * aNewX + aNewY * aNewY, 4.0, THE_TOLERANCE);
  EXPECT_NEAR(aNewX * aNewY, 1.0, THE_TOLERANCE);
}

TEST(MathSys_LM_Test, CompareWithOldAPI_Rosenbrock)
{
  RosenbrockResidualOld anOldFunc;
  RosenbrockResidual    aNewFunc;

  math_Vector aStart(1, 2);
  aStart(1) = -1.0;
  aStart(2) = 1.0;

  math_Vector aTol(1, 2, 1.0e-10);

  // Old API
  math_FunctionSetRoot anOldSolver(anOldFunc, aTol, 200);
  anOldSolver.Perform(anOldFunc, aStart);

  // New API
  MathSys::LMConfig aConfig;
  aConfig.Tolerance     = 1.0e-10;
  aConfig.FTolerance    = 1.0e-10;
  aConfig.MaxIterations = 200;
  auto aNewResult       = MathSys::LevenbergMarquardt(aNewFunc, aStart, aConfig);

  ASSERT_TRUE(aNewResult.IsDone());

  // New API should find the optimal solution
  EXPECT_NEAR((*aNewResult.Solution)(1), 1.0, THE_LOOSE_TOL);
  EXPECT_NEAR((*aNewResult.Solution)(2), 1.0, THE_LOOSE_TOL);
}

TEST(MathSys_LM_Test, CompareWithOldAPI_Bounded)
{
  CircleHyperbolaOld anOldFunc;
  CircleHyperbola    aNewFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 1.0;
  aStart(2) = 1.0;

  math_Vector aLower(1, 2);
  aLower(1) = 0.0;
  aLower(2) = 0.0;

  math_Vector aUpper(1, 2);
  aUpper(1) = 5.0;
  aUpper(2) = 5.0;

  math_Vector aTol(1, 2, 1.0e-10);

  // Old API with bounds
  math_FunctionSetRoot anOldSolver(anOldFunc, aTol, 100);
  anOldSolver.Perform(anOldFunc, aStart, aLower, aUpper);

  // New API with bounds
  MathSys::LMConfig aConfig;
  aConfig.Tolerance     = 1.0e-10;
  aConfig.FTolerance    = 1.0e-10;
  aConfig.MaxIterations = 100;
  auto aNewResult = MathSys::LevenbergMarquardtBounded(aNewFunc, aStart, aLower, aUpper, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find solutions within bounds
  const math_Vector& anOldSol = anOldSolver.Root();
  EXPECT_GE(anOldSol(1), 0.0);
  EXPECT_LE(anOldSol(1), 5.0);
  EXPECT_GE(anOldSol(2), 0.0);
  EXPECT_LE(anOldSol(2), 5.0);

  const double aNewX = (*aNewResult.Solution)(1);
  const double aNewY = (*aNewResult.Solution)(2);
  EXPECT_GE(aNewX, 0.0);
  EXPECT_LE(aNewX, 5.0);
  EXPECT_GE(aNewY, 0.0);
  EXPECT_LE(aNewY, 5.0);
}

// ============================================================================
// Convergence and robustness tests
// ============================================================================

TEST(MathSys_LM_Test, DifferentStartingPoints)
{
  CircleHyperbola aFunc;

  // Test different starting points
  double aStarts[][2] = {{1.5, 0.5}, {2.0, 0.5}, {1.0, 1.0}, {2.5, 0.5}, {0.5, 2.0}};

  for (const auto& aStartPt : aStarts)
  {
    math_Vector aStart(1, 2);
    aStart(1) = aStartPt[0];
    aStart(2) = aStartPt[1];

    MathSys::LMConfig aConfig;
    aConfig.Tolerance     = 1.0e-10;
    aConfig.FTolerance    = 1.0e-10;
    aConfig.MaxIterations = 100;

    auto aResult = MathSys::LevenbergMarquardt(aFunc, aStart, aConfig);

    ASSERT_TRUE(aResult.IsDone()) << "Failed with start = (" << aStartPt[0] << ", " << aStartPt[1]
                                  << ")";

    const double aX = (*aResult.Solution)(1);
    const double aY = (*aResult.Solution)(2);

    EXPECT_NEAR(aX * aX + aY * aY, 4.0, THE_TOLERANCE)
      << "Failed equation 1 with start = (" << aStartPt[0] << ", " << aStartPt[1] << ")";
    EXPECT_NEAR(aX * aY, 1.0, THE_TOLERANCE)
      << "Failed equation 2 with start = (" << aStartPt[0] << ", " << aStartPt[1] << ")";
  }
}

TEST(MathSys_LM_Test, ConvergenceIterations)
{
  LinearSystem2D aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  MathSys::LMConfig aConfig;
  aConfig.Tolerance     = 1.0e-12;
  aConfig.FTolerance    = 1.0e-12;
  aConfig.MaxIterations = 100;

  auto aResult = MathSys::LevenbergMarquardt(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());

  // Linear system should converge quickly
  EXPECT_LT(aResult.NbIterations, 20);

  EXPECT_NEAR((*aResult.Solution)(1), 2.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 1.0, THE_TOLERANCE);
}

// ============================================================================
// Error handling tests
// ============================================================================

TEST(MathSys_LM_Test, MaxIterationsReached)
{
  RosenbrockResidual aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = -1.0;
  aStart(2) = 1.0;

  MathSys::LMConfig aConfig;
  aConfig.Tolerance     = 1.0e-15; // Very tight tolerance
  aConfig.MaxIterations = 5;       // Very few iterations

  auto aResult = MathSys::LevenbergMarquardt(aFunc, aStart, aConfig);

  // Should either succeed or reach max iterations (not crash)
  EXPECT_TRUE(aResult.Solution.has_value());
}

TEST(MathSys_LM_Test, InvalidInputDimensions)
{
  LinearSystem2D aFunc;

  // Wrong dimension for start vector
  math_Vector aStart(1, 3); // 3 elements but function expects 2
  aStart(1) = 0.0;
  aStart(2) = 0.0;
  aStart(3) = 0.0;

  MathSys::LMConfig aConfig;

  auto aResult = MathSys::LevenbergMarquardt(aFunc, aStart, aConfig);

  EXPECT_FALSE(aResult.IsDone());
  EXPECT_EQ(aResult.Status, MathSys::Status::InvalidInput);
}

TEST(MathSys_LM_Test, BoundedInvalidDimensions)
{
  LinearSystem2D aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  math_Vector aLower(1, 3); // Wrong size
  aLower(1) = -10.0;
  aLower(2) = -10.0;
  aLower(3) = -10.0;

  math_Vector aUpper(1, 2);
  aUpper(1) = 10.0;
  aUpper(2) = 10.0;

  MathSys::LMConfig aConfig;

  auto aResult = MathSys::LevenbergMarquardtBounded(aFunc, aStart, aLower, aUpper, aConfig);

  EXPECT_FALSE(aResult.IsDone());
  EXPECT_EQ(aResult.Status, MathSys::Status::InvalidInput);
}
