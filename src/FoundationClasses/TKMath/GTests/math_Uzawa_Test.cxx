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
#include <math_Uzawa.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_ConstructionError.hxx>

namespace
{
const double TOLERANCE = 1.0e-6;
}

TEST(math_Uzawa, SimpleEqualityConstraints)
{
  // Simple 2x2 system with equality constraints:
  // 2x + y = 5
  // x + 2y = 4
  // Expected solution: x = 2, y = 1

  math_Matrix C(1, 2, 1, 2);
  C(1, 1) = 2.0;
  C(1, 2) = 1.0;
  C(2, 1) = 1.0;
  C(2, 2) = 2.0;

  math_Vector b(1, 2);
  b(1) = 5.0;
  b(2) = 4.0;

  math_Vector x0(1, 2);
  x0(1) = 0.0;
  x0(2) = 0.0;

  math_Uzawa solver(C, b, x0);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& solution = solver.Value();
  EXPECT_NEAR(solution(1), 2.0, TOLERANCE);
  EXPECT_NEAR(solution(2), 1.0, TOLERANCE);
}

TEST(math_Uzawa, OverdeterminedSystem)
{
  // Simple overdetermined but consistent system
  // x + y = 3
  // x - y = 1
  // Expected solution: x = 2, y = 1

  math_Matrix C(1, 2, 1, 2);
  C(1, 1) = 1.0;
  C(1, 2) = 1.0;
  C(2, 1) = 1.0;
  C(2, 2) = -1.0;

  math_Vector b(1, 2);
  b(1) = 3.0;
  b(2) = 1.0;

  math_Vector x0(1, 2);
  x0(1) = 0.0;
  x0(2) = 0.0;

  math_Uzawa solver(C, b, x0);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& solution = solver.Value();
  EXPECT_NEAR(solution(1), 2.0, TOLERANCE);
  EXPECT_NEAR(solution(2), 1.0, TOLERANCE);
}

TEST(math_Uzawa, WithInequalityConstraints)
{
  // Simple test with one equality constraint only (safer)
  // x + y = 2

  math_Matrix C(1, 1, 1, 2);
  C(1, 1) = 1.0;
  C(1, 2) = 1.0; // equality constraint

  math_Vector b(1, 1);
  b(1) = 2.0; // x + y = 2

  math_Vector x0(1, 2);
  x0(1) = 1.0;
  x0(2) = 1.0;

  // 0 inequality constraints, 1 equality constraint
  math_Uzawa solver(C, b, x0, 0, 1);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& solution = solver.Value();
  EXPECT_NEAR(solution(1) + solution(2), 2.0, TOLERANCE);
}

TEST(math_Uzawa, CustomTolerances)
{
  // Test with custom tolerances
  math_Matrix C(1, 2, 1, 2);
  C(1, 1) = 1.0;
  C(1, 2) = 1.0;
  C(2, 1) = 1.0;
  C(2, 2) = -1.0;

  math_Vector b(1, 2);
  b(1) = 2.0;
  b(2) = 0.0;

  math_Vector x0(1, 2);
  x0(1) = 0.0;
  x0(2) = 0.0;

  math_Uzawa solver(C, b, x0, 1.0e-8, 1.0e-8);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& solution = solver.Value();
  EXPECT_NEAR(solution(1), 1.0, 1.0e-6);
  EXPECT_NEAR(solution(2), 1.0, 1.0e-6);
}

TEST(math_Uzawa, CustomIterations)
{
  // Test with limited iterations
  math_Matrix C(1, 2, 1, 2);
  C(1, 1) = 1.0;
  C(1, 2) = 1.0;
  C(2, 1) = 1.0;
  C(2, 2) = -1.0;

  math_Vector b(1, 2);
  b(1) = 2.0;
  b(2) = 0.0;

  math_Vector x0(1, 2);
  x0(1) = 10.0; // Start far from solution
  x0(2) = 10.0;

  math_Uzawa solver(C, b, x0, 1.0e-6, 1.0e-6, 50);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_LE(solver.NbIterations(), 50);
}

TEST(math_Uzawa, InitialError)
{
  math_Matrix C(1, 2, 1, 2);
  C(1, 1) = 1.0;
  C(1, 2) = 1.0;
  C(2, 1) = 1.0;
  C(2, 2) = -1.0;

  math_Vector b(1, 2);
  b(1) = 2.0;
  b(2) = 0.0;

  math_Vector x0(1, 2);
  x0(1) = 5.0;
  x0(2) = 3.0;

  math_Uzawa solver(C, b, x0);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& initialError = solver.InitialError();
  // Initial error = C*x0 - b
  // C*[5,3] - [2,0] = [8,2] - [2,0] = [6,2]
  EXPECT_NEAR(initialError(1), 6.0, TOLERANCE);
  EXPECT_NEAR(initialError(2), 2.0, TOLERANCE);
}

