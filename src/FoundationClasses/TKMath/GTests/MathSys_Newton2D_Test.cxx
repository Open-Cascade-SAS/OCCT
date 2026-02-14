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

#include <MathSys_Newton2D.hxx>

#include <array>
#include <cmath>

namespace
{

class QuadraticFunc
{
public:
  bool ValueAndJacobian(double  theU,
                        double  theV,
                        double& theF1,
                        double& theF2,
                        double& theJ11,
                        double& theJ12,
                        double& theJ21,
                        double& theJ22) const
  {
    theF1  = 2.0 * theU;
    theF2  = 2.0 * theV;
    theJ11 = 2.0;
    theJ12 = 0.0;
    theJ21 = 0.0;
    theJ22 = 2.0;
    return true;
  }
};

class SymmetricDistFunc
{
public:
  SymmetricDistFunc(double theTargetU, double theTargetV)
      : myTargetU(theTargetU),
        myTargetV(theTargetV)
  {
  }

  bool Value(double theU, double theV, double& theFu, double& theFv) const
  {
    theFu = 2.0 * (theU - myTargetU);
    theFv = 2.0 * (theV - myTargetV);
    return true;
  }

  bool ValueAndJacobian(double  theU,
                        double  theV,
                        double& theFu,
                        double& theFv,
                        double& theDFuu,
                        double& theDFuv,
                        double& theDFvv) const
  {
    theFu   = 2.0 * (theU - myTargetU);
    theFv   = 2.0 * (theV - myTargetV);
    theDFuu = 2.0;
    theDFuv = 0.0;
    theDFvv = 2.0;
    return true;
  }

private:
  double myTargetU;
  double myTargetV;
};

//! Constant residual with huge Jacobian creates tiny Newton step.
class HugeJacobianConstantResidual
{
public:
  bool Value(double /*theU*/, double /*theV*/, double& theF1, double& theF2) const
  {
    theF1 = 1.0;
    theF2 = 1.0;
    return true;
  }

  bool ValueAndJacobian(double /*theU*/,
                        double /*theV*/,
                        double& theF1,
                        double& theF2,
                        double& theJ11,
                        double& theJ12,
                        double& theJ22) const
  {
    theF1  = 1.0;
    theF2  = 1.0;
    theJ11 = 1.0e20;
    theJ12 = 0.0;
    theJ22 = 1.0e20;
    return true;
  }
};

//! Synthetic system for Armijo directional derivative regression.
//! Residual is constant and cannot decrease. For this system:
//! - F . d is positive (old condition could accept)
//! - (J^T F) . d is negative (correct Armijo rejects all trial steps)
class ArmijoRegressionFunc
{
public:
  bool Value(double /*theU*/, double /*theV*/, double& theF1, double& theF2) const
  {
    theF1 = 1.0;
    theF2 = 2.0;
    return true;
  }

  bool ValueAndJacobian(double /*theU*/,
                        double /*theV*/,
                        double& theF1,
                        double& theF2,
                        double& theJ11,
                        double& theJ12,
                        double& theJ22) const
  {
    theF1  = 1.0;
    theF2  = 2.0;
    theJ11 = 1.0;
    theJ12 = 0.0;
    theJ22 = -1.0;
    return true;
  }
};

} // namespace

TEST(MathSys_Newton2DTest, Solve2D_Quadratic_Converges)
{
  QuadraticFunc aFunc;

  MathSys::NewtonBoundsN<2> aBounds;
  aBounds.Min = {-10.0, -10.0};
  aBounds.Max = {10.0, 10.0};

  MathSys::NewtonOptions aOptions;
  aOptions.FTolerance    = 1.0e-10;
  aOptions.MaxIterations = 50;

  const MathSys::NewtonResultN<2> aResult = MathSys::Solve2D(aFunc, {5.0, 3.0}, aBounds, aOptions);

  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.Status, MathUtils::Status::OK);
  EXPECT_NEAR(aResult.X[0], 0.0, 1.0e-12);
  EXPECT_NEAR(aResult.X[1], 0.0, 1.0e-12);
  EXPECT_LT(aResult.ResidualNorm, 1.0e-10);
}

TEST(MathSys_Newton2DTest, Solve2DSymmetric_Target_Converges)
{
  SymmetricDistFunc aFunc(3.5, 7.2);

  MathSys::NewtonBoundsN<2> aBounds;
  aBounds.Min = {-10.0, -10.0};
  aBounds.Max = {10.0, 10.0};

  MathSys::NewtonOptions aOptions;
  aOptions.FTolerance    = 1.0e-10;
  aOptions.MaxIterations = 50;

  const MathSys::NewtonResultN<2> aResult =
    MathSys::Solve2DSymmetric(aFunc, {0.0, 0.0}, aBounds, aOptions);

  EXPECT_TRUE(aResult.IsDone());
  EXPECT_EQ(aResult.Status, MathUtils::Status::OK);
  EXPECT_NEAR(aResult.X[0], 3.5, 1.0e-9);
  EXPECT_NEAR(aResult.X[1], 7.2, 1.0e-9);
  EXPECT_LT(aResult.ResidualNorm, 1.0e-10);
}

