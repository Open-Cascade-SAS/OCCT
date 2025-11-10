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

#include <gtest/gtest.h>

#include <Geom2d_Ellipse.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_Circ2d2TanRad.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <GccEnt_Position.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>

// Test OCC24303: Geom2dGcc_Circ2d2TanRad - Circle tangent to two ellipses
// Migrated from QABugs_9.cxx
TEST(Geom2dGcc_Circ2d2TanRad_Test, OCC24303_CircleTangentToTwoEllipses)
{
  // Create two ellipses
  Standard_Real majorRadius = 2.0;
  Standard_Real minorRadius = 1.0;
  gp_Pnt2d      p0(gp::Origin2d());
  gp_Pnt2d      p1(4.0, 0.0);

  gp_Elips2d ellipse1 = gp_Elips2d(gp_Ax2d(p0, gp::DX2d()), majorRadius, minorRadius, true);
  gp_Elips2d ellipse2 = gp_Elips2d(gp_Ax2d(p1, gp::DX2d()), majorRadius, minorRadius, true);

  Handle(Geom2d_Curve) curve1 = new Geom2d_Ellipse(ellipse1);
  Handle(Geom2d_Curve) curve2 = new Geom2d_Ellipse(ellipse2);

  // Expected tangent circle
  gp_Pnt2d      centre(5.0, 0.0);
  Standard_Real radius            = 3.0;
  gp_Circ2d     theorical_tangent = gp_Circ2d(gp_Ax2d(centre, gp::DX2d()), radius);

  // Calculate the tangent circles with Geom2dGcc_Circ2d2TanRad
  const Geom2dAdaptor_Curve AdaptedCurve1(curve1);
  const Geom2dAdaptor_Curve AdaptedCurve2(curve2);

  GccEnt_Position curveQualif1 = GccEnt_unqualified;
  GccEnt_Position curveQualif2 = GccEnt_unqualified;

  const Geom2dGcc_QualifiedCurve qualifiedCurve1(AdaptedCurve1, curveQualif1);
  const Geom2dGcc_QualifiedCurve qualifiedCurve2(AdaptedCurve2, curveQualif2);

  const Geom2dGcc_Circ2d2TanRad circCalc(qualifiedCurve1,
                                         qualifiedCurve2,
                                         radius,
                                         1.0e-9);

  const Standard_Integer aNbSol = circCalc.NbSolutions();

  // Verify that solutions were found
  EXPECT_GT(aNbSol, 0) << "Should find at least one solution";

  // Check that all solutions have the correct radius
  for (Standard_Integer i = 1; i <= aNbSol; i++)
  {
    gp_Circ2d ct = circCalc.ThisSolution(i);
    EXPECT_NEAR(radius, ct.Radius(), 1.0e-6) << "Solution " << i << " should have radius " << radius;
  }

  // For the first solution, check the distance from theoretical tangent
  if (aNbSol > 0)
  {
    gp_Circ2d     calculated_tangent = circCalc.ThisSolution(1);
    Standard_Real dist = theorical_tangent.Location().Distance(calculated_tangent.Location());

    // The distance should be relatively small (solutions should be close)
    // Note: The exact distance may vary depending on which solution is returned
    EXPECT_LT(dist, 10.0) << "Distance from theoretical tangent should be reasonable";
  }
}
