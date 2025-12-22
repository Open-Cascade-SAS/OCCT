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

#include <MathOpt_Brent.hxx>

#include <cmath>

namespace
{
constexpr double THE_TOLERANCE = 1.0e-7;
constexpr double THE_PI        = 3.14159265358979323846;

//! Parabola: f(x) = (x - 3)^2 + 1
//! Minimum at x = 3, f(3) = 1
class ParabolaFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = (theX - 3.0) * (theX - 3.0) + 1.0;
    return true;
  }
};

//! Parabola with negative minimum: f(x) = x^2 - 4x + 3
//! Minimum at x = 2, f(2) = -1
class ParabolaFunc2
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = theX * theX - 4.0 * theX + 3.0;
    return true;
  }
};

//! Cosine function: f(x) = cos(x)
//! Local minimum at x = PI
class CosFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::cos(theX);
    return true;
  }
};

//! Quartic: f(x) = x^4 - 2x^2
//! Local minima at x = -1 and x = 1
class QuarticFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    const double aX2 = theX * theX;
    theF             = aX2 * aX2 - 2.0 * aX2;
    return true;
  }
};

//! Exponential: f(x) = e^x + e^(-x)
//! Minimum at x = 0, f(0) = 2
class CoshLikeFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::exp(theX) + std::exp(-theX);
    return true;
  }
};

//! Rosenbrock-like 1D: f(x) = (1 - x)^2 + 100*(x^2 - x)^2
//! Minimum at x = 1
class Rosenbrock1DFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    const double a = 1.0 - theX;
    const double b = theX * theX - theX;
    theF           = a * a + 100.0 * b * b;
    return true;
  }
};

//! Absolute value shifted: f(x) = |x - 2| + 1
//! Minimum at x = 2, f(2) = 1 (non-smooth)
class AbsShiftedFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::abs(theX - 2.0) + 1.0;
    return true;
  }
};

//! Sinusoidal with noise: f(x) = sin(x) + 0.5*sin(3x)
//! Multiple local minima
class SinWithHarmonicFunc
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::sin(theX) + 0.5 * std::sin(3.0 * theX);
    return true;
  }
};
} // namespace

// ============================================================================
// Brent minimization tests
// ============================================================================

