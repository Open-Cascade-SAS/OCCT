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

#include <MathPoly_Laguerre.hxx>

#include <cmath>
#include <limits>

//! Test fixture for MathPoly Laguerre solver tests.
class MathPoly_LaguerreTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-10;

  //! Helper to verify a root.
  static double EvaluatePoly(const double* theCoeffs, int theDegree, double theX)
  {
    double aResult = theCoeffs[theDegree];
    for (int i = theDegree - 1; i >= 0; --i)
    {
      aResult = aResult * theX + theCoeffs[i];
    }
    return aResult;
  }

  static std::complex<double> EvaluatePoly(const double*               theCoeffs,
                                           int                         theDegree,
                                           const std::complex<double>& theX)
  {
    std::complex<double> aResult = theCoeffs[theDegree];
    for (int anIndex = theDegree - 1; anIndex >= 0; --anIndex)
    {
      aResult = aResult * theX + theCoeffs[anIndex];
    }
    return aResult;
  }

  static double ComplexEvaluationRoundoff(const double*               theCoeffs,
                                          int                         theDegree,
                                          const std::complex<double>& theX)
  {
    double       aMagnitude = std::abs(theCoeffs[theDegree]);
    const double anAbsX     = std::abs(theX);
    for (int anIndex = theDegree - 1; anIndex >= 0; --anIndex)
    {
      aMagnitude = aMagnitude * anAbsX + std::abs(theCoeffs[anIndex]);
    }
    const double anOperationRoundoff = theDegree * std::numeric_limits<double>::epsilon();
    return anOperationRoundoff / (1.0 - anOperationRoundoff) * aMagnitude;
  }
};

// Test quintic with known roots: (x-1)(x-2)(x-3)(x-4)(x-5) = x^5 - 15x^4 + 85x^3 - 225x^2 + 274x -
// 120
TEST_F(MathPoly_LaguerreTest, Quintic_FiveDistinctRoots)
{
  auto aResult = MathPoly::Quintic(1.0, -15.0, 85.0, -225.0, 274.0, -120.0);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 5u);

  for (size_t i = 0; i < aResult.NbRoots; ++i)
  {
    double aVal = aResult.Roots[i];
    EXPECT_TRUE(std::abs(aVal - 1.0) < THE_TOL || std::abs(aVal - 2.0) < THE_TOL
                || std::abs(aVal - 3.0) < THE_TOL || std::abs(aVal - 4.0) < THE_TOL
                || std::abs(aVal - 5.0) < THE_TOL)
      << "Root " << aVal << " not expected";
  }
}

// Test sextic with known roots: (x-1)(x-2)(x-3)(x-4)(x-5)(x-6)
TEST_F(MathPoly_LaguerreTest, Sextic_SixDistinctRoots)
{
  // (x-1)(x-2)(x-3)(x-4)(x-5)(x-6) = x^6 - 21x^5 + 175x^4 - 735x^3 + 1624x^2 - 1764x + 720
  auto aResult = MathPoly::Sextic(1.0, -21.0, 175.0, -735.0, 1624.0, -1764.0, 720.0);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 6u);

  for (size_t i = 0; i < aResult.NbRoots; ++i)
  {
    double aVal = aResult.Roots[i];
    EXPECT_TRUE(std::abs(aVal - 1.0) < THE_TOL || std::abs(aVal - 2.0) < THE_TOL
                || std::abs(aVal - 3.0) < THE_TOL || std::abs(aVal - 4.0) < THE_TOL
                || std::abs(aVal - 5.0) < THE_TOL || std::abs(aVal - 6.0) < THE_TOL)
      << "Root " << aVal << " not expected";
  }
}

// Test general Laguerre with all 6 roots
TEST_F(MathPoly_LaguerreTest, Laguerre_SexticAllRoots)
{
  // (x-1)(x-2)(x-3)(x-4)(x-5)(x-6)
  double aCoeffs[7] = {720.0, -1764.0, 1624.0, -735.0, 175.0, -21.0, 1.0};

  auto aResult = MathPoly::Laguerre(aCoeffs, 6);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 6u);

  for (size_t i = 0; i < aResult.NbRoots; ++i)
  {
    double aExpected = static_cast<double>(i + 1);
    EXPECT_NEAR(aResult.Roots[i], aExpected, THE_TOL);
  }
}

// Test quintic with all real roots: (x+2)(x+1)(x)(x-1)(x-2) = x^5 - 5x^3 + 4x
TEST_F(MathPoly_LaguerreTest, Quintic_SymmetricRoots)
{
  double aCoeffs[6] = {0.0, 4.0, 0.0, -5.0, 0.0, 1.0};

  auto aResult = MathPoly::Laguerre(aCoeffs, 5);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 5u);

  // Should find roots: -2, -1, 0, 1, 2
  EXPECT_NEAR(aResult.Roots[0], -2.0, THE_TOL);
  EXPECT_NEAR(aResult.Roots[1], -1.0, THE_TOL);
  EXPECT_NEAR(aResult.Roots[2], 0.0, THE_TOL);
  EXPECT_NEAR(aResult.Roots[3], 1.0, THE_TOL);
  EXPECT_NEAR(aResult.Roots[4], 2.0, THE_TOL);
}