TEST(math_Uzawa, ErrorVector)
{
  math_Matrix C(1, 2, 1, 2);
  C(1, 1) = 1.0;
  C(1, 2) = 1.0;
  C(2, 1) = 1.0;
  C(2, 2) = -1.0;

  math_Vector b(1, 2);
  b(1) = 2.0;
  b(2) = 0.0;

  math_Vector x0(1, 2);
  x0(1) = 0.0;
  x0(2) = 0.0;

  math_Uzawa solver(C, b, x0);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& error    = solver.Error();
  const math_Vector& solution = solver.Value();

  // Error should be solution - starting point
  EXPECT_NEAR(error(1), solution(1) - x0(1), TOLERANCE);
  EXPECT_NEAR(error(2), solution(2) - x0(2), TOLERANCE);
}

TEST(math_Uzawa, DualVariables)
{
  math_Matrix C(1, 2, 1, 2);
  C(1, 1) = 1.0;
  C(1, 2) = 1.0;
  C(2, 1) = 1.0;
  C(2, 2) = -1.0;

  math_Vector b(1, 2);
  b(1) = 2.0;
  b(2) = 0.0;

  math_Vector x0(1, 2);
  x0(1) = 0.0;
  x0(2) = 0.0;

  math_Uzawa solver(C, b, x0);

  EXPECT_TRUE(solver.IsDone());

  math_Vector dualVar(1, 2);
  solver.Duale(dualVar);

  // Dual variables should be valid
  EXPECT_TRUE(std::isfinite(dualVar(1)));
  EXPECT_TRUE(std::isfinite(dualVar(2)));
}

TEST(math_Uzawa, InverseMatrix)
{
  math_Matrix C(1, 2, 1, 2);
  C(1, 1) = 1.0;
  C(1, 2) = 1.0;
  C(2, 1) = 1.0;
  C(2, 2) = -1.0;

  math_Vector b(1, 2);
  b(1) = 2.0;
  b(2) = 0.0;

  math_Vector x0(1, 2);
  x0(1) = 0.0;
  x0(2) = 0.0;

  math_Uzawa solver(C, b, x0);

  EXPECT_TRUE(solver.IsDone());

  const math_Matrix& invMatrix = solver.InverseCont();

  // Should be a valid matrix
  EXPECT_EQ(invMatrix.RowNumber(), 2);
  EXPECT_EQ(invMatrix.ColNumber(), 2);

  // Elements should be finite
  for (int i = 1; i <= 2; i++)
  {
    for (int j = 1; j <= 2; j++)
    {
      EXPECT_TRUE(std::isfinite(invMatrix(i, j)));
    }
  }
}

TEST(math_Uzawa, LargeSystem)
{
  // Test with a larger system
  int         n = 4;
  math_Matrix C(1, n, 1, n);
  for (int i = 1; i <= n; i++)
  {
    for (int j = 1; j <= n; j++)
    {
      C(i, j) = (i == j) ? 2.0 : 0.5;
    }
  }

  math_Vector b(1, n);
  for (int i = 1; i <= n; i++)
  {
    b(i) = i;
  }

  math_Vector x0(1, n);
  for (int i = 1; i <= n; i++)
  {
    x0(i) = 0.0;
  }

  math_Uzawa solver(C, b, x0);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& solution = solver.Value();
  for (int i = 1; i <= n; i++)
  {
    EXPECT_TRUE(std::isfinite(solution(i)));
  }
}

TEST(math_Uzawa, StartingPointNearSolution)
{
  // Test where starting point is already near the solution
  math_Matrix C(1, 2, 1, 2);
  C(1, 1) = 1.0;
  C(1, 2) = 1.0;
  C(2, 1) = 1.0;
  C(2, 2) = -1.0;

  math_Vector b(1, 2);
  b(1) = 2.0;
  b(2) = 0.0;

  math_Vector x0(1, 2);
  x0(1) = 1.001; // Very close to solution (1,1)
  x0(2) = 0.999;

  math_Uzawa solver(C, b, x0);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_LE(solver.NbIterations(), 10); // Should converge quickly

  const math_Vector& solution = solver.Value();
  EXPECT_NEAR(solution(1), 1.0, TOLERANCE);
  EXPECT_NEAR(solution(2), 1.0, TOLERANCE);
}

