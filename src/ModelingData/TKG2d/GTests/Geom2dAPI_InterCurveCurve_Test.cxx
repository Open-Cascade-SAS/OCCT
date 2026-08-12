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

#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAPI_ExtremaCurveCurve.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <Precision.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

TEST(Geom2dAPI_InterCurveCurve_Test, OCC24889_IntersectionParameterWithinLimits)
{
  // Bug OCC24889: Geom2dAPI_InterCurveCurve produces result with parameter outside the curve limits
  // This test verifies that intersection parameters are within the curve parameter limits

  // Create two circles
  occ::handle<Geom2d_Circle> aCircle1 =
    new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(25, -25), gp_Dir2d(gp_Dir2d::D::X), gp_Dir2d(-0, 1)), 155);

  occ::handle<Geom2d_Circle> aCircle2 =
    new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(25, 25), gp_Dir2d(gp_Dir2d::D::X), gp_Dir2d(-0, 1)), 155);

  // Create trimmed curves
  occ::handle<Geom2d_TrimmedCurve> aTrim1 =
    new Geom2d_TrimmedCurve(aCircle1, 1.57079632679490, 2.97959469729228);
  occ::handle<Geom2d_TrimmedCurve> aTrim2 =
    new Geom2d_TrimmedCurve(aCircle2, 3.30359060633978, 4.71238898038469);

  // Perform intersection
  constexpr double          aTol = Precision::Confusion();
  Geom2dAPI_InterCurveCurve aIntTool(aTrim1, aTrim2, aTol);

  ASSERT_GT(aIntTool.NbPoints(), 0) << "Intersection should find at least one point";

  const IntRes2d_IntersectionPoint& aIntPnt = aIntTool.Intersector().Point(1);

  double aParOnC1   = aIntPnt.ParamOnFirst();
  double aParOnC2   = aIntPnt.ParamOnSecond();
  double aFirstPar1 = aTrim1->FirstParameter();
  double aLastPar1  = aTrim1->LastParameter();
  double aFirstPar2 = aTrim2->FirstParameter();
  double aLastPar2  = aTrim2->LastParameter();

  // Verify that intersection parameters are within the curve limits
  EXPECT_GE(aParOnC1, aFirstPar1) << "IntParameter on curve 1 (" << aParOnC1
                                  << ") should be >= FirstParam (" << aFirstPar1 << ")";
  EXPECT_LE(aParOnC1, aLastPar1) << "IntParameter on curve 1 (" << aParOnC1
                                 << ") should be <= LastParam (" << aLastPar1 << ")";

  EXPECT_GE(aParOnC2, aFirstPar2) << "IntParameter on curve 2 (" << aParOnC2
                                  << ") should be >= FirstParam (" << aFirstPar2 << ")";
  EXPECT_LE(aParOnC2, aLastPar2) << "IntParameter on curve 2 (" << aParOnC2
                                 << ") should be <= LastParam (" << aLastPar2 << ")";

  // Verify that both curves evaluate to the same point at the intersection parameters
  gp_Pnt2d aP1 = aTrim1->Value(aParOnC1);
  gp_Pnt2d aP2 = aTrim2->Value(aParOnC2);

  double aDist2 = aP1.SquareDistance(aP2);
  EXPECT_LT(aDist2, 1.0e-14) << "Points on both curves at intersection parameters should coincide";
}

TEST(Geom2dAPI_InterCurveCurve_Test, FClassesBug_25635_1_TangentEllipses)
{
  // fclasses/bug25635_1: extrema between two tangent ellipses includes the zero-distance result.
  occ::handle<Geom2d_Ellipse> anEllipse1 =
    new Geom2d_Ellipse(gp_Elips2d(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 2.0, 1.0));
  occ::handle<Geom2d_Ellipse> anEllipse2 =
    new Geom2d_Ellipse(gp_Elips2d(gp_Ax2d(gp_Pnt2d(4.0, 0.0), gp_Dir2d(1.0, 0.0)), 2.0, 1.0));

  Geom2dAPI_ExtremaCurveCurve anExtrema(anEllipse1,
                                        anEllipse2,
                                        anEllipse1->FirstParameter(),
                                        anEllipse1->LastParameter(),
                                        anEllipse2->FirstParameter(),
                                        anEllipse2->LastParameter());
  ASSERT_GT(anExtrema.NbExtrema(), 0);
  EXPECT_NEAR(anExtrema.Distance(1), 0.0, 7.0e-5);
}

TEST(Geom2dAPI_InterCurveCurve_Test, FClassesBug_25635_2_TangentCircles)
{
  // fclasses/bug25635_2: all four extrema are reported and ext_2 is the tangent point.
  occ::handle<Geom2d_Circle> aCircle1 =
    new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0), gp_Dir2d(0.0, 1.0)), 2.0);
  occ::handle<Geom2d_Circle> aCircle2 =
    new Geom2d_Circle(gp_Ax22d(gp_Pnt2d(4.0, 0.0), gp_Dir2d(1.0, 0.0), gp_Dir2d(0.0, 1.0)), 2.0);

  Geom2dAPI_ExtremaCurveCurve anExtrema(aCircle1,
                                        aCircle2,
                                        aCircle1->FirstParameter(),
                                        aCircle1->LastParameter(),
                                        aCircle2->FirstParameter(),
                                        aCircle2->LastParameter());
  ASSERT_GE(anExtrema.NbExtrema(), 4);
  EXPECT_LE(anExtrema.Distance(2), Precision::PConfusion());

  gp_Pnt2d aPoint1, aPoint2;
  anExtrema.Points(2, aPoint1, aPoint2);
  EXPECT_TRUE(aPoint1.IsEqual(aPoint2, Precision::PConfusion()));
}
