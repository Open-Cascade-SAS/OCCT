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

#include <MathRoot_Newton.hxx>
#include <MathRoot_Brent.hxx>
#include <MathRoot_Bisection.hxx>
#include <MathRoot_Secant.hxx>
#include <MathUtils_Core.hxx>

#include <cmath>
#include <limits>

namespace
{
constexpr double THE_TOLERANCE = 1.0e-10;

//! Function with derivative: f(x) = x^2 - 2, f'(x) = 2x
//! Root at x = sqrt(2) ~= 1.41421356...
class SqrtTwoFunc
{
public:
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

//! Function: f(x) = cos(x) - x
//! Root at x ~= 0.739085...
class CosMinusXFunc
{
public:
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

//! Function: f(x) = x^3 - x - 2
//! Root at x ~= 1.5214...
class CubicFunc
{
public:
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

//! Function: f(x) = sin(x)
//! Roots at x = n*PI for integer n
class SinFunc
{
public:
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

//! Function: f(x) = e^x - 3
//! Root at x = ln(3) ~= 1.0986...
class ExpMinusThreeFunc
{
public:
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

//! Linear function: f(x) = 2x - 4
//! Root at x = 2
class LinearFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = 2.0 * theX - 4.0;
    return true;
  }

  bool Values(double theX, double& theF, double& theDf) const
  {
    theF  = 2.0 * theX - 4.0;
    theDf = 2.0;
    return true;
  }
};

//! Function: f(x) = exp(x) - 50
class ExpMinusFiftyFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::exp(theX) - 50.0;
    return true;
  }

  bool Values(double theX, double& theF, double& theDf) const
  {
    theF  = std::exp(theX) - 50.0;
    theDf = std::exp(theX);
    return true;
  }
};

//! Function: f(x) = sin(x) - 0.25
class ShiftedSinFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::sin(theX) - 0.25;
    return true;
  }

  bool Values(double theX, double& theF, double& theDf) const
  {
    theF  = std::sin(theX) - 0.25;
    theDf = std::cos(theX);
    return true;
  }
};

//! Function: f(x) = log(x) + 2x - 5
class LogLinearFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::log(theX) + 2.0 * theX - 5.0;
    return true;
  }

  bool Values(double theX, double& theF, double& theDf) const
  {
    theF  = std::log(theX) + 2.0 * theX - 5.0;
    theDf = 1.0 / theX + 2.0;
    return true;
  }
};

//! Function: f(x) = x^5 + x + 3
class QuinticFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    const double aX2 = theX * theX;
    theF             = aX2 * aX2 * theX + theX + 3.0;
    return true;
  }

  bool Values(double theX, double& theF, double& theDf) const
  {
    const double aX2 = theX * theX;
    theF             = aX2 * aX2 * theX + theX + 3.0;
    theDf            = 5.0 * aX2 * aX2 + 1.0;
    return true;
  }
};

//! Function: f(x) = x / (x + 2) - cos(x)
class RationalCosFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = theX / (theX + 2.0) - std::cos(theX);
    return true;
  }

  bool Values(double theX, double& theF, double& theDf) const
  {
    const double aDenominator = theX + 2.0;
    theF                     = theX / aDenominator - std::cos(theX);
    theDf                    = 2.0 / (aDenominator * aDenominator) + std::sin(theX);
    return true;
  }
};

//! Function: f(x) = x * exp(x) - 100
class ScaledExpFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = theX * std::exp(theX) - 100.0;
    return true;
  }

  bool Values(double theX, double& theF, double& theDf) const
  {
    const double anExp = std::exp(theX);
    theF               = theX * anExp - 100.0;
    theDf              = (theX + 1.0) * anExp;
    return true;
  }
};

class EndpointFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = theX - 1.0;
    return true;
  }

  bool Values(double theX, double& theF, double& theDf) const
  {
    theF  = theX - 1.0;
    theDf = 1.0;
    return true;
  }
};

