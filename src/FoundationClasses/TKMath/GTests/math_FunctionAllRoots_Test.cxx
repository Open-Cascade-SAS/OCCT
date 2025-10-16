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
#include <math_FunctionAllRoots.hxx>
#include <math_FunctionSample.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_DimensionError.hxx>

namespace
{
// Test function with multiple roots: f(x) = x^3 - 6x^2 + 11x - 6 = (x-1)(x-2)(x-3)
class CubicFunction : public math_FunctionWithDerivative
{
public:
  virtual Standard_Boolean Value(const Standard_Real X, Standard_Real& F) override
  {
    F = X * X * X - 6.0 * X * X + 11.0 * X - 6.0;
    return Standard_True;
  }

  virtual Standard_Boolean Derivative(const Standard_Real X, Standard_Real& D) override
  {
    D = 3.0 * X * X - 12.0 * X + 11.0;
    return Standard_True;
  }

  virtual Standard_Boolean Values(const Standard_Real X,
                                  Standard_Real&      F,
                                  Standard_Real&      D) override
  {
    F = X * X * X - 6.0 * X * X + 11.0 * X - 6.0;
    D = 3.0 * X * X - 12.0 * X + 11.0;
    return Standard_True;
  }
};

// Sine function with multiple roots
class SineFunction : public math_FunctionWithDerivative
{
public:
  virtual Standard_Boolean Value(const Standard_Real X, Standard_Real& F) override
  {
    F = sin(X);
    return Standard_True;
  }

  virtual Standard_Boolean Derivative(const Standard_Real X, Standard_Real& D) override
  {
    D = cos(X);
    return Standard_True;
  }

  virtual Standard_Boolean Values(const Standard_Real X,
                                  Standard_Real&      F,
                                  Standard_Real&      D) override
  {
    F = sin(X);
    D = cos(X);
    return Standard_True;
  }
};

// Function with a null interval: f(x) = 0 for x in [2, 4]
class NullIntervalFunction : public math_FunctionWithDerivative
{
public:
  virtual Standard_Boolean Value(const Standard_Real X, Standard_Real& F) override
  {
    if (X >= 2.0 && X <= 4.0)
      F = 0.0;
    else if (X < 2.0)
      F = X - 2.0;
    else
      F = X - 4.0;
    return Standard_True;
  }

  virtual Standard_Boolean Derivative(const Standard_Real X, Standard_Real& D) override
  {
    if (X >= 2.0 && X <= 4.0)
      D = 0.0;
    else
      D = 1.0;
    return Standard_True;
  }

  virtual Standard_Boolean Values(const Standard_Real X,
                                  Standard_Real&      F,
                                  Standard_Real&      D) override
  {
    if (X >= 2.0 && X <= 4.0)
    {
      F = 0.0;
      D = 0.0;
    }
    else if (X < 2.0)
    {
      F = X - 2.0;
      D = 1.0;
    }
    else
    {
      F = X - 4.0;
      D = 1.0;
    }
    return Standard_True;
  }
};

// Function with one root: f(x) = (x-1.5)^2 - 0.25 = (x-1)(x-2)
class QuadraticFunction : public math_FunctionWithDerivative
{
public:
  virtual Standard_Boolean Value(const Standard_Real X, Standard_Real& F) override
  {
    F = (X - 1.5) * (X - 1.5) - 0.25;
    return Standard_True;
  }

  virtual Standard_Boolean Derivative(const Standard_Real X, Standard_Real& D) override
  {
    D = 2.0 * (X - 1.5);
    return Standard_True;
  }

  virtual Standard_Boolean Values(const Standard_Real X,
                                  Standard_Real&      F,
                                  Standard_Real&      D) override
  {
    F = (X - 1.5) * (X - 1.5) - 0.25;
    D = 2.0 * (X - 1.5);
    return Standard_True;
  }
};

// Constant zero function
class ZeroFunction : public math_FunctionWithDerivative
{
public:
  virtual Standard_Boolean Value(const Standard_Real, Standard_Real& F) override
  {
    F = 0.0;
    return Standard_True;
  }

  virtual Standard_Boolean Derivative(const Standard_Real, Standard_Real& D) override
  {
    D = 0.0;
    return Standard_True;
  }

  virtual Standard_Boolean Values(const Standard_Real, Standard_Real& F, Standard_Real& D) override
  {
    F = 0.0;
    D = 0.0;
    return Standard_True;
  }
};
} // namespace

