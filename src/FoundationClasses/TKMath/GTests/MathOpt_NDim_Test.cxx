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
#include <limits>

//=============================================================================
// Test functions for N-dimensional optimization
//=============================================================================

// Rosenbrock function: f(x,y) = (1-x)^2 + 100(y-x^2)^2
// Minimum at (1, 1) with f = 0
class MathOptTest_RosenbrockFunction
{
public:
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    theF            = (1.0 - aX) * (1.0 - aX) + 100.0 * (aY - aX * aX) * (aY - aX * aX);
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    const double aX     = theX.At(0);
    const double aY     = theX.At(1);
    theGrad.ChangeAt(0) = -2.0 * (1.0 - aX) - 400.0 * aX * (aY - aX * aX);
    theGrad.ChangeAt(1) = 200.0 * (aY - aX * aX);
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
    for (size_t i = 0; i < theX.Size(); ++i)
    {
      theF += theX.At(i) * theX.At(i);
    }
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    for (size_t i = 0; i < theX.Size(); ++i)
    {
      theGrad.ChangeAt(i) = 2.0 * theX.At(i);
    }
    return true;
  }
};

//! Saddle with descent only along mixed coordinate directions at the origin.
struct BilinearSaddle
{
  bool Value(const math_Vector& theX, double& theF)
  {
    theF = theX.At(0) * theX.At(1);
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
    for (size_t i = 0; i < theX.Size(); ++i)
    {
      theF += theX.At(i) * theX.At(i);
    }
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    for (size_t i = 0; i < theX.Size(); ++i)
    {
      theGrad.ChangeAt(i) = 2.0 * theX.At(i);
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
    const double aX  = theX.At(0);
    const double aY  = theX.At(1);
    const double aT1 = aX + 2.0 * aY - 7.0;
    const double aT2 = 2.0 * aX + aY - 5.0;
    theF             = aT1 * aT1 + aT2 * aT2;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    const double aX     = theX.At(0);
    const double aY     = theX.At(1);
    const double aT1    = aX + 2.0 * aY - 7.0;
    const double aT2    = 2.0 * aX + aY - 5.0;
    theGrad.ChangeAt(0) = 2.0 * aT1 + 4.0 * aT2;
    theGrad.ChangeAt(1) = 4.0 * aT1 + 2.0 * aT2;
    return true;
  }
};

class RotatedQuadraticFunction
{
public:
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    theF = 3.0 * aX * aX + 2.0 * aX * aY + 2.0 * aY * aY - 7.0 * aX + aY + 5.0;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGradient)
  {
    theGradient.ChangeAt(0) = 6.0 * theX.At(0) + 2.0 * theX.At(1) - 7.0;
    theGradient.ChangeAt(1) = 2.0 * theX.At(0) + 4.0 * theX.At(1) + 1.0;
    return true;
  }
};

class LineSearchFailureFunction
{
public:
  explicit LineSearchFailureFunction(bool theReturnNonFinite = false)
      : myReturnNonFinite(theReturnNonFinite)
  {
  }

  bool Value(const math_Vector& theX, double& theF)
  {
    if (++myNbValueCalls > 1)
    {
      if (!myReturnNonFinite)
      {
        return false;
      }
      theF = std::numeric_limits<double>::quiet_NaN();
      return true;
    }
    theF = theX.At(0) * theX.At(0);
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGradient)
  {
    theGradient.ChangeAt(0) = 2.0 * theX.At(0);
    return true;
  }

private:
  bool myReturnNonFinite = false;
  int  myNbValueCalls    = 0;
};

struct GradientCallbackFailureFunction
{
  bool Value(const math_Vector& theX, double& theF)
  {
    theF = theX.At(0) * theX.At(0);
    return true;
  }

  bool Gradient(const math_Vector&, math_Vector&) { return false; }
};

struct ValueCallbackFailureFunction
{
  bool Value(const math_Vector&, double&) { return false; }
};