class FailingFunc
{
public:
  bool Value(double theX, double& theF)
  {
    if (++myCalls == 3)
    {
      return false;
    }
    theF = theX * theX - 2.0;
    return true;
  }

  bool Values(double theX, double& theF, double& theDf)
  {
    if (++myCalls == 3)
    {
      return false;
    }
    theF  = theX * theX - 2.0;
    theDf = 2.0 * theX;
    return true;
  }

private:
  int myCalls = 0;
};

class NonFiniteFunc
{
public:
  bool Value(double, double& theF) const
  {
    theF = std::numeric_limits<double>::quiet_NaN();
    return true;
  }

  bool Values(double, double& theF, double& theDf) const
  {
    theF  = 1.0;
    theDf = std::numeric_limits<double>::infinity();
    return true;
  }
};
} // namespace

// ============================================================================
// Newton method tests
// ============================================================================

TEST(MathRoot_NewtonTest, SqrtTwo)
{
  SqrtTwoFunc            aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Newton(aFunc, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, std::sqrt(2.0), THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 0.0, THE_TOLERANCE);
}

TEST(MathRoot_NewtonTest, CosMinusX)
{
  CosMinusXFunc          aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Newton(aFunc, 0.5);
  ASSERT_TRUE(aResult.IsDone());
  // Verify root satisfies equation
  double aFx = std::cos(*aResult.Root) - *aResult.Root;
  EXPECT_NEAR(aFx, 0.0, THE_TOLERANCE);
}

TEST(MathRoot_NewtonTest, CubicEquation)
{
  CubicFunc              aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Newton(aFunc, 1.5);
  ASSERT_TRUE(aResult.IsDone());
  // Verify root
  double aFx = *aResult.Root * *aResult.Root * *aResult.Root - *aResult.Root - 2.0;
  EXPECT_NEAR(aFx, 0.0, THE_TOLERANCE);
}

TEST(MathRoot_NewtonTest, LinearFunction)
{
  LinearFunc             aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Newton(aFunc, 0.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 2.0, THE_TOLERANCE);
}

TEST(MathRoot_NewtonTest, ExpFunction)
{
  ExpMinusThreeFunc      aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Newton(aFunc, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, std::log(3.0), THE_TOLERANCE);
}

TEST(MathRoot_NewtonTest, CustomTolerance)
{
  SqrtTwoFunc      aFunc;
  MathRoot::Config aConfig;
  aConfig.XTolerance    = 1.0e-14;
  aConfig.FTolerance    = 1.0e-14;
  aConfig.MaxIterations = 100;

  MathRoot::ScalarResult aResult = MathRoot::Newton(aFunc, 1.0, aConfig);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, std::sqrt(2.0), 1.0e-14);
}

TEST(MathRoot_NewtonTest, RationalCosineEquation)
{
  RationalCosFunc  aFunc;
  MathRoot::Config aConfig;
  aConfig.XTolerance = 1.0e-14;
  aConfig.FTolerance = 1.0e-14;

  const MathRoot::ScalarResult aResult = MathRoot::Newton(aFunc, 1.0, aConfig);
  ASSERT_EQ(aResult.Status, MathRoot::Status::OK);
  ASSERT_TRUE(aResult.Root.has_value());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(*aResult.Root, 1.1887722838868333, 2.0e-13);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-13);
}

TEST(MathRoot_NewtonTest, ScaledExponentialEquation)
{
  ScaledExpFunc     aFunc;
  MathRoot::Config aConfig;
  aConfig.XTolerance = 1.0e-14;
  aConfig.FTolerance = 1.0e-13;

  const MathRoot::ScalarResult aResult = MathRoot::Newton(aFunc, 3.5, aConfig);
  ASSERT_EQ(aResult.Status, MathRoot::Status::OK);
  ASSERT_TRUE(aResult.Root.has_value());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(*aResult.Root, 3.3856301402900502, 2.0e-13);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-12);
}

