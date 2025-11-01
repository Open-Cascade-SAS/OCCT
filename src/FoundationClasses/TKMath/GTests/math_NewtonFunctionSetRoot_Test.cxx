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
#include <math_NewtonFunctionSetRoot.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Vector.hxx>
#include <math_Matrix.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_DimensionError.hxx>

#ifndef M_SQRT1_2
  #define M_SQRT1_2 0.707106781186547524400844362104849039
#endif

namespace
{
const Standard_Real TOLERANCE = 1.0e-6;

// Simple 2x2 system: x^2 + y^2 = 1, x - y = 0
// Solution: x = y = +/-1/sqrt(2)
class CircleLineSystem : public math_FunctionSetWithDerivatives
{
public:
  virtual Standard_Integer NbVariables() const override { return 2; }

  virtual Standard_Integer NbEquations() const override { return 2; }

  virtual Standard_Boolean Value(const math_Vector& X, math_Vector& F) override
  {
    F(1) = X(1) * X(1) + X(2) * X(2) - 1.0; // x^2 + y^2 - 1 = 0
    F(2) = X(1) - X(2);                     // x - y = 0
    return Standard_True;
  }

  virtual Standard_Boolean Derivatives(const math_Vector& X, math_Matrix& D) override
  {
    D(1, 1) = 2.0 * X(1); // df1/dx = 2x
    D(1, 2) = 2.0 * X(2); // df1/dy = 2y
    D(2, 1) = 1.0;        // df2/dx = 1
    D(2, 2) = -1.0;       // df2/dy = -1
    return Standard_True;
  }

  virtual Standard_Boolean Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override
  {
    Value(X, F);
    Derivatives(X, D);
    return Standard_True;
  }
};

// Linear system: 2x + y = 3, x + 2y = 3
// Solution: x = 1, y = 1
class LinearSystem : public math_FunctionSetWithDerivatives
{
public:
  virtual Standard_Integer NbVariables() const override { return 2; }

  virtual Standard_Integer NbEquations() const override { return 2; }

  virtual Standard_Boolean Value(const math_Vector& X, math_Vector& F) override
  {
    F(1) = 2.0 * X(1) + X(2) - 3.0; // 2x + y - 3 = 0
    F(2) = X(1) + 2.0 * X(2) - 3.0; // x + 2y - 3 = 0
    return Standard_True;
  }

  virtual Standard_Boolean Derivatives(const math_Vector&, math_Matrix& D) override
  {
    D(1, 1) = 2.0; // df1/dx = 2
    D(1, 2) = 1.0; // df1/dy = 1
    D(2, 1) = 1.0; // df2/dx = 1
    D(2, 2) = 2.0; // df2/dy = 2
    return Standard_True;
  }

  virtual Standard_Boolean Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override
  {
    Value(X, F);
    Derivatives(X, D);
    return Standard_True;
  }
};

// Single equation: x^2 - 4 = 0
// Solution: x = +/-2
class QuadraticFunction : public math_FunctionSetWithDerivatives
{
public:
  virtual Standard_Integer NbVariables() const override { return 1; }

  virtual Standard_Integer NbEquations() const override { return 1; }

  virtual Standard_Boolean Value(const math_Vector& X, math_Vector& F) override
  {
    F(1) = X(1) * X(1) - 4.0;
    return Standard_True;
  }

  virtual Standard_Boolean Derivatives(const math_Vector& X, math_Matrix& D) override
  {
    D(1, 1) = 2.0 * X(1);
    return Standard_True;
  }

  virtual Standard_Boolean Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override
  {
    Value(X, F);
    Derivatives(X, D);
    return Standard_True;
  }
};

// 3x3 system: x + y + z = 6, x - y = 0, 2z = 4
// Solution: x = 2, y = 2, z = 2
class ThreeVariableSystem : public math_FunctionSetWithDerivatives
{
public:
  virtual Standard_Integer NbVariables() const override { return 3; }

  virtual Standard_Integer NbEquations() const override { return 3; }

  virtual Standard_Boolean Value(const math_Vector& X, math_Vector& F) override
  {
    F(1) = X(1) + X(2) + X(3) - 6.0; // x + y + z - 6 = 0
    F(2) = X(1) - X(2);              // x - y = 0
    F(3) = 2.0 * X(3) - 4.0;         // 2z - 4 = 0
    return Standard_True;
  }

