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

#include <MathOpt_Powell.hxx>
#include <MathOpt_BFGS.hxx>

#include <cmath>

//=============================================================================
// Test functions for N-dimensional optimization
//=============================================================================

// Rosenbrock function: f(x,y) = (1-x)^2 + 100(y-x^2)^2
// Minimum at (1, 1) with f = 0
class RosenbrockFunction
{
public:
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX = theX(1);
    const double aY = theX(2);
    theF            = (1.0 - aX) * (1.0 - aX) + 100.0 * (aY - aX * aX) * (aY - aX * aX);
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    const double aX = theX(1);
    const double aY = theX(2);
    theGrad(1)      = -2.0 * (1.0 - aX) - 400.0 * aX * (aY - aX * aX);
    theGrad(2)      = 200.0 * (aY - aX * aX);
    return true;
  }
};

// Simple quadratic: f(x,y) = x^2 + y^2
// Minimum at (0, 0) with f = 0
class SimpleQuadratic
{
public:
  bool Value(const math_Vector& theX, double& theF)
  {
    theF = 0.0;
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theF += theX(i) * theX(i);
    }
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theGrad(i) = 2.0 * theX(i);
    }
    return true;
  }
};

// Sphere function in N dimensions: f(x) = sum(x_i^2)
// Minimum at origin with f = 0
class SphereFunction
{
public:
  bool Value(const math_Vector& theX, double& theF)
  {
    theF = 0.0;
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theF += theX(i) * theX(i);
    }
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    for (int i = theX.Lower(); i <= theX.Upper(); ++i)
    {
      theGrad(i) = 2.0 * theX(i);
    }
    return true;
  }
};

// Booth function: f(x,y) = (x + 2y - 7)^2 + (2x + y - 5)^2
// Minimum at (1, 3) with f = 0
class BoothFunction
{
public:
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aX + 2.0 * aY - 7.0;
    const double aT2 = 2.0 * aX + aY - 5.0;
    theF             = aT1 * aT1 + aT2 * aT2;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = aX + 2.0 * aY - 7.0;
    const double aT2 = 2.0 * aX + aY - 5.0;
    theGrad(1)       = 2.0 * aT1 + 4.0 * aT2;
    theGrad(2)       = 4.0 * aT1 + 2.0 * aT2;
    return true;
  }
};

// Beale's function: f(x,y) = (1.5 - x + xy)^2 + (2.25 - x + xy^2)^2 + (2.625 - x + xy^3)^2
// Minimum at (3, 0.5) with f = 0
class BealeFunction
{
public:
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aT1 = 1.5 - aX + aX * aY;
    const double aT2 = 2.25 - aX + aX * aY * aY;
    const double aT3 = 2.625 - aX + aX * aY * aY * aY;
    theF             = aT1 * aT1 + aT2 * aT2 + aT3 * aT3;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    const double aX  = theX(1);
    const double aY  = theX(2);
    const double aY2 = aY * aY;
    const double aY3 = aY2 * aY;
    const double aT1 = 1.5 - aX + aX * aY;
    const double aT2 = 2.25 - aX + aX * aY2;
    const double aT3 = 2.625 - aX + aX * aY3;
    theGrad(1) = 2.0 * aT1 * (-1.0 + aY) + 2.0 * aT2 * (-1.0 + aY2) + 2.0 * aT3 * (-1.0 + aY3);
    theGrad(2) = 2.0 * aT1 * aX + 2.0 * aT2 * (2.0 * aX * aY) + 2.0 * aT3 * (3.0 * aX * aY2);
    return true;
  }
};

//=============================================================================
// Powell's Method Tests
//=============================================================================

TEST(MathOpt_PowellTest, SimpleQuadratic)
{
  SimpleQuadratic aFunc;
  math_Vector     aStart(1, 2);
  aStart(1) = 5.0;
  aStart(2) = 5.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 200;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;

  auto aResult = MathOpt::Powell(aFunc, aStart, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-6);
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, 1.0e-4);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, 1.0e-4);
}

TEST(MathOpt_PowellTest, BoothFunction)
{
  BoothFunction aFunc;
  math_Vector   aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 200;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;

  auto aResult = MathOpt::Powell(aFunc, aStart, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-6);
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 1.0e-4);
  EXPECT_NEAR((*aResult.Solution)(2), 3.0, 1.0e-4);
}

TEST(MathOpt_PowellTest, SphereFunction3D)
{
  SphereFunction aFunc;
  math_Vector    aStart(1, 3);
  aStart(1) = 3.0;
  aStart(2) = -2.0;
  aStart(3) = 4.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 200;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;

  auto aResult = MathOpt::Powell(aFunc, aStart, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-6);
  for (int i = 1; i <= 3; ++i)
  {
    EXPECT_NEAR((*aResult.Solution)(i), 0.0, 1.0e-4);
  }
}

//=============================================================================
// BFGS Tests
//=============================================================================

TEST(MathOpt_BFGSTest, SimpleQuadratic)
{
  SimpleQuadratic aFunc;
  math_Vector     aStart(1, 2);
  aStart(1) = 5.0;
  aStart(2) = 5.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-10;
  aConfig.FTolerance    = 1.0e-12;

  auto aResult = MathOpt::BFGS(aFunc, aStart, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-8);
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, 1.0e-6);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, 1.0e-6);
  EXPECT_LT(aResult.NbIterations, 20); // BFGS should converge quickly on quadratics
}

