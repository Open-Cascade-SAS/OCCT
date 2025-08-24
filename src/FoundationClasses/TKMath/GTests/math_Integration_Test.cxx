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

#include <math_GaussSingleIntegration.hxx>
#include <math_KronrodSingleIntegration.hxx>
#include <math_Function.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <StdFail_NotDone.hxx>
#include <Precision.hxx>

#include <cmath>

namespace
{

// Test function classes for numerical integration

// Constant function: f(x) = c
class ConstantFunction : public math_Function
{
private:
  Standard_Real myValue;

public:
  ConstantFunction(Standard_Real theValue)
      : myValue(theValue)
  {
  }

  Standard_Boolean Value(const Standard_Real /*theX*/, Standard_Real& theF) override
  {
    theF = myValue;
    return Standard_True;
  }
};

// Linear function: f(x) = ax + b
class LinearFunction : public math_Function
{
private:
  Standard_Real myA, myB;

public:
  LinearFunction(Standard_Real theA, Standard_Real theB)
      : myA(theA),
        myB(theB)
  {
  }

  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = myA * theX + myB;
    return Standard_True;
  }
};

// Quadratic function: f(x) = ax^2 + bx + c
class QuadraticFunction : public math_Function
{
private:
  Standard_Real myA, myB, myC;

public:
  QuadraticFunction(Standard_Real theA, Standard_Real theB, Standard_Real theC)
      : myA(theA),
        myB(theB),
        myC(theC)
  {
  }

  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = myA * theX * theX + myB * theX + myC;
    return Standard_True;
  }
};

// Polynomial function: f(x) = x^n
class PowerFunction : public math_Function
{
private:
  Standard_Integer myPower;

public:
  PowerFunction(Standard_Integer thePower)
      : myPower(thePower)
  {
  }

  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = pow(theX, myPower);
    return Standard_True;
  }
};

// Trigonometric function: f(x) = sin(x)
class SineFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = sin(theX);
    return Standard_True;
  }
};

// Exponential function: f(x) = e^x
class ExponentialFunction : public math_Function
{
public:
  Standard_Boolean Value(const Standard_Real theX, Standard_Real& theF) override
  {
    theF = exp(theX);
    return Standard_True;
  }
};

// Tests for math_GaussSingleIntegration
TEST(MathIntegrationTest, GaussConstantFunction)
{
  ConstantFunction aFunc(5.0);
  Standard_Real    aLower  = 0.0;
  Standard_Real    anUpper = 2.0;
  Standard_Integer anOrder = 4;

  math_GaussSingleIntegration anIntegrator(aFunc, aLower, anUpper, anOrder);

  EXPECT_TRUE(anIntegrator.IsDone()) << "Gauss integration should succeed for constant function";

  // Integral of constant 5 from 0 to 2 should be 5 * (2-0) = 10
  Standard_Real anExpected = 5.0 * (anUpper - aLower);
  EXPECT_NEAR(anIntegrator.Value(), anExpected, 1.0e-12)
    << "Constant function integration should be exact";
}

TEST(MathIntegrationTest, GaussLinearFunction)
{
  LinearFunction   aFunc(2.0, 3.0); // f(x) = 2x + 3
  Standard_Real    aLower  = 1.0;
  Standard_Real    anUpper = 4.0;
  Standard_Integer anOrder = 2; // Should be exact for linear functions

  math_GaussSingleIntegration anIntegrator(aFunc, aLower, anUpper, anOrder);

  EXPECT_TRUE(anIntegrator.IsDone()) << "Gauss integration should succeed for linear function";

  // Integral of 2x + 3 from 1 to 4: [x^2 + 3x] from 1 to 4 = (16 + 12) - (1 + 3) = 24
  Standard_Real anExpected = 24.0;
  EXPECT_NEAR(anIntegrator.Value(), anExpected, 1.0e-12)
    << "Linear function integration should be exact with order 2";
}

