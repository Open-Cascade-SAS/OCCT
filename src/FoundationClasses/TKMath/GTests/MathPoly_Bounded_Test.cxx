// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <MathPoly_Bounded.hxx>

#include <array>
#include <limits>

TEST(MathPoly_BoundedTest, DistinctRootsAndEndpoints)
{
  // x*(x-1)^2*(x+1)^2: one interior simple root and two repeated endpoints.
  const double                  aPolynomial[] = {0, 1, 0, -2, 0, 1};
  const MathPoly::BoundedResult aResult       = MathPoly::Bounded(aPolynomial, 5, -1, 1);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbRoots, 3u);
  EXPECT_DOUBLE_EQ(aResult.Roots[0], -1);
  EXPECT_DOUBLE_EQ(aResult.Roots[1], 0);
  EXPECT_DOUBLE_EQ(aResult.Roots[2], 1);
}

//=================================================================================================

TEST(MathPoly_BoundedTest, RepeatedComplexFactorsAndScale)
{
  // (x^2+1)^4*(x^2-1/4); no complex root needs to be found or deflated.
  const std::array<double, 11> aPolynomial = {-0.25, 0, 0, 0, 2.5, 0, 5, 0, 3.75, 0, 1};
  std::array<double, 11>       aScaled;
  for (const double aScale : {1.e-200, 1.0, 1.e200})
  {
    for (size_t i = 0; i < aPolynomial.size(); ++i)
    {
      aScaled[i] = aPolynomial[i] * aScale;
    }
    const MathPoly::BoundedResult aResult = MathPoly::Bounded(aScaled.data(), 10, -1, 1);
    ASSERT_TRUE(aResult.IsDone());
    ASSERT_EQ(aResult.NbRoots, 2u);
    EXPECT_NEAR(aResult.Roots[0], -0.5, 1.e-14);
    EXPECT_NEAR(aResult.Roots[1], 0.5, 1.e-14);
  }
}

//=================================================================================================

TEST(MathPoly_BoundedTest, StationaryAndCloselySpacedRoots)
{
  const double                  aRepeated[] = {0.0625, -0.5, 1};
  const MathPoly::BoundedResult aDouble     = MathPoly::Bounded(aRepeated, 2, -1, 1);
  ASSERT_TRUE(aDouble.IsDone());
  ASSERT_EQ(aDouble.NbRoots, 1u);
  EXPECT_DOUBLE_EQ(aDouble.Roots[0], 0.25);

  const double                  aClose[] = {-1.e-12, 0, 1};
  const MathPoly::BoundedResult aPair    = MathPoly::Bounded(aClose, 2, -1, 1);
  ASSERT_TRUE(aPair.IsDone());
  ASSERT_EQ(aPair.NbRoots, 2u);
  EXPECT_NEAR(aPair.Roots[0], -1.e-6, 1.e-20);
  EXPECT_NEAR(aPair.Roots[1], 1.e-6, 1.e-20);
}

//=================================================================================================

TEST(MathPoly_BoundedTest, ConstantAndInvalidInput)
{
  const double aZero[] = {0};
  EXPECT_EQ(MathPoly::Bounded(aZero, 0, -1, 1).Status, MathUtils::Status::InfiniteSolutions);
  const double                  aConstant[] = {3, 0, 0};
  const MathPoly::BoundedResult aResult     = MathPoly::Bounded(aConstant, 2, -1, 1);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 0u);
  EXPECT_EQ(MathPoly::Bounded(aConstant, 2, 1, -1).Status, MathUtils::Status::InvalidInput);
  EXPECT_EQ(MathPoly::Bounded(nullptr, 1, -1, 1).Status, MathUtils::Status::InvalidInput);
  const double aNaN[] = {std::numeric_limits<double>::quiet_NaN(), 1};
  EXPECT_EQ(MathPoly::Bounded(aNaN, 1, -1, 1).Status, MathUtils::Status::InvalidInput);
}

//=================================================================================================

TEST(MathPoly_BoundedTest, SingletonInterval)
{
  const double                  aPolynomial[] = {0, 1};
  const MathPoly::BoundedResult aResult       = MathPoly::Bounded(aPolynomial, 1, 0, 0);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbRoots, 1u);
  EXPECT_DOUBLE_EQ(aResult.Roots[0], 0);
}

//=================================================================================================