// ============================================================================
// Newton bounded tests
// ============================================================================

TEST(MathRoot_NewtonBoundedTest, SqrtTwoWithBounds)
{
  SqrtTwoFunc            aFunc;
  MathRoot::ScalarResult aResult = MathRoot::NewtonBounded(aFunc, 1.5, 1.0, 2.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, std::sqrt(2.0), THE_TOLERANCE);
}

TEST(MathRoot_NewtonBoundedTest, SinWithBounds)
{
  SinFunc                aFunc;
  MathRoot::ScalarResult aResult = MathRoot::NewtonBounded(aFunc, 3.0, 2.0, 4.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, MathUtils::THE_PI, THE_TOLERANCE);
}

// ============================================================================
// Secant method tests
// ============================================================================

TEST(MathRoot_SecantTest, SqrtTwo)
{
  SqrtTwoFunc            aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Secant(aFunc, 1.0, 2.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, std::sqrt(2.0), THE_TOLERANCE);
}

TEST(MathRoot_SecantTest, CosMinusX)
{
  CosMinusXFunc          aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Secant(aFunc, 0.0, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  double aFx = std::cos(*aResult.Root) - *aResult.Root;
  EXPECT_NEAR(aFx, 0.0, THE_TOLERANCE);
}

TEST(MathRoot_SecantTest, ExpFunction)
{
  ExpMinusThreeFunc      aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Secant(aFunc, 0.0, 2.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, std::log(3.0), THE_TOLERANCE);
}

// ============================================================================
// Brent method tests
// ============================================================================

TEST(MathRoot_BrentTest, SqrtTwo)
{
  SqrtTwoFunc            aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Brent(aFunc, 1.0, 2.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, std::sqrt(2.0), THE_TOLERANCE);
}

TEST(MathRoot_BrentTest, CosMinusX)
{
  CosMinusXFunc          aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Brent(aFunc, 0.0, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  double aFx = std::cos(*aResult.Root) - *aResult.Root;
  EXPECT_NEAR(aFx, 0.0, THE_TOLERANCE);
}

TEST(MathRoot_BrentTest, CosMinusXHighAccuracy)
{
  CosMinusXFunc    aFunc;
  MathRoot::Config aConfig;
  aConfig.XTolerance                    = 1.0e-14;
  aConfig.FTolerance                    = 1.0e-14;
  const MathUtils::ScalarResult aResult = MathRoot::Brent(aFunc, 0.0, 1.0, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Root.has_value());
  EXPECT_NEAR(*aResult.Root, 0.73908513321516045, 1.0e-13);
}

TEST(MathRoot_BrentTest, CubicEquation)
{
  CubicFunc              aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Brent(aFunc, 1.0, 2.0);
  ASSERT_TRUE(aResult.IsDone());
  double aFx = *aResult.Root * *aResult.Root * *aResult.Root - *aResult.Root - 2.0;
  EXPECT_NEAR(aFx, 0.0, THE_TOLERANCE);
}

TEST(MathRoot_BrentTest, SinPi)
{
  SinFunc                aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Brent(aFunc, 2.0, 4.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, MathUtils::THE_PI, THE_TOLERANCE);
}

TEST(MathRoot_BrentTest, ExpFunction)
{
  ExpMinusThreeFunc      aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Brent(aFunc, 0.0, 2.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, std::log(3.0), THE_TOLERANCE);
}

TEST(MathRoot_BrentTest, ExponentialLevel)
{
  ExpMinusFiftyFunc aFunc;
  MathRoot::Config  aConfig;
  aConfig.XTolerance = 1.0e-14;
  aConfig.FTolerance = 1.0e-13;

  const MathRoot::ScalarResult aResult = MathRoot::Brent(aFunc, 3.0, 5.0, aConfig);
  ASSERT_EQ(aResult.Status, MathRoot::Status::OK);
  ASSERT_TRUE(aResult.Root.has_value());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(*aResult.Root, 3.9120230054281464, 2.0e-13);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-12);
}

TEST(MathRoot_BrentTest, ShiftedSineUpperBranch)
{
  ShiftedSinFunc   aFunc;
  MathRoot::Config aConfig;
  aConfig.XTolerance = 1.0e-14;
  aConfig.FTolerance = 1.0e-14;

  const MathRoot::ScalarResult aResult = MathRoot::Brent(aFunc, 2.0, 4.0, aConfig);
  ASSERT_EQ(aResult.Status, MathRoot::Status::OK);
  ASSERT_TRUE(aResult.Root.has_value());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(*aResult.Root, 2.8889123984477143, 2.0e-13);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-13);
}

