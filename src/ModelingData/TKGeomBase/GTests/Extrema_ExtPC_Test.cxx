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

#include <Extrema_ExtPC.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_Circle.hxx>
#include <ElSLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax2.hxx>
#include <gp_Cylinder.hxx>

#include <gtest/gtest.h>

// Test for bug OCC24945
// Extrema_ExtPElC::Perform does not consider angular tolerance when calculates angle between two
// vectors
TEST(Extrema_ExtPC_Test, Bug24945_CylinderParameterNormalization)
{
  // Point to project
  gp_Pnt aP3D(-1725.97, 843.257, -4.22741e-013);

  // Circle for projection
  gp_Ax2              aAxis(gp_Pnt(0, 843.257, 0), gp_Dir(gp::DY()).Reversed(), gp::DX());
  Handle(Geom_Circle) aCircle = new Geom_Circle(aAxis, 1725.9708621929999);
  GeomAdaptor_Curve   aC3D(aCircle);

  // Project point onto circle
  Extrema_ExtPC aExtPC(aP3D, aC3D);
  ASSERT_TRUE(aExtPC.IsDone());
  ASSERT_GT(aExtPC.NbExt(), 0);

  gp_Pnt aProj = aExtPC.Point(1).Value();

  // Check projected point coordinates
  EXPECT_NEAR(aProj.X(), -1725.97, 1.0e-2);
  EXPECT_NEAR(aProj.Y(), 843.257, 1.0e-2);
  EXPECT_NEAR(aProj.Z(), 0.0, 1.0e-10);

  // Compute parameters on cylinder
  gp_Ax2      aCylAxis(gp_Pnt(0, 2103.87, 0), gp::DY().Reversed(), gp::DX().Reversed());
  gp_Cylinder aCylinder(aCylAxis, 1890.);

  Standard_Real aU = 0., aV = 0.;
  ElSLib::Parameters(aCylinder, aProj, aU, aV);

  // Check parameters
  // U should be normalized to [0, 2*PI) - the test previously expected 6.2832 (2*PI)
  // but the improved normalization now consistently returns 0.0 for the seam
  EXPECT_NEAR(aU, 0.0, 1.0e-4);
  EXPECT_NEAR(aV, 1260.613, 1.0e-2);
}