// Test sextic: (x+3)(x+2)(x+1)(x-1)(x-2)(x-3) = x^6 - 14x^4 + 49x^2 - 36
TEST_F(MathPoly_LaguerreTest, Sextic_SymmetricRoots)
{
  double aCoeffs[7] = {-36.0, 0.0, 49.0, 0.0, -14.0, 0.0, 1.0};

  auto aResult = MathPoly::Laguerre(aCoeffs, 6);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 6u);

  EXPECT_NEAR(aResult.Roots[0], -3.0, THE_TOL);
  EXPECT_NEAR(aResult.Roots[1], -2.0, THE_TOL);
  EXPECT_NEAR(aResult.Roots[2], -1.0, THE_TOL);
  EXPECT_NEAR(aResult.Roots[3], 1.0, THE_TOL);
  EXPECT_NEAR(aResult.Roots[4], 2.0, THE_TOL);
  EXPECT_NEAR(aResult.Roots[5], 3.0, THE_TOL);
}

// Test octic with 8 real roots: (x-1)(x-2)(x-3)(x-4)(x-5)(x-6)(x-7)(x-8)
TEST_F(MathPoly_LaguerreTest, Octic_EightDistinctRoots)
{
  // (x-1)(x-2)(x-3)(x-4)(x-5)(x-6)(x-7)(x-8) = x^8 - 36x^7 + 546x^6 - 4536x^5 + 22449x^4 - 67284x^3
  // + 118124x^2 - 109584x + 40320
  double aCoeffs[9] = {40320.0, -109584.0, 118124.0, -67284.0, 22449.0, -4536.0, 546.0, -36.0, 1.0};

  auto aResult = MathPoly::Octic(aCoeffs);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 8u); // Eight real roots: 1, 2, 3, 4, 5, 6, 7, 8

  for (size_t i = 0; i < aResult.NbRoots; ++i)
  {
    double aExpected = static_cast<double>(i + 1);
    EXPECT_NEAR(aResult.Roots[i], aExpected, 1.0e-6) << "Root " << i << " differs";
  }
}

// Test with simple distinct roots (not multiple)
TEST_F(MathPoly_LaguerreTest, Quintic_SimpleRoots)
{
  // (x-1)(x-2)(x-3)(x-4)(x-5) = x^5 - 15x^4 + 85x^3 - 225x^2 + 274x - 120
  double aCoeffs[6] = {-120.0, 274.0, -225.0, 85.0, -15.0, 1.0};

  auto aResult = MathPoly::Laguerre(aCoeffs, 5);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 5u);

  // Check roots are 1, 2, 3, 4, 5
  for (size_t i = 0; i < aResult.NbRoots; ++i)
  {
    double aExpected = static_cast<double>(i + 1);
    EXPECT_NEAR(aResult.Roots[i], aExpected, THE_TOL);
  }
}

// Test quartic via Laguerre (degree reduction)
TEST_F(MathPoly_LaguerreTest, Laguerre_Quartic)
{
  // (x-1)(x-2)(x-3)(x-4) = x^4 - 10x^3 + 35x^2 - 50x + 24
  double aCoeffs[5] = {24.0, -50.0, 35.0, -10.0, 1.0};

  auto aResult = MathPoly::Laguerre(aCoeffs, 4);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 4u);

  EXPECT_NEAR(aResult.Roots[0], 1.0, THE_TOL);
  EXPECT_NEAR(aResult.Roots[1], 2.0, THE_TOL);
  EXPECT_NEAR(aResult.Roots[2], 3.0, THE_TOL);
  EXPECT_NEAR(aResult.Roots[3], 4.0, THE_TOL);
}

// Test accuracy: verify roots satisfy the polynomial
TEST_F(MathPoly_LaguerreTest, Laguerre_VerifyRoots)
{
  // Well-behaved polynomial with known real roots: (x-1)(x-2)(x-3)(x-4)(x-5)(x-6)
  double aCoeffs[7] = {720.0, -1764.0, 1624.0, -735.0, 175.0, -21.0, 1.0};

  auto aResult = MathPoly::Laguerre(aCoeffs, 6);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 6u);

  // Verify each real root
  for (size_t i = 0; i < aResult.NbRoots; ++i)
  {
    double aVal = EvaluatePoly(aCoeffs, 6, aResult.Roots[i]);
    EXPECT_NEAR(aVal, 0.0, 1.0e-6) << "Root " << aResult.Roots[i] << " gives P(x) = " << aVal;
  }
}