// Beale's function: f(x,y) = (1.5 - x + xy)^2 + (2.25 - x + xy^2)^2 + (2.625 - x + xy^3)^2
// Minimum at (3, 0.5) with f = 0
class BealeFunction
{
public:
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX  = theX.At(0);
    const double aY  = theX.At(1);
    const double aT1 = 1.5 - aX + aX * aY;
    const double aT2 = 2.25 - aX + aX * aY * aY;
    const double aT3 = 2.625 - aX + aX * aY * aY * aY;
    theF             = aT1 * aT1 + aT2 * aT2 + aT3 * aT3;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGrad)
  {
    const double aX  = theX.At(0);
    const double aY  = theX.At(1);
    const double aY2 = aY * aY;
    const double aY3 = aY2 * aY;
    const double aT1 = 1.5 - aX + aX * aY;
    const double aT2 = 2.25 - aX + aX * aY2;
    const double aT3 = 2.625 - aX + aX * aY3;
    theGrad.ChangeAt(0) =
      2.0 * aT1 * (-1.0 + aY) + 2.0 * aT2 * (-1.0 + aY2) + 2.0 * aT3 * (-1.0 + aY3);
    theGrad.ChangeAt(1) =
      2.0 * aT1 * aX + 2.0 * aT2 * (2.0 * aX * aY) + 2.0 * aT3 * (3.0 * aX * aY2);
    return true;
  }
};

class MathOptTest_RotatedQuadratic2DFunction
{
public:
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX = theX.At(0);
    const double aY = theX.At(1);
    theF            = 5.0 * aX * aX + 4.0 * aX * aY + 2.0 * aY * aY - 12.0 * aX + 2.0 * aY + 7.0;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGradient)
  {
    theGradient.ChangeAt(0) = 10.0 * theX.At(0) + 4.0 * theX.At(1) - 12.0;
    theGradient.ChangeAt(1) = 4.0 * theX.At(0) + 4.0 * theX.At(1) + 2.0;
    return true;
  }
};

class MathOptTest_RotatedQuadratic3DFunction
{
public:
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aR1 = theX.At(0) + theX.At(1) - 1.0;
    const double aR2 = theX.At(1) + theX.At(2) + 2.0;
    const double aR3 = theX.At(0) - 2.0 * theX.At(2) - 4.0;
    theF             = aR1 * aR1 + 2.0 * aR2 * aR2 + 3.0 * aR3 * aR3 + 0.75;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGradient)
  {
    const double aR1        = theX.At(0) + theX.At(1) - 1.0;
    const double aR2        = theX.At(1) + theX.At(2) + 2.0;
    const double aR3        = theX.At(0) - 2.0 * theX.At(2) - 4.0;
    theGradient.ChangeAt(0) = 2.0 * aR1 + 6.0 * aR3;
    theGradient.ChangeAt(1) = 2.0 * aR1 + 4.0 * aR2;
    theGradient.ChangeAt(2) = 4.0 * aR2 - 12.0 * aR3;
    return true;
  }
};

class MathOptTest_WeightedShiftedSphereFunction
{
public:
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aD0 = theX.At(0) - 1.25;
    const double aD1 = theX.At(1) + 2.5;
    const double aD2 = theX.At(2) - 0.75;
    const double aD3 = theX.At(3) + 1.0;
    theF             = 0.5 * aD0 * aD0 + 3.0 * aD1 * aD1 + 7.0 * aD2 * aD2 + 2.0 * aD3 * aD3 + 4.5;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGradient)
  {
    theGradient.ChangeAt(0) = theX.At(0) - 1.25;
    theGradient.ChangeAt(1) = 6.0 * (theX.At(1) + 2.5);
    theGradient.ChangeAt(2) = 14.0 * (theX.At(2) - 0.75);
    theGradient.ChangeAt(3) = 4.0 * (theX.At(3) + 1.0);
    return true;
  }
};

class MathOptTest_SeparableQuarticFunction
{
public:
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aX  = theX.At(0);
    const double aY  = theX.At(1);
    const double aX2 = aX * aX;
    const double aY2 = aY * aY;
    theF             = aX2 * aX2 + 2.0 * aX2 - 3.0 * aX + 0.5 * aY2 * aY2 + aY2 + 2.0 * aY + 6.0;
    return true;
  }
};

class MathOptTest_ExponentialQuadraticFunction
{
public:
  bool Value(const math_Vector& theX, double& theF)
  {
    const double anExp = std::exp(theX.At(0) + theX.At(1));
    const double aDx   = theX.At(0) - 1.0;
    const double aDy   = theX.At(1) + 1.0;
    theF               = anExp + aDx * aDx + 2.0 * aDy * aDy;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGradient)
  {
    const double anExp      = std::exp(theX.At(0) + theX.At(1));
    theGradient.ChangeAt(0) = anExp + 2.0 * (theX.At(0) - 1.0);
    theGradient.ChangeAt(1) = anExp + 4.0 * (theX.At(1) + 1.0);
    return true;
  }
};