TEST(MathSys_Newton2DTest, Regression_TinyStepDoesNotConvergeWithLargeResidual)
{
  HugeJacobianConstantResidual aFunc;

  MathSys::NewtonBoundsN<2> aBounds;
  aBounds.Min = {-1.0, -1.0};
  aBounds.Max = {1.0, 1.0};

  MathSys::NewtonOptions aOptions;
  aOptions.FTolerance    = 1.0e-8;
  aOptions.XTolerance    = 1.0e-16;
  aOptions.MaxIterations = 10;

  const MathSys::NewtonResultN<2> aResult =
    MathSys::Solve2DSymmetric(aFunc, {0.0, 0.0}, aBounds, aOptions);

  EXPECT_FALSE(aResult.IsDone());
  EXPECT_NE(aResult.Status, MathUtils::Status::OK);
  EXPECT_GT(aResult.ResidualNorm, 1.0e-2);
}

TEST(MathSys_Newton2DTest, Regression_ArmijoUsesJtFDirectionalDerivative)
{
  ArmijoRegressionFunc aFunc;

  MathSys::NewtonBoundsN<2> aBounds;
  aBounds.Min = {-10.0, -10.0};
  aBounds.Max = {10.0, 10.0};

  MathSys::NewtonOptions aOptions;
  aOptions.EnableLineSearch = true;
  aOptions.MaxIterations    = 10;

  const MathSys::NewtonResultN<2> aResult =
    MathSys::Solve2DSymmetric(aFunc, {0.0, 0.0}, aBounds, aOptions);

  EXPECT_FALSE(aResult.IsDone());
  EXPECT_EQ(aResult.Status, MathUtils::Status::NonDescentDirection);
}

TEST(MathSys_Newton2DTest, Solve2DSymmetric_HighPrecision)
{
  SymmetricDistFunc aFunc(5.123456789, 2.987654321);

  MathSys::NewtonBoundsN<2> aBounds;
  aBounds.Min = {-10.0, -10.0};
  aBounds.Max = {10.0, 10.0};

  MathSys::NewtonOptions aOptions;
  aOptions.FTolerance    = 5.0e-8;
  aOptions.MaxIterations = 100;

  const MathSys::NewtonResultN<2> aResult =
    MathSys::Solve2DSymmetric(aFunc, {0.0, 0.0}, aBounds, aOptions);

  EXPECT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.X[0], 5.123456789, 1.0e-7);
  EXPECT_NEAR(aResult.X[1], 2.987654321, 1.0e-7);
  EXPECT_LT(aResult.ResidualNorm, 5.0e-8);
}

TEST(MathSys_Newton2DTest, Solve2DSymmetric_InvalidInput)
{
  SymmetricDistFunc aFunc(0.0, 0.0);

  MathSys::NewtonBoundsN<2> aBounds;
  aBounds.Min = {10.0, 0.0};
  aBounds.Max = {0.0, 10.0};

  MathSys::NewtonOptions aOptions;
  aOptions.FTolerance = 1.0e-10;

  const MathSys::NewtonResultN<2> aResult =
    MathSys::Solve2DSymmetric(aFunc, {0.0, 0.0}, aBounds, aOptions);

  EXPECT_EQ(aResult.Status, MathUtils::Status::InvalidInput);
  EXPECT_FALSE(aResult.IsDone());
}

TEST(MathSys_Newton2DTest, Solve2DSymmetric_ConsistentFromDifferentStarts)
{
  SymmetricDistFunc aFunc(5.0, 5.0);

  MathSys::NewtonBoundsN<2> aBounds;
  aBounds.Min = {0.0, 0.0};
  aBounds.Max = {10.0, 10.0};

  MathSys::NewtonOptions aOptions;
  aOptions.FTolerance    = 1.0e-10;
  aOptions.MaxIterations = 50;

  const std::array<std::array<double, 2>, 4> aStarts = {std::array<double, 2>{0.0, 0.0},
                                                        std::array<double, 2>{10.0, 10.0},
                                                        std::array<double, 2>{2.0, 8.0},
                                                        std::array<double, 2>{8.0, 2.0}};

  for (const std::array<double, 2>& aStart : aStarts)
  {
    const MathSys::NewtonResultN<2> aResult =
      MathSys::Solve2DSymmetric(aFunc, aStart, aBounds, aOptions);
    ASSERT_TRUE(aResult.IsDone());
    EXPECT_NEAR(aResult.X[0], 5.0, 1.0e-8);
    EXPECT_NEAR(aResult.X[1], 5.0, 1.0e-8);
  }
}