TEST(MathIntegrationTest, GaussQuadraticFunction)
{
  QuadraticFunction aFunc(1.0, -2.0, 1.0); // f(x) = x^2 - 2x + 1 = (x-1)^2
  Standard_Real     aLower  = 0.0;
  Standard_Real     anUpper = 2.0;
  Standard_Integer  anOrder = 3; // Should be exact for quadratic functions

  math_GaussSingleIntegration anIntegrator(aFunc, aLower, anUpper, anOrder);

  EXPECT_TRUE(anIntegrator.IsDone()) << "Gauss integration should succeed for quadratic function";

  // Integral of (x-1)^2 from 0 to 2: [(x-1)^3/3] from 0 to 2 = (1/3) - (-1/3) = 2/3
  Standard_Real anExpected = 2.0 / 3.0;

  EXPECT_NEAR(anIntegrator.Value(), anExpected, 1.0e-12)
    << "Quadratic function integration should be exact with order 3";
}

TEST(MathIntegrationTest, GaussSineFunction)
{
  SineFunction     aFunc;
  Standard_Real    aLower  = 0.0;
  Standard_Real    anUpper = M_PI;
  Standard_Integer anOrder = 10;

  math_GaussSingleIntegration anIntegrator(aFunc, aLower, anUpper, anOrder);

  EXPECT_TRUE(anIntegrator.IsDone()) << "Gauss integration should succeed for sine function";

  // Integral of sin(x) from 0 to PI: [-cos(x)] from 0 to PI = -(-1) - (-1) = 2
  Standard_Real anExpected = 2.0;
  EXPECT_NEAR(anIntegrator.Value(), anExpected, 1.0e-10)
    << "Sine function integration should be accurate";
}

TEST(MathIntegrationTest, GaussExponentialFunction)
{
  ExponentialFunction aFunc;
  Standard_Real       aLower  = 0.0;
  Standard_Real       anUpper = 1.0;
  Standard_Integer    anOrder = 15;

  math_GaussSingleIntegration anIntegrator(aFunc, aLower, anUpper, anOrder);

  EXPECT_TRUE(anIntegrator.IsDone()) << "Gauss integration should succeed for exponential function";

  // Integral of e^x from 0 to 1: [e^x] from 0 to 1 = e - 1 approximately 1.71828
  Standard_Real anExpected = exp(1.0) - 1.0;
  EXPECT_NEAR(anIntegrator.Value(), anExpected, 1.0e-8)
    << "Exponential function integration should be accurate";
}

TEST(MathIntegrationTest, GaussDifferentOrders)
{
  QuadraticFunction aFunc(1.0, 0.0, 0.0); // f(x) = x^2
  Standard_Real     aLower  = 0.0;
  Standard_Real     anUpper = 1.0;

  // Expected result: integral of x^2 from 0 to 1 = [x^3/3] = 1/3
  Standard_Real anExpected = 1.0 / 3.0;

  // Test different orders
  std::vector<Standard_Integer> anOrders = {2, 3, 5, 10, 20};

  for (Standard_Integer anOrder : anOrders)
  {
    math_GaussSingleIntegration anIntegrator(aFunc, aLower, anUpper, anOrder);

    EXPECT_TRUE(anIntegrator.IsDone()) << "Integration should succeed for order " << anOrder;

    if (anOrder >= 3)
    {
      // Should be exact for order 3 and above (quadratic function)
      EXPECT_NEAR(anIntegrator.Value(), anExpected, 1.0e-12)
        << "High order integration should be exact for order " << anOrder;
    }
    else
    {
      // Lower order might not be exact but should be reasonable
      EXPECT_NEAR(anIntegrator.Value(), anExpected, 1.0e-6)
        << "Low order integration should be reasonably accurate for order " << anOrder;
    }
  }
}

TEST(MathIntegrationTest, GaussWithTolerance)
{
  QuadraticFunction aFunc(1.0, 0.0, 0.0); // f(x) = x^2
  Standard_Real     aLower     = 0.0;
  Standard_Real     anUpper    = 1.0;
  Standard_Integer  anOrder    = 5;
  Standard_Real     aTolerance = 1.0e-10;

  math_GaussSingleIntegration anIntegrator(aFunc, aLower, anUpper, anOrder, aTolerance);

  EXPECT_TRUE(anIntegrator.IsDone()) << "Integration with tolerance should succeed";

  Standard_Real anExpected = 1.0 / 3.0;
  EXPECT_NEAR(anIntegrator.Value(), anExpected, aTolerance * 10)
    << "Integration with tolerance should meet accuracy requirements";
}

