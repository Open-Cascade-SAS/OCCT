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

// New API
#include <MathRoot_Newton.hxx>
#include <MathRoot_Brent.hxx>
#include <MathRoot_Bisection.hxx>
#include <MathRoot_Secant.hxx>

// Old API
#include <math_BissecNewton.hxx>
#include <math_BracketedRoot.hxx>
#include <math_FunctionRoot.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <math_NewtonFunctionRoot.hxx>

#include <cmath>

namespace
{
constexpr double THE_TOLERANCE = 1.0e-9;
constexpr double THE_PI        = 3.14159265358979323846;

// ============================================================================
// Adapter classes for old API (inherit from math_FunctionWithDerivative)
// ============================================================================

//! f(x) = x^2 - 2, f'(x) = 2x
//! Root at sqrt(2)
class SqrtTwoFuncOld : public math_FunctionWithDerivative
{
public:
  bool Value(const double theX, double& theF) override
  {
    theF = theX * theX - 2.0;
    return true;
  }

  bool Derivative(const double theX, double& theD) override
  {
    theD = 2.0 * theX;
    return true;
  }

  bool Values(const double theX, double& theF, double& theD) override
  {
    theF = theX * theX - 2.0;
    theD = 2.0 * theX;
    return true;
  }
};

//! f(x) = cos(x) - x, f'(x) = -sin(x) - 1
//! Root at approximately 0.739085
class CosMinusXFuncOld : public math_FunctionWithDerivative
{
public:
  bool Value(const double theX, double& theF) override
  {
    theF = std::cos(theX) - theX;
    return true;
  }

  bool Derivative(const double theX, double& theD) override
  {
    theD = -std::sin(theX) - 1.0;
    return true;
  }

  bool Values(const double theX, double& theF, double& theD) override
  {
    theF = std::cos(theX) - theX;
    theD = -std::sin(theX) - 1.0;
    return true;
  }
};

//! f(x) = sin(x), f'(x) = cos(x)
//! Roots at n*PI
class SinFuncOld : public math_FunctionWithDerivative
{
public:
  bool Value(const double theX, double& theF) override
  {
    theF = std::sin(theX);
    return true;
  }

  bool Derivative(const double theX, double& theD) override
  {
    theD = std::cos(theX);
    return true;
  }

  bool Values(const double theX, double& theF, double& theD) override
  {
    theF = std::sin(theX);
    theD = std::cos(theX);
    return true;
  }
};

//! f(x) = x^3 - x - 2, f'(x) = 3x^2 - 1
//! Root at approximately 1.5214
class CubicFuncOld : public math_FunctionWithDerivative
{
public:
  bool Value(const double theX, double& theF) override
  {
    theF = theX * theX * theX - theX - 2.0;
    return true;
  }

  bool Derivative(const double theX, double& theD) override
  {
    theD = 3.0 * theX * theX - 1.0;
    return true;
  }

  bool Values(const double theX, double& theF, double& theD) override
  {
    theF = theX * theX * theX - theX - 2.0;
    theD = 3.0 * theX * theX - 1.0;
    return true;
  }
};

//! f(x) = e^x - 3, f'(x) = e^x
//! Root at ln(3)
class ExpMinusThreeFuncOld : public math_FunctionWithDerivative
{
public:
  bool Value(const double theX, double& theF) override
  {
    theF = std::exp(theX) - 3.0;
    return true;
  }

  bool Derivative(const double theX, double& theD) override
  {
    theD = std::exp(theX);
    return true;
  }

  bool Values(const double theX, double& theF, double& theD) override
  {
    theF = std::exp(theX) - 3.0;
    theD = std::exp(theX);
    return true;
  }
};

// ============================================================================
// Function classes for new API (simple structs with Value/Values methods)
// ============================================================================

struct SqrtTwoFuncNew
{
  bool Value(double theX, double& theF) const
  {
    theF = theX * theX - 2.0;
    return true;
  }

  bool Values(double theX, double& theF, double& theDf) const
  {
    theF  = theX * theX - 2.0;
    theDf = 2.0 * theX;
    return true;
  }
};

struct CosMinusXFuncNew
{
  bool Value(double theX, double& theF) const
  {
    theF = std::cos(theX) - theX;
    return true;
  }