class MathOptTest_ActiveBoundaryFunction
{
public:
  bool Value(const math_Vector& theX, double& theF)
  {
    const double aR1 = theX.At(0) + theX.At(1) - 3.0;
    const double aR2 = theX.At(0) - 2.0 * theX.At(1) + 1.0;
    theF             = aR1 * aR1 + 2.0 * aR2 * aR2 + 0.25;
    return true;
  }

  bool Gradient(const math_Vector& theX, math_Vector& theGradient)
  {
    const double aR1        = theX.At(0) + theX.At(1) - 3.0;
    const double aR2        = theX.At(0) - 2.0 * theX.At(1) + 1.0;
    theGradient.ChangeAt(0) = 2.0 * aR1 + 4.0 * aR2;
    theGradient.ChangeAt(1) = 2.0 * aR1 - 8.0 * aR2;
    return true;
  }
};

//=============================================================================
// Practical convex optimization tests
//=============================================================================

TEST(MathOpt_BFGSTest, RotatedQuadratic2DGeneralMinimum)
{
  MathOptTest_RotatedQuadratic2DFunction aFunc;
  math_Vector                            aStart(size_t{2}, 4.0);
  MathOpt::Config                        aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-12;
  aConfig.FTolerance    = 1.0e-12;

  const MathUtils::VectorResult aResult = MathOpt::BFGS(aFunc, aStart, aConfig);

  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_TRUE(aResult.Solution.has_value());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(aResult.Solution->At(0), 2.333333333333333, 1.0e-9);
  EXPECT_NEAR(aResult.Solution->At(1), -2.833333333333333, 1.0e-9);
  EXPECT_NEAR(*aResult.Value, -9.833333333333332, 1.0e-10);
}

TEST(MathOpt_BFGSTest, RotatedQuadratic3DLeastSquares)
{
  MathOptTest_RotatedQuadratic3DFunction aFunc;
  math_Vector                            aStart(3);
  aStart.ChangeAt(0) = -3.0;
  aStart.ChangeAt(1) = 4.0;
  aStart.ChangeAt(2) = 2.0;
  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-12;
  aConfig.FTolerance    = 1.0e-12;

  const MathUtils::VectorResult aResult = MathOpt::BFGS(aFunc, aStart, aConfig);

  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_TRUE(aResult.Solution.has_value());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(aResult.Solution->At(0), 2.0, 1.0e-9);
  EXPECT_NEAR(aResult.Solution->At(1), -1.0, 1.0e-9);
  EXPECT_NEAR(aResult.Solution->At(2), -1.0, 1.0e-9);
  EXPECT_NEAR(*aResult.Value, 0.75, 1.0e-10);
}

TEST(MathOpt_LBFGSTest, WeightedShiftedSphere4D)
{
  MathOptTest_WeightedShiftedSphereFunction aFunc;
  math_Vector                               aStart(4);
  aStart.ChangeAt(0) = -4.0;
  aStart.ChangeAt(1) = 3.0;
  aStart.ChangeAt(2) = -2.0;
  aStart.ChangeAt(3) = 5.0;
  MathOpt::Config aConfig;
  aConfig.MaxIterations = 150;
  aConfig.XTolerance    = 1.0e-11;
  aConfig.FTolerance    = 1.0e-11;

  const MathUtils::VectorResult aResult = MathOpt::LBFGS(aFunc, aStart, 6, aConfig);

  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_TRUE(aResult.Solution.has_value());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(aResult.Solution->At(0), 1.25, 1.0e-7);
  EXPECT_NEAR(aResult.Solution->At(1), -2.5, 1.0e-7);
  EXPECT_NEAR(aResult.Solution->At(2), 0.75, 1.0e-7);
  EXPECT_NEAR(aResult.Solution->At(3), -1.0, 1.0e-7);
  EXPECT_NEAR(*aResult.Value, 4.5, 1.0e-10);
}

TEST(MathOpt_PowellTest, SeparableQuarticNonzeroMinimum)
{
  MathOptTest_SeparableQuarticFunction aFunc;
  math_Vector                          aStart(2);
  aStart.ChangeAt(0) = -1.5;
  aStart.ChangeAt(1) = 1.5;
  MathOpt::Config aConfig;
  aConfig.MaxIterations = 200;
  aConfig.XTolerance    = 1.0e-10;
  aConfig.FTolerance    = 1.0e-12;

  const MathUtils::VectorResult aResult = MathOpt::Powell(aFunc, aStart, aConfig);

  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_TRUE(aResult.Solution.has_value());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(aResult.Solution->At(0), 0.5673642266809229, 2.0e-6);
  EXPECT_NEAR(aResult.Solution->At(1), -0.6823278038280193, 2.0e-6);
  EXPECT_NEAR(*aResult.Value, 4.254626565881520, 1.0e-9);
}