TEST(MathPoly_BoundedTest, StationaryResidualDistinguishesRealAndComplexPairs)
{
  for (const double aScale : {0x1p-500, 1.0, 0x1p500})
  {
    for (const double aSign : {-1.0, 0.0, 1.0})
    {
      const double aPolynomial[] = {(0.0625 + aSign * 0x1p-54) * aScale, -0.5 * aScale, aScale};
      const MathPoly::BoundedResult aResult = MathPoly::Bounded(aPolynomial, 2, -1, 1);
      ASSERT_TRUE(aResult.IsDone());
      if (aSign < 0)
      {
        ASSERT_EQ(aResult.NbRoots, 2u);
        EXPECT_DOUBLE_EQ(aResult.Roots[0], 0.25 - 0x1p-27);
        EXPECT_DOUBLE_EQ(aResult.Roots[1], 0.25 + 0x1p-27);
      }
      else if (aSign == 0)
      {
        ASSERT_EQ(aResult.NbRoots, 1u);
        EXPECT_DOUBLE_EQ(aResult.Roots[0], 0.25);
      }
      else
      {
        EXPECT_EQ(aResult.NbRoots, 0u);
      }
    }
  }
}

//=================================================================================================

TEST(MathPoly_BoundedTest, CompensatedHornerPreservesSmallNonzeroValues)
{
  const double aPolynomial[] = {0.0625, -0.5, 1};
  const double aX            = 0.25 + 0x1p-30;
  EXPECT_DOUBLE_EQ(MathUtils::EvalPolyCompensated(aPolynomial, 2, aX), 0x1p-60);
}

//=================================================================================================

TEST(MathPoly_BoundedTest, NormalizationPreservesSmallCoefficients)
{
  // Normalizing by the largest coefficient alone erases the constant term.
  // These polynomials then both become x^2 and incorrectly acquire a root at zero.
  for (const double aSign : {-1.0, 1.0})
  {
    const double                  aPolynomial[] = {aSign * 0x1p-900, 0.0, 0x1p900};
    const MathPoly::BoundedResult aResult       = MathPoly::Bounded(aPolynomial, 2, -1.0, 1.0);
    ASSERT_TRUE(aResult.IsDone());
    if (aSign > 0.0)
    {
      EXPECT_EQ(aResult.NbRoots, 0u);
    }
    else
    {
      ASSERT_EQ(aResult.NbRoots, 2u);
      EXPECT_DOUBLE_EQ(aResult.Roots[0], -0x1p-900);
      EXPECT_DOUBLE_EQ(aResult.Roots[1], 0x1p-900);
    }
  }
}

//=================================================================================================

TEST(MathPoly_BoundedTest, NormalizationPreservesSubnormalConstant)
{
  for (const double aLeading : {1.0, 0.25})
  {
    const double aPolynomial[] = {-std::numeric_limits<double>::denorm_min(), 0.0, aLeading};
    const MathPoly::BoundedResult aResult = MathPoly::Bounded(aPolynomial, 2, -1.0, 1.0);
    ASSERT_TRUE(aResult.IsDone());
    ASSERT_EQ(aResult.NbRoots, 2u);
    const double aRoot = aLeading == 1.0 ? 0x1p-537 : 0x1p-536;
    EXPECT_DOUBLE_EQ(aResult.Roots[0], -aRoot);
    EXPECT_DOUBLE_EQ(aResult.Roots[1], aRoot);
  }
}

//=================================================================================================

TEST(MathPoly_BoundedTest, UnrepresentableDerivativeReportsNumericalError)
{
  // Bringing the constant into the normal range overflows the leading
  // coefficient. Do not publish a polynomial with an erased constant.
  const double                  aPolynomial[] = {std::numeric_limits<double>::denorm_min(),
                                                 0.0,
                                                 std::numeric_limits<double>::max()};
  const MathPoly::BoundedResult aResult       = MathPoly::Bounded(aPolynomial, 2, -1.0, 1.0);
  EXPECT_EQ(aResult.Status, MathUtils::Status::NumericalError);
  EXPECT_EQ(aResult.NbRoots, 0u);
}

//=================================================================================================

TEST(MathPoly_BoundedTest, MaximumSupportedDegree)
{
  // x^20 - 1 has two real roots on [-1, 1] and exercises the fixed-capacity limit.
  std::array<double, MathPoly::THE_MAX_POLY_DEGREE + 1> aPolynomial = {};
  aPolynomial[0]                                                    = -1.0;
  aPolynomial[MathPoly::THE_MAX_POLY_DEGREE]                        = 1.0;
  const MathPoly::BoundedResult aResult =
    MathPoly::Bounded(aPolynomial.data(), MathPoly::THE_MAX_POLY_DEGREE, -1.0, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbRoots, 2u);
  EXPECT_DOUBLE_EQ(aResult.Roots[0], -1.0);
  EXPECT_DOUBLE_EQ(aResult.Roots[1], 1.0);
}
