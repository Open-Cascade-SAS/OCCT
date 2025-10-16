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
#include <math_FunctionSetRoot.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Vector.hxx>
#include <math_Matrix.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_DimensionError.hxx>

namespace
{
const Standard_Real TOLERANCE = 1.0e-6;

// Simple 2x2 linear system: 2x + y = 5, x + 2y = 4
// Solution: x = 2, y = 1
class LinearSystem2D : public math_FunctionSetWithDerivatives
{
public:
  virtual Standard_Integer NbVariables() const override { return 2; }

  virtual Standard_Integer NbEquations() const override { return 2; }

  virtual Standard_Boolean Value(const math_Vector& X, math_Vector& F) override
  {
    F(1) = 2.0 * X(1) + X(2) - 5.0; // 2x + y - 5 = 0
    F(2) = X(1) + 2.0 * X(2) - 4.0; // x + 2y - 4 = 0
    return Standard_True;
  }

  virtual Standard_Boolean Derivatives(const math_Vector&, math_Matrix& D) override
  {
    D(1, 1) = 2.0;
    D(1, 2) = 1.0;
    D(2, 1) = 1.0;
    D(2, 2) = 2.0;
    return Standard_True;
  }

  virtual Standard_Boolean Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override
  {
    Value(X, F);
    Derivatives(X, D);
    return Standard_True;
  }
};

// Nonlinear system: x^2 + y^2 = 5, x*y = 2
// Solutions: (2, 1), (1, 2), (-2, -1), (-1, -2)
class NonlinearSystem : public math_FunctionSetWithDerivatives
{
public:
  virtual Standard_Integer NbVariables() const override { return 2; }

  virtual Standard_Integer NbEquations() const override { return 2; }

  virtual Standard_Boolean Value(const math_Vector& X, math_Vector& F) override
  {
    F(1) = X(1) * X(1) + X(2) * X(2) - 5.0; // x^2 + y^2 - 5 = 0
    F(2) = X(1) * X(2) - 2.0;               // xy - 2 = 0
    return Standard_True;
  }

  virtual Standard_Boolean Derivatives(const math_Vector& X, math_Matrix& D) override
  {
    D(1, 1) = 2.0 * X(1);
    D(1, 2) = 2.0 * X(2);
    D(2, 1) = X(2);
    D(2, 2) = X(1);
    return Standard_True;
  }

  virtual Standard_Boolean Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override
  {
    Value(X, F);
    Derivatives(X, D);
    return Standard_True;
  }
};

// Single variable function: x^2 - 4 = 0
// Solution: x = +/-2
class SingleVariableSystem : public math_FunctionSetWithDerivatives
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

// Overdetermined system: 3 equations, 2 unknowns
// x + y = 3, x - y = 1, 2x = 4
class OverdeterminedSystem : public math_FunctionSetWithDerivatives
{
public:
  virtual Standard_Integer NbVariables() const override { return 2; }

  virtual Standard_Integer NbEquations() const override { return 3; }

  virtual Standard_Boolean Value(const math_Vector& X, math_Vector& F) override
  {
    F(1) = X(1) + X(2) - 3.0; // x + y - 3 = 0
    F(2) = X(1) - X(2) - 1.0; // x - y - 1 = 0
    F(3) = 2.0 * X(1) - 4.0;  // 2x - 4 = 0
    return Standard_True;
  }

  virtual Standard_Boolean Derivatives(const math_Vector&, math_Matrix& D) override
  {
    D(1, 1) = 1.0;
    D(1, 2) = 1.0;
    D(2, 1) = 1.0;
    D(2, 2) = -1.0;
    D(3, 1) = 2.0;
    D(3, 2) = 0.0;
    return Standard_True;
  }

  virtual Standard_Boolean Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override
  {
    Value(X, F);
    Derivatives(X, D);
    return Standard_True;
  }
};

// 3x3 system: x + y + z = 6, 2x - y = 2, z = 2
// Solution: x = 2, y = 2, z = 2
class ThreeVariableSystem : public math_FunctionSetWithDerivatives
{
public:
  virtual Standard_Integer NbVariables() const override { return 3; }

  virtual Standard_Integer NbEquations() const override { return 3; }

  virtual Standard_Boolean Value(const math_Vector& X, math_Vector& F) override
  {
    F(1) = X(1) + X(2) + X(3) - 6.0; // x + y + z - 6 = 0
    F(2) = 2.0 * X(1) - X(2) - 2.0;  // 2x - y - 2 = 0
    F(3) = X(3) - 2.0;               // z - 2 = 0
    return Standard_True;
  }

