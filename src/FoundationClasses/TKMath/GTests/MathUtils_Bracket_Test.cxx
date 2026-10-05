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

#include <MathUtils_Bracket.hxx>
#include <MathUtils_Config.hxx>
#include <MathUtils_Convergence.hxx>
#include <MathUtils_Deriv.hxx>
#include <MathUtils_Domain.hxx>
#include <MathUtils_FunctorVector.hxx>
#include <MathUtils_LineSearch.hxx>
#include <MathUtils_Poly.hxx>
#include <Standard_OutOfMemory.hxx>

#include <cmath>
#include <limits>

namespace
{
class QuadraticMinimum
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = (theX - 2.0) * (theX - 2.0);
    return true;
  }
};

class QuadraticWithPrecomputedEndpoints
{
public:
  bool Value(double theX, double& theF) const
  {
    if (std::abs(theX - 0.0) < 1.0e-15 || std::abs(theX - 1.0) < 1.0e-15)
    {
      return false;
    }
    theF = (theX - 2.0) * (theX - 2.0);
    return true;
  }
};

class FailingScalar
{
public:
  bool Value(double, double&) const { return false; }
};

class NonFiniteScalar
{
public:
  bool Value(double, double& theF) const
  {
    theF = std::numeric_limits<double>::quiet_NaN();
    return true;
  }
};

class LinearVectorFunction
{
public:
  bool Value(const math_Vector& theX, double& theF) const
  {
    theF = theX.At(0);
    return true;
  }
};

class DecreasingLinearFunction
{
public:
  bool Value(const math_Vector& theX, double& theF) const
  {
    theF = -theX.At(0);
    return true;
  }

  bool Gradient(const math_Vector&, math_Vector& theGradient) const
  {
    theGradient.ChangeAt(0) = -1.0;
    return true;
  }
};

class DistantDirectionalMinimum
{
public:
  bool Value(const math_Vector& theX, double& theF) const
  {
    theF = (theX.At(0) - 25.0) * (theX.At(0) - 25.0);
    return true;
  }
};

class FailingDuringLineBracketing
{
public:
  bool Value(const math_Vector& theX, double& theF) const
  {
    if (theX.At(0) > 3.0)
    {
      return false;
    }
    theF = (theX.At(0) - 25.0) * (theX.At(0) - 25.0);
    return true;
  }
};
} // namespace

TEST(MathUtils_BracketTest, BracketMinimum_WithLimits_Succeeds)
{
  QuadraticMinimum aFunc;

  MathUtils::MinBracketOptions anOptions;
  anOptions.MaxIterations = 50;
  anOptions.UseLimits     = true;
  anOptions.LeftLimit     = 0.0;
  anOptions.RightLimit    = 5.0;

  const MathUtils::MinBracketResult aResult = MathUtils::BracketMinimum(aFunc, 0.0, 1.0, anOptions);
  ASSERT_TRUE(aResult.IsValid);
  EXPECT_GE(aResult.A, anOptions.LeftLimit);
  EXPECT_LE(aResult.C, anOptions.RightLimit);
  EXPECT_LT(aResult.Fb, aResult.Fa);
  EXPECT_LT(aResult.Fb, aResult.Fc);
}

TEST(MathUtils_BracketTest, BracketMinimum_WithRestrictiveLimits_Fails)
{
  QuadraticMinimum aFunc;

  MathUtils::MinBracketOptions anOptions;
  anOptions.MaxIterations = 50;
  anOptions.UseLimits     = true;
  anOptions.LeftLimit     = 0.0;
  anOptions.RightLimit    = 1.0;

  const MathUtils::MinBracketResult aResult = MathUtils::BracketMinimum(aFunc, 0.0, 0.5, anOptions);
  EXPECT_FALSE(aResult.IsValid);
}

TEST(MathUtils_BracketTest, BracketMinimum_UsesPrecomputedEndpointValues)
{
  QuadraticWithPrecomputedEndpoints aFunc;

  MathUtils::MinBracketOptions anOptions;
  anOptions.MaxIterations = 50;
  anOptions.HasFA         = true;
  anOptions.HasFB         = true;
  anOptions.FA            = 4.0;
  anOptions.FB            = 1.0;

  const MathUtils::MinBracketResult aResult = MathUtils::BracketMinimum(aFunc, 0.0, 1.0, anOptions);
  ASSERT_TRUE(aResult.IsValid);
  EXPECT_LT(aResult.Fb, aResult.Fa);
  EXPECT_LT(aResult.Fb, aResult.Fc);
}

