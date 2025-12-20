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

#include <MathUtils_FunctorScalar.hxx>
#include <MathUtils_FunctorVector.hxx>
#include <MathRoot_Brent.hxx>
#include <MathRoot_Newton.hxx>
#include <MathOpt_Powell.hxx>
#include <MathOpt_BFGS.hxx>

#include <cmath>

namespace
{
constexpr double THE_TOLERANCE = 1e-10;
constexpr double THE_PI        = 3.14159265358979323846;
}

//==================================================================================================
// Scalar Functor Tests
//==================================================================================================

TEST(MathUtils_Functor_Scalar, ScalarLambda_Value)
{
  auto aFunc = MathUtils::MakeScalar([](double x, double& y) {
    y = x * x - 2.0;
    return true;
  });

  double aValue = 0.0;
  EXPECT_TRUE(aFunc.Value(2.0, aValue));
  EXPECT_NEAR(aValue, 2.0, THE_TOLERANCE);

  EXPECT_TRUE(aFunc.Value(std::sqrt(2.0), aValue));
  EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Scalar, ScalarLambdaWithDerivative_Values)
{
  auto aFunc = MathUtils::MakeScalarWithDerivative([](double x, double& y, double& dy) {
    y  = x * x - 2.0;
    dy = 2.0 * x;
    return true;
  });

  double aValue = 0.0;
  double aDeriv = 0.0;
  EXPECT_TRUE(aFunc.Values(3.0, aValue, aDeriv));
  EXPECT_NEAR(aValue, 7.0, THE_TOLERANCE);
  EXPECT_NEAR(aDeriv, 6.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Scalar, Polynomial_Value)
{
  // p(x) = 1 + 2x + 3x^2
  MathUtils::Polynomial aPoly({1.0, 2.0, 3.0});

  double aValue = 0.0;
  EXPECT_TRUE(aPoly.Value(0.0, aValue));
  EXPECT_NEAR(aValue, 1.0, THE_TOLERANCE);

  EXPECT_TRUE(aPoly.Value(1.0, aValue));
  EXPECT_NEAR(aValue, 6.0, THE_TOLERANCE); // 1 + 2 + 3

  EXPECT_TRUE(aPoly.Value(2.0, aValue));
  EXPECT_NEAR(aValue, 17.0, THE_TOLERANCE); // 1 + 4 + 12
}

TEST(MathUtils_Functor_Scalar, Polynomial_Values)
{
  // p(x) = x^2 - 2, p'(x) = 2x
  MathUtils::Polynomial aPoly({-2.0, 0.0, 1.0});

  double aValue = 0.0;
  double aDeriv = 0.0;
  EXPECT_TRUE(aPoly.Values(3.0, aValue, aDeriv));
  EXPECT_NEAR(aValue, 7.0, THE_TOLERANCE);
  EXPECT_NEAR(aDeriv, 6.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Scalar, Polynomial_WithRoots)
{
  // Using Brent's method with polynomial
  // p(x) = x^2 - 2, roots at +/-sqrt(2)
  MathUtils::Polynomial aPoly({-2.0, 0.0, 1.0});

  auto aResult = MathRoot::Brent(aPoly, 0.0, 2.0);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_TRUE(aResult.Root.has_value());
  EXPECT_NEAR(*aResult.Root, std::sqrt(2.0), 1e-9);
}

TEST(MathUtils_Functor_Scalar, Polynomial_WithNewton)
{
  // Using Newton's method with polynomial (has derivative)
  // p(x) = x^3 - 2x - 5
  MathUtils::Polynomial aPoly({-5.0, -2.0, 0.0, 1.0});

  auto aResult = MathRoot::Newton(aPoly, 2.0);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_TRUE(aResult.Root.has_value());
  // Root is approximately 2.0945514815
  EXPECT_NEAR(*aResult.Root, 2.0945514815, 1e-8);
}

TEST(MathUtils_Functor_Scalar, Rational_Value)
{
  // f(x) = (x + 1) / (x^2 + 1)
  MathUtils::Rational aRat({1.0, 1.0}, {1.0, 0.0, 1.0});

  double aValue = 0.0;
  EXPECT_TRUE(aRat.Value(0.0, aValue));
  EXPECT_NEAR(aValue, 1.0, THE_TOLERANCE);

  EXPECT_TRUE(aRat.Value(1.0, aValue));
  EXPECT_NEAR(aValue, 1.0, THE_TOLERANCE); // 2/2 = 1

  EXPECT_TRUE(aRat.Value(2.0, aValue));
  EXPECT_NEAR(aValue, 0.6, THE_TOLERANCE); // 3/5 = 0.6
}

TEST(MathUtils_Functor_Scalar, Rational_DivisionByZero)
{
  // f(x) = x / (x - 1), undefined at x = 1
  MathUtils::Rational aRat({0.0, 1.0}, {-1.0, 1.0});

  double aValue = 0.0;
  EXPECT_FALSE(aRat.Value(1.0, aValue));
}

TEST(MathUtils_Functor_Scalar, Composite_Value)
{
  // f(g(x)) where f(x) = x^2, g(x) = x + 1
  // Result: (x+1)^2
  MathUtils::Polynomial aOuter({0.0, 0.0, 1.0}); // x^2
  MathUtils::Polynomial aInner({1.0, 1.0});      // x + 1
  auto aComposite = MathUtils::MakeComposite(aOuter, aInner);

  double aValue = 0.0;
  EXPECT_TRUE(aComposite.Value(0.0, aValue));
  EXPECT_NEAR(aValue, 1.0, THE_TOLERANCE); // (0+1)^2 = 1

  EXPECT_TRUE(aComposite.Value(2.0, aValue));
  EXPECT_NEAR(aValue, 9.0, THE_TOLERANCE); // (2+1)^2 = 9
}

TEST(MathUtils_Functor_Scalar, Sum_Value)
{
  // f(x) + g(x) = (x^2) + (2x + 1) = x^2 + 2x + 1
  MathUtils::Polynomial aF({0.0, 0.0, 1.0}); // x^2
  MathUtils::Polynomial aG({1.0, 2.0});      // 2x + 1
  auto aSum = MathUtils::MakeSum(aF, aG);

  double aValue = 0.0;
  EXPECT_TRUE(aSum.Value(1.0, aValue));
  EXPECT_NEAR(aValue, 4.0, THE_TOLERANCE); // 1 + 3 = 4

  EXPECT_TRUE(aSum.Value(2.0, aValue));
  EXPECT_NEAR(aValue, 9.0, THE_TOLERANCE); // 4 + 5 = 9
}

TEST(MathUtils_Functor_Scalar, Difference_Value)
{
  // f(x) - g(x) = (x^2) - (x) = x^2 - x
  MathUtils::Polynomial aF({0.0, 0.0, 1.0}); // x^2
  MathUtils::Polynomial aG({0.0, 1.0});      // x
  auto aDiff = MathUtils::MakeDifference(aF, aG);

  double aValue = 0.0;
  EXPECT_TRUE(aDiff.Value(3.0, aValue));
  EXPECT_NEAR(aValue, 6.0, THE_TOLERANCE); // 9 - 3 = 6
}

TEST(MathUtils_Functor_Scalar, Product_Value)
{
  // f(x) * g(x) = (x) * (x + 1) = x^2 + x
  MathUtils::Polynomial aF({0.0, 1.0});      // x
  MathUtils::Polynomial aG({1.0, 1.0});      // x + 1
  auto aProd = MathUtils::MakeProduct(aF, aG);

  double aValue = 0.0;
  EXPECT_TRUE(aProd.Value(2.0, aValue));
  EXPECT_NEAR(aValue, 6.0, THE_TOLERANCE); // 2 * 3 = 6
}

TEST(MathUtils_Functor_Scalar, Quotient_Value)
{
  // f(x) / g(x) = (x^2) / (x) = x
  MathUtils::Polynomial aF({0.0, 0.0, 1.0}); // x^2
  MathUtils::Polynomial aG({0.0, 1.0});      // x
  auto aQuot = MathUtils::MakeQuotient(aF, aG);

  double aValue = 0.0;
  EXPECT_TRUE(aQuot.Value(5.0, aValue));
  EXPECT_NEAR(aValue, 5.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Scalar, Scaled_Value)
{
  // 3 * f(x) = 3 * x^2
  MathUtils::Polynomial aF({0.0, 0.0, 1.0}); // x^2
  auto aScaled = MathUtils::MakeScaled(aF, 3.0);

  double aValue = 0.0;
  EXPECT_TRUE(aScaled.Value(2.0, aValue));
  EXPECT_NEAR(aValue, 12.0, THE_TOLERANCE); // 3 * 4 = 12
}

TEST(MathUtils_Functor_Scalar, Shifted_Value)
{
  // f(x) + 5 = x^2 + 5
  MathUtils::Polynomial aF({0.0, 0.0, 1.0}); // x^2
  auto aShifted = MathUtils::MakeShifted(aF, 5.0);

  double aValue = 0.0;
  EXPECT_TRUE(aShifted.Value(2.0, aValue));
  EXPECT_NEAR(aValue, 9.0, THE_TOLERANCE); // 4 + 5 = 9
}

TEST(MathUtils_Functor_Scalar, Negated_Value)
{
  // -f(x) = -x^2
  MathUtils::Polynomial aF({0.0, 0.0, 1.0}); // x^2
  auto aNegated = MathUtils::MakeNegated(aF);

  double aValue = 0.0;
  EXPECT_TRUE(aNegated.Value(3.0, aValue));
  EXPECT_NEAR(aValue, -9.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Scalar, Constant_Value)
{
  MathUtils::Constant aConst(42.0);

  double aValue = 0.0;
  double aDeriv = 0.0;
  EXPECT_TRUE(aConst.Value(123.456, aValue));
  EXPECT_NEAR(aValue, 42.0, THE_TOLERANCE);

  EXPECT_TRUE(aConst.Values(789.0, aValue, aDeriv));
  EXPECT_NEAR(aValue, 42.0, THE_TOLERANCE);
  EXPECT_NEAR(aDeriv, 0.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Scalar, Linear_Value)
{
  // f(x) = 2x + 3
  MathUtils::Linear aLinear(2.0, 3.0);

  double aValue = 0.0;
  double aDeriv = 0.0;
  EXPECT_TRUE(aLinear.Value(5.0, aValue));
  EXPECT_NEAR(aValue, 13.0, THE_TOLERANCE);

  EXPECT_TRUE(aLinear.Values(5.0, aValue, aDeriv));
  EXPECT_NEAR(aValue, 13.0, THE_TOLERANCE);
  EXPECT_NEAR(aDeriv, 2.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Scalar, Sine_Value)
{
  MathUtils::Sine aSine(1.0, 1.0, 0.0, 0.0);

  double aValue = 0.0;
  EXPECT_TRUE(aSine.Value(0.0, aValue));
  EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE);

  EXPECT_TRUE(aSine.Value(THE_PI / 2.0, aValue));
  EXPECT_NEAR(aValue, 1.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Scalar, Sine_Values)
{
  MathUtils::Sine aSine(2.0, 1.0, 0.0, 0.0); // 2*sin(x)

  double aValue = 0.0;
  double aDeriv = 0.0;
  EXPECT_TRUE(aSine.Values(0.0, aValue, aDeriv));
  EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aDeriv, 2.0, THE_TOLERANCE); // 2*cos(0) = 2
}

TEST(MathUtils_Functor_Scalar, Cosine_Value)
{
  MathUtils::Cosine aCosine(1.0, 1.0, 0.0, 0.0);

  double aValue = 0.0;
  EXPECT_TRUE(aCosine.Value(0.0, aValue));
  EXPECT_NEAR(aValue, 1.0, THE_TOLERANCE);

  EXPECT_TRUE(aCosine.Value(THE_PI, aValue));
  EXPECT_NEAR(aValue, -1.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Scalar, Exponential_Value)
{
  MathUtils::Exponential aExp(1.0, 1.0, 0.0); // exp(x)

  double aValue = 0.0;
  EXPECT_TRUE(aExp.Value(0.0, aValue));
  EXPECT_NEAR(aValue, 1.0, THE_TOLERANCE);

  EXPECT_TRUE(aExp.Value(1.0, aValue));
  EXPECT_NEAR(aValue, std::exp(1.0), THE_TOLERANCE);
}

TEST(MathUtils_Functor_Scalar, Power_Value)
{
  MathUtils::Power aPower(2.0, 1.0, 0.0); // x^2

  double aValue = 0.0;
  EXPECT_TRUE(aPower.Value(3.0, aValue));
  EXPECT_NEAR(aValue, 9.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Scalar, Power_NonInteger)
{
  MathUtils::Power aPower(0.5, 1.0, 0.0); // sqrt(x)

  double aValue = 0.0;
  EXPECT_TRUE(aPower.Value(4.0, aValue));
  EXPECT_NEAR(aValue, 2.0, THE_TOLERANCE);

  // Negative x with non-integer exponent should fail
  EXPECT_FALSE(aPower.Value(-4.0, aValue));
}

TEST(MathUtils_Functor_Scalar, Gaussian_Value)
{
  MathUtils::Gaussian aGauss(1.0, 0.0, 1.0); // Standard normal

  double aValue = 0.0;
  EXPECT_TRUE(aGauss.Value(0.0, aValue));
  EXPECT_NEAR(aValue, 1.0, THE_TOLERANCE); // Peak at mean

  EXPECT_TRUE(aGauss.Value(1.0, aValue));
  EXPECT_NEAR(aValue, std::exp(-0.5), THE_TOLERANCE);
}

//==================================================================================================
// Vector Functor Tests
//==================================================================================================

TEST(MathUtils_Functor_Vector, VectorLambda_Value)
{
  auto aFunc = MathUtils::MakeVector([](const math_Vector& x, double& y) {
    y = x(1) * x(1) + x(2) * x(2); // Sphere
    return true;
  });

  math_Vector aX(1, 2);
  aX(1) = 3.0;
  aX(2) = 4.0;

  double aValue = 0.0;
  EXPECT_TRUE(aFunc.Value(aX, aValue));
  EXPECT_NEAR(aValue, 25.0, THE_TOLERANCE); // 9 + 16 = 25
}

TEST(MathUtils_Functor_Vector, VectorLambdaWithGradient_Values)
{
  auto aFunc = MathUtils::MakeVectorWithGradient(
    [](const math_Vector& x, double& y) {
      y = x(1) * x(1) + x(2) * x(2);
      return true;
    },
    [](const math_Vector& x, math_Vector& g) {
      g(1) = 2.0 * x(1);
      g(2) = 2.0 * x(2);
      return true;
    });

  math_Vector aX(1, 2);
  aX(1) = 3.0;
  aX(2) = 4.0;

  double      aValue = 0.0;
  math_Vector aGrad(1, 2);
  EXPECT_TRUE(aFunc.Values(aX, aValue, aGrad));
  EXPECT_NEAR(aValue, 25.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrad(1), 6.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrad(2), 8.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Vector, Rosenbrock_Value)
{
  MathUtils::Rosenbrock aRosen; // Default a=1, b=100

  math_Vector aX(1, 2);
  aX(1) = 1.0;
  aX(2) = 1.0;

  double aValue = 0.0;
  EXPECT_TRUE(aRosen.Value(aX, aValue));
  EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE); // Minimum at (1, 1)
}

TEST(MathUtils_Functor_Vector, Rosenbrock_AwayFromMinimum)
{
  MathUtils::Rosenbrock aRosen;

  math_Vector aX(1, 2);
  aX(1) = 0.0;
  aX(2) = 0.0;

  double aValue = 0.0;
  EXPECT_TRUE(aRosen.Value(aX, aValue));
  EXPECT_NEAR(aValue, 1.0, THE_TOLERANCE); // (1-0)^2 + 100*(0-0)^2 = 1
}

TEST(MathUtils_Functor_Vector, Rosenbrock_Gradient)
{
  MathUtils::Rosenbrock aRosen;

  math_Vector aX(1, 2);
  aX(1) = 1.0;
  aX(2) = 1.0;

  math_Vector aGrad(1, 2);
  EXPECT_TRUE(aRosen.Gradient(aX, aGrad));
  EXPECT_NEAR(aGrad(1), 0.0, THE_TOLERANCE); // Gradient is zero at minimum
  EXPECT_NEAR(aGrad(2), 0.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Vector, Sphere_Value)
{
  MathUtils::Sphere aSphere;

  math_Vector aX(1, 3);
  aX(1) = 1.0;
  aX(2) = 2.0;
  aX(3) = 3.0;

  double aValue = 0.0;
  EXPECT_TRUE(aSphere.Value(aX, aValue));
  EXPECT_NEAR(aValue, 14.0, THE_TOLERANCE); // 1 + 4 + 9 = 14
}

TEST(MathUtils_Functor_Vector, Sphere_Gradient)
{
  MathUtils::Sphere aSphere;

  math_Vector aX(1, 3);
  aX(1) = 1.0;
  aX(2) = 2.0;
  aX(3) = 3.0;

  math_Vector aGrad(1, 3);
  EXPECT_TRUE(aSphere.Gradient(aX, aGrad));
  EXPECT_NEAR(aGrad(1), 2.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrad(2), 4.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrad(3), 6.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Vector, Booth_Value)
{
  MathUtils::Booth aBooth;

  math_Vector aX(1, 2);
  aX(1) = 1.0;
  aX(2) = 3.0;

  double aValue = 0.0;
  EXPECT_TRUE(aBooth.Value(aX, aValue));
  EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE); // Minimum at (1, 3)
}

TEST(MathUtils_Functor_Vector, Beale_Value)
{
  MathUtils::Beale aBeale;

  math_Vector aX(1, 2);
  aX(1) = 3.0;
  aX(2) = 0.5;

  double aValue = 0.0;
  EXPECT_TRUE(aBeale.Value(aX, aValue));
  EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE); // Minimum at (3, 0.5)
}

TEST(MathUtils_Functor_Vector, Himmelblau_Value)
{
  MathUtils::Himmelblau aHimmel;

  // Test one of the four minima: (3, 2)
  math_Vector aX(1, 2);
  aX(1) = 3.0;
  aX(2) = 2.0;

  double aValue = 0.0;
  EXPECT_TRUE(aHimmel.Value(aX, aValue));
  EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Vector, Rastrigin_Value)
{
  MathUtils::Rastrigin aRast;

  math_Vector aX(1, 2);
  aX(1) = 0.0;
  aX(2) = 0.0;

  double aValue = 0.0;
  EXPECT_TRUE(aRast.Value(aX, aValue));
  EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE); // Minimum at origin
}

TEST(MathUtils_Functor_Vector, Ackley_Value)
{
  MathUtils::Ackley aAckley;

  math_Vector aX(1, 2);
  aX(1) = 0.0;
  aX(2) = 0.0;

  double aValue = 0.0;
  EXPECT_TRUE(aAckley.Value(aX, aValue));
  EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE); // Minimum at origin
}

TEST(MathUtils_Functor_Vector, QuadraticForm_Value)
{
  // f(x) = x^T * I * x = ||x||^2 (identity matrix)
  math_Matrix aA(1, 2, 1, 2, 0.0);
  aA(1, 1) = 1.0;
  aA(2, 2) = 1.0;

  math_Vector aB(1, 2, 0.0);
  double      aC = 0.0;

  MathUtils::QuadraticForm aQuad(aA, aB, aC);

  math_Vector aX(1, 2);
  aX(1) = 3.0;
  aX(2) = 4.0;

  double aValue = 0.0;
  EXPECT_TRUE(aQuad.Value(aX, aValue));
  EXPECT_NEAR(aValue, 25.0, THE_TOLERANCE); // 9 + 16 = 25
}

TEST(MathUtils_Functor_Vector, QuadraticForm_Gradient)
{
  // f(x) = x^T * I * x, gradient = 2x
  math_Matrix aA(1, 2, 1, 2, 0.0);
  aA(1, 1) = 1.0;
  aA(2, 2) = 1.0;

  math_Vector aB(1, 2, 0.0);
  double      aC = 0.0;

  MathUtils::QuadraticForm aQuad(aA, aB, aC);

  math_Vector aX(1, 2);
  aX(1) = 3.0;
  aX(2) = 4.0;

  math_Vector aGrad(1, 2);
  EXPECT_TRUE(aQuad.Gradient(aX, aGrad));
  EXPECT_NEAR(aGrad(1), 6.0, THE_TOLERANCE);
  EXPECT_NEAR(aGrad(2), 8.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Vector, LinearResidual_Value)
{
  // Solve: [1 0; 0 1] * x = [3; 4]
  // Residual at x = [3, 4] should be 0
  math_Matrix aA(1, 2, 1, 2, 0.0);
  aA(1, 1) = 1.0;
  aA(2, 2) = 1.0;

  math_Vector aB(1, 2);
  aB(1) = 3.0;
  aB(2) = 4.0;

  MathUtils::LinearResidual aRes(aA, aB);

  math_Vector aX(1, 2);
  aX(1) = 3.0;
  aX(2) = 4.0;

  double aValue = 0.0;
  EXPECT_TRUE(aRes.Value(aX, aValue));
  EXPECT_NEAR(aValue, 0.0, THE_TOLERANCE);
}

TEST(MathUtils_Functor_Vector, LinearResidual_NonZero)
{
  math_Matrix aA(1, 2, 1, 2, 0.0);
  aA(1, 1) = 1.0;
  aA(2, 2) = 1.0;

  math_Vector aB(1, 2);
  aB(1) = 3.0;
  aB(2) = 4.0;

  MathUtils::LinearResidual aRes(aA, aB);

  math_Vector aX(1, 2);
  aX(1) = 0.0;
  aX(2) = 0.0;

  double aValue = 0.0;
  EXPECT_TRUE(aRes.Value(aX, aValue));
  EXPECT_NEAR(aValue, 25.0, THE_TOLERANCE); // ||[3,4]||^2 = 25
}

//==================================================================================================
// Integration Tests - Functors with Solvers
//==================================================================================================

TEST(MathUtils_Functor_Integration, Polynomial_WithBrent)
{
  // Find root of x^3 - x - 2 = 0 (root near 1.52)
  MathUtils::Polynomial aPoly({-2.0, -1.0, 0.0, 1.0});

  auto aResult = MathRoot::Brent(aPoly, 1.0, 2.0);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_TRUE(aResult.Root.has_value());

  // Verify the root
  double aValue = 0.0;
  aPoly.Value(*aResult.Root, aValue);
  EXPECT_NEAR(aValue, 0.0, 1e-10);
}

TEST(MathUtils_Functor_Integration, Sine_RootFinding)
{
  // Find root of sin(x) in [2, 4] (root at pi)
  MathUtils::Sine aSine;

  auto aResult = MathRoot::Brent(aSine, 2.0, 4.0);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_TRUE(aResult.Root.has_value());
  EXPECT_NEAR(*aResult.Root, THE_PI, 1e-10);
}

TEST(MathUtils_Functor_Integration, Sphere_WithPowell)
{
  // Minimize sphere function - should find origin
  MathUtils::Sphere aSphere;

  math_Vector aStart(1, 2);
  aStart(1) = 5.0;
  aStart(2) = 5.0;

  auto aResult = MathOpt::Powell(aSphere, aStart);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR((*aResult.Solution)(1), 0.0, 1e-5);
  EXPECT_NEAR((*aResult.Solution)(2), 0.0, 1e-5);
}

TEST(MathUtils_Functor_Integration, Booth_WithBFGS)
{
  // Minimize Booth function - should find (1, 3)
  MathUtils::Booth aBooth;

  math_Vector aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  auto aResult = MathOpt::BFGS(aBooth, aStart);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 1e-5);
  EXPECT_NEAR((*aResult.Solution)(2), 3.0, 1e-5);
}

TEST(MathUtils_Functor_Integration, Rosenbrock_WithBFGS)
{
  // Minimize Rosenbrock - more challenging
  MathUtils::Rosenbrock aRosen;

  math_Vector aStart(1, 2);
  aStart(1) = -1.0;
  aStart(2) = 1.0;

  MathUtils::NDimConfig aConfig;
  aConfig.MaxIterations = 500;
  aConfig.FTolerance    = 1e-10;
  aConfig.XTolerance    = 1e-10;

  auto aResult = MathOpt::BFGS(aRosen, aStart, aConfig);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 1e-4);
  EXPECT_NEAR((*aResult.Solution)(2), 1.0, 1e-4);
}

TEST(MathUtils_Functor_Integration, Quadratic_WithNewton)
{
  // Minimize quadratic form with Newton's method
  // f(x) = x^T * A * x + b^T * x + c
  // where A = diag(2, 4), b = [-4, -8], c = 10
  // Minimum at x = A^(-1) * (-b/2) = [1, 1] with f = 6
  math_Matrix aA(1, 2, 1, 2, 0.0);
  aA(1, 1) = 2.0;
  aA(2, 2) = 4.0;

  math_Vector aB(1, 2);
  aB(1) = -4.0;
  aB(2) = -8.0;

  MathUtils::QuadraticForm aQuad(aA, aB, 10.0);

  math_Vector aStart(1, 2);
  aStart(1) = 0.0;
  aStart(2) = 0.0;

  auto aResult = MathOpt::BFGS(aQuad, aStart);
  EXPECT_TRUE(aResult.IsDone());
  EXPECT_TRUE(aResult.Solution.has_value());
  EXPECT_NEAR((*aResult.Solution)(1), 1.0, 1e-5);
  EXPECT_NEAR((*aResult.Solution)(2), 1.0, 1e-5);
}