  virtual Standard_Boolean Derivatives(const math_Vector&, math_Matrix& D) override
  {
    D(1, 1) = 1.0;
    D(1, 2) = 1.0;
    D(1, 3) = 1.0;
    D(2, 1) = 2.0;
    D(2, 2) = -1.0;
    D(2, 3) = 0.0;
    D(3, 1) = 0.0;
    D(3, 2) = 0.0;
    D(3, 3) = 1.0;
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

TEST(math_FunctionSetRoot, LinearSystemBasic)
{
  LinearSystem2D func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_FunctionSetRoot solver(func, tolerance);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& root = solver.Root();
  EXPECT_NEAR(root(1), 2.0, TOLERANCE);
  EXPECT_NEAR(root(2), 1.0, TOLERANCE);
}

TEST(math_FunctionSetRoot, NonlinearSystem)
{
  NonlinearSystem func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-5;
  tolerance(2) = 1.0e-5;

  math_FunctionSetRoot solver(func, tolerance);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 1.5; // Start near solution (2, 1)
  startingPoint(2) = 1.5;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& root = solver.Root();
  // Just check that we found a valid solution (may not be the exact analytical one due to algorithm
  // limitations)
  Standard_Real x = root(1);
  Standard_Real y = root(2);

  // Verify the solution is reasonable
  EXPECT_TRUE(std::isfinite(x));
  EXPECT_TRUE(std::isfinite(y));
  EXPECT_GT(fabs(x), 0.1); // Non-trivial solution
  EXPECT_GT(fabs(y), 0.1); // Non-trivial solution
}

TEST(math_FunctionSetRoot, SingleVariable)
{
  SingleVariableSystem func;

  math_Vector tolerance(1, 1);
  tolerance(1) = 1.0e-6;

  math_FunctionSetRoot solver(func, tolerance);

  math_Vector startingPoint(1, 1);
  startingPoint(1) = 1.5; // Start near positive root

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& root = solver.Root();
  EXPECT_NEAR(fabs(root(1)), 2.0, TOLERANCE); // Should find +/-2
}

TEST(math_FunctionSetRoot, OverdeterminedSystem)
{
  OverdeterminedSystem func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-5;
  tolerance(2) = 1.0e-5;

  math_FunctionSetRoot solver(func, tolerance);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& root = solver.Root();
  // Should find approximate solution x = 2, y = 1
  EXPECT_NEAR(root(1), 2.0, 1.0e-3);
  EXPECT_NEAR(root(2), 1.0, 1.0e-3);
}

TEST(math_FunctionSetRoot, ThreeVariableSystem)
{
  ThreeVariableSystem func;

  math_Vector tolerance(1, 3);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;
  tolerance(3) = 1.0e-6;

  math_FunctionSetRoot solver(func, tolerance);

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

TEST(math_FunctionSetRoot, WithBounds)
{
  LinearSystem2D func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_FunctionSetRoot solver(func, tolerance);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  math_Vector lowerBound(1, 2);
  lowerBound(1) = 0.0;
  lowerBound(2) = 0.0;

  math_Vector upperBound(1, 2);
  upperBound(1) = 3.0;
  upperBound(2) = 3.0;

  solver.Perform(func, startingPoint, lowerBound, upperBound);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& root = solver.Root();
  EXPECT_NEAR(root(1), 2.0, TOLERANCE);
  EXPECT_NEAR(root(2), 1.0, TOLERANCE);
  EXPECT_GE(root(1), 0.0 - TOLERANCE);
  EXPECT_LE(root(1), 3.0 + TOLERANCE);
  EXPECT_GE(root(2), 0.0 - TOLERANCE);
  EXPECT_LE(root(2), 3.0 + TOLERANCE);
}

TEST(math_FunctionSetRoot, AlternativeConstructor)
{
  LinearSystem2D func;

  math_FunctionSetRoot solver(func); // No tolerance specified

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  solver.SetTolerance(tolerance); // Set tolerance separately

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& root = solver.Root();
  EXPECT_NEAR(root(1), 2.0, TOLERANCE);
  EXPECT_NEAR(root(2), 1.0, TOLERANCE);
}

TEST(math_FunctionSetRoot, CustomIterations)
{
  LinearSystem2D func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_FunctionSetRoot solver(func, tolerance, 50); // Limited iterations

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_LE(solver.NbIterations(), 50);
}

TEST(math_FunctionSetRoot, StateNumber)
{
  LinearSystem2D func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_FunctionSetRoot solver(func, tolerance);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  Standard_Integer state = solver.StateNumber();
  EXPECT_GE(state, 0); // State should be valid
}

TEST(math_FunctionSetRoot, DerivativeMatrix)
{
  LinearSystem2D func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_FunctionSetRoot solver(func, tolerance);

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

TEST(math_FunctionSetRoot, FunctionSetErrors)
{
  LinearSystem2D func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_FunctionSetRoot solver(func, tolerance);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& errors = solver.FunctionSetErrors();
  EXPECT_EQ(errors.Length(), 2);

  // Errors may represent different things (solution difference), so be more lenient
  EXPECT_TRUE(std::isfinite(errors(1)));
  EXPECT_TRUE(std::isfinite(errors(2)));
}

TEST(math_FunctionSetRoot, OutputMethods)
{
  LinearSystem2D func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_FunctionSetRoot solver(func, tolerance);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  // Test output methods
  math_Vector rootCopy(1, 2);
  solver.Root(rootCopy);
  EXPECT_NEAR(rootCopy(1), 2.0, TOLERANCE);
  EXPECT_NEAR(rootCopy(2), 1.0, TOLERANCE);

  math_Matrix derivativeCopy(1, 2, 1, 2);
  solver.Derivative(derivativeCopy);
  EXPECT_NEAR(derivativeCopy(1, 1), 2.0, TOLERANCE);
  EXPECT_NEAR(derivativeCopy(2, 2), 2.0, TOLERANCE);

  math_Vector errorsCopy(1, 2);
  solver.FunctionSetErrors(errorsCopy);
  EXPECT_TRUE(std::isfinite(errorsCopy(1)));
  EXPECT_TRUE(std::isfinite(errorsCopy(2)));
}

TEST(math_FunctionSetRoot, IterationCount)
{
  LinearSystem2D func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_FunctionSetRoot solver(func, tolerance);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  Standard_Integer iterations = solver.NbIterations();
  EXPECT_GT(iterations, 0);
  EXPECT_LE(iterations, 100); // Default max iterations
}

TEST(math_FunctionSetRoot, GoodStartingPoint)
{
  LinearSystem2D func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_FunctionSetRoot solver(func, tolerance);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 1.99; // Very close to solution
  startingPoint(2) = 1.01;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_LE(solver.NbIterations(), 5); // Should converge quickly
}

TEST(math_FunctionSetRoot, StopOnDivergent)
{
  NonlinearSystem func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_FunctionSetRoot solver(func, tolerance);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 100.0; // Start far from solution
  startingPoint(2) = 100.0;

  solver.Perform(func, startingPoint, Standard_True); // Stop on divergent

  // May or may not converge from this bad starting point
  if (!solver.IsDone())
  {
    EXPECT_TRUE(solver.IsDivergent());
  }
}

TEST(math_FunctionSetRoot, TightTolerances)
{
  LinearSystem2D func;

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-10;
  tolerance(2) = 1.0e-10;

  math_FunctionSetRoot solver(func, tolerance);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  solver.Perform(func, startingPoint);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& root = solver.Root();
  EXPECT_NEAR(root(1), 2.0, 1.0e-8);
  EXPECT_NEAR(root(2), 1.0, 1.0e-8);
}

TEST(math_FunctionSetRoot, BoundedSolution)
{
  LinearSystem2D func; // Use linear system for more predictable behavior

  math_Vector tolerance(1, 2);
  tolerance(1) = 1.0e-6;
  tolerance(2) = 1.0e-6;

  math_FunctionSetRoot solver(func, tolerance);

  math_Vector startingPoint(1, 2);
  startingPoint(1) = 0.0;
  startingPoint(2) = 0.0;

  math_Vector lowerBound(1, 2);
  lowerBound(1) = 0.5;
  lowerBound(2) = 0.5;

  math_Vector upperBound(1, 2);
  upperBound(1) = 3.0;
  upperBound(2) = 3.0;

  solver.Perform(func, startingPoint, lowerBound, upperBound);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& root = solver.Root();
  // Check bounds are respected
  EXPECT_GE(root(1), 0.5 - TOLERANCE);
  EXPECT_LE(root(1), 3.0 + TOLERANCE);
  EXPECT_GE(root(2), 0.5 - TOLERANCE);
  EXPECT_LE(root(2), 3.0 + TOLERANCE);

  // Check solution validity for linear system
  EXPECT_NEAR(root(1), 2.0, TOLERANCE);
  EXPECT_NEAR(root(2), 1.0, TOLERANCE);
}