TEST(MathIntegrationTest, GaussNegativeInterval)
{
  LinearFunction   aFunc(1.0, 0.0); // f(x) = x
  Standard_Real    aLower  = -2.0;
  Standard_Real    anUpper = 2.0;
  Standard_Integer anOrder = 3;

  math_GaussSingleIntegration anIntegrator(aFunc, aLower, anUpper, anOrder);

  EXPECT_TRUE(anIntegrator.IsDone()) << "Integration over symmetric interval should succeed";

  // Integral of x from -2 to 2: [x^2/2] from -2 to 2 = 2 - 2 = 0
  EXPECT_NEAR(anIntegrator.Value(), 0.0, 1.0e-12)
    << "Symmetric odd function integral should be zero";
}

// Tests for math_KronrodSingleIntegration
TEST(MathIntegrationTest, KronrodConstantFunction)
{
  ConstantFunction aFunc(3.0);
  Standard_Real    aLower  = 1.0;
  Standard_Real    anUpper = 5.0;
  Standard_Integer anOrder = 15; // Kronrod typically uses 15, 21, 31, etc.

  math_KronrodSingleIntegration anIntegrator(aFunc, aLower, anUpper, anOrder);

  EXPECT_TRUE(anIntegrator.IsDone()) << "Kronrod integration should succeed for constant function";

  // Integral of constant 3 from 1 to 5 should be 3 * (5-1) = 12
  Standard_Real anExpected = 3.0 * (anUpper - aLower);
  EXPECT_NEAR(anIntegrator.Value(), anExpected, 1.0e-12)
    << "Constant function integration should be exact";
}

TEST(MathIntegrationTest, KronrodQuadraticFunction)
{
  QuadraticFunction aFunc(2.0, -1.0, 3.0); // f(x) = 2x^2 - x + 3
  Standard_Real     aLower  = 0.0;
  Standard_Real     anUpper = 1.0;
  Standard_Integer  anOrder = 15;

  math_KronrodSingleIntegration anIntegrator(aFunc, aLower, anUpper, anOrder);

  EXPECT_TRUE(anIntegrator.IsDone()) << "Kronrod integration should succeed for quadratic function";

  // Integral of 2x^2 - x + 3 from 0 to 1: [2x^3/3 - x^2/2 + 3x] from 0 to 1 = 2/3 - 1/2 + 3 = 2/3 -
  // 3/6 + 18/6 = 4/6 + 18/6 - 3/6 = 19/6
  Standard_Real anExpected = 19.0 / 6.0;
  EXPECT_NEAR(anIntegrator.Value(), anExpected, 1.0e-10)
    << "Quadratic function integration should be very accurate";
}

TEST(MathIntegrationTest, KronrodSineFunction)
{
  SineFunction     aFunc;
  Standard_Real    aLower  = 0.0;
  Standard_Real    anUpper = M_PI / 2.0;
  Standard_Integer anOrder = 21;

  math_KronrodSingleIntegration anIntegrator(aFunc, aLower, anUpper, anOrder);

  EXPECT_TRUE(anIntegrator.IsDone()) << "Kronrod integration should succeed for sine function";

  // Integral of sin(x) from 0 to PI/2: [-cos(x)] from 0 to PI/2 = 0 - (-1) = 1
  Standard_Real anExpected = 1.0;
  EXPECT_NEAR(anIntegrator.Value(), anExpected, 1.0e-12)
    << "Sine function integration should be very accurate";
}