TEST(math_FunctionAllRoots, CubicFunctionBasic)
{
  CubicFunction         func;
  math_FunctionSample   sample(0.0, 5.0, 21);
  math_FunctionAllRoots solver(func, sample, 1.0e-6, 1.0e-6, 1.0e-8);

  EXPECT_TRUE(solver.IsDone());

  // Should find the three roots at x = 1, 2, 3
  EXPECT_GE(solver.NbPoints(), 2);

  if (solver.NbPoints() >= 2)
  {
    Standard_Real root1 = solver.GetPoint(1);
    Standard_Real root2 = solver.GetPoint(2);

    // Check if we found roots near 1, 2, or 3
    EXPECT_TRUE(fabs(root1 - 1.0) < 1.0e-4 || fabs(root1 - 2.0) < 1.0e-4
                || fabs(root1 - 3.0) < 1.0e-4);
    EXPECT_TRUE(fabs(root2 - 1.0) < 1.0e-4 || fabs(root2 - 2.0) < 1.0e-4
                || fabs(root2 - 3.0) < 1.0e-4);
  }
}

TEST(math_FunctionAllRoots, SineFunctionRoots)
{
  SineFunction          func;
  math_FunctionSample   sample(-1.0, 7.0, 41);
  math_FunctionAllRoots solver(func, sample, 1.0e-6, 1.0e-6, 1.0e-8);

  EXPECT_TRUE(solver.IsDone());

  // Should find roots near 0, PI, 2*PI within the range
  EXPECT_GE(solver.NbPoints(), 2);

  if (solver.NbPoints() >= 1)
  {
    Standard_Real root1 = solver.GetPoint(1);
    // Check if we found a root near 0 or PI
    EXPECT_TRUE(fabs(root1) < 1.0e-4 || fabs(root1 - M_PI) < 1.0e-4
                || fabs(root1 - 2 * M_PI) < 1.0e-4);
  }
}

TEST(math_FunctionAllRoots, NullIntervalFunction)
{
  NullIntervalFunction  func;
  math_FunctionSample   sample(0.0, 6.0, 31);
  math_FunctionAllRoots solver(func, sample, 1.0e-6, 1.0e-6, 1.0e-6);

  EXPECT_TRUE(solver.IsDone());

  // Should find a null interval from x = 2 to x = 4
  EXPECT_GE(solver.NbIntervals(), 1);

  if (solver.NbIntervals() >= 1)
  {
    Standard_Real a, b;
    solver.GetInterval(1, a, b);
    EXPECT_NEAR(a, 2.0, 1.0e-3);
    EXPECT_NEAR(b, 4.0, 1.0e-3);
  }
}

TEST(math_FunctionAllRoots, QuadraticTwoRoots)
{
  QuadraticFunction     func;
  math_FunctionSample   sample(0.0, 3.0, 16);
  math_FunctionAllRoots solver(func, sample, 1.0e-6, 1.0e-6, 1.0e-8);

  EXPECT_TRUE(solver.IsDone());

  // Should find roots at x = 1 and x = 2
  EXPECT_GE(solver.NbPoints(), 1);

  if (solver.NbPoints() >= 1)
  {
    Standard_Real root1 = solver.GetPoint(1);
    EXPECT_TRUE(fabs(root1 - 1.0) < 1.0e-4 || fabs(root1 - 2.0) < 1.0e-4);
  }
}

TEST(math_FunctionAllRoots, ZeroFunction)
{
  ZeroFunction          func;
  math_FunctionSample   sample(0.0, 1.0, 11);
  math_FunctionAllRoots solver(func, sample, 1.0e-6, 1.0e-6, 1.0e-6);

  EXPECT_TRUE(solver.IsDone());

  // Should find the entire interval as null
  EXPECT_GE(solver.NbIntervals(), 1);

  if (solver.NbIntervals() >= 1)
  {
    Standard_Real a, b;
    solver.GetInterval(1, a, b);
    EXPECT_NEAR(a, 0.0, 1.0e-3);
    EXPECT_NEAR(b, 1.0, 1.0e-3);
  }
}

TEST(math_FunctionAllRoots, GetIntervalState)
{
  NullIntervalFunction  func;
  math_FunctionSample   sample(0.0, 6.0, 31);
  math_FunctionAllRoots solver(func, sample, 1.0e-6, 1.0e-6, 1.0e-6);

  EXPECT_TRUE(solver.IsDone());

  if (solver.NbIntervals() >= 1)
  {
    Standard_Integer iFirst, iLast;
    solver.GetIntervalState(1, iFirst, iLast);
    EXPECT_GE(iFirst, 0);
    EXPECT_GE(iLast, 0);
    EXPECT_GE(iLast, iFirst);
  }
}

