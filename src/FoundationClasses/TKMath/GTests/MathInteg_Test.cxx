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

#include <MathInteg_Gauss.hxx>
#include <MathInteg_Kronrod.hxx>
#include <MathInteg_DoubleExp.hxx>

#include <cmath>

namespace
{
constexpr double THE_TOLERANCE = 1.0e-10;
constexpr double THE_PI        = 3.14159265358979323846;

//! Constant function: f(x) = 5
//! Integral from a to b = 5*(b-a)
class ConstantFunc
{
public:
  bool Value(double, double& theF) const
  {
    theF = 5.0;
    return true;
  }
};

//! Linear function: f(x) = 2x + 1
//! Integral from a to b = x^2 + x |_a^b
class LinearFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = 2.0 * theX + 1.0;
    return true;
  }
};

//! Quadratic function: f(x) = x^2
//! Integral from 0 to 1 = 1/3
class QuadraticFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = theX * theX;
    return true;
  }
};

//! Sine function: f(x) = sin(x)
//! Integral from 0 to PI = 2
class SinFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::sin(theX);
    return true;
  }
};

//! Cosine function: f(x) = cos(x)
//! Integral from 0 to PI/2 = 1
class CosFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::cos(theX);
    return true;
  }
};

//! Exponential function: f(x) = e^x
//! Integral from 0 to 1 = e - 1
class ExpFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::exp(theX);
    return true;
  }
};

//! Polynomial: f(x) = x^5 - 3x^3 + 2x
//! Gauss quadrature should be exact for polynomials up to degree 2n-1
class PolynomialFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    const double aX2 = theX * theX;
    const double aX3 = aX2 * theX;
    theF             = aX2 * aX3 - 3.0 * aX3 + 2.0 * theX;
    return true;
  }
};

//! Gaussian function: f(x) = e^(-x^2)
//! Integral from -inf to inf = sqrt(PI)
class GaussianFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::exp(-theX * theX);
    return true;
  }
};

//! Reciprocal function: f(x) = 1/x
//! Integral from 1 to e = 1
class ReciprocalFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = 1.0 / theX;
    return true;
  }
};

//! Oscillatory function: f(x) = sin(10x)
//! Requires more points or subdivision
class OscillatoryFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::sin(10.0 * theX);
    return true;
  }
};

//! Square root: f(x) = sqrt(x)
//! Integral from 0 to 1 = 2/3
class SqrtFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::sqrt(theX);
    return true;
  }
};
} // namespace

// ============================================================================
// Basic Gauss quadrature tests
// ============================================================================

TEST(MathInteg_GaussTest, ConstantFunction)
{
  ConstantFunc aFunc;
  // Integral of 5 from 0 to 2 = 10
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, 2.0, 3);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 10.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, LinearFunction)
{
  LinearFunc aFunc;
  // Integral of (2x+1) from 0 to 3 = [x^2 + x]_0^3 = 9 + 3 = 12
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, 3.0, 3);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 12.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, QuadraticFunction)
{
  QuadraticFunc aFunc;
  // Integral of x^2 from 0 to 1 = 1/3
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, 1.0, 3);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 1.0 / 3.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, SineFunction)
{
  SinFunc aFunc;
  // Integral of sin(x) from 0 to PI = 2
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, THE_PI, 15);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 2.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, CosineFunction)
{
  CosFunc aFunc;
  // Integral of cos(x) from 0 to PI/2 = 1
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, THE_PI / 2.0, 15);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 1.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, ExponentialFunction)
{
  ExpFunc aFunc;
  // Integral of e^x from 0 to 1 = e - 1
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, 1.0, 15);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, std::exp(1.0) - 1.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, ReciprocalFunction)
{
  ReciprocalFunc aFunc;
  // Integral of 1/x from 1 to e = ln(e) - ln(1) = 1
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 1.0, std::exp(1.0), 15);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 1.0, THE_TOLERANCE);
}

// ============================================================================
// Different quadrature orders
// ============================================================================