TEST(MathRoot_BrentTest, InvalidBracket)
{
  SqrtTwoFunc aFunc;
  // Both endpoints positive - no sign change
  MathRoot::ScalarResult aResult = MathRoot::Brent(aFunc, 2.0, 3.0);
  EXPECT_EQ(aResult.Status, MathRoot::Status::InvalidInput);
}

TEST(MathRoot_BrentTest, ReversedBracket)
{
  SqrtTwoFunc            aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Brent(aFunc, 2.0, 1.0);
  EXPECT_EQ(aResult.Status, MathRoot::Status::InvalidInput);
}

TEST(MathRoot_BrentTest, LargeMagnitudeBracketUsesFloatingPointScaleTolerance)
{
  class SpacingFunc
  {
  public:
    explicit SpacingFunc(double theUpper)
        : myUpper(theUpper)
    {
    }

    bool Value(double theX, double& theF) const
    {
      theF = theX < myUpper ? -1.0 : 1.0;
      return true;
    }

  private:
    double myUpper;
  };

  const double aLower = 1.0e16;
  const double aUpper = std::nextafter(aLower, std::numeric_limits<double>::infinity());
  SpacingFunc  aFunc(aUpper);

  MathRoot::Config aConfig;
  aConfig.XTolerance = 1.0e-20;
  aConfig.FTolerance = 0.0;

  const MathRoot::ScalarResult aResult = MathRoot::Brent(aFunc, aLower, aUpper, aConfig);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Root.has_value());
  EXPECT_TRUE(*aResult.Root == aLower || *aResult.Root == aUpper);
}

TEST(MathRoot_BrentTest, InverseQuadraticInterpolationConvergesOnAsymmetricCubic)
{
  CubicFunc        aFunc;
  MathRoot::Config aConfig;
  aConfig.XTolerance    = 1.0e-14;
  aConfig.FTolerance    = 0.0;
  aConfig.MaxIterations = 100;

  const MathRoot::ScalarResult aResult = MathRoot::Brent(aFunc, 1.0, 2.0, aConfig);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Root.has_value());
  EXPECT_LT(aResult.NbIterations, 12u);
  EXPECT_NEAR(*aResult.Root, 1.5213797068045676, 1.0e-14);
}

TEST(MathRoot_BrentTest, ZeroFunctionToleranceConvergesPositionally)
{
  SqrtTwoFunc      aFunc;
  MathRoot::Config aConfig;
  aConfig.XTolerance    = 1.0e-14;
  aConfig.FTolerance    = 0.0;
  aConfig.MaxIterations = 100;

  const MathRoot::ScalarResult aResult = MathRoot::Brent(aFunc, 1.0, 2.0, aConfig);
  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Root.has_value());
  EXPECT_NEAR(*aResult.Root, std::sqrt(2.0), 2.0e-14);
}