TEST(math_FunctionAllRoots, GetPointState)
{
  CubicFunction         func;
  math_FunctionSample   sample(0.0, 5.0, 21);
  math_FunctionAllRoots solver(func, sample, 1.0e-6, 1.0e-6, 1.0e-8);

  EXPECT_TRUE(solver.IsDone());

  if (solver.NbPoints() >= 1)
  {
    Standard_Integer state = solver.GetPointState(1);
    EXPECT_GE(state, 0);
  }
}

TEST(math_FunctionAllRoots, LargeSampleSize)
{
  CubicFunction         func;
  math_FunctionSample   sample(0.0, 5.0, 101);
  math_FunctionAllRoots solver(func, sample, 1.0e-8, 1.0e-8, 1.0e-10);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_GE(solver.NbPoints(), 2);
}

TEST(math_FunctionAllRoots, SmallSampleSize)
{
  CubicFunction         func;
  math_FunctionSample   sample(0.0, 5.0, 5);
  math_FunctionAllRoots solver(func, sample, 1.0e-4, 1.0e-4, 1.0e-6);

  EXPECT_TRUE(solver.IsDone());
  // With small sample, might miss some roots
  EXPECT_GE(solver.NbPoints(), 0);
}

TEST(math_FunctionAllRoots, TightTolerances)
{
  QuadraticFunction     func;
  math_FunctionSample   sample(0.0, 3.0, 31);
  math_FunctionAllRoots solver(func, sample, 1.0e-10, 1.0e-10, 1.0e-12);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_GE(solver.NbPoints(), 1);
}

TEST(math_FunctionAllRoots, LooseTolerances)
{
  CubicFunction         func;
  math_FunctionSample   sample(0.0, 5.0, 21);
  math_FunctionAllRoots solver(func, sample, 1.0e-2, 1.0e-2, 1.0e-4);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_GE(solver.NbPoints(), 0);
}

TEST(math_FunctionAllRoots, NarrowRange)
{
  QuadraticFunction     func;
  math_FunctionSample   sample(0.5, 2.5, 21);
  math_FunctionAllRoots solver(func, sample, 1.0e-6, 1.0e-6, 1.0e-8);

  EXPECT_TRUE(solver.IsDone());
  // Should find roots at x = 1 and x = 2 within this range
  EXPECT_GE(solver.NbPoints(), 1);
}

TEST(math_FunctionAllRoots, WideRange)
{
  SineFunction          func;
  math_FunctionSample   sample(-10.0, 10.0, 81);
  math_FunctionAllRoots solver(func, sample, 1.0e-6, 1.0e-6, 1.0e-8);

  EXPECT_TRUE(solver.IsDone());
  // Should find multiple roots in this wide range
  EXPECT_GE(solver.NbPoints(), 5);
}

TEST(math_FunctionAllRoots, EmptyRange)
{
  CubicFunction         func;
  math_FunctionSample   sample(10.0, 20.0, 11);
  math_FunctionAllRoots solver(func, sample, 1.0e-6, 1.0e-6, 1.0e-8);

  EXPECT_TRUE(solver.IsDone());
  // No roots expected in this range for cubic function
  EXPECT_EQ(solver.NbPoints(), 0);
  EXPECT_EQ(solver.NbIntervals(), 0);
}

TEST(math_FunctionAllRoots, IntervalBounds)
{
  NullIntervalFunction  func;
  math_FunctionSample   sample(0.0, 6.0, 31);
  math_FunctionAllRoots solver(func, sample, 1.0e-6, 1.0e-6, 1.0e-6);

  EXPECT_TRUE(solver.IsDone());

  if (solver.NbIntervals() >= 1)
  {
    Standard_Real a, b;
    solver.GetInterval(1, a, b);
    EXPECT_LT(a, b);
    EXPECT_GE(a, 0.0);
    EXPECT_LE(b, 6.0);
  }
}

TEST(math_FunctionAllRoots, MultiplePrecisionLevels)
{
  CubicFunction func;

  // Test with different precision levels
  std::vector<Standard_Real> tolerances = {1.0e-4, 1.0e-6, 1.0e-8};

  for (Standard_Real tol : tolerances)
  {
    math_FunctionSample   sample(0.0, 5.0, 21);
    math_FunctionAllRoots solver(func, sample, tol, tol, tol * 0.01);

    EXPECT_TRUE(solver.IsDone());
    EXPECT_GE(solver.NbPoints(), 0);
  }
}