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

#include <gtest/gtest.h>

#include <Bisector_BisecCC.hxx>
#include <Bisector_FunctionH.hxx>
#include <BSplCLib.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2dConvert.hxx>
#include <Bisector_Bisec.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_Real.hxx>
#include <MathRoot_Multiple.hxx>

#include <algorithm>
#include <cmath>

TEST(Bisector_FunctionHTest, ParallelTangentsDoNotAddCircleRoot)
{
  const occ::handle<Geom2d_Curve> aCurve =
    new Geom2d_TrimmedCurve(new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 1.0),
                            M_PI,
                            2.0 * M_PI);
  Bisector_FunctionH       aFunction(aCurve, gp_Pnt2d(5.0, 0.0), gp_Vec2d(1.0, 0.0));
  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples  = 20;
  aConfig.XTolerance = Precision::PConfusion();
  aConfig.FTolerance = Precision::Confusion();
  const MathRoot::MultipleResult aRoots =
    MathRoot::FindAllRootsWithDerivative(aFunction, M_PI, 2.0 * M_PI, aConfig);
  ASSERT_TRUE(aRoots.IsDone());
  ASSERT_EQ(aRoots.NbRoots(), 1);
  const double aParameter = aRoots.Roots.Value(0);
  EXPECT_NEAR(aParameter, 2.0 * M_PI + std::atan2(-12.0, 5.0), Precision::PConfusion());
  const auto [aPoint, aTangent] = aCurve->EvalD1(aParameter);
  EXPECT_NEAR(gp_Vec2d(aPoint, gp_Pnt2d(5.0, -12.0)).Dot(aTangent), 0.0, Precision::Confusion());
}

TEST(Bisector_FunctionHTest, AntiparallelTangentsDoNotAddCircleRoot)
{
  const occ::handle<Geom2d_Curve> aCurve =
    new Geom2d_TrimmedCurve(new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 1.0),
                            0.0,
                            M_PI);
  Bisector_FunctionH       aFunction(aCurve, gp_Pnt2d(5.0, 0.0), gp_Vec2d(1.0, 0.0));
  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples = 20;
  const MathRoot::MultipleResult aRoots =
    MathRoot::FindAllRootsWithDerivative(aFunction, 0.0, M_PI, aConfig);
  ASSERT_TRUE(aRoots.IsDone());
  EXPECT_EQ(aRoots.NbRoots(), 0);

  const double aParameter = 0.5 * M_PI;
  const double aStep      = std::cbrt(Epsilon(1.0));
  double       aValue, aDerivative, aBefore, anAfter;
  ASSERT_TRUE(aFunction.Values(aParameter, aValue, aDerivative));
  ASSERT_TRUE(aFunction.Value(aParameter - aStep, aBefore));
  ASSERT_TRUE(aFunction.Value(aParameter + aStep, anAfter));
  EXPECT_NEAR(aDerivative, (anAfter - aBefore) / (2.0 * aStep), Precision::Confusion());
}

TEST(Bisector_FunctionHTest, CircleTangencyNearAntiparallelTangents)
{
  const occ::handle<Geom2d_Curve> aCurve =
    new Geom2d_TrimmedCurve(new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 1.0),
                            0.0,
                            M_PI);
  for (const double anX : {-1.0e-5, 0.0, 1.0e-5})
  {
    SCOPED_TRACE(anX);
    Bisector_FunctionH       aFunction(aCurve, gp_Pnt2d(anX, 0.0), gp_Vec2d(1.0, 0.0));
    MathRoot::MultipleConfig aConfig;
    aConfig.NbSamples = 20;
    const MathRoot::MultipleResult aRoots =
      MathRoot::FindAllRootsWithDerivative(aFunction, 0.0, M_PI, aConfig);
    ASSERT_TRUE(aRoots.IsDone());
    ASSERT_EQ(aRoots.NbRoots(), 1);
    const auto [aPoint, aTangent] = aCurve->EvalD1(aRoots.Roots.Value(0));
    const double   aRadius        = 0.5 * (1.0 - anX * anX);
    const gp_Pnt2d aCentre(anX, aRadius);
    EXPECT_NEAR(aCentre.Distance(aPoint), aRadius, Precision::Confusion());
    EXPECT_NEAR(gp_Vec2d(aPoint, aCentre).Dot(aTangent), 0.0, Precision::Confusion());
  }
}