TEST(MathRoot_BrentTest, CallbackRefusalDuringIterationIsPropagated)
{
  class RefusingFunc
  {
  public:
    bool Value(double theX, double& theF)
    {
      if (++myCalls == 3)
      {
        return false;
      }
      theF = theX * theX - 2.0;
      return true;
    }

  private:
    int myCalls = 0;
  } aFunc;

  const MathRoot::ScalarResult aResult = MathRoot::Brent(aFunc, 1.0, 2.0);
  EXPECT_EQ(aResult.Status, MathRoot::Status::CallbackError);
  EXPECT_EQ(aResult.NbIterations, 1u);
  EXPECT_FALSE(aResult.Root.has_value());
  EXPECT_FALSE(aResult.Value.has_value());
}

// ============================================================================
// Bisection method tests
// ============================================================================

TEST(MathRoot_BisectionTest, SqrtTwo)
{
  SqrtTwoFunc            aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Bisection(aFunc, 1.0, 2.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, std::sqrt(2.0), THE_TOLERANCE);
}

TEST(MathRoot_BisectionTest, CosMinusX)
{
  CosMinusXFunc          aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Bisection(aFunc, 0.0, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  double aFx = std::cos(*aResult.Root) - *aResult.Root;
  EXPECT_NEAR(aFx, 0.0, THE_TOLERANCE);
}

TEST(MathRoot_BisectionTest, SinPi)
{
  SinFunc                aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Bisection(aFunc, 2.0, 4.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, MathUtils::THE_PI, THE_TOLERANCE);
}

TEST(MathRoot_BisectionTest, InvalidBracket)
{
  SqrtTwoFunc aFunc;
  // No sign change
  MathRoot::ScalarResult aResult = MathRoot::Bisection(aFunc, 2.0, 3.0);
  EXPECT_EQ(aResult.Status, MathRoot::Status::InvalidInput);
}

// ============================================================================
// Bisection-Newton hybrid tests
// ============================================================================

TEST(MathRoot_BisectionNewtonTest, SqrtTwo)
{
  SqrtTwoFunc            aFunc;
  MathRoot::ScalarResult aResult = MathRoot::BisectionNewton(aFunc, 1.0, 2.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, std::sqrt(2.0), THE_TOLERANCE);
}

TEST(MathRoot_BisectionNewtonTest, CosMinusX)
{
  CosMinusXFunc          aFunc;
  MathRoot::ScalarResult aResult = MathRoot::BisectionNewton(aFunc, 0.0, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  double aFx = std::cos(*aResult.Root) - *aResult.Root;
  EXPECT_NEAR(aFx, 0.0, THE_TOLERANCE);
}

TEST(MathRoot_BisectionNewtonTest, LogLinearEquation)
{
  LogLinearFunc    aFunc;
  MathRoot::Config aConfig;
  aConfig.XTolerance = 1.0e-14;
  aConfig.FTolerance = 1.0e-14;

  const MathRoot::ScalarResult aResult =
    MathRoot::BisectionNewton(aFunc, 0.25, 4.0, aConfig);
  ASSERT_EQ(aResult.Status, MathRoot::Status::OK);
  ASSERT_TRUE(aResult.Root.has_value());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(*aResult.Root, 2.1234734225812275, 2.0e-13);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-13);
}

TEST(MathRoot_BisectionNewtonTest, NegativeQuinticRoot)
{
  QuinticFunc      aFunc;
  MathRoot::Config aConfig;
  aConfig.XTolerance = 1.0e-14;
  aConfig.FTolerance = 1.0e-14;

  const MathRoot::ScalarResult aResult =
    MathRoot::BisectionNewton(aFunc, -2.0, -0.5, aConfig);
  ASSERT_EQ(aResult.Status, MathRoot::Status::OK);
  ASSERT_TRUE(aResult.Root.has_value());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(*aResult.Root, -1.1329975658850653, 2.0e-13);
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-13);
}

