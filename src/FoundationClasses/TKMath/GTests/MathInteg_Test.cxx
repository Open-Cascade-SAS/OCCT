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
#include <MathInteg_Multiple.hxx>
#include <MathInteg_Set.hxx>
#include <MathUtils_Core.hxx>

#include <cmath>
#include <limits>

namespace
{
constexpr double THE_TOLERANCE = 1.0e-10;

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

class FailingFunc
{
public:
  bool Value(double, double&) const { return false; }
};

class NonFiniteFunc
{
public:
  bool Value(double, double& theF) const
  {
    theF = std::numeric_limits<double>::quiet_NaN();
    return true;
  }
};

class KinkFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::abs(theX - 0.3141592653589793);
    return true;
  }
};

class DampedCosineFunc
{
public:
  bool Value(double theX, double& theF)
  {
    theF = std::exp(-theX) * std::cos(2.0 * theX);
    return true;
  }
};

class ExponentialSineFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::exp(std::sin(theX));
    return true;
  }
};

class ShiftedRationalFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = 1.0 / (theX * theX + theX + 1.0);
    return true;
  }
};

class ShiftedGaussianFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    const double aShift = theX - 0.4;
    theF                = std::exp(-2.3 * aShift * aShift);
    return true;
  }
};

class RationalOscillationFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::cos(37.0 * theX) / (1.0 + theX * theX);
    return true;
  }
};

class LogCosineFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::log1p(theX * theX) * std::cos(0.5 * theX);
    return true;
  }
};

class SquareRootLogFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::sqrt(theX) * std::log1p(theX);
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
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, MathUtils::THE_PI, 15);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 2.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, CosineFunction)
{
  CosFunc aFunc;
  // Integral of cos(x) from 0 to PI/2 = 1
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, MathUtils::THE_PI / 2.0, 15);
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
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, MathUtils::THE_PI, 15);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbPoints, 15);
  EXPECT_NEAR(*aResult.Value, 2.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, Order21)
{
  SinFunc                aFunc;
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, MathUtils::THE_PI, 21);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbPoints, 21);
  EXPECT_NEAR(*aResult.Value, 2.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, Order9)
{
  SinFunc                aFunc;
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, MathUtils::THE_PI, 9);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbPoints, 9);
  EXPECT_NEAR(*aResult.Value, 2.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, Order61)
{
  SinFunc                aFunc;
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, MathUtils::THE_PI, 61);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbPoints, 61);
  EXPECT_NEAR(*aResult.Value, 2.0, THE_TOLERANCE);
}

TEST(MathInteg_GaussTest, InvalidOrderNonPositive)
{
  SinFunc                aFunc;
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, MathUtils::THE_PI, 0);
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

  MathInteg::IntegResult aResult =
    MathInteg::GaussAdaptive(aFunc, 0.0, MathUtils::THE_PI, aConfig);
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
  MathInteg::IntegResult aResult =
    MathInteg::GaussAdaptive(aFunc, 0.0, MathUtils::THE_PI, aConfig);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-6);
}

TEST(MathInteg_GaussAdaptiveTest, ProvidesErrorEstimate)
{
  SinFunc                aFunc;
  MathInteg::IntegConfig aConfig;
  aConfig.Tolerance = 1.0e-8;

  MathInteg::IntegResult aResult =
    MathInteg::GaussAdaptive(aFunc, 0.0, MathUtils::THE_PI, aConfig);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.AbsoluteError.has_value());
  ASSERT_TRUE(aResult.RelativeError.has_value());
  EXPECT_TRUE(std::isfinite(*aResult.AbsoluteError));
  EXPECT_TRUE(std::isfinite(*aResult.RelativeError));
  EXPECT_GE(*aResult.AbsoluteError, 0.0);
  EXPECT_GE(*aResult.RelativeError, 0.0);
  EXPECT_LT(*aResult.AbsoluteError, 1.0e-6);
}