TEST(MathIntegrationTest, GaussVsKronrodComparison)
{
  PowerFunction aFunc(4); // f(x) = x^4
  Standard_Real aLower  = 0.0;
  Standard_Real anUpper = 2.0;

  // Expected: integral of x^4 from 0 to 2 = [x^5/5] from 0 to 2 = 32/5 = 6.4
  Standard_Real anExpected = 32.0 / 5.0;

  // Gauss integration
  math_GaussSingleIntegration aGaussIntegrator(aFunc, aLower, anUpper, 10);
  EXPECT_TRUE(aGaussIntegrator.IsDone()) << "Gauss integration should succeed";

  // Kronrod integration
  math_KronrodSingleIntegration aKronrodIntegrator(aFunc, aLower, anUpper, 21);
  EXPECT_TRUE(aKronrodIntegrator.IsDone()) << "Kronrod integration should succeed";

  // Both should be accurate
  EXPECT_NEAR(aGaussIntegrator.Value(), anExpected, 1.0e-10)
    << "Gauss integration should be accurate for x^4";
  EXPECT_NEAR(aKronrodIntegrator.Value(), anExpected, 1.0e-10)
    << "Kronrod integration should be accurate for x^4";

  // Results should be similar (within tolerance)
  EXPECT_NEAR(aGaussIntegrator.Value(), aKronrodIntegrator.Value(), 1.0e-8)
    << "Gauss and Kronrod results should be similar";
}

TEST(MathIntegrationTest, DefaultConstructorAndPerform)
{
  // Test default constructor followed by separate computation
  math_GaussSingleIntegration anIntegrator1;

  LinearFunction   aFunc(1.0, 2.0); // f(x) = x + 2
  Standard_Real    aLower  = 0.0;
  Standard_Real    anUpper = 3.0;
  Standard_Integer anOrder = 5;

  // Note: We can't call Perform directly in the public interface,
  // so we test by creating with parameters after default construction
  // This tests that the default constructor doesn't crash

  math_GaussSingleIntegration anIntegrator2(aFunc, aLower, anUpper, anOrder);
  EXPECT_TRUE(anIntegrator2.IsDone()) << "Integration after explicit construction should succeed";

  // Integral of x + 2 from 0 to 3: [x^2/2 + 2x] from 0 to 3 = 9/2 + 6 = 10.5
  EXPECT_NEAR(anIntegrator2.Value(), 10.5, 1.0e-12) << "Linear function integral should be exact";
}

// Tests for edge cases and error handling
TEST(MathIntegrationTest, ZeroLengthInterval)
{
  ConstantFunction aFunc(1.0);
  Standard_Real    aLower  = 5.0;
  Standard_Real    anUpper = 5.0; // Same as lower
  Standard_Integer anOrder = 5;

  math_GaussSingleIntegration anIntegrator(aFunc, aLower, anUpper, anOrder);

  // Integration over zero-length interval should give zero
  if (anIntegrator.IsDone())
  {
    EXPECT_NEAR(anIntegrator.Value(), 0.0, Precision::Confusion())
      << "Zero-length interval should give zero integral";
  }
}

TEST(MathIntegrationTest, ReverseInterval)
{
  LinearFunction   aFunc(1.0, 0.0); // f(x) = x
  Standard_Real    aLower  = 2.0;
  Standard_Real    anUpper = 0.0; // Upper < Lower
  Standard_Integer anOrder = 5;

  math_GaussSingleIntegration anIntegrator(aFunc, aLower, anUpper, anOrder);

  if (anIntegrator.IsDone())
  {
    // Integral from 2 to 0 should be negative of integral from 0 to 2
    // Integral of x from 0 to 2 = [x^2/2] = 2, so from 2 to 0 = -2
    EXPECT_NEAR(anIntegrator.Value(), -2.0, 1.0e-12)
      << "Reverse interval should give negative result";
  }
}

TEST(MathIntegrationTest, HighOrderIntegration)
{
  PowerFunction    aFunc(10); // f(x) = x^10
  Standard_Real    aLower  = 0.0;
  Standard_Real    anUpper = 1.0;
  Standard_Integer anOrder = 30; // High order

  math_GaussSingleIntegration anIntegrator(aFunc, aLower, anUpper, anOrder);

  EXPECT_TRUE(anIntegrator.IsDone()) << "High order integration should succeed";

  // Integral of x^10 from 0 to 1 = [x^11/11] = 1/11
  Standard_Real anExpected = 1.0 / 11.0;
  EXPECT_NEAR(anIntegrator.Value(), anExpected, 1.0e-12)
    << "High order integration of polynomial should be exact";
}

} // anonymous namespace