TEST(MathRoot_BisectionNewtonTest, FasterThanPureBisection)
{
  SqrtTwoFunc      aFunc;
  MathRoot::Config aConfig;
  aConfig.MaxIterations = 100;

  MathRoot::ScalarResult aBisec  = MathRoot::Bisection(aFunc, 1.0, 2.0, aConfig);
  MathRoot::ScalarResult aHybrid = MathRoot::BisectionNewton(aFunc, 1.0, 2.0, aConfig);

  ASSERT_TRUE(aBisec.IsDone());
  ASSERT_TRUE(aHybrid.IsDone());

  // Hybrid should converge in fewer iterations
  EXPECT_LE(aHybrid.NbIterations, aBisec.NbIterations);
}

TEST(MathRoot_BisectionNewtonTest, StalledNewtonStepWithLargeResidualIsNotConverged)
{
  class StagnatingDerivativeFunc
  {
  public:
    bool Values(double theX, double& theF, double& theDf) const
    {
      theF  = theX - 0.25;
      theDf = 1.0e300;
      return true;
    }
  } aFunc;

  MathRoot::Config aConfig;
  aConfig.XTolerance    = 1.0e-12;
  aConfig.FTolerance    = 0.0;
  aConfig.MaxIterations = 1;
  const MathRoot::ScalarResult aResult = MathRoot::BisectionNewton(aFunc, 0.0, 2.0, aConfig);
  EXPECT_FALSE(aResult.IsDone());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_GT(std::abs(*aResult.Value), aConfig.FTolerance);
}

// ============================================================================
// Convergence and iteration tests
// ============================================================================

