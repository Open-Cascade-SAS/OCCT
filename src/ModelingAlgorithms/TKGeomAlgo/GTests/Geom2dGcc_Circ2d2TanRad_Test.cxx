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

#include <GccEnt.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_Circ2d2TanRad.hxx>
#include <Geom2dGcc_Circ2dTanCen.hxx>
#include <Geom2dGcc.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Line.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>

#include <GCPnts_AbscissaPoint.hxx>

namespace
{
double circleLength(const gp_Circ2d& theCircle)
{
  const occ::handle<Geom2d_Circle> aCurve = new Geom2d_Circle(theCircle);
  Geom2dAdaptor_Curve              anAdaptor(aCurve);
  return GCPnts_AbscissaPoint::Length(anAdaptor);
}
} // namespace

// Test BUC60897: Geom2dGcc_Circ2d2TanRad finds circles tangent to a line and
// a Bezier curve. Verifies that each tangency point lies at the circle radius
// from the circle center (within 1% relative tolerance).
TEST(Geom2dGcc_Circ2d2TanRadTest, BUC60897_TangentToLineAndBezier)
{
  // Create a Geom2d_Line from point (100, 0) going in the -X direction
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(gp_Pnt2d(100, 0), gp_Dir2d(gp_Dir2d::D::NX));

  // Create a Geom2d_BezierCurve from three control points
  NCollection_Array1<gp_Pnt2d> aPoints(1, 3);
  aPoints.SetValue(1, gp_Pnt2d(0, 0));
  aPoints.SetValue(2, gp_Pnt2d(50, 50));
  aPoints.SetValue(3, gp_Pnt2d(0, 100));
  occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoints);

  // Build qualified curves (outside tangency)
  Geom2dAdaptor_Curve      aCLine(aLine);
  Geom2dAdaptor_Curve      aCCurve(aCurve);
  Geom2dGcc_QualifiedCurve aQualifCurve1(aCLine, GccEnt_outside);
  Geom2dGcc_QualifiedCurve aQualifCurve2(aCCurve, GccEnt_outside);

  // Find circles with radius 10 tangent to both curves
  const double            aRadius    = 10.0;
  const double            aTolerance = 1e-7;
  Geom2dGcc_Circ2d2TanRad aGccCirc2d(aQualifCurve1, aQualifCurve2, aRadius, aTolerance);

  ASSERT_TRUE(aGccCirc2d.IsDone()) << "Geom2dGcc_Circ2d2TanRad failed to compute";
  ASSERT_GT(aGccCirc2d.NbSolutions(), 0) << "No tangent circles found";

  // For each solution, verify tangency points are at the circle radius from its center
  const double aMaxDeltaPercent = 1.0; // 1% tolerance as used in the original Draw test
  for (int i = 1; i <= aGccCirc2d.NbSolutions(); i++)
  {
    const gp_Circ2d aCirc2d = aGccCirc2d.ThisSolution(i);
    const gp_Pnt2d  aCenter = aCirc2d.Location();
    const double    aR      = aCirc2d.Radius();

    double   aParSol1, aParArg1, aParSol2, aParArg2;
    gp_Pnt2d aPntSol1, aPntSol2;
    aGccCirc2d.Tangency1(i, aParSol1, aParArg1, aPntSol1);
    aGccCirc2d.Tangency2(i, aParSol2, aParArg2, aPntSol2);

    // Distance from tangency point 1 to circle center must equal radius within 1%
    const double aD1     = aPntSol1.Distance(aCenter);
    const double aDelta1 = std::abs(aD1 - aR) / aR * 100.0;
    EXPECT_LE(aDelta1, aMaxDeltaPercent)
      << "Solution " << i << ": tangency1 distance error " << aDelta1 << "% exceeds 1%";

    // Distance from tangency point 2 to circle center must equal radius within 1%
    const double aD2     = aPntSol2.Distance(aCenter);
    const double aDelta2 = std::abs(aD2 - aR) / aR * 100.0;
    EXPECT_LE(aDelta2, aMaxDeltaPercent)
      << "Solution " << i << ": tangency2 distance error " << aDelta2 << "% exceeds 1%";
  }
}

