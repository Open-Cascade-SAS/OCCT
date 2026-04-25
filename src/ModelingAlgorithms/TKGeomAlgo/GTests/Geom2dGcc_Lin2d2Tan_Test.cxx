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
#include <Geom2dGcc_Lin2d2Tan.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Plane.hxx>
#include <GeomAPI.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

// Test OCC813: Geom2dGcc_Lin2d2Tan - tangent line between a 2D ellipse and a point.
// Migrated from QABugs_17.cxx OCC813
TEST(Geom2dGcc_Lin2d2TanTest, OCC813_EllipseAndPoint)
{
  // Construct 3D ellipse and projection plane
  const gp_Ax2 anAx2(gp_Pnt(1262.224429, 425.040878, 363.609716),
                      gp_Dir(0.173648, 0.984808, 0.000000),
                      gp_Dir(-0.932169, 0.164367, -0.322560));

  Handle(Geom_Ellipse) anEll  = new Geom_Ellipse(anAx2, 150, 100);
  Handle(Geom_Plane)   aPlane = new Geom_Plane(gp_Ax3(anAx2));
  const gp_Pln         aPln   = aPlane->Pln();

  // Project ellipse onto the plane and build 2D qualified curve
  Handle(Geom2d_Curve)     aCurve2d = GeomAPI::To2d(anEll, aPln);
  Geom2dAdaptor_Curve      anAdapt(aCurve2d);
  Geom2dGcc_QualifiedCurve aQCurve(anAdapt, GccEnt_outside);

  // Query tangent line from 2D point to the projected ellipse
  const gp_Pnt2d           aPnt2d(200.0, 200.0);
  Geom2dGcc_Lin2d2Tan aLinTan(aQCurve, aPnt2d, 0.1);

  EXPECT_GT(aLinTan.NbSolutions(), 0) << "Expected at least one tangent line solution";
}

// Test OCC814: Geom2dGcc_Lin2d2Tan - tangent line between a 2D circle and a 2D ellipse.
// Migrated from QABugs_17.cxx OCC814
TEST(Geom2dGcc_Lin2d2TanTest, OCC814_CircleAndEllipse)
{
  const gp_Ax2 anAx2(gp_Pnt(1262.224429, 425.040878, 363.609716),
                      gp_Dir(0.173648, 0.984808, 0.000000),
                      gp_Dir(-0.932169, 0.164367, -0.322560));

  Handle(Geom_Circle)  aCir   = new Geom_Circle(gp_Ax2(gp_Pnt(823.687192, 502.366825, 478.960440),
                                                        gp_Dir(0.173648, 0.984808, 0.000000),
                                                        gp_Dir(-0.932169, 0.164367, -0.322560)),
                                                50);
  Handle(Geom_Ellipse) anEll  = new Geom_Ellipse(anAx2, 150, 100);
  Handle(Geom_Plane)   aPlane = new Geom_Plane(gp_Ax3(anAx2));
  const gp_Pln         aPln   = aPlane->Pln();

  // Project both curves onto the plane and build qualified curves
  Handle(Geom2d_Curve)     aCurve2d     = GeomAPI::To2d(anEll, aPln);
  Handle(Geom2d_Curve)     aFromCurve2d = GeomAPI::To2d(aCir, aPln);
  Geom2dAdaptor_Curve      anAdaptEll(aCurve2d);
  Geom2dAdaptor_Curve      anAdaptCir(aFromCurve2d);
  Geom2dGcc_QualifiedCurve aQEll(anAdaptEll, GccEnt_outside);
  Geom2dGcc_QualifiedCurve aQCir(anAdaptCir, GccEnt_outside);

  Geom2dGcc_Lin2d2Tan aLinTan(aQEll, aQCir, 0.1);

  EXPECT_GT(aLinTan.NbSolutions(), 0) << "Expected at least one tangent line solution";
}
