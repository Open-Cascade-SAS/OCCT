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

#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dGcc_Circ2d3Tan.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Circle.hxx>
#include <GeomAPI.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pln.hxx>

#include <gtest/gtest.h>

TEST(Geom2dGcc_Circ2d3Tan_Test, OCC353_TangentCircleDoesNotHang)
{
  // Bug OCC353: Attached DRAW command hangs in Geom2dGcc_Circ2d3Tan
  // This test verifies that computing a circle tangent to two circles and
  // passing through a point doesn't hang

  // Create two 3D circles
  gp_Ax2              anAx21(gp_Pnt(100, 0, 0), gp_Dir(gp::DZ()));
  Handle(Geom_Circle) aCirc1 = new Geom_Circle(anAx21, 25);

  gp_Ax2              anAx22(gp_Pnt(-100, 0, 0), gp_Dir(gp::DZ()));
  Handle(Geom_Circle) aCirc2 = new Geom_Circle(anAx22, 25);

  // Reference plane for 2D projection
  gp_Pln aRefPln(gp_Pnt(0, 0, 0), gp_Dir(gp::DZ()));

  // Convert 3D circles to 2D
  Handle(Geom2d_Curve) aCirc2d1 = GeomAPI::To2d(aCirc1, aRefPln);
  Handle(Geom2d_Curve) aCirc2d2 = GeomAPI::To2d(aCirc2, aRefPln);

  // Create adaptors
  Geom2dAdaptor_Curve anAdap1(aCirc2d1);
  Geom2dAdaptor_Curve anAdap2(aCirc2d2);

  // Create qualified curves (enclosing)
  Geom2dGcc_QualifiedCurve aQCur1(anAdap1, GccEnt_enclosing);
  Geom2dGcc_QualifiedCurve aQCur2(anAdap2, GccEnt_enclosing);

  // Create a 2D point
  gp_Pnt2d                      aPt2d(0, 175);
  Handle(Geom2d_CartesianPoint) aPt = new Geom2d_CartesianPoint(aPt2d);

  // Compute tangent circle - this should not hang
  Geom2dGcc_Circ2d3Tan aSol(aQCur1, aQCur2, aPt, 0.001, 0.0, 0.0);

  // Verify that the computation completed successfully
  EXPECT_TRUE(aSol.IsDone()) << "Geom2dGcc_Circ2d3Tan computation should complete";

  // Verify that we have solutions
  if (aSol.IsDone())
  {
    Standard_Integer aNbSol = aSol.NbSolutions();
    EXPECT_GT(aNbSol, 0) << "Should find at least one solution";

    // Verify that we can access the solutions
    for (Standard_Integer i = 1; i <= aNbSol; i++)
    {
      EXPECT_NO_THROW(aSol.ThisSolution(i)) << "Should be able to access solution " << i;
    }
  }
}
