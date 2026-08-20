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

#include <ExtremaPC2d_OffsetCurve.hxx>

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_TOL = 1.0e-8;

occ::handle<Geom2d_OffsetCurve> makeOffsetCircle(double theOffset)
{
  occ::handle<Geom2d_Circle> aCircle =
    new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0), true), 3.0);
  return new Geom2d_OffsetCurve(aCircle, theOffset);
}
} // namespace

TEST(ExtremaPC2d_OffsetCurveTest, OffsetCircleFindsExactMinAndMax)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetCircle(1.0);
  Geom2dAdaptor_Curve             anAdaptor(aCurve);
  ExtremaPC2d_OffsetCurve         anEvaluator(anAdaptor, {0.0, 2.0 * M_PI});
  const ExtremaPC2d::Result&      aResult = anEvaluator.Perform(gp_Pnt2d(8.0, 0.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 2);
  EXPECT_NEAR(aResult.MinSquareDistance(), 16.0, THE_TOL);
  EXPECT_NEAR(aResult.MaxSquareDistance(), 144.0, THE_TOL);
}

TEST(ExtremaPC2d_OffsetCurveTest, CenterHasConstantDistance)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetCircle(1.0);
  Geom2dAdaptor_Curve             anAdaptor(aCurve);
  ExtremaPC2d_OffsetCurve         anEvaluator(anAdaptor, {0.0, 2.0 * M_PI});
  const ExtremaPC2d::Result&      aResult = anEvaluator.Perform(gp_Pnt2d(), THE_TOL);
  EXPECT_TRUE(aResult.IsInfinite());
  EXPECT_NEAR(aResult.InfiniteSquareDistance, 16.0, THE_TOL);
}

TEST(ExtremaPC2d_OffsetCurveTest, NegativeOffsetUsesCorrectSide)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetCircle(-1.0);
  Geom2dAdaptor_Curve             anAdaptor(aCurve);
  ExtremaPC2d_OffsetCurve         anEvaluator(anAdaptor, {0.0, 2.0 * M_PI});
  EXPECT_NEAR(anEvaluator.Perform(gp_Pnt2d(8.0, 0.0), THE_TOL).MinSquareDistance(), 36.0, THE_TOL);
}

TEST(ExtremaPC2d_OffsetCurveTest, OffsetLineProjectionIsExact)
{
  occ::handle<Geom2d_Line>        aLine  = new Geom2d_Line(gp_Pnt2d(), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_OffsetCurve> aCurve = new Geom2d_OffsetCurve(aLine, 2.0);
  Geom2dAdaptor_Curve             anAdaptor(aCurve, 0.0, 4.0);
  ExtremaPC2d_OffsetCurve         anEvaluator(anAdaptor, {0.0, 4.0});
  const gp_Pnt2d                  aQuery  = aCurve->Value(2.0).Translated(gp_Vec2d(0.0, 3.0));
  const ExtremaPC2d::Result&      aResult = anEvaluator.Perform(aQuery, THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].Parameter, 2.0, THE_TOL);
  EXPECT_NEAR(aResult[0].SquareDistance, 9.0, THE_TOL);
}

TEST(ExtremaPC2d_OffsetCurveTest, PartialArcAndSearchModes)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetCircle(1.0);
  Geom2dAdaptor_Curve             anAdaptor(aCurve, 0.0, M_PI_2);
  ExtremaPC2d_OffsetCurve         anEvaluator(anAdaptor, {0.0, M_PI_2});
  const ExtremaPC2d::Result&      aMin =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(8.0, -1.0), THE_TOL, ExtremaPC2d::SearchMode::Min);
  ASSERT_EQ(aMin.NbExt(), 1);
  EXPECT_NEAR(aMin[0].Parameter, 0.0, THE_TOL);
  const ExtremaPC2d::Result& aMax =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(8.0, -1.0), THE_TOL, ExtremaPC2d::SearchMode::Max);
  ASSERT_EQ(aMax.NbExt(), 1);
  EXPECT_NEAR(aMax[0].Parameter, M_PI_2, THE_TOL);
}

TEST(ExtremaPC2d_OffsetCurveTest, InvalidDomainAndTolerance)
{
  occ::handle<Geom2d_OffsetCurve> aCurve = makeOffsetCircle(1.0);
  Geom2dAdaptor_Curve             anAdaptor(aCurve);
  ExtremaPC2d_OffsetCurve         anInvalid(anAdaptor, {1.0, 0.0});
  EXPECT_EQ(anInvalid.Perform(gp_Pnt2d(), THE_TOL).Status, ExtremaPC2d::Status::InvalidInput);
  ExtremaPC2d_OffsetCurve anEvaluator(anAdaptor, {0.0, 2.0 * M_PI});
  EXPECT_EQ(anEvaluator.Perform(gp_Pnt2d(), 0.0).Status, ExtremaPC2d::Status::InvalidInput);
}

TEST(ExtremaPC2d_OffsetCurveTest, EndpointPathRefreshesPartitionAfterMutation)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 10);
  for (size_t anIndex = 0; anIndex < aPoles.Size(); ++anIndex)
  {
    const double aParameter  = static_cast<double>(anIndex) / (aPoles.Size() - 1);
    aPoles.ChangeAt(anIndex) = gp_Pnt2d(aParameter, 0.0);
  }
  occ::handle<Geom2d_BezierCurve> aBasis = new Geom2d_BezierCurve(aPoles);
  occ::handle<Geom2d_OffsetCurve> aCurve = new Geom2d_OffsetCurve(aBasis, 0.1);
  Geom2dAdaptor_Curve             anAdaptor(aCurve);
  ExtremaPC2d_OffsetCurve         aReusedEvaluator(anAdaptor, {0.0, 1.0});

  for (size_t anIndex = 1; anIndex + 1 < aPoles.Size(); ++anIndex)
  {
    const int    aPoleNumber = static_cast<int>(anIndex + 1);
    const double aParameter  = static_cast<double>(anIndex) / (aPoles.Size() - 1);
    aBasis->SetPole(aPoleNumber, gp_Pnt2d(aParameter, aPoleNumber % 2 == 0 ? 2.0 : -2.0));
  }

  const gp_Pnt2d             aQuery(0.5, 0.25);
  const ExtremaPC2d::Result& aReusedResult = aReusedEvaluator.PerformWithEndpoints(aQuery, THE_TOL);
  ExtremaPC2d_OffsetCurve    aFreshEvaluator(anAdaptor, {0.0, 1.0});
  const ExtremaPC2d::Result& aFreshResult = aFreshEvaluator.PerformWithEndpoints(aQuery, THE_TOL);

  ASSERT_EQ(aReusedResult.Status, aFreshResult.Status);
  ASSERT_EQ(aReusedResult.NbExt(), aFreshResult.NbExt());
  for (size_t anIndex = 0; anIndex < aFreshResult.NbExt(); ++anIndex)
  {
    EXPECT_NEAR(aReusedResult[anIndex].Parameter, aFreshResult[anIndex].Parameter, 1.0e-7);
    EXPECT_NEAR(aReusedResult[anIndex].SquareDistance,
                aFreshResult[anIndex].SquareDistance,
                1.0e-7);
  }
}