// Test high-degree polynomial (degree 10)
TEST_F(MathPoly_LaguerreTest, Laguerre_Degree10)
{
  // (x-1)(x-2)...(x-10) - only test that it computes without crashing
  // Coefficients from expansion of product
  double aCoeffs[11] = {3628800.0,   // 10!
                        -10628640.0, // sum of products
                        12753576.0,
                        -8409500.0,
                        3416930.0,
                        -902055.0,
                        157773.0,
                        -18150.0,
                        1320.0,
                        -55.0,
                        1.0};

  auto aResult = MathPoly::Laguerre(aCoeffs, 10);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 10u);

  // Verify roots are 1, 2, ..., 10
  for (size_t i = 0; i < aResult.NbRoots; ++i)
  {
    double aExpected = static_cast<double>(i + 1);
    EXPECT_NEAR(aResult.Roots[i], aExpected, 1.0e-6);
  }
}

// Performance comparison (informational)
TEST_F(MathPoly_LaguerreTest, Performance_CompareToQuartic)
{
  // Compare Laguerre on degree 4 vs dedicated Quartic solver
  double aCoeffs[5] = {24.0, -50.0, 35.0, -10.0, 1.0}; // (x-1)(x-2)(x-3)(x-4)

  // Laguerre
  auto aLagResult = MathPoly::Laguerre(aCoeffs, 4);
  ASSERT_TRUE(aLagResult.IsDone());

  // Quartic
  auto aQuartResult = MathPoly::Quartic(1.0, -10.0, 35.0, -50.0, 24.0);
  ASSERT_TRUE(aQuartResult.IsDone());

  EXPECT_EQ(aLagResult.NbRoots, aQuartResult.NbRoots);

  for (size_t i = 0; i < aQuartResult.NbRoots; ++i)
  {
    EXPECT_NEAR(aLagResult.Roots[i], aQuartResult.Roots[i], THE_TOL);
  }
}

TEST_F(MathPoly_LaguerreTest, RepeatedRootsReportMultiplicity)
{
  // (x-1)^3 * (x+2)^2
  const double         aCoeffs[6] = {-4.0, 8.0, -1.0, -5.0, 1.0, 1.0};
  MathPoly::PolyResult aResult    = MathPoly::Laguerre(aCoeffs, 5, 1.0e-11);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbRoots, 2u);
  EXPECT_NEAR(aResult.Roots[0], -2.0, 1.0e-6);
  EXPECT_NEAR(aResult.Roots[1], 1.0, 1.0e-6);
  EXPECT_EQ(aResult.Multiplicities[0], 2);
  EXPECT_EQ(aResult.Multiplicities[1], 3);
}

TEST_F(MathPoly_LaguerreTest, SmallPureImaginaryRootsRemainComplex)
{
  constexpr double     THE_CONSTANT = 1.0e-14;
  const double         aCoeffs[3]   = {THE_CONSTANT, 0.0, 1.0};
  MathPoly::PolyResult aResult      = MathPoly::Laguerre(aCoeffs, 2);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 0u);
  ASSERT_EQ(aResult.NbComplexRoots, 2u);
  EXPECT_EQ(aResult.ComplexMultiplicities[0], 1u);
  EXPECT_EQ(aResult.ComplexMultiplicities[1], 1u);
  EXPECT_EQ(aResult.ComplexRoots[1], std::conj(aResult.ComplexRoots[0]));
  for (size_t anIndex = 0; anIndex < aResult.NbComplexRoots; ++anIndex)
  {
    EXPECT_NEAR(aResult.ComplexRoots[anIndex].real(), 0.0, THE_TOL);
    EXPECT_NEAR(std::abs(aResult.ComplexRoots[anIndex].imag()),
                std::sqrt(THE_CONSTANT),
                THE_TOL);
    EXPECT_LE(std::abs(EvaluatePoly(aCoeffs, 2, aResult.ComplexRoots[anIndex])),
              ComplexEvaluationRoundoff(aCoeffs, 2, aResult.ComplexRoots[anIndex]));
  }
}

TEST_F(MathPoly_LaguerreTest, UnitPureImaginaryRootsRemainComplex)
{
  const double         aCoeffs[3] = {1.0, 0.0, 1.0};
  MathPoly::PolyResult aResult    = MathPoly::Laguerre(aCoeffs, 2);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 0u);
  ASSERT_EQ(aResult.NbComplexRoots, 2u);
  EXPECT_EQ(aResult.ComplexMultiplicities[0], 1u);
  EXPECT_EQ(aResult.ComplexMultiplicities[1], 1u);
  EXPECT_EQ(aResult.ComplexRoots[1], std::conj(aResult.ComplexRoots[0]));
  for (size_t anIndex = 0; anIndex < aResult.NbComplexRoots; ++anIndex)
  {
    EXPECT_NEAR(aResult.ComplexRoots[anIndex].real(), 0.0, THE_TOL);
    EXPECT_NEAR(std::abs(aResult.ComplexRoots[anIndex].imag()), 1.0, THE_TOL);
    EXPECT_LE(std::abs(EvaluatePoly(aCoeffs, 2, aResult.ComplexRoots[anIndex])),
              ComplexEvaluationRoundoff(aCoeffs, 2, aResult.ComplexRoots[anIndex]));
  }
}

