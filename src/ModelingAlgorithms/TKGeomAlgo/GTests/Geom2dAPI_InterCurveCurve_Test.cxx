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
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>

#include <gtest/gtest.h>

TEST(Geom2dAPI_InterCurveCurve_Test, OCC29289_EllipseIntersectionNewtonRoot)
{
  // Create two ellipses
  gp_Elips2d                  e1(gp_Ax2d(gp_Pnt2d(0., 0.), gp_Dir2d(gp_Dir2d::D::X)), 2., 1.);
  occ::handle<Geom2d_Ellipse> Ge1 = new Geom2d_Ellipse(e1);
  gp_Elips2d                  e2(gp_Ax2d(gp_Pnt2d(0.5, 0.5), gp_Dir2d(1., 1.)), 2., 1.);
  occ::handle<Geom2d_Ellipse> Ge2 = new Geom2d_Ellipse(e2);

  // Find intersection points
  Geom2dAPI_InterCurveCurve Intersector;
  Intersector.Init(Ge1, Ge2, 1.e-7);
  EXPECT_GT(Intersector.NbPoints(), 0) << "Error: intersector found no points";

  // Setup trigonometric equation: A*std::cos(x) + B*Sin(x) + C*std::cos(2*x) + D*Sin(2*x) + E
  double A, B, C, D, E;
  A = 1.875;
  B = -.75;
  C = -.5;
  D = -.25;
  E = -.25;
  math_TrigonometricEquationFunction MyF(A, B, C, D, E);

  double Tol1  = 1.e-15;
  double Eps   = 1.5e-12;
  int    Nit[] = {5, 6, 7, 6};

  // For each intersection point, verify Newton root finding
  double TetaPrev = 0.;
  int    i;
  for (i = 1; i <= Intersector.NbPoints(); i++)
  {
    double Teta = Intersector.Intersector().Point(i).ParamOnFirst();
    double X    = Teta - 0.1 * (Teta - TetaPrev);
    TetaPrev    = Teta;

    math_NewtonFunctionRoot Resol(MyF, X, Tol1, Eps, Nit[i - 1]);
    ASSERT_TRUE(Resol.IsDone()) << "Error: Newton is not done for " << Teta;

    double TetaNewton = Resol.Root();
    EXPECT_LE(std::abs(Teta - TetaNewton), 1.e-7) << "Error: Newton root is wrong for " << Teta;
  }
}

TEST(Geom2dAPI_InterCurveCurve_Test, PointRejectsZeroIndex)
{
  gp_Elips2d                  anEllipse1(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 2.0, 1.0);
  occ::handle<Geom2d_Ellipse> aCurve1 = new Geom2d_Ellipse(anEllipse1);
  gp_Elips2d                  anEllipse2(gp_Ax2d(gp_Pnt2d(0.5, 0.5), gp_Dir2d(1.0, 1.0)), 2.0, 1.0);
  occ::handle<Geom2d_Ellipse> aCurve2 = new Geom2d_Ellipse(anEllipse2);

  Geom2dAPI_InterCurveCurve anIntersector(aCurve1, aCurve2, 1.0e-7);
  ASSERT_GT(anIntersector.NbPoints(), 0);

#ifndef No_Exception
  EXPECT_THROW((void)anIntersector.Point(0), Standard_OutOfRange);
#endif
}

// Regression test for bug #11: Init with null handle must raise Standard_NullObject.
TEST(Geom2dAPI_InterCurveCurve_Test, Init_NullHandle_RaisesException)
{
#ifndef No_Exception
  occ::handle<Geom2d_Curve> aNullCurve;
  gp_Elips2d                anEllipse(gp_Ax2d(gp_Pnt2d(0., 0.), gp_Dir2d(1., 0.)), 2., 1.);
  occ::handle<Geom2d_Curve> aValidCurve = new Geom2d_Ellipse(anEllipse);

  // Both null.
  EXPECT_THROW(Geom2dAPI_InterCurveCurve(aNullCurve, aNullCurve, 1.0e-7), Standard_NullObject);

  // First null.
  EXPECT_THROW(Geom2dAPI_InterCurveCurve(aNullCurve, aValidCurve, 1.0e-7), Standard_NullObject);

  // Second null.
  EXPECT_THROW(Geom2dAPI_InterCurveCurve(aValidCurve, aNullCurve, 1.0e-7), Standard_NullObject);

  // Single-curve Init with null.
  Geom2dAPI_InterCurveCurve anInter;
  EXPECT_THROW(anInter.Init(aNullCurve, 1.0e-7), Standard_NullObject);
#endif
}