TEST(MathInteg_GaussTest, Order3)
{
  QuadraticFunc          aFunc;
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, 1.0, 3);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbPoints, 3);
  EXPECT_NEAR(*aResult.Value, 1.0 / 3.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, Order5)
{
  QuadraticFunc          aFunc;
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, 1.0, 5);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbPoints, 5);
  EXPECT_NEAR(*aResult.Value, 1.0 / 3.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, Order7)
{
  QuadraticFunc          aFunc;
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, 1.0, 7);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbPoints, 7);
  EXPECT_NEAR(*aResult.Value, 1.0 / 3.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, Order15)
{
  SinFunc                aFunc;
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, THE_PI, 15);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbPoints, 15);
  EXPECT_NEAR(*aResult.Value, 2.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, Order21)
{
  SinFunc                aFunc;
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, THE_PI, 21);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbPoints, 21);
  EXPECT_NEAR(*aResult.Value, 2.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, InvalidOrder)
{
  SinFunc aFunc;
  // Order 9 is not supported (supported orders: 3, 4, 5, 6, 7, 8, 10, 15, 21, 31)
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, THE_PI, 9);
  EXPECT_EQ(aResult.Status, MathInteg::Status::InvalidInput);
}

// ============================================================================
// Polynomial exactness tests
// ============================================================================

TEST(MathInteg_GaussTest, PolynomialExactness_Order3)
{
  // 3-point Gauss should be exact for polynomials up to degree 5
  PolynomialFunc aFunc;
  // Integral of (x^5 - 3x^3 + 2x) from -1 to 1 = 0 (odd function)
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, -1.0, 1.0, 3);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, PolynomialExactness_Order7)
{
  // 7-point Gauss should be exact for polynomials up to degree 13
  PolynomialFunc         aFunc;
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, -1.0, 1.0, 7);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

// ============================================================================
// Adaptive Gauss tests
// ============================================================================

TEST(MathInteg_GaussAdaptiveTest, SineFunction)
{
  SinFunc                aFunc;
  MathInteg::IntegConfig aConfig;
  aConfig.Tolerance = 1.0e-12;

  MathInteg::IntegResult aResult = MathInteg::GaussAdaptive(aFunc, 0.0, THE_PI, aConfig);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 2.0, 1.0e-11);
}

TEST(MathInteg_GaussAdaptiveTest, ExponentialFunction)
{
  ExpFunc                aFunc;
  MathInteg::IntegConfig aConfig;
  aConfig.Tolerance = 1.0e-12;

  MathInteg::IntegResult aResult = MathInteg::GaussAdaptive(aFunc, 0.0, 1.0, aConfig);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, std::exp(1.0) - 1.0, 1.0e-11);
}

TEST(MathInteg_GaussAdaptiveTest, OscillatoryFunction)
{
  OscillatoryFunc        aFunc;
  MathInteg::IntegConfig aConfig;
  aConfig.Tolerance     = 1.0e-8;
  aConfig.MaxIterations = 20;

  // Integral of sin(10x) from 0 to PI = (1 - cos(10*PI))/10 = 0
  MathInteg::IntegResult aResult = MathInteg::GaussAdaptive(aFunc, 0.0, THE_PI, aConfig);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-6);
}

TEST(MathInteg_GaussAdaptiveTest, ProvidesErrorEstimate)
{
  SinFunc                aFunc;
  MathInteg::IntegConfig aConfig;
  aConfig.Tolerance = 1.0e-8;

  MathInteg::IntegResult aResult = MathInteg::GaussAdaptive(aFunc, 0.0, THE_PI, aConfig);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_GT(aResult.AbsoluteError, 0.0);
  EXPECT_LT(aResult.AbsoluteError, 1.0e-6);
}

// ============================================================================
// Composite Gauss tests
// ============================================================================

TEST(MathInteg_GaussCompositeTest, SineFunction)
{
  SinFunc aFunc;
  // 10 subintervals with 7-point Gauss each
  MathInteg::IntegResult aResult = MathInteg::GaussComposite(aFunc, 0.0, THE_PI, 10, 7);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 2.0, THE_TOLERANCE);
  EXPECT_EQ(aResult.NbPoints, 70); // 10 * 7
}

TEST(MathInteg_GaussCompositeTest, OscillatoryFunction)
{
  OscillatoryFunc aFunc;
  // More subintervals for oscillatory function
  MathInteg::IntegResult aResult = MathInteg::GaussComposite(aFunc, 0.0, THE_PI, 50, 7);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-8);
}

TEST(MathInteg_GaussCompositeTest, GaussianFunction)
{
  GaussianFunc aFunc;
  // Integral from -3 to 3 (approximates -inf to inf)
  MathInteg::IntegResult aResult = MathInteg::GaussComposite(aFunc, -3.0, 3.0, 50, 15);
  ASSERT_TRUE(aResult.IsDone());
  // Should be close to sqrt(PI) ~= 1.7724538509
  EXPECT_NEAR(*aResult.Value, std::sqrt(THE_PI), 1.0e-4);
}

