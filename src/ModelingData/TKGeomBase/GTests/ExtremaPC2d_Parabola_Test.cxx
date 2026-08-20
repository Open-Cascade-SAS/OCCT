// Copyright (c) 2026 OPEN CASCADE SAS
// This file is part of Open CASCADE Technology software library.

#include <ExtremaPC2d_Parabola.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_TOL = 1.0e-8;

gp_Parab2d makeParabola(double theFocal = 1.0)
{
  return gp_Parab2d(gp_Ax22d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0), true), theFocal);
}
} // namespace

TEST(ExtremaPC2d_ParabolaTest, VertexAndBehindVertexHaveUniqueMinimum)
{
  ExtremaPC2d_Parabola       anEvaluator(makeParabola());
  const ExtremaPC2d::Result& aVertex = anEvaluator.Perform(gp_Pnt2d(), THE_TOL);
  ASSERT_EQ(aVertex.NbExt(), 1);
  EXPECT_NEAR(aVertex[0].Parameter, 0.0, THE_TOL);
  EXPECT_NEAR(aVertex[0].SquareDistance, 0.0, THE_TOL);
  const ExtremaPC2d::Result& aBehind = anEvaluator.Perform(gp_Pnt2d(-2.0, 0.0), THE_TOL);
  ASSERT_EQ(aBehind.NbExt(), 1);
  EXPECT_NEAR(aBehind[0].SquareDistance, 4.0, THE_TOL);
}

TEST(ExtremaPC2d_ParabolaTest, AxisPointAheadHasThreeExtrema)
{
  ExtremaPC2d_Parabola       anEvaluator(makeParabola());
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(3.0, 0.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 3);
  EXPECT_NEAR(aResult.MinSquareDistance(), 8.0, THE_TOL);
  EXPECT_NEAR(aResult.MaxSquareDistance(), 9.0, THE_TOL);
  int aMinima = 0;
  for (size_t anIndex = 0; anIndex < aResult.NbExt(); ++anIndex)
  {
    aMinima += aResult[anIndex].IsMinimum ? 1 : 0;
  }
  EXPECT_EQ(aMinima, 2);
}

TEST(ExtremaPC2d_ParabolaTest, SearchModesReturnExactClasses)
{
  ExtremaPC2d_Parabola anEvaluator(makeParabola());
  EXPECT_EQ(anEvaluator.Perform(gp_Pnt2d(3.0, 0.0), THE_TOL, ExtremaPC2d::SearchMode::Min).NbExt(),
            2);
  const ExtremaPC2d::Result& aMax =
    anEvaluator.Perform(gp_Pnt2d(3.0, 0.0), THE_TOL, ExtremaPC2d::SearchMode::Max);
  ASSERT_EQ(aMax.NbExt(), 1);
  EXPECT_NEAR(aMax[0].Parameter, 0.0, THE_TOL);
}

TEST(ExtremaPC2d_ParabolaTest, KnownCubicHasThreeRoots)
{
  ExtremaPC2d_Parabola       anEvaluator(makeParabola());
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(3.75, -0.75), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 3);
  bool hasMinus3 = false;
  bool has1      = false;
  bool has2      = false;
  for (size_t anIndex = 0; anIndex < aResult.NbExt(); ++anIndex)
  {
    hasMinus3 = hasMinus3 || std::abs(aResult[anIndex].Parameter + 3.0) <= THE_TOL;
    has1      = has1 || std::abs(aResult[anIndex].Parameter - 1.0) <= THE_TOL;
    has2      = has2 || std::abs(aResult[anIndex].Parameter - 2.0) <= THE_TOL;
  }
  EXPECT_TRUE(hasMinus3);
  EXPECT_TRUE(has1);
  EXPECT_TRUE(has2);
}

TEST(ExtremaPC2d_ParabolaTest, RootWithinParameterToleranceIsClampedToBoundary)
{
  const double               anUpper = 1.0 - 0.5 * ExtremaPC2d::THE_PARAM_TOLERANCE;
  ExtremaPC2d_Parabola       anEvaluator(makeParabola(), {0.5, anUpper});
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(3.75, -0.75), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_EQ(aResult[0].Parameter, anUpper);
}

TEST(ExtremaPC2d_ParabolaTest, PointOnCurveHasZeroDistanceMinimum)
{
  const gp_Parab2d           aCurve = makeParabola();
  ExtremaPC2d_Parabola       anEvaluator(aCurve);
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(ElCLib::Value(2.0, aCurve), THE_TOL);
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOL);
  EXPECT_NEAR(aResult[aResult.MinIndex()].Parameter, 2.0, THE_TOL);
}

TEST(ExtremaPC2d_ParabolaTest, IndirectOrientationPreservesParameterSense)
{
  const gp_Ax22d             aPosition(gp_Pnt2d(1.0, 2.0), gp_Dir2d(0.0, 1.0), gp_Dir2d(1.0, 0.0));
  ExtremaPC2d_Parabola       anEvaluator(gp_Parab2d(aPosition, 1.0));
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(1.0, 5.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 3);
  EXPECT_NEAR(aResult.MinSquareDistance(), 8.0, THE_TOL);
}

TEST(ExtremaPC2d_ParabolaTest, BoundedArcAddsEndpointMinima)
{
  ExtremaPC2d_Parabola       anEvaluator(makeParabola(), {-1.0, 1.0});
  const ExtremaPC2d::Result& aResult =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(3.0, 0.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 3);
  EXPECT_NEAR(aResult[aResult.MaxIndex()].Parameter, 0.0, THE_TOL);
}

TEST(ExtremaPC2d_ParabolaTest, ZeroFocalMatchesMirrorAxisLine)
{
  const gp_Ax22d             aPosition(gp_Pnt2d(1.0, 2.0), gp_Dir2d(0.0, 1.0), true);
  ExtremaPC2d_Parabola       anEvaluator(gp_Parab2d(aPosition, 0.0));
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(4.0, 7.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].Parameter, 5.0, THE_TOL);
  EXPECT_NEAR(aResult[0].SquareDistance, 9.0, THE_TOL);
}

TEST(ExtremaPC2d_ParabolaTest, SingletonInvalidAndValueAccessor)
{
  ExtremaPC2d_Parabola aSingleton(makeParabola(), {0.4, 0.4});
  EXPECT_EQ(aSingleton.Perform(gp_Pnt2d(), THE_TOL).NbExt(), 0);
  EXPECT_EQ(aSingleton.PerformWithEndpoints(gp_Pnt2d(), THE_TOL).NbExt(), 1);
  ExtremaPC2d_Parabola anInvalid(makeParabola(), {1.0, 0.0});
  EXPECT_EQ(anInvalid.Perform(gp_Pnt2d(), THE_TOL).Status, ExtremaPC2d::Status::InvalidInput);
  EXPECT_EQ(aSingleton.Value(-0.7).Distance(ElCLib::Value(-0.7, makeParabola())), 0.0);
}