  virtual Standard_Boolean Derivatives(const math_Vector& X, math_Matrix& D) override
  {
    (void)X;
    D(1, 1) = 1.0;
    D(1, 2) = 1.0;
    D(1, 3) = 1.0;
    D(2, 1) = 1.0;
    D(2, 2) = -1.0;
    D(2, 3) = 0.0;
    D(3, 1) = 0.0;
    D(3, 2) = 0.0;
    D(3, 3) = 2.0;
    return Standard_True;
  }

  virtual Standard_Boolean Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override
  {
    Value(X, F);
    Derivatives(X, D);
    return Standard_True;
  }
};
} // namespace

TEST(math_NewtonFunctionSetRoot, LinearSystemBasic)
{
  LinearSystem func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_NewtonFunctionSetRoot solver(func, tolerance, 1.0e-6);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& root = solver.Root();
  EXPECT_NEAR(root(1), 1.0, TOLERANCE);
  EXPECT_NEAR(root(2), 1.0, TOLERANCE);
}

TEST(math_NewtonFunctionSetRoot, QuadraticSingleVariable)
{
  QuadraticFunction func;

  math_Vector tolerance(1, 1);
  tolerance(1) = 1.0e-6;

  math_NewtonFunctionSetRoot solver(func, tolerance, 1.0e-6);

  math_Vector startingPoint(1, 1);
  startingPoint(1) = 1.5; // Start near positive root

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& root = solver.Root();
  EXPECT_NEAR(fabs(root(1)), 2.0, TOLERANCE); // Should find +/-2
}

TEST(math_NewtonFunctionSetRoot, CircleLineIntersection)
{
  CircleLineSystem func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_NewtonFunctionSetRoot solver(func, tolerance, 1.0e-6);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.7; // Start near positive solution
  startingPoint(2) = 0.7;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& root = solver.Root();
  EXPECT_NEAR(fabs(root(1)), M_SQRT1_2, 1.0e-5);
  EXPECT_NEAR(fabs(root(2)), M_SQRT1_2, 1.0e-5);
  EXPECT_NEAR(root(1), root(2), TOLERANCE); // x = y constraint
}

TEST(math_NewtonFunctionSetRoot, ThreeVariableSystem)
{
  ThreeVariableSystem func;

  math_Vector tolerance(1, 3);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;
  tolerance(3) = 1.0e-6;

  math_NewtonFunctionSetRoot solver(func, tolerance, 1.0e-6);

  math_Vector startingPoint(1, 3);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;
  startingPoint(3) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& root = solver.Root();
  EXPECT_NEAR(root(1), 2.0, TOLERANCE);
  EXPECT_NEAR(root(2), 2.0, TOLERANCE);
  EXPECT_NEAR(root(3), 2.0, TOLERANCE);
}

TEST(math_NewtonFunctionSetRoot, WithBounds)
{
  LinearSystem func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_NewtonFunctionSetRoot solver(func, tolerance, 1.0e-6);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  math_Vector lowerBound(1, 2);
  lowerBound(1) = 0.0;
  lowerBound(2) = 0.0;

  math_Vector upperBound(1, 2);
  upperBound(1) = 2.0;
  upperBound(2) = 2.0;

  solver.Perform(func, startingPoint, lowerBound, upperBound);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& root = solver.Root();
  EXPECT_NEAR(root(1), 1.0, TOLERANCE);
  EXPECT_NEAR(root(2), 1.0, TOLERANCE);
  EXPECT_GE(root(1), 0.0 - TOLERANCE);
  EXPECT_LE(root(1), 2.0 + TOLERANCE);
  EXPECT_GE(root(2), 0.0 - TOLERANCE);
  EXPECT_LE(root(2), 2.0 + TOLERANCE);
}

TEST(math_NewtonFunctionSetRoot, AlternativeConstructor)
{
  LinearSystem func;

  math_NewtonFunctionSetRoot solver(func, 1.0e-6); // Only function tolerance

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  solver.SetTolerance(tolerance); // Set x tolerance separately

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& root = solver.Root();
  EXPECT_NEAR(root(1), 1.0, TOLERANCE);
  EXPECT_NEAR(root(2), 1.0, TOLERANCE);
}

