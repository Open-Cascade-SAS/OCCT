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
    const double aDx = theX.At(0) - 1.0;
    const double aDy = theX.At(1) - 2.0;
    theF             = aDx * aDx + aDy * aDy;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    theGrad.ChangeAt(0) = 2.0 * (theX.At(0) - 1.0);
    theGrad.ChangeAt(1) = 2.0 * (theX.At(1) - 2.0);
    return true;
  }

  bool Hessian(const math_Vector& /*theX*/, math_Matrix& theHess)
  {
    theHess.ChangeAt(0, 0) = 2.0;
    theHess.ChangeAt(0, 1) = 0.0;
    theHess.ChangeAt(1, 0) = 0.0;
    theHess.ChangeAt(1, 1) = 2.0;
    return true;
  }
};

//! Rosenbrock function: f(x,y) = 100*(y-x^2)^2 + (1-x)^2
//! Minimum at (1, 1) with f = 0
struct RosenbrockFunc
{
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX  = theX.At(0);
    const double aY  = theX.At(1);
    const double aT1 = aY - aX * aX;
    const double aT2 = 1.0 - aX;
    theF             = 100.0 * aT1 * aT1 + aT2 * aT2;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    const double aX     = theX.At(0);
    const double aY     = theX.At(1);
    theGrad.ChangeAt(0) = -400.0 * aX * (aY - aX * aX) - 2.0 * (1.0 - aX);
    theGrad.ChangeAt(1) = 200.0 * (aY - aX * aX);
    return true;
  }

  bool Hessian(const math_Vector& theX, math_Matrix& theHess)
  {
    const double aX        = theX.At(0);
    const double aY        = theX.At(1);
    theHess.ChangeAt(0, 0) = 1200.0 * aX * aX - 400.0 * aY + 2.0;
    theHess.ChangeAt(0, 1) = -400.0 * aX;
    theHess.ChangeAt(1, 0) = -400.0 * aX;
    theHess.ChangeAt(1, 1) = 200.0;
    return true;
  }
};

//! Booth function: f(x,y) = (x + 2y - 7)^2 + (2x + y - 5)^2
//! Minimum at (1, 3) with f = 0
struct BoothFunc
{
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX  = theX.At(0);
    const double aY  = theX.At(1);
    const double aT1 = aX + 2.0 * aY - 7.0;
    const double aT2 = 2.0 * aX + aY - 5.0;
    theF             = aT1 * aT1 + aT2 * aT2;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    const double aX     = theX.At(0);
    const double aY     = theX.At(1);
    const double aT1    = aX + 2.0 * aY - 7.0;
    const double aT2    = 2.0 * aX + aY - 5.0;
    theGrad.ChangeAt(0) = 2.0 * aT1 + 4.0 * aT2;
    theGrad.ChangeAt(1) = 4.0 * aT1 + 2.0 * aT2;
    return true;
  }

  bool Hessian(const math_Vector& /*theX*/, math_Matrix& theHess)
  {
    theHess.ChangeAt(0, 0) = 10.0; // 2 + 8
    theHess.ChangeAt(0, 1) = 8.0;  // 4 + 4
    theHess.ChangeAt(1, 0) = 8.0;
    theHess.ChangeAt(1, 1) = 10.0; // 8 + 2
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
    for (size_t i = 0; i < theX.Size(); ++i)
    {
      theF += theX.At(i) * theX.At(i);
    }
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    for (size_t i = 0; i < theX.Size(); ++i)
    {
      theGrad.ChangeAt(i) = 2.0 * theX.At(i);
    }
    return true;
  }

  bool Hessian(const math_Vector& theX, math_Matrix& theHess)
  {
    for (size_t i = 0; i < theX.Size(); ++i)
    {
      for (size_t j = 0; j < theX.Size(); ++j)
      {
        theHess.ChangeAt(i, j) = (i == j) ? 2.0 : 0.0;
      }
    }
    return true;
  }
};

//! Coupled quadratic whose constrained minimum has an active lower bound.
struct BoundsCheckingQuadratic
{
  bool IsOutside(const math_Vector& theX)
  {
    const bool isOutside =
      theX.At(0) < 0.0 || theX.At(0) > 1.0 || theX.At(1) < 0.0 || theX.At(1) > 1.0;
    NbOutsideCalls += isOutside ? 1 : 0;
    return isOutside;
  }

  bool Value(const math_Vector& theX, double& theF)
  {
    if (IsOutside(theX))
    {
      return false;
    }
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    theF            = aX * aX - aX * aY + aY * aY + 2.0 * aX;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    if (IsOutside(theX))
    {
      return false;
    }
    theGrad.ChangeAt(0) = 2.0 * theX.At(0) - theX.At(1) + 2.0;
    theGrad.ChangeAt(1) = 2.0 * theX.At(1) - theX.At(0);
    return true;
  }