TEST(MathInteg_GaussCompositeTest, InvalidSubintervals)
{
  SinFunc                aFunc;
  MathInteg::IntegResult aResult = MathInteg::GaussComposite(aFunc, 0.0, THE_PI, 0, 7);
  EXPECT_EQ(aResult.Status, MathInteg::Status::InvalidInput);
}

// ============================================================================
// Integration bounds tests
// ============================================================================

TEST(MathInteg_BoundsTest, ReversedBounds)
{
  SinFunc aFunc;
  // Integral from PI to 0 should be -2
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, THE_PI, 0.0, 15);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, -2.0, THE_TOLERANCE);
}

TEST(MathInteg_BoundsTest, NegativeBounds)
{
  QuadraticFunc aFunc;
  // Integral of x^2 from -1 to 1 = 2/3
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, -1.0, 1.0, 7);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 2.0 / 3.0, THE_TOLERANCE);
}

TEST(MathInteg_BoundsTest, SmallInterval)
{
  SinFunc aFunc;
  // Very small interval
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, 0.001, 15);
  ASSERT_TRUE(aResult.IsDone());
  // sin(x) ~= x for small x, so integral ~= x^2/2 = 0.0000005
  EXPECT_NEAR(*aResult.Value, 0.0000005, 1.0e-10);
}

TEST(MathInteg_BoundsTest, LargeInterval)
{
  // f(x) = 1/(1+x^2), integral from -inf to inf = PI
  class LorentzianFunc
  {
  public:
    bool Value(double theX, double& theF) const
    {
      theF = 1.0 / (1.0 + theX * theX);
      return true;
    }
  };

  LorentzianFunc aFunc;
  // Approximate with large but finite bounds
  MathInteg::IntegResult aResult = MathInteg::GaussComposite(aFunc, -100.0, 100.0, 200, 15);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, THE_PI, 0.05); // Wide tolerance for infinite integral approximation
}

// ============================================================================
// Special functions tests
// ============================================================================

TEST(MathInteg_SpecialTest, SqrtFunction)
{
  SqrtFunc aFunc;
  // Integral of sqrt(x) from 0 to 1 = 2/3
  // Note: integrand has singularity in derivative at x=0
  // Use composite integration to avoid singularity issue
  MathInteg::IntegResult aResult = MathInteg::GaussComposite(aFunc, 0.001, 1.0, 20, 15);
  ASSERT_TRUE(aResult.IsDone());
  // Integral from 0.001 to 1 is slightly less than 2/3
  const double aExpected = (2.0 / 3.0) * (1.0 - std::pow(0.001, 1.5));
  EXPECT_NEAR(*aResult.Value, aExpected, 1.0e-6);
}

// ============================================================================
// Boolean conversion tests
// ============================================================================

TEST(MathInteg_BoolConversionTest, SuccessfulResultIsTrue)
{
  SinFunc                aFunc;
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, THE_PI, 15);
  EXPECT_TRUE(static_cast<bool>(aResult));
}

TEST(MathInteg_BoolConversionTest, InvalidInputIsFalse)
{
  SinFunc aFunc;
  // Order 9 is not supported
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, THE_PI, 9);
  EXPECT_FALSE(static_cast<bool>(aResult));
}

// ============================================================================
// Accuracy comparison tests
// ============================================================================

TEST(MathInteg_AccuracyTest, HigherOrderMoreAccurate)
{
  SinFunc      aFunc;
  const double aExact = 2.0;

  MathInteg::IntegResult aResult3  = MathInteg::Gauss(aFunc, 0.0, THE_PI, 3);
  MathInteg::IntegResult aResult7  = MathInteg::Gauss(aFunc, 0.0, THE_PI, 7);
  MathInteg::IntegResult aResult15 = MathInteg::Gauss(aFunc, 0.0, THE_PI, 15);

  ASSERT_TRUE(aResult3.IsDone());
  ASSERT_TRUE(aResult7.IsDone());
  ASSERT_TRUE(aResult15.IsDone());

  double aError3  = std::abs(*aResult3.Value - aExact);
  double aError7  = std::abs(*aResult7.Value - aExact);
  double aError15 = std::abs(*aResult15.Value - aExact);

  EXPECT_GT(aError3, aError7);
  EXPECT_GT(aError7, aError15);
}