  bool Values(double theX, double& theF, double& theDf) const
  {
    theF  = std::cos(theX) - theX;
    theDf = -std::sin(theX) - 1.0;
    return true;
  }
};

struct SinFuncNew
{
  bool Value(double theX, double& theF) const
  {
    theF = std::sin(theX);
    return true;
  }

  bool Values(double theX, double& theF, double& theDf) const
  {
    theF  = std::sin(theX);
    theDf = std::cos(theX);
    return true;
  }
};

struct CubicFuncNew
{
  bool Value(double theX, double& theF) const
  {
    theF = theX * theX * theX - theX - 2.0;
    return true;
  }

  bool Values(double theX, double& theF, double& theDf) const
  {
    theF  = theX * theX * theX - theX - 2.0;
    theDf = 3.0 * theX * theX - 1.0;
    return true;
  }
};

struct ExpMinusThreeFuncNew
{
  bool Value(double theX, double& theF) const
  {
    theF = std::exp(theX) - 3.0;
    return true;
  }

  bool Values(double theX, double& theF, double& theDf) const
  {
    theF  = std::exp(theX) - 3.0;
    theDf = std::exp(theX);
    return true;
  }
};
} // namespace

// ============================================================================
// math_BissecNewton vs MathRoot::BisectionNewton comparison tests
// ============================================================================

TEST(MathRoot_ComparisonTest, BisectionNewton_SqrtTwo)
{
  SqrtTwoFuncOld anOldFunc;
  SqrtTwoFuncNew aNewFunc;

  // Old API
  math_BissecNewton anOldSolver(THE_TOLERANCE);
  anOldSolver.Perform(anOldFunc, 1.0, 2.0, 100);

  // New API
  MathRoot::ScalarResult aNewResult = MathRoot::BisectionNewton(aNewFunc, 1.0, 2.0);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Root(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Root(), std::sqrt(2.0), THE_TOLERANCE);
}

TEST(MathRoot_ComparisonTest, BisectionNewton_CosMinusX)
{
  CosMinusXFuncOld anOldFunc;
  CosMinusXFuncNew aNewFunc;

  // Old API
  math_BissecNewton anOldSolver(THE_TOLERANCE);
  anOldSolver.Perform(anOldFunc, 0.0, 1.0, 100);

  // New API
  MathRoot::ScalarResult aNewResult = MathRoot::BisectionNewton(aNewFunc, 0.0, 1.0);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Verify both find the same root
  EXPECT_NEAR(anOldSolver.Root(), *aNewResult.Root, THE_TOLERANCE);

  // Verify the root is correct
  double aFx = std::cos(*aNewResult.Root) - *aNewResult.Root;
  EXPECT_NEAR(aFx, 0.0, THE_TOLERANCE);
}

TEST(MathRoot_ComparisonTest, BisectionNewton_SinPi)
{
  SinFuncOld anOldFunc;
  SinFuncNew aNewFunc;

  // Old API
  math_BissecNewton anOldSolver(THE_TOLERANCE);
  anOldSolver.Perform(anOldFunc, 2.0, 4.0, 100);

  // New API
  MathRoot::ScalarResult aNewResult = MathRoot::BisectionNewton(aNewFunc, 2.0, 4.0);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Root(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, THE_PI, THE_TOLERANCE);
}

TEST(MathRoot_ComparisonTest, BisectionNewton_Cubic)
{
  CubicFuncOld anOldFunc;
  CubicFuncNew aNewFunc;

  // Old API
  math_BissecNewton anOldSolver(THE_TOLERANCE);
  anOldSolver.Perform(anOldFunc, 1.0, 2.0, 100);

  // New API
  MathRoot::ScalarResult aNewResult = MathRoot::BisectionNewton(aNewFunc, 1.0, 2.0);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Root(), *aNewResult.Root, THE_TOLERANCE);
}

// ============================================================================
// math_NewtonFunctionRoot vs MathRoot::Newton comparison tests
// ============================================================================

