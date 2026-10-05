// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <ExtremaPC2d_Ellipse.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_TOL = 1.0e-8;

gp_Elips2d makeEllipse(double theMajor = 5.0, double theMinor = 3.0)
{
  return gp_Elips2d(gp_Ax22d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0), true), theMajor, theMinor);
}
} // namespace

TEST(ExtremaPC2d_EllipseTest, MajorAxisOutsideHasTwoExtrema)
{
  ExtremaPC2d_Ellipse        anEvaluator(makeEllipse());
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(10.0, 0.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 2);
  EXPECT_NEAR(aResult[aResult.MinIndex()].Parameter, 0.0, THE_TOL);
  EXPECT_NEAR(aResult.MinSquareDistance(), 25.0, THE_TOL);
  EXPECT_NEAR(aResult.MaxSquareDistance(), 225.0, THE_TOL);
}

TEST(ExtremaPC2d_EllipseTest, CenterHasFourAxisExtrema)
{
  ExtremaPC2d_Ellipse        anEvaluator(makeEllipse());
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 4);
  int aMinima = 0;
  int aMaxima = 0;
  for (size_t anIndex = 0; anIndex < aResult.NbExt(); ++anIndex)
  {
    aMinima += aResult[anIndex].IsMinimum ? 1 : 0;
    aMaxima += aResult[anIndex].IsMaximum ? 1 : 0;
  }
  EXPECT_EQ(aMinima, 2);
  EXPECT_EQ(aMaxima, 2);
  EXPECT_NEAR(aResult.MinSquareDistance(), 9.0, THE_TOL);
  EXPECT_NEAR(aResult.MaxSquareDistance(), 25.0, THE_TOL);
}

TEST(ExtremaPC2d_EllipseTest, SearchModesAtCenterReturnSubsets)
{
  ExtremaPC2d_Ellipse anEvaluator(makeEllipse());
  EXPECT_EQ(anEvaluator.Perform(gp_Pnt2d(), THE_TOL, ExtremaPC2d::SearchMode::Min).NbExt(), 2);
  EXPECT_EQ(anEvaluator.Perform(gp_Pnt2d(), THE_TOL, ExtremaPC2d::SearchMode::Max).NbExt(), 2);
}

TEST(ExtremaPC2d_EllipseTest, PointOnCurveHasZeroDistanceMinimum)
{
  const gp_Elips2d           anEllipse = makeEllipse();
  constexpr double           aU        = M_PI_4;
  ExtremaPC2d_Ellipse        anEvaluator(anEllipse);
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(ElCLib::Value(aU, anEllipse), THE_TOL);
  ASSERT_GE(aResult.NbExt(), 2);
  EXPECT_NEAR(aResult[aResult.MinIndex()].Parameter, aU, THE_TOL);
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
}

TEST(ExtremaPC2d_EllipseTest, IndirectOrientationMapsLocalAxes)
{
  const gp_Ax22d             aPosition(gp_Pnt2d(1.0, 2.0), gp_Dir2d(0.0, 1.0), gp_Dir2d(1.0, 0.0));
  ExtremaPC2d_Ellipse        anEvaluator(gp_Elips2d(aPosition, 5.0, 3.0));
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(11.0, 2.0), THE_TOL);
  EXPECT_NEAR(aResult[aResult.MinIndex()].Parameter, M_PI_2, THE_TOL);
  EXPECT_NEAR(aResult.MinSquareDistance(), 49.0, THE_TOL);
}

TEST(ExtremaPC2d_EllipseTest, CircularEllipseCenterIsInfinite)
{
  ExtremaPC2d_Ellipse        anEvaluator(makeEllipse(5.0, 5.0));
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(), THE_TOL);
  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 25.0, THE_TOL);
}

TEST(ExtremaPC2d_EllipseTest, ShiftedPeriodAndHalfArcEndpoints)
{
  ExtremaPC2d_Ellipse aShifted(makeEllipse(), {7.0, 7.0 + 2.0 * M_PI});
  EXPECT_EQ(aShifted.PerformWithEndpoints(gp_Pnt2d(10.0, 0.0), THE_TOL).NbExt(), 2);

  ExtremaPC2d_Ellipse        aHalf(makeEllipse(), {M_PI_2, 3.0 * M_PI_2});
  const ExtremaPC2d::Result& aHalfResult = aHalf.PerformWithEndpoints(gp_Pnt2d(10.0, 0.0), THE_TOL);
  ASSERT_EQ(aHalfResult.NbExt(), 3);
  EXPECT_NEAR(aHalfResult[aHalfResult.MaxIndex()].Parameter, M_PI, THE_TOL);
}

TEST(ExtremaPC2d_EllipseTest, MultiplePeriodsReturnAllParameterExtrema)
{
  ExtremaPC2d_Ellipse        anEvaluator(makeEllipse(), {0.0, 4.0 * M_PI});
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(12.0, 0.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 4);
  int aFirstPeriod  = 0;
  int aSecondPeriod = 0;
  for (size_t anIndex = 0; anIndex < aResult.NbExt(); ++anIndex)
  {
    if (aResult[anIndex].Parameter < 2.0 * M_PI)
    {
      ++aFirstPeriod;
    }
    else
    {
      ++aSecondPeriod;
    }
  }
  EXPECT_EQ(aFirstPeriod, 2);
  EXPECT_EQ(aSecondPeriod, 2);
}

TEST(ExtremaPC2d_EllipseTest, SingletonInvalidAndAccessors)
{
  ExtremaPC2d_Ellipse aSingleton(makeEllipse(), {0.4, 0.4});
  EXPECT_EQ(aSingleton.Perform(gp_Pnt2d(), THE_TOL).NbExt(), 0);
  EXPECT_EQ(aSingleton.PerformWithEndpoints(gp_Pnt2d(), THE_TOL).NbExt(), 1);
  ExtremaPC2d_Ellipse anInvalid(makeEllipse(), {1.0, 0.0});
  EXPECT_EQ(anInvalid.Perform(gp_Pnt2d(), THE_TOL).Status, ExtremaPC2d::Status::InvalidInput);
  EXPECT_EQ(aSingleton.Value(0.2).Distance(ElCLib::Value(0.2, makeEllipse())), 0.0);
}
