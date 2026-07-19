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

#include <MathRoot_Multiple.hxx>
#include <MathUtils_FunctorScalar.hxx>
#include <math_FunctionRoots.hxx>
#include <math_FunctionWithDerivative.hxx>

#include <cmath>

#include <gtest/gtest.h>

namespace
{
constexpr double THE_TOLERANCE = 1e-8;
constexpr double THE_PI        = 3.14159265358979323846;
} // namespace

//=================================================================================================
// FindAllRoots Tests (Value-only interface)
//=================================================================================================

TEST(MathRoot_Multiple, FindAllRoots_NoRoots)
{
  // f(x) = x^2 + 1, no real roots
  MathUtils::Polynomial aFunc({1.0, 0.0, 1.0});

  auto aResult = MathRoot::FindAllRoots(aFunc, -10.0, 10.0);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots(), 0u);
}

TEST(MathRoot_Multiple, FindAllRoots_SingleRoot)
{
  // f(x) = x - 2, single root at x = 2
  MathUtils::Polynomial aFunc({-2.0, 1.0});

  auto aResult = MathRoot::FindAllRoots(aFunc, 0.0, 5.0);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots(), 1u);
  EXPECT_NEAR(aResult.Roots[0], 2.0, THE_TOLERANCE);
}

TEST(MathRoot_Multiple, FindAllRoots_TwoRoots)
{
  // f(x) = x^2 - 4, roots at x = -2 and x = 2
  MathUtils::Polynomial aFunc({-4.0, 0.0, 1.0});

  auto aResult = MathRoot::FindAllRoots(aFunc, -5.0, 5.0);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots(), 2u);
  EXPECT_NEAR(aResult.Roots[0], -2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 2.0, THE_TOLERANCE);
}

TEST(MathRoot_Multiple, FindAllRoots_ThreeRoots)
{
  // f(x) = (x - 1)(x - 2)(x - 3) = x^3 - 6x^2 + 11x - 6
  MathUtils::Polynomial aFunc({-6.0, 11.0, -6.0, 1.0});

  auto aResult = MathRoot::FindAllRoots(aFunc, 0.0, 4.0);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots(), 3u);
  EXPECT_NEAR(aResult.Roots[0], 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[2], 3.0, THE_TOLERANCE);
}

TEST(MathRoot_Multiple, FindAllRoots_SineFunction)
{
  // f(x) = sin(x), roots at 0, pi, 2*pi, 3*pi in [0, 10]
  MathUtils::Sine aSine;

  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples  = 200;
  aConfig.FTolerance = 1e-10;

  auto aResult = MathRoot::FindAllRoots(aSine, 0.0, 10.0, aConfig);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots(), 4u); // 0, pi, 2*pi, 3*pi

  EXPECT_NEAR(aResult.Roots[0], 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], THE_PI, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[2], 2.0 * THE_PI, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[3], 3.0 * THE_PI, THE_TOLERANCE);
}

TEST(MathRoot_Multiple, FindAllRoots_WithOffset)
{
  // f(x) = x^2, find roots of f(x) - 4 = 0, i.e., x^2 = 4
  // Roots at x = -2 and x = 2
  MathUtils::Polynomial aFunc({0.0, 0.0, 1.0});

  MathRoot::MultipleConfig aConfig;
  aConfig.Offset = 4.0;

  auto aResult = MathRoot::FindAllRoots(aFunc, -5.0, 5.0, aConfig);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots(), 2u);
  EXPECT_NEAR(aResult.Roots[0], -2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 2.0, THE_TOLERANCE);
}

TEST(MathRoot_Multiple, FindAllRoots_CloseRoots)
{
  // f(x) = (x - 1)(x - 1.01) = x^2 - 2.01x + 1.01
  // Two close roots at x = 1 and x = 1.01
  MathUtils::Polynomial aFunc({1.01, -2.01, 1.0});

  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples  = 500;
  aConfig.XTolerance = 1e-4;

  auto aResult = MathRoot::FindAllRoots(aFunc, 0.0, 2.0, aConfig);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots(), 2u);
  EXPECT_NEAR(aResult.Roots[0], 1.0, 1e-3);
  EXPECT_NEAR(aResult.Roots[1], 1.01, 1e-3);
}