TEST(Bisector_BisecCCTest, SplineEndpointTangencyRetainsOffsetBranch)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 6);
  aPoles.ChangeAt(0) = gp_Pnt2d(1255.0231560677553, 3005.95393292995);
  aPoles.ChangeAt(1) = gp_Pnt2d(1255.0232190702313, 3003.0100100761015);
  aPoles.ChangeAt(2) = gp_Pnt2d(1255.0184659224899, 3000.1711483925742);
  aPoles.ChangeAt(3) = gp_Pnt2d(1255.0185568593738, 2996.1854878390418);
  aPoles.ChangeAt(4) = gp_Pnt2d(1255.0179824861941, 2995.0379088192185);
  aPoles.ChangeAt(5) = gp_Pnt2d(1255.0179629461943, 2995.0376836173023);
  NCollection_Array1<double> aKnots(1, 3);
  aKnots.ChangeAt(0) = 0.0;
  aKnots.ChangeAt(1) = 0.500309227505778;
  aKnots.ChangeAt(2) = 1.0;
  NCollection_Array1<int> aMultiplicities(1, 3);
  aMultiplicities.ChangeAt(0) = 4;
  aMultiplicities.ChangeAt(1) = 2;
  aMultiplicities.ChangeAt(2) = 4;
  const occ::handle<Geom2d_Curve> aSpline =
    new Geom2d_TrimmedCurve(new Geom2d_BSplineCurve(aPoles, aKnots, aMultiplicities, 3),
                            aKnots.At(1),
                            1.0);
  const occ::handle<Geom2d_Curve> aLine =
    new Geom2d_TrimmedCurve(new Geom2d_Line(gp_Pnt2d(1255.0144891820614, 2994.7620926478912),
                                            gp_Dir2d(-0.012529478436019164, -0.9999215030041716)),
                            -0.27561469222034779,
                            0.0);
  for (const double aSide : {1.0, -1.0})
  {
    SCOPED_TRACE(aSide);
    const occ::handle<Geom2d_Curve> aGuide  = occ::down_cast<Geom2d_Curve>(aLine->Copy());
    const occ::handle<Geom2d_Curve> anOther = occ::down_cast<Geom2d_Curve>(aSpline->Copy());
    gp_Pnt2d                        anOrigin(1255.0254143585616, 2995.0370370837445);
    if (aSide < 0.0)
    {
      gp_Trsf2d aMirror;
      aMirror.SetMirror(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)));
      aGuide->Transform(aMirror);
      anOther->Transform(aMirror);
      anOrigin.Transform(aMirror);
    }
    Bisector_BisecCC aBisector(aGuide, anOther, aSide, aSide, anOrigin);
    ASSERT_FALSE(aBisector.IsEmpty());
    const double aParameter = -0.25;
    ASSERT_LT(aBisector.FirstParameter(), aParameter);
    ASSERT_GT(aBisector.LastParameter(), aParameter);
    double         aU1, aU2, aSquaredDistance;
    const gp_Pnt2d aPoint = aBisector.ValueAndDist(aParameter, aU1, aU2, aSquaredDistance);
    const auto [aPoint1, aTangent1] = aGuide->EvalD1(aU1);
    const auto [aPoint2, aTangent2] = anOther->EvalD1(aU2);
    EXPECT_NEAR(aPoint.Distance(aPoint1), aPoint.Distance(aPoint2), Precision::Confusion());
    EXPECT_NEAR(gp_Vec2d(aPoint1, aPoint).Dot(aTangent1) / aTangent1.Magnitude(),
                0.0,
                Precision::Confusion());
    EXPECT_NEAR(gp_Vec2d(aPoint2, aPoint).Dot(aTangent2) / aTangent2.Magnitude(),
                0.0,
                Precision::Confusion());
  }
}

TEST(Bisector_BisecCCTest, ShortCurvedBranchBeforeFirstSample)
{
  for (const double aSide : {1.0, -1.0})
  {
    SCOPED_TRACE(aSide);
    NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
    aPoles.ChangeAt(0)                            = gp_Pnt2d(0.0, 0.0);
    aPoles.ChangeAt(1)                            = gp_Pnt2d(1.0, aSide * 0.5);
    aPoles.ChangeAt(2)                            = gp_Pnt2d(2.0, -aSide);
    aPoles.ChangeAt(3)                            = gp_Pnt2d(3.0, aSide * 20.0);
    const occ::handle<Geom2d_BezierCurve>  aCurve = new Geom2d_BezierCurve(aPoles);
    const occ::handle<Geom2d_TrimmedCurve> aLine =
      new Geom2d_TrimmedCurve(new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, aSide * 0.45)),
                              -2.0,
                              0.0);

    Bisector_BisecCC aBisector(aCurve, aLine, aSide, aSide, gp_Pnt2d(0.0, 0.0));
    ASSERT_FALSE(aBisector.IsEmpty());
    EXPECT_LT(aBisector.FirstParameter(), aBisector.LastParameter());
    EXPECT_LT(aBisector.LastParameter(), 0.05);
    for (int i = 1; i < 20; ++i)
    {
      const double aParameter =
        aBisector.FirstParameter()
        + (aBisector.LastParameter() - aBisector.FirstParameter()) * i / 20.0;
      const gp_Pnt2d                aPoint = aBisector.EvalD0(aParameter);
      Geom2dAPI_ProjectPointOnCurve aCurveProjection(aPoint, aCurve);
      Geom2dAPI_ProjectPointOnCurve aLineProjection(aPoint, aLine);
      ASSERT_GT(aCurveProjection.NbPoints(), 0);
      ASSERT_GT(aLineProjection.NbPoints(), 0);
      EXPECT_NEAR(aCurveProjection.LowerDistance(),
                  aLineProjection.LowerDistance(),
                  Precision::Confusion());
    }
  }
}

