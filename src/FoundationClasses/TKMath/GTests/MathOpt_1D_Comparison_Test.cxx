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
#include <MathOpt_Brent.hxx>

// Old API
#include <math_BrentMinimum.hxx>
#include <math_Function.hxx>

#include <cmath>

namespace
{
  constexpr double THE_TOLERANCE = 1.0e-7;
  constexpr double THE_PI        = 3.14159265358979323846;

  // ============================================================================
  // Adapter classes for old API (inherit from math_Function)
  // ============================================================================

  //! f(x) = (x - 3)^2 + 1
  //! Minimum at x = 3, f(3) = 1
  class ParabolaFuncOld : public math_Function
  {
  public:
    Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
    {
      theF = (theX - 3.0) * (theX - 3.0) + 1.0;
      return Standard_True;
    }
  };

  //! f(x) = x^2 - 4x + 3
  //! Minimum at x = 2, f(2) = -1
  class ParabolaFunc2Old : public math_Function
  {
  public:
    Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
    {
      theF = theX * theX - 4.0 * theX + 3.0;
      return Standard_True;
    }
  };

  //! f(x) = cos(x)
  //! Local minimum at x = PI, f(PI) = -1
  class CosFuncOld : public math_Function
  {
  public:
    Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
    {
      theF = std::cos(theX);
      return Standard_True;
    }
  };

  //! f(x) = x^4 - 2x^2
  //! Local minima at x = -1 and x = 1, f(-1) = f(1) = -1
  class QuarticFuncOld : public math_Function
  {
  public:
    Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
    {
      const double aX2 = theX * theX;
      theF             = aX2 * aX2 - 2.0 * aX2;
      return Standard_True;
    }
  };

  //! f(x) = e^x + e^(-x)
  //! Minimum at x = 0, f(0) = 2
  class CoshLikeFuncOld : public math_Function
  {
  public:
    Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
    {
      theF = std::exp(theX) + std::exp(-theX);
      return Standard_True;
    }
  };

  //! f(x) = x^2
  //! Minimum at x = 0, f(0) = 0
  class SquareFuncOld : public math_Function
  {
  public:
    Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
    {
      theF = theX * theX;
      return Standard_True;
    }
  };

  // ============================================================================
  // Function classes for new API
  // ============================================================================

  struct ParabolaFuncNew
  {
    bool Value(double theX, double& theF) const
    {
      theF = (theX - 3.0) * (theX - 3.0) + 1.0;
      return true;
    }
  };

  struct ParabolaFunc2New
  {
    bool Value(double theX, double& theF) const
    {
      theF = theX * theX - 4.0 * theX + 3.0;
      return true;
    }
  };

  struct CosFuncNew
  {
    bool Value(double theX, double& theF) const
    {
      theF = std::cos(theX);
      return true;
    }
  };

  struct QuarticFuncNew
  {
    bool Value(double theX, double& theF) const
    {
      const double aX2 = theX * theX;
      theF             = aX2 * aX2 - 2.0 * aX2;
      return true;
    }
  };

  struct CoshLikeFuncNew
  {
    bool Value(double theX, double& theF) const
    {
      theF = std::exp(theX) + std::exp(-theX);
      return true;
    }
  };

  struct SquareFuncNew
  {
    bool Value(double theX, double& theF) const
    {
      theF = theX * theX;
      return true;
    }
  };
}

// ============================================================================
// math_BrentMinimum vs MathOpt::Brent comparison tests
// ============================================================================

