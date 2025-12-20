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

#include <MathOpt_Newton.hxx>
#include <MathOpt_BFGS.hxx>
#include <math_NewtonMinimum.hxx>
#include <math_MultipleVarFunctionWithHessian.hxx>
#include <math_Vector.hxx>
#include <math_Matrix.hxx>

#include <cmath>

namespace
{
constexpr double THE_TOLERANCE = 1.0e-6;

// ============================================================================
// Test function classes for new API (with Hessian)
// ============================================================================

//! Quadratic function: f(x,y) = (x-1)^2 + (y-2)^2
//! Minimum at (1, 2) with f = 0
struct QuadraticFunc
{
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aDx = theX(1) - 1.0;
    const double aDy = theX(2) - 2.0;
    theF             = aDx * aDx + aDy * aDy;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    theGrad(1) = 2.0 * (theX(1) - 1.0);
    theGrad(2) = 2.0 * (theX(2) - 2.0);
    return true;
  }

  bool Hessian(const math_Vector& /*theX*/, math_Matrix& theHess)
  {
    theHess(1, 1) = 2.0;
    theHess(1, 2) = 0.0;
    theHess(2, 1) = 0.0;
    theHess(2, 2) = 2.0;
    return true;
  }
};

//! Rosenbrock function: f(x,y) = 100*(y-x^2)^2 + (1-x)^2
//! Minimum at (1, 1) with f = 0
struct RosenbrockFunc
{
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aY - aX * aX;
    const double aT2 = 1.0 - aX;
    theF             = 100.0 * aT1 * aT1 + aT2 * aT2;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    const double aX = theX(1);
    const double aY = theX(2);
    theGrad(1)      = -400.0 * aX * (aY - aX * aX) - 2.0 * (1.0 - aX);
    theGrad(2)      = 200.0 * (aY - aX * aX);
    return true;
  }

  bool Hessian(const math_Vector& theX, math_Matrix& theHess)
  {
    const double aX   = theX(1);
    const double aY   = theX(2);
    theHess(1, 1)     = 1200.0 * aX * aX - 400.0 * aY + 2.0;
    theHess(1, 2)     = -400.0 * aX;
    theHess(2, 1)     = -400.0 * aX;
    theHess(2, 2)     = 200.0;
    return true;
  }
};

//! Booth function: f(x,y) = (x + 2y - 7)^2 + (2x + y - 5)^2
//! Minimum at (1, 3) with f = 0
struct BoothFunc
{
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aX + 2.0 * aY - 7.0;
    const double aT2 = 2.0 * aX + aY - 5.0;
    theF             = aT1 * aT1 + aT2 * aT2;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aX + 2.0 * aY - 7.0;
    const double aT2 = 2.0 * aX + aY - 5.0;
    theGrad(1)       = 2.0 * aT1 + 4.0 * aT2;
    theGrad(2)       = 4.0 * aT1 + 2.0 * aT2;
    return true;
  }

  bool Hessian(const math_Vector& /*theX*/, math_Matrix& theHess)
  {
    theHess(1, 1) = 10.0;  // 2 + 8
    theHess(1, 2) = 8.0;   // 4 + 4
    theHess(2, 1) = 8.0;
    theHess(2, 2) = 10.0;  // 8 + 2
    return true;
  }
};

//! Sphere function in N dimensions: f(x) = sum(x_i^2)
//! Minimum at origin with f = 0
struct SphereFunc
{
  bool Value(const math_Vector& theX, double& theF)
  {
    theF = 0.0;
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theF += theX(i) * theX(i);
    }
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theGrad(i) = 2.0 * theX(i);
    }
    return true;
  }

  bool Hessian(const math_Vector& theX, math_Matrix& theHess)
  {
    const int aLower = theX.Lower();
    const int aUpper = theX.Upper();
    for (int i = aLower; i <= aUpper; ++i)
    {
      for (int j = aLower; j <= aUpper; ++j)
      {
        theHess(i, j) = (i == j) ? 2.0 : 0.0;
      }
    }
    return true;
  }
};

// ============================================================================
// Old API adapter class
// ============================================================================

class QuadraticFuncOld : public math_MultipleVarFunctionWithHessian
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    const double aDx = theX(1) - 1.0;
    const double aDy = theX(2) - 2.0;
    theF             = aDx * aDx + aDy * aDy;
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
    return Value(theX, theF) && Gradient(theX, theG);
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG, math_Matrix& theH) override
  {
    if (!Value(theX, theF)) return Standard_False;
    if (!Gradient(theX, theG)) return Standard_False;
    theH(1, 1) = 2.0;
    theH(1, 2) = 0.0;
    theH(2, 1) = 0.0;
    theH(2, 2) = 2.0;
    return Standard_True;
  }
};

