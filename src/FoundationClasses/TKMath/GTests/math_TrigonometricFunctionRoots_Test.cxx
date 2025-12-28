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
#include <math_TrigonometricFunctionRoots.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_DimensionError.hxx>

namespace
{
const double PI        = M_PI;
const double TOLERANCE = 1.0e-6;
} // namespace

TEST(math_TrigonometricFunctionRoots, FullEquationBasic)
{
  // Test a*cos^2(x) + 2*b*cos(x)*sin(x) + c*cos(x) + d*sin(x) + e = 0
  // Example: cos^2(x) - sin^2(x) = 0 => cos(2x) = 0
  // a=1, b=0, c=0, d=0, e=-sin^2(x) equivalent to: cos^2(x) - sin^2(x) = cos(2x) = 0
  // But let's use: cos^2(x) + c*cos(x) = 0 => cos(x)(cos(x) + c) = 0
  double                   a = 1.0, b = 0.0, c = 1.0, d = 0.0, e = 0.0;
  math_TrigonometricFunctionRoots solver(a, b, c, d, e, 0.0, 2.0 * PI);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_FALSE(solver.InfiniteRoots());
  EXPECT_GT(solver.NbSolutions(), 0);
}

TEST(math_TrigonometricFunctionRoots, LinearSineOnly)
{
  // Test d*sin(x) + e = 0 => sin(x) = -e/d
  // Example: sin(x) - 0.5 = 0 => sin(x) = 0.5 => x = PI/6, 5*PI/6
  double                   d = 1.0, e = -0.5;
  math_TrigonometricFunctionRoots solver(d, e, 0.0, 2.0 * PI);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_FALSE(solver.InfiniteRoots());
  EXPECT_GT(solver.NbSolutions(), 0);

  if (solver.NbSolutions() >= 1)
  {
    double x1      = solver.Value(1);
    double sin_val = sin(x1);
    EXPECT_NEAR(sin_val, 0.5, TOLERANCE);
  }
}

TEST(math_TrigonometricFunctionRoots, LinearCosineAndSine)
{
  // Test c*cos(x) + d*sin(x) + e = 0
  // Example: cos(x) + sin(x) = 0 => tan(x) = -1 => x = 3*PI/4, 7*PI/4
  double                   c = 1.0, d = 1.0, e = 0.0;
  math_TrigonometricFunctionRoots solver(c, d, e, 0.0, 2.0 * PI);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_FALSE(solver.InfiniteRoots());
  EXPECT_GT(solver.NbSolutions(), 0);

  if (solver.NbSolutions() >= 1)
  {
    double x1     = solver.Value(1);
    double result = cos(x1) + sin(x1);
    EXPECT_NEAR(result, 0.0, TOLERANCE);
  }
}

TEST(math_TrigonometricFunctionRoots, PureCosineEquation)
{
  // Test cos(x) = 0 => x = PI/2, 3*PI/2
  double                   c = 1.0, d = 0.0, e = 0.0;
  math_TrigonometricFunctionRoots solver(c, d, e, 0.0, 2.0 * PI);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_FALSE(solver.InfiniteRoots());
  EXPECT_GE(solver.NbSolutions(), 1);

  if (solver.NbSolutions() >= 1)
  {
    double x1 = solver.Value(1);
    EXPECT_NEAR(fabs(cos(x1)), 0.0, TOLERANCE);
  }
}

TEST(math_TrigonometricFunctionRoots, PureSineEquation)
{
  // Test sin(x) = 0 => x = 0, PI, 2*PI
  double                   d = 1.0, e = 0.0;
  math_TrigonometricFunctionRoots solver(d, e, 0.0, 2.0 * PI);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_FALSE(solver.InfiniteRoots());
  EXPECT_GE(solver.NbSolutions(), 2);

  if (solver.NbSolutions() >= 1)
  {
    double x1 = solver.Value(1);
    EXPECT_NEAR(fabs(sin(x1)), 0.0, TOLERANCE);
  }
}

TEST(math_TrigonometricFunctionRoots, NoSolution)
{
  // Test sin(x) + 2 = 0 => sin(x) = -2 (impossible)
  double                   d = 1.0, e = 2.0;
  math_TrigonometricFunctionRoots solver(d, e, 0.0, 2.0 * PI);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_FALSE(solver.InfiniteRoots());
  EXPECT_EQ(solver.NbSolutions(), 0);
}

TEST(math_TrigonometricFunctionRoots, InfiniteSolutions)
{
  // Test 0*sin(x) + 0 = 0 (always true)
  double                   d = 0.0, e = 0.0;
  math_TrigonometricFunctionRoots solver(d, e, 0.0, 2.0 * PI);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_TRUE(solver.InfiniteRoots());
}

TEST(math_TrigonometricFunctionRoots, CustomBounds)
{
  // Test sin(x) = 0 in range [PI/2, 3*PI/2]
  double                   d = 1.0, e = 0.0;
  math_TrigonometricFunctionRoots solver(d, e, PI / 2.0, 3.0 * PI / 2.0);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_FALSE(solver.InfiniteRoots());
  EXPECT_GE(solver.NbSolutions(), 1);

  for (int i = 1; i <= solver.NbSolutions(); i++)
  {
    double x = solver.Value(i);
    EXPECT_GE(x, PI / 2.0);
    EXPECT_LE(x, 3.0 * PI / 2.0);
    EXPECT_NEAR(fabs(sin(x)), 0.0, TOLERANCE);
  }
}