  bool Hessian(const math_Vector& theX, math_Matrix& theHess)
  {
    if (IsOutside(theX))
    {
      return false;
    }
    theHess.ChangeAt(0, 0) = 2.0;
    theHess.ChangeAt(0, 1) = -1.0;
    theHess.ChangeAt(1, 0) = -1.0;
    theHess.ChangeAt(1, 1) = 2.0;
    return true;
  }

  int NbOutsideCalls = 0;
};

struct NewtonLineSearchCallbackFailure
{
  bool Value(const math_Vector& theX, double& theF)
  {
    if (++NbValueCalls > 1)
    {
      return false;
    }
    theF = theX.At(0) * theX.At(0);
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGradient)
  {
    theGradient.ChangeAt(0) = 2.0 * theX.At(0);
    return true;
  }

  bool Hessian(const math_Vector&, math_Matrix& theHessian)
  {
    theHessian.ChangeAt(0, 0) = 2.0;
    return true;
  }

  int NbValueCalls = 0;
};

struct HessianCallbackFailure
{
  bool Value(const math_Vector& theX, double& theF)
  {
    theF = theX.At(0) * theX.At(0);
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGradient)
  {
    theGradient.ChangeAt(0) = 2.0 * theX.At(0);
    return true;
  }

  bool Hessian(const math_Vector&, math_Matrix&) { return false; }
};

// ============================================================================
// Old API adapter class
// ============================================================================

class QuadraticFuncOld : public math_MultipleVarFunctionWithHessian
{
public:
  int NbVariables() const override { return 2; }

  bool Value(const math_Vector& theX, double& theF) override
  {
    const double aDx = theX(1) - 1.0;
    const double aDy = theX(2) - 2.0;
    theF             = aDx * aDx + aDy * aDy;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    theG(1) = 2.0 * (theX(1) - 1.0);
    theG(2) = 2.0 * (theX(2) - 2.0);
    return true;
  }

  bool Values(const math_Vector& theX, double& theF, math_Vector& theG) override
  {
    return Value(theX, theF) && Gradient(theX, theG);
  }

  bool Values(const math_Vector& theX, double& theF, math_Vector& theG, math_Matrix& theH) override
  {
    if (!Value(theX, theF))
    {
      return false;
    }
    if (!Gradient(theX, theG))
    {
      return false;
    }
    theH(1, 1) = 2.0;
    theH(1, 2) = 0.0;
    theH(2, 1) = 0.0;
    theH(2, 2) = 2.0;
    return true;
  }
};

class BoothFuncOld : public math_MultipleVarFunctionWithHessian
{
public:
  int NbVariables() const override { return 2; }

  bool Value(const math_Vector& theX, double& theF) override
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aX + 2.0 * aY - 7.0;
    const double aT2 = 2.0 * aX + aY - 5.0;
    theF             = aT1 * aT1 + aT2 * aT2;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aX + 2.0 * aY - 7.0;
    const double aT2 = 2.0 * aX + aY - 5.0;
    theG(1)          = 2.0 * aT1 + 4.0 * aT2;
    theG(2)          = 4.0 * aT1 + 2.0 * aT2;
    return true;
  }

  bool Values(const math_Vector& theX, double& theF, math_Vector& theG) override
  {
    return Value(theX, theF) && Gradient(theX, theG);
  }

  bool Values(const math_Vector& theX, double& theF, math_Vector& theG, math_Matrix& theH) override
  {
    if (!Value(theX, theF))
    {
      return false;
    }
    if (!Gradient(theX, theG))
    {
      return false;
    }
    theH(1, 1) = 10.0;
    theH(1, 2) = 8.0;
    theH(2, 1) = 8.0;
    theH(2, 2) = 10.0;
    return true;
  }
};

} // namespace

// ============================================================================
// Basic Newton minimization tests
// ============================================================================

TEST(MathOpt_NewtonTest, Quadratic)
{
  QuadraticFunc aFunc;

  math_Vector aStart(2);
  aStart.ChangeAt(0) = 5.0;
  aStart.ChangeAt(1) = 7.0;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-10;

  auto aResult = MathOpt::Newton(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Solution->At(1), 2.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);

  // Newton should converge in very few iterations for quadratic
  EXPECT_LE(aResult.NbIterations, 5);
}

TEST(MathOpt_NewtonTest, Booth)
{
  BoothFunc aFunc;

  math_Vector aStart(2);
  aStart.ChangeAt(0) = 0.0;
  aStart.ChangeAt(1) = 0.0;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-10;

  auto aResult = MathOpt::Newton(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Solution->At(1), 3.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_NewtonTest, Rosenbrock)
{
  RosenbrockFunc aFunc;

  math_Vector aStart(2);
  aStart.ChangeAt(0) = 0.5; // Start closer to solution for Newton
  aStart.ChangeAt(1) = 0.5;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 100;
  aConfig.Tolerance     = 1.0e-8;

  auto aResult = MathOpt::Newton(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, 1.0e-3);
  EXPECT_NEAR(aResult.Solution->At(1), 1.0, 1.0e-3);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-4);
}

