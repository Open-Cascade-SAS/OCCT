// Copyright (c) 2026 OPEN CASCADE SAS
// This file is part of Open CASCADE Technology software library.

#include <ExtremaPC2d_Curve.hxx>

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dEval_SineWaveCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <NCollection_Array1.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_TOL = 1.0e-8;

void compareResults(const ExtremaPC2d::Result& theFirst, const ExtremaPC2d::Result& theSecond)
{
  ASSERT_EQ(theFirst.Status, theSecond.Status);
  ASSERT_EQ(theFirst.NbExt(), theSecond.NbExt());
  for (size_t anIndex = 0; anIndex < theFirst.NbExt(); ++anIndex)
  {
    EXPECT_NEAR(theFirst[anIndex].Parameter, theSecond[anIndex].Parameter, THE_TOL);
    EXPECT_NEAR(theFirst[anIndex].SquareDistance, theSecond[anIndex].SquareDistance, THE_TOL);
    EXPECT_EQ(theFirst[anIndex].IsMinimum, theSecond[anIndex].IsMinimum);
    EXPECT_EQ(theFirst[anIndex].IsMaximum, theSecond[anIndex].IsMaximum);
  }
}

occ::handle<Geom2d_BezierCurve> makeBezier()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(2.0, 3.0);
  aPoles(3) = gp_Pnt2d(4.0, 0.0);
  return new Geom2d_BezierCurve(aPoles);
}

occ::handle<Geom2d_BSplineCurve> makeBSpline()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(1.0, 2.0);
  aPoles(3) = gp_Pnt2d(3.0, -1.0);
  aPoles(4) = gp_Pnt2d(4.0, 0.0);
  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;
  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;
  return new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3);
}
} // namespace

TEST(ExtremaPC2d_CurveTest, NullCurveIsUninitializedAndNotDone)
{
  occ::handle<Geom2d_Curve> aNull;
  ExtremaPC2d_Curve anEvaluator(aNull);
  EXPECT_FALSE(anEvaluator.IsInitialized());
  EXPECT_EQ(anEvaluator.Perform(gp_Pnt2d(), THE_TOL).Status,
            ExtremaPC2d::Status::NotDone);
  EXPECT_EQ(anEvaluator.PerformWithEndpoints(gp_Pnt2d(), THE_TOL).Status,
            ExtremaPC2d::Status::NotDone);
}

TEST(ExtremaPC2d_CurveTest, AnalyticalAdaptorDispatchMatchesSpecialists)
{
  const gp_Pnt2d aQuery(8.0, 3.0);
  occ::handle<Geom2d_Line> aLine =
    new Geom2d_Line(gp_Pnt2d(), gp_Dir2d(1.0, 0.0));
  Geom2dAdaptor_Curve aLineAdaptor(aLine, -10.0, 10.0);
  ExtremaPC2d_Curve aLineAggregate(aLineAdaptor);
  ExtremaPC2d_Line aLineDirect(aLine->Lin2d(), {-10.0, 10.0});
  compareResults(aLineAggregate.Perform(aQuery, THE_TOL), aLineDirect.Perform(aQuery, THE_TOL));

  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(
    gp_Ax22d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0), true),
    4.0);
  ExtremaPC2d_Curve aCircleAggregate(aCircle);
  ExtremaPC2d_Circle aCircleDirect(aCircle->Circ2d());
  compareResults(aCircleAggregate.Perform(aQuery, THE_TOL),
                 aCircleDirect.Perform(aQuery, THE_TOL));

  occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(
    gp_Ax22d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0), true),
    5.0,
    2.0);
  ExtremaPC2d_Curve anEllipseAggregate(anEllipse);
  ExtremaPC2d_Ellipse anEllipseDirect(anEllipse->Elips2d());
  compareResults(anEllipseAggregate.Perform(aQuery, THE_TOL),
                 anEllipseDirect.Perform(aQuery, THE_TOL));

  occ::handle<Geom2d_Hyperbola> aHyperbola = new Geom2d_Hyperbola(
    gp_Ax22d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0), true),
    3.0,
    2.0);
  Geom2dAdaptor_Curve aHyperbolaAdaptor(aHyperbola, -2.0, 2.0);
  ExtremaPC2d_Curve aHyperbolaAggregate(aHyperbolaAdaptor);
  ExtremaPC2d_Hyperbola aHyperbolaDirect(aHyperbola->Hypr2d(), {-2.0, 2.0});
  compareResults(aHyperbolaAggregate.Perform(aQuery, THE_TOL),
                 aHyperbolaDirect.Perform(aQuery, THE_TOL));

  occ::handle<Geom2d_Parabola> aParabola = new Geom2d_Parabola(
    gp_Ax22d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0), true),
    2.0);
  Geom2dAdaptor_Curve aParabolaAdaptor(aParabola, -2.0, 2.0);
  ExtremaPC2d_Curve aParabolaAggregate(aParabolaAdaptor);
  ExtremaPC2d_Parabola aParabolaDirect(aParabola->Parab2d(), {-2.0, 2.0});
  compareResults(aParabolaAggregate.Perform(aQuery, THE_TOL),
                 aParabolaDirect.Perform(aQuery, THE_TOL));
}