TEST(Bisector_BisecCCTest, SmallNonzeroDistancePreservesBisector)
{
  NCollection_Array1<gp_Pnt2d> aFirstPoles(1, 5);
  aFirstPoles.ChangeAt(0) = gp_Pnt2d(274.76521837596579, -560.23592614188806);
  aFirstPoles.ChangeAt(1) = gp_Pnt2d(275.80530458858192, -559.8117621325398);
  aFirstPoles.ChangeAt(2) = gp_Pnt2d(276.84179370140873, -559.37960139823031);
  aFirstPoles.ChangeAt(3) = gp_Pnt2d(277.87459494294785, -558.93949274952695);
  aFirstPoles.ChangeAt(4) = gp_Pnt2d(278.90418838874598, -558.49141145568012);
  NCollection_Array1<gp_Pnt2d> aSecondPoles(1, 5);
  aSecondPoles.ChangeAt(0) = gp_Pnt2d(278.9023830896171, -558.49210491459451);
  aSecondPoles.ChangeAt(1) = gp_Pnt2d(278.90235704463868, -558.49204328473695);
  aSecondPoles.ChangeAt(2) = gp_Pnt2d(278.90776202849247, -558.4896857659794);
  aSecondPoles.ChangeAt(3) = gp_Pnt2d(278.91887970157791, -558.48488457722658);
  aSecondPoles.ChangeAt(4) = gp_Pnt2d(278.93589511135389, -558.47753643940905);
  NCollection_Array1<double> aFirstKnots(1, 2), aSecondKnots(1, 2);
  aFirstKnots.ChangeAt(0)  = 0.71740585649605193;
  aFirstKnots.ChangeAt(1)  = 0.75850777941917302;
  aSecondKnots.ChangeAt(0) = 0.0013307603518307848;
  aSecondKnots.ChangeAt(1) = 0.0069996541768193058;
  NCollection_Array1<int> aMultiplicities(1, 2);
  aMultiplicities.Init(5);
  const occ::handle<Geom2d_BSplineCurve> aFirst =
    new Geom2d_BSplineCurve(aFirstPoles, aFirstKnots, aMultiplicities, 4);
  const occ::handle<Geom2d_BSplineCurve> aSecond =
    new Geom2d_BSplineCurve(aSecondPoles, aSecondKnots, aMultiplicities, 4);
  const gp_Pnt2d anOrigin(278.90230437958468, -558.49213817771044);

  Geom2dAPI_ProjectPointOnCurve aFirstProjection(anOrigin, aFirst);
  Geom2dAPI_ProjectPointOnCurve aSecondProjection(anOrigin, aSecond);
  ASSERT_GT(aFirstProjection.NbPoints(), 0);
  ASSERT_GT(aSecondProjection.NbPoints(), 0);
  const double aDistance = aFirstProjection.LowerDistance();
  ASSERT_GT(aDistance, Precision::Confusion());
  ASSERT_LT(aDistance * aDistance, Precision::Confusion());
  ASSERT_NEAR(aDistance, aSecondProjection.LowerDistance(), Precision::Confusion());

  Bisector_BisecCC aBisector;
  aBisector.Perform(aSecond, aFirst, 1.0, 1.0, anOrigin);
  ASSERT_FALSE(aBisector.IsEmpty());
  EXPECT_LT(aBisector.FirstParameter(), aBisector.LastParameter());
  EXPECT_LE(aBisector.Value(aBisector.FirstParameter()).Distance(anOrigin), Precision::Confusion());
}

