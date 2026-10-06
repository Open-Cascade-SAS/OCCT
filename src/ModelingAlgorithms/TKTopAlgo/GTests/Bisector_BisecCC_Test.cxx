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

#include <algorithm>

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