TEST(MathOpt_BFGSTest, ExponentialQuadraticConvexMinimum)
{
  MathOptTest_ExponentialQuadraticFunction aFunc;
  math_Vector                              aStart(2);
  aStart.ChangeAt(0) = -2.0;
  aStart.ChangeAt(1) = 2.0;
  MathOpt::Config aConfig;
  aConfig.MaxIterations = 150;
  aConfig.XTolerance    = 1.0e-11;
  aConfig.FTolerance    = 1.0e-11;

  const MathUtils::VectorResult aResult = MathOpt::BFGS(aFunc, aStart, aConfig);

  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_TRUE(aResult.Solution.has_value());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(aResult.Solution->At(0), 0.6872331928700078, 1.0e-7);
  EXPECT_NEAR(aResult.Solution->At(1), -1.156383403564996, 1.0e-7);
  EXPECT_NEAR(*aResult.Value, 0.7722682277234190, 1.0e-10);
}

TEST(MathOpt_BFGSBoundedTest, CoupledQuadraticActiveUpperBoundary)
{
  MathOptTest_ActiveBoundaryFunction aFunc;
  math_Vector                        aStart(size_t{2}, 0.5);
  math_Vector                        aLower(2);
  math_Vector                        anUpper(2);
  aLower.ChangeAt(0)  = -1.0;
  aLower.ChangeAt(1)  = 0.0;
  anUpper.ChangeAt(0) = 1.0;
  anUpper.ChangeAt(1) = 2.0;
  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-11;
  aConfig.FTolerance    = 1.0e-11;

  const MathUtils::VectorResult aResult =
    MathOpt::BFGSBounded(aFunc, aStart, aLower, anUpper, aConfig);

  ASSERT_EQ(aResult.Status, MathUtils::Status::OK);
  ASSERT_TRUE(aResult.Solution.has_value());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, 1.0e-10);
  EXPECT_NEAR(aResult.Solution->At(1), 1.111111111111111, 1.0e-7);
  EXPECT_NEAR(*aResult.Value, 1.138888888888889, 1.0e-10);
}

//=============================================================================
// Powell's Method Tests
//=============================================================================

TEST(MathOpt_PowellTest, SimpleQuadratic)
{
  SimpleQuadratic aFunc;
  math_Vector     aStart(2);
  aStart.ChangeAt(0) = 5.0;
  aStart.ChangeAt(1) = 5.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 200;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;

  auto aResult = MathOpt::Powell(aFunc, aStart, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-6);
  EXPECT_NEAR(aResult.Solution->At(0), 0.0, 1.0e-4);
  EXPECT_NEAR(aResult.Solution->At(1), 0.0, 1.0e-4);
}

TEST(MathOpt_PowellTest, BoothFunction)
{
  BoothFunction aFunc;
  math_Vector   aStart(2);
  aStart.ChangeAt(0) = 0.0;
  aStart.ChangeAt(1) = 0.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 200;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;

  auto aResult = MathOpt::Powell(aFunc, aStart, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-6);
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, 1.0e-4);
  EXPECT_NEAR(aResult.Solution->At(1), 3.0, 1.0e-4);
}

TEST(MathOpt_PowellTest, SphereFunction3D)
{
  SphereFunction aFunc;
  math_Vector    aStart(3);
  aStart.ChangeAt(0) = 3.0;
  aStart.ChangeAt(1) = -2.0;
  aStart.ChangeAt(2) = 4.0;

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
    EXPECT_NEAR(aResult.Solution->At(static_cast<size_t>(i - 1)), 0.0, 1.0e-4);
  }
}

TEST(MathOpt_PowellTest, MixedDirectionSaddleIsNotConverged)
{
  BilinearSaddle aFunc;
  math_Vector    aStart(size_t{2}, 0.0);

  const MathUtils::VectorResult aResult = MathOpt::Powell(aFunc, aStart);

  EXPECT_EQ(aResult.Status, MathUtils::Status::MaxIterations);
  EXPECT_FALSE(aResult.IsDone());

  math_Matrix aDirections(size_t{2}, size_t{2}, 0.0);
  aDirections.ChangeAt(0, 0) = 1.0;
  aDirections.ChangeAt(1, 1) = 1.0;
  const MathUtils::VectorResult aCustomResult =
    MathOpt::PowellWithDirections(aFunc, aStart, aDirections);
  EXPECT_EQ(aCustomResult.Status, MathUtils::Status::MaxIterations);
  EXPECT_FALSE(aCustomResult.IsDone());
}

