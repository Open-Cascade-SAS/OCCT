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
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Line.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>

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
    const double aD1    = aPntSol1.Distance(aCenter);
    const double aDelta1 = std::abs(aD1 - aR) / aR * 100.0;
    EXPECT_LE(aDelta1, aMaxDeltaPercent) << "Solution " << i << ": tangency1 distance error " << aDelta1 << "% exceeds 1%";

    // Distance from tangency point 2 to circle center must equal radius within 1%
    const double aD2    = aPntSol2.Distance(aCenter);
    const double aDelta2 = std::abs(aD2 - aR) / aR * 100.0;
    EXPECT_LE(aDelta2, aMaxDeltaPercent) << "Solution " << i << ": tangency2 distance error " << aDelta2 << "% exceeds 1%";
  }
}