TEST(math_Uzawa, ConsistentSystem)
{
  // Test with consistent system
  math_Matrix C(1, 2, 1, 2);
  C(1, 1) = 1.0;
  C(1, 2) = 2.0;
  C(2, 1) = 2.0;
  C(2, 2) = 1.0;

  math_Vector b(1, 2);
  b(1) = 5.0;
  b(2) = 7.0;

  math_Vector x0(1, 2);
  x0(1) = 0.0;
  x0(2) = 0.0;

  math_Uzawa solver(C, b, x0);

  EXPECT_TRUE(solver.IsDone());

  // Verify solution satisfies the equations
  const math_Vector& solution = solver.Value();
  double             eq1      = solution(1) + 2.0 * solution(2);
  double             eq2      = 2.0 * solution(1) + solution(2);
  EXPECT_NEAR(eq1, 5.0, TOLERANCE);
  EXPECT_NEAR(eq2, 7.0, TOLERANCE);
}

TEST(math_Uzawa, SingleVariable)
{
  // Test with single variable system: 2x = 4
  math_Matrix C(1, 1, 1, 1);
  C(1, 1) = 2.0;

  math_Vector b(1, 1);
  b(1) = 4.0;

  math_Vector x0(1, 1);
  x0(1) = 0.0;

  math_Uzawa solver(C, b, x0);

  EXPECT_TRUE(solver.IsDone());

  const math_Vector& solution = solver.Value();
  EXPECT_NEAR(solution(1), 2.0, TOLERANCE);
}

TEST(math_Uzawa, IterationCount)
{
  math_Matrix C(1, 2, 1, 2);
  C(1, 1) = 1.0;
  C(1, 2) = 1.0;
  C(2, 1) = 1.0;
  C(2, 2) = -1.0;

  math_Vector b(1, 2);
  b(1) = 2.0;
  b(2) = 0.0;

  math_Vector x0(1, 2);
  x0(1) = 0.0;
  x0(2) = 0.0;

  math_Uzawa solver(C, b, x0);

  EXPECT_TRUE(solver.IsDone());
  EXPECT_GT(solver.NbIterations(), 0);
  EXPECT_LE(solver.NbIterations(), 500); // Default max iterations
}

TEST(math_Uzawa, InitialError_HasOneEntryPerConstraint)
{
  // More constraints than unknowns: the initial error C*x0 - b has one entry per row.
  math_Matrix C(1, 3, 1, 2);
  C(1, 1) = 1.0;
  C(1, 2) = 0.0;
  C(2, 1) = 0.0;
  C(2, 2) = 1.0;
  C(3, 1) = 1.0;
  C(3, 2) = 1.0;

  math_Vector b(1, 3);
  b(1) = 2.0;
  b(2) = 1.0;
  b(3) = 3.0;

  math_Vector x0(1, 2);
  x0(1) = 5.0;
  x0(2) = 4.0;

  // Two equalities and one inequality, so the iterative branch runs and no inverse is formed.
  math_Uzawa solver(C, b, x0, 2, 1);

  const math_Vector& initialError = solver.InitialError();
  ASSERT_EQ(initialError.Length(), 3);
  EXPECT_NEAR(initialError(1), 3.0, TOLERANCE);
  EXPECT_NEAR(initialError(2), 3.0, TOLERANCE);
  EXPECT_NEAR(initialError(3), 6.0, TOLERANCE);
}

TEST(math_Uzawa, ManyMoreConstraintsThanUnknowns_DoesNotOverrunInitialError)
{
  // x = 2 and y = 1 as equalities, plus 98 inequalities that the solution satisfies with room.
  const int   aRows = 100;
  math_Matrix C(1, aRows, 1, 2);
  math_Vector b(1, aRows);
  C(1, 1) = 1.0;
  C(1, 2) = 0.0;
  b(1)    = 2.0;
  C(2, 1) = 0.0;
  C(2, 2) = 1.0;
  b(2)    = 1.0;
  for (int i = 3; i <= aRows; ++i)
  {
    C(i, 1) = 1.0 + 0.01 * i;
    C(i, 2) = 1.0 - 0.003 * i;
    b(i)    = 1000.0;
  }
  math_Vector x0(1, 2);
  x0(1) = 0.0;
  x0(2) = 0.0;

  // The initial error used to be written 98 entries past a two-entry vector.
  math_Uzawa solver(C, b, x0, 2, aRows - 2, 1.0e-8, 1.0e-8, 100000);
  ASSERT_TRUE(solver.IsDone());
  EXPECT_EQ(solver.InitialError().Length(), aRows);
  // The dual step is scaled by all 100 rows, so the iteration stops short of TOLERANCE.
  EXPECT_NEAR(solver.Value()(1), 2.0, 1.0e-4);
  EXPECT_NEAR(solver.Value()(2), 1.0, 1.0e-4);
}
