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

#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <math_NewtonFunctionRoot.hxx>
#include <math_TrigonometricEquationFunction.hxx>

#include <gtest/gtest.h>

TEST(Geom2dAPI_InterCurveCurve_Test, OCC29289_EllipseIntersectionNewtonRoot)
{
  // Create two ellipses
  gp_Elips2d             e1(gp_Ax2d(gp_Pnt2d(0., 0.), gp_Dir2d(gp_Dir2d::D::X)), 2., 1.);
  Handle(Geom2d_Ellipse) Ge1 = new Geom2d_Ellipse(e1);
  gp_Elips2d             e2(gp_Ax2d(gp_Pnt2d(0.5, 0.5), gp_Dir2d(1., 1.)), 2., 1.);
  Handle(Geom2d_Ellipse) Ge2 = new Geom2d_Ellipse(e2);

  // Find intersection points
  Geom2dAPI_InterCurveCurve Intersector;
  Intersector.Init(Ge1, Ge2, 1.e-7);
  EXPECT_GT(Intersector.NbPoints(), 0) << "Error: intersector found no points";

  // Setup trigonometric equation: A*std::cos(x) + B*Sin(x) + C*std::cos(2*x) + D*Sin(2*x) + E
  Standard_Real A, B, C, D, E;
  A = 1.875;
  B = -.75;
  C = -.5;
  D = -.25;
  E = -.25;
  math_TrigonometricEquationFunction MyF(A, B, C, D, E);

  Standard_Real    Tol1  = 1.e-15;
  Standard_Real    Eps   = 1.5e-12;
  Standard_Integer Nit[] = {5, 6, 7, 6};

  // For each intersection point, verify Newton root finding
  Standard_Real    TetaPrev = 0.;
  Standard_Integer i;
  for (i = 1; i <= Intersector.NbPoints(); i++)
  {
    Standard_Real Teta = Intersector.Intersector().Point(i).ParamOnFirst();
    Standard_Real X    = Teta - 0.1 * (Teta - TetaPrev);
    TetaPrev           = Teta;

    math_NewtonFunctionRoot Resol(MyF, X, Tol1, Eps, Nit[i - 1]);
    ASSERT_TRUE(Resol.IsDone()) << "Error: Newton is not done for " << Teta;

    Standard_Real TetaNewton = Resol.Root();
    EXPECT_LE(std::abs(Teta - TetaNewton), 1.e-7) << "Error: Newton root is wrong for " << Teta;
  }
}