TEST_F(MathPoly_LaguerreTest, RepeatedComplexConjugatesReportMultiplicity)
{
  const double         aCoeffs[5] = {1.0, 0.0, 2.0, 0.0, 1.0};
  MathPoly::PolyResult aResult    = MathPoly::Laguerre(aCoeffs, 4);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbRoots, 0u);
  ASSERT_EQ(aResult.NbComplexRoots, 2u);
  EXPECT_EQ(aResult.ComplexMultiplicities[0], 2u);
  EXPECT_EQ(aResult.ComplexMultiplicities[1], 2u);
  EXPECT_EQ(aResult.ComplexRoots[1], std::conj(aResult.ComplexRoots[0]));
  for (size_t anIndex = 0; anIndex < aResult.NbComplexRoots; ++anIndex)
  {
    EXPECT_NEAR(aResult.ComplexRoots[anIndex].real(), 0.0, THE_TOL);
    EXPECT_NEAR(std::abs(aResult.ComplexRoots[anIndex].imag()), 1.0, 1.0e-8);
    EXPECT_LE(std::abs(EvaluatePoly(aCoeffs, 4, aResult.ComplexRoots[anIndex])),
              ComplexEvaluationRoundoff(aCoeffs, 4, aResult.ComplexRoots[anIndex]));
  }
}

TEST_F(MathPoly_LaguerreTest, RepeatedRealAndComplexRootsUseMatchingDeflation)
{
  // (x-1)^2 * (x^2+1)
  const double               aCoefficients[5] = {1.0, -2.0, 2.0, -2.0, 1.0};
  const MathPoly::PolyResult aResult          = MathPoly::Laguerre(aCoefficients, 4);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbRoots, 1u);
  EXPECT_NEAR(aResult.Roots[0], 1.0, 1.0e-8);
  EXPECT_EQ(aResult.Multiplicities[0], 2u);
  ASSERT_EQ(aResult.NbComplexRoots, 2u);
  EXPECT_NEAR(aResult.ComplexRoots[0].real(), 0.0, 1.0e-8);
  EXPECT_NEAR(std::abs(aResult.ComplexRoots[0].imag()), 1.0, 1.0e-8);
  EXPECT_EQ(aResult.ComplexMultiplicities[0], 1u);
  EXPECT_EQ(aResult.ComplexMultiplicities[1], 1u);
}

TEST_F(MathPoly_LaguerreTest, GloballyScaledEquation)
{
  const double aCoeffs[6]      = {-1.2e-298, 2.74e-298, -2.25e-298, 8.5e-299, -1.5e-299, 1.0e-300};
  MathPoly::PolyResult aResult = MathPoly::Laguerre(aCoeffs, 5);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_EQ(aResult.NbRoots, 5u);
  for (size_t anIndex = 0; anIndex < aResult.NbRoots; ++anIndex)
  {
    EXPECT_NEAR(aResult.Roots[anIndex], static_cast<double>(anIndex + 1), 1.0e-8);
  }
}

TEST_F(MathPoly_LaguerreTest, RejectsNonFiniteInputAndTolerance)
{
  const double aNan       = std::numeric_limits<double>::quiet_NaN();
  const double aCoeffs[3] = {1.0, aNan, 1.0};
  EXPECT_EQ(MathPoly::Laguerre(aCoeffs, 2).Status, MathUtils::Status::InvalidInput);
  const double aFiniteCoeffs[3] = {-1.0, 0.0, 1.0};
  EXPECT_EQ(MathPoly::Laguerre(aFiniteCoeffs, 2, aNan).Status, MathUtils::Status::InvalidInput);
  EXPECT_EQ(MathPoly::Laguerre(aFiniteCoeffs, 2, 0.0).Status, MathUtils::Status::InvalidInput);
}

TEST_F(MathPoly_LaguerreTest, ForcedNonConvergenceIsReported)
{
  const double         aCoeffs[6] = {-120.0, 274.0, -225.0, 85.0, -15.0, 1.0};
  MathPoly::PolyResult aResult    = MathPoly::Laguerre(aCoeffs, 5, 1.0e-12, 0);
  EXPECT_FALSE(aResult.IsDone());
  EXPECT_EQ(aResult.Status, MathUtils::Status::MaxIterations);
}
