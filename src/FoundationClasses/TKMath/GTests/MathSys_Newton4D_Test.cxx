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

#include <MathSys_Newton4D.hxx>

#include <array>

class MathSys_Newton4DTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1.0e-10;
};

TEST_F(MathSys_Newton4DTest, Solve4D_LinearSystem)
{
  auto aFunc =
    [](double theX1, double theX2, double theX3, double theX4, double theF[4], double theJ[4][4])
    -> bool {
    theF[0] = theX1 - 1.0;
    theF[1] = theX2 - 2.0;
    theF[2] = theX3 - 3.0;
    theF[3] = theX4 - 4.0;

    theJ[0][0] = 1.0;
    theJ[0][1] = 0.0;
    theJ[0][2] = 0.0;
    theJ[0][3] = 0.0;

    theJ[1][0] = 0.0;
    theJ[1][1] = 1.0;
    theJ[1][2] = 0.0;
    theJ[1][3] = 0.0;

    theJ[2][0] = 0.0;
    theJ[2][1] = 0.0;
    theJ[2][2] = 1.0;
    theJ[2][3] = 0.0;

    theJ[3][0] = 0.0;
    theJ[3][1] = 0.0;
    theJ[3][2] = 0.0;
    theJ[3][3] = 1.0;
    return true;
  };

  MathSys::NewtonBoundsN<4> aBounds;
  aBounds.HasBounds = false;

  MathSys::NewtonOptions aOptions;
  aOptions.ResidualTol = THE_TOL;

  const MathSys::NewtonResultN<4> aResult =
    MathSys::Solve4D(aFunc, {0.0, 0.0, 0.0, 0.0}, aBounds, aOptions);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.X[0], 1.0, 1.0e-12);
  EXPECT_NEAR(aResult.X[1], 2.0, 1.0e-12);
  EXPECT_NEAR(aResult.X[2], 3.0, 1.0e-12);
  EXPECT_NEAR(aResult.X[3], 4.0, 1.0e-12);
}

TEST_F(MathSys_Newton4DTest, Solve4D_Bounded)
{
  auto aFunc =
    [](double theX1, double theX2, double theX3, double theX4, double theF[4], double theJ[4][4])
    -> bool {
    theF[0] = theX1 - 1.0;
    theF[1] = theX2 - 2.0;
    theF[2] = theX3 - 3.0;
    theF[3] = theX4 - 4.0;

    for (int r = 0; r < 4; ++r)
    {
      for (int c = 0; c < 4; ++c)
      {
        theJ[r][c] = (r == c) ? 1.0 : 0.0;
      }
    }
    return true;
  };

  MathSys::NewtonBoundsN<4> aBounds;
  aBounds.Min = {-10.0, -10.0, -10.0, -10.0};
  aBounds.Max = {10.0, 10.0, 10.0, 10.0};

  MathSys::NewtonOptions aOptions;
  aOptions.ResidualTol = THE_TOL;

  const MathSys::NewtonResultN<4> aResult =
    MathSys::Solve4D(aFunc, {9.0, 8.0, 7.0, 6.0}, aBounds, aOptions);

  ASSERT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.X[0], 1.0, 1.0e-12);
  EXPECT_NEAR(aResult.X[1], 2.0, 1.0e-12);
  EXPECT_NEAR(aResult.X[2], 3.0, 1.0e-12);
  EXPECT_NEAR(aResult.X[3], 4.0, 1.0e-12);
}

TEST_F(MathSys_Newton4DTest, Solve4D_InvalidInput)
{
  auto aFunc = [](double /*theX1*/,
                  double /*theX2*/,
                  double /*theX3*/,
                  double /*theX4*/,
                  double theF[4],
                  double theJ[4][4]) -> bool {
    for (int i = 0; i < 4; ++i)
    {
      theF[i] = 0.0;
      for (int j = 0; j < 4; ++j)
      {
        theJ[i][j] = (i == j) ? 1.0 : 0.0;
      }
    }
    return true;
  };

  MathSys::NewtonBoundsN<4> aBounds;
  aBounds.Min = {1.0, 0.0, 0.0, 0.0};
  aBounds.Max = {0.0, 1.0, 1.0, 1.0};

  MathSys::NewtonOptions aOptions;
  aOptions.ResidualTol = THE_TOL;

  const MathSys::NewtonResultN<4> aResult =
    MathSys::Solve4D(aFunc, {0.0, 0.0, 0.0, 0.0}, aBounds, aOptions);

  EXPECT_EQ(aResult.Status, MathSys::NewtonStatus::InvalidInput);
  EXPECT_FALSE(aResult.IsDone());
}

namespace
{
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

TEST_F(MathSys_Newton4DTest, SolveSurfaceSurfaceExtrema4D_Smoke)
{
  PlaneSurfaceEval aSurf1;
  PlaneSurfaceEval aSurf2;

  MathSys::NewtonBoundsN<4> aBounds;
  aBounds.Min = {-10.0, -10.0, -10.0, -10.0};
  aBounds.Max = {10.0, 10.0, 10.0, 10.0};

  MathSys::NewtonOptions aOptions;
  aOptions.ResidualTol = THE_TOL;

  const MathSys::NewtonResultN<4> aResult =
    MathSys::SolveSurfaceSurfaceExtrema4D(aSurf1,
                                          aSurf2,
                                          std::array<double, 4>{1.0, 2.0, 1.0, 2.0},
                                          aBounds,
                                          aOptions);

  EXPECT_TRUE(aResult.IsDone());
  EXPECT_NEAR(aResult.ResidualNorm, 0.0, 1.0e-12);
}