//=============================================================================
// BFGS Tests
//=============================================================================

TEST(MathOpt_BFGSTest, SimpleQuadratic)
{
  SimpleQuadratic aFunc;
  math_Vector     aStart(2);
  aStart.ChangeAt(0) = 5.0;
  aStart.ChangeAt(1) = 5.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-10;
  aConfig.FTolerance    = 1.0e-12;

  auto aResult = MathOpt::BFGS(aFunc, aStart, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-8);
  EXPECT_NEAR(aResult.Solution->At(0), 0.0, 1.0e-6);
  EXPECT_NEAR(aResult.Solution->At(1), 0.0, 1.0e-6);
  EXPECT_LT(aResult.NbIterations, 20); // BFGS should converge quickly on quadratics
}

TEST(MathOpt_BFGSTest, BoothFunction)
{
  BoothFunction aFunc;
  math_Vector   aStart(2);
  aStart.ChangeAt(0) = 0.0;
  aStart.ChangeAt(1) = 0.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-10;
  aConfig.FTolerance    = 1.0e-12;

  auto aResult = MathOpt::BFGS(aFunc, aStart, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-8);
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, 1.0e-6);
  EXPECT_NEAR(aResult.Solution->At(1), 3.0, 1.0e-6);
}

TEST(MathOpt_BFGSTest, RotatedQuadraticMinimum)
{
  RotatedQuadraticFunction aFunc;
  math_Vector              aStart(size_t{2}, 4.0);
  MathUtils::Config        aConfig;
  aConfig.XTolerance                    = 1.0e-12;
  aConfig.FTolerance                    = 1.0e-12;
  const MathUtils::VectorResult aResult = MathOpt::BFGS(aFunc, aStart, aConfig);

  ASSERT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  ASSERT_TRUE(aResult.Value.has_value());
  EXPECT_NEAR(aResult.Solution->At(0), 1.5, 1.0e-10);
  EXPECT_NEAR(aResult.Solution->At(1), -1.0, 1.0e-10);
  EXPECT_NEAR(*aResult.Value, -0.75, 1.0e-11);
}

TEST(MathOpt_BFGSTest, RosenbrockFunction)
{
  MathOptTest_RosenbrockFunction aFunc;
  math_Vector        aStart(2);
  aStart.ChangeAt(0) = -1.0;
  aStart.ChangeAt(1) = 1.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 500;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;

  auto aResult = MathOpt::BFGS(aFunc, aStart, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-4);
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, 1.0e-3);
  EXPECT_NEAR(aResult.Solution->At(1), 1.0, 1.0e-3);
}

TEST(MathOpt_BFGSTest, BealeFunction)
{
  BealeFunction aFunc;
  math_Vector   aStart(2);
  aStart.ChangeAt(0) = 0.0;
  aStart.ChangeAt(1) = 0.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 200;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;

  auto aResult = MathOpt::BFGS(aFunc, aStart, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-4);
  EXPECT_NEAR(aResult.Solution->At(0), 3.0, 1.0e-3);
  EXPECT_NEAR(aResult.Solution->At(1), 0.5, 1.0e-3);
}

TEST(MathOpt_BFGSTest, SphereFunction5D)
{
  SphereFunction aFunc;
  math_Vector    aStart(5);
  for (size_t i = 0; i < aStart.Size(); ++i)
  {
    aStart.ChangeAt(i) = static_cast<double>(i + 1);
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
    EXPECT_NEAR(aResult.Solution->At(static_cast<size_t>(i - 1)), 0.0, 1.0e-6);
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
      for (size_t i = 0; i < theX.Size(); ++i)
      {
        theF += theX.At(i) * theX.At(i);
      }
      return true;
    }
  };

  QuadraticNoGrad aFunc;
  math_Vector     aStart(2);
  aStart.ChangeAt(0) = 5.0;
  aStart.ChangeAt(1) = 5.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-8;
  aConfig.FTolerance    = 1.0e-10;

  auto aResult = MathOpt::BFGSNumerical(aFunc, aStart, 1.0e-7, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-6);
  EXPECT_NEAR(aResult.Solution->At(0), 0.0, 1.0e-4);
  EXPECT_NEAR(aResult.Solution->At(1), 0.0, 1.0e-4);
}