TEST(MathRoot_Multiple, FindAllRoots_NullFunction)
{
  // f(x) = 0 (constant zero function)
  MathUtils::Constant aFunc(0.0);

  MathRoot::MultipleConfig aConfig;
  aConfig.NullTolerance = 1e-10;

  auto aResult = MathRoot::FindAllRoots(aFunc, -1.0, 1.0, aConfig);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_TRUE(aResult.IsAllNull);
}

TEST(MathRoot_Multiple, FindAllRoots_LambdaFunction)
{
  // f(x) = cos(x) - 0.5, roots where cos(x) = 0.5
  auto aFunc = MathUtils::MakeScalar([](double x, double& y) {
    y = std::cos(x) - 0.5;
    return true;
  });

  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples = 100;

  auto aResult = MathRoot::FindAllRoots(aFunc, 0.0, 2.0 * THE_PI, aConfig);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots(), 2u);
  // cos(x) = 0.5 at x = pi/3 and x = 5*pi/3
  EXPECT_NEAR(aResult.Roots[0], THE_PI / 3.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 5.0 * THE_PI / 3.0, THE_TOLERANCE);
}

//=================================================================================================
// FindAllRootsWithDerivative Tests
//=================================================================================================

TEST(MathRoot_Multiple, FindAllRootsWithDerivative_ThreeRoots)
{
  // f(x) = (x - 1)(x - 2)(x - 3)
  MathUtils::Polynomial aFunc({-6.0, 11.0, -6.0, 1.0});

  auto aResult = MathRoot::FindAllRootsWithDerivative(aFunc, 0.0, 4.0);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots(), 3u);
  EXPECT_NEAR(aResult.Roots[0], 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[2], 3.0, THE_TOLERANCE);
}

TEST(MathRoot_Multiple, FindAllRootsWithDerivative_DoubleRoot)
{
  // f(x) = (x - 2)^2 = x^2 - 4x + 4, double root at x = 2
  // This is a tangent root (touches zero without crossing)
  MathUtils::Polynomial aFunc({4.0, -4.0, 1.0});

  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples  = 100;
  aConfig.FTolerance = 1e-8;

  auto aResult = MathRoot::FindAllRootsWithDerivative(aFunc, 0.0, 4.0, aConfig);
  EXPECT_TRUE(aResult.IsDone());
  // Should find the double root (or detect it as near-zero extremum)
  EXPECT_GE(aResult.NbRoots(), 1);
  if (aResult.NbRoots() > 0)
  {
    EXPECT_NEAR(aResult.Roots[0], 2.0, 1e-4);
  }
}

TEST(MathRoot_Multiple, FindAllRootsWithDerivative_SineFunction)
{
  // f(x) = sin(x) with derivative cos(x)
  class SineWithDerivative
  {
  public:
    bool Value(double theX, double& theY) const
    {
      theY = std::sin(theX);
      return true;
    }

    bool Values(double theX, double& theY, double& theDY) const
    {
      theY  = std::sin(theX);
      theDY = std::cos(theX);
      return true;
    }
  };

  SineWithDerivative aSine;

  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples = 200;

  auto aResult = MathRoot::FindAllRootsWithDerivative(aSine, 0.0, 7.0, aConfig);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots(), 3u); // 0, pi, 2*pi

  EXPECT_NEAR(aResult.Roots[0], 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], THE_PI, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[2], 2.0 * THE_PI, THE_TOLERANCE);
}

//=================================================================================================
// Comparison with math_FunctionRoots
//=================================================================================================

TEST(MathRoot_Multiple, CompareWithReference_Polynomial)
{
  // Compare results with math_FunctionRoots behavior
  // f(x) = x^3 - 6x^2 + 11x - 6 = (x-1)(x-2)(x-3)
  MathUtils::Polynomial aFunc({-6.0, 11.0, -6.0, 1.0});

  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples  = 50;
  aConfig.XTolerance = 1e-10;
  aConfig.FTolerance = 1e-10;

  auto aResult = MathRoot::FindAllRoots(aFunc, 0.5, 3.5, aConfig);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots(), 3u);

  // Verify function value at roots is close to zero
  for (int i = 0; i < aResult.NbRoots(); ++i)
  {
    double aValue = 0.0;
    aFunc.Value(aResult.Roots[i], aValue);
    EXPECT_NEAR(aValue, 0.0, 1e-9);
  }
}

