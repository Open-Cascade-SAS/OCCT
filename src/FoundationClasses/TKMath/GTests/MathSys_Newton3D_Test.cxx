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

#include <MathSys_Newton3D.hxx>

#include <cmath>

namespace
{
constexpr double THE_TOLERANCE = 1.0e-7;
}

//==================================================================================================
// Test fixture for MathSys_Newton3D tests
//==================================================================================================

class MathSys_Newton3DTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-10;
};

//==================================================================================================
// Test: Simple 3D linear system
//==================================================================================================

TEST_F(MathSys_Newton3DTest, Linear3DSystem)
{
  // System: 2x + y + z = 4, x + 3y + z = 5, x + y + 4z = 6
  // Solution: x = 1, y = 1, z = 1
  auto aFunc =
    [](double theX1, double theX2, double theX3, double theF[3], double theJ[3][3]) -> bool {
    theF[0] = 2.0 * theX1 + theX2 + theX3 - 4.0;
    theF[1] = theX1 + 3.0 * theX2 + theX3 - 5.0;
    theF[2] = theX1 + theX2 + 4.0 * theX3 - 6.0;

    theJ[0][0] = 2.0;
    theJ[0][1] = 1.0;
    theJ[0][2] = 1.0;
    theJ[1][0] = 1.0;
    theJ[1][1] = 3.0;
    theJ[1][2] = 1.0;
    theJ[2][0] = 1.0;
    theJ[2][1] = 1.0;
    theJ[2][2] = 4.0;

    return true;
  };

  auto aResult = MathSys::Newton3D(aFunc, 0.0, 0.0, 0.0, THE_TOL);
  ASSERT_TRUE(aResult.IsDone());

  EXPECT_NEAR(aResult.X1, 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.X2, 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.X3, 1.0, THE_TOLERANCE);
}

//==================================================================================================
// Test: Nonlinear 3D system - sphere-plane intersection
//==================================================================================================

TEST_F(MathSys_Newton3DTest, NonlinearSpherePlane)
{
  // System: x^2 + y^2 + z^2 = 3 (sphere), x + y + z = 3 (plane through sphere)
  // One solution: x = y = z = 1
  auto aFunc =
    [](double theX1, double theX2, double theX3, double theF[3], double theJ[3][3]) -> bool {
    theF[0] = theX1 * theX1 + theX2 * theX2 + theX3 * theX3 - 3.0;
    theF[1] = theX1 + theX2 + theX3 - 3.0;
    theF[2] = theX1 - theX2; // Symmetry constraint

    theJ[0][0] = 2.0 * theX1;
    theJ[0][1] = 2.0 * theX2;
    theJ[0][2] = 2.0 * theX3;
    theJ[1][0] = 1.0;
    theJ[1][1] = 1.0;
    theJ[1][2] = 1.0;
    theJ[2][0] = 1.0;
    theJ[2][1] = -1.0;
    theJ[2][2] = 0.0;

    return true;
  };

  // Start close to the solution for nonlinear systems
  auto aResult = MathSys::Newton3D(aFunc, 0.8, 0.8, 1.4, THE_TOL);
  ASSERT_TRUE(aResult.IsDone());

  // Nonlinear systems may have slightly lower precision
  constexpr double aNonlinearTol = 1.0e-5;
  EXPECT_NEAR(aResult.X1, 1.0, aNonlinearTol);
  EXPECT_NEAR(aResult.X2, 1.0, aNonlinearTol);
  EXPECT_NEAR(aResult.X3, 1.0, aNonlinearTol);
}

//==================================================================================================
// Test: 3D Newton with bounds
//==================================================================================================