//=============================================================================
// L-BFGS Tests
//=============================================================================

TEST(MathOpt_LBFGSTest, SimpleQuadratic)
{
  SimpleQuadratic aFunc;
  math_Vector     aStart(2);
  aStart.ChangeAt(0) = 5.0;
  aStart.ChangeAt(1) = 5.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-10;
  aConfig.FTolerance    = 1.0e-12;

  auto aResult = MathOpt::LBFGS(aFunc, aStart, 10, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-8);
  EXPECT_NEAR(aResult.Solution->At(0), 0.0, 1.0e-6);
  EXPECT_NEAR(aResult.Solution->At(1), 0.0, 1.0e-6);
}

TEST(MathOpt_LBFGSTest, BoothFunction)
{
  BoothFunction aFunc;
  math_Vector   aStart(2);
  aStart.ChangeAt(0) = 0.0;
  aStart.ChangeAt(1) = 0.0;

  MathOpt::Config aConfig;
  aConfig.MaxIterations = 100;
  aConfig.XTolerance    = 1.0e-10;
  aConfig.FTolerance    = 1.0e-12;

  auto aResult = MathOpt::LBFGS(aFunc, aStart, 10, aConfig);

  EXPECT_TRUE(aResult.IsDone());
  ASSERT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR(*aResult.Value, 0.0, 1.0e-8);
  EXPECT_NEAR(aResult.Solution->At(0), 1.0, 1.0e-6);
  EXPECT_NEAR(aResult.Solution->At(1), 3.0, 1.0e-6);
}

TEST(MathOpt_LBFGSTest, SphereFunction10D)
{
  SphereFunction aFunc;
  math_Vector    aStart(10);
  for (size_t i = 0; i < aStart.Size(); ++i)
  {
    aStart.ChangeAt(i) = static_cast<double>(i + 1);
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
    EXPECT_NEAR(aResult.Solution->At(static_cast<size_t>(i - 1)), 0.0, 1.0e-6);
  }
}

TEST(MathOpt_BFGSStatusTest, LineSearchCallbackErrorIsPreserved)
{
  math_Vector aStart(size_t{1}, 1.0);

  LineSearchFailureFunction aBFGSFunc;
  EXPECT_EQ(MathOpt::BFGS(aBFGSFunc, aStart).Status, MathUtils::Status::CallbackError);

  LineSearchFailureFunction anLBFGSFunc;
  EXPECT_EQ(MathOpt::LBFGS(anLBFGSFunc, aStart).Status, MathUtils::Status::CallbackError);

  math_Vector               aLower(size_t{1}, -2.0);
  math_Vector               anUpper(size_t{1}, 2.0);
  LineSearchFailureFunction aBoundedFunc;
  EXPECT_EQ(MathOpt::BFGSBounded(aBoundedFunc, aStart, aLower, anUpper).Status,
            MathUtils::Status::CallbackError);

  LineSearchFailureFunction aPowellFunc;
  EXPECT_EQ(MathOpt::Powell(aPowellFunc, aStart).Status, MathUtils::Status::CallbackError);
}

TEST(MathOpt_BFGSStatusTest, LineSearchNonFiniteValueIsNumericalError)
{
  LineSearchFailureFunction aFunc(true);
  math_Vector               aStart(size_t{1}, 1.0);

  EXPECT_EQ(MathOpt::BFGS(aFunc, aStart).Status, MathUtils::Status::NumericalError);
}

TEST(MathOpt_NDimStatusTest, DirectCallbackFailureIsCallbackError)
{
  math_Vector aStart(size_t{1}, 1.0);

  GradientCallbackFailureFunction aBFGSFunc;
  EXPECT_EQ(MathOpt::BFGS(aBFGSFunc, aStart).Status, MathUtils::Status::CallbackError);

  ValueCallbackFailureFunction aPowellFunc;
  EXPECT_EQ(MathOpt::Powell(aPowellFunc, aStart).Status, MathUtils::Status::CallbackError);
}

//=============================================================================
// Comparison Tests: Powell vs BFGS
//=============================================================================

TEST(MathOpt_ComparisonTest, BFGSFasterThanPowellOnQuadratic)
{
  SimpleQuadratic aFunc;
  math_Vector     aStart(2);
  aStart.ChangeAt(0) = 10.0;
  aStart.ChangeAt(1) = 10.0;

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