TEST(MathOpt_NewtonTest, Sphere3D)
{
  SphereFunc aFunc;

  math_Vector aStart(3);
  aStart.ChangeAt(0) = 3.0;
  aStart.ChangeAt(1) = -2.0;
  aStart.ChangeAt(2) = 4.0;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-10;

  auto aResult = MathOpt::Newton(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  for (size_t i = 0; i < aResult.Solution->Size(); ++i)
  {
    EXPECT_NEAR(aResult.Solution->At(i), 0.0, THE_TOLERANCE);
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

  math_Vector aStart(2);
  aStart.ChangeAt(0) = 5.0;
  aStart.ChangeAt(1) = 7.0;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-10;

  auto aResult = MathOpt::NewtonModified(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Solution->At(1), 2.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_NewtonTest, ModifiedNewton_Rosenbrock)
{
  RosenbrockFunc aFunc;

  math_Vector aStart(2);
  aStart.ChangeAt(0) = -1.0;
  aStart.ChangeAt(1) = 1.0;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations  = 200;
  aConfig.Tolerance      = 1.0e-6;
  aConfig.Regularization = 1.0e-4; // Higher regularization for Rosenbrock from far start

  auto aResult = MathOpt::NewtonModified(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, 1.0e-3);
  EXPECT_NEAR(aResult.Solution->At(1), 1.0, 1.0e-3);
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
      const double aDx = theX.At(0) - 1.0;
      const double aDy = theX.At(1) - 2.0;
      theF             = aDx * aDx + aDy * aDy;
      return true;
    }

    bool Gradient(const math_Vector& theX, math_Vector& theGrad)
    {
      theGrad.ChangeAt(0) = 2.0 * (theX.At(0) - 1.0);
      theGrad.ChangeAt(1) = 2.0 * (theX.At(1) - 2.0);
      return true;
    }
  };

  QuadraticGradOnly aFunc;

  math_Vector aStart(2);
  aStart.ChangeAt(0) = 5.0;
  aStart.ChangeAt(1) = 7.0;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-8;

  auto aResult = MathOpt::NewtonNumericalHessian(aFunc, aStart, 1.0e-6, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, 1.0e-4);
  EXPECT_NEAR(aResult.Solution->At(1), 2.0, 1.0e-4);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-6);
}

TEST(MathOpt_NewtonTest, FullNumerical_Quadratic)
{
  // Function with value only
  struct QuadraticValueOnly
  {
    bool Value(const math_Vector& theX, double& theF)
    {
      const double aDx = theX.At(0) - 1.0;
      const double aDy = theX.At(1) - 2.0;
      theF             = aDx * aDx + aDy * aDy;
      return true;
    }
  };

  QuadraticValueOnly aFunc;

  math_Vector aStart(2);
  aStart.ChangeAt(0) = 5.0;
  aStart.ChangeAt(1) = 7.0;

  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-6;

  auto aResult = MathOpt::NewtonNumerical(aFunc, aStart, 1.0e-8, 1.0e-6, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, 1.0e-3);
  EXPECT_NEAR(aResult.Solution->At(1), 2.0, 1.0e-3);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-4);
}

TEST(MathOpt_NewtonTest, BoundedCallbacksStayInsideBounds)
{
  math_Vector aStart(size_t{2}, 0.5);
  math_Vector aLower(size_t{2}, 0.0);
  math_Vector anUpper(size_t{2}, 1.0);

  BoundsCheckingQuadratic aNewtonFunc;
  MathOpt::NewtonConfig   aNewtonConfig;
  aNewtonConfig.MaxIterations = 10;
  const MathUtils::VectorResult aNewtonResult =
    MathOpt::NewtonBounded(aNewtonFunc, aStart, aLower, anUpper, aNewtonConfig);

  ASSERT_TRUE(aNewtonResult.IsDone());
  ASSERT_TRUE(aNewtonResult.Solution.has_value());
  ASSERT_TRUE(aNewtonResult.Value.has_value());
  EXPECT_NEAR(aNewtonResult.Solution->At(0), 0.0, 1.0e-10);
  EXPECT_NEAR(aNewtonResult.Solution->At(1), 0.0, 1.0e-10);
  EXPECT_NEAR(*aNewtonResult.Value, 0.0, 1.0e-12);
  EXPECT_EQ(aNewtonFunc.NbOutsideCalls, 0);

  BoundsCheckingQuadratic aBFGSFunc;
  MathOpt::Config         aBFGSConfig;
  aBFGSConfig.MaxIterations = 10;
  const MathUtils::VectorResult aBFGSResult =
    MathOpt::BFGSBounded(aBFGSFunc, aStart, aLower, anUpper, aBFGSConfig);

  ASSERT_TRUE(aBFGSResult.IsDone());
  ASSERT_TRUE(aBFGSResult.Solution.has_value());
  ASSERT_TRUE(aBFGSResult.Value.has_value());
  EXPECT_NEAR(aBFGSResult.Solution->At(0), 0.0, 1.0e-10);
  EXPECT_NEAR(aBFGSResult.Solution->At(1), 0.0, 1.0e-10);
  EXPECT_NEAR(*aBFGSResult.Value, 0.0, 1.0e-12);
  EXPECT_EQ(aBFGSFunc.NbOutsideCalls, 0);
}