TEST(MathRoot_ConvergenceTest, NewtonIterationCount)
{
  SqrtTwoFunc            aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Newton(aFunc, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  // Newton should converge quickly (typically < 10 iterations)
  EXPECT_LT(aResult.NbIterations, 15);
}

TEST(MathRoot_ConvergenceTest, BrentIterationCount)
{
  SqrtTwoFunc            aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Brent(aFunc, 1.0, 2.0);
  ASSERT_TRUE(aResult.IsDone());
  // Brent should converge reasonably fast
  EXPECT_LT(aResult.NbIterations, 50);
}

// ============================================================================
// Boolean conversion tests
// ============================================================================

TEST(MathRoot_BoolConversionTest, SuccessfulResultIsTrue)
{
  SqrtTwoFunc            aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Newton(aFunc, 1.0);
  EXPECT_TRUE(static_cast<bool>(aResult));
}

TEST(MathRoot_BoolConversionTest, InvalidInputIsFalse)
{
  SqrtTwoFunc            aFunc;
  MathRoot::ScalarResult aResult = MathRoot::Brent(aFunc, 2.0, 3.0);
  EXPECT_FALSE(static_cast<bool>(aResult));
}

//=================================================================================================

TEST(MathRoot_CorrectnessTest, BracketedMethodsAcceptExactEndpoints)
{
  EndpointFunc aFunc;
  for (const MathRoot::ScalarResult& aResult : {MathRoot::Bisection(aFunc, 1.0, 3.0),
                                                MathRoot::BisectionNewton(aFunc, 1.0, 3.0),
                                                MathRoot::Brent(aFunc, 1.0, 3.0),
                                                MathRoot::NewtonBounded(aFunc, 2.0, 1.0, 3.0),
                                                MathRoot::Bisection(aFunc, 0.0, 1.0),
                                                MathRoot::BisectionNewton(aFunc, 0.0, 1.0),
                                                MathRoot::Brent(aFunc, 0.0, 1.0),
                                                MathRoot::NewtonBounded(aFunc, 0.5, 0.0, 1.0)})
  {
    ASSERT_TRUE(aResult.IsDone());
    ASSERT_TRUE(aResult.Root.has_value());
    ASSERT_TRUE(aResult.Value.has_value());
    EXPECT_DOUBLE_EQ(*aResult.Root, 1.0);
    EXPECT_DOUBLE_EQ(*aResult.Value, 0.0);
  }
}

TEST(MathRoot_CorrectnessTest, ReversedAndNonFiniteBoundsAreInvalid)
{
  SqrtTwoFunc aFunc;
  EXPECT_EQ(MathRoot::Bisection(aFunc, 2.0, 1.0).Status, MathRoot::Status::InvalidInput);
  EXPECT_EQ(MathRoot::BisectionNewton(aFunc, 2.0, 1.0).Status, MathRoot::Status::InvalidInput);
  EXPECT_EQ(MathRoot::Brent(aFunc, 2.0, 1.0).Status, MathRoot::Status::InvalidInput);
  EXPECT_EQ(MathRoot::NewtonBounded(aFunc, 1.5, 2.0, 1.0).Status, MathRoot::Status::InvalidInput);
  EXPECT_EQ(MathRoot::Brent(aFunc, 1.0, std::numeric_limits<double>::infinity()).Status,
            MathRoot::Status::InvalidInput);
}

TEST(MathRoot_CorrectnessTest, InvalidConfigurationIsRejected)
{
  SqrtTwoFunc      aFunc;
  MathRoot::Config aConfig;
  aConfig.MaxIterations = 0;
  EXPECT_EQ(MathRoot::Newton(aFunc, 1.0, aConfig).Status, MathRoot::Status::InvalidInput);
  aConfig.MaxIterations = 10;
  aConfig.FTolerance    = std::numeric_limits<double>::quiet_NaN();
  EXPECT_EQ(MathRoot::Secant(aFunc, 1.0, 2.0, aConfig).Status, MathRoot::Status::InvalidInput);
}

TEST(MathRoot_CorrectnessTest, NonFiniteCallbackOutputIsNumericalError)
{
  NonFiniteFunc aFunc;
  EXPECT_EQ(MathRoot::Bisection(aFunc, 0.0, 2.0).Status, MathRoot::Status::NumericalError);
  EXPECT_EQ(MathRoot::Newton(aFunc, 1.0).Status, MathRoot::Status::NumericalError);
}

TEST(MathRoot_CorrectnessTest, CallbackFailureMidSolveIsPropagated)
{
  FailingFunc aBisectionFunc;
  FailingFunc aNewtonFunc;
  EXPECT_EQ(MathRoot::Bisection(aBisectionFunc, 1.0, 2.0).Status, MathRoot::Status::CallbackError);
  EXPECT_EQ(MathRoot::Newton(aNewtonFunc, 1.0).Status, MathRoot::Status::CallbackError);
}

TEST(MathRoot_CorrectnessTest, BracketConvergenceDoesNotRequireExactIrrationalResidual)
{
  MathRoot::Config aConfig;
  aConfig.XTolerance    = 1.0e-12;
  aConfig.FTolerance    = 0.0;
  aConfig.MaxIterations = 100;

  SqrtTwoFunc aFunc;
  for (const MathRoot::ScalarResult& aResult : {MathRoot::Bisection(aFunc, 1.0, 2.0, aConfig),
                                                MathRoot::BisectionNewton(aFunc, 1.0, 2.0, aConfig),
                                                MathRoot::Brent(aFunc, 1.0, 2.0, aConfig)})
  {
    ASSERT_TRUE(aResult.IsDone());
    ASSERT_TRUE(aResult.Root.has_value());
    EXPECT_NEAR(*aResult.Root, std::sqrt(2.0), 2.0 * aConfig.XTolerance);
  }
}

TEST(MathRoot_CorrectnessTest, NewtonTinyStepStillRequiresAcceptableResidual)
{
  class LargeDerivativeFunc
  {
  public:
    bool Values(double, double& theF, double& theDf) const
    {
      theF  = 1.0;
      theDf = 1.0e300;
      return true;
    }
  } aFunc;

  MathRoot::Config aConfig;
  aConfig.XTolerance    = 1.0e-10;
  aConfig.FTolerance    = 0.0;
  aConfig.MaxIterations = 2;
  EXPECT_FALSE(MathRoot::Newton(aFunc, 1.0, aConfig).IsDone());
}

TEST(MathRoot_CorrectnessTest, CallbackRefusalAndNonFiniteOutputHaveDistinctStatuses)
{
  class RefusingFunc
  {
  public:
    bool Value(double, double&) const { return false; }

    bool Values(double, double&, double&) const { return false; }
  } aRefusingFunc;

  NonFiniteFunc aNonFiniteFunc;

  EXPECT_EQ(MathRoot::Brent(aRefusingFunc, 0.0, 1.0).Status, MathRoot::Status::CallbackError);
  EXPECT_EQ(MathRoot::Secant(aRefusingFunc, 0.0, 1.0).Status, MathRoot::Status::CallbackError);
  EXPECT_EQ(MathRoot::BisectionNewton(aRefusingFunc, 0.0, 1.0).Status,
            MathRoot::Status::CallbackError);
  EXPECT_EQ(MathRoot::Brent(aNonFiniteFunc, 0.0, 1.0).Status, MathRoot::Status::NumericalError);
  EXPECT_EQ(MathRoot::Secant(aNonFiniteFunc, 0.0, 1.0).Status,
            MathRoot::Status::NumericalError);
  EXPECT_EQ(MathRoot::BisectionNewton(aNonFiniteFunc, 0.0, 1.0).Status,
            MathRoot::Status::NumericalError);
}

TEST(MathRoot_CorrectnessTest, MaxIterationStateIsCoherent)
{
  SqrtTwoFunc      aFunc;
  MathRoot::Config aConfig;
  aConfig.MaxIterations = 1;
  aConfig.FTolerance    = 0.0;

  const MathRoot::ScalarResult aResult = MathRoot::Newton(aFunc, 1.0, aConfig);
  ASSERT_EQ(aResult.Status, MathRoot::Status::MaxIterations);
  ASSERT_TRUE(aResult.Root.has_value());
  ASSERT_TRUE(aResult.Value.has_value());
  ASSERT_TRUE(aResult.Derivative.has_value());
  EXPECT_DOUBLE_EQ(*aResult.Value, *aResult.Root * *aResult.Root - 2.0);
  EXPECT_DOUBLE_EQ(*aResult.Derivative, 2.0 * *aResult.Root);

  const MathRoot::ScalarResult aBracketed = MathRoot::Bisection(aFunc, 1.0, 2.0, aConfig);
  ASSERT_EQ(aBracketed.Status, MathRoot::Status::MaxIterations);
  ASSERT_TRUE(aBracketed.Root.has_value());
  ASSERT_TRUE(aBracketed.Value.has_value());
  EXPECT_DOUBLE_EQ(*aBracketed.Value, *aBracketed.Root * *aBracketed.Root - 2.0);

  const MathRoot::ScalarResult aBrent = MathRoot::Brent(aFunc, 1.0, 2.0, aConfig);
  ASSERT_EQ(aBrent.Status, MathRoot::Status::MaxIterations);
  EXPECT_EQ(aBrent.NbIterations, 1u);
  ASSERT_TRUE(aBrent.Root.has_value());
  ASSERT_TRUE(aBrent.Value.has_value());
  EXPECT_DOUBLE_EQ(*aBrent.Value, *aBrent.Root * *aBrent.Root - 2.0);
}

TEST(MathRoot_CorrectnessTest, BoundedNewtonDoesNotAcceptClampedBoundary)
{
  class OutsideRootFunc
  {
  public:
    bool Values(double theX, double& theF, double& theDf) const
    {
      theF  = theX + 1.0;
      theDf = 1.0;
      return true;
    }
  } aFunc;

  const MathRoot::ScalarResult aResult = MathRoot::NewtonBounded(aFunc, 0.5, 0.0, 2.0);
  EXPECT_FALSE(aResult.IsDone());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_GT(std::abs(*aResult.Value), 0.5);
}