// Migrated from tests/lowalgos/2dgcc/buc60607_1.  Keep the two solution
// lengths produced by a point/line fixed-radius construction.
TEST(Geom2dGcc_Circ2d2TanRadTest, BUC60607_1_PointLineFixedRadius)
{
  const occ::handle<Geom2d_Line> aLine =
    new Geom2d_Line(gp_Pnt2d(0.0, 500.0), gp_Dir2d(2000.0, 500.0));
  Geom2dAdaptor_Curve      anAdaptor(aLine);
  Geom2dGcc_QualifiedCurve aQualifiedLine = Geom2dGcc::Unqualified(anAdaptor);

  Geom2dGcc_Circ2d2TanRad aSolver(aQualifiedLine,
                                  new Geom2d_CartesianPoint(gp_Pnt2d(1600.0, 300.0)),
                                  400.0,
                                  Precision::Confusion());
  ASSERT_TRUE(aSolver.IsDone());
  ASSERT_EQ(aSolver.NbSolutions(), 2);

  for (int anIndex = 1; anIndex <= aSolver.NbSolutions(); ++anIndex)
  {
    EXPECT_NEAR(circleLength(aSolver.ThisSolution(anIndex)),
                2513.2741228718346,
                2513.2741228718346 * 1.e-8);
  }
}

// Migrated from tests/lowalgos/2dgcc/buc60607_2.  The DRAW case did not
// print a result, but the construction is a valid no-solution configuration.
TEST(Geom2dGcc_Circ2d2TanRadTest, BUC60607_2_PointLineFixedRadiusNoSolution)
{
  const occ::handle<Geom2d_Line> aLine =
    new Geom2d_Line(gp_Pnt2d(0.0, 500.0), gp_Dir2d(2000.0, 500.0));
  Geom2dAdaptor_Curve      anAdaptor(aLine);
  Geom2dGcc_QualifiedCurve aQualifiedLine = Geom2dGcc::Unqualified(anAdaptor);

  Geom2dGcc_Circ2d2TanRad aSolver(aQualifiedLine,
                                  new Geom2d_CartesianPoint(gp_Pnt2d(1600.0, 300.0)),
                                  200.0,
                                  Precision::Confusion());
  ASSERT_TRUE(aSolver.IsDone());
  EXPECT_EQ(aSolver.NbSolutions(), 0);
}

// Migrated from tests/lowalgos/2dgcc/buc60607_3.  Preserve the exact DRAW
// line definition (the last two values are its direction, not an endpoint).
TEST(Geom2dGcc_Circ2d2TanRadTest, BUC60607_3_PointLineFixedRadiusNoSolution)
{
  const occ::handle<Geom2d_Line> aLine =
    new Geom2d_Line(gp_Pnt2d(3000.0, 1700.0), gp_Dir2d(1000.0, 1550.0));
  Geom2dAdaptor_Curve      anAdaptor(aLine);
  Geom2dGcc_QualifiedCurve aQualifiedLine = Geom2dGcc::Unqualified(anAdaptor);

  Geom2dGcc_Circ2d2TanRad aSolver(aQualifiedLine,
                                  new Geom2d_CartesianPoint(gp_Pnt2d(1400.0, 1400.0)),
                                  300.0,
                                  Precision::Confusion());
  ASSERT_TRUE(aSolver.IsDone());
  EXPECT_EQ(aSolver.NbSolutions(), 0);
}

// Migrated from tests/lowalgos/2dgcc/buc60607_4.  This is the same geometry
// as BUC60607_3 with the smaller requested radius.
TEST(Geom2dGcc_Circ2d2TanRadTest, BUC60607_4_PointLineFixedRadiusNoSolution)
{
  const occ::handle<Geom2d_Line> aLine =
    new Geom2d_Line(gp_Pnt2d(3000.0, 1700.0), gp_Dir2d(1000.0, 1550.0));
  Geom2dAdaptor_Curve      anAdaptor(aLine);
  Geom2dGcc_QualifiedCurve aQualifiedLine = Geom2dGcc::Unqualified(anAdaptor);

  Geom2dGcc_Circ2d2TanRad aSolver(aQualifiedLine,
                                  new Geom2d_CartesianPoint(gp_Pnt2d(1400.0, 1400.0)),
                                  150.0,
                                  Precision::Confusion());
  ASSERT_TRUE(aSolver.IsDone());
  EXPECT_EQ(aSolver.NbSolutions(), 0);
}