TEST(MathRoot_Multiple, CompareWithReference_HigherDegree)
{
  // f(x) = x^4 - 10x^2 + 9 = (x^2-1)(x^2-9) = (x-1)(x+1)(x-3)(x+3)
  // Roots at -3, -1, 1, 3
  MathUtils::Polynomial aFunc({9.0, 0.0, -10.0, 0.0, 1.0});

  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples = 100;

  auto aResult = MathRoot::FindAllRoots(aFunc, -5.0, 5.0, aConfig);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots(), 4u);

  EXPECT_NEAR(aResult.Roots[0], -3.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], -1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[2], 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[3], 3.0, THE_TOLERANCE);
}

TEST(MathRoot_Multiple, CompareWithReference_ClusteredTangentialRootsSparseSampling)
{
  struct ClusteredTangentialReference : public math_FunctionWithDerivative
  {
    bool Value(const double theX, double& theF) override
    {
      const double aD1 = theX - 1.003;
      const double aD2 = theX - 1.007;
      theF             = aD1 * aD1 * aD2 * aD2;
      return true;
    }

    bool Derivative(const double theX, double& theD) override
    {
      const double aD1 = theX - 1.003;
      const double aD2 = theX - 1.007;
      theD             = 2.0 * aD1 * aD2 * (aD1 + aD2);
      return true;
    }

    bool Values(const double theX, double& theF, double& theD) override
    {
      Value(theX, theF);
      Derivative(theX, theD);
      return true;
    }
  };

  struct ClusteredTangentialCallable
  {
    bool Value(const double theX, double& theF) const
    {
      const double aD1 = theX - 1.003;
      const double aD2 = theX - 1.007;
      theF             = aD1 * aD1 * aD2 * aD2;
      return true;
    }

    bool Values(const double theX, double& theF, double& theD) const
    {
      const double aD1 = theX - 1.003;
      const double aD2 = theX - 1.007;
      theF             = aD1 * aD1 * aD2 * aD2;
      theD             = 2.0 * aD1 * aD2 * (aD1 + aD2);
      return true;
    }
  };

  ClusteredTangentialReference aReferenceFunc;
  ClusteredTangentialCallable  aCallableFunc;

  math_FunctionRoots aReferenceRoots(aReferenceFunc, 0.99, 1.02, 4, 1.0e-8, 1.0e-12, 1.0e-12);
  ASSERT_TRUE(aReferenceRoots.IsDone());
  ASSERT_FALSE(aReferenceRoots.IsAllNull());
  ASSERT_EQ(aReferenceRoots.NbSolutions(), 2);

  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples     = 4;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-12;
  aConfig.NullTolerance = 1.0e-12;

  auto aResult = MathRoot::FindAllRootsWithDerivative(aCallableFunc, 0.99, 1.02, aConfig);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_FALSE(aResult.IsAllNull);
  ASSERT_EQ(aResult.NbRoots(), 2u);

  EXPECT_NEAR(aResult.Roots[0], aReferenceRoots.Value(1), 1.0e-8);
  EXPECT_NEAR(aResult.Roots[1], aReferenceRoots.Value(2), 1.0e-8);
}

TEST(MathRoot_Multiple, FindAllRoots_ConvenienceOverload)
{
  // Test the convenience overload with explicit sample count
  MathUtils::Polynomial aFunc({-4.0, 0.0, 1.0}); // x^2 - 4

  auto aResult = MathRoot::FindAllRoots(aFunc, -5.0, 5.0, 50);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots(), 2u);
  EXPECT_NEAR(aResult.Roots[0], -2.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.Roots[1], 2.0, THE_TOLERANCE);
}

TEST(MathRoot_Multiple, FindAllRoots_Exponential)
{
  // f(x) = exp(x) - 2, root at ln(2) ~= 0.693
  auto aFunc = MathUtils::MakeScalar([](double x, double& y) {
    y = std::exp(x) - 2.0;
    return true;
  });

  auto aResult = MathRoot::FindAllRoots(aFunc, -1.0, 2.0, 50);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots(), 1u);
  EXPECT_NEAR(aResult.Roots[0], std::log(2.0), THE_TOLERANCE);
}

//=================================================================================================
// Tests for negative extrema probe direction fix (BUG 1)
//=================================================================================================