TEST(MathInteg_GaussAdaptiveTest, UsesConfiguredOrders)
{
  QuadraticFunc          aFunc;
  MathInteg::IntegConfig aConfig;
  aConfig.InitialOrder  = 9;
  aConfig.MaxOrder      = 18;
  aConfig.Tolerance     = 1.0e-12;
  aConfig.MaxIterations = 2;

  MathInteg::IntegResult aResult = MathInteg::GaussAdaptive(aFunc, 0.0, 1.0, aConfig);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.NbPoints, 27u);
  EXPECT_NEAR(*aResult.Value, 1.0 / 3.0, THE_TOLERANCE);
}

// ============================================================================
// Composite Gauss tests
// ============================================================================

TEST(MathInteg_GaussCompositeTest, SineFunction)
{
  SinFunc aFunc;
  // 10 subintervals with 7-point Gauss each
  MathInteg::IntegResult aResult =
    MathInteg::GaussComposite(aFunc, 0.0, MathUtils::THE_PI, 10, 7);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 2.0, THE_TOLERANCE);
  EXPECT_EQ(aResult.NbPoints, 70); // 10 * 7
}

TEST(MathInteg_GaussCompositeTest, OscillatoryFunction)
{
  OscillatoryFunc aFunc;
  // More subintervals for oscillatory function
  MathInteg::IntegResult aResult =
    MathInteg::GaussComposite(aFunc, 0.0, MathUtils::THE_PI, 50, 7);
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
  EXPECT_NEAR(*aResult.Value, std::sqrt(MathUtils::THE_PI), 1.0e-4);
}

TEST(MathInteg_GaussCompositeTest, InvalidSubintervals)
{
  SinFunc                aFunc;
  MathInteg::IntegResult aResult =
    MathInteg::GaussComposite(aFunc, 0.0, MathUtils::THE_PI, 0, 7);
  EXPECT_EQ(aResult.Status, MathInteg::Status::InvalidInput);
}

// ============================================================================
// Integration bounds tests
// ============================================================================

TEST(MathInteg_BoundsTest, ReversedBounds)
{
  SinFunc                aFunc;
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, MathUtils::THE_PI, 0.0, 15);
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
  EXPECT_NEAR(*aResult.Value,
              MathUtils::THE_PI,
              0.05); // Wide tolerance for infinite integral approximation
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
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, MathUtils::THE_PI, 15);
  EXPECT_TRUE(static_cast<bool>(aResult));
}

TEST(MathInteg_BoolConversionTest, InvalidInputIsFalse)
{
  SinFunc                aFunc;
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, MathUtils::THE_PI, 0);
  EXPECT_FALSE(static_cast<bool>(aResult));
}

// ============================================================================
// Accuracy comparison tests
// ============================================================================

TEST(MathInteg_AccuracyTest, HigherOrderMoreAccurate)
{
  SinFunc      aFunc;
  const double aExact = 2.0;

  MathInteg::IntegResult aResult3  = MathInteg::Gauss(aFunc, 0.0, MathUtils::THE_PI, 3);
  MathInteg::IntegResult aResult7  = MathInteg::Gauss(aFunc, 0.0, MathUtils::THE_PI, 7);
  MathInteg::IntegResult aResult15 = MathInteg::Gauss(aFunc, 0.0, MathUtils::THE_PI, 15);

  ASSERT_TRUE(aResult3.IsDone());
  ASSERT_TRUE(aResult7.IsDone());
  ASSERT_TRUE(aResult15.IsDone());

  double aError3  = std::abs(*aResult3.Value - aExact);
  double aError7  = std::abs(*aResult7.Value - aExact);
  double aError15 = std::abs(*aResult15.Value - aExact);

  EXPECT_GT(aError3, aError7);
  EXPECT_GT(aError7, aError15);
}