TEST(Bisector_BisecCCTest, NonparallelEndTangentsDoNotExtend)
{
  NCollection_Array1<gp_Pnt2d> aParabolaPoles(1, 3);
  aParabolaPoles.ChangeAt(0) = gp_Pnt2d(0.0, 0.0);
  aParabolaPoles.ChangeAt(1) = gp_Pnt2d(0.5, 0.0);
  aParabolaPoles.ChangeAt(2) = gp_Pnt2d(1.0, 1.0);
  NCollection_Array1<gp_Pnt2d> aLinePoles(1, 2);
  aLinePoles.ChangeAt(0)                          = gp_Pnt2d(0.0, 0.0);
  aLinePoles.ChangeAt(1)                          = gp_Pnt2d(1.0, 1.0);
  const occ::handle<Geom2d_BezierCurve> aParabola = new Geom2d_BezierCurve(aParabolaPoles);
  const occ::handle<Geom2d_BezierCurve> aLine     = new Geom2d_BezierCurve(aLinePoles);

  Bisector_BisecCC aBisector(aParabola, aLine, 1.0, -1.0, gp_Pnt2d(0.0, 0.0));
  ASSERT_FALSE(aBisector.IsEmpty());
  EXPECT_FALSE(aBisector.IsExtendAtEnd());
}

TEST(Bisector_BisecCCTest, TangentConcaveCurvesHaveEquidistantBranch)
{
  NCollection_Array1<gp_Pnt2d> aLeftPoles(1, 3), aRightPoles(1, 3);
  aLeftPoles.ChangeAt(0)                     = gp_Pnt2d(-1.0, 1.0);
  aLeftPoles.ChangeAt(1)                     = gp_Pnt2d(-0.5, 0.0);
  aLeftPoles.ChangeAt(2)                     = gp_Pnt2d(0.0, 0.0);
  aRightPoles.ChangeAt(0)                    = gp_Pnt2d(0.0, 0.0);
  aRightPoles.ChangeAt(1)                    = gp_Pnt2d(0.5, 0.0);
  aRightPoles.ChangeAt(2)                    = gp_Pnt2d(1.0, 2.0);
  const occ::handle<Geom2d_Curve> aCurves[2] = {new Geom2d_BezierCurve(aLeftPoles),
                                                new Geom2d_BezierCurve(aRightPoles)};

  for (double aSide : {-1.0, 1.0})
  {
    SCOPED_TRACE(aSide);
    Bisector_Bisec aBisector;
    aBisector.Perform(aCurves[0],
                      aCurves[1],
                      gp_Pnt2d(0.0, 0.0),
                      gp_Vec2d(1.0, 0.0),
                      gp_Vec2d(-1.0, 0.0),
                      aSide,
                      GeomAbs_Arc,
                      Precision::Confusion(),
                      true);
    const occ::handle<Geom2d_TrimmedCurve>& aCurve = aBisector.Value();
    const double                            aFirst = aCurve->FirstParameter();
    const double                            aLast = std::min(aCurve->LastParameter(), aFirst + 2.0);
    ASSERT_GT(aLast, aFirst);
    for (int i = 0; i <= 40; ++i)
    {
      const gp_Pnt2d aPoint = aCurve->Value(aFirst + (aLast - aFirst) * i / 40.0);
      double         aDistances[2];
      for (size_t j = 0; j < 2; ++j)
      {
        aDistances[j] = std::min(aPoint.Distance(aCurves[j]->Value(0.0)),
                                 aPoint.Distance(aCurves[j]->Value(1.0)));
        Geom2dAPI_ProjectPointOnCurve aProjection(aPoint, aCurves[j]);
        if (aProjection.NbPoints() > 0)
        {
          aDistances[j] = std::min(aDistances[j], aProjection.LowerDistance());
        }
      }
      EXPECT_NEAR(aDistances[0], aDistances[1], Precision::Confusion());
    }
  }
}