TEST(MathUtils_CoreTest, NormAndDotProduct_HandleScaleExtremes)
{
  math_Vector aLarge(1, 2);
  aLarge(1) = 3.0e300;
  aLarge(2) = 4.0e300;
  EXPECT_DOUBLE_EQ(MathUtils::Norm(aLarge), 5.0e300);

  math_Vector aSmall(1, 2);
  aSmall(1) = 3.0e-300;
  aSmall(2) = 4.0e-300;
  EXPECT_DOUBLE_EQ(MathUtils::Norm(aSmall), 5.0e-300);

  math_Vector aA(1, 3);
  math_Vector aB(-2, 0, 1.0);
  aA(1) = 1.0e16;
  aA(2) = 1.0;
  aA(3) = -1.0e16;
  EXPECT_DOUBLE_EQ(MathUtils::DotProduct(aA, aB), 1.0);
}

TEST(MathUtils_CoreTest, RejectsNonFiniteValuesAndInvalidConfiguration)
{
  EXPECT_FALSE(MathUtils::IsEqual(std::numeric_limits<double>::infinity(),
                                  std::numeric_limits<double>::infinity()));
  EXPECT_FALSE(
    MathUtils::IsWithinTolerance(std::numeric_limits<double>::quiet_NaN(), 0.0, 1.0, 1.0));
  EXPECT_FALSE(MathUtils::IsWithinTolerance(-std::numeric_limits<double>::max(),
                                            std::numeric_limits<double>::max(),
                                            0.0,
                                            2.0));

  MathUtils::Config aConfig;
  aConfig.RelativeTolerance = std::numeric_limits<double>::quiet_NaN();
  EXPECT_FALSE(aConfig.IsValid());
  EXPECT_FALSE(MathUtils::IsConverged(1.0, 1.0, 0.0, aConfig));

  const MathUtils::Domain1D aReversed(2.0, -2.0);
  EXPECT_FALSE(aReversed.IsValid());
  EXPECT_FALSE(aReversed.Contains(0.0));
}

TEST(MathUtils_ConvergenceTest, CombinesAbsoluteAndRelativeTolerance)
{
  MathUtils::Config aConfig;
  aConfig.XTolerance        = 1.0e-12;
  aConfig.RelativeTolerance = 1.0e-8;
  EXPECT_TRUE(MathUtils::IsConverged(1.0e12, 1.0e12 + 1.0, 1.0, aConfig));
  EXPECT_FALSE(MathUtils::IsConverged(0.0, 1.0e-6, 1.0, aConfig));
}

TEST(MathUtils_DerivTest, AdaptiveGradientUsesRepresentablePerVariableStep)
{
  LinearVectorFunction aFunc;
  math_Vector          aX(5, 5);
  math_Vector          aGradient(-2, -2);
  aX(5) = 1.0e300;

  ASSERT_TRUE(MathUtils::NumericalGradientAdaptive(aFunc, aX, aGradient, 1.0e-8));
  EXPECT_NEAR(aGradient(-2), 1.0, 1.0e-8);
  EXPECT_DOUBLE_EQ(aX(5), 1.0e300);
  EXPECT_FALSE(MathUtils::NumericalGradientAdaptive(aFunc, aX, aGradient, 0.0));
}

TEST(MathUtils_DerivTest, CallbackFailureRestoresInput)
{
  auto        aFunc = MathUtils::MakeVector([](const math_Vector&, double&) { return false; });
  math_Vector aX(3, 3);
  math_Vector aGradient(3, 3);
  aX(3) = 7.0;
  EXPECT_FALSE(MathUtils::NumericalGradientAdaptive(aFunc, aX, aGradient));
  EXPECT_DOUBLE_EQ(aX(3), 7.0);
}

TEST(MathUtils_BracketTest, ReportsInvalidAndCallbackFailures)
{
  QuadraticMinimum aQuadratic;
  EXPECT_EQ(
    MathUtils::BracketRoot(aQuadratic, std::numeric_limits<double>::quiet_NaN(), 1.0).Status,
    MathUtils::Status::InvalidInput);

  FailingScalar                  aFailing;
  const MathUtils::BracketResult aFailure = MathUtils::BracketRoot(aFailing, -1.0, 1.0);
  EXPECT_FALSE(aFailure.IsValid);
  EXPECT_EQ(aFailure.Status, MathUtils::Status::CallbackError);

  NonFiniteScalar aNonFinite;
  EXPECT_EQ(MathUtils::BracketRoot(aNonFinite, -1.0, 1.0).Status,
            MathUtils::Status::NumericalError);
}

TEST(MathUtils_BracketTest, SupportsArbitraryFiniteLimits)
{
  QuadraticMinimum             aFunc;
  MathUtils::MinBracketOptions anOptions;
  anOptions.UseLimits  = true;
  anOptions.LeftLimit  = -7.0;
  anOptions.RightLimit = 11.0;
  const MathUtils::MinBracketResult aResult =
    MathUtils::BracketMinimum(aFunc, 9.0, -3.0, anOptions);
  ASSERT_TRUE(aResult.IsValid);
  EXPECT_LT(aResult.A, aResult.B);
  EXPECT_LT(aResult.B, aResult.C);
  EXPECT_GE(aResult.A, anOptions.LeftLimit);
  EXPECT_LE(aResult.C, anOptions.RightLimit);
}