TEST(ExtremaPC2d_CurveTest, NumericalHandleDispatchMatchesSpecialists)
{
  const gp_Pnt2d aQuery(2.0, 1.5);
  occ::handle<Geom2d_BezierCurve> aBezier = makeBezier();
  ExtremaPC2d_Curve aBezierAggregate(aBezier);
  ExtremaPC2d_BezierCurve aBezierDirect(aBezier);
  compareResults(aBezierAggregate.Perform(aQuery, THE_TOL),
                 aBezierDirect.Perform(aQuery, THE_TOL));

  occ::handle<Geom2d_BSplineCurve> aBSpline = makeBSpline();
  ExtremaPC2d_Curve aBSplineAggregate(aBSpline);
  ExtremaPC2d_BSplineCurve aBSplineDirect(aBSpline);
  compareResults(aBSplineAggregate.Perform(aQuery, THE_TOL),
                 aBSplineDirect.Perform(aQuery, THE_TOL));
}

TEST(ExtremaPC2d_CurveTest, OffsetDispatchAndSearchModes)
{
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(
    gp_Ax22d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0), true),
    3.0);
  occ::handle<Geom2d_OffsetCurve> anOffset = new Geom2d_OffsetCurve(aCircle, 1.0);
  ExtremaPC2d_Curve anEvaluator(anOffset, 0.0, 2.0 * M_PI);
  const ExtremaPC2d::Result& aMin = anEvaluator.Perform(gp_Pnt2d(8.0, 0.0), THE_TOL,
                                                        ExtremaPC2d::SearchMode::Min);
  ASSERT_EQ(aMin.NbExt(), 1);
  EXPECT_TRUE(aMin[0].IsMinimum);
  const ExtremaPC2d::Result& aMax = anEvaluator.Perform(gp_Pnt2d(8.0, 0.0), THE_TOL,
                                                        ExtremaPC2d::SearchMode::Max);
  ASSERT_EQ(aMax.NbExt(), 1);
  EXPECT_TRUE(aMax[0].IsMaximum);
}

TEST(ExtremaPC2d_CurveTest, GeneralCurveDispatchMatchesSpecialist)
{
  occ::handle<Geom2dEval_SineWaveCurve> aCurve = new Geom2dEval_SineWaveCurve(
    gp_Ax2d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0)),
    1.0,
    2.0);
  Geom2dAdaptor_Curve anAdaptor(aCurve, -2.0, 2.0);
  ExtremaPC2d_Curve anAggregate(anAdaptor);
  ExtremaPC2d_OtherCurve aDirect(anAdaptor, {-2.0, 2.0});
  compareResults(anAggregate.Perform(gp_Pnt2d(0.5, 1.0), THE_TOL),
                 aDirect.Perform(gp_Pnt2d(0.5, 1.0), THE_TOL));
}