class BoothFuncOld : public math_MultipleVarFunctionWithHessian
{
public:
  Standard_Integer NbVariables() const override { return 2; }

  Standard_Boolean Value(const math_Vector& theX, Standard_Real& theF) override
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aX + 2.0 * aY - 7.0;
    const double aT2 = 2.0 * aX + aY - 5.0;
    theF             = aT1 * aT1 + aT2 * aT2;
    return Standard_True;
  }

  Standard_Boolean Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aX + 2.0 * aY - 7.0;
    const double aT2 = 2.0 * aX + aY - 5.0;
    theG(1)          = 2.0 * aT1 + 4.0 * aT2;
    theG(2)          = 4.0 * aT1 + 2.0 * aT2;
    return Standard_True;
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG) override
  {
    return Value(theX, theF) && Gradient(theX, theG);
  }

  Standard_Boolean Values(const math_Vector& theX, Standard_Real& theF, math_Vector& theG, math_Matrix& theH) override
  {
    if (!Value(theX, theF)) return Standard_False;
    if (!Gradient(theX, theG)) return Standard_False;
    theH(1, 1) = 10.0;
    theH(1, 2) = 8.0;
    theH(2, 1) = 8.0;
    theH(2, 2) = 10.0;
    return Standard_True;
  }
};

} // namespace

// ============================================================================
// Basic Newton minimization tests
// ============================================================================

TEST(MathOpt_NewtonTest, Quadratic)
{
  QuadraticFunc aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 5.0;
  aStart(2) = 7.0;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-10;

  auto aResult = MathOpt::Newton(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 2.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);

  // Newton should converge in very few iterations for quadratic
  EXPECT_LE(aResult.NbIterations, 5);
}

TEST(MathOpt_NewtonTest, Booth)
{
  BoothFunc aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-10;

  auto aResult = MathOpt::Newton(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 3.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_NewtonTest, Rosenbrock)
{
  RosenbrockFunc aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.5;  // Start closer to solution for Newton
  aStart(2) = 0.5;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 100;
  aConfig.Tolerance     = 1.0e-8;

  auto aResult = MathOpt::Newton(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 1.0e-3);
  EXPECT_NEAR((*aResult.Solution)(2), 1.0, 1.0e-3);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-4);
}

TEST(MathOpt_NewtonTest, Sphere3D)
{
  SphereFunc aFunc;

  math_Vector aStart(1, 3);
  aStart(1) = 3.0;
  aStart(2) = -2.0;
  aStart(3) = 4.0;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-10;

  auto aResult = MathOpt::Newton(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  for (int i = 1; i <= 3; ++i)
  {
    EXPECT_NEAR((*aResult.Solution)(i), 0.0, THE_TOLERANCE);
  }
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);

  // Newton should converge in very few iterations for quadratic
  EXPECT_LE(aResult.NbIterations, 5);
}

// ============================================================================
// Modified Newton tests
// ============================================================================

TEST(MathOpt_NewtonTest, ModifiedNewton_Quadratic)
{
  QuadraticFunc aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 5.0;
  aStart(2) = 7.0;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-10;

  auto aResult = MathOpt::NewtonModified(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, THE_TOLERANCE);
  EXPECT_NEAR((*aResult.Solution)(2), 2.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_NewtonTest, ModifiedNewton_Rosenbrock)
{
  RosenbrockFunc aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = -1.0;
  aStart(2) = 1.0;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations   = 200;
  aConfig.Tolerance       = 1.0e-6;
  aConfig.Regularization  = 1.0e-4; // Higher regularization for Rosenbrock from far start

  auto aResult = MathOpt::NewtonModified(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 1.0e-3);
  EXPECT_NEAR((*aResult.Solution)(2), 1.0, 1.0e-3);
}

// ============================================================================
// Numerical Hessian tests
// ============================================================================

TEST(MathOpt_NewtonTest, NumericalHessian_Quadratic)
{
  // Function with gradient only
  struct QuadraticGradOnly
  {
    bool Value(const math_Vector& theX, double& theF)
    {
      const double aDx = theX(1) - 1.0;
      const double aDy = theX(2) - 2.0;
      theF             = aDx * aDx + aDy * aDy;
      return true;
    }

    bool Gradient(const math_Vector& theX, math_Vector& theGrad)
    {
      theGrad(1) = 2.0 * (theX(1) - 1.0);
      theGrad(2) = 2.0 * (theX(2) - 2.0);
      return true;
    }
  };

  QuadraticGradOnly aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 5.0;
  aStart(2) = 7.0;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-8;

  auto aResult = MathOpt::NewtonNumericalHessian(aFunc, aStart, 1.0e-6, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 1.0e-4);
  EXPECT_NEAR((*aResult.Solution)(2), 2.0, 1.0e-4);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-6);
}