TEST(MathOpt_BFGSTest, BoothFunction)
{
  BoothFunction aFunc;
  math_Vector   aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-10;
  aConfig.FTolerance    = 1.0e-12;

  auto aResult = MathOpt::BFGS(aFunc, aStart, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-8);
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 1.0e-6);
  EXPECT_NEAR((*aResult.Solution)(2), 3.0, 1.0e-6);
}

TEST(MathOpt_BFGSTest, RosenbrockFunction)
{
  RosenbrockFunction aFunc;
  math_Vector        aStart(1, 2);
  aStart(1) = -1.0;
  aStart(2) = 1.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 500;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;

  auto aResult = MathOpt::BFGS(aFunc, aStart, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-4);
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 1.0e-3);
  EXPECT_NEAR((*aResult.Solution)(2), 1.0, 1.0e-3);
}

TEST(MathOpt_BFGSTest, BealeFunction)
{
  BealeFunction aFunc;
  math_Vector   aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 200;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;

  auto aResult = MathOpt::BFGS(aFunc, aStart, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-4);
  EXPECT_NEAR((*aResult.Solution)(1), 3.0, 1.0e-3);
  EXPECT_NEAR((*aResult.Solution)(2), 0.5, 1.0e-3);
}

TEST(MathOpt_BFGSTest, SphereFunction5D)
{
  SphereFunction aFunc;
  math_Vector    aStart(1, 5);
  for (int i = 1; i <= 5; ++i)
  {
    aStart(i) = static_cast<double>(i);
  }

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-10;
  aConfig.FTolerance    = 1.0e-12;

  auto aResult = MathOpt::BFGS(aFunc, aStart, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-8);
  for (int i = 1; i <= 5; ++i)
  {
    EXPECT_NEAR((*aResult.Solution)(i), 0.0, 1.0e-6);
  }
  EXPECT_LT(aResult.NbIterations, 20); // Should converge quickly on quadratics
}

//=============================================================================
// BFGS Numerical Gradient Tests
//=============================================================================

TEST(MathOpt_BFGSNumericalTest, SimpleQuadratic)
{
  // Same test but using numerical gradient
  class QuadraticNoGrad
  {
  public:
    bool Value(const math_Vector& theX, double& theF)
    {
      theF = 0.0;
      for (int i = theX.Lower(); i <= theX.Upper(); ++i)
      {
        theF += theX(i) * theX(i);
      }
      return true;
    }
  };

  QuadraticNoGrad aFunc;
  math_Vector     aStart(1, 2);
  aStart(1) = 5.0;
  aStart(2) = 5.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;

  auto aResult = MathOpt::BFGSNumerical(aFunc, aStart, 1.0e-7, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-6);
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, 1.0e-4);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, 1.0e-4);
}

//=============================================================================
// L-BFGS Tests
//=============================================================================

TEST(MathOpt_LBFGSTest, SimpleQuadratic)
{
  SimpleQuadratic aFunc;
  math_Vector     aStart(1, 2);
  aStart(1) = 5.0;
  aStart(2) = 5.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-10;
  aConfig.FTolerance    = 1.0e-12;

  auto aResult = MathOpt::LBFGS(aFunc, aStart, 10, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-8);
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, 1.0e-6);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, 1.0e-6);
}

TEST(MathOpt_LBFGSTest, BoothFunction)
{
  BoothFunction aFunc;
  math_Vector   aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-10;
  aConfig.FTolerance    = 1.0e-12;

  auto aResult = MathOpt::LBFGS(aFunc, aStart, 10, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-8);
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 1.0e-6);
  EXPECT_NEAR((*aResult.Solution)(2), 3.0, 1.0e-6);
}

TEST(MathOpt_LBFGSTest, SphereFunction10D)
{
  SphereFunction aFunc;
  math_Vector    aStart(1, 10);
  for (int i = 1; i <= 10; ++i)
  {
    aStart(i) = static_cast<double>(i);
  }

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-10;
  aConfig.FTolerance    = 1.0e-12;

  // Use small memory size for L-BFGS
  auto aResult = MathOpt::LBFGS(aFunc, aStart, 5, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-8);
  for (int i = 1; i <= 10; ++i)
  {
    EXPECT_NEAR((*aResult.Solution)(i), 0.0, 1.0e-6);
  }
}

//=============================================================================
// Comparison Tests: Powell vs BFGS
//=============================================================================

TEST(MathOpt_ComparisonTest, BFGSFasterThanPowellOnQuadratic)
{
  SimpleQuadratic aFunc;
  math_Vector     aStart(1, 2);
  aStart(1) = 10.0;
  aStart(2) = 10.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 500;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;

  auto aPowellResult = MathOpt::Powell(aFunc, aStart, aConfig);
  auto aBFGSResult   = MathOpt::BFGS(aFunc, aStart, aConfig);

  EXPECT_TRUE(aPowellResult.IsDone());
  EXPECT_TRUE(aBFGSResult.IsDone());

  // Both should find the same minimum
  EXPECT_NEAR(*aPowellResult.Value, *aBFGSResult.Value, 1.0e-4);

  // BFGS should use fewer iterations on quadratic functions
  EXPECT_LT(aBFGSResult.NbIterations, aPowellResult.NbIterations);
}
