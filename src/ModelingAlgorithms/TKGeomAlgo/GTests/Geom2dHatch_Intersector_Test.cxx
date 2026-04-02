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

#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Intersector.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

// Verify LocalGeometry produces valid outputs on a circle (well-defined tangent).
TEST(Geom2dHatch_Intersector, LocalGeometry_Circle_ValidOutputs)
{
  gp_Ax2d                   anAxis(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(anAxis, 1.0);
  Geom2dAdaptor_Curve       aCurveAdaptor(aCircle);

  Geom2dHatch_Intersector anIntersector(1.0e-7, 1.0e-7);

  gp_Dir2d aTang, aNorm;
  double   aCurv = -1.0;
  anIntersector.LocalGeometry(aCurveAdaptor, 0.0, aTang, aNorm, aCurv);

  // Circle tangent at param=0 is (0,1), normal is (-1,0), curvature is 1.0.
  EXPECT_NEAR(aTang.X(), 0.0, 1.0e-10);
  EXPECT_NEAR(aTang.Y(), 1.0, 1.0e-10);
  EXPECT_NEAR(aCurv, 1.0, 1.0e-10);
}

// Verify LocalGeometry produces valid outputs on a straight line (zero curvature).
TEST(Geom2dHatch_Intersector, LocalGeometry_Line_ZeroCurvature)
{
  gp_Ax2d                 anAxis(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(anAxis);
  Geom2dAdaptor_Curve     aCurveAdaptor(aLine);

  Geom2dHatch_Intersector anIntersector(1.0e-7, 1.0e-7);

  gp_Dir2d aTang, aNorm;
  double   aCurv = -1.0;
  anIntersector.LocalGeometry(aCurveAdaptor, 0.5, aTang, aNorm, aCurv);

  EXPECT_NEAR(aTang.X(), 1.0, 1.0e-10);
  EXPECT_NEAR(aTang.Y(), 0.0, 1.0e-10);
  EXPECT_NEAR(aCurv, 0.0, 1.0e-10);
  // Normal should be perpendicular to tangent.
  EXPECT_NEAR(aNorm.X(), 0.0, 1.0e-10);
  EXPECT_NEAR(std::abs(aNorm.Y()), 1.0, 1.0e-10);
}

// Regression test for bug #24: LocalGeometry with undefined tangent
// must still produce initialized (deterministic) output values.
TEST(Geom2dHatch_Intersector, LocalGeometry_DegenerateCurve_InitializedOutputs)
{
  // Construct a degenerate BSpline where all control points coincide,
  // causing IsTangentDefined() to return false.
  NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
  aPoles(1) = gp_Pnt2d(1.0, 1.0);
  aPoles(2) = gp_Pnt2d(1.0, 1.0);
  aPoles(3) = gp_Pnt2d(1.0, 1.0);
  aPoles(4) = gp_Pnt2d(1.0, 1.0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom2d_BSplineCurve> aDegenerateCurve =
    new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 3);
  Geom2dAdaptor_Curve aCurveAdaptor(aDegenerateCurve);

  Geom2dHatch_Intersector anIntersector(1.0e-7, 1.0e-7);

  gp_Dir2d aTang, aNorm;
  double   aCurv = -999.0;

  // Must not crash and must produce initialized outputs.
  EXPECT_NO_THROW(anIntersector.LocalGeometry(aCurveAdaptor, 0.5, aTang, aNorm, aCurv));

  // Curvature should be initialized to 0 for degenerate case.
  EXPECT_NEAR(aCurv, 0.0, 1.0e-10);
  // Tang and Norm should be valid unit directions (magnitude == 1).
  const double aTangMag = std::sqrt(aTang.X() * aTang.X() + aTang.Y() * aTang.Y());
  EXPECT_NEAR(aTangMag, 1.0, 1.0e-10);
  const double aNormMag = std::sqrt(aNorm.X() * aNorm.X() + aNorm.Y() * aNorm.Y());
  EXPECT_NEAR(aNormMag, 1.0, 1.0e-10);
}