TEST(Bisector_BisecCCTest, TangentCurvesWithOppositeConcavityHaveEquidistantBranch)
{
  NCollection_Array1<gp_Pnt2d> aLeftPoles(1, 3), aRightPoles(1, 3);
  aLeftPoles.ChangeAt(0)                     = gp_Pnt2d(-1.0, 1.0);
  aLeftPoles.ChangeAt(1)                     = gp_Pnt2d(-0.5, 0.0);
  aLeftPoles.ChangeAt(2)                     = gp_Pnt2d(0.0, 0.0);
  aRightPoles.ChangeAt(0)                    = gp_Pnt2d(0.0, 0.0);
  aRightPoles.ChangeAt(1)                    = gp_Pnt2d(0.5, 0.0);
  aRightPoles.ChangeAt(2)                    = gp_Pnt2d(1.0, -2.0);
  const occ::handle<Geom2d_Curve> aCurves[2] = {new Geom2d_BezierCurve(aLeftPoles),
                                                new Geom2d_BezierCurve(aRightPoles)};

  for (double aSide : {-1.0, 1.0})
  {
    SCOPED_TRACE(aSide);
    Bisector_Bisec aBisector;
    aBisector.Perform(aCurves[0],
                      aCurves[1],
                      gp_Pnt2d(0.0, 0.0),
                      gp_Vec2d(1.0, 0.0),
                      gp_Vec2d(-1.0, 0.0),
                      aSide,
                      GeomAbs_Arc,
                      Precision::Confusion(),
                      true);
    const occ::handle<Geom2d_TrimmedCurve>& aCurve = aBisector.Value();
    ASSERT_FALSE(aCurve.IsNull());
    const double aFirst = aCurve->FirstParameter();
    const double aLast  = std::min(aCurve->LastParameter(), aFirst + 2.0);
    ASSERT_GT(aLast, aFirst);
    for (int i = 0; i <= 40; ++i)
    {
      const gp_Pnt2d aPoint = aCurve->Value(aFirst + (aLast - aFirst) * i / 40.0);
      double         aDistances[2];
      for (size_t j = 0; j < 2; ++j)
      {
        aDistances[j] = std::min(aPoint.Distance(aCurves[j]->Value(0.0)),
                                 aPoint.Distance(aCurves[j]->Value(1.0)));
        Geom2dAPI_ProjectPointOnCurve aProjection(aPoint, aCurves[j]);
        if (aProjection.NbPoints() > 0)
        {
          aDistances[j] = std::min(aDistances[j], aProjection.LowerDistance());
        }
      }
      EXPECT_NEAR(aDistances[0], aDistances[1], Precision::Confusion());
    }
  }
}

TEST(Bisector_BisecCCTest, TangentCircularArcsRetainFiniteNormalSegment)
{
  NCollection_Array1<gp_Pnt2d> aLeftPoles(1, 5), aRightPoles(1, 5);
  aLeftPoles.ChangeAt(0)  = gp_Pnt2d(0.0, -4.0);
  aLeftPoles.ChangeAt(1)  = gp_Pnt2d(-2.0, -4.0);
  aLeftPoles.ChangeAt(2)  = gp_Pnt2d(-2.0, -2.0);
  aLeftPoles.ChangeAt(3)  = gp_Pnt2d(-2.0, 0.0);
  aLeftPoles.ChangeAt(4)  = gp_Pnt2d(0.0, 0.0);
  aRightPoles.ChangeAt(0) = gp_Pnt2d(0.0, 0.0);
  aRightPoles.ChangeAt(1) = gp_Pnt2d(1.0, 0.0);
  aRightPoles.ChangeAt(2) = gp_Pnt2d(1.0, 1.0);
  aRightPoles.ChangeAt(3) = gp_Pnt2d(1.0, 2.0);
  aRightPoles.ChangeAt(4) = gp_Pnt2d(0.0, 2.0);
  NCollection_Array1<double> aWeights(1, 5), aKnots(1, 3);
  NCollection_Array1<int>    aMultiplicities(1, 3);
  aWeights.Init(1.0);
  aWeights.ChangeAt(1) = std::sqrt(0.5);
  aWeights.ChangeAt(3) = std::sqrt(0.5);
  aKnots.ChangeAt(0)   = 0.0;
  aKnots.ChangeAt(1)   = 0.5;
  aKnots.ChangeAt(2)   = 1.0;
  aMultiplicities.Init(2);
  aMultiplicities.ChangeAt(0) = 3;
  aMultiplicities.ChangeAt(2) = 3;
  const occ::handle<Geom2d_BSplineCurve> aLeft =
    new Geom2d_BSplineCurve(aLeftPoles, aWeights, aKnots, aMultiplicities, 2);
  const occ::handle<Geom2d_BSplineCurve> aRight =
    new Geom2d_BSplineCurve(aRightPoles, aWeights, aKnots, aMultiplicities, 2);
  for (const bool isReflected : {false, true})
  {
    const occ::handle<Geom2d_Curve> aGuide  = occ::down_cast<Geom2d_Curve>(aRight->Copy());
    const occ::handle<Geom2d_Curve> anOther = occ::down_cast<Geom2d_Curve>(aLeft->Copy());
    const double                    aSide   = isReflected ? -1.0 : 1.0;
    if (isReflected)
    {
      gp_Trsf2d aMirror;
      aMirror.SetMirror(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)));
      aGuide->Transform(aMirror);
      anOther->Transform(aMirror);
    }
    for (const double anOriginY : {0.0, 0.25})
    {
      for (const double aDistMax : {0.5, 2.0})
      {
        SCOPED_TRACE(::testing::Message() << isReflected << "," << anOriginY << "," << aDistMax);
        Bisector_BisecCC aBisector(aGuide,
                                   anOther,
                                   aSide,
                                   aSide,
                                   gp_Pnt2d(0.0, aSide * anOriginY),
                                   aDistMax);
        ASSERT_FALSE(aBisector.IsEmpty());
        const double aLastY = std::min(1.0, aDistMax);
        EXPECT_NEAR(aBisector.LastParameter() - aBisector.FirstParameter(),
                    aLastY - anOriginY,
                    Precision::Confusion());
        EXPECT_EQ(aBisector.NbIntervals(), 1);
        for (int i = 0; i <= 10; ++i)
        {
          const double aFraction   = i / 10.0;
          const double anExpectedY = anOriginY + aFraction * (aLastY - anOriginY);
          const double aParameter =
            aBisector.FirstParameter()
            + aFraction * (aBisector.LastParameter() - aBisector.FirstParameter());
          double         aU1, aU2, aDistance;
          const gp_Pnt2d aPoint = aBisector.ValueAndDist(aParameter, aU1, aU2, aDistance);
          EXPECT_NEAR(aPoint.X(), 0.0, Precision::Confusion());
          EXPECT_NEAR(aPoint.Y(), aSide * anExpectedY, Precision::Confusion());
          EXPECT_NEAR(aDistance, anExpectedY, Precision::Confusion());
          EXPECT_DOUBLE_EQ(aU1, aGuide->FirstParameter());
          EXPECT_DOUBLE_EQ(aU2, anOther->LastParameter());
        }
      }
    }
  }
}