TEST(MathInteg_CorrectnessTest, FixedRulesPropagateCallbackAndNonFiniteFailures)
{
  FailingFunc   aFailingFunc;
  NonFiniteFunc aNonFiniteFunc;

  MathInteg::IntegResult aGaussCallback    = MathInteg::Gauss(aFailingFunc, 0.0, 1.0, 7);
  MathInteg::IntegResult aGaussNonFinite   = MathInteg::Gauss(aNonFiniteFunc, 0.0, 1.0, 7);
  MathInteg::IntegResult aKronrodCallback  = MathInteg::KronrodRule(aFailingFunc, 0.0, 1.0, 7);
  MathInteg::IntegResult aKronrodNonFinite = MathInteg::KronrodRule(aNonFiniteFunc, 0.0, 1.0, 7);

  EXPECT_EQ(aGaussCallback.Status, MathInteg::Status::CallbackError);
  EXPECT_EQ(aGaussNonFinite.Status, MathInteg::Status::NumericalError);
  EXPECT_EQ(aKronrodCallback.Status, MathInteg::Status::CallbackError);
  EXPECT_EQ(aKronrodNonFinite.Status, MathInteg::Status::NumericalError);
  EXPECT_EQ(aGaussCallback.NbPoints, 1u);
  EXPECT_EQ(aGaussNonFinite.NbPoints, 1u);
  EXPECT_EQ(aKronrodCallback.NbPoints, 1u);
  EXPECT_EQ(aKronrodNonFinite.NbPoints, 1u);
}

TEST(MathInteg_CorrectnessTest, AdaptiveRulesPreserveChildCallbackStatus)
{
  class DelayedFailure
  {
  public:
    explicit DelayedFailure(size_t theSuccessfulCalls)
        : mySuccessfulCalls(theSuccessfulCalls)
    {
    }

    bool Value(double theX, double& theF)
    {
      if (myCalls++ >= mySuccessfulCalls)
      {
        return false;
      }
      theF = std::abs(theX - 0.3141592653589793);
      return true;
    }

  private:
    size_t mySuccessfulCalls;
    size_t myCalls = 0;
  };

  DelayedFailure         aGaussFunc(3);
  MathInteg::IntegConfig aGaussConfig;
  aGaussConfig.InitialOrder  = 1;
  aGaussConfig.MaxOrder      = 2;
  aGaussConfig.MaxIterations = 10;
  aGaussConfig.Tolerance     = 1.0e-15;
  MathInteg::IntegResult aGaussResult =
    MathInteg::GaussAdaptive(aGaussFunc, 0.0, 1.0, aGaussConfig);

  DelayedFailure           aKronrodFunc(15);
  MathInteg::KronrodConfig aKronrodConfig;
  aKronrodConfig.MaxIterations = 10;
  aKronrodConfig.Tolerance     = 1.0e-15;
  MathInteg::IntegResult aKronrodResult =
    MathInteg::Kronrod(aKronrodFunc, 0.0, 1.0, aKronrodConfig);

  EXPECT_EQ(aGaussResult.Status, MathInteg::Status::CallbackError);
  EXPECT_EQ(aGaussResult.NbPoints, 4u);
  EXPECT_EQ(aKronrodResult.Status, MathInteg::Status::CallbackError);
  EXPECT_EQ(aKronrodResult.NbPoints, 16u);
}

TEST(MathInteg_KronrodTest, AutoCountsExploratoryAndAdaptiveEvaluations)
{
  class CountingKink
  {
  public:
    bool Value(double theX, double& theF)
    {
      ++NbCalls;
      theF = std::abs(theX - 0.3141592653589793);
      return true;
    }

    size_t NbCalls = 0;
  };

  CountingKink           aFunc;
  MathInteg::IntegResult aResult = MathInteg::KronrodAuto(aFunc, 0.0, 1.0, 1.0e-30, 7);

  EXPECT_EQ(aResult.NbPoints, aFunc.NbCalls);
  EXPECT_GT(aResult.NbPoints, 15u);
}

TEST(MathInteg_KronrodTest, AppliesQuadpackRoundoffFloor)
{
  ConstantFunc           aFunc;
  MathInteg::IntegResult aResult = MathInteg::KronrodRule(aFunc, 0.0, 2.0, 7);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.AbsoluteError.has_value());
  EXPECT_GE(*aResult.AbsoluteError,
            50.0 * std::numeric_limits<double>::epsilon() * std::abs(*aResult.Value));
}