TEST(MathRoot_Multiple, FindAllRootsWithDerivative_NegativeExtrema_BothProbeDirections)
{
  // f(x) = -(x-1)(x-3) = -x^2+4x-3, has a maximum at x=2 with value 1.
  // Shifted by offset=1: g(x) = f(x)-1 = -x^2+4x-4 = -(x-2)^2
  // Root at x=2 (double root at the extremum).
  // This exercises the negative extrema detection path where the second probe
  // must try the opposite direction (aMidX + aDx) to detect the extremum.
  auto aFunc = MathUtils::MakeScalarWithDerivative([](double theX, double& theY, double& theDY) {
    theY  = -(theX - 1.0) * (theX - 3.0);
    theDY = -2.0 * theX + 4.0;
    return true;
  });

  MathRoot::MultipleConfig aConfig;
  aConfig.Offset = 1.0;

  auto aResult = MathRoot::FindAllRootsWithDerivative(aFunc, 0.0, 4.0, aConfig);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbRoots(), 1);
  // The root at x=2 should be found
  bool isFound = false;
  for (int i = 0; i < aResult.NbRoots(); ++i)
  {
    if (std::abs(aResult.Roots[i] - 2.0) < THE_TOLERANCE)
    {
      isFound = true;
      break;
    }
  }
  EXPECT_TRUE(isFound) << "Root at x=2 (negative extremum touching zero) should be found";
}

TEST(MathRoot_Multiple, FindAllRootsWithDerivative_NegativeExtrema_AsymmetricRoots)
{
  // f(x) = -sin(x), on [0, 2*pi].
  // Shifted by offset=-0.5: g(x) = -sin(x)+0.5, roots where sin(x)=0.5
  // Roots at x = pi/6, 5*pi/6.
  // The negative extremum near x=pi/2 (where -sin is minimized) must probe
  // both directions to trigger rediscretization.
  auto aFunc = MathUtils::MakeScalarWithDerivative([](double theX, double& theY, double& theDY) {
    theY  = -std::sin(theX);
    theDY = -std::cos(theX);
    return true;
  });

  MathRoot::MultipleConfig aConfig;
  aConfig.Offset     = -0.5;
  aConfig.FTolerance = 1.0e-12;

  auto aResult = MathRoot::FindAllRootsWithDerivative(aFunc, 0.0, 2.0 * THE_PI, aConfig);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots(), 2);
  if (aResult.NbRoots() == 2)
  {
    EXPECT_NEAR(aResult.Roots[0], THE_PI / 6.0, THE_TOLERANCE);
    EXPECT_NEAR(aResult.Roots[1], 5.0 * THE_PI / 6.0, THE_TOLERANCE);
  }
}

//=================================================================================================
// Tests for exact zero sample bracketing fix (BUG 2)
//=================================================================================================

TEST(MathRoot_Multiple, FindAllRoots_ExactZeroAtSample)
{
  // f(x) = x * (x - 1), roots at x=0 and x=1.
  // With 20 samples on [0, 2], sample step = 0.1, so x=0 and x=1 are exact sample points.
  // The zero-at-sample bracket logic should correctly identify these roots.
  MathUtils::Polynomial aFunc({0.0, -1.0, 1.0});

  auto aResult = MathRoot::FindAllRoots(aFunc, 0.0, 2.0, 10);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbRoots(), 2);

  bool isZeroFound = false;
  bool isOneFound  = false;
  for (int i = 0; i < aResult.NbRoots(); ++i)
  {
    if (std::abs(aResult.Roots[i]) < THE_TOLERANCE)
    {
      isZeroFound = true;
    }
    if (std::abs(aResult.Roots[i] - 1.0) < THE_TOLERANCE)
    {
      isOneFound = true;
    }
  }
  EXPECT_TRUE(isZeroFound) << "Root at x=0 (exact zero at sample) should be found";
  EXPECT_TRUE(isOneFound) << "Root at x=1 (exact zero at sample) should be found";
}

TEST(MathRoot_Multiple, FindAllRoots_ExactZeroWithSignChange)
{
  // f(x) = x, single root at x=0.
  // With sufficient samples on [-1, 1], x=0 may be an exact sample point.
  // The fixed bracketing (left = x - dx/2, right = x + dx/2) should detect
  // the sign change and refine the root.
  auto aFunc = MathUtils::MakeScalar([](double theX, double& theY) {
    theY = theX;
    return true;
  });

  auto aResult = MathRoot::FindAllRoots(aFunc, -1.0, 1.0, 10);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_GE(aResult.NbRoots(), 1);

  bool isFound = false;
  for (int i = 0; i < aResult.NbRoots(); ++i)
  {
    if (std::abs(aResult.Roots[i]) < THE_TOLERANCE)
    {
      isFound = true;
      break;
    }
  }
  EXPECT_TRUE(isFound) << "Root at x=0 should be found when it is an exact zero at a sample point";
}