// Migrated from tests/lowalgos/2dgcc/buc60941.  The DRAW regression is about
// the two-curve constructor not raising while processing a BSpline curve.
TEST(Geom2dGcc_Circ2d2TanRadTest, BUC60941_TwoCurveFixedRadiusDoesNotThrow)
{
  const occ::handle<Geom2d_Circle> aCircle =
    new Geom2d_Circle(gp_Circ2d(gp_Ax2d(gp_Pnt2d(1.79356127430613, -11.9148900515605),
                                        gp_Dir2d(1.0, 0.0)),
                                12.0491272292007));

  NCollection_Array1<gp_Pnt2d> aPoles(1, 6);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(-2.14533906935467, 4.92056269221697);
  aPoles(3) = gp_Pnt2d(-0.537398638327716, 10.0937503357781);
  aPoles(4) = gp_Pnt2d(10.3564232507092, 8.73929826869582);
  aPoles(5) = gp_Pnt2d(16.6366922939454, 3.11858911810943);
  aPoles(6) = gp_Pnt2d(16.39, -0.262);

  NCollection_Array1<double> aKnots(1, 4);
  aKnots(1) = 0.0;
  aKnots(2) = 8.6971225126475;
  aKnots(3) = 16.903955884905;
  aKnots(4) = 28.0087167558108;

  NCollection_Array1<int> aMultiplicities(1, 4);
  aMultiplicities(1) = 4;
  aMultiplicities(2) = 1;
  aMultiplicities(3) = 1;
  aMultiplicities(4) = 4;

  NCollection_Array1<double> aWeights(1, 6);
  for (int anIndex = aWeights.Lower(); anIndex <= aWeights.Upper(); ++anIndex)
  {
    aWeights(anIndex) = 1.0;
  }
  const occ::handle<Geom2d_BSplineCurve> aBSpline =
    new Geom2d_BSplineCurve(aPoles, aWeights, aKnots, aMultiplicities, 3);

  Geom2dAdaptor_Curve      anAdaptor1(aCircle);
  Geom2dAdaptor_Curve      anAdaptor2(aBSpline);
  Geom2dGcc_QualifiedCurve aQualifiedCircle = Geom2dGcc::Unqualified(anAdaptor1);
  Geom2dGcc_QualifiedCurve aQualifiedBSpline = Geom2dGcc::Unqualified(anAdaptor2);

  bool anIsDone = false;
  int  aNbSolutions = -1;
  EXPECT_NO_THROW({
    Geom2dGcc_Circ2d2TanRad aSolver(aQualifiedCircle,
                                    aQualifiedBSpline,
                                    2.0,
                                    Precision::Confusion());
    anIsDone = aSolver.IsDone();
    if (anIsDone)
    {
      aNbSolutions = aSolver.NbSolutions();
    }
  });
  EXPECT_TRUE(anIsDone);
  EXPECT_GE(aNbSolutions, 0);
}

// Migrated from tests/lowalgos/2dgcc/bug29694.  This keeps the center and
// radius checks from the DRAW script while using the algorithm API directly.
TEST(Geom2dGcc_Circ2d2TanRadTest, Bug29694_CurvePointCenter)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles(1) = gp_Pnt2d(-10.0, -5.0);
  aPoles(2) = gp_Pnt2d(10.0, -5.0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMultiplicities(1, 2);
  aMultiplicities(1) = 2;
  aMultiplicities(2) = 2;
  const occ::handle<Geom2d_BSplineCurve> aSegment =
    new Geom2d_BSplineCurve(aPoles, aKnots, aMultiplicities, 1);

  Geom2dAdaptor_Curve      anAdaptor(aSegment);
  Geom2dGcc_QualifiedCurve aQualifiedSegment = Geom2dGcc::Unqualified(anAdaptor);
  Geom2dGcc_Circ2dTanCen   aSolver(aQualifiedSegment,
                                   new Geom2d_CartesianPoint(gp_Pnt2d(1.0, 1.0)),
                                   Precision::Confusion());

  ASSERT_TRUE(aSolver.IsDone());
  ASSERT_EQ(aSolver.NbSolutions(), 1);
  const gp_Circ2d aSolution = aSolver.ThisSolution(1);
  EXPECT_NEAR(aSolution.Location().X(), 1.0, 1.e-7);
  EXPECT_NEAR(aSolution.Location().Y(), 1.0, 1.e-7);
  EXPECT_NEAR(aSolution.Radius(), 6.0, 1.e-7);
}
