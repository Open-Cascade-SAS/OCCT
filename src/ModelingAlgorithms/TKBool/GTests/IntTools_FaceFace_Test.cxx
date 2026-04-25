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

#include <BRep_Builder.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <IntTools_Curve.hxx>
#include <IntTools_FaceFace.hxx>
#include <IntTools_PntOn2Faces.hxx>
#include <NCollection_Sequence.hxx>
#include <Precision.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

// Test OCC24005: IntTools_FaceFace should complete quickly and correctly when
// intersecting a slightly off-angle plane with a cylinder. Previously took 7+ seconds.
TEST(IntTools_FaceFaceTest, OCC24005_PlaneCylinderIntersection)
{
  // Hardcoded geometry from the original regression test
  Handle(Geom_Plane) aPlane = new Geom_Plane(
    gp_Ax3(gp_Pnt(-72.948737453424499, 754.30437716359393, 259.52151854671678),
           gp_Dir(6.2471473085930200e-007, -0.99999999999980493, 0.00000000000000000),
           gp_Dir(0.99999999999980493, 6.2471473085930200e-007, 0.00000000000000000)));

  Handle(Geom_CylindricalSurface) aCylinder = new Geom_CylindricalSurface(
    gp_Ax3(gp_Pnt(-6.4812490053250649, 753.39408794522092, 279.16400974257465),
           gp_Dir(1.0000000000000000, 0.0, 0.00000000000000000),
           gp_Dir(0.0, 1.0000000000000000, 0.00000000000000000)),
    19.712534607908712);

  BRep_Builder aBuilder;
  TopoDS_Face  aFace1, aFace2;
  aBuilder.MakeFace(aFace1, aPlane, Precision::Confusion());
  aBuilder.MakeFace(aFace2, aCylinder, Precision::Confusion());

  IntTools_FaceFace anInters;
  anInters.SetParameters(false, true, true, Precision::Confusion());
  anInters.Perform(aFace1, aFace2);

  ASSERT_TRUE(anInters.IsDone()) << "IntTools_FaceFace::Perform did not complete";

  // The original test verified that intersection completes without hanging.
  // Check that we get at least one intersection curve.
  const NCollection_Sequence<IntTools_Curve>& aCurves = anInters.Lines();
  EXPECT_GE(aCurves.Length() + anInters.Points().Length(), 1)
    << "Expected at least one intersection result (curve or point)";
}