TEST(ExtremaPC2d_CurveTest, TrimmedCurveUsesTrimBounds)
{
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(
    gp_Ax22d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0), true),
    3.0);
  occ::handle<Geom2d_TrimmedCurve> aTrimmed = new Geom2d_TrimmedCurve(aCircle, 0.2, 1.0);
  ExtremaPC2d_Curve anEvaluator(aTrimmed);
  const ExtremaPC2d::Result& aResult =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(6.0, 0.0), THE_TOL);
  ASSERT_TRUE(aResult.IsDone());
  for (size_t anIndex = 0; anIndex < aResult.NbExt(); ++anIndex)
  {
    EXPECT_GE(aResult[anIndex].Parameter, 0.2 - THE_TOL);
    EXPECT_LE(aResult[anIndex].Parameter, 1.0 + THE_TOL);
  }
}

TEST(ExtremaPC2d_CurveTest, ExplicitBoundsIntersectTrimmedBounds)
{
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(
    gp_Ax22d(gp_Pnt2d(), gp_Dir2d(1.0, 0.0), true),
    3.0);
  occ::handle<Geom2d_TrimmedCurve> aTrimmed = new Geom2d_TrimmedCurve(aCircle, 0.2, 1.0);
  ExtremaPC2d_Curve anEvaluator(aTrimmed, 0.5, 2.0);
  const ExtremaPC2d::Result& aResult =
    anEvaluator.PerformWithEndpoints(gp_Pnt2d(6.0, 0.0), THE_TOL);
  for (size_t anIndex = 0; anIndex < aResult.NbExt(); ++anIndex)
  {
    EXPECT_GE(aResult[anIndex].Parameter, 0.5 - THE_TOL);
    EXPECT_LE(aResult[anIndex].Parameter, 1.0 + THE_TOL);
  }
}

TEST(ExtremaPC2d_CurveTest, BezierMutationFlowsThroughAggregator)
{
  occ::handle<Geom2d_BezierCurve> aCurve = makeBezier();
  ExtremaPC2d_Curve anEvaluator(aCurve);
  const double aBefore = anEvaluator.Perform(gp_Pnt2d(2.0, 2.0), THE_TOL).MinSquareDistance();
  aCurve->SetPole(2, gp_Pnt2d(3.5, -4.0));
  const ExtremaPC2d::Result& anAfter = anEvaluator.Perform(gp_Pnt2d(2.0, 2.0), THE_TOL);
  EXPECT_GT(std::abs(anAfter.MinSquareDistance() - aBefore), 1.0e-4);
  EXPECT_EQ(anAfter[anAfter.MinIndex()].Point.Distance(
              aCurve->Value(anAfter[anAfter.MinIndex()].Parameter)), 0.0);
}

TEST(ExtremaPC2d_CurveTest, AnalyticalDispatchCopiesPrimitiveGeometry)
{
  occ::handle<Geom2d_Line> aLine =
    new Geom2d_Line(gp_Pnt2d(), gp_Dir2d(1.0, 0.0));
  Geom2dAdaptor_Curve anAdaptor(aLine, -10.0, 10.0);
  ExtremaPC2d_Curve anEvaluator(anAdaptor);
  anAdaptor.Load(new Geom2d_Line(gp_Pnt2d(), gp_Dir2d(0.0, 1.0)), -10.0, 10.0);
  const ExtremaPC2d::Result& aResult = anEvaluator.Perform(gp_Pnt2d(4.0, 3.0), THE_TOL);
  ASSERT_EQ(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult[0].Parameter, 4.0, THE_TOL);
}

TEST(ExtremaPC2d_CurveTest, ResultStorageIsEvaluatorLocal)
{
  ExtremaPC2d_Curve aFirst(makeBezier());
  ExtremaPC2d_Curve aSecond(makeBSpline());
  const ExtremaPC2d::Result* aFirstResult = &aFirst.Perform(gp_Pnt2d(1.0, 2.0), THE_TOL);
  const ExtremaPC2d::Result* aSecondResult = &aSecond.Perform(gp_Pnt2d(1.0, 2.0), THE_TOL);
  EXPECT_NE(aFirstResult, aSecondResult);
  EXPECT_EQ(aFirstResult, &aFirst.Perform(gp_Pnt2d(3.0, 2.0), THE_TOL));
}