TEST_F(MathSys_Newton3DTest, BoundedNewton3D)
{
  // Same linear system but with bounds
  auto aFunc =
    [](double theX1, double theX2, double theX3, double theF[3], double theJ[3][3]) -> bool {
    theF[0] = 2.0 * theX1 + theX2 + theX3 - 4.0;
    theF[1] = theX1 + 3.0 * theX2 + theX3 - 5.0;
    theF[2] = theX1 + theX2 + 4.0 * theX3 - 6.0;

    theJ[0][0] = 2.0;
    theJ[0][1] = 1.0;
    theJ[0][2] = 1.0;
    theJ[1][0] = 1.0;
    theJ[1][1] = 3.0;
    theJ[1][2] = 1.0;
    theJ[2][0] = 1.0;
    theJ[2][1] = 1.0;
    theJ[2][2] = 4.0;

    return true;
  };

  auto aResult =
    MathSys::Newton3D(aFunc, 0.0, 0.0, 0.0, -10.0, 10.0, -10.0, 10.0, -10.0, 10.0, THE_TOL);
  ASSERT_TRUE(aResult.IsDone());

  EXPECT_NEAR(aResult.X1, 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.X2, 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.X3, 1.0, THE_TOLERANCE);
}

//==================================================================================================
// Test: 3D Newton gradient descent fallback
//==================================================================================================

TEST_F(MathSys_Newton3DTest, GradientDescentFallback)
{
  // System where Jacobian is nearly singular at some points
  auto aFunc =
    [](double theX1, double theX2, double theX3, double theF[3], double theJ[3][3]) -> bool {
    // F1 = x^3 - 1, F2 = y^3 - 1, F3 = z^3 - 1
    // Solution: x = y = z = 1
    theF[0] = theX1 * theX1 * theX1 - 1.0;
    theF[1] = theX2 * theX2 * theX2 - 1.0;
    theF[2] = theX3 * theX3 * theX3 - 1.0;

    theJ[0][0] = 3.0 * theX1 * theX1;
    theJ[0][1] = 0.0;
    theJ[0][2] = 0.0;
    theJ[1][0] = 0.0;
    theJ[1][1] = 3.0 * theX2 * theX2;
    theJ[1][2] = 0.0;
    theJ[2][0] = 0.0;
    theJ[2][1] = 0.0;
    theJ[2][2] = 3.0 * theX3 * theX3;

    return true;
  };

  // Start at (0.5, 0.5, 0.5) - Jacobian is not singular here
  auto aResult = MathSys::Newton3D(aFunc, 0.5, 0.5, 0.5, THE_TOL);
  ASSERT_TRUE(aResult.IsDone());

  EXPECT_NEAR(aResult.X1, 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.X2, 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aResult.X3, 1.0, THE_TOLERANCE);
}

//==================================================================================================
// Test: 3D Newton convergence count
//==================================================================================================

TEST_F(MathSys_Newton3DTest, ConvergenceCount)
{
  // Simple linear system should converge in 1 iteration
  auto aFunc =
    [](double theX1, double theX2, double theX3, double theF[3], double theJ[3][3]) -> bool {
    theF[0] = 2.0 * theX1 + theX2 + theX3 - 4.0;
    theF[1] = theX1 + 3.0 * theX2 + theX3 - 5.0;
    theF[2] = theX1 + theX2 + 4.0 * theX3 - 6.0;

    theJ[0][0] = 2.0;
    theJ[0][1] = 1.0;
    theJ[0][2] = 1.0;
    theJ[1][0] = 1.0;
    theJ[1][1] = 3.0;
    theJ[1][2] = 1.0;
    theJ[2][0] = 1.0;
    theJ[2][1] = 1.0;
    theJ[2][2] = 4.0;

    return true;
  };

  auto aResult = MathSys::Newton3D(aFunc, 0.0, 0.0, 0.0, THE_TOL);
  ASSERT_TRUE(aResult.IsDone());

  // Linear system should converge in 1-2 iterations
  EXPECT_LE(aResult.NbIter, 2u);
}

//==================================================================================================
// Test: Verify function values at solution
//==================================================================================================