TEST(MathInteg_KronrodTest, UnrepresentableMidpointIsNotConverged)
{
  ConstantFunc aFunc;
  const double aLower  = 0x1.0p52;
  const double anUpper = std::nextafter(aLower, std::numeric_limits<double>::infinity());
  MathInteg::KronrodConfig aConfig;
  aConfig.Tolerance     = 1.0e-30;
  aConfig.MaxIterations = 2;

  MathInteg::IntegResult aResult = MathInteg::Kronrod(aFunc, aLower, anUpper, aConfig);

  EXPECT_EQ(aResult.Status, MathInteg::Status::NotConverged);
  EXPECT_EQ(aResult.NbIterations, 1u);
}

TEST(MathInteg_CorrectnessTest, FiniteRulesRejectInvalidIntervals)
{
  SinFunc      aFunc;
  const double anInf = std::numeric_limits<double>::infinity();

  EXPECT_EQ(MathInteg::Gauss(aFunc, 1.0, 1.0, 7).Status, MathInteg::Status::InvalidInput);
  EXPECT_EQ(MathInteg::Gauss(aFunc, 0.0, anInf, 7).Status, MathInteg::Status::InvalidInput);
  EXPECT_EQ(MathInteg::KronrodRule(aFunc, 0.0, 1.0, 31).Status, MathInteg::Status::InvalidInput);
  EXPECT_EQ(MathInteg::TanhSinh(aFunc, 0.0, anInf).Status, MathInteg::Status::InvalidInput);
}

TEST(MathInteg_GaussAdaptiveTest, WorkBudgetExhaustionRetainsEstimateAndError)
{
  KinkFunc               aFunc;
  MathInteg::IntegConfig aConfig;
  aConfig.InitialOrder  = 1;
  aConfig.MaxOrder      = 2;
  aConfig.MaxIterations = 1;
  aConfig.Tolerance     = 1.0e-15;

  MathInteg::IntegResult aResult = MathInteg::GaussAdaptive(aFunc, 0.0, 1.0, aConfig);
  EXPECT_EQ(aResult.Status, MathInteg::Status::MaxIterations);
  EXPECT_TRUE(aResult.Value.has_value());
  EXPECT_TRUE(aResult.AbsoluteError.has_value());
  EXPECT_GT(*aResult.AbsoluteError, 0.0);
  EXPECT_EQ(aResult.NbIterations, 1u);
  EXPECT_EQ(aResult.NbPoints, 3u);
}

TEST(MathInteg_KronrodTest, UnattainableToleranceIsNotSuccess)
{
  KinkFunc                 aFunc;
  MathInteg::KronrodConfig aConfig;
  aConfig.MaxIterations = 1;
  aConfig.Tolerance     = 1.0e-16;

  MathInteg::IntegResult aResult = MathInteg::Kronrod(aFunc, 0.0, 1.0, aConfig);
  EXPECT_EQ(aResult.Status, MathInteg::Status::MaxIterations);
  EXPECT_TRUE(aResult.Value.has_value());
  EXPECT_TRUE(aResult.AbsoluteError.has_value());
}