TEST(MathRoot_ComparisonTest, Newton_SqrtTwo)
{
  SqrtTwoFuncOld anOldFunc;
  SqrtTwoFuncNew aNewFunc;

  // Old API
  math_NewtonFunctionRoot anOldSolver(anOldFunc, 1.5, THE_TOLERANCE, THE_TOLERANCE, 100);

  // New API
  MathRoot::Config aConfig;
  aConfig.XTolerance                = THE_TOLERANCE;
  aConfig.FTolerance                = THE_TOLERANCE;
  MathRoot::ScalarResult aNewResult = MathRoot::Newton(aNewFunc, 1.5, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Root(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, std::sqrt(2.0), THE_TOLERANCE);
}

TEST(MathRoot_ComparisonTest, Newton_ExpMinusThree)
{
  ExpMinusThreeFuncOld anOldFunc;
  ExpMinusThreeFuncNew aNewFunc;

  // Old API
  math_NewtonFunctionRoot anOldSolver(anOldFunc, 1.0, THE_TOLERANCE, THE_TOLERANCE, 100);

  // New API
  MathRoot::Config aConfig;
  aConfig.XTolerance                = THE_TOLERANCE;
  aConfig.FTolerance                = THE_TOLERANCE;
  MathRoot::ScalarResult aNewResult = MathRoot::Newton(aNewFunc, 1.0, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Root(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, std::log(3.0), THE_TOLERANCE);
}

TEST(MathRoot_ComparisonTest, Newton_CosMinusX)
{
  CosMinusXFuncOld anOldFunc;
  CosMinusXFuncNew aNewFunc;

  // Old API
  math_NewtonFunctionRoot anOldSolver(anOldFunc, 0.5, THE_TOLERANCE, THE_TOLERANCE, 100);

  // New API
  MathRoot::Config aConfig;
  aConfig.XTolerance                = THE_TOLERANCE;
  aConfig.FTolerance                = THE_TOLERANCE;
  MathRoot::ScalarResult aNewResult = MathRoot::Newton(aNewFunc, 0.5, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Root(), *aNewResult.Root, THE_TOLERANCE);
}

// ============================================================================
// math_BracketedRoot vs MathRoot::Brent comparison tests
// ============================================================================

TEST(MathRoot_ComparisonTest, Brent_SqrtTwo)
{
  SqrtTwoFuncOld anOldFunc;
  SqrtTwoFuncNew aNewFunc;

  // Old API
  math_BracketedRoot anOldSolver(anOldFunc, 1.0, 2.0, THE_TOLERANCE, 100);

  // New API
  MathRoot::Config aConfig;
  aConfig.XTolerance                = THE_TOLERANCE;
  MathRoot::ScalarResult aNewResult = MathRoot::Brent(aNewFunc, 1.0, 2.0, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Root(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, std::sqrt(2.0), THE_TOLERANCE);
}

TEST(MathRoot_ComparisonTest, Brent_CosMinusX)
{
  CosMinusXFuncOld anOldFunc;
  CosMinusXFuncNew aNewFunc;

  // Old API
  math_BracketedRoot anOldSolver(anOldFunc, 0.0, 1.0, THE_TOLERANCE, 100);

  // New API
  MathRoot::Config aConfig;
  aConfig.XTolerance                = THE_TOLERANCE;
  MathRoot::ScalarResult aNewResult = MathRoot::Brent(aNewFunc, 0.0, 1.0, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Root(), *aNewResult.Root, THE_TOLERANCE);
}

TEST(MathRoot_ComparisonTest, Brent_SinPi)
{
  SinFuncOld anOldFunc;
  SinFuncNew aNewFunc;

  // Old API
  math_BracketedRoot anOldSolver(anOldFunc, 2.0, 4.0, THE_TOLERANCE, 100);

  // New API
  MathRoot::Config aConfig;
  aConfig.XTolerance                = THE_TOLERANCE;
  MathRoot::ScalarResult aNewResult = MathRoot::Brent(aNewFunc, 2.0, 4.0, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Root(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, THE_PI, THE_TOLERANCE);
}

TEST(MathRoot_ComparisonTest, Brent_ExpMinusThree)
{
  ExpMinusThreeFuncOld anOldFunc;
  ExpMinusThreeFuncNew aNewFunc;

  // Old API
  math_BracketedRoot anOldSolver(anOldFunc, 0.0, 2.0, THE_TOLERANCE, 100);

  // New API
  MathRoot::Config aConfig;
  aConfig.XTolerance                = THE_TOLERANCE;
  MathRoot::ScalarResult aNewResult = MathRoot::Brent(aNewFunc, 0.0, 2.0, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Root(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, std::log(3.0), THE_TOLERANCE);
}

// ============================================================================
// math_FunctionRoot vs MathRoot::NewtonBounded comparison tests
// ============================================================================

TEST(MathRoot_ComparisonTest, FunctionRoot_SqrtTwo)
{
  SqrtTwoFuncOld anOldFunc;
  SqrtTwoFuncNew aNewFunc;

  // Old API
  math_FunctionRoot anOldSolver(anOldFunc, 1.5, THE_TOLERANCE, 1.0, 2.0, 100);

  // New API
  MathRoot::Config aConfig;
  aConfig.XTolerance                = THE_TOLERANCE;
  MathRoot::ScalarResult aNewResult = MathRoot::NewtonBounded(aNewFunc, 1.5, 1.0, 2.0, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Root(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, std::sqrt(2.0), THE_TOLERANCE);
}

TEST(MathRoot_ComparisonTest, FunctionRoot_SinPi)
{
  SinFuncOld anOldFunc;
  SinFuncNew aNewFunc;

  // Old API
  math_FunctionRoot anOldSolver(anOldFunc, 3.0, THE_TOLERANCE, 2.0, 4.0, 100);

  // New API
  MathRoot::Config aConfig;
  aConfig.XTolerance                = THE_TOLERANCE;
  MathRoot::ScalarResult aNewResult = MathRoot::NewtonBounded(aNewFunc, 3.0, 2.0, 4.0, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Root(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, THE_PI, THE_TOLERANCE);
}

// ============================================================================
// Iteration count comparison tests
// ============================================================================

TEST(MathRoot_ComparisonTest, IterationCount_NewtonVsBrent)
{
  SqrtTwoFuncNew aFunc;

  MathRoot::Config aConfig;
  aConfig.MaxIterations = 100;

  MathRoot::ScalarResult aNewtonResult = MathRoot::Newton(aFunc, 1.5, aConfig);
  MathRoot::ScalarResult aBrentResult  = MathRoot::Brent(aFunc, 1.0, 2.0, aConfig);

  ASSERT_TRUE(aNewtonResult.IsDone());
  ASSERT_TRUE(aBrentResult.IsDone());

  // Newton should typically converge faster for smooth functions
  EXPECT_LE(aNewtonResult.NbIterations, aBrentResult.NbIterations);

  // Both should find the same root
  EXPECT_NEAR(*aNewtonResult.Root, *aBrentResult.Root, THE_TOLERANCE);
}

// ============================================================================
// Robustness comparison - challenging cases
// ============================================================================

TEST(MathRoot_ComparisonTest, ChallengingCase_CloseToZeroDerivative)
{
  // f(x) = x^3 at x=0 has derivative near zero
  struct CubicNearZeroOld : public math_FunctionWithDerivative
  {
    bool Value(const double theX, double& theF) override
    {
      theF = theX * theX * theX - 0.001;
      return true;
    }

    bool Derivative(const double theX, double& theD) override
    {
      theD = 3.0 * theX * theX;
      return true;
    }

    bool Values(const double theX, double& theF, double& theD) override
    {
      Value(theX, theF);
      Derivative(theX, theD);
      return true;
    }
  };

  struct CubicNearZeroNew
  {
    bool Value(double theX, double& theF) const
    {
      theF = theX * theX * theX - 0.001;
      return true;
    }

    bool Values(double theX, double& theF, double& theDf) const
    {
      theF  = theX * theX * theX - 0.001;
      theDf = 3.0 * theX * theX;
      return true;
    }
  };

  CubicNearZeroOld anOldFunc;
  CubicNearZeroNew aNewFunc;

  // Old API - Brent (more robust for this case)
  math_BracketedRoot anOldSolver(anOldFunc, 0.0, 1.0, THE_TOLERANCE, 100);

  // New API - Brent
  MathRoot::ScalarResult aNewResult = MathRoot::Brent(aNewFunc, 0.0, 1.0);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Root should be at 0.1 (cube root of 0.001)
  EXPECT_NEAR(anOldSolver.Root(), 0.1, 1.0e-6);
  EXPECT_NEAR(*aNewResult.Root, 0.1, 1.0e-6);
  EXPECT_NEAR(anOldSolver.Root(), *aNewResult.Root, THE_TOLERANCE);
}
