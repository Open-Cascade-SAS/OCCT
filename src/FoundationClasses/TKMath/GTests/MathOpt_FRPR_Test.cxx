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

#include <MathOpt_FRPR.hxx>
#include <MathOpt_BFGS.hxx>
#include <math_FRPR.hxx>
#include <math_MultipleVarFunctionWithGradient.hxx>
#include <math_Vector.hxx>

#include <cmath>
#include <limits>

namespace
{
constexpr double THE_TOLERANCE = 1.0e-6;

// ============================================================================
// Test function classes for new API
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
};

//! Beale function: f(x,y) = (1.5 - x + xy)^2 + (2.25 - x + xy^2)^2 + (2.625 - x + xy^3)^2
//! Minimum at (3, 0.5) with f = 0
struct BealeFunc
{
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX  = theX.At(0);
    const double aY  = theX.At(1);
    const double aT1 = 1.5 - aX * (1.0 - aY);
    const double aT2 = 2.25 - aX * (1.0 - aY * aY);
    const double aT3 = 2.625 - aX * (1.0 - aY * aY * aY);
    theF             = aT1 * aT1 + aT2 * aT2 + aT3 * aT3;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    const double aX  = theX.At(0);
    const double aY  = theX.At(1);
    const double aY2 = aY * aY;
    const double aY3 = aY2 * aY;
    const double aT1 = 1.5 - aX * (1.0 - aY);
    const double aT2 = 2.25 - aX * (1.0 - aY2);
    const double aT3 = 2.625 - aX * (1.0 - aY3);
    theGrad.ChangeAt(0) =
      -2.0 * aT1 * (1.0 - aY) - 2.0 * aT2 * (1.0 - aY2) - 2.0 * aT3 * (1.0 - aY3);
    theGrad.ChangeAt(1) = 2.0 * aT1 * aX + 2.0 * aT2 * 2.0 * aX * aY + 2.0 * aT3 * 3.0 * aX * aY2;
    return true;
  }
};

struct FRPRLineSearchFailure
{
  explicit FRPRLineSearchFailure(bool theReturnNonFinite = false)
      : ReturnNonFinite(theReturnNonFinite)
  {
  }

  bool Value(const math_Vector& theX, double& theF)
  {
    if (++NbValueCalls > 1)
    {
      if (!ReturnNonFinite)
      {
        return false;
      }
      theF = std::numeric_limits<double>::quiet_NaN();
      return true;
    }
    theF = theX.At(0) * theX.At(0);
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGradient)
  {
    theGradient.ChangeAt(0) = 2.0 * theX.At(0);
    return true;
  }

  bool ReturnNonFinite = false;
  int  NbValueCalls    = 0;
};

// ============================================================================
// Old API adapter class
// ============================================================================

class QuadraticFuncOld : public math_MultipleVarFunctionWithGradient
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
};

class RosenbrockFuncOld : public math_MultipleVarFunctionWithGradient
{
public:
  int NbVariables() const override { return 2; }

  bool Value(const math_Vector& theX, double& theF) override
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aY - aX * aX;
    const double aT2 = 1.0 - aX;
    theF             = 100.0 * aT1 * aT1 + aT2 * aT2;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theG) override
  {
    const double aX = theX(1);
    const double aY = theX(2);
    theG(1)         = -400.0 * aX * (aY - aX * aX) - 2.0 * (1.0 - aX);
    theG(2)         = 200.0 * (aY - aX * aX);
    return true;
  }

  bool Values(const math_Vector& theX, double& theF, math_Vector& theG) override
  {
    return Value(theX, theF) && Gradient(theX, theG);
  }
};

} // namespace

// ============================================================================
// Basic FRPR tests
// ============================================================================