TEST(Bisector_BisecCCTest, CoincidentTangentEndpointsHaveNoIsolatedIntersection)
{
  for (const double anOriginY : {0.0, 1000.0, -1000.0})
  {
    SCOPED_TRACE(anOriginY);
    const gp_Pnt2d               anOrigin(0.0, anOriginY);
    const double                 aGap = Epsilon(std::max(1.0, std::abs(anOriginY)));
    NCollection_Array1<gp_Pnt2d> aLeftPoles(1, 3), aRightPoles(1, 3);
    aLeftPoles.ChangeAt(0)                 = gp_Pnt2d(-1.0, anOriginY + 1.0);
    aLeftPoles.ChangeAt(1)                 = gp_Pnt2d(-0.5, anOriginY);
    aLeftPoles.ChangeAt(2)                 = anOrigin;
    aRightPoles.ChangeAt(0)                = gp_Pnt2d(0.0, anOriginY - aGap);
    aRightPoles.ChangeAt(1)                = gp_Pnt2d(0.5, anOriginY - aGap);
    aRightPoles.ChangeAt(2)                = gp_Pnt2d(1.0, anOriginY + 2.0 - aGap);
    const occ::handle<Geom2d_Curve> aLeft  = new Geom2d_BezierCurve(aLeftPoles);
    const occ::handle<Geom2d_Curve> aRight = new Geom2d_BezierCurve(aRightPoles);

    for (const double aSide : {-1.0, 1.0})
    {
      SCOPED_TRACE(aSide);
      Bisector_BisecCC aBisector;
      aBisector.Perform(aRight, aLeft, aSide, aSide, anOrigin);
      if (aSide < 0.0)
      {
        Bisector_Bisec aNormal;
        aNormal.Perform(aLeft,
                        aRight,
                        anOrigin,
                        gp_Vec2d(1.0, 0.0),
                        gp_Vec2d(-1.0, 0.0),
                        aSide,
                        GeomAbs_Arc,
                        Precision::Confusion(),
                        true);
        const occ::handle<Geom2d_TrimmedCurve>& aNormalCurve = aNormal.Value();
        const double                            aDistance    = 0.1;
        const gp_Pnt2d                          aNormalPoint =
          aNormalCurve->EvalD0(aNormalCurve->FirstParameter() + aDistance);
        EXPECT_NEAR(aNormalPoint.X(), anOrigin.X(), Precision::Confusion());
        EXPECT_NEAR(aNormalPoint.Y(), anOriginY - aDistance, Precision::Confusion());
      }
      EXPECT_EQ(aBisector.IsEmpty(), aSide < 0.0);
      EXPECT_EQ(aBisector.IsExtendAtStart(), aSide > 0.0);
      const Bisector_BisecCC aConstructed(aRight, aLeft, aSide, aSide, anOrigin);
      EXPECT_EQ(aConstructed.LinkBisCurve(aRight->FirstParameter()), aRight->FirstParameter());
      double         aFirstParameter, aSecondParameter, aDistance;
      const gp_Pnt2d aPoint = aBisector.ValueByInt(aRight->FirstParameter(),
                                                   aFirstParameter,
                                                   aSecondParameter,
                                                   aDistance);
      EXPECT_LE(aPoint.Distance(anOrigin), Precision::Confusion());
      EXPECT_EQ(aFirstParameter, aRight->FirstParameter());
      EXPECT_EQ(aSecondParameter, aLeft->LastParameter());
      EXPECT_EQ(aDistance, aSide > 0.0 ? Precision::Infinite() : 0.0);
    }
  }
}