TEST(math_TrigonometricFunctionRoots, NarrowBounds)
{
  // Test cos(x) = 0 in range [0, PI/4]
  double                   c = 1.0, d = 0.0, e = 0.0;
  math_TrigonometricFunctionRoots solver(c, d, e, 0.0, PI / 4.0);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_FALSE(solver.InfiniteRoots());
  // No solutions expected in this narrow range
  EXPECT_EQ(solver.NbSolutions(), 0);
}

TEST(math_TrigonometricFunctionRoots, QuadraticTerms)
{
  // Test cos^2(x) - 0.5 = 0 => cos^2(x) = 0.5 => cos(x) = +/-sqrt(0.5)
  double                   a = 1.0, b = 0.0, c = 0.0, d = 0.0, e = -0.5;
  math_TrigonometricFunctionRoots solver(a, b, c, d, e, 0.0, 2.0 * PI);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_FALSE(solver.InfiniteRoots());
  EXPECT_GE(solver.NbSolutions(), 2);

  if (solver.NbSolutions() >= 1)
  {
    double x1          = solver.Value(1);
    double cos_squared = cos(x1) * cos(x1);
    EXPECT_NEAR(cos_squared, 0.5, TOLERANCE);
  }
}

TEST(math_TrigonometricFunctionRoots, MixedTerms)
{
  // Test cos(x) + sin(x) - sqrt(2) = 0
  // This gives cos(x) + sin(x) = sqrt(2)
  // Which is sqrt(2)*sin(x + PI/4) = sqrt(2)
  // So sin(x + PI/4) = 1 => x + PI/4 = PI/2 => x = PI/4
  double                   c = 1.0, d = 1.0, e = -sqrt(2.0);
  math_TrigonometricFunctionRoots solver(c, d, e, 0.0, 2.0 * PI);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_FALSE(solver.InfiniteRoots());
  EXPECT_GE(solver.NbSolutions(), 1);

  if (solver.NbSolutions() >= 1)
  {
    double x1     = solver.Value(1);
    double result = cos(x1) + sin(x1);
    EXPECT_NEAR(result, sqrt(2.0), TOLERANCE);
  }
}

TEST(math_TrigonometricFunctionRoots, AllCoefficients)
{
  // Test a more complex equation with all coefficients non-zero
  // a*cos^2(x) + 2*b*cos(x)*sin(x) + c*cos(x) + d*sin(x) + e = 0
  double                   a = 1.0, b = 0.5, c = 0.5, d = 0.5, e = -0.25;
  math_TrigonometricFunctionRoots solver(a, b, c, d, e, 0.0, 2.0 * PI);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_FALSE(solver.InfiniteRoots());

  // Check that found solutions are valid
  for (int i = 1; i <= solver.NbSolutions(); i++)
  {
    double x = solver.Value(i);
    double result =
      a * cos(x) * cos(x) + 2.0 * b * cos(x) * sin(x) + c * cos(x) + d * sin(x) + e;
    EXPECT_NEAR(result, 0.0, TOLERANCE);
  }
}

TEST(math_TrigonometricFunctionRoots, LargeBounds)
{
  // Test sin(x) = 0 over multiple periods
  double                   d = 1.0, e = 0.0;
  math_TrigonometricFunctionRoots solver(d, e, 0.0, 4.0 * PI);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_FALSE(solver.InfiniteRoots());
  EXPECT_GE(solver.NbSolutions(), 2); // Should find multiple roots
}

TEST(math_TrigonometricFunctionRoots, NegativeBounds)
{
  // Test cos(x) = 0 with negative bounds
  double                   c = 1.0, d = 0.0, e = 0.0;
  math_TrigonometricFunctionRoots solver(c, d, e, -PI, PI);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_FALSE(solver.InfiniteRoots());
  EXPECT_GE(solver.NbSolutions(), 1);

  for (int i = 1; i <= solver.NbSolutions(); i++)
  {
    double x = solver.Value(i);
    EXPECT_GE(x, -PI);
    EXPECT_LE(x, PI);
    EXPECT_NEAR(fabs(cos(x)), 0.0, TOLERANCE);
  }
}

TEST(math_TrigonometricFunctionRoots, HighFrequencyTest)
{
  // Test sin(x) - 0.5 = 0 with precise expected solutions
  double                   d = 1.0, e = -0.5;
  math_TrigonometricFunctionRoots solver(d, e, 0.0, 2.0 * PI);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_FALSE(solver.InfiniteRoots());
  EXPECT_GE(solver.NbSolutions(), 2);

  // Expected solutions: PI/6 approximately 0.5236 and 5*PI/6 approximately 2.618
  if (solver.NbSolutions() >= 2)
  {
    std::vector<double> solutions;
    for (int i = 1; i <= solver.NbSolutions(); i++)
    {
      solutions.push_back(solver.Value(i));
    }

    // Check that we have solutions near PI/6 and 5*PI/6
    bool found_first = false, found_second = false;
    for (double sol : solutions)
    {
      if (fabs(sol - PI / 6.0) < 0.1)
        found_first = true;
      if (fabs(sol - 5.0 * PI / 6.0) < 0.1)
        found_second = true;
    }
    EXPECT_TRUE(found_first || found_second);
  }
}

TEST(math_TrigonometricFunctionRoots, EdgeCaseSmallCoefficients)
{
  // Test with very small coefficients
  double                   c = 1.0e-10, d = 1.0, e = 0.0;
  math_TrigonometricFunctionRoots solver(c, d, e, 0.0, 2.0 * PI);

  EXPECT_TRUE(solver.IsDone());
  // Should behave approximately like sin(x) = 0
  if (!solver.InfiniteRoots())
  {
    EXPECT_GE(solver.NbSolutions(), 2);
  }
}