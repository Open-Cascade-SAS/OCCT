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

#include <array>

class MathSys_Newton3DTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-10;
};

TEST_F(MathSys_Newton3DTest, Solve3D_LinearSystem)
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

  MathSys::NewtonBoundsN<3> aBounds;
  aBounds.HasBounds = false;

  MathSys::NewtonOptions aOptions;
  aOptions.FTolerance = THE_TOL;

  const MathSys::NewtonResultN<3> aResult =
    MathSys::Solve3D(aFunc, {0.0, 0.0, 0.0}, aBounds, aOptions);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.X[0], 1.0, 1.0e-8);
  EXPECT_NEAR(aResult.X[1], 1.0, 1.0e-8);
  EXPECT_NEAR(aResult.X[2], 1.0, 1.0e-8);
}

TEST_F(MathSys_Newton3DTest, Solve3D_NonlinearSystem)
{
  auto aFunc =
    [](double theX1, double theX2, double theX3, double theF[3], double theJ[3][3]) -> bool {
    theF[0] = theX1 * theX1 + theX2 * theX2 + theX3 * theX3 - 3.0;
    theF[1] = theX1 + theX2 + theX3 - 3.0;
    theF[2] = theX1 - theX2;

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

  MathSys::NewtonBoundsN<3> aBounds;
  aBounds.HasBounds = false;

  MathSys::NewtonOptions aOptions;
  aOptions.FTolerance   = 1.0e-10;
  aOptions.MaxIterations = 50;

  const MathSys::NewtonResultN<3> aResult =
    MathSys::Solve3D(aFunc, {0.8, 0.8, 1.4}, aBounds, aOptions);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.X[0], 1.0, 1.0e-5);
  EXPECT_NEAR(aResult.X[1], 1.0, 1.0e-5);
  EXPECT_NEAR(aResult.X[2], 1.0, 1.0e-5);
}

TEST_F(MathSys_Newton3DTest, Solve3D_Bounded)
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

  MathSys::NewtonBoundsN<3> aBounds;
  aBounds.Min = {-10.0, -10.0, -10.0};
  aBounds.Max = {10.0, 10.0, 10.0};

  MathSys::NewtonOptions aOptions;
  aOptions.FTolerance = THE_TOL;

  const MathSys::NewtonResultN<3> aResult =
    MathSys::Solve3D(aFunc, {0.0, 0.0, 0.0}, aBounds, aOptions);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.X[0], 1.0, 1.0e-8);
  EXPECT_NEAR(aResult.X[1], 1.0, 1.0e-8);
  EXPECT_NEAR(aResult.X[2], 1.0, 1.0e-8);
}

TEST_F(MathSys_Newton3DTest, Solve3D_InvalidInput)
{
  auto aFunc = [](double /*theX1*/,
                  double /*theX2*/,
                  double /*theX3*/,
                  double theF[3],
                  double theJ[3][3]) -> bool {
    theF[0]    = 0.0;
    theF[1]    = 0.0;
    theF[2]    = 0.0;
    theJ[0][0] = 1.0;
    theJ[0][1] = 0.0;
    theJ[0][2] = 0.0;
    theJ[1][0] = 0.0;
    theJ[1][1] = 1.0;
    theJ[1][2] = 0.0;
    theJ[2][0] = 0.0;
    theJ[2][1] = 0.0;
    theJ[2][2] = 1.0;
    return true;
  };

  MathSys::NewtonBoundsN<3> aBounds;
  aBounds.Min = {1.0, 0.0, 0.0};
  aBounds.Max = {0.0, 1.0, 1.0};

  MathSys::NewtonOptions aOptions;
  aOptions.FTolerance = THE_TOL;

  const MathSys::NewtonResultN<3> aResult =
    MathSys::Solve3D(aFunc, {0.0, 0.0, 0.0}, aBounds, aOptions);

  EXPECT_EQ(aResult.Status, MathUtils::Status::InvalidInput);
  EXPECT_FALSE(aResult.IsDone());
}

namespace
{
class LineCurveEval
{
public:
  void D2(double theT, gp_Pnt& theP, gp_Vec& theD1, gp_Vec& theD2) const
  {
    theP.SetCoord(theT, 0.0, 0.0);
    theD1.SetCoord(1.0, 0.0, 0.0);
    theD2.SetCoord(0.0, 0.0, 0.0);
  }
};

class PlaneSurfaceEval
{
public:
  void D2(double  theU,
          double  theV,
          gp_Pnt& theP,
          gp_Vec& theD1U,
          gp_Vec& theD1V,
          gp_Vec& theD2UU,
          gp_Vec& theD2VV,
          gp_Vec& theD2UV) const
  {
    theP.SetCoord(theU, theV, 0.0);
    theD1U.SetCoord(1.0, 0.0, 0.0);
    theD1V.SetCoord(0.0, 1.0, 0.0);
    theD2UU.SetCoord(0.0, 0.0, 0.0);
    theD2VV.SetCoord(0.0, 0.0, 0.0);
    theD2UV.SetCoord(0.0, 0.0, 0.0);
  }
};
} // namespace

TEST_F(MathSys_Newton3DTest, SolveCurveSurfaceExtrema3D_Smoke)
{
  LineCurveEval    aCurve;
  PlaneSurfaceEval aSurface;

  MathSys::NewtonBoundsN<3> aBounds;
  aBounds.Min = {-10.0, -10.0, -10.0};
  aBounds.Max = {10.0, 10.0, 10.0};

  MathSys::NewtonOptions aOptions;
  aOptions.FTolerance   = THE_TOL;
  aOptions.MaxIterations = 20;

  const MathSys::NewtonResultN<3> aResult =
    MathSys::SolveCurveSurfaceExtrema3D(aCurve,
                                        aSurface,
                                        std::array<double, 3>{1.0, 1.0, 0.0},
                                        aBounds,
                                        aOptions);

  EXPECT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.ResidualNorm, 0.0, 1.0e-12);
}