TEST(MathOpt_1D_BrentTest, SimpleParabola)
{
  ParabolaFunc          aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Brent(aFunc, 0.0, 10.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 3.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 1.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_BrentTest, ParabolaWithNegativeMinimum)
{
  ParabolaFunc2         aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Brent(aFunc, 0.0, 5.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 2.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, -1.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_BrentTest, CosineMinimum)
{
  CosFunc               aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Brent(aFunc, 2.0, 4.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, THE_PI, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, -1.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_BrentTest, QuarticMinimumRight)
{
  QuarticFunc aFunc;
  // Search for right minimum at x = 1
  MathOpt::ScalarResult aResult = MathOpt::Brent(aFunc, 0.0, 2.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 1.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, -1.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_BrentTest, QuarticMinimumLeft)
{
  QuarticFunc aFunc;
  // Search for left minimum at x = -1
  MathOpt::ScalarResult aResult = MathOpt::Brent(aFunc, -2.0, 0.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, -1.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, -1.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_BrentTest, CoshLikeMinimum)
{
  CoshLikeFunc          aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Brent(aFunc, -5.0, 5.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 0.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 2.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_BrentTest, NarrowInterval)
{
  ParabolaFunc          aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Brent(aFunc, 2.9, 3.1);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 3.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_BrentTest, WideInterval)
{
  ParabolaFunc          aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Brent(aFunc, -100.0, 100.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 3.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_BrentTest, CustomTolerance)
{
  ParabolaFunc    aFunc;
  MathOpt::Config aConfig;
  aConfig.XTolerance    = 1.0e-10;
  aConfig.MaxIterations = 200;

  MathOpt::ScalarResult aResult = MathOpt::Brent(aFunc, 0.0, 10.0, aConfig);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 3.0, 1.0e-9);
}

TEST(MathOpt_1D_BrentTest, NonSmoothFunction)
{
  AbsShiftedFunc        aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Brent(aFunc, 0.0, 5.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 2.0, 1.0e-6); // Lower tolerance for non-smooth
  EXPECT_NEAR(*aResult.Value, 1.0, 1.0e-6);
}

// ============================================================================
// Golden section tests
// ============================================================================

TEST(MathOpt_1D_GoldenTest, SimpleParabola)
{
  ParabolaFunc          aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Golden(aFunc, 0.0, 10.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 3.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 1.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_GoldenTest, CosineMinimum)
{
  CosFunc               aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Golden(aFunc, 2.0, 4.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, THE_PI, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, -1.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_GoldenTest, QuarticMinimum)
{
  QuarticFunc           aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Golden(aFunc, 0.0, 2.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 1.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, -1.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_GoldenTest, CoshLikeMinimum)
{
  CoshLikeFunc          aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Golden(aFunc, -5.0, 5.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 0.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 2.0, THE_TOLERANCE);
}

// ============================================================================
// Brent with automatic bracket tests
// ============================================================================

TEST(MathOpt_1D_BrentWithBracketTest, SimpleParabola)
{
  ParabolaFunc          aFunc;
  MathOpt::ScalarResult aResult = MathOpt::BrentWithBracket(aFunc, 0.0, 1.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 3.0, THE_TOLERANCE);
  EXPECT_NEAR(*aResult.Value, 1.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_BrentWithBracketTest, StartNearMinimum)
{
  ParabolaFunc          aFunc;
  MathOpt::ScalarResult aResult = MathOpt::BrentWithBracket(aFunc, 2.5, 0.5);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 3.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_BrentWithBracketTest, CoshLike)
{
  CoshLikeFunc aFunc;
  // Start from a point closer to the minimum with a small step
  // The algorithm expands to find a bracket containing the minimum
  MathOpt::ScalarResult aResult = MathOpt::BrentWithBracket(aFunc, 0.1, 0.2);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 0.0, THE_TOLERANCE);
}

// ============================================================================
// Comparison tests
// ============================================================================

TEST(MathOpt_1D_ComparisonTest, BrentVsGoldenSameResult)
{
  ParabolaFunc aFunc;

  MathOpt::ScalarResult aBrent  = MathOpt::Brent(aFunc, 0.0, 10.0);
  MathOpt::ScalarResult aGolden = MathOpt::Golden(aFunc, 0.0, 10.0);

  ASSERT_TRUE(aBrent.IsDone());
  ASSERT_TRUE(aGolden.IsDone());

  EXPECT_NEAR(*aBrent.Root, *aGolden.Root, THE_TOLERANCE);
  EXPECT_NEAR(*aBrent.Value, *aGolden.Value, THE_TOLERANCE);
}

TEST(MathOpt_1D_ComparisonTest, BrentFasterThanGolden)
{
  ParabolaFunc    aFunc;
  MathOpt::Config aConfig;
  aConfig.MaxIterations = 200;

  MathOpt::ScalarResult aBrent  = MathOpt::Brent(aFunc, 0.0, 10.0, aConfig);
  MathOpt::ScalarResult aGolden = MathOpt::Golden(aFunc, 0.0, 10.0, aConfig);

  ASSERT_TRUE(aBrent.IsDone());
  ASSERT_TRUE(aGolden.IsDone());

  // Brent typically converges faster due to parabolic interpolation
  EXPECT_LE(aBrent.NbIterations, aGolden.NbIterations);
}

// ============================================================================
// Edge cases and robustness tests
// ============================================================================

TEST(MathOpt_1D_RobustnessTest, MinimumAtLeftBoundary)
{
  // f(x) = x^2 on [0, 10] - minimum at x = 0
  class SquareFunc
  {
  public:
    bool Value(double theX, double& theF) const
    {
      theF = theX * theX;
      return true;
    }
  };

  SquareFunc            aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Brent(aFunc, 0.0, 10.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 0.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_RobustnessTest, MinimumAtRightBoundary)
{
  // f(x) = (10 - x)^2 on [0, 10] - minimum at x = 10
  class ShiftedSquareFunc
  {
  public:
    bool Value(double theX, double& theF) const
    {
      theF = (10.0 - theX) * (10.0 - theX);
      return true;
    }
  };

  ShiftedSquareFunc     aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Brent(aFunc, 0.0, 10.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 10.0, THE_TOLERANCE);
}

TEST(MathOpt_1D_RobustnessTest, VeryFlatFunction)
{
  // f(x) = (x - 5)^4 - very flat near minimum
  class FlatQuarticFunc
  {
  public:
    bool Value(double theX, double& theF) const
    {
      const double aDiff = theX - 5.0;
      theF               = aDiff * aDiff * aDiff * aDiff;
      return true;
    }
  };

  FlatQuarticFunc       aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Brent(aFunc, 0.0, 10.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(*aResult.Root, 5.0, 1.0e-5); // Lower tolerance for flat function
}

// ============================================================================
// Boolean conversion tests
// ============================================================================

TEST(MathOpt_1D_BoolConversionTest, SuccessfulResultIsTrue)
{
  ParabolaFunc          aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Brent(aFunc, 0.0, 10.0);
  EXPECT_TRUE(static_cast<bool>(aResult));
}

// ============================================================================
// Iteration count tests
// ============================================================================

TEST(MathOpt_1D_IterationTest, BrentConvergesQuickly)
{
  ParabolaFunc          aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Brent(aFunc, 0.0, 10.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_LT(aResult.NbIterations, 50);
}

TEST(MathOpt_1D_IterationTest, GoldenConverges)
{
  ParabolaFunc          aFunc;
  MathOpt::ScalarResult aResult = MathOpt::Golden(aFunc, 0.0, 10.0);
  ASSERT_TRUE(aResult.IsDone());
  EXPECT_LT(aResult.NbIterations, 100);
}