TEST(math_NewtonFunctionSetRoot, CustomIterations)
{
  LinearSystem func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_NewtonFunctionSetRoot solver(func, tolerance, 1.0e-6, 10); // Limited iterations

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_LE(solver.NbIterations(), 10);
}

TEST(math_NewtonFunctionSetRoot, ConvergenceIterations)
{
  LinearSystem func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_NewtonFunctionSetRoot solver(func, tolerance, 1.0e-6);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_GT(solver.NbIterations(), 0);
  EXPECT_LT(solver.NbIterations(), 20); // Should converge reasonably fast
}

TEST(math_NewtonFunctionSetRoot, DerivativeMatrix)
{
  LinearSystem func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_NewtonFunctionSetRoot solver(func, tolerance, 1.0e-6);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  const math_Matrix& jacobian = solver.Derivative();
  EXPECT_EQ(jacobian.RowNumber(), 2);
  EXPECT_EQ(jacobian.ColNumber(), 2);

  // For linear system, Jacobian should be constant
  EXPECT_NEAR(jacobian(1, 1), 2.0, TOLERANCE);
  EXPECT_NEAR(jacobian(1, 2), 1.0, TOLERANCE);
  EXPECT_NEAR(jacobian(2, 1), 1.0, TOLERANCE);
  EXPECT_NEAR(jacobian(2, 2), 2.0, TOLERANCE);
}

TEST(math_NewtonFunctionSetRoot, FunctionSetErrors)
{
  LinearSystem func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_NewtonFunctionSetRoot solver(func, tolerance, 1.0e-6);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& errors = solver.FunctionSetErrors();
  EXPECT_EQ(errors.Length(), 2);

  // Errors should be very small at the root
  EXPECT_LT(fabs(errors(1)), 1.0e-5);
  EXPECT_LT(fabs(errors(2)), 1.0e-5);
}

TEST(math_NewtonFunctionSetRoot, OutputMethods)
{
  LinearSystem func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_NewtonFunctionSetRoot solver(func, tolerance, 1.0e-6);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  // Test output methods
  math_Vector rootCopy(1, 2);
  solver.Root(rootCopy);
  EXPECT_NEAR(rootCopy(1), 1.0, TOLERANCE);
  EXPECT_NEAR(rootCopy(2), 1.0, TOLERANCE);

  math_Matrix derivativeCopy(1, 2, 1, 2);
  solver.Derivative(derivativeCopy);
  EXPECT_NEAR(derivativeCopy(1, 1), 2.0, TOLERANCE);
  EXPECT_NEAR(derivativeCopy(2, 2), 2.0, TOLERANCE);

  math_Vector errorsCopy(1, 2);
  solver.FunctionSetErrors(errorsCopy);
  EXPECT_LT(fabs(errorsCopy(1)), 1.0e-5);
  EXPECT_LT(fabs(errorsCopy(2)), 1.0e-5);
}

TEST(math_NewtonFunctionSetRoot, IterationCount)
{
  LinearSystem func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_NewtonFunctionSetRoot solver(func, tolerance, 1.0e-6);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  Standard_Integer iterations = solver.NbIterations();
  EXPECT_GT(iterations, 0);
  EXPECT_LE(iterations, 100); // Default max iterations
}

TEST(math_NewtonFunctionSetRoot, GoodStartingPoint)
{
  LinearSystem func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_NewtonFunctionSetRoot solver(func, tolerance, 1.0e-6);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.99; // Very close to solution
  startingPoint(2) = 1.01;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_LE(solver.NbIterations(), 5); // Should converge quickly
}

TEST(math_NewtonFunctionSetRoot, TightTolerances)
{
  LinearSystem func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-10;
  tolerance(2) = 1.0e-10;

  math_NewtonFunctionSetRoot solver(func, tolerance, 1.0e-10);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& root = solver.Root();
  EXPECT_NEAR(root(1), 1.0, 1.0e-8);
  EXPECT_NEAR(root(2), 1.0, 1.0e-8);
}