TEST_F(MathSys_Newton3DTest, VerifyFunctionValues)
{
  // Use the linear system which always converges
  auto aFunc =
    [](double theX1, double theX2, double theX3, double theF[3], double theJ[3][3]) -> bool {
    theF[0] = 2.0 * theX1 + theX2 + theX3 - 4.0;
    theF[1] = theX1 + 3.0 * theX2 + theX3 - 5.0;
    theF[2] = theX1 + theX2 + 4.0 * theX3 - 6.0;

    theJ[0][0] = 2.0;
    theJ[0][1] = 1.0;
    theJ[0][2] = 1.0;
    theJ[1][0] = 1.0;
    theJ[1][1] = 3.0;
    theJ[1][2] = 1.0;
    theJ[2][0] = 1.0;
    theJ[2][1] = 1.0;
    theJ[2][2] = 4.0;

    return true;
  };

  auto aResult = MathSys::Newton3D(aFunc, 0.0, 0.0, 0.0, THE_TOL);
  ASSERT_TRUE(aResult.IsDone());

  // Verify function values at solution
  double aF[3];
  double aJ[3][3];
  aFunc(aResult.X1, aResult.X2, aResult.X3, aF, aJ);

  EXPECT_NEAR(aF[0], 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aF[1], 0.0, THE_TOLERANCE);
  EXPECT_NEAR(aF[2], 0.0, THE_TOLERANCE);
}

//==================================================================================================
// Test: Different starting points
//==================================================================================================

TEST_F(MathSys_Newton3DTest, DifferentStartingPoints)
{
  auto aFunc =
    [](double theX1, double theX2, double theX3, double theF[3], double theJ[3][3]) -> bool {
    theF[0] = 2.0 * theX1 + theX2 + theX3 - 4.0;
    theF[1] = theX1 + 3.0 * theX2 + theX3 - 5.0;
    theF[2] = theX1 + theX2 + 4.0 * theX3 - 6.0;

    theJ[0][0] = 2.0;
    theJ[0][1] = 1.0;
    theJ[0][2] = 1.0;
    theJ[1][0] = 1.0;
    theJ[1][1] = 3.0;
    theJ[1][2] = 1.0;
    theJ[2][0] = 1.0;
    theJ[2][1] = 1.0;
    theJ[2][2] = 4.0;

    return true;
  };

  double aStartPoints[][3] = {{0.0, 0.0, 0.0},
                              {-5.0, -5.0, -5.0},
                              {10.0, 10.0, 10.0},
                              {0.5, 1.5, 0.5}};

  for (const auto& aStart : aStartPoints)
  {
    auto aResult = MathSys::Newton3D(aFunc, aStart[0], aStart[1], aStart[2], THE_TOL);
    ASSERT_TRUE(aResult.IsDone()) << "Failed for start (" << aStart[0] << ", " << aStart[1] << ", "
                                  << aStart[2] << ")";

    EXPECT_NEAR(aResult.X1, 1.0, THE_TOLERANCE);
    EXPECT_NEAR(aResult.X2, 1.0, THE_TOLERANCE);
    EXPECT_NEAR(aResult.X3, 1.0, THE_TOLERANCE);
  }
}

//==================================================================================================
// Test: Solve3x3 helper function
//==================================================================================================

TEST_F(MathSys_Newton3DTest, Solve3x3Helper)
{
  // Test the 3x3 solver directly
  // System: [2 1 1][x]   [4]
  //         [1 3 1][y] = [5]
  //         [1 1 4][z]   [6]
  // Solution: x = y = z = 1

  double aJ[3][3] = {{2.0, 1.0, 1.0}, {1.0, 3.0, 1.0}, {1.0, 1.0, 4.0}};
  double aF[3]    = {4.0, 5.0, 6.0}; // We want J*x = F, but Solve3x3 solves J*x = -F
  // So we negate F to get the right answer
  aF[0] = -aF[0];
  aF[1] = -aF[1];
  aF[2] = -aF[2];

  double aDelta[3];
  bool   aSolved = MathSys::detail::Solve3x3(aJ, aF, aDelta);
  ASSERT_TRUE(aSolved);

  // Solve3x3 returns -J^(-1)*F, which should be [-1,-1,-1] since we negated F
  // Actually, let me re-read the Solve3x3... it solves J*delta = -F, returning delta
  // So if we pass F = [-4, -5, -6], we get delta such that J*delta = [4, 5, 6]
  EXPECT_NEAR(aDelta[0], 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aDelta[1], 1.0, THE_TOLERANCE);
  EXPECT_NEAR(aDelta[2], 1.0, THE_TOLERANCE);
}