TEST(MathOpt_NewtonStatusTest, LineSearchCallbackErrorIsPreserved)
{
  math_Vector aStart(size_t{1}, 1.0);

  NewtonLineSearchCallbackFailure aNewtonFunc;
  EXPECT_EQ(MathOpt::Newton(aNewtonFunc, aStart).Status, MathUtils::Status::CallbackError);

  math_Vector                     aLower(size_t{1}, -2.0);
  math_Vector                     anUpper(size_t{1}, 2.0);
  NewtonLineSearchCallbackFailure aBoundedFunc;
  EXPECT_EQ(MathOpt::NewtonBounded(aBoundedFunc, aStart, aLower, anUpper).Status,
            MathUtils::Status::CallbackError);
}

TEST(MathOpt_NewtonStatusTest, HessianCallbackFailureIsCallbackError)
{
  HessianCallbackFailure aFunc;
  math_Vector            aStart(size_t{1}, 1.0);

  EXPECT_EQ(MathOpt::Newton(aFunc, aStart).Status, MathUtils::Status::CallbackError);
}

// ============================================================================
// Comparison with old API
// ============================================================================

TEST(MathOpt_NewtonTest, CompareWithOldAPI_Quadratic)
{
  QuadraticFuncOld anOldFunc;
  QuadraticFunc    aNewFunc;

  math_Vector anOldStart(1, 2);
  anOldStart(1) = 5.0;
  anOldStart(2) = 7.0;

  math_Vector aNewStart(2);
  aNewStart.ChangeAt(0) = 5.0;
  aNewStart.ChangeAt(1) = 7.0;

  // Old API
  math_NewtonMinimum anOldSolver(anOldFunc, 1.0e-8, 50, 1.0e-10);
  anOldSolver.Perform(anOldFunc, anOldStart);

  // New API
  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-10;
  auto aNewResult       = MathOpt::Newton(aNewFunc, aNewStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find the same minimum
  EXPECT_NEAR(anOldSolver.Location().At(0), aNewResult.Solution->At(0), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Location().At(1), aNewResult.Solution->At(1), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);
}

TEST(MathOpt_NewtonTest, CompareWithOldAPI_Booth)
{
  BoothFuncOld anOldFunc;
  BoothFunc    aNewFunc;

  math_Vector anOldStart(1, 2);
  anOldStart(1) = 0.0;
  anOldStart(2) = 0.0;

  math_Vector aNewStart(2);
  aNewStart.ChangeAt(0) = 0.0;
  aNewStart.ChangeAt(1) = 0.0;

  // Old API
  math_NewtonMinimum anOldSolver(anOldFunc, 1.0e-8, 50, 1.0e-10);
  anOldSolver.Perform(anOldFunc, anOldStart);

  // New API
  MathOpt::NewtonConfig aConfig;
  aConfig.MaxIterations = 50;
  aConfig.Tolerance     = 1.0e-10;
  auto aNewResult       = MathOpt::Newton(aNewFunc, aNewStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find the same minimum
  EXPECT_NEAR(anOldSolver.Location().At(0), aNewResult.Solution->At(0), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Location().At(1), aNewResult.Solution->At(1), THE_TOLERANCE);
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
      const double aDx = theX.At(0) - 1.0;
      const double aDy = theX.At(1) - 2.0;
      theF             = aDx * aDx + aDy * aDy;
      return true;
    }

    bool Gradient(const math_Vector& theX, math_Vector& theGrad)
    {
      theGrad.ChangeAt(0) = 2.0 * (theX.At(0) - 1.0);
      theGrad.ChangeAt(1) = 2.0 * (theX.At(1) - 2.0);
      return true;
    }
  };

  QuadraticNoBFGS aBFGSFunc;

  math_Vector aStart(2);
  aStart.ChangeAt(0) = 5.0;
  aStart.ChangeAt(1) = 7.0;

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

  math_Vector aStart(2);
  aStart.ChangeAt(0) = 100.0;
  aStart.ChangeAt(1) = 100.0;

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