TEST(MathInteg_KronrodTest, DampedCosineOverPi)
{
  DampedCosineFunc           aFunc;
  MathInteg::KronrodConfig   aConfig(1.0e-12, 100);
  const MathUtils::IntegResult aResult =
    MathInteg::Kronrod(aFunc, 0.0, MathUtils::THE_PI, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(*aResult.Value, 0.19135721634724556, 1.0e-11);
}

TEST(MathInteg_DoubleExpTest, CallbackFailureAndLevelExhaustionAreReported)
{
  FailingFunc                aFailingFunc;
  KinkFunc                   aKinkFunc;
  MathInteg::DoubleExpConfig aConfig;
  aConfig.NbLevels = 1;

  MathInteg::IntegResult aFailure     = MathInteg::TanhSinh(aFailingFunc, 0.0, 1.0, aConfig);
  MathInteg::IntegResult anExhausted  = MathInteg::TanhSinh(aKinkFunc, 0.0, 1.0, aConfig);
  MathInteg::IntegResult anExpFailure = MathInteg::ExpSinh(aFailingFunc, 0.0, aConfig);
  MathInteg::IntegResult aSinhFailure = MathInteg::SinhSinh(aFailingFunc, aConfig);
  EXPECT_EQ(aFailure.Status, MathInteg::Status::CallbackError);
  EXPECT_EQ(anExpFailure.Status, MathInteg::Status::CallbackError);
  EXPECT_EQ(aSinhFailure.Status, MathInteg::Status::CallbackError);
  EXPECT_EQ(anExhausted.Status, MathInteg::Status::MaxIterations);
  EXPECT_TRUE(anExhausted.Value.has_value());
  EXPECT_FALSE(anExhausted.AbsoluteError.has_value());
}

TEST(MathInteg_CorrectnessTest, SingularSampleIsNumericalError)
{
  class SingularFunc
  {
  public:
    bool Value(double theX, double& theF) const
    {
      theF = 1.0 / theX;
      return true;
    }
  };

  SingularFunc aFunc;
  EXPECT_EQ(MathInteg::Gauss(aFunc, -1.0, 1.0, 3).Status, MathInteg::Status::NumericalError);
}

TEST(MathInteg_MultipleTest, ValidatesDimensionsAndPropagatesCallbackFailure)
{
  class MultipleFunc
  {
  public:
    bool Value(const math_Vector&, double&) const { return false; }
  };

  MultipleFunc           aFunc;
  math_Vector            aLower(3, 4, 0.0);
  math_Vector            anUpper(5, 6, 1.0);
  math_IntegerVector     anOrders(7, 8, 2);
  MathInteg::IntegResult aResult = MathInteg::GaussMultiple(aFunc, 2, aLower, anUpper, anOrders);
  EXPECT_EQ(aResult.Status, MathInteg::Status::CallbackError);
  EXPECT_EQ(aResult.NbPoints, 1u);

  anUpper(5) = 0.0;
  EXPECT_EQ(MathInteg::GaussMultiple(aFunc, 2, aLower, anUpper, anOrders).Status,
            MathInteg::Status::InvalidInput);
}

TEST(MathInteg_SetTest, UsesArbitraryBoundsAndRejectsNonFiniteComponents)
{
  class SetFunc
  {
  public:
    int NbEquations() const { return 2; }

    bool Value(const math_Vector&, math_Vector& theValues) const
    {
      theValues.ChangeAt(0) = 1.0;
      theValues.ChangeAt(1) = std::numeric_limits<double>::infinity();
      return true;
    }
  };

  SetFunc              aFunc;
  MathInteg::SetResult aResult = MathInteg::GaussSet(aFunc, 0.0, 1.0, 3);
  EXPECT_EQ(aResult.Status, MathInteg::Status::NumericalError);
  EXPECT_EQ(aResult.NbPoints, 1u);
}

TEST(MathInteg_SetTest, CallbackFailureIsDistinctFromNonFiniteComponents)
{
  class FailingSetFunc
  {
  public:
    int NbEquations() const { return 1; }

    bool Value(const math_Vector&, math_Vector&) const { return false; }
  };

  FailingSetFunc       aFunc;
  MathInteg::SetResult aResult = MathInteg::GaussSet(aFunc, 0.0, 1.0, 3);
  EXPECT_EQ(aResult.Status, MathInteg::Status::CallbackError);
  EXPECT_EQ(aResult.NbPoints, 1u);
}

TEST(MathInteg_BoundsTest, FiniteRulesPreserveIntervalOrientation)
{
  class MultipleConstantFunc
  {
  public:
    bool Value(const math_Vector&, double& theF) const
    {
      theF = 1.0;
      return true;
    }
  };
  class SetConstantFunc
  {
  public:
    int NbEquations() const { return 1; }

    bool Value(const math_Vector&, math_Vector& theF) const
    {
      theF.ChangeAt(0) = 1.0;
      return true;
    }
  };

  SinFunc                aSinFunc;
  MathInteg::IntegResult aKronrod =
    MathInteg::KronrodRule(aSinFunc, MathUtils::THE_PI, 0.0, 7);
  MathInteg::IntegResult aTanhSinh = MathInteg::TanhSinh(aSinFunc, MathUtils::THE_PI, 0.0);
  MultipleConstantFunc   aMultipleFunc;
  math_Vector            aLower(2);
  math_Vector            anUpper(2);
  math_IntegerVector     anOrders(size_t{2}, 2);
  aLower.ChangeAt(0) = 1.0;
  aLower.ChangeAt(1) = 0.0;
  anUpper.ChangeAt(0) = 0.0;
  anUpper.ChangeAt(1) = 2.0;
  MathInteg::IntegResult aMultiple =
    MathInteg::GaussMultiple(aMultipleFunc, 2, aLower, anUpper, anOrders);
  SetConstantFunc      aSetFunc;
  MathInteg::SetResult aSet = MathInteg::GaussSet(aSetFunc, 2.0, 0.0, 3);

  ASSERT_TRUE(aKronrod.IsDone());
  ASSERT_TRUE(aTanhSinh.IsDone());
  ASSERT_TRUE(aMultiple.IsDone());
  ASSERT_TRUE(aSet.IsDone());
  ASSERT_TRUE(aSet.Values.has_value());
  EXPECT_NEAR(*aKronrod.Value, -2.0, THE_TOLERANCE);
  EXPECT_NEAR(*aTanhSinh.Value, -2.0, 1.0e-6);
  EXPECT_NEAR(*aMultiple.Value, -2.0, THE_TOLERANCE);
  EXPECT_NEAR(aSet.Values->At(0), -2.0, THE_TOLERANCE);
}

TEST(MathInteg_DoubleExpTest, TinyStepIsBoundedPerLevel)
{
  class ZeroFunc
  {
  public:
    bool Value(double, double& theF) const
    {
      theF = 0.0;
      return true;
    }
  };

  ZeroFunc                   aFunc;
  MathInteg::DoubleExpConfig aConfig;
  aConfig.StepFactor        = std::numeric_limits<double>::denorm_min();
  aConfig.MaxPointsPerLevel = 4;

  MathInteg::IntegResult aTanhResult = MathInteg::TanhSinh(aFunc, 0.0, 1.0, aConfig);
  MathInteg::IntegResult anExpResult = MathInteg::ExpSinh(aFunc, 0.0, aConfig);
  MathInteg::IntegResult aSinhResult = MathInteg::SinhSinh(aFunc, aConfig);

  EXPECT_EQ(aTanhResult.Status, MathInteg::Status::MaxIterations);
  EXPECT_EQ(anExpResult.Status, MathInteg::Status::MaxIterations);
  EXPECT_EQ(aSinhResult.Status, MathInteg::Status::MaxIterations);
  EXPECT_EQ(aTanhResult.NbPoints, 4u);
  EXPECT_EQ(anExpResult.NbPoints, 4u);
  EXPECT_EQ(aSinhResult.NbPoints, 4u);
}

TEST(MathInteg_CorrectnessTest, SmallIntegralsUseRelativeTolerance)
{
  class ScaledKinkFunc
  {
  public:
    bool Value(double theX, double& theF) const
    {
      theF = 1.0e-12 * std::abs(theX - 0.3141592653589793);
      return true;
    }
  };
  class CountingScaledKinkFunc
  {
  public:
    bool Value(double theX, double& theF)
    {
      ++NbCalls;
      theF = 1.0e-12 * std::abs(theX - 0.3141592653589793);
      return true;
    }

    size_t NbCalls = 0;
  };

  ScaledKinkFunc         aGaussFunc;
  MathInteg::IntegConfig aGaussConfig;
  aGaussConfig.InitialOrder  = 1;
  aGaussConfig.MaxOrder      = 2;
  aGaussConfig.MaxIterations = 1;
  aGaussConfig.Tolerance     = 1.0e-10;
  MathInteg::IntegResult aGaussResult =
    MathInteg::GaussAdaptive(aGaussFunc, 0.0, 1.0, aGaussConfig);

  ScaledKinkFunc           aKronrodFunc;
  MathInteg::KronrodConfig aKronrodConfig;
  aKronrodConfig.MaxIterations = 1;
  aKronrodConfig.Tolerance     = 1.0e-10;
  MathInteg::IntegResult aKronrodResult =
    MathInteg::Kronrod(aKronrodFunc, 0.0, 1.0, aKronrodConfig);

  ScaledKinkFunc             aDoubleExpFunc;
  MathInteg::DoubleExpConfig aDoubleExpConfig;
  aDoubleExpConfig.NbLevels  = 2;
  aDoubleExpConfig.Tolerance = 1.0e-10;
  MathInteg::IntegResult aDoubleExpResult =
    MathInteg::TanhSinh(aDoubleExpFunc, 0.0, 1.0, aDoubleExpConfig);

  CountingScaledKinkFunc aAutoFunc;
  MathInteg::IntegResult aAutoResult =
    MathInteg::KronrodAuto(aAutoFunc, 0.0, 1.0, 1.0e-10, 7);

  EXPECT_EQ(aGaussResult.Status, MathInteg::Status::MaxIterations);
  EXPECT_EQ(aKronrodResult.Status, MathInteg::Status::MaxIterations);
  EXPECT_EQ(aDoubleExpResult.Status, MathInteg::Status::MaxIterations);
  EXPECT_EQ(aAutoResult.NbPoints, aAutoFunc.NbCalls);
  EXPECT_GT(aAutoResult.NbPoints, 15u);
}

TEST(MathInteg_KronrodTest, NearZeroResultHasNoRelativeErrorEstimate)
{
  SinFunc                aFunc;
  MathInteg::IntegResult aResult =
    MathInteg::KronrodRule(aFunc, -MathUtils::THE_PI, MathUtils::THE_PI, 7);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
  EXPECT_FALSE(aResult.RelativeError.has_value());
}

TEST(MathInteg_SetTest, RejectsNegativeEquationCountBeforeAllocation)
{
  class NegativeSizeFunc
  {
  public:
    int NbEquations() const { return -1; }

    bool Value(const math_Vector&, math_Vector&) const { return true; }
  };

  NegativeSizeFunc     aFunc;
  MathInteg::SetResult aResult = MathInteg::GaussSet(aFunc, 0.0, 1.0, 3);
  EXPECT_EQ(aResult.Status, MathInteg::Status::InvalidInput);
  EXPECT_EQ(aResult.NbPoints, 0u);
}

TEST(MathInteg_GaussTest, ExponentialSineOnFiniteInterval)
{
  ExponentialSineFunc    aFunc;
  MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, 1.3, 21);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(*aResult.Value, 2.3771445613036857, 2.0e-13);
}

