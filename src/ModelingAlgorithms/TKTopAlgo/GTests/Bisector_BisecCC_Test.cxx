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
#include <Bisector_Bisec.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Precision.hxx>
#include <Standard_Real.hxx>

#include <algorithm>
#include <cmath>

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