TEST(Bisector_BisecCCTest, TangentSplineJoinDoesNotCreateZeroDistanceBranch)
{
  NCollection_Array1<int> aMultiplicities(1, 2);
  aMultiplicities.Init(9);
  NCollection_Array1<gp_Pnt2d> aPoles1(1, 9);
  aPoles1.ChangeAt(0) = gp_Pnt2d(-307.90510129766335, -1940.0660319358035);
  aPoles1.ChangeAt(1) = gp_Pnt2d(-314.63757106336578, -1945.8142194005986);
  aPoles1.ChangeAt(2) = gp_Pnt2d(-321.36902621150097, -1951.5633887030001);
  aPoles1.ChangeAt(3) = gp_Pnt2d(-328.09965872367707, -1957.3137036587514);
  aPoles1.ChangeAt(4) = gp_Pnt2d(-334.82947573080526, -1963.066955250099);
  aPoles1.ChangeAt(5) = gp_Pnt2d(-341.55815318455893, -1968.827555440163);
  aPoles1.ChangeAt(6) = gp_Pnt2d(-348.28483899361532, -1974.6038584825801);
  aPoles1.ChangeAt(7) = gp_Pnt2d(-355.00789547596219, -1980.4098735134642);
  aPoles1.ChangeAt(8) = gp_Pnt2d(-361.72457100928943, -1986.267432239546);
  NCollection_Array1<double> aKnots1(1, 2);
  aKnots1.ChangeAt(0) = 0.5823015797383948;
  aKnots1.ChangeAt(1) = 0.59682247666440191;
  const occ::handle<Geom2d_Curve> aSpline1 =
    new Geom2d_BSplineCurve(aPoles1, aKnots1, aMultiplicities, 8);
  const occ::handle<Geom2d_Curve> aCurve1 =
    new Geom2d_TrimmedCurve(aSpline1, aKnots1.First(), aKnots1.Last());
  NCollection_Array1<gp_Pnt2d> aPoles2(1, 9);
  aPoles2.ChangeAt(0) = gp_Pnt2d(-361.72457100928943, -1986.267432239546);
  aPoles2.ChangeAt(1) = gp_Pnt2d(-372.4080793643862, -1995.5844339735222);
  aPoles2.ChangeAt(2) = gp_Pnt2d(-383.03339604604821, -2005.0137664357874);
  aPoles2.ChangeAt(3) = gp_Pnt2d(-393.61304267256423, -2014.5303608158026);
  aPoles2.ChangeAt(4) = gp_Pnt2d(-404.15835402544838, -2024.111634054927);
  aPoles2.ChangeAt(5) = gp_Pnt2d(-414.67947804916571, -2033.7374888461734);
  aPoles2.ChangeAt(6) = gp_Pnt2d(-425.18537585096237, -2043.3903136340591);
  aPoles2.ChangeAt(7) = gp_Pnt2d(-435.6838217007608, -2053.0549826145161);
  aPoles2.ChangeAt(8) = gp_Pnt2d(-446.18140303109612, -2062.7188557348354);
  const double aGap   = Epsilon(std::abs(aPoles2.At(0).Y()));
  aPoles2.ChangeAt(0).SetY(aPoles2.At(0).Y() + aGap);
  aPoles2.ChangeAt(1).SetY(aPoles2.At(1).Y() + aGap);
  NCollection_Array1<double> aKnots2(1, 2);
  aKnots2.ChangeAt(0) = 0.59682247666440191;
  aKnots2.ChangeAt(1) = 0.61991933816188072;
  const occ::handle<Geom2d_Curve> aSpline2 =
    new Geom2d_BSplineCurve(aPoles2, aKnots2, aMultiplicities, 8);
  const occ::handle<Geom2d_Curve> aCurve2 =
    new Geom2d_TrimmedCurve(aSpline2, aKnots2.First(), aKnots2.Last());

  const auto aLeftEnd    = aCurve1->EvalD1(aCurve1->LastParameter());
  const auto aRightStart = aCurve2->EvalD1(aCurve2->FirstParameter());
  ASSERT_TRUE(aLeftEnd.D1.IsParallel(aRightStart.D1, Precision::Angular()));
  const gp_Pnt2d   anOrigin = aPoles1.Last();
  Bisector_BisecCC aBisector;
  aBisector.Perform(aCurve2, aCurve1, 1.0, 1.0, anOrigin);
  ASSERT_FALSE(aBisector.IsEmpty());
  EXPECT_TRUE(aBisector.IsExtendAtStart());
  double         aFirstParameter, aSecondParameter, aDistance;
  const gp_Pnt2d aPoint =
    aBisector.ValueByInt(aCurve2->FirstParameter(), aFirstParameter, aSecondParameter, aDistance);
  EXPECT_LE(aPoint.Distance(anOrigin), Precision::Confusion());
  EXPECT_EQ(aDistance, Precision::Infinite());
}