TEST(MathOpt_NewtonTest, FullNumerical_Quadratic)
{
  // Function with value only
  struct QuadraticValueOnly
  {
    bool Value(const math_Vector& theX, double& theF)
    {
      const double aDx = theX(1) - 1.0;
      const double aDy = theX(2) - 2.0;
      theF             = aDx * aDx + aDy * aDy;
      return true;
    }
  };

  QuadraticValueOnly aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 5.0;
  aStart(2) = 7.0;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-6;

  auto aResult = MathOpt::NewtonNumerical(aFunc, aStart, 1.0e-8, 1.0e-6, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 1.0e-3);
  EXPECT_NEAR((*aResult.Solution)(2), 2.0, 1.0e-3);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-4);
}

// ============================================================================
// Comparison with old API
// ============================================================================

TEST(MathOpt_NewtonTest, CompareWithOldAPI_Quadratic)
{
  QuadraticFuncOld anOldFunc;
  QuadraticFunc    aNewFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 5.0;
  aStart(2) = 7.0;

  // Old API
  math_NewtonMinimum anOldSolver(anOldFunc, 1.0e-8, 50, 1.0e-10);
  anOldSolver.Perform(anOldFunc, aStart);

  // New API
  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-10;
  auto aNewResult       = MathOpt::Newton(aNewFunc, aStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find the same minimum
  EXPECT_NEAR(anOldSolver.Location()(1), (*aNewResult.Solution)(1), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Location()(2), (*aNewResult.Solution)(2), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);
}

TEST(MathOpt_NewtonTest, CompareWithOldAPI_Booth)
{
  BoothFuncOld anOldFunc;
  BoothFunc    aNewFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  // Old API
  math_NewtonMinimum anOldSolver(anOldFunc, 1.0e-8, 50, 1.0e-10);
  anOldSolver.Perform(anOldFunc, aStart);

  // New API
  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-10;
  auto aNewResult       = MathOpt::Newton(aNewFunc, aStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find the same minimum
  EXPECT_NEAR(anOldSolver.Location()(1), (*aNewResult.Solution)(1), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Location()(2), (*aNewResult.Solution)(2), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);
}

// ============================================================================
// Comparison with BFGS
// ============================================================================

TEST(MathOpt_NewtonTest, CompareWithBFGS)
{
  QuadraticFunc aQuadFunc;

  // Function that works with both Newton and BFGS
  struct QuadraticNoBFGS
  {
    bool Value(const math_Vector& theX, double& theF)
    {
      const double aDx = theX(1) - 1.0;
      const double aDy = theX(2) - 2.0;
      theF             = aDx * aDx + aDy * aDy;
      return true;
    }

    bool Gradient(const math_Vector& theX, math_Vector& theGrad)
    {
      theGrad(1) = 2.0 * (theX(1) - 1.0);
      theGrad(2) = 2.0 * (theX(2) - 2.0);
      return true;
    }
  };

  QuadraticNoBFGS aBFGSFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 5.0;
  aStart(2) = 7.0;

  MathOpt::Config aBFGSConfig;
  aBFGSConfig.MaxIterations = 100;
  aBFGSConfig.XTolerance    = 1.0e-10;
  aBFGSConfig.FTolerance    = 1.0e-12;

  MathOpt::NewtonConfig aNewtonConfig;
  aNewtonConfig.MaxIterations = 50;
  aNewtonConfig.Tolerance     = 1.0e-10;

  auto aNewtonResult = MathOpt::Newton(aQuadFunc, aStart, aNewtonConfig);
  auto aBFGSResult   = MathOpt::BFGS(aBFGSFunc, aStart, aBFGSConfig);

  ASSERT_TRUE(aNewtonResult.IsDone());
  ASSERT_TRUE(aBFGSResult.IsDone());

  // Both should find the same minimum
  EXPECT_NEAR(*aNewtonResult.Value, *aBFGSResult.Value, THE_TOLERANCE);

  // Newton should use fewer iterations for quadratic functions
  EXPECT_LE(aNewtonResult.NbIterations, aBFGSResult.NbIterations);
}

// ============================================================================
// Convergence rate test
// ============================================================================

TEST(MathOpt_NewtonTest, QuadraticConvergence)
{
  QuadraticFunc aFunc;

  math_Vector aStart(1, 2);
  aStart(1) = 100.0;
  aStart(2) = 100.0;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 10;
  aConfig.Tolerance     = 1.0e-12;

  auto aResult = MathOpt::Newton(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());

  // Pure Newton should converge in exactly 1 iteration for a quadratic
  // (or 2 with line search)
  EXPECT_LE(aResult.NbIterations, 2);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-10);
}