TEST(MathUtils_LineSearchTest, WolfeExhaustionNeverClaimsSuccess)
{
  DecreasingLinearFunction          aFunc;
  math_Vector                       aX(4, 4, 0.0);
  math_Vector                       aDirection(-1, -1, 1.0);
  math_Vector                       aGradient(8, 8, -1.0);
  const MathUtils::LineSearchResult aResult =
    MathUtils::WolfeSearch(aFunc, aX, aDirection, aGradient, 0.0, 1.0, 1.0e-4, 0.9, 2);
  EXPECT_FALSE(aResult.IsValid);
  EXPECT_EQ(aResult.Status, MathUtils::Status::MaxIterations);
  EXPECT_DOUBLE_EQ(aResult.FNew, -aResult.Alpha);
}

TEST(MathUtils_LineSearchTest, RejectsInvalidConfigAndPropagatesCallbackFailure)
{
  math_Vector              aX(1, 1, 0.0);
  math_Vector              aDirection(1, 1, 1.0);
  math_Vector              aGradient(1, 1, -1.0);
  DecreasingLinearFunction aFunc;
  EXPECT_EQ(MathUtils::WolfeSearch(aFunc, aX, aDirection, aGradient, 0.0, -1.0).Status,
            MathUtils::Status::InvalidInput);

  auto aFailing =
    MathUtils::MakeVectorWithGradient([](const math_Vector&, double&) { return false; },
                                      [](const math_Vector&, math_Vector&) { return true; });
  EXPECT_EQ(MathUtils::WolfeSearch(aFailing, aX, aDirection, aGradient, 0.0).Status,
            MathUtils::Status::CallbackError);
}

TEST(MathUtils_LineSearchTest, ExactSearchFindsMinimumBeyondAlphaTen)
{
  DistantDirectionalMinimum aFunc;
  math_Vector               aX(size_t{1}, 0.0);
  math_Vector               aDirection(size_t{1}, 1.0);

  const MathUtils::LineSearchResult aResult =
    MathUtils::ExactLineSearch(aFunc, aX, aDirection, 100.0, 1.0e-10);

  ASSERT_TRUE(aResult.IsValid);
  EXPECT_EQ(aResult.Status, MathUtils::Status::OK);
  EXPECT_NEAR(aResult.Alpha, 25.0, 1.0e-6);
  EXPECT_NEAR(aResult.FNew, 0.0, 1.0e-12);
}

TEST(MathUtils_LineSearchTest, ExactSearchPropagatesCallbackFailureDuringBracketing)
{
  FailingDuringLineBracketing aFunc;
  math_Vector                 aX(size_t{1}, 0.0);
  math_Vector                 aDirection(size_t{1}, 1.0);

  const MathUtils::LineSearchResult aResult =
    MathUtils::ExactLineSearch(aFunc, aX, aDirection, 10.0, 1.0e-10);

  EXPECT_FALSE(aResult.IsValid);
  EXPECT_EQ(aResult.Status, MathUtils::Status::CallbackError);
  EXPECT_GE(aResult.Alpha, 0.0);
  EXPECT_LE(aResult.Alpha, 3.0);
  EXPECT_DOUBLE_EQ(aResult.FNew, (aResult.Alpha - 25.0) * (aResult.Alpha - 25.0));
}

TEST(MathUtils_LineSearchTest, FourArgumentCallRetainsAlphaLimitMeaning)
{
  DistantDirectionalMinimum aFunc;
  math_Vector               aX(size_t{1}, 0.0);
  math_Vector               aDirection(size_t{1}, 1.0);

  const MathUtils::LineSearchResult aResult =
    MathUtils::ExactLineSearch(aFunc, aX, aDirection, 10.0);

  ASSERT_TRUE(aResult.IsValid);
  EXPECT_LE(std::abs(aResult.Alpha), 10.0);
  EXPECT_GT(aResult.FNew, 0.0);
}

TEST(MathUtils_PolyTest, DuplicatePolicyIsScaleRelativeAndRejectsNonFiniteRoots)
{
  double aLargeRoots[] = {1.0e12, 1.0e12 + 1.0};
  EXPECT_EQ(MathUtils::RemoveDuplicateRoots(aLargeRoots, 2, 1.0e-10), 1u);

  double aSmallRoots[] = {0.0, 1.0e-9};
  EXPECT_EQ(MathUtils::RemoveDuplicateRoots(aSmallRoots, 2, 1.0e-10), 2u);

  double aBadRoots[] = {0.0, std::numeric_limits<double>::infinity()};
  EXPECT_EQ(MathUtils::RemoveDuplicateRoots(aBadRoots, 2), 0u);

  double aBadFirstRoot[] = {std::numeric_limits<double>::quiet_NaN(), 1.0};
  EXPECT_EQ(MathUtils::RemoveDuplicateRoots(aBadFirstRoot, 2), 0u);
}