TEST(MathOpt_1D_ComparisonTest, Brent_Parabola)
{
  ParabolaFuncOld anOldFunc;
  ParabolaFuncNew aNewFunc;

  // Old API requires a bracketing triplet (Ax, Bx, Cx) where f(Bx) < f(Ax) and f(Bx) < f(Cx)
  // For f(x) = (x-3)^2 + 1, minimum at x=3
  // Use Ax=0, Bx=3, Cx=6 as the bracket
  math_BrentMinimum anOldSolver(THE_TOLERANCE, 100);
  anOldSolver.Perform(anOldFunc, 0.0, 3.0, 6.0);

  // New API takes interval bounds
  MathOpt::ScalarResult aNewResult = MathOpt::Brent(aNewFunc, 0.0, 6.0);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Location(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, 3.0, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Value, 1.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_ComparisonTest, Brent_ParabolaNegativeMinimum)
{
  ParabolaFunc2Old anOldFunc;
  ParabolaFunc2New aNewFunc;

  // f(x) = x^2 - 4x + 3, minimum at x=2, f(2)=-1
  math_BrentMinimum anOldSolver(THE_TOLERANCE, 100);
  anOldSolver.Perform(anOldFunc, 0.0, 2.0, 5.0);

  MathOpt::ScalarResult aNewResult = MathOpt::Brent(aNewFunc, 0.0, 5.0);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Location(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, 2.0, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Value, -1.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_ComparisonTest, Brent_Cosine)
{
  CosFuncOld anOldFunc;
  CosFuncNew aNewFunc;

  // cos(x) minimum at x=PI, f(PI)=-1
  math_BrentMinimum anOldSolver(THE_TOLERANCE, 100);
  anOldSolver.Perform(anOldFunc, 2.0, THE_PI, 4.0);

  MathOpt::ScalarResult aNewResult = MathOpt::Brent(aNewFunc, 2.0, 4.0);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Location(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, THE_PI, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Value, -1.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_ComparisonTest, Brent_QuarticRight)
{
  QuarticFuncOld anOldFunc;
  QuarticFuncNew aNewFunc;

  // x^4 - 2x^2, right minimum at x=1
  math_BrentMinimum anOldSolver(THE_TOLERANCE, 100);
  anOldSolver.Perform(anOldFunc, 0.0, 1.0, 2.0);

  MathOpt::ScalarResult aNewResult = MathOpt::Brent(aNewFunc, 0.0, 2.0);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Location(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, 1.0, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Value, -1.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_ComparisonTest, Brent_QuarticLeft)
{
  QuarticFuncOld anOldFunc;
  QuarticFuncNew aNewFunc;

  // x^4 - 2x^2, left minimum at x=-1
  math_BrentMinimum anOldSolver(THE_TOLERANCE, 100);
  anOldSolver.Perform(anOldFunc, -2.0, -1.0, 0.0);

  MathOpt::ScalarResult aNewResult = MathOpt::Brent(aNewFunc, -2.0, 0.0);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Location(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, -1.0, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Value, -1.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_ComparisonTest, Brent_CoshLike)
{
  CoshLikeFuncOld anOldFunc;
  CoshLikeFuncNew aNewFunc;

  // e^x + e^(-x), minimum at x=0, f(0)=2
  math_BrentMinimum anOldSolver(THE_TOLERANCE, 100);
  anOldSolver.Perform(anOldFunc, -5.0, 0.0, 5.0);

  MathOpt::ScalarResult aNewResult = MathOpt::Brent(aNewFunc, -5.0, 5.0);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Location(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, 0.0, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Value, 2.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_ComparisonTest, Brent_Square)
{
  SquareFuncOld anOldFunc;
  SquareFuncNew aNewFunc;

  // x^2, minimum at x=0, f(0)=0
  math_BrentMinimum anOldSolver(THE_TOLERANCE, 100);
  anOldSolver.Perform(anOldFunc, -10.0, 0.0, 10.0);

  MathOpt::ScalarResult aNewResult = MathOpt::Brent(aNewFunc, -10.0, 10.0);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Location(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(anOldSolver.Minimum(), *aNewResult.Value, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, 0.0, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Value, 0.0, THE_TOLERANCE);
}

// ============================================================================
// Iteration count comparison tests
// ============================================================================

TEST(MathOpt_1D_ComparisonTest, IterationCount_BrentVsGolden)
{
  ParabolaFuncNew aFunc;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 200;

  MathOpt::ScalarResult aBrentResult  = MathOpt::Brent(aFunc, 0.0, 10.0, aConfig);
  MathOpt::ScalarResult aGoldenResult = MathOpt::Golden(aFunc, 0.0, 10.0, aConfig);

  ASSERT_TRUE(aBrentResult.IsDone());
  ASSERT_TRUE(aGoldenResult.IsDone());

  // Both should find the same minimum
  EXPECT_NEAR(*aBrentResult.Root, *aGoldenResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(*aBrentResult.Value, *aGoldenResult.Value, THE_TOLERANCE);

  // Brent typically converges faster due to parabolic interpolation
  EXPECT_LE(aBrentResult.NbIterations, aGoldenResult.NbIterations);
}

// ============================================================================
// Accuracy comparison tests
// ============================================================================

TEST(MathOpt_1D_ComparisonTest, Accuracy_BrentVsOld)
{
  ParabolaFuncOld anOldFunc;
  ParabolaFuncNew aNewFunc;

  // Test with tighter tolerance
  const double aTightTol = 1.0e-12;

  math_BrentMinimum anOldSolver(aTightTol, 200);
  anOldSolver.Perform(anOldFunc, 0.0, 3.0, 10.0);

  MathOpt::Config aConfig;
  aConfig.XTolerance    = aTightTol;
  aConfig.MaxIterations = 200;
  MathOpt::ScalarResult aNewResult = MathOpt::Brent(aNewFunc, 0.0, 10.0, aConfig);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  // Both should achieve good accuracy
  EXPECT_NEAR(anOldSolver.Location(), 3.0, 1.0e-8);
  EXPECT_NEAR(*aNewResult.Root, 3.0, 1.0e-8);
}

// ============================================================================
// Wide interval comparison tests
// ============================================================================

TEST(MathOpt_1D_ComparisonTest, WideInterval_Parabola)
{
  ParabolaFuncOld anOldFunc;
  ParabolaFuncNew aNewFunc;

  // Very wide interval
  math_BrentMinimum anOldSolver(THE_TOLERANCE, 200);
  anOldSolver.Perform(anOldFunc, -100.0, 3.0, 100.0);

  MathOpt::ScalarResult aNewResult = MathOpt::Brent(aNewFunc, -100.0, 100.0);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Location(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, 3.0, THE_TOLERANCE);
}

// ============================================================================
// Narrow interval comparison tests
// ============================================================================

TEST(MathOpt_1D_ComparisonTest, NarrowInterval_Parabola)
{
  ParabolaFuncOld anOldFunc;
  ParabolaFuncNew aNewFunc;

  // Narrow interval around minimum
  math_BrentMinimum anOldSolver(THE_TOLERANCE, 100);
  anOldSolver.Perform(anOldFunc, 2.9, 3.0, 3.1);

  MathOpt::ScalarResult aNewResult = MathOpt::Brent(aNewFunc, 2.9, 3.1);

  ASSERT_TRUE(anOldSolver.IsDone());
  ASSERT_TRUE(aNewResult.IsDone());

  EXPECT_NEAR(anOldSolver.Location(), *aNewResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(*aNewResult.Root, 3.0, THE_TOLERANCE);
}

// ============================================================================
// MathOpt::Golden comparison tests
// ============================================================================

TEST(MathOpt_1D_ComparisonTest, Golden_Parabola)
{
  ParabolaFuncNew aNewFunc;

  // Golden section should find the same minimum as Brent
  MathOpt::ScalarResult aBrentResult  = MathOpt::Brent(aNewFunc, 0.0, 10.0);
  MathOpt::ScalarResult aGoldenResult = MathOpt::Golden(aNewFunc, 0.0, 10.0);

  ASSERT_TRUE(aBrentResult.IsDone());
  ASSERT_TRUE(aGoldenResult.IsDone());

  EXPECT_NEAR(*aBrentResult.Root, *aGoldenResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(*aBrentResult.Value, *aGoldenResult.Value, THE_TOLERANCE);
  EXPECT_NEAR(*aGoldenResult.Root, 3.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_ComparisonTest, Golden_CoshLike)
{
  CoshLikeFuncNew aNewFunc;

  MathOpt::ScalarResult aBrentResult  = MathOpt::Brent(aNewFunc, -5.0, 5.0);
  MathOpt::ScalarResult aGoldenResult = MathOpt::Golden(aNewFunc, -5.0, 5.0);

  ASSERT_TRUE(aBrentResult.IsDone());
  ASSERT_TRUE(aGoldenResult.IsDone());

  EXPECT_NEAR(*aBrentResult.Root, *aGoldenResult.Root, THE_TOLERANCE);
  EXPECT_NEAR(*aBrentResult.Value, *aGoldenResult.Value, THE_TOLERANCE);
  EXPECT_NEAR(*aGoldenResult.Root, 0.0, THE_TOLERANCE);
}