TEST(MathInteg_KronrodTest, ShiftedRationalOnFiniteInterval)
{
  ShiftedRationalFunc      aFunc;
  MathInteg::KronrodConfig aConfig(1.0e-12, 100);
  MathInteg::IntegResult   aResult = MathInteg::Kronrod(aFunc, -0.4, 2.3, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(*aResult.Value, 1.3346874620709692, 2.0e-11);
}

TEST(MathInteg_GaussCompositeTest, ShiftedGaussianOnFiniteInterval)
{
  ShiftedGaussianFunc    aFunc;
  MathInteg::IntegResult aResult = MathInteg::GaussComposite(aFunc, -1.2, 2.1, 16, 15);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(*aResult.Value, 1.1682152570688034, 2.0e-13);
}

TEST(MathInteg_GaussCompositeTest, RationalOscillationOnFiniteInterval)
{
  RationalOscillationFunc aFunc;
  MathInteg::IntegResult   aResult = MathInteg::GaussComposite(aFunc, 0.2, 3.4, 64, 15);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(*aResult.Value, -0.023002601281013002, 2.0e-12);
}

TEST(MathInteg_KronrodTest, LogCosineOnAsymmetricInterval)
{
  LogCosineFunc             aFunc;
  MathInteg::KronrodConfig  aConfig(1.0e-12, 100);
  MathInteg::IntegResult    aResult = MathInteg::Kronrod(aFunc, -0.7, 2.4, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(*aResult.Value, 1.4805767078645715, 2.0e-11);
}

TEST(MathInteg_DoubleExpTest, ReversedSquareRootLogInterval)
{
  SquareRootLogFunc          aFunc;
  MathInteg::DoubleExpConfig aConfig(1.0e-11, 100);
  aConfig.NbLevels = 12;
  MathInteg::IntegResult aResult = MathInteg::TanhSinh(aFunc, 2.0, 0.0, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(*aResult.Value, -1.4263470681209449, 2.0e-9);
}