TEST(Bisector_BisecCCTest, TangentArcNormalSegmentIsStableAtLargeRadii)
{
  const gp_Dir2d             aTangent(-1.0, -0.3);
  const gp_Dir2d             aNormal(-aTangent.Y(), aTangent.X());
  NCollection_Array1<double> aKnots(1, 2);
  for (const gp_Pnt2d& aJoin :
       {gp_Pnt2d(0.0, 0.0), gp_Pnt2d(5000.0, 1000.0), gp_Pnt2d(-5000.0, -1000.0)})
  {
    SCOPED_TRACE(::testing::Message() << aJoin.X() << "," << aJoin.Y());
    occ::handle<Geom2d_Curve> aCurves[2];
    for (int i = 0; i < 2; ++i)
    {
      const double   aRadius = i == 0 ? 200000.0 : 20000.0;
      const double   aSense  = i == 0 ? 1.0 : -1.0;
      const gp_Pnt2d aCentre = aJoin.Translated(aSense * aRadius * gp_Vec2d(aNormal));
      const gp_Dir2d aRadial(-aSense * aNormal.X(), -aSense * aNormal.Y());
      const occ::handle<Geom2d_Circle> aCircle =
        new Geom2d_Circle(gp_Ax2d(aCentre, aRadial), aRadius, i == 0);
      const occ::handle<Geom2d_BSplineCurve> aCurve =
        Geom2dConvert::CurveToBSplineCurve(new Geom2d_TrimmedCurve(aCircle,
                                                                   i == 0 ? -300.0 / aRadius : 0.0,
                                                                   i == 0 ? 0.0 : 300.0 / aRadius));
      ASSERT_EQ(aCurve->NbKnots(), 2);
      aKnots.Assign(aCurve->Knots());
      BSplCLib::Reparametrize(i == 0 ? 0.0 : 0.5, i == 0 ? 0.5 : 1.0, aKnots);
      aCurve->SetKnots(aKnots);
      aCurves[i] = aCurve;
    }
    const gp_Pnt2d anOrigin = aCurves[0]->EvalD0(aCurves[0]->LastParameter());
    for (const double aSide : {-1.0, 1.0})
    {
      SCOPED_TRACE(aSide);
      Bisector_BisecCC aBisector(aCurves[1], aCurves[0], aSide, aSide, anOrigin, 500.0);
      ASSERT_FALSE(aBisector.IsEmpty());
      const double aLength = aBisector.LastParameter() - aBisector.FirstParameter();
      EXPECT_NEAR(aLength, 500.0, Precision::Confusion());
      if (std::abs(aLength - 500.0) > Precision::Confusion())
      {
        continue;
      }
      for (const double aDistance : {0.0, 157.0, 500.0})
      {
        double         aU1, aU2, aReportedDistance;
        const gp_Pnt2d aPoint     = aBisector.ValueAndDist(aBisector.FirstParameter() + aDistance,
                                                       aU1,
                                                       aU2,
                                                       aReportedDistance);
        const gp_Pnt2d anExpected = anOrigin.Translated(aSide * aDistance * gp_Vec2d(aNormal));
        EXPECT_LE(aPoint.Distance(anExpected), Precision::Confusion());
        EXPECT_NEAR(aReportedDistance, aDistance, Precision::Confusion());
        for (const auto& aCurve : aCurves)
        {
          double aClosest = std::min(aPoint.Distance(aCurve->EvalD0(aCurve->FirstParameter())),
                                     aPoint.Distance(aCurve->EvalD0(aCurve->LastParameter())));
          Geom2dAPI_ProjectPointOnCurve aProjection(aPoint, aCurve);
          if (aProjection.NbPoints() > 0)
          {
            aClosest = std::min(aClosest, aProjection.LowerDistance());
          }
          EXPECT_NEAR(aClosest, aDistance, Precision::Confusion());
        }
      }
    }
  }
}