TEST(MathOpt_FRPRTest, Quadratic_FletcherReeves)
{
  QuadraticFunc aFunc;

  math_Vector aStart(2);
  aStart.ChangeAt(0) = 5.0;
  aStart.ChangeAt(1) = 7.0;

  MathOpt::FRPRConfig aConfig(1.0e-8, 100);
  aConfig.Formula = MathOpt::ConjugateGradientFormula::FletcherReeves;

  auto aResult = MathOpt::FRPR(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Solution->At(1), 2.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_FRPRTest, Quadratic_PolakRibiere)
{
  QuadraticFunc aFunc;

  math_Vector aStart(2);
  aStart.ChangeAt(0) = 5.0;
  aStart.ChangeAt(1) = 7.0;

  MathOpt::FRPRConfig aConfig;
  aConfig.MaxIterations = 100;
  aConfig.Tolerance     = 1.0e-8;
  aConfig.Formula       = MathOpt::ConjugateGradientFormula::PolakRibiere;

  auto aResult = MathOpt::FRPR(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Solution->At(1), 2.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_FRPRTest, Booth)
{
  BoothFunc aFunc;

  math_Vector aStart(2);
  aStart.ChangeAt(0) = 0.0;
  aStart.ChangeAt(1) = 0.0;

  MathOpt::FRPRConfig aConfig(1.0e-8, 100);

  auto aResult = MathOpt::FRPR(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Solution->At(1), 3.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_FRPRTest, Rosenbrock)
{
  RosenbrockFunc aFunc;

  math_Vector aStart(2);
  aStart.ChangeAt(0) = -1.0;
  aStart.ChangeAt(1) = 1.0;

  MathOpt::FRPRConfig aConfig;
  aConfig.MaxIterations = 2000;
  aConfig.Tolerance     = 1.0e-6;
  aConfig.Formula       = MathOpt::ConjugateGradientFormula::PolakRibiere;

  auto aResult = MathOpt::FRPR(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, 1.0e-3);
  EXPECT_NEAR(aResult.Solution->At(1), 1.0, 1.0e-3);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-4);
}

TEST(MathOpt_FRPRTest, Sphere3D)
{
  SphereFunc aFunc;

  math_Vector aStart(3);
  aStart.ChangeAt(0) = 3.0;
  aStart.ChangeAt(1) = -2.0;
  aStart.ChangeAt(2) = 4.0;

  MathOpt::FRPRConfig aConfig;
  aConfig.MaxIterations = 100;
  aConfig.Tolerance     = 1.0e-8;

  auto aResult = MathOpt::FRPR(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  for (int i = 1; i <= 3; ++i)
  {
    EXPECT_NEAR(aResult.Solution->At(static_cast<size_t>(i - 1)), 0.0, THE_TOLERANCE);
  }
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_FRPRTest, Sphere5D)
{
  SphereFunc aFunc;

  math_Vector aStart(5);
  for (size_t i = 0; i < aStart.Size(); ++i)
  {
    aStart.ChangeAt(i) = static_cast<double>(i + 1);
  }

  MathOpt::FRPRConfig aConfig;
  aConfig.MaxIterations = 100;
  aConfig.Tolerance     = 1.0e-8;

  auto aResult = MathOpt::FRPR(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  for (int i = 1; i <= 5; ++i)
  {
    EXPECT_NEAR(aResult.Solution->At(static_cast<size_t>(i - 1)), 0.0, THE_TOLERANCE);
  }
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

// ============================================================================
// Numerical gradient tests
// ============================================================================

TEST(MathOpt_FRPRTest, NumericalGradient_Quadratic)
{
  // Value-only function
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

  MathOpt::FRPRConfig aConfig;
  aConfig.MaxIterations = 100;
  aConfig.Tolerance     = 1.0e-6;

  auto aResult = MathOpt::FRPRNumerical(aFunc, aStart, 1.0e-8, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, 1.0e-4);
  EXPECT_NEAR(aResult.Solution->At(1), 2.0, 1.0e-4);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-4);
}

TEST(MathOpt_FRPRStatusTest, LineSearchEvaluationStatusIsPreserved)
{
  math_Vector aStart(size_t{1}, 1.0);

  FRPRLineSearchFailure aCallbackFailure;
  EXPECT_EQ(MathOpt::FRPR(aCallbackFailure, aStart).Status, MathUtils::Status::CallbackError);

  FRPRLineSearchFailure aNonFiniteFailure(true);
  EXPECT_EQ(MathOpt::FRPR(aNonFiniteFailure, aStart).Status, MathUtils::Status::NumericalError);
}

// ============================================================================
// Comparison with old API
// ============================================================================

TEST(MathOpt_FRPRTest, CompareWithOldAPI_Quadratic)
{
  QuadraticFuncOld anOldFunc;
  QuadraticFunc    aNewFunc;

  math_Vector anOldStart(1, 2);
  anOldStart(1) = 5.0;
  anOldStart(2) = 7.0;

  math_Vector aNewStart(2);
  aNewStart.ChangeAt(0) = 5.0;
  aNewStart.ChangeAt(1) = 7.0;

  // Old API - construct with tolerance and max iterations, then call Perform
  math_FRPR anOldSolver(anOldFunc, 1.0e-8, 100);
  anOldSolver.Perform(anOldFunc, anOldStart);

  // New API
  MathOpt::FRPRConfig aConfig;
  aConfig.MaxIterations = 100;
  aConfig.Tolerance     = 1.0e-8;
  auto aNewResult       = MathOpt::FRPR(aNewFunc, aNewStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find the same minimum
  EXPECT_NEAR(anOldSolver.Location().At(0), aNewResult.Solution->At(0), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Location().At(1), aNewResult.Solution->At(1), THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);
}

TEST(MathOpt_FRPRTest, CompareWithOldAPI_Rosenbrock)
{
  RosenbrockFuncOld anOldFunc;
  RosenbrockFunc    aNewFunc;

  math_Vector anOldStart(1, 2);
  anOldStart(1) = -1.0;
  anOldStart(2) = 1.0;

  math_Vector aNewStart(2);
  aNewStart.ChangeAt(0) = -1.0;
  aNewStart.ChangeAt(1) = 1.0;

  // Old API - construct with tolerance and max iterations, then call Perform
  math_FRPR anOldSolver(anOldFunc, 1.0e-6, 2000);
  anOldSolver.Perform(anOldFunc, anOldStart);

  // New API
  MathOpt::FRPRConfig aConfig;
  aConfig.MaxIterations = 2000;
  aConfig.Tolerance     = 1.0e-6;
  auto aNewResult       = MathOpt::FRPR(aNewFunc, aNewStart, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should find the minimum (with reasonable tolerance for Rosenbrock)
  EXPECT_NEAR(anOldSolver.Location().At(0), aNewResult.Solution->At(0), 0.01);
  EXPECT_NEAR(anOldSolver.Location().At(1), aNewResult.Solution->At(1), 0.01);
}

// ============================================================================
// Comparison with BFGS
// ============================================================================

TEST(MathOpt_FRPRTest, CompareWithBFGS)
{
  QuadraticFunc aFunc;

  math_Vector aStart(2);
  aStart.ChangeAt(0) = 5.0;
  aStart.ChangeAt(1) = 7.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;

  MathOpt::FRPRConfig aFRPRConfig;
  aFRPRConfig.MaxIterations = 100;
  aFRPRConfig.Tolerance     = 1.0e-8;

  auto aFRPRResult = MathOpt::FRPR(aFunc, aStart, aFRPRConfig);
  auto aBFGSResult = MathOpt::BFGS(aFunc, aStart, aConfig);

  ASSERT_TRUE(aFRPRResult.IsDone());
  ASSERT_TRUE(aBFGSResult.IsDone());

  // Both should find the same minimum
  EXPECT_NEAR(*aFRPRResult.Value, *aBFGSResult.Value, THE_TOLERANCE);
  EXPECT_NEAR(aFRPRResult.Solution->At(0), aBFGSResult.Solution->At(0), THE_TOLERANCE);
  EXPECT_NEAR(aFRPRResult.Solution->At(1), aBFGSResult.Solution->At(1), THE_TOLERANCE);
}

// ============================================================================
// Formula comparison tests
// ============================================================================

TEST(MathOpt_FRPRTest, FormulaComparison)
{
  QuadraticFunc aFunc;

  math_Vector aStart(2);
  aStart.ChangeAt(0) = 5.0;
  aStart.ChangeAt(1) = 7.0;

  MathOpt::FRPRConfig aConfigFR;
  aConfigFR.MaxIterations = 100;
  aConfigFR.Tolerance     = 1.0e-8;
  aConfigFR.Formula       = MathOpt::ConjugateGradientFormula::FletcherReeves;

  MathOpt::FRPRConfig aConfigPR;
  aConfigPR.MaxIterations = 100;
  aConfigPR.Tolerance     = 1.0e-8;
  aConfigPR.Formula       = MathOpt::ConjugateGradientFormula::PolakRibiere;

  MathOpt::FRPRConfig aConfigHS;
  aConfigHS.MaxIterations = 100;
  aConfigHS.Tolerance     = 1.0e-8;
  aConfigHS.Formula       = MathOpt::ConjugateGradientFormula::HestenesStiefel;

  auto aResultFR = MathOpt::FRPR(aFunc, aStart, aConfigFR);
  auto aResultPR = MathOpt::FRPR(aFunc, aStart, aConfigPR);
  auto aResultHS = MathOpt::FRPR(aFunc, aStart, aConfigHS);

  ASSERT_TRUE(aResultFR.IsDone());
  ASSERT_TRUE(aResultPR.IsDone());
  ASSERT_TRUE(aResultHS.IsDone());

  // All should find the same minimum
  EXPECT_NEAR(*aResultFR.Value, *aResultPR.Value, THE_TOLERANCE);
  EXPECT_NEAR(*aResultFR.Value, *aResultHS.Value, THE_TOLERANCE);
  EXPECT_NEAR(aResultFR.Solution->At(0), 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResultFR.Solution->At(1), 2.0, THE_TOLERANCE);